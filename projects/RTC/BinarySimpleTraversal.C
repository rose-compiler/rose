#ifndef BINARYSIMPLETRAVERSAL_C
#define BINARYSIMPLETRAVERSAL_C
#include "BinarySimpleTraversal.h"
#include "string.h"
#include "sageGeneric.h"

using namespace SageInterface;
using namespace SageBuilder;

void BinarySimpleTraversal::nodeCollection(SgProject* project) {

  //traverseInputFiles(project, postorder);

  traverse(project, postorder);
}

bool BinarySimpleTraversal::isUseful(SgBinaryOp* Bop) {

  switch(Bop->variantT()) {
  case V_SgAddOp: printf("AddOp\n"); return true;
  case V_SgSubtractOp: printf("SubOp\n"); return true;
  case V_SgMultiplyOp: printf("MultOp\n"); return true;
  case V_SgDivideOp: printf("DivOp\n"); return true;
  default: return false;
  }

    assert(0);
}


void BinarySimpleTraversal::atTraversalStart() {
  SelectedNodes.clear();
}

void BinarySimpleTraversal::atTraversalEnd() {
    TraversalBase::processBinaryOps(SelectedNodes);
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


#if 0
static
SgType* skip_PointerType(SgType* t)
{
  SgPointerType* sgptr = isSgPointerType(t);
  if (sgptr != NULL) return sgptr->get_base_type();

  return t;
}

static
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

static
SgType* resolveTypedefsAndReferencesToBaseTypes(SgType* type) {
  printf("resolve\n");
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

static
SgType* skip_RefsPointersAndTypedefs(SgType* type) {
  printf("skip_\n");
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
#endif

// This segfaults ---- WHY????
extern SgType* resolveTypedefsAndReferencesToBaseTypes(SgType* type);
extern SgType* skip_RefsPointersAndTypedefs(SgType* type);

void BinarySimpleTraversal::visit(SgNode* node) {

    printf("node: %s = %s\n", isSgNode(node)->sage_class_name(), isSgNode(node)->unparseToString().c_str());

  if(isSgFunctionCallExp(node)) {
    // FIXME: Function Declaration of the function may not available if its a late binding function
    SgFunctionDeclaration* funcDecl = isSgFunctionCallExp(node)->getAssociatedFunctionDeclaration();
    if(funcDecl) {
      printf("name: %s\n", funcDecl->get_name().str());
      if(strcmp(funcDecl->get_name().str(),"malloc") == 0) {
        printf("Found malloc\n");
      }
    }
  }

  // Use this to find malloc calls
  if(isSgFunctionRefExp(node)) {
    SgFunctionDeclaration* funcRef = isSgFunctionRefExp(node)->getAssociatedFunctionDeclaration();
    printf("name: %s\n", funcRef->get_name().str());
    if(strcmp(funcRef->get_name().str(),"malloc") == 0) {
      printf("Found malloc -- 2\n");
      TraversalBase::MallocRefs.push_back(node);
    }
  }

  // Find SgInitializedNames of pointers
  if(isSgInitializedName(node)) {
    // (i) if its a pointer
    // (ii) if its a pointer of the type that we want
    SgInitializedName* name = isSgInitializedName(node);
    SgType* baseType;

    if(name->get_type()->variantT() == V_SgPointerType) {
      printf("Found pointer.\n");
      printf("var: %s\n", name->get_name().str());
      baseType = resolveTypedefsAndReferencesToBaseTypes(name->get_type());
      baseType = skip_RefsPointersAndTypedefs(name->get_type());
      printf("baseType: %s\n", baseType->class_name().c_str());

      // Currently, we only support unsigned int type pointers
      if(baseType->variantT() != V_SgTypeUnsignedInt) {
        printf("Not Unsigned Int\n");
      }
      else {
        printf("VarNames - Found Unsigned Int Pointer\n");
        TraversalBase::VarNames.push_back(node);
        printf("VarNames.size() : %u\n", TraversalBase::VarNames.size());
      }
    }
    else {
      printf("Not a pointer\n");
      printf("var: %s\n", name->get_name().str());
    }
  }

  // Similar to the SgInitializedName approach above to find the declarations of pointers
  // we could also look for the uses of those pointers
  if(isSgVarRefExp(node)) {
    // (i) if its a pointer
    // (ii) if its a pointer of the type that we want
    SgType* varType = isSgVarRefExp(node)->get_type();
    SgType* baseType;

    if(varType->variantT() == V_SgPointerType) {
      printf("Found pointer.\n");
      printf("var: %s\n", isSgVarRefExp(node)->get_symbol()->get_name().str());
      baseType = resolveTypedefsAndReferencesToBaseTypes(varType);
      baseType = skip_RefsPointersAndTypedefs(varType);
      printf("baseType: %s\n", baseType->class_name().c_str());

      // Currently, we only support unsigned int type pointers
      if(baseType->variantT() != V_SgTypeUnsignedInt) {
        printf("Not Unsigned Int\n");
      }
      else {
        printf("Found Unsigned Int Pointer\n");
        TraversalBase::VarRefs.push_back(node);
      }
    }
    else {
      printf("Not a pointer\n");
      printf("var: %s\n", isSgVarRefExp(node)->get_symbol()->get_name().str());
    }

  }

  // Catch pointer deref operators
  if(isSgPointerDerefExp(node)) {
    SgType* type = isSgPointerDerefExp(node)->get_type();
    if(type->variantT() != V_SgTypeUnsignedInt) {
      printf("Not Unsigned Int\n");
    }
    else {
      printf("Found Unsigned Int Pointer Deref\n");

      // Lets find the exact pointer
      SgUnaryOp* uop = isSgUnaryOp(node);
      SgExpression* operand = uop->get_operand();
      printf("node: %s = %s\n", isSgNode(operand)->sage_class_name(), isSgNode(operand)->unparseToString().c_str());
    }
  }

  // Catch unary and binary operators using a general rule
  // It looks like there isn't a particular rule that we could apply to these expressions. Instead, we would have to
  // use a bottom up traversal with an evaluated attribute to find out when to insert a function call. That is the
  // the best way to do it I guess.
  // We could start doing all the above stuff -- and then finally, replacing, with a bottom up traversal, all the
  // operations involving struct_type nodes, with function calls



   if(isSgBinaryOp(node)) {
    if(isUseful(isSgBinaryOp(node))) {
      SelectedNodes.push_back(isSgBinaryOp(node));
    }
   }
}


void BinarySimpleTraversal::nodeInstrumentation(SgProject* project) {
  TraversalBase::naiveInstrumentation();
}


#endif
