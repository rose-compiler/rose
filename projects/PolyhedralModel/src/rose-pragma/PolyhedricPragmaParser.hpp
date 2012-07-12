
#ifndef _POLYHEDRIC_PRAGMA_PARSER_HPP_
#define _POLYHEDRIC_PRAGMA_PARSER_HPP_

#include "ParserBuilder.hpp"

#include <iostream>

using namespace ParserBuilder;

class PolyhedricPragmaParser : public Parser<PolyhedricPragmaParser> {
    protected:
        std::vector<SgExpression *> p_obtained_constraint;
        LabelNode * p_pragma_cuda_opt;
        EqualityExpressionSageNode * p_constraint;

    public:
        PolyhedricPragmaParser(SgScopeStatement * = NULL);
        
        void operator () (GrammarNode::LexemeInstance *, std::string indent = std::string());
        
        void changeScope(SgScopeStatement * scope);
};

#endif /* _POLYHEDRIC_PRAGMA_PARSER_HPP_ */
