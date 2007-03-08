// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: StatementAttributeTraversal.h,v 1.2 2007-03-08 15:36:48 markus Exp $

#ifndef STATEMENTATTRIBUTETRAVERSAL_H
#define STATEMENTATTRIBUTETRAVERSAL_H

#include <config.h>
#include "CFGTraversal.h"
#include "ProcTraversal.h"

//#include "iterate.h"

template<typename DFI_STORE_TYPE>
class StatementAttributeTraversal : public AstSimpleProcessing
{
public:
  StatementAttributeTraversal(DFI_STORE store_) : store(store_) {}
  virtual ~StatementAttributeTraversal();

protected:
  void visit(SgNode *);
  std::string currentFunction();

  virtual void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo) {}
  virtual void visitBasicBlock(SgBasicBlock* stmt) {}

  std::string getPreInfo(SgStatement* stmt);
  std::string getPostInfo(SgStatement* stmt);

  DFI_STORE_TYPE store;
  std::string _currentFunction;

  virtual std::string statementPreInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { return ""; }
  virtual std::string statementPostInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { return ""; }
private:
  SgFunctionDeclaration* enclosing_function(SgNode *node);
};

template<typename DFI_STORE_TYPE>
class DfiCommentAnnotator : public StatementAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiCommentAnnotator(DFI_STORE_TYPE store_):StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {}
  virtual ~DfiCommentAnnotator();

protected:
  virtual void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo);

  void addComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node);
  void addCommentBeforeNode(std::string comment, SgStatement* node);
  void addCommentAfterNode(std::string comment, SgStatement* node);

private:
};

template<typename DFI_STORE_TYPE>
class DfiTextPrinter : public StatementAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiTextPrinter(DFI_STORE_TYPE store_) : StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {}
  virtual ~DfiTextPrinter();

protected:
  void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo);

private:
};

template<typename DFI_STORE_TYPE>
class DfiDotGenerator : public StatementAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiDotGenerator(DFI_STORE_TYPE store_);
  virtual ~DfiDotGenerator();
  void edgeInfoOn();
  void edgeInfoOff();
  bool edgeInfo();

protected:
  virtual void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo);
  virtual void visitBasicBlock(SgBasicBlock* stmt);
  std::string addEdgeLabel(std::string s);
  SgStatement* lastStmtofStmtList(SgStatementPtrList& l);
private:
  bool _edgeInfo;
};

#include "StatementAttributeTraversal.C"

#endif
