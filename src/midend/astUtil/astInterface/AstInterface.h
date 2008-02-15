
#ifndef SAGE_AST_TREE_INTERFACE
#define SAGE_AST_TREE_INTERFACE

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <ObserveObject.h>

// #warning "Inside of AstInterface.h"

class SgNode;
// #include "sage3.h"

class AstNodePtr {
  SgNode* repr;
 public:
  typedef SgNode PtrBaseClass;
  AstNodePtr( SgNode* n = 0) : repr(n) {}
  AstNodePtr( const AstNodePtr& that) : repr(that.repr) {}
  AstNodePtr& operator = (const AstNodePtr &that) 
      { repr = that.repr; return *this; }
  ~AstNodePtr() {}
  operator SgNode* () const { return repr; }
  SgNode* operator -> () const { return repr; }
  void Dump() const;

// DQ (3/23/2006): I was deperate to get the value of the private variable
// and nothing else appeared to work.
// SgNode* get_ptr() const { return repr; }
};
class SgType;
typedef SgType* AstNodeType;


class AstInterfaceBase
{
 public:
  typedef enum { PreOrder, PostOrder, ReversePreOrder, ReversePostOrder, 
                 PreAndPostOrder } TraversalOrderType;
  typedef enum {PreVisit, PostVisit} TraversalVisitType;
  typedef std::list<AstNodePtr> AstNodeList;
  typedef std::list<AstNodeType> AstTypeList;
  template <class Node>
  class AstListIterator 
   {
  // DQ (9/4/2005): Swapped order of data members to fixup compiler warnings
  // typename list<Node>::iterator repr;
     std::list<Node> *l;
     typename std::list<Node>::iterator repr;
    public:
     AstListIterator( std::list<Node> &lt) : l(&lt) { repr = lt.begin(); }
     AstListIterator( const AstListIterator<Node> &that) 
          : l(that.l), repr(that.repr) {}
     AstListIterator<Node>& operator = (const AstListIterator<Node> &that) 
          { l = that.l; repr = that.repr; return *this; }
     ~AstListIterator() {}
     Node operator *() const { return *repr; }
     Node& operator *() { return *repr; }
  // DQ (3/8/2006): Removed Boolean macro set to int from use in header files
  // Boolean ReachEnd() const { return repr == l->end(); }
     int ReachEnd() const { return repr == l->end(); }
  // Boolean operator==(const AstListIterator<Node>& that ) const {
     int operator==(const AstListIterator<Node>& that ) const {
           return l == that.l && repr == that.repr;
     }
  // Boolean operator!=(const AstListIterator<Node>& that ) const {
     int operator!=(const AstListIterator<Node>& that ) const {
           return !((*this) == that);
     }
     void Reset() { repr = l->begin(); }
     void Advance() { if (repr != l->end()) ++repr; }
     void operator++() { Advance(); }
     void operator ++(int) { Advance(); }
   };
  typedef AstListIterator<AstNodePtr> AstNodeListIterator;
  typedef AstListIterator<AstNodeType> AstTypeListIterator;

  static AstNodeList CreateList()  ;
  static AstNodeListIterator GetAstNodeListIterator( AstNodeList& l);
  static AstTypeListIterator GetAstTypeListIterator( AstTypeList& l);
  static void ListAppend( AstNodeList& l, const AstNodePtr& s) ;
  static void ListPrepend( AstNodeList& l, const AstNodePtr& s) ;
  static void ListReverse( AstNodeList& l) { l.reverse(); }

  static void write( const AstNodePtr& s, std::ostream& out); 
  static void DumpAst(  const AstNodePtr& s);
  static std::string AstToString( const AstNodePtr& s);
/* 
  static Boolean AstTreeIdentical( const AstNodePtr& n1, const AstNodePtr& n2);
  static Boolean AstNodeIdentical( const AstNodePtr& n1, const AstNodePtr& n2);
*/
  static void InsertStmt( const AstNodePtr& orig, const AstNodePtr& n, 
                          bool before = true, bool extractFromBasicBlock = false);
  static void FreeAstTree( const AstNodePtr& n);
  static AstNodeList GetChildrenList( const AstNodePtr &n);

//static Boolean IsLoop( const AstNodePtr& s, 
  static int IsLoop( const AstNodePtr& s, 
                          AstNodePtr* init=0, AstNodePtr* cond=0,
                         AstNodePtr* incr = 0, AstNodePtr* body = 0) ;

