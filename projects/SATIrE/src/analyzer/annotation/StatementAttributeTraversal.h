// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: StatementAttributeTraversal.h,v 1.3 2007-06-25 10:39:28 pr012 Exp $

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
  virtual ~StatementAttributeTraversal() {}

protected:
  void visit(SgNode *);
  std::string currentFunction();

  virtual void visitStatement(SgStatement* stmt) {}
  virtual void visitBasicBlock(SgBasicBlock* stmt) {}

  DFI_STORE_TYPE store;
  std::string _currentFunction;
private:
  SgFunctionDeclaration* enclosing_function(SgNode *node);
};

template<typename DFI_STORE_TYPE>
class StatementStringAttributeTraversal : public StatementAttributeTraversal<DFI_STORE_TYPE>
{
public:
  StatementStringAttributeTraversal(DFI_STORE store_) : StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {}
  virtual ~StatementStringAttributeTraversal() {}

protected:
  std::string getPreInfo(SgStatement* stmt);
  std::string getPostInfo(SgStatement* stmt);

  virtual std::string statementPreInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { return ""; }
  virtual std::string statementPostInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { return ""; }
};

template<typename DFI_STORE_TYPE>
class DfiCommentAnnotator : public StatementStringAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiCommentAnnotator(DFI_STORE_TYPE store_):StatementStringAttributeTraversal<DFI_STORE_TYPE>(store_) {}
  virtual ~DfiCommentAnnotator();

protected:
  virtual void visitStatement(SgStatement* stmt);

  void addComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node);
  void addCommentBeforeNode(std::string comment, SgStatement* node);
  void addCommentAfterNode(std::string comment, SgStatement* node);

private:
};

template<typename DFI_STORE_TYPE>
class DfiTextPrinter : public StatementStringAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiTextPrinter(DFI_STORE_TYPE store_) : StatementStringAttributeTraversal<DFI_STORE_TYPE>(store_) {}
  virtual ~DfiTextPrinter();

protected:
  virtual void visitStatement(SgStatement* stmt);

private:
};

template<typename DFI_STORE_TYPE>
class DfiDotGenerator : public StatementStringAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiDotGenerator(DFI_STORE_TYPE store_);
  virtual ~DfiDotGenerator();
  void edgeInfoOn();
  void edgeInfoOff();
  bool edgeInfo();

protected:
  virtual void visitStatement(SgStatement* stmt);
  virtual void visitBasicBlock(SgBasicBlock* stmt);
  std::string addEdgeLabel(std::string s);
  SgStatement* lastStmtofStmtList(SgStatementPtrList& l);
private:
  bool _edgeInfo;
};

#include "StatementAttributeTraversal.C"

#endif
