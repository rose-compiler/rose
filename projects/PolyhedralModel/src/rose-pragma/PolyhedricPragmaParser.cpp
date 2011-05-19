
#include "rose-pragma/PolyhedricPragmaParser.hpp"

PolyhedricPragmaParser::PolyhedricPragmaParser(SgScopeStatement * scope) :
    Parser<PolyhedricPragmaParser>(),
    p_obtained_constraint(),
    p_pragma_cuda_opt(new LabelNode("cuda_kernel")),
    p_constraint(new EqualityExpressionSageNode(scope))
{
    SpaceNode * space = new SpaceNode();
    
    LabelNode * comma = new LabelNode(",");
    
    LabelNode * openning_parent = new LabelNode("(");
    
    LabelNode * closing_parent = new LabelNode(")");
    
    LabelNode * pragma_name = new LabelNode("polyhedric_program");
    
    LabelNode * pragma_cuda_opt = p_pragma_cuda_opt;
    
    LabelNode * pragma_constraints_opt = new LabelNode("constraints");
    
    EqualityExpressionSageNode * constraint = p_constraint;
    
    // Non-Terminal GrammarNode
    
    /*
        constraint_list := (constraint space? comma space?)* constraint
        constraint_list := constraint space? comma space? constraint_list
                         | constraint
    */
    NonTerminalNode * constraints_list = new NonTerminalNode();

        constraints_list->addProduction();
        
            constraints_list->addToLastProduction(constraint);
        
            constraints_list->addToLastProduction(new ZeroOrOne(space));

            constraints_list->addToLastProduction(comma);

            constraints_list->addToLastProduction(new ZeroOrOne(space));

            constraints_list->addToLastProduction(constraints_list);

        constraints_list->addProduction();
        
            constraints_list->addToLastProduction(constraint);

    /*
        pragma_opt := pragma_cuda_opt space?
                    | pragma_constraints_opt space openning_parent space? constraints_list? space? closing_parent space?
    */
    NonTerminalNode * pragma_opt = new NonTerminalNode();

        pragma_opt->addProduction();

            pragma_opt->addToLastProduction(space);

            pragma_opt->addToLastProduction(pragma_cuda_opt);

        pragma_opt->addProduction();

            pragma_opt->addToLastProduction(space);

            pragma_opt->addToLastProduction(pragma_constraints_opt);

            pragma_opt->addToLastProduction(space);
        
            pragma_opt->addToLastProduction(openning_parent);

            pragma_opt->addToLastProduction(new ZeroOrOne(space));

            pragma_opt->addToLastProduction(constraints_list);

            pragma_opt->addToLastProduction(new ZeroOrOne(space));
        
            pragma_opt->addToLastProduction(closing_parent);

    /*
        polyhedric_pragma := pragma_name space pragma_opt*
    */
    NonTerminalNode * polyhedric_pragma = new NonTerminalNode();

        polyhedric_pragma->addProduction();
    
            polyhedric_pragma->addToLastProduction(pragma_name);
    
            polyhedric_pragma->addToLastProduction(new Any(pragma_opt));

            polyhedric_pragma->addToLastProduction(new ZeroOrOne(space));
            
    setRoot(polyhedric_pragma);
}

void PolyhedricPragmaParser::changeScope(SgScopeStatement * scope) {
    p_constraint->setScope(scope);
}

void PolyhedricPragmaParser::operator () (GrammarNode::LexemeInstance * lexeme, std::string indent) {
#if 0
    std::cout << "\t" << indent << "[" << lexeme->original_string << "]" << std::endl;
    NonTerminalNode::NonTerminalLexemeInstance * non_terminal_lexeme =
        dynamic_cast<NonTerminalNode::NonTerminalLexemeInstance *>(lexeme);
    if (non_terminal_lexeme != NULL) {
        std::vector<GrammarNode::LexemeInstance *>::iterator it;
        for (it = non_terminal_lexeme->childs_productions.begin(); it != non_terminal_lexeme->childs_productions.end(); it++)
            (*this)(*it, indent + "|\t");
    }
#else
    NonTerminalNode::NonTerminalLexemeInstance * non_terminal_lexeme =
        dynamic_cast<NonTerminalNode::NonTerminalLexemeInstance *>(lexeme);
    if (non_terminal_lexeme != NULL) {
//        std::cerr << "\tNonTerminalLexemeInstance" << std::endl;
        std::vector<GrammarNode::LexemeInstance *>::iterator it;
        for (it = non_terminal_lexeme->childs_productions.begin(); it != non_terminal_lexeme->childs_productions.end(); it++)
            (*this)(*it);
    }
    else {
        if (lexeme->matching_node == p_constraint) {
            SgNode * parsed_sage_node = ((SageNode::SageNodeLexemeInstance *)lexeme)->sage_node;
            SgExpression * parsed_sage_expression = isSgExpression(parsed_sage_node);
            if (parsed_sage_expression == NULL) {
                std::cerr << "The SAGE's node associated to the lexeme \"" << lexeme->original_string
                    << "\" is not an SgExpression, but "
                    << ( parsed_sage_node == NULL ? "is NULL." : parsed_sage_node->class_name() + "." ) << std::endl;
            }
            else {
                p_obtained_constraint.push_back(parsed_sage_expression);
            }
        }
        else if (lexeme->matching_node == p_pragma_cuda_opt) {
            /* TODO
               find the enclosing function
               if (it's a cuda's kernel)
                   foreach callsites
                       generate a set of constraints (SgExpression) from the device configuration ("<<< ... >>>")
                   if (one call site)
                       append his set of constraints to 'p_obtained_constraint'
                   else
                       find the common scope of all callsite
                       remove constraints refering to variables/functions that didn't come from this scope (or a parent)
                       union of these sets of constraints:
                               <<< (a0, b0, c0) , (d0, e0, f0) >>>
                               <<< (a1, b1, c1) , (d1, e1, f1) >>>
                               ===================================
                               <<< (a , b , c ) , (d , e , f ) >>>
                           with
                               a = max(a0, a1)
                               b = max(b0, b1)
                               ...
                           As constraints need to "static" (sum of variables and expressions), we need to compute these "max".
                           If we can't, we don't use the constraint.
               else
                   ERROR
            */
            std::cerr << "The option \"cuda_kernel\" is not implemented yet." << std::endl;
        }
//        else
//            std::cerr << "\tTerminalLexemeInstance" << std::endl;
    }
#endif
}











































