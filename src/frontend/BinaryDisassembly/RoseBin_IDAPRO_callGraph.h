/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that represents the call graph
 ****************************************************/
#ifndef __RoseBin_CALLGRAPH__
#define __RoseBin_CALLGRAPH__

#include <stdio.h>
#include <iostream>


class CallGraphType {
 public:
  int id;
  int src;
  int src_basic_block_id;
  int src_address;
  int dst;
};

#endif

