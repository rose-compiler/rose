#include <CFG.h>
#include <PtrMap.h>
#include <ProcessAstTree.h>
#include <CommandOptions.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

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

