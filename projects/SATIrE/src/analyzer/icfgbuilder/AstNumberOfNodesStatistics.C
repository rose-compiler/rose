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
