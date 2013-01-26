#include "logging.h"

#ifdef __QNX__
slog2_buffer_t              buffer_handle;

void logging_init()
{
    slog2_buffer_set_config_t buffer_config;

    buffer_config.buffer_set_name = "qterm";
    buffer_config.num_buffers = 1;
    buffer_config.verbosity_level = SLOG2_INFO;
    buffer_config.buffer_config[0].buffer_name = "qterm";
    buffer_config.buffer_config[0].num_pages = 4;

    // Register the Buffer Set
    slog2_register( &buffer_config, &buffer_handle, 0 );
}

void slog(const char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    slog2f(buffer_handle, 0, SLOG2_INFO, fmt,args);
    va_end(args);
}
#else
void slog(const char *fmt, ...)
{
    va_list args;
    va_start(args,fmt);
    printf(buffer_handle, 0, SLOG2_INFO, fmt,args);
    va_end(args);
}
#endif
