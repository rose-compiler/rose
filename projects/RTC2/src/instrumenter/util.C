#include "util.h"
#include <cctype>
#include <algorithm>
#include "sageGeneric.h"

#define RMM_TYPETRACKER

namespace Util {

SgDeclarationStatement* FirstDeclStmtInFile;
SgProject* project;
unsigned int VarCounter;

void setProject(SgProject* proj) {
  project = proj;
}


SgExpression* safeCopy(SgExpression* original){
   AstAttribute* attr = original->getAttribute("tempName");
   SgExpression* result = SI::copyExpression(original);
   result->setAttribute("tempName", attr);
   return result;
}


//RMM FIX

SgName sanitizeMangledName(SgName n)
{
  std::string nS = n.getString();

  nS.erase(remove_if(nS.begin(), nS.end(), isspace), nS.end());
  for (size_t i=0; i<nS.length(); ++i)
  {
    if (!isalnum(nS[i]) && nS[i] != '_')
      nS[i] = 'x';
  }

  return SgName(nS);
}

//RMM FIX

SgName getNameForType(SgType* type) {
  return sanitizeMangledName(type->get_mangled());
}

bool checkIfStatic(SgDeclarationStatement* decl_stmt)
{
  SgDeclarationModifier& decl_mod = decl_stmt->get_declarationModifier();
  SgStorageModifier& stor_mod = decl_mod.get_storageModifier();
  #ifdef STATIC_DEBUG
  printf("checkIfStatic:\n");
  printf("decl_stmt: ");
  printf("%s = %s\n", decl_stmt->sage_class_name(), decl_stmt->unparseToString().c_str());
  printf("static: ");
  #endif

  if(stor_mod.isStatic()) {
    #ifdef STATIC_DEBUG
    printf("true\n");
    #endif
    return true;
  }
  else {
    #ifdef STATIC_DEBUG
    printf("false\n");
    #endif
    return false;
  }
}

SgBasicBlock* getBBForFn(SgFunctionDeclaration* fn_decl)
{
  SgFunctionDefinition* fn_def = fn_decl->get_definition();
  ROSE_ASSERT(fn_def != NULL);
  return fn_def->get_body();
}

SgFunctionDeclaration* getFnFor(SgNode* node)
{
  if (isSgFunctionDeclaration(node)) {
    return isSgFunctionDeclaration(node);
  }

  ROSE_ASSERT(node->get_parent() != NULL);
  return getFnFor(node->get_parent());
}

SgDeclarationStatement* getGloballyVisibleDecl(SgNode* pos)
{
  if(isSgMemberFunctionDeclaration(pos)) {
    SgClassDefinition* def = isSgClassDefinition(isSgMemberFunctionDeclaration(pos)->get_scope());
    SgClassDeclaration* decl = isSgClassDeclaration(def->get_parent());
    ROSE_ASSERT(isSgGlobal(decl->get_scope()));
    return decl;
  }
  else if(isSgFunctionDeclaration(pos)) {
    ROSE_ASSERT(isSgGlobal(isSgStatement(pos)->get_scope()));
    return isSgDeclarationStatement(pos);
  }
  else {
    printf("pos is not a member func/func decl\n");
    printNode(pos);
    ROSE_ASSERT(0);
  }
}

bool isPrimitiveDataType(SgType* type)
{
  return (  isSgTypeInt(type)
         || isSgTypeChar(type)
         || isSgTypeDouble(type)
         || isSgTypeFloat(type)
         || isSgTypeLong(type)
         || isSgTypeLongLong(type)
         || isSgTypeSignedChar(type)
         || isSgTypeUnsignedChar(type)
         || isSgTypeUnsignedInt(type)
         || isSgTypeUnsignedLong(type)
         || isSgTypeUnsignedLongLong(type)
         || isSgTypeShort(type)
         );
}


void printStatements(SgGlobal* global)
{
  SgDeclarationStatementPtrList& decls = global->get_declarations();
  SgDeclarationStatementPtrList::iterator iter;

  for(iter = decls.begin(); iter != decls.end(); ++iter) {
    printf("stmt: %p\n", *iter);
    printNode(*iter);
    printf("------------------------\n");
  }
}

bool isNamespaceScope(SgScopeStatement* scope, std::string str)
{
  if (isSgGlobal(scope)) return false;

  if (isSgNamespaceDefinitionStatement(scope))
  {
    SgNamespaceDeclarationStatement* nsp_decl = isSgNamespaceDefinitionStatement(scope)->

    get_namespaceDeclaration();
    if(strcmp(nsp_decl->get_name().getString().c_str(), str.c_str()) == 0) {
      return true;
    }

    return isNamespaceScope(isSgStatement(scope)->get_scope(), str);
  }

  return isNamespaceScope(isSgStatement(scope)->get_scope(), str);
}


bool isStdOrPrimitiveType(SgType* ty)
{
  if (SgDeclarationStatement* decl = ty->getAssociatedDeclaration())
  {
    // Check where this decl is
    if (!isNamespaceScope(decl->get_scope(), "std"))
    {
      return false;
    }
  }
  // No decl is assumed to be Primitive Type
  return true;
}

bool isInNamespace(SgFunctionDeclaration* fn_decl, std::string) {
  return isNamespaceScope(fn_decl->get_scope(), "std");
}

bool compareNames(SgName first, SgName second)
{
  //return (first == second);
  return (strcmp(first.getString().c_str(), second.getString().c_str()) == 0);
}

bool compareTypes(SgType* first, SgType* second) {

  //return (first->get_mangled() == second->get_mangled());
  return compareNames(first->get_mangled(), second->get_mangled());

}

bool isCondString(SgName s_name)
{
  return (  compareNames(s_name, "LessThan")
         || compareNames(s_name, "GreaterThan")
         || compareNames(s_name, "NotEqual")
         || compareNames(s_name, "LessOrEqual")
         || compareNames(s_name, "GreaterOrEqual")
         || compareNames(s_name, "Equality")
         );
}

bool isExternal(SgName s_name)
{
  std::string ExtFns[] = {
           "create_entry",
           "create_dummy_entry",
           "malloc_overload",
           "free_overload",
           "realloc_overload",
           "check_entry",
           "array_bound_check",
           "remove_entry",
           "array_bound_check_using_lookup",
           "EnterScope",
           "ExitScope",
           "getTopKey",
           "getTopLock",
           "update_initinfo",
           "check_initinfo",
           "execAtFirst",
           "execAtLast",
           "create_entry_if_src_exists",
           "create_entry_with_new_lock",
           "null_check",
           "push_to_stack",
           "pop_from_stack",
           "get_from_stack"
#ifdef RMM_TYPETRACKER
          , "traverseAndPrint",
           "typetracker_add"
#endif
        };

  for(unsigned int index = 0; index < sizeof(ExtFns)/sizeof(std::string); index++) {
    if(strcmp(ExtFns[index].c_str(), s_name.str()) == 0) {
      return true;
    }
  }

  return false;
}

std::string getTransfName(SgName s_name, SgType* retType, SgExprListExp* param_list) {

  std::string transf_String = getNameForType(retType) + "_Ret_" + s_name.getString();

  SgExpressionPtrList& exprs = param_list->get_expressions();
  for(SgExpressionPtrList::iterator iter = exprs.begin(); iter != exprs.end();
                              iter++) {
    SgExpression* ce = *iter;
    transf_String = transf_String + "_" + getNameForType(ce->get_type()) + "_Arg";
  }

  return transf_String;
}

SgVariableDeclaration* createLocalVariable(SgInitializedName* orig) {
  ROSE_ASSERT(orig->get_scope());
  // Create a local variable of the type given by orig
  std::string local_var_name = orig->get_name().getString() + "_recr";

  SgVariableDeclaration* local_var_decl = SageBuilder::buildVariableDeclaration(local_var_name, orig->get_type(), NULL, orig->get_scope());

  return local_var_decl;
}

SgVariableDeclaration* createLocalVariable(SgName name, SgType* type, SgExpression* init, SgScopeStatement* scope) {
  ROSE_ASSERT(scope != NULL);

  SgAssignInitializer* assign_init = NULL;
  if(init != NULL) {
    // Create a local variable from the arguments
    assign_init = SB::buildAssignInitializer(init, init->get_type());
  }

  SgVariableDeclaration* local_var_decl = SageBuilder::buildVariableDeclaration(name, type, assign_init, scope);

  return local_var_decl;

}

SgInitializedName* getNameForDecl(SgVariableDeclaration* decl) {

  // get info about original declaration
  SgInitializedNamePtrList& name_list = decl->get_variables();
  ROSE_ASSERT(name_list.size() == 1);
  SgInitializedName* name = *(name_list.begin());

  ROSE_ASSERT(name);
  return name;
}


SgAddressOfOp* createAddressOfOpFor(SgExpression* exp) {


  #ifdef ADDROP_LVAL_FALSE
  exp->set_lvalue(false);
  #endif


  #ifdef STRIP_TYPEDEFS
  //exp = castAwayTypedef(exp); //RMM: Problem occurs here.

  #endif

  SgAddressOfOp* address_of_op = SB::buildAddressOfOp(exp);

  return address_of_op;
}

SgAddressOfOp* createAddressOfOpFor(SgInitializedName* name) {

  SgVarRefExp* var_ref_exp = SB::buildVarRefExp(name, name->get_scope());

  return createAddressOfOpFor(var_ref_exp);

}

SgExprStatement* createDotAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS) {

  // for the ptr exp
  SgExpression* str_exp = createDotExpFor(str_name, str_elem);

  // build the assign op: output.ptr = input1
  SgExpression* str_assign = SB::buildAssignOp(str_exp, RHS);

  SgExprStatement* str_stmt = SB::buildExprStatement(str_assign);

  return str_stmt;
}

