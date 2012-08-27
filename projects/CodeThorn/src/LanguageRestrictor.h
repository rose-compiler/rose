#ifndef LANGUAGE_RESTRICTOR_H
#define LANGUAGE_RESTRICTOR_H

#include "rose.h"
#include <set>
#include <string>
#include "MyAst.h"

using namespace std;

class LanguageRestrictor {
 public:
  typedef set<VariantT> VariantSet;
  void allowAllAstNodes();
  void disallowAllAstNodes();
  VariantSet computeVariantSetOfProvidedAst(SgNode*);
  string variantToString(VariantT variant);
  void allowAstNodesRequiredForEmptyProgram();
  void setAstNodeVariant(VariantT variant, bool allowed);
  void setAstNodeVariantSet(VariantSet variants, bool allowed);
  bool isAllowedAstNode(SgNode* node);
  bool isAllowedAstNodeVariant(VariantT variant);
  bool isAllowedAstNodeVariantSet(VariantSet variants);
  VariantSet getAllowedAstNodeVariantSet();
  virtual bool checkIfAstIsAllowed(SgNode* node); 
 private:
  void ensureVariantIsValid(VariantT variant);
  bool variants[V_SgNumVariants];
};

#endif
