// vim:et sta sw=4 ts=4:
#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <algorithm>

#include <boost/foreach.hpp>

#include "rosez.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

static
inline
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

  bool operator()(const std::map<SgVarRefExp*, RtedArray*>::value_type& v) const
  {
    return v.second->initName == obj;
  }
};


/* -----------------------------------------------------------
 * Is the Initialized Name already known as an array element ?
 * -----------------------------------------------------------*/
bool RtedTransformation::isVarRefInCreateArray(SgInitializedName* search)
{
  using std::find_if;

  if (create_array_define_varRef_multiArray_stack.find(search) != create_array_define_varRef_multiArray_stack.end())
    return true;

  std::map<SgVarRefExp*, RtedArray*>::iterator aa = create_array_define_varRef_multiArray.begin();
  std::map<SgVarRefExp*, RtedArray*>::iterator zz = create_array_define_varRef_multiArray.end();

  return std::find_if(aa, zz, InitNameComp(search)) != zz;
}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateCall
 * -----------------------------------------------------------*/
void RtedTransformation::insertArrayCreateCall(SgVarRefExp* n, RtedArray* value)
{
   ROSE_ASSERT(n && value);

   SgInitializedName* initName = n->get_symbol()->get_declaration();
   ROSE_ASSERT(initName);

   SgStatement* stmt = value->surroundingStatement;
   ROSE_ASSERT(stmt);

   insertArrayCreateCall(stmt, initName, n, value);
}


