// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ProcTraversal.h,v 1.3 2008-03-28 10:36:28 gergo Exp $

#ifndef H_PROCTRAVERSAL
#define H_PROCTRAVERSAL

#include <config.h>
#include <rose.h>

#include "cfg_support.h"

// this must be used preorder
class ProcTraversal : public AstSimpleProcessing
{
public:
  ProcTraversal();
  std::deque<Procedure *> *get_procedures() const;
  unsigned long original_ast_nodes, original_ast_statements;
  void setPrintCollectedFunctionNames(bool pcf) ;
  bool getPrintCollectedFunctionNames();
  
protected:
  void visit(SgNode *);
  void atTraversalStart();
  void atTraversalEnd();

private:
  std::deque<Procedure *> *procedures;
  int node_id;
  int procnum;
  bool _printCollectedFunctionNames;
  TimingPerformance *timer;
};

/* find default constructor method in a given class declaration
 * if no default constructor exists, a "dummy"-default constructor is created
 */
SgMemberFunctionDeclaration *get_default_constructor(
        SgClassDeclaration *);

#endif
