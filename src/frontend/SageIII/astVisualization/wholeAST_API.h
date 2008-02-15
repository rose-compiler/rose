
// external API for generation of graphs of AST

void generateWholeGraphOfAST( std::string filename, std::set<SgNode*> & skippedNodeSet );

void generateWholeGraphOfAST( std::string filename );

void generateWholeGraphOfAST_filteredFrontendSpecificNodes( std::string filename );

void generateGraphOfAST( SgProject* project, std::string filename, std::set<SgNode*> skippedNodeSet );

void generateGraphOfAST( SgProject* project, std::string filename );

