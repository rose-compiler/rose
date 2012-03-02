// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: AstNumberOfNodesStatistics.C,v 1.2 2007-03-08 15:36:48 markus Exp $

#include "AstNumberOfNodesStatistics.h"

AstNumberOfNodesStatistics::AstNumberOfNodesStatistics()
    : numnod(0), numst(0)
{
}

unsigned long AstNumberOfNodesStatistics::get_numberofnodes() const
{
    return numnod;
}

unsigned long AstNumberOfNodesStatistics::get_numberofstatements() const
{
    return numst;
}

void AstNumberOfNodesStatistics::visit(SgNode *node)
{
    numnod++;
    if (isSgStatement(node))
        numst++;
}
