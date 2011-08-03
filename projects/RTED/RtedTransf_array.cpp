// vim:et sta sw=4 ts=4:
#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <algorithm>
#include <numeric>

#include <boost/foreach.hpp>

#include "sageGeneric.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

static inline
void ROSE_ASSERT_MSG(bool b, const std::string& msg)
{
  if (!b)
  {
    std::cerr << msg << std::endl;
    ROSE_ASSERT(false);
  }
}

struct InitNameComp
{
  const SgInitializedName * const obj;

  explicit
  InitNameComp(const SgInitializedName* iname)
  : obj(iname)
  {}

  bool operator()(const std::map<SgVarRefExp*, RtedArray>::value_type& v) const
  {
    return v.second.initName == obj;
  }
};


static
void appendUpcBlocksize(SgExprListExp* arg_list, SgType* arrtype)
{
  //               == -2 ... distributed with max blocking (i.e., shared [*] )
  //               == -1 ... block size omitted ( same as blocksize = 1)
  //               ==  0 ... non-distributed (indefinite block size)
  long               blocksize = upcBlocksize(arrtype);

  if (blocksize == -1 /* default blocksize */)
  {
    blocksize = 1;
  }

  // -2 ... max blocking (blocking factor calculated at runtime)
  //  0 ... non-distributed
  // >0 ... blocking factor
  ROSE_ASSERT(blocksize >= 0 || blocksize == -2);

  appendExpression(arg_list, buildLongIntVal(blocksize));
}

static
void appendUpcBlocksize(SgExprListExp* arg_list, SgInitializedName* initName)
{
  appendUpcBlocksize(arg_list, initName->get_type());
}

static
void appendUpcBlocksize(SgExprListExp* arg_list, const RtedArray& arr)
{
  appendUpcBlocksize(arg_list, arr.initName);
}



/* -----------------------------------------------------------
 * Is the Initialized Name already known as an array element ?
 * -----------------------------------------------------------*/
bool RtedTransformation::isVarRefInCreateArray(SgInitializedName* search)
{
  if (create_array_define_varRef_multiArray_stack.find(search) != create_array_define_varRef_multiArray_stack.end())
    return true;

  std::map<SgVarRefExp*, RtedArray>::iterator aa = create_array_define_varRef_multiArray.begin();
  std::map<SgVarRefExp*, RtedArray>::iterator zz = create_array_define_varRef_multiArray.end();

  return std::find_if(aa, zz, InitNameComp(search)) != zz;
}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateCall
 * -----------------------------------------------------------*/
void RtedTransformation::insertArrayCreateCall(const RtedArray& arr, SgVarRefExp* n)
{
   ROSE_ASSERT(n);

   // \pp \todo check why n can be w/o parent
   SgExpression*      srcexp = n->get_parent() ? getExprBelowAssignment(n) : n;

   insertArrayCreateCall(srcexp, arr);
}


void RtedTransformation::insertArrayCreateCall(const RtedArray& arr)
{
   SgVarRefExp* var_ref = genVarRef(arr.initName);

   insertArrayCreateCall(var_ref, arr);
}

// \pp \todo add to SageBuilder
static
SgEnumVal* buildEnumVal(int value, SgEnumDeclaration* decl, SgName name)
{
  SgEnumVal* enumVal = new SgEnumVal(value, decl, name);
  ROSE_ASSERT(enumVal);
  setOneSourcePositionForTransformation(enumVal);
  return enumVal;
}

static
std::string enumstring(AllocKind ak)
{
  const char* res = NULL;

  switch (ak)
  {
    case akUndefined:       res = "akUndefined"; break;
    case akGlobal:          res = "akGlobal"; break;
    case akStack:           res = "akStack"; break;
    case akCHeap:           res = "akCHeap"; break;

    /* C++ */
    case akCxxNew:          res = "akCxxNew"; break;
    case akCxxArrayNew:     res = "akCxxArrayNew"; break;

    /* UPC */
    case akUpcShared:       res = "akUpcShared"; break;
    case akUpcAlloc:        res = "akUpcAlloc"; break;
    case akUpcGlobalAlloc:  res = "akUpcGlobalAlloc"; break;
    case akUpcAllAlloc:     res = "akUpcAllAlloc"; break;
    case akUpcSharedGlobal: res = "akUpcSharedGlobal"; break;

    default:                ROSE_ASSERT(false);
  }

  ROSE_ASSERT(res != NULL);
  return res;
}


SgEnumVal*
RtedTransformation::mkAllocKind(AllocKind ak) const
{
  ROSE_ASSERT(symbols.roseAllocKind);

  return buildEnumVal(ak, symbols.roseAllocKind, enumstring(ak));
}


SgStatement*
RtedTransformation::buildArrayCreateCall(SgExpression* const src_exp, const RtedArray& array, SgStatement* const stmt)
{
   ROSE_ASSERT(src_exp && stmt);

   std::cerr << "@@@ " << src_exp->unparseToString() << std::endl;

   // build the function call:  rs.createHeapArr(...);
   //                        or rs.createHeapPtr(...);
   SgInitializedName* initName = array.initName;
   SgExprListExp*     arg_list = buildExprListExp();

   // the type of the node
   SgType*            src_type = src_exp->get_type();

   // the underlying type, after skipping modifiers (shall we also skip typedefs?)
   SgType*            under_type = skip_TopLevelTypes(src_type);
   const bool         isCreateArray = (under_type->class_name() == "SgArrayType");

   // what kind of types do we get?
   // std::cerr << typeid(under_type).name() << "  " << under_type->class_name() << std::endl;
   ROSE_ASSERT(isCreateArray || under_type->class_name() == "SgPointerType");

   // if we have an array, then it has to be on the stack
   ROSE_ASSERT( !isCreateArray || ((array.allocKind & (akStack | akGlobal)) != 0) );

   SgScopeStatement*  scope = get_scope(initName);

   appendExpression(arg_list, ctorTypeDesc(mkTypeInformation(src_type, false, false)));
   appendAddressAndSize(arg_list, Whole, scope, src_exp, NULL /* unionclass */);

   // track how and where the memory/array was allocated
   appendAllocKind(arg_list, array.allocKind);

   SgType*            type = initName->get_type();
   SgFunctionSymbol*  rted_fun = NULL;

   // target specific parameters
   if (isCreateArray)
   {
     // Is this array distributed across threads?
     appendUpcBlocksize( arg_list, array );

     // Is this array initialized?  int x[] = { 1, 2, 3 };
     appendBool( arg_list, (initName->get_initializer() != NULL) );

     // Dimension info
     appendDimensions(arg_list, array);

     // source name
     appendExpression(arg_list, buildStringVal(initName->get_name()));

     // mangeled name
     appendExpression(arg_list, buildStringVal(initName->get_mangled_name().str()));

     rted_fun = symbols.roseCreateArray;
   }
   else
   {
     // \pp \todo not sure whether finding the first pointer type is sufficient
     //           for pointer assignments to variables having arbitrary types.
     SgPointerType* ptrtype = discover_PointerType(type);

     // get the blocksize of the allocation?
     appendUpcBlocksize( arg_list, ptrtype->get_base_type() );

     ROSE_ASSERT(array.size);
     SgExpression*      size = buildCastExp(array.size, buildUnsignedLongType());

     appendExpression(arg_list, size);

     rted_fun = symbols.roseAllocMem;
   }

   appendClassName(arg_list, type);
   appendFileInfo(arg_list, stmt);

   ROSE_ASSERT(rted_fun != NULL);
   SgFunctionRefExp*  memRef_r = buildFunctionRefExp(rted_fun);
   SgExprStatement*   exprStmt = buildFunctionCallStmt(memRef_r, arg_list);

   return exprStmt;
}

