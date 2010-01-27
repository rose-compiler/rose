// tps : Switching from rose.h to sage3 changed size from 17,2 MB to 0,169MB
// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.
#ifdef _MSC_VER
// seems to cause problems under Linux
//#include "sage3basic.h"
#endif

#include <CFG.h>
#include <PtrMap.h>
#include <ProcessAstTree.h>
#include <CommandOptions.h>

bool debug_cfg()
{
  static int r = 0;
  if (r == 0) {
    if ( CmdOptions::GetInstance()->HasOption("-debugcfg"))
        r = 1;
    else
        r = -1;
  }
  return r == 1;
}

