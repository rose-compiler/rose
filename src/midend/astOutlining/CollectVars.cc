/**
 *  \file Transform/CollectVars.cc
 *
 *  \brief Collect variable references that need to be passed through
 *  the outlined-function call interface.  SgBasicBlock.
 */
#include <rose.h>
#include <iostream>
#include <list>
#include <string>


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
//  if (SgProject::get_verbose () >= 2)
    if (Outliner::enable_debug)
    cerr << tag << '{' << ASTtools::toString (V) << '}' << endl;
}
//! Collect the variables to be passed if 's' is to be outlined
// It classifies variables used in s as the following categories
//  * L: locally declared  
//  * Q: declared within the function surrounding 's'
//  *  : globally declared beyond the function surrounding 's'   
//  The variables used but not locally declared, and declared within the function
//  should be passed as parameters to the outlined function
//
//  The goal is to minimize the number of variables to be passed.
//  since global variables are always visible even to the outlined function.
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

  // if the outlined function is put into a separated file
  // There are two choices for global variables
  // * pass them as parameters anyway to be lazy
  // * does not pass them as parameters, put extern xxx declarations in the separated file
  if (Outliner::useNewFile) // lazy 
  {
    syms= diff_U_L;
    // TODO  a better way is to find intersection of U-L and global variables and
    //  add extern xxx, for them
    
  }
  else
  {
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
}

// eof