  static AstNodePtr GetPrevStmt( const AstNodePtr& s);
  static AstNodePtr GetNextStmt( const AstNodePtr& s);

//static Boolean IsDecls( const AstNodePtr& s) ;
  static int IsDecls( const AstNodePtr& s) ;
//static Boolean IsExecutableStmt( const AstNodePtr& s) ;
  static int IsExecutableStmt( const AstNodePtr& s) ;
//static Boolean IsStatement( const AstNodePtr& s);
  static int IsStatement( const AstNodePtr& s);
//static Boolean IsBasicBlock( const AstNodePtr& exp);
  static int IsBasicBlock( const AstNodePtr& exp);
  static AstNodeList GetBasicBlockStmtList( const AstNodePtr& n);
  static AstNodePtr GetBasicBlockFirstStmt( const AstNodePtr& n);
  static AstNodePtr GetBasicBlockLastStmt( const AstNodePtr& n);
  static int GetBasicBlockSize( const AstNodePtr& n);
  static AstNodePtr CreateBasicBlock( const AstNodePtr& orig = 0) ;
  static void BasicBlockAppendStmt( AstNodePtr& b, const AstNodePtr& s);
  static void BasicBlockPrependStmt( AstNodePtr& b, const AstNodePtr& s);
  
//static Boolean IsPostTestLoop( const AstNodePtr& s);
  static int IsPostTestLoop( const AstNodePtr& s);
//static Boolean IsIf( const AstNodePtr& s, AstNodePtr* cond = 0, 
  static int IsIf( const AstNodePtr& s, AstNodePtr* cond = 0, 
                       AstNodePtr* truebody = 0, AstNodePtr* falsebody = 0);
//static Boolean IsGoto( const AstNodePtr& s, AstNodePtr* dest = 0);
  static int IsGoto( const AstNodePtr& s, AstNodePtr* dest = 0);
//static Boolean IsGotoBefore( const AstNodePtr& s); // goto the point before destination
  static int IsGotoBefore( const AstNodePtr& s); // goto the point before destination
//static Boolean IsGotoAfter( const AstNodePtr& s); // goto the point after destination
  static int IsGotoAfter( const AstNodePtr& s); // goto the point after destination
//static Boolean IsLabelStatement( const AstNodePtr& s);
  static int IsLabelStatement( const AstNodePtr& s);

//static Boolean IsFunctionDefinition(  const AstNodePtr& s, AstNodePtr* decl = 0,
  static int IsFunctionDefinition(  const AstNodePtr& s, AstNodePtr* decl = 0,
                                        AstNodePtr* body = 0);
//static Boolean IsFunctionDecl( const AstNodePtr& s, std::string* declname = 0,  
  static int IsFunctionDecl( const AstNodePtr& s, std::string* declname = 0,  
				 AstNodeType* returntype = 0, 
				 AstTypeList* paramType = 0, AstNodeList* params = 0);
//static Boolean IsFunctionCall( const AstNodePtr& s, AstNodePtr* func = 0,
  static int IsFunctionCall( const AstNodePtr& s, AstNodePtr* func = 0,
                               AstNodeList* args = 0);
//static Boolean IsAssignment( const AstNodePtr& s, AstNodePtr* lhs = 0, 
  static int IsAssignment( const AstNodePtr& s, AstNodePtr* lhs = 0, 
                               AstNodePtr* rhs = 0, bool* readlhs = 0); 
//static Boolean IsIOInputStmt( const AstNodePtr& s, AstNodeList* varlist = 0);
  static int IsIOInputStmt( const AstNodePtr& s, AstNodeList* varlist = 0);
//static Boolean IsIOOutputStmt( const AstNodePtr& s, AstNodeList* explist = 0);
  static int IsIOOutputStmt( const AstNodePtr& s, AstNodeList* explist = 0);

//static Boolean IsConstInt( const AstNodePtr& exp, int* value = 0) ;
  static int IsConstInt( const AstNodePtr& exp, int* value = 0) ;
//static Boolean IsConstant( const AstNodePtr& exp, std::string* value = 0) ;
  static int IsConstant( const AstNodePtr& exp, std::string* value = 0) ;
//static Boolean IsVarRef( const AstNodePtr& exp, AstNodeType* vartype = 0,
  static int IsVarRef( const AstNodePtr& exp, AstNodeType* vartype = 0,
                           std::string* varname = 0, AstNodePtr* scope = 0, bool *isglobal = 0) ;
//static Boolean IsSameVarRef( const AstNodePtr& v1, const AstNodePtr& v2);
  static int IsSameVarRef( const AstNodePtr& v1, const AstNodePtr& v2);
//static Boolean IsVariableDecl( const AstNodePtr& exp, AstNodeList* vars = 0,
  static int IsVariableDecl( const AstNodePtr& exp, AstNodeList* vars = 0,
                                 AstNodeList* inits = 0);
//static Boolean IsArrayAccess( const AstNodePtr& s, AstNodePtr* array = 0,
  static int IsArrayAccess( const AstNodePtr& s, AstNodePtr* array = 0,
                                   AstNodeList* index = 0)  ;
//static Boolean IsMemoryAccess( const AstNodePtr& s);
  static int IsMemoryAccess( const AstNodePtr& s);
//static Boolean IsExpression( const AstNodePtr& s, AstNodeType* exptype =0 );
  static int IsExpression( const AstNodePtr& s, AstNodeType* exptype =0 );
//static Boolean IsBinaryOp(  const AstNodePtr& exp, AstNodePtr* opd1 = 0, 
  static int IsBinaryOp(  const AstNodePtr& exp, AstNodePtr* opd1 = 0, 
			      AstNodePtr* opd2 = 0, std::string* opname = 0 );
//static Boolean IsBinaryTimes( const AstNodePtr& exp, AstNodePtr* opd1 = 0,
  static int IsBinaryTimes( const AstNodePtr& exp, AstNodePtr* opd1 = 0,
                                AstNodePtr* opd2 = 0 ) ;
//static Boolean IsBinaryPlus( const AstNodePtr& exp, AstNodePtr* opd1 = 0,
  static int IsBinaryPlus( const AstNodePtr& exp, AstNodePtr* opd1 = 0,
                               AstNodePtr* opd2 = 0) ;
//static Boolean IsBinaryMinus( const AstNodePtr& exp, AstNodePtr* opd1 = 0,
  static int IsBinaryMinus( const AstNodePtr& exp, AstNodePtr* opd1 = 0,
                               AstNodePtr* opd2 = 0) ;
//static Boolean IsRelEQ( const AstNodePtr& s, AstNodePtr* opd1 = 0, 
  static int IsRelEQ( const AstNodePtr& s, AstNodePtr* opd1 = 0, 
                           AstNodePtr* opd2 = 0);
//static Boolean IsRelNE( const AstNodePtr& s, AstNodePtr* opd1 = 0, 
  static int IsRelNE( const AstNodePtr& s, AstNodePtr* opd1 = 0, 
                          AstNodePtr* opd2 = 0);
//static Boolean IsUnaryOp( const AstNodePtr& exp, AstNodePtr* opd = 0, 
  static int IsUnaryOp( const AstNodePtr& exp, AstNodePtr* opd = 0, 
                            std::string* fname = 0) ;
//static Boolean IsUnaryMinus( const AstNodePtr& exp, AstNodePtr* opd = 0) ;
  static int IsUnaryMinus( const AstNodePtr& exp, AstNodePtr* opd = 0) ;