void RtedTransformation::insertArrayCreateCall(SgInitializedName* initName, RtedArray* value) {
   ROSE_ASSERT(initName && value);

   SgVarRefExp* var_ref = genVarRef(initName);
   SgStatement* stmt = value->surroundingStatement;
   ROSE_ASSERT(stmt);

   insertArrayCreateCall(stmt, initName, var_ref, value);
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
    case akStack:          res = "akStack"; break;
    case akCHeap:          res = "akCHeap"; break;

    /* C++ */
    case akCxxNew:         res = "akCxxNew"; break;
    case akCxxArrayNew:    res = "akCxxArrayNew"; break;

    /* UPC */
    case akUpcSharedHeap:  res = "akUpcSharedHeap"; break;
    case akUpcAlloc:       res = "akUpcAlloc"; break;
    case akUpcGlobalAlloc: res = "akUpcGlobalAlloc"; break;
    case akUpcAllAlloc:    res = "akUpcAllAlloc"; break;

    default:               ROSE_ASSERT(false);
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
RtedTransformation::buildArrayCreateCall(SgInitializedName* initName, SgVarRefExp* varRef, RtedArray* array, SgStatement* stmt)
{
   // build the function call:  rs.createHeapArr(...);
   //                        or rs.createHeapPtr(...);
   SgExprListExp*     arg_list = buildExprListExp();
   const bool         parent_exists = (varRef->get_parent() != NULL);
   SgExpression*      src_exp = parent_exists ? getExprBelowAssignment(varRef)
                                              : varRef
                                              ;
   SgType*            src_type = src_exp->get_type();
   const bool         isCreateHeapArr = (src_type->class_name() == "SgArrayType");

   // what kind of types do we get?
   ROSE_ASSERT(isCreateHeapArr || src_type->class_name() == "SgPointerType");

   // if we have an array, then it has to be on the stack
   // \pp \todo handle C++ new[]
   ROSE_ASSERT(!isCreateHeapArr || array->allocKind == akStack);

   appendExpression(arg_list, ctorTypeDesc(mkTypeInformation(NULL, src_type)));

   SgScopeStatement*  scope = get_scope(initName);
#if 0
   // \pp ???
   SgClassDefinition* unionclass = isSgClassDefinition(scope);
   bool               isUnionClass = (unionclass && unionclass->get_declaration()->get_class_type() == SgClassDeclaration::e_union);
#endif

   appendAddressAndSize(arg_list, Whole, scope, src_exp, NULL /* unionclass */);

   //SgIntVal* ismalloc = buildIntVal( 0 );
   SgExpression*      size = array -> size;

   if (size)
   {
     size = buildCastExp(size, buildUnsignedLongType());
   }
   else
   {
     ROSE_ASSERT( array -> allocKind == akStack );
     size = buildIntVal(0);  // \pp this seems always to give 0 when allocKind == akStack ...
   }

   appendExpression(arg_list, size);

   // target specific parameters
   if (isCreateHeapArr)
   {
     appendDimensions(arg_list, array);

     // source name
     appendExpression(arg_list, buildStringVal(initName->get_name()));

     // mangeled name
     appendExpression(arg_list, buildStringVal(initName->get_mangled_name().str()));
   }
   else
   {
     // track whether heap memory was allocated via malloc or new, to ensure
     // that free/delete matches
     appendExpression(arg_list, mkAllocKind(array->allocKind));
   }

   appendClassName(arg_list, initName->get_type());
   appendFileInfo(arg_list, stmt);

   // create the call nodes
   SgFunctionSymbol*  rted_fun = isCreateHeapArr ? symbols.roseCreateHeapArr
                                                 : symbols.roseCreateHeapPtr
                                                 ;

   ROSE_ASSERT(rted_fun != NULL);
   SgFunctionRefExp*  memRef_r = buildFunctionRefExp(rted_fun);
   SgExprStatement*   exprStmt = buildFunctionCallStmt(memRef_r, arg_list);

   return exprStmt;
}

void RtedTransformation::insertArrayCreateCall( SgStatement* stmt,
                                                SgInitializedName* initName,
                                                SgVarRefExp* varRef,
                                                RtedArray* array
                                              )
{
   ROSE_ASSERT(stmt && varRef && initName && array);

   // Skipping extern arrays because they will be handled in the defining
   //   translation unit.
   // \pp \todo Maybe we should check that there really is one...
   if (isGlobalExternVariable(stmt)) return;

   bool                 global_stmt = false;
   SgScopeStatement*    scope = stmt->get_scope();
   ROSE_ASSERT(scope);

   // what if there is an array creation within a ClassDefinition
   if (isSgClassDefinition(scope)) {
       // new stmt = the classdef scope
       SgClassDeclaration* decl = isSgClassDeclaration(scope->get_parent());
       ROSE_ASSERT(decl);
       stmt = isSgVariableDeclaration(decl->get_parent());
       if (!stmt) {
         cerr << " Error . stmt is unknown : " << decl->get_parent()->class_name() << endl;
         ROSE_ASSERT( false );
       }
       scope = scope->get_scope();
       // We want to insert the stmt before this classdefinition, if its still in a valid block
       cerr << " ....... Found ClassDefinition Scope. New Scope is : " << scope->class_name() << "  stmt:"
             << stmt->class_name() << endl;
   }
   // what is there is an array creation in a global scope
   else if (isSgGlobal(scope)) {
       scope = mainBody;
       global_stmt = true;
   }

   if (isSgIfStmt(scope)) {
       SgStatement* exprStmt = buildArrayCreateCall(initName, varRef, array, stmt);
       ROSE_ASSERT(exprStmt);
       // get the two bodies of the ifstmt and prepend to them
       cerr << "If Statment : inserting createHeap" << endl;
       SgStatement* trueb = isSgIfStmt(scope)->get_true_body();
       SgStatement* falseb = isSgIfStmt(scope)->get_false_body();
       bool partOfTrue = traverseAllChildrenAndFind(varRef, trueb);
       bool partOfFalse = traverseAllChildrenAndFind(varRef, falseb);
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
       } else if (partOfCondition) {
         // \pp \todo why do we modify the false branch, if it was a condition?
         // \pp \note this branch looks dead b/c partOfCondition is tested before
         ROSE_ASSERT(false);

         // create false statement, this is sometimes needed
         SgBasicBlock* bb = buildBasicBlock();
         bb->set_parent(isSgIfStmt(scope));
         isSgIfStmt(scope)->set_false_body(bb);
         prependStatement(exprStmt, isSgScopeStatement(bb));
       }
   } else if (isSgBasicBlock(scope)) {
       SgStatement* exprStmt = buildArrayCreateCall(initName, varRef, array, stmt);

      if (global_stmt && initName->get_scope() != mainFirst->get_scope()) {
         mainBody -> prepend_statement(exprStmt);
         cerr << "+++++++ insert Before... " << endl;
      } else {
         // insert new stmt (exprStmt) after (old) stmt
         insertStatementAfter(stmt, exprStmt);
         cerr << "+++++++ insert After... " << endl;
      }

      string empty_comment = "";
      attachComment(exprStmt, empty_comment, PreprocessingInfo::before);
      string
            comment =
                  "RS : Create Array Variable, paramaters : (name, manglname, typr, basetype, address, sizeof(type), array size, fromMalloc, filename, linenr, linenrTransformed, dimension info ...)";
      attachComment(exprStmt, comment, PreprocessingInfo::before);
  } else {
      const std::string    name = initName->get_mangled_name().str();

      cerr << "RuntimeInstrumentation :: WARNING - Scope not handled!!! : " << name << " : " << scope->class_name() << endl;
      ROSE_ASSERT(isSgNamespaceDefinitionStatement(scope));
  }

   // unfortunately the arrays are filled with '\0' which is a problem
   // for detecting other bugs such as not null terminated strings
   // therefore we call a function that appends code to the
   // original program to add padding different from '\0'
   if (array->allocKind != akStack)
      addPaddingToAllocatedMemory(stmt, array);
}

