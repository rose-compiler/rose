#ifndef WHOLE_AST_API_H
#define WHOLE_AST_API_H

#include <string>
#include <vector>
// external API for generation of graphs of AST

//! Initialize filters to be used from command line options. Should call this before calling frontend()
// Liao, 10/26/2009
void generateGraphOfAST_initFilters (std::vector <std::string>& argvList);

void generateWholeGraphOfAST( std::string filename, std::set<SgNode*> & skippedNodeSet );

void generateWholeGraphOfAST( std::string filename);

void generateWholeGraphOfAST_filteredFrontendSpecificNodes( std::string filename);

void generateGraphOfAST( SgProject* project, std::string filename, std::set<SgNode*> skippedNodeSet );

void generateGraphOfAST( SgProject* project, std::string filename );

// Include debugging visualization support used for AST Copy and AST Outlining (when done to a separate file).
#include "copyGraph.h"

#endif // WHOLE_AST_API_H
