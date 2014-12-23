#ifndef _MLMAPI_H
#define _MLMAPI_H

#include <rose.h>
#include "polyopt/PolyOpt.hpp"

namespace MLMAPIInsertion
{
class mlmAttribute : public AstAttribute
{
private:
  void init()
  {
    mlmType = 0;
  }
public:
  int mlmType;

  mlmAttribute()
  {
    init();
  }
  mlmAttribute(int type)
  {
    mlmType = type;
  }
  int getMemType()
  {
    return mlmType;
  }
};

class mlmFrontend : public AstSimpleProcessing
{
public:
  void attachAttribute(SgPragmaDeclaration*, AstAttribute*);
protected:
  void virtual visit (SgNode*);	
};

class mlmTransform : public AstSimpleProcessing
{
public:
  void insertHeaders(SgProject* project);
  void transformCallExp(SgCallExpression* callExp);
  void transformForStmt(SgForStatement* forStmt);
  bool loopTiling(SgForStatement*, size_t, size_t);
protected:
  void virtual visit (SgNode*);	
};

}

#endif  //_MLMAPI_H
