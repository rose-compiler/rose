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

#if 0
//collect clause variables from an OpenMP statement's clause with a variable list
static void
collectOmpClauseVars(SgOmpClauseBodyStatement * clause_stmt, ASTtools::VarSymSet_t& omp_syms, const VariantT & vt)
{
  ROSE_ASSERT(clause_stmt != NULL);
  Rose_STL_Container<SgOmpClause*> p_clause =
    NodeQuery::queryNodeList<SgOmpClause>(clause_stmt->get_clauses(),vt);

  if (p_clause.size()== 0)  // Does a matching clause exist? 
    return;

  // ROSE_ASSERT(p_clause.size() ==1); // multiple reduction clauses may exist
  //cout<<"Found private clause:"<<isSgOmpPrivateClause(p_clause[0])->unparseToString()<<endl;

  for (int i =0; i< p_clause.size(); i++)
  {
    SgInitializedNamePtrList namelist = isSgOmpVariablesClause(p_clause[i])->get_variables();
    SgInitializedNamePtrList::const_iterator iter = namelist.begin();
    for (; iter!=namelist.end(); iter++)
    {
      SgInitializedName * iname = *iter;
      ROSE_ASSERT(iname != NULL);
      SgSymbol* symbol = iname->get_symbol_from_symbol_table();
      ROSE_ASSERT(symbol);
      SgVariableSymbol* varsymbol = isSgVariableSymbol(symbol);
      ROSE_ASSERT( varsymbol != NULL);
      omp_syms.insert(varsymbol);
    }
  } // for all matched clauses
}

#endif

//! Collect the variables to be passed if 's' is to be outlined
// It classifies variables used in s as the following categories
//  * L: locally declared  
//  * Q: declared within the function surrounding 's'
//  *  : globally declared beyond the function surrounding 's'   
//  The variables used but not locally declared, and declared within the function
//  should be passed as parameters to the outlined function
void
Outliner::collectVars (const SgStatement* s, 
                       ASTtools::VarSymSet_t& syms,  
                       ASTtools::VarSymSet_t& psyms,
                       ASTtools::VarSymSet_t& fpsyms,
                       ASTtools::VarSymSet_t& reductionSyms)
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

#if 0 // Handled by OmpSupport::transOmpVariables()
  // Collect OpenMP private variables 
  // Is OpenMP lowering requested?
  if  (SageInterface::getEnclosingFileNode(const_cast<SgStatement*>(s))->get_openmp_lowering())
  {
   // cout<<"detected openmp lowering ...."<<endl;
    //ROSE_ASSERT(s != NULL);
    ROSE_ASSERT(s->get_parent() != NULL);
    SgOmpParallelStatement * p_stmt = 
         const_cast<SgOmpParallelStatement *>(isSgOmpParallelStatement(s->get_parent()));
    if (p_stmt) // is the target the block of a parallel region?
    {
#if 0
      Rose_STL_Container<SgOmpClause*> p_clause = 
        NodeQuery::queryNodeList<SgOmpClause>(p_stmt->get_clauses(),V_SgOmpPrivateClause);
      if (p_clause.size()>0)  // Does a private clause exist? 
      {
        ROSE_ASSERT(p_clause.size() ==1); // we only expect one private clause in AST
        //cout<<"Found private clause:"<<isSgOmpPrivateClause(p_clause[0])->unparseToString()<<endl;
        SgInitializedNamePtrList namelist = isSgOmpPrivateClause(p_clause[0])->get_variables();
        SgInitializedNamePtrList::const_iterator iter = namelist.begin();
        for (; iter!=namelist.end(); iter++)
        {
          SgInitializedName * iname = *iter;
          ROSE_ASSERT(iname != NULL);
          SgSymbol* symbol = iname->get_symbol_from_symbol_table();
          ROSE_ASSERT(symbol);
          SgVariableSymbol* varsymbol = isSgVariableSymbol(symbol);
          ROSE_ASSERT( varsymbol != NULL);
          psyms.insert(varsymbol);
        }
      } // end if private clause
#else
      collectOmpClauseVars(p_stmt, psyms, V_SgOmpPrivateClause); 
#endif
    } // end if body of omp parallel

    // set subtract  original_parameters - private_parameters  
    // TODO private_parameters == neither live-in nor live-out variables
    ASTtools::VarSymSet_t diff_private;
    set_difference(syms.begin(), syms.end(), psyms.begin(), psyms.end(),
                inserter (diff_private, diff_private.begin()));
    syms = diff_private; 

    // collect firstprivate, reduction etc. They should be kept 
    // in both syms and fpsyms since a parameter for each of them need to be passed 
    // and a private copy is also needed. 
  collectOmpClauseVars(p_stmt, fpsyms, V_SgOmpFirstprivateClause); 
  collectOmpClauseVars(p_stmt, reductionSyms, V_SgOmpReductionClause); 
     
  } // end if openmp lowering
#endif  
}

// eof
