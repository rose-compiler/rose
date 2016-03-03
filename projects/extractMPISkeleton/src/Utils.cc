#include <boost/foreach.hpp>
#include <rose.h>
#include "Utils.h"
#define foreach BOOST_FOREACH

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
    for(di = defTable.begin(); di != defTable.end(); ++di) {
        StaticSingleAssignment::ReachingDefPtr dp = di->second;
        defs.insert(dp->getDefinitionNode());
    }
    if(debug) {
        std::cout << "Defs for " << n->unparseToString()
                  << ":" << std::endl;
        std::set <SgNode*>::iterator di = defs.begin();
        for(; di != defs.end(); ++di) {
            std::cout << "  " << (*di)->unparseToString() << std::endl;
        }
    }
    return defs;
}

void getNodeVarDefsTransSSA(StaticSingleAssignment *ssa,
                            SgNode *n, std::set<SgNode*> *defs) {
    std::set <SgNode*> newDefs = getNodeVarDefsSSA(ssa, n);
    std::set <SgNode*>::iterator di = newDefs.begin();
    for(; di != newDefs.end(); ++di) {
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

void addInclude ( const std::string & header
                , const SgNode*     node
                ) {
  // For each global scope, only insert header one time.
  // FIXME: may add an extraneous "include <header>" if one exists.

  static SgScopeStatement *scope = NULL;

  SgScopeStatement *current = getGlobalScope(node);

  if ( current != scope ) {
    scope = current;
    insertHeader ( header
                 , PreprocessingInfo::after
                 , true
                 , current
                 );
  }
}

void addStdioH (const SgNode* node) {
  addInclude("stdio.h", node);
}

void addStdlibH (const SgNode* node) {
  addInclude("stdlib.h", node);
}

SgFunctionCallExp*
isFunctionCall( SgNode* node ) {
  if ( node ) {
    switch ( node->variantT() ) {
      case V_SgFunctionCallExp: {
        return isSgFunctionCallExp(node);
      }
      case V_SgExprStatement: {
        SgExprStatement* e_stmt = isSgExprStatement(node);
        return isFunctionCall(e_stmt->get_expression());
      }
      case V_SgAssignOp: {
        SgAssignOp* op = isSgAssignOp(node);
        return isFunctionCall(op->get_rhs_operand());
      }
      case V_SgCastExp: {
        SgCastExp* cast = isSgCastExp(node);
        return isFunctionCall(cast->get_operand());
      }
      case V_SgReturnStmt: {
        SgReturnStmt* r_stmt = isSgReturnStmt(node);
        return isFunctionCall(r_stmt->get_expression());
      }
      default: {
        return NULL;
      }
    }
  }

  return NULL;
}

SgSymbol*
findFunctionHead( SgNode* node ) {
  if ( node ) {
    SgFunctionCallExp* call = isFunctionCall(node);
    if ( call ) {
      return (SgSymbol*) call->getAssociatedFunctionSymbol();
    }
  }

  return NULL;
}

void
buildSymbolToDeclMap( SgProject* const project
                    , std::map <SgSymbol*, SgFunctionDeclaration* > declTable
                    ) {
    NodeQuerySynthesizedAttributeType defs =
        NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    foreach(const SgNode * const node, defs) {
        const SgFunctionDefinition * const def = isSgFunctionDefinition(node);
        if ( def ) {
          SgFunctionDeclaration* const decl = def->get_declaration();
          if ( decl ) {
            SgSymbol* const fun = decl->get_symbol_from_symbol_table();
            if ( fun ) {
              if ( declTable[fun] == NULL ) {
                declTable[fun] = decl;
                if ( debug ) {
                  std::cout << "Added binding for "
                            << fun->get_name().str()            << std::endl;
                }
              } else if ( debug ) {
                std::cout << "Duplicate definition for "
                          << fun->get_name().str()              << std::endl;
              }
            } else if ( debug ) {
              std::cout << "No declaration associated with definition"
                                                                << std::endl;
            }
          } else if ( debug ) {
            std::cout << "Duplicate definition for "
                                                                << std::endl;
          }
        } else if ( debug ) {
          std::cout << "No definition?"
                                                                << std::endl;
        }
    }
}

