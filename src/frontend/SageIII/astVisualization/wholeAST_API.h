
// external API for generation of graphs of AST

void generateWholeGraphOfAST( std::string filename, std::set<SgNode*> & skippedNodeSet );

void generateWholeGraphOfAST( std::string filename );

void generateWholeGraphOfAST_filteredFrontendSpecificNodes( std::string filename );

void generateGraphOfAST( SgProject* project, std::string filename, std::set<SgNode*> skippedNodeSet );

void generateGraphOfAST( SgProject* project, std::string filename );

// Include debugging visualization support used for AST Copy and AST Outlining (when done to a separate file).
#include "copyGraph.h"