void RtedTransformation::insertArrayCreateCall( SgExpression* const srcexp, const RtedArray& array )
{
   ROSE_ASSERT(srcexp);

   SgStatement*               stmt = array.surroundingStatement;
   SgInitializedName* const   initName = array.initName;

   // Skipping extern arrays because they will be handled in the defining
   //   translation unit.
   // \pp \todo Maybe we should check that there really is one...
   if (isGlobalExternVariable(stmt)) return;

   SgStatement*               insloc = stmt;
   SgScopeStatement*          scope = stmt->get_scope();
   ROSE_ASSERT(scope);

   // what if there is an array creation within a ClassDefinition
   if (isSgClassDefinition(scope)) {
       // new stmt = the classdef scope
       SgClassDeclaration* decl = isSgClassDeclaration(scope->get_parent());
       ROSE_ASSERT(decl);
       stmt = isSgVariableDeclaration(decl->get_parent());
       if (!stmt) {
         std::cerr << " Error . stmt is unknown : " << decl->get_parent()->class_name() << std::endl;
         ROSE_ASSERT( false );
       }
       scope = scope->get_scope();
       insloc = stmt;
       // We want to insert the stmt before this classdefinition, if its still in a valid block
       std::cerr << " ....... Found ClassDefinition Scope. New Scope is : " << scope->class_name() << "  stmt:"
            << stmt->class_name() << std::endl;
   }
   // what if there is an array creation in a global scope
   else if (isSgGlobal(scope)) {
       scope = mainBody;

       ROSE_ASSERT(globalsInitLoc);
       insloc = globalsInitLoc;
   }
   // \pp \todo handle variables defined in namespace

   if (isSgIfStmt(scope)) {
       SgStatement* exprStmt = buildArrayCreateCall(srcexp, array, stmt);
       ROSE_ASSERT(exprStmt);
       // get the two bodies of the ifstmt and prepend to them
       std::cerr << "If Statment : inserting createHeap" << std::endl;
       SgStatement* trueb = isSgIfStmt(scope)->get_true_body();
       SgStatement* falseb = isSgIfStmt(scope)->get_false_body();
       bool partOfTrue = traverseAllChildrenAndFind(srcexp, trueb);
       bool partOfFalse = traverseAllChildrenAndFind(srcexp, falseb);
       bool partOfCondition = (!partOfTrue && !partOfFalse);

       if (trueb && (partOfTrue || partOfCondition)) {
         if (!isSgBasicBlock(trueb)) {
             removeStatement(trueb);
             SgBasicBlock* bb = buildBasicBlock();
             bb->set_parent(isSgIfStmt(scope));
             isSgIfStmt(scope)->set_true_body(bb);
             bb->prepend_statement(trueb);
             trueb = bb;
         }
         prependStatement(exprStmt, isSgScopeStatement(trueb));
       }
       if (falseb && (partOfFalse || partOfCondition)) {
         if (!isSgBasicBlock(falseb)) {
             removeStatement(falseb);
             SgBasicBlock* bb = buildBasicBlock();
             bb->set_parent(isSgIfStmt(scope));
             isSgIfStmt(scope)->set_false_body(bb);
             bb->prepend_statement(falseb);
             falseb = bb;
         }
         prependStatement(exprStmt, isSgScopeStatement(falseb));
       }
   } else if (isSgBasicBlock(scope)) {
       SgStatement* exprStmt = buildArrayCreateCall(srcexp, array, stmt);

       insertStatementAfter(insloc, exprStmt);

       // insert in sequence
       if (insloc == globalsInitLoc) globalsInitLoc = exprStmt;

       std::string empty_comment = "";
       attachComment(exprStmt, empty_comment, PreprocessingInfo::before);
       std::string comment = "RS : Create Array Variable, parameters : (name, manglname, typr, basetype, address, sizeof(type), array size, fromMalloc, filename, linenr, linenrTransformed, dimension info ...)";
       attachComment(exprStmt, comment, PreprocessingInfo::before);
  } else {
      const std::string    name = initName->get_mangled_name().str();

      std::cerr << "RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << std::endl;
      ROSE_ASSERT(isSgNamespaceDefinitionStatement(scope));
  }

   // unfortunately the arrays are filled with '\0' which is a problem
   // for detecting other bugs such as not null terminated strings
   // therefore we call a function that appends code to the
   // original program to add padding different from '\0'
   if ( (array.allocKind & (akStack | akGlobal)) == 0 )
      addPaddingToAllocatedMemory(stmt, array);
}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateAccessCall
 * -----------------------------------------------------------*/
void RtedTransformation::insertArrayAccessCall(SgPntrArrRefExp* arrayExp, const RtedArray& value)
{
   ROSE_ASSERT( arrayExp );

   insertArrayAccessCall(value.surroundingStatement, arrayExp, value);
}


struct ReadWriteContextFinder
{
  typedef std::pair<int, const SgNode*> Result;

  Result              res;
  const SgNode* const child;

  explicit
  ReadWriteContextFinder(const SgNode* node)
  : res(RtedTransformation::BoundsCheck, NULL), child(node)
  {}

  void Read()  { res.first |= RtedTransformation::Read; }
  void Write() { /*res.first |= RtedTransformation::Write;*/ }

  void handle(const SgNode& n)
  {
    std::cerr << typeid(n).name() << std::endl;
    assert(false);
  }

  // int x = arr[3];
  void handle(const SgInitializedName& n) { Read(); }

  // if not a write in statement context, its a read
  // \pp this is not necessarily the case
  //     consider:
  //       int a[2][2];
  //       a[2] + 3;  /* no effect */
  //     the expression has no effect (unless in an upc_forall context)
  void handle(const SgStatement&) { Read(); }

  // forwarding function
  void handle_statement(const SgStatement& n) { handle(n); }

