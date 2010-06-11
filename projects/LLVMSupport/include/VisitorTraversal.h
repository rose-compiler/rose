#ifndef _VISITOR_TRAVERSAL_H
#define _VISITOR_TRAVERSAL_H 

#include "rose.h"

class VisitorTraversal: public AstSimpleProcessing
{
    public:
        VisitorTraversal();
        virtual void visit(SgNode*);
        virtual void atTraversalEnd();
};

#endif
