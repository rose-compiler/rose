/**
 *  \file NonLocalDecls.cc
 *
 *  \brief This preprocessing phase collects local function
 *  declarations into a to-be-outlined basic block.
 *
 *  \todo Implement gathering of local type declarations.
 */
#include <rose.h>
#include <iostream>
#include <list>
#include <string>


#include "Preprocess.hh"
#include "Copy.hh"

// =====================================================================

using namespace std;

// =====================================================================

static
void
copyNonLocalFuncDecls (SgBasicBlock* b)
{
  typedef Rose_STL_Container<SgNode *> NodeList_t;
  NodeList_t refs = NodeQuery::querySubTree (b, V_SgFunctionRefExp);
  for (NodeList_t::iterator i = refs.begin (); i != refs.end (); ++i)
    {
      SgFunctionRefExp* ref = isSgFunctionRefExp (*i);
      ROSE_ASSERT (ref);
      SgFunctionSymbol* sym = ref->get_symbol ();
      ROSE_ASSERT (sym);
      SgFunctionDeclaration* decl = sym->get_declaration ();
      ROSE_ASSERT (decl);

      SgBasicBlock* bb_parent = isSgBasicBlock (decl->get_parent ());
      if (bb_parent && bb_parent != b)
        {
          SgFunctionDeclaration* proto_copy = isSgFunctionDeclaration (ASTtools::deepCopy (decl));
          ROSE_ASSERT (proto_copy);
          ROSE_ASSERT (!proto_copy->get_definition ());
          b->prepend_statement (proto_copy);
          proto_copy->set_parent (b);
          //Liao, 11/5/2007, maitain symbol table
          //it shares the same function symbol, bug 104
          // just some clue obtained from AST graph, don't quite understand why. 
          proto_copy->set_firstNondefiningDeclaration(decl);
#if 0
          // it has no separated function symbol, but share one with previous declaration.
          SgFunctionSymbol * symbol_1 = new SgFunctionSymbol(proto_copy);
          b->insert_symbol(proto_copy->get_name(), symbol_1);
#endif
        }
    }
}

// =====================================================================

void
Outliner::Preprocess::gatherNonLocalDecls (SgBasicBlock* b)
{
  copyNonLocalFuncDecls (b);
}

// eof