SgExprStatement* createArrowAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS) {

  // for the ptr exp
  SgExpression* str_exp = createArrowExpFor(str_name, str_elem);

  // build the assign op: output.ptr = input1
  SgExpression* str_assign = SB::buildAssignOp(str_exp, RHS);

  SgExprStatement* str_stmt = SB::buildExprStatement(str_assign);

  return str_stmt;
}

SgType* getVoidPointerType() {
  return SgPointerType::createType(SgTypeVoid::createType());
}


SgDotExp* createDotExpFor(SgInitializedName* str_var_name, std::string str_elem) {

  #ifdef DOT_EXP_DEBUG
  printf("create Dot Exp For\n");
  printf("str_var_name: \n");
  printNode(str_var_name);
  #endif

  SgVariableDeclaration* ptr_decl = getCorrectMemberDecl(str_var_name, str_elem);
  #ifdef DOT_EXP_DEBUG
  printf("ptr_decl:\n");
  printNode(ptr_decl);
  #endif

  SgVarRefExp* ptr_ref_exp = SageBuilder::buildVarRefExp(ptr_decl);
  #ifdef DOT_EXP_DEBUG
  printf("ptr_ref_exp:\n");
  printNode(ptr_ref_exp);
  #endif

  //SgVarRefExp* str_var_ref_exp = SageBuilder::buildVarRefExp(str_var_decl);
  SgVarRefExp* str_var_ref_exp = SageBuilder::buildVarRefExp(str_var_name, str_var_name->get_scope());

  SgDotExp* dot_exp = SageBuilder::buildDotExp(str_var_ref_exp, ptr_ref_exp);

  #ifdef DOT_EXP_DEBUG
  printf("ptr_ref_exp: type: %s\n", ptr_ref_exp->get_type()->get_mangled().getString().c_str());
  #endif

  return dot_exp;
}

