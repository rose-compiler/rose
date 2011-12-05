
#ifndef MINTARRAYINTERFACE_H
#define MINTARRAYINTERFACE_H

#include "rose.h"
//#include "ASTtools.hh"
#include <vector>

#include "../../types/MintTypes.h"
using namespace std;
using namespace SageInterface;

class MintArrayInterface
{

 public:

  static bool isArrayReference(SgExpression* ref, 
			       SgExpression** arrayName/*=NULL*/, 
			       std::vector<SgExpression*>* subscripts/*=NULL*/);

  static void linearizeArrays(SgFunctionDeclaration* node);
  static void linearizeArrays(SgSourceFile* file);



  static bool isStencilArray(std::vector<SgExpression*> expList) ;

  static void getArrayReferenceList(SgNode* kernel_body, 
				    MintInitNameMapExpList_t& refList,
				    bool sharedMemRef = true);//return shared mem ref as well
  

  static SgVariableSymbol* getSymbolFromName(SgBasicBlock* block, string varStr);

  //  static void findAllArrayRefExpressions(SgFunctionDeclaration* kernel, 
  //					 std::vector<SgExpression*>& arrRefList);

  static int getDimension(const SgInitializedName* var, SgType* var_type=NULL);
 private:

  static SgExpression* linearizeThisArrayRefs(SgNode* arrayNode, SgBasicBlock* kernel_body,
					      SgScopeStatement* indexScope, 
					      SgInitializedName* arr_iname, 
					      bool useSameIndex=false);

};

#endif //MINTARRAYINTERFACE_H
