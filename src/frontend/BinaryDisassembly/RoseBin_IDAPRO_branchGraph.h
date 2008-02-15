/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that represents the branch graph
 *              Branches appear between blocks (src, dst) 
 ****************************************************/
#ifndef __RoseBin_BRANCHGRAPH__
#define __RoseBin_BRANCHGRAPH__

#include <stdio.h>
#include <iostream>


class BranchGraphType {
 public:
  int id;
  int parent_function;
  int src;
  int dst;
  int kind;
};

#endif

