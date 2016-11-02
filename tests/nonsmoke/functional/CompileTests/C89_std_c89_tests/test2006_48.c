#include <stdarg.h>
#include <stdio.h>

void printMessage(FILE *f, const char *format, ...) {
   va_list ap;
   va_start(ap, format);
   vfprintf(f, format, ap);
   va_end(ap);
}
