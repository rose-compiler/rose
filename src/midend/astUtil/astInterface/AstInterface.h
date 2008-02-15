
#ifndef AST_TREE_INTERFACE_H
#define AST_TREE_INTERFACE_H

#include <iostream>
#include <list>
#include <ObserveObject.h>

#define STD std::

class AstInterfaceImpl;
class AstNodePtr {
 protected:
  void* repr;
 public:
  AstNodePtr() : repr(0) {}
  AstNodePtr( const AstNodePtr& that) : repr(that.repr) {}
  AstNodePtr& operator = (const AstNodePtr &that) 
      { repr = that.repr; return *this; }
  bool operator != (const AstNodePtr &that) const
    { return repr != that.repr; }
  bool operator == (const AstNodePtr &that) const
    { return repr == that.repr; }
  bool operator == (void *p) const
    { return repr == p; }
  bool operator != (void *p) const
    { return repr != p; }
  bool operator < (const AstNodePtr &that) const
    { return repr < that.repr; }
  ~AstNodePtr() {}
  void * get_ptr() const { return repr; }
};
#define AST_NULL AstNodePtr()

class AstNodeType {
 protected:
  void* repr;
 public:
  AstNodeType() : repr(0) {}
  AstNodeType( const AstNodeType& that) : repr(that.repr) {}
  AstNodeType& operator = (const AstNodeType &that) 
      { repr = that.repr; return *this; }
  ~AstNodeType() {}
  void * get_ptr() const { return repr; }
};

STD string AstToString( const AstNodePtr& s);

class AstObserver {
  public:
   virtual void ObserveCopyAst( AstInterfaceImpl& fa, const AstNodePtr& orig, const AstNodePtr& n) = 0;
};

class CopyAstRecord : public ObserveInfo< AstObserver>
{
  AstNodePtr orig, n;
  AstInterfaceImpl& fa;
 public:
  CopyAstRecord(AstInterfaceImpl& _fa, const AstNodePtr& o, const AstNodePtr& _n) 
      : fa(_fa), orig(o), n(_n) {}
  virtual void UpdateObserver( AstObserver& o) const 
         {  o.ObserveCopyAst(fa, orig, n); }
};
 

class SymbolicVal;
class SymbolicVar;
class AstInterface 
{
 protected:
  AstInterfaceImpl *impl;
 public:
  AstInterface( AstInterfaceImpl* _impl) : impl(_impl) {}
  ~AstInterface() {}
  AstInterfaceImpl* get_impl() { return impl; }

  typedef enum {OP_NONE, 
           UOP_MINUS, UOP_ADDR, UOP_DEREF, UOP_ALLOCATE, UOP_NOT,
           UOP_CAST, UOP_INCR1, UOP_DECR1,
           BOP_DOT_ACCESS, BOP_ARROW_ACCESS, 
           BOP_TIMES, BOP_DIVIDE, BOP_PLUS, BOP_MINUS, 
           BOP_EQ, BOP_LE, BOP_LT, BOP_NE, BOP_GT, BOP_GE, 
           BOP_AND, BOP_OR,
           BOP_BIT_AND,BOP_BIT_OR, BOP_BIT_RSHIFT, BOP_BIT_LSHIFT,
           OP_ARRAY_ACCESS} OperatorEnum;

  AstNodePtr GetRoot() const;
  AstNodePtr getNULL() const { return AstNodePtr(); }
  void SetRoot( const AstNodePtr& root);

  typedef enum { PreOrder, PostOrder, ReversePreOrder, ReversePostOrder, 
                 PreAndPostOrder } TraversalOrderType;
  typedef enum {PreVisit, PostVisit} TraversalVisitType;
  typedef STD list<AstNodePtr>  AstNodeList;
  typedef STD list<AstNodeType> AstTypeList;

  void AttachObserver(AstObserver* ob);
  void DetachObserver(AstObserver* ob);

  bool get_fileInfo(const AstNodePtr& n, std:: string* fname= 0, int* lineno = 0);

  void InsertStmt( const AstNodePtr& orig, const AstNodePtr& n, 
                   bool before = true, bool extractFromBlock = false);
  void InsertAnnot( const AstNodePtr& n, const std::string& annot, 
                   bool before = true);
  bool ReplaceAst( const AstNodePtr& orig, const AstNodePtr& n);
  bool RemoveStmt( const AstNodePtr& n);
  void FreeAstTree( const AstNodePtr& n);
  AstNodePtr CopyAstTree( const AstNodePtr& n);

