
#include <rose.h>

#include <iostream>

using namespace ParserBuilder;

class Printer {
    public:
        void operator () (GrammarNode::LexemeInstance * lexeme, std::string indent = std::string()) {
            std::cout << "\t" << indent << "[" << lexeme->original_string << "]" << std::endl;
            NonTerminalNode::NonTerminalLexemeInstance * non_terminal_lexeme =
                dynamic_cast<NonTerminalNode::NonTerminalLexemeInstance *>(lexeme);
            if (non_terminal_lexeme != NULL) {
                std::vector<GrammarNode::LexemeInstance *>::iterator it;
                for (it = non_terminal_lexeme->childs_productions.begin(); it != non_terminal_lexeme->childs_productions.end(); it++)
                    (*this)(*it, indent + "|\t");
            }
        }
};

int main() {
    
    // Terminals GrammarNode
    
    SpaceNode * space = new SpaceNode();
    
    LabelNode * pragma_name = new LabelNode("polyhedric_program");
    
    LabelNode * pragma_cuda_opt = new LabelNode("cuda_kernel");
    
    LabelNode * pragma_constraints_opt = new LabelNode("constraints");
    
    LabelNode * fake_constraint_list = new LabelNode("( n > 0 , p > 0 , n >= p )");
    
    // Non-Terminal GrammarNode
    
    NonTerminalNode * pragma_opt = new NonTerminalNode();

        pragma_opt->addProduction();

            pragma_opt->addToLastProduction(pragma_cuda_opt);

        pragma_opt->addProduction();

            pragma_opt->addToLastProduction(pragma_constraints_opt);

            pragma_opt->addToLastProduction(space);

            pragma_opt->addToLastProduction(fake_constraint_list);
            
    NonTerminalNode * pragma_space_opt = new NonTerminalNode();

        pragma_space_opt->addProduction();

            pragma_space_opt->addToLastProduction(space);

            pragma_space_opt->addToLastProduction(pragma_opt);

    NonTerminalNode * root = new NonTerminalNode();

        root->addProduction();
    
            root->addToLastProduction(pragma_name);
    
            root->addToLastProduction(new Any(pragma_space_opt));

	// The parser, it use the class 'Printer' which need to implement void operator () (GrammarNode::LexemeInstance *).
	// In this case, it is just a tree printer (indeed each non terminal lexeme is composed of his child).
    Parser<Printer> my_parser;

    my_parser.setRoot(root);
    
    // Run
    
    Printer printer;
    
    std::string str1("polyhedric_program constraints ( n > 0 , p > 0 , n >= p )");

        std::cout << "Test 1: \"" << str1 << "\"" << std::endl;
    
        my_parser.parse(str1, printer);
        
        std::cout << std::endl << std::endl;

    std::string str2("polyhedric_program cuda_kernel");

        std::cout << "Test 2: \"" << str2 << "\"" << std::endl;
    
        my_parser.parse(str2, printer);
        
        std::cout << std::endl << std::endl;

    std::string str3("polyhedric_program cuda_kernel constraints ( n > 0 , p > 0 , n >= p )");

        std::cout << "Test 3: \"" << str3 << "\"" << std::endl;
    
        my_parser.parse(str3, printer);
        
        std::cout << std::endl << std::endl;

    std::string str4("polyhedric_program constraints ( n > 0 , p > 0 , n >= p ) cuda_kernel constraints ( n > 0 , p > 0 , n >= p )");

        std::cout << "Test 4: \"" << str4 << "\"" << std::endl;
    
        my_parser.parse(str4, printer);
        
        std::cout << std::endl << std::endl;

    return 0;
}