SgArrowExp* createArrowExpFor(SgInitializedName* str_var_name, std::string str_elem) {

  #ifdef ARROW_EXP_DEBUG
  printf("create Arrow Exp For\n");
  printf("str_var_name: \n");
  printNode(str_var_name);
  #endif

  SgVariableDeclaration* ptr_decl = getCorrectMemberDecl(str_var_name, str_elem);
  #ifdef ARROW_EXP_DEBUG
  printf("ptr_decl:\n");
  printNode(ptr_decl);
  #endif

  SgVarRefExp* ptr_ref_exp = SageBuilder::buildVarRefExp(ptr_decl);
  #ifdef ARROW_EXP_DEBUG
  printf("ptr_ref_exp:\n");
  printNode(ptr_ref_exp);
  #endif

  //SgVarRefExp* str_var_ref_exp = SageBuilder::buildVarRefExp(str_var_decl);
  SgVarRefExp* str_var_ref_exp = SageBuilder::buildVarRefExp(str_var_name, str_var_name->get_scope());

  SgArrowExp* arrow_exp = SageBuilder::buildArrowExp(str_var_ref_exp, ptr_ref_exp);

  #ifdef ARROW_EXP_DEBUG
  printf("ptr_ref_exp: type: %s\n", ptr_ref_exp->get_type()->get_mangled().getString().c_str());
  #endif

  return arrow_exp;
}

SgType* getSizeOfType() {
  return SgTypeUnsignedLong::createType();
}

SgVariableDeclaration* getCorrectMemberDecl(SgInitializedName* str_var_name, std::string str_elem) {

  // We can't use the str_elem to figure out which variable to use, since all the structures
  // have internal variables by the same name! So, only the last struct's version of that
  // variable will show up.

//  SgDeclarationStatement* str_decl = str_var_name->get_declaration();

  #ifdef CORRECT_MEMBER_DECL_DEBUG
  printf("getCorrectMemberDecl\n");

  printf("str_var_name: \n");
  printNode(str_var_name);
  #endif

  #if 0
  SgDeclarationStatement* str_decl = str_var_name->get_type()->getAssociatedDeclaration();

  ROSE_ASSERT(isSgClassDeclaration(str_decl));
  SgClassDeclaration* class_decl = isSgClassDeclaration(str_decl);

  printf("class decl\n");
  printNode(class_decl);

  ROSE_ASSERT(class_decl != NULL);

  SgClassDeclaration* class_decl_def = isSgClassDeclaration(class_decl->get_definingDeclaration());

  SgClassDefinition* class_def = class_decl_def->get_definition();

  ROSE_ASSERT(class_def != NULL);

  #endif

  // skip_ ... required to handle arrow exp cases, where str_var_name is a pointer to a struct type
  // OLD WORKING CASE -- doesn't handle arrow exps
  //SgClassType* cls_type = isSgClassType(str_var_name->get_type());
  SgClassType* cls_type = isSgClassType(skip_RefsPointersAndTypedefs(str_var_name->get_type()));

  #ifdef CORRECT_MEMBER_DECL_DEBUG
  printf("cls_type\n");
  printNode(cls_type);
  #endif

  SgClassDeclaration * cls_decl = isSgClassDeclaration (cls_type->get_declaration());



    ROSE_ASSERT (cls_decl);

  #ifdef CORRECT_MEMBER_DECL_DEBUG
  printf("cls_decl\n");
  //printNode(cls_decl);
  printNodeExt(cls_decl);
  #endif

    ROSE_ASSERT(cls_decl->get_firstNondefiningDeclaration()!= NULL);

  #ifdef CORRECT_MEMBER_DECL_DEBUG
  printf("cls_first_nondefining_decl\n");
  printNode(cls_decl->get_firstNondefiningDeclaration());
  printNodeExt(cls_decl->get_firstNondefiningDeclaration());
  #endif

    ROSE_ASSERT(isSgClassDeclaration(cls_decl->get_firstNondefiningDeclaration()) == cls_decl);

  SgClassDeclaration* class_decl_def = isSgClassDeclaration(cls_decl->get_definingDeclaration());

  SgClassDefinition* class_def = class_decl_def->get_definition();

  ROSE_ASSERT(class_def != NULL);

  SgDeclarationStatementPtrList& members = class_def->get_members();

  SgDeclarationStatementPtrList::iterator iter = members.begin();

  bool found = false;
  SgVariableDeclaration* ptr_decl = NULL;
  for(; iter != members.end(); ++iter) {

    SgDeclarationStatement* decl_stmt = *iter;
    // This is a struct that we created -- should only have var decls.. right?
    ROSE_ASSERT(isSgVariableDeclaration(decl_stmt));
    SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl_stmt);

    SgInitializedName* var_name = getNameForDecl(var_decl);

    if(strcmp(var_name->get_name().getString().c_str(), str_elem.c_str()) == 0) {
      found = true;
      ptr_decl = var_decl;
      break;
    }
  }

  ROSE_ASSERT(found);

  #ifdef CORRECT_MEMBER_DECL_DEBUG
  printf("ptr_decl\n");
  printNode(ptr_decl);
  #endif

  return ptr_decl;
}

