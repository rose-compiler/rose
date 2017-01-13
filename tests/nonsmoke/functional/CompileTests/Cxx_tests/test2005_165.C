/*
Bug reported by Andreas (10/6/2005).

with ROSE gives the following error message

In build_EDG_CommandLine(): Option -c not found (compile AND link) set
autoInstantiation = true ...
In build_EDG_CommandLine(): autoInstantiation = true adding
--auto_instantiation -tused ...
"genfc.cpp", line 8: error: argument of type "const char *" is incompatible
          with parameter of type "void *"
    va_start(ap, fmt);
    ^

"genfc.cpp", line 8: warning: variable "ap" is used before its value is set
    va_start(ap, fmt);
    ^

Errors in EDG Processing!
Aborted
 */

#include <stdarg.h>

//-----------------------------------------------------------------------------
void eform2 (const char* fmt, ...)
//-----------------------------------------------------------------------------
{
  va_list ap;
  va_start(ap, fmt);
}

