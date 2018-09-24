#ifndef FUNCDECL_H
#define FUNCDECL_H

#include <iostream>
#include <cstdlib>
//#include "util.h"
#include "strDecl.h"
//#include "rose.h"

#include <boost/lexical_cast.hpp>

bool isExternal(SgName fnName);

//#define FUNFACT_DEBUG

class funcDecl {
  protected:
  SgName fnName;
  SgExprListExp* paramList;
  SgType* retType;
  SgScopeStatement* scope;
  SgNode* pos;
  SgFunctionDeclaration* fndecl;
  bool derived;
  SgName transfName;
  SgFunctionParameterList* args;

  virtual void placeTheDecl() = 0;
  void makeDeclStatic();

  funcDecl(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos,
      bool derived
      );


  public:
  virtual void fillOutBody() = 0;
  SgFunctionDeclaration* getDecl() { return fndecl; }
};

class nonDefDecl : public funcDecl {
  friend class funFact;


  void placeTheDecl();

  void fillOutBody() { }

  nonDefDecl(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived);
};



class defDecl : public funcDecl {

  private:
  void placeTheDecl();

  protected:
  SgFunctionDefinition* def;
  SgBasicBlock* body;

  SgStatementPtrList &getStmts();
  SgInitializedNamePtrList& getArgs();

  void insertReturnStmt(SgExpression* ret_exp);
  void appendStmtToBody(SgStatement* stmt);



  public:
  defDecl(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived);

  virtual void fillOutBody() = 0;
};

class createStrFn : public defDecl {
  friend class funFact;
  createStrFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class createStrFromAddrFn : public defDecl {
  friend class funFact;
  createStrFromAddrFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }

  virtual void fillOutBody();
};

class rtcCopyFn : public defDecl {
  friend class funFact;

  rtcCopyFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class addressOfFn : public defDecl {
  friend class funFact;
  addressOfFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class derefFn : public defDecl {
  friend class funFact;
  derefFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class pntrArrRefFn : public defDecl {
  friend class funFact;
  pntrArrRefFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos,derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class assignFn : public defDecl {
  friend class funFact;
  assignFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class incrDecrFn : public defDecl {
  bool isDecr;

  friend class funFact;
  incrDecrFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool isDecr, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived),
      isDecr(isDecr) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class incrDecrDerefFn : public defDecl {
  bool isDecr;

  friend class funFact;
  incrDecrDerefFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool isDecr, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived),
      isDecr(isDecr) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class condFn : public defDecl {
  friend class funFact;
  condFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }

  SgExpression* getExp(SgInitializedName* arg);

  SgExpression* createConditionalExp(SgExpression* lhs, SgExpression* rhs);
  virtual void fillOutBody();
};

class addSubFn : public defDecl {
  bool isSub;

  friend class funFact;
  addSubFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool isSub, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived),
      isSub(isSub) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class addSubDerefFn : public defDecl {
  bool isSub;

  friend class funFact;
  addSubDerefFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool isSub, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived),
      isSub(isSub) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class castFn : public defDecl {
  friend class funFact;
  castFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }

  void varRefToStr();
  void valToStr();
  void strToVal();
  void strToStr();
  virtual void fillOutBody();
};

class plusMinusFn : public defDecl {
  bool isMinus;

  friend class funFact;
  plusMinusFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool isMinus, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived),
      isMinus(isMinus) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class ptrCheckFn : public defDecl {
  friend class funFact;
  ptrCheckFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class derefCheckFn : public defDecl {
  friend class funFact;
  derefCheckFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();

};

class derefCheckWithStrFn : public defDecl {
  friend class funFact;
  derefCheckWithStrFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class assignAndCopyFn : public defDecl {
  friend class funFact;
  assignAndCopyFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class derefAssignAndCopyFn : public defDecl {
  friend class funFact;
  derefAssignAndCopyFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class lhsDerefAssignAndCopyFn : public defDecl {
  friend class funFact;
  lhsDerefAssignAndCopyFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class rhsDerefAssignAndCopyFn : public defDecl {
  friend class funFact;
  rhsDerefAssignAndCopyFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class doubleDerefCheckFn : public defDecl {
  friend class funFact;
  doubleDerefCheckFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class derefCreateStrFn : public defDecl {
  friend class funFact;
  derefCreateStrFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class returnPtrFn : public defDecl {
  friend class funFact;
  returnPtrFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class createStrAssignAndCopyFn : public defDecl {
  friend class funFact;
  createStrAssignAndCopyFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }
  virtual void fillOutBody();
};

class createEntryWithNewLockGenFn : public defDecl {
  friend class funFact;
  createEntryWithNewLockGenFn(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived) : defDecl(fnName, paramList, retType, scope, pos, derived) {
    // the function would be built by now...
    // nothign to do here...
  }

  virtual void fillOutBody();

};

class funFact {

  static SgFunctionDeclaration* build(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos, bool derived);


  public:

  static SgFunctionCallExp* buildDerivedFuncCall(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos);

  static SgFunctionCallExp* buildFuncCall(SgName fnName,
      SgExprListExp* paramList,
      SgType* retType,
      SgScopeStatement* scope,
      SgNode* pos);

  static SgFunctionCallExp* buildOrCreate(SgName fnName, SgExprListExp* paramList, SgType* retType, SgScopeStatement* scope, SgNode* pos, bool derived);

  static SgNode* correctPositionIfNecessary(SgExprListExp* paramList, SgType* retType, SgNode* pos, bool derived);

  static SgFunctionDeclaration* getFuncDeclInScope(SgName transfName, SgScopeStatement* scope);

  static bool funcExists(SgName transfName, SgScopeStatement* scope);

    static SgScopeStatement* getInsertScope(SgName transfName, SgExprListExp* paramList, SgType* retType, SgNode* pos, bool derived, SgName fnName);
};

#endif
