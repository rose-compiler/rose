#ifndef CFGVISITOR_HH
#define CFGVISITOR_HH


#include "common.hh"

class Relation;

/* A traversal to create control flow Relations
 */
class CFGVisitor : public ROSE_VisitTraversal{
public:
    CFGVisitor();
    virtual void visit( SgNode * );
    void explore(SgNode*);
private:
    Relation *cfgNext;
/*    Relation *cfgBefore;
    Relation *cfgAfter;
    */
};

#endif
