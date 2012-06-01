#ifndef ARITHCHECK_C
#define ARITHCHECK_C
#include "ArithCheck.h"
#include "sageGeneric.h"
#include <string>
#include <boost/lexical_cast.hpp>

#define STRUCT_ARRAY 7
#define GENERATE_DEFS
#define CAST_TYPE SgCastExp::e_C_style_cast
//#define COND_BOOL_RETVAL
#define CHECK_STATIC
#define CREATE_ENTRY_NOT_STATIC
#define MAKE_DECLS_STATIC
//#define FORWARD_CLASS_DECLS
#define ADDROP_LVAL_FALSE
//#define INLINING_CHECK
//#define IMPLICIT_CASTS
#define HACKY_QUICK_INSERT_LOC

#define ONLY_ADD_ARRAYS_TO_OVR
#define CLASS_STRUCT_COPY


using namespace SageInterface;
using namespace SageBuilder;

SgType* ArithCheck::created_struct_type;
unsigned int ArithCheck::VarCounter = 0;
TypeMap_t ArithCheck::UnderlyingType;
TypeMap_t ArithCheck::RelevantStructType;
NodeContainer ArithCheck::NodesToInstrument;
VariableSymbolMap_t ArithCheck::varRemap;
VariableSymbolMap_t ArithCheck::ReverseMap;
VariableSymbolMap_t ArithCheck::ClassStructVarRemap;
SgFunctionDeclaration* ArithCheck::MallocWrapFnDecl;
FuncInfoList ArithCheck::ForwardDecls;
uint64_t ArithCheck::TravCtr = 0;
StringClassMap_t ArithCheck::TransClassDecls;
TypeMap_t ArithCheck::RelevantArgStructType;
NameVarMap_t ArithCheck::VarDeclForName;
ExprTypeMap_t ArithCheck::OriginalVarType;
ExprMap_t ArithCheck::OriginalVarRef;
SgProject* ArithCheck::ACProject;

SgInitializedName* getNameForDecl(SgVariableDeclaration* decl);


void printInfo(SgBinaryOp* Parent, SgExpression* LHS, SgExpression* RHS) {
  Sg_File_Info* ParentFile = isSgNode(Parent)->get_file_info();
  printf("----------------------------------------\n");
  printf("BinaryOp: (%s, %d, %d) %s = %s\n", ParentFile->get_filenameString().c_str(),
                      ParentFile->get_line(),
                      ParentFile->get_col(),
                      isSgNode(Parent)->sage_class_name(),
                      isSgNode(Parent)->unparseToString().c_str());


  printf("Operand: LHS: %s = %s\n", isSgNode(LHS)->sage_class_name(), isSgNode(LHS)->unparseToString().c_str());
  printf("Operand: RHS: %s = %s\n", isSgNode(RHS)->sage_class_name(), isSgNode(RHS)->unparseToString().c_str());
  printf("----------------------------------------\n");
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

bool isFree(SgFunctionCallExp* fncall) {
  if(fncall->getAssociatedFunctionDeclaration() == NULL) {
    return false;
  }

  return (strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "free") == 0);
}


static SgStatement* getSurroundingStatement(SgExpression& n)
{
  return &sg::ancestor<SgStatement>(n);
}

static SgStatement* getSurroundingStatement(SgNode& n)
{
  return &sg::ancestor<SgStatement>(n);
}


static SgType* skip_PointerType(SgType* t)
{
  SgPointerType* sgptr = isSgPointerType(t);
  if (sgptr != NULL) return sgptr->get_base_type();

  return t;
}

static SgType* skip_ReferencesAndTypedefs( SgType* type ) {
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

static SgType* resolveTypedefsAndReferencesToBaseTypes(SgType* type) {
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

static SgType* skip_RefsPointersAndTypedefs(SgType* type) {
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

char* ArithCheck::getStringFor(enum OVL_TYPE op) {
  char* output = (char*)malloc(50*sizeof(char));
  switch(op) {
  case DEREF: sprintf(output, "Deref"); break;
  case CAST: sprintf(output, "Cast"); break;
  case SPECIAL_CAST: sprintf(output, "SpecialCast"); break;
  case STRUCT: sprintf(output, "Struct"); break;
  case NORMAL: sprintf(output, "Normal"); break;
  default: sprintf(output, "DontKnow"); break;
  }

  return output;
}


char* getString(VariantT Res, VariantT Op, VariantT LHS, VariantT RHS) {

  char* ResString = getStringForVariantT(Res);
  char* OpString = getStringForVariantT(Op);
  char* LHSString = getStringForVariantT(LHS);
  char* RHSString = getStringForVariantT(RHS);

  char* FunctionBuf = (char*)malloc(50*sizeof(char));
  sprintf(FunctionBuf, "%s_%s_%s_%s", ResString, OpString, LHSString, RHSString);
  //printf("FunctionBuf: %s\n", FunctionBuf);
  return FunctionBuf;
}

#if 0
nodeType getNodeType(SgNode* node) {

  if(isSgExpression(node)) {
    nodeType temp(isSgExpression(node)->get_type(), node);
    return temp;
  }
  else {
    nodeType temp(node);
    return temp;
  }
}
#endif

bool OverloadOp(SgNode* node) {

  switch(node->variantT()) {
  case V_SgAddOp:
  case V_SgSubtractOp:
  case V_SgPlusPlusOp:
  case V_SgMinusMinusOp:
  case V_SgGreaterThanOp:
  case V_SgLessThanOp:
  case V_SgGreaterOrEqualOp:
  case V_SgLessOrEqualOp:
  case V_SgEqualityOp:
  case V_SgCastExp:
  case V_SgNotEqualOp: return true;
  default: return false;
  }

}

bool isPointerDeref(SgNode* node) {
  return isSgPointerDerefExp(node);
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



// We need to treat variables of ArrayType the same way as we
// treat the PointerType -- i.e. everywhere the ArrayType is in an
// op, we need to overload it. In many cases, we wouldn't catch the
// array type, but the output of V_SgPntrArrRefExp --- which
// is similar to Deref

bool NeedsToBeOverloaded(SgNode* node) {

  switch(node->variantT()) {
  case V_SgAddOp:
  case V_SgSubtractOp:
  case V_SgPlusPlusOp:
  case V_SgMinusMinusOp:
  case V_SgPlusAssignOp:
  case V_SgMinusAssignOp:
  case V_SgAssignOp: return true;
  default: return false;
  }
}

bool isQualifyingType(SgType* type) {
  printf("QualifyingType: ");
  if(isSgPointerType(type) && isSgArrayType(type)) {
    printf("both pointer and array type!\n");
    ROSE_ASSERT(0);
  }

  if(isSgPointerType(type)) {
    printf("Pointer\n");
    return true;
  }
  else if(isSgArrayType(type)) {
    printf("Array\n");
    return true;
  }
  else {
    printf("Not qual type\n");
    return false;
  }
}

void CheckExprSanity(SgExpression* expr) {

  if(isQualifyingType(expr->get_type())) {
    if(SgBinaryOp* bop = isSgBinaryOp(expr)) {
      ROSE_ASSERT(isQualifyingType(bop->get_lhs_operand()->get_type()) ||
            isQualifyingType(bop->get_rhs_operand()->get_type()));
    }
    else if(SgUnaryOp* uop = isSgUnaryOp(expr)) {
      ROSE_ASSERT(isQualifyingType(uop->get_operand()->get_type()));
    }
    else {
      printf("Can't check sanity for expr:\n");
      printf("expr: %s = %s\n", isSgNode(expr)->sage_class_name(), isSgNode(expr)->unparseToString().c_str());
      ROSE_ASSERT(0);
    }
  }
}

bool isQualifyingLibCall(SgFunctionCallExp* fncall) {

  if(fncall->getAssociatedFunctionDeclaration() == NULL) {
    return false;
  }

  if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "free") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "printf") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__builtin_object_size") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__builtin___memset_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__inline_memset_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__builtin___memcpy_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__inline_memcpy_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__builtin___memmove_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__inline_memmove_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__builtin___strcpy_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__inline_strcpy_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__builtin___strncpy_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__inline_strncpy_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__builtin___strcat_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__inline_strcat_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__builtin___strncat_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "__inline_strncat_chk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "strlen") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "strchr") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "strpbrk") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "strspn") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "strstr") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "feof") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "fopen") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "rewind") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "fscanf") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "fclose") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "fgetc") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "fputc") == 0) {
    return true;
  }


  return false;
  //return (strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "free") == 0);

}

bool checkIfStatic(SgDeclarationStatement* decl_stmt) {

  SgDeclarationModifier& decl_mod = decl_stmt->get_declarationModifier();
  SgStorageModifier& stor_mod = decl_mod.get_storageModifier();
  printf("checkIfStatic:\n");
  printf("decl_stmt: ");
  printf("%s = %s\n", decl_stmt->sage_class_name(), decl_stmt->unparseToString().c_str());
  printf("static: ");
  if(stor_mod.isStatic()) {
    printf("true\n");
    return true;
  }
  else {
    printf("false\n");
    return false;
  }
}

SgName getNameForType(SgType* type) {
  return type->get_mangled();
}


SgBasicBlock* getBBForFn(SgFunctionDeclaration* fn_decl) {

  SgFunctionDefinition* fn_def = fn_decl->get_definition();
  ROSE_ASSERT(fn_def != NULL);
  return fn_def->get_body();
}



SgType* findInUnderlyingType(SgType* type) {

  TypeMap_t::iterator tm;

  tm = ArithCheck::UnderlyingType.begin();

  for(; tm != ArithCheck::UnderlyingType.end(); ++tm) {
    SgType* curr = (tm->first);

    if(getNameForType(type) == getNameForType(curr)) {
      SgType* ret = (tm->second);
      return ret;
    }
  }

  return NULL;
}

bool isValidStructType(SgType* type) {

  return (findInUnderlyingType(type) != NULL);

  #if 0
  TypeMap_t::iterator tm = ArithCheck::UnderlyingType.find(type);

  return (tm != ArithCheck::UnderlyingType.end());
  #endif
}



bool isOriginalClassType(SgType* type) {
  return (isSgClassType(type) && !isValidStructType(type));
}



// Not using inherited attribute since we aren't looking at classes and data
// structures for now
nodeType TopBotTrack2::evaluateSynthesizedAttribute(SgNode* node,
        nodeType inh, SynthesizedAttributesList synList) {

  printf("TopBotTrack2: Ev_Synth_Attr\n");
  printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());

  #ifdef CHECK_STATIC
  if(isSgDeclarationStatement(node)) {
    checkIfStatic(isSgDeclarationStatement(node));
  }
  #endif

  printf("Is variable declaration? ");
  if(isSgVariableDeclaration(node)) {
    printf("Yes\n");

    SgVariableDeclaration* var_decl = isSgVariableDeclaration(node);

    SgInitializedName* var_name = getNameForDecl(var_decl);

    SgType* var_type = var_name->get_type();

    // FIXME: We don't do anything to uninitialized variable decls
    // and they could be filtered out here.

    if(isQualifyingType(var_type)) {
      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }

  }
  else {
    // Not var decl at all. don't do anything
    printf("No\n");
  }


  printf("is variable reference? ");
  if(isSgVarRefExp(node)) {
    printf("Yes\n");

    SgVarRefExp* var_ref = isSgVarRefExp(node);
    SgType* var_type = var_ref->get_type();

    if(isQualifyingType(var_type)) {
      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    #ifdef CLASS_STRUCT_COPY
    else if(isOriginalClassType(var_type)) {
      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    #endif
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }

  }
  else {
    // Not var decl at all. don't do anything
    printf("No\n");
  }

  printf("Is it a function call?");
  if(isSgFunctionCallExp(node)) {
    printf("Yes\n");
    SgFunctionCallExp* fncall = isSgFunctionCallExp(node);

    // Always add... to check for implicit casts in the arguments
    ArithCheck::NodesToInstrument.push_back(node);

    SgType* fncall_type = fncall->get_type();
    if(isQualifyingType(fncall_type)) {
//      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else if(isFree(fncall)) {
//      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else if(isQualifyingLibCall(fncall)) {
//      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }


  }
  else {
    // not a func call exp. don't do anything
    printf("No\n");
  }


  // FIXME: In the case of deref exp, we can't blind move from
  // STR_TYPE to NOT_STR_TYPE since it could be a double pointer
  // in which case, the return type would also be STR_TYPE
  // We might need to investigate how our technique would handle
  // such a case since we shouldn't always return VoidStruct or
  // the POD type for a deref. The return type changes based on
  // the input type... which is not known in VoidStruct
  // But we would know this statically, so, we could use a different
  // Deref_Overload_DoublePtr in that case which would output
  // a VoidStruct

  // In case of a deref_exp we would need to check here if the
  // operand is a single or double pointer, since that defines
  // where we return a STR_TYPE or a NOT_STR_TYPE

  // FIXME: I don't think the code below detects double pointers correctly
  // And, I am not sure, how we would actually handle double pointer cases
  // This piece of code is causing the runs to crash for some reason.
  // Commenting it out
  #if 1
  printf("is it a deref exp?");
  if(isSgPointerDerefExp(node)) {
    printf("Yes\n");

    SgPointerDerefExp* ptr_deref = isSgPointerDerefExp(node);


    // Check that all derefs do in fact, operate on pointers or
    // arrays!
    SgExpression* oper = ptr_deref->get_operand();
    ROSE_ASSERT(isQualifyingType(oper->get_type()));

    // There is always work to do on a deref!
    ArithCheck::NodesToInstrument.push_back(node);

    if(isQualifyingType(ptr_deref->get_type())) {
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }


  }
  else {
    // Not a pointer deref exp. do nothing
    printf("No\n");
  }
  #endif


  printf("is it a pntr arr ref exp? ");
  if(isSgPntrArrRefExp(node)) {
    printf("Yes\n");

    SgPntrArrRefExp* array_ref = isSgPntrArrRefExp(node);

    // Check that all pnts arr refs operate on pointers or
    // arrays
    SgExpression* oper = array_ref->get_lhs_operand();
    ROSE_ASSERT(isQualifyingType(oper->get_type()));

    // There is always work to do on an pntr arr ref!
    ArithCheck::NodesToInstrument.push_back(node);

    if(isQualifyingType(array_ref->get_type())) {
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }

  }
  else {
    // Not a pointer deref exp. do nothing
    printf("No\n");
  }

  printf("is it a dot exp? ");
  if(isSgDotExp(node)) {
    printf("Yes\n");

    SgDotExp* dot_exp = isSgDotExp(node);

    // Technically, only those dot expressions dealing with
    // pointers and arrays matter to us... since the others
    // are dealing with obj.var refs.
    SgType* dot_exp_type = dot_exp->get_type();

    if(isQualifyingType(dot_exp_type)) {
      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
  }
  else {
    printf("No\n");
  }

  printf("is it an arrow exp? ");
  if(isSgArrowExp(node)) {
    printf("Yes\n");

    SgArrowExp* arrow_exp = isSgArrowExp(node);

    // Arrow expressions always work on pointers... and maybe
    // array types -- not sure how though. In any case,
    // has to qualifying type. And there's work to do!
    SgExpression* oper = arrow_exp->get_lhs_operand();
    ROSE_ASSERT(isQualifyingType(oper->get_type()));

    // There is always work to do on an arrow exp!
    ArithCheck::NodesToInstrument.push_back(node);

    if(isQualifyingType(arrow_exp->get_type())) {
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
  }
  else {
    printf("No\n");
  }

  // Handle cast exp here itself since it can create a pointer out
  // of nowhere, through arbitrary casts.
  printf("is it a cast exp? ");
  if(isSgCastExp(node)) {
    printf("Yes\n");

    SgCastExp* cast_exp = isSgCastExp(node);

    // If the operand or
    // the return type are of qualifying type, then
    // there is work to do.
    SgExpression* oper = cast_exp->get_operand();

    if(isQualifyingType(cast_exp->get_type()) ||
      isQualifyingType(oper->get_type())) {
      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
  }
  else {
    printf("No\n");
  }

  // Address Of Op can also create a qualifying type out of any variable....
  // So, we are definitely looking at a qualifying type here.
  printf("is address of op? ");
  if(isSgAddressOfOp(node)) {
    printf("Yes\n");

    SgAddressOfOp* addr_of = isSgAddressOfOp(node);

    // Assert that we create a qualifying type
    ROSE_ASSERT(isQualifyingType(addr_of->get_type()));
    ArithCheck::NodesToInstrument.push_back(node);
    nodeType synattr(STR_TYPE, INH_UNKNOWN);
    return synattr;
  }
  else {
    printf("No\n");
  }


  // Lets handle the comma op case, where the type is determined
  // by the last expression type
  // No need to add to NodesToInstrument since we don't have to
  // do anything for a comma op
  printf("is comma op?");
  if(isSgCommaOpExp(node)) {
    printf("Yes\n");

    // Find the type we are looking for in the
    // last synthesized attribute
    nodeType nt = synList[synList.size() - 1];
    if(nt.attr == NOT_STR_TYPE) {
      printf("Not returning pointer from comma op\n");
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else if(nt.attr == STR_TYPE) {
      printf("Returning pointer from comma op\n");
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      // FIXME: Should we return UNKNOWN?
      printf("FIXME: Unknown type from comma op. Need to look at node type\n");
      ROSE_ASSERT(0);
    }
  }
  else {
    //Not comma op. don't do anything
    printf("No\n");
  }


  // Conditional ops. They might take in structs, but always
  // return bool (NOT_STR_TYPE)
  printf("is it a conditional op?");
  if(isConditionalOp(node)) {
    printf("Yes\n");

    SgBinaryOp* bop = isSgBinaryOp(node);

    SgExpression* LHS = bop->get_lhs_operand();
    SgExpression* RHS = bop->get_rhs_operand();

    if(isQualifyingType(LHS->get_type()) ||
      isQualifyingType(RHS->get_type())) {
      ArithCheck::NodesToInstrument.push_back(node);
    }

    printf("Return NOT_STR_TYPE\n");
    nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
    return synattr;


  }
  else {
    // Not a conditional op. Nothing to do
    printf("No\n");
  }

  printf("is it a param list?");
  if(isSgFunctionParameterList(node)) {
    printf("Yes\n");

    SgInitializedNamePtrList& name_list = isSgFunctionParameterList(node)->get_args();

    for(SgInitializedNamePtrList::iterator iter = name_list.begin(); iter != name_list.end(); ++iter) {

      SgInitializedName* name = *iter;


      if(isQualifyingType(name->get_type())) {
        printf("Arg qualifies\n");
        ArithCheck::NodesToInstrument.push_back(node);
        break;
      }
      #ifdef CLASS_STRUCT_COPY
      if(isSgClassType(name->get_type())) {
        printf("Arg qualifies -- class type\n");
        ArithCheck::NodesToInstrument.push_back(node);
      }
      #endif
    }
  }
  else {
    // Not a param list. nothing to do
    printf("No\n");
  }





  printf("is it a func decl?");
  if(isSgFunctionDeclaration(node)) {
    printf("Yes\n");


    SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(node);
    // Find return type
    SgFunctionType* fn_type = fn_decl->get_type();
    SgType* ret_type = fn_type->get_return_type();

    if(isQualifyingType(ret_type)) {
      ArithCheck::NodesToInstrument.push_back(node);
    }
    #ifdef CLASS_STRUCT_COPY
    else if(isOriginalClassType(ret_type)) {
      ArithCheck::NodesToInstrument.push_back(node);
    }
    #endif

    printf("Always returning NOT_STR_TYPE\n");
    nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
    return synattr;



  }
  else {
    // Not a func decl, nothing to do
    printf("No\n");
  }

  #ifdef CLASS_STRUCT_COPY
  printf("is it a return stmt?");
  if(isSgReturnStmt(node)) {
    printf("Yes\n");

    // Check if its of the OriginalClassType...
    printf("is it original class type?");
    if(isOriginalClassType(isSgReturnStmt(node)->get_expression()->get_type())) {
      printf("Yes\n");
      ArithCheck::NodesToInstrument.push_back(node);
    }
    else {
      printf("No\n");
    }
    printf("Always returning NOT_STR_TYPE\n");
    nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
    return synattr;
  }
  else {
    printf("No\n");
  }
  #endif


  printf("is it an overloadable op?");
  // Arith ops...
  if(NeedsToBeOverloaded(node)) {
    printf("Yes\n");

    // These are all expressions
    ROSE_ASSERT(isSgBinaryOp(node) || isSgUnaryOp(node));
    SgExpression* expr = isSgExpression(node);
    ROSE_ASSERT(expr != NULL);
    // The result of should be of qualifying type
    // to consider this node.
    if(isQualifyingType(expr->get_type())) {
      CheckExprSanity(expr);
      ArithCheck::NodesToInstrument.push_back(node);
      nodeType synattr(STR_TYPE, INH_UNKNOWN);
      return synattr;
    }
    else {
      nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
      return synattr;
    }


  }
  else {
    // need not be overloaded. Nothing to do.
    printf("No\n");
  }


  // One golden rule which we can apply to figure out the relevant nodes -- i.e. the
  // ones that we need to manipulate -- is to see if a node actually has a get_type()
  // or rather, if a node has an associated type. If it doesn't have a type
  // associated with it, it doesn't return any value, right? this means,
  // it need not be overloaded, or changed in any way.


  //printf("Not propagating attributes upwards\n");
  //nodeType synattr(UNKNOWN, INH_UNKNOWN);

  printf("Propagating NOT_STR_TYPE\n");
  nodeType synattr(NOT_STR_TYPE, INH_UNKNOWN);
  return synattr;


}

nodeType TopBotTrack2::evaluateInheritedAttribute(SgNode* node,
                        nodeType inh) {
  #if 0
  printf("TopBotTrack2: Ev_Inh_Attr\n");
  printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());

  printf("Doing nothing right now.\n");
  #endif

  return inh;

}



#if 0
void ArithCheck::process(SgNode*) {
}

void ArithCheck::instrument(SgNode*) {
}
#endif




void ArithCheck::setProject(SgProject* proj) {
  ArithCheck::ACProject = proj;
}

void ArithCheck::registerCheck() {
  //printf("ArithCheck:registerCheck\n");
  #if 0
  CheckBase::RegisterBinaryOpCheck(this);
  #endif
}

void ArithCheck::process(BopContainer& BopCtr) {
  printf("ArithCheck:process BopCtr\n");

    // Can't do a mass copy. Need to figure out which binary ops we need.
  //SelectedBops.insert(SelectedBops.end(), BopCtr.begin(), BopCtr.end());
  for(BopContainer::iterator bpit = BopCtr.begin(); bpit != BopCtr.end();
                                bpit++) {
    SgBinaryOp* Bop = *bpit;
    process(Bop);
    }
}

bool ArithCheck::isUseful(SgBinaryOp* Bop) {

  switch(Bop->variantT()) {
  case V_SgAddOp: return true;
  case V_SgSubtractOp: return true;
  case V_SgMultiplyOp: return true;
  case V_SgDivideOp: return true;
  default: return false;
  }

    assert(0);
}



void ArithCheck::process(SgBinaryOp* Bop) {
  printf("ArithCheck:process Bop\n");

    if(isUseful(Bop)) {
    printInfo(Bop, Bop->get_lhs_operand(), Bop->get_rhs_operand());
    SelectedBops.push_back(Bop);
    }

  #if 0
  switch(Bop->variantT()) {
  case V_SgAddOp: process(isSgAddOp(Bop))
  #endif
}

void ArithCheck::process(SgNode* Node) {
  printf("ArithCheck:process Node. Blank\n");

}


void ArithCheck::insertLibHeader() {

    SgProject* project = ArithCheck::ACProject;

    SgFilePtrList file_list = project->get_fileList();
  SgFilePtrList::iterator iter;
  for(iter = file_list.begin(); iter!=file_list.end(); iter++) {
    SgSourceFile* cur_file = isSgSourceFile(*iter);
    SgGlobal* global_scope = cur_file->get_globalScope();
//    SageInterface::insertHeader("metadata.h", PreprocessingInfo::after,false,global_scope);
    // Removed Rajesh Sept 11 5PM
    //SageInterface::insertHeader("foo2_c.h", PreprocessingInfo::after,false,global_scope);
    //SageInterface::insertHeader("ptr_hdr.h", PreprocessingInfo::after,false,global_scope);
  }



}

SgStatement* ArithCheck::getSuitablePrevStmt(SgStatement* stmt) {

  SgNode* parent = isSgNode(stmt)->get_parent();
  printf("Parent: %s = %s\n", isSgNode(parent)->sage_class_name(), isSgNode(parent)->unparseToString().c_str());

  if(isSgForInitStatement(parent)) {
    printf("Found forloop case.\n");
    // This is a for loop initialization statement.
    // We can't put just the variable declaration here, without
    // any initialization. So, lets put it prior to the
    // this statement
    SgNode* forloopstmt = parent->get_parent();
    printf("ForInit: %s = %s\n", isSgNode(forloopstmt)->sage_class_name(), isSgNode(forloopstmt)->unparseToString().c_str());
    ROSE_ASSERT(isSgForStatement(isSgStatement(forloopstmt)));
    return isSgStatement(forloopstmt);
  }
  return stmt;

}


SgType* ArithCheck::findInRelevantStructType(SgType* type) {

  TypeMap_t::iterator tm;

  tm = ArithCheck::RelevantStructType.begin();

  for(; tm != ArithCheck::RelevantStructType.end(); ++tm) {
    SgType* curr = (tm->first);

    if(getNameForType(type) == getNameForType(curr)) {
      SgType* ret = (tm->second);
      return ret;
    }
  }

  return NULL;
}



SgType* ArithCheck::getStructType(SgType* type, SgNode* pos, bool create) {

  // Expect a pointer type since RelevantStructType has pointer types
  // within it, and createStruct expects pointer type
  ROSE_ASSERT(isSgPointerType(type));

  #if 0

  TypeMap_t::iterator tm = ArithCheck::RelevantStructType.find(type);

  bool found = (tm != ArithCheck::RelevantStructType.end());

  #endif

  bool found = (findInRelevantStructType(type) != NULL);

  if(!found) {
    if(!create) {
      printf("Struct type for given type not found.\n");
      ROSE_ASSERT(0);
    }
    else {
      ROSE_ASSERT(!found && create);
      ROSE_ASSERT(pos != NULL);
      SgType* str_type = createStruct(type, pos);

      ROSE_ASSERT(findInRelevantStructType(type) != NULL);

      #if 0
      // Checking if createStruct did its job
      tm = ArithCheck::RelevantStructType.find(type);
      ROSE_ASSERT(tm != ArithCheck::RelevantStructType.end());
      #endif

    }
  }

  // Found struct type. return it.
  return findInRelevantStructType(type);
}

SgType* ArithCheck::getStructTypeIfPossible(SgType* type) {

  return findInRelevantStructType(type);

  #if 0
  TypeMap_t::iterator tm = ArithCheck::RelevantStructType.find(type);

  return tm->second;
  #endif
}



#if 0
char* ArithCheck::getStringFor(enum OVL_TYPE op) {

  char* opstring = (char*)malloc(50*sizeof(char));

  switch(op) {
  case DEREF: sprintf(opstring, "Deref"); break;
  case CAST:  sprintf(opstring, "Cast"); break;
  case STRUCT: sprintf(opstring, "Struct"); break;
  case NORMAL: sprintf(opstring, "Normal"); break;
  default: ROSE_ASSERT(0);
  }
}
#endif

char* ArithCheck::getStringFor(SgBinaryOp* bop, enum OVL_TYPE op) {

  char* BString = getStringForVariantT(bop->variantT());
  char* OpString = getStringFor(op);

  char* output = (char*)malloc(50*sizeof(char));
  sprintf(output, "%s_%s", OpString, BString);

  return output;

}

bool ArithCheck::compareNames(SgName first, SgName second) {

  return (first == second);

}

bool ArithCheck::compareTypes(SgType* first, SgType* second) {

  return (first->get_mangled() == second->get_mangled());

}


void ArithCheck::printNode(SgNode* node) {
  printf("node: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
}

void ArithCheck::printNodeExt(SgNode* node) {
  Sg_File_Info* File = isSgNode(node)->get_file_info();
  printNode(node);
  printf("File: (%s, %d, %d) %s = %s\n", File->get_filenameString().c_str(),
                    File->get_line(),
                    File->get_col(),
                    isSgNode(node)->sage_class_name(),
                    isSgNode(node)->unparseToString().c_str());

}

void ArithCheck::createFuncInfo(SgName s_name, SgType* retType, SgExprListExp* parameter_list) {

  class FuncInfo* fi = new class FuncInfo;
  fi->fn_name = s_name;
//  fi->retType = isSgType(deepCopyNode(isSgNode(retType)));
  fi->retType = retType;
  printf("retType\n");
  printNode(isSgNode(retType));
  printf("fi->retType\n");
  printNode(isSgNode(fi->retType));
  fi->ParamTypes = new SgTypeList;

  SgExpressionPtrList& expr_list = parameter_list->get_expressions();

  SgExpressionPtrList::iterator iter = expr_list.begin();

  for(iter = expr_list.begin(); iter != expr_list.end(); iter++) {

    printf("expr\n");
    printNode(*iter);
    SgType* expr_type = (*iter)->get_type();
    fi->ParamTypes->push_back(expr_type);
    //SgType* expr_type_copy = isSgType(deepCopyNode(isSgNode(expr_type)));
    //fi->ParamTypes->push_back(expr_type_copy);
    printf("expr_type\n");
    printNode(isSgNode(expr_type));
//    printf("expr_type_copy\n");
//    printNode(isSgNode(expr_type_copy));
  }

  // A few checks
  ROSE_ASSERT(fi->fn_name == s_name);
  // Using get_mangled to compare types
  ROSE_ASSERT(fi->retType->get_mangled() == retType->get_mangled());
  ROSE_ASSERT(fi->ParamTypes->size() == expr_list.size());

  // Add this to the ForwardDecls list
  ForwardDecls.push_back(fi);

}



bool ArithCheck::checkIfDeclExists(SgName s_name, SgType* retType, SgExprListExp* parameter_list) {
  printf("checkIfDeclExists\n");

  FuncInfoList::iterator iter;

  for(iter = ForwardDecls.begin(); iter != ForwardDecls.end(); iter++) {

    class FuncInfo* fi = *iter;

    bool name_check = compareNames(s_name, fi->fn_name);

    // This name doesn't match... try the next one in ForwardDecls
    if(!name_check) {
      printf("name doesn't match\n");
      continue;
    }

    bool ret_type_check = compareTypes(retType, fi->retType);

    // The ret type doesn't match... try the next one in ForwardDecls
    if(!ret_type_check) {
      continue;
    }

    // Check params

    SgTypeList* paramTypes = fi->ParamTypes;

    SgExpressionPtrList& expr_list = parameter_list->get_expressions();

    // Size mismatch -- can't be the same function -- try next one
    if(paramTypes->size() != expr_list.size()) {
      continue;
    }

    SgTypeList::iterator iter2;
    SgExpressionPtrList::iterator iter3 = expr_list.begin();

    bool param_match = true;

    for(iter2 = paramTypes->begin(); iter2 != paramTypes->end(); iter2++, iter3++) {

      SgType* stored_type = *iter2;
      SgType* expr_type = (*iter3)->get_type();

      // Type mismatch
      if(!compareTypes(stored_type, expr_type)) {
        param_match = false;
        break;
      }
    }

    if(param_match) {
      // name, ret_type and params matched. must be correct
      return true;
    }
  }

  printf("Can't find the decl. Creating one\n");
  // None of the decls matched. lets create a new one here.
  createFuncInfo(s_name, retType, parameter_list);

  // Recursive call to check if the createFuncInfo worked
  ROSE_ASSERT(checkIfDeclExists(s_name, retType, parameter_list));

  return false;

}

SgStatement* ArithCheck::findInsertLocationFromParams(SgExprListExp* param_list) {

  SgExpressionPtrList& exprs = param_list->get_expressions();

  ROSE_ASSERT(exprs.size() >= 1);
  SgExpressionPtrList::iterator iter = exprs.begin();
  SgExpression* arg1 = *iter;
  SgStatement* loc = findInsertLocation(arg1);

  return loc;
}

void ArithCheck::makeDeclStatic(SgDeclarationStatement* decl_stmt) {

  // If its already static, nothing to do.
  if(checkIfStatic(decl_stmt)) {
    return;
  }

  SgDeclarationModifier& decl_mod = decl_stmt->get_declarationModifier();
  SgStorageModifier& stor_mod = decl_mod.get_storageModifier();
  stor_mod.setStatic();

  ROSE_ASSERT(stor_mod.isStatic());

  return;
}

SgFunctionDeclaration* ArithCheck::createDefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
              SgScopeStatement* scope, SgProject* project, SgNode* pos) {
  printf("Creating Defining decl\n");

  #if 0
  SgGlobal* globalScope = getFirstGlobalScope(project);
  pushScopeStack(globalScope);
  #endif

  SgFunctionParameterList* new_param_list = buildFunctionParameterList();

  SgExpressionPtrList& exprs = param_list->get_expressions();

  printf("creating param_list\n");

  unsigned int arg_counter = 0;
  for(SgExpressionPtrList::iterator iter = exprs.begin(); iter != exprs.end();
                              iter++) {
    arg_counter++;
    SgExpression* ce = *iter;
    printf("ce: %s = %s\n", isSgNode(ce)->sage_class_name(), isSgNode(ce)->unparseToString().c_str());
    std::string arg_name = "input" + boost::lexical_cast<std::string>(arg_counter);
    SgInitializedName* new_name = buildInitializedName(SgName(arg_name),ce->get_type(), NULL);
    printf("created new_name\n");
    // FIXME: Scope should be fndecl, not globalScope.
    //new_name->set_scope(globalScope);
    //new_name->set_scope(new_param_list->get_scope());
    new_param_list->append_arg(new_name);
    new_name->set_parent(new_param_list);
  }

  printf("creating defdecl\n");

  // Working one
  #if 0
  SgFunctionDeclaration* defdecl = buildDefiningFunctionDeclaration(transf_name,
                                    retType,
                                    new_param_list);
  #else
  SgFunctionDeclaration* defdecl = buildDefiningFunctionDeclaration(transf_name,
                                    retType,
                                    new_param_list,
                                    isSgStatement(pos)->get_scope());
  #endif

  // Instead of simply placing the defdecl at the top of the file... we'll
  // place it as close to the use as possible. This should ensure that all the
  // necessary data structures used, are declared.
//  prependStatement(isSgStatement(defdecl));
//  SgStatement* loc = findInsertLocationFromParams(param_list);
//  insertStatementBefore(loc, defdecl);
//  insertStatementBefore(scope, defdecl);

  #if 0
  insertStmtAfterLast(defdecl, globalScope);
  #else
  insertStatementBefore(isSgStatement(pos), defdecl);
  #endif


  #ifdef MAKE_DECLS_STATIC
  makeDeclStatic(isSgDeclarationStatement(defdecl));
  #endif

  #if 0
  popScopeStack();
  #endif

  return defdecl;
}

SgExprStatement* ArithCheck::createDotAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS) {

  // for the ptr exp
  SgExpression* str_exp = createDotExpFor(str_name, str_elem);

  // build the assign op: output.ptr = input1
  SgExpression* str_assign = buildAssignOp(str_exp, RHS);

  SgExprStatement* str_stmt = buildExprStatement(str_assign);

  return str_stmt;
}

SgExprStatement* ArithCheck::createArrowAssign(SgInitializedName* str_name, std::string str_elem, SgExpression* RHS) {

  // for the ptr exp
  SgExpression* str_exp = createArrowExpFor(str_name, str_elem);

  // build the assign op: output.ptr = input1
  SgExpression* str_assign = buildAssignOp(str_exp, RHS);

  SgExprStatement* str_stmt = buildExprStatement(str_assign);

  return str_stmt;
}


void ArithCheck::appendReturnStmt(SgExpression* ret_exp, SgBasicBlock* fnbody) {

  SgReturnStmt* ret_stmt = buildReturnStmt(ret_exp);

  fnbody->append_statement(ret_stmt);
}

SgType* ArithCheck::getSizeOfType() {
  return SgTypeUnsignedLong::createType();
}

void ArithCheck::handleCreateStructDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                    SgScopeStatement* scope, SgProject* project, SgNode* pos) {
  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);

  // There are two types of functions which end up here.
  // 1. create_struct(orig_ptr, &orig_ptr)
  // translates to create_struct(input1, input2)
  // str output;
  //output.ptr = input1;
  //output.addr = input2;
  //return output;

  // 2. create_struct(orig_ptr, &orig_ptr, size)
  // translates to create_struct(input1, input2, input3)
  // str output;
  //output.ptr = input1;
  //output.addr = input2;
  //create_entry(input2, reinterpret_cast<unsigned long long>(input1), input3);
  //return output;

  // In the second case, we actually create an entry using the size argument.
  // The create_entry library function will handle this. We don't need to emulate that
  // part.

  // Assert that we are returning a struct type...
  ROSE_ASSERT(isValidStructType(retType));

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // create local variable of return type
  SgVariableDeclaration* output_var = createLocalVariable("output", retType, NULL, fnbody);

  // Both are equivalent...
  //getScope(fnbody)->insertStatementInScope(output_var, true);
  fnbody->append_statement(output_var);

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  SgInitializedNamePtrList::iterator iter = args.begin();
  ROSE_ASSERT(args.size() >= 2);

  // Lets pluck the first argument out of the fndecl
  SgInitializedName* arg1 = *iter;

  // Now, pluck the second argument out
  iter++;
  SgInitializedName* arg2 = *iter;


  SgExprStatement* ptr_assign = createDotAssign(getNameForDecl(output_var), "ptr", buildVarRefExp(arg1));
  fnbody->append_statement(ptr_assign);

  SgExprStatement* addr_assign = createDotAssign(getNameForDecl(output_var), "addr", buildVarRefExp(arg2));
  fnbody->append_statement(addr_assign);

  // Now, use buildOverloadFn to form a create_entry, if a third argument exists.
  if(args.size() == 3) {
    // Get the third arg.
    ++iter;
    SgInitializedName* arg3 = *iter;

    // the function call: create_entry(input2, reinterpret_cast<unsigned long long>(input1), input3);
    SgExpression* ce_arg1 = buildVarRefExp(arg2);
    SgExpression* ce_arg2 = castToAddr(buildVarRefExp(arg1));
    // cast it to size type...
    //SgExpression* ce_arg3 = buildCastExp(buildVarRefExp(arg3), getSizeOfType(), SgCastExp::e_reinterpret_cast);
    SgExpression* ce_arg3 = buildCastExp(buildVarRefExp(arg3), getSizeOfType(), SgCastExp::e_C_style_cast);

    // Working one
    //SgExpression* ce_call = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), fnbody, findInsertLocation(fnbody));
    SgExpression* ce_call = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), fnbody, fndecl);

    SgExprStatement* ce_stmt = buildExprStatement(ce_call);

    fnbody->append_statement(ce_stmt);
  }

  // Now, for the return value
  // the return stmt: return output;
  appendReturnStmt(buildVarRefExp(output_var), fnbody);
}

