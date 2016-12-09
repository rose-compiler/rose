int f(int code, ...) {
  __builtin_va_list args;
  __builtin_stdarg_start (args, code);
}

int g(char * code, ...) {
  __builtin_va_list args;
  __builtin_stdarg_start (args, code);
}
