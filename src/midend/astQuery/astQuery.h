#ifndef ROSE_AST_QUERY
#define ROSE_AST_QUERY

#include "AstProcessing.h"

#include <functional>
// Support for operations like (SgTypeInt | SgTypeFloat)
// note that non-terminals would be expanded into the associated terminals!
// So SgType would generate (SgTypeInt | SgTypeFloat | SgTypeDouble | ... | <last type>)
//        typedef class NodeQuery::VariantVector VariantVector;
#include "astQueryInheritedAttribute.h"




class VariantVector : public std::vector < VariantT >
   {
  // This class is derived from the STL vector class
     public:
	  VariantVector () {}
	  VariantVector (const VariantT & X);
	  VariantVector (const VariantVector & X);
	  VariantVector operator= (const VariantVector & X);

	  friend VariantVector operator+ (VariantT lhs, VariantT rhs);
	  friend VariantVector operator+ (VariantT lhs, const VariantVector & rhs);
	  friend VariantVector operator+ (const VariantVector & lhs, VariantT rhs);
	  friend VariantVector operator+ (const VariantVector & lhs, const VariantVector & rhs);

	  static void printVariantVector (const VariantVector & X);
   };

// DQ (4/23/2006): g++ 4.1.0 requires friend functions to be declared outside the class as well!
VariantVector operator+ (VariantT lhs, VariantT rhs);
VariantVector operator+ (VariantT lhs, const VariantVector & rhs);
VariantVector operator+ (const VariantVector & lhs, VariantT rhs);
VariantVector operator+ (const VariantVector & lhs, const VariantVector & rhs);





