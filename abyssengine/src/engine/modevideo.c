/**
 * Copyright (C) 2021 Tim Sarbin
 * This file is part of AbyssEngine <https://github.com/AbyssEngine>.
 *
 * AbyssEngine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AbyssEngine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with AbyssEngine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "modevideo.h"
#include "libabyss/utils.h"
#include <SDL2/SDL.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

AVFormatContext *av_format_context = NULL;
AVIOContext *avio_context;
void *video_buffer_data;
int video_buffer_data_size;
int video_buffer_position;
unsigned char *av_buffer;
int video_stream_idx;
SDL_Texture *video_texture;
AVFrame *av_frame;
struct SwsContext *sws_ctx;
AVCodecContext *video_codec_context;
Uint8 *yPlane, *uPlane, *vPlane;
size_t yPlaneSz, uvPlaneSz;
int uvPitch;
bool mouse_was_unpressed;
bool frames_ready;
SDL_Rect target_rect;

#define DECODE_BUFFER_SIZE (1024 * 32)

int modevideo_stream_read(void *opaque, uint8_t *buf, int buf_size) {
    int actual_len = buf_size;
    int remaining = video_buffer_data_size - video_buffer_position;
    if (actual_len > remaining) {
        actual_len = remaining;
    }
    memmove(buf, &video_buffer_data[video_buffer_position], actual_len);
    video_buffer_position += actual_len;
    return actual_len;
}

int64_t modevideo_stream_seek(void *opaque, int64_t offset, int whence) {
    int64_t to;
    switch (whence) {
    case SEEK_SET:
        to = offset;
        break;
    case SEEK_CUR:
        to = video_buffer_position + offset;
        break;
    case SEEK_END:
        to = video_buffer_data_size + offset;
        break;
    case AVSEEK_SIZE:
        return video_buffer_data_size;
    default:
        return -1;
    }

    if (to < 0)
        return -1;

    if (to >= video_buffer_data_size)
        return -1;

    video_buffer_position = (int)to;

    return 0;
}

void modevideo_load_file(engine *src, const char *file_path) {
    VERIFY_ENGINE_THREAD

    mouse_was_unpressed = false;
    frames_ready = false;

    char *new_path = calloc(1, 4096);
    strcat(new_path, file_path);
    char *actual_path = util_fix_mpq_path(new_path);
    video_buffer_data = loader_load(engine_get_loader(src), actual_path, &video_buffer_data_size);
    free(new_path);

    if (video_buffer_data == NULL) {
        char *msg = calloc(1, 4096);
        sprintf(msg, "Could not locate video file:\n%s", actual_path);
        engine_trigger_crash(src, msg);
        free(msg);
    }

    video_buffer_position = 0;
    av_buffer = av_malloc(DECODE_BUFFER_SIZE);
    avio_context = avio_alloc_context(av_buffer, DECODE_BUFFER_SIZE, 0, video_buffer_data, modevideo_stream_read, 0, modevideo_stream_seek);

    av_format_context = avformat_alloc_context();

    av_format_context->pb = avio_context;
    av_format_context->flags |= AVFMT_FLAG_CUSTOM_IO;
    int av_err;

    if ((av_err = avformat_open_input(&av_format_context, "", NULL, NULL)) < 0) {
        char *err = calloc(1, 4096);
        av_make_error_string(err, 4096, av_err);
        engine_trigger_crash(src, err);
        free(err);
        return;
    }

    if ((av_err = avformat_find_stream_info(av_format_context, NULL)) < 0) {
        char *err = calloc(1, 4096);
        av_make_error_string(err, 4096, av_err);
        engine_trigger_crash(src, err);
        free(err);
        return;
    }

    video_stream_idx = -1;
    for (int i = 0; i < av_format_context->nb_streams; i++) {
        if (av_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    if ((av_err = video_stream_idx) == -1) {
        char *err = calloc(1, 4096);
        av_make_error_string(err, 4096, av_err);
        engine_trigger_crash(src, err);
        free(err);
        return;
    }

    AVCodecParameters *codec_par = av_format_context->streams[video_stream_idx]->codecpar;
    AVCodec *video_decoder = avcodec_find_decoder(codec_par->codec_id);
    if (video_decoder == NULL) {
        engine_trigger_crash(src, "Missing video codec; cannot play video file.");
        return;
    }

    video_codec_context = avcodec_alloc_context3(video_decoder);
    if ((av_err = avcodec_parameters_to_context(video_codec_context, codec_par) < 0)) {
        char *err = calloc(1, 4096);
        av_make_error_string(err, 4096, av_err);
        engine_trigger_crash(src, err);
        free(err);
        return;
    }

    if ((av_err = avcodec_open2(video_codec_context, video_decoder, NULL)) < 0) {
        char *err = calloc(1, 4096);
        av_make_error_string(err, 4096, av_err);
        engine_trigger_crash(src, err);
        free(err);
        return;
    }

    video_texture = SDL_CreateTexture(engine_get_renderer(src), SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, video_codec_context->width,
                                      video_codec_context->height);

    target_rect.x = 0;
    target_rect.w = GAME_WIDTH;
    target_rect.h = video_codec_context->height;
    target_rect.y = (GAME_HEIGHT / 2) - (video_codec_context->height / 2);

    SDL_SetTextureBlendMode(video_texture, SDL_BLENDMODE_NONE);

    if (video_texture == NULL) {
        engine_trigger_crash(src, "Could not create video rendering texture.");
        return;
    }

    sws_ctx = sws_getContext(video_codec_context->width, video_codec_context->height, video_codec_context->pix_fmt, video_codec_context->width,
                             video_codec_context->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);

    yPlaneSz = video_codec_context->width * video_codec_context->height;
    uvPlaneSz = video_codec_context->width * video_codec_context->height / 4;
    yPlane = (Uint8 *)calloc(1, yPlaneSz);
    uPlane = (Uint8 *)calloc(1, uvPlaneSz);
    vPlane = (Uint8 *)calloc(1, uvPlaneSz);
    uvPitch = video_codec_context->width / 2;

    av_frame = av_frame_alloc();
}

void engine_render_video(engine *src) {
    SDL_Renderer *renderer = engine_get_renderer(src);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    if (frames_ready)
        SDL_RenderCopy(renderer, video_texture, NULL, &target_rect);
}

void modevideo_cleanup() {
    VERIFY_ENGINE_THREAD

    avformat_free_context(av_format_context);
    av_format_context = NULL;
    avio_context_free(&avio_context);
    free(video_buffer_data);
    avcodec_free_context(&video_codec_context);
    SDL_DestroyTexture(video_texture);
    sws_freeContext(sws_ctx);
    av_frame_unref(av_frame);
    free(yPlane);
    free(uPlane);
    free(vPlane);

    engine_end_video(engine_get_global_instance());
}

void engine_update_video(engine *src, uint32_t tick_diff) {
    if (av_format_context == NULL)
        return;

    enum e_mouse_button button_state = engine_get_mouse_button_state(src);
    if (!mouse_was_unpressed) {
        if (button_state == 0)
            mouse_was_unpressed = true;
    } else {
        if (button_state != 0) {
            modevideo_cleanup();
            return;
        }
    }
    AVPacket packet;
    if (av_read_frame(av_format_context, &packet) < 0) {
        // TODO: Free all the things
        av_packet_unref(&packet);
        modevideo_cleanup();
        return;
    }

    if (packet.stream_index == video_stream_idx) {
        int av_err;
        if ((av_err = avcodec_send_packet(video_codec_context, &packet)) < 0) {
            char *err = calloc(1, 4096);
            av_make_error_string(err, 4096, av_err);
            engine_trigger_crash(src, err);
            free(err);
            return;
        }
        if ((av_err = avcodec_receive_frame(video_codec_context, av_frame)) < 0) {
            char *err = calloc(1, 4096);
            av_make_error_string(err, 4096, av_err);
            engine_trigger_crash(src, err);
            free(err);
            return;
        }
        // avcodec_decode_video2(video_codec_context, av_frame, &frame_finished, &packet);

        AVPicture pict;
        pict.data[0] = yPlane;
        pict.data[1] = uPlane;
        pict.data[2] = vPlane;
        pict.linesize[0] = video_codec_context->width;
        pict.linesize[1] = uvPitch;
        pict.linesize[2] = uvPitch;

        // Convert the image into YUV format that SDL uses
        sws_scale(sws_ctx, (uint8_t const *const *)av_frame->data, av_frame->linesize, 0, video_codec_context->height, pict.data, pict.linesize);

        if (SDL_UpdateYUVTexture(video_texture, NULL, yPlane, video_codec_context->width, uPlane, uvPitch, vPlane, uvPitch) < 0) {
            log_fatal("Error updating YUV Texture: %s", SDL_GetError());
            engine_trigger_crash(src, "Failed to update YUV texture.");
            return;
        }

        frames_ready = true;
    }

    av_packet_unref(&packet);
}

void modevideo_set_callbacks(engine *src) { engine_set_callbacks(src, engine_render_video, engine_update_video); }
