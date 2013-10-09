
#ifndef AST_TREE_INTERFACE_H
#define AST_TREE_INTERFACE_H

#include <iostream>
#include <list>
#include <string>
#include <typeinfo>
#include "ObserveObject.h"
#include "rosedll.h"

class AstNodePtr;
ROSE_DLL_API std::string AstToString( const AstNodePtr& s);
ROSE_DLL_API std::string getAstLocation( const AstNodePtr& s);

class SgProject;
ROSE_DLL_API void FixSgProject( SgProject &sageProject);

class AST_Error { 
   std::string msg;
  public:
   AST_Error(const std::string& _msg) : msg(_msg) {}
   std::string get_msg() { return msg; }
};

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
  virtual void print() const // for debugging
    { std::cerr << AstToString(*this); }
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

//! This is the base class for anyone who wants to be notified when AST nodes are being copied.
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
      : orig(o), n(_n), fa(_fa) {}
  virtual void UpdateObserver( AstObserver& o) const 
         {  o.ObserveCopyAst(fa, orig, n); }
  virtual ~CopyAstRecord() {}
};
 

// AstInterface
/**
 *  @brief  Holds entire AST, and provides interface to access/modify its nodes.
 */
class ROSE_DLL_API AstInterface 
{
protected:
  AstInterfaceImpl *impl;

public:
  // Types:
  typedef enum {OP_NONE = 0, 
           UOP_MINUS, UOP_ADDR, UOP_DEREF, UOP_ALLOCATE, UOP_NOT,
           UOP_CAST, UOP_INCR1, UOP_DECR1,
           BOP_DOT_ACCESS, BOP_ARROW_ACCESS, 
           BOP_TIMES, BOP_DIVIDE, BOP_MOD, BOP_PLUS, BOP_MINUS, 
           BOP_EQ, BOP_LE, BOP_LT, BOP_NE, BOP_GT, BOP_GE, 
           BOP_AND, BOP_OR,
           BOP_BIT_AND,BOP_BIT_OR, BOP_BIT_RSHIFT, BOP_BIT_LSHIFT,
           OP_ARRAY_ACCESS, OP_ASSIGN, OP_UNKNOWN} OperatorEnum;

  typedef std::list<AstNodePtr>  AstNodeList;
  typedef std::list<AstNodeType> AstTypeList;

  typedef AstNodePtr    value_type;
  typedef AstNodeList   container_type;
 
  // Constructors/destructor:

  AstInterface(AstInterfaceImpl* __impl) : impl(__impl) {}
  ~AstInterface() {}
  AstInterfaceImpl* get_impl() { return impl; }


  // Queires:

  AstNodePtr GetRoot() const;
  AstNodePtr getNULL() const { return AstNodePtr(); }
  void SetRoot( const AstNodePtr& root);

  typedef enum { PreOrder, PostOrder, ReversePreOrder, ReversePostOrder, 
                 PreAndPostOrder } TraversalOrderType;
  typedef enum {PreVisit, PostVisit} TraversalVisitType;

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

  void SetParent(const AstNodePtr& n, const AstNodePtr& p);
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

  //! Check whether $s$ is a Fortran-style loop in the form: for (ivar=lb;ivar<=ub;ivar+=step)
  bool IsFortranLoop( const AstNodePtr& s, AstNodePtr* ivar = 0,
                       AstNodePtr* lb = 0, AstNodePtr* ub=0,
                       AstNodePtr* step =0, AstNodePtr* body=0);
  AstNodePtr CreateLoop( const AstNodePtr& cond, const AstNodePtr& body); 
  AstNodePtr CreateLoop( const AstNodePtr& ivar, const AstNodePtr& lb, 
                         const AstNodePtr& ub, const AstNodePtr& step, 
                         const AstNodePtr& stmts, bool negativeStep);

  bool IsIf( const AstNodePtr& s, AstNodePtr* cond = 0, 
                       AstNodePtr* truebody = 0, AstNodePtr* falsebody = 0);

  ///  Creates if-else-statement, or if-statement (if \a __else_stmt is null).
  value_type
  CreateIf(const value_type& __cond, const value_type& __if_stmt,
           const value_type& __else_stmt = AST_NULL) const;

  // I/O Statements (Fortran oriented, to be refined):

  ///  Creates a statement that reads \a __refs from \e stdin.
  value_type
  CreateReadStatement(const container_type& __refs) const;

  ///  overload
  value_type
  CreateReadStatement(const value_type& __ref) const
  { return CreateReadStatement(container_type(1, __ref)); }

  ///  Creates a statement that writes \a __refs to \e stdout.
  value_type
  CreateWriteStatement(const container_type& __vals) const;

  ///  overload
  value_type
  CreateWriteStatement(const value_type& __val) const
  { return CreateWriteStatement(container_type(1, __val)); }

  ///  Creates a statement that prints \a __refs to \e stdout.
  value_type
  CreatePrintStatement(const container_type& __vals) const;

  ///  overload
  value_type
  CreatePrintStatement(const value_type& __val) const
  { return CreatePrintStatement(container_type(1, __val)); }

  ///  Creates an empty statement.
  value_type
  CreateNullStatement() const;

  //! Check whether $s$ is a jump (goto, return, continue, break, etc) stmt;
  //! if yes, grab the jump destination in 'dest'
  bool IsGoto( const AstNodePtr& s, AstNodePtr* dest = 0);
  bool IsGotoBefore( const AstNodePtr& s); // goto the point before destination
  bool IsGotoAfter( const AstNodePtr& s); // goto the point after destination
  bool IsLabelStatement( const AstNodePtr& s);
  bool IsReturn(const AstNodePtr& s, AstNodePtr* val=0);

