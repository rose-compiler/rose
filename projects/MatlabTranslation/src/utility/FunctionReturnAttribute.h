#ifndef ROSE_UTILS_FUNCTION_RETURN_ATTRIBUTE_H
#define ROSE_UTILS_FUNCTION_RETURN_ATTRIBUTE_H

#include "rose.h"

/*
This attribute stores the return variable of a matlab function

eg. function [a, b] = getAB()
      a = 5
      b = 6
    end

This attribute will store a, b
*/
class FunctionReturnAttribute : public AstAttribute
{
 private:
  SgExprListExp *returnList;
  
 public:
  FunctionReturnAttribute(SgExprListExp *returnList)
    {
      this->returnList = returnList;
    }
  
  void attachTo(SgNode *node)
  {
    node->setAttribute("RETURN_VARS", this);
  }

  static SgExprListExp* getReturnList(SgNode *node)
  {
    if(node->attributeExists("RETURN_VARS"))
      return ((FunctionReturnAttribute*)node->getAttribute("RETURN_VARS"))->returnList;
    else
      return NULL;
  }
  
};

#endif
