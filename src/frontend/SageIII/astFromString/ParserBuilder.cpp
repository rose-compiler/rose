
#include "ParserBuilder.hpp"
#include "AstFromString.h"

namespace ParserBuilder {

/*  */

GrammarNode::GrammarNode() {}

GrammarNode::~GrammarNode() {}

void GrammarNode::collectChilds(std::set<GrammarNode *> & childs) const {}

/*  */

NonTerminalNode::NonTerminalNode() :
    GrammarNode(),
    p_composed_of()
{}

NonTerminalNode::~NonTerminalNode() {}

NonTerminalNode::LexemeInstance::~LexemeInstance() {}
       
bool NonTerminalNode::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
    
    NonTerminalLexemeInstance * lexeme = new NonTerminalLexemeInstance();
        lexeme->original_string = std::string();
        lexeme->matching_node = this;
        lexeme->childs_productions = std::vector<LexemeInstance *>();
        
    std::vector<LexemeInstance *> tmp_lexeme;

    std::vector<std::vector<GrammarNode *> >::iterator it_vect;
    std::vector<GrammarNode *>::iterator it;
    
    for (it_vect = p_composed_of.begin(); it_vect != p_composed_of.end(); it_vect++) {
        bool match = true;
        for (it = it_vect->begin(); it != it_vect->end(); it++) {
        
            match &= (**it)(str, tmp_lexeme);
            
            if (match) {
                std::vector<LexemeInstance *>::iterator it;
                for (it = tmp_lexeme.begin(); it != tmp_lexeme.end(); it++) {
                    lexeme->original_string += (*it)->original_string;
                    lexeme->childs_productions.push_back(*it);
                }
                tmp_lexeme.clear();
            }
            else break;
        }
        if (match) {
            if (it_vect->back() == this) {
                lexemes.insert(lexemes.end(), lexeme->childs_productions.begin(), lexeme->childs_productions.end());
                delete lexeme;
            }
            else {
                lexemes.push_back(lexeme);
            }
            return true;
        }
        else {
            std::vector<LexemeInstance *>::iterator it;
            for (it = lexeme->childs_productions.begin(); it != lexeme->childs_productions.end(); it++)
                delete *it;
            str = lexeme->original_string + str;
        }
        lexeme->childs_productions.clear();
    }
    delete lexeme;
    return false;
}
        
void NonTerminalNode::addProduction() {
    p_composed_of.push_back(std::vector<GrammarNode *>());
}

void NonTerminalNode::addToLastProduction(GrammarNode * n) {
    p_composed_of.back().push_back(n);
}

void NonTerminalNode::collectChilds(std::set<GrammarNode *> & childs) const {
    std::vector<std::vector<GrammarNode *> >::const_iterator it_vect;
    std::vector<GrammarNode *>::const_iterator it;
    for (it_vect = p_composed_of.begin(); it_vect != p_composed_of.end(); it_vect++) {
        for (it = it_vect->begin(); it != it_vect->end(); it++)
            if (*it != this && childs.find(*it) != childs.end())
                (*it)->collectChilds(childs);
        childs.insert(it_vect->begin(), it_vect->end());
    }
}

NonTerminalNode::NonTerminalLexemeInstance::~NonTerminalLexemeInstance() {}

/*  */

ZeroOrOne::ZeroOrOne(GrammarNode * node) :
    NonTerminalNode(),
    p_node(node)
{}

ZeroOrOne::~ZeroOrOne() {}
        
bool ZeroOrOne::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
    (*p_node)(str, lexemes);
    return true;
}

void ZeroOrOne::collectChilds(std::set<GrammarNode *> & childs) const {
    p_node->collectChilds(childs);
    childs.insert(p_node);
}

/*  */

Any::Any(GrammarNode * node) :
    NonTerminalNode(),
    p_node(node)
{}

Any::~Any() {}
        
bool Any::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
    while ((*p_node)(str, lexemes));
    return true;
}

void Any::collectChilds(std::set<GrammarNode *> & childs) const {
    p_node->collectChilds(childs);
    childs.insert(p_node);
}

/*  */

OneOrMore::OneOrMore(GrammarNode * node) :
    NonTerminalNode(),
    p_node(node)
{}

OneOrMore::~OneOrMore() {}
        
bool OneOrMore::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
    unsigned int beginning_size = lexemes.size();
    while ((*p_node)(str, lexemes));
    return beginning_size < lexemes.size();
}

void OneOrMore::collectChilds(std::set<GrammarNode *> & childs) const {
    p_node->collectChilds(childs);
    childs.insert(p_node);
}

/*  */

TerminalNode::TerminalNode() :
    GrammarNode()
{}

TerminalNode::~TerminalNode() {}

TerminalNode::TerminalNodeLexemeInstance::~TerminalNodeLexemeInstance() {}