/* -----------------------------------------------------------
 * Perform Transformation: insertArrayCreateAccessCall
 * -----------------------------------------------------------*/
void RtedTransformation::insertArrayAccessCall(SgExpression* arrayExp, RtedArray* value)
{
   ROSE_ASSERT( arrayExp && value );

   SgStatement* stmt = value->surroundingStatement;
   ROSE_ASSERT( stmt );

   insertArrayAccessCall(stmt, arrayExp, value);
}

void RtedTransformation::insertArrayAccessCall(SgStatement* stmt, SgExpression* arrayExp, RtedArray* array)
{
  SgScopeStatement* scope = stmt->get_scope();
  SgPntrArrRefExp*  arrRefExp = isSgPntrArrRefExp(arrayExp);

  ROSE_ASSERT(scope);
  ROSE_ASSERT(arrRefExp);

  // Recursively check each dimension of a multidimensional array access.
  // This doesn't matter for stack arrays, since they're contiguous and can
  // therefore be conceptually flattened, but it does matter for double
  // pointer array access.
  if (isSgPntrArrRefExp(arrRefExp -> get_lhs_operand())) {
     //      a[ i ][ j ] = x;
     //      x = a[ i ][ j ];
     // in either case, a[ i ] is read, and read before a[ i ][ j ].
     insertArrayAccessCall(stmt, arrRefExp -> get_lhs_operand(), array);
  }

  int read_write_mask = 0;
  // determine whether this array access is a read or write
  SgNode* iter = arrayExp;
  do {
     SgNode* child = iter;
     iter = iter->get_parent();
     SgBinaryOp* binop = isSgBinaryOp(iter);

     if (isSgAssignOp(iter)) {
        ROSE_ASSERT( binop );

        // lhs write only, rhs read only
        if (binop->get_lhs_operand() == child)
           read_write_mask |= Write;
        else
           read_write_mask |= Read;
        // regardless of which side arrayExp was on, we can stop now
        break;
     } else if (isSgAndAssignOp(iter) || isSgDivAssignOp(iter) || isSgIorAssignOp(iter) || isSgLshiftAssignOp(iter)
           || isSgMinusAssignOp(iter) || isSgModAssignOp(iter) || isSgMultAssignOp(iter) || isSgPlusAssignOp(iter)
           || isSgPointerAssignOp(iter) || isSgRshiftAssignOp(iter) || isSgXorAssignOp(iter)) {

        ROSE_ASSERT( binop );
        // lhs read & write, rhs read only
        read_write_mask |= Read;
        if (binop->get_lhs_operand() == child)
           read_write_mask |= Write;
        // regardless of which side arrayExp was on, we can stop now
        break;
     } else if (isSgPntrArrRefExp(iter)) {
        // outer[ inner[ ix ]] = val;
        //  inner[ ix ]  is only a read
        break;
     } else if (isSgDotExp(iter)) {
        ROSE_ASSERT( binop );
        if (child == binop -> get_lhs_operand()) {
           // arr[ ix ].member is neither a read nor write of the array
           // itself.
           break;
        } // else foo.arr[ ix ] depends on parent context, so keep going
     }
  } while (iter);
  // always do a bounds check
  read_write_mask |= BoundsCheck;

  // for contiguous array, base is at &array[0] whether on heap or on stack
  SgPntrArrRefExp*  array_base = deepCopy(arrRefExp);

  array_base -> set_rhs_operand(buildIntVal(0));

  SgExprListExp*    arg_list = buildExprListExp();

  appendAddress(arg_list, array_base);
  appendAddressAndSize(arg_list, Whole, NULL, arrRefExp, NULL);

  appendExpression(arg_list, ctorAddressDesc(mkAddressDesc(array_base)));
  appendExpression(arg_list, buildIntVal(read_write_mask));
  appendFileInfo(arg_list, stmt);

  ROSE_ASSERT(symbols.roseAccessHeap);
  insertCheck( ilBefore,
               stmt,
               symbols.roseAccessHeap,
               arg_list,
               "RS : Access Array Variable, paramaters : (name, dim 1 location, dim 2 location, read_write_mask, filename, linenr, linenrTransformed, part of error message)"
              );
}

