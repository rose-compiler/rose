#include "rose.h"
#include "Utils.h"

using namespace SageBuilder;
using namespace SageInterface;

bool debug = false;

void showSrcLoc (SgNode *i) {
  Sg_File_Info &fileInfo = *(i->get_file_info());
  printf ("Query node = %p = %s in %s \n ----- at line %d on column %d \n",
          i,i->sage_class_name(),fileInfo.get_filename(),
          fileInfo.get_line(), fileInfo.get_col());
}

std::set<SgNode*> getNodeVarDefsSSA(StaticSingleAssignment *ssa, SgNode *n) {
    std::set <SgNode*> defs;
    StaticSingleAssignment::NodeReachingDefTable defTable =
        ssa->getUsesAtNode(n);
    StaticSingleAssignment::NodeReachingDefTable::iterator di;
    for(di = defTable.begin(); di != defTable.end(); di++) {
        StaticSingleAssignment::ReachingDefPtr dp = di->second;
        defs.insert(dp->getDefinitionNode());
    }
    if(debug) {
        std::cout << "Defs for " << n->unparseToString()
                  << ":" << std::endl;
        std::set <SgNode*>::iterator di = defs.begin();
        for(; di != defs.end(); di++) {
            std::cout << "  " << (*di)->unparseToString() << std::endl;
        }
    }
    return defs;
}

void getNodeVarDefsTransSSA(StaticSingleAssignment *ssa,
                            SgNode *n, std::set<SgNode*> *defs) {
    std::set <SgNode*> newDefs = getNodeVarDefsSSA(ssa, n);
    std::set <SgNode*>::iterator di = newDefs.begin();
    for(; di != newDefs.end(); di++) {
        if(defs->find(*di) == defs->end()) {
            defs->insert(*di);
            getNodeVarDefsTransSSA(ssa, *di, defs);
        }
    }
}

SgSymbol *getEnclosingSym(SgNode *n) {
    SgFunctionDefinition *def;
    if(n == NULL) return NULL;
    def = isSgFunctionDefinition(n);
    if(def)
        return def->get_declaration()->
            search_for_symbol_from_symbol_table();
    return getEnclosingSym(n->get_parent());
}

void addStdioH (const SgNode *n) {
  // For each global scope, only insert header one time.
  // FIXME: may add an extraneous "include <stdio.h>" if one exists.

  static SgScopeStatement *gs = NULL;

  SgScopeStatement *current_gs = getGlobalScope(n);

  if (current_gs != gs) {
    gs = current_gs;
    insertHeader ( "stdio.h",
                   PreprocessingInfo::after,
                   true,
                   current_gs);
  }
}

void addStdlibH (const SgNode *n) {
  // For each global scope, only insert header one time.
  // FIXME: may add an extraneous "include <stdlib.h>" if one exists.

  static SgScopeStatement *gs = NULL;

  SgScopeStatement *current_gs = getGlobalScope(n);

  if (current_gs != gs) {
    gs = current_gs;
    insertHeader ( "stdlib.h",
                   PreprocessingInfo::after,
                   true,
                   current_gs);
  }
}