SgFunctionDefinition* ArithCheck::getFuncDef(SgFunctionDeclaration* fndecl) {
  SgFunctionDefinition* fndef = fndecl->get_definition();
  ROSE_ASSERT(fndef != NULL);
  return fndef;
}

SgBasicBlock* ArithCheck::getFuncBody(SgFunctionDefinition* fndef) {
  SgBasicBlock* bb = fndef->get_body();
  ROSE_ASSERT(bb != NULL);
  return bb;
}


void ArithCheck::handleCreateStructFromAddrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                  SgScopeStatement* scope, SgProject* project, SgNode* pos) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // This fn takes in an address, and create a struct out of it.
  // fn call: create_struct_from_addr(input1)
  // str output;
  // output.ptr = *(reinterpret_cast<class node**>(input1));
  // output.addr = input1;
  // return output;
  //
  // Convert the input1 to the addr of the underlying type of the return type. :)

  // Assert that we are returning a struct type...
  ROSE_ASSERT(isValidStructType(retType));

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // create local variable of return type
  SgVariableDeclaration* output_var = createLocalVariable("output", retType, NULL, fnbody);

  // Both are equivalent...
  //getScope(fnbody)->insertStatementInScope(output_var, true);
  fnbody->append_statement(output_var);

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  // create_struct_using_addr has only one addr as arg
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Lets pluck the first argument out of the fndecl
  SgInitializedName* arg1 = *iter;

  // Create the RHS for the first assignment above: output.ptr = *(reinterpret_cast<class node**>(input1))
  SgExpression* input_ref = buildVarRefExp(arg1);
  // Pointer to the underlying ptr, in the return type
  SgType* ret_type_ptr = findInUnderlyingType(retType);
  ROSE_ASSERT(ret_type_ptr != NULL);
  SgType* ptr_of = SgPointerType::createType(ret_type_ptr);
//  SgExpression* input_casted = buildCastExp(input_ref, ptr_of, SgCastExp::e_reinterpret_cast);
  SgExpression* input_casted = buildCastExp(input_ref, ptr_of, CAST_TYPE);
  // Now, deref it.
  SgExpression* input_cast_deref = buildPointerDerefExp(input_casted);

  // this is the ptr assignment: output.ptr = *(reinterpret_cast<class node**>(input1))
  SgExprStatement* ptr_assign = createDotAssign(getNameForDecl(output_var), "ptr", input_cast_deref);
  fnbody->append_statement(ptr_assign);

  // this is the addr assignment: output.addr = input
  SgExprStatement* addr_assign = createDotAssign(getNameForDecl(output_var), "addr", buildVarRefExp(arg1));
  fnbody->append_statement(addr_assign);

  // the return stmt: return output
  appendReturnStmt(buildVarRefExp(output_var), fnbody);

}

void ArithCheck::handleDerefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* pos) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // fn call: deref(input1_str)
  // check_entry((unsigned long long)input1_str.ptr, input1_str.addr)
  // return input_str.ptr;

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  // deref has only one str arg
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // the arg is of struct type.
  ROSE_ASSERT(isValidStructType(arg1->get_type()));

  // get input1_str.ptr and cast it to unsigned long long
  SgExpression* input1_ptr = castToAddr(createDotExpFor(arg1, "ptr"));
  // get input1_str.addr
  SgExpression* input1_addr = createDotExpFor(arg1, "addr");

  // The alternative to "pos" would be findInsertLocation(fnbody) -- this may not work with the Namespace Definition stuff
  // in C++, since the scope of this declaration won't be the same of us insertion...
  // Working one
  #if 0
  SgExpression* ce = buildOverloadFn("check_entry", input1_ptr, input1_addr, SgTypeVoid::createType(), scope, pos);
  #else
  SgExpression* ce = buildOverloadFn("check_entry", input1_ptr, input1_addr, SgTypeVoid::createType(), scope, fndecl);
  #endif

  SgExprStatement* ce_stmt = buildExprStatement(ce);
  fnbody->append_statement(ce_stmt);

  appendReturnStmt(createDotExpFor(arg1, "ptr"), fnbody);

}

void ArithCheck::handlePntrArrRefDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* pos) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // fn call: pntrarrref(input1_str, index)
  // return input1_str.ptr + index;

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  // pntrarrref has two args. first is of struct type
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // the first arg is of struct type
  ROSE_ASSERT(isValidStructType(arg1->get_type()));

  // create pointer dot exp
  SgExpression* ptr_exp = createDotExpFor(arg1, "ptr");

  // get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  // Now, create the add op, in the return stmt...
  appendReturnStmt(buildAddOp(ptr_exp, buildVarRefExp(arg2)), fnbody);

}


void ArithCheck::handleAssignDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* pos) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // Two cases:
  // Case 1:
  // this fn takes in two structs. first by ref, second by val
  // and performs as "assignment".
  // fncall: assign(*input1_str, input2_str)
  // input1_str->ptr = input2_str.ptr;
  // create_entry(input1_str->addr, input2_str.addr);
  // return *input1_str;
  //
  // Case 2:
  // this fn takes in one struct and a char* (string val)
  // and performs an "assignment".
  // fncall: assign(*input_str, input2)
  // input1_str->ptr = input2
  // create_entry(input1_str->addr, input1_str->ptr, sizeof(input2))
  // return *input1_str;

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  // assign has two args. first is of pointer type
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // first arg is pointer type -- pointer to struct
  ROSE_ASSERT(isSgPointerType(arg1->get_type()));

  SgExprStatement* ce_stmt;
  SgExpression* ce_call;

  // Get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  if(isValidStructType(arg2->get_type())) {
  // second arg is struct type

    // input2_str.ptr
    SgExpression* ptr_exp = createDotExpFor(arg2, "ptr");

    // input1_str->ptr = input2_str.ptr
    SgExprStatement* ptr_assign = createArrowAssign(arg1, "ptr", ptr_exp);
    fnbody->append_statement(ptr_assign);

    // create_entry fn call: create_entry(input1_str->addr, input2_str.addr)
    SgExpression* ce_arg1 = createArrowExpFor(arg1, "addr");
    SgExpression* ce_arg2 = createDotExpFor(arg2, "addr");

    // Working one
    // FIXME: This might need to be changed to comply with the Namespace Definition stuff in C++
    //ce_call = buildOverloadFn("create_entry", ce_arg1, ce_arg2, SgTypeVoid::createType(), scope, findInsertLocation(fnbody));
    ce_call = buildOverloadFn("create_entry", ce_arg1, ce_arg2, SgTypeVoid::createType(), scope, fndecl);


  }
  else {
    // this is a value -- like a string value..., or a char pointer

    // input1_str.ptr = input2
    SgExprStatement* ptr_assign = createArrowAssign(arg1, "ptr", buildVarRefExp(arg2));
    fnbody->append_statement(ptr_assign);

    // create entry fn call: create_entry(input1_str->addr, input1_str.ptr, sizeof(input2))
    SgExpression* ce_arg1 = createArrowExpFor(arg1, "addr");
    SgExpression* ce_arg2 = castToAddr(createArrowExpFor(arg1, "ptr"));
    SgExpression* size_of_arg2 = buildSizeOfOp(buildVarRefExp(arg2));
    SgExpression* ce_arg3 = buildCastExp(size_of_arg2, getSizeOfType(), SgCastExp::e_C_style_cast);

    // Working one
    // FIXME: This might need to be changed to comply with the Namespace Definition stuff in C++
    //ce_call = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, findInsertLocation(fnbody));
    ce_call = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, fndecl);

  }

  ce_stmt = buildExprStatement(ce_call);

  fnbody->append_statement(ce_stmt);

  // return *input1_str
  appendReturnStmt(buildPointerDerefExp(buildVarRefExp(arg1)), fnbody);

}

void ArithCheck::handleIncrDecrDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* pos, bool isDecr) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // this fn takes in a single struct, by ref
  // fncall: increment/decrement(*input1_str)
  // for increment: input1_str->ptr++;
  // for decrement: input1_str->ptr--;
  // return *input1_str;

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // first arg is pointer type -- pointer to struct
  ROSE_ASSERT(isSgPointerType(arg1->get_type()));

  // create an arrow exp, and perform incr/decr on it.
  SgExpression* ptr_exp = createArrowExpFor(arg1, "ptr");

  SgExpression* incr_decr_exp;
  if(isDecr) {
    incr_decr_exp = buildMinusMinusOp(ptr_exp);
  }
  else {
    incr_decr_exp = buildPlusPlusOp(ptr_exp);
  }

  SgExprStatement* incr_decr_stmt = buildExprStatement(incr_decr_exp);
  fnbody->append_statement(incr_decr_stmt);

  // return *input_str
  appendReturnStmt(buildPointerDerefExp(buildVarRefExp(arg1)), fnbody);

}

SgExpression* ArithCheck::getCondExpOper(SgInitializedName* var_name) {

  // create the appropriate oper --
  // if arg1 is a struct, create str.ptr
  // else just create a reference to the argument...
  SgExpression* oper;

  if(isValidStructType(var_name->get_type())) {
    oper = createDotExpFor(var_name, "ptr");
  }
  else {
    // anything else, will be seen within the function as
    // a variable...
    oper = buildVarRefExp(var_name);
  }

  return oper;
}

SgExpression* ArithCheck::createConditionalExp(SgName s_name, SgExpression* lhs, SgExpression* rhs) {
  // For reference
  #if 0
  case V_SgLessThanOp: sprintf(output, "LessThan"); break;
  case V_SgGreaterThanOp: sprintf(output, "GreaterThan"); break;
  case V_SgNotEqualOp: sprintf(output, "NotEqual"); break;
  case V_SgLessOrEqualOp: sprintf(output, "LessOrEqual"); break;
  case V_SgGreaterOrEqualOp: sprintf(output, "GreaterOrEqual"); break;
  case V_SgEqualityOp: sprintf(output, "Equality"); break;
  #endif

  SgExpression* cond_exp;

  if(compareNames(s_name, "LessThan")) {
    cond_exp = buildLessThanOp(lhs, rhs);
  }
  else if(compareNames(s_name, "GreaterThan")) {
    cond_exp = buildGreaterThanOp(lhs, rhs);
  }
  else if(compareNames(s_name, "NotEqual")) {
    cond_exp = buildNotEqualOp(lhs, rhs);
  }
  else if(compareNames(s_name, "LessOrEqual")) {
    cond_exp = buildLessOrEqualOp(lhs, rhs);
  }
  else if(compareNames(s_name, "GreaterOrEqual")) {
    cond_exp = buildGreaterOrEqualOp(lhs, rhs);
  }
  else if(compareNames(s_name, "Equality")) {
    cond_exp = buildEqualityOp(lhs, rhs);
  }
  else {
    printf("Unsupported conditional op: %s\n", s_name.getString().c_str());
    ROSE_ASSERT(0);
  }

  return cond_exp;

}

bool ArithCheck::isCondString(SgName s_name) {

  if(compareNames(s_name, "LessThan")) {
    return true;
  }
  else if(compareNames(s_name, "GreaterThan")) {
    return true;
  }
  else if(compareNames(s_name, "NotEqual")) {
    return true;
  }
  else if(compareNames(s_name, "LessOrEqual")) {
    return true;
  }
  else if(compareNames(s_name, "GreaterOrEqual")) {
    return true;
  }
  else if(compareNames(s_name, "Equality")) {
    return true;
  }
  else {
    return false;
  }


}

void ArithCheck::handleConditionalDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgName s_name, SgNode* node) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, node);
  // this fn takes in a single struct, by ref
  // fncall: cond(*input1_str)
  // return lhs.ptr (cond) rhs.ptr;

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;

  // create the appropriate lhs expr --
  // if arg1 is a struct, create str.ptr
  // else just create a reference to the argument...
  SgExpression* lhs_exp;

  if(isValidStructType(arg1->get_type())) {
    lhs_exp = createDotExpFor(arg1, "ptr");
  }
  else {
    // anything else, will be seen within the function as
    // a variable...
    lhs_exp = buildVarRefExp(arg1);
  }

  // Get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  SgExpression* rhs_exp;
  // treat as first arg
  if(isValidStructType(arg2->get_type())) {
    rhs_exp = createDotExpFor(arg2, "ptr");
  }
  else {
    rhs_exp = buildVarRefExp(arg2);
  }

  SgExpression* cond_exp = createConditionalExp(s_name, lhs_exp, rhs_exp);

  appendReturnStmt(cond_exp, fnbody);
}

void ArithCheck::handleAddSubDefs(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* pos, bool isSub) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // this fn takes in a single struct, by ref
  // fncall: add/sub(input1_str, input2)
  // for addition: str.ptr += val;
  // for subtraction: str.ptr -= val;
  // return str;

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // get the first arg
  SgInitializedName* arg1 = *iter;
  ROSE_ASSERT(isValidStructType(arg1->get_type()));

  // get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  SgExpression* arith_assign;
  if(isSub) {
    arith_assign = buildMinusAssignOp(createDotExpFor(arg1, "ptr"), buildVarRefExp(arg2));
  }
  else {
    arith_assign = buildPlusAssignOp(createDotExpFor(arg1, "ptr"), buildVarRefExp(arg2));
  }
  SgExprStatement* arith_stmt = buildExprStatement(arith_assign);
  fnbody->append_statement(arith_stmt);

  // pass the struct back.. in return
  appendReturnStmt(buildVarRefExp(arg1), fnbody);
}

void ArithCheck::handleValueToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
              SgScopeStatement* scope, SgProject* project) {

  // this is a value to struct cast. So, no metadata to be recorded.
  // the address would be the address of the output.ptr --- FIXME: NOT SURE if this right...
  // the fncall: Cast(input1)
  // str output;
  // output.ptr = reinterpret_cast<(str.ptr type)>(input1);
  // output.addr = &output.ptr
  // return output;

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // the first arg...
  SgInitializedName* arg1 = *iter;

  // create local variable of return type: str output;
  SgVariableDeclaration* output_var = createLocalVariable("output", retType, NULL, getScope(fnbody));
  fnbody->append_statement(output_var);

  // now: output.ptr = reinterpret_cast<(str.ptr type)>(input1);
  SgType* ptr_type = findInUnderlyingType(retType);
//  SgExpression* rhs = buildCastExp(buildVarRefExp(arg1), ptr_type, SgCastExp::e_reinterpret_cast);
  SgExpression* rhs = buildCastExp(buildVarRefExp(arg1), ptr_type, CAST_TYPE);

  SgExprStatement* ptr_assign = createDotAssign(getNameForDecl(output_var), "ptr", rhs);
  fnbody->append_statement(ptr_assign);

  // now: output.addr = &output.ptr
  SgType* addr_type = getAddrType();
  SgExpression* ptr_exp = createDotExpFor(getNameForDecl(output_var), "ptr");
  SgExpression* addr_ptr_exp = createAddressOfOpFor(ptr_exp);
//  SgExpression* addr_rhs = buildCastExp(addr_ptr_exp, addr_type, SgCastExp::e_reinterpret_cast);
  // Changing it from output.addr = &output.ptr
  // to output.addr = NULL (represented as an integer 0)
  #if 0
  SgExpression* addr_rhs = buildCastExp(addr_ptr_exp, addr_type, CAST_TYPE);
  SgExprStatement* addr_assign = createDotAssign(getNameForDecl(output_var), "addr", addr_rhs);
  #else
  SgExprStatement* addr_assign = createDotAssign(getNameForDecl(output_var), "addr", buildIntVal(0));
  #endif

  fnbody->append_statement(addr_assign);

  // now: return output;
  appendReturnStmt(buildVarRefExp(output_var), fnbody);

}

void ArithCheck::handleStructToValueCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
              SgScopeStatement* scope, SgProject* project) {

  // this is a struct to value cast. So, we simply cast the pointer
  // within the struct to the ret Type.
  // the fncall: Cast(input_str)
  // return reinterpret_cast<(addr type)>(input_str.ptr);

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // the first arg...
  SgInitializedName* arg1 = *iter;

  SgExpression* dot_exp = createDotExpFor(arg1, "ptr");
//  SgExpression* casted_exp = buildCastExp(dot_exp, getAddrType(), SgCastExp::e_reinterpret_cast);
  SgExpression* casted_exp = buildCastExp(dot_exp, getAddrType(), CAST_TYPE);

  // send this through the return stmt
  appendReturnStmt(casted_exp, fnbody);

}

void ArithCheck::handleStructToStructCastDef(SgFunctionDeclaration* fndecl, SgType* retType, SgExprListExp* param_list,
              SgScopeStatement* scope, SgProject* project) {

  // this is a struct to struct cast. probably the most common type.
  // here, we cast the pointer from the input type, to the output type (return type)
  // copy the address over as is. this simple copy should be okay since
  // this operation by itself doesn't change the metadata anyways.
  // the fncall: Cast(input_str)
  // struct __Pb__Ui__Pe__ output;
  // output.ptr = (output_str.ptr type*)input_str.ptr;
  // output.addr = input_str.addr;
  // return output;

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  SgInitializedNamePtrList::iterator iter = args.begin();

  // the first arg...
  SgInitializedName* arg1 = *iter;

  // create local variable of return type: str output;
  SgVariableDeclaration* output_var = createLocalVariable("output", retType, NULL, getScope(fnbody));
  fnbody->append_statement(output_var);

  // now: output.ptr = (output.ptr type*)input_str.ptr;
  SgExpression* input_ptr_exp = createDotExpFor(arg1, "ptr");
  SgType* ret_type_ptr = findInUnderlyingType(retType);
//  SgExpression* casted_input_ptr_exp = buildCastExp(input_ptr_exp, ret_type_ptr, SgCastExp::e_reinterpret_cast);
  SgExpression* casted_input_ptr_exp = buildCastExp(input_ptr_exp, ret_type_ptr, CAST_TYPE);
  SgExprStatement* ptr_assign = createDotAssign(getNameForDecl(output_var), "ptr", casted_input_ptr_exp);
  fnbody->append_statement(ptr_assign);

  // now: output.addr = input_str.addr;
  SgExpression* input_addr_exp = createDotExpFor(arg1, "addr");
  SgExprStatement* addr_assign = createDotAssign(getNameForDecl(output_var), "addr", input_addr_exp);
  fnbody->append_statement(addr_assign);

  // return output;
  appendReturnStmt(buildVarRefExp(output_var), fnbody);

}



void ArithCheck::handleCastDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
              SgScopeStatement* scope, SgProject* project, SgNode* pos) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // three cases
  // 1. value to struct
  // 2. struct to struct
  // 3. struct to value


  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  ROSE_ASSERT(args.size() == 1);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // get the first arg
  SgInitializedName* arg1 = *iter;

  if(!isValidStructType(arg1->get_type())) {
    // The output should be a struct in this case.
    ROSE_ASSERT(isValidStructType(retType));
    // This is a value to struct...
    handleValueToStructCastDef(fndecl, retType, param_list, scope, project);
  }
  else {
    // in the other two cases, the argument is a struct
    ROSE_ASSERT(isValidStructType(arg1->get_type()));

    if(!isValidStructType(retType)) {
      // returning a value from a struct
      handleStructToValueCastDef(fndecl, retType, param_list, scope, project);
    }
    else {
      // struct to struct cast
      handleStructToStructCastDef(fndecl, retType, param_list, scope, project);
    }
  }
}

void ArithCheck::handleStructAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* pos) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // This is the function that handles something like "&ptr"... This turns into... "&str"...
  // and the fncall here looks like
  // fncall: AddressOf(input1_ptr, input2)
  // body: input1_ptr->ptr = input2.addr (since we are taking the address of the pointer)
  //       input1_ptr->addr = &input1_ptr->ptr (this is the variable that comes in from the caller)
  //   create_entry(input1_ptr->addr, (unsigned long long)input1_ptr->ptr, sizeof(input2.addr))
  //       return (void)

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // get the first arg
  SgInitializedName* arg1 = *iter;

  // get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  // input2.addr
  SgExpression* input2_addr = createDotExpFor(arg2, "addr");
  SgExpression* input2_addr_casted = buildCastExp(input2_addr, findInUnderlyingType(isSgPointerType(arg1->get_type())->get_base_type()), SgCastExp::e_C_style_cast);

  // input1_ptr->ptr = input2.addr
  SgExprStatement* ptr_assign = createArrowAssign(arg1, "ptr", input2_addr_casted);
  fnbody->append_statement(ptr_assign);

  // input1_ptr->addr = &input1_ptr->ptr;
  SgExpression* ptr_addr = castToAddr(createAddressOfOpFor(createArrowExpFor(arg1, "ptr")));
  SgExprStatement* addr_assign = createArrowAssign(arg1, "addr", ptr_addr);
  fnbody->append_statement(addr_assign);

  // create_entry(input1_ptr->addr, (unsigned long long)input1_ptr->ptr, sizeof(input2.addr))
  SgExpression* ce_arg1 = createArrowExpFor(arg1, "addr");
  SgExpression* ce_arg2 = castToAddr(createArrowExpFor(arg1, "ptr"));
  SgExpression* ce_arg3 = buildCastExp(buildSizeOfOp(createDotExpFor(arg2, "addr")), getSizeOfType(), SgCastExp::e_C_style_cast);

  // Working one
  #if 0
  SgExpression* overload = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, findInsertLocation(fnbody));
  #else
//  SgExpression* overload = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, pos);
  SgExpression* overload = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, fndecl);
  #endif
  SgExprStatement* ovl_stmt = buildExprStatement(overload);
  fnbody->append_statement(ovl_stmt);

  // returning void... nothing to do.
}

void ArithCheck::handleNormalAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* pos) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // Difference between AddressOf and create_struct -- AddressOf provides the pointer and the size,
  // but never the address to be stored in the struct. create_struct provides the ptr and addr.
  // AddressOf provides the ptr and size fields, not the addr field.
  // AddressOf(&stack_var, sizeof(stackvar))
  // the fncall: AddressOf(input1_ptr, input2)
  // str output;
  // output.ptr = reinterpret_cast<output.ptr type>(input1_ptr);
  // output.addr = &output.ptr;
  // create_entry(output.addr, output.ptr, input2);
  // return output;


  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // get the first arg
  SgInitializedName* arg1 = *iter;

  // create local variable of return type: str output;
  SgVariableDeclaration* output_var = createLocalVariable("output", retType, NULL, fnbody);
  fnbody->append_statement(output_var);

  // output.ptr = reinterpret_cast<output.ptr type>(input1_ptr);
  SgType* ret_ptr_type = findInUnderlyingType(retType);
//  SgExpression* casted_input = buildCastExp(buildVarRefExp(arg1), ret_ptr_type, SgCastExp::e_reinterpret_cast);
  SgExpression* casted_input = buildCastExp(buildVarRefExp(arg1), ret_ptr_type, CAST_TYPE);
  SgExprStatement* ptr_assign = createDotAssign(getNameForDecl(output_var), "ptr", casted_input);
  fnbody->append_statement(ptr_assign);

  // output.addr = &output.ptr
  SgExpression* ptr_addr = createAddressOfOpFor(createDotExpFor(getNameForDecl(output_var), "ptr"));
//  SgExpression* casted_ptr_addr = buildCastExp(ptr_addr, getAddrType(), SgCastExp::e_reinterpret_cast);
  SgExpression* casted_ptr_addr = buildCastExp(ptr_addr, getAddrType(), CAST_TYPE);
  SgExprStatement* addr_assign = createDotAssign(getNameForDecl(output_var), "addr", casted_ptr_addr);
  fnbody->append_statement(addr_assign);

  // create_entry(output.addr, output.ptr, input2)
  SgExpression* ce_arg1 = createDotExpFor(getNameForDecl(output_var), "addr");
  // need to cast the ptr to addr type...
//  SgExpression* ce_arg2 = buildCastExp(createDotExpFor(getNameForDecl(output_var), "ptr"), getAddrType(), SgCastExp::e_reinterpret_cast);
  SgExpression* ce_arg2 = buildCastExp(createDotExpFor(getNameForDecl(output_var), "ptr"), getAddrType(), CAST_TYPE);
  // Get second arg
  iter++;
  SgInitializedName* arg2 = *iter;
  // cast second arg to size of type
  //SgExpression* ce_arg3 = buildCastExp(buildVarRefExp(arg2), getSizeOfType(), SgCastExp::e_reinterpret_cast);
  SgExpression* ce_arg3 = buildCastExp(buildVarRefExp(arg2), getSizeOfType(), SgCastExp::e_C_style_cast);

  // the create_entry function
  // Working one
  #if 0
  SgExpression* ce_fn = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, findInsertLocation(fnbody));
  #else
  //SgExpression* ce_fn = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, pos);
  SgExpression* ce_fn = build3ArgOverloadFn("create_entry", ce_arg1, ce_arg2, ce_arg3, SgTypeVoid::createType(), scope, fndecl);
  #endif
  SgExprStatement* ce_stmt = buildExprStatement(ce_fn);
  fnbody->append_statement(ce_stmt);

  // return output;
  appendReturnStmt(buildVarRefExp(getNameForDecl(output_var)), fnbody);

}

void ArithCheck::handleAddressOfDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                SgScopeStatement* scope, SgProject* project, SgNode* pos) {

  // Here, we need to figure out if this function call is catching the address of a struct
  // of a normal var... for that, the second arg will be of struct type, in the former....

  SgExpressionPtrList& exprs = param_list->get_expressions();
  SgExpressionPtrList::iterator iter = exprs.begin();
  ++iter;

  SgExpression* second_arg = *iter;


  if(isValidStructType(second_arg->get_type())) {
    handleStructAddressOfDef(transf_name, retType, param_list, scope, project, pos);
  }
  else {
    handleNormalAddressOfDef(transf_name, retType, param_list, scope, project, pos);
  }
}


void ArithCheck::handlePlusMinusAssignDef(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                  SgScopeStatement* scope, SgProject* project, SgNode* pos, bool minus) {

  SgFunctionDeclaration* fndecl = createDefiningDecl(transf_name, retType, param_list, scope, project, pos);
  // This function does a plus/minus assign on a struct.
  // fncall: PlusAssign(*input1_str, input2)
  // plusassign body: input1_str->ptr += input2;
  // minusassign body: input1_str->ptr -= input2;
  // return *input1_str

  // Get the function definition...
  SgFunctionDefinition* fndef = getFuncDef(fndecl);
  SgBasicBlock* fnbody = getFuncBody(fndef);
  SgStatementPtrList& stmts = fnbody->get_statements();

  // the args
  SgInitializedNamePtrList& args = fndecl->get_args();
  // assign has two args. first is of pointer type
  ROSE_ASSERT(args.size() == 2);
  SgInitializedNamePtrList::iterator iter = args.begin();

  // Get the first arg
  SgInitializedName* arg1 = *iter;
  // first arg is pointer type -- pointer to struct
  ROSE_ASSERT(isSgPointerType(arg1->get_type()));

  // input1_str->ptr
  SgExpression* ptr_exp = createArrowExpFor(arg1, "ptr");

  // Get the second arg
  iter++;
  SgInitializedName* arg2 = *iter;

  SgExpression* assign;
  if(minus) {
    assign = buildMinusAssignOp(ptr_exp, buildVarRefExp(arg2));
  }
  else {
    assign = buildPlusAssignOp(ptr_exp, buildVarRefExp(arg2));
  }

  SgExprStatement* assign_stmt = buildExprStatement(assign);
  fnbody->append_statement(assign_stmt);

  // return *input1_str
  appendReturnStmt(buildPointerDerefExp(buildVarRefExp(arg1)), fnbody);

}

void ArithCheck::createFunctionFrom(SgName s_name, SgType* retType, SgExprListExp* param_list,
        SgScopeStatement* scope, SgProject* project, SgName transf_name, SgNode* pos) {

  // create nondefining decl for all these functions before the first statement...
  createNondefiningDecl(transf_name, retType, param_list, scope, project, pos, s_name);

  if(compareNames(s_name, "create_struct")) {
    handleCreateStructDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "create_struct_from_addr")) {
    handleCreateStructFromAddrDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "create_entry")) {
    // create_entry is handled in the library. This is due to 2 reasons:
    // 1. The implementation of the metadata structure is not linked to the implementation
    // here. This will allow us to plug in different implementations without changing
    // the instrumentation generated from here.
    // 2. create_entry can get complicated, in terms of the statements used within,
    // and creating a definition here would be difficult, and possibly buggy.

    // In light of these reasons, we create a nondefiningdecl...
    printf("create_entry -- creating nondefining decl\n");
    //createNondefiningDecl(transf_name, retType, param_list, scope, project, node);
    //handleCreateEntryDef(transf_name, retType, param_list, scope, project);
  }
  else if(compareNames(s_name, "check_entry")) {
    printf("check_entry -- creating nondefining decl\n");
  }
  else if(compareNames(s_name, "malloc_overload")) {
    // Since these functions are
    // 1. tied to the metadata structure (possibly), and
    // 2. only one version needs to be implemented,
    // we create a nondefining decl..
    //handleMallocOverloadDef(transf_name, retType, param_list, scope, project);
    printf("malloc_overload -- creating nondefining decl -- already done above\n");
    //createNondefiningDecl(transf_name, retType, param_list, scope, project);

  }
  else if(compareNames(s_name, "realloc_overload")) {
    // see explanation for malloc_overload above
    printf("realloc_overload -- creating nondefining decl -- already done above\n");
  }
  else if(compareNames(s_name, "free_overload")) {
    // see explanation for malloc_overload above
    //handleFreeOverloadDef(transf_name, retType, param_list, scope, project);
    printf("free_overload -- creating nondefining decl -- already done above\n");
    //createNondefiningDecl(transf_name, retType, param_list, scope, project);

  }
  else if(compareNames(s_name, "AddressOf")) {
    handleAddressOfDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "Deref")) {
    handleDerefDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "PntrArrRef")) {
    handlePntrArrRefDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "Assign")) {
    handleAssignDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "Increment")) {
    handleIncrDecrDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "Decrement")) {
    handleIncrDecrDef(transf_name, retType, param_list, scope, project, pos, true);
  }
  else if(isCondString(s_name)) {
    handleConditionalDefs(transf_name, retType, param_list, scope, project, s_name, pos);
  }
  else if(compareNames(s_name, "Add")) {
    handleAddSubDefs(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "Sub")) {
    handleAddSubDefs(transf_name, retType, param_list, scope, project, pos, true);
  }
  else if(compareNames(s_name, "Cast")) {
    handleCastDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "PlusAssign")) {
    handlePlusMinusAssignDef(transf_name, retType, param_list, scope, project, pos);
  }
  else if(compareNames(s_name, "MinusAssign")) {
    handlePlusMinusAssignDef(transf_name, retType, param_list, scope, project, pos, true);
  }
  else {
    // This should become a ROSE_ASSERT(0) once all fns are defined properly
    printf("Can't create def. transf_name: %s\n", transf_name.getString().c_str());
    // already created nondefining decl above..
    //createNondefiningDecl(transf_name, retType, param_list, scope, project);
    ROSE_ASSERT(0);
  }

}