void RtedTransformation::populateDimensions(RtedArray* array, SgInitializedName* init, SgArrayType* type_) {
   std::vector<SgExpression*>& indices = array -> getIndices();

   bool implicit_index = false;
   SgType* type = type_;
   while (isSgArrayType(type)) {
      SgExpression* index = isSgArrayType(type) -> get_index();
      if (index)
         indices.push_back(index);
      else
         implicit_index = true;
      type = isSgArrayType(type) -> get_base_type();
   }

   // handle implicit first dimension for array initializers
   // for something like
   //      int p[][2][3] = {{{ 1, 2, 3 }, { 4, 5, 6 }}}
   //  we can calculate the first dimension as
   //      sizeof( p ) / ( sizeof( int ) * 2 * 3 )
   if (implicit_index) {
      SgType* uint = buildUnsignedIntType();
      Sg_File_Info* file_info = init -> get_file_info();

      std::vector<SgExpression*>::iterator i = indices.begin();
      SgExpression* denominator = buildSizeOfOp(type);
      while (i != indices.end()) {
         denominator = new SgMultiplyOp(file_info, denominator, *i, uint);
         ++i;
      }
      ROSE_ASSERT( denominator != NULL );

      indices.insert(indices.begin(), new SgDivideOp(file_info, buildSizeOfOp(buildVarRefExp(init,
            getSurroundingStatement(init) -> get_scope())), denominator, uint));
   }
}

