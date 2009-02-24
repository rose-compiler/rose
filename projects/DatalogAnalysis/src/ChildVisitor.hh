#ifndef CHILDVISITOR_HH
#define CHILDVISITOR_HH


#include "common.hh"

class Relation;

/* A traversal to create control flow Relations
 */
class ChildVisitor : public ROSE_VisitTraversal{
public:
    ChildVisitor();
    virtual void visit( SgNode * );
private:
    Relation *childRelation;
};

#endif