/*  */

SpaceNode::SpaceNode() :
    TerminalNode()
{}

SpaceNode::~SpaceNode() {}

bool SpaceNode::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
    SpaceNodeLexemeInstance * lexeme = new SpaceNodeLexemeInstance();
    lexeme->original_string = std::string();
    lexeme->matching_node = this;
    
    while (str.size() > 0 && (str[0] == ' ' || str[0] == '\t' || str[0] == '\n')) {
        lexeme->original_string += str[0];
        str = str.substr(1);
    }
    
    if (lexeme->original_string.size() > 0) {
        lexemes.push_back(lexeme);
        return true;
    }
    else {
        delete lexeme;
        return false;
    }
}

SpaceNode::SpaceNodeLexemeInstance::~SpaceNodeLexemeInstance() {}

/*  */
     
LabelNode::LabelNode(std::string string_to_match) :
    TerminalNode(),
    p_string_to_match(string_to_match)
{}
     
LabelNode::~LabelNode() {}

bool LabelNode::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
//    std::cerr << "Try to parse: << " << p_string_to_match << " >> from [" << str << "]" << std::endl;

    if (str.find(p_string_to_match) == 0) {
//        std::cerr << "\tOK" << std::endl;
        LabelNodeLexemeInstance * lexeme = new LabelNodeLexemeInstance();
        lexeme->original_string = p_string_to_match;
        lexeme->matching_node = this;
        lexemes.push_back(lexeme);
        
        str = str.substr(p_string_to_match.size());
        return true;
    }
    else
        return false;
}

LabelNode::LabelNodeLexemeInstance::~LabelNodeLexemeInstance() {}

/*  */

SageNode::SageNodeLexemeInstance::~SageNodeLexemeInstance() {}

SageNode::SageNode(VariantT v, SgScopeStatement * scope) :
    TerminalNode(),
    p_sage_variant_to_match(v),
    p_scope(scope)
{}

SageNode::~SageNode() {}

void SageNode::setScope(SgScopeStatement * scope) {
    p_scope = scope;
}

bool SageNode::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
    AstFromString::c_char = str.c_str();
    AstFromString::c_sgnode = p_scope;
    
    bool match;
    
    switch (p_sage_variant_to_match) {
        case V_SgExpression:
            match = AstFromString::afs_match_expression();
            break;
        default:
            std::cerr << "Unsurported Sage node variant: " << p_sage_variant_to_match << std::endl;
            match = false;
    }
    
    if (match) {
        SageNodeLexemeInstance * lexeme = new SageNodeLexemeInstance();
        lexeme->original_string = str.substr(str.size() - strlen(AstFromString::c_char));
        lexeme->matching_node = this;
        
        lexeme->sage_node = AstFromString::c_parsed_node;
        
        lexemes.push_back(lexeme);
        return true;
    }
    return false;
}

/*  */

EqualityExpressionSageNode::EqualityExpressionSageNode(SgScopeStatement * scope) :
    SageNode(V_SgBinaryOp, scope)
{}

EqualityExpressionSageNode::~EqualityExpressionSageNode() {}
        
bool EqualityExpressionSageNode::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
    const char * c_string = str.c_str();
    AstFromString::c_char = c_string;
    AstFromString::c_sgnode = p_scope;
    
    if (AstFromString::afs_match_equality_expression()) {
        SageNodeLexemeInstance * lexeme = new SageNodeLexemeInstance();
        lexeme->original_string = str.substr(0, AstFromString::c_char - c_string);
        lexeme->matching_node = this;
        
        lexeme->sage_node = AstFromString::c_parsed_node;
        
        lexemes.push_back(lexeme);
        
        str = str.substr(AstFromString::c_char - c_string);
        
        return true;
    }
    return false;
}

/*  */

RelationalExpressionSageNode::RelationalExpressionSageNode(SgScopeStatement * scope) :
    SageNode(V_SgBinaryOp, scope)
{}

RelationalExpressionSageNode::~RelationalExpressionSageNode() {}
        
bool RelationalExpressionSageNode::operator () (std::string & str, std::vector<LexemeInstance *> & lexemes) {
    const char * c_string = str.c_str();
    AstFromString::c_char = c_string;
    AstFromString::c_sgnode = p_scope;
    
    if (AstFromString::afs_match_relational_expression()) {
        SageNodeLexemeInstance * lexeme = new SageNodeLexemeInstance();
        lexeme->original_string = str.substr(0, AstFromString::c_char - c_string);
        lexeme->matching_node = this;
        
        lexeme->sage_node = AstFromString::c_parsed_node;
        
        lexemes.push_back(lexeme);
        
        str = str.substr(AstFromString::c_char - c_string);
        
        return true;
    }
    return false;
}



}
