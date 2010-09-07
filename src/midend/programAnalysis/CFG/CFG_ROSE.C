

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

