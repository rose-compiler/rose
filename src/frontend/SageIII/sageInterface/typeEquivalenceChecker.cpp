
#include "typeEquivalenceChecker.hpp"

TypeEquivalenceChecker::TypeEquivalenceChecker(bool profile): profile_(profile),
  namedType_(0), pointerType_(0),
  arrayType_(0), functionType_(0) {
}

bool
TypeEquivalenceChecker::typesAreEqual(SgType *t1, SgType *t2) {
  bool equal = false;
  if(t1 == NULL || t2 == NULL){
    return equal;
  }
  RoseAst subT1(t1);
  RoseAst subT2(t2);

  for (RoseAst::iterator i = subT1.begin(), j = subT2.begin();
       i != subT1.end(), j != subT2.end(); ++i, ++j) {
    SgNode *nodeT1 = *i;
    SgNode *nodeT2 = *j;

//    std::cout << "nodeT1: " << nodeT1->class_name() << " nodeT2: " << nodeT2->class_name() << std::endl;

    if (isSgTypedefType(nodeT1)) {
      std::cout << "This is a typedef nodeT1. We strip everything away and compare the hidden types." << std::endl;
      nodeT1 = isSgTypedefType(nodeT1)->stripType(SgType::STRIP_TYPEDEF_TYPE);
    }
    if (isSgTypedefType(nodeT2)) {
      std::cout << "This is a typedef nodeT2. We strip everything away and compare the hidden types." << std::endl;
      nodeT2 = isSgTypedefType(nodeT2)->stripType(SgType::STRIP_TYPEDEF_TYPE);
    }

    if (nodeT1->variantT() == nodeT2->variantT()) {

      if (isSgNamedType(nodeT1)) {      // Two different names -> Must be two different things
        if (profile_) {
          namedType_++;
        }
        i.skipChildrenOnForward();
        j.skipChildrenOnForward();
        SgNamedType *c1 = isSgNamedType(nodeT1);
        SgNamedType *c2 = isSgNamedType(nodeT2);

//        std::cout << c1->get_qualified_name() << std::endl;
        // XXX A function to check whether a named type is anonymous or not would speed
        // up this check, since we could get rid of this string compare.
//        if (c1->get_qualified_name().getString().find("__anonymous_") != std::string::npos) {
        if(!c1->get_autonomous_declaration()){
          return false;
        }
        if (!c2->get_autonomous_declaration()){
          return false;
        }
        if (c1->get_qualified_name() == c2->get_qualified_name()) {
          return true;
        } else {
          return false;
        }

      } else if (isSgPointerType(nodeT1)) {
        if (profile_) {
          pointerType_++;
        }
        SgPointerType *t1 = isSgPointerType(nodeT1);
        SgPointerType *t2 = isSgPointerType(nodeT2);

        return typesAreEqual(t1->get_base_type(), t2->get_base_type());

      } else if(isSgReferenceType(nodeT1)){
        SgReferenceType *t1 = isSgReferenceType(nodeT1);
        SgReferenceType *t2 = isSgReferenceType(nodeT2);

        return typesAreEqual(t1->get_base_type(), t2->get_base_type());
      } else if (isSgArrayType(nodeT1)) {
        if (profile_) {
          arrayType_++;
        }
        SgArrayType *a1 = isSgArrayType(nodeT1);
        SgArrayType *a2 = isSgArrayType(nodeT2);

        bool arrayBaseIsEqual =	typesAreEqual(a1->get_base_type(), a2->get_base_type());

        SimpleExpressionEvaluator evalA, evalB;

        bool arrayIndexExpressionIsEquivalent =	(evalA.traverse(a1->get_index()) == evalB.traverse(a2->get_index()));
        bool arraysAreEqual = (arrayBaseIsEqual && arrayIndexExpressionIsEquivalent);
        return arraysAreEqual;
      } else if (isSgFunctionType(nodeT1)) {
        if(profile_) {
          functionType_++;
        }
        SgFunctionType *funcTypeA = isSgFunctionType(nodeT1);
        SgFunctionType *funcTypeB = isSgFunctionType(nodeT2);
        if(typesAreEqual(funcTypeA->get_return_type(), funcTypeB->get_return_type())) {
          // If functions don't have the same number of arguments, they are not type-equal
          if(funcTypeA->get_arguments().size() != funcTypeB->get_arguments().size()) {
            return false;
          }
          for(SgTypePtrList::const_iterator ii = funcTypeA->get_arguments().begin(),
              jj = funcTypeB->get_arguments().begin();
              ii != funcTypeA->get_arguments().end(),
              jj != funcTypeB->get_arguments().end();
              ++ii, ++jj) {
            // For all argument types check whether they are equal
            if(!typesAreEqual((*ii), (*jj))) {
              return false;
            }
          }
          return true;
        }
        return false;
      } else {
        // We don't have a named type, pointer type or array type, so they are equal
        // This is for the primitive type - case
        return true;
      }
    } else {
      // In this case the types are not equal, since its variantT is not equal.
      return false;
    }
  }
  // this should be unreachable code...
  return equal;
}

int TypeEquivalenceChecker::getNamedTypeCount() {
  return namedType_;
}

int TypeEquivalenceChecker::getPointerTypeCount() {
  return pointerType_;
}

int TypeEquivalenceChecker::getArrayTypeCount() {
  return arrayType_;
}

int TypeEquivalenceChecker::getFunctionTypeCount() {
  return functionType_;
}


bool SageInterface::checkTypesAreEqual(SgType *typeA, SgType *typeB) {

  TypeEquivalenceChecker tec(false);

  return tec.typesAreEqual(typeA, typeB);

}
