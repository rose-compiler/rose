
namespace ParserBuilder {

template <class Recipient>
Parser<Recipient>::Parser() :
    p_grammar_root(NULL)
{}

template <class Recipient>
Parser<Recipient>::~Parser() {
    std::set<GrammarNode *> childs;
    p_grammar_root->collectChilds(childs);
    
    std::set<GrammarNode *>::iterator it;
    for (it = childs.begin(); it != childs.end(); it++)
        delete *it;
    delete p_grammar_root;
}

template <class Recipient>
void Parser<Recipient>::setRoot(GrammarNode * grammar_root) { p_grammar_root = grammar_root; }

template <class Recipient>
bool Parser<Recipient>::parse(std::string & str, Recipient & rec) {
    if (p_grammar_root == NULL)
        return true;

    std::vector<GrammarNode::LexemeInstance *> lexemes;

    if ((*p_grammar_root)(str, lexemes) && str.size() == 0) {
        std::vector<GrammarNode::LexemeInstance *>::iterator it;
        for (it = lexemes.begin(); it != lexemes.end(); it++) {
            rec(*it);
            delete *it;
        }
        return true;
    }
    return false;
}

}
