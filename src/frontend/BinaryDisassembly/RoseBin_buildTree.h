/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that actually builds the Rose Tree
 ****************************************************/

#ifndef __RoseBin_buildTree__
#define __RoseBin_buildTree__

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>

// #include "rose.h"
#include "RoseBin_IDAPRO_exprTree.h"
// #include "RoseBin_support.h"

class RoseBin_buildTree  {
 protected:
  std::map <SgAsmNode*, exprTreeType> debugHelpMap;


  /****************************************************
   * return information about the register
   ****************************************************/
  void resolveRegisterArm(std::string symbol, RegisterDescriptor*);
  void resolveRegisterX86(std::string symbol, RegisterDescriptor*);

 public:
  RoseBin_buildTree() {};


  /****************************************************
   * return debug information from the helpMap
   ****************************************************/
  exprTreeType getDebugHelp (SgAsmNode* sgBinNode);

};

#endif