SgExpression* castToAddr(SgExpression* exp) {
  //SgExpression* cast_exp = SB::buildCastExp(exp, getAddrType(), SgCastExp::e_reinterpret_cast);
  SgExpression* cast_exp = SB::buildCastExp(exp, getAddrType(), CAST_TYPE);  //original

  return cast_exp;
}

SgType* getAddrType() {
  SgType* addr_type = SgTypeUnsignedLongLong::createType();
  return addr_type;
}



std::string generateNameForType(SgType* ptr_type)
{
#ifdef GENERATE_NAME_FOR_TYPE_DEBUG
    std::cerr << "ptr_type->get_mangled(): "
              << ptr_type->get_mangled().getString()
              << std::endl;
    //RMM
    //return skip_RefsPointersAndTypedefs(ptr_type)->sage_class_name();
    //RMM

    //printf("ptr_type->decl()->get_name(): %s\n", ptr_type->getAssociatedDeclaration()->get_mangled_name().getString().c_str());
#endif /* GENERATE_NAME_FOR_TYPE_DEBUG */
  return ptr_type->get_mangled() + "_Type";

#if OBSOLETE_CODE
  // \pp

  if (ptr_type->getAssociatedDeclaration()) {
    #ifdef GENERATE_NAME_FOR_TYPE_DEBUG
    printf("ptr_type->get_mangled(): %s\n", ptr_type->get_mangled().getString().c_str());

    //RMM
    //return skip_RefsPointersAndTypedefs(ptr_type)->sage_class_name();
    //RMM

    //printf("ptr_type->decl()->get_name(): %s\n", ptr_type->getAssociatedDeclaration()->get_mangled_name().getString().c_str());
    #endif
    //return ptr_type->getAssociatedDeclaration()->get_mangled_name() + "_GenType";
    //return ptr_type->get_mangled() + "_GenType";   //RMM FIX
    return ptr_type->get_mangled() + "_Type";

    //return ptr_type->getAssociatedDeclaration()->get_mangled_name();
  }
  else {
    #ifdef GENERATE_NAME_FOR_TYPE_DEBUG
    //printf("ptr_type->get_mangled(): %s\n", ptr_type->get_mangled().getString().c_str());
    printf("ptr_type->get_mangled(): %s\n", ptr_type->get_mangled().getString().c_str());


    //RMM
    //return skip_RefsPointersAndTypedefs(ptr_type)->sage_class_name();
    //RMM

    #endif
    //return ptr_type->get_mangled() + "_GenType"; //RMM FIX
    return ptr_type->get_mangled() + "_Type";

    //return ptr_type->get_mangled();
  }
#endif /* OBSOLETE_CODE */
}

SgType* skip_Typedefs(SgType* type) {
  if(isSgTypedefType(type)) {
    return skip_Typedefs(isSgTypedefType(type)->get_base_type());
  }
  return type;
}


SgType* getType(SgType* type) {
  #ifdef ENUM_TO_INT_HACK
  SgType* ty = skip_Typedefs(type);
  if(isSgEnumType(ty)) {
    return SgTypeInt::createType();
  }
  return ty;
  #else
  return skip_Typedefs(type);
  #endif
}

SgExpression* castAwayTypedef(SgExpression* exp) {

  SgType* type = exp->get_type();

  if(isSgTypedefType(type)) {
    return SB::buildCastExp(exp, skip_Typedefs(type), SgCastExp::e_C_style_cast);
  }
  return exp;
}

