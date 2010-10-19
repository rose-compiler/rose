/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that actually builds the Rose Tree
 ****************************************************/

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "RoseBin_buildTree.h"

using namespace std;


/****************************************************
 * return information about the register
 ****************************************************/
void RoseBin_buildTree::resolveRegisterX86(string symbol,
                                           RegisterDescriptor *registerSg)
{
    const RegisterDictionary *rdict = RegisterDictionary::dictionary_amd64();

    /* Symbol is upper case. Dictionary stores register names in lower case. */
    for (string::size_type i=0; i<symbol.size(); i++)
        symbol[i] = tolower(symbol[i]);

    const RegisterDescriptor *rdesc = rdict->lookup(symbol);
    if (rdesc) {
        *registerSg = *rdesc;
    } else {
        cerr <<"ERROR !!! ::: x86 symbol could not be resolved! : " <<symbol <<"\n" <<endl;
    }
}


void RoseBin_buildTree::resolveRegisterArm(string symbol,
                                           RegisterDescriptor *registerSg) {

  // ARM architecture
  const RegisterDictionary *rdict = RegisterDictionary::dictionary_arm7();

  /* Symbol is upper case. Dictionary stores register names in lower case. */
  for (string::size_type i=0; i<symbol.size(); i++)
    symbol[i] = tolower(symbol[i]);

  // Some of the names pased in here are not present in the dictionary. The dictionary doesn't have these names because these
  // registers are not always used for this purpose.
  if (symbol=="sp") {
    symbol = "r13";
  } else if (symbol=="pc") {
    symbol = "r15";
  } else if (symbol=="lr") {
    symbol = "r14";
  } else if (symbol=="sl") {
    symbol = "r10";
  } else if (symbol=="ip") {
    symbol = "r12";
  } else if (symbol=="fp") {
    symbol = "r11";
  }

  const RegisterDescriptor *rdesc = rdict->lookup(symbol);
  if (rdesc) {
    *registerSg = *rdesc;
  } else {
    cerr << "ERROR !!! ::: arm symbol could not be resolved! : " << symbol << "\n" << endl;
  }
}

  /****************************************************
   * return debug information from the helpMap
   ****************************************************/
exprTreeType RoseBin_buildTree::getDebugHelp (SgAsmNode* sgBinNode) {
  return debugHelpMap[sgBinNode];
}
