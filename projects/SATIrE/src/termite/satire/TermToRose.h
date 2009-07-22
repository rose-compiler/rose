/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#ifndef PROLOGTOROSE_H_
#define PROLOGTOROSE_H_
#include <satire_rose.h>
#include "termite.h"
#include "RoseEnums.h"
#include "RoseToTerm.h"
#include <iostream>
#include <deque>
#include <string>
#include <map>
#include <vector>

/**
 * Class for creating a ROSE-IR (made for unparsing)
 * from its PROLOG term-representation
 */
class TermToRose {
public:
  TermToRose() {};
  ~TermToRose() {};
  void unparse(std::string, std::string, std::string, SgNode*);
  SgNode* toRose(PrologTerm*);
  SgNode* toRose(const char* filename);

  static void addSymbol(SgScopeStatement*, SgDeclarationStatement*);
private:
  /* enum <-> atom conversion */
  RoseEnums re;
  
  /* fixups */
  std::vector<SgDeclarationStatement*> declarationStatementsWithoutScope;
  std::vector<SgLabelStatement*> labelStatementsWithoutScope;
  std::multimap<std::string,SgGotoStatement*> gotoStatementsWithoutLabel;
  std::vector<SgClassDefinition*> classDefinitions;
  /* our own little symbol tables */
  std::deque<PrologTerm*>* globalDecls;
  std::map<std::string,SgType*> typeMap;
  std::map<std::string,SgDeclarationStatement*> declarationMap;
  std::map<std::string,SgInitializedName*> initializedNameMap;

  /* arity specific node generation*/
  SgNode* leafToRose(PrologCompTerm*, std::string);
  SgNode* unaryToRose(PrologCompTerm*, std::string);
  SgNode* binaryToRose(PrologCompTerm*, std::string);
  SgNode* ternaryToRose(PrologCompTerm*, std::string);
  SgNode* quaternaryToRose(PrologCompTerm*, std::string);
  SgNode* listToRose(PrologCompTerm*, std::string);
  /*helpers*/
  void unparseFile(SgSourceFile&, std::string, std::string, SgUnparse_Info*);
  void warn_msg(std::string);
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
  void abort_unless(bool, std::string);
  void debug(std::string);
  bool isValueExp(std::string);
  bool isUnaryOp(std::string);
  bool isBinaryOp(std::string);
  SgInitializedName* inameFromAnnot(PrologCompTerm*);
  void testFileInfo(Sg_File_Info*);
  SgClassDeclaration* createDummyClassDeclaration(std::string, int);
  SgMemberFunctionDeclaration* createDummyMemberFunctionDeclaration(std::string s,int c_type); /* TODO */
  SgBitVector* createBitVector(PrologTerm*, std::map<std::string, int>);
  int createEnum(PrologTerm*, std::map<std::string, int>);
  SgFunctionDeclaration* setFunctionDeclarationBody(SgFunctionDeclaration*, SgNode*);
  SgClassDeclaration* setClassDeclarationBody(SgClassDeclaration*, SgNode*);

