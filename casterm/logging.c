#include "logging.h"

slog2_buffer_t              buffer_handle;

void logging_init()
{
    slog2_buffer_set_config_t buffer_config;

    buffer_config.buffer_set_name = "casterm";
    buffer_config.num_buffers = 1;
    buffer_config.verbosity_level = SLOG2_INFO;
    buffer_config.buffer_config[0].buffer_name = "casterm";
    buffer_config.buffer_config[0].num_pages = 4;

    // Register the Buffer Set
    slog2_register( &buffer_config, &buffer_handle, 0 );
}
