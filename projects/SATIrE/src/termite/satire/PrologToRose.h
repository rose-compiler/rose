/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGTOROSE_H_
#define PROLOGTOROSE_H_
#include "rose.h"
#include "TermRep.h"
#include <vector>

using namespace std;
/**
 * Class for creating a ROSE-IR (made for unparsing)
 * from its PROLOG term-representation
 */
class PrologToRose {
	public:
		static SgNode* toRose(PrologTerm*);
/*public for testing.
 * private:*/
		/* arity specific node generation*/
		static SgNode* leafToRose(PrologCompTerm*,string);
		static SgNode* unaryToRose(PrologCompTerm*,string);
		static SgNode* binaryToRose(PrologCompTerm*,string);
		static SgNode* ternaryToRose(PrologCompTerm*,string);
		static SgNode* quaternaryToRose(PrologCompTerm*,string);
		static SgNode* listToRose(PrologCompTerm*,string);
		/*helpers*/
		static void warn_msg(string);
		static Sg_File_Info* createFileInfo(PrologTerm*);
		static SgType* createType(PrologTerm*);
		static SgFunctionType* createFunctionType(PrologTerm*);
		static SgMemberFunctionType* createMemberFunctionType(PrologTerm*);
		static SgClassType* createClassType(PrologTerm*);
		static SgPointerType* createPointerType(PrologTerm*);
		static SgEnumType* createEnumType(PrologTerm*);
		static SgReferenceType* createReferenceType(PrologTerm*);
		static SgArrayType* createArrayType(PrologTerm*);
		static PrologCompTerm* retrieveAnnotation(PrologCompTerm*);
		static void abort_unless(bool,string);
		static void debug(string);
		static bool isValueExp(string);
		static bool isUnaryOp(string);	
		static bool isBinaryOp(string);
		static SgInitializedName* inameFromAnnot(PrologCompTerm*);
		static void testFileInfo(Sg_File_Info*);
		static SgClassDeclaration* createDummyClassDeclaration(string,int);
		static SgMemberFunctionDeclaration* createDummyMemberFunctionDeclaration(string s,int c_type); /* TODO */
		static SgBitVector* createBitVector(PrologTerm*);
		static SgLabelStatement* makeLabel(Sg_File_Info*,string);
		static PrologInt* isPrologInt(PrologTerm*);
		static PrologList* isPrologList(PrologTerm*);
		static PrologCompTerm* isPrologCompTerm(PrologTerm*);
		static PrologAtom* isPrologAtom(PrologTerm*);
		static PrologString* isPrologString(PrologTerm*);
		static string* toStringP(PrologTerm*);
		static int toInt(PrologTerm*);
		static void pciDeclarationStatement(SgDeclarationStatement*,PrologTerm*);
		static void fakeParentScope(SgDeclarationStatement*);
		static void fakeClassScope(string,int,SgDeclarationStatement*);
		static SgAccessModifier* createAccessModifier(PrologTerm*);
		static SgBaseClassModifier* createBaseClassModifier(PrologTerm*);
		static SgFunctionModifier* createFunctionModifier(PrologTerm*);
		static SgSpecialFunctionModifier* createSpecialFunctionModifier(PrologTerm*);
		static SgStorageModifier* createStorageModifier(PrologTerm*);
		static SgLinkageModifier* createLinkageModifier(PrologTerm*);
		static SgElaboratedTypeModifier* createElaboratedTypeModifier(PrologTerm*);
		static SgConstVolatileModifier* createConstVolatileModifier(PrologTerm*);
		static SgUPC_AccessModifier* createUPC_AccessModifier(PrologTerm*);
		static SgTypeModifier* createTypeModifier(PrologTerm*);
		static void setTypeModifier(PrologTerm*,SgTypeModifier*);
		static SgDeclarationModifier* createDeclarationModifier(PrologTerm*);
		static void setDeclarationModifier(PrologTerm*,SgDeclarationModifier*);
		static SgModifierType* createModifierType(PrologTerm*);
		static SgFunctionDeclaration* createDummyFunctionDeclaration(string*,PrologTerm*);
		static SgFunctionSymbol* createDummyFunctionSymbol(string*,PrologTerm*);
		static SgMemberFunctionSymbol* createDummyMemberFunctionSymbol(PrologTerm*);
		static void fakeNamespaceScope(string,int,SgDeclarationStatement*);
		static SgTypedefType* createTypedefType(PrologTerm*);
		/* type specific node generation */
		/*unary nodes*/
		static SgExpression* createValueExp(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgUnaryOp* createUnaryOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgFile* createFile(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgReturnStmt* createReturnStmt(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgFunctionDefinition* createFunctionDefinition(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgInitializedName* createInitializedName(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgAssignInitializer* createAssignInitializer(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgExprStatement* createExprStatement(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgDefaultOptionStmt* createDefaultOptionStmt(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgClassDeclaration* createClassDeclaration(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgDeleteExp* createDeleteExp(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgVarArgOp* createVarArgOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgVarArgStartOneOperandOp* createVarArgStartOneOperandOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgVarArgEndOp* createVarArgEndOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgAggregateInitializer* createAggregateInitializer(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgNamespaceDeclarationStatement* createNamespaceDeclarationStatement(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgSizeOfOp* createSizeOfOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgConstructorInitializer* createConstructorInitializer(Sg_File_Info*,SgNode*,PrologCompTerm*);
		static SgPragmaDeclaration* createPragmaDeclaration(Sg_File_Info*,SgNode*,PrologCompTerm*);
	
		/*binary nodes*/
		static SgFunctionDeclaration* createFunctionDeclaration(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgBinaryOp* createBinaryOp(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgSwitchStatement* createSwitchStatement(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgDoWhileStmt* createDoWhileStmt(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgWhileStmt* createWhileStmt(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgVarArgCopyOp* createVarArgCopyOp(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgVarArgStartOp* createVarArgStartOp(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgFunctionCallExp* createFunctionCallExp(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgTryStmt* createTryStmt(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		static SgCatchOptionStmt* createCatchOptionStmt(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
		/*ternary nodes*/
		static SgIfStmt* createIfStmt(Sg_File_Info*, SgNode*, SgNode*, SgNode*, PrologCompTerm*);
		static SgCaseOptionStmt* createCaseOptionStmt(Sg_File_Info*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
		static SgMemberFunctionDeclaration* createMemberFunctionDeclaration(Sg_File_Info*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
		static SgNewExp* createNewExp(Sg_File_Info*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
		static SgConditionalExp* createConditionalExp(Sg_File_Info*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
		/*quaternary nodes*/
		static SgForStatement* createForStatement(Sg_File_Info*,SgNode*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
		/*list nodes*/
		static SgFunctionParameterList* createFunctionParameterList(Sg_File_Info*, vector<SgNode*>*);
		static SgBasicBlock* createBasicBlock(Sg_File_Info*,vector<SgNode*>*);
		static SgGlobal* createGlobal(Sg_File_Info*,vector<SgNode*>*);
		static SgVariableDeclaration* createVariableDeclaration(Sg_File_Info*,vector<SgNode*>*,PrologCompTerm*);
		static SgForInitStatement* createForInitStatement(Sg_File_Info*,vector<SgNode*>*);
		static SgClassDefinition* createClassDefinition(Sg_File_Info*,vector<SgNode*>*,PrologCompTerm* t);
		static SgCtorInitializerList* createCtorInitializerList(Sg_File_Info*,vector<SgNode*>*);
		static SgEnumDeclaration* createEnumDeclaration(Sg_File_Info*,vector<SgNode*>*,PrologCompTerm*);
		static SgExprListExp* createExprListExp(Sg_File_Info*,vector<SgNode*>*);
		static SgNamespaceDefinitionStatement* createNamespaceDefinitionStatement(Sg_File_Info*,vector<SgNode*>*);
		static SgCatchStatementSeq* createCatchStatementSeq(Sg_File_Info*,vector<SgNode*>*);
		/*leaf nodes*/
		static SgVarRefExp* createVarRefExp(Sg_File_Info*,PrologCompTerm*);
		static SgBreakStmt* createBreakStmt(Sg_File_Info*,PrologCompTerm*);
		static SgContinueStmt* createContinueStmt(Sg_File_Info*,PrologCompTerm*);
		static SgLabelStatement* createLabelStatement(Sg_File_Info*,PrologCompTerm*);
		static SgGotoStatement* createGotoStatement(Sg_File_Info*,PrologCompTerm*);
		static SgRefExp* createRefExp(Sg_File_Info*,PrologCompTerm*);
		static SgFunctionRefExp* createFunctionRefExp(Sg_File_Info*,PrologCompTerm*);
		static SgMemberFunctionRefExp* createMemberFunctionRefExp(Sg_File_Info*,PrologCompTerm*);
		static SgThisExp* createThisExp(Sg_File_Info*,PrologCompTerm*);
		static SgTypedefDeclaration* createTypedefDeclaration(Sg_File_Info*,PrologCompTerm*);
		static SgPragma* createPragma(Sg_File_Info*,PrologCompTerm*);
		static char unescape_char(std::string s);
};
#endif
