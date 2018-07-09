#include "instr.h"

namespace Instr {

  void instr(SgNewExp* ne) {

#ifdef NEW_EXP_DEBUG
    printf("Begin handleNewExp\n");
    printf("new_exp\n");
    Util::printNode(ne);
#endif

    // Original:
    // new class A;
    // Transformed
    // class A* newvar;
    // newvar = new class A, create_entry_with_new_lock_gen(&newvar, newvar, newvar + alloc_size)
    // where alloc_size = sizeof(class A) if its a single object..
    // and alloc_size = array_size*sizeof(class A) if its an array type

    SgType* nty = ne->get_type();
    printf("new_exp->get_type()\n");
    Util::printNode(nty);

    // Converting it into a single pointer... stripping out array types
    // For the case: class A* ptr = new class A [5];
    // Here, the RHS is of type A (*)[5], whereas the LHS is of type A (*)
#ifdef STRIP_ARRAY_TYPE
    nty = Util::skip_ArrPntrRefTypedefs(nty);
    nty = SgPointerType::createType(nty);
#endif

    SgType* sty = ne->get_specified_type();
    printf("new_exp->get_specified_type()\n");
    Util::printNode(sty);


    ROSE_ASSERT(isSgPointerType(nty));

    SgConstructorInitializer* ci = ne->get_constructor_args();
    SgExprListExp* explist = ci->get_args();

    SgType* retType = strDecl::getStructType(Util::getTypeForPntrArr(nty),
        GEFD(SI::getEnclosingStatement(ne)),
        true);

    SgExpression* alloc_size;

    if(SgArrayType* arr = isSgArrayType(sty)) {
      // We are allocating an array of objects here...
      // So, we pass the index for this number of objects
      // to be created into the new_ovl function.
      // Also, there shouldn't be any constructor args...
      // Can't initialize objects within array
      ROSE_ASSERT(explist->empty());

      SgExpression* index = SI::copyExpression(arr->get_index());
      SgType* base = arr->get_base_type();
      SgExpression* sizeof_base = SB::buildSizeOfOp(base);

      // The total allocation size is: index*sizeof(base_type)
      alloc_size = SB::buildMultiplyOp(index, sizeof_base);
    }
    else {
      // This is the case where we are creating a single object...
      // And there might be constructor args...
      // The allocation size will be simply the size of the specified type
      alloc_size = SB::buildSizeOfOp(sty);
    }

    // Create a local variable which will hold the value of the new_exp
    // This variable will have the starting value for the new_exp.
    // The variable will also provide an address at which the metadata will
    // be stored.

    // Create a variable declaration
    // 1. create a local varible... newvar of nty
    SgName var_name("newvar_" + boost::lexical_cast<std::string>(Util::VarCounter++));
    SgVariableDeclaration* newvar = SB::buildVariableDeclaration(var_name, nty, NULL, SI::getScope(ne));
    SI::insertStatementBefore(SI::getEnclosingStatement(ne), newvar);

    // assign the new exp to newvar
    SgExpression* newvar_assign = SB::buildAssignOp(SB::buildVarRefExp(newvar), SI::copyExpression(ne));

    // build the params for create_entry_with_new_lock_gen
    SgExprListExp* plist = SB::buildExprListExp(Util::castToAddr(Util::createAddressOfOpFor(SB::buildVarRefExp(newvar))),
        SB::buildVarRefExp(newvar),
        SB::buildCastExp(alloc_size, Util::getSizeOfType(),SgCastExp::e_C_style_cast));

    // Now, call create_entry_with_new_lock_gen
    SgExpression* ovl = buildMultArgOverloadFn("create_entry_with_new_lock_gen", plist, retType,
        Util::getScopeForExp(ne),
        GEFD(SI::getEnclosingStatement(ne)));

    // Build a comma op exp with newvar_assign and ovl
    SgExpression* cop = SB::buildCommaOpExp(newvar_assign, ovl);

    // replace new_exp with comma op
    replaceWrapper(ne, cop);

#ifdef NEW_EXP_DEBUG
    printf("Done with handleNewExp\n");
#endif
  }
}