void ArithCheck::printAndCheckScopeContents(SgScopeStatement* scope, SgFunctionDeclaration* decl) {

  SgStatementPtrList stmtList;

  if(scope->containsOnlyDeclarations()) {
    SgDeclarationStatementPtrList declList = scope->getDeclarationList();
    SgDeclarationStatementPtrList::iterator i;
    for(i = declList.begin(); i != declList.end(); i++) {
      stmtList.push_back(*i);
    }
  }
  else {
    stmtList = scope->getStatementList();
  }

  printf("Printing Scope Contents - Begin\n");
  SgStatementPtrList::iterator j;
  for(j = stmtList.begin(); j != stmtList.end(); j++) {

    SgStatement* stmt = isSgStatement(*j);
    printNode(stmt);

    SgFunctionDeclaration* fndecl = isSgFunctionDeclaration(stmt);
    if(fndecl) {
      if(isSameFunction(fndecl, decl)) {
        printf("----------------------------<<<<<< Current Func Decl\n");
      }
    }
  }
  printf("Printing Scope Contents - End\n");

}

bool ArithCheck::isExternal(SgName s_name) {

  if(compareNames(SgName("create_entry"), s_name)) {
    return true;
  }
  else if(compareNames(SgName("malloc_overload"), s_name)) {
    return true;
  }
  else if(compareNames(SgName("free_overload"), s_name)) {
    return true;
  }
  else if(compareNames(SgName("realloc_overload"), s_name)) {
    return true;
  }
  else if(compareNames(SgName("check_entry"), s_name)) {
    return true;
  }

  return false;
}

void ArithCheck::createNondefiningDecl(SgName transf_name, SgType* retType, SgExprListExp* param_list,
                  SgScopeStatement* scope, SgProject* project, SgNode* pos, SgName s_name) {

  printf("Creating Nondefining forward decl\n");

  #if 0
  SgGlobal* globalScope = getFirstGlobalScope(project);
  pushScopeStack(globalScope);
  #endif

  SgFunctionParameterList* new_param_list = buildFunctionParameterList();

  SgExpressionPtrList& exprs = param_list->get_expressions();

  printf("creating param_list\n");

  for(SgExpressionPtrList::iterator iter = exprs.begin(); iter != exprs.end();
                              iter++) {

    SgExpression* ce = *iter;
    printf("ce: %s = %s\n", isSgNode(ce)->sage_class_name(), isSgNode(ce)->unparseToString().c_str());
    SgInitializedName* new_name = buildInitializedName(SgName(""),ce->get_type(), NULL);
    printf("created new_name\n");
    // FIXME: Scope should be fndecl, not globalScope.
    // Removed since we don't want the new_name to be in the globalScope
    //new_name->set_scope(globalScope);
    new_param_list->append_arg(new_name);
    new_name->set_parent(new_param_list);
  }

  printf("creating fwddecl\n");
  #if 0
  SgFunctionDeclaration* fwddecl = buildNondefiningFunctionDeclaration(s_name,
                  retType,
                  new_param_list);
  #endif
  // Working one
  #if 0
  SgFunctionDeclaration* fwddecl = buildNondefiningFunctionDeclaration(transf_name,
                  retType,
                  new_param_list);
  printAndCheckScopeContents(globalScope, fwddecl);
  #else
  SgFunctionDeclaration* fwddecl = buildNondefiningFunctionDeclaration(transf_name,
                  retType,
                  new_param_list,
                  isSgStatement(pos)->get_scope());
  printAndCheckScopeContents(isSgStatement(pos)->get_scope(), fwddecl);
  #endif


  // Instead of simply placing the defdecl at the top of the file... we'll
  // place it as close to the use as possible. This should ensure that all the
  // necessary data structures used, are declared.
  //prependStatement(isSgStatement(fwddecl));
  //SgStatement* loc = findInsertLocationFromParams(param_list);
  //insertStatementBefore(loc, fwddecl);
  //insertStatementBefore(scope, fwddecl);

  // Working one
  //insertStmtBeforeFirst(fwddecl, globalScope);
//  insertStatementBefore(findInsertLocation(node), fwddecl);

//  insertStatementBefore(getInsertLocation(node, scope), fwddecl);
  insertStatementBefore(isSgStatement(pos), fwddecl);
  #if 0
  if(isSgVarRefExp(node)) {
    // For var ref exps, the scope is based on their declaration
    // However, we don't want that here. We would like to know
    // where the statement using the var ref lies, and find
    // the insert location from that.
    insertStatementBefore(findInsertLocation(node), fwddecl);
  }
  else {
    insertStatementBefore(findInsertLocation(scope), fwddecl);
  }
  #endif



  #ifdef MAKE_DECLS_STATIC
  // isExternal checks if the definition of the function is in
  // the library --
  if(!isExternal(s_name)) {
    makeDeclStatic(isSgDeclarationStatement(fwddecl));
  }
  #endif

  #if 0
  popScopeStack();
  #endif
}

std::string ArithCheck::getTransfName(SgName s_name, SgType* retType, SgExprListExp* param_list) {

  std::string transf_String = getNameForType(retType) + "_Ret_" + s_name.getString();

  SgExpressionPtrList& exprs = param_list->get_expressions();
  for(SgExpressionPtrList::iterator iter = exprs.begin(); iter != exprs.end();
                              iter++) {
    SgExpression* ce = *iter;
    transf_String = transf_String + "_" + getNameForType(ce->get_type()) + "_Arg";
  }

  return transf_String;
}

SgName ArithCheck::createForwardDeclIfNotAvailable(SgName s_name, SgType* retType,
            SgExprListExp* parameter_list,
            SgScopeStatement* scope, SgProject* project,
            SgNode* pos) {

  // Create the transf_name
  SgName transf_name(getTransfName(s_name, retType, parameter_list));

  printf("createForwardDecl\n");
  #if 0
  if(checkIfDeclExists(s_name, retType, parameter_list)) {
    return;
  }
  #else
  if(checkIfDeclExists(transf_name, retType, parameter_list)) {
    return transf_name;
  }
  #endif

  #ifdef GENERATE_DEFS
  createFunctionFrom(s_name, retType, parameter_list, scope, project, transf_name, pos);
  #else
  createNondefiningDecl(transf_name, retType, parameter_list, scope, project, pos, s_name);
  #if 0
  printf("Creating new forward decl\n");

  SgGlobal* globalScope = getFirstGlobalScope(project);
  pushScopeStack(globalScope);

  SgFunctionParameterList* new_param_list = buildFunctionParameterList();

  SgExpressionPtrList& exprs = parameter_list->get_expressions();

  printf("creating param_list\n");

  for(SgExpressionPtrList::iterator iter = exprs.begin(); iter != exprs.end();
                              iter++) {

    SgExpression* ce = *iter;
    printf("ce: %s = %s\n", isSgNode(ce)->sage_class_name(), isSgNode(ce)->unparseToString().c_str());
    SgInitializedName* new_name = buildInitializedName(SgName(""),ce->get_type(), NULL);
    printf("created new_name\n");
    // FIXME: Scope should be fndecl, not globalScope.
    new_name->set_scope(globalScope);
    new_param_list->append_arg(new_name);
    new_name->set_parent(new_param_list);
  }

  printf("creating fwddecl\n");
  #if 0
  SgFunctionDeclaration* fwddecl = buildNondefiningFunctionDeclaration(s_name,
                                    retType,
                                    new_param_list);
  #endif
  SgFunctionDeclaration* fwddecl = buildNondefiningFunctionDeclaration(transf_name,
                                    retType,
                                    new_param_list);


  prependStatement(isSgStatement(fwddecl));
  popScopeStack();
  #endif
  #endif

  printf("createForwardDecl - Done\n");

  return transf_name;
}

bool isFreeOrFlush(SgFunctionCallExp* fncall) {

  if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "free") == 0) {
    return true;
  }
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "fflush") == 0) {
    return true;
  }
  return false;
}

void ArithCheck::checkType(SgType* type) {

  printf("checkType\n");
  printf("type: \n");
  printNode(type);

  SgClassType *cls_type = isSgClassType (type);
  ROSE_ASSERT(cls_type != NULL);

  printf("cls_type: \n");
  printNode(cls_type);

    SgClassDeclaration *cls_decl = isSgClassDeclaration (cls_type->get_declaration());
    ROSE_ASSERT(cls_decl);

  printf("cls_decl:\n");
  printNode(cls_decl);
    ROSE_ASSERT(cls_decl->get_firstNondefiningDeclaration()!= NULL);

  printf("First Defining decl: \n");
  printNode(isSgClassDeclaration(cls_decl->get_firstNondefiningDeclaration()));

    ROSE_ASSERT(isSgClassDeclaration(cls_decl->get_firstNondefiningDeclaration()) == cls_decl);
}

std::string ArithCheck::generateNameForType(SgType* ptr_type) {


  return ptr_type->get_mangled() + "_Type";

  #if 0
  SgType* baseType = skip_RefsPointersAndTypedefs(ptr_type);

  std::string name;

  if(isSgClassType(baseType)) {
    name = "Class_";
  }
  else {
    switch(baseType->variantT()) {
    case V_SgTypeInt: name = "Int_"; break;
    case V_SgTypeLong: name = "Long_"; break;
    case V_SgTypeLongLong: name = "TLongLong_"; break;
    case V_SgTypeFloat: name = "Float_"; break;
    case V_SgTypeDouble: name = "Double_"; break;
    case V_SgTypeChar: name = "Char_"; break;
    case V_SgTypeUnsignedInt: name = "UnsignedInt_"; break;
    case V_SgTypeVoid: name = "Void_"; break;
    default: printf("Unknown Type: %u\n", baseType->variantT()), ROSE_ASSERT(0);
    }

  }

  name = name + "Type" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++);

  return name;
  #endif
}

SgFunctionDeclaration* ArithCheck::getFuncDeclForName(SgInitializedName* name) {
  SgNode* grand_parent = name->get_parent()->get_parent();
  ROSE_ASSERT(isSgFunctionDeclaration(grand_parent));
  return isSgFunctionDeclaration(grand_parent);
}

bool ArithCheck::ScopeIsGlobal(SgNode* node) {

  SgStatement* stmt = isSgStatement(node);
  SgScopeStatement* scope = stmt->get_scope();
  return isSgGlobal(scope);
}

SgStatement* ArithCheck::findInsertLocation(SgNode* node) {


  if(isSgInitializedName(node) && isSgFunctionParameterList(node->get_parent())) {
    SgFunctionDeclaration* funcdecl = getFuncDeclForName(isSgInitializedName(node));
    return findInsertLocation(funcdecl);
  }

  // Motivation for this: If the current node's parent is SgGlobal,
  // the decl/def can go before this -- since this is likely to be
  // a class decl/fn decl/temp inst decl
  #ifdef HACKY_QUICK_INSERT_LOC
  if(isSgMemberFunctionDeclaration(node)) {
    SgStatement* class_def = isSgMemberFunctionDeclaration(node)->get_scope();
    ROSE_ASSERT(isSgClassDefinition(class_def));
    return isSgClassDeclaration(isSgClassDefinition(class_def)->get_declaration());
  }
  #if 0
  if(isSgGlobal(node->get_parent())) {
    ROSE_ASSERT(isSgStatement(node));
    return isSgStatement(node);
  }
  #endif
  #endif

  if(isSgClassDeclaration(node) && ScopeIsGlobal(node)) {
    return isSgStatement(node);
  }
  else if(isSgFunctionDeclaration(node)) {
    if(isSgMemberFunctionDeclaration(node)) {
      if(node->get_parent() == NULL) {
        printf("Can't find suitable parent for... \n");
        printNode(node);
        ROSE_ASSERT(0);
      }
      else {
        return findInsertLocation(node->get_parent());
      }
    }
    else if(ScopeIsGlobal(node) || isSgNamespaceDefinitionStatement(isSgFunctionDeclaration(node)->get_scope())) {
      return isSgStatement(node);
    }
    else {
      return findInsertLocation(node->get_parent());
    }
  }
  else {
    if(node->get_parent() == NULL) {
      printf("Can't find suitable parent for... \n");
      printNode(node);
      ROSE_ASSERT(0);
    }
    else {
      return findInsertLocation(node->get_parent());
    }
  }
}


void ArithCheck::insertStmtBeforeFirst(SgStatement* stmt, SgScopeStatement* scope) {

  SgStatement* first_stmt = getFirstStatementInScope(scope);
  insertStatementBefore(first_stmt, stmt);
}

void ArithCheck::insertStmtAfterLast(SgStatement* stmt, SgScopeStatement* scope) {

  SgStatement* last_stmt = getLastStatementInScope(scope);
  insertStatementAfter(last_stmt, stmt);
}

SgStatement* ArithCheck::getLastStatementInScope(SgScopeStatement* scope) {

  SgDeclarationStatementPtrList& decls = scope->getDeclarationList();
  SgDeclarationStatementPtrList::iterator iter = decls.begin();
  iter += (decls.size() - 1);
  return *(iter);

}

SgStatement* ArithCheck::getFirstStatementInScope(SgScopeStatement* scope) {

  static SgStatement* stmt = *(scope->getDeclarationList().begin());

  #if 1
  printf("First stmt:\n");
  printNode(stmt);
  #endif

  return stmt;

  #if 0
  #if 0
  return scope->firstStatement();
  #endif
  SgDeclarationStatementPtrList& decls = scope->getDeclarationList();
  return *(decls.begin());
  #endif

}

void ArithCheck::insertForwardDecl(SgType* cls_type, SgScopeStatement* scope) {

  ROSE_ASSERT(isSgClassType(cls_type));

  // now, insert a struct declaration.
  //SgClassDeclaration* str_decl = SageBuilder::buildStructDeclaration(isSgClassType(cls_type)->get_name(), scope);
  SgClassDeclaration* str_decl = SageBuilder::buildNondefiningClassDeclaration(isSgClassType(cls_type)->get_name(), scope);
  // Builds a class definition, not a forward decl.
  //SgClassDeclaration* str_decl = SageBuilder::buildClassDeclaration(isSgClassType(cls_type)->get_name(), scope);

  insertStmtBeforeFirst(str_decl, scope);

}

SgStatement* ArithCheck::getInsertLocation(SgNode* node, SgScopeStatement* scope) {


  if(isSgVarRefExp(node)) {
    // For var ref exps, the scope is based on their declaration
    // However, we don't want that here. We would like to know
    // where the statement using the var ref lies, and find
    // the insert location from that.
    return findInsertLocation(node);
  }
  else {
    return findInsertLocation(scope);
  }
}

SgType* ArithCheck::createStruct(SgType* ptr_type, SgNode* pos) {

  printf("creating struct of type: ");
  printNode(ptr_type);

  SgName str_name(generateNameForType(ptr_type));

  // Input type has to be pointer type -- obviously
  ROSE_ASSERT(isSgPointerType(ptr_type));
  // Addresses can be stored in unsigned long long
  //SgType* addr_type = SgTypeUnsignedLongLong::createType();
  SgType* addr_type = getAddrType();

  SgFilePtrList file_list = ArithCheck::ACProject->get_fileList();
  SgFilePtrList::iterator iter;


  SgType* str_type;

  int index = 0;

  for(iter = file_list.begin(); iter != file_list.end(); iter++) {
    printf("iter: %u\n", index++);

    // Getting current file and scope for it
    SgSourceFile* cur_file = isSgSourceFile(*iter);
    SgGlobal* global_scope = cur_file->get_globalScope();

    // Get the top node for this file. Struct will be inserted at this scope
    // FIXME: May want to do this at the "project" level instead, but not sure yet.
    // The problem with this approach is that there might re-definitions, since each file
    // has its version of struct
    SgNode* fileTop = isSgNode(cur_file);

    #ifdef FORWARD_CLASS_DECLS
    if(isSgClassType(skip_RefsPointersAndTypedefs(ptr_type))) {
      insertForwardDecl(skip_RefsPointersAndTypedefs(ptr_type), global_scope);
    }
    #endif


    SgClassDeclaration* str_decl = SageBuilder::buildStructDeclaration(str_name, global_scope);
//    SgClassDeclaration* str_decl = SageBuilder::buildDefiningClassDeclaration(str_name, global_scope);
    str_type = str_decl->get_type();

    // To get this structure into ROSE's symbol table, and access its members, the struct/class
    // needs to be defined here.
    SgClassDefinition* str_def = buildClassDefinition(str_decl);

    #if 0
    str_decl->set_definition(str_def);

    str_def->set_parent(str_decl);
    #endif

    ROSE_ASSERT(str_decl->get_definition() != NULL);

    // Adding the ptr and addr variables
    // struct str_name {
    //    (ptr_type) ptr;
    //    unsigned long long addr;
    // };


    //SgVariableDeclaration* ptr_var = SageBuilder::buildVariableDeclaration("ptr", ptr_type, NULL, global_scope);
    SgVariableDeclaration* ptr_var = SageBuilder::buildVariableDeclaration("ptr", ptr_type, NULL, str_def);
//    ArithCheck::VarDeclForName.insert(NameVarMap_t::value_type("ptr", ptr_var));
    str_def->append_member(ptr_var);
    //SgVariableDeclaration* addr_var = SageBuilder::buildVariableDeclaration("addr", addr_type, NULL, global_scope);
    SgVariableDeclaration* addr_var = SageBuilder::buildVariableDeclaration("addr", addr_type, NULL, str_def);
//    ArithCheck::VarDeclForName.insert(NameVarMap_t::value_type("addr", addr_var));
    str_def->append_member(addr_var);

    // FIXME: This could be done outside of the loop as well.
    // In this mapping into respective structures so that we can look it up later
    #if 0
    SgType* str_type_copy = isSgType(deepCopyNode(str_type));
    SgType* ptr_type_copy = isSgType(deepCopyNode(ptr_type));

    SgType* str_type_copy2 = isSgType(deepCopyNode(str_type));
    SgType* ptr_type_copy2 = isSgType(deepCopyNode(ptr_type));
    #endif

    ArithCheck::UnderlyingType.insert(TypeMap_t::value_type(str_type, ptr_type));
//    ArithCheck::UnderlyingType.insert(TypeMap_t::value_type(str_type_copy, ptr_type_copy));

    ArithCheck::RelevantStructType.insert(TypeMap_t::value_type(ptr_type, str_type));
//    ArithCheck::RelevantStructType.insert(TypeMap_t::value_type(ptr_type_copy2, str_type_copy2));

    // insert the struct declaration in the current scope
    // FIXME: This tries to look for the globalScope in the file where the node is called
    // from... so it might not match the global_scope for the "cur_file" when multiple
    // files are involved

    //isSgScopeStatement(global_scope)->insertStatementInScope(str_decl, true);
    #if 0
    SgStatement* insert_loc = findInsertLocation(node);
    insertStatementBefore(insert_loc, str_decl);
    #endif

    // Working one
    //insertStmtBeforeFirst(str_decl, global_scope);
    //insertStatementBefore(findInsertLocation(node), str_decl);
    //insertStatementBefore(getInsertLocation(node, global_scope), str_decl);
    insertStatementBefore(isSgStatement(pos), str_decl);

    printNodeExt(str_decl);
    ROSE_ASSERT(str_decl->get_definition() != NULL);
    printNodeExt(str_def);
  }

  checkType(str_type);

  return str_type;
}


//SgVariableDeclaration* ArithCheck::createStructVariableFor(SgVariableDeclaration* var_decl) {
SgVariableDeclaration* ArithCheck::createStructVariableFor(SgVariableDeclaration* var_decl, bool array_var) {


  SgInitializedName* var_name = getNameForDecl(var_decl);

  SgScopeStatement* var_scope = var_name->get_scope();

  std::string str_var_name_String;

  if(strcmp(var_name->get_name().getString().c_str(), "") == 0) {
    str_var_name_String = "";
  }
  else {
    str_var_name_String = var_name->get_name().getString() + "_str";
  }

  printf("str_var_name_String: %s\n", str_var_name_String.c_str());

  #if 0
  SgType* str_ptr_type = SgPointerType::createType(SgTypeVoid::createType());

  // Use the VoidStruct. To find that, use a void pointer.
  TypeMap_t::iterator iter = ArithCheck::RelevantStructType.find(str_ptr_type);

  ROSE_ASSERT(iter != ArithCheck::RelevantStructType.end());

  SgType* str_var_type = iter->second;
  #endif

  #if 0
  SgType* var_type;

  if(array_var) {
    // For arrays.. get the base type, and create a pointer out of it
    // since the array variable is a pointer to the type contained in the
    // array itself. So, for example, a variable which represents an array
    // of pointers, is, in itself, a double pointer.
    SgType* temp_type = var_name->get_type();

    SgArrayType* arr_type = isSgArrayType(temp_type);

    SgType* base_type = arr_type->get_base_type();

    // create the struct type necessary to hold the base pointer
    // for this array
    // Before that, we need to change the base_type to its corresponding
    // pointer type
    var_type = SgPointerType::createType(base_type);
  }
  else {
    // For pointers, simply get the type of init name
    var_type = var_name->get_type();
  }
  #endif



//  SgType* str_var_type = getStructType(getTypeForPntrArr(var_name->get_type()), getScope(var_decl), true);
  SgType* str_var_type = getStructType(getTypeForPntrArr(var_name->get_type()), findInsertLocation(var_decl), true);


  // Lets zero out the initializer for now
  SgVariableDeclaration* str_var_decl = SageBuilder::buildVariableDeclaration(str_var_name_String, str_var_type, NULL, var_scope);

  return str_var_decl;
}

void ArithCheck::handleSymbolTable(SgInitializedName* orig_name, SgInitializedName* str_name, bool addToMap) {

  SgScopeStatement* orig_scope = orig_name->get_scope();

  SgSymbol* orig_symbol = orig_name->get_symbol_from_symbol_table();
  SgVariableSymbol *orig_var_symbol = isSgVariableSymbol(orig_symbol);
  printf("Got scope. Now, symbolTable\n");

  SgSymbol* str_symbol = str_name->get_symbol_from_symbol_table();

  if(str_symbol == NULL) {

    SgVariableSymbol* str_var_symbol = new SgVariableSymbol(str_name);
    isSgNode(str_var_symbol)->set_parent(isSgNode(orig_var_symbol)->get_parent());
    SgSymbolTable* currSymbolTable = isSgSymbolTable(isSgNode(orig_var_symbol)->get_parent());
    currSymbolTable->insert(str_name->get_name(), str_var_symbol);

    // Check that this worked
    str_symbol = str_name->get_symbol_from_symbol_table();
    ROSE_ASSERT(str_symbol);
  }

  if(addToMap) {
    varRemap.insert(VariableSymbolMap_t::value_type(orig_var_symbol, isSgVariableSymbol(str_symbol)));

    ReverseMap.insert(VariableSymbolMap_t::value_type(isSgVariableSymbol(str_symbol), orig_var_symbol));
  }
}



SgInitializedName* getNameForDecl(SgVariableDeclaration* decl) {

  // get info about original declaration
  SgInitializedNamePtrList& name_list = decl->get_variables();
  ROSE_ASSERT(name_list.size() == 1);
  SgInitializedName* name = *(name_list.begin());

  return name;
}


void ArithCheck::handleSymbolTable(SgVariableDeclaration* orig_decl, SgVariableDeclaration* str_decl, bool addToMap) {

  SgInitializedName* orig_name = getNameForDecl(orig_decl);
  SgInitializedName* str_name = getNameForDecl(str_decl);

  handleSymbolTable(orig_name, str_name, addToMap);
}

SgVariableDeclaration* ArithCheck::getCorrectMemberDecl(SgInitializedName* str_var_name, std::string str_elem) {

  // We can't use the str_elem to figure out which variable to use, since all the structures
  // have internal variables by the same name! So, only the last struct's version of that
  // variable will show up.

//  SgDeclarationStatement* str_decl = str_var_name->get_declaration();

  printf("getCorrectMemberDecl\n");

  printf("str_var_name: \n");
  printNode(str_var_name);

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

  printf("cls_type\n");
  printNode(cls_type);

  SgClassDeclaration * cls_decl = isSgClassDeclaration (cls_type->get_declaration());



    ROSE_ASSERT (cls_decl);

  printf("cls_decl\n");
  //printNode(cls_decl);
  printNodeExt(cls_decl);

    ROSE_ASSERT(cls_decl->get_firstNondefiningDeclaration()!= NULL);

  printf("cls_first_nondefining_decl\n");
  printNode(cls_decl->get_firstNondefiningDeclaration());
  printNodeExt(cls_decl->get_firstNondefiningDeclaration());

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

  printf("ptr_decl\n");
  printNode(ptr_decl);

  return ptr_decl;
}

SgArrowExp* ArithCheck::createArrowExpFor(SgInitializedName* str_var_name, std::string str_elem) {

  printf("create Arrow Exp For\n");
  printf("str_var_name: \n");
  printNode(str_var_name);

  SgVariableDeclaration* ptr_decl = getCorrectMemberDecl(str_var_name, str_elem);
  printf("ptr_decl:\n");
  printNode(ptr_decl);

  SgVarRefExp* ptr_ref_exp = SageBuilder::buildVarRefExp(ptr_decl);
  printf("ptr_ref_exp:\n");
  printNode(ptr_ref_exp);

  //SgVarRefExp* str_var_ref_exp = SageBuilder::buildVarRefExp(str_var_decl);
  SgVarRefExp* str_var_ref_exp = SageBuilder::buildVarRefExp(str_var_name, str_var_name->get_scope());

  SgArrowExp* arrow_exp = SageBuilder::buildArrowExp(str_var_ref_exp, ptr_ref_exp);

  printf("ptr_ref_exp: type: %s\n", ptr_ref_exp->get_type()->get_mangled().getString().c_str());

  return arrow_exp;
}


SgDotExp* ArithCheck::createDotExpFor(SgInitializedName* str_var_name, std::string str_elem) {

  #if 0
  // Next, lets use the dot exp to create a reference to the newly created struct var's ptr
  NameVarMap_t::iterator iter2 = ArithCheck::VarDeclForName.find(str_elem);
  ROSE_ASSERT(iter2 != ArithCheck::VarDeclForName.end());

  SgVariableDeclaration* ptr_decl = iter2->second;
  #endif

  printf("create Dot Exp For\n");
  printf("str_var_name: \n");
  printNode(str_var_name);

  SgVariableDeclaration* ptr_decl = getCorrectMemberDecl(str_var_name, str_elem);
  printf("ptr_decl:\n");
  printNode(ptr_decl);

  SgVarRefExp* ptr_ref_exp = SageBuilder::buildVarRefExp(ptr_decl);
  printf("ptr_ref_exp:\n");
  printNode(ptr_ref_exp);

  //SgVarRefExp* str_var_ref_exp = SageBuilder::buildVarRefExp(str_var_decl);
  SgVarRefExp* str_var_ref_exp = SageBuilder::buildVarRefExp(str_var_name, str_var_name->get_scope());

  SgDotExp* dot_exp = SageBuilder::buildDotExp(str_var_ref_exp, ptr_ref_exp);

  printf("ptr_ref_exp: type: %s\n", ptr_ref_exp->get_type()->get_mangled().getString().c_str());

  return dot_exp;
}

SgType* ArithCheck::getVoidPointerType() {
  return SgPointerType::createType(SgTypeVoid::createType());
}

//SgCommaOpExp* ArithCheck::initializeStructWith(SgVariableDeclaration* var_decl, SgVariableDeclaration* str_var_decl) {
//SgCommaOpExp* ArithCheck::initializeStructWith(SgInitializedName* var_name, SgInitializedName* str_var_name) {
SgCommaOpExp* ArithCheck::initializeStructWith(SgInitializedName* var_name, SgInitializedName* str_var_name, bool flip_ptr_init) {
  printf("initializeStructWith -- \n");
  printf("original var -\n");
  printNode(var_name);

  // Lets create a variable reference expression for the pointer var
  SgVarRefExp* var_ref_exp = SageBuilder::buildVarRefExp(var_name, var_name->get_scope());

  // This cast is not required since the type of the ptr in the struct is already the same type
  #if 0
  // Now, for the ptr assignment, cast it to a void pointer (ptr type in the struct)
  SgCastExp* cast_exp = SageBuilder::buildCastExp(var_ref_exp, getVoidPointerType(), SgCastExp::e_reinterpret_cast);
  #endif

  SgDotExp* dot_exp = createDotExpFor(str_var_name, "ptr");

  #if 0
  SgAssignOp* assign_op = SageBuilder::buildAssignOp(dot_exp, cast_exp);
  #endif

  // This assign input_recr = input_str.ptr instead of the default case (else below)
  // where input_str.ptr = input_recr
  SgAssignOp* assign_op;
  if(flip_ptr_init) {
    assign_op = SageBuilder::buildAssignOp(var_ref_exp, dot_exp);
  }
  else {
    assign_op = SageBuilder::buildAssignOp(dot_exp, var_ref_exp);
  }

  printf(" ptr init: %s = %s\n", isSgNode(assign_op)->sage_class_name(),
          isSgNode(assign_op)->unparseToString().c_str());



  // Get variable address
  // Now, address of op on this var ref
  SgAddressOfOp* addr_of_op = createAddressOfOpFor(var_name);


  // This casting is required since the addr in the struct is of unsigned long long type
  // Now, cast it to long long -- this is the type of addr in the VoidStruct
  //SgType* str_addr_type = SgTypeUnsignedLongLong::createType();
  SgType* str_addr_type = getAddrType();

//  SgCastExp* cast_exp2 = SageBuilder::buildCastExp(addr_of_op, str_addr_type, SgCastExp::e_reinterpret_cast);
  SgCastExp* cast_exp2 = SageBuilder::buildCastExp(addr_of_op, str_addr_type, CAST_TYPE);

  SgDotExp* dot_exp2 = createDotExpFor(str_var_name, "addr");

  SgAssignOp* assign_op2 = SageBuilder::buildAssignOp(dot_exp2, cast_exp2);

  printf(" addr init: %s = %s\n", isSgNode(assign_op2)->sage_class_name(),
          isSgNode(assign_op2)->unparseToString().c_str());

  // Create comma exp with both these exprs
  SgCommaOpExp* comma_op = buildCommaOpExp(assign_op, assign_op2);

  return comma_op;

}

SgExpression* ArithCheck::buildOverloadFn(SgName fn_name, SgExpression* lhs, SgExpression* rhs, SgType* retType, SgScopeStatement* scope, SgNode* pos) {

  printf("buildOverloadFn\n");

  // Not updated ... use build3ArgOverloadFn
  #if 0
  SgExprListExp* parameter_list = buildExprListExp(lhs);

  if(rhs != NULL) {
    appendExpression(parameter_list, rhs);
  }

  createForwardDeclIfNotAvailable(fn_name, retType, parameter_list, scope, ArithCheck::ACProject);


  SgExpression* NewFunction = SageBuilder::buildFunctionCallExp(fn_name,
                                                                retType,
                                                                parameter_list,
                                                                scope);

  printf("NewFunction\n");
  printNode(NewFunction);
  printf("NewFunction - type\n");
  printNode(NewFunction->get_type());
  printf("retType - type\n");
  printNode(retType);

  ROSE_ASSERT(NewFunction->get_type() == retType);

  #else
//  SgExpression* NewFunction = build3ArgOverloadFn(fn_name, lhs, rhs, NULL, retType, scope);
  SgExpression* NewFunction = build3ArgOverloadFn(fn_name, lhs, rhs, NULL, retType, scope, pos);
  #endif

  printf("buildOverloadFn - Done\n");
  return NewFunction;
}

SgExpression* ArithCheck::build3ArgOverloadFn(SgName fn_name, SgExpression* arg1, SgExpression* arg2, SgExpression* arg3, SgType* retType, SgScopeStatement* scope, SgNode* pos) {

  printf("build3ArgOverloadFn\n");
  #if 1
  SgExprListExp* parameter_list = buildExprListExp();

  if(arg1 != NULL) {
    appendExpression(parameter_list, arg1);
  }
  #else
  SgExprListExp* parameter_list = buildExprListExp(arg1);
  #endif

  if(arg2 != NULL) {
    appendExpression(parameter_list, arg2);
  }

  if(arg3 != NULL) {
    appendExpression(parameter_list, arg3);
  }

  SgName transf_name = createForwardDeclIfNotAvailable(fn_name, retType, parameter_list, scope, ArithCheck::ACProject, pos);


//  SgExpression* NewFunction = SageBuilder::buildFunctionCallExp(fn_name,
  SgExpression* NewFunction = SageBuilder::buildFunctionCallExp(transf_name,
                                                                retType,
                                                                parameter_list,
                                                                scope);


  printf("NewFunction\n");
  printNode(NewFunction);
  printf("NewFunction: %p\n", NewFunction);
  printf("NewFunction - type\n");
  printNode(NewFunction->get_type());
  printf("retType - type\n");
  printNode(retType);

  ROSE_ASSERT(NewFunction->get_type() == retType);

  printf("build3ArgOverloadFn - Done\n");
  return NewFunction;
}


SgExpression* ArithCheck::buildAssignOverload(SgExpression* lhs, SgExpression* rhs, SgScopeStatement* scope, SgNode* pos) {

  SgType* retType = lhs->get_type();

  // Need to use the address of lhs - since we are updating it during the assign op.
  ROSE_ASSERT(isSgVarRefExp(lhs));
  SgExpression* lhs_addr = createAddressOfOpFor(isSgVarRefExp(lhs));

//  SgExpression* overload = buildOverloadFn("Assign", lhs_addr, rhs, retType, scope);
  SgExpression* overload = buildOverloadFn("Assign", lhs_addr, rhs, retType, scope, pos);

  return overload;
}

//SgExpression* ArithCheck::updatePointer(SgVariableDeclaration* var_decl, SgVariableDeclaration* str_var_decl) {
SgExpression* ArithCheck::updatePointer(SgInitializedName* var_name, SgInitializedName* str_var_name) {

  // LHS - original pointer ref
  SgVarRefExp* var_ref = buildVarRefExp(var_name, var_name->get_scope());

  // RHS - str.ptr ref
  SgDotExp* dot_exp = createDotExpFor(str_var_name, "ptr");


  #if 0
  // We don't need to cast here since the type of the variable from the dot_exp
  // is correct
  // Cast it to the right type
  SgType* var_type = var_name->get_type();
  SgExpression* cast_exp = buildCastExp(dot_exp, var_type, SgCastExp::e_C_style_cast);
  #endif

  // Do the assignment
  //SgAssignOp* update_op = SageBuilder::buildAssignOp(var_ref, cast_exp);
  SgAssignOp* update_op = SageBuilder::buildAssignOp(var_ref, dot_exp);

  return update_op;

}

