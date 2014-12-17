/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Interface to user
 ****************************************************/

#ifndef __RoseObj__
#define __RoseObj__

#include "RoseBin_abstract.h"
#include <stdio.h>
#include <iostream>
// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "rose.h"


class RoseObj : public RoseBin_abstract {

 public:

  RoseObj() {
    RoseBin_support::setDataBaseSupport(false);    
  }
  
  ~RoseObj() {
  }

  void visit(SgNode* node) {};

};

#endif

