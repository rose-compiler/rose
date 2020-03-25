
#include "sage3basic.h"
#include <iostream>
#include "CastTransformer.h"
#include "SgNodeHelper.h"
#include "TFTypeTransformer.h" // only required for trace

using namespace std;

bool CastTransformer::isLessPrecise(SgType* t1, SgType* t2) {
  ROSE_ASSERT(isFloatingPointType(t1));
  ROSE_ASSERT(isFloatingPointType(t2));
  return((isSgTypeFloat(t1)&&isSgTypeDouble(t2))
         ||(isSgTypeFloat(t1)&&isSgTypeLongDouble(t2))
         ||(isSgTypeDouble(t1)&&isSgTypeLongDouble(t2)));
}

bool CastTransformer::isEqualPrecise(SgType* t1, SgType* t2) {
  return t1==t2;
}

bool CastTransformer::isLessOrEqualPrecise(SgType* t1, SgType* t2) {
  return isEqualPrecise(t1,t2) || isLessPrecise(t1,t2);
}

bool CastTransformer::isFloatingPointType(SgType* type) {
  return isSgTypeFloat(type)||isSgTypeDouble(type)||isSgTypeLongDouble(type);
}

bool CastTransformer::castIsNecessary(SgType* source, SgType* target) {
  ROSE_ASSERT(isFloatingPointType(source));
  ROSE_ASSERT(isFloatingPointType(target));
  return isLessPrecise(target,source);
}

void CastTransformer::eliminateCast(SgCastExp* castExp) {
  TFTypeTransformer::trace("eliminating cast:"+SgNodeHelper::sourceLineColumnToString(castExp)+":"+castExp->unparseToString());
  SgExpression* oldExp=castExp;
  SgExpression* newExp=castExp->get_operand();
  bool keepOldExp=true;
  SgNodeHelper::replaceExpression(oldExp, newExp, keepOldExp);
}

void CastTransformer::introduceCast(SgExpression* exp, SgType* type) {
  TFTypeTransformer::trace("introducing cast @:"+SgNodeHelper::sourceLineColumnToString(exp)+":"+exp->unparseToString());
  SgNode* parentOfExpBackup=exp->get_parent();
  SgCastExp* newCastExp=SageBuilder::buildCastExp(exp,type);
  // set parent back to original value, because buildCastExp modifies
  // the parent in the existing AST and this conflicts with the replacements of the original node
  exp->set_parent(parentOfExpBackup); 

  // replace original exp (which is now a child of the new cast exp)
  // with the new cast exp.
  bool keepOldExp=false;
  SgNodeHelper::replaceExpression(exp, newCastExp, keepOldExp);
  // set parent of exp now
  exp->set_parent(newCastExp);
}

void CastTransformer::changeCast(SgCastExp* castExp, SgType* newType) {
  TFTypeTransformer::trace("changing cast :"
        +SgNodeHelper::sourceLineColumnToString(castExp)
        +":"+castExp->unparseToString()+" TO "+newType->unparseToString());
}

bool CastTransformer::isOutsideTemplate(CTInheritedAttributeType inh) {
  return !inh; 
}

CTSynthesizedAttributeType
CastTransformer::defaultSynthesizedAttribute(CTInheritedAttributeType inh) {
  return nullptr;
}

CTInheritedAttributeType
CastTransformer::evaluateInheritedAttribute (SgNode* astNode, CTInheritedAttributeType inheritedAttr) {
  if(isSgTemplateClassDefinition(astNode)||isSgTemplateFunctionDefinition(astNode)) {
    return true;
  } else {
    return inheritedAttr;
  }
}

CTSynthesizedAttributeType CastTransformer::evaluateSynthesizedAttribute (SgNode* node, CTInheritedAttributeType inhAttr, SubTreeSynthesizedAttributes synAttrList) {
  CTSynthesizedAttributeType syn=nullptr;
  if(isOutsideTemplate(inhAttr)) {
    if(isSgBinaryOp(node)) {
      //cout<<"Found binary op:"<<node->class_name()<<endl;
    }
    if(SgCastExp* castExp=isSgCastExp(node)) {
      if(SgExpression* targetExp=isSgExpression(castExp->get_parent())) {
        SgType* targetType=targetExp->get_type();
        SgType* sourceType=isSgExpression(SgNodeHelper::getUnaryOpChild(castExp))->get_type();
        if(isFloatingPointType(sourceType)&&isFloatingPointType(targetType)) {
          if(castIsNecessary(sourceType,targetType)) {
            if(targetType!=castExp->get_type()) {
              changeCast(castExp,targetType);
            }
          } else {
            eliminateCast(castExp);
          }
        }
      }
    } else {
#if 1
      if(SgExpression* exp=isSgExpression(node)) {
        if(SgExpression* targetExp=isSgExpression(exp->get_parent())) {
          SgType* targetType=targetExp->get_type();
          SgType* sourceType=exp->get_type();
          if(isFloatingPointType(sourceType)&&isFloatingPointType(targetType)) {
            if(castIsNecessary(sourceType,targetType)) {
              introduceCast(exp,targetType);
            }
          }
        }
      }
#endif
    }
#if 0
    if(SgVarRefExp* var=isSgVarRefExp(node)) {
      cout<<"Found VarRefExp: "<<var->unparseToString()<<" : "<<var->get_type()->unparseToString()<<endl;
    }
#endif
  }
  return syn;
}

void CastTransformer::transformCommandLineFiles(SgProject* project) {
  traverseWithinCommandLineFiles(project);
}

void CastTransformer::traverseWithinCommandLineFiles(SgProject* project) {
  // attribute used to avoid traversing template definitions
  // true means the traversal is inside some template definition
  CTInheritedAttributeType inh=false;
  CastTransformer::traverseWithinCommandLineFiles(project,inh);
}

void CastTransformer::traverseWithinCommandLineFiles(SgProject* project, CTInheritedAttributeType inh) {
  traverseInputFiles(project,inh);
}

