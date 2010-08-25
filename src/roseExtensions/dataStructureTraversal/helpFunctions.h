#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H




// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std; 

typedef std::vector <SgNode *, std::allocator <SgNode * > > SgNodePtrVector;
extern string findPragmaStringUnionControl (string pragmaString, string unionName);
extern SgNodePtrVector findScopes (SgNode * astNode);
extern SgNode* findClassDeclarationFromType (SgNodePtrVector nodeVector, SgType * sageType);
extern SgTypedefDeclaration* findTypedefFromTypeName (SgNodePtrVector nodeVector, const string sageName);
extern list<SgNode * > findClassDeclarationsFromTypeName (SgNodePtrVector nodeVector,
                                                          const string sageName);
extern list<SgNode * > queryNodeClassDeclarationFromTypedefName (SgNode * astNode,
                                                                 SgNode * nameNode);
#endif