void RtedTransformation::visit_isSgPointerDerefExp(SgPointerDerefExp* const n)
{
   ROSE_ASSERT(n);

   SgExpression*        right = n->get_operand();
   // right hand side should contain some VarRefExp
   std::vector<SgNode*> vars = NodeQuery::querySubTree(right, V_SgVarRefExp);
   std::vector<SgNode*>::const_iterator it = vars.begin();

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
         // \pp is this correct? why not get_lhs_operand
         left = dotExp->get_rhs_operand();
      }
      else if (arrowExp)
      {
         left = arrowExp->get_lhs_operand();
      }

      if (left == varRef || left == NULL)
      {
         variable_access_pointerderef[n] = varRef;
         cerr << "$$$ DotExp: " << dotExp << "   arrowExp: " << arrowExp << endl;
         cerr << "  &&& Adding : " << varRef->unparseToString() << endl;
      }
      else
      {
         cerr << "$$$ Found a SgPointerDerefExp  but not adding to list. " << endl;
         cerr << "  $$$ DotExp: " << dotExp << "   arrowExp: " << arrowExp << endl;
         cerr << "  $$$ left: " << left->unparseToString() << "   varRef: " << varRef->unparseToString() << endl;
      }
   }

   if (vars.size() > 1) {
      cerr << "Warning : We added more than one SgVarRefExp to this map for SgPointerDerefExp. This might be a problem" << endl;
      //exit(1);
   }

#if 1
   std::vector<SgNode*>                 vars2 = NodeQuery::querySubTree(right, V_SgThisExp);
   std::vector<SgNode*>::const_iterator it2 = vars2.begin();
   for (; it2 != vars2.end(); ++it2) {
      SgThisExp* varRef = isSgThisExp(*it2);
      ROSE_ASSERT(varRef);
      variable_access_arrowthisexp[n] = varRef;
      cerr << " &&& Adding : " << varRef->unparseToString() << endl;
   }
   if (vars2.size() > 1) {
      cerr << "Warning : We added more than one SgThisExp to this map for SgArrowExp. This might be a problem" << endl;
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
   std::vector<SgNode*> vars = NodeQuery::querySubTree(left, V_SgVarRefExp);
   std::vector<SgNode*>::const_iterator it = vars.begin();

   for (; it != vars.end(); ++it) {
      SgVarRefExp* varRef = isSgVarRefExp(*it);
      ROSE_ASSERT(varRef);
      SgExpression* left = n->get_lhs_operand();
      if (left == varRef) {
         variable_access_arrowexp[n] = varRef;
         cerr << " &&& Adding : " << varRef->unparseToString() << endl;
      } else {
         cerr << " &&& Not adding varRef because on right hand side of -> :" << varRef->unparseToString() << endl;
         cerr << "   &&& left : " << left->unparseToString() << "  varRef: " << varRef << "  left:" << left << endl;
      }
   }

   if (vars.size() > 1) {
      cerr << "Warning : We added more than one SgVarRefExp to this map for SgArrowExp. This might be a problem" << endl;
      //exit(1);
   }
#if 1
   std::vector<SgNode*> vars2 = NodeQuery::querySubTree(left, V_SgThisExp);
   std::vector<SgNode*>::const_iterator it2 = vars2.begin();
   for (; it2 != vars2.end(); ++it2) {
      SgThisExp* varRef = isSgThisExp(*it2);
      ROSE_ASSERT(varRef);
      variable_access_arrowthisexp[n] = varRef;
      cerr << " &&& Adding : " << varRef->unparseToString() << endl;
   }
   if (vars2.size() > 1) {
      cerr << "Warning : We added more than one SgThisExp to this map for SgArrowExp. This might be a problem" << endl;
      //exit(1);
   }
#endif
}

void
RtedTransformation::arrayHeapAlloc(SgInitializedName* initName, SgVarRefExp* varRef, SgExpression* sz, AllocKind ak)
{
  ROSE_ASSERT(initName && varRef && sz);

  SgStatement* stmt = getSurroundingStatement(varRef);
  RtedArray*   array = new RtedArray(initName, stmt, ak, sz);

  // varRef can not be an array access, its only an array Create
  variablesUsedForArray.push_back(varRef);
  create_array_define_varRef_multiArray[varRef] = array;
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
                                    SgFunctionDeclaration* funcd
                                  )
{
  ROSE_ASSERT(initName && varRef && args && funcd);

  AllocKind   howAlloced = akUndefined;
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

  ROSE_ASSERT(howAlloced != akUndefined);
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
    res = arrayAllocCall(initName, varRef, args, funcr->getAssociatedFunctionDeclaration());
  } else {
     // right hand side of assign should only contain call to malloc somewhere
     cerr << "RtedTransformation: UNHANDLED AND ACCEPTED FOR NOW. Right of Assign : Unknown (Array creation) : "
          << "  line:" << varRef->unparseToString() << endl;
     //	    ROSE_ASSERT(false);
  }

  return res;
}



