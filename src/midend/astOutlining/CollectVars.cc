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
//! Collect the variables to be passed if 's' is to be outlined
void
Outliner::Transform::collectVars (const SgStatement* s, ASTtools::VarSymSet_t& syms)
{
  // Determine the function definition surrounding 's'. The enclosing function of 's'
  const SgFunctionDefinition* outer_func_s = ASTtools::findFirstFuncDef (s);
  ROSE_ASSERT (outer_func_s);

  // L = {symbols defined within 's'}, local variables declared within 's'
  ASTtools::VarSymSet_t L;
  ASTtools::collectDefdVarSyms (s, L);
  dump (L, "L = ");

  // U = {symbols used within 's'}
  ASTtools::VarSymSet_t U;
  ASTtools::collectRefdVarSyms (s, U);
  dump (U, "U = ");

  // U - L = {symbols used within 's' but not defined in 's'}
  // variable references to non-local-declared variables
  ASTtools::VarSymSet_t diff_U_L;
  set_difference (U.begin (), U.end (), L.begin (), L.end (),
                  inserter (diff_U_L, diff_U_L.begin ()));
  dump (diff_U_L, "U - L = ");

  // Q = {symbols defined within the function surrounding 's' that are
  // visible at 's'}, including function parameters
  ASTtools::VarSymSet_t Q;
  ASTtools::collectLocalVisibleVarSyms (outer_func_s->get_declaration (),
                                        s, Q);
  dump (Q, "Q = ");

  // (U - L) \cap Q = {variables that need to be passed as parameters
  // to the outlined function}
  // a sub set of variables that are not globally visible (no need to pass at all)
  // It excludes the variables with a scope between global and the enclosing function
  set_intersection (diff_U_L.begin (), diff_U_L.end (),
                    Q.begin (), Q.end (),
                    inserter (syms, syms.begin ()));
  dump (syms, "(U - L) \\cap Q = ");
}

// eof