  // \pp \quickfix see comment in handle(const SgStatement&)
  void handle(const SgUpcForAllStatement& n)
  {
    if (n.get_affinity() != child)
    {
      handle_statement(n);
    }
  }

  // continue looking
  void handle(const SgExpression& n)
  {
    res.second = &n;
  }

  void handle(const SgAssignOp& n)
  {
    if (n.get_lhs_operand() == child)
      Write();
    else
      Read();
  }

  void handle_short_cut_operators(const SgBinaryOp& n)
  {
    Read();

    if (n.get_lhs_operand() == child)
      Write();
  }

  void handle(const SgAndAssignOp& n)     { handle_short_cut_operators(n); }
  void handle(const SgDivAssignOp& n)     { handle_short_cut_operators(n); }
  void handle(const SgIorAssignOp& n)     { handle_short_cut_operators(n); }
  void handle(const SgLshiftAssignOp& n)  { handle_short_cut_operators(n); }

  void handle(const SgMinusAssignOp& n)   { handle_short_cut_operators(n); }
  void handle(const SgModAssignOp& n)     { handle_short_cut_operators(n); }
  void handle(const SgMultAssignOp& n)    { handle_short_cut_operators(n); }
  void handle(const SgPlusAssignOp& n)    { handle_short_cut_operators(n); }

  void handle(const SgPointerAssignOp& n) { handle_short_cut_operators(n); }
  void handle(const SgRshiftAssignOp& n)  { handle_short_cut_operators(n); }
  void handle(const SgXorAssignOp& n)     { handle_short_cut_operators(n); }

  // when there is an SgPntrArrRefExp, the first SgPntrArrRefExp encountered
  //   will only be bounds checked (no read/write)
  void handle(const SgPntrArrRefExp& n)   { }

  void handle(const SgDotExp& n)
  {
    if (n.get_lhs_operand() != child)
    {
      // recurse
      res.second = &n;
    }
    else
    {
      // \pp why empty?
    }
  }

  operator Result() const { return res; }
};


static
int read_write_context(const SgExpression* node)
{
  ReadWriteContextFinder::Result res(0, node);

  while (res.second)
  {
    res = sg::dispatch(ReadWriteContextFinder(res.second), res.second->get_parent());
  }

  return res.first;
}

void RtedTransformation::insertArrayAccessCall(SgStatement* stmt, SgPntrArrRefExp* arrRefExp, const RtedArray& array)
{
  SgScopeStatement* scope = stmt->get_scope();
  ROSE_ASSERT(scope);

  // Recursively check each dimension of a multidimensional array access.
  // This doesn't matter for stack arrays, since they're contiguous and can
  // therefore be conceptually flattened, but it does matter for double
  // pointer array access.
  if (SgPntrArrRefExp* lhs_arrexp = isSgPntrArrRefExp(arrRefExp -> get_lhs_operand())) {
     //      a[ i ][ j ] = x;
     //      x = a[ i ][ j ];
     // in either case, a[ i ] is read, and read before a[ i ][ j ].
     insertArrayAccessCall(stmt, lhs_arrexp, array);
  }

  // determine whether this array access is a read or write
  const int         read_write_mask = read_write_context(arrRefExp);

  // for contiguous array, base is at &array[0] whether on heap or on stack
  SgPntrArrRefExp*  array_base = deepCopy(arrRefExp);

  array_base -> set_rhs_operand(buildIntVal(0));  // \pp \todo \memleak

  SgExprListExp*    arg_list = buildExprListExp();

  appendAddress(arg_list, array_base);
  appendAddressAndSize(arg_list, Whole, NULL, arrRefExp, NULL);
  appendExpression(arg_list, buildIntVal(read_write_mask));
  appendFileInfo(arg_list, stmt);

  ROSE_ASSERT(symbols.roseAccessArray);
  insertCheck( ilBefore,
               stmt,
               symbols.roseAccessArray,
               arg_list,
               "RS : Access Array Variable, parameters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message)"
              );
}


void RtedTransformation::populateDimensions(RtedArray& array, SgInitializedName& init, SgArrayType& type_)
{
   std::vector<SgExpression*> indices;
   bool                       implicit_index = false;
   SgArrayType*               arrtype = &type_;

   while (arrtype)
   {
      SgExpression* index = arrtype -> get_index();
      if (index)
         indices.push_back(index);
      else
         implicit_index = true;

      arrtype = isSgArrayType(arrtype -> get_base_type());
   }

   // handle implicit first dimension for array initializers
   // for something like
   //      int p[][2][3] = {{{ 1, 2, 3 }, { 4, 5, 6 }}}
   //  we can calculate the first dimension as
   //      sizeof( p ) / ( sizeof( int ) * 2 * 3 )
   if (implicit_index)
   {
      SgExpression* mulid = buildIntVal(1);
      SgExpression* denominator = std::accumulate(indices.begin(), indices.end(), mulid, buildMultiplyOp);
      SgVarRefExp*  varref = buildVarRefExp(&init, getSurroundingStatement(&init)->get_scope());
      SgExpression* sz = buildDivideOp(buildSizeOfOp(varref), denominator);

      indices.insert(indices.begin(), sz);
   }

   array.getIndices().swap(indices);
}


// obsolete
void RtedTransformation::visit_isSgPointerDerefExp(SgPointerDerefExp* const n)
{
   ROSE_ASSERT(n);

   SgExpression*                 right = n->get_operand();
   // right hand side should contain some VarRefExp
   // \pp \note \todo I am not sure why we consider all VarRefExp that
   //                 are underneath the deref.
   const SgNodePtrList&          vars = NodeQuery::querySubTree(right, V_SgVarRefExp);
   SgNodePtrList::const_iterator it = vars.begin();

   for (; it != vars.end(); ++it) {
      SgVarRefExp*  varRef = isSgVarRefExp(*it);
      ROSE_ASSERT(varRef);

      SgExpression* parent = isSgExpression(varRef->get_parent());
      ROSE_ASSERT(parent);

      SgDotExp*     dotExp = isSgDotExp(parent);
      SgArrowExp*   arrowExp = isSgArrowExp(parent);
      SgExpression* left = NULL;

      if (dotExp)
      {
         left = dotExp->get_rhs_operand();
      }
      else if (arrowExp)
      {
         left = arrowExp->get_lhs_operand();
      }

      if (left == varRef || left == NULL)
      {
         variable_access_pointerderef[n] = varRef;
         std::cerr << "$$$ DotExp: " << dotExp << "   arrowExp: " << arrowExp << std::endl;
         std::cerr << "  &&& Adding : " << varRef->unparseToString() << std::endl;
      }
      else
      {
         std::cerr << "$$$ Found a SgPointerDerefExp  but not adding to list. " << std::endl;
         std::cerr << "  $$$ DotExp: " << dotExp << "   arrowExp: " << arrowExp << std::endl;
         std::cerr << "  $$$ left: " << left->unparseToString() << "   varRef: " << varRef->unparseToString() << std::endl;
      }
   }

   if (vars.size() > 1) {
      std::cerr << "Warning : We added more than one SgVarRefExp to this map for SgPointerDerefExp. This might be a problem" << std::endl;
      //exit(1);
   }

#if 1
   const SgNodePtrList&          vars2 = NodeQuery::querySubTree(right, V_SgThisExp);
   SgNodePtrList::const_iterator it2 = vars2.begin();
   for (; it2 != vars2.end(); ++it2) {
      SgThisExp* varRef = isSgThisExp(*it2);
      ROSE_ASSERT(varRef);
      variable_access_arrowthisexp[n] = varRef;
      std::cerr << " &&& Adding : " << varRef->unparseToString() << std::endl;
   }
   if (vars2.size() > 1) {
      std::cerr << "Warning : We added more than one SgThisExp to this map for SgArrowExp. This might be a problem" << std::endl;
      //exit(1);
   }
#endif
}