SgCommaOpExp* ArithCheck::copyVarInitAndUpdate(SgVariableDeclaration* var_decl,
              SgVariableDeclaration* str_var_decl) {

  printf("copy var init and update\n");
  SgInitializedName* var_name = getNameForDecl(var_decl);
  printf("Original var decl\n");
  printNode(var_decl);
  printf("Struct var decl\n");
  printNode(str_var_decl);

  SgInitializer* var_initializer = var_name->get_initializer();

  if(var_initializer == NULL) {
    printf("variable not initialized at decl. Nothing to do\n");
    return NULL;
  }

  printf("initializer: %s = %s\n", isSgNode(var_initializer)->sage_class_name(),
                        isSgNode(var_initializer)->unparseToString().c_str());

  // Handle only assign initializers for now.
  if(!isSgAssignInitializer(var_initializer)) {
    printf("Only handling assign initializers for now.\n");
    printf("This is not assign initializer\n");
    ROSE_ASSERT(0);
  }

  SgAssignInitializer* assign_init = isSgAssignInitializer(var_initializer);

  // All the variable references would be replaced by their struct counterparts.
  // At initialization, the lhs will still be struct since its not a variable reference
  // exp, but instead, its an intialized name.
  // So, we copy over the operand of the initializer, which should be of struct type
  // and use assign_overload to do the struct to struct copy
  // Afterwards, we update the original pointer, using the ptr field in the struct var

  SgExpression* init_exp = assign_init->get_operand();
  SgExpression* init_exp_copy = copyExpression(init_exp);

  SgExpression* var_ref_exp = buildVarRefExp(str_var_decl);

  SgExpression* init_op = buildAssignOverload(var_ref_exp, init_exp_copy, var_name->get_scope(), findInsertLocation(var_decl));

  printf("init_op\n");
  printNode(init_op);

  SageInterface::deepDelete(isSgNode(var_initializer));
  var_name->set_initializer(NULL);

  // Now, to update the original pointer
  SgExpression* update_op = updatePointer(getNameForDecl(var_decl), getNameForDecl(str_var_decl));
  printf("update_op\n");
  printNode(update_op);

  SgCommaOpExp* comma_op = SageBuilder::buildCommaOpExp(init_op, update_op);
  printf("comma_op\n");
  printNode(comma_op);

  return comma_op;

}

bool ArithCheck::NoInitializer(SgVariableDeclaration* var_decl) {

  SgInitializedName* var_name = getNameForDecl(var_decl);
  if(var_name->get_initializer() == NULL) {
    return true;
  }
  else {
    return false;
  }
}

// Using multiple types of structs seems to be the best way to handle the
// the type and arithmetic problem.

void ArithCheck::handlePointerVars(SgVariableDeclaration* var_decl) {

  SgProject* project = ArithCheck::ACProject;

  // 1. Create a tracking struct (VoidStruct) variable to track this pointer
  // 2. Initialize that variable with the location that this pointer points to,
  // and the address of this pointer
  // 3. If this pointer variable is initialized, use the struct version instead
  // in the initialization
  // and create a copy from the struct var.ptr to this pointer after the
  // initialization

  printf("handlePointerVars\n");

  // Checking for initializer:
  if(NoInitializer(var_decl)) {
    // If the variable is not initialized, nothing to do.
    return;
  }

  SgVariableDeclaration* str_var_decl = createStructVariableFor(var_decl);

  SageInterface::insertStatementAfter(var_decl, str_var_decl);

  // Don't add the decls to varRemap/ReverseMap -- we want to recreate the variables
  // on each use.
  handleSymbolTable(var_decl, str_var_decl, false);

  printf(" Created variable decl: %s = %s\n", isSgNode(str_var_decl)->sage_class_name(),
                        isSgNode(str_var_decl)->unparseToString().c_str());

  // Initialize created str variable with original var info
  SgCommaOpExp* init_expr = initializeStructWith(getNameForDecl(var_decl), getNameForDecl(str_var_decl));

  // For testing purposes. Typically, this should #if 1.
  #if 1
  // Now, we do the initialization of the original var_decl, to the
  // str var
  SgCommaOpExp* copy_update_expr = copyVarInitAndUpdate(var_decl, str_var_decl);
  #else
  SgCommaOpExp* copy_update_expr = NULL;
  #endif

  // FIXME: Shouldn't the return value here, be the var ref to the struct variable?
  // So, shouldn't it be like... str = rhs_exp, orig_ptr = str.ptr, str
  // now, the last "str" is missing. so the return type of the comma op is
  // pointer type...

  printf("creating comma op by combining init_expr and copy_update\n");

  SgExprStatement* init_update_stmt;

  if(copy_update_expr != NULL) {
    // Combine the comma ops. Create a statement. Insert it after current one.
    SgCommaOpExp* init_update_expr = buildCommaOpExp(init_expr, copy_update_expr);

    init_update_stmt = buildExprStatement(isSgExpression(init_update_expr));
  }
  else {
    init_update_stmt = buildExprStatement(isSgExpression(init_expr));
  }

  #if 1
  SageInterface::insertStatementAfter(str_var_decl, init_update_stmt);

  printf(" Created init_update_stmt: %s = %s\n", isSgNode(init_update_stmt)->sage_class_name(),
                        isSgNode(init_update_stmt)->unparseToString().c_str());
  #endif

}

#if 0
SgVariableDeclaration* ArithCheck::createStructVariableForArrayVar(SgVariableDeclaration* var_decl) {

  SgInitializedName* var_name = getNameForDecl(var_decl);
  // Check that we only ever get array variables here.
  ROSE_ASSERT(isSgArrayType(var_name->get_type()));

  SgType* var_type = var_name->get_type();

  SgArrayType* arr_type = isSgArrayType(var_type);

  SgType* base_type = arr_type->get_base_type();

  // create the struct type necessary to hold the base pointer
  // for this array
  // Before that, we need to change the base_type to its corresponding
  // pointer type
  SgType* base_ptr_type = SgPointerType::createType(base_type);
  SgType* str_var_type = getStructType(base_ptr_type, var_decl, true);

  SgScopeStatement* str_var_scope = var_name->get_scope();

  std::string str_var_name_String;

  if(strcmp(var_name->get_name().getString().c_str(), "") == 0) {
    str_var_name_String = "";
  }
  else {
    str_var_name_String = var_name->get_name().getString() + "_str";
  }

  printf("str_var_name_String: %s\n", str_var_name_String.c_str());

  // Lets zero out the initializer for now
  SgVariableDeclaration* str_var_decl = SageBuilder::buildVariableDeclaration(str_var_name_String, str_var_type, NULL, str_var_scope);

  return str_var_decl;
}
#endif



SgVariableDeclaration* ArithCheck::pushIndexToLocalVar(SgName arr_name, SgExpression* index_exp, SgScopeStatement* scope) {
  // Move the index_exp into a single statement by itself, and assign it to a local
  // variable.

//  SgType* arr_type = isSgArrayType(array_name->get_type());

  SgType* index_var_type = getArrayIndexType();
  SgExpression* index_var_init = copyExpression(index_exp);
//  SgName index_var_sgname(array_name->get_name().getString() + "_index" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));
  SgName index_var_sgname(arr_name.getString() + "_index" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));

  SgVariableDeclaration* index_var_decl = createLocalVariable(index_var_sgname, index_var_type, index_var_init, scope);

  return index_var_decl;
}

SgType* ArithCheck::getArrayIndexType() {
  return SgTypeUnsignedLong::createType();
}

SgExpression* ArithCheck::getArrayIndex(SgInitializedName* name) {

  SgType* type = name->get_type();
  SgArrayType* array_type = isSgArrayType(type);
  return array_type->get_index();

}

void ArithCheck::handleArrayVars(SgVariableDeclaration* var_decl) {

  SgProject* project = ArithCheck::ACProject;

  // This is an array declaration ... which means that
  // the bounds are set by the declaration itself. To find the
  // the bounds, we will use the fields in the ArrayType

  SgInitializedName* var_name = getNameForDecl(var_decl);
  // Check that we only ever get array variables here.
  ROSE_ASSERT(isSgArrayType(var_name->get_type()));

  SgType* var_type = var_name->get_type();
  printf("var_type\n");
  printNode(var_type);

  SgArrayType* arr_type = isSgArrayType(var_type);
  printf("arr_type\n");
  printNode(arr_type);

  printf("rank\n");
  printf("%u\n", arr_type->get_rank());

  printf("base_type\n");
  printNode(arr_type->get_base_type());

  if(arr_type->get_dim_info()) {
    printf("dim_info\n");
    printNode(arr_type->get_dim_info());
  }

  if(arr_type->get_index()) {
    printf("index\n");
    printNode(arr_type->get_index());
  }

  SgType* base_type = arr_type->get_base_type();
  //ROSE_ASSERT(!isSgPointerType(base_type));
  printf("base_type\n");
  printNode(base_type);

  // Changed: Nov 21 2011, 1PM
  #if 0
  SgExpression* index_exp = arr_type->get_index();
  printf("index_exp\n");
  printNode(index_exp);
  #endif

  // Move the index_exp into a single statement by itself, and assign it to a local
  // variable.
  // That local variable will be used in the variable declaration, and
  // also in the create_entry function.

  // Nov 3 2011 5 PM
  // Don't do this pushing into an index exp -- assume that the index for an array is
  // constant, and we can call it any number of times
  #if 0
  SgVariableDeclaration* index_var_decl = pushIndexToLocalVar(var_name->get_name(), index_exp, var_name->get_scope());

  #if 0
  SgExpression* index_var_init = copyExpression(index_exp);
  SgType* index_var_type = base_type;
  SgName index_var_sgname(var_name->get_name().getString() + "_index");

  SgVariableDeclaration* index_var_decl = createLocalVariable(index_var_sgname, index_var_type, index_var_init, var_name->get_scope());
  #endif
  // Insert the local variable decl before the current var_decl
  insertStatementBefore(var_decl, index_var_decl);


  printf("index_var_decl\n");
  printNode(index_var_decl);

  SgVarRefExp* index_var_ref = buildVarRefExp(index_var_decl);

  // Make the switch from index_exp to index_var
  deepDelete(isSgNode(index_exp));
  arr_type->set_index(index_var_ref);
  index_var_ref->set_parent(arr_type);
  #endif

  // We don't need to create a struct variable to shadow the array... we'll create a struct at each access from the original variable
  // ****************************RETURNING******************************
  return;



  // Now, we create a local variable to shadow the array variable
  //SgVariableDeclaration* str_var_decl = createStructVariableForArrayVar(var_decl);
  SgVariableDeclaration* str_var_decl = createStructVariableFor(var_decl, true);
  // Insert this var_decl after the original decl
  insertStatementAfter(var_decl, str_var_decl);
  handleSymbolTable(var_decl, str_var_decl);

  printf("str_var_decl\n");
  printNode(str_var_decl);

  // Lets initialize the struct
  // No flipping of pointer init -- thats useful in the func param case
  SgCommaOpExp* init_expr = initializeStructWith(getNameForDecl(var_decl), getNameForDecl(str_var_decl));

  // Now, for the interesting part --
  // To actually create an entry -- from the array lower bound and upper bound.
  // fncall will be: create_entry(addr, base, size);
  // addr - is address of the array variable
  // base - reference to array variable
  // size - index_var_ref

  SgName fn_name("create_entry");
  // In fact, we could create a new function which takes the struct
  // and the index, and does the insertion. We are taking a more generic
  // approach here by providing the address, the base and the offset -- like
  // the malloc case
  SgExpression* addr = createDotExpFor(getNameForDecl(str_var_decl), "addr");
  SgExpression* base = createDotExpFor(getNameForDecl(str_var_decl), "ptr");

  // We will be using the base as the lower bound... so, cast it to the addr type
  // C doesn't like unnecessary casts.
  if(base->get_type()->get_mangled() != getAddrType()->get_mangled()) {
    base = castToAddr(base);
  }

//  SgExpression* index_ref = copyExpression(index_var_ref);
  SgExpression* index_ref = copyExpression(getArrayIndex(getNameForDecl(var_decl)));
//  SgExpression* casted_index_ref = buildCastExp(index_ref, getSizeOfType(), SgCastExp::e_reinterpret_cast);
  SgExpression* casted_index_ref = buildCastExp(index_ref, getSizeOfType(), CAST_TYPE);
  SgType* retType = SgTypeVoid::createType();

  #ifdef OLD_NAME_GEN
//  SgExpression* create_fn = build3ArgOverloadFn(fn_name, addr, base, casted_index_ref, retType, var_name->get_scope());
  SgExpression* create_fn = build3ArgOverloadFn(fn_name, addr, base, casted_index_ref, retType, var_name->get_scope(), findInsertLocation(str_var_decl));
  #else
//  SgExpression* create_fn = build3ArgOverloadFn("create_entry", addr, base, casted_index_ref, retType, var_name->get_scope());
  SgExpression* create_fn = build3ArgOverloadFn("create_entry", addr, base, casted_index_ref, retType, var_name->get_scope(), findInsertLocation(str_var_decl));
  #endif

  SgExpression* init_create = buildCommaOpExp(init_expr, create_fn);

  SgExprStatement* init_create_stmt = buildExprStatement(init_create);

  insertStatementAfter(str_var_decl, init_create_stmt);

  printf("init_create_stmt\n");
  printNode(init_create_stmt);

}


void ArithCheck::handleVarRefs(SgVarRefExp* var_ref) {
  // FIXME: Instead of looking up the map to use a previously declared struct,
  // create a struct everytime a variable is referenced.
  // Currently, this handles both pointer and array var refs
  printf("handleVarRefs\n");

  SgVariableSymbol* var_symbol = var_ref->get_symbol();
  //SgSymbol* var_symbol = var_ref->get_symbol();
  ROSE_ASSERT(var_symbol != NULL);
  VariableSymbolMap_t::iterator iter = ArithCheck::varRemap.find(var_ref->get_symbol());
  ROSE_ASSERT(iter != ArithCheck::varRemap.end());
  SgVariableSymbol* str_symbol = iter->second;

  ROSE_ASSERT(str_symbol != NULL);
  SgVarRefExp* str_var_ref = buildVarRefExp(isSgVariableSymbol(str_symbol));
  ROSE_ASSERT(str_var_ref != NULL);
  mapExprToOriginalType(str_var_ref, var_ref->get_type());
  printf("handleVarRefs\n");
  #ifdef ONLY_ADD_ARRAYS_TO_OVR
  if(isSgArrayType(var_ref->get_type())) {
    mapExprToOriginalVarRef(str_var_ref, var_ref);
    printf("mapping\n");
    printf("str_var_ref\n");
    printNode(str_var_ref);
    printf("key: %llu\n", (unsigned long long)str_var_ref);
    printf("var_ref\n");
    printNode(var_ref);

  }
  replaceExpression(var_ref, str_var_ref);
  #else
  mapExprToOriginalVarRef(str_var_ref, var_ref);
  printf("mapping\n");
  printf("str_var_ref\n");
  printNode(str_var_ref);
  printf("key: %llu\n", (unsigned long long)str_var_ref);
  printf("var_ref\n");
  printNode(var_ref);

  replaceExpression(var_ref, str_var_ref);
  #endif
  printf("Done\n");
}

SgName ArithCheck::getFunctionName(SgFunctionCallExp* fncall) {

  SgFunctionSymbol* fnsymbol = fncall->getAssociatedFunctionSymbol();
  return fnsymbol->get_name();
}


SgExprListExp* ArithCheck::makeImplicitCastsExplicitForFnCall(SgFunctionCallExp* fncall) {

  SgFunctionDeclaration* fn_decl = fncall->getAssociatedFunctionDeclaration();
  SgFunctionParameterList* param_list = fn_decl->get_parameterList();
  SgInitializedNamePtrList& param_names = param_list->get_args();

  SgExprListExp* curr_params = fncall->get_args();
  SgExpressionPtrList& expressions = curr_params->get_expressions();

  SgInitializedNamePtrList::iterator name_iter = param_names.begin();
  SgExpressionPtrList::iterator expr_iter = expressions.begin();

  SgExprListExp* new_params = buildExprListExp();

  // param_names.size() >= exprs.size() --- default arguments need not be specified
  printf("fn_decl(param_names)\n");
  printNode(fn_decl);
  printf("fncall(expressions)\n");
  printNode(fncall);
  printf("size: param_names: %d, expressions: %d\n", param_names.size(), expressions.size());
  ROSE_ASSERT(param_names.size() >= expressions.size());
  for(; name_iter != param_names.end(); name_iter++, expr_iter++) {

    SgInitializedName* name = *name_iter;
    SgExpression* expr = *expr_iter;

    if((isQualifyingType(name->get_type()) || isValidStructType(name->get_type()))) {
      // This is a pointer/array argument.
      if(isQualifyingType(expr->get_type())) {
        printf("expr not converted to struct type. expr:\n");
        printNode(expr);
        ROSE_ASSERT(0);
      }
      else if(!isValidStructType(expr->get_type())) {
        printf("implicit cast detected for expr:\n");
        printNode(expr);

        SgType* casted_type;
        if(isQualifyingType(name->get_type())) {
          casted_type = name->get_type();
        }
        else {
          ROSE_ASSERT(isValidStructType(name->get_type()));
          casted_type = findInUnderlyingType(name->get_type());
        }

        // Take the address of this expression, since we are trying to pass this by reference.

        SgCastExp* cast_exp = buildCastExp(copyExpression(expr), casted_type, SgCastExp::e_C_style_cast);

        // We could use this second_copy to replace the original expr back... although, there might not
        // be any case to do so...
        SgExpression* second_copy = copyExpression(expr);

        // we are replacing expr with cast_exp, so that we can get scope in the handleCastExp
        replaceExpression(expr, cast_exp);

        SgExpression* overload = handleCastExp(cast_exp);

        new_params->append_expression(overload);

      }
      else {
        ROSE_ASSERT(isValidStructType(expr->get_type()));
        new_params->append_expression(copyExpression(expr));
      }
    }
    else {
      new_params->append_expression(copyExpression(expr));
    }
  }

  // Just to make sure we haven't lost any args in this process
  ROSE_ASSERT(new_params->get_expressions().size() == curr_params->get_expressions().size());

  return new_params;

}

SgScopeStatement* ArithCheck::getScopeForExp(SgExpression* exp) {
//  SgStatement* stmt = getSurroundingStatement(*isSgNode(exp));
  SgStatement* stmt = getEnclosingStatement(isSgNode(exp));
  ROSE_ASSERT(stmt != NULL);
    SgScopeStatement* scope = stmt->get_scope();
  return scope;
}

bool ArithCheck::argumentsAreStructs(SgFunctionCallExp* fncall) {

  SgExprListExp* args = fncall->get_args();
  SgExpressionPtrList& exprs = args->get_expressions();
  SgExpressionPtrList::iterator it = exprs.begin();

  for(; it != exprs.end(); ++it) {

    SgExpression* ce = *it;
    // This arg is of class type, but not of the structs that we created...
    // If its a struct that we created, no need to transform since it is a pointer
    // begin passed. No implicit copy involved.
    if(isOriginalClassType(ce->get_type())) {
      return true;
    }
  }

  return false;

}


SgExprListExp* ArithCheck::convertStructArgsToRefs(SgFunctionCallExp* fncall) {

  SgExprListExp* args = fncall->get_args();
  SgExpressionPtrList& exprs = args->get_expressions();

  SgExprListExp* new_params = buildExprListExp();

  SgExpressionPtrList::iterator it = exprs.begin();

  for(; it != exprs.end(); ++it) {
    SgExpression* ce = *it;

    if(isOriginalClassType(ce->get_type())) {
      //FIXME: Would this work for cases where address of can't be taken... ?
      SgExpression* ce_addr = createAddressOfOpFor(copyExpression(ce));
      new_params->append_expression(ce_addr);
    }
    else {
      new_params->append_expression(copyExpression(ce));
    }
  }

  SgExpressionPtrList& new_exprs = new_params->get_expressions();

  ROSE_ASSERT(exprs.size() == new_exprs.size());

  return new_params;
}

void ArithCheck::handleFuncCalls(SgFunctionCallExp* fncall) {

  printf("handleFuncCalls\n");
  printf("Function: %s\n", fncall->getAssociatedFunctionDeclaration()->get_name().str());

  // The function has to return the struct since we lose the metadata otherwise...
  // But this func-call exp will stand out as casting a struct to a pointer right?
  #if 0
  if(isDotOrArrow(fncall->get_parent())) {
    printf("Parent is dot/arrow exp\n");
    printNode(fncall->get_parent());
    printf("returning\n");
    return;
  }
  #endif

  // Replace malloc calls with malloc_overload
  if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "malloc") == 0) {
    printf("Handling malloc\n");
    SgStatement* stmt = getSurroundingStatement(*isSgExpression(fncall));
        SgScopeStatement* scope = stmt->get_scope();

    SgExprListExp* args = fncall->get_args();
    SgExpressionPtrList& exprs = args->get_expressions();
    ROSE_ASSERT(exprs.size() == 1);
    SgExpression* arg1 = *(exprs.begin());

    SgExpression* arg1_copy = copyExpression(arg1);
    // Do a c_Style_cast to unsigned int
    SgExpression* arg1_copy_casted = buildCastExp(arg1_copy, getSizeOfType(), SgCastExp::e_C_style_cast);


//    SgExpression* args_copy = copyExpression(args);

    // Return type is VoidStruct
    // The return type for malloc stays as VoidStruct since this is the parallel to returning
    // a void pointer
    // We could be more systematic here, and use the getStructType
    // Since we may not have used void pointers before, we should account
    // for the fact that a new structure might need to be created
    SgType* retType = getStructType(fncall->get_type(), findInsertLocation(getEnclosingStatement(fncall)), true);
    //SgType* retType = getStructType(SgPointerType::createType(SgTypeVoid::createType()));

    // We use buildOverloadFn here because we are supplying the function malloc_overload.
    // In typically func-calls, the function signature is being changed everywhere, so we
    // would be using the programmer's versions of the functions -- albeit slightly tweaked
//    SgExpression* overload = buildOverloadFn("malloc_overload", args_copy, NULL, retType, scope);
//    SgExpression* overload = buildOverloadFn("malloc_overload", arg1_copy, NULL, retType, scope);
    // Working one... Commented out to add fncall
//    SgExpression* overload = buildOverloadFn("malloc_overload", arg1_copy_casted, NULL, retType, scope);
    SgExpression* overload = buildOverloadFn("malloc_overload", arg1_copy_casted, NULL, retType, scope, findInsertLocation(getEnclosingStatement(fncall)));

    replaceExpression(fncall, overload);

  }
  // Replace realloc calls with realloc_overload
  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "realloc") == 0) {
    printf("Handling realloc\n");
    SgStatement* stmt = getSurroundingStatement(*isSgExpression(fncall));
      SgScopeStatement* scope = stmt->get_scope();

    SgExprListExp* args = fncall->get_args();
    SgExpressionPtrList& exprs = args->get_expressions();
    ROSE_ASSERT(exprs.size() == 2);
    SgExpressionPtrList::iterator iter = exprs.begin();
    SgExpression* arg1 = *iter;
    ROSE_ASSERT(isValidStructType(arg1->get_type()));

    SgExpression* arg1_copy = copyExpression(arg1);

    ++iter;
    SgExpression* arg2 = *iter;

    SgExpression* arg2_copy = copyExpression(arg2);
    // Do a c_Style_cast to unsigned int
    SgExpression* arg2_copy_casted = buildCastExp(arg2_copy, getSizeOfType(), SgCastExp::e_C_style_cast);

    SgType* retType = getStructType(fncall->get_type(), findInsertLocation(getEnclosingStatement(fncall)), true);

//    SgExpression* overload = buildOverloadFn("realloc_overload", arg1_copy, arg2_copy_casted, retType, scope);
    SgExpression* overload = buildOverloadFn("realloc_overload", arg1_copy, arg2_copy_casted, retType, scope, findInsertLocation(getEnclosingStatement(fncall)));

    replaceExpression(fncall, overload);

  }
  else if(isFree(fncall)) {
    printf("Handling free\n");
    SgStatement* stmt = getSurroundingStatement(*isSgExpression(fncall));
        SgScopeStatement* scope = stmt->get_scope();

    SgExprListExp* args = fncall->get_args();
    SgExpressionPtrList& exprs = args->get_expressions();
    ROSE_ASSERT(exprs.size() == 1);
    SgExpression* arg1 = *(exprs.begin());
    SgExpression* arg1_copy = copyExpression(arg1);


//    SgExpression* args_copy = copyExpression(args);
//    printf("args_copy type\n");
//    printNode(args_copy->get_type());
    ROSE_ASSERT(fncall->get_type()->get_mangled() == SgTypeVoid::createType()->get_mangled());
    SgType* retType = SgTypeVoid::createType();
//    SgExpression* overload = buildOverloadFn("free_overload", args_copy, NULL, retType, scope);
    // Working one... Commented out to add fncall
//    SgExpression* overload = buildOverloadFn("free_overload", arg1_copy, NULL, retType, scope);
    SgExpression* overload = buildOverloadFn("free_overload", arg1_copy, NULL, retType, scope, findInsertLocation(getEnclosingStatement(fncall)));

    replaceExpression(fncall, overload);

  }
  // This should work in general for all "qualifying lib calls"
//  else if(strcmp(fncall->getAssociatedFunctionDeclaration()->get_name().str(), "printf") == 0) {
  else if(isQualifyingLibCall(fncall)) {
    printf("Handling qual lib call\n");
    // Basically, deref all the struct arguments

    SgExprListExp* args = fncall->get_args();
    SgExpressionPtrList& exprs = args->get_expressions();

    SgExpressionPtrList::iterator iter = exprs.begin();
    SgExprListExp* new_args = buildExprListExp();

    for(; iter != exprs.end(); ++iter) {
      SgExpression* ce = *iter;
      SgExpression* ce_copy = copyExpression(ce);
      if(isValidStructType(ce->get_type())) {
        //SgExpression* new_exp = createDerefOvlForStruct(ce_copy);
        SgExpression* new_exp = createDerefOvlForStruct(ce);
        new_args->append_expression(new_exp);
      }
      else {
        new_args->append_expression(ce_copy);
      }
    }

    SgName fn_name(fncall->getAssociatedFunctionDeclaration()->get_name());
    // if this get_type doesn't work, use fn->getAssociatedFunctionDeclaration()->get_orig_return_type()
    SgType* retType = fncall->get_type();
    printf("fncall\n");
    printNode(fncall);
    printf("retType\n");
    printNode(retType);

    // if return type is of struct type, return the underlying type... this is a library call
    // we don't change the return types for these things
    if(isValidStructType(retType)) {
      // We will never return a struct type --- correct?
      ROSE_ASSERT(0);
      retType = findInUnderlyingType(retType);
    }


    SgExpression* new_fncall = buildFunctionCallExp(fn_name, retType, new_args, getScope(fncall));
    replaceExpression(fncall, new_fncall);

    if(isQualifyingType(retType)) {
      // Need to create a struct out of it.
      // First create a local variable... which will take the return value of the function
      // then, create_entry from that local variable.
      // retType libcall_ret;
      // libcall_ret = qual_lib_call(), create_entry(&ret, ret, sizeof(ret)), create_struct_from_addr(&ret)

      // name, type, init, scope
      SgName s_name(retType->get_mangled() + "_libcall_ret_" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));
      SgVariableDeclaration* var_decl = createLocalVariable(s_name, retType, NULL, getScope(new_fncall));
      insertStatementBefore(getSuitablePrevStmt(getEnclosingStatement(new_fncall)), var_decl);

      // libcall_ret = qual_lib_call()
      SgExpression* assign_op = buildAssignOp(buildVarRefExp(var_decl), copyExpression(new_fncall));

      // create_entry
      SgExpression* ce1 = castToAddr(createAddressOfOpFor(buildVarRefExp(var_decl)));
      SgExpression* ce2 = buildCastExp(buildVarRefExp(var_decl), getAddrType(), SgCastExp::e_C_style_cast);
      SgExpression* ce3 = buildCastExp(buildSizeOfOp(buildVarRefExp(var_decl)), getSizeOfType(), SgCastExp::e_C_style_cast);

      SgExpression* ce_fncall = build3ArgOverloadFn("create_entry", ce1, ce2, ce3, SgTypeVoid::createType(), getScope(new_fncall), findInsertLocation(getEnclosingStatement(new_fncall)));

      // create_struct_from_addr(&ret)
      SgExpression* cs_from_addr = createStructUsingAddressOf(buildVarRefExp(var_decl), findInsertLocation(var_decl));

      SgExpression* init_ce = buildCommaOpExp(assign_op, ce_fncall);
      SgExpression* init_ce_cs = buildCommaOpExp(init_ce, cs_from_addr);

      replaceExpression(new_fncall, init_ce_cs);
    }

  }
  else {
    //FIXME: Implicit cast stuff is removing certain function calls... don't know why
    // array[ctr].set_val becomes set_val
    #ifdef CLASS_STRUCT_COPY
    if(!isSgPointerType(fncall->get_type()) && !argumentsAreStructs(fncall)) {
      // No implicit casts. pointers not returned.
      // Arguments are not of class/struct type.
      //So, nothing to do.
      return;
    }
    #else
    if(!isSgPointerType(fncall->get_type())) {
      // No implicit casts. pointers not returned.
      //So, nothing to do.
      return;
    }
    #endif

    // This part handles all other function calls... even those which don't have
    // pointer return values... we would have those function calls... to check for
    // implicit casts...

    // If there are some args, which are not pointers yet, but would be converted
    // to pointers, we need to do a value to struct cast.
    // The AST keeps this implicit conversion, implicit! We need to, therefore, find
    // in which cases such an implicit conversion happens, and make it explicit, with
    // a value to struct cast

    if(isSgPointerType(fncall->get_type())) {
      printf("Function call returning pointer. Change return type to struct\n");
    }
    else {
      printf("Function call not returning pointer. Check for struct objects and implicit casts\n");
    }

    //ROSE_ASSERT(isSgPointerType(fncall->get_type()));

    // We'll create a new func call with a different type, but everything else
    // stays the same.

    #ifdef CLASS_STRUCT_COPY
    // ****IMPORTANT**** We change the parameters below again!
    SgExprListExp* parameters;
    if(argumentsAreStructs(fncall)) {
      parameters = convertStructArgsToRefs(fncall);
    }
    else {
      parameters = isSgExprListExp(copyExpression(isSgCallExpression(fncall)->get_args()));
    }
    #else
    SgExprListExp* parameters = isSgExprListExp(copyExpression(isSgCallExpression(fncall)->get_args()));
    #endif

    SgScopeStatement* scope = getScopeForExp(fncall);
    SgName fn_name(getFunctionName(fncall));

    // New return type -- accounting for both pointer and array types here -- although
    // could you return an array type from a function?
    #ifdef CLASS_STRUCT_COPY
    bool convertFnCallToCommaOp = false;
    SgVariableDeclaration* retvar;
    #endif
    SgType* retType;
    if(isQualifyingType(fncall->get_type())) {
      retType = getStructType(getTypeForPntrArr(fncall->get_type()), findInsertLocation(getEnclosingStatement(fncall)), true);
    }
    #ifdef CLASS_STRUCT_COPY
    else if(isOriginalClassType(fncall->get_type())) {
      // Here we are returning a variable/object of struct/class type... other than the ones we created... so,
      // we should create a local variable to hold the ret value, and pass it by reference.
      // original code: struct A var = fn(...)
      // transformed code: struct A retvar; struct A var = (fn(..., &retvar), retvar)

      // 1. create a local varible... of the same type as the return type.
      SgName var_name("retvar_" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));
      retvar = buildVariableDeclaration(var_name, fncall->get_type(), NULL, getScope(fncall));
      insertStatementBefore(getEnclosingStatement(fncall), retvar);

      // 2. add &retvar to the end of a parameters list
      parameters->append_expression(createAddressOfOpFor(buildVarRefExp(retvar)));

      // 3... this part requires the final fncall expr... fncall(..., &retvar), retvar
      convertFnCallToCommaOp = true;

      // 4. Change the return type to void...
      retType = SgTypeVoid::createType();

    }
    #endif
    else {
      // leave the return type as is...
      retType = fncall->get_type();
    }

    SgFunctionCallExp* new_fncall = buildFunctionCallExp(fn_name, retType, parameters, scope);
    replaceExpression(fncall, new_fncall);

    #ifdef CLASS_STRUCT_COPY
    if(convertFnCallToCommaOp) {
      ROSE_ASSERT(retvar != NULL);
      // Now to do the third part... replace fn(..., &retvar) with fn(..., &retvar), retvar
      SgCommaOpExp* comma_op = buildCommaOpExp(copyExpression(new_fncall), buildVarRefExp(retvar));
      replaceExpression(new_fncall, comma_op);
    }
    #endif

  }
  printf("handleFuncCalls - Done\n");
}

SgType* ArithCheck::getUnderlyingTypeForStruct(SgType* str_type) {

  TypeMap_t::iterator iter = ArithCheck::UnderlyingType.find(str_type);

  ROSE_ASSERT(iter != ArithCheck::UnderlyingType.end());

  return iter->second;
}


SgExpression* ArithCheck::createStructUsingAddressOf(SgExpression* exp, SgNode* pos) {
  printf("createStructUsingAddressOf\n");
  printNode(exp);

  SgScopeStatement* scope;
  if(isSgVarRefExp(exp)) {
    scope = getScopeForVarRef(isSgVarRefExp(exp));
  }
  else {
    //scope = getScope(exp);
    scope = getScopeForExp(exp);
  }

  // Lvalue should be set as false since we are using the address
  // here to create a struct. Not writing to this expression at all
  SgExpression* copy_exp = copyExpression(exp);
  copy_exp->set_lvalue(false);

  SgExpression* addr_of = castToAddr(createAddressOfOpFor(copy_exp));

//  SgType* retType = getStructType(getTypeForPntrArr(exp->get_type()), getScopeForExp(exp), true);
//  SgType* retType = getStructType(getTypeForPntrArr(exp->get_type()), findInsertLocation(getEnclosingStatement(exp)), true);
  SgType* retType = getStructType(getTypeForPntrArr(exp->get_type()), pos, true);


  #if 0
  if(isSgArrayType(exp->get_type())) {
    // If its an array type, then we need to use the pointer to the base type
    // struct
    SgType* base_type = (isSgArrayType(exp->get_type()))->get_base_type();
    retType = getStructType(SgPointerType::createType(base_type), true);
  }
  else {
    retType = getStructType(exp->get_type(), true);
  }
  #endif

  //SgScopeStatement* scope = getScopeForExp(exp);

  #if 0
  printf("exp->get_parent()\n");
  printNode(exp->get_parent());
  #endif

  #ifdef OLD_NAME_GEN
  SgName fn_name(getStringForFn("create_struct_from_addr", addr_of->get_type(), NULL, NULL, retType));
//  SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope);
//  SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope, findInsertLocation(getEnclosingStatement(exp)));
  SgExpression* createfn = buildOverloadFn(fn_name, addr_of, NULL, retType, scope, pos);

  #else
//  SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope);
//  SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope, findInsertLocation(getEnclosingStatement(exp)));
  SgExpression* createfn = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, scope, pos);
  #endif

  return createfn;

}

