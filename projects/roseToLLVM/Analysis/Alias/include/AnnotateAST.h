#ifndef _ANNOTATEAST_H
#define _ANNOTATEAST_H

#include <rosetollvm/LLVMVisitor.h>
#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>

class AnnotateAST: public LLVMVisitor
{
    public:
        AnnotateAST(Option &option_, Control &control_) : LLVMVisitor(option_, control_)
        {                                                     
        }

        virtual void preOrderVisit(SgNode *node);
        virtual void postOrderVisit(SgNode *node);

};

#endif
