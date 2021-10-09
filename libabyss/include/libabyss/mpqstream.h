#ifndef LIBABYSS_MPQ_STREAM
#define LIBABYSS_MPQ_STREAM

typedef struct mpq_stream mpq_stream;

mpq_stream *mpq_stream_new(mpq *mpq, mpq_block *block, const char *filename);
void mpq_stream_destroy(mpq_stream *source);
uint32_t mpq_stream_read(mpq_stream *source, void* buffer, uint32_t offset, uint32_t size);
#endif // LIBABYSS_MPQ_STREAM
