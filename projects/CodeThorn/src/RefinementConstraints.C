#include "RefinementConstraints.h"

using namespace CodeThorn;

void RefinementConstraints::addConstraintsByLabel(Label label) {
  set<pair<VariableId, int> > newConstraints = getAtomicPropositionsFromConditions(label);
#if 1 // print the new constraints
  cout << "INFO: new constraints. Label " << _labeler->labelToString(label) << ":"; 
  for (set<pair<VariableId, int> >::iterator u = newConstraints.begin(); u != newConstraints.end(); ++u) {
    if (u != newConstraints.begin()) {
      cout << ",";
    }
    cout << "[" << (_varIdMapping->variableName(u->first)) << "<" << (u->second) << "]";
  }
  cout << endl;
#endif
  // add the newly discovered constraints to the maintained ones ("_constraintsLessThan")
  for (set<pair<VariableId, int> >::iterator i=newConstraints.begin(); i != newConstraints.end(); ++i) {
    VarToIntMap::iterator mapEntry = _constraintsLessThan.find(i->first);
    if (mapEntry != _constraintsLessThan.end()) { // some constraints for VariableId (*i) exist already
      (mapEntry->second).insert(i->second);
    } else { //create new map entry vor VariableId (*i)
      set<int> constraints;
      constraints.insert(i->second);
      _constraintsLessThan.insert(pair<VariableId, set<int> >(i->first, constraints));
    }
  }
}

set<int> RefinementConstraints::getConstraintsForVariable(VariableId varId) {
  set<int> result;
  VarToIntMap::iterator mapEntry = _constraintsLessThan.find(varId);
  if (mapEntry != _constraintsLessThan.end()) {
    result = mapEntry->second; 
  }
  return result;
}

set<RConstraint> RefinementConstraints::getAtomicPropositionsFromConditions(Label label) {
  set<RConstraint> result;
  Label* currentLabel = &label;
  bool allConditionsCollected = false;
  while (!allConditionsCollected) {
    SgNode* nodeToAnalyze=_cfanalyzer->getNode(*currentLabel);
    // Stop collecting constraints when either the "calculate_output" function or the "errorCheck" function is reached. 
    // All relevant conditions of the 2014 RERS programs are to be found within those function.
    if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nodeToAnalyze)) {
      assert(funCall);
      string funName=SgNodeHelper::getFunctionName(funCall);
      if(funName=="calculate_output") {
        allConditionsCollected = true;
      } else if (funName=="errorCheck") {
        allConditionsCollected = true;
      }
    }
    // collect constraints from conditions
    if(SgNodeHelper::isCond(nodeToAnalyze)) {
      //cout << "DEBUG: constraints to be collected from condition: " << SgNodeHelper::unparseCond(nodeToAnalyze)
      //     << "  (Label: " << _labeler->labelToString(*currentLabel) << ")" <<endl;
      SgNode* rootOfCondition;
      nodeToAnalyze=nodeToAnalyze->get_parent();
      if (SgIfStmt* ifstmt=isSgIfStmt(nodeToAnalyze)) {
        SgStatement* conditional = ifstmt->get_conditional();
        rootOfCondition = SgNodeHelper::getExprStmtChild(conditional);
      } else {
        cout << "ERROR: type of condition unsupported by the traversal of RefinementConstraints." << endl;
        assert(0);
      }
      if (!dynamic_cast<SgBinaryOp*>(rootOfCondition)) {
        cout << "ERROR: root of Sage condition is not a binary operation." << endl;
        assert(0); 
      }
      // add the atomic propositions of the condition to the set of constraints
      set<RConstraint> condConstraints = conditionToConstraints(dynamic_cast<SgBinaryOp*>(rootOfCondition));
      result.insert(condConstraints.begin(), condConstraints.end());
      //skip other conditions that have no effect on the occurence of "label"
      Flow edgeSet=_cfg->inEdges(*currentLabel);      
      while (edgeSet.size()>=2) {
        // follow the {forward, false} edge leading to the current condition that would be traversed the earliest
        // (during a forward traversal starting from the cfg start node)
        Label pred = *currentLabel;
        for (Flow::iterator i = edgeSet.begin(); i!=edgeSet.end(); i++) {
          Edge inEdge = *i;
          if( inEdge.isType(EDGE_FALSE) && (inEdge.source.getId() < pred.getId()) ) {
            pred = inEdge.source;
            bool isCond = SgNodeHelper::isCond(_labeler->getNode(pred));
            assert(isCond);
          }
        }
        currentLabel = &pred;
        edgeSet=_cfg->inEdges(*currentLabel);
      }
    }
    //proceed to the next label to be analyzed (predecessor in cfg)
    if (!allConditionsCollected) {
      Flow edgeSet=_cfg->inEdges(*currentLabel);
      // Apart from conditions, all cfg node within a RERS program's "calculate_output" and "errorCheck" functions
      // should have exactly one predeccesor.
      assert(edgeSet.size()==1);
      Flow::iterator predecessor = edgeSet.begin();
      Edge pred = *predecessor;
      currentLabel = &pred.source;
    }
  }
  return result;
}