  bool GetFunctionCallSideEffect( const AstNodePtr& fc,  
                     CollectObject<AstNodePtr>& collectmod,  
                     CollectObject<AstNodePtr>& collectread);
  bool IsFunctionCall( const AstNodePtr& s, AstNodePtr* f = 0, 
                       AstNodeList* args = 0, AstNodeList* outargs = 0, 
                       AstTypeList* paramtypes = 0, AstNodeType* returntype=0);
  bool IsMin(const AstNodePtr& exp);
  bool IsMax(const AstNodePtr& exp);
  AstNodePtr CreateFunctionCall(const std::string& func, const AstNodeList& args);
  AstNodePtr CreateFunctionCall(const AstNodePtr& func, const AstNodeList& args);

  AstNodePtr GetFunctionDefinition( const AstNodePtr &n, std::string* name=0);
  bool IsFunctionDefinition(  const AstNodePtr& s, std::string* name = 0,
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

  //!Check whether $exp$ is a constant value of type int, float, string, etc.
  bool IsConstant( const AstNodePtr& exp, std::string* valtype=0, std::string* value = 0) ;
  //! Create AST for constant values of  types int, bool, string, float, etc. as well as names of variable and function references. e.g: CreateConstant("memberfunction","floatArray::length")
  AstNodePtr CreateConstant( const std::string& valtype, const std::string& val);
  //! Check whether $exp$ is a variable reference; If yes, return type, name, scope, and global/local etc.
  bool IsVarRef( const AstNodePtr& exp, AstNodeType* vartype = 0,
                   std::string* varname = 0, AstNodePtr* scope = 0, 
                    bool *isglobal = 0) ;

  std::string GetVarName( const AstNodePtr& exp);

  bool IsSameVarRef( const AstNodePtr& v1, const AstNodePtr& v2);
  std::string NewVar (const AstNodeType& t, const std::string& name = "", 
                bool makeunique = false, const AstNodePtr& declLoc=AstNodePtr(),
                const AstNodePtr& init = AstNodePtr());
  void AddNewVarDecls(const AstNodePtr& nblock, const AstNodePtr& oldblock); 

  // assert error if __varname is already declared in __declLoc.
  AstNodePtr CreateVarRef( std::string varname, const AstNodePtr& declLoc = AstNodePtr()); 

  ///  Returns whether variable __varname is in the specified scope __declLoc.
  bool
  HasVarRef(std::string __varname, const AstNodePtr& __declLoc = AstNodePtr());

  bool IsScalarType(const AstNodeType& t);
  bool IsPointerType(const AstNodeType& t);
  bool IsArrayType(const AstNodeType& t, int* dim = 0, AstNodeType* base = 0);

  AstNodeType GetType(const std::string& name);
  bool IsCompatibleType( const AstNodeType& t1, const AstNodeType& t2);
  static void GetTypeInfo( const AstNodeType& t, std::string* name = 0, 
                           std::string* stripname = 0, int* size = 0);
  static std::string GetTypeName(const AstNodeType& t) 
     { std::string r; GetTypeInfo(t, &r); return r; }
  static std::string GetBaseTypeName(const AstNodeType& t) 
     { std::string r; GetTypeInfo(t, 0, &r); return r; }

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

  ///  Returns the enclosing block.
  AstNodePtr
  GetParentBlock(const AstNodePtr& __n)
  {
    if (__n == AST_NULL)
      return AST_NULL;

    AstNodePtr p = GetParent(__n);
    while (p != AST_NULL && !IsBlock(p))
      p = GetParent(p);
    return p;
  }

  ///  Returns the enclosing statement.
  AstNodePtr
  GetParentStatement(const AstNodePtr& __n)
  {
    if (__n == AST_NULL)
      return AST_NULL;

    AstNodePtr p = GetParent(__n);
    while (p != AST_NULL && !IsStatement(p))
      p = GetParent(p);
    return p;
  }

  bool
  IsFortranLanguage();

public:
  void
  _debug(const AstNodePtr&) const;
};

// Types:
typedef AstInterface::AstNodeList AstNodeList;
typedef AstInterface::AstTypeList AstTypeList;

//! Interface class for processing each AstNode from within the ReadAstTraverse function.
class ProcessAstNode
{
  public:
    //! return true if asking the traversal to continue; false otherwise
   virtual bool Traverse( AstInterface &fa, const AstNodePtr& n, 
                             AstInterface::TraversalVisitType t) = 0;
};

//! Traverse an entire AST, where $op$ is invoked on each AST node to gather information. 
bool ReadAstTraverse(AstInterface& fa, const AstNodePtr& root, 
                        ProcessAstNode& op, 
                        AstInterface::TraversalOrderType t = AstInterface::PreOrder); 

//! Interface class for processing each AstNode from within the TransformAstTraverse function.
class TransformAstTree
{
 public:
    //! return true if asking the traversal to continue; false otherwise
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n, 
                           AstNodePtr& result) = 0;
};

//! Traverse and transform an entire AST, where $op$ is invoked to transform each sub-Tree.
ROSE_DLL_API AstNodePtr TransformAstTraverse( AstInterface& fa, const AstNodePtr& r, 
                    bool (*op)( AstInterface& fa, const AstNodePtr& head, 
                                AstNodePtr& result), 
                    AstInterface::TraversalVisitType t = AstInterface::PreVisit );
ROSE_DLL_API AstNodePtr TransformAstTraverse( AstInterface& fa, const AstNodePtr& r, 
                              TransformAstTree& op,
                        AstInterface::TraversalVisitType t = AstInterface::PreVisit);

#endif
