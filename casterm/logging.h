#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <sys/slog2.h>

#ifdef __cplusplus
extern "C" {
#endif

extern slog2_buffer_t buffer_handle;

void logging_init();

#ifdef __cplusplus
}
#endif

#endif /* __LOGGING_H__ */
