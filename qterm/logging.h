#ifndef __LOGGING_H__
#define __LOGGING_H__

#ifdef __QNX__
#include <sys/slog2.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __QNX__
void logging_init();
#endif

void slog(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __LOGGING_H__ */