void printNode(SgNode* node) {
  printf("node: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
}

void printNodeExt(SgNode* node) {
  Sg_File_Info* File = isSgNode(node)->get_file_info();
  printNode(node);
  printf("File: (%s, %d, %d) %s = %s\n", File->get_filenameString().c_str(),
                    File->get_line(),
                    File->get_col(),
                    isSgNode(node)->sage_class_name(),
                    isSgNode(node)->unparseToString().c_str());

}



bool isQualifyingType(SgType* type) {

  #ifdef STRIP_TYPEDEFS
  ROSE_ASSERT(!isSgTypedefType(type));
  #endif

  #ifdef QUAL_DEBUG
  printf("QualifyingType: ");
  #endif

  #ifdef SKIP_TYPEDEFS_QUAL_TYPES
  type = skip_Typedefs(type);
  #endif

  #ifdef SUPPORT_REFERENCES
  if(isSgReferenceType(type)) {
    return false;
  }
  #endif

  if(isSgPointerType(type) && isSgArrayType(type)) {
    #ifdef QUAL_DEBUG
    printf("both pointer and array type!\n");
    #endif
    ROSE_ASSERT(0);
  }

  if(isSgPointerType(type)) {
    #ifdef QUAL_DEBUG
    printf("Pointer\n");
    #endif
    return true;
  }
  else if(isSgArrayType(type)) {
    #ifdef QUAL_DEBUG
    printf("Array\n");
    #endif
    return true;
  }


  else {
    #ifdef QUAL_DEBUG
    printf("Not qual type\n");
    #endif
    return false;
  }


}

SgExprListExp* getArrayBoundCheckParams(SgExpression*, SgExpression* arg2, SgExpression* arg3, SgType* arg4) {
  // For arrays
  // output: array_bound_check(sizeof(arg2)/sizeof(arg4), arg3)

  SgExpression* upper_bound_bytes = SB::buildSizeOfOp(SI::copyExpression(arg2));

  SgExpression* upper_bound = SB::buildDivideOp(upper_bound_bytes, SB::buildSizeOfOp(arg4));

  #ifdef INT_ARRAY_INDEX
  SgExpression* index_casted = SB::buildCastExp(arg3,
      upper_bound_bytes->get_type(), SgCastExp::e_C_style_cast);
  #else
  SgExpression* index_casted = arg3;
  #endif

  SgExprListExp* param_list = SB::buildExprListExp(upper_bound, index_casted);

  return param_list;
}

SgExprListExp* getArrayBoundCheckUsingLookupParams(SgExpression* arg1, SgExpression* arg2, SgExpression* arg3, SgType*) {

  // For pointers
  #ifdef ARRAY_LOOKUP_USING_ADDR
  // output: array_bound_check_using_lookup(arg1, (sizeof_type)(&arg2[arg3]))
  #else
  // output: array_bound_check_using_lookup(arg1, (sizeof_type)arg2 + arg3*sizeof(arg4))
  #endif

  // FIXME: Apparently, not required...
//  SgExpression* arg2_casted = SB::buildCastExp(arg2, getSizeOfType(), SgCastExp::e_C_style_cast);

  #ifdef ARRAY_LOOKUP_USING_ADDR
  // (sizeof_type)(&arg2[arg3])

  // This one casts the addresses into their base form, and that somehow causes
  // bugs in benchmarks... case in point: art: (unsigned long)&(struct ...)(f1_layer[])
  // The (struct ...) part screws the actaul value transmitted by the address of operator
  // This might be a bug in gcc that we are hitting, or something else, but
  // instead of that, we'll simply create address of op for this here.
  //SgExpression* addr = createAddressOfOpFor(SB::buildPntrArrRefExp(arg2, arg3));

  SgExpression* array_ref = SB::buildPntrArrRefExp(arg2, arg3);
  array_ref->set_lvalue(false);
  SgExpression* addr = SB::buildAddressOfOp(array_ref);

  SgExpression* total = SB::buildCastExp(addr, getSizeOfType(), SgCastExp::e_C_style_cast);
  #else
  // index*sizeof(base_type)
  SgExpression* sizeof_exp = SB::buildSizeOfOp(arg4);
  //SgExpression* offset = SB::buildMultiplyOp(arg3, sizeof_exp);
  // arg3 * sizeof(arg4)/sizeof(arg3)
  // We need the sizeof(arg3) since arg3 already increments by a particular
  // count -- typically integer
  SgExpression* offset = SB::buildDivideOp(SB::buildMultiplyOp(arg3, sizeof_exp), SB::buildSizeOfOp(arg3));

  // (sizeof_type)param.ptr + index*sizeof(base_type)
  SgExpression* total = SB::buildAddOp(arg2_casted, offset);
  #endif

  // Insert the function call
  SgExprListExp* param_list = SB::buildExprListExp(arg1, total);

  return param_list;

}

bool isRHSOfDotOrArrow(SgNode* node) {
  ROSE_ASSERT(isDotOrArrow(node->get_parent()));
  return (isSgNode((isSgBinaryOp(node->get_parent()))->get_rhs_operand()) == node);
}

bool isVarRefOrDotArrow(SgExpression* exp) {


  if(isSgVarRefExp(exp)) {
    return true;
  }
  else if(isSgPntrArrRefExp(exp)) {
    return true;
  }
  else if( isSgDotExp(exp) || isSgArrowExp(exp)) {
    SgBinaryOp* dot_arrow = isSgBinaryOp(exp);
    if(isSgVarRefExp(dot_arrow->get_rhs_operand())) {
      return true;
    }
    return false;
  }
  return false;
}

bool isDotOrArrow(SgNode* node) {
  return isSgDotExp(node) || isSgArrowExp(node);
}


bool isConditionalOp(SgNode* node) {
  // Conditional ops are binary ops
  if(!isSgBinaryOp(node)) {
    return false;
  }
  else  {
    SgBinaryOp* bop = isSgBinaryOp(node);

    switch(bop->variantT()) {
    case V_SgGreaterOrEqualOp:
    case V_SgLessOrEqualOp:
    case V_SgLessThanOp:
    case V_SgGreaterThanOp:
    case V_SgNotEqualOp:
    case V_SgEqualityOp: return true;
    default: return false;
    }
  }
  ROSE_ASSERT(0);
}

SgStatement* getSurroundingStatement(SgExpression& n)
{
  return &sg::ancestor<SgStatement>(n);
}

SgStatement* getSurroundingStatement(SgNode& n)
{
  return &sg::ancestor<SgStatement>(n);
}


SgType* skip_PointerType(SgType* t)
{
  SgPointerType* sgptr = isSgPointerType(t);
  if (sgptr != NULL) return sgptr->get_base_type();

  return t;
}

SgType* skip_ReferencesAndTypedefs( SgType* type ) {
     if( isSgTypedefType( type )) {
        return skip_ReferencesAndTypedefs(
            isSgTypedefType( type ) -> get_base_type() );
    } else if( isSgReferenceType( type )) {
        // resolve reference to reference ... to pointer
        return skip_ReferencesAndTypedefs(
            isSgReferenceType( type ) -> get_base_type() );
    }

    return type;
}


SgType* resolveTypedefsAndReferencesToBaseTypes(SgType* type) {
    SgType* baseType = skip_ReferencesAndTypedefs(type);

    SgType* retType;

    if(baseType->class_name() == "SgModifierType") {
        SgModifierType* modType = isSgModifierType(baseType);
        retType = modType->get_base_type();
    }
    else {
        retType = baseType;
    }

    return retType;
}

SgType* skip_RefsPointersAndTypedefs(SgType* type) {
  SgType* retType = type;
  if(isSgPointerType(type)) {
    SgType* baseType = skip_PointerType(type);
    retType = skip_RefsPointersAndTypedefs(baseType);
  }
  else if(isSgTypedefType(type) || isSgReferenceType(type)) {
    SgType* baseType = skip_ReferencesAndTypedefs(type);
    retType = skip_RefsPointersAndTypedefs(baseType);
  }
  else if(isSgModifierType(type)) {
    SgType* baseType = resolveTypedefsAndReferencesToBaseTypes(type);
    retType = skip_RefsPointersAndTypedefs(baseType);
  }

  return retType;
}

SgType* skip_ArrPntrRefTypedefs(SgType* type) {
  if(isSgPointerType(type)) {
    SgPointerType* p = isSgPointerType(type);
    return skip_ArrPntrRefTypedefs(p->get_base_type());
  }
  else if(isSgArrayType(type)) {
    SgArrayType* a = isSgArrayType(type);
    return skip_ArrPntrRefTypedefs(a->get_base_type());
  }
  else if(isSgTypedefType(type) || isSgReferenceType(type)) {
    SgType* baseType = skip_ReferencesAndTypedefs(type);
    return skip_RefsPointersAndTypedefs(baseType);
  }
  else if(isSgModifierType(type)) {
    SgType* baseType = resolveTypedefsAndReferencesToBaseTypes(type);
    return skip_RefsPointersAndTypedefs(baseType);
  }
  else {
    return type;
  }
}

bool NoInitializer(SgVariableDeclaration* var_decl) {

  SgInitializedName* var_name = Util::getNameForDecl(var_decl);
  if(var_name->get_initializer() == NULL) {
    return true;
  }
  else {
    return false;
  }
}

SgScopeStatement* getScopeForExp(SgExpression* exp) {
//  SgStatement* stmt = Util::getSurroundingStatement(*isSgNode(exp));
  SgStatement* stmt = SI::getEnclosingStatement(isSgNode(exp));
  ROSE_ASSERT(stmt != NULL);
    SgScopeStatement* scope = stmt->get_scope();
  return scope;
}

void insertAtTopOfBB(SgBasicBlock* bb, SgStatement* stmt) {

  SgStatementPtrList& stmts = bb->get_statements();
  if(stmts.size() == 0) {
    bb->prepend_statement(stmt);
  }
  else {
    SageInterface::insertStatementBefore(*(stmts.begin()), stmt);
  }
}

bool isFree(SgFunctionCallExp* fncall) {
  if(fncall->getAssociatedFunctionDeclaration() == NULL) {
    return false;
  }

  return (strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "free") == 0);
}

