#ifndef H_SATIRE_UTILS
#define H_SATIRE_UTILS

#include "satire.h"

namespace SATIrE {

// Build an AnalyzerOptions object from a given command line; the object is
// dynamically allocated, its ownership passes to the caller.
AnalyzerOptions *extractOptions(int argc, char **argv);

// Build a ROSE AST, given a command line; the command line specifies either
// some source files or exactly one binary AST file. This also performs AST
// sanity checks if requested.
SgProject *createRoseAst(AnalyzerOptions *options);

// This function computes the ICFG for a given AST and a given set of
// options. Also tests the ICFG if requested by the analyzer options, and
// outputs a visualization if requested by the analyzer options.
CFG *createICFG(SgProject *astRoot, AnalyzerOptions *options);

// This function generates various representations of the AST -- source
// or binary AST -- as requested by the options.
void outputRoseAst(SgProject *astRoot, AnalyzerOptions *options);

// This is the general driver for any program output as requested by the
// user.
void outputProgramRepresentation(Program *program, AnalyzerOptions *options);

}

#endif