  static AstNodePtr CreateConstInt( int val)  ;
  static AstNodePtr CreateFunctionCall( const AstNodePtr& f, AstNodeList args);
  static AstNodePtr CreateIf( const AstNodePtr& cond, const AstNodePtr& stmts) ;

//static Boolean
  static int
     GetArrayBound( const AstNodePtr& arrayname, int dim, int &lb, int &ub) ;
  static void GetTypeInfo( const AstNodeType& t, std::string* name = 0, 
                           std::string* stripname = 0, int* size = 0);
  static std::string GetTypeName( const AstNodeType& t);
  static std::string GetTypeSpec( const AstNodeType& t);
//static Boolean IsScalarType( const AstNodeType& t);
  static int IsScalarType( const AstNodeType& t);
//static Boolean IsCompatibleType( const AstNodeType& t1, const AstNodeType& t2);
  static int IsCompatibleType( const AstNodeType& t1, const AstNodeType& t2);
  static std::string GetVarName( const AstNodePtr& exp);
  static AstNodeType GetExpressionType( const AstNodePtr& s);
  static AstNodePtr GetFunctionDecl( const AstNodePtr& s);
  static std::string GetFunctionName( const AstNodePtr& f);

};

class AstInterface;
class AstObserver {
  public:
  virtual ~AstObserver() {};
   virtual void ObserveCopyAst( AstInterface& fa, const AstNodePtr& orig, const AstNodePtr& n) = 0;
};