SgExpression* stripDeref(SgExpression* exp) {

  ROSE_ASSERT(isSgPointerDerefExp(exp));
  SgExpression* oper = isSgPointerDerefExp(exp)->get_operand();


  #ifdef STRIP_TYPEDEFS
  oper = Util::castAwayTypedef(oper);
  #endif
  return oper;
}

SgType* getTypeForPntrArr(SgType* type) {
  //RMM TEST
  type = type->stripTypedefsAndModifiers();
  if(isSgTypeInt(type)) //RMMTEST
    return type;

  #ifdef STRIP_TYPEDEFS
  ROSE_ASSERT(!isSgTypedefType(type));
  #endif

  #ifdef SKIP_TYPEDEFS_QUAL_TYPES
  type = Util::skip_Typedefs(type);
  #endif

  #ifdef SUPPORT_REFERENCES
  ROSE_ASSERT(!isSgReferenceType(type));
  #if 0
  if(isSgReferenceType(type)) {
    return type;
  }
  #endif
  #endif

  if(isSgPointerType(type)) {
    return type;
  }
  else if(isSgArrayType(type)) {


    #ifdef DELAYED_INSTR
    SgType* base_type = isSgArrayType(type)->get_base_type();
    return SgPointerType::createType(base_type);

    #else

    // Original working version...
    #if 0
    SgType* base_type = isSgArrayType(type)->get_base_type();
    SgType* retType = SgPointerType::createType(base_type);
    return retType;
    #else
    SgType* base_type = skipArrayTypes(isSgArrayType(type));
    // Should we do this? -- I think we shouldn't remove the
    // pointer since this could be a case of array of pointers
    #if 0
    if(isSgPointerType(base_type)) {
      base_type = skip_RefsPointersAndTypedefs(base_type);
    }
    #endif
    // Now that we have the correct base type (i.e. non array type)
    // create a pointer version of it...
    return SgPointerType::createType(base_type);
    #endif
    #endif
  }
  else {
    ROSE_ASSERT(Util::isQualifyingType(type));
    printf("Qualifying Type not supported in getTypeForPntrArr\n");
    ROSE_ASSERT(0);
  }
}

