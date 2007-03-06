#ifndef H_ASTNUMBEROFNODESSTATISTICS
#define H_ASTNUMBEROFNODESSTATISTICS

#include "rose.h"

class AstNumberOfNodesStatistics : public SgSimpleProcessing
{
public:
    AstNumberOfNodesStatistics();
    unsigned long get_numberofnodes() const;
    unsigned long get_numberofstatements() const;

protected:
    virtual void visit(SgNode *node);
    unsigned long numnod, numst;
};

#endif
