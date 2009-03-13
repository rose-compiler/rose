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
private:
  RoseEnums re;
  PrologCompTerm* getPreprocessingInfo(AttachedPreprocessingInfoType*);
  PrologCompTerm* getFunctionDeclarationSpecific(SgFunctionDeclaration*);
  PrologCompTerm* getUnaryOpSpecific(SgUnaryOp*);
  PrologCompTerm* getBinaryOpSpecific(SgBinaryOp*);
  PrologCompTerm* getValueExpSpecific(SgValueExp*);
  PrologCompTerm* getInitializedNameSpecific(SgInitializedName*);
  PrologCompTerm* getVarRefExpSpecific(SgVarRefExp*);
  PrologCompTerm* getAssignInitializerSpecific(SgAssignInitializer*);
  PrologTerm* getTypeSpecific(SgType*);
  PrologCompTerm* getFunctionTypeSpecific(SgType*);
  PrologCompTerm* getPointerTypeSpecific(SgType*);
  PrologCompTerm* getClassTypeSpecific(SgType*);
  PrologCompTerm* getTypedefTypeSpecific(SgType*);
  PrologCompTerm* getEnumTypeSpecific(SgType*);
  PrologCompTerm* getClassDeclarationSpecific(SgClassDeclaration*);
  PrologCompTerm* getClassDefinitionSpecific(SgClassDefinition*);
  PrologTerm* getBitVector(const SgBitVector&, const std::vector<std::string>&);
  PrologTerm* getEnum(int enum_val, const std::vector<std::string>&);

  PrologCompTerm* getLabelStatementSpecific(SgLabelStatement*);
  PrologCompTerm* getGotoStatementSpecific(SgGotoStatement*);
  PrologCompTerm* getConditionalExpSpecific(SgConditionalExp*);
  PrologCompTerm* getEnumDeclarationSpecific(SgEnumDeclaration*);
  PrologCompTerm* getDeclarationAttributes(SgDeclarationStatement*);
  PrologTerm* traverseSingleNode(SgNode*);
  PrologCompTerm* getDeleteExpSpecific(SgDeleteExp*);
  PrologCompTerm* getRefExpSpecific(SgRefExp*);
  PrologCompTerm* getVarArgSpecific(SgExpression*);
  //PrologCompTerm* getAccessModifierSpecific(SgAccessModifier*);
  PrologCompTerm* getBaseClassModifierSpecific(SgBaseClassModifier*); 
  PrologCompTerm* getFunctionModifierSpecific(SgFunctionModifier*);
  PrologCompTerm* getSpecialFunctionModifierSpecific(SgSpecialFunctionModifier*);
  PrologCompTerm* getLinkageModifierSpecific(SgLinkageModifier*);
  PrologAtom* createStorageModifierAtom(SgStorageModifier&);
  PrologAtom* createAccessModifierAtom(SgAccessModifier&);
  PrologCompTerm* getStorageModifierSpecific(SgStorageModifier*);
  PrologCompTerm* getElaboratedTypeModifierSpecific(SgElaboratedTypeModifier*);
  PrologAtom* createConstVolatileModifierAtom(SgConstVolatileModifier&);
  PrologCompTerm* getConstVolatileModifierSpecific(SgConstVolatileModifier*);
  PrologCompTerm* getUPC_AccessModifierSpecific(SgUPC_AccessModifier*);
  PrologCompTerm* getTypeModifierSpecific(SgTypeModifier*);
  PrologCompTerm* getDeclarationModifierSpecific(SgDeclarationModifier*);
  PrologCompTerm* getArrayTypeSpecific(SgType*);
  PrologCompTerm* getModifierTypeSpecific(SgType*);
  PrologCompTerm* getFunctionRefExpSpecific(SgFunctionRefExp*);
  PrologCompTerm* getFunctionCallExpSpecific(SgFunctionCallExp*);
  PrologCompTerm* getMemberFunctionDeclarationSpecific(SgMemberFunctionDeclaration*);
  PrologTerm* getTypePtrListSpecific(SgTypePtrList&);
  PrologCompTerm* getMemberFunctionTypeSpecific(SgType*);
  PrologCompTerm* getClassScopeName(SgClassDefinition*);
  PrologCompTerm* getMemberFunctionSymbolSpecific(SgMemberFunctionSymbol*);
  PrologCompTerm* getMemberFunctionRefExpSpecific(SgMemberFunctionRefExp*);
  PrologCompTerm* getNamespaceScopeName(SgNamespaceDefinitionStatement*);
  PrologCompTerm* getNamespaceDeclarationStatementSpecific(SgNamespaceDeclarationStatement*);
  PrologCompTerm* getSizeOfOpSpecific(SgSizeOfOp*);
  PrologCompTerm* getVariableDeclarationSpecific(SgVariableDeclaration*);
  PrologCompTerm* getTypedefDeclarationSpecific(SgTypedefDeclaration*);
  PrologCompTerm* getConstructorInitializerSpecific(SgConstructorInitializer*);
  PrologCompTerm* getNewExpSpecific(SgNewExp*);

  PrologCompTerm* getPragmaSpecific(SgPragma*);
		
  static char toLower(const char);
  static bool isUpper(const char);
		
	       	
};

#endif