SgStatement* getSuitablePrevStmt(SgStatement* stmt) {

  SgNode* parent = isSgNode(stmt)->get_parent();
  #ifdef PREV_STMT_DEBUG
  printf("Parent: %s = %s\n", isSgNode(parent)->sage_class_name(), isSgNode(parent)->unparseToString().c_str());
  #endif

  if(isSgForInitStatement(parent)) {
    #ifdef PREV_STMT_DEBUG
    printf("Found forloop case.\n");
    #endif
    // This is a for loop initialization statement.
    // We can't put just the variable declaration here, without
    // any initialization. So, lets put it prior to the
    // this statement
    SgNode* forloopstmt = parent->get_parent();
    #ifdef PREV_STMT_DEBUG
    printf("ForInit: %s = %s\n", isSgNode(forloopstmt)->sage_class_name(), isSgNode(forloopstmt)->unparseToString().c_str());
    #endif
    ROSE_ASSERT(isSgForStatement(isSgStatement(forloopstmt)));
    return isSgStatement(forloopstmt);
  }
  else if(isSgSwitchStatement(parent)) {
    #ifdef PREV_STMT_DEBUG
    printf("Switch stmt: parent: %s\n", isSgNode(parent)->unparseToString().c_str());
    #endif
    return isSgStatement(parent);
  }
  return stmt;

}

SgExpression* getDummyLock() {
  return SB::buildIntVal(DUMMY_LOCK);
}

SgExpression* getLookupTicket() {
  static uint64_t TicketCounter = 1;
  return SB::buildIntVal(TicketCounter++);
}

char* getStringForVariantT(VariantT var) {
  char* output = (char*)malloc(50*sizeof(char));

  switch(var) {
  case V_SgAddOp: sprintf(output, "Add"); break;
  case V_SgSubtractOp: sprintf(output, "Sub"); break;
  case V_SgMultAssignOp: //printf("MultAssignOp\n");
  case V_SgMultiplyOp: sprintf(output, "Mult"); break;
  case V_SgDivAssignOp: //printf("DivAssignOp\n");
  case V_SgDivideOp: sprintf(output, "Div"); break;
  case V_SgLessThanOp: sprintf(output, "LessThan"); break;
  case V_SgGreaterThanOp: sprintf(output, "GreaterThan"); break;
  case V_SgNotEqualOp: sprintf(output, "NotEqual"); break;
  case V_SgLessOrEqualOp: sprintf(output, "LessOrEqual"); break;
  case V_SgGreaterOrEqualOp: sprintf(output, "GreaterOrEqual"); break;
  case V_SgEqualityOp: sprintf(output, "Equality"); break;
  case V_SgModOp: sprintf(output, "Mod"); break;
  case V_SgIntegerDivideOp: sprintf(output, "IntDiv"); break;
  case V_SgTypeInt: sprintf(output, "Int"); break;
  case V_SgIntVal: sprintf(output, "Int"); break;
  case V_SgTypeUnsignedInt: sprintf(output, "UInt"); break;
  case V_SgUnsignedIntVal: sprintf(output, "UInt"); break;
  case V_SgTypeUnsignedLong: sprintf(output, "ULong"); break;
  case V_SgUnsignedLongVal: sprintf(output, "ULong"); break;
  case V_SgTypeUnsignedLongLong: sprintf(output, "ULongLong"); break;
  case V_SgUnsignedLongLongIntVal: sprintf(output, "ULongLong"); break;
  case V_SgTypeLongLong: sprintf(output, "LongLong"); break;
  case V_SgLongLongIntVal: sprintf(output, "LongLong"); break;
  case V_SgTypeLong: sprintf(output, "Long"); break;
  case V_SgLongIntVal: sprintf(output, "Long"); break;
  case V_SgTypeShort: sprintf(output, "Short"); break;
  case V_SgShortVal: sprintf(output, "Short"); break;
  case V_SgTypeChar: sprintf(output, "Char"); break;
  case V_SgTypeVoid: sprintf(output, "Void"); break;
  case V_SgFloatVal:
  case V_SgTypeFloat: sprintf(output, "Float"); break;
  case V_SgDoubleVal:
  case V_SgTypeDouble: sprintf(output, "Double"); break;
  case V_SgPlusPlusOp: sprintf(output, "Increment"); break;
  case V_SgMinusMinusOp: sprintf(output, "Decrement"); break;
  case V_SgPlusAssignOp: sprintf(output, "PlusAssign"); break;
  case V_SgMinusAssignOp: sprintf(output, "MinusAssign"); break;
  case V_SgAssignOp: sprintf(output, "Assign"); break;
  case V_SgAddressOfOp: sprintf(output, "AddressOf"); break;
  case V_SgCastExp: sprintf(output, "Cast"); break;
  case V_SgPntrArrRefExp: sprintf(output, "PntrArrRef"); break;
  case V_SgDotExp: sprintf(output, "DotExp"); break;
  case V_SgArrowExp: sprintf(output, "ArrowExp"); break;
  default: sprintf(output, "IDK_%u", var);
  }

  return output;
}