class CopyAstRecord : public ObserveInfo< AstObserver>
   {
          AstNodePtr orig, n;
          AstInterface& fa;
     public:
       // DQ (8/20/2005): Changed order of initialization in constructor preinitialization (avoid compiler warning)
       // CopyAstRecord(AstInterface& _fa, const AstNodePtr& o, const AstNodePtr& _n)  : fa(_fa), orig(o), n(_n) {}
          CopyAstRecord(AstInterface& _fa, const AstNodePtr& o, const AstNodePtr& _n)  : orig(o), n(_n), fa(_fa) {}
          virtual void UpdateObserver( AstObserver& o) const 
             {  o.ObserveCopyAst(fa, orig, n); }

       // DQ (9/4/2005): Added virtual destructor to satisy compiler warning
          virtual ~CopyAstRecord() {}
   };
 
class AstInterfaceImpl;
class AstInterface : public AstInterfaceBase, public ObserveObject< AstObserver>
{
  AstInterfaceImpl *impl;
 protected:
 public:
  AstInterface( const AstNodePtr& root);
  ~AstInterface() ;

  AstNodePtr GetRoot() const;
  void SetRoot( const AstNodePtr& root);
  AstNodePtr GetVarDecl( const std::string& varname);
  AstNodePtr CopyAstTree( const AstNodePtr& n);

  void AddDeclaration( const std::string& decl);
  std::string NewVar (const AstNodeType& t, const std::string& name = "", 
                 bool makeunique = false, const AstNodePtr& declLoc = 0,
                 const AstNodePtr& init = 0);

  AstNodePtr GetParent( const AstNodePtr &n);

  AstNodePtr CreateVarRef( std::string varname, const AstNodePtr& loc = 0) const;
  AstNodePtr CreateConst( const std::string& val, const std::string& valtype) const;
  AstNodePtr CreateFunction(  const std::string& name, const std::string& decl_if_not_found);
  AstInterfaceBase::CreateFunctionCall;
  AstNodePtr CreateFunctionCall( const std::string& func, const std::string& decl, AstNodeList args);
  AstNodePtr CreateAssignment( const AstNodePtr& lhs, const AstNodePtr& rhs);
  AstNodePtr CreateLoop( const AstNodePtr& ivar, const AstNodePtr& lb, 
                         const AstNodePtr& ub, const AstNodePtr& step, 
                         const AstNodePtr& stmts);
  AstNodePtr CreateBinaryOP( const std::string& op, const AstNodePtr& a0, 
                                   const AstNodePtr& a2) const;
  AstNodePtr CreateArrayAccess( const AstNodePtr& arr, const AstNodeList& index);
  AstNodePtr CreateBinaryMinus( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateBinaryPlus( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateBinaryTimes( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateUnaryOP( const std::string& op, const AstNodePtr& arg);
  AstNodePtr CreateRelNE( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateRelEQ( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateRelLT( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateRelGT( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateRelLE( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateRelGE( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr CreateLogicalAND( const AstNodePtr& a1, const AstNodePtr& a2) ;
  AstNodePtr AllocateArray( const AstNodePtr& arr, const AstNodeType& elemtype, 
                                   const std::vector<AstNodePtr>& indexsize);
  AstNodePtr DeleteArray( const AstNodePtr& arr);

  bool ReplaceAst( const AstNodePtr& orig, const AstNodePtr& n);
  bool RemoveStmt( const AstNodePtr& n);

  AstNodeType GetType( std::string name);
  AstNodeType GetArrayType( const AstNodeType& base, const std::vector<AstNodePtr>& indexsize);
};

class ProcessAstNode
{
  public:
  virtual ~ProcessAstNode() {};
// virtual Boolean Traverse( AstInterface &fa, const AstNodePtr& n, AstInterface::TraversalVisitType t) = 0;
   virtual int Traverse( AstInterface &fa, const AstNodePtr& n, AstInterface::TraversalVisitType t) = 0;
};

// Boolean ReadAstTraverse(AstInterface& fa, const AstNodePtr& root, 
int ReadAstTraverse(AstInterface& fa, const AstNodePtr& root, 
                        ProcessAstNode& op, 
                        AstInterface::TraversalOrderType t = AstInterface::PreOrder); 

class TransformAstTree
{
 public:
  virtual ~TransformAstTree() {};
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