namespace AstQueryNamespace
   {

     template<typename AstQuerySynthesizedAttributeType>
     struct helpFunctionalOneParamater
        : public std::unary_function<SgNode* , Rose_STL_Container<AstQuerySynthesizedAttributeType> >
        {
	    // When elementMatchCount == 1 then a match has been made
          typedef Rose_STL_Container<AstQuerySynthesizedAttributeType> (*roseFunctionPointerOneParameter)  (SgNode *);
          roseFunctionPointerOneParameter queryFunctionOneParameter;

       // Constructor
          helpFunctionalOneParamater (roseFunctionPointerOneParameter function)
             {
               queryFunctionOneParameter = function;
             }

          typename Rose_STL_Container<AstQuerySynthesizedAttributeType>  operator()(SgNode* node)
             {
               return queryFunctionOneParameter(node);
             }
        };

     struct helpF : public std::binary_function<bool,bool,bool >
        {
          bool operator()(bool x, bool y)
             {
               return x;
             }
        };

     template<typename AstQuerySynthesizedAttributeType, typename ArgumentType>
     struct helpFunctionalTwoParamaters
        : public std::binary_function<SgNode*,ArgumentType, Rose_STL_Container<AstQuerySynthesizedAttributeType> >
        {
	    // When elementMatchCount==1 then a match has been made
          typedef Rose_STL_Container<AstQuerySynthesizedAttributeType> (*roseFunctionPointerTwoParameters)  (SgNode *, ArgumentType);
          roseFunctionPointerTwoParameters queryFunctionTwoParameters;

          helpFunctionalTwoParamaters(roseFunctionPointerTwoParameters function)
             {
               queryFunctionTwoParameters = function;
             }
          Rose_STL_Container<AstQuerySynthesizedAttributeType>  operator()(SgNode* node, ArgumentType arg)
             {
               return queryFunctionTwoParameters(node,arg);
             }
        };

     enum QueryDepth
        {
          UnknownListElementTypeQueryDepth = 0,
          ChildrenOnly                     = 1,
          AllNodes                         = 2,
       // DQ (4/8/2004): Added support for extracting types from tranversed 
       //                nodes (types are not generally traversed).
          ExtractTypes                     = 3,
          END_OF_NODE_TYPE_LIST_QUERY_DEPTH
        };

  // #include "variantVector.h"

  // **********************************
  // Prototypes for Variable Node Query
  // **********************************

  // forward declaration
     class AstQueryInheritedAttributeType;


  /*
     template<typename ResultType>
     void Merge(std::list<ResultType> & mergeWith, std::list<ResultType> & mergeTo ){
     mergeWith.merge(mergeTo);
     };
   */
  //void Merge(std::list<SgNode*>& mergeWith, std::list<SgNode*>  mergeTo );

#if 0
     inline void Merge(std::list<SgNode*, std::allocator<SgNode*> >& mergeWith, std::list<SgNode*,  std::allocator<SgNode*> >&  mergeTo ){
	  mergeWith.merge(mergeTo);
     };
#endif
     void Merge(Rose_STL_Container<SgNode*>& mergeWith, Rose_STL_Container<SgNode*>  mergeTo );
     void Merge(Rose_STL_Container<SgFunctionDeclaration*>& mergeWith, Rose_STL_Container<SgFunctionDeclaration*>  mergeTo );
  // void Merge(Rose_STL_Container<bool> & mergeWith, Rose_STL_Container<bool>  mergeTo );
     void Merge(Rose_STL_Container<int> & mergeWith, Rose_STL_Container<int>  mergeTo );
     void Merge(Rose_STL_Container<std::string> & mergeWith, Rose_STL_Container<std::string>  mergeTo );

  // DQ & AS (3/14/2007): Added to support us of astQuery with functions returing void* (not clear why void 
  // does not work, except that void is not really a return type but instead is a signal that there is no 
  // return type).  This is used in the generation of the list of return types (used for testing in AST 
  // Consistancy tests).
     void Merge(void* mergeWith, void* mergeTo );

     template<typename ResultType>
	  void Merge(std::vector<ResultType> & mergeWith, std::vector<ResultType> & mergeTo )
        {
          mergeWith.push_back(mergeTo);
        }

  //typedef std::list<SgNode*> AstQuerySynthesizedAttributeType;

  /*!
    \defgroup nodeQueryLib Node Query Library
    \brief This class simplifies the development of queries on the AST resulting in a 
    list of AST nodes (SgNode pointer values).
    \authors Quinlan, Saebjornsen
    \ingroup subtreeQueryLib

    This class represents a library of queries. Basically it will support a large number of
    different types of queries that can be ask of an AST and that return a list of AST nodes.
   */

  /*! \brief This class simplifies the development of queries on the AST resulting in a 
    list of AST nodes.
    \ingroup nodeQueryLib

    This class used several  member function within it's interface.  Each member function
    takes an AST node pointer (any subtree of the AST).
    This class represents a library of queries.. Basically it will support a large number of
    different types of queries that can be ask of an AST and that return a list of SgNode pointers.
   */
     class DefaultNodeFunctional :  public std::unary_function<SgNode*, Rose_STL_Container<SgNode*> > 
     {
       public:
	    result_type operator()(SgNode* node ) 
	       { 
		 result_type returnType;
		 returnType.push_back(node);
		 return returnType; 
	       }
     };


     class AstQuery_DUMMY{};

  /*******************************************************
   * The class
   *    class AstQuery
   * traverses the memory pool and performs the action
   * specified in a functional on every node. The return
   *  value is a list of SgNode*.
   *****************************************************/
     template<typename AST_Query_Base = AstQuery_DUMMY, typename NodeFunctional = DefaultNodeFunctional>
	     class AstQuery : public AST_Query_Base
			   //public ROSE_VisitTraversal, public AstSimpleProcessing
	     {

	    //Instantiate the functional which returns the list of nodes to be processed
	       NodeFunctional* nodeFunc;

	    //When a node satisfies a functional it is added to
	    //this list.
	       typedef typename NodeFunctional::result_type AstQueryReturnType;
	       AstQueryReturnType listOfNodes;
	       public:
	       AstQuery();

	       AstQuery(NodeFunctional* funct);

	       virtual ~AstQuery();

	    //set the node Functional
	       void setPredicate (NodeFunctional* funct);


	    //get the result from the query
	       AstQueryReturnType get_listOfNodes();



	    //clear the result list of the query. a new
	    //query can then be restarted using the same object.
	       void clear_listOfNodes();

	    //visitor function will apply the functional to every node
	    //and add the result to the listOfNodes
	       void visit(SgNode *node);
	     };

     template<typename AST_Query_Base, typename NodeFunctional>
	     AstQuery<AST_Query_Base,NodeFunctional>::AstQuery(NodeFunctional* funct)  
		{
		  nodeFunc = funct;
		}

     template<typename AST_Query_Base, typename NodeFunctional>
	     AstQuery<AST_Query_Base,NodeFunctional>::AstQuery()  
		{
		  nodeFunc = new DefaultNodeFunctional();
		}

     template<typename AST_Query_Base, typename NodeFunctional>
	     void 
	     AstQuery<AST_Query_Base,NodeFunctional>::setPredicate (NodeFunctional* funct)
		{
		  nodeFunc = funct;
		}



     template<typename AST_Query_Base, typename NodeFunctional>
	     AstQuery<AST_Query_Base,NodeFunctional>::~AstQuery(){};

     template<typename AST_Query_Base, typename NodeFunctional>
	     typename AstQuery<AST_Query_Base,NodeFunctional>::AstQueryReturnType
	     AstQuery<AST_Query_Base,NodeFunctional>::get_listOfNodes(){
		  return listOfNodes;
	     };

     template<typename AST_Query_Base, typename NodeFunctional>
	     void
	     AstQuery<AST_Query_Base,NodeFunctional>::clear_listOfNodes(){
		  listOfNodes.clear();

	     };

     template<typename AST_Query_Base, typename NodeFunctional>
	     void 
	     AstQuery<AST_Query_Base,NodeFunctional>::visit(SgNode *node) {
		  ROSE_ASSERT (node != NULL);
		  ROSE_ASSERT (nodeFunc != NULL); 

#if DEBUG_NODEQUERY
		  printf ("%%%%% TOP of evaluateSynthesizedAttribute (astNode->sage_class_name() = %s,synthesizedAttributeList.size() = %d) \n",
				  astNode->sage_class_name(), synthesizedAttributeList.size());
#endif

	       // This function assemble the elements of the input list (a list of lists) to form the output (a single list)
             AstQueryNamespace::Merge(listOfNodes, nodeFunc->operator()(node));
	     }




  /********************************************************************************
   * The function
   *      std::list<ListElement> querySubTree<ListElement>(SgNode* node, Predicate& _pred)
   * will query the subtree of the IR node in the first argument for nodes satisfying
   * the criteria specified in and returned by the predicate in the second argument.
   ********************************************************************************/
     template<typename NodeFunctional>
	     typename NodeFunctional::result_type 
	     querySubTree(SgNode* node, NodeFunctional nodeFunc, AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes,
			     t_traverseOrder treeTraversalOrder = preorder)
		{
		  ROSE_ASSERT(node!=NULL);

		  AstQuery<AstSimpleProcessing,NodeFunctional> astQuery(&nodeFunc);

		  switch (defineQueryType)
		     {
		       case AstQueryNamespace::AllNodes:
			     {
			       astQuery.traverse(node,treeTraversalOrder);

			       break;
			     }
		       case AstQueryNamespace::ChildrenOnly:
			     {
			    //visit only the nodes which is pointed to by this class
			       typedef std::vector< std::pair<SgNode *, std::string > > DataMemberPointerType;

			       DataMemberPointerType returnData = node->returnDataMemberPointers ();

			    // A child of a node is the nodes it points to.
			       for(DataMemberPointerType::iterator i = returnData.begin(); i != returnData.end(); ++i)
				  {
				 // visit the node which is pointed to by this SgNode
				    if( i->first != NULL )
					 astQuery.visit(i->first);

				  };


			       break;
			     }
		       case AstQueryNamespace::ExtractTypes:
			  printf ("Sorry, ExtractTypes case not implemented for e_OneParameterFunctionClassification! \n");
			  ROSE_ABORT();
			  break;
		       default:
			  printf ("default reached in switch (queryType) \n");
			  ROSE_ABORT();
		     }

		  return astQuery.get_listOfNodes();
		}


  /********************************************************************************
   * The function
   *      _Result querySubTree ( SgNode * subTree,
   *                   _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
   *                   AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes )
   ** will query the subtree of the IR node in the first argument for nodes satisfying
   * the criteria specified in and returned by the function pointer in the second argument.
   ********************************************************************************/
     template <class _Arg, class _Result> 
	     _Result querySubTree ( SgNode * subTree,
			     _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
			     AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes ){
		  return querySubTree(subTree,std::bind2nd(std::ptr_fun(__x),x_arg),defineQueryType);
	     }

  /********************************************************************************
   * The function
   *      _Result querySubTree ( SgNode * subTree,
   *                   _Result (*__x)(SgNode*),
   *                   AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes )
   ** will query the subtree of the IR node in the first argument for nodes satisfying
   * the criteria specified in and returned by the function pointer in the second argument.
   ********************************************************************************/
     template <class _Result> 
	     _Result querySubTree ( SgNode * subTree,
			     _Result (*__x)(SgNode*),
			     AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes ){
		  return querySubTree(subTree,std::ptr_fun(__x),defineQueryType);
	     }

  /********************************************************************************
   * The function
   * _Result queryRange(typename _Result::const_iterator& begin, typename _Result::const_iterator& end,
   *                   Predicate _pred)
   * will query the iterator _Result::const_iterator from 'begin' to 'end' for IR nodes satisfying
   * the criteria specified in and returned by the predicate in the third argument.
   ********************************************************************************/
     template <class Iterator, class NodeFunctional>
	     typename NodeFunctional::result_type 
	     queryRange(Iterator begin, Iterator end, 
			     NodeFunctional nodeFunc){

		  AstQuery<AstQuery_DUMMY,NodeFunctional> astQuery(&nodeFunc);

		  for(; begin!=end; ++begin){
		       astQuery.visit(*begin);
		  }

		  return astQuery.get_listOfNodes();
	     }

  /********************************************************************************
   * The function
   * _Result queryRange(typename _Result::const_iterator& begin, typename _Result::const_iterator& end,
   *                   _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
   *                   AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes )
   * will query the iterator _Result::const_iterator from 'begin' to 'end' for IR nodes satisfying
   * the criteria specified in and returned by the function pointer in the third argument given the
   * fourth argument 'x_arg'.
   ********************************************************************************/
     template <class _Arg, class _Result> 
	     _Result queryRange ( typename _Result::iterator begin, const typename _Result::iterator end,
			     _Result (*__x)(SgNode*,_Arg), _Arg x_arg){
		  return queryRange(begin,end,std::bind2nd(std::ptr_fun(__x),x_arg));
	     }

  /********************************************************************************
   * The function
   * _Result queryRange(typename _Result::const_iterator& begin, typename _Result::const_iterator& end,
   *                   _Result (*__x)(SgNode*), 
   *                   AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes )
   * will query the iterator _Result::const_iterator from 'begin' to 'end' for IR nodes satisfying
   * the criteria specified in and returned by the function pointer in the third argument.
   ********************************************************************************/
     template <class _Result> 
	     _Result queryRange (typename _Result::iterator begin, typename _Result::iterator end,
			     _Result (*__x)(SgNode*)){
		  return queryRange(begin,end,std::ptr_fun(__x));
	     }

  /****************************************************************************
   * The function
   *  void queryMemoryPool(ROSE_VisitTraversal& astQuery, VariantVector* variantsToTraverse);
   * traverses the parts of the memory pool which has corresponding variants
   * in VariantVector.
   ***************************************************************************/

     template <class FunctionalType> 
     void queryMemoryPool(AstQuery<ROSE_VisitTraversal,FunctionalType>& astQuery, VariantVector* variantsToTraverse);

  /********************************************************************************
   * The function
   *      std::list<ListElement> querySubTree<ListElement>(SgNode* node, Predicate& _pred)
   * will query the subtree of the IR node in the first argument for nodes satisfying
   * the criteria specified in and returned by the predicate in the second argument.
   ********************************************************************************/
     template<typename NodeFunctional>
	     typename NodeFunctional::result_type 
	     queryMemoryPool(NodeFunctional nodeFunc , VariantVector* targetVariantVector = NULL)
		{

		  AstQuery<ROSE_VisitTraversal,NodeFunctional> astQuery(&nodeFunc);
		  if(targetVariantVector == NULL){
		    //Query the whole memory pool
		       astQuery.traverseMemoryPool();
		  }else{
		       queryMemoryPool<NodeFunctional>(astQuery, targetVariantVector);
		  }; // end if-else



		  return astQuery.get_listOfNodes();
		}


  /********************************************************************************
   * The function
   *      _Result querySubTree ( SgNode * subTree,
   *                   _Result (*__x)(SgNode*,_Arg), _Arg x_arg,
   *                   AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes ){
   ** will query the subtree of the IR node in the first argument for nodes satisfying
   * the criteria specified in and returned by the function pointer in the second argument.
   ********************************************************************************/
template <class _Arg, class _Result> 
_Result queryMemoryPool ( 
		_Result (*__x)(SgNode*,_Arg), _Arg x_arg,
		VariantVector* targetVariantVector = NULL){
     return queryMemoryPool(std::bind2nd(std::ptr_fun(__x),x_arg),targetVariantVector);
}

/********************************************************************************
 * The function
 *      _Result querySubTree ( SgNode * subTree,
 *                   _Result (*__x)(SgNode*),
 *                   AstQueryNamespace::QueryDepth defineQueryType = AstQueryNamespace::AllNodes ){
 ** will query the subtree of the IR node in the first argument for nodes satisfying
 * the criteria specified in and returned by the function pointer in the second argument.
 ********************************************************************************/
template <class _Result> 
_Result queryMemoryPool (
		_Result (*__x)(SgNode*),
		VariantVector* targetVariantVector = NULL ){
     return queryMemoryPool(std::ptr_fun(__x),targetVariantVector);
}




};// END NAMESPACE ASTQUERY


