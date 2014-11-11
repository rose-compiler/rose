/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 6Jun07
 * Decription : Interface for analysis to compare
 *              Source Tree with Binary Tree
 ****************************************************/

#ifndef __RoseBin_ASTAttribute__
#define __RoseBin_ASTAttribute__

// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "rose.h"


/* *****************************************
 * Using this to store Attributes to AST
 * *****************************************/
class MyAstAttribute : public AstAttribute {
private:
  int val;
public:
  MyAstAttribute(int v):val(v){};
  int getVal() {return val;};

};


#endif

