/**
 *  \brief Collect variable references that need to be passed through
 *  the outlined cuda kernel call interface.  
 */

#include "sage3basic.h"
#include <iostream>
#include <list>
#include <string>

#include "CudaOutliner.h"
#include "Outliner.hh"
#include "ASTtools.hh"
#include "VarSym.hh"

// =====================================================================

using namespace std;

// =====================================================================
static void
dump (const ASTtools::VarSymSet_t& V, const std::string& tag)
{
  //if (Outliner::enable_debug)
      cout << tag << "{ " << ASTtools::toString (V) << " }" << endl << endl;
}

//! Collect the variables to be passed if 'kernel_body' is to be outlined
// It classifies variables used in s as the following categories
//  * L: locally declared  
//  * U: declared within the function surrounding 'kernel_body'
//  * Q : globally declared beyond the function surrounding 'kernel_body' 
//  * R : reserved variables in cuda such as threadidx.x   
//  The variables used but not locally declared, and declared within the function
//  should be passed as parameters to the outlined function
void
CudaOutliner::collectVars (const SgStatement* kernel_body, 
			   ASTtools::VarSymSet_t& syms)  

{
  // Determine the function definition surrounding 'kernel_body'. The enclosing function of 'kernel'
  const SgFunctionDefinition* outer_func_s = ASTtools::findFirstFuncDef (kernel_body);
  ROSE_ASSERT (outer_func_s);

  // L = {symbols defined within 'kernel_body'}, local variables declared within 'kernel_body'
  ASTtools::VarSymSet_t L;
  ASTtools::collectDefdVarSyms (kernel_body, L);
  //dump (L, "L = ");

  // U = {symbols used within 'kernel_body'}
  ASTtools::VarSymSet_t U;
  ASTtools::collectRefdVarSyms (kernel_body, U);
  //dump (U, "U = ");

  // U - L = {symbols used within 'kernel_body' but not defined in 'kernel_body'}
  // variable references to non-local-declared variables
   ASTtools::VarSymSet_t diff_U_L;
  set_difference (U.begin (), U.end (), L.begin (), L.end (),
                  inserter (diff_U_L, diff_U_L.begin ()));
  //dump (diff_U_L, "U - L = ");

  //we need to pass the global variables as well 
  //but we cannot pass the cuda reserved variables such as threadidx.x 
  //we need to exclude that set from U-L and pass only U-L-R 
  
  // if the outlined function is put into a separated file
  // There are two choices for global variables
  // * pass them as parameters anyway to be lazy
  // * does not pass them as parameters, put extern xxx declarations in the separated file
  if (true)//Outliner::useNewFile) // lazy 
  {
    //TODO 
    // need to handle constant variables separately
    // let's put contants into constant memory and do not 
    // pass as an argument
    for (ASTtools::VarSymSet_t::const_iterator i = diff_U_L.begin (); i!= diff_U_L.end (); ++i)
      {     
	syms.insert(*i);
      }

    //syms= diff_U_L;
    // TODO  a better way is to find intersection of U-L and global variables and
    //  add extern xxx, for them    
  }
  else
  {
    // Q = {symbols defined within the function surrounding 'kernel_body' that are
    // visible at 'kernel_body'}, including function parameters
    ASTtools::VarSymSet_t Q;
    ASTtools::collectLocalVisibleVarSyms (outer_func_s->get_declaration (),
        kernel_body, Q);
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

  cout << "  INFO:Mint: Arguments passed to the kernel: "  ;
  dump (syms, " ");
}

void CudaOutliner::addNewVars (ASTtools::VarSymSet_t& all_deviceSyms, 
			       const ASTtools::VarSymSet_t new_syms)  
{//adds new kernels variables to the parallel region var list 
  for (ASTtools::VarSymSet_t::const_iterator i = new_syms.begin (); i!= new_syms.end (); ++i)
    {     
      ASTtools::VarSymSet_t::iterator found = find(all_deviceSyms.begin(), all_deviceSyms.end(), *i );

      if(found == all_deviceSyms.end())
	{//this is a new symbol, should add into the list
	  all_deviceSyms.insert(*i);
	}
      else 
	{//we have this already in the list, 
	  //do nothing.
	}
    }
}

void CudaOutliner::getDevSymsNotInStruct (ASTtools::VarSymSet_t& new_deviceSyms, 
					  const ASTtools::VarSymSet_t dev_syms, 
					  const MintHostSymToDevInitMap_t hostToDevVars)  
{
  for (ASTtools::VarSymSet_t::const_iterator i = dev_syms.begin (); i!= dev_syms.end (); ++i)
    {     
     
      SgVariableSymbol* sym= const_cast<SgVariableSymbol*> (*i);

      //Check if is of the fields of the struct
      if(hostToDevVars.find(sym) != hostToDevVars.end())
	{
	  //do nothing, it is already in the struct
	}
      else 
	{//we need to pass this one to the kernel because it is not in the struct
	  new_deviceSyms.insert(sym);
	}

    }//end of sym list
}

// eof