// TODO 2 djh:  rewrite this function to be more robust
//  i.e. handle general cases
//  consider whether getting the initname is important
void RtedTransformation::visit_isArraySgAssignOp(SgAssignOp* const assign)
{
   ROSE_ASSERT(assign);

   SgInitializedName* initName = NULL;
   SgExpression*      expr_l = assign->get_lhs_operand();
   SgExpression*      expr_r = assign->get_rhs_operand();

   // varRef ([indx1][indx2]) = malloc (size); // total array alloc
   // varRef [indx1]([]) = malloc (size); // indx2 array alloc
   SgExpression*      indx1 = NULL;
   SgExpression*      indx2 = NULL;

   cerr << "   ::: Checking assignment : " << assign->unparseToString() << endl;

   // FIXME 2: This probably does not handle n-dimensional arrays
   //
   // left side contains SgInitializedName somewhere ... search
   SgVarRefExp*       varRef = isSgVarRefExp(expr_l);
   SgPntrArrRefExp*   pntrArr = isSgPntrArrRefExp(expr_l);
   SgPointerDerefExp* pointerDeref = isSgPointerDerefExp(expr_l);

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
      indx1 = pntrArr->get_rhs_operand();
      ROSE_ASSERT(expr_ll);
      varRef = isSgVarRefExp(expr_ll);
      if (varRef) {
         // we assume pntr[indx1] = malloc
         initName = varRef->get_symbol()->get_declaration();
      } else if (isSgPntrArrRefExp(expr_ll)) {
         SgPntrArrRefExp* pntrArr2 = isSgPntrArrRefExp(expr_ll);
         ROSE_ASSERT(pntrArr2);
         SgExpression* expr_lll = pntrArr2->get_lhs_operand();
         indx2 = pntrArr2->get_rhs_operand();
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
            cerr << "RtedTransformation : Left of pntrArr2 - Unknown : " << expr_lll->class_name() << endl;
            ROSE_ASSERT(false);
         }
      } else if (isSgDotExp(expr_ll)) {
         cerr << "RtedTransformation : isSgDotExp : " << endl;

         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfDot(isSgDotExp(expr_ll),
               "Left of pntrArr - Right of Dot  - line: " + expr_ll->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         if (initName)
            ROSE_ASSERT(varRef);
      } else if (isSgPointerDerefExp(expr_ll)) {
         cerr << "RtedTransformation : isSgPointerDerefExp : " << endl;

         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfPointerDeref(isSgPointerDerefExp(expr_ll),
               "Left of pntrArr - Right of PointerDeref  - line: " + expr_ll->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         if (initName)
            ROSE_ASSERT(varRef);
      } else if (isSgArrowExp(expr_ll)) {
         cerr << "RtedTransformation : isSgArrowExp : " << endl;
         std::pair<SgInitializedName*, SgVarRefExp*> mypair = getRightOfArrow(isSgArrowExp(expr_ll),
               "Left of pntrArr - Right of Arrow  - line: " + expr_ll->unparseToString() + " ", varRef);
         initName = mypair.first;
         varRef = mypair.second;
         if (initName)
            ROSE_ASSERT(varRef);
      } else {
         cerr << "RtedTransformation : Left of pntrArr - Unknown : " << expr_ll->class_name() << "  line:"
               << expr_ll->unparseToString() << endl;
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
         std::vector<SgNode*> vars = NodeQuery::querySubTree(exp, V_SgVarRefExp);
         ROSE_ASSERT( vars.size() > 0 );

         varRef = isSgVarRefExp(vars[0]);
         ROSE_ASSERT( varRef );

         initName = varRef -> get_symbol() -> get_declaration();
         ROSE_ASSERT(varRef);
      }// ------------------------------------------------------------
      else {
         cerr << "RtedTransformation : PointerDerefExp - Unknown : " << exp->class_name() << "  line:"
               << pointerDeref->unparseToString() << endl;

         //      ROSE_ASSERT(false);
      }
   } // ------------------------------------------------------------
   else if (isSgFunctionCallExp(expr_l)) {
      cerr << "RtedTransformation: UNHANDLED BUT ACCEPTED FOR NOW - Left of assign - Unknown : " << expr_l->class_name()
            << "  line:" << expr_l->unparseToString() << endl;
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
      cerr << "RtedTransformation : Left of assign - Unknown : " << expr_l->class_name() << "  line:"
            << expr_l->unparseToString() << endl;
      ROSE_ASSERT(false);
   }
   cerr << " expr_l : " << expr_l->class_name() << endl;
   ROSE_ASSERT(initName);

   // handle MALLOC: function call
   AllocKind                     last_alloc = akCxxNew; // \pp should this be akUndefined?
   SgNodePtrList                 calls = NodeQuery::querySubTree(expr_r, V_SgFunctionCallExp);
   SgNodePtrList::const_iterator it = calls.begin();
   for (; it != calls.end(); ++it) {
      SgFunctionCallExp* funcc = isSgFunctionCallExp(*it);
      ROSE_ASSERT(funcc);

      SgExprListExp*     size = deepCopy(funcc->get_args());
      ROSE_ASSERT(size);

      // find if sizeof present in size operator
      // \pp why do we require that there is exactly one sizeof operand?
      vector<SgNode*> results = NodeQuery::querySubTree(size, V_SgSizeOfOp);
      ROSE_ASSERT_MSG(results.size() == 1, "Expected to find excactly 1 sizeof operand. Abort.");

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
      ROSE_ASSERT( varRef );

      const bool      arraynew = isSgArrayType( skip_ModifierType(new_op->get_type()) );
      const AllocKind allocKind = (arraynew ? akCxxArrayNew : akCxxNew);

      arrayHeapAlloc(initName, varRef, buildSizeOfOp(new_op -> get_specified_type()), allocKind);
   }

   // ---------------------------------------------
   // handle variables ..............................
   // here we should know the initName of the variable on the left hand side
   ROSE_ASSERT(initName && varRef);

   // we now know that this variable must be initialized
   // if we have not set this variable to be initialized yet,
   // we do so
   cerr << ">> Setting this var to be initialized : " << initName->unparseToString() << endl;
   variableIsInitialized[varRef] = InitializedVarMap::mapped_type(initName, last_alloc);
}

