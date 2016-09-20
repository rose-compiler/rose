/* Test substitution of varargs with something that would preserve them in the translation through ROSE

   This test code is similar to test2006_91.C but this is tested using --rose:C_only (C instead of C+++).

*/

// #include "test2006_91.C"
// code copied from test2006_91.C (can't use include since the generated code would just have the include directive)
#include <stdarg.h>
#include <stdio.h>

void foo_int(int i, ...) {
   int arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, int);
   va_end(ap);  // use of va_end does not appear to be critical
   printf("given arguments: %d %d\n", i, arg);
}

void foo_char(char *i, ...)
   {
     int arg;
     va_list ap;
     va_start(ap,i);
     arg = va_arg(ap, int);
     va_end(ap);
     printf("given arguments: %s %d\n", i, arg);
   }

void foo_const_char(const char *i, ...)
   {
     int arg;
     va_list ap;
     va_start(ap,i);
     arg = va_arg(ap, int);
     va_end(ap);
     printf("given arguments: %s %d\n", i, arg);
   }

void foo_long(long i, ...) {
   long arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, long);
   va_end(ap);
   printf("given arguments: %ld %d\n", i, arg);
}

void foo_double(double i, ...) {
   double arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, double);
   va_end(ap);
   printf("given arguments: %f %d\n", i, arg);
}

void foo_void_ptr(void* i, ...) {
   void* arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, void*);
   va_end(ap);
   printf("given arguments: %p %d\n", i, arg);
}

void foo_float(float i, ...) {
   float arg;
   va_list ap;
   va_start(ap, i);
// DQ (7/21/2006): This has to be a double instead of a float
   arg = va_arg(ap, double);
   va_end(ap);
   printf("given arguments: %f %d\n", i, arg);
}

int main(int argc, char **argv) {
   foo_int(5, 7);
   foo_long(5,7);
   foo_double(5.0,7.0);
   foo_char("test",7);
   foo_const_char("const test",7);
   foo_void_ptr(&argc,7);
   foo_float(5.0,7.0);
   return 0;
}

#if 0
void foo_char(char *fmt, ...)
   {
     int i;
     va_list args;
     va_start(args,fmt);
     i = va_arg(args,int);
     va_end(args);
   }

void foo_int(int i, ...) {
   int arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, int);
   printf("given arguments: %d %d\n", i, arg);
}

void foo_long(long i, ...) {
   long arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, long);
   printf("given arguments: %ld %d\n", i, arg);
}

void foo_float(float i, ...) {
   float arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, float);
   printf("given arguments: %f %d\n", i, arg);
}

void foo_double(double i, ...) {
   double arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, double);
   printf("given arguments: %f %d\n", i, arg);
}

void foo_void(void* i, ...) {
   void* arg;
   va_list ap;
   va_start(ap, i);
   arg = va_arg(ap, void*);
   printf("given arguments: %p %d\n", i, arg);
}
#endif