SgExpression* ArithCheck::createDerefOutputStruct(SgPointerDerefExp* ptr_deref) {
  printf("createDerefOutputStruct\n");
  return createStructUsingAddressOf(ptr_deref, findInsertLocation(getEnclosingStatement(ptr_deref)));

}

void ArithCheck::handleDerefExp(SgPointerDerefExp* ptr_deref) {

  printf("handleDerefExp\n");



  SgExpression* oper = isSgUnaryOp(ptr_deref)->get_operand();

  // createDerefOvlForStruct creates a copy.
  SgExpression* overload = createDerefOvlForStruct(oper);
  printf("overload\n");
  printNode(overload);

  #if 0
  SgExpression* oper_copy = copyExpression(oper);

  // Find scope
  SgScopeStatement* scope = getScopeForExp(ptr_deref);

  // The return type is given by the ReverseMap from struct var
  //SgType* retType = SgPointerType::createType(SgTypeInt::createType());

  // Assert that current operand type is struct type
  ROSE_ASSERT(isValidStructType(oper->get_type()));
  // Now, get the type that we will be returning
//  SgType* retType = getUnderlyingTypeForStruct(oper->get_type());
  SgType* retType = findInUnderlyingType(oper->get_type());

  SgExpression* overload = buildOverloadFn("Deref", oper_copy, NULL, retType, scope);
  printf("overload\n");
  printNode(overload);
  #endif

  SgExpression* new_deref_exp = buildPointerDerefExp(overload);

//  replaceExpression(oper, overload);
  replaceExpression(ptr_deref, new_deref_exp);
  printf("new_deref_exp\n");
  printNode(new_deref_exp);




  // Now, suppose the deref returns a pointer/array... that needs to be handled...
  // we would need to create a struct for it, on the fly. What we should do
  // is pass the address of the return var of the deref to a create_struct.
  // function. This function takes the address, and create a struct
  // of the appropriate type from it... and return that struct.
  // The crucial thing here is that we are using the address of that variable
  // deref'ed or pntr arr ref'ed, to create a struct.
  // We can't take the pointer value, and address to the create_struct
  // because doing the deref twice might have side effects.


//  if(isQualifyingType(ptr_deref->get_type())) {
  if(isQualifyingType(new_deref_exp->get_type())) {

    printf("Deref returns a qualifying type\n");

//    SgExpression* createfn = createStructUsingAddressOf(new_deref_exp);
    SgExpression* createfn = createDerefOutputStruct(isSgPointerDerefExp(new_deref_exp));

    replaceExpression(new_deref_exp, createfn);

  }



  // FIXME: Until we know how to handle func derefs correctly,
  // we can't fill up this gap
  #if 0
  // Two cases here:
  // 1. Return VoidStruct if the type of node is pointer
  // 2. Return POD if the type of the node is something else

  SgType* expr_type = ptr_deref->get_type();

  SgType* ret_type;

  if(isSgPointerType(expr_type)) {
  #endif


}

void ArithCheck::handleConditionals(SgBinaryOp* bop) {

  printf("handleConditionals\n");

  //std::string op_name(getStringForVariantT(bop->variantT()));

  // not defined..
  #ifdef COND_BOOL_RETVAL
  SgType* retType = SgTypeBool::createType();
  #else
  SgType* retType = SgTypeInt::createType();
  #endif

  // Conditionals always return bool
  std::string op_name = getStringForOp(isSgNode(bop), retType);

  SgExpression* LHS = bop->get_lhs_operand();
  SgExpression* LHS_Copy = copyExpression(LHS);

  SgExpression* RHS = bop->get_rhs_operand();
  SgExpression* RHS_Copy = copyExpression(RHS);

  if(isSgValueExp(RHS) || isSgValueExp(LHS)) {
    // Both can't be values -- right?
    ROSE_ASSERT(isSgValueExp(LHS) && isSgValueExp(RHS));
    op_name = "_ValueCast";
  }

  #ifdef OLD_NAME_GEN
//  SgExpression* overload = buildOverloadFn(SgName(op_name), LHS_Copy, RHS_Copy, retType, getScopeForExp(bop));
  SgExpression* overload = buildOverloadFn(SgName(op_name), LHS_Copy, RHS_Copy, retType, getScopeForExp(bop), findInsertLocation(getEnclosingStatement(bop)));
  #else
//  SgExpression* overload = buildOverloadFn(SgName(getStringForVariantT(bop->variantT())), LHS_Copy, RHS_Copy, retType, getScopeForExp(bop));
  SgExpression* overload = buildOverloadFn(SgName(getStringForVariantT(bop->variantT())), LHS_Copy, RHS_Copy, retType, getScopeForExp(bop), findInsertLocation(getEnclosingStatement(bop)));
  #endif

  replaceExpression(bop, overload);

  #ifdef INLINING_CHECK
//  bool status = inlining(isSgFunctionCallExp(overload));
  bool status = doInline(isSgFunctionCallExp(overload));
  printf("INLINING_CHECK: status: %s\n", status ? "true" : "false");
  #endif
}

SgType* skipArrayTypes(SgArrayType* arr_type) {

  SgType* base_type = arr_type->get_base_type();
  if(isSgArrayType(base_type)) {
    return skipArrayTypes(isSgArrayType(base_type));
  }
  return base_type;
}

SgType* ArithCheck::getTypeForPntrArr(SgType* type) {

  if(isSgPointerType(type)) {
    return type;
  }
  else if(isSgArrayType(type)) {
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
  }
  else {
    ROSE_ASSERT(isQualifyingType(type));
    printf("Qualifying Type not supported in getTypeForPntrArr\n");
    ROSE_ASSERT(0);
  }
}

SgInitializedName* ArithCheck::createStructVariableFor(SgInitializedName* name) {

  std::string new_name_string;

  if(strcmp(name->get_name().getString().c_str(), "") == 0) {
    new_name_string = "";
  }
  else {
    new_name_string = name->get_name().getString() + "_str";
  }

  SgName new_name_sg(new_name_string);
  printf("created new name: %s\n", new_name_string.c_str());

  // FIXME: Assuming no initializer
  SgInitializer* name_init = name->get_initializer();
  ROSE_ASSERT(name_init == NULL);

  #if 0
  // Get the VoidStruct Type
  SgType* new_name_type = getStructType(SgPointerType::createType(SgTypeVoid::createType()));
  #endif

  // create one if necessary

  // Working one
//  SgType* new_name_type = getStructType(getTypeForPntrArr(name->get_type()), name, true);
  SgType* new_name_type = getStructType(getTypeForPntrArr(name->get_type()), findInsertLocation(name), true);

  SgInitializedName* new_name = buildInitializedName(new_name_sg, new_name_type, NULL);

  ROSE_ASSERT(new_name != NULL);

  new_name->set_scope(name->get_scope());

  return new_name;
}


SgVariableDeclaration* ArithCheck::createLocalVariable(SgInitializedName* orig) {

  // Create a local variable of the type given by orig
  std::string local_var_name = orig->get_name().getString() + "_recr";

  SgVariableDeclaration* local_var_decl = SageBuilder::buildVariableDeclaration(local_var_name, orig->get_type(), NULL, orig->get_scope());

  return local_var_decl;
}

SgVariableDeclaration* ArithCheck::createLocalVariable(SgName name, SgType* type, SgExpression* init, SgScopeStatement* scope) {


  SgAssignInitializer* assign_init = NULL;
  if(init != NULL) {
    // Create a local variable from the arguments
    assign_init = buildAssignInitializer(init, init->get_type());
  }

  SgVariableDeclaration* local_var_decl = SageBuilder::buildVariableDeclaration(name, type, assign_init, scope);

  return local_var_decl;

}

SgAddressOfOp* ArithCheck::createAddressOfOpFor(SgInitializedName* name) {

  SgVarRefExp* var_ref_exp = buildVarRefExp(name, name->get_scope());

  return createAddressOfOpFor(var_ref_exp);

}

#if 0
SgAddressOfOp* ArithCheck::createAddressOfOpFor(SgVarRefExp* exp) {

  SgAddressOfOp* address_of_op = buildAddressOfOp(exp);

  return address_of_op;
}
#endif

SgAddressOfOp* ArithCheck::createAddressOfOpFor(SgExpression* exp) {


  #ifdef ADDROP_LVAL_FALSE
  exp->set_lvalue(false);
  #endif

  SgAddressOfOp* address_of_op = buildAddressOfOp(exp);

  return address_of_op;
}




void ArithCheck::createLocalVariableAndUpdateMetadata(SgInitializedName* vs,
                            SgInitializedName* orig,
                            SgFunctionDeclaration* fn_decl) {


  SgVariableDeclaration* local_var_decl = createLocalVariable(orig);

  // Insert this variable at the top of the body
  SgFunctionDefinition* fn_def = fn_decl->get_definition();
  ROSE_ASSERT(fn_def != NULL);
  SgBasicBlock* bb = fn_def->get_body();
  SgStatementPtrList& stmts = bb->get_statements();

  //isSgScopeStatement(fn_decl->get_scope())->insertStatementInScope(local_var_decl, true);
  SageInterface::insertStatementBefore(*(stmts.begin()), local_var_decl);
  printf("Insert local_var_decl\n");


  // Now, insert a function call which creates an entry from this variable's address
  // and uses data from the vs addr
  SgExpression* addressOfOp = createAddressOfOpFor(getNameForDecl(local_var_decl));
  // Lets cast this addressOfOp to unsigned long long -- addr type
  //SgCastExp* cast_exp = buildCastExp(addressOfOp, getAddrType(), SgCastExp::e_reinterpret_cast);
  SgCastExp* cast_exp = buildCastExp(addressOfOp, getAddrType(), CAST_TYPE);

  SgExpression* dot_exp = createDotExpFor(vs, "addr");

  // Could also use the fn def scope
  //SgExpression* overload = buildOverloadFn("create_entry", addressOfOp, dot_exp, SgTypeVoid::createType(), getNameForDecl(local_var_decl)->get_scope());
  // Working one.. Commented out to add local_var_decl
//  SgExpression* overload = buildOverloadFn("create_entry", cast_exp, dot_exp, SgTypeVoid::createType(), getNameForDecl(local_var_decl)->get_scope());
  SgExpression* overload = buildOverloadFn("create_entry", cast_exp, dot_exp, SgTypeVoid::createType(), getNameForDecl(local_var_decl)->get_scope(), local_var_decl);

  // We should assign input_recr = input_str.ptr -- not the other way round
  // This tweak makes initializeStructWith act this way.
  bool flip_ptr_init = true;
  // Initialize this local_var with the ptr value in the vs
  SgCommaOpExp* init_op = initializeStructWith(getNameForDecl(local_var_decl), vs, flip_ptr_init);

  // Combine both
  SgCommaOpExp* overload_init = SageBuilder::buildCommaOpExp(overload, init_op);


  // create a statement and insert it here
  SgExprStatement* ov_in = SageBuilder::buildExprStatement(isSgExpression(overload_init));

  SageInterface::insertStatementAfter(local_var_decl, ov_in);



  // Now, update the symbol so that all references to the orig
  // variable are replaced with references to vs
  handleSymbolTable(orig, vs);

}

void ArithCheck::handleFuncDecls(SgFunctionDeclaration* fn_decl) {

  // Changes to parameter lists alter the body of the function -- to insert
  // instructions that update metadata. This work is done in handleFuncParams
  // since doing it here would be too late --- we arrive at this function after
  // having transformed the body of the function.
  // This function only changes the return type to struct type if its of
  // qualifying type.

  printf("handleFuncDecls\n");
  printf("fn_decl\n");
  printNode(fn_decl);

  SgName fn_name = fn_decl->get_name();

  SgScopeStatement* scope = fn_decl->get_scope();
  printf("Got scope\n");

  SgFunctionParameterList* param_list = fn_decl->get_parameterList();
  ROSE_ASSERT(param_list != NULL);

  printf("Got paramList\n");

  SgFunctionDefinition* fndef = fn_decl->get_definition();
  bool defining_decl = (fndef != NULL);


  // Return type is given by getStructType on the current poitner type
  SgFunctionType* fn_type = fn_decl->get_type();
  SgType* ret_type = fn_type->get_return_type();

  SgType* new_ret_type;

  printf("Return type is qualifying type?\n");
  if(isQualifyingType(ret_type)) {
    printf("Yes\n");
    // fn_decl is a scope in itself
    new_ret_type = getStructType(getTypeForPntrArr(ret_type), fn_decl, true);
  }
  #ifdef CLASS_STRUCT_COPY
  else if(isOriginalClassType(ret_type)) {
    printf("Original Class Type\n");
    // return void type.. since we will be using the arguments to return this value...
    new_ret_type = SgTypeVoid::createType();
  }
  #endif
  else {
    printf("No. Retaining old ret type\n");
    new_ret_type = ret_type;
  }

  // Since the parameter work is done in handleFuncParams already, just copy over the parameter list
  SgFunctionParameterList* new_args = isSgFunctionParameterList(SageInterface::deepCopyNode(isSgNode(param_list)));

  // Keeping decorator list the same
  SgExprListExp* dec_list = fn_decl->get_decoratorList();
  SgExprListExp* new_dec_list = isSgExprListExp(deepCopyNode(isSgNode(dec_list)));

  SgFunctionDeclaration* new_fn_decl;
  if(fndef) {
    printf("Defining fn decl\n");
    // This is a defining func decl.
    new_fn_decl = buildDefiningFunctionDeclaration(fn_name,
                new_ret_type,
                new_args,
                scope,
                new_dec_list);

    SgFunctionDefinition* new_fndef = isSgFunctionDefinition(deepCopyNode(isSgNode(fndef)));

    new_fn_decl->set_definition(new_fndef);
    isSgNode(new_fndef)->set_parent(new_fn_decl);

    printf("fn_decl\n");
    printNode(fn_decl);

    printf("new_fn_decl\n");
    printNode(new_fn_decl);

    replaceStatement(fn_decl, new_fn_decl);

  }
  else {
    printf("Nondefining fn decl\n");
    // Nondefining fn decl
    new_fn_decl = buildNondefiningFunctionDeclaration(fn_name,
                new_ret_type,
                new_args,
                scope,
                new_dec_list);
    printf("fn_decl\n");
    printNode(fn_decl);

    printf("new_fn_decl\n");
    printNode(new_fn_decl);

    replaceStatement(fn_decl, new_fn_decl);

  }


  // Checking that we did the job correctly
  if(isQualifyingType(ret_type)) {
    ROSE_ASSERT(isValidStructType(new_fn_decl->get_type()->get_return_type()));
  }
  #ifdef CLASS_STRUCT_COPY
  else if(isOriginalClassType(ret_type)) {
    ROSE_ASSERT(compareTypes(SgTypeVoid::createType(), new_fn_decl->get_type()->get_return_type()));
  }
  #endif
  else {
    ROSE_ASSERT(compareTypes(ret_type, new_fn_decl->get_type()->get_return_type()));
  }

}


void ArithCheck::updateMetaDataAndReassignAddr(SgInitializedName* str,
          SgInitializedName* orig,
          SgFunctionDeclaration* fn_decl) {

  printf("updateMetaDataAndReassignAddr\n");
  // create_entry(dest, src)
  // this will be: create_entry(&str.ptr, str.addr);
  // At this stage, str.addr contains the caller variable's address.
  // so we copy over the metadata
  SgExpression* ptr_expr = createDotExpFor(str, "ptr");
  SgExpression* dest_addr = castToAddr(createAddressOfOpFor(ptr_expr));

  SgExpression* src_addr = createDotExpFor(str, "addr");

  SgScopeStatement* scope = str->get_scope();

  // create_entry function call.
//  SgExpression* createfn = buildOverloadFn("create_entry", dest_addr, src_addr, SgTypeVoid::createType(), scope);
  SgExpression* createfn = buildOverloadFn("create_entry", dest_addr, src_addr, SgTypeVoid::createType(), scope, findInsertLocation(fn_decl));

  printf("createfn\n");
  printNode(createfn);

  // Now, we make the following assignment:
  // str.addr = &str.ptr;

  SgExpression* lhs = copyExpression(src_addr);
  SgExpression* rhs = copyExpression(dest_addr);

  SgExpression* assign_op = buildAssignOp(lhs, rhs);
  printf("assign_op\n");
  printNode(assign_op);

  // Now, create a comma op to hold both these exprs
  SgExpression* comma_op = buildCommaOpExp(createfn, assign_op);

  // Create a statement from this
  SgStatement* expr_stmt = buildExprStatement(comma_op);


  // Insert this statement at the top of the body
  #if 0
  SgFunctionDefinition* fn_def = fn_decl->get_definition();
  ROSE_ASSERT(fn_def != NULL);
  SgBasicBlock* bb = fn_def->get_body();
  #endif
  SgBasicBlock* bb = getBBForFn(fn_decl);
  SgStatementPtrList& stmts = bb->get_statements();

  SageInterface::insertStatementBefore(*(stmts.begin()), expr_stmt);

  printf("expr_stmt\n");
  printNode(expr_stmt);


  //handleSymbolTable(orig, str); -- already completed in parent function ---
}

SgInitializedName* ArithCheck::createRefVersionFor(SgInitializedName* name) {

  std::string new_name_string;

  if(strcmp(name->get_name().getString().c_str(), "") == 0) {
    new_name_string = "";
  }
  else {
    new_name_string = name->get_name().getString() + "_ref";
  }

  SgName new_name_sg(new_name_string);
  printf("created new name: %s\n", new_name_string.c_str());

  // FIXME: Assuming no initializer
  SgInitializer* name_init = name->get_initializer();
  ROSE_ASSERT(name_init == NULL);

  #if 0
  // Get the VoidStruct Type
  SgType* new_name_type = getStructType(SgPointerType::createType(SgTypeVoid::createType()));
  #endif

  // create one if necessary

  // Working one
  SgType* new_name_type = SgPointerType::createType(name->get_type());

  SgInitializedName* new_name = buildInitializedName(new_name_sg, new_name_type, NULL);

  ROSE_ASSERT(new_name != NULL);

  new_name->set_scope(name->get_scope());

  return new_name;
}

SgVariableDeclaration* ArithCheck::assignRefToLocalVar(SgInitializedName* name, SgFunctionDeclaration* fn_decl, SgScopeStatement* bb_scope) {

  SgExpression* init = buildPointerDerefExp(buildVarRefExp(name));
  SgAssignInitializer* init_assign = buildAssignInitializer(init, init->get_type());
  ROSE_ASSERT(isSgPointerType(name->get_type()));
  SgType* type = isSgPointerType(name->get_type())->get_base_type();
  ROSE_ASSERT(!isSgPointerType(type));

  //SgScopeStatement* scope = name->get_scope();

  SgName s_name(name->get_name().getString() + "_local");

  SgVariableDeclaration* local_var = buildVariableDeclaration(s_name, type, init_assign, bb_scope);

  return local_var;
}

void ArithCheck::addToSymbolTable(SgInitializedName* name, SgNode* parent) {

  SgSymbol* symbol = name->get_symbol_from_symbol_table();

  if(symbol == NULL) {

    SgVariableSymbol* var_symbol = new SgVariableSymbol(name);
    isSgNode(var_symbol)->set_parent(parent);
    SgSymbolTable* currSymbolTable = isSgSymbolTable(isSgNode(var_symbol)->get_parent());
    currSymbolTable->insert(name->get_name(), var_symbol);

    // Check that this worked
    symbol = name->get_symbol_from_symbol_table();
    ROSE_ASSERT(symbol);
  }
}

void ArithCheck::addToSymbolTableAndClassRemap(SgInitializedName* orig_name, SgInitializedName* local_name) {

  SgScopeStatement* orig_scope = orig_name->get_scope();

  SgSymbol* orig_symbol = orig_name->get_symbol_from_symbol_table();
  SgVariableSymbol *orig_var_symbol = isSgVariableSymbol(orig_symbol);
  printf("Got scope. Now, symbolTable\n");

  addToSymbolTable(local_name, isSgNode(orig_var_symbol)->get_parent());

  #if 0

  SgSymbol* local_symbol = local_name->get_symbol_from_symbol_table();

  if(local_symbol == NULL) {

    SgVariableSymbol* local_var_symbol = new SgVariableSymbol(local_name);
    isSgNode(local_var_symbol)->set_parent(isSgNode(orig_var_symbol)->get_parent());
    SgSymbolTable* currSymbolTable = isSgSymbolTable(isSgNode(local_var_symbol)->get_parent());
    currSymbolTable->insert(local_name->get_name(), local_var_symbol);

    // Check that this worked
    local_symbol = local_name->get_symbol_from_symbol_table();
    ROSE_ASSERT(local_symbol);
  }
  #endif

  SgSymbol* local_symbol = local_name->get_symbol_from_symbol_table();

  ClassStructVarRemap.insert(VariableSymbolMap_t::value_type(orig_var_symbol, isSgVariableSymbol(local_symbol)));

}

SgFunctionDeclaration* ArithCheck::createRTCCopy(SgInitializedName* src, SgInitializedName* dest, SgFunctionDeclaration* fn_decl) {

  printf("createRTCCopy\n");
  // Just to make sure that src and dest are in fact from the same data struct...
  // src is a pointer... dest is an object...
  ROSE_ASSERT(isSgPointerType(src->get_type())->get_base_type() == dest->get_type());
  // Its a user defined struct, not one we inserted
  ROSE_ASSERT(isOriginalClassType(dest->get_type()));

  // src is a pointer to the struct/class type.
  // dest is a variable of struct/class type.
  // both need to be passed by reference.
  // FIXME: lock and key need to be added to the param list
  SgExprListExp* param_list = buildExprListExp(createAddressOfOpFor(buildVarRefExp(dest)), buildVarRefExp(src));

  SgName fn_name("rtc_copy");

//  if(checkIfDeclExists(fn_name, SgTypeVoid::createType(), param_list)) {
  SgScopeStatement* scope = getScope(fn_decl);
  ROSE_ASSERT(scope->get_symbol_table());
  if(scope->symbol_exists(fn_name)) {
    // Find the function in the scope where it would've been inserted... i.e. scope of fn_decl...
    SgFunctionSymbol* fn_sym = scope->lookup_function_symbol(fn_name);
    return fn_sym->get_declaration();
  }


  // FIXME: This decl shouldn't be static... createDefiningDecl automatically makes it static!
  // create a defining decl
  SgFunctionDeclaration* def_fn_decl = createDefiningDecl(SgName("rtc_copy"), SgTypeVoid::createType(), param_list,
              getScope(fn_decl), ArithCheck::ACProject, fn_decl);


  // Here, we need to get hold of the params in def_fn_decl.
  SgInitializedNamePtrList& args = def_fn_decl->get_args();
  SgInitializedName* dest_arg = *args.begin();
  SgInitializedName* src_arg = *(++args.begin());

  // Both arguments should be pointer types to the original class types
  ROSE_ASSERT(isPointerType(src_arg->get_type()) && isOriginalClassType(isSgPointerType(src_arg->get_type())->get_base_type()));
  ROSE_ASSERT(isPointerType(dest_arg->get_type()) && isOriginalClassType(isSgPointerType(dest_arg->get_type())->get_base_type()));

  // Now, find all the pointers in the data structure, and do a source to dest copy...
  SgClassType* str_type = isSgClassType(dest->get_type());

  // This part below gets us the pointers...
  SgClassDeclaration * str_decl = isSgClassDeclaration (str_type->get_declaration());

    ROSE_ASSERT (str_decl);

  printf("str_decl\n");
  printNodeExt(str_decl);

    ROSE_ASSERT(str_decl->get_firstNondefiningDeclaration()!= NULL);

  printf("str_first_nondefining_decl\n");
  printNode(str_decl->get_firstNondefiningDeclaration());
  printNodeExt(str_decl->get_firstNondefiningDeclaration());

    ROSE_ASSERT(isSgClassDeclaration(str_decl->get_firstNondefiningDeclaration()) == str_decl);

  SgClassDeclaration* str_decl_def = isSgClassDeclaration(str_decl->get_definingDeclaration());

  SgClassDefinition* str_def = str_decl_def->get_definition();

  ROSE_ASSERT(str_def != NULL);

  SgDeclarationStatementPtrList& members = str_def->get_members();

  SgDeclarationStatementPtrList::iterator iter = members.begin();

  // Now, as we iterate over the members... we check if they are pointer type,
  // and if they are, perform a create_entry(dest, src)... We will accumulate all
  // these create_entry fn calls into a single expr list exp.. which will be returned
  SgExprListExp* exprs = buildExprListExp();

  // Now, we get the basic block for def_fn_decl and insert this expression there.
  SgBasicBlock* bb = getBBForFn(def_fn_decl);

  for(; iter != members.end(); ++iter) {

    SgDeclarationStatement* decl_stmt = *iter;
    SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl_stmt);
    SgInitializedName* var_name = getNameForDecl(var_decl);

    if(isSgPointerType(var_name->get_type())) {
      // Now we have a pointer member... we need to create:

      // create_entry(&(dest->var_name), &(src->var_name))
      SgExpression* dest_arrow = buildArrowExp(buildVarRefExp(dest_arg), buildVarRefExp(var_name));
      SgExpression* ce1 = castToAddr(createAddressOfOpFor(dest_arrow));

      SgExpression* src_arrow = buildArrowExp(buildVarRefExp(src_arg), buildVarRefExp(var_name));
      SgExpression* ce2 = castToAddr(createAddressOfOpFor(src_arrow));

      SgType* retType = SgTypeVoid::createType();

      SgScopeStatement* scope = getScope(bb);

      SgExpression* overload = buildOverloadFn("create_entry", ce1, ce2, retType, scope, def_fn_decl);

      exprs->append_expression(overload);
    }
  }

  insertAtTopOfBB(bb, buildExprStatement(exprs));
  #if 0
  // stmts.begin() is zero since the body is empty
  SgStatementPtrList& stmts = bb->get_statements();
  SageInterface::insertStatementBefore(*(stmts.begin()), buildExprStatement(exprs));
  #endif

  return def_fn_decl;
}

SgExpression* ArithCheck::copyElementMetadata(SgInitializedName* src, SgInitializedName* dest, SgFunctionDeclaration* fn_decl) {

  // This function performs a copy of the metadata, of all the pointer variables within
  // src to those within dest...
  // The copying is performed using a function. This function will be placed in the body
  // of a class/struct in case of C++, and outside the struct in case of C. The code would
  // look like follows:
  // C++:
  // class/struct A{
  //     int *ptr;
  //     rtc_copy(&src, lock, key) {
  //        // call to inheritance copy functions...
  //      create_entry(&ptr, &src.ptr, lock, key); // order: dest_addr, src_addr, lock, key
  //     }
  // };
  //
  // C:
  // struct A {
  //     int *ptr;
  // };
  // rtc_copy(&dest, &src, lock, key) { -- This function has to be defined in the current function...
  //     create_entry(&dest.ptr, &src.ptr, lock, key) // order: dest_addr, src_addr, lock, key
  // }
  //
  // In an example function:
  // fn1(class A As) {
  // }
  //
  //
  // main() {
  //    class A Am;
  //    fn1(Am);
  // }
  //
  // Transformed to....
  // fn1(class A* As) {
  //    lock, key;
  //    class A Al = *As;
  //    Al.rtc_copy(As, lock, key); -- This has to be done in this function...
  // }
  //
  // main() {
  //    class A Am;
  //    fn1(&Am);
  // }
  //
  //
  // In C++, the copy constructor can't be used since we need the updated lock and key for the new scope.
  // These are not available in the traditional copy constructor call. An option that can be explored here
  // is the object level lock and key, rather than identifier level lock and key, but that would run against
  // the rest of the setup... which is pointer id based.
  //
  // In this function: src: As, dest: Al

  printf("copyElementMetadata\n");


  // Just to make sure that src and dest are in fact from the same data struct...
  // src is a pointer... dest is an object...
  ROSE_ASSERT(isSgPointerType(src->get_type())->get_base_type() == dest->get_type());
  // Its a user defined struct, not one we inserted
  ROSE_ASSERT(isOriginalClassType(dest->get_type()));

  SgExpression* src_ref = buildVarRefExp(src);
  SgExpression* dest_ref = buildVarRefExp(dest);

  // Now, find all the pointers in the data structure, and do a source to dest copy...
  SgClassType* str_type = isSgClassType(dest->get_type());

  // Two parts now...
  // 1. create the rted_copy function, for now we'll do it without the lock and key...
  // 2. call the rtc_copy function from this function... this fncall exp is the one that will
  // be returned

  // create rtc_copy function... and return the function declaration.
  // the declaration will be inserted prior to the current function for structs, and in
  // the class body for class objects.
  // FIXME: Only create if not available... use FuncInfo to track this.
  SgFunctionDeclaration* rtc_copy_fn = createRTCCopy(src, dest, fn_decl);


  // 2. creating the function call exp
  SgExprListExp* param_list = buildExprListExp(createAddressOfOpFor(buildVarRefExp(dest)), buildVarRefExp(src));
  SgFunctionSymbol* rtc_copy_fn_symbol = isSgFunctionSymbol(rtc_copy_fn->get_symbol_from_symbol_table());
  ROSE_ASSERT(rtc_copy_fn_symbol);

  SgExpression* overload = buildFunctionCallExp(rtc_copy_fn_symbol, param_list);

  return overload;
}

void ArithCheck::insertAtTopOfBB(SgBasicBlock* bb, SgStatement* stmt) {

  SgStatementPtrList& stmts = bb->get_statements();
  if(stmts.size() == 0) {
    bb->prepend_statement(stmt);
  }
  else {
    SageInterface::insertStatementBefore(*(stmts.begin()), stmt);
  }
}

SgFunctionDeclaration* ArithCheck::getFuncDeclForNode(SgNode* node) {

  SgNode* parent = node->get_parent();
  if(isSgFunctionDeclaration(parent)) {
    return isSgFunctionDeclaration(parent);
  }
  else {
    return getFuncDeclForNode(parent);
  }
}

void ArithCheck::handleReturnStmts(SgReturnStmt* retstmt) {

  printf("handleReturnStmts\n");
  SgType* retType = retstmt->get_expression()->get_type();
  ROSE_ASSERT(isOriginalClassType(retType));

  SgFunctionDeclaration* fn_decl = getFuncDeclForNode(retstmt);

  SgInitializedNamePtrList& args = fn_decl->get_args();

  // Now, find the last arg in the list.
  SgInitializedNamePtrList::iterator i = args.begin();
  i += args.size() - 1;
  SgInitializedName* last_arg = *i;

  printf("arg check:\n");
  printNode(last_arg);

  // Check that its type is of the pointer type of the ret stmt type
  ROSE_ASSERT(isSgPointerType(last_arg->get_type()));
  ROSE_ASSERT(isSgPointerType(last_arg->get_type())->get_base_type() == retType);

  // Now, do the assignment and void return substitution
  // original: return ret_exp
  // transformed: *retvar = ret_exp
  //              return

  // this copies the expression in the return statement and uses it as rhs of the assign op
  // *retvar = ret_exp;
  SgAssignOp* assign_op = buildAssignOp(buildPointerDerefExp(buildVarRefExp(last_arg)), copyExpression(retstmt->get_expression()));
  SgExprStatement* assign_stmt = buildExprStatement(assign_op);

  SgStatement* voidretstmt = buildReturnStmt();

  replaceStatement(retstmt, assign_stmt);

  insertStatementAfter(assign_stmt, voidretstmt);

  return;
}

