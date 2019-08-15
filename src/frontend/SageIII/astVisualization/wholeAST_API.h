#ifndef WHOLE_AST_API_H
#define WHOLE_AST_API_H

#include <string>
#include <vector>
// external API for generation of graphs of AST

#include "wholeAST.h"

// DQ (8/20/2013): Added initial support for IR node specific graphs.
#include "intermediateRepresentationNodeGraphs.h"

//! Generate a whole AST graph with a specified filename, using filter_flags to turn on/off builtin node filters and using skippedNodeSet to exclude customized node set.
ROSE_DLL_API void generateWholeGraphOfAST( std::string filename, std::set<SgNode*> & skippedNodeSet, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags);
//!Generate a whole AST graph with a specified filename and a set of explicitly excluded nodes
ROSE_DLL_API void generateWholeGraphOfAST( std::string filename, std::set<SgNode*> & skippedNodeSet );

//! Generate a whole AST graph with a specified filename, and the default filter sets, excluding frontend-specific nodes by default
ROSE_DLL_API void generateWholeGraphOfAST( std::string filename);
//! Generate a whole AST graph with a specified filename and a set of filter flags, excluding frontend-specific nodes by default
ROSE_DLL_API void generateWholeGraphOfAST( std::string filename, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags);

ROSE_DLL_API void generateWholeGraphOfAST_filteredFrontendSpecificNodes( std::string filename, CustomMemoryPoolDOTGeneration::s_Filter_Flags* flags/*=NULL*/);

ROSE_DLL_API void generateGraphOfAST( SgProject* project, std::string filename, std::set<SgNode*> skippedNodeSet );

ROSE_DLL_API void generateGraphOfAST( SgProject* project, std::string filename );

// Include debugging visualization support used for AST Copy and AST Outlining (when done to a separate file).
#include "copyGraph.h"

// DQ (11/10/2018): Adding support for specialized include graph.
#include "includeGraph.h"

ROSE_DLL_API void generateGraphOfIncludeFiles( SgProject* project, std::string filename );
ROSE_DLL_API void generateGraphOfIncludeFiles( SgSourceFile* sourceFile, std::string filename );

#endif // WHOLE_AST_API_H
