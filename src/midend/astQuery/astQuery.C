
#include "rose.h"

using namespace std;


#define DEBUG_NODEQUERY 0


// **********************************************************
//     Support for nested class: AstQuery<AstQuerySynthesizedAttributeType>::VariantVector
// **********************************************************

VariantVector::VariantVector ( const VariantT & X )
{

  // int stopIndex =0;
  size_t i         =0;

  this->push_back(X);

  do{
    SgNode::getClassHierarchySubTreeFunction( (*this)[i], *this);
    i++;
  }while( i < this->size() );

  // printVariantVector(*this);
  // printf ("End of VariantVector constructor \n");
  // ROSE_ASSERT (false);
}

VariantVector::VariantVector ( const VariantVector & X )
{
  // printf ("Top of AstQuery<AstQuerySynthesizedAttributeType>::VariantVector copy constructor \n");
  // printVariantVector(*this);

  // if (X.size() > 0)
  insert(end(),X.begin(),X.end());

  // printf ("Bottom of AstQuery<AstQuerySynthesizedAttributeType>::VariantVector copy constructor \n");
  // printVariantVector(*this);
}


  VariantVector
VariantVector::operator= ( const VariantVector & X )
{
  // printf ("Top of AstQuery<AstQuerySynthesizedAttributeType>::VariantVector::operator= (X) \n");
  // printVariantVector(*this);

  erase(begin(),end());
  insert(begin(),X.begin(),X.end());

  // printf ("Bottom of AstQuery<AstQuerySynthesizedAttributeType>::VariantVector::operator= (X) \n");
  // printVariantVector(*this);

  return *this;
}

  VariantVector
operator+ ( VariantT lhs, VariantT rhs )
{
  // printf ("In operator+ (VariantT,VariantT) \n");
  VariantVector returnVector(lhs);
  returnVector.push_back(rhs);
  return returnVector;
}

  VariantVector
operator+ ( VariantT lhs, const VariantVector & rhs )
{
  // printf ("In operator+ (VariantT,VariantVector) \n");
  VariantVector returnVector(lhs);
  returnVector.insert(returnVector.end(),rhs.begin(),rhs.end());
  return returnVector;
}


  VariantVector
operator+ ( const VariantVector & lhs, VariantT rhs )
{
  // printf ("In operator+ (VariantVector,VariantT) \n");
  VariantVector returnVector(lhs);
  returnVector.push_back(rhs);
  return returnVector;
}


  VariantVector
operator+ ( const VariantVector & lhs, const VariantVector & rhs )
{
  // printf ("In operator+ (VariantVector,VariantVector) \n");
  VariantVector returnVector(lhs);
  returnVector.insert(returnVector.end(),rhs.begin(),rhs.end());
  return returnVector;
}

// DQ (4/7/2004): Added to support more general lookup of data in the AST (vector of variants)
  void 
VariantVector::printVariantVector ( const VariantVector & X )
{
  // Supporting function for querySolverGrammarElementFromVariantVector

  printf ("\n\n");
  printf ("In printVariantVector: \n");
  for (vector<VariantT>::const_iterator i = X.begin(); i != X.end(); i++)
  {
    printf ("     node %s \n",getVariantName(*i).c_str());
  }
  printf ("\n\n");
}






namespace AstQueryNamespace{

  void Merge(Rose_STL_Container<int> & mergeWith, Rose_STL_Container<int>  mergeTo ){
    // mergeWith.merge(mergeTo);
    mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
  }

  void Merge(Rose_STL_Container<std::string> & mergeWith, Rose_STL_Container<std::string>  mergeTo ){
    // mergeWith.merge(mergeTo);
    mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
  }

  void Merge(Rose_STL_Container<SgNode*>& mergeWith, Rose_STL_Container<SgNode*>  mergeTo ){
    // mergeWith.merge(mergeTo);
    mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
  }

  void Merge(Rose_STL_Container<SgFunctionDeclaration*>& mergeWith, Rose_STL_Container<SgFunctionDeclaration*>  mergeTo ){;
    // mergeWith.merge(mergeTo);
    mergeWith.insert(mergeWith.end(),mergeTo.begin(),mergeTo.end());
  }

  void Merge(void* mergeWith, void* mergeTo ){}

}
// DQ (12/31/2005): This is OK if not declared in a header file


template<typename AstQuerySynthesizedAttributeType>
struct testFunctionals: public std::unary_function<SgNode*,std::list<AstQuerySynthesizedAttributeType> >{
  //When elementMatchCount==1 then a match has been made
  typedef std::list<AstQuerySynthesizedAttributeType> (*roseFunctionPointerOneParameter)  (SgNode *);
  roseFunctionPointerOneParameter queryFunctionOneParameter;

  testFunctionals(roseFunctionPointerOneParameter function){
    queryFunctionOneParameter=function;
  }
  typename std::list<AstQuerySynthesizedAttributeType>  operator()(SgNode* node) {
    return queryFunctionOneParameter(node);
  }
};

std::list<SgNode*> queryNodeAnonymousTypedef2(SgNode* node)
{
  std::list<SgNode*> returnList;
  ROSE_ASSERT( node     != NULL );

  SgTypedefDeclaration* sageTypedefDeclaration = isSgTypedefDeclaration(node);
  if (sageTypedefDeclaration != NULL)
    if(isSgClassType(sageTypedefDeclaration->get_base_type()))
      returnList.push_back(node);

  return returnList;
} /* End function:queryNodeCLassDeclarationFromName() */

struct testFunctionals2: public std::binary_function<SgNode*,SgNode*, std::list<SgNode*> >{
  int y;
  void setPred(int x){
    y=x;
  } 
  std::list<SgNode*>  operator()(SgNode* node,SgNode* test) const{
    std::list<SgNode*>* x;
    return *x;
  }
};

