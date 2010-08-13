#ifndef _ANNOTATEAST_H
#define _ANNOTATEAST_H

#include <rose.h>
#include <string>
#include <set>
#include <rosetollvm/LLVMVisitor.h>
#include <rosetollvm/Option.h>
#include <rosetollvm/Control.h>
#include <AliasSetContainer.h>
#include <AliasSetHandler.h>
#include <rosetollvm/RootAstAttribute.h>
#include <algorithm>
#include <iterator>

class AliasSetAstAttribute : public RootAstAttribute
{
    AliasDataSet::RoseAliasSet _AliasSets;

    public:
    AliasSetAstAttribute(AliasDataSet::RoseAliasSet aliassets) { _AliasSets = aliassets; }
    AliasDataSet::RoseAliasSet getAliasSet() { return _AliasSets; }

};

class AnnotateAST: public LLVMVisitor
{
    std::string _functionname;
    AliasSetContainer *_container;

    public:
        AnnotateAST(Option &option_, Control &control_) : LLVMVisitor(option_, control_)
        {                                                     
        }

        virtual void preOrderVisit(SgNode *node);
        virtual void postOrderVisit(SgNode *node);

        static const char* ALIAS_SET;

        void setFunctionName(std::string _function);
        void findContainer(std::string _modulename);
};

#endif