void RtedTransformation::visit_isSgArrowExp(SgArrowExp* const n)
{
   ROSE_ASSERT(n);

   SgExpression* left = isSgExpression(n->get_lhs_operand());
   ROSE_ASSERT(left);
   // left hand side should be a varrefexp or a thisOp
   const SgNodePtrList&          vars = NodeQuery::querySubTree(left, V_SgVarRefExp);
   SgNodePtrList::const_iterator it = vars.begin();

   for (; it != vars.end(); ++it) {
      SgVarRefExp* varRef = isSgVarRefExp(*it);
      ROSE_ASSERT(varRef);
      SgExpression* left = n->get_lhs_operand();
      if (left == varRef) {
         variable_access_arrowexp[n] = varRef;
         std::cerr << " &&& Adding : " << varRef->unparseToString() << std::endl;
      } else {
         std::cerr << " &&& Not adding varRef because on right hand side of -> :" << varRef->unparseToString() << std::endl;
         std::cerr << "   &&& left : " << left->unparseToString() << "  varRef: " << varRef << "  left:" << left << std::endl;
      }
   }

   if (vars.size() > 1) {
      std::cerr << "Warning : We added more than one SgVarRefExp to this map for SgArrowExp. This might be a problem" << std::endl;
      //exit(1);
   }
#if 1
   const SgNodePtrList&          vars2 = NodeQuery::querySubTree(left, V_SgThisExp);
   SgNodePtrList::const_iterator it2 = vars2.begin();
   for (; it2 != vars2.end(); ++it2) {
      SgThisExp* varRef = isSgThisExp(*it2);
      ROSE_ASSERT(varRef);
      variable_access_arrowthisexp[n] = varRef;
      std::cerr << " &&& Adding : " << varRef->unparseToString() << std::endl;
   }
   if (vars2.size() > 1) {
      std::cerr << "Warning : We added more than one SgThisExp to this map for SgArrowExp. This might be a problem" << std::endl;
      //exit(1);
   }
#endif
}

void
RtedTransformation::arrayHeapAlloc(SgInitializedName* initName, SgVarRefExp* varRef, SgExpression* sz, AllocKind ak)
{
  ROSE_ASSERT(initName && varRef && sz);

  // varRef can not be an array access, its only an array Create
  variablesUsedForArray.push_back(varRef);
  create_array_define_varRef_multiArray[varRef] = RtedArray(initName, getSurroundingStatement(varRef), ak, sz);
}

void RtedTransformation::arrayHeapAlloc1( SgInitializedName* initName,
                                          SgVarRefExp* varRef,
                                          SgExpressionPtrList& args,
                                          AllocKind ak
                                        )
{
  ROSE_ASSERT( args.size() == 1 );

  arrayHeapAlloc(initName, varRef, args[0], ak);
}

void RtedTransformation::arrayHeapAlloc2( SgInitializedName* initName,
                                          SgVarRefExp* varRef,
                                          SgExpressionPtrList& args,
                                          AllocKind ak
                                        )
{
  ROSE_ASSERT( args.size() == 2 );

  // \pp \note where are the nodes freed, that are created here?
  SgExpression* size_to_use = buildMultiplyOp(args[0], args[1]);

  arrayHeapAlloc(initName, varRef, size_to_use, ak);
}

AllocKind
RtedTransformation::arrayAllocCall( SgInitializedName* initName,
                                    SgVarRefExp* varRef,
                                    SgExprListExp* args,
                                    SgFunctionDeclaration* funcd,
                                    AllocKind default_result
                                  )
{
  ROSE_ASSERT(initName && varRef && args && funcd);

  AllocKind   howAlloced = default_result;
  std::string funcname = funcd->get_name().str();

  std::cerr << "... Detecting func call on right hand side : " << funcname << "     and size : "
            << args->unparseToString()
            << std::endl;

  if (funcname == "malloc")
  {
    howAlloced = akCHeap;
    arrayHeapAlloc1(initName, varRef, args->get_expressions(), howAlloced);
  }
  else if (funcname == "calloc")
  {
    howAlloced = akCHeap;
    arrayHeapAlloc2(initName, varRef, args->get_expressions(), howAlloced);
  }
  else if (funcname == "upc_alloc")
  {
    howAlloced = akUpcAlloc;
    arrayHeapAlloc1(initName, varRef, args->get_expressions(), howAlloced);
  }
  else if (funcname == "upc_local_alloc")
  {
    // deprecated
    howAlloced = akUpcAlloc; // has the same effect as upc_alloc
    arrayHeapAlloc2(initName, varRef, args->get_expressions(), howAlloced);
  }
  else if (funcname == "upc_all_alloc")
  {
    howAlloced = akUpcAllAlloc;
    arrayHeapAlloc2(initName, varRef, args->get_expressions(), howAlloced);
  }
  else if (funcname == "upc_global_alloc")
  {
    howAlloced = akUpcGlobalAlloc;
    arrayHeapAlloc2(initName, varRef, args->get_expressions(), howAlloced);
  }

  return howAlloced;
}

AllocKind RtedTransformation::arrayAllocCall(SgInitializedName* initName, SgVarRefExp* varRef, SgExprListExp* args, SgFunctionRefExp* funcr, AllocKind default_result)
{
  ROSE_ASSERT(varRef);

  // \pp \todo
  //   the function should probably return whether the allocation was handled
  //   thus making the default_result superfluous.
  //   However the original implementation does not reflect that,
  //   therefore I add the extra parameter.
  AllocKind res = default_result;

  if (funcr) {
    res = arrayAllocCall(initName, varRef, args, funcr->getAssociatedFunctionDeclaration(), default_result);
  } else {
     // right hand side of assign should only contain call to malloc somewhere
     std::cerr << "RtedTransformation: UNHANDLED AND ACCEPTED FOR NOW. Right of Assign : Unknown (Array creation) : "
               << "  line:" << varRef->unparseToString() << std::endl;
     //     ROSE_ASSERT(false);
  }

  return res;
}


