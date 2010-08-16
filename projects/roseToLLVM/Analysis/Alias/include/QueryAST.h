#ifndef _QUERYAST_H
#define _QUERYAST_H

#include <rose.h>
#include <AnnotateAST.h>
#include <string>
#include <set>
#include <AliasSetContainer.h>
#include <iterator>


/*
 * Queries AST for Alias Analysis Information
 */

namespace QueryASTData
{
    typedef std::set<std::string> _Locations;
    typedef std::pair<AliasType, _Locations> _Set;
    typedef std::vector<_Set> _List;
}

class SkipNodeAttribute : public RootAstAttribute
{
    bool skip;

    public:
    SkipNodeAttribute(bool val) { skip = val; }
   
};

class QueryAST: public LLVMVisitor
{
    std::string _functionname;
    AliasSetContainer *_container;
    QueryASTData::_List _aliasSetList;

    public:
        QueryAST(Option &option_, Control &control_) : LLVMVisitor(option_, control_)
        {                                                     
        }

        virtual void preOrderVisit(SgNode *node);
        virtual void postOrderVisit(SgNode *node);
        virtual void atTraversalEnd();

        static const char* SKIP_NODE;

        void setFunctionName(std::string _function);
        void findContainer(std::string _modulename);
};
#endif