void makeDeclStatic(SgDeclarationStatement* decl_stmt) {

  // If its already static, nothing to do.
  if(Util::checkIfStatic(decl_stmt)) {
    return;
  }

  SgDeclarationModifier& decl_mod = decl_stmt->get_declarationModifier();
  SgStorageModifier& stor_mod = decl_mod.get_storageModifier();
  stor_mod.setStatic();

  ROSE_ASSERT(stor_mod.isStatic());

  return;
}

SgName getFunctionName(SgFunctionCallExp* fncall) {

  SgFunctionSymbol* fnsymbol = fncall->getAssociatedFunctionSymbol();
  return fnsymbol->get_name();
}

SgFunctionDefinition* getFuncDef(SgFunctionDeclaration* fndecl) {
  SgFunctionDefinition* fndef = fndecl->get_definition();
  ROSE_ASSERT(fndef != NULL);
  return fndef;
}

SgBasicBlock* getFuncBody(SgFunctionDefinition* fndef) {
  SgBasicBlock* bb = fndef->get_body();
  ROSE_ASSERT(bb != NULL);
  return bb;
}

void appendReturnStmt(SgExpression* ret_exp, SgBasicBlock* fnbody) {

  SgReturnStmt* ret_stmt = SB::buildReturnStmt(ret_exp);

  fnbody->append_statement(ret_stmt);
}

SgScopeStatement* getScopeForVarRef(SgVarRefExp* var_ref) {

  SgVariableSymbol* var_symbol = var_ref->get_symbol();
  SgInitializedName* init_name = getNameForSymbol(var_symbol);
  return init_name->get_scope();
}

SgInitializedName* getNameForSymbol(SgVariableSymbol* var_symbol) {

  return var_symbol->get_declaration();
}

std::string getStringForFn(std::string fn_name, SgType* arg1, SgType* arg2, SgType* arg3,
                    SgType* retType) {

  SgName arg_name;
  if(arg1 != NULL) {
    arg_name = Util::getNameForType(arg1);
    fn_name += "_" + arg_name + "_Arg_";
  }

  if(arg2 != NULL) {
    arg_name = Util::getNameForType(arg2);
    fn_name += "_" + arg_name + "_Arg_";
  }

  if(arg3 != NULL) {
    arg_name = Util::getNameForType(arg3);
    fn_name += "_" + arg_name + "_Arg_";
  }

  if(retType != NULL) {
    arg_name = Util::getNameForType(retType);
    fn_name += "_" + arg_name + "_Ret_";
  }

  return fn_name;
}

std::string getStringForOp(SgNode* node, SgType* retType) {

  std::string fn_name_String = Util::getStringForVariantT(node->variantT());

  if(retType != NULL) {
    fn_name_String = retType->get_mangled() + "_Ret_" + fn_name_String;
  }
  else {
    fn_name_String = fn_name_String;
  }

  if(isSgUnaryOp(node)) {
    SgUnaryOp* uop = isSgUnaryOp(node);

    SgExpression* oper = uop->get_operand();

    fn_name_String = fn_name_String + oper->get_type()->get_mangled() + "_Arg_";
  }
  else if(isSgBinaryOp(node)) {
    SgBinaryOp* bop = isSgBinaryOp(node);

    SgExpression* lhs_oper = bop->get_lhs_operand();
    SgExpression* rhs_oper = bop->get_rhs_operand();

    fn_name_String = fn_name_String + lhs_oper->get_type()->get_mangled()
                    + "_Arg_" + rhs_oper->get_type()->get_mangled() + "_Arg_";
  }
  else {
    printf("getStringForOp: Unknown op\n");
    ROSE_ASSERT(0);
  }

  return fn_name_String;
}


}