/****************************************
 * This function returns InitializedName
 * for a DotExpr
 ****************************************/
static
SgVarRefExp* getRightOfDot(SgDotExp* dot)
{
  SgExpression*      rightDot = dot->get_rhs_operand();
  ROSE_ASSERT(rightDot);

  SgVarRefExp*       varRef = isSgVarRefExp(rightDot);
  ROSE_ASSERT(varRef);

  return varRef;
}


/****************************************
 * This function returns InitializedName
 * for a ArrowExp
 ****************************************/
static
SgVarRefExp* getRightOfArrow(SgArrowExp* arrow)
{
  SgExpression*      rightArrow = arrow->get_rhs_operand();
  ROSE_ASSERT(rightArrow);

  SgVarRefExp*       varRef = isSgVarRefExp(rightArrow);
  ROSE_ASSERT(varRef);

  return varRef;
}

/****************************************
 * This function returns InitializedName
 * for a PlusPlusOp
 ****************************************/
static
SgVarRefExp* getPlusPlusOp(SgPlusPlusOp* plus)
{
  SgExpression*      expPl = plus->get_operand();
  ROSE_ASSERT(expPl);

  SgVarRefExp*       varRef =  isSgVarRefExp(expPl);
  ROSE_ASSERT(varRef);

  return varRef;
}


/****************************************
 * This function returns InitializedName
 * for a DotExpr
 ****************************************/
static
SgVarRefExp* getRightOfPointerDeref(SgPointerDerefExp* dot)
{
  SgExpression* rightDot = dot->get_operand();
  ROSE_ASSERT(rightDot);

  SgVarRefExp*  varRef = isSgVarRefExp(rightDot);
  ROSE_ASSERT(varRef);

  return varRef;
}

static
SgVarRefExp* getRightOfDotStar(SgDotStarOp* dot)
{
  SgExpression*      rightDot = dot->get_rhs_operand();
  ROSE_ASSERT(rightDot);

  SgVarRefExp*       varRef = isSgVarRefExp(rightDot);
  ROSE_ASSERT(varRef);

  return varRef;
}

static
SgVarRefExp* getRightOfArrowStar(SgArrowStarOp* arrowstar)
{
  SgExpression*      rightArrow = arrowstar->get_rhs_operand();
  ROSE_ASSERT(rightArrow);

  SgVarRefExp*       varRef = isSgVarRefExp(rightArrow);
  ROSE_ASSERT(varRef);

  return varRef;
}


/****************************************
 * This function returns InitializedName
 * for a MinusMinusOp
 ****************************************/
static
SgVarRefExp* getMinusMinusOp(SgMinusMinusOp* minus)
{
  SgExpression*      expPl = minus->get_operand();
  ROSE_ASSERT(expPl);

  SgVarRefExp*       varRef = isSgVarRefExp(expPl);
  ROSE_ASSERT(varRef);

  return varRef;
}

namespace
{
  struct VarrefFinderResult
  {
      static
      SgInitializedName* declOf(SgVarRefExp* vref)
      {
        ROSE_ASSERT(vref && vref->get_symbol());

        SgInitializedName* iname = vref->get_symbol()->get_declaration();
        ROSE_ASSERT(iname);

        return iname;
      }

      explicit
      VarrefFinderResult(SgVarRefExp* vref)
      : varf(vref), skip(false)
      {}

      VarrefFinderResult()
      : varf(NULL), skip(true)
      {}

      SgInitializedName* initname()  const { return declOf(varf); }
      SgVarRefExp*       varref()    const { return varf; }
      bool               ignore()    const { return skip; }

    private:
      SgVarRefExp*       varf;
      bool               skip;
  };

  struct VarrefFinder
  {
    typedef VarrefFinderResult Result;

    Result res;

    VarrefFinder()
    : res(NULL)
    {}

    void set_result(SgVarRefExp* n)
    {
      ROSE_ASSERT(n);
      res = Result(n);
    }

    void failed()
    {
      res = Result();
    }

    void handle(SgNode&) { assert(false); }

    void handle(SgVarRefExp& n)
    {
      set_result(&n);
    }

    void handle_dot(SgDotExp& n)
    {
      set_result(getRightOfDot(&n));
    }

    void handle_arrow(SgArrowExp& n)
    {
      set_result(getRightOfArrow(&n));
    }

    void handle_incr(SgPlusPlusOp& n)
    {
      set_result(getPlusPlusOp(&n));
    }

    void handle_decr(SgMinusMinusOp& n)
    {
      set_result(getMinusMinusOp(&n));
    }

    void handle_pointerderef(SgPointerDerefExp& n)
    {
      set_result(getRightOfPointerDeref(&n));
    }

    void handle(SgPntrArrRefExp& n)
    {
      // \pp maybe we should just invoke visitSgNode(VarInfoFinder(), n.get_lhs_operand())

      SgPntrArrRefExp* arrref = &n;
      while ( arrref )
      {
        SgExpression* exp = arrref->get_lhs_operand();

        arrref = NULL;
        if (SgVarRefExp* varRef = isSgVarRefExp(exp))
        {
          set_result(varRef);
        }
        else if (SgDotExp* dotexp = isSgDotExp(exp))
        {
          handle_dot(*dotexp);
        }
        else if (SgArrowExp* arrowexp = isSgArrowExp(exp))
        {
          handle_arrow(*arrowexp);
        }
        else if (SgPointerDerefExp* ptrderef = isSgPointerDerefExp(exp))
        {
          handle_pointerderef(*ptrderef);
        }
        else if (SgPntrArrRefExp* arrrefexp = isSgPntrArrRefExp(exp))
        {
          // loop variable update
          arrref = arrrefexp;
        }
        else
        {
          std::cerr << "RtedTransformation : Left of pntrArr2 - Unknown : " << exp->class_name() << std::endl;
          ROSE_ASSERT(false);
        }
      }
    }