void RtedTransformation::addPaddingToAllocatedMemory(SgStatement* stmt, RtedArray* array)
{
    printf(">>> Padding allocated memory with blank space\n");
    //SgStatement* stmt = getSurroundingStatement(varRef);
    ROSE_ASSERT(stmt);
    // if you find this:
    //   str1 = ((char *)(malloc(((((4 * n)) * (sizeof(char )))))));
    // add the following lines:
    //   int i;
    //   for (i = 0; (i) < malloc(((((4 * n)) * (sizeof(char )); i++)
    //     str1[i] = ' ';

    // we do this only for char*
    SgInitializedName* initName = array->initName;
    SgType*            type = initName->get_type();
    ROSE_ASSERT(type);
    cerr << " Padding type : " << type->class_name() << endl;

    // \pp \todo do we need to skip modifiers?
    if (!isSgPointerType(type)) return;

    SgType* basetype = isSgPointerType(type)->get_base_type();
    cerr << " Base type : " << basetype->class_name() << endl;

    // since this is mainly to handle char* correctly, we only deal with one dim array for now
    if (basetype && isSgTypeChar(basetype) && array->getDimension() == 1)
    {
      // allocated size
      SgScopeStatement* scope = stmt->get_scope();
      SgExpression* size = array->getIndices()[0];
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
      SgExpression* lhs = buildPntrArrRefExp(buildVarRefExp(array->initName->get_name()), buildVarRefExp("i"));
      SgExpression* rhs = buildCharVal(' ');
      loop_body = buildAssignStatement(lhs, rhs);
      //loop_body = buildExprStatement(stmt2);


      SgForStatement* forloop = buildForStatement(init_stmt, cond_stmt, incr_exp, loop_body);

      SgBasicBlock* bb = buildBasicBlock(stmt1, forloop);
      insertStatementAfter(stmt, bb);
      string comment = "RS: Padding this newly generated array with empty space.";
      attachComment(bb, comment, PreprocessingInfo::before);
      popScopeStack();
   }
}

