
#ifndef _PARSER_BUILDER_HPP_
#define _PARSER_BUILDER_HPP_

#include <string>
#include <vector>
#include <set>
#include "rose.h"

namespace ParserBuilder {

class GrammarNode;

template <class Recipient>
class Parser {
    protected:
        GrammarNode * p_grammar_root;

    public:
        Parser();
        virtual ~Parser();
        
        void setRoot(GrammarNode * grammar_root);
        
        bool parse(std::string &, Recipient &);
};

class GrammarNode {
    public:
        struct LexemeInstance {
            std::string original_string;
            GrammarNode * matching_node;
            virtual void polymorphic() const = 0;
        };

    public:
        GrammarNode();
        virtual ~GrammarNode();
        
        virtual bool operator () (std::string &, std::vector<LexemeInstance *> &) = 0;
        
        virtual void collectChilds(std::set<GrammarNode *> & childs) const;

    template <class Recipient>
    friend class Grammar;
};

class NonTerminalNode : public GrammarNode {
    public:
        struct NonTerminalLexemeInstance : public GrammarNode::LexemeInstance {
            std::vector<LexemeInstance *> childs_productions;
            virtual void polymorphic() const;
        };

    protected:
        std::vector<std::vector<GrammarNode *> > p_composed_of;
        
    public:
        NonTerminalNode();
        virtual ~NonTerminalNode();
        
        virtual bool operator () (std::string &, std::vector<LexemeInstance *> &);
        
        void addProduction();
        void addToLastProduction(GrammarNode *);
        
        virtual void collectChilds(std::set<GrammarNode *> & childs) const;

    template <class Recipient>
    friend class Grammar;
};

class ZeroOrOne : public NonTerminalNode {
    protected:
        GrammarNode * p_node;
        
    public:
        ZeroOrOne(GrammarNode * node);
        virtual ~ZeroOrOne();
        
        virtual bool operator () (std::string &, std::vector<LexemeInstance *> &);
        
        virtual void collectChilds(std::set<GrammarNode *> & childs) const;

    template <class Recipient>
    friend class Grammar;
};

class Any : public NonTerminalNode {
    protected:
        GrammarNode * p_node;
        
    public:
        Any(GrammarNode * node);
        virtual ~Any();
        
        virtual bool operator () (std::string &, std::vector<LexemeInstance *> &);
        
        virtual void collectChilds(std::set<GrammarNode *> & childs) const;

    template <class Recipient>
    friend class Grammar;
};

class OneOrMore : public NonTerminalNode {
    protected:
        GrammarNode * p_node;
        
    public:
        OneOrMore(GrammarNode * node);
        virtual ~OneOrMore();
        
        virtual bool operator () (std::string &, std::vector<LexemeInstance *> &);
        
        virtual void collectChilds(std::set<GrammarNode *> & childs) const;

    template <class Recipient>
    friend class Grammar;
};

class TerminalNode : public GrammarNode {
    public:
        struct TerminalNodeLexemeInstance : public GrammarNode::LexemeInstance {
            virtual void polymorphic() const = 0;
        };
        
    public:
        TerminalNode();
        virtual ~TerminalNode();
        
        virtual bool operator () (std::string &, std::vector<LexemeInstance *> &) = 0;

    template <class Recipient>
    friend class Grammar;
};

class SpaceNode : public TerminalNode {
    public:
        struct SpaceNodeLexemeInstance : public TerminalNode::TerminalNodeLexemeInstance {
            virtual void polymorphic() const;
        };
        
    public:
        SpaceNode();
        virtual ~SpaceNode();
        
        bool operator () (std::string &, std::vector<LexemeInstance *> &);

    template <class Recipient>
    friend class Grammar;
};

class LabelNode : public TerminalNode {
    public:
        struct LabelNodeLexemeInstance : public TerminalNode::TerminalNodeLexemeInstance {
            virtual void polymorphic() const;
        };
        
    protected:
        std::string p_string_to_match;
        
    public:
        LabelNode(std::string string_to_match);
        virtual ~LabelNode();
        
        bool operator () (std::string &, std::vector<LexemeInstance *> &);

    template <class Recipient>
    friend class Grammar;
};

class SageNode : public TerminalNode {
    public:
        struct SageNodeLexemeInstance : public TerminalNode::TerminalNodeLexemeInstance {
            SgNode * sage_node;
            virtual void polymorphic() const;
        };
        
    protected:
        VariantT p_sage_variant_to_match;
        SgScopeStatement * p_scope;
        
    public:
        SageNode(VariantT, SgScopeStatement *);
        virtual ~SageNode();
        
        virtual bool operator () (std::string &, std::vector<LexemeInstance *> &);
        
        void setScope(SgScopeStatement * scope);

    template <class Recipient>
    friend class Grammar;
};

class EqualityExpressionSageNode : public SageNode {
    public:
        EqualityExpressionSageNode(SgScopeStatement *);
        virtual ~EqualityExpressionSageNode();
        
        bool operator () (std::string &, std::vector<LexemeInstance *> &);

    template <class Recipient>
    friend class Grammar;
};

class RelationalExpressionSageNode : public SageNode {
    public:
        RelationalExpressionSageNode(SgScopeStatement *);
        virtual ~RelationalExpressionSageNode();
        
        bool operator () (std::string &, std::vector<LexemeInstance *> &);

    template <class Recipient>
    friend class Grammar;
};

}

#include "ParserBuilder.tpp"

#endif /* _PARSER_BUILDER_HPP_ */
