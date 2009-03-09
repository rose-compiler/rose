/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/

#ifndef PROLOGSUPPORT_H_
#define PROLOGSUPPORT_H_ 
#include <satire_rose.h>
#include "termite.h"
#include "RoseEnums.h"
#include <string>
#include <vector>

/**
 * Class supporting generation of PROLOG trees representing ROSE-IR
 *
 */
class RoseToProlog {
public:
  void addSpecific(SgNode*,PrologCompTerm*);
  PrologCompTerm* getFileInfo(Sg_File_Info*);
  static std::string prologize(std::string);
  static std::string escape_string(std::string s);
private:
  RoseEnums re;;
  PrologTerm* getFunctionDeclarationSpecific(SgFunctionDeclaration*);
  PrologTerm* getUnaryOpSpecific(SgUnaryOp*);
  PrologTerm* getBinaryOpSpecific(SgBinaryOp*);
  PrologTerm* getValueExpSpecific(SgValueExp*);
  PrologTerm* getInitializedNameSpecific(SgInitializedName*);
  PrologTerm* getVarRefExpSpecific(SgVarRefExp*);
  PrologTerm* getAssignInitializerSpecific(SgAssignInitializer*);
  PrologTerm* getTypeSpecific(SgType*);
  PrologTerm* getFunctionTypeSpecific(SgType*);
  PrologTerm* getPointerTypeSpecific(SgType*);
  PrologTerm* getClassTypeSpecific(SgType*);
  PrologTerm* getTypedefTypeSpecific(SgType*);
  PrologTerm* getEnumTypeSpecific(SgType*);
  PrologTerm* getClassDeclarationSpecific(SgClassDeclaration*);
  PrologTerm* getClassDefinitionSpecific(SgClassDefinition*);
  PrologTerm* getBitVector(const SgBitVector&, const std::vector<std::string>&);
  PrologTerm* getEnum(int enum_val, const std::vector<std::string>&);

  PrologTerm* getLabelStatementSpecific(SgLabelStatement*);
  PrologTerm* getGotoStatementSpecific(SgGotoStatement*);
  PrologTerm* getConditionalExpSpecific(SgConditionalExp*);
  PrologTerm* getEnumDeclarationSpecific(SgEnumDeclaration*);
  PrologTerm* getDeclarationAttributes(SgDeclarationStatement*);
  PrologTerm* traverseSingleNode(SgNode*);
  PrologTerm* getDeleteExpSpecific(SgDeleteExp*);
  PrologTerm* getRefExpSpecific(SgRefExp*);
  PrologTerm* getVarArgSpecific(SgExpression*);
  //PrologTerm* getAccessModifierSpecific(SgAccessModifier*);
  PrologTerm* getBaseClassModifierSpecific(SgBaseClassModifier*); 
  PrologTerm* getFunctionModifierSpecific(SgFunctionModifier*);
  PrologTerm* getSpecialFunctionModifierSpecific(SgSpecialFunctionModifier*);
  PrologTerm* getLinkageModifierSpecific(SgLinkageModifier*);
  PrologAtom* createStorageModifierAtom(SgStorageModifier&);
  PrologAtom* createAccessModifierAtom(SgAccessModifier&);
  PrologTerm* getStorageModifierSpecific(SgStorageModifier*);
  PrologTerm* getElaboratedTypeModifierSpecific(SgElaboratedTypeModifier*);
  PrologAtom* createConstVolatileModifierAtom(SgConstVolatileModifier&);
  PrologTerm* getConstVolatileModifierSpecific(SgConstVolatileModifier*);
  PrologTerm* getUPC_AccessModifierSpecific(SgUPC_AccessModifier*);
  PrologTerm* getTypeModifierSpecific(SgTypeModifier*);
  PrologTerm* getDeclarationModifierSpecific(SgDeclarationModifier*);
  PrologTerm* getArrayTypeSpecific(SgType*);
  PrologTerm* getModifierTypeSpecific(SgType*);
  PrologTerm* getFunctionRefExpSpecific(SgFunctionRefExp*);
  PrologTerm* getFunctionCallExpSpecific(SgFunctionCallExp*);
  PrologTerm* getMemberFunctionDeclarationSpecific(SgMemberFunctionDeclaration*);
  PrologTerm* getTypePtrListSpecific(SgTypePtrList&);
  PrologTerm* getMemberFunctionTypeSpecific(SgType*);
  PrologTerm* getClassScopeName(SgClassDefinition*);
  PrologTerm* getMemberFunctionSymbolSpecific(SgMemberFunctionSymbol*);
  PrologTerm* getMemberFunctionRefExpSpecific(SgMemberFunctionRefExp*);
  PrologTerm* getNamespaceScopeName(SgNamespaceDefinitionStatement*);
  PrologTerm* getNamespaceDeclarationStatementSpecific(SgNamespaceDeclarationStatement*);
  PrologTerm* getSizeOfOpSpecific(SgSizeOfOp*);
  PrologTerm* getVariableDeclarationSpecific(SgVariableDeclaration*);
  PrologTerm* getTypedefDeclarationSpecific(SgTypedefDeclaration*);
  PrologTerm* getConstructorInitializerSpecific(SgConstructorInitializer*);
  PrologTerm* getNewExpSpecific(SgNewExp*);

  PrologTerm* getPragmaSpecific(SgPragma*);
		
  static char toLower(const char);
  static bool isUpper(const char);
		
	       	
};

#endif