set<RConstraint> RefinementConstraints::conditionToConstraints(SgBinaryOp* binOp) {
  set<RConstraint> result;
  SgNode* lhs=SgNodeHelper::getLhs(binOp);
  SgNode* rhs=SgNodeHelper::getRhs(binOp);
  switch(binOp->variantT()) {
        case V_SgEqualityOp: {
          if (isIntValue(lhs) && isIntValue(rhs)) { 
            // a condition with only constant values, for example due to constant propagation. no new constraints
          } else {
            pair<RConstraint, RConstraint> newConstraints = createEqualityConstraints(lhs, rhs);
            result.insert(newConstraints.first);
            result.insert(newConstraints.second);
          }
          break;
        }
        case V_SgNotEqualOp: {
          if (isIntValue(lhs) && isIntValue(rhs)) { 
             // a condition with only constant values, for example due to constant propagation. no new constraints
          } else {
            pair<RConstraint, RConstraint> newConstraints = createEqualityConstraints(lhs, rhs);
            result.insert(newConstraints.first);
            result.insert(newConstraints.second);
          }
          break;
        }
        case V_SgGreaterOrEqualOp: {
          pair<VariableId, int> newConstraint;
          VariableId varId;
          int intVal;
          // variable on the lhs and constant value on the rhs
          if(_exprAnalyzer->variable(lhs,varId) && isIntValue(rhs)) {
            intVal=getIntValue(rhs);
            newConstraint = createConstraint(varId, intVal, 0);
            result.insert(newConstraint);
          // constant value on the lhs and variable on the rhs
          } else if (isIntValue(lhs) && _exprAnalyzer->variable(rhs,varId)) {
            intVal=getIntValue(lhs);
            newConstraint = createConstraint(varId, intVal, 1);
            result.insert(newConstraint);
          } else if (isIntValue(lhs) && isIntValue(rhs)) { 
             // a condition with only constant values, for example due to constant propagation. no new constraints
          } else {
            cout << "ERROR: conditionToConstraints: unsupported type of atomic proposition. (GreaterOrEqual operator)" << endl;
            assert(0); 
          }        
          break;
        }
        case V_SgGreaterThanOp: {
          pair<VariableId, int> newConstraint;
          VariableId varId;
          int intVal;
          // variable on the lhs and constant value on the rhs
          if(_exprAnalyzer->variable(lhs,varId) && isIntValue(rhs)) {
            intVal=getIntValue(rhs);
            newConstraint = createConstraint(varId, intVal, 1);
            result.insert(newConstraint);
          // constant value on the lhs and variable on the rhs
          } else if (isIntValue(lhs) && _exprAnalyzer->variable(rhs,varId)) {
            intVal=getIntValue(lhs);
            newConstraint = createConstraint(varId, intVal, 0);
            result.insert(newConstraint);
          } else if (isIntValue(lhs) && isIntValue(rhs)) { 
             // a condition with only constant values, for example due to constant propagation. no new constraints
          } else {
            cout << "ERROR: conditionToConstraints: unsupported type of atomic proposition. (GreaterThan operator)" << endl;
            assert(0); 
          }
          break;
        }
        case V_SgLessThanOp: {
          pair<VariableId, int> newConstraint;
          VariableId varId;
          int intVal;
          // variable on the lhs and constant value on the rhs
          if(_exprAnalyzer->variable(lhs,varId) && isIntValue(rhs)) {
            intVal=getIntValue(rhs);
            newConstraint = createConstraint(varId, intVal, 0);
            result.insert(newConstraint);
          // constant value on the lhs and variable on the rhs
          } else if (isIntValue(lhs) && _exprAnalyzer->variable(rhs,varId)) {
            intVal=getIntValue(lhs);
            newConstraint = createConstraint(varId, intVal, 1);
            result.insert(newConstraint);
          } else if (isIntValue(lhs) && isIntValue(rhs)) { 
             // a condition with only constant values, for example due to constant propagation. no new constraints
          } else {
            cout << "ERROR: conditionToConstraints: unsupported type of atomic proposition. (LessThan operator)" << endl;
            assert(0); 
          }
          break;
        }
        case V_SgLessOrEqualOp: {
          pair<VariableId, int> newConstraint;
          VariableId varId;
          int intVal;
          // variable on the lhs and constant value on the rhs
          if(_exprAnalyzer->variable(lhs,varId) && isIntValue(rhs)) {
            intVal=getIntValue(rhs);
            newConstraint = createConstraint(varId, intVal, 1);
            result.insert(newConstraint);
          // constant value on the lhs and variable on the rhs
          } else if (isIntValue(lhs) && _exprAnalyzer->variable(rhs,varId)) {
            intVal=getIntValue(lhs);
            newConstraint = createConstraint(varId, intVal, 0);
            result.insert(newConstraint);
          } else if (isIntValue(lhs) && isIntValue(rhs)) { 
             // a condition with only constant values, for example due to constant propagation. no new constraints
          } else {
            cout << "ERROR: conditionToConstraints: unsupported type of atomic proposition. (LessOrEqual operator)" << endl;
            assert(0); 
          }
          break;
        }
        // simply collect all constraints in operands of "and" and "or" operators
        case V_SgAndOp: {
          if (!dynamic_cast<SgBinaryOp*>(lhs) || !dynamic_cast<SgBinaryOp*>(rhs)) {
            cout << "ERROR: conditionToConstraints: one or more children of sage AND node are not binary operations." << endl;
            assert(0); 
          }
          set<pair<VariableId, int> > lhsConstraints = conditionToConstraints(dynamic_cast<SgBinaryOp*>(lhs));
          set<pair<VariableId, int> > rhsConstraints = conditionToConstraints(dynamic_cast<SgBinaryOp*>(rhs));
          result.insert(lhsConstraints.begin(), lhsConstraints.end());
          result.insert(rhsConstraints.begin(), rhsConstraints.end());
          break;
        }
        case V_SgOrOp: {
          if (!dynamic_cast<SgBinaryOp*>(lhs) || !dynamic_cast<SgBinaryOp*>(rhs)) {
            cout << "ERROR: conditionToConstraints: one or more children of sage OR node are not binary operations." << endl;
            assert(0); 
          }
          set<pair<VariableId, int> > lhsConstraints = conditionToConstraints(dynamic_cast<SgBinaryOp*>(lhs));
          set<pair<VariableId, int> > rhsConstraints = conditionToConstraints(dynamic_cast<SgBinaryOp*>(rhs));
          result.insert(lhsConstraints.begin(), lhsConstraints.end());
          result.insert(rhsConstraints.begin(), rhsConstraints.end());
          break;
        }
        default: {
          cout << "ERROR: conditionToConstraints: unsupported node in condition." << endl;
          assert(0);
        }
  }
  return result;
}

