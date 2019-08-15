#ifndef LANGUAGE_RESTRICTOR_H
#define LANGUAGE_RESTRICTOR_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>
#include <string>
#include "RoseAst.h"

namespace CodeThorn {
/*!
  \brief Allows to check an AST for a restricted subset of C/C++.
  \details This class can be used to check the AST of an input program to contain only nodes of a certain subset of C/C++.
  \date 2012
  \author Markus Schordan
 */
class LanguageRestrictor {
 public:
  typedef std::set<VariantT> VariantSet;
  void allowAllAstNodes();
  void disallowAllAstNodes();
  VariantSet computeVariantSetOfProvidedAst(SgNode*);
  std::string variantToString(VariantT variant);
  void allowAstNodesRequiredForEmptyProgram();
  void setAstNodeVariant(VariantT variant, bool allowed);
  void setAstNodeVariantSet(VariantSet variants, bool allowed);
  bool isAllowedAstNode(SgNode* node);
  bool isAllowedAstNodeVariant(VariantT variant);
  bool isAllowedAstNodeVariantSet(VariantSet variants);
  VariantSet getAllowedAstNodeVariantSet();
  virtual bool checkProgram(SgNode* node);
 protected:
  virtual void initialize();
  virtual bool checkIfAstIsAllowed(SgNode* node); 
 private:
  void ensureVariantIsValid(VariantT variant);
  bool variants[V_SgNumVariants+1];
};

}

#endif