  SgLabelStatement* makeLabel(Sg_File_Info*, std::string);
  PrologInt* isPrologInt(PrologTerm*);
  PrologList* isPrologList(PrologTerm*);
  PrologCompTerm* isPrologCompTerm(PrologTerm*);
  PrologAtom* isPrologAtom(PrologTerm*);
  //PrologString* isPrologString(PrologTerm*); /* DEPRECATED */
  std::string* toStringP(PrologTerm*);
  int toInt(PrologTerm*);
  void pciDeclarationStatement(SgDeclarationStatement*,PrologTerm*);
  void fakeParentScope(SgDeclarationStatement*);
  void fakeClassScope(std::string, int, SgDeclarationStatement*);
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
  void setTypeModifier(PrologTerm*, SgTypeModifier*);
  SgDeclarationModifier* createDeclarationModifier(PrologTerm*);
  void setDeclarationModifier(PrologTerm*, SgDeclarationModifier*);
  SgModifierType* createModifierType(PrologTerm*);
  SgFunctionDeclaration* createDummyFunctionDeclaration(std::string*, PrologTerm*);
  SgFunctionSymbol* createDummyFunctionSymbol(std::string*, PrologTerm*);
  SgMemberFunctionSymbol* createDummyMemberFunctionSymbol(PrologTerm*);
  void fakeNamespaceScope(std::string, int, SgDeclarationStatement*);
  SgTypedefType* createTypedefType(PrologTerm*);
  /* type specific node generation */
  /*unary nodes*/
  SgExpression* createValueExp(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgUnaryOp* createUnaryOp(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgSourceFile* createFile(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgReturnStmt* createReturnStmt(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgFunctionDefinition* createFunctionDefinition(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgInitializedName* createInitializedName(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgAssignInitializer* createAssignInitializer(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgExprStatement* createExprStatement(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgDefaultOptionStmt* createDefaultOptionStmt(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgClassDeclaration* createClassDeclaration(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgDeleteExp* createDeleteExp(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgVarArgOp* createVarArgOp(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgVarArgStartOneOperandOp* createVarArgStartOneOperandOp(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgVarArgEndOp* createVarArgEndOp(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgAggregateInitializer* createAggregateInitializer(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgNamespaceDeclarationStatement* createNamespaceDeclarationStatement(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgSizeOfOp* createSizeOfOp(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgConstructorInitializer* createConstructorInitializer(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgPragmaDeclaration* createPragmaDeclaration(Sg_File_Info*, SgNode*, PrologCompTerm*);

  /*binary nodes*/
  SgFunctionDeclaration* createFunctionDeclaration(Sg_File_Info*, SgNode*, PrologCompTerm*);
  SgBinaryOp* createBinaryOp(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  SgSwitchStatement* createSwitchStatement(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  SgDoWhileStmt* createDoWhileStmt(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  SgWhileStmt* createWhileStmt(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  SgVarArgCopyOp* createVarArgCopyOp(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  SgVarArgStartOp* createVarArgStartOp(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  SgFunctionCallExp* createFunctionCallExp(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  SgTryStmt* createTryStmt(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  SgCatchOptionStmt* createCatchOptionStmt(Sg_File_Info*, SgNode*, SgNode*, PrologCompTerm*);
  /*ternary nodes*/
  SgIfStmt* createIfStmt(Sg_File_Info*,  SgNode*,  SgNode*,  SgNode*,  PrologCompTerm*);
  SgCaseOptionStmt* createCaseOptionStmt(Sg_File_Info*, SgNode*, SgNode*, SgNode*, PrologCompTerm*);
  SgMemberFunctionDeclaration* createMemberFunctionDeclaration(Sg_File_Info*, SgNode*, SgNode*, SgNode*, PrologCompTerm*);
  SgNewExp* createNewExp(Sg_File_Info*, SgNode*, SgNode*, SgNode*, PrologCompTerm*);
  SgConditionalExp* createConditionalExp(Sg_File_Info*, SgNode*, SgNode*, SgNode*, PrologCompTerm*);
  /*quaternary nodes*/
  SgForStatement* createForStatement(Sg_File_Info*, SgNode*, SgNode*, SgNode*, SgNode*, PrologCompTerm*);
  /*list nodes*/
  SgFunctionParameterList* createFunctionParameterList(Sg_File_Info*,  std::deque<SgNode*>*);
  SgBasicBlock* createBasicBlock(Sg_File_Info*, std::deque<SgNode*>*);
  SgGlobal* createGlobal(Sg_File_Info*, std::deque<SgNode*>*);
  SgProject* createProject(Sg_File_Info*, std::deque<SgNode*>*);
  SgVariableDeclaration* createVariableDeclaration(Sg_File_Info*, std::deque<SgNode*>*, PrologCompTerm*, SgDeclarationStatement*);
  SgForInitStatement* createForInitStatement(Sg_File_Info*, std::deque<SgNode*>*);
  SgClassDefinition* createClassDefinition(Sg_File_Info*, std::deque<SgNode*>*, PrologCompTerm* t);
  SgCtorInitializerList* createCtorInitializerList(Sg_File_Info*, std::deque<SgNode*>*);
  SgEnumDeclaration* createEnumDeclaration(Sg_File_Info*, std::deque<SgNode*>*, PrologCompTerm*);
  SgExprListExp* createExprListExp(Sg_File_Info*, std::deque<SgNode*>*);
  SgNamespaceDefinitionStatement* createNamespaceDefinitionStatement(Sg_File_Info*, std::deque<SgNode*>*);
  SgCatchStatementSeq* createCatchStatementSeq(Sg_File_Info*, std::deque<SgNode*>*);
  /*leaf nodes*/
  SgVarRefExp* createVarRefExp(Sg_File_Info*, PrologCompTerm*);
  SgBreakStmt* createBreakStmt(Sg_File_Info*, PrologCompTerm*);
  SgContinueStmt* createContinueStmt(Sg_File_Info*, PrologCompTerm*);
  SgLabelStatement* createLabelStatement(Sg_File_Info*, PrologCompTerm*);
  SgGotoStatement* createGotoStatement(Sg_File_Info*, PrologCompTerm*);
  SgRefExp* createRefExp(Sg_File_Info*, PrologCompTerm*);
  SgFunctionRefExp* createFunctionRefExp(Sg_File_Info*, PrologCompTerm*);
  SgMemberFunctionRefExp* createMemberFunctionRefExp(Sg_File_Info*, PrologCompTerm*);
  SgThisExp* createThisExp(Sg_File_Info*, PrologCompTerm*);
  SgTypedefDeclaration* createTypedefDeclaration(Sg_File_Info*, PrologCompTerm*);
  SgPragma* createPragma(Sg_File_Info*, PrologCompTerm*);
  char unescape_char(std::string s);

public:
  // This is a helper function to create the necessary unique first
  // nondefining definition
  template< class DeclType, typename A, typename B, typename C >
  DeclType* createDummyNondefDecl(DeclType* decl, Sg_File_Info* fi, 
			     A a, B b, C c) 
  {
    decl->set_forward(0);
    decl->set_definingDeclaration(decl);
    
    DeclType* ndd = new DeclType(fi,a,b,c);
    ndd->set_endOfConstruct(fi);
    ndd->set_parent(NULL);
    
    /* Set the internal reference to the non-defining declaration */  
    ndd->set_firstNondefiningDeclaration(ndd);
    ndd->set_definingDeclaration(decl);
    ndd->setForward();
    decl->set_firstNondefiningDeclaration(ndd);
    declarationStatementsWithoutScope.push_back(ndd);
    return ndd;
  }

  template< class DeclType >
  DeclType* lookupDecl(DeclType** decl, std::string id, bool fail=true) 
  {
    if (declarationMap.find(id) != declarationMap.end()) {
      *decl = dynamic_cast<DeclType*>(declarationMap[id]);
      ROSE_ASSERT(*decl != NULL);
    } else if (fail) {
      std::cerr<<"**ERROR: Symbol lookup failed: ("
	/*<<decl->class_name()<<"*)*/<<id<<std::endl;
      ROSE_ASSERT(false);
      *decl = NULL;
    }
    return *decl;
  }

  template< class TypeType >
  TypeType* lookupType(TypeType** type, std::string id, bool fail=true) 
  {   
    if (typeMap.find(id) != typeMap.end()) {
      SgType* t = typeMap[id];
      *type = dynamic_cast<TypeType*>(t);
      ROSE_ASSERT(*type != NULL || t == NULL); // allow NULL pointer
    } else if (fail) {
      std::cerr<<"**ERROR: Symbol lookup failed: ("
	/*<<type->class_name()<<"*)*/<<id<<std::endl;
      ROSE_ASSERT(false);
      *type = NULL;
    }
    return *type;
  }

  template< class DeclType >
  DeclType* lookaheadDecl(DeclType** decl, std::string pattern) 
  {
    *decl = NULL;
    if (globalDecls) {
      for (std::deque<PrologTerm*>::iterator it = globalDecls->begin();
	   it != globalDecls->end(); ++it) {
	if ((*it)->matches(pattern)) {
	  //std::cerr<<pattern<<std::endl;
	  *decl = dynamic_cast<DeclType*>(toRose(*it));
	  //std::cerr<<d->class_name()<<std::endl;
	  ROSE_ASSERT(*decl != NULL);
	}
      }
    }
    return *decl;
  }

};
#endif
