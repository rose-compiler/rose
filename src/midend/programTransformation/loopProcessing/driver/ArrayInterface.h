#ifndef ARRAY_INTERFACE_H
#define ARRAY_INTERFACE_H

#include <ArrayAnnot.h>
#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ValuePropagate.h>
#include <LoopTransformInterface.h>

class SgFunctionDefinition;
class ArrayInterface 
  : public AstObserver, public AliasAnalysisInterface, public ArrayAbstractionInterface
{
 private:
  StmtVarAliasCollect aliasCollect;
  ValuePropagate valueCollect;
  std::map <AstNodePtr, int> dimmap;
  std::map <AstNodePtr, SymbolicFunctionDeclarationGroup> lenmap;
  std::map <std::string, ArrayOptDescriptor> optmap;

  void ObserveCopyAst( AstInterfaceImpl& fa, const AstNodePtr& orig, const AstNodePtr& copy);

  virtual bool IsArrayRef( CPPAstInterface& fa, const AstNodePtr& t);
  virtual bool IsArray( CPPAstInterface& fa, const AstNodePtr& s) ;
  virtual bool IsUniqueArray( AstInterface& fa, const AstNodePtr& array);
  virtual bool IsArrayAccess( AstInterface& fa,
                                 const AstNodePtr& s, AstNodePtr* array = 0,
                                 AstInterface::AstNodeList* index = 0) ;
  virtual bool GetArrayBound( AstInterface& fa,
                                 const AstNodePtr& array,
                                 int dim, int &lb, int &ub) ;
  virtual AstNodePtr CreateArrayAccess( AstInterface& fa, const AstNodePtr& arr,
                                const AstNodeList& index);
  virtual SymbolicVal CreateArrayAccess(const SymbolicVal&, const SymbolicVal&);
  bool get_array_opt(CPPAstInterface& fa, const AstNodePtr& array, ArrayOptDescriptor& d);
 public:
  ArrayInterface( ArrayAnnotation& a) : aliasCollect(&a) {}
  void initialize( AstInterface& fa, const AstNodePtr& h);
  //! Expose alias analysis for unit testing
  virtual void analyze( AstInterface& fa, const AstNodePtr& h);
  void observe( AstInterface& fa) ;
  void stop_observe( AstInterface& fa) ;
  bool may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2);

  bool is_array_exp( CPPAstInterface& fa, const AstNodePtr& array, 
                     int *dimp = 0, SymbolicFunctionDeclarationGroup *len = 0, 
                     bool *changeshape = 0);
  void set_array_dimension( const AstNodePtr& arrayexp, int dim);
  bool is_array_mod_op( CPPAstInterface& fa, const AstNodePtr& arrayExp, 
                        AstNodePtr* modArray = 0, int *dimp = 0, 
                        SymbolicFunctionDeclarationGroup *len = 0, SymbolicFunctionDeclarationGroup* elem = 0, 
                        bool *reshape = 0);
  bool is_array_construct_op( CPPAstInterface& fa, const AstNodePtr& arrayExp, 
                              AstInterface::AstNodeList* alias = 0,
                              int *dimp = 0, SymbolicFunctionDeclarationGroup *len = 0, SymbolicFunctionDeclarationGroup* elem = 0);
 
  AstNodePtr impl_array_opt_init( CPPAstInterface& fa, const AstNodePtr& array, const AstNodePtr& scope);
  AstNodePtr impl_access_array_length(  CPPAstInterface& fa, const AstNodePtr& array, int dim,
                                        int plus = 0);
  AstNodePtr impl_access_array_elem(  CPPAstInterface& fa, const AstNodePtr& array, 
                                      const AstInterface::AstNodeList& args );
  AstNodePtr impl_reshape_array(  CPPAstInterface& fa, const AstNodePtr& array, 
                                  AstInterface::AstNodeList& args );
 static ArrayInterface * get_inst( ArrayAnnotation& a, AstInterface& fa, SgFunctionDefinition* funcDef, const AstNodePtr& node );
};

#endif
