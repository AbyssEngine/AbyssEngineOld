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
#include <libavutil/opt.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>

// compatibility with newer API
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc avcodec_alloc_frame
#define av_frame_free avcodec_free_frame
#endif

AVFormatContext *av_format_context = NULL;
AVIOContext *avio_context = NULL;
SwrContext *resample_contex = NULL;
void *video_buffer_data = NULL;
int video_buffer_data_size;
int video_buffer_position;
unsigned char *av_buffer = NULL; // may need free
int video_stream_idx;
int audio_stream_idx;
SDL_Texture *video_texture = NULL;
AVFrame *av_frame = NULL;
struct SwsContext *sws_ctx = NULL;
AVCodecContext *video_codec_context = NULL;
AVCodecContext *audio_codec_context = NULL;
Uint8 *yPlane, *uPlane, *vPlane;
size_t yPlaneSz, uvPlaneSz;
int uvPitch;
bool mouse_was_unpressed;
bool frames_ready;
SDL_Rect target_rect;
uint64_t micros_per_frame;
uint64_t video_timestamp;

#define DECODE_BUFFER_SIZE (1024 * 32)

int modevideo_stream_read(void *opaque, uint8_t *buf, int buf_size) {
    int actual_len = buf_size;
    const int remaining = video_buffer_data_size - video_buffer_position;
    if (actual_len > remaining) {
        actual_len = remaining;
    }
    memmove(buf, &((char *)video_buffer_data)[video_buffer_position], actual_len);
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
    for (uint32_t i = 0; i < av_format_context->nb_streams; i++) {
        if (av_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = (int)i;
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

    audio_stream_idx = -1;
    for (uint32_t i = 0; i < av_format_context->nb_streams; i++) {
        if (av_format_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_idx = (int)i;
            break;
        }
    }

    // const AVRational frame_rate = av_format_context->streams[video_stream_idx]->r_frame_rate;
    // float fps = (float)frame_rate.num / (float)frame_rate.den;
    micros_per_frame = (uint64_t)(1000000 / ((float)av_format_context->streams[video_stream_idx]->r_frame_rate.num /
                                             (float)av_format_context->streams[video_stream_idx]->r_frame_rate.den));

    const AVCodecParameters *video_codec_par = av_format_context->streams[video_stream_idx]->codecpar;
    AVCodec *video_decoder = avcodec_find_decoder(video_codec_par->codec_id);
    if (video_decoder == NULL) {
        engine_trigger_crash(src, "Missing video codec; cannot play video file.");
        return;
    }

    video_codec_context = avcodec_alloc_context3(video_decoder);
    if ((av_err = avcodec_parameters_to_context(video_codec_context, video_codec_par) < 0)) {
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

    if (audio_stream_idx >= 0) {
        const AVCodecParameters *audio_codec_par = av_format_context->streams[audio_stream_idx]->codecpar;
        AVCodec *audio_decoder = avcodec_find_decoder(audio_codec_par->codec_id);
        if (audio_decoder == NULL) {
            engine_trigger_crash(src, "Missing audio codec; cannot play video file.");
            return;
        }

        audio_codec_context = avcodec_alloc_context3(audio_decoder);
        if ((av_err = avcodec_parameters_to_context(audio_codec_context, audio_codec_par) < 0)) {
            char *err = calloc(1, 4096);
            av_make_error_string(err, 4096, av_err);
            engine_trigger_crash(src, err);
            free(err);
            return;
        }

        if ((av_err = avcodec_open2(audio_codec_context, audio_decoder, NULL)) < 0) {
            char *err = calloc(1, 4096);
            av_make_error_string(err, 4096, av_err);
            engine_trigger_crash(src, err);
            free(err);
            return;
        }

        resample_contex = swr_alloc();
        av_opt_set_channel_layout(resample_contex, "in_channel_layout", audio_codec_context->channel_layout, 0);
        av_opt_set_channel_layout(resample_contex, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
        av_opt_set_int(resample_contex, "in_sample_rate", audio_codec_context->sample_rate, 0);
        av_opt_set_int(resample_contex, "out_sample_rate", 44100, 0);
        av_opt_set_sample_fmt(resample_contex, "in_sample_fmt", audio_codec_context->sample_fmt, 0);
        av_opt_set_sample_fmt(resample_contex, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);

        if ((av_err = swr_init(resample_contex)) < 0) {
            char *err = calloc(1, 4096);
            av_make_error_string(err, 4096, av_err);
            engine_trigger_crash(src, err);
            free(err);
            return;
        }
    }

    video_texture = SDL_CreateTexture(engine_get_renderer(src), SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, video_codec_context->width,
                                      video_codec_context->height);

    target_rect.x = 0;
    target_rect.w = GAME_WIDTH;
    const float r = (float)video_codec_context->height / (float)video_codec_context->width;
    target_rect.h = (int)((float)GAME_WIDTH * r);
    target_rect.y = (GAME_HEIGHT / 2) - (target_rect.h / 2);

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
    video_timestamp = av_gettime();
    engine_reset_audio_buffer(src);
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
    
    av_free(avio_context->buffer);
    avio_context_free(&avio_context);
    free(video_buffer_data);
    avcodec_free_context(&video_codec_context);
    avcodec_free_context(&audio_codec_context);
    SDL_DestroyTexture(video_texture);
    sws_freeContext(sws_ctx);
    swr_free(&resample_contex);
    av_frame_free(&av_frame);
    free(yPlane);
    free(uPlane);
    free(vPlane);
    avformat_free_context(av_format_context);

    engine_end_video(engine_get_global_instance());
}

bool engine_process_frame(engine *src) {
    if (av_format_context == NULL)
        return false;
    
    AVPacket packet;
    if (av_read_frame(av_format_context, &packet) < 0) {
        av_packet_unref(&packet);
        modevideo_cleanup();
        return true;
    }

    if (packet.stream_index == video_stream_idx) {
        int av_err;
        if ((av_err = avcodec_send_packet(video_codec_context, &packet)) < 0) {
            char *err = calloc(1, 4096);
            av_make_error_string(err, 4096, av_err);
            engine_trigger_crash(src, err);
            free(err);
            av_packet_unref(&packet);
            return true;
        }

        if ((av_err = avcodec_receive_frame(video_codec_context, av_frame)) < 0) {
            char *err = calloc(1, 4096);
            av_make_error_string(err, 4096, av_err);
            engine_trigger_crash(src, err);
            free(err);
            av_packet_unref(&packet);
            return true;
        }

        uint8_t *data[AV_NUM_DATA_POINTERS];
        data[0] = yPlane;
        data[1] = uPlane;
        data[2] = vPlane;

        int line_size[AV_NUM_DATA_POINTERS];
        line_size[0] = video_codec_context->width;
        line_size[1] = uvPitch;
        line_size[2] = uvPitch;

        // Convert the image into YUV format that SDL uses
        sws_scale(sws_ctx, (const unsigned char *const *)av_frame->data, av_frame->linesize, 0, video_codec_context->height, data, line_size);

        if (SDL_UpdateYUVTexture(video_texture, NULL, yPlane, video_codec_context->width, uPlane, uvPitch, vPlane, uvPitch) < 0) {
            log_fatal("Error updating YUV Texture: %s", SDL_GetError());
            engine_trigger_crash(src, "Failed to update YUV texture.");
            av_packet_unref(&packet);
            return true;
        }

        frames_ready = true;
        av_packet_unref(&packet);
        return true;

    } else if (packet.stream_index == audio_stream_idx) {
        int av_err;
        if ((av_err = avcodec_send_packet(audio_codec_context, &packet)) < 0) {
            char *err = calloc(1, 4096);
            av_make_error_string(err, 4096, av_err);
            engine_trigger_crash(src, err);
            free(err);
            av_packet_unref(&packet);
            return true;
        }

        while (true) {
            if ((av_err = avcodec_receive_frame(audio_codec_context, av_frame)) < 0) {
                if (av_err == AVERROR(EAGAIN) || av_err == AVERROR_EOF) {
                    break;
                }

                char *err = calloc(1, 4096);
                av_make_error_string(err, 4096, av_err);
                engine_trigger_crash(src, err);
                free(err);
                av_packet_unref(&packet);
                return true;
            }

            const int out_size = av_samples_get_buffer_size(NULL, audio_codec_context->channels, av_frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
            uint8_t *out_buff = malloc(out_size);
            uint8_t *out_buff_array[1];
            out_buff_array[0] = out_buff;
            swr_convert(resample_contex, out_buff_array, av_frame->nb_samples, &av_frame->data[0], av_frame->nb_samples);
            engine_write_audio_buffer(src, out_buff, out_size);
            free(out_buff);
        }

        av_packet_unref(&packet);
        return false;
    }

    return false;
}

void engine_update_video(engine *src, uint32_t tick_diff) {
    if (av_format_context == NULL)
        return;

    const enum e_mouse_button button_state = engine_get_mouse_button_state(src);
    if (!mouse_was_unpressed) {
        if (button_state == 0)
            mouse_was_unpressed = true;
    } else {
        if (button_state != 0) {
            modevideo_cleanup();
            return;
        }
    }

    while(true) {
        const uint64_t diff = av_gettime() - video_timestamp;
        if (diff < micros_per_frame)
            break;

        video_timestamp += micros_per_frame;
        while (!engine_process_frame(src)) {
        }
    }


}

void modevideo_set_callbacks(engine *src) { engine_set_callbacks(src, engine_render_video, engine_update_video); }
