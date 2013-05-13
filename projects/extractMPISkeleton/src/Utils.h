#ifndef __Utils_H_LOADED__
#define __Utils_H_LOADED__

#include <staticSingleAssignment.h>

#include <vector>

void showSrcLoc (SgNode *i);

extern bool debug;

std::set<SgNode*> getNodeVarDefsSSA(StaticSingleAssignment *ssa, SgNode *n);
void getNodeVarDefsTransSSA(StaticSingleAssignment *ssa,
                            SgNode *n, std::set<SgNode*> *defs);
SgSymbol *getEnclosingSym(SgNode *n);

void addStdioH (const SgNode *n);
void addStdlibH (const SgNode *n);

SgFunctionCallExp* isFunctionCall( SgNode* node );
SgSymbol* findFunctionHead( SgNode* node );

void
buildSymbolToDeclMap( SgProject* const project
                    , std::map <SgSymbol*, SgFunctionDeclaration* > declTable
                    );

#endif
