#ifndef _ASSOCIATEROSEAST_H
#define _ASSOCIATEROSEAST_H

#include <rose.h>
#include <string>
#include <AliasSetContainer.h>
#include <AliasSetHandler.h>
#include <rosetollvm/LLVMVisitor.h>
#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>

class AssociateRoseAST: public LLVMVisitor
{
    std::string _functionname;
    AliasSetContainer *_container;

    public:
        AssociateRoseAST(Option &option_, Control &control_) : LLVMVisitor(option_, control_)
        {                                                     
        }

        void setFunctionName(std::string _function);
        void findContainer(std::string _modulename);

        virtual void preOrderVisit(SgNode*);
        virtual void postOrderVisit(SgNode*);
};

#endif
