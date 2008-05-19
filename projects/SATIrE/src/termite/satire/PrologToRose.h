/*
Copyright 2006 Christoph Bonitz (christoph.bonitz@gmail.com)
see LICENSE in the root folder of this project
*/
#ifndef PROLOGTOROSE_H_
#define PROLOGTOROSE_H_
#include <satire_rose.h>
#include "TermRep.h"
#include <vector>
#include <iostream>

using namespace std;
/**
 * Class for creating a ROSE-IR (made for unparsing)
 * from its PROLOG term-representation
 */
class PrologToRose {
private:
  ofstream ofile;
public:
  PrologToRose(const char* filename) : ofile(filename) {};
  ~PrologToRose() { ofile.close(); };
  SgNode* toRose(PrologTerm*);
/*public for testing.
 * private:*/
  /* arity specific node generation*/
  SgNode* leafToRose(PrologCompTerm*,string);
  SgNode* unaryToRose(PrologCompTerm*,string);
  SgNode* binaryToRose(PrologCompTerm*,string);
  SgNode* ternaryToRose(PrologCompTerm*,string);
  SgNode* quaternaryToRose(PrologCompTerm*,string);
  SgNode* listToRose(PrologCompTerm*,string);
  /*helpers*/
  void warn_msg(string);
  Sg_File_Info* createFileInfo(PrologTerm*);
  SgType* createType(PrologTerm*);
  SgFunctionType* createFunctionType(PrologTerm*);
  SgMemberFunctionType* createMemberFunctionType(PrologTerm*);
  SgClassType* createClassType(PrologTerm*);
  SgPointerType* createPointerType(PrologTerm*);
  SgEnumType* createEnumType(PrologTerm*);
  SgReferenceType* createReferenceType(PrologTerm*);
  SgArrayType* createArrayType(PrologTerm*);
  PrologCompTerm* retrieveAnnotation(PrologCompTerm*);
  void abort_unless(bool,string);
  void debug(string);
  bool isValueExp(string);
  bool isUnaryOp(string);	
  bool isBinaryOp(string);
  SgInitializedName* inameFromAnnot(PrologCompTerm*);
  void testFileInfo(Sg_File_Info*);
  SgClassDeclaration* createDummyClassDeclaration(string,int);
  SgMemberFunctionDeclaration* createDummyMemberFunctionDeclaration(string s,int c_type); /* TODO */
  SgBitVector* createBitVector(PrologTerm*);
  SgLabelStatement* makeLabel(Sg_File_Info*,string);
  PrologInt* isPrologInt(PrologTerm*);
  PrologList* isPrologList(PrologTerm*);
  PrologCompTerm* isPrologCompTerm(PrologTerm*);
  PrologAtom* isPrologAtom(PrologTerm*);
  PrologString* isPrologString(PrologTerm*);
  string* toStringP(PrologTerm*);
  int toInt(PrologTerm*);
  void pciDeclarationStatement(SgDeclarationStatement*,PrologTerm*);
  void fakeParentScope(SgDeclarationStatement*);
  void addSymbol(SgGlobal*, SgDeclarationStatement*);
  void fakeClassScope(string,int,SgDeclarationStatement*);
  SgAccessModifier* createAccessModifier(PrologTerm*);
  SgBaseClassModifier* createBaseClassModifier(PrologTerm*);
  SgFunctionModifier* createFunctionModifier(PrologTerm*);
  SgSpecialFunctionModifier* createSpecialFunctionModifier(PrologTerm*);
  SgStorageModifier* createStorageModifier(PrologTerm*);
  SgLinkageModifier* createLinkageModifier(PrologTerm*);
  SgElaboratedTypeModifier* createElaboratedTypeModifier(PrologTerm*);
  SgConstVolatileModifier* createConstVolatileModifier(PrologTerm*);
  SgUPC_AccessModifier* createUPC_AccessModifier(PrologTerm*);
  SgTypeModifier* createTypeModifier(PrologTerm*);
  void setTypeModifier(PrologTerm*,SgTypeModifier*);
  SgDeclarationModifier* createDeclarationModifier(PrologTerm*);
  void setDeclarationModifier(PrologTerm*,SgDeclarationModifier*);
  SgModifierType* createModifierType(PrologTerm*);
  SgFunctionDeclaration* createDummyFunctionDeclaration(string*,PrologTerm*);
  SgFunctionSymbol* createDummyFunctionSymbol(string*,PrologTerm*);
  SgMemberFunctionSymbol* createDummyMemberFunctionSymbol(PrologTerm*);
  void fakeNamespaceScope(string,int,SgDeclarationStatement*);
  SgTypedefType* createTypedefType(PrologTerm*);
  /* type specific node generation */
  /*unary nodes*/
  SgExpression* createValueExp(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgUnaryOp* createUnaryOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgFile* createFile(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgReturnStmt* createReturnStmt(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgFunctionDefinition* createFunctionDefinition(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgInitializedName* createInitializedName(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgAssignInitializer* createAssignInitializer(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgExprStatement* createExprStatement(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgDefaultOptionStmt* createDefaultOptionStmt(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgClassDeclaration* createClassDeclaration(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgDeleteExp* createDeleteExp(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgVarArgOp* createVarArgOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgVarArgStartOneOperandOp* createVarArgStartOneOperandOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgVarArgEndOp* createVarArgEndOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgAggregateInitializer* createAggregateInitializer(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgNamespaceDeclarationStatement* createNamespaceDeclarationStatement(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgSizeOfOp* createSizeOfOp(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgConstructorInitializer* createConstructorInitializer(Sg_File_Info*,SgNode*,PrologCompTerm*);
  SgPragmaDeclaration* createPragmaDeclaration(Sg_File_Info*,SgNode*,PrologCompTerm*);
	
  /*binary nodes*/
  SgFunctionDeclaration* createFunctionDeclaration(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgBinaryOp* createBinaryOp(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgSwitchStatement* createSwitchStatement(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgDoWhileStmt* createDoWhileStmt(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgWhileStmt* createWhileStmt(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgVarArgCopyOp* createVarArgCopyOp(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgVarArgStartOp* createVarArgStartOp(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgFunctionCallExp* createFunctionCallExp(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgTryStmt* createTryStmt(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  SgCatchOptionStmt* createCatchOptionStmt(Sg_File_Info*,SgNode*,SgNode*,PrologCompTerm*);
  /*ternary nodes*/
  SgIfStmt* createIfStmt(Sg_File_Info*, SgNode*, SgNode*, SgNode*, PrologCompTerm*);
  SgCaseOptionStmt* createCaseOptionStmt(Sg_File_Info*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
  SgMemberFunctionDeclaration* createMemberFunctionDeclaration(Sg_File_Info*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
  SgNewExp* createNewExp(Sg_File_Info*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
  SgConditionalExp* createConditionalExp(Sg_File_Info*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
  /*quaternary nodes*/
  SgForStatement* createForStatement(Sg_File_Info*,SgNode*,SgNode*,SgNode*,SgNode*,PrologCompTerm*);
  /*list nodes*/
  SgFunctionParameterList* createFunctionParameterList(Sg_File_Info*, vector<SgNode*>*);
  SgBasicBlock* createBasicBlock(Sg_File_Info*,vector<SgNode*>*);
  SgGlobal* createGlobal(Sg_File_Info*,vector<SgNode*>*);
  SgProject* createProject(Sg_File_Info*,vector<SgNode*>*);
  SgVariableDeclaration* createVariableDeclaration(Sg_File_Info*,vector<SgNode*>*,PrologCompTerm*);
  SgForInitStatement* createForInitStatement(Sg_File_Info*,vector<SgNode*>*);
  SgClassDefinition* createClassDefinition(Sg_File_Info*,vector<SgNode*>*,PrologCompTerm* t);
  SgCtorInitializerList* createCtorInitializerList(Sg_File_Info*,vector<SgNode*>*);
  SgEnumDeclaration* createEnumDeclaration(Sg_File_Info*,vector<SgNode*>*,PrologCompTerm*);
  SgExprListExp* createExprListExp(Sg_File_Info*,vector<SgNode*>*);
  SgNamespaceDefinitionStatement* createNamespaceDefinitionStatement(Sg_File_Info*,vector<SgNode*>*);
  SgCatchStatementSeq* createCatchStatementSeq(Sg_File_Info*,vector<SgNode*>*);
  /*leaf nodes*/
  SgVarRefExp* createVarRefExp(Sg_File_Info*,PrologCompTerm*);
  SgBreakStmt* createBreakStmt(Sg_File_Info*,PrologCompTerm*);
  SgContinueStmt* createContinueStmt(Sg_File_Info*,PrologCompTerm*);
  SgLabelStatement* createLabelStatement(Sg_File_Info*,PrologCompTerm*);
  SgGotoStatement* createGotoStatement(Sg_File_Info*,PrologCompTerm*);
  SgRefExp* createRefExp(Sg_File_Info*,PrologCompTerm*);
  SgFunctionRefExp* createFunctionRefExp(Sg_File_Info*,PrologCompTerm*);
  SgMemberFunctionRefExp* createMemberFunctionRefExp(Sg_File_Info*,PrologCompTerm*);
  SgThisExp* createThisExp(Sg_File_Info*,PrologCompTerm*);
  SgTypedefDeclaration* createTypedefDeclaration(Sg_File_Info*,PrologCompTerm*);
  SgPragma* createPragma(Sg_File_Info*,PrologCompTerm*);
  char unescape_char(std::string s);
};
#endif
