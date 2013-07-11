/**
 *  \file Transform/CollectVars.cc
 *
 *  \brief Collect variable references that need to be passed through
 *  the outlined-function call interface.  SgBasicBlock.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <iostream>
#include <list>
#include <string>


#include "Outliner.hh"
//#include "Transform.hh"
#include "ASTtools.hh"
#include "VarSym.hh"

// =====================================================================

using namespace std;


// =====================================================================
static void
dump (const ASTtools::VarSymSet_t& V, const std::string& tag)
{
//  if (SgProject::get_verbose () >= 2)
    if (Outliner::enable_debug)
    cerr << tag << '{' << ASTtools::toString (V) << '}' << endl;
}

//! Collect the variables to be passed if 's' is to be outlined
//  The variables used (U) but not internally declared (L), and declared within the function (not globally declared)
//  should be passed as parameters to the outlined function
//
// It classifies variables used in s as the following categories
//  * U: all used (referenced) variables within 's'
//  * L: locally (internally to be accurate) declared within the code block 's' to be outlined 
//  * Q: declared within the enclosing function surrounding 's'
//       but not globally declared beyond the function surrounding 's'  (global variables should not be passed if the outlined function  is put within the same file )  
//
void
Outliner::collectVars (const SgStatement* s, 
                       ASTtools::VarSymSet_t& syms) // return the symbols(variables) that need to be passed into the outlined function 
{
  // Determine the function definition surrounding 's'. The enclosing function of 's'
  const SgFunctionDefinition* outer_func_s = ASTtools::findFirstFuncDef (s);
  ROSE_ASSERT (outer_func_s);

  // U = {symbols used within 's'}
  ASTtools::VarSymSet_t U;
  ASTtools::collectRefdVarSyms (s, U);
  dump (U, "U (variables used within s) = ");

  // L = {symbols defined within 's'}, local variables declared within 's'
  ASTtools::VarSymSet_t L;
  ASTtools::collectDefdVarSyms (s, L);
  dump (L, "L (local variables declared within s) = ");

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
    // Q = {symbols defined within the function surrounding 's' that are visible at 's'}, including function parameters
    ASTtools::VarSymSet_t Q;
    ASTtools::collectLocalVisibleVarSyms (outer_func_s->get_declaration (),
        s, Q);
    dump (Q, "Q (variables defined within the function surrounding s that are visible at s) = ");

    // (U - L) \cap Q = {variables that need to be passed as parameters to the outlined function}
    // This is a subset of variables that are not globally visible (no need to pass at all)
    // It excludes the variables with a scope between global and the enclosing function
    set_intersection (diff_U_L.begin (), diff_U_L.end (),
        Q.begin (), Q.end (),
        inserter (syms, syms.begin ()));
    dump (syms, "(U - L) InterSection Q [the variables to be passed into the outlined function]= ");
  }
}

// eof
