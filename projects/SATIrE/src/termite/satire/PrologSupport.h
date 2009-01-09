/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/

#ifndef PROLOGSUPPORT_H_
#define PROLOGSUPPORT_H_ 
#include <satire_rose.h>
#include "termite.h"
#include <string>
#include <vector>

/**
 * Class supporting generation of PROLOG trees representing ROSE-IR
 *
 */
class PrologSupport {
	public:
		static void addSpecific(SgNode*,PrologCompTerm*);
		static PrologCompTerm* getFileInfo(Sg_File_Info*);
		static std::string prologize(std::string);
                static std::string escape_string(std::string s);
	private:
		static PrologTerm* getFunctionDeclarationSpecific(SgFunctionDeclaration*);
		static PrologTerm* getUnaryOpSpecific(SgUnaryOp*);
		static PrologTerm* getBinaryOpSpecific(SgBinaryOp*);
		static PrologTerm* getValueExpSpecific(SgValueExp*);
		static PrologTerm* getInitializedNameSpecific(SgInitializedName*);
		static PrologTerm* getVarRefExpSpecific(SgVarRefExp*);
		static PrologTerm* getAssignInitializerSpecific(SgAssignInitializer*);
		static PrologTerm* getTypeSpecific(SgType*);
		static PrologTerm* getFunctionTypeSpecific(SgType*);
		static PrologTerm* getPointerTypeSpecific(SgType*);
		static PrologTerm* getClassTypeSpecific(SgType*);
		static PrologTerm* getTypedefTypeSpecific(SgType*);
		static PrologTerm* getEnumTypeSpecific(SgType*);
		static PrologTerm* getClassDeclarationSpecific(SgClassDeclaration*);
		static PrologTerm* getClassDefinitionSpecific(SgClassDefinition*);
		static PrologTerm* getBitVector(SgBitVector v);
		static PrologTerm* getLabelStatementSpecific(SgLabelStatement*);
		static PrologTerm* getGotoStatementSpecific(SgGotoStatement*);
		static PrologTerm* getConditionalExpSpecific(SgConditionalExp*);
		static PrologTerm* getEnumDeclarationSpecific(SgEnumDeclaration*);
		static PrologTerm* getDeclarationAttributes(SgDeclarationStatement*);
		static PrologTerm* traverseSingleNode(SgNode*);
		static PrologTerm* getDeleteExpSpecific(SgDeleteExp*);
		static PrologTerm* getRefExpSpecific(SgRefExp*);
		static PrologTerm* getVarArgSpecific(SgExpression*);
		static PrologTerm* getAccessModifierSpecific(SgAccessModifier*);
		static PrologTerm* getBaseClassModifierSpecific(SgBaseClassModifier*); 
		static PrologTerm* getFunctionModifierSpecific(SgFunctionModifier*);
		static PrologTerm* getSpecialFunctionModifierSpecific(SgSpecialFunctionModifier*);
		static PrologTerm* getLinkageModifierSpecific(SgLinkageModifier*);
        static PrologAtom* createStorageModifierAtom(SgStorageModifier&);
		static PrologTerm* getStorageModifierSpecific(SgStorageModifier*);
		static PrologTerm* getElaboratedTypeModifierSpecific(SgElaboratedTypeModifier*);
        static PrologAtom* createConstVolatileModifierAtom(SgConstVolatileModifier&);
		static PrologTerm* getConstVolatileModifierSpecific(SgConstVolatileModifier*);
		static PrologTerm* getUPC_AccessModifierSpecific(SgUPC_AccessModifier*);
		static PrologTerm* getTypeModifierSpecific(SgTypeModifier*);
		static PrologTerm* getDeclarationModifierSpecific(SgDeclarationModifier*);
		static PrologTerm* getArrayTypeSpecific(SgType*);
		static PrologTerm* getModifierTypeSpecific(SgType*);
		static PrologTerm* getFunctionRefExpSpecific(SgFunctionRefExp*);
		static PrologTerm* getFunctionCallExpSpecific(SgFunctionCallExp*);
		static PrologTerm* getMemberFunctionDeclarationSpecific(SgMemberFunctionDeclaration*);
		static PrologTerm* getTypePtrListSpecific(SgTypePtrList&);
		static PrologTerm* getMemberFunctionTypeSpecific(SgType*);
		static PrologTerm* getClassScopeName(SgClassDefinition*);
		static PrologTerm* getMemberFunctionSymbolSpecific(SgMemberFunctionSymbol*);
		static PrologTerm* getMemberFunctionRefExpSpecific(SgMemberFunctionRefExp*);
		static PrologTerm* getNamespaceScopeName(SgNamespaceDefinitionStatement*);
		static PrologTerm* getNamespaceDeclarationStatementSpecific(SgNamespaceDeclarationStatement*);
		static PrologTerm* getSizeOfOpSpecific(SgSizeOfOp*);
		static PrologTerm* getVariableDeclarationSpecific(SgVariableDeclaration*);
		static PrologTerm* getTypedefDeclarationSpecific(SgTypedefDeclaration*);
		static PrologTerm* getConstructorInitializerSpecific(SgConstructorInitializer*);
		static PrologTerm* getNewExpSpecific(SgNewExp*);

		static PrologTerm* getPragmaSpecific(SgPragma*);
		
		static char toLower(const char);
		static bool isUpper(const char);
		
	       	
};
#endif