void ArithCheck::handleFuncParams(SgFunctionParameterList* param_list) {

  // Change those parameters which are pointers, to VoidStruct.
  // Add patch up code to the body
  printf("handleFuncParams\n");
  // 2. parameter list
  // Instead of using the type list, we are using the initialized name
  // list, since we would need to look up the symbol in case we
  // chnage the arg in a defining declaration
  SgInitializedNamePtrList& args = param_list->get_args();

  SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(isSgNode(param_list)->get_parent());
  ROSE_ASSERT(fn_decl != NULL);




  SgFunctionParameterList* new_param_list = SageBuilder::buildFunctionParameterList();
  SgInitializedNamePtrList& new_args = new_param_list->get_args();

  printf("created empty func params\n");

  // Doing this to ensure that new_args has a parent... this is required when
  // printing out where the node is...
//  new_args->set_parent(isSgNode(fn_decl));
  new_param_list->set_parent(isSgNode(fn_decl));


  SgFunctionDefinition* fndef = fn_decl->get_definition();
  bool defining_decl = (fndef != NULL);


  printf("Traversing parameter list\n");

  for(SgInitializedNamePtrList::iterator iter = args.begin(); iter != args.end(); iter++) {

    // Get the initialize name
    SgInitializedName* arg_name = *iter;

    printf("arg_name: %s = %s\n", isSgNode(arg_name)->sage_class_name(), isSgNode(arg_name)->unparseToString().c_str());

    // Check if this arg is a pointer
    SgType* arg_type = arg_name->get_type();

//    printf("pointer type?");
//    if(isSgPointerType(arg_type)) {
    printf("is qualifying type?");
    if(isQualifyingType(arg_type)) {
      printf("Yes\n");


      // Now, we have to replace this arg with a VoidStruct version of it
      SgInitializedName* new_arg_name = createStructVariableFor(arg_name);
//      new_args->append_arg(new_arg_name);
//      new_arg_name->set_parent(new_args);
//      new_args.push_back(new_arg_name);
      new_arg_name->set_parent(arg_name->get_parent());
      new_param_list->append_arg(new_arg_name);

      // FIXME: If its not a forward declaration, we would need to make
      // further changes -- like create a local version of the original argument
      // and then copy over the pointer data from VoidStruct and update
      // VoidStruct metadata
      if(defining_decl) {
        printf("Defining decl\n");
//        createLocalVariableAndUpdateMetadata(new_arg_name, arg_name, fn_decl);
        // Replace all uses of the original arg_name with the new_arg_name
        // This creates a symbol for new_arg_name, and creates the necessary
        // mappings
        // By default, handleSymbolTable creates a mapping. But, just to be sure..
        // insert true at the end of the arg list -- addToMap = true
        handleSymbolTable(arg_name, new_arg_name, true);

        ROSE_ASSERT(new_arg_name->get_symbol_from_symbol_table() != NULL);

        // Things to do: 1. create local variable
        // 2. create entry at address given by local variable.
        // 3. initialize this variable with local variable address
        // FIXME: 4. copy over any initialization from orig variable, and updatePointer afterwards
        // also add new struct variable at the end... since return for the whole expression
        // has to be the new struct variable

        //createLocalVariableAndUpdateMetadata(new_arg_name, arg_name, fn_decl);

        // CHANGES FROM HERE ----

        // The changes are as follows:
        // 1. Instead of creating a local and then transfering metadata to its address in the TrackingDB,
        // we use the struct variable itself... and the address of str.ptr to keep a local copy of
        // the metadata
        // 2. We create a mapping from the original parameter (arg_name) and the str variable. Whenever
        // we encounter the use of the arg_name in handlePointerVarRefs/handleArrayVarRefs, we will
        // replace it with the str variable. No need to create a struct variable at that point. We already
        // have that.
        // 3. overloaded ops will have an update pointer at the end... in cases of ++/-- and assign.
        // This can be left as is, since it will only cause an update of hte pointer to itself...
        // *str.addr = str.ptr is harmless, and in this case, its equal to str.ptr = str.ptr since
        // str.addr = &str.ptr !

        // arg_name is used to update the symbol table mapping in varRemap
        updateMetaDataAndReassignAddr(new_arg_name, arg_name, fn_decl);

      }

    }
    #ifdef CLASS_STRUCT_COPY
    else if(isOriginalClassType(arg_type)) {
      printf("Original Struct/Class object copy\n");

      // Here we have a struct var being passed to a function. We need to convert it to
      // a ref... and then create a local variable within the function, which takes the value of this
      // ref... That variable will be fed to ClassStructVarRemap so that it can replace this original
      // variable. Also, the whole point of this operation is to copy over the internal pointers
      // within the ref to the local variable newly created...

      // Now, replace this arg with a ref version of it...
      SgInitializedName* new_arg_name = createRefVersionFor(arg_name);
      new_arg_name->set_parent(arg_name->get_parent());
      new_param_list->append_arg(new_arg_name);

      if(defining_decl) {

        #if 1
        // Symbol tables only exist for defining decls... if I am not wrong..
        // Add this new arg to the symbol table
        SgSymbol* arg_symbol = arg_name->get_symbol_from_symbol_table();
        SgVariableSymbol *arg_var_symbol = isSgVariableSymbol(arg_symbol);
        printf("Got scope. Now, symbolTable\n");
        #endif

        addToSymbolTable(new_arg_name, isSgNode(arg_var_symbol)->get_parent());


        // Here, we do the assigning from ref to the local variable... and then the ClassStructVarRemap
        // and finally, the element to element copy

        SgBasicBlock* bb = getBBForFn(fn_decl);
        // Assign ref to local variable
        SgVariableDeclaration* local_var = assignRefToLocalVar(new_arg_name, fn_decl, isSgScopeStatement(bb));


        insertAtTopOfBB(bb, local_var);

        // Element to Element copy -- from the new pointer arg, to the local_var
        SgExpression* copyExp = copyElementMetadata(new_arg_name, getNameForDecl(local_var), fn_decl);
        insertStatementAfter(local_var, buildExprStatement(copyExp));

        // Add it to the ClassStructVarRemap
        addToSymbolTableAndClassRemap(arg_name, getNameForDecl(local_var));

      }
    }
    #endif
    else {
      printf("No\n");
      SgInitializedName* new_arg_name = isSgInitializedName(SageInterface::deepCopyNode(isSgNode(arg_name)));

//      new_args->append_arg(new_arg_name);
//      new_arg_name->set_parent(new_args);
//      new_args.push_back(new_arg_name);
      new_arg_name->set_parent(arg_name->get_parent());
      new_param_list->append_arg(new_arg_name);


      // Nothing else to do in this case
    }
  }

  // We hit this parameter list before we change the function decl, so we can check if the
  // fn_decl has a OriginalClassType return type, and make the necessary changes...
  #ifdef CLASS_STRUCT_COPY
  bool modifiedArgSize = false;
  if(isOriginalClassType(fn_decl->get_type()->get_return_type())) {
    modifiedArgSize = true;
    // We need to add a ref variable to the end of arg list...
    // If its a defining decl, we also need to replace all return statements
    // with assignments, following by return void stmts. The return type will
    // be changed to void in handleFuncDecls.
    // original code: class_type fn1(....)
    // transformed code: class_type fn1(...., class_type *retvar)


    std::string new_name_string;
    if(!defining_decl) {
      new_name_string = "";
    }
    else {
      new_name_string = "retvar" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++);
    }
    SgName new_name(new_name_string);

    SgType* ret_type = fn_decl->get_type()->get_return_type();

    SgType* new_arg_type = SgPointerType::createType(ret_type);

    // Using param_list as scope. Shoudl be okay I guess...
    SgInitializedName* new_arg_name = buildInitializedName(new_name, new_arg_type, NULL);

    new_arg_name->set_scope(getScope(param_list));

    new_arg_name->set_parent(param_list);
    new_param_list->append_arg(new_arg_name);

    if(defining_decl) {

      // Symbol tables only exist for defining decls... if I am not wrong..
      // Add this new arg to the symbol table
      SgScopeStatement* scope = getScope(param_list);
      SgSymbolTable* symtab = scope->get_symbol_table();

      addToSymbolTable(new_arg_name, symtab);


      // We change return stmts in handleReturnStmts. This ensures that all the modifications on
      // the expression in the ret stmt take place as usual.
      #if 0
      // Now change all the return statements to return void, and do an assignment instead.
      // Get hold of all the return stmts
      Rose_STL_Container <SgNode*> stmts = NodeQuery::querySubTree(fn_decl, V_SgReturnStmt);

      Rose_STL_Container<SgNode*>::iterator i;
      for(i = stmts.begin(); i != stmts.end(); i++) {
        SgReturnStmt* cur_stmt = isSgReturnStmt(*i);
        ROSE_ASSERT(cur_stmt);
        // paranoid checks...
        ROSE_ASSERT(isSgReturnStmt(cur_stmt));
        ROSE_ASSERT(isSgReturnStmt(cur_stmt)->get_expression()->get_type() == fn_decl->get_type()->get_return_type());

        // this copies the expression in the return statement and uses it as rhs of the assign op
        // *retvar = ret_exp;
        SgAssignOp* assign_op = buildAssignOp(buildPointerDerefExp(buildVarRefExp(new_arg_name)), copyExpression(isSgReturnStmt(cur_stmt)->get_expression()));
        SgExprStatement* assign_stmt = buildExprStatement(assign_op);

        SgStatement* retstmt = buildReturnStmt();

        // original: return ret_exp
        // transformed: *retvar = ret_exp
        //              return
        replaceStatement(cur_stmt, assign_stmt);
        insertStatementAfter(assign_stmt, retstmt);
        // Need to do this in a separate function for return stmts...
        // It can't be done here since the expression hasn't yet been modified, and whatever modifications need
        // to take place on the expression, haven't yet taken place. So, its best if we catch these return stmts
        // in TopBotTrack2 and handle the return stmts like we handle assign ops, and other such stmts.
        // Right now, the bug is: *retvar = copy_this, instead of *retvar = copy_this_local
      }
      #endif
    }


  }
  #endif



//  SageInterface::deepDelete(isSgNode(param_list));

  printf("fn_decl\n");
  printNode(fn_decl);

  printf("size: new_args: %d, args: %d\n", new_args.size(), args.size());

  #ifdef CLASS_STRUCT_COPY
  // We shouldn't add more than 1 in any case
  if(modifiedArgSize) {
    ROSE_ASSERT(new_args.size() == args.size() + 1);
  }
  else {
    ROSE_ASSERT(new_args.size() == args.size());
  }
  #else
  ROSE_ASSERT(new_args.size() == args.size());
  #endif

//  new_args->set_parent(isSgNode(fn_decl));

//  fn_decl->set_parameterList(new_args);
  fn_decl->set_parameterList(new_param_list);

}

bool ArithCheck::ArithOrAssignOp(SgNode* node) {

  if(ArithOpWithSelfUpdate(node)) {
    return true;
  }

  // These are arith ops which don't change
  // the original variable
  switch(node->variantT()) {
  case V_SgAddOp:
  case V_SgSubtractOp: return true;
  default: return false;
  }
}

// These arith ops change the original variable as well
bool ArithCheck::ArithOpWithSelfUpdate(SgNode* node) {
  switch(node->variantT()) {
  case V_SgPlusPlusOp:
  case V_SgMinusMinusOp:
  case V_SgPlusAssignOp:
  case V_SgMinusAssignOp:
  case V_SgAssignOp: return true;
  default: return false;
  }
}

// Not updated to changes -- don't use this...
#if 0
SgExpression* ArithCheck::buildOverloadFnForOp(SgName fn_name, SgExpression* Op, SgType* retType, SgExpression* extraArg) {

  SgExprListExp* arguments;

  if(SgBinaryOp* bop = isSgBinaryOp(Op)) {

    SgExpression* LHS = bop->get_lhs_operand();
    SgExpression* RHS = bop->get_rhs_operand();

    SgExpression* LHS_Copy = SageInterface::copyExpression(LHS);
    SgExpression* RHS_Copy = SageInterface::copyExpression(RHS);

    arguments = SageBuilder::buildExprListExp(LHS_Copy, RHS_Copy);
  }
  else if(SgUnaryOp* uop = isSgUnaryOp(Op)) {
    SgExpression* operand = uop->get_operand();

    SgExpression* operand_copy = SageInterface::copyExpression(operand);

    arguments = SageBuilder::buildExprListExp(operand_copy);
  }
  else {
    printf("Unsupported Op\n");
    ROSE_ASSERT(0);
  }

  if(extraArg != NULL) {
      appendExpression(arguments, extraArg);
  }

  SgScopeStatement* scope = getScopeForExp(Op);


  SgExpression* NewFunction = SageBuilder::buildFunctionCallExp(fn_name,
                                                                retType,
                                                                arguments,
                                                                scope);

  createForwardDeclIfNotAvailable(fn_name, retType, arguments, scope, ArithCheck::ACProject);

  return NewFunction;
}
#endif

// FIXME: In order to generate the sizeof argument properly, it is
// is critical that we know the base type we are refering to --
// or atleast, the offset for the base size. In fact, in order to
// correctly cast any pointer to another, we would need to know the
// exact type, but since we always use VoidStructs, we don't have that
// problem at the moment.
// Since we do not have that base type available to us, at this point
// of time, we need to find a way to atleast get the size of that
// base type, which would be used as a factor when changing the pointer
// size
// For now, we always increment by sizeof(char)
SgExpression* ArithCheck::createSizeOfArg(SgNode* node) {

  // Defaulting to sizeof(int) for now.
  SgExpression* sizeofArg = SageBuilder::buildSizeOfOp(SgTypeInt::createType());


  return sizeofArg;

}

SgVariableSymbol* ArithCheck::findSymbolForVarRef(SgVarRefExp* exp) {

  if(exp == NULL) {
    ROSE_ASSERT(0);
  }

  return exp->get_symbol();
}

SgVariableSymbol* ArithCheck::findInReverseMap(SgVariableSymbol* var_symbol) {


  VariableSymbolMap_t::iterator iter = ArithCheck::ReverseMap.find(var_symbol);
  ROSE_ASSERT(iter != ArithCheck::ReverseMap.end());
  SgVariableSymbol* str_symbol = iter->second;

  return str_symbol;
}

SgInitializedName* ArithCheck::getNameForSymbol(SgVariableSymbol* var_symbol) {

  return var_symbol->get_declaration();
}

std::string ArithCheck::getStringForFn(std::string fn_name, SgType* arg1, SgType* arg2, SgType* arg3,
                    SgType* retType) {

  SgName arg_name;
  if(arg1 != NULL) {
    arg_name = getNameForType(arg1);
    fn_name += "_" + arg_name + "_Arg_";
  }

  if(arg2 != NULL) {
    arg_name = getNameForType(arg2);
    fn_name += "_" + arg_name + "_Arg_";
  }

  if(arg3 != NULL) {
    arg_name = getNameForType(arg3);
    fn_name += "_" + arg_name + "_Arg_";
  }

  if(retType != NULL) {
    arg_name = getNameForType(retType);
    fn_name += "_" + arg_name + "_Ret_";
  }

  return fn_name;
}

std::string ArithCheck::getStringForOp(SgNode* node, SgType* retType) {

  std::string fn_name_String = getStringForVariantT(node->variantT());

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

SgType* ArithCheck::getAddrType() {
  SgType* addr_type = SgTypeUnsignedLongLong::createType();
  return addr_type;
}

SgExpression* ArithCheck::castToAddr(SgExpression* exp) {
//  SgExpression* cast_exp = buildCastExp(exp, getAddrType(), SgCastExp::e_reinterpret_cast);
  SgExpression* cast_exp = buildCastExp(exp, getAddrType(), CAST_TYPE);
  return cast_exp;
}


SgExpression* ArithCheck::handleNormalVarAddressOfOp(SgAddressOfOp* aop) {

  SgUnaryOp* uop =isSgUnaryOp(aop);

  SgExpression* oper = uop->get_operand();

  ROSE_ASSERT(isSgVarRefExp(oper));
  // The lvalue for an operand to the addressofop should be false.
  // since the expression's address is being taken, its not being changed
  // in the process.
  ROSE_ASSERT(oper->get_lvalue() == false);

  // We need the address of this variable -- so just copy this op itself!
//  SgExpression* aop_copy = copyExpression(aop);

  SgExpression* aop_copy = createAddressOfOpFor(isSgVarRefExp(copyExpression(oper)));

  // Need to cast it to address type -- and thats our first parameter
  SgExpression* lhs = castToAddr(aop_copy);

  // Second parameter is the sizeof the variable type
  //SgExpression* rhs = buildSizeOfOp(oper->get_type());
  // sizeof works on variable and type. sizeof(variable) should give correct
  // responses for arrays and normal stack variables.
  SgExpression* rhs = buildSizeOfOp(copyExpression(oper));

  // Return type is the struct type of the address of return type
  // create one if necessary
  // We effectively create a struct variable of this type, which
  // will hold the address of this variable. This is similar
  // to a cast in that sense, since we create a struct variable in the
  // overloaded function --- but its unlike a cast function, in
  // that we are providing the limits, explicitly as arguments
  // to the address of function
  // This operand is not of a pointer -- convert it to a pointer
  // before sending it in, since getStructType expects a pointer type
  SgType* retType = getStructType(SgPointerType::createType(oper->get_type()),findInsertLocation(getEnclosingStatement(aop)) , true);

//  SgExpression* overload = buildOverloadFn("AddressOf", lhs, rhs, retType, getScopeForExp(aop));
  SgExpression* overload = buildOverloadFn("AddressOf", lhs, rhs, retType, getScopeForExp(aop), findInsertLocation(getEnclosingStatement(aop)));

  return overload;
}

// RTED_Example:c_G_1_1_b.c
void ArithCheck::handleStructVarAddressOfOp(SgAddressOfOp* aop) {

  SgExpression* oper = isSgUnaryOp(aop)->get_operand();
  ROSE_ASSERT(isValidStructType(oper->get_type()));

  // the address that we need is str.addr... this is the address that
  // the pointer will point to...
  // the bounds will be size(str.ptr)
  // So, we create... from &(str)
  // new_str.ptr = str.addr
  // new_str.addr = &new_str.ptr -- we are taking the address of a local pointer here...
  // so, instead we pass a local temp variable in...
  // addr(&temp, str), temp --- so taht we return the temp var

  SgName s_name(oper->get_type()->get_mangled() + "_Var_addr_" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));

  // The struct type for the return type of the address op.
  // Find the underlying type for the struct operand... and then, create a pointer of that type... and
  // a struct of that again...
  SgType* str_type = findInUnderlyingType(oper->get_type());
  SgType* ptr_str_type = SgPointerType::createType(str_type);
  SgType* var_type = getStructType(ptr_str_type, findInsertLocation(getEnclosingStatement(aop)), true);

  SgVariableDeclaration* local_var_decl = createLocalVariable(s_name, var_type, NULL, getScopeForExp(aop));

  SgStatement* stmt = getSuitablePrevStmt(getEnclosingStatement(aop));
  insertStatementBefore(stmt, local_var_decl);

  // Now, for the addressof function...
  // addr(&temp, str)
  SgExpression* var_addr = createAddressOfOpFor(buildVarRefExp(local_var_decl));
  SgExpression* str = copyExpression(oper);

//  SgExpression* overload = buildOverloadFn("AddressOf", var_addr, str, SgTypeVoid::createType(), getScopeForExp(aop));
  SgExpression* overload = buildOverloadFn("AddressOf", var_addr, str, SgTypeVoid::createType(), getScopeForExp(aop), findInsertLocation(getEnclosingStatement(aop)));

  // final ref to temp
  SgExpression* var_ref = buildVarRefExp(local_var_decl);

  // addr(&temp, str), temp
  SgExpression* comma_op = buildCommaOpExp(overload, var_ref);

  replaceExpression(aop, comma_op);

  return;
}

void ArithCheck::handleAddressOfOp(SgAddressOfOp* aop) {

  SgUnaryOp* uop =isSgUnaryOp(aop);

  SgExpression* oper = uop->get_operand();

  if(isSgVarRefExp(oper)) {
    // Its a variable ref exp -- check if its of struct type
    // or its a normal stack variable

    // Of course, it can't be a pointer!
    ROSE_ASSERT(!isSgPointerType(oper->get_type()));

    if(!isValidStructType(oper->get_type())) {
      // If its not a struct type variable, its a normal (global or stack
      // variable
      // Simplest case
      // global or stack variable
      // take its address and size and create an entry. pass that entry back
      // this is a particularly interesting case since we don't know
      // where to create the entry. we'll make up a variable within
      // the overloaded function, and pass that variable and its
      // address back.
      // This is unlike previous cases where the address of the variable
      // is supplied to create_entry

      SgExpression* replacement = handleNormalVarAddressOfOp(aop);
      replaceExpression(aop, replacement);
    }
    else {
      // Its a struct type variable
      // In effect, we are taking the address of a pointer -- creating
      // a double pointer
      handleStructVarAddressOfOp(aop);
    }

  }
  else {

    if(isValidStructType(oper->get_type())) {
      handleStructVarAddressOfOp(aop);
    }

    // Taking the address of an arbitrary location --
    // We could detect the metadata for the expression
    // if its struct type, and use it to create a struct
    // which uses the old metadata, and points to the
    // address given by this op
    // i.e. TrackingDB[address given by aop] = TrackingDB[expression's metadata location]
    // but this won't be correct since by taking the address
    // we are no longer bound by the limits placed by
    // previous metadata
    // In effect, this boils down to the problem of double
    // pointers -- or something there of
  }

}

SgExpression* ArithCheck::createBoundsCheckFn(SgInitializedName* str_array_name, SgExpression* index_exp, SgScopeStatement* scope) {

  SgExpression* str_addr = createDotExpFor(str_array_name, "addr");

//  SgExpression* fncall = buildOverloadFn("bounds_check", str_addr, index_exp, SgTypeVoid::createType(), scope);
  SgExpression* fncall = buildOverloadFn("bounds_check", str_addr, index_exp, SgTypeVoid::createType(), scope, str_array_name);

  return fncall;

}

SgExpression* ArithCheck::buildPntrArrRefOverload(SgPntrArrRefExp* array_ref, SgType* retType, SgExpression* new_lhs, SgExpression* new_rhs) {

  printf("buildPntrArrRefExpOverload\n");
  // use the array to find the return type, scope and name
  //SgType* retType = array_ref->get_type();
  //SgType* retType = isSgArrayType(array_type)->get_base_type();

  printf("retType\n");
  printNode(retType);

  // We could use the original expression's types to make up the
  // function name -- we retain the types from the original
  // we simply change the rhs operand to use a var ref, rather
  // than the earlier expression
  SgName fn_name(getStringForOp(array_ref, retType));

  SgScopeStatement* scope = getScopeForExp(array_ref);

  // use the new_lhs and new_rhs as operands
  #ifdef OLD_NAME_GEN
//  SgExpression* fncall = buildOverloadFn(fn_name, new_lhs, new_rhs, retType, scope);
  SgExpression* fncall = buildOverloadFn(fn_name, new_lhs, new_rhs, retType, scope, array_ref);
  #else
//  SgExpression* fncall = buildOverloadFn("PntrArrRef", new_lhs, new_rhs, retType, scope);
  SgExpression* fncall = buildOverloadFn("PntrArrRef", new_lhs, new_rhs, retType, scope, array_ref);
  #endif

  return fncall;
}

#if 1
// This version tries to cast the pointer to the relevant array type, and then
// do a pntr arr ref. But the standard doesn't like to convert from pointers to
// array types, although the other way round is allowed. This seems to stem from the
// "second class citizen" treatment of arrays.
#if 0
void ArithCheck::handlePntrArrRefExp(SgPntrArrRefExp* array_ref) {

  printf("handlePntrArrRefExp\n");

  // original
  // lhs[rhs]
  // decl _rhs_arr_
  // _rhs_arr_ = rhs, bound_check, create_struct(casted_lhs_deref[_rhs_arr_], &casted_lhs_deref[_rhs_arr_])

  SgExpression* lhs = array_ref->get_lhs_operand();
  ROSE_ASSERT(isValidStructType(lhs->get_type()));

  SgType* lhs_orig_type = findInOriginalVarType(lhs);

  SgExpression* lhs_deref = createDerefOvlForStruct(lhs);

  SgExpression* casted_lhs_deref = buildCastExp(lhs_deref, lhs_orig_type, SgCastExp::e_C_style_cast);

  // Copy rhs to a variable
  SgExpression* rhs = array_ref->get_rhs_operand();
  SgName s_name(rhs->get_type()->get_mangled() + "_rhs_arr_" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));
  SgVariableDeclaration* rhs_var_decl = createLocalVariable(s_name, rhs->get_type(), NULL, getScopeForExp(array_ref));
  SgStatement* stmt = getSuitablePrevStmt(getEnclosingStatement(array_ref));
  insertStatementBefore(stmt, rhs_var_decl);

  // tempvar
  SgExpression* assign_op = buildAssignOp(buildVarRefExp(rhs_var_decl), copyExpression(rhs));

  // FIXME: Bound check comes here

  // the new pntr arr ref
  SgExpression* new_par = buildPntrArrRefExp(casted_lhs_deref, buildVarRefExp(rhs_var_decl));

  if(isQualifyingType(new_par->get_type())) {
    // create a struct... and create a map from that expr to new_par->get_type()

    SgExpression* ce1 = new_par;
    SgExpression* ce2 = castToAddr(createAddressOfOpFor(copyExpression(new_par)));

    SgType* retType = getStructType(getTypeForPntrArr(new_par->get_type()), findInsertLocation(getEnclosingStatement(array_ref)), true);

    SgExpression* overload = buildOverloadFn("create_struct", ce1, ce2, retType, getScope(array_ref), findInsertLocation(getEnclosingStatement(array_ref)));

    SgExpression* comma_op = buildCommaOpExp(assign_op, overload);

    // We went from the new_par->get_type() to the overloaded function type... which is struct type
    mapExprToOriginalType(overload, new_par->get_type());
    mapExprToOriginalType(comma_op, new_par->get_type());

    replaceExpression(array_ref, comma_op);
  }
  else {
    // we are returning a non-pointer, non-array type...
    // we do: _rhs_arr_ = rhs, casted_lhs_deref[_rhs_arr_]
    SgExpression* comma_op = buildCommaOpExp(assign_op, new_par);

    replaceExpression(array_ref, comma_op);
  }

}
#endif
// This one creates the struct using the create_struct function... which requires that we
// we create a local copy of the array indices, to negate any loss of semantic by their extra use
// in the create_struct (from one use, to two uses).
// Another problem is that in case the original var ref is a pointer, the variable might be an
// argument and hence may not actually be available. This case also arises for a local pointer being used
// with the pntr arr ref. In both cases, we can deref the struct variable, and use it.
#if 0
void ArithCheck::handlePntrArrRefExp(SgPntrArrRefExp* array_ref) {

  printf("handlePntrArrRefExp\n");

  // original
  // lhs[rhs]
  // decl _rhs_arr_
  // _rhs_arr_ = rhs, bound_check, create_struct(original_lhs[_rhs_arr_], &original_lhs[_rhs_arr_])  if we return a pointer/array
  // _rhs_arr_ = rhs, bound_check, original_lhs[_rhs_arr_]

  SgExpression* lhs = array_ref->get_lhs_operand();

  SgExpression* orig_lhs = findInOriginalVarRef(lhs);

  printf("lhs:\n");
  printNode(lhs);
  printf("key: %llu\n", (unsigned long long)lhs);
  printf("orig_lhs:\n");
  printNode(orig_lhs);

  // Copy rhs to a variable
  SgExpression* rhs = array_ref->get_rhs_operand();
  SgName s_name(rhs->get_type()->get_mangled() + "_rhs_arr_" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));
  SgVariableDeclaration* rhs_var_decl = createLocalVariable(s_name, rhs->get_type(), NULL, getScopeForExp(array_ref));
  SgStatement* stmt = getSuitablePrevStmt(getEnclosingStatement(array_ref));
  insertStatementBefore(stmt, rhs_var_decl);

  // tempvar
  SgExpression* assign_op = buildAssignOp(buildVarRefExp(rhs_var_decl), copyExpression(rhs));
  insertStatementBefore(getSuitablePrevStmt(getEnclosingStatement(array_ref)), buildExprStatement(assign_op));

  // FIXME: Bound check comes here

  // the new pntr arr ref
  SgExpression* new_par = buildPntrArrRefExp(copyExpression(orig_lhs), buildVarRefExp(rhs_var_decl));

  if(isQualifyingType(new_par->get_type())) {
    // create a struct... and create a map from that expr to new_par

    // This casts the array variable into a normal integer pointer variable... this makes sure that
    // we don't need the array indices to be available outside of their original declaration
    SgExpression* ce1 = buildCastExp(new_par, getTypeForPntrArr(new_par->get_type()), SgCastExp::e_C_style_cast);
    SgExpression* ce2 = castToAddr(createAddressOfOpFor(copyExpression(new_par)));

    SgType* retType = getStructType(getTypeForPntrArr(new_par->get_type()), findInsertLocation(getEnclosingStatement(array_ref)), true);

    SgExpression* overload = buildOverloadFn("create_struct", ce1, ce2, retType, getScope(array_ref), findInsertLocation(getEnclosingStatement(array_ref)));

//    SgExpression* comma_op = buildCommaOpExp(assign_op, overload);

    // We went from the new_par->get_type() to the overloaded function type... which is struct type
//    mapExprToOriginalType(overload, new_par->get_type());
//    mapExprToOriginalType(comma_op, new_par->get_type());
    mapExprToOriginalType(overload, new_par->get_type());

//    mapExprToOriginalVarRef(overload, new_par);
//    mapExprToOriginalVarRef(comma_op, new_par);
    printf("handlePntrArrRefExp\n");
    printf("mapping: \n");
    printf("overload\n");
    printNode(overload);
    printf("key: %llu\n", (unsigned long long)overload);
    printf("new_par\n");
    printNode(new_par);
    mapExprToOriginalVarRef(overload, new_par);


//    replaceExpression(array_ref, comma_op);
    replaceExpression(array_ref, overload);
  }
  else {
    // we are returning a non-pointer, non-array type...
    // we do: _rhs_arr_ = rhs, casted_lhs_deref[_rhs_arr_]
//    SgExpression* comma_op = buildCommaOpExp(assign_op, new_par);

//    replaceExpression(array_ref, comma_op);
    replaceExpression(array_ref, new_par);
  }

}
#endif

SgExpression* ArithCheck::skipToOriginalVarRef(SgExpression* replacement) {

  SgExpression* orig = findInOriginalVarRef(replacement);
  if(isValidStructType(orig->get_type())) {
    return skipToOriginalVarRef(replacement);
  }
  return orig;
}

SgExpression* ArithCheck::checkInOriginalVarRef(SgExpression* replacement) {
  ExprMap_t::iterator em;

  em = ArithCheck::OriginalVarRef.find(replacement);

  if(em != ArithCheck::OriginalVarRef.end()) {
    SgExpression* orig = em->second;
    return orig;
  }

  return NULL;
}


void ArithCheck::handlePntrArrRefExp(SgPntrArrRefExp* array_ref) {

  printf("handlePntrArrRefExp\n");

  // original
  // lhs[rhs]
  // transformed... for case of available array variable
  // bound_check, create_struct_using_addr(&original_lhs[rhs])  if we return a pointer/array
  // bound_check, original_lhs[rhs]
  // transformed... for case of pointer/argument
  // bound_check, create_struct_using_addr(deref(lhs)[rhs]) if we return a pointer/array
  // bound_check, deref(lhs)[rhs]

  SgExpression* lhs = array_ref->get_lhs_operand();

//  SgExpression* orig_lhs = skipToOriginalVarRef(lhs);

  SgExpression* orig_lhs = checkInOriginalVarRef(lhs);

  SgExpression* correct_lhs;

  if(orig_lhs == NULL) {
    // this is a pointer... hence, should be a struct by now
    // deref(lhs)
    ROSE_ASSERT(isValidStructType(lhs->get_type()));

    correct_lhs = createDerefOvlForStruct(lhs);
  }
  else {
    // this is an array type... hence, we simply use
    // returned var ref
    correct_lhs = copyExpression(orig_lhs);
  }

  // the new pntr arr ref
  SgExpression* new_par = buildPntrArrRefExp(correct_lhs, copyExpression(array_ref->get_rhs_operand()));


  if(isQualifyingType(new_par->get_type())) {
    // create struct using addr... and create a map from that expr to new_par

    // Lvalue should be set as false since we are using the address
    // here to create a struct. Not writing to this expression at all
    new_par->set_lvalue(false);

    SgExpression* addr_of = castToAddr(createAddressOfOpFor(new_par));

    SgType* retType = getStructType(getTypeForPntrArr(new_par->get_type()), findInsertLocation(getEnclosingStatement(array_ref)), true);

    SgExpression* overload = buildOverloadFn("create_struct_from_addr", addr_of, NULL, retType, getScope(array_ref), findInsertLocation(getEnclosingStatement(array_ref)));

    mapExprToOriginalType(overload, new_par->get_type());

    #ifdef ONLY_ADD_ARRAYS_TO_OVR
    if(isSgArrayType(new_par->get_type())) {
      printf("handlePntrArrRefExp\n");
      printf("mapping: \n");
      printf("overload\n");
      printNode(overload);
      printf("key: %llu\n", (unsigned long long)overload);
      printf("new_par\n");
      printNode(new_par);
      mapExprToOriginalVarRef(overload, new_par);
    }
    #else
    printf("handlePntrArrRefExp\n");
    printf("mapping: \n");
    printf("overload\n");
    printNode(overload);
    printf("key: %llu\n", (unsigned long long)overload);
    printf("new_par\n");
    printNode(new_par);
    mapExprToOriginalVarRef(overload, new_par);
    #endif

    replaceExpression(array_ref, overload);
  }
  else {
    // we are returning a non-pointer, non-array type...
    // we do: _rhs_arr_ = rhs, casted_lhs_deref[_rhs_arr_]
//    SgExpression* comma_op = buildCommaOpExp(assign_op, new_par);

//    replaceExpression(array_ref, comma_op);
    replaceExpression(array_ref, new_par);
  }



}

