// tps (12/09/2009) : Playing with precompiled headers in Windows. Requires rose.h as the first line in source files.
// tps : Switching from rose.h to sage3 changed size from 18,5 MB to 1,5MB
//#include "sage3basic.h"
#include "DefUseChain.h"
#include "StmtInfoCollect.h"
#include "SinglyLinkedList.h"
#include "CommandOptions.h"
#include "GraphUtils.h"
#include "GraphIO.h"

#include <vector>



void DefUseChainNode::Dump() const
{
  if (isdef) 
    std::cerr << "definition: ";
  else
    std::cerr << "use: ";
  std::cerr << AstToString(ref) << " : " << AstToString(stmt) << std::endl;
}

std::string DefUseChainNode::toString() const
{
  std::string result;
  if (isdef)
     result = "definition:";
  else
    result = "use:";
  result = result + AstToString(ref) + ":" + AstToString(stmt) + " ";
  return result;
}


bool DebugDefUseChain()
{
  static int r =  0;
  if (r == 0) {
     if (CmdOptions::GetInstance()->HasOption("-debugdefusechain"))
         r = 1;
     else
         r = -1;
   }
  return r == 1;
}