    void handle(SgPointerDerefExp& n)
    {
      SgExpression* exp = n.get_operand();

      if (SgVarRefExp* varrefexp = isSgVarRefExp(exp))
      {
        set_result(varrefexp);
      }
      else if (SgPlusPlusOp* plusplus = isSgPlusPlusOp(exp))
      {
        handle_incr(*plusplus);
      }
      else if (SgMinusMinusOp* minusminus = isSgMinusMinusOp(exp))
      {
        handle_decr(*minusminus);
      }
      else if (SgDotExp* dotexp = isSgDotExp(exp))
      {
        handle_dot(*dotexp);
      }// ------------------------------------------------------------
      else if (SgPointerDerefExp* ptrderefexp = isSgPointerDerefExp(exp))
      {
        handle_pointerderef(*ptrderefexp);
      }// ------------------------------------------------------------
      else if (SgArrowExp* arrowexp = isSgArrowExp(exp))
      {
        handle_arrow(*arrowexp);
      }// ------------------------------------------------------------
      else if (isSgCastExp(exp))
      {
         // \pp this does not seem to be the right thing to do
         const SgNodePtrList& vars = NodeQuery::querySubTree(exp, V_SgVarRefExp);
         ROSE_ASSERT( vars.size() > 0 );

         set_result(isSgVarRefExp(vars[0]));
      }
      else if (isSgAddOp(exp) || isSgSubtractOp(exp))
      {
        // \pp look on left side?
        failed();
      }
      else
      {
        ROSE_ASSERT(exp);
        std::cerr << "RtedTransformation : PointerDerefExp - Unknown : " << exp->class_name() << "  line:" << n.unparseToString() << std::endl;
      }
    }

    void handle(SgDotExp& n)
    {
      handle_dot(n);
    }

    void handle(SgArrowExp& n)
    {
      handle_arrow(n);
    }

    void handle(SgFunctionCallExp& n)
    {
      std::cerr << "RtedTransformation: UNHANDLED BUT ACCEPTED FOR NOW - Left of assign - line:" << n.unparseToString() << std::endl;

      SgExpression* exp = n.get_function();
      SgDotExp*     dotexp = isSgDotExp(exp);

      if (dotexp)
      {
         SgExpression* const     rightDot = dotexp->get_rhs_operand();
         SgExpression* const     leftDot = dotexp->get_lhs_operand();
         ROSE_ASSERT(rightDot && leftDot);

         SgVarRefExp* const      varRefL = isSgVarRefExp(leftDot);
         SgMemberFunctionRefExp* varRefR = isSgMemberFunctionRefExp(rightDot);

         if (varRefL && varRefR)
         {
            set_result(varRefL);
         }
      }
    }

    void handle(SgArrowStarOp& n)
    {
      set_result(getRightOfArrowStar(&n));
    }

    void handle(SgDotStarOp& n)
    {
      set_result(getRightOfDotStar(&n));
    }

    operator Result() { return res; }
  };
}


// TODO 2 djh:  rewrite this function to be more robust
//  i.e. handle general cases
//  consider whether getting the initname is important
void RtedTransformation::visit_isArraySgAssignOp(SgAssignOp* const assign)
{
   ROSE_ASSERT(assign);

   std::cerr << "   ::: Checking assignment : " << assign->unparseToString() << std::endl;

   SgExpression* const           expr_l = assign->get_lhs_operand();
   VarrefFinder::Result          res = sg::dispatch(VarrefFinder(), expr_l);

   if (!res.varref())
   {
     // did not find anything meaningful
     ROSE_ASSERT(res.ignore()); // breaks on unexpected code

     std::cerr << "#WARNING: unhandled assignment operation: '" << assign->unparseToString() << "'" << std::endl;
     return;
   }

   SgInitializedName* const      initName = res.initname();
   SgVarRefExp* const            varRef = res.varref();
   ROSE_ASSERT(initName && initName->get_type() && varRef);

   // handle MALLOC: function call
   SgExpression* const           expr_r = assign->get_rhs_operand();
   AllocKind                     last_alloc = akUndefined;
   const SgNodePtrList&          calls = NodeQuery::querySubTree(expr_r, V_SgFunctionCallExp);
   SgNodePtrList::const_iterator it = calls.begin();
   for (; it != calls.end(); ++it) {
      SgFunctionCallExp* funcc = isSgFunctionCallExp(*it);
      ROSE_ASSERT(funcc);

      SgExprListExp*     size = deepCopy(funcc->get_args());
      ROSE_ASSERT(size);

      SgExpression*      func = funcc->get_function();

      last_alloc = arrayAllocCall(initName, varRef, size, isSgFunctionRefExp(func), last_alloc);
   }

   // FIXME 3: This won't handle weird cases with, e.g. multiple news on the rhs,
   //    but for those cases this entire function is probably broken.  Consider,
   //    e.g.
   //        int *a, *b;
   //        a = ( b = new int, new int );
   //
   // handle new (implicit C++ malloc)
   const SgNodePtrList& newnodes = NodeQuery::querySubTree( expr_r, V_SgNewExp );
   BOOST_FOREACH(SgNode* exp, newnodes)
   {
      // FIXME 2: this is a false positive if operator new is overloaded
      SgNewExp* new_op = isSgNewExp(exp);
      ROSE_ASSERT( new_op );

      const AllocKind allocKind = cxxHeapAllocKind(new_op->get_type());

      arrayHeapAlloc(initName, varRef, buildSizeOfOp(new_op -> get_specified_type()), allocKind);
   }

   // ---------------------------------------------
   // handle variables ..............................
   // here we should know the initName of the variable on the left hand side
   // we now know that this variable must be initialized
   // if we have not set this variable to be initialized yet,
   // we do so
   std::cerr << ">> Setting this var to be initialized : " << initName->unparseToString() << std::endl;
   variableIsInitialized[varRef] = InitializedVarMap::mapped_type(initName, last_alloc);
}

void RtedTransformation::addPaddingToAllocatedMemory(SgStatement* stmt, const RtedArray& array)
{
    ROSE_ASSERT(stmt);
    // if you find this:
    //   str1 = ((char *)(malloc(((((4 * n)) * (sizeof(char )))))));
    // add the following lines:
    //   int i;
    //   for (i = 0; (i) < malloc(((((4 * n)) * (sizeof(char )); i++)
    //     str1[i] = ' ';

    // we do this only for char*
    SgInitializedName* initName = array.initName;
    SgType*            type = initName->get_type();
    ROSE_ASSERT(type);
    std::cerr << " Padding type : " << type->class_name() << std::endl;

    // \pp \todo do we need to skip modifiers?
    if (!isSgPointerType(type)) return;

    SgType* basetype = isSgPointerType(type)->get_base_type();
    std::cerr << " Base type : " << basetype->class_name() << std::endl;

    // since this is mainly to handle char* correctly, we only deal with one dim array for now
    if (basetype && isSgTypeChar(basetype) && array.getDimension() == 1)
    {
      // allocated size
      SgScopeStatement* scope = stmt->get_scope();
      SgExpression* size = array.getIndices()[0];
      pushScopeStack(scope);
      // int i;
      SgVariableDeclaration* stmt1 = buildVariableDeclaration("i", buildIntType(), NULL);
      //for(i=0;..)
      SgStatement* init_stmt = buildAssignStatement(buildVarRefExp("i"), buildIntVal(0));

      // for(..,i<size,...) It is an expression, not a statement!
      SgExprStatement* cond_stmt = NULL;
      cond_stmt = buildExprStatement(buildLessThanOp(buildVarRefExp("i"), size));

      // for (..,;...;i++); not ++i;
      SgExpression* incr_exp = NULL;
      incr_exp = buildPlusPlusOp(buildVarRefExp("i"), SgUnaryOp::postfix);
      // loop body statement
      SgStatement* loop_body = NULL;
      SgExpression* lhs = buildPntrArrRefExp(buildVarRefExp(array.initName->get_name()), buildVarRefExp("i"));
      SgExpression* rhs = buildCharVal(' ');
      loop_body = buildAssignStatement(lhs, rhs);
      //loop_body = buildExprStatement(stmt2);


      SgForStatement* forloop = buildForStatement(init_stmt, cond_stmt, incr_exp, loop_body);

      SgBasicBlock* bb = buildBasicBlock(stmt1, forloop);
      insertStatementAfter(stmt, bb);
      std::string comment = "RS: Padding this newly generated array with empty space.";
      attachComment(bb, comment, PreprocessingInfo::before);
      popScopeStack();
   }
}