#if 0
void ArithCheck::handlePntrArrRefExp(SgPntrArrRefExp* array_ref) {


  printf("handlePntrArrRefExp\n");

  // original
  // lhs[rhs]
  // decl tempvar
  // tempvar = rhs, create_struct(pntrarrref(lhs, tempvar*(sizeof(lhs[0])/sizeof(lhs[0])), tempvar < sizeof(lhs)/sizeof(lhs[0])))

  ROSE_ASSERT(isValidStructType(array_ref->get_lhs_operand()->get_type()));

  SgExpression* lhs = isSgBinaryOp(array_ref)->get_lhs_operand();
  SgExpression* rhs = isSgBinaryOp(array_ref)->get_rhs_operand();

  SgName s_name(rhs->get_type()->get_mangled() + "_rhs_arr_" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));
  SgVariableDeclaration* rhs_var_decl = createLocalVariable(s_name, rhs->get_type(), NULL, getScopeForExp(array_ref));
  SgStatement* stmt = getSuitablePrevStmt(getEnclosingStatement(array_ref));
  insertStatementBefore(stmt, rhs_var_decl);

  // tempvar = rhs
  SgExpression* assign_op = buildAssignOp(buildVarRefExp(rhs_var_decl), copyExpression(rhs));

  // pntrarrref(lhs, tempvar*(sizeof(lhs[0])/sizeof(lhs[0])), tempvar < sizeof(lhs)/sizeof(lhs[0]))
  SgExpression* ce1 = copyExpression(lhs);
  // cast ptr of lhs to the base_type of the pntrarrref expression... this will turn into
  // the correct type, once we apply the [0] on it...
  // Deref lhs first -- that should give the pointer value... and cast this pointer
  // to the base_type of the pntrarrref --

  SgType* par_type = array_ref->get_type();
  if(isSgArrayType(par_type)) {
    par_type = isSgArrayType(par_type)->get_base_type();
  }
  else {
    // Nothing to do otherwise, correct?
  }

  SgExpression* lhs_derefed = createDerefOvlForStruct(copyExpression(lhs));

  // tempvar*(sizeof(lhs[0])/sizeof(lhs[0]))
  SgExpression* lhs_ptr_casted = buildCastExp(lhs_derefed, par_type, SgCastExp::e_C_style_cast);
  SgExpression* lhs_0 = buildPntrArrRefExp(lhs_ptr_casted, buildIntVal(0));
  SgExpression* sizeof_lhs_0 = buildSizeOfOp(lhs_0);
  SgExpression* ce2 = buildMultiplyOp(buildVarRefExp(rhs_var_decl), (buildDivideOp(sizeof_lhs_0, copyExpression(sizeof_lhs_0))));

  // tempvar < sizeof(lhs)/sizeof(lhs[0])
  SgExpression* sizeof_lhs = buildSizeOfOp(lhs_ptr_casted);
  SgExpression* ce3 = buildLessThanOp(buildVarRefExp(rhs_var_decl), buildDivideOp(sizeof_lhs, copyExpression(sizeof_lhs_0)));

  SgType* retType = array_ref->get_type();

  // New name gen stuff
  SgExpression* overload = build3ArgOverloadFn(SgName(getStringForVariantT(array_ref->variantT())), ce1, ce2, ce3,
              retType, getScopeForExp(array_ref), findInsertLocation(getEnclosingStatement(array_ref)));

  // Now, we need to create a struct, if the output is a pointer/array type
  SgExpression* create_struct = createStructUsingAddressOf(overload, findInsertLocation(getEnclosingStatement(array_ref)));

  // Build a comma expression
  SgExpression* comma_op = buildCommaOpExp(assign_op, create_struct);

  replaceExpression(array_ref, comma_op);
}
#endif
#else
void ArithCheck::handlePntrArrRefExp(SgPntrArrRefExp* array_ref) {

  printf("handlePntrArrRefExp\n");

  // SgPntrArrRef --- we have str[index]
  // since we always maintain the structs with pointers, and not
  // "array" variables, we have to add the offset to the pointer
  // and return a pointer... We would then build a deref around
  // it, and this can be used as an lvalue or rvalue after that.
  // just like the original ones...
  // Trick here is to always create a struct with pointer inside
  // This we do... so, we treat this like an offset to that pointer
  // variable returned --- and then, derefed.
  // The problem here is that, since we "deref" it, we lose the metadata
  // since we move away from the struct... to possibly a value...
  // And, applying an address of operator on this wouldn't yield the
  // right result. It looks like the pntr arr ref is a pretty complicated
  // expression... and acts very much like a deref expression. The
  // problem here is that we don't know the metadata associated with the
  // array after the deref...
  // Lets handle the address of op, on top of this deref, later... for now,
  // we overload the pntr arr ref, with a function, followed by a deref.

  ROSE_ASSERT(isValidStructType(array_ref->get_lhs_operand()->get_type()));


  SgExpression* lhs = isSgBinaryOp(array_ref)->get_lhs_operand();
  SgExpression* rhs = isSgBinaryOp(array_ref)->get_rhs_operand();

  SgType* ptr_type = findInUnderlyingType(lhs->get_type());

  // We should be returning a pointer... since, this is pointing to
  // a location.
  ROSE_ASSERT(isSgPointerType(ptr_type));

  SgType* retType = ptr_type;
  ROSE_ASSERT(isSgPointerType(retType));

  // The rhs op here needs to be changed, but the bigger question is how
  // changing the underlying base type to pointer of the base type, always...
  // and ripping out the rest of the array types in the middle, changes
  // this setup... we went from using unsigned int* [] for a unsigned int [][]
  // to using unsigned int*
  // This can screw up things... besides adding the "sizeof" stuff to the
  // rhs op, and the bounds check operation.


//  SgName fn_name(getStringForOp(array_ref, retType));
  SgName fn_name(getStringForFn(getStringForVariantT(array_ref->variantT()), lhs->get_type(), rhs->get_type(),
                NULL, retType));

  // FIXME: How do we find the bounds for arrays here? It won't be available in the
  // underlying type... We would need the bounds to do a bounds check.
  // For now, not passing the bounds in...

//  SgScopeStatement* scope = getScopeForVarRef(isSgVarRefExp(array_ref->get_lhs_operand()));
//  SgScopeStatement* scope = getScopeForExp(array_ref);
  SgScopeStatement* scope = getScope(array_ref);
  ROSE_ASSERT(scope != NULL);


  //SgExpression* overload = buildOverloadFn(fn_name, copyExpression(lhs), copyExpression(rhs),
  //            retType, getScopeForExp(array_ref));

  #ifdef OLD_NAME_GEN
//  SgExpression* overload = buildOverloadFn(fn_name, copyExpression(lhs), copyExpression(rhs),
//              retType, scope);
  SgExpression* overload = buildOverloadFn(fn_name, copyExpression(lhs), copyExpression(rhs),
              retType, scope, findInsertLocation(getEnclosingStatement(array_ref)));
  #else
//  SgExpression* overload = buildOverloadFn(SgName(getStringForVariantT(array_ref->variantT())), copyExpression(lhs), copyExpression(rhs),
//              retType, scope);
  SgExpression* overload = buildOverloadFn(SgName(getStringForVariantT(array_ref->variantT())), copyExpression(lhs), copyExpression(rhs),
              retType, scope, findInsertLocation(getEnclosingStatement(array_ref)));
  #endif


  ROSE_ASSERT(overload->get_type() == retType);
  ROSE_ASSERT(isSgPointerType(overload->get_type()));


  // Now, create a deref expr from on top of this overload...
  SgExpression* deref_exp = buildPointerDerefExp(overload);

  // Replace the original array_ref with deref_exp
  replaceExpression(array_ref, deref_exp);

  printf("deref_exp\n");
  printNode(deref_exp);

  printf("deref_exp_parent\n");
  printNode(deref_exp->get_parent());

  // If the output of the deref is a pointer/array, we have to
  // create a struct.
  if(isQualifyingType(deref_exp->get_type())) {
    SgExpression* createfn = createDerefOutputStruct(isSgPointerDerefExp(deref_exp));
    replaceExpression(deref_exp, createfn);
    printf("createfn\n");
    printNode(createfn);

    printf("createfn_parent\n");
    printNode(createfn->get_parent());
  }


  #if 0

  // Get the lhs and rhs operands.
  // lhs should be the array/ptr variable -- so, it should now be a struct
  // variable, and rhs is an expression.
  SgExpression* lhs = isSgBinaryOp(array_ref)->get_lhs_operand();
  SgExpression* rhs = isSgBinaryOp(array_ref)->get_rhs_operand();

  // Assert that the base type on the lhs has been converted to a struct type
  ROSE_ASSERT(isValidStructType(lhs->get_type()));
  // and its refering to an array
  //ROSE_ASSERT(isSgVarRefExp(lhs)); -- It could be the output of a create struct
  // since no longer have references to struct variables, but rather
  // create_struct is called in their place.

  // We don't need to copy the index var since its already being passed into the
  // pntr arr ref overload function...
  #if 0
  SgVariableDeclaration* index_var_decl = pushIndexToLocalVar("index_" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++),
                                rhs,
                                getScopeForExp(array_ref));

  insertStatementBefore(getEnclosingStatement(array_ref), index_var_decl);

  printf("index_var_decl\n");
  printNode(index_var_decl);

  // copy lhs.
  SgExpression* new_lhs = copyExpression(lhs);
  // build rhs
  SgExpression* new_rhs = buildVarRefExp(index_var_decl);
  #endif

  // copy lhs.
  SgExpression* new_lhs = copyExpression(lhs);
  // copy rhs -- since there is only one use of the earlier RHS
  SgExpression* new_rhs = copyExpression(rhs);

  // We have to find the return type... which we can do, by
  // actually finding the underlying type for the struct, and stripping
  // the pointer type in that.
  SgType* ptr_type = getUnderlyingTypeForStruct(lhs->get_type());
  // Now, we have a pointer type -- assert that.
  ROSE_ASSERT(isSgPointerType(ptr_type));
  // This base type is the one that is the returned.
  // Remember that when we create the struct, we do the opposite, we start with the
  // base type, and wrap a pointer around it.
  SgType* base_type = isSgPointerType(ptr_type)->get_base_type();

  SgExpression* new_array_ref = buildPntrArrRefOverload(array_ref, base_type, new_lhs, new_rhs);

  // Now, the bounds check can be done in the pntr_arr_ref_overload function itself... so lets not
  // worry about it for now.

  // Lets replace the array_ref with new_array_ref.
  replaceExpression(array_ref, new_array_ref);

  // new_array_ref->get_type() should be equal to base_type
  ROSE_ASSERT(new_array_ref->get_type()->get_mangled() == base_type->get_mangled());
  if(isQualifyingType(new_array_ref->get_type())) {
    printf("PntrArrRefExp is returning a pointer/array type. Create a struct using addr\n");
    SgExpression* createfn = createStructUsingAddressOf(new_array_ref);
    // Here, we replace new_array_ref with createfn
    replaceExpression(new_array_ref, createfn);
  }

  #if 0
  // Find the original variable for this
  // Find symbol for var_ref operand
  #if 0
  SgVariableSymbol* var_symbol = findSymbolForVarRef(isSgVarRefExp(lhs));

  ROSE_ASSERT(isValidStructType(isSgSymbol(var_symbol)->get_type()));

  SgVariableSymbol* orig_var_symbol = findInReverseMap(var_symbol);

  SgInitializedName* orig_name = getNameForSymbol(orig_var_symbol);

  #endif
  // Now, we have to create a local variable to hold the index var, as we
  // did in the handleArrayVars.
  // That index variable will be used to both do a bounds check, and then
  // in the array ref.
  // It is because of the two uses, that we have to create a local variable.
  SgType* array_type = orig_name->get_type();
  // Now, assert that its of ArrayType
  ROSE_ASSERT(isSgArrayType(array_type));

  SgVariableDeclaration* index_var_decl = pushIndexToLocalVar(orig_name->get_name(), rhs, getScopeForExp(array_ref));

  // Insert this calculate as the previous stmt -- previous stmt because its a variable declaration
  // and can't be in a comma op
  insertStatementBefore(getEnclosingStatement(array_ref), index_var_decl);

  printf("index_var_decl\n");
  printNode(index_var_decl);

  // Now, push index_var ref in place of rhs -- instead of swapping things, we take a slightly different
  // route here.
  // We create a new op -- we anyway remove the current one later, and create a comma op with bounds check

  // copy lhs.
  SgExpression* new_lhs = copyExpression(lhs);
  // build rhs
  SgExpression* new_rhs = buildVarRefExp(index_var_decl);

//  SgPntrArrRefExp* new_array_ref = buildPntrArrRefExp(new_lhs, new_rhs);
  SgExpression* new_array_ref = buildPntrArrRefOverload(array_ref, array_type, new_lhs, new_rhs);

  // Now, for the bounds check function
  // This function only requires the address of the entry, and the current index.
  // It can add the current index to the lower bounds found at TrackingDB[addr].L
  // and do the bounds check.

  SgExpression* bound_check = createBoundsCheckFn(getNameForSymbol(var_symbol), buildVarRefExp(index_var_decl), getScopeForExp(array_ref));

  // lets create a comma op with the new_array_ref and bounds_check.. and
  // replace the original expression with this one.
  // By keeping the array at the rhs, we automatically ensure its the value passed back
  SgCommaOpExp* comma_op = buildCommaOpExp(bound_check, new_array_ref);

  replaceExpression(array_ref, comma_op);

  // If we are returning a pointer/array type from the comma op,
  // we need to take the address, and create a struct from that -- just like deref.
  if(isQualifyingType(comma_op->get_type())) {
    printf("PntrArrRefExp is returning a pointer/array type. Create a struct using addr\n");
    SgExpression* createfn = createStructUsingAddressOf(comma_op);

    replaceExpression(comma_op, createfn);
  }
  #endif
  #endif
}
#endif

SgExpression* ArithCheck::handleCastExp(SgCastExp* cast_exp) {

  printf("handleCastExp\n");
  // The Cast exp now has actual meaning... it changes from one struct variable
  // to another...
  // The operand for the cast exp could be of pointer (now Struct) type
  // or other long int type (like NULL). We need to handle the latter as
  // Cast_Value cases

  SgExpression* oper = isSgUnaryOp(cast_exp)->get_operand();
  SgExpression* oper_copy = copyExpression(oper);
  // The argument to a cast expression shouldn't be changed in the
  // instrumentation below. We would return a different struct
  // in place of the current one, and not change the current one
  // in the process. So, lvalue should be set to false.
  oper_copy->set_lvalue(false);
  ROSE_ASSERT(oper_copy->get_lvalue() == false);

  // We might be casting to a type for which we haven't created an
  // entry yet (we create entries when we see variable declarations
  // and parameter lists)

  // There are three types of casts:
  // 1. struct to struct
  // 2. struct to value
  // 3. value to struct

  SgType* retType;
  std::string fn_name_String;
  if(isQualifyingType(cast_exp->get_type())) {
    retType = getStructType(getTypeForPntrArr(cast_exp->get_type()), findInsertLocation(getEnclosingStatement(cast_exp)), true);
    fn_name_String = getStringForOp(cast_exp, retType) + "_PntrArr";
  }
  else {
    retType = cast_exp->get_type();
    fn_name_String = getStringForOp(cast_exp, retType) + "_ValRet";
  }

  printf("cast_exp: %s = %s\n", isSgNode(cast_exp->get_type())->sage_class_name(), isSgNode(cast_exp->get_type())->unparseToString().c_str());
  printf("retType: %s = %s\n", isSgNode(retType)->sage_class_name(), isSgNode(retType)->unparseToString().c_str());

  if(isSgValueExp(oper)) {
     fn_name_String += "_ValCast";
  }

  SgName fn_name(fn_name_String);
  // RHS operand is NULL
  #ifdef OLD_NAME_GEN
//  SgExpression* overload = buildOverloadFn(fn_name, oper_copy, NULL, retType, getScopeForExp(cast_exp));
  SgExpression* overload = buildOverloadFn(fn_name, oper_copy, NULL, retType, getScopeForExp(cast_exp), findInsertLocation(getEnclosingStatement(cast_exp)));
  #else
//  SgExpression* overload = buildOverloadFn(SgName(getStringForVariantT(cast_exp->variantT())), oper_copy, NULL, retType, getScopeForExp(cast_exp));
  SgExpression* overload = buildOverloadFn(SgName(getStringForVariantT(cast_exp->variantT())), oper_copy, NULL, retType, getScopeForExp(cast_exp), findInsertLocation(getEnclosingStatement(cast_exp)));
  #endif

  replaceExpression(cast_exp, overload);

  ROSE_ASSERT(overload->get_type() == retType);
  printf("handleCastExp - Done\n");

  return overload;
}

SgExpression* ArithCheck::updatePointerUsingAddr(SgVarRefExp* var_ref) {

  SgVarRefExp* var_ref_copy = isSgVarRefExp(copyExpression(var_ref));

  ROSE_ASSERT(isValidStructType(var_ref_copy->get_type()));
  // Basically, this function uses the address in the struct, to update
  // the original pointer.

  SgVariableSymbol* var_symbol = var_ref_copy->get_symbol();

  // str.ptr
  SgExpression* ptr_exp = createDotExpFor(getNameForSymbol(var_symbol), "ptr");

  // str.addr
  SgExpression* addr_exp = createDotExpFor(getNameForSymbol(var_symbol), "addr");
  // Cast the addr to the correct type -- i.e pointer of the ptr type
  //SgExpression* addr_dbl_ptr_exp = buildCastExp(addr_exp, SgPointerType::createType(ptr_exp->get_type()), SgCastExp::e_reinterpret_cast);
  SgExpression* addr_dbl_ptr_exp = buildCastExp(addr_exp, SgPointerType::createType(ptr_exp->get_type()), CAST_TYPE);


  // First deref the addr
  // *(reinterpret_cast<ptr(ptr_type)>str.addr)
  SgExpression* deref_exp = buildPointerDerefExp(addr_dbl_ptr_exp);
  // This final cast seems to create the error "invalid lvalue in assignment"
//  SgExpression* casted_addr = buildCastExp(deref_exp, getUnderlyingTypeForStruct(var_ref->get_type()), SgCastExp::e_C_style_cast);


  // *(reinterpret_cast<ptr(ptr_type)>str.addr) = str.ptr
//  SgExpression* update = buildAssignOp(casted_addr, ptr_exp);
  SgExpression* update = buildAssignOp(deref_exp, ptr_exp);

  return update;

}

void ArithCheck::handleOverloadOps(SgNode* node) {
  // The operations listed here would be
  //AddOp:
  //SubtractOp:
  //PlusPlusOp:
  //MinusMinusOp:
  //CastExp:
  //AssignOp:
  //PlusAssignOp:
  //MinusAssignOp:

  // For add and subtract ops, replace operation with overload functions

  // For plusplus and minusminus op, replace operation with overload functions
  // and update the original ptr

  // For cast exp, if the return type is not pointer type, insert a
  // deref overload with a cast to the required type.
  // If the return type is pointer type, overload with Cast_Overload

  // For assign op, replace operation with overload functions and
  // update the original ptr


  // We could update the add/subtract as well, so that we handle
  // add, subtract, plusplus, minusminus and assign in the same
  // fashion -- but this is too much work when it comes to add
  // and subtract, since a new variable would need to be created
  // instead, we'll only update for assign, plusplus and minusminus
  // -- where its absolutely required

  // Add, subtract, plusplus, minusminus, assign
  if(ArithOrAssignOp(node)) {



    #if 0
    SgType* retType = getStructType(getVoidPointerType());
    #endif
    // Return type is that given by the op itself since we
    // have different structs for different types
    SgType* retType = isSgExpression(node)->get_type();

    SgName fn_name(getStringForOp(node, retType));

    SgExpression* overload;
    SgExpression* sizeofArg = NULL;

    // SizeofArg is not required since we always work
    // with the correct type of pointers --
    #if 0
    if(!isSgAssignOp(node)) {
      sizeofArg = createSizeOfArg(node);
    }
    #endif

    //overload = buildOverloadFnForOp(fn_name, isSgExpression(node), retType, sizeofArg);

    // If its a plusplus, or minusminus -- update the pointer
    // If the underlying operand is a var-ref, then we should find the original
    // pointer, and use updatePointer --- and then, return the struct --
    if(ArithOpWithSelfUpdate(node)) {
//    if(isSgPlusPlusOp(node) || isSgMinusMinusOp(node) || isSgAssignOp(node)) {


      //overload = buildOverloadFnForOp(fn_name, isSgExpression(node), retType, sizeofArg);

      SgExpression* oper;
      SgExpression* lhs_copy;
      SgExpression* rhs_copy;

//      if(isSgPlusPlusOp(node) || isSgMinusMinusOp(node)) {
      if(isSgUnaryOp(node)) {

        SgUnaryOp* uop = isSgUnaryOp(node);

        oper = uop->get_operand();

        lhs_copy = copyExpression(oper);

        rhs_copy = NULL;

      }
      else {
        //ROSE_ASSERT(isSgAssignOp(node));

        SgBinaryOp* bop = isSgBinaryOp(node);

        oper = bop->get_lhs_operand();

        lhs_copy = copyExpression(oper);

        rhs_copy = copyExpression(bop->get_rhs_operand());
      }

      // build an overload fn which handles the operation.... take the
      // address of the lhs so that the update happens to the struct
      // without making a copy in the callee.

      ROSE_ASSERT(isValidStructType(oper->get_type()));

      if(!isSgVarRefExp(oper)) {
        // create a local variable of the same type as the operand
        // Generate name using type and counter
        SgName s_name(oper->get_type()->get_mangled() + "_Var_ovl_" + boost::lexical_cast<std::string>(ArithCheck::VarCounter++));

        SgVariableDeclaration* local_var_decl = createLocalVariable(s_name, oper->get_type(), NULL, getScopeForExp(isSgExpression(node)));


        SgStatement* stmt = getSuitablePrevStmt(getEnclosingStatement(node));
        insertStatementBefore(stmt, local_var_decl);
        //insertStatementBefore(getEnclosingStatement(node), local_var_decl);


        SgVarRefExp* local_var_ref = buildVarRefExp(local_var_decl);

        // assign oper to local variable
        SgExpression* local_var_assign = buildAssignOp(local_var_ref, lhs_copy);

        // Now, we should insert this prior to the overloaded fn... and use
        // the local in the overloaded fn..

        // After the assign operation above, the local_var_ref will now have its lvalue set.
        // Since, we have to use the lvalue as an operand to a function, it shouldn't have its
        // lvalue set.
        SgExpression* local_var_ref_copy = copyExpression(local_var_ref);
        local_var_ref_copy->set_lvalue(false);

        // Lets not cast it to an address... instead, we could leave it at the pointer to the struct
        // type... Can't think of a case where this would be a problem
        // ****IMPORTANT****: If this is changed, also change the same in the else condition below
        //SgExpression* local_var_addr =  castToAddr(createAddressOfOpFor(copyExpression(local_var_ref_copy)));
        SgExpression* local_var_addr =  createAddressOfOpFor(copyExpression(local_var_ref_copy));

        #ifdef OLD_NAME_GEN
//        overload = buildOverloadFn(fn_name, local_var_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)));
        overload = buildOverloadFn(fn_name, local_var_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)), findInsertLocation(getEnclosingStatement(node)));
        #else
//        overload = buildOverloadFn(SgName(getStringForVariantT(node->variantT())), local_var_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)));
        overload = buildOverloadFn(SgName(getStringForVariantT(node->variantT())), local_var_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)), findInsertLocation(getEnclosingStatement(node)));
        #endif

        // build a comma op with local_var_assign and local_var_overload
        SgExpression* assign_over = buildCommaOpExp(local_var_assign, overload);

        // Now, we update the original variable's value. For that, since we don't maintain the
        // reverse map anymore... we will use the address of the variable itself!
        SgExpression* update_exp = updatePointerUsingAddr(isSgVarRefExp(local_var_ref_copy));

        // build a comma op with assign_over and update
        SgExpression* assign_over_update = buildCommaOpExp(assign_over, update_exp);

        // Now, create a copy of local_var_ref and add it as the last one
        SgExpression* assign_over_update_local = buildCommaOpExp(assign_over_update, copyExpression(local_var_ref));

        printf("assign_over_update_local -- the final one in handleOverloadOps\n");
        printNode(assign_over_update_local);

        // Now, replace the original expression, with this one.
        replaceExpression(isSgExpression(node), assign_over_update_local);

      }
      else {

        // Lets not cast it to an address... instead, we could leave it at the pointer to the struct
        // type... Can't think of a case where this would be a problem
        //SgExpression* lhs_addr = castToAddr(createAddressOfOpFor(lhs_copy));
        SgExpression* lhs_addr = createAddressOfOpFor(lhs_copy);


        #ifdef OLD_NAME_GEN
//        overload = buildOverloadFn(fn_name, lhs_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)));
        overload = buildOverloadFn(fn_name, lhs_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)), findInsertLocation(getEnclosingStatement(node)));
        #else
//        overload = buildOverloadFn(SgName(getStringForVariantT(node->variantT())), lhs_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)));
        overload = buildOverloadFn(SgName(getStringForVariantT(node->variantT())), lhs_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)), findInsertLocation(getEnclosingStatement(node)));
        #endif

        // Now, we update the original variable's value. For that, since we don't maintain the
        // reverse map anymore... we will use the address of the variable itself!
        SgExpression* update_exp = updatePointerUsingAddr(isSgVarRefExp(lhs_copy));

        // build a comma op with both these exps
        SgCommaOpExp* over_update = buildCommaOpExp(overload, update_exp);

        // And of course, put the struct as the last
        // Assuming here that the lhs is a variable.
        SgExpression* lhs_struct = copyExpression(lhs_copy);

        SgCommaOpExp* over_update_lhs = buildCommaOpExp(over_update, lhs_struct);

        replaceExpression(isSgExpression(node), over_update_lhs);
      }


      #if 0

      if(isSgVarRefExp(oper)) {

        // A variable is being updated here -- so, we need to send the address of the lhs -- the variable being
        // updated by this operation
        // Also, we return a variable with the updated data -- by value -- as normal semantics dictate

        SgExpression* lhs_addr = createAddressOfOpFor(isSgVarRefExp(lhs_copy));

        overload = buildOverloadFn(fn_name, lhs_addr, rhs_copy, retType, getScopeForExp(isSgExpression(node)));

        // This is a variable reference... do an update pointer

        // Find symbol for var_ref operand
        SgVariableSymbol* var_symbol = findSymbolForVarRef(isSgVarRefExp(oper));

        // This doesn't hold true anymore... instead, we could check that the type of the
        // var_symbol is of struct type
        //ROSE_ASSERT(isSgSymbol(var_symbol)->get_type() == getStructType(getVoidPointerType()));
        ROSE_ASSERT(isValidStructType(isSgSymbol(var_symbol)->get_type()));


        SgVariableSymbol* orig_var_symbol = findInReverseMap(var_symbol);

        SgExpression* update_exp = updatePointer(getNameForSymbol(orig_var_symbol), getNameForSymbol(var_symbol));

        SgCommaOpExp* over_update = buildCommaOpExp(overload, update_exp);

        // We need to return the struct -- not the pointer -- so, add the struct as a single
        // var ref expression, to the end of the comma op.

        SgVarRefExp* var_ref = buildVarRefExp(var_symbol);

        SgCommaOpExp* over_update_var = buildCommaOpExp(over_update, var_ref);

        replaceExpression(isSgExpression(node), over_update_var);
      }
      else {
        // Since its not a variable reference, no one pointer is begin affected
        // Its more of a change in value...
        // we treat this as a simple plus/minus - insert overload fn and replace expression
        overload = buildOverloadFnForOp(fn_name, isSgExpression(node), retType, sizeofArg);
        replaceExpression(isSgExpression(node), overload);
      }
      #endif

    }
    else {
      // For add/subtract, no need to send in the address

      SgBinaryOp* bop = isSgBinaryOp(node);

      SgExpression* lhs_copy = copyExpression(bop->get_lhs_operand());
      SgExpression* rhs_copy = copyExpression(bop->get_rhs_operand());

      SgScopeStatement* scope = getScopeForExp(bop);

      #ifdef OLD_NAME_GEN
//      overload = buildOverloadFn(fn_name, lhs_copy, rhs_copy, retType, scope);
      overload = buildOverloadFn(fn_name, lhs_copy, rhs_copy, retType, scope, findInsertLocation(getEnclosingStatement(node)));
      #else
//      overload = buildOverloadFn(SgName(getStringForVariantT(node->variantT())), lhs_copy, rhs_copy, retType, scope);
      overload = buildOverloadFn(SgName(getStringForVariantT(node->variantT())), lhs_copy, rhs_copy, retType, scope, findInsertLocation(getEnclosingStatement(node)));
      #endif

      replaceExpression(isSgExpression(node), overload);

      #if 0
      // sizeofArg = NULL
      #ifdef OLD_NAME_GEN
      overload = buildOverloadFnForOp(fn_name, isSgExpression(node), retType, sizeofArg);
      #else
      overload = buildOverloadFnForOp(SgName(getStringForVariantT(node->variantT)), isSgExpression(node), retType, sizeofArg);
      #endif
      replaceExpression(isSgExpression(node), overload);
      #endif
    }

  }
  else {
    printf("handleOverloadOps:What else?\n");
    ROSE_ASSERT(0);
  }

}

SgScopeStatement* ArithCheck::getScopeThruParent(SgNode* node) {
  if(node->get_parent() != NULL) {
    SgNode* parent = node->get_parent();
    printNode(parent);
    if(isSgScopeStatement(parent)) {
      return isSgScopeStatement(parent);
    }
    else {
      return getScopeThruParent(parent);
    }
  }
  else {
    return NULL;
  }
}

SgScopeStatement* ArithCheck::getScopeForVarRef(SgVarRefExp* var_ref) {

  SgVariableSymbol* var_symbol = var_ref->get_symbol();
  SgInitializedName* init_name = getNameForSymbol(var_symbol);
  return init_name->get_scope();
}

bool ArithCheck::findInVarRemap(SgVarRefExp* var_ref) {
  printf("handleVarRefs\n");

  SgVariableSymbol* var_symbol = var_ref->get_symbol();
  ROSE_ASSERT(var_symbol != NULL);
  VariableSymbolMap_t::iterator iter = ArithCheck::varRemap.find(var_ref->get_symbol());

  if(iter == ArithCheck::varRemap.end()) {
    return false;
  }

  ROSE_ASSERT(iter != ArithCheck::varRemap.end());

  return true;
}

bool ArithCheck::isDotOrArrow(SgNode* node) {
  return isSgDotExp(node) || isSgArrowExp(node);
}

SgExpression* ArithCheck::createStructForVarRef(SgVarRefExp* var_ref) {

  // Essentially, this variable reference, which could have been an lvalue
  // is not, no longer one, because we are using it in as an argument to a
  // function.
  SgExpression* var_ref_copy = copyExpression(var_ref);
  var_ref_copy->set_lvalue(false);

  SgExpression* ptr_value = var_ref_copy;

  SgExpression* addr_ptr = castToAddr(createAddressOfOpFor(copyExpression(var_ref_copy)));

  // We will be returning a struct variable based on the type
  // of this variable.
  // The struct might need to be created
  SgType* retType = getStructType(getTypeForPntrArr(var_ref->get_type()), findInsertLocation(getEnclosingStatement(var_ref)), true);
  printf("getScopeForExp:var_ref\n");
  printNode(var_ref);

  SgScopeStatement* scope = getScopeForVarRef(var_ref);
  printf("Got scope:\n");
  printNode(scope);

  SgExpression* createfn;

  if(isSgArrayType(var_ref->get_type())) {

    #if 1
    // create_struct(var_ref, &var_ref, sizeof(var_ref))
    // we don't need to do a divide at the sizeof(var_ref) since we need the size in bytes...
    // for metadatacalculations. We won't use this size for additions/subtractions.
    SgExpression* ce1 = buildCastExp(ptr_value, getTypeForPntrArr(var_ref->get_type()), SgCastExp::e_C_style_cast);
    SgExpression* ce2 = castToAddr(createAddressOfOpFor(copyExpression(var_ref)));
    SgExpression* ce3 = buildSizeOfOp(copyExpression(var_ref));

    createfn = build3ArgOverloadFn("create_struct", ce1, ce2, ce3, retType, scope, findInsertLocation(getEnclosingStatement(var_ref)));
    #else
    // Old working one
    // Change the first argument to a pointer... its currently
    // of array variable type ... something like unsigned int[bounds].
    // We'll transform that into unsigned int*
    SgExpression* casted_ptr_value = buildCastExp(ptr_value, getTypeForPntrArr(var_ref->get_type()), SgCastExp::e_C_style_cast);

    // Insert third arg as bounds of the array
    SgExpression* array_index = getArrayIndex(isSgVarRefExp(var_ref_copy));
    SgExpression* third_arg = copyExpression(array_index);


    #ifdef OLD_NAME_GEN
    SgName fn_name(getStringForFn("create_struct", casted_ptr_value->get_type(), addr_ptr->get_type(), third_arg->get_type(), retType));
//    createfn = build3ArgOverloadFn(fn_name, casted_ptr_value, addr_ptr, third_arg, retType, scope);
    createfn = build3ArgOverloadFn(fn_name, casted_ptr_value, addr_ptr, third_arg, retType, scope, findInsertLocation(getEnclosingStatement(var_ref)));
    #else
//    createfn = build3ArgOverloadFn("create_struct", casted_ptr_value, addr_ptr, third_arg, retType, scope);
    createfn = build3ArgOverloadFn("create_struct", casted_ptr_value, addr_ptr, third_arg, retType, scope, findInsertLocation(getEnclosingStatement(var_ref)));
    #endif
    #endif
  }
  else {
    SgName fn_name(getStringForFn("create_struct", ptr_value->get_type(), addr_ptr->get_type(), NULL, retType));
    #ifdef OLD_NAME_GEN
//    createfn = buildOverloadFn(fn_name, ptr_value, addr_ptr, retType, scope);
    createfn = buildOverloadFn(fn_name, ptr_value, addr_ptr, retType, scope, findInsertLocation(getEnclosingStatement(var_ref)));
    #else
//    createfn = buildOverloadFn("create_struct", ptr_value, addr_ptr, retType, scope);
    createfn = buildOverloadFn("create_struct", ptr_value, addr_ptr, retType, scope, findInsertLocation(getEnclosingStatement(var_ref)));
    #endif

  }

  printf("createfn\n");
  printNode(createfn);

  return createfn;
}

void ArithCheck::mapExprToOriginalType(SgExpression* replacement, SgType* original_type) {
  ROSE_ASSERT(isQualifyingType(original_type));
  ArithCheck::OriginalVarType.insert(ExprTypeMap_t::value_type(replacement, original_type));
}

SgType* ArithCheck::findInOriginalVarType(SgExpression* replacement) {
  ExprTypeMap_t::iterator etm;

  etm = ArithCheck::OriginalVarType.find(replacement);

  if(etm != ArithCheck::OriginalVarType.end()) {
    SgType* orig_type = etm->second;
    return orig_type;
  }

  ROSE_ASSERT(0);
}

void ArithCheck::mapExprToOriginalVarRef(SgExpression* replacement, SgExpression* original) {
  ROSE_ASSERT(isQualifyingType(original->get_type()));
  ExprMap_t::iterator em = OriginalVarRef.find(replacement);
  if(em != OriginalVarRef.end()) {
    OriginalVarRef.erase(em);
  }

  ArithCheck::OriginalVarRef.insert(ExprMap_t::value_type(replacement, copyExpression(original)));
}

SgExpression* ArithCheck::findInOriginalVarRef(SgExpression* replacement) {
  ExprMap_t::iterator em;

  em = ArithCheck::OriginalVarRef.find(replacement);

  if(em != ArithCheck::OriginalVarRef.end()) {
    SgExpression* orig = em->second;
    return orig;
  }

  ROSE_ASSERT(0);
}



void ArithCheck::handlePointerVarRefs(SgVarRefExp* var_ref) {

  printf("handlePointerVarRefs\n");
  // Here, we call create_struct function, which will
  // create a struct variable using the pointer value
  // and the pointer variable's address
  // Any initialization data for this pointer would already be
  // in the TrackingDB if this pointer was initialized.

  // do the old trick for cases where the symbol is found
  // in the varRemap -- these are variables in the parameter
  // list that have been converted to structs.

  // If the parent to this var_ref is an arrow/dot expression, don't
  // touch it... this variable is not a local/global variable... its
  // address and metadata is given by the result of the dot/arrow exp
  if(isDotOrArrow(var_ref->get_parent())) {
    printf("Parent is dot/arrow exp\n");
    printNode(var_ref->get_parent());
    printf("returning\n");
    return;
  }


  if(findInVarRemap(var_ref)) {
    handleVarRefs(var_ref);
    return;
  }

  SgExpression* createfn = createStructForVarRef(var_ref);

  #if 0
  ROSE_ASSERT(isSgPointerType(var_ref->get_type()));

  SgExpression* ptr_value = copyExpression(var_ref);
  SgExpression* addr_ptr = castToAddr(createAddressOfOpFor(copyExpression(var_ref)));

  // We will be returning a struct variable based on the type
  // of this variable.
  // The struct might need to be created
  SgType* retType = getStructType(var_ref->get_type(), true);
  printf("getScopeForExp:var_ref\n");
  printNode(var_ref);

//  SgScopeStatement* scope = getScopeForExp(var_ref);
//  SgScopeStatement* scope = getScopeThruParent(isSgNode(var_ref));
  SgScopeStatement* scope = getScopeForVarRef(var_ref);
  printf("Got scope:\n");
  printNode(scope);

  SgExpression* createfn = buildOverloadFn("create_struct", ptr_value, addr_ptr, retType, scope);
  printf("createfn\n");
  printNode(createfn);
  #endif

  mapExprToOriginalType(createfn, var_ref->get_type());
  #ifndef ONLY_ADD_ARRAYS_TO_OVR
  mapExprToOriginalVarRef(createfn, var_ref);
  printf("handlePointerVarRefs\n");
  printf("mapping\n");
  printf("createfn\n");
  printNode(createfn);
  printf("key: %llu\n", (unsigned long long)createfn);
  printf("var_ref\n");
  printNode(var_ref);
  #endif

  replaceExpression(var_ref, createfn);

  // OLD STUFF
  #if 0
  // For now, we use the handleVarRefs to do pointer var refs
  handleVarRefs(var_ref);
  #endif
}

SgExpression* ArithCheck::getArrayIndex(SgExpression* exp) {

  SgType* exp_type = exp->get_type();
  SgArrayType* exp_arr_type = isSgArrayType(exp_type);
  return exp_arr_type->get_index();
}

