#include "LanguageRestrictor.h"

LanguageRestrictor::VariantSet LanguageRestrictor::computeVariantSetOfProvidedAst(SgNode* node) {
  LanguageRestrictor::VariantSet vset;
  MyAst ast(node);
  for(MyAst::iterator i=ast.begin();i!=ast.end();++i) {
	vset.insert((*i)->variantT());
  }
  return vset;
}

// the enum used for specifying the size is hard coded in the header file Cxx_Grammar.h
// but the way it is generated it is the same as specificied here
extern TerminalNamesType Cxx_GrammarTerminalNames[V_SgNumVariants+1]; 
std::string LanguageRestrictor::variantToString(VariantT variant) {
  assert(variant>=0 && variant<V_SgNumVariants);
  for(int i=0;i<V_SgNumVariants+1;++i) {
	if(Cxx_GrammarTerminalNames[i].variant==variant)
	  return Cxx_GrammarTerminalNames[i].name;
  }

  // we must have found a provided variant in array 'Cxx_GrammarTerminalNames'
  assert(false);
}

void LanguageRestrictor::ensureVariantIsValid(VariantT variant)  {
  assert(variant>=0 && variant<V_SgNumVariants);
}

void LanguageRestrictor::allowAllAstNodes() {
  for(int i=0; i<V_SgNumVariants;++i) {
	variants[i]=true;
  }
}

void LanguageRestrictor::disallowAllAstNodes() {
  for(int i=0; i<V_SgNumVariants;++i) {
	variants[i]=false;
  }
}

LanguageRestrictor::VariantSet LanguageRestrictor::getAllowedAstNodeVariantSet() {
  LanguageRestrictor::VariantSet vs;
  for(int i=0; i<V_SgNumVariants;++i) {
	if(variants[i]) 
	  vs.insert((VariantT)i); // MS: this conversion is guaranteed to be correct.
  }
  return vs;
}


void LanguageRestrictor::allowAstNodesRequiredForEmptyProgram() {
  disallowAllAstNodes();
  VariantSet s;
  s.insert(V_SgFileList);
  s.insert(V_SgBasicBlock);
  s.insert(V_SgFunctionDeclaration);
  s.insert(V_SgFunctionDefinition);
  s.insert(V_SgFunctionParameterList);
  s.insert(V_SgGlobal);
  s.insert(V_SgInitializedName);
  s.insert(V_SgProject);
  s.insert(V_SgSourceFile);
  setAstNodeVariantSet(s,true);
}

void LanguageRestrictor::setAstNodeVariant(VariantT variant, bool allowed) {
  variants[variant]=allowed;
}

void LanguageRestrictor::setAstNodeVariantSet(VariantSet variants, bool allowed) {
  for(LanguageRestrictor::VariantSet::iterator i=variants.begin();i!=variants.end();++i) {
	setAstNodeVariant(*i,allowed);
  }
}

bool LanguageRestrictor::isAllowedAstNodeVariant(VariantT variant) {
  return variants[variant];
}

bool LanguageRestrictor::isAllowedAstNodeVariantSet(VariantSet variants) {
  for(LanguageRestrictor::VariantSet::iterator i=variants.begin();i!=variants.end();++i) {
	if(!isAllowedAstNodeVariant(*i))
	  return false;
  }
  return true;
}

bool LanguageRestrictor::isAllowedAstNode(SgNode* node) {
  return isAllowedAstNodeVariant(node->variantT());
}

bool LanguageRestrictor::checkIfAstIsAllowed(SgNode* node) {
  MyAst ast(node);
  for(MyAst::iterator i=ast.begin();i!=ast.end();++i) {
	if(!isAllowedAstNode(*i)) {
	  cerr << "Error: Unsupported language construct found: " << (*i)->sage_class_name() << endl;
	  return false;
	}
  }
}