  AstNodePtr GetParent( const AstNodePtr &n);
  AstNodePtr GetPrevStmt( const AstNodePtr& s);
  AstNodePtr GetNextStmt( const AstNodePtr& s);
  AstNodeList GetChildrenList( const AstNodePtr &n);

  bool IsDecls( const AstNodePtr& s) ;
  bool IsVariableDecl( const AstNodePtr& exp, AstNodeList* vars = 0,
                                 AstNodeList* inits = 0);
  bool IsExecutableStmt( const AstNodePtr& s) ;
  bool IsStatement( const AstNodePtr& s);

  bool IsBlock( const AstNodePtr& exp);
  AstNodeList GetBlockStmtList( const AstNodePtr& n);
  AstNodePtr GetBlockFirstStmt( const AstNodePtr& n);
  AstNodePtr GetBlockLastStmt( const AstNodePtr& n);
  int GetBlockSize( const AstNodePtr& n);
  AstNodePtr CreateBlock( const AstNodePtr& orig = AstNodePtr()) ;
  void BlockAppendStmt( AstNodePtr& b, const AstNodePtr& s);
  void BlockPrependStmt( AstNodePtr& b, const AstNodePtr& s);
  
  bool IsLoop( const AstNodePtr& s, 
                          AstNodePtr* init=0, AstNodePtr* cond=0,
                         AstNodePtr* incr = 0, AstNodePtr* body = 0) ;
  bool IsPostTestLoop( const AstNodePtr& s);
  bool IsFortranLoop( const AstNodePtr& s, AstNodePtr* ivar = 0,
                       AstNodePtr* lb = 0, AstNodePtr* ub=0,
                       AstNodePtr* step =0, AstNodePtr* body=0);
  AstNodePtr CreateLoop( const AstNodePtr& cond, const AstNodePtr& body); 
  AstNodePtr CreateLoop( const AstNodePtr& ivar, const AstNodePtr& lb, 
                         const AstNodePtr& ub, const AstNodePtr& step, 
                         const AstNodePtr& stmts, bool negativeStep);

  bool IsIf( const AstNodePtr& s, AstNodePtr* cond = 0, 
                       AstNodePtr* truebody = 0, AstNodePtr* falsebody = 0);
  AstNodePtr CreateIf( const AstNodePtr& cond, const AstNodePtr& stmts) ;

  bool IsGoto( const AstNodePtr& s, AstNodePtr* dest = 0);
  bool IsGotoBefore( const AstNodePtr& s); // goto the point before destination
  bool IsGotoAfter( const AstNodePtr& s); // goto the point after destination
  bool IsLabelStatement( const AstNodePtr& s);
  bool IsReturn(const AstNodePtr& s, AstNodePtr* val=0);

  bool GetFunctionCallSideEffect( const AstNodePtr& fc,  // the most conservative estimation
                     CollectObject<AstNodePtr>& collectmod,  // of function side effect
                     CollectObject<AstNodePtr>& collectread);
  bool IsFunctionCall( const AstNodePtr& s, AstNodePtr* f = 0, 
                       AstNodeList* args = 0, AstNodeList* outargs = 0, 
                       AstTypeList* paramtypes = 0, AstNodeType* returntype=0);
  bool IsMin(const AstNodePtr& exp);
  bool IsMax(const AstNodePtr& exp);
  AstNodePtr CreateFunctionCall(const STD string& func, const AstNodeList& args);
  AstNodePtr CreateFunctionCall(const AstNodePtr& func, const AstNodeList& args);

  AstNodePtr GetFunctionDefinition( const AstNodePtr &n, std::string* name=0);
  bool IsFunctionDefinition(  const AstNodePtr& s, STD string* name = 0,
                    AstNodeList* params = 0, AstNodeList* outpars = 0,
                    AstNodePtr* body = 0,
                    AstTypeList* paramtypes = 0, AstNodeType* returntype=0);

  bool IsAssignment( const AstNodePtr& s, AstNodePtr* lhs = 0, 
                               AstNodePtr* rhs = 0, bool* readlhs = 0); 
  AstNodePtr CreateAssignment( const AstNodePtr& lhs, const AstNodePtr& rhs);

