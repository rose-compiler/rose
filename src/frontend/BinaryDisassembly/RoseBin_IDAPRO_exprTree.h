/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that represents the expression types
 ****************************************************/
#ifndef __RoseBin_EXPRTREE__
#define __RoseBin_EXPRTREE__

#include <stdio.h>
#include <iostream>


class exprTreeType {
 public:
  int id;
  int expr_type;
  std::string symbol;
  int immediate;
  int position;
  int parent_id;
};

#endif

