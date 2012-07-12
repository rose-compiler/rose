#ifndef CUDAMEMORYMANAGEMENT_H
#define CUDAMEMORYMANAGEMENT_H

#include "rose.h"

#include "rewrite.h"            //ast rewrite mechanism: insert, remove, replace

#include "Outliner.hh"
#include "DataTransferSizes.h"
//#include "CudaOutliner.h"          
//#include "NameGenerator.hh"     //unique function name generator for outlining
#include "unparser.h"           //parses types into strings 

#include "../../types/MintTypes.h"
#include "ASTtools.hh"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#define DEBUG 1 

using namespace std;
using namespace OmpSupport;
using namespace SageBuilder;
using namespace SageInterface;

class CudaMemoryManagement
{
 public:

  static void freeParallelRegionVars(SgStatement* last_stmt, 
				     MintHostSymToDevInitMap_t& hostToDevVars);

  //this function creates device pointers on the host, allocate memory on the dev and issues memory transfer                               
  //outside of the parallel region if the transfer is not handled through a pragma 
 static void handleCudaHostVars(SgNode* node, //std::string& wrapper_name,
				ASTtools::VarSymSet_t& deviceSyms, 
				std::set<SgInitializedName*>& readOnlyVars,
				MintSymSizesMap_t& trfSizes, 
				MintHostSymToDevInitMap_t& hostToDevVars);

 //swap pointer fix 
 static void postTransformationVariableHandling(SgNode* node,
						ASTtools::VarSymSet_t& deviceSyms, 
						MintHostSymToDevInitMap_t& hostToDevVars);


 //currently not used, issue 1D transfers with no padding
 static void issueDataTransferToDevice(SgNode* node, 
				       SgVariableSymbol* src_sym, 
				       SgInitializedName* dest, 
				       SgExpression* numElements);
 

 //converts user pragmas into memory transfer calls 
 //uses pitched memory transfers 
 static void issueDataTransferToDevice(SgNode* node, 
				       const MintTrfParams_t params,
				       MintHostSymToDevInitMap_t& hostToDevVars, bool inside=false);

 static void issueDataTransferFromDevice(SgNode* node, 
					 const MintTrfParams_t params,
					 MintHostSymToDevInitMap_t& hostToDevVars, bool inside=false);


  static SgVariableDeclaration* issueTransferForArrayStruct(SgStatement* target, string wrapper_name);

private:

  //declares and allocates device pointers on the device memory
  static SgStatement* declareAndAllocateDevicePtr(SgStatement* target,
							    SgVariableDeclaration* &, 
							    SgVariableDeclaration* &, 
							    SgVariableDeclaration* &,
							    SgType* baseType,
							    string ptr_name, 
							    const MintTrfParams_t params);

 //swap pointer fix 
 static void replaceHostRefsWithDeviceRefs(SgNode* node,
					   ASTtools::VarSymSet_t& deviceSyms, 
					   MintHostSymToDevInitMap_t& hostToDevVars, 
					   bool typecast = false);

 /*Generates cuda host variables if pragma is not used
   Aims to automatically determine what to copy, but not reliable */
 static void generateCudaHostVar(ASTtools::VarSymSet_t& deviceSyms, 
				 MintSymSizesMap_t& trfSizes,
				 SgNode* node, 
				 MintHostSymToDevInitMap_t& hostToDevVars);

 /*The following function is used to determine the size of an array when the size is not 
  specificed in a copy pragma, not always correct                               
 */  
 static SgExpression* getSizeExpressionOfDynamicArray(SgStatement* target, 
						      SgInitializedName* var_name,
						      MintSymSizesMap_t& trfSizes);
  
 /*Following functions are used in normal 1D memory transfers, no padding/pitch*/
 
 static SgStatement* generateCudaMallocCall(SgVariableSymbol* var_decl, SgExpression* numElements);  

 static SgStatement* generateCudaMemCpyCallToDevice(SgVariableSymbol* dest_sym, 
						    SgVariableSymbol* src_sym, 
						    SgExpression* numElements,
						    SgExpression* trfKind);

 static SgStatement* generateCudaMemCpyCallFromDevice(SgVariableSymbol* dest_sym, 
						      SgVariableSymbol* src_sym, 
						      SgExpression* numElements,
						      SgExpression* trfKind);

 static  SgStatement* generateCudaFreeCall(SgVariableSymbol* sym);

 /*
   The following functions are used in pitched memory transfers
   They are called after processing mint copy pragmas 
 */
 static SgFunctionCallExp* generateCudaMemCpyCall(SgVariableDeclaration* param_dec,
						  SgScopeStatement* scope);
 
 static SgVariableDeclaration* generateCudaExtentVar(MintTrfParams_t params, 
						     SgScopeStatement* g_scope,
						     SgType* baseType);

 static SgFunctionCallExp* generateCudaMallocCall(SgVariableDeclaration* dev_decl,
						  SgVariableDeclaration* extent_decl,			
						  SgScopeStatement* scope);

 static SgStatement* generateCheckCudaSuccessStatement(SgVarRefExp* status,
						       SgScopeStatement* scope);
 

 static void generateCudaMemCpyToDeviceParams(SgStatement* target, 
					      SgVariableDeclaration* param_decl,
					      SgVarRefExp* dev_var,			
					      SgVarRefExp* extent_var,			
					      SgInitializedName* src_name,
					      MintTrfParams_t params,
					      SgScopeStatement* scope);

 static void generateCudaMemCpyFromDeviceParams(SgStatement* target, 
					      SgVariableDeclaration* param_decl,
					      SgVarRefExp* dev_var,			
					      SgVarRefExp* extent_var,			
					      SgInitializedName* src_name,
					      MintTrfParams_t params,
					      SgScopeStatement* scope);
 
};
#endif 
