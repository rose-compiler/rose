#ifndef _FAKE_ERR_H
#define _FAKE_ERR_H

static inline
       void err(int eval, const char *fmt, ...) {}

static inline
       void errx(int eval, const char *fmt, ...) {}

static inline
       void warn(const char *fmt, ...) {}

static inline
       void warnx(const char *fmt, ...) {}
/*
       #include <stdarg.h>

       void verr(int eval, const char *fmt, va_list args);

       void verrx(int eval, const char *fmt, va_list args);

       void vwarn(const char *fmt, va_list args);

       void vwarnx(const char *fmt, va_list args);
*/

#endif /* _FAKE_ERR_H */