SgType* ArithCheck::getArrayBaseType(SgExpression* exp) {

  SgType* exp_type = exp->get_type();
  SgArrayType* exp_arr_type = isSgArrayType(exp_type);
  return exp_arr_type->get_base_type();
}

void ArithCheck::handleArrayVarRefs(SgVarRefExp* var_ref) {

  // FIXME: Think this needs to be modified on the lines of
  // handlePoitnerVarRefs...

  // If the parent to this var_ref is an arrow/dot expression, don't
  // touch it... this variable is not a local/global variable... its
  // address and metadata is given by the result of the dot/arrow exp
  if(isDotOrArrow(var_ref->get_parent())) {
    printf("Parent is dot/arrow exp\n");
    printNode(var_ref->get_parent());
    printf("returning\n");
    return;
  }

  // Array variables can be passed without turning into pointers at the other end...
  if(findInVarRemap(var_ref)) {
    handleVarRefs(var_ref);
    return;
  }

  SgExpression* createfn = createStructForVarRef(var_ref);

  #if 0
  // Here, we call create_struct function, which will
  // create a struct variable using the array's base
  // address, its index (size of the array from the ArrayType)
  // and the array variable's address.
  // the size of the array is required because this would
  // be inserted into the TrackingDB if an entry doesn't exist
  // for the array.

  ROSE_ASSERT(isSgArrayType(var_ref->get_type()));

  SgExpression* array_value = copyExpression(var_ref);
  SgExpression* addr_array = castToAddr(createAddressOfOpFor(copyExpression(var_ref)));
  SgExpression* array_index = copyExpression(getArrayIndex(var_ref));

  // We get the base type of the array... and then, get a pointer
  // version of that... and create a struct of that type.
  // Change here is that we are converting that this array type... into an
  // equivalent pointer type
  SgType* retType = getStructType(SgPointerType::createType(getArrayBaseType(var_ref)), true);

  SgScopeStatement* scope = getScopeForExp(var_ref);

  SgExpression* createfn = build3ArgOverloadFn("create_struct", array_value, addr_array, array_index, retType, scope);
  #endif

  printf("handleArrayVarRefs\n");
  mapExprToOriginalType(createfn, var_ref->get_type());
  mapExprToOriginalVarRef(createfn, var_ref);
  printf("mapping\n");
  printf("createfn\n");
  printNode(createfn);
  printf("key: %llu\n", (unsigned long long)createfn);
  printf("var_ref\n");
  printNode(var_ref);


  replaceExpression(var_ref, createfn);

  // OLD STUFF
  #if 0
  // For now, we use the handleVarRefs to do array var refs
  handleVarRefs(var_ref);
  #endif
}

SgExpression* ArithCheck::createDotArrowOverloadExp(SgBinaryOp* dotarrow_exp) {

  ROSE_ASSERT(isSgDotExp(dotarrow_exp) || isSgArrowExp(dotarrow_exp));

  // Here, we know that its a dot exp with a variable operand
  // If its a pointer, do the following:
  // create_struct(dotarrow_exp, &dotarrow_exp);

  // If its an array, do the following:
  // create_struct(dotarrow_exp, &dotarrow_exp, array_size)


  SgExpression* dotarrow_exp_copy = copyExpression(dotarrow_exp);
  dotarrow_exp_copy->set_lvalue(false);
  SgExpression* ptr_exp = dotarrow_exp_copy;
  ptr_exp->set_lvalue(false);
  SgExpression* addr_exp = castToAddr(createAddressOfOpFor(copyExpression(dotarrow_exp_copy)));
  addr_exp->set_lvalue(false);
  ROSE_ASSERT(addr_exp->get_lvalue() == false);

  SgExpression* oper = dotarrow_exp->get_rhs_operand();
  SgType* oper_type = oper->get_type();

  // The return type is the struct version of the type of the operand.
  // Since the operand can be an array or pointer, we use the generic function to get the correct type
  SgType* retType = getStructType(getTypeForPntrArr(oper_type), findInsertLocation(getEnclosingStatement(dotarrow_exp)), true);

  SgName fn_name(getStringForOp(dotarrow_exp, retType));

  SgExpression* overload;


  if(isSgArrayType(oper->get_type())) {

    SgArrayType* array = isSgArrayType(oper->get_type());
    SgExpression* array_size = copyExpression(array->get_index());

    #ifdef OLD_NAME_GEN
//    overload = build3ArgOverloadFn(fn_name, ptr_exp, addr_exp, array_size, retType, getScopeForExp(dotarrow_exp));
    overload = build3ArgOverloadFn(fn_name, ptr_exp, addr_exp, array_size, retType, getScopeForExp(dotarrow_exp), findInsertLocation(getEnclosingStatement(dotarrow_exp)));
    #else
    // OLD
    //overload = build3ArgOverloadFn(SgName(getStringForVariantT(dotarrow_exp->variantT())), ptr_exp, addr_exp, array_size, retType, getScopeForExp(dotarrow_exp));
    // Working one. Commented for adding node at the end of the list
//    overload = build3ArgOverloadFn("create_struct", ptr_exp, addr_exp, array_size, retType, getScopeForExp(dotarrow_exp));
    overload = build3ArgOverloadFn("create_struct", ptr_exp, addr_exp, array_size, retType, getScopeForExp(dotarrow_exp), findInsertLocation(getEnclosingStatement(dotarrow_exp)));
    #endif
  }
  else {
    #ifdef OLD_NAME_GEN
//    overload = buildOverloadFn(fn_name, ptr_exp, addr_exp, retType, getScopeForExp(dotarrow_exp));
    overload = buildOverloadFn(fn_name, ptr_exp, addr_exp, retType, getScopeForExp(dotarrow_exp), findInsertLocation(getEnclosingStatement(dotarrow_exp)));
    #else
    //overload = buildOverloadFn(SgName(getStringForVariantT(dotarrow_exp->variantT())), ptr_exp, addr_exp, retType, getScopeForExp(dotarrow_exp));
    // Working one... Commented out to add dotarrow_exp
//    overload = buildOverloadFn("create_struct", ptr_exp, addr_exp, retType, getScopeForExp(dotarrow_exp));
    overload = buildOverloadFn("create_struct", ptr_exp, addr_exp, retType, getScopeForExp(dotarrow_exp), findInsertLocation(getEnclosingStatement(dotarrow_exp)));
    #endif
  }

  return overload;

}

void ArithCheck::handleDotExpVarOper(SgDotExp* dot_exp) {

  SgExpression* overload = createDotArrowOverloadExp(dot_exp);

  // replace the dot_exp with the overloaded one.
  replaceExpression(dot_exp, overload);
}


void ArithCheck::handleDotExpFuncOper(SgDotExp* dot_exp) {

}

void ArithCheck::handleDotExp(SgDotExp* dot_exp) {

  // So, we get wrap the dot exp in a create struct... right?
  // But that doesn't always work because if its an array we would need
  // initialization data... We need to create a struct which uses
  // the dot exp's addr, and location it points to.
  // If this is an array type, then we might need to send in the initialization
  // data for that array, since we may not have recorded it.

  // Two cases here... var operand... function operand
  if(isSgVarRefExp(dot_exp->get_rhs_operand())) {
    handleDotExpVarOper(dot_exp);
  }
  else if(isSgFunctionCallExp(dot_exp->get_rhs_operand())) {
    handleDotExpFuncOper(dot_exp);
  }
  else {
    printf("Not a variable/func call in dot exp?\n");
    printNode(dot_exp);
    ROSE_ASSERT(0);
  }
}

SgExpression* ArithCheck::createDerefOvlForStruct(SgExpression* str) {

  ROSE_ASSERT(isValidStructType(str->get_type()));

  SgType* retType = findInUnderlyingType(str->get_type());

  SgScopeStatement* scope = getScopeForExp(str);

  SgExpression* arg = copyExpression(str);

  #if OLD_NAME_GEN
  SgName fn_name(getStringForFn("Deref", arg->get_type(), NULL, NULL, retType));
//  SgExpression* dereffn = buildOverloadFn(fn_name, arg, NULL, retType, scope);
  SgExpression* dereffn = buildOverloadFn(fn_name, arg, NULL, retType, scope, findInsertLocation(getEnclosingStatement(str)));
  #else
//  SgExpression* dereffn = buildOverloadFn("Deref", arg, NULL, retType, scope);
  SgExpression* dereffn = buildOverloadFn("Deref", arg, NULL, retType, scope, findInsertLocation(getEnclosingStatement(str)));
  #endif

  return dereffn;

}

SgExpression* ArithCheck::derefLHSOfArrowExp(SgArrowExp* arrow_exp) {
  // Now, deref it...
  ROSE_ASSERT(isValidStructType(arrow_exp->get_lhs_operand()->get_type()));

  SgExpression* deref_ovl = createDerefOvlForStruct(arrow_exp->get_lhs_operand());

  // Now, create an arrow expr using the deref_ovl and copy of rhs.
  SgExpression* rhs_copy = copyExpression(arrow_exp->get_rhs_operand());

  // create the arrow exp
  SgExpression* new_arrow_exp = buildArrowExp(deref_ovl, rhs_copy);

  return new_arrow_exp;
}


SgExpression* ArithCheck::handlePntrArrLHSArrowExp(SgArrowExp* arrow_exp) {

  printf("arrow_exp: \n");
  printNode(arrow_exp);
  printf("arrow_exp->lhs: \n");
  printNode(arrow_exp->get_lhs_operand());
  printf("arrow_exp->rhs: \n");
  printNode(arrow_exp->get_rhs_operand());

  // Assert that we are looking at a var ref here.
  ROSE_ASSERT(isSgVarRefExp(arrow_exp->get_lhs_operand()));
  // Assert that its the qualifying type
  ROSE_ASSERT(isQualifyingType(arrow_exp->get_lhs_operand()->get_type()));

  SgArrowExp* rep_arrow_exp;

  if(findInVarRemap(isSgVarRefExp(arrow_exp->get_lhs_operand()))) {
    handleVarRefs(isSgVarRefExp(arrow_exp->get_lhs_operand()));
    rep_arrow_exp = arrow_exp;
    printf("Found arrow->lhs_operand in var-remap -- lhs oper--\n");
    printNode(arrow_exp->get_lhs_operand());
  }
  else {
    // Now, call the function to create the struct version of this var ref
    SgExpression* createfn = createStructForVarRef(isSgVarRefExp(arrow_exp->get_lhs_operand()));

    // replace original one with this one... in fact, be careful and create a new
    // arrow exp.
    rep_arrow_exp = buildArrowExp(createfn, copyExpression(arrow_exp->get_rhs_operand()));

    replaceExpression(arrow_exp, rep_arrow_exp);

    printf("created rep arrow rep\n");
    printNode(rep_arrow_exp);

  }

  // Now, deref it...

  SgExpression* new_arrow_exp = derefLHSOfArrowExp(rep_arrow_exp);

  replaceExpression(rep_arrow_exp, new_arrow_exp);

  printf("Created new arrow exp\n");
  printNode(new_arrow_exp);

  return new_arrow_exp;
}

SgExpression* ArithCheck::handleStructLHSArrowExp(SgArrowExp* arrow_exp) {

  SgExpression* new_arrow_exp = derefLHSOfArrowExp(arrow_exp);

  replaceExpression(arrow_exp, new_arrow_exp);

  return new_arrow_exp;
}


SgExpression* ArithCheck::handlePntrArrRHSArrowExp(SgArrowExp* arrow_exp) {

  SgExpression* overload = createDotArrowOverloadExp(isSgBinaryOp(arrow_exp));

  replaceExpression(arrow_exp, overload);

  printf("Created overload to replace arrow exp\n");
  printNode(overload);

  return overload;
}

void ArithCheck::handleArrowExpVarOper(SgArrowExp* arrow_exp) {

  // With arrow, we first create a deref function for
  // the struct on lhs. Then, we use createDotArrowOverloadExp
  // to overload the arrow exp
  printf("handleArrowExpVarOper\n");

  // The lhs of the arrow_exp is still a pointer/array type. Need to handle
  // it here -- by converting into the respective struct.
  // ... we only do this if it isn't in the varRemap

  SgExpression* new_arrow_exp = handlePntrArrLHSArrowExp(arrow_exp);


  // Now, we need createDotArrowOverloadExp, to create the overloaded function
  SgExpression* overload = createDotArrowOverloadExp(isSgBinaryOp(new_arrow_exp));

  // Now, replace the new_arrow_exp, with overload
  replaceExpression(new_arrow_exp, overload);

  printf("Created overload to replace new arrow exp\n");
  printNode(overload);
}


void ArithCheck::handleArrowExpFuncOper(SgArrowExp* arrow_exp) {

  // create a deref function for the lhs struct, and
  // the function already returns a struct... so we should be
  // good?
  // Yes...

  handlePntrArrLHSArrowExp(arrow_exp);
  // No need to overload... the RHS since its a function returning
  // a struct... so, nothing else to do...
}

void ArithCheck::handleArrowExp(SgArrowExp* arrow_exp) {

  if(isSgVarRefExp(arrow_exp->get_rhs_operand())) {
    handleArrowExpVarOper(arrow_exp);
  }
  else if(isSgFunctionCallExp(arrow_exp->get_rhs_operand())) {
    handleArrowExpFuncOper(arrow_exp);
  }
  else {
    printf("Not a variable/func in arrow exp?\n");
    printNode(arrow_exp);
    ROSE_ASSERT(0);
  }
}

void ArithCheck::handlePointerDeref(SgPointerDerefExp* deref_exp) {
  handleDerefExp(deref_exp);
}

void ArithCheck::handleArrayDeref(SgPointerDerefExp* deref_exp) {
  // For now, we use handleDerefExp to handle array derefs
  // handleDerefExp returns the underlying type
  handleDerefExp(deref_exp);
}

void ArithCheck::handlePointerArrRefs(SgPntrArrRefExp* pntr_arr_ref) {
  // Using handlePntrArrRefExp on pointers as well as array types
  // since the operation is similar on both variables
  handlePntrArrRefExp(pntr_arr_ref);
}

void ArithCheck::handleArrayArrRefs(SgPntrArrRefExp* pntr_arr_ref) {
  handlePntrArrRefExp(pntr_arr_ref);
}

void ArithCheck::handlePointerDotExp(SgDotExp* dot_exp) {
  handleDotExp(dot_exp);
}

void ArithCheck::handleArrayDotExp(SgDotExp* dot_exp) {
  handleDotExp(dot_exp);
}

void ArithCheck::handlePointerArrowExp(SgArrowExp* arrow_exp) {
  handleArrowExp(arrow_exp);
}

void ArithCheck::handleArrayArrowExp(SgArrowExp* arrow_exp) {
  handleArrowExp(arrow_exp);
}

void ArithCheck::handlePointerCastExp(SgCastExp* cast_exp) {
  // Cast expressions typically go from one struct to another
  // or one value to a struct, or vice versa.
  // This behavior is regardless of whether
  // its a pointer/array type
  handleCastExp(cast_exp);
}

void ArithCheck::handleArrayCastExp(SgCastExp* cast_exp) {
  handleCastExp(cast_exp);
}

void ArithCheck::handlePointerAddrOfOp(SgAddressOfOp* addr_of) {
  // FIXME: Not checking whether the address of op exactly
  // fits in here well...
  handleAddressOfOp(addr_of);
}

void ArithCheck::handleArrayAddrOfOp(SgAddressOfOp* addr_of) {
  // FIXME: Not checking whether the address of op exactly
  // fits in here well...
  handleAddressOfOp(addr_of);

}




void ArithCheck::handleVarDecls4(SgVariableDeclaration* var_decl) {

  SgInitializedName* var_name = getNameForDecl(var_decl);

  if(isSgPointerType(var_name->get_type())) {
    printf("variable is a pointer\n");
    handlePointerVars(var_decl);
  }
  #if 1
  else if(isSgArrayType(var_name->get_type())) {
    printf("variable is an array\n");

    handleArrayVars(var_decl);
  }
  else {
    printf("What is this variable decl doing here?\n");
    printNode(var_decl);
    ROSE_ASSERT(0);
  }
  #endif
}

void ArithCheck::handleClassStructVarRefs(SgVarRefExp* var_ref) {
  // This function replaces class object var refs/struct var refs
  // with the correct version.. since, the object/var might have been
  // replaced in the func param list by a ref... this replacement
  // is done so that we can copy over the pointers... The actual
  // pointer copy occurs when the func param is replaced. At that
  // place, we insert into the ClassStructVarRemap. Here, we check that
  // map to see if we need to replace the current var ref with a new one

  printf("handleClassStructVarRefs\n");

  SgVariableSymbol* var_symbol = var_ref->get_symbol();
  ROSE_ASSERT(var_symbol != NULL);
  VariableSymbolMap_t::iterator iter = ArithCheck::ClassStructVarRemap.find(var_ref->get_symbol());
  if(iter == ArithCheck::ClassStructVarRemap.end()) {
    // This var ref need not be replaced
    return;
  }

  SgVariableSymbol* str_symbol = iter->second;

  ROSE_ASSERT(str_symbol != NULL);
  SgVarRefExp* str_var_ref = buildVarRefExp(isSgVariableSymbol(str_symbol));
  ROSE_ASSERT(str_var_ref != NULL);
  // This map to original type was required only when replacing pointers.
  // to figure out what the underlying array type was.
  #if 0
  mapExprToOriginalType(str_var_ref, var_ref->get_type());
  printf("handleVarRefs\n");
  #ifdef ONLY_ADD_ARRAYS_TO_OVR
  if(isSgArrayType(var_ref->get_type())) {
    mapExprToOriginalVarRef(str_var_ref, var_ref);
    printf("mapping\n");
    printf("str_var_ref\n");
    printNode(str_var_ref);
    printf("key: %llu\n", (unsigned long long)str_var_ref);
    printf("var_ref\n");
    printNode(var_ref);

  }
  replaceExpression(var_ref, str_var_ref);
  #else
  mapExprToOriginalVarRef(str_var_ref, var_ref);
  printf("mapping\n");
  printf("str_var_ref\n");
  printNode(str_var_ref);
  printf("key: %llu\n", (unsigned long long)str_var_ref);
  printf("var_ref\n");
  printNode(var_ref);

  replaceExpression(var_ref, str_var_ref);
  #endif
  #endif
  replaceExpression(var_ref, str_var_ref);
  printf("Done\n");


}

void ArithCheck::handleVarRefs4(SgVarRefExp* var_ref) {
  // FIXME: Skip cases where parent is
  // arrow or dot exp... and this is the rhs!
  printf("handleVarRefs4\n");
  ROSE_ASSERT(var_ref->get_symbol() != NULL);



  // Removing these checks since parameter inputs are
  // removed from the function, and replaced by structs.
  // using get_type() seg faults.
  #if 1
  if(isSgPointerType(var_ref->get_type())) {
    handlePointerVarRefs(var_ref);
  }
  else if(isSgArrayType(var_ref->get_type())) {
    handleArrayVarRefs(var_ref);
  }
  #ifdef CLASS_STRUCT_COPY
  else if(isSgClassType(var_ref->get_type())) {
    handleClassStructVarRefs(var_ref);
  }
  #endif
  else {
    printf("What is this var ref doing here?\n");
    printNode(var_ref);
    ROSE_ASSERT(0);
  }
  #endif


}

void ArithCheck::handleFuncCalls4(SgFunctionCallExp* fncall) {

  // Working one...
  #if 0
  if(isQualifyingType(fncall->get_type()) || isFree(fncall) || isQualifyingLibCall(fncall)) {
    handleFuncCalls(fncall);
  }
  else {
    printf("What is this fn call doing here?\n");
    printNode(fncall);
    ROSE_ASSERT(0);
  }
  #else
  handleFuncCalls(fncall);
  #endif


}

void ArithCheck::handleDerefs4(SgPointerDerefExp* deref_exp) {

  // We will always see the type to be class type...
  // since the var ref would be changed to struct.
  #if 0
  if(isSgPointerType(deref_exp->get_type())) {
    handlePointerDeref(deref_exp);
  }
  else if(isSgArrayType(deref_exp->get_type())) {
    handleArrayDeref(deref_exp);
  }
  #endif
  if(isSgClassType(deref_exp->get_operand()->get_type())) {
    handleDerefExp(deref_exp);
  }
  else {
    printf("What is this deref?\n");
    printNode(deref_exp);
    ROSE_ASSERT(0);
  }

}

void ArithCheck::handlePntrArrRefs4(SgPntrArrRefExp* pntr_arr_ref) {

  // We will always see the type to be class type...
  // since the var ref would be changed to struct.

  #if 0
  if(isSgPointerType(pntr_arr_ref->get_type())) {
    handlePointerArrRefs(pntr_arr_ref);
  }
  else if(isSgArrayType(pntr_arr_ref->get_type())) {
    handleArrayArrRefs(pntr_arr_ref);
  }
  #endif
  //if(isSgClassType(pntr_arr_ref->get_type())) {
  if(isSgClassType(pntr_arr_ref->get_lhs_operand()->get_type())) {
    handlePntrArrRefExp(pntr_arr_ref);
  }
  else {
    printf("What is this pntr arr ref?");
    printNode(pntr_arr_ref);
    ROSE_ASSERT(0);
  }
}

void ArithCheck::handleDotExp4(SgDotExp* dot_exp) {

  // We will always see the type to be class type...
  // since the var ref would be changed to struct.
  // FIXME: This is not correct, I think.

  #if 0
  if(isSgPointerType(dot_exp->get_type())) {
    handlePointerDotExp(dot_exp);
  }
  else if(isSgArrayType(dot_exp->get_type())) {
    handleArrayDotExp(dot_exp);
  }
  #endif
  if(isQualifyingType(dot_exp->get_type())) {
    handleDotExp(dot_exp);
  }
  else {
    printf("What is this dot exp?");
    printNode(dot_exp);
    ROSE_ASSERT(0);
  }

}

SgExpression* ArithCheck::handleLHSArrowExp(SgArrowExp* arrow_exp) {
  printf("handleLHSArrowExp\n");

  SgExpression* lhs = arrow_exp->get_lhs_operand();

  SgExpression* new_arrow_exp;
  if(isSgThisExp(lhs)) {
    // this->val... nothing to do...
    new_arrow_exp = arrow_exp;
  }
  else if(isValidStructType(lhs->get_type())) {
    // Struct from a function...
    new_arrow_exp = handleStructLHSArrowExp(arrow_exp);
  }
  else if(isQualifyingType(lhs->get_type())) {
    new_arrow_exp = handlePntrArrLHSArrowExp(arrow_exp);
  }

  else {
    printf("What is this LHS of arrow exp?\n");
    printNode(arrow_exp);
    printNode(arrow_exp->get_lhs_operand());
    ROSE_ASSERT(0);
  }

  return new_arrow_exp;
}

SgExpression* ArithCheck::handleRHSArrowExp(SgArrowExp* arrow_exp) {

  printf("handleLHSArrowExp\n");

  SgExpression* rhs = arrow_exp->get_rhs_operand();

  SgExpression* new_arrow_exp;

  if(isQualifyingType(rhs->get_type())) {
    new_arrow_exp = handlePntrArrRHSArrowExp(arrow_exp);
  }
  else {
    // Shouldn't have to do anything else...
    new_arrow_exp = arrow_exp;
  }

  return new_arrow_exp;
}

void ArithCheck::handleArrowExp4(SgArrowExp* arrow_exp) {

  // We will always see the type to be class type...
  // since the var ref would be changed to struct.
  // FIXME: This is not correct, I think.


  // Handle LHS of Arrow exp... and then handle
  // the RHS...
  // This function replaces the original arrow exp with the
  // new one.
  SgExpression* new_arrow_exp = handleLHSArrowExp(arrow_exp);

  // Now to handle the RHS...
  SgExpression* final_arrow_exp = handleRHSArrowExp(isSgArrowExp(new_arrow_exp));

  #if 0
  if(isSgPointerType(arrow_exp->get_type())) {
    handlePointerArrowExp(arrow_exp);
  }
  else if(isSgArrayType(arrow_exp->get_type())) {
    handleArrayArrowExp(arrow_exp);
  }
  else if(isQualifyingType(arrow_exp->get_lhs_operand()->get_type())) {
    // This handles cases where the output of the arrow exp
    // is not a pointer/array, but the lhs operand is.
    handlePntrArrLHSArrowExp(arrow_exp);
  }
  else {
    printf("What is this arrow exp?");
    printNode(arrow_exp);
    ROSE_ASSERT(0);
  }
  #endif
}

void ArithCheck::handleCastExp4(SgCastExp* cast_exp) {

  // We will always see the type to be class type...
  // since the var ref would be changed to struct.
  // FIXME: This could be a value as well... Handle
  // that case

  #if 1
  if(isQualifyingType(cast_exp->get_type()) ||
    isValidStructType(cast_exp->get_operand()->get_type())) {
    handleCastExp(cast_exp);
  }
  else {
    printf("What is this cast exp?");
    printNode(cast_exp);
    ROSE_ASSERT(0);
  }
  #endif
}

void ArithCheck::handleAddressOfOp4(SgAddressOfOp* addr_of) {

  // We will always see the type to be class type...
  // since the var ref would be changed to struct.
  // FIXME: This could be address of stack variable... not handled
  // here... !

  handleAddressOfOp(addr_of);

  #if 0
  if(isSgPointerType(addr_of->get_type())) {
    handlePointerAddrOfOp(addr_of);
  }
  else if(isSgArrayType(addr_of->get_type())) {
    handleArrayAddrOfOp(addr_of);
  }
  else {
    printf("What is this addr of op?");
    printNode(addr_of);
    ROSE_ASSERT(0);
  }
  #endif
}


void ArithCheck::InstrumentNodes4(SgProject* project) {

  printf("InstrumentNodes4 - begin\n");
  for(NodeContainer::iterator nit = ArithCheck::NodesToInstrument.begin(); nit != ArithCheck::NodesToInstrument.end(); ++nit) {

    SgNode* node = *nit;
    Sg_File_Info* File = isSgNode(node)->get_file_info();
    printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
    printf("File: (%s, %d, %d) %s = %s\n", File->get_filenameString().c_str(),
                      File->get_line(),
                      File->get_col(),
                      isSgNode(node)->sage_class_name(),
                      isSgNode(node)->unparseToString().c_str());




    printf("is variable declaration?");
    if(isSgVariableDeclaration(node)) {
      printf("Yes\n");
      handleVarDecls4(isSgVariableDeclaration(node));
      continue;
    }
    else {
      printf("No\n");
    }

    #if 1
    printf("is variable reference?");
    if(isSgVarRefExp(node)) {
      printf("Yes\n");
      handleVarRefs4(isSgVarRefExp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    printf("is func call?");
    if(isSgFunctionCallExp(node)) {
      printf("Yes\n");
      handleFuncCalls4(isSgFunctionCallExp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    printf("is deref exp?");
    if(isSgPointerDerefExp(node)) {
      printf("Yes\n");
      handleDerefs4(isSgPointerDerefExp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    printf("is pntr arr ref?");
    if(isSgPntrArrRefExp(node)) {
      printf("Yes\n");
      handlePntrArrRefs4(isSgPntrArrRefExp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    printf("is dot exp?");
    if(isSgDotExp(node)) {
      printf("Yes\n");
      handleDotExp4(isSgDotExp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    printf("is arrow exp?");
    if(isSgArrowExp(node)) {
      printf("Yes\n");
      handleArrowExp4(isSgArrowExp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    printf("is cast exp?");
    if(isSgCastExp(node)) {
      printf("Yes\n");
      handleCastExp4(isSgCastExp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    printf("is address of exp?");
    if(isSgAddressOfOp(node)) {
      printf("Yes\n");
      handleAddressOfOp4(isSgAddressOfOp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    printf("is comma op?");
    if(isSgCommaOpExp(node)) {
      printf("Yes\n");
      printf("Nothing to do.\n");
      continue;
    }
    else {
      printf("No\n");
    }

    // Conditional ops return bool... and its
    // a simple overload -- irrespective of whether
    // its a pointer or array type -- just use the
    // struct available
    printf("is conditional op?");
    if(isConditionalOp(node)) {
      printf("Yes\n");
      handleConditionals(isSgBinaryOp(node));
      continue;
    }
    else {
      printf("No\n");
    }

    // if the return type is of qualifying type..
    // change it to struct type... works the same for
    // pointers and arrays.
    printf("is function decl?");
    if(isSgFunctionDeclaration(node)) {
      printf("Yes\n");
      handleFuncDecls(isSgFunctionDeclaration(node));
      continue;
    }
    else {
      printf("No\n");
    }

    // We will modify handleFuncParams to handle pointers
    // and arrays differently. No need to create different
    // calls at the top level.
    printf("is param list?");
    if(isSgFunctionParameterList(node)) {
      printf("Yes\n");
      handleFuncParams(isSgFunctionParameterList(node));
      continue;
    }
    else {
      printf("No\n");
    }

    #ifdef CLASS_STRUCT_COPY
    // Finding return stmts which need to be converted to assign
    // and void returns...
    printf("is return stmt?");
    if(isSgReturnStmt(node)) {
      printf("Yes\n");
      handleReturnStmts(isSgReturnStmt(node));
      continue;
    }
    else {
      printf("No\n");
    }
    #endif

    // Changed needs to be overloaded to check for arith ops
    // only -- so this would be used to handle those ops which
    // don't change the type of pointer/array, or create
    // a pointer/array type
    // they simply
    // perform arithmetic operations on the given type.
    // should work the same for both pointer and array types
    printf("is it an overloadable op?");
    if(NeedsToBeOverloaded(node)) {
      printf("Yes\n");
      handleOverloadOps(node);
      continue;
    }
    else {
      printf("No\n");
    }
    #endif
    // Casts from those expressions which output VoidStr should be removed -- or we could simply
    // put a Cast_Overload. Removing an expression might require clean up. Putting a Cast_Overload for
    // now would be a good idea.

  }
  printf("InstrumentNodes4 - end\n");

}

void ArithCheck::HackyPtrCheck7() {

  printf("HackyPtrCheck7\n");

  SgProject* project = ArithCheck::ACProject;

  // This will remove all the original expression trees -
  // generated through constant propagation/folding
  SageInterface::removeAllOriginalExpressionTrees(isSgNode(project));

  TopBotTrack2 TB2;
  nodeType inhAttr(UNKNOWN, INH_UNKNOWN);
  TB2.traverseInputFiles(project, inhAttr);

  //createTrackingStruct(project);

  getFirstStatementInScope(getFirstGlobalScope(project));

  InstrumentNodes4(project);
}

void ArithCheck::insertPreamble() {

  insertLibHeader();

  HackyPtrCheck7();

  //insertTimingSupport(ArithCheck::ACProject);

}

void ArithCheck::insertExecFunctions() {

  SgNode* TopNode = isSgNode(ArithCheck::ACProject);

  SgFunctionDeclaration* MainFn = SageInterface::findMain(TopNode);

  SgScopeStatement* scope = MainFn->get_scope();

  SgFunctionDefinition* MainFnDef = MainFn->get_definition();

  SgBasicBlock* FirstBlock = MainFnDef->get_body();

  SgStatementPtrList& Stmts = FirstBlock->get_statements();

}

void ArithCheck::instrument() {

    insertPreamble();

    // FIXME: Removed actual overflow checks to test pointer checking
    #if 0
  for(BopContainer::iterator bpit = SelectedBops.begin(); bpit != SelectedBops.end();
      ++bpit) {

    SgBinaryOp* Bop = *bpit;
    instrument(Bop);
  }
  #endif
}






bool checkDontInstrumentList(SgBinaryOp* Parent, SgExpression* LHS, SgExpression* RHS) {

  // Unsigned Subtract Ops - cannot overflow
  if(Parent->variantT() == V_SgSubtractOp &&
    (LHS->get_type()->isUnsignedType() || RHS->get_type()->isUnsignedType())) {
    return true;
  }
  // Pointer Types are not currently handled - Return type or Op types shouldn't be pointer types
  else if((Parent->get_type()->variantT() == V_SgPointerType) ||
    (LHS->get_type()->variantT() == V_SgPointerType) ||
    (RHS->get_type()->variantT() == V_SgPointerType)) {
    return true;
  }
  // Array Types are not currently handled - Return type or Op types shouldn't be array types
  else if((Parent->get_type()->variantT() == V_SgArrayType) ||
    (LHS->get_type()->variantT() == V_SgArrayType) ||
    (RHS->get_type()->variantT() == V_SgArrayType)) {
    return true;
  }

  // We can handle all other cases
  return false;

}

void ArithCheck::instrument(SgBinaryOp* bop) {

    printf("insertArithCheck\n");

    printf("bop: %lu\n", &(*bop));

    // Seg faulting here. Possibly due to the fact that the bop
    // has been "removed" somehow... --- can't seem to get a
    // read on it

    SgExpression* LHS = bop->get_lhs_operand();
    SgExpression* RHS = bop->get_rhs_operand();


    printf("LHS: %lu\n", &(*LHS));
  printf("RHS: %lu\n", &(*RHS));



    if(checkDontInstrumentList(bop, LHS, RHS)) {
        printf("Part of dontInstrumentList\n");
        return;
    }

    printInfo(bop, LHS, RHS);

    SgStatement* stmt = getSurroundingStatement(*isSgExpression(bop));
    SgScopeStatement* scope = stmt->get_scope();

    ROSE_ASSERT(scope);

    SgExpression* LHS_Copy = copyExpression(LHS);
    SgExpression* RHS_Copy = copyExpression(RHS);

    SgType* LHSTy = resolveTypedefsAndReferencesToBaseTypes(LHS->get_type());
    SgType* RHSTy = resolveTypedefsAndReferencesToBaseTypes(RHS->get_type());
    SgType* ParTy = resolveTypedefsAndReferencesToBaseTypes(bop->get_type());

    VariantT ParV = bop->variantT();
    VariantT ParV_Ty = ParTy->variantT();
    VariantT LHSV = LHSTy->variantT();
    VariantT RHSV = RHSTy->variantT();

    ROSE_ASSERT(LHSV != V_SgTypedefType);
    ROSE_ASSERT(RHSV != V_SgTypedefType);

    char* FnName = getString(ParV_Ty, ParV, LHSV, RHSV);

    SgName s_name(FnName);
    SgExprListExp* parameter_list = buildExprListExp();

    appendExpression(parameter_list, LHS_Copy);
    appendExpression(parameter_list, RHS_Copy);

    SgExpression* NewFunction = SageBuilder::buildFunctionCallExp(s_name,
                                                                bop->get_type(),
                                                                parameter_list,
                                                                scope);

    replaceExpression(bop, NewFunction);


}


void ArithCheck::instrument(SgNode* Node) {
  printf("ArithCheck:process\n");
  if(isSgBinaryOp(Node)) {
    instrument(isSgBinaryOp(Node));
    }
}



#endif
