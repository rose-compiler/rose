#ifndef STRDECL_H
#define STRDECL_H

// handle struct declaration

#include "util.h"

namespace strDecl
{
  static ScopeTypeSet TypesInScope;
#ifdef CPP_SCOPE_MGMT
  static Scope2TypeMap_t RelevantStructType;
#else /* CPP_SCOPE_MGMT */
  static TypeMap_t RelevantStructType;
#endif /* CPP_SCOPE_MGMT */

  SgType* getStructType(SgType* type, SgNode* pos, bool create);
  SgDeclarationStatement* getCorrectScopeForType(SgType* type, SgNode* pos);
  SgType* findInUnderlyingType(SgType* type);
  bool isValidStructType(SgType* type, SgStatement* stmt);
  bool isOriginalClassType(SgType* type);
  bool functionInteractsWithStdOrPrimitives(SgExprListExp* params, SgType* ret, SgNode* pos);
}

#endif /* STRDECL_H */
