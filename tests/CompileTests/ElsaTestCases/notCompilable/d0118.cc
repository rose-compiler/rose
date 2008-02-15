// __builtin_stdarg_start should take a void const \* pointer as its second argument

typedef __builtin_va_list __gnuc_va_list;

typedef __gnuc_va_list va_list;

typedef struct _jmethodID *jmethodID;

void NewObject(jmethodID methodID, ...)
{
  va_list args;
  __builtin_stdarg_start ((args), methodID);
}
