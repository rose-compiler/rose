/*
 *  Created on: Feb 21, 2011
 *      Author: didem
 */

#ifndef LOOPUNROLLOPT_H
#define LOOPUNROLLOPT_H

#include "rose.h"
#include <string>

using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

class LoopUnrollOptimizer
{

public:
  LoopUnrollOptimizer();
  virtual ~LoopUnrollOptimizer();
  
  static bool unrollShortLoops(SgFunctionDeclaration* kernel);

private:

  static bool unrollShortLoop(SgForStatement* loop, size_t unrolling_factor);

  static int findUnrollingFactor(SgForStatement* target_loop);
};

#endif 
