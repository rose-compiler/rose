#ifndef ASTBIN_TRAVERSAL_H
#define ASTBIN_TRAVERSAL_H

#include "RoseBin_support.h"
#include "AsmUnparser_compat.h"
#include <fstream>

class AST_BIN_Traversal
  : public AstSimpleProcessing {
  // Checker specific parameters should be allocated here.
    std::ofstream myfile;
    int nrOfInstructions;
public:
    int getNrOfInstructions() {return nrOfInstructions;}
    AST_BIN_Traversal() {}
    ~AST_BIN_Traversal() {}
  // The implementation of the run function has to match the traversal being called.
  void run(SgNode* n, std::string file);
  void visit(SgNode* n);
};

#endif
