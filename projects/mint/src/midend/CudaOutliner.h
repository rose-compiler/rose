/*
  by Didem Unat 
  
  This class generates the outlined function for cuda kernels 

*/

#ifndef _CUDAOUTLINER_H
#define _CUDAOUTLINER_H

//#include <rose.h>
#include <iostream>
#include <string>
#include <sstream>


#include "Outliner.hh"
#include "ASTtools.hh"
#include "VarSym.hh"
#include "StmtRewrite.hh"

#include "../types/MintTypes.h"

using namespace std;
using namespace OmpSupport;
using namespace SageBuilder;
using namespace SageInterface;

class CudaOutliner
{
public:

  static SgStatement* generateCall (SgFunctionDeclaration* out_func,
				    MintHostSymToDevInitMap_t hostToDevVars,
                                    const ASTtools::VarSymSet_t& syms,
                                    SgScopeStatement* scope, 
				    string, string);

  static void setKernelParams(int dim, SgStatement* , SgScopeStatement* scope);

  static void collectVars (const SgStatement* s, 
			   ASTtools::VarSymSet_t& syms)  ;

  
  static SgFunctionDeclaration * generateFunction ( SgBasicBlock* s,
                                                    const string& func_name_str,
						    ASTtools::VarSymSet_t& syms,
						    MintHostSymToDevInitMap_t hostToDevVars,	
						    const ASTtools::VarSymSet_t& pdSyms,
                                                    const ASTtools::VarSymSet_t& psyms,
                                                    SgScopeStatement* scope);

  static void addNewVars(ASTtools::VarSymSet_t& all_devSyms, 
			 const ASTtools::VarSymSet_t new_syms) ;

  static void getDevSymsNotInStruct(ASTtools::VarSymSet_t& all_devSyms, 
				    const ASTtools::VarSymSet_t new_syms,
				    const MintHostSymToDevInitMap_t hostToDevVars) ;


private:
  static SgFunctionDeclaration *createFuncSkeleton (const string& name, SgType* ret_type,
						    SgFunctionParameterList* params, SgScopeStatement* scope);

  static void functionParameterHandling(ASTtools::VarSymSet_t& syms, // regular (shared) parameters 
					MintHostSymToDevInitMap_t hostToDevVars,						    
					const ASTtools::VarSymSet_t& pdSyms, // those must use pointer dereference                  
					const ASTtools::VarSymSet_t& pSyms,  
					// private variables, handles dead variables (neither livein nor liveout: TODO)      
					std::set<SgInitializedName*> & readOnlyVars,
					std::set<SgInitializedName*> & liveOutVars,
					SgFunctionDeclaration* func); // the outlined function  
};

#endif //_CUDAOUTLINER_H