namespace
{
  struct VarRefFinder
  {
    SgVarRefExp* res;

    VarRefFinder()
    : res(NULL)
    {}

    void handle(SgNode&) { assert(false); }

    void handle_dotarrow(SgBinaryOp& n)
    {
      res = isSgVarRefExp(n.get_rhs_operand());
    }

    void handle(SgDotExp& n) { handle_dotarrow(n); }
    void handle(SgArrowExp& n) { handle_dotarrow(n); }

    void handle(SgPointerDerefExp& n)
    {
      res = isSgVarRefExp(n.get_operand());
    }

    void handle(SgPntrArrRefExp& n)
    {
      res = sg::dispatch(VarRefFinder(), n.get_lhs_operand());
    }

    void handle(SgVarRefExp& n) { res = &n; }

    operator SgVarRefExp*() { return res; }
  };

  struct ArrayInfoFinder
  {
    SgVarRefExp* varref;

    ArrayInfoFinder()
    : varref(NULL)
    {}

    void set_varref(SgVarRefExp* n)
    {
      ROSE_ASSERT(n);
      varref = n;
    }

    void handle_member_selection(SgBinaryOp& n)
    {
      set_varref(isSgVarRefExp(n.get_rhs_operand()));
    }

    void handle(SgNode&) { ROSE_ASSERT(false); }

    void handle(SgVarRefExp& n)
    {
      set_varref(&n);
    }

    void handle(SgArrowExp& n)
    {
      handle_member_selection(n);
    }

    void handle(SgDotExp& n)
    {
      handle_member_selection(n);
    }

    void handle(SgPointerDerefExp& n)
    {
      set_varref(isSgVarRefExp(n.get_operand()));
    }

    void handle(SgPntrArrRefExp& n)
    {
      SgExpression* lhs = n.get_lhs_operand();
      SgVarRefExp*  res = sg::dispatch(VarRefFinder(), lhs);

      set_varref(res);
    }

    operator SgVarRefExp*() const
    {
      ROSE_ASSERT(varref);
      return varref;
    }
  };
}

void RtedTransformation::visit_isArrayPntrArrRefExp(SgPntrArrRefExp* const arrRefExp)
{
    ROSE_ASSERT(arrRefExp);

    // make sure the parent is not another pntr array (pntr->pntr), we only want the top one
    // also, ensure we don't count arr[ix].member as an array access, e.g. in the
    // following:
    //    arr[ix].member = 2;
    // we need only checkwrite &( arr[ix].member ), which is handled by init var.
    if (isSgPntrArrRefExp(arrRefExp->get_parent())) return;

    // right hand side can be any expression!
    SgExpression*                             left = arrRefExp->get_lhs_operand();
    SgVarRefExp*                              varref = sg::dispatch(ArrayInfoFinder(), left);
    std::vector<SgVarRefExp*>::const_iterator aa = variablesUsedForArray.begin();
    std::vector<SgVarRefExp*>::const_iterator zz = variablesUsedForArray.end();

    // \pp why do we create an array access call if we do NOT find the varref?
    const bool                                create_access_call = (std::find(aa, zz, varref) == zz);

    if (create_access_call)
    {
       SgInitializedName* initName = varref->get_symbol()->get_declaration();
       ROSE_ASSERT(initName);

       std::cerr << "!! CALL : " << varref << " - " << varref->unparseToString() << "    size : " << create_array_access_call.size()
                 << " : " << arrRefExp->unparseToString() << std::endl;

       // \todo distinguish between akStack and akGlobal
       create_array_access_call[arrRefExp] = RtedArray(initName, getSurroundingStatement(arrRefExp), akStack);
    }
}


#endif


