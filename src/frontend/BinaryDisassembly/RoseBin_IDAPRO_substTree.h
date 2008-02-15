/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that represents the substitutions
 ****************************************************/

#ifndef __RoseBin_SUBSTTREE__
#define __RoseBin_SUBSTTREE__

#include <stdio.h>
#include <iostream>


class exprSubstitutionType {
 public:
  int id;
  int address;
  int operand_id;
  int expr_id;
  std::string replacement;
};

#endif