#include "AstQueryMemoryPool.h"

#if 0
// This function has to be defined in a header file because it is a template function and must be readily available.
template <class FunctionalType> 
void
AstQueryNamespace::queryMemoryPool(AstQuery<ROSE_VisitTraversal,FunctionalType>& astQuery, VariantVector* variantsToTraverse)
   {
  // Query only the parts of the memory pool corresponding to the variants in the VariantVector
  // The VariantVector stores a vector of types of IR nodes (variants) an each relates to a 
  // separate memory pool which will be searched.
     for (VariantVector::iterator it = variantsToTraverse->begin(); it != variantsToTraverse->end(); ++it)
        {
          switch(*it)
          {
	       case V_SgModifier:
		     { 
		       SgModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBitAttribute:
		     { 
		       SgBitAttribute::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAttribute:
		     { 
		       SgAttribute::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgSupport:
		     { 
		       SgSupport::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPartialFunctionType:
		     { 
		       SgPartialFunctionType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMemberFunctionType:
		     { 
		       SgMemberFunctionType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionType:
		     { 
		       SgFunctionType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPointerType:
		     { 
		       SgPointerType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNamedType:
		     { 
		       SgNamedType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgType:
		     { 
		       SgType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgInitializer:
		     { 
		       SgInitializer::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnaryOp:
		     { 
		       SgUnaryOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBinaryOp:
		     { 
		       SgBinaryOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgValueExp:
		     { 
		       SgValueExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgExpression:
		     { 
		       SgExpression::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgClassDeclaration:
		     { 
		       SgClassDeclaration::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgClassDefinition:
		     { 
		       SgClassDefinition::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgScopeStatement:
		     { 
		       SgScopeStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMemberFunctionDeclaration:
		     { 
		       SgMemberFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionDeclaration:
		     { 
		       SgFunctionDeclaration::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDeclarationStatement:
		     { 
		       SgDeclarationStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgStatement:
		     { 
		       SgStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionSymbol:
		     { 
		       SgFunctionSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgSymbol:
		     { 
		       SgSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLocatedNode:
		     { 
		       SgLocatedNode::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNode:
		     { 
		       SgNode::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgSymbolTable:
		     { 
		       SgSymbolTable::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgInitializedName:
		     { 
		       SgInitializedName::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgName:
		     { 
		       SgName::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPragma:
		     { 
		       SgPragma::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgModifierNodes:
		     { 
		       SgModifierNodes::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgConstVolatileModifier:
		     { 
		       SgConstVolatileModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgStorageModifier:
		     { 
		       SgStorageModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAccessModifier:
		     { 
		       SgAccessModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionModifier:
		     { 
		       SgFunctionModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUPC_AccessModifier:
		     { 
		       SgUPC_AccessModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLinkageModifier:
		     { 
		       SgLinkageModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgSpecialFunctionModifier:
		     { 
		       SgSpecialFunctionModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeModifier:
		     { 
		       SgTypeModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgElaboratedTypeModifier:
		     { 
		       SgElaboratedTypeModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBaseClassModifier:
		     { 
		       SgBaseClassModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDeclarationModifier:
		     { 
		       SgDeclarationModifier::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_Sg_File_Info:
		     { 
		       Sg_File_Info::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFile:
		     { 
		       SgFile::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFileList:
		     { 
		       SgFileList::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDirectory:
		     { 
		       SgDirectory::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDirectoryList:
		     { 
		       SgDirectoryList::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgProject:
		     { 
		       SgProject::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgOptions:
		     { 
		       SgOptions::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnparse_Info:
		     { 
		       SgUnparse_Info::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFuncDecl_attr:
		     { 
		       SgFuncDecl_attr::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgClassDecl_attr:
		     { 
		       SgClassDecl_attr::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypedefSeq:
		     { 
		       SgTypedefSeq::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionParameterTypeList:
		     { 
		       SgFunctionParameterTypeList::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateParameter:
		     { 
		       SgTemplateParameter::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateArgument:
		     { 
		       SgTemplateArgument::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBaseClass:
		     { 
		       SgBaseClass::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgQualifiedName:
		     { 
		       SgQualifiedName::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeUnknown:
		     { 
		       SgTypeUnknown::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeChar:
		     { 
		       SgTypeChar::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeSignedChar:
		     { 
		       SgTypeSignedChar::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeUnsignedChar:
		     { 
		       SgTypeUnsignedChar::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeShort:
		     { 
		       SgTypeShort::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeSignedShort:
		     { 
		       SgTypeSignedShort::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeUnsignedShort:
		     { 
		       SgTypeUnsignedShort::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeInt:
		     { 
		       SgTypeInt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeSignedInt:
		     { 
		       SgTypeSignedInt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeUnsignedInt:
		     { 
		       SgTypeUnsignedInt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeLong:
		     { 
		       SgTypeLong::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeSignedLong:
		     { 
		       SgTypeSignedLong::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeUnsignedLong:
		     { 
		       SgTypeUnsignedLong::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeVoid:
		     { 
		       SgTypeVoid::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeGlobalVoid:
		     { 
		       SgTypeGlobalVoid::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeWchar:
		     { 
		       SgTypeWchar::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeFloat:
		     { 
		       SgTypeFloat::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeDouble:
		     { 
		       SgTypeDouble::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeLongLong:
		     { 
		       SgTypeLongLong::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeUnsignedLongLong:
		     { 
		       SgTypeUnsignedLongLong::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeLongDouble:
		     { 
		       SgTypeLongDouble::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeString:
		     { 
		       SgTypeString::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeBool:
		     { 
		       SgTypeBool::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeComplex:
		     { 
		       SgTypeComplex::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeImaginary:
		     { 
		       SgTypeImaginary::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeDefault:
		     { 
		       SgTypeDefault::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPointerMemberType:
		     { 
		       SgPointerMemberType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgReferenceType:
		     { 
		       SgReferenceType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgClassType:
		     { 
		       SgClassType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateType:
		     { 
		       SgTemplateType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgEnumType:
		     { 
		       SgEnumType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypedefType:
		     { 
		       SgTypedefType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgModifierType:
		     { 
		       SgModifierType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPartialFunctionModifierType:
		     { 
		       SgPartialFunctionModifierType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgArrayType:
		     { 
		       SgArrayType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeEllipse:
		     { 
		       SgTypeEllipse::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnknownMemberFunctionType:
		     { 
		       SgUnknownMemberFunctionType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgQualifiedNameType:
		     { 
		       SgQualifiedNameType::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgExprListExp:
		     { 
		       SgExprListExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVarRefExp:
		     { 
		       SgVarRefExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgClassNameRefExp:
		     { 
		       SgClassNameRefExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionRefExp:
		     { 
		       SgFunctionRefExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMemberFunctionRefExp:
		     { 
		       SgMemberFunctionRefExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionCallExp:
		     { 
		       SgFunctionCallExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgSizeOfOp:
		     { 
		       SgSizeOfOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVarArgStartOp:
		     { 
		       SgVarArgStartOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVarArgStartOneOperandOp:
		     { 
		       SgVarArgStartOneOperandOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVarArgOp:
		     { 
		       SgVarArgOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVarArgEndOp:
		     { 
		       SgVarArgEndOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVarArgCopyOp:
		     { 
		       SgVarArgCopyOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypeIdOp:
		     { 
		       SgTypeIdOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgConditionalExp:
		     { 
		       SgConditionalExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNewExp:
		     { 
		       SgNewExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDeleteExp:
		     { 
		       SgDeleteExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgThisExp:
		     { 
		       SgThisExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgRefExp:
		     { 
		       SgRefExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAggregateInitializer:
		     { 
		       SgAggregateInitializer::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgConstructorInitializer:
		     { 
		       SgConstructorInitializer::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAssignInitializer:
		     { 
		       SgAssignInitializer::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgExpressionRoot:
		     { 
		       SgExpressionRoot::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMinusOp:
		     { 
		       SgMinusOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnaryAddOp:
		     { 
		       SgUnaryAddOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNotOp:
		     { 
		       SgNotOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPointerDerefExp:
		     { 
		       SgPointerDerefExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAddressOfOp:
		     { 
		       SgAddressOfOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMinusMinusOp:
		     { 
		       SgMinusMinusOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPlusPlusOp:
		     { 
		       SgPlusPlusOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBitComplementOp:
		     { 
		       SgBitComplementOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgCastExp:
		     { 
		       SgCastExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgThrowOp:
		     { 
		       SgThrowOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgArrowExp:
		     { 
		       SgArrowExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDotExp:
		     { 
		       SgDotExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDotStarOp:
		     { 
		       SgDotStarOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgArrowStarOp:
		     { 
		       SgArrowStarOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgEqualityOp:
		     { 
		       SgEqualityOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLessThanOp:
		     { 
		       SgLessThanOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgGreaterThanOp:
		     { 
		       SgGreaterThanOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNotEqualOp:
		     { 
		       SgNotEqualOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLessOrEqualOp:
		     { 
		       SgLessOrEqualOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgGreaterOrEqualOp:
		     { 
		       SgGreaterOrEqualOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAddOp:
		     { 
		       SgAddOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgSubtractOp:
		     { 
		       SgSubtractOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMultiplyOp:
		     { 
		       SgMultiplyOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDivideOp:
		     { 
		       SgDivideOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgIntegerDivideOp:
		     { 
		       SgIntegerDivideOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgModOp:
		     { 
		       SgModOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAndOp:
		     { 
		       SgAndOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgOrOp:
		     { 
		       SgOrOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBitXorOp:
		     { 
		       SgBitXorOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBitAndOp:
		     { 
		       SgBitAndOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBitOrOp:
		     { 
		       SgBitOrOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgCommaOpExp:
		     { 
		       SgCommaOpExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLshiftOp:
		     { 
		       SgLshiftOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgRshiftOp:
		     { 
		       SgRshiftOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPntrArrRefExp:
		     { 
		       SgPntrArrRefExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgScopeOp:
		     { 
		       SgScopeOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAssignOp:
		     { 
		       SgAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPlusAssignOp:
		     { 
		       SgPlusAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMinusAssignOp:
		     { 
		       SgMinusAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAndAssignOp:
		     { 
		       SgAndAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgIorAssignOp:
		     { 
		       SgIorAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMultAssignOp:
		     { 
		       SgMultAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDivAssignOp:
		     { 
		       SgDivAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgModAssignOp:
		     { 
		       SgModAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgXorAssignOp:
		     { 
		       SgXorAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLshiftAssignOp:
		     { 
		       SgLshiftAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgRshiftAssignOp:
		     { 
		       SgRshiftAssignOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBoolValExp:
		     { 
		       SgBoolValExp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgStringVal:
		     { 
		       SgStringVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgShortVal:
		     { 
		       SgShortVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgCharVal:
		     { 
		       SgCharVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnsignedCharVal:
		     { 
		       SgUnsignedCharVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgWcharVal:
		     { 
		       SgWcharVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnsignedShortVal:
		     { 
		       SgUnsignedShortVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgIntVal:
		     { 
		       SgIntVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgEnumVal:
		     { 
		       SgEnumVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnsignedIntVal:
		     { 
		       SgUnsignedIntVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLongIntVal:
		     { 
		       SgLongIntVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLongLongIntVal:
		     { 
		       SgLongLongIntVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnsignedLongLongIntVal:
		     { 
		       SgUnsignedLongLongIntVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUnsignedLongVal:
		     { 
		       SgUnsignedLongVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFloatVal:
		     { 
		       SgFloatVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDoubleVal:
		     { 
		       SgDoubleVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLongDoubleVal:
		     { 
		       SgLongDoubleVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgComplexVal:
		     { 
		       SgComplexVal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNullExpression:
		     { 
		       SgNullExpression::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVariantExpression:
		     { 
		       SgVariantExpression::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgStatementExpression:
		     { 
		       SgStatementExpression::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAsmOp:
		     { 
		       SgAsmOp::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgForStatement:
		     { 
		       SgForStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgForInitStatement:
		     { 
		       SgForInitStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgCatchStatementSeq:
		     { 
		       SgCatchStatementSeq::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionParameterList:
		     { 
		       SgFunctionParameterList::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgCtorInitializerList:
		     { 
		       SgCtorInitializerList::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBasicBlock:
		     { 
		       SgBasicBlock::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgGlobal:
		     { 
		       SgGlobal::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgIfStmt:
		     { 
		       SgIfStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionDefinition:
		     { 
		       SgFunctionDefinition::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgWhileStmt:
		     { 
		       SgWhileStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDoWhileStmt:
		     { 
		       SgDoWhileStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgSwitchStatement:
		     { 
		       SgSwitchStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgCatchOptionStmt:
		     { 
		       SgCatchOptionStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVariableDeclaration:
		     { 
		       SgVariableDeclaration::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVariableDefinition:
		     { 
		       SgVariableDefinition::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgEnumDeclaration:
		     { 
		       SgEnumDeclaration::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgAsmStmt:
		     { 
		       SgAsmStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypedefDeclaration:
		     { 
		       SgTypedefDeclaration::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionTypeTable:
		     { 
		       SgFunctionTypeTable::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgExprStatement:
		     { 
		       SgExprStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLabelStatement:
		     { 
		       SgLabelStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgCaseOptionStmt:
		     { 
		       SgCaseOptionStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTryStmt:
		     { 
		       SgTryStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDefaultOptionStmt:
		     { 
		       SgDefaultOptionStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgBreakStmt:
		     { 
		       SgBreakStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgContinueStmt:
		     { 
		       SgContinueStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgReturnStmt:
		     { 
		       SgReturnStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgGotoStatement:
		     { 
		       SgGotoStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgSpawnStmt:
		     { 
		       SgSpawnStmt::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNullStatement:
		     { 
		       SgNullStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVariantStatement:
		     { 
		       SgVariantStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgPragmaDeclaration:
		     { 
		       SgPragmaDeclaration::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateDeclaration:
		     { 
		       SgTemplateDeclaration::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateInstantiationDecl:
		     { 
		       SgTemplateInstantiationDecl::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateInstantiationDefn:
		     { 
		       SgTemplateInstantiationDefn::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateInstantiationFunctionDecl:
		     { 
		       SgTemplateInstantiationFunctionDecl::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateInstantiationMemberFunctionDecl:
		     { 
		       SgTemplateInstantiationMemberFunctionDecl::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgClinkageStartStatement:
		     { 
		       SgClinkageStartStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNamespaceDeclarationStatement:
		     { 
		       SgNamespaceDeclarationStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNamespaceAliasDeclarationStatement:
		     { 
		       SgNamespaceAliasDeclarationStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNamespaceDefinitionStatement:
		     { 
		       SgNamespaceDefinitionStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUsingDeclarationStatement:
		     { 
		       SgUsingDeclarationStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgUsingDirectiveStatement:
		     { 
		       SgUsingDirectiveStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateInstantiationDirectiveStatement:
		     { 
		       SgTemplateInstantiationDirectiveStatement::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgVariableSymbol:
		     { 
		       SgVariableSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgFunctionTypeSymbol:
		     { 
		       SgFunctionTypeSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgClassSymbol:
		     { 
		       SgClassSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTemplateSymbol:
		     { 
		       SgTemplateSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgEnumSymbol:
		     { 
		       SgEnumSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgEnumFieldSymbol:
		     { 
		       SgEnumFieldSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgTypedefSymbol:
		     { 
		       SgTypedefSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgMemberFunctionSymbol:
		     { 
		       SgMemberFunctionSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgLabelSymbol:
		     { 
		       SgLabelSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgDefaultSymbol:
		     { 
		       SgDefaultSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

	       case V_SgNamespaceSymbol:
		     { 
		       SgNamespaceSymbol::traverseMemoryPoolNodes(astQuery);
		       break;
		     }

     /*  case V_SgNumVariants:
           {
             SgNumVariants::traverseMemoryPoolNodes(astQuery);
             break;
           }
		*/

	       default:
            // This is a common error after adding a new IR node (because this function should have been automatically generated).
               printf ("Error: default reached in astQuery.h template function AstQueryNamespace::queryMemoryPool(): (*it) = %d = %s \n",*it,Cxx_GrammarTerminalNames[*it].name.c_str());
               std::cout << "Case not implemented in queryMemoryPool(..). Exiting." << std::endl;
               ROSE_ASSERT(false);
               break;
          } // end switch-case
        } // end for-loop
   }
#endif

// endif for ROSE_NAME_QUERY
#endif
