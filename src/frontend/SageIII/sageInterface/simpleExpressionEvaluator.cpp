#include "simpleExpressionEvaluator.hpp"

EvaluatorExpressionRepresentation::EvaluatorExpressionRepresentation(): value_
  (-1),
  evalutable_
  (false) 
{
}

EvaluatorExpressionRepresentation::EvaluatorExpressionRepresentation(long long value):
  value_(value),
  evalutable_(true) 
{
}

bool EvaluatorExpressionRepresentation::isEvalutable() {
  return evalutable_;
}

void EvaluatorExpressionRepresentation::setEvalutable(bool evalutable) {
  evalutable_ = evalutable;
}

long long EvaluatorExpressionRepresentation::getValue() {
  return value_;
}

void EvaluatorExpressionRepresentation::setValue(long long value) {
  value_ = value;
}

bool EvaluatorExpressionRepresentation::operator==(const EvaluatorExpressionRepresentation &eer) {
  return (value_ == eer.value_) && (evalutable_ && eer.evalutable_);
}


SimpleExpressionEvaluator::SimpleExpressionEvaluator() {
}

#if 0
EvaluatorExpressionRepresentation
SimpleExpressionEvaluator::getExpressionValue() {
  return EvaluatorExpressionRepresentation();
}
#endif
EvaluatorExpressionRepresentation SimpleExpressionEvaluator::getValueExpressionValue(SgValueExp *valExp) {
  long long subtreeVal = 0;

  if (isSgIntVal(valExp)) {
    subtreeVal = isSgIntVal(valExp)->get_value();
  } else if (isSgLongIntVal(valExp)) {
    subtreeVal = isSgLongIntVal(valExp)->get_value();
  } else if (isSgLongLongIntVal(valExp)) {
    subtreeVal = isSgLongLongIntVal(valExp)->get_value();
  } else if (isSgShortVal(valExp)) {
    subtreeVal = isSgShortVal(valExp)->get_value();
  } else if (isSgUnsignedIntVal(valExp)) {
    subtreeVal = isSgUnsignedIntVal(valExp)->get_value();
  } else if (isSgUnsignedLongVal(valExp)) {
    subtreeVal = isSgUnsignedLongVal(valExp)->get_value();
  } else if (isSgUnsignedLongLongIntVal(valExp)) {
    subtreeVal = isSgUnsignedLongLongIntVal(valExp)->get_value();
  } else if (isSgUnsignedShortVal(valExp)) {
    subtreeVal = isSgUnsignedShortVal(valExp)->get_value();
  }
  return EvaluatorExpressionRepresentation(subtreeVal);
}

EvaluatorExpressionRepresentation SimpleExpressionEvaluator::evaluateVariableReference(SgVarRefExp *vRef) {
  if (isSgModifierType(vRef->get_type()) == NULL) {
    return EvaluatorExpressionRepresentation();
  }
  if (isSgModifierType(vRef->get_type())->get_typeModifier().get_constVolatileModifier().isConst()) {
    // We know that the var value is const, so get the initialized name and evaluate it
    SgVariableSymbol *sym = vRef->get_symbol();
    SgInitializedName *iName = sym->get_declaration();
    SgInitializer *ini = iName->get_initializer();

    if (isSgAssignInitializer(ini)) {
      SgAssignInitializer *initializer = isSgAssignInitializer(ini);
      SgExpression *rhs = initializer->get_operand();
      SimpleExpressionEvaluator variableEval;

      return variableEval.traverse(rhs);
    }
  }
}


EvaluatorExpressionRepresentation SimpleExpressionEvaluator::evaluateSynthesizedAttribute(SgNode *node, SynthesizedAttributesList synList) {
  if (isSgExpression(node)) {
    if (isSgValueExp(node)) {
      return this->getValueExpressionValue(isSgValueExp(node));
    }

    if (isSgVarRefExp(node)) {
//      std::cout << "Hit variable reference expression!" << std::endl;
      return evaluateVariableReference(isSgVarRefExp(node));
    }
    // Early break out for assign initializer // other possibility?
    if (isSgAssignInitializer(node)) {
      if (synList.at(0).isEvalutable()) {
//        std::cout << "Returning an evaluated value of: " << synList.at(0).getValue() << " from a SgAssignInitializer node" << std::endl;
        return EvaluatorExpressionRepresentation(synList.at(0).getValue());
      } else {
        return EvaluatorExpressionRepresentation();
      }
    }

    long long evaluatedValue = 0;
//    std::cout << "Node: " << node->class_name() << ":" << synList.size() << std::endl;

//    assert(synList.size() == 2);
#if 0
    if(synList.size() != 2){
      for(SynthesizedAttributesList::iterator it = synList.begin(); it != synList.end(); ++it){
        std::cout << "Node: " << node->unparseToString() << "\n" << (*it).getValue() << std::endl;
        std::cout << "Parent: " << node->get_parent()->unparseToString() << std::endl;
        std::cout << "Parent, Parent: " << node->get_parent()->get_parent()->unparseToString() << std::endl;
      }
    }
#endif
    for (SynthesizedAttributesList::iterator it = synList.begin(); it != synList.end(); ++it) {
      if ((*it).isEvalutable()) {
        if (isSgAddOp(node)) {
          assert(synList.size() == 2);
          evaluatedValue = synList[0].getValue() + synList[1].getValue() ;
        } else if (isSgSubtractOp(node)) {
          assert(synList.size() == 2);
          evaluatedValue = synList[0].getValue()  - synList[1].getValue() ;
        } else if (isSgMultiplyOp(node)) {
          assert(synList.size() == 2);
          evaluatedValue = synList[0].getValue()  * synList[1].getValue() ;
        } else if (isSgDivideOp(node)) {
          assert(synList.size() == 2);
          evaluatedValue = synList[0].getValue()  / synList[1].getValue() ;
        } else if (isSgModOp(node)) {
          assert(synList.size() == 2);
          evaluatedValue = synList[0].getValue()  % synList[1].getValue() ;
        }
      } else {
        std::cerr << "Expression is not evaluatable" << std::endl;
        return EvaluatorExpressionRepresentation();
      }
    }
//    std::cout << "Returning evaluated expression with value: " << evaluatedValue << std::endl;
    return EvaluatorExpressionRepresentation(evaluatedValue);

  }
  return EvaluatorExpressionRepresentation();
}

namespace SageInterface {
  long long evaluateSimpleIntegerExpression(SgExpression *expr){
    SimpleExpressionEvaluator eval;
    return eval.traverse(expr).getValue();
  }
}

#ifdef WITH_MAIN
int
main(int argc, char **argv) {

  SimpleExpressionEvaluator eval;
  SgProject *project = frontend(argc, argv);

  eval.traverse(project);

  return 0;
}
#endif