#if 0

   if (varRef) {
      // is variable on left side
      // could be something like int** pntr; pntr = malloc ... (double array)
      // assume in this case explicitly pntr[indx1]=...
      initName = varRef->get_symbol()->get_declaration();
   } // ------------------------------------------------------------
   else if (pntrArr) {
      // is array on left side
      // could be pntr[indx1]  or pntr[indx1][indx2]
      SgExpression* expr_ll = pntrArr->get_lhs_operand();
      ROSE_ASSERT(expr_ll);
      varRef = isSgVarRefExp(expr_ll);
      if (varRef) {
         // we assume pntr[indx1] = malloc
         initName = varRef->get_symbol()->get_declaration();
      } else if (isSgPntrArrRefExp(expr_ll)) {
         SgPntrArrRefExp* pntrArr2 = isSgPntrArrRefExp(expr_ll);
         ROSE_ASSERT(pntrArr2);
         SgExpression* expr_lll = pntrArr2->get_lhs_operand();
         varRef = isSgVarRefExp(expr_lll);
         if (varRef) {
            // we assume pntr[indx1][indx2] = malloc
            initName = varRef->get_symbol()->get_declaration();
         } else if (isSgDotExp(expr_lll)) {
            std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_lll),
                  "Left of pntrArr2 - Right of Dot  - line: " + expr_lll->unparseToString() + " ", varRef);
            initName = mypair.first;
            varRef = mypair.second;
            if (initName)
               ROSE_ASSERT(varRef);
         } else if (isSgArrowExp(expr_lll)) {
            std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfArrow(isSgArrowExp(expr_lll),
                  "Left of pntrArr2 - Right of Arrow  - line: " + expr_lll->unparseToString() + " ", varRef);
            initName = mypair.first;
            varRef = mypair.second;
            if (initName)
               ROSE_ASSERT(varRef);
         } else if (isSgPointerDerefExp(expr_lll)) {
            varRef = isSgVarRefExp(isSgPointerDerefExp(expr_lll) -> get_operand());
            ROSE_ASSERT( varRef );
            initName = varRef -> get_symbol() -> get_declaration();
         } else {
            std::cerr << "RtedTransformation : Left of pntrArr2 - Unknown : " << expr_lll->class_name() << std::endl;
            ROSE_ASSERT(false);
         }
      } else if (isSgDotExp(expr_ll)) {
         std::cerr << "RtedTransformation : isSgDotExp : " << std::endl;

         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_ll),
               "Left of pntrArr - Right of Dot  - line: " + expr_ll->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         if (initName)
            ROSE_ASSERT(varRef);
      } else if (isSgPointerDerefExp(expr_ll)) {
         std::cerr << "RtedTransformation : isSgPointerDerefExp : " << std::endl;

         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfPointerDeref(isSgPointerDerefExp(expr_ll),
               "Left of pntrArr - Right of PointerDeref  - line: " + expr_ll->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         if (initName)
            ROSE_ASSERT(varRef);
      } else if (isSgArrowExp(expr_ll)) {
         std::cerr << "RtedTransformation : isSgArrowExp : " << std::endl;
         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfArrow(isSgArrowExp(expr_ll),
               "Left of pntrArr - Right of Arrow  - line: " + expr_ll->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         if (initName)
            ROSE_ASSERT(varRef);
      } else {
         std::cerr << "RtedTransformation : Left of pntrArr - Unknown : " << expr_ll->class_name() << "  line:"
               << expr_ll->unparseToString() << std::endl;
         ROSE_ASSERT(false);
      }
   } // ------------------------------------------------------------
   else if (isSgDotExp(expr_l)) {
      std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_l), "Right of Dot  - line: "
            + expr_l->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      ROSE_ASSERT(!initName || varRef);
   }// ------------------------------------------------------------
   else if (isSgArrowExp(expr_l)) {
      std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfArrow(isSgArrowExp(expr_l), "Right of Arrow  - line: "
            + expr_l->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      ROSE_ASSERT(!initName || varRef);
   } // ------------------------------------------------------------
   else if (pointerDeref) {
      SgExpression* exp = pointerDeref->get_operand();
      ROSE_ASSERT(exp);
      if (isSgPlusPlusOp(exp)) {
         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getPlusPlusOp(isSgPlusPlusOp(exp),
               "PointerDerefExp PlusPlus  - line: " + pointerDeref->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         ROSE_ASSERT(varRef);
      } else if (isSgMinusMinusOp(exp)) {
         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getMinusMinusOp(isSgMinusMinusOp(exp),
               "PointerDerefExp MinusMinus  - line: " + pointerDeref->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         ROSE_ASSERT(varRef);
      } else if (isSgVarRefExp(exp)) {
         initName = isSgVarRefExp(exp)->get_symbol()->get_declaration();
         varRef = isSgVarRefExp(exp);
         ROSE_ASSERT(varRef);
      } else if (isSgDotExp(exp)) {
         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(exp), "Right of Dot  - line: "
               + exp->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         ROSE_ASSERT(!initName || varRef);
      }// ------------------------------------------------------------
      else if (isSgPointerDerefExp(exp)) {
         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfPointerDeref(isSgPointerDerefExp(exp),
               "Right of PointerDeref  - line: " + exp->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         ROSE_ASSERT(!initName || varRef);
      }// ------------------------------------------------------------
      else if (isSgArrowExp(exp)) {
         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfArrow(isSgArrowExp(exp),
               "Right of PointerDeref  - line: " + exp->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         ROSE_ASSERT(!initName || varRef);
      }// ------------------------------------------------------------
      else if (isSgCastExp(exp)) {
         const SgNodePtrList& vars = NodeQuery::querySubTree(exp, V_SgVarRefExp);
         ROSE_ASSERT( vars.size() > 0 );

         varRef = isSgVarRefExp(vars[0]);
         ROSE_ASSERT( varRef );

         initName = varRef -> get_symbol() -> get_declaration();
      }// ------------------------------------------------------------
      else if (isSgAddOp(exp) || isSgSubtractOp(exp))
      {
        // \pp this case is not handled by the original RTED code
        //     --> postpone until later
        return;
      }
      else {
         std::cerr << "RtedTransformation : PointerDerefExp - Unknown : " << exp->class_name() << "  line:"
               << pointerDeref->unparseToString() << std::endl;

         //      ROSE_ASSERT(false);
      }
   } // ------------------------------------------------------------
   else if (isSgFunctionCallExp(expr_l)) {
      std::cerr << "RtedTransformation: UNHANDLED BUT ACCEPTED FOR NOW - Left of assign - Unknown : " << expr_l->class_name()
            << "  line:" << expr_l->unparseToString() << std::endl;
#if 1
      SgFunctionCallExp* expcall = isSgFunctionCallExp(expr_l);
      SgExpression* exp = expcall->get_function();
      if (exp && isSgDotExp(exp)) {
         SgExpression* rightDot = isSgDotExp(exp)->get_rhs_operand();
         SgExpression* leftDot = isSgDotExp(exp)->get_lhs_operand();
         ROSE_ASSERT(rightDot);
         ROSE_ASSERT(leftDot);
         SgVarRefExp* varRefL = isSgVarRefExp(leftDot);
         SgMemberFunctionRefExp* varRefR = isSgMemberFunctionRefExp(rightDot);
         if (varRefL && varRefR) {
            // variable is on the left hand side
            varRef = varRefL;
            initName = (varRef)->get_symbol()->get_declaration();
            ROSE_ASSERT(initName && varRef);
         }
      }// ------------------------------------------------------------
#endif
   } else if (isSgArrowStarOp(expr_l)) {
      std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfArrowStar(isSgArrowStarOp(expr_l),
            "Right of Arrow  - line: " + expr_l->unparseToString() + " ", varRef);
      ROSE_ASSERT(mypair.first);
      ROSE_ASSERT(mypair.second);
      initName = mypair.first;
      varRef = mypair.second;
      ROSE_ASSERT(!initName || varRef);
   }// ------------------------------------------------------------
   else if (isSgDotStarOp(expr_l)) {
      std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfDotStar(isSgDotStarOp(expr_l), "Right of Dot  - line: "
            + expr_l->unparseToString() + " ", varRef);
      initName = mypair.first;
      varRef = mypair.second;
      ROSE_ASSERT(!initName || varRef);
   }// ------------------------------------------------------------
   else {
      std::cerr << "RtedTransformation : Left of assign - Unknown : " << expr_l->class_name() << "  line:"
            << expr_l->unparseToString() << std::endl;
      ROSE_ASSERT(false);
   }
   std::cerr << " expr_l : " << expr_l->class_name() << std::endl;
#endif
