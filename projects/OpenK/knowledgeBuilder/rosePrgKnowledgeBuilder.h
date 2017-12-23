#ifndef ROSE_PRG_KNOWLEDGE_BUILDER_H
#define ROSE_PRG_KNOWLEDGE_BUILDER_H

#include <rose.h>
#include <iostream>
#include <map>

using namespace SageInterface;
using namespace SageBuilder;

namespace KG {

// This function generate the localname as in the
// ontology ID prefix:localname for a SgNode
std::string getLocalName(SgNode * n);

int buildKnowledgeBase(SgNode * n);
void exportKnowledgeBase();

// The following functions handle the specific constructs as indicated by their name
// The input is the AST node as the subcalss of SgNode
int translateStatement(SgNode * n, std::string thisIRI);
int translateExpression(SgNode * n, std::string thisIRI); 
int translateLocatedNodeSupport(SgNode * n, std::string thisIRI); 
std::string translateType(SgType * t, std::string thisIRI, bool returnOnly=false);
std::string translateClass(SgNode * n); 
std::string translateModifier(SgModifier * n);

// This function is not useful now, please don't use it
bool sideEffectAnalysis(SgStatement * n, std::string thisIRI); 

}

#endif // ROSE_PRG_KNOWLEDGE_BUILDER_H


