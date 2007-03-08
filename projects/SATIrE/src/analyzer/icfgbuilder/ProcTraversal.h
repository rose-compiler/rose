// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ProcTraversal.h,v 1.3 2007-03-08 15:36:48 markus Exp $

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

protected:
    void visit(SgNode *);

private:
    std::deque<Procedure *> *procedures;
    int node_id;
    int procnum;
};

SgMemberFunctionDeclaration *get_default_constructor(
        SgClassDeclaration *);

#endif
