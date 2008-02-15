#ifndef ARRAY_INTERFACE_H
#define ARRAY_INTERFACE_H

#include <ArrayAnnot.h>
#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ValuePropagate.h>
#include <LoopTransformInterface.h>

class ArrayInterface 
  : public AstObserver, public AliasAnalysisInterface, public ArrayAbstractionInterface
{
  StmtVarAliasCollect aliasCollect;
  ValuePropagate valueCollect;
  map <AstNodePtr, int> dimmap;
  map <AstNodePtr, SymbolicFunctionDeclarationGroup> lenmap;
  map <string, ArrayOptDescriptor> optmap;
  void ObserveCopyAst( AstInterface& fa, const AstNodePtr& orig, const AstNodePtr& copy);

//virtual Boolean IsArrayRef( AstInterface& fa, const AstNodePtr& t);
  virtual int IsArrayRef( AstInterface& fa, const AstNodePtr& t);
//Boolean IsArray( AstInterface& fa, const AstNodePtr& s) ;
  int IsArray( AstInterface& fa, const AstNodePtr& s) ;
//virtual Boolean IsArrayAccess( AstInterface& fa,
  virtual int IsArrayAccess( AstInterface& fa,
                                 const AstNodePtr& s, AstNodePtr* array = 0,
                                 AstInterface::AstNodeList* index = 0) ;
//virtual Boolean GetArrayBound( AstInterface& fa,
  virtual int GetArrayBound( AstInterface& fa,
                                 const AstNodePtr& array,
                                 int dim, int &lb, int &ub) ;

  virtual AstNodePtr CreateArrayAccess(AstInterface& fa, 
                                       const AstNodePtr& arr, 
                                       AstInterface::AstNodeList& index);
  bool get_array_opt(AstInterface& fa, const AstNodePtr& array, ArrayOptDescriptor& d);
 public:
  ArrayInterface( ArrayAnnotation& a) : aliasCollect(&a) {}
  void initialize( AstInterface& fa, const AstNodePtr& h);
  void observe( AstInterface& fa) ;
  void stop_observe( AstInterface& fa) ;
  bool may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2);

  bool is_array_exp( const AstNodePtr& array, int *dimp = 0, SymbolicFunctionDeclarationGroup *len = 0, 
		     bool *changeshape = 0);
  void set_array_dimension( const AstNodePtr& arrayexp, int dim);
  bool is_array_mod_op( const AstNodePtr& arrayExp, AstNodePtr* modArray = 0, int *dimp = 0, 
			SymbolicFunctionDeclarationGroup *len = 0, SymbolicFunctionDeclarationGroup* elem = 0, 
			bool *reshape = 0);
  bool is_array_construct_op( const AstNodePtr& arrayExp, AstInterface::AstNodeList* alias = 0,
			      int *dimp = 0, SymbolicFunctionDeclarationGroup *len = 0, SymbolicFunctionDeclarationGroup* elem = 0);
 
  AstNodePtr impl_array_opt_init( AstInterface& fa, const AstNodePtr& array, 
				  bool insertinit = false);
  AstNodePtr impl_access_array_length(  AstInterface& fa, const AstNodePtr& array, int dim,
					int plus = 0);
  AstNodePtr impl_access_array_elem(  AstInterface& fa, const AstNodePtr& array, 
				      AstInterface::AstNodeList& args );
  AstNodePtr impl_reshape_array(  AstInterface& fa, const AstNodePtr& array, 
				  AstInterface::AstNodeList& args );
};

#endif