pair<RConstraint, RConstraint> RefinementConstraints::createEqualityConstraints(SgNode* lhs, SgNode* rhs) {
  pair<RConstraint, RConstraint> result;
  pair<VariableId, int> lowerConstraint;
  pair<VariableId, int> upperConstraint;
  VariableId varId;
  int intVal;
  // variable on the lhs and constant value on the rhs
  if(_exprAnalyzer->variable(lhs,varId) && isIntValue(rhs)) {
    intVal=getIntValue(rhs);
  // constant value on the lhs and variable on the rhs
  } else if (isIntValue(lhs) && _exprAnalyzer->variable(rhs,varId)) {
    intVal=getIntValue(lhs);
  } else {
    cout << "ERROR: conditionToConstraints: unsupported type of atomic proposition. (equality operator)" << endl;
    assert(0); 
  }
  // add two new constraints: one for the less than the checked value and one for less than (checked value + 1).
  lowerConstraint.second = intVal;
  lowerConstraint.first = varId;
  result.first = lowerConstraint;
  upperConstraint.second = (intVal + 1);
  upperConstraint.first = varId;
  result.second = upperConstraint;
  return result;
}

bool RefinementConstraints::isIntValue(SgNode* node) {
  if(dynamic_cast<SgUnaryOp*>(node)) {
    if (node->variantT() == V_SgMinusOp) {
      SgNode* child=SgNodeHelper::getFirstChild(node);
      if (child->variantT() == V_SgIntVal) {
        return true;
      }
    }
  }
  if (node->variantT() == V_SgIntVal) {
    return true;
  }
  return false;
}

int RefinementConstraints::getIntValue(SgNode* node) {
  if(dynamic_cast<SgUnaryOp*>(node)) {
    if (node->variantT() == V_SgMinusOp) {
      SgNode* child=SgNodeHelper::getFirstChild(node);
      if (SgIntVal* intValNode=isSgIntVal(child)) {
        return ( - (intValNode->get_value()) );
      }
    }
  }
  if (SgIntVal* intValNode=isSgIntVal(node)) {
    return intValNode->get_value();
  }
  cout << "ERROR: getIntValue called on an SgNode* that does not represent such a value." << endl;
  assert(0);
  return -1;
}

RConstraint RefinementConstraints::createConstraint(VariableId varId, int val, int offsetFromVal) {
  pair<VariableId, int> result;
  result.first = varId;
  result.second = (val+offsetFromVal);
  return result;
}

