#include "traverse.h"

namespace Trav {

NodeContainer NodesToInstrument;
NodeContainer DeclsToClone;
SgProject* project;



bool candidateFnCall(SgNode* node) {
  SgFunctionCallExp* fncall = isSgFunctionCallExp(node);
  ROSE_ASSERT(fncall);
  #ifdef CAND_FN_DEBUG
  printf("candidateFnCall\n");
  #endif

  // Check if the return type is qual
  // if CLASS_STRUCT_COPY, return type is class/struct
  SgExprListExp* param_list = fncall->get_args();

  SgExpressionPtrList& exprs = param_list->get_expressions();
  SgExpressionPtrList::iterator it = exprs.begin();

  for(; it != exprs.end(); ++it) {

    SgType* ce_type = (*it)->get_type();
    #ifdef CAND_FN_DEBUG
    Util::printNode(ce_type);
    #endif

    #ifdef STRIP_TYPEDEFS
    ce_type = Util::getType(ce_type);
    #endif

    if(Util::isQualifyingType(ce_type)) {
      return true;
    }

    #ifdef CLASS_STRUCT_COPY
    if(strDecl::isOriginalClassType(ce_type)) {
      return true;
    }
    #endif
  }

  SgType* fncall_retType = fncall->get_type();
  #ifdef STRIP_TYPEDEFS
  fncall_retType = Util::getType(fncall_retType);
  #endif

  if(Util::isQualifyingType(fncall_retType)) {
    return true;
  }
  #ifdef CLASS_STRUCT_COPY
  else if(strDecl::isOriginalClassType(fncall_retType)) {
    return true;
  }
  #endif
  else {
    return false;
  }
}

bool candidateFnDecl(SgFunctionDeclaration* fn_decl) {
  // Find return type
  SgFunctionType* fn_type = fn_decl->get_type();
  SgType* ret_type = fn_type->get_return_type();

  #ifdef STRIP_TYPEDEFS
  ret_type = Util::getType(ret_type);
  #endif

  if(Util::isQualifyingType(ret_type)) {
    return true;
  }
  #ifdef CLASS_STRUCT_COPY
  else if(strDecl::isOriginalClassType(ret_type)) {
    return true;
  }
  #endif
  return false;
}

void CheckExprSanity(SgExpression* expr) {

  SgType* expr_type = expr->get_type();
  #ifdef STRIP_TYPEDEFS
  expr_type = Util::getType(expr_type);
  #endif

  if(Util::isQualifyingType(expr_type)) {
    if(SgBinaryOp* bop = isSgBinaryOp(expr)) {
      SgType* lhs_type = bop->get_lhs_operand()->get_type();
      #ifdef STRIP_TYPEDEFS
      lhs_type = Util::getType(lhs_type);
      #endif

      SgType* rhs_type = bop->get_rhs_operand()->get_type();
      #ifdef STRIP_TYPEDEFS
      rhs_type = Util::getType(rhs_type);
      #endif

      ROSE_ASSERT(Util::isQualifyingType(lhs_type) ||
            Util::isQualifyingType(rhs_type));
    }
    else if(SgUnaryOp* uop = isSgUnaryOp(expr)) {
      SgType* oper_type = uop->get_operand()->get_type();
      #ifdef STRIP_TYPEDEFS
      oper_type = Util::getType(oper_type);
      #endif

      ROSE_ASSERT(Util::isQualifyingType(oper_type));
    }
    else {
      printf("Can't check sanity for expr:\n");
      printf("expr: %s = %s\n", isSgNode(expr)->sage_class_name(), isSgNode(expr)->unparseToString().c_str());
      ROSE_ASSERT(0);
    }
  }
}

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

bool isQualifyingLibCall(SgFunctionCallExp* fncall) {

  if(fncall->getAssociatedFunctionDeclaration() == NULL) {
    return false;
  }

  // get the namespace for the function.. and if the namespace is
  // std... then label the function as a lib call
  #ifdef EXCLUDE_STD_NAMESP_FUNC
  SgFunctionDeclaration* fn_decl = fncall->getAssociatedFunctionDeclaration();
  if(Util::isInNamespace(fn_decl, "std")) {
    return true;
  }
  #endif


  std::string LibFns[] = {"free",
              "printf",
              "atoi",
              "atol",
              "sscanf",
              "fprintf",
              "open",
              "read",
              "__builtin_object_size",
              "__builtin___memset_chk",
              "__inline_memset_chk",
                "__builtin___memcpy_chk",
              "__inline_memcpy_chk",
              "__builtin___memmove_chk",
              "__inline_memmove_chk",
              "__builtin___strcpy_chk",
              "__inline_strcpy_chk",
              "__builtin___strncpy_chk",
              "__inline_strncpy_chk",
              "__builtin___strcat_chk",
              "__inline_strcat_chk",
              "__builtin___strncat_chk",
              "__inline_strncat_chk",
              "memset",
              "memcpy",
              "strlen",
              "strchr",
              "strpbrk",
              "strspn",
              "strstr",
              "strtok",
              "strcmp",
              "feof",
              "fopen",
              "rewind",
              "fscanf",
              "fclose",
              "fgetc",
              "gets",
              "fgets",
              "fputc",
              "__assert_rtn",
              "__assert_fail",
              "__builtin___sprintf_chk",
              "fflush",
              "__error",
              "strerror",
              "perror",
              "remove",
              "pthread_create",
              "pthread_join",
              "pthread_exit",
              "pthread_mutex_init",
              "pthread_mutex_lock",
              "pthread_mutex_unlock",
              "__errno_location()"
              };

  for(unsigned int index = 0; index < sizeof(LibFns)/sizeof(std::string); index++) {
    if(strcmp(LibFns[index].c_str(),
      fncall->getAssociatedFunctionDeclaration()->get_name().str()) == 0) {
      return true;
    }
  }

  return false;
}

// Not using inherited attribute since we aren't looking at classes and data
// structures for now
Util::nodeType TopBotTrack2::evaluateSynthesizedAttribute(SgNode* node,
        Util::nodeType, SynthesizedAttributesList synList) {
  #ifdef SYNTH_DEBUG
  printf("TopBotTrack2: Ev_Synth_Attr\n");
  printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
  Util::printNodeExt(node);
  #endif

  #ifdef CHECK_STATIC
  if(isSgDeclarationStatement(node)) {
    Util::checkIfStatic(isSgDeclarationStatement(node));
  }
  #endif

  #ifdef DISCARD_GENERATED_NODES
  Sg_File_Info* File = isSgNode(node)->get_file_info();
  if(strcmp(File->get_filenameString().c_str(), "compilerGenerated") == 0) {
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;
  }
  #endif



  #ifdef SYNTH_DEBUG
  printf("Is variable declaration? ");
  #endif
  if(isSgVariableDeclaration(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    SgVariableDeclaration* var_decl = isSgVariableDeclaration(node);

    SgInitializedName* var_name = Util::getNameForDecl(var_decl);


    SgType* var_type = Util::getType(var_name->get_type());
    #ifdef STRIP_TYPEDEFS
    var_type = Util::getType(var_type);
    #endif

    #ifdef ALLOW_UNTRACKED_VARIABLES
    // Even if this is a var decl that we don't intend to track,
    // we should maintain its metadata, since it might be part of an expression
    // for which we require metadata. In effect, allowing untracked variables only
    // removes the var refs... and other related expressions from contention, not
    // the metadata gathering itself.

    #else
    if(Util::isQualifyingType(var_type) && (var_name->get_initializer() != NULL)) {
      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(var_type) && (var_name->get_initializer() != NULL)) {
      // Only handling assign initializers
      if(isSgAssignInitializer(var_name->get_initializer())) {
        // Now, this var decl only matters if its being initialized here
        // Hence, check if it has an initializer...
        NodesToInstrument.push_back(node);
        Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
        return synattr;
      }
    }
    #endif
    else {
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    #endif

  }
  else {
    #ifdef SYNTH_DEBUG
    // Not var decl at all. don't do anything
    printf("No\n");
    #endif
  }


  #ifdef SYNTH_DEBUG
  printf("is variable reference? ");
  #endif
  if(isSgVarRefExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    printf("Is it an lvalue? %s", isSgVarRefExp(node)->get_lvalue() ? "Yes\n" : "No\n");
    printf("Is it an LVal? %s", isSgVarRefExp(node)->isLValue() ? "Yes\n" : "No\n");
    #endif

    SgVarRefExp* var_ref = isSgVarRefExp(node);
    SgType* var_type = var_ref->get_type();
    #ifdef STRIP_TYPEDEFS
    var_type = Util::getType(var_type);
    #endif

    // Require this to replace var refs to str from varRemap -- for
    // params to a function
    if(Util::isQualifyingType(var_type)) {
      if(strcmp(var_ref->get_symbol()->get_name().getString().c_str(),"Fibonacci") == 0) {
        printf("Found Fibonacci\n");
      }
      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    // Require this to replace var refs with reference versions
    // from ClassStructVarRemap -- for params to a function
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(var_type)) {
      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    #endif
    else {
      #ifdef INIT_CHECKS
      // For performing initialization checks, we need to track all
      // the uses of variables
      NodesToInstrument.push_back(node);
      #endif
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }

  }
  else {
    // Not var decl at all. don't do anything
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

  #ifdef INSTR_THIS_EXP
  #ifdef SYNTH_DEBUG
  printf("is this exp? ");
  #endif
  if(isSgThisExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif
    // "this" is a pointer, so we'll be converting it to
    // struct type
    NodesToInstrument.push_back(node);
    Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
    return synattr;
  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }
  #endif



  #if 0
  #ifdef SYNTH_DEBUG
  printf("is function ref? ");
  #endif
  if(isSgFunctionRefExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    SgFunctionRefExp* fnref = isSgFunctionRefExp(node);
    // Just that we know that we are dealing with a function pointer I guess...?
  }
  else {
    // Not a function ref.
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }
  #endif

  #ifdef SYNTH_DEBUG
  printf("Is it a function call?");
  #endif
  if(isSgFunctionCallExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif
    SgFunctionCallExp* fncall = isSgFunctionCallExp(node);

    #ifdef PANIC_DEBUG
    SgFunctionDeclaration* fn_decl = fncall->getAssociatedFunctionDeclaration();
    ROSE_ASSERT(fn_decl);
    if(strcmp(fn_decl->get_name().getString().c_str(), "panic") == 0) {
      printf("FnCall:Found panic\n");
      SgExprListExp* args = fncall->get_args();
      SgExpressionPtrList& exprs = args->get_expressions();
      SgExpressionPtrList::iterator iter = exprs.begin();
      for(; iter != exprs.end(); iter++) {
        SgExpression* exp = *iter;
        printf("exp: \n");
        Util::printNode(exp);
      }
      printf("printing args\n");
      // Associated Function Declaration.. param list types
      SgFunctionDeclaration* fn_decl = fncall->getAssociatedFunctionDeclaration();
      SgInitializedNamePtrList& decl_args = fn_decl->get_args();
      SgInitializedNamePtrList::iterator iter2 = decl_args.begin();
      for(; iter2 != decl_args.end(); iter2++) {
        SgInitializedName* arg = *iter2;
        printf("arg: \n");
        Util::printNode(arg);
        printf("arg_type: \n");
        Util::printNode(arg->get_type());
      }
      printf("Done printing panic\n");
    }
    #endif

    // Don't need to check for implicit casts... So, lets filter out
    // the useless ones here.
    // Always add... to check for implicit casts in the arguments

    // FIXME: If the fncall returns a class/struct or ptr/qual.
    // Or if one or more arguments is ptr/qual or class/struct.

    if(candidateFnCall(node)) {
      #ifdef SYNTH_DEBUG
      printf("Added to NodesToInstrument\n");
      #endif
      NodesToInstrument.push_back(node);
    }

    SgType* fncall_type = fncall->get_type();
    #ifdef STRIP_TYPEDEFS
    fncall_type = Util::getType(fncall_type);
    #endif

    if(Util::isQualifyingType(fncall_type)) {
//      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else if(Util::isFree(fncall)) {
//      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else if(isQualifyingLibCall(fncall)) {
//      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else {
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }


  }
  else {
    // not a func call exp. don't do anything
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

  // Shifted to Inherited Attribute...
  #if 0
  #ifdef LOCK_KEY_INSERT
  printf("LOCK_KEY_INSERT: Is it a basic block?");
  if(isSgBasicBlock(node)) {
    printf("Yes\n");
    NodesToInstrument.push_back(node);
  }
  else {
    printf("No\n");
  }
  #endif
  #endif


  // FIXME: In the case of deref exp, we can't blind move from
  // Util::STR_TYPE to Util::NOT_STR_TYPE since it could be a double pointer
  // in which case, the return type would also be Util::STR_TYPE
  // We might need to investigate how our technique would handle
  // such a case since we shouldn't always return VoidStruct or
  // the POD type for a deref. The return type changes based on
  // the input type... which is not known in VoidStruct
  // But we would know this statically, so, we could use a different
  // Deref_Overload_DoublePtr in that case which would output
  // a VoidStruct

  // In case of a deref_exp we would need to check here if the
  // operand is a single or double pointer, since that defines
  // where we return a Util::STR_TYPE or a Util::NOT_STR_TYPE

  // FIXME: I don't think the code below detects double pointers correctly
  // And, I am not sure, how we would actually handle double pointer cases
  // This piece of code is causing the runs to crash for some reason.
  // Commenting it out
  #if 1
  #ifdef SYNTH_DEBUG
  printf("is it a deref exp?");
  #endif
  if(isSgPointerDerefExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    SgPointerDerefExp* ptr_deref = isSgPointerDerefExp(node);


    // Check that all derefs do in fact, operate on pointers or
    // arrays!
    SgExpression* oper = ptr_deref->get_operand();
    SgType* oper_type = oper->get_type();
    #ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
    #endif

    #ifdef SUPPORT_REFERENCES
    //ROSE_ASSERT(Util::isQualifyingType(oper_type) || isSgReferenceType(oper_type));
    if(isSgReferenceType(oper_type)) {
      // nothing to do...
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    #else
    ROSE_ASSERT(Util::isQualifyingType(oper_type));
    #endif


    // There is always work to do on a deref!
    NodesToInstrument.push_back(node);

    SgType* ptr_deref_type = ptr_deref->get_type();
    #ifdef STRIP_TYPEDEFS
    ptr_deref_type = Util::getType(ptr_deref_type);
    #endif

    if(Util::isQualifyingType(ptr_deref_type)) {
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else {
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }


  }
  else {
    // Not a pointer deref exp. do nothing
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }
  #endif

  #ifdef SYNTH_DEBUG
  printf("is it a pntr arr ref exp? ");
  #endif
  if(isSgPntrArrRefExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    // It is possible that the pntr arr ref is not
    // used by a ptr/array. Case in question:
    // #define PART_SEP "-"
    // PART[0] -- This is a macro based use of pntr arr ref.
    SgPntrArrRefExp* array_ref = isSgPntrArrRefExp(node);
    SgType* lhs_type = array_ref->get_lhs_operand()->get_type();
    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif

    if(!Util::isQualifyingType(lhs_type)) {
      #ifdef SYNTH_DEBUG
      printf("PntrArrRef. Not qualifing type... \n");
      Util::printNode(node);
      #endif
      // Nothing to do in this case.
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }



    // Check that all pnts arr refs operate on pointers or
    // arrays
    //SgExpression* oper = array_ref->get_lhs_operand();
    //ROSE_ASSERT(Util::isQualifyingType(oper->get_type()));
    ROSE_ASSERT(Util::isQualifyingType(lhs_type));

    // INIT_CHECKS: PntrArrRefs are always added to
    // the NodesToInstrument. So, nothing more to do.
    // There is always work to do on an pntr arr ref!
    #ifdef SYNTH_DEBUG
    printf("Added PntrArrRef to NodesToInstrument\n");
    #endif
    NodesToInstrument.push_back(node);

    SgType* array_type = array_ref->get_type();
    #ifdef STRIP_TYPEDEFS
    array_type = Util::getType(array_type);
    #endif
    if(Util::isQualifyingType(array_type)) {
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else {
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }

  }
  else {
    // Not a pointer deref exp. do nothing
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

  #ifdef SYNTH_DEBUG
  printf("is it a dot exp? ");
  #endif
  if(isSgDotExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    SgDotExp* dot_exp = isSgDotExp(node);

    // Technically, only those dot expressions dealing with
    // pointers and arrays matter to us... since the others
    // are dealing with obj.var refs.
    SgType* dot_exp_type = dot_exp->get_type();
    #ifdef STRIP_TYPEDEFS
    dot_exp_type = Util::getType(dot_exp_type);
    #endif

    if(Util::isQualifyingType(dot_exp_type)) {
      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else {
      #ifdef INIT_CHECKS
      // For initialization, we need to know about all the
      // var refs-- which includes dot and arrow exps
      NodesToInstrument.push_back(node);
      #endif
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

  #ifdef SYNTH_DEBUG
  printf("is it an arrow exp? ");
  #endif
  if(isSgArrowExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    SgArrowExp* arrow_exp = isSgArrowExp(node);

    // Arrow expressions always work on pointers... and maybe
    // array types -- not sure how though. In any case,
    // has to qualifying type. And there's work to do!
    //SgExpression* oper = arrow_exp->get_lhs_operand();
    //ROSE_ASSERT(Util::isQualifyingType(oper->get_type()));
    SgType* lhs_type = arrow_exp->get_lhs_operand()->get_type();
    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif
    ROSE_ASSERT(Util::isQualifyingType(lhs_type));

    // INIT_CHECKS: All arrow exps are already added
    // to NodesToInstrument. Nothing more to do.
    // There is always work to do on an arrow exp!
    NodesToInstrument.push_back(node);

    SgType* arrow_type = arrow_exp->get_type();
    #ifdef STRIP_TYPEDEFS
    arrow_type = Util::getType(arrow_type);
    #endif

    if(Util::isQualifyingType(arrow_type)) {
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else {
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

  // Handle cast exp here itself since it can create a pointer out
  // of nowhere, through arbitrary casts.
  #ifdef SYNTH_DEBUG
  printf("is it a cast exp? ");
  #endif
  if(isSgCastExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    SgCastExp* cast_exp = isSgCastExp(node);

    // If the operand or
    // the return type are of qualifying type, then
    // there is work to do.
    SgType* cast_type = cast_exp->get_type();
    #ifdef STRIP_TYPEDEFS
    cast_type = Util::getType(cast_type);
    #endif

    SgType* oper_type = cast_exp->get_operand()->get_type();

    #ifdef STRIP_TYPEDEFS
    oper_type = Util::getType(oper_type);
    #endif

    if(Util::isQualifyingType(cast_type) ||
      Util::isQualifyingType(oper_type)) {
      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else {
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

  // Address Of Op can also create a qualifying type out of any variable....
  // So, we are definitely looking at a qualifying type here.
  #ifdef SYNTH_DEBUG
  printf("is address of op? ");
  #endif
  if(isSgAddressOfOp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    SgAddressOfOp* addr_of = isSgAddressOfOp(node);
    SgType* addr_of_type = addr_of->get_type();
    #ifdef STRIP_TYPEDEFS
    addr_of_type = Util::getType(addr_of_type);
    #endif

    // Assert that we create a qualifying type
    ROSE_ASSERT(Util::isQualifyingType(addr_of_type));
    NodesToInstrument.push_back(node);
    Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
    return synattr;
  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }


  // Lets handle the comma op case, where the type is determined
  // by the last expression type
  // No need to add to NodesToInstrument since we don't have to
  // do anything for a comma op
  #ifdef SYNTH_DEBUG
  printf("is comma op?");
  #endif
  if(isSgCommaOpExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    // Find the type we are looking for in the
    // last synthesized attribute
    Util::nodeType nt = synList[synList.size() - 1];
    if(nt.attr == Util::NOT_STR_TYPE) {
      #ifdef SYNTH_DEBUG
      printf("Not returning pointer from comma op\n");
      #endif
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else if(nt.attr == Util::STR_TYPE) {
      #ifdef SYNTH_DEBUG
      printf("Returning pointer from comma op\n");
      #endif
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
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
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }


  // Conditional ops. They might take in structs, but always
  // return bool (Util::NOT_STR_TYPE)
  #ifdef SYNTH_DEBUG
  printf("is it a conditional op?");
  #endif
  if(Util::isConditionalOp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    SgBinaryOp* bop = isSgBinaryOp(node);

    SgType* lhs_type = bop->get_lhs_operand()->get_type();

    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif

    SgType* rhs_type = bop->get_rhs_operand()->get_type();

    #ifdef STRIP_TYPEDEFS
    rhs_type = Util::getType(rhs_type);
    #endif


    if(Util::isQualifyingType(lhs_type) ||
      Util::isQualifyingType(rhs_type)) {
      NodesToInstrument.push_back(node);
    }
    #ifdef SYNTH_DEBUG
    printf("Return Util::NOT_STR_TYPE\n");
    #endif
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;


  }
  else {
    // Not a conditional op. Nothing to do
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

  #ifdef SYNTH_DEBUG
  printf("is it a param list?");
  #endif
  if(isSgFunctionParameterList(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    // Skip this function param list if its the main function
    SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(node->get_parent());
    ROSE_ASSERT(fn_decl != NULL);

    #ifdef PANIC_DEBUG
    if(strcmp(fn_decl->get_name().getString().c_str(), "panic") == 0) {
      printf("ParamList:Found panic\n");
    }
    #endif

    #ifdef SKIP_MAIN_PARAMS
    if(SI::isMain(fn_decl)) {
      #ifdef SYNTH_DEBUG
      printf("Main function param list found. Skipping\n");
      printf("Always returning Util::NOT_STR_TYPE\n");
      #endif
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    #endif


    SgInitializedNamePtrList& name_list = isSgFunctionParameterList(node)->get_args();

    for(SgInitializedNamePtrList::iterator iter = name_list.begin(); iter != name_list.end(); ++iter) {

      SgInitializedName* name = *iter;
      SgType* name_type = name->get_type();
      #ifdef STRIP_TYPEDEFS
      name_type = Util::getType(name_type);
      #endif

      if(Util::isQualifyingType(name_type)) {
        #ifdef SYNTH_DEBUG
        printf("Arg qualifies\n");
        #endif

        #ifdef CLONE_FUNCTIONS
        // If this function won't be inserted below... then insert it here...
        // This makes sure that there is no duplication.
        if(!candidateFnDecl(fn_decl)) {
          if(SI::isMain(fn_decl)) {
            printf("adding main at func param 1\n");
          }
          // add to decls to clone
          DeclsToClone.push_back(fn_decl);
        }
        #endif

        NodesToInstrument.push_back(node);
        break;
      }
      #ifdef CLASS_STRUCT_COPY
      else if(strDecl::isOriginalClassType(name_type)) {
        #ifdef SYNTH_DEBUG
        printf("Arg qualifies -- class type\n");
        #endif

        #ifdef CLONE_FUNCTIONS
        // If this function won't be inserted below... then insert it here...
        // This makes sure that there is no duplication.
        if(!candidateFnDecl(fn_decl)) {
          if(SI::isMain(fn_decl)) {
            printf("adding main at func param 2\n");
          }
          // add to decls to clone
          DeclsToClone.push_back(fn_decl);
        }
        #endif


        NodesToInstrument.push_back(node);
        break;
      }
      #endif
    }
    #ifdef SYNTH_DEBUG
    printf("Always returning Util::NOT_STR_TYPE\n");
    #endif
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;

  }
  else {
    // Not a param list. nothing to do
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

  #ifdef SYNTH_DEBUG
  printf("is it a func decl?");
  #endif
  if(isSgFunctionDeclaration(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif


    SgFunctionDeclaration* fn_decl = isSgFunctionDeclaration(node);

    #if 1
    if(candidateFnDecl(fn_decl)) {
      if(SI::isMain(fn_decl)) {
        printf("adding main at isSgFunctionDecl\n");
      }
      // add to decls to clone
      DeclsToClone.push_back(fn_decl);

      NodesToInstrument.push_back(node);
    }
    #else
    // Find return type
    SgFunctionType* fn_type = fn_decl->get_type();
    SgType* ret_type = fn_type->get_return_type();

    #ifdef STRIP_TYPEDEFS
    ret_type = Util::getType(ret_type);
    #endif

    if(Util::isQualifyingType(ret_type)) {
      NodesToInstrument.push_back(node);
    }
    #ifdef CLASS_STRUCT_COPY
    else if(strDecl::isOriginalClassType(ret_type)) {
      NodesToInstrument.push_back(node);
    }
    #endif
    #endif

    #ifdef SYNTH_DEBUG
    printf("Always returning Util::NOT_STR_TYPE\n");
    #endif
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;



  }
  else {
    // Not a func decl, nothing to do
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }

//  #ifdef CLASS_STRUCT_COPY
  #ifdef SYNTH_DEBUG
  printf("is it a return stmt?");
  #endif
  if(isSgReturnStmt(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif


    #ifdef DELAYED_INSTR
    // Should add all returns stmts which return
    // pointers -- some of these pointers might need to
    // be converted to struct at the return stmt...
    // If the return exp is of array type, then
    // create_entry needs to be performed as well


    SgExpression* ret_exp = isSgReturnStmt(node)->get_expression();

    #ifdef LOCK_KEY_INSERT
    NodesToInstrument.push_back(node);
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;
    #endif


    if(ret_exp != NULL) {
      SgType* ret_type = ret_exp->get_type();
      #ifdef STRIP_TYPEDEFS
      ret_type = Util::getType(ret_type);
      #endif

      #ifdef CLASS_STRUCT_COPY
      if(strDecl::isOriginalClassType(ret_type)) {
        NodesToInstrument.push_back(node);
      }
      // If the ret_exp returning qual type stays as qual
      // type by the time we hit the return stmt (in InstrumentNodes4)
      // then we'll use create_struct and create_entry (if its
      // an array
      else if(Util::isQualifyingType(ret_type)) {
        NodesToInstrument.push_back(node);
      }
      #else
      if(Util::isQualifyingType(ret_type)) {
        NodesToInstrument.push_back(node);
      }
      #endif
    }
    Util::nodeType my_synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return my_synattr;

    #else
    #if 0
    #ifdef LOCK_KEY_INSERT
    // all return stmts need to be instrumented with exit scope
    #ifdef SYNTH_DEBUG
    printf("LOCK_KEY_INSERT: Adding to NodesToInstrument\n");
    #endif
    NodesToInstrument.push_back(node);
    #ifdef SYNTH_DEBUG
    printf("Always returning Util::NOT_STR_TYPE\n");
    #endif
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;
    #else



    // Check if its of the OriginalClassType...
    #ifdef SYNTH_DEBUG
    printf("is it original class type?");
    #endif
    SgExpression* ret_exp = isSgReturnStmt(node)->get_expression();
    if(ret_exp == NULL) {
      // Nothing to do. This return statement has no expression associated with it.
      #ifdef SYNTH_DEBUG
      printf("No expression in the ret stmt\n");
      #endif
    }
    else {

      if(strDecl::isOriginalClassType(ret_exp->get_type())) {
        #ifdef SYNTH_DEBUG
        printf("Yes\n");
        #endif
        NodesToInstrument.push_back(node);
      }
      else {
        #ifdef SYNTH_DEBUG
        printf("No\n");
        #endif
      }
    }
    #ifdef SYNTH_DEBUG
    printf("Always returning Util::NOT_STR_TYPE\n");
    #endif
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;


    #endif
    #endif
    #endif

  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }
//  #endif

  #ifdef CLASS_STRUCT_COPY
//  #if defined(CLASS_STRUCT_COPY) || defined(LOCK_KEY_INSERT)
  #ifdef SYNTH_DEBUG
  printf("is it an assign stmt?");
  #endif
  if(isSgAssignOp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif
    SgType* lhs_type = isSgAssignOp(node)->get_lhs_operand()->get_type();

    #ifdef STRIP_TYPEDEFS
    lhs_type = Util::getType(lhs_type);
    #endif

    if(strDecl::isOriginalClassType(lhs_type)) {
      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }
  #endif

  #ifdef NEW_OVL_ENABLED
  if(isSgNewExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    #ifdef NEW_EXP_DEBUG
    SgNewExp* ne = isSgNewExp(node);
    printf("new exp\n");
    Util::printNodeExt(ne);

    SgType* nty = ne->get_type();
    printf("new_exp->get_type()\n");
    Util::printNode(nty);

    SgType* sty = ne->get_specified_type();
    printf("new_exp->get_specified_type()\n");
    Util::printNode(sty);
    #endif


    NodesToInstrument.push_back(node);
    Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
    return synattr;
  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }
  #endif

  #ifdef DELETE_OVL_ENABLED
  if(isSgDeleteExp(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    #ifdef DELETE_EXP_DEBUG
    #endif

    NodesToInstrument.push_back(node);
    Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
    return synattr;
  }
  else {
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }
  #endif


  #ifdef SYNTH_DEBUG
  printf("is it an overloadable op?");
  #endif
  // Arith ops...
  if(NeedsToBeOverloaded(node)) {
    #ifdef SYNTH_DEBUG
    printf("Yes\n");
    #endif

    // These are all expressions
    ROSE_ASSERT(isSgBinaryOp(node) || isSgUnaryOp(node));
    SgExpression* expr = isSgExpression(node);
    ROSE_ASSERT(expr != NULL);
    // The result of should be of qualifying type
    // to consider this node.

    SgType* expr_type = expr->get_type();
    #ifdef STRIP_TYPEDEFS
    expr_type = Util::getType(expr_type);
    #endif

    if(Util::isQualifyingType(expr_type)) {
      CheckExprSanity(expr);
      NodesToInstrument.push_back(node);
      Util::nodeType synattr(Util::STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
    else {
      Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
      return synattr;
    }
  }
  else {
    // need not be overloaded. Nothing to do.
    #ifdef SYNTH_DEBUG
    printf("No\n");
    #endif
  }


  // One golden rule which we can apply to figure out the relevant nodes -- i.e. the
  // ones that we need to manipulate -- is to see if a node actually has a get_type()
  // or rather, if a node has an associated type. If it doesn't have a type
  // associated with it, it doesn't return any value, right? this means,
  // it need not be overloaded, or changed in any way.


  //printf("Not propagating attributes upwards\n");
  //Util::nodeType synattr(UNKNOWN, Util::INH_UNKNOWN);

  #ifdef SYNTH_DEBUG
  printf("Propagating Util::NOT_STR_TYPE\n");
  #endif
  Util::nodeType synattr(Util::NOT_STR_TYPE, Util::INH_UNKNOWN);
  return synattr;


}

Util::nodeType TopBotTrack2::evaluateInheritedAttribute(SgNode* node,
                        Util::nodeType inh) {
  #if 1
  #ifdef INH_DEBUG
  printf("TopBotTrack2: Ev_Inh_Attr\n");
  printf("Current: %s = %s\n", node->sage_class_name(), node->unparseToString().c_str());
  Sg_File_Info* File = isSgNode(node)->get_file_info();
  printf("file: %s\n", File->get_filename());
  printf("Doing nothing right now.\n");
  #endif
  #endif

  #ifdef CPP_SCOPE_MGMT
  static bool first_decl_stmt = false;



  if((isSgFunctionDeclaration(node) || isSgClassDeclaration(node)
    || isSgVariableDeclaration(node)) && !first_decl_stmt) {
    SgName fn_name;
    if(SgFunctionDeclaration* fndecl = isSgFunctionDeclaration(node)) {
      fn_name = fndecl->get_name();
    }
    else if(SgClassDeclaration* classdecl = isSgClassDeclaration(node)) {
      fn_name = classdecl->get_name();
    }
    else if(SgVariableDeclaration* vardecl = isSgVariableDeclaration(node)) {
      fn_name = Util::getNameForDecl(vardecl)->get_name();
    }
    else {
      ROSE_ASSERT(0);
    }

    if(!Util::isNamespaceScope(isSgDeclarationStatement(node)->get_scope(), "std")) {
      // check if the decl stmt node is in the current file...
      Sg_File_Info* File = isSgNode(node)->get_file_info();
      //printf("file: %s\n", File->get_filename());
      SgFilePtrList file_list = project->get_fileList();
      ROSE_ASSERT(file_list.size() == 1); //RMM TEST: COMMENTED OUT...

      SgSourceFile* cur_file = isSgSourceFile(*file_list.begin());
      if(strcmp(cur_file->getFileName().c_str(), File->get_filename()) == 0) {
        Util::FirstDeclStmtInFile = isSgDeclarationStatement(node);
        first_decl_stmt = true;
        #ifdef INH_DEBUG
        Util::printNodeExt(node);
        printf("Found decl stmt in input file\n");
        #endif
      }
    }
  }
  #endif



  #ifdef LOCK_KEY_INSERT
  #ifdef INH_DEBUG
  printf("INHERITED_ATTRIBUTE: LOCK_KEY_INSERT: Is it a basic block?");
  #endif

  if(isSgDeclarationStatement(node)) {
    // check if the decl stmt node is in the current file...
    Sg_File_Info* File = isSgNode(node)->get_file_info();
    printf("file: %s\n", File->get_filename());
    SgFilePtrList file_list = project->get_fileList();
    //ROSE_ASSERT(file_list.size() == 1);

    SgSourceFile* cur_file = isSgSourceFile(*file_list.begin());
    if(strcmp(cur_file->getFileName().c_str(), File->get_filename()) == 0) {
      printf("Found decl stmt in input file\n");
    }

  }

  if(isSgBasicBlock(node)) {
    #ifdef INH_DEBUG
    printf("Yes\n");
    #endif
    NodesToInstrument.push_back(node);
  }
  else if(isSgGlobal(node)) {
    // This handles global variables by inserting a lock
    // at the global scope.
    #ifdef INH_DEBUG
    printf("Yes\n");
    #endif
    NodesToInstrument.push_back(node);
  }
  else {
    #ifdef INH_DEBUG
    printf("No\n");
    #endif
  }
  #endif

  return inh;

}



void traverse(SgProject* proj, SgFile* file) {
  project = proj;
  #ifdef RMM
  #ifndef TURN_OFF_INSTR
  TopBotTrack2 TB2;
  Util::nodeType inhAttr(Util::UNKNOWN, Util::INH_UNKNOWN);
  #ifdef JUST_TRAVERSE
  TB2.traverse(project, inhAttr);
  #elif defined(TRAVERSE_INPUT_FILE_TOP)
  //SgFilePtrList file_list = project->get_fileList();
  //ROSE_ASSERT(file_list.size() == 1);

  SgSourceFile* cur_file = file;
  SgGlobal* global_scope = cur_file->get_globalScope();
  TB2.traverse(global_scope, inhAttr);
  #else
  TB2.traverseInputFiles(project, inhAttr);
  #endif
  #endif
  #endif
  TopBotTrack2 TB2;
  Util::nodeType inhAttr(Util::UNKNOWN, Util::INH_UNKNOWN);
  SgGlobal* global_scope = isSgSourceFile(file)->get_globalScope();
  TB2.traverse(global_scope, inhAttr);

}

}
