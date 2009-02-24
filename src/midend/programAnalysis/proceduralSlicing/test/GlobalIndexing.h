#ifndef GLOBALINDEX_IS_DEFINED
#define GLOBALINDEX_IS_DEFINED



/*!
  This AstAttriubute class is for assigning an AST statement node a global index.
*/ 

class GlobalIndex : public AstAttribute{

 public:
  GlobalIndex(int i):global_index(i){};
  void setAttribute(int i){global_index = i;}

 private:
  int global_index;
};
#endif

#ifndef PRAGMAINSERT_IS_DEFINED
#define PRAGMAINSERT_IS_DEFINED



/*!

/class PragmaInsert
This class traveres the AST and collects a list of the statements. All statements are assigned an AstAttribute given the global index of the statement in the statement list.
 */

class GlobalIndexing : public AstSimpleProcessing{
 public:
  list<SgNode*> get_list_of_statements(){return list_of_statements;}
  void set_global_index(int i){global_index=i;}
 protected:
  void virtual visit(SgNode* node);
  
 private:
  list<SgNode*> list_of_statements;
  int global_index;
};
#endif

