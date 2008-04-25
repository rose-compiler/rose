/**
 *  \file Transform/CollectVars.cc
 *
 *  \brief Collect variable references that need to be passed through
 *  the outlined-function call interface.  SgBasicBlock.
 */

#include <iostream>
#include <list>
#include <string>
#include <rose.h>

#include "Transform.hh"
#include "ASTtools.hh"
#include "VarSym.hh"

// =====================================================================

using namespace std;

// =====================================================================
static
void
dump (const ASTtools::VarSymSet_t& V, const std::string& tag)
{
  if (SgProject::get_verbose () >= 2)
    cerr << tag << '{' << ASTtools::toString (V) << '}' << endl;
}

void
Outliner::Transform::collectVars (const SgStatement* s,
                                     ASTtools::VarSymSet_t& syms)
{
  // Determine the function definition surrounding 's'.
  const SgFunctionDefinition* outer_func_s = ASTtools::findFirstFuncDef (s);
  ROSE_ASSERT (outer_func_s);

  // L = {symbols defined within 's'}
  ASTtools::VarSymSet_t L;
  ASTtools::collectDefdVarSyms (s, L);
  dump (L, "L = ");

  // U = {symbols used within 's'}
  ASTtools::VarSymSet_t U;
  ASTtools::collectRefdVarSyms (s, U);
  dump (U, "U = ");

  // U - L = {symbols used within 's' but not defined in 's'}
  ASTtools::VarSymSet_t diff_U_L;
  set_difference (U.begin (), U.end (), L.begin (), L.end (),
                  inserter (diff_U_L, diff_U_L.begin ()));
  dump (diff_U_L, "U - L = ");

  // Q = {symbols defined in the function surrounding 's' that are
  // visible at 's'}
  ASTtools::VarSymSet_t Q;
  ASTtools::collectLocalVisibleVarSyms (outer_func_s->get_declaration (),
                                        s, Q);
  dump (Q, "Q = ");

  // (U - L) \cap Q = {variables that need to be passed as parameters
  // to the outlined function}
  set_intersection (diff_U_L.begin (), diff_U_L.end (),
                    Q.begin (), Q.end (),
                    inserter (syms, syms.begin ()));
  dump (syms, "(U - L) \\cap Q = ");
}

// eof