void RtedTransformation::visit_isArrayPntrArrRefExp(SgPntrArrRefExp* const arrRefExp)
{
   ROSE_ASSERT(arrRefExp);

   // make sure the parent is not another pntr array (pntr->pntr), we only want the top one
   // also, ensure we don't count arr[ix].member as an array access, e.g. in the
   // following:
   //    arr[ix].member = 2;
   // we need only checkwrite &( arr[ix].member ), which is handled by init var.
   if (!isSgPntrArrRefExp(arrRefExp->get_parent())) {

      int dimension = 1;
      SgExpression* left = arrRefExp->get_lhs_operand();
      // right hand side can be any expression!
      SgExpression* right1 = arrRefExp->get_rhs_operand();
      SgExpression* right2 = NULL;
      ROSE_ASSERT(right1);
      SgVarRefExp* varRef = isSgVarRefExp(left);
      if (varRef == NULL) {
         SgArrowExp* arrow = isSgArrowExp(left);
         SgDotExp* dot = isSgDotExp(left);
         SgPointerDerefExp* pointerDeref = isSgPointerDerefExp(left);
         SgPntrArrRefExp* arrRefExp2 = isSgPntrArrRefExp(left);
         if (arrow) {
            varRef = isSgVarRefExp(arrow->get_rhs_operand());
            ROSE_ASSERT(varRef);
         } else if (dot) {
            varRef = isSgVarRefExp(dot->get_rhs_operand());
            ROSE_ASSERT(varRef);
         } else if (pointerDeref) {
            varRef = isSgVarRefExp(pointerDeref->get_operand());
            ROSE_ASSERT(varRef);
         } else if (arrRefExp2) {
            dimension = 2;
            SgExpression* expr2 = arrRefExp2->get_lhs_operand();
            right2 = arrRefExp2->get_rhs_operand();
            varRef = isSgVarRefExp(expr2);

            if (!varRef) {
               varRef = resolveToVarRefRight(expr2);
            }

            ROSE_ASSERT(varRef);
         } else {
            cerr << ">> RtedTransformation::ACCESS::SgPntrArrRefExp:: unknown left of SgArrowExp: " << left->class_name()
                  << " --" << arrRefExp->unparseToString() << "-- " << endl;
            ROSE_ASSERT(false);
         }
      }
      ROSE_ASSERT(varRef);
      bool create_access_call = true;
      vector<SgVarRefExp*>::const_iterator cv = variablesUsedForArray.begin();
      for (; cv != variablesUsedForArray.end(); ++cv) {
         SgVarRefExp* stored = *cv;
         if (stored == varRef)
            create_access_call = false;
      }
      if (create_access_call) {
         SgInitializedName* initName = varRef->get_symbol()->get_declaration();
         ROSE_ASSERT(initName);
         RtedArray* array = new RtedArray(initName, getSurroundingStatement(arrRefExp), akStack);
         cerr << "!! CALL : " << varRef << " - " << varRef->unparseToString() << "    size : " << create_array_access_call.size()
               << "  -- " << array->unparseToString() << " : " << arrRefExp->unparseToString() << endl;
         create_array_access_call[arrRefExp] = array;
      }
   }

}


#endif