  bool IsIOInputStmt( const AstNodePtr& s, AstNodeList* varlist = 0);
  bool IsIOOutputStmt( const AstNodePtr& s, AstNodeList* explist = 0);

  bool IsMemoryAccess( const AstNodePtr& s);
  AstNodePtr IsExpression( const AstNodePtr& s, AstNodeType* exptype =0);
  AstNodeType GetExpressionType( const AstNodePtr& s);

  bool IsConstInt( const AstNodePtr& exp, int* value = 0) ;
  AstNodePtr CreateConstInt( int val)  ;

  bool IsConstant( const AstNodePtr& exp, STD string* valtype=0, STD string* value = 0) ;
  AstNodePtr CreateConstant( const STD string& valtype, const STD string& val);

  bool IsVarRef( const AstNodePtr& exp, AstNodeType* vartype = 0,
                   STD string* varname = 0, AstNodePtr* scope = 0, 
                    bool *isglobal = 0) ;
  STD string GetVarName( const AstNodePtr& exp);

  bool IsSameVarRef( const AstNodePtr& v1, const AstNodePtr& v2);
  bool IsAliasedRef( const AstNodePtr& s1, const AstNodePtr& s2);
  STD string NewVar (const AstNodeType& t, const STD string& name = "", 
                bool makeunique = false, const AstNodePtr& declLoc=AstNodePtr(),
                const AstNodePtr& init = AstNodePtr());
  void AddNewVarDecls(const AstNodePtr& nblock, const AstNodePtr& oldblock); 
  AstNodePtr CreateVarRef( STD string varname, const AstNodePtr& declLoc = AstNodePtr()); 

  bool IsScalarType( const AstNodeType& t);
  bool IsPointerType( const AstNodeType& t);
  AstNodeType GetType( const STD string& name);
  bool IsCompatibleType( const AstNodeType& t1, const AstNodeType& t2);
  void GetTypeInfo( const AstNodeType& t, STD string* name = 0, 
                           STD string* stripname = 0, int* size = 0);
  STD string GetTypeName(const AstNodeType& t) 
     { STD string r; GetTypeInfo(t, &r); return r; }

  bool GetArrayBound( const AstNodePtr& arrayname, int dim, int &lb, int &ub) ;
  AstNodeType GetArrayType( const AstNodeType& base, const AstNodeList& indexsize);

  AstNodePtr CreateAllocateArray( const AstNodePtr& arr, const AstNodeType& elemtype, 
                                const AstNodeList& indexsize);
  AstNodePtr CreateDeleteArray( const AstNodePtr& arr);
  bool IsArrayAccess( const AstNodePtr& s, AstNodePtr* array = 0,
                                   AstNodeList* index = 0)  ;
  AstNodePtr CreateArrayAccess( const AstNodePtr& arr, const AstNodeList& index);

  bool IsBinaryOp(  const AstNodePtr& exp, OperatorEnum* opr=0, 
                    AstNodePtr* opd1 = 0, AstNodePtr* opd2 = 0);
  bool IsUnaryOp( const AstNodePtr& exp, OperatorEnum* op = 0, 
                   AstNodePtr* opd = 0); 
  AstNodePtr CreateBinaryOP( OperatorEnum op, const AstNodePtr& a0, 
                                   const AstNodePtr& a2);
  AstNodePtr CreateUnaryOP( OperatorEnum op, const AstNodePtr& arg);
};

class ProcessAstNode
{
  public:
   virtual bool Traverse( AstInterface &fa, const AstNodePtr& n, 
                             AstInterface::TraversalVisitType t) = 0;
};

bool ReadAstTraverse(AstInterface& fa, const AstNodePtr& root, 
                        ProcessAstNode& op, 
                        AstInterface::TraversalOrderType t = AstInterface::PreOrder); 

class TransformAstTree
{
 public:
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n, 
                           AstNodePtr& result) = 0;
};

AstNodePtr TransformAstTraverse( AstInterface& fa, const AstNodePtr& r, 
                    bool (*op)( AstInterface& fa, const AstNodePtr& head, 
                                AstNodePtr& result), 
                    AstInterface::TraversalVisitType t = AstInterface::PreVisit );
AstNodePtr TransformAstTraverse( AstInterface& fa, const AstNodePtr& r, 
                              TransformAstTree& op,
                        AstInterface::TraversalVisitType t = AstInterface::PreVisit);

#endif
