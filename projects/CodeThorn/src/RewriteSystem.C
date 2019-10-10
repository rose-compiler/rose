#include "sage3basic.h"
#include "RewriteSystem.h"
#include "TimeMeasurement.h"
#include "AstTerm.h"
#include "RoseAst.h"
#include "SgNodeHelper.h"
#include <list>

using namespace std;
using namespace CodeThorn;
using namespace Sawyer::Message;

Sawyer::Message::Facility RewriteSystem::logger;

RewriteSystem::RewriteSystem():_trace(false),_rewriteCondStmt(false) {
}

void RewriteSystem::setTrace(bool trace) {
  _trace=trace;
}

bool RewriteSystem::getTrace() {
  return _trace;
}

RewriteStatistics RewriteSystem::getRewriteStatistics() {
  return _rewriteStatistics;
}

RewriteStatistics RewriteSystem::getStatistics() {
  return _rewriteStatistics;
}

void RewriteSystem::resetStatistics() {
  _rewriteStatistics.reset();
}

void RewriteSystem::initDiagnostics() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::RewriteSystem", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

void RewriteSystem::rewriteCompoundAssignmentsInAst(SgNode* root, VariableIdMapping* variableIdMapping) {
  RoseAst ast(root);
  typedef list<SgCompoundAssignOp*> AssignOpListType;
  AssignOpListType assignOpList;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgCompoundAssignOp* compoundAssignOp=isSgCompoundAssignOp(*i)) {
      assignOpList.push_back(compoundAssignOp);
    }
  }
  size_t assignOpNum=assignOpList.size();
  logger[INFO] <<"transforming "<<assignOpNum<<" compound assignment expressions: started."<<endl;
  size_t assignOpNr=1;
  TimeMeasurement timer;
  double buildTime=0.0, replaceTime=0.0;
  for(AssignOpListType::iterator i=assignOpList.begin();i!=assignOpList.end();++i) {
    //cout<<"INFO: normalizing compound assign op "<<assignOpNr<<" of "<<assignOpNum<<endl;
    timer.start();
    SgExpression* newRoot=isSgExpression(buildRewriteCompoundAssignment(*i,variableIdMapping));
    buildTime+=timer.getTimeDuration().milliSeconds();

    if(newRoot) {
      timer.start();
      SgNodeHelper::replaceExpression(*i,newRoot);
      replaceTime+=timer.getTimeDuration().milliSeconds();
      assignOpNr++;
    } else {
      logger[WARN]<<"not an expression. transformation not applied: "<<(*i)->class_name()<<":"<<(*i)->unparseToString()<<endl;
    }
    //cout<<"Buildtime: "<<buildTime<<" Replacetime: "<<replaceTime<<endl;
  }
  logger[INFO]<<"transforming "<<assignOpNum<<" compound assignment expressions: done."<<endl;
}

SgNode* RewriteSystem::buildRewriteCompoundAssignment(SgNode* root, VariableIdMapping* variableIdMapping) {
  // Rewrite-rule 0: $Left OP= $Right => $Left = $Left OP $Right
  if(isSgCompoundAssignOp(root)) {
    _rewriteStatistics.numElimCompoundAssignOperator++;
    SgExpression* lhsCopy=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getLhs(root)));
    SgExpression* lhsCopy2=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getLhs(root)));
    SgExpression* rhsCopy=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getRhs(root)));
    SgExpression* newExp=0;
    //TODO: check whether build functions set parent pointers
    switch(root->variantT()) {
    case V_SgPlusAssignOp:
      newExp=SageBuilder::buildAddOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgDivAssignOp:
      newExp=SageBuilder::buildDivideOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgMinusAssignOp:
      newExp=SageBuilder::buildSubtractOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgMultAssignOp:
      newExp=SageBuilder::buildMultiplyOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgModAssignOp:
      newExp=SageBuilder::buildModOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgIorAssignOp: // bitwise or
      newExp=SageBuilder::buildBitOrOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgAndAssignOp: // bitwise and
      newExp=SageBuilder::buildBitAndOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgXorAssignOp:
      newExp=SageBuilder::buildBitXorOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgLshiftAssignOp:
      newExp=SageBuilder::buildLshiftOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    case V_SgRshiftAssignOp:
      newExp=SageBuilder::buildRshiftOp(lhsCopy,rhsCopy);
      return SageBuilder::buildAssignOp(lhsCopy2,newExp);
    default: /* ignore all other cases - all other expr remain unmodified */
      return 0;
    }
  }
  return 0;
}

void RewriteSystem::rewriteCompoundAssignments(SgNode*& root, VariableIdMapping* variableIdMapping) {
#if 1
  SgNode* newRoot=buildRewriteCompoundAssignment(root,variableIdMapping);
  if(newRoot)
    root=newRoot;
#else
  // Rewrite-rule 0: $Left OP= $Right => $Left = $Left OP $Right
  if(isSgCompoundAssignOp(root)) {
    _rewriteStatistics.numElimCompoundAssignOperator++;
    SgExpression* lhsCopy=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getLhs(root)));
    SgExpression* lhsCopy2=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getLhs(root)));
    SgExpression* rhsCopy=SageInterface::copyExpression(isSgExpression(SgNodeHelper::getRhs(root)));
    SgExpression* newExp;
    //TODO: check whether build functions set parent pointers
    switch(root->variantT()) {
    case V_SgPlusAssignOp:
      newExp=SageBuilder::buildAddOp(lhsCopy,rhsCopy);
      root=SageBuilder::buildAssignOp(lhsCopy2,newExp);
      break;
    case V_SgDivAssignOp:
      newExp=SageBuilder::buildDivideOp(lhsCopy,rhsCopy);
      root=SageBuilder::buildAssignOp(lhsCopy2,newExp);
      break;
    case V_SgMinusAssignOp:
      newExp=SageBuilder::buildSubtractOp(lhsCopy,rhsCopy);
      root=SageBuilder::buildAssignOp(lhsCopy2,newExp);
      break;
    case V_SgMultAssignOp:
      newExp=SageBuilder::buildMultiplyOp(lhsCopy,rhsCopy);
      root=SageBuilder::buildAssignOp(lhsCopy2,newExp);
      break;
    default: /* ignore all other cases - all other expr remain unmodified */
      ;
    }
  }
  //  if(n>0)
  //  cout<<"REWRITE:"<<root->unparseToString()<<endl;
#endif
}

void RewriteSystem::rewriteCondStmtInAst(SgNode* root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgNodeHelper::isCondStmt(*i)) {
      rewriteCondStmt(*i);
    }
  }
}

void RewriteSystem::rewriteCondStmt(SgNode* condStmt) {
  if(SgNodeHelper::isCondStmt(condStmt) /*&& !isSgSwitchStatement(condStmt)*/) {
    ROSE_ASSERT(isSgStatement(condStmt));
    SgNode* cond=SgNodeHelper::getCond(condStmt);
    //cout<<"DEBUG: cond:"<<cond->unparseToString()<<endl;
    //SgStatementExpression* normalizedCond=SageBuilder::buildStatementExpression(isSgStatement(cond));
    SgStatement* normalizedCond=SageBuilder::buildBasicBlock(isSgStatement(cond));
    //cout<<"DEBUG: condStmt:"<<condStmt->unparseToString()<<":"<<condStmt->class_name()<<endl;
    //cout<<"DEBUG: gnu     :"<<gnuStmtExpr->unparseToString()<<endl;
    SgNodeHelper::setCond(isSgStatement(condStmt),normalizedCond);
  }
}

void RewriteSystem::setRewriteCondStmt(bool flag) {
  _rewriteCondStmt=flag;
}

bool RewriteSystem::getRewriteCondStmt() {
  return _rewriteCondStmt;
}

void RewriteSystem::normalizeFloatingPointNumbersForUnparsing(SgNode*& root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    // due to lack of a common base class this testing has to be done for each class
    if(SgFloatVal* val=isSgFloatVal(*i)) {
      if(val->get_valueString()!="") {
        val->set_valueString("");
      }
    } else if(SgDoubleVal* val=isSgDoubleVal(*i)) {
      if(val->get_valueString()!="") {
        val->set_valueString("");
      }
    } else if(SgLongDoubleVal* val=isSgLongDoubleVal(*i)) {
      if(val->get_valueString()!="") {
        val->set_valueString("");
      }
    }
  }
}

// returns true for required swap, false if no swap is required
bool requiresSwap(SgExpression* lhs, SgExpression* rhs, VariableIdMapping* variableIdMapping) {
  return AstTerm::astTermWithNullValuesToString(lhs)>AstTerm::astTermWithNullValuesToString(rhs);
}

void RewriteSystem::establishCommutativeOrder(SgNode*& root, VariableIdMapping* variableIdMapping) {
  RoseAst ast(root);
  list<SgNode*> nodes;
  // prepare reverse pre-order
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    nodes.push_front(*i);
  }
  // perform rewrite on reverse pre-order
  for(list<SgNode*>::iterator i=nodes.begin();i!=nodes.end();++i) {
    if(isSgAddOp(*i)||isSgMultiplyOp(*i)) {
      SgBinaryOp* op=isSgBinaryOp(*i);
      ROSE_ASSERT(op);
      SgExpression* lhs=op->get_lhs_operand();
      SgExpression* rhs=op->get_rhs_operand();
      if(requiresSwap(lhs,rhs,variableIdMapping)) {
        _rewriteStatistics.numCommutativeSwap++;
        //cout<<"DEBUG: commutative swap "<<_rewriteStatistics.numCommutativeSwap<<" "<<op->class_name()<<": @"<<op<<": "<<op->unparseToString()<<" ==> ";
        // swap lhs and rhs
        op->set_lhs_operand(rhs);
        op->set_rhs_operand(lhs);
        //cout<<op->unparseToString()<<endl;
      } else {
        //cout<<"DEBUG: NOT swapping: "<<op->unparseToString()<<endl;
      }
    }
  }
}

bool isValueOne(SgExpression* valueNode) {
  if(SgIntVal* val=isSgIntVal(valueNode)) {
    int value=val->get_value();
    if(value==1) {
      return true;
    }
  } else if(SgFloatVal* val=isSgFloatVal(valueNode)) {
    float value=val->get_value();
    if(value==1.0f) {
      return true;
    }
  } else if(SgDoubleVal* val=isSgDoubleVal(valueNode)) {
    double value=val->get_value();
    if(value==1.0d) {
      return true;
    }
  }
  return false;
}    

bool isValueZero(SgExpression* valueNode) {
  if(SgIntVal* val=isSgIntVal(valueNode)) {
    int value=val->get_value();
    if(value==0) {
      return true;
    }
  } else if(SgFloatVal* val=isSgFloatVal(valueNode)) {
    float value=val->get_value();
    if(value==0.0f) {
      return true;
    }
  } else if(SgDoubleVal* val=isSgDoubleVal(valueNode)) {
    double value=val->get_value();
    if(value==0.0d) {
      return true;
    }
  }
  return false;
}


// rewrites an AST
// requirements: all variables have been replaced by constants
// uses AstMatching to match patterns.
void RewriteSystem::rewriteAst(SgNode*& root, VariableIdMapping* variableIdMapping, bool ruleAddReorder, bool performCompoundAssignmentsElimination, bool ruleAlgebraic) {
  //cout<<"Rewriting AST:"<<AstTerm::astTermWithNullValuesToString(root)<<endl;
  bool someTransformationApplied=false;
  bool transformationApplied=false;
  AstMatching m;
  // outer loop (overall fixpoint on all transformations)
   /* Transformations:
      1) eliminate unary operator -(integer) in tree
      2) normalize expressions (reordering of inner nodes and leave nodes)
      3) constant folding (leave nodes)
   */
  
  if(getRewriteCondStmt()) {
    rewriteCondStmtInAst(root);
  }
  
  if(performCompoundAssignmentsElimination) {
    rewriteCompoundAssignments(root,variableIdMapping);
  }
  normalizeFloatingPointNumbersForUnparsing(root);

  do {
     someTransformationApplied=false;
#if 0
     do {
       // Rewrite-rule 1: $UnaryOpSg=MinusOp($IntVal1=SgIntVal) => SgIntVal.val=-$Intval.val
       transformationApplied=false;
       MatchResult res=m.performMatching("$UnaryOp=SgMinusOp($IntVal=SgIntVal)",root);
       if(res.size()>0) {
         for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
           // match found
           SgExpression* op=isSgExpression((*i)["$UnaryOp"]);
           SgIntVal* val=isSgIntVal((*i)["$IntVal"]);
           //cout<<"FOUND UNARY CONST: "<<op->unparseToString()<<endl;
           int rawval=val->get_value();
           // replace with folded value (using integer semantics)
           switch(op->variantT()) {
           case V_SgMinusOp:
             SgNodeHelper::replaceExpression(op,SageBuilder::buildIntVal(-rawval),false);
             break;
           default:
             cerr<<"Error: rewrite phase: unsupported operator in matched unary expression. Bailing out."<<endl;
             exit(1);
           }
           transformationApplied=true;
           someTransformationApplied=true;
           _rewriteStatistics.numElimMinusOperator++;
         }
       }
     } while(transformationApplied); // a loop will eliminate -(-(5)) to 5
#endif
     if(ruleAlgebraic) {
       {
         // E1+(-E2) => E1-E2
         string m1="$OriginalExp=SgAddOp($E1,SgMinusOp($E2))";
         MatchResult res=m.performMatching(m1,root);
         if(res.size()>0) {
           for(MatchResult::iterator mr=res.begin();mr!=res.end();++mr) {
             SgExpression* originalExp=isSgExpression((*mr)["$OriginalExp"]);
             SgExpression* e1=isSgExpression((*mr)["$E1"]);
             SgExpression* e2=isSgExpression((*mr)["$E2"]);
             SgExpression* newSubtractOp=SageBuilder::buildSubtractOp(e1,e2);
             if(getTrace()) {
               cout<<"Rule algebraic: "<<originalExp->unparseToString()<<" => "<<newSubtractOp->unparseToString()<<endl;
             }
             SgNodeHelper::replaceExpression(originalExp,newSubtractOp,false);
             _rewriteStatistics.numUnaryMinusToBinaryMinusConversion++;
             someTransformationApplied=true;
           }
         }
       }       
       {
         // (-E2)+E1 => E1-E2
         string m2="$OriginalExp=SgAddOp(SgMinusOp($E2),$E1)"; // this must be a separate rule (cannot use 'or' with above rule) because both can match
         MatchResult res=m.performMatching(m2,root);
         if(res.size()>0) {
           for(MatchResult::iterator mr=res.begin();mr!=res.end();++mr) {
             SgExpression* originalExp=isSgExpression((*mr)["$OriginalExp"]);
             SgExpression* e1=isSgExpression((*mr)["$E1"]);
             SgExpression* e2=isSgExpression((*mr)["$E2"]);
             SgExpression* newSubtractOp=SageBuilder::buildSubtractOp(e1,e2);
             if(getTrace()) {
               cout<<"Rule algebraic: "<<originalExp->unparseToString()<<" => "<<newSubtractOp->unparseToString()<<endl;
             }
             SgNodeHelper::replaceExpression(originalExp,newSubtractOp,false);
             _rewriteStatistics.numUnaryMinusToBinaryMinusConversion++;
             someTransformationApplied=true;
           }
         }       
       }
       {
         // (-E1)-E2 => -(E1+E2)
         string m3="$OriginalExp=SgSubtractOp(SgMinusOp($E1),$E2)";
         MatchResult res=m.performMatching(m3,root);
         if(res.size()>0) {
           for(MatchResult::iterator mr=res.begin();mr!=res.end();++mr) {
             SgExpression* originalExp=isSgExpression((*mr)["$OriginalExp"]);
             SgExpression* e1=isSgExpression((*mr)["$E1"]);
             SgExpression* e2=isSgExpression((*mr)["$E2"]);
             SgExpression* newAddOp=SageBuilder::buildAddOp(e1,e2);
             SgExpression* newMinusOp=SageBuilder::buildMinusOp(newAddOp);
             if(getTrace()) {
               cout<<"Rule algebraic: "<<originalExp->unparseToString()<<" => "<<newMinusOp->unparseToString()<<endl;
             }
             _rewriteStatistics.numBinaryAndUnaryMinusToBinarySubConversion++;
             SgNodeHelper::replaceExpression(originalExp,newMinusOp,false);
             someTransformationApplied=true;
           }
         }       
       }
       {
         // E1-(-E2) => E1+E2
         string m4="$OriginalExp=SgSubtractOp($E1,SgMinusOp($E2))";
         MatchResult res=m.performMatching(m4,root);
         if(res.size()>0) {
           for(MatchResult::iterator mr=res.begin();mr!=res.end();++mr) {
             SgExpression* originalExp=isSgExpression((*mr)["$OriginalExp"]);
             SgExpression* e1=isSgExpression((*mr)["$E1"]);
             SgExpression* e2=isSgExpression((*mr)["$E2"]);
             SgExpression* newAddOp=SageBuilder::buildAddOp(e1,e2);
             if(getTrace()) {
               cout<<"Rule algebraic: "<<originalExp->unparseToString()<<" => "<<newAddOp->unparseToString()<<endl;
             }
             _rewriteStatistics.numBinaryAndUnaryMinusToBinaryAddConversion++;
             SgNodeHelper::replaceExpression(originalExp,newAddOp,false);
             someTransformationApplied=true;
           }
         }       
       }
     }     

     if(ruleAlgebraic) {
       int cnt=0;
       do {

         // the following rules guarantee convergence
         transformationApplied=false;

         // E*(-1) => -E
         string multiplyMinusRight="$OriginalOp=SgMultiplyOp($Remains,$Minus=SgMinusOp($Val=SgDoubleVal))|$OriginalOp=SgMultiplyOp($Remains,$Minus=SgMinusOp($Val=SgFloatVal))";
         // (-1)*E => -E
         string multiplyMinusLeft ="$OriginalOp=SgMultiplyOp($Minus=SgMinusOp($Val=SgDoubleVal),$Remains)|$OriginalOp=SgMultiplyOp($Minus=SgMinusOp($Val=SgFloatVal),$Remains)";

         MatchResult res=m.performMatching(multiplyMinusRight+"|"+multiplyMinusLeft,root);
         if(res.size()>0) {
           for(MatchResult::iterator kk=res.begin();kk!=res.end();++kk) {
             // match found
             SgExpression* valueNode=isSgExpression((*kk)["$Val"]);
             SgExpression* op=isSgExpression((*kk)["$OriginalOp"]);
             SgExpression* remainsNode=isSgExpression((*kk)["$Remains"]);
             SgMinusOp* minusNode=isSgMinusOp((*kk)["$Minus"]);

             bool algebraicIdentityTransformation=false;
             if(valueNode && isSgMultiplyOp(op)) {
               if(isValueOne(valueNode)) {
                 algebraicIdentityTransformation=true;
                 _rewriteStatistics.numMultiplyMinusOneConversion++;
               } else {
                 //not normalize
                 //cout<<"WARNING: Found unsupported value-type in alebraic multiply-transformation rule :"<<cnt<<": "<<op->unparseToString()<<endl;
               }
             }
             if(valueNode && isSgAddOp(op)) {
               // unreachable
               ROSE_ASSERT(false);
               if(isValueZero(valueNode)) {
                 algebraicIdentityTransformation=true;
               } else {
                 //not normalized
                 //cout<<"DEBUG: Found unsupported value-type in alebraic multiply-transformation rule :"<<cnt<<": "<<op->unparseToString()<<endl;
               }
             }
             if(algebraicIdentityTransformation) {
               if(getTrace()) {
                 cout<<"Rule algebraic: "<<op->unparseToString()<<" => -"<<remainsNode->unparseToString()<<endl;
               }
               minusNode->set_operand(remainsNode);
               SgNodeHelper::replaceExpression(op,minusNode,false);
               transformationApplied=true; 
               someTransformationApplied=true;
               cnt++;
             }
           }
         }
       } while(transformationApplied);
     }

     if(ruleAlgebraic) {
       int cnt=0;
       do {

         // the following rules guarantee convergence
         transformationApplied=false;
         // 0-E => -E
         string subtractRight="$OriginalOp=SgSubtractOp($Val=SgDoubleVal,$Remains)";
         MatchResult res=m.performMatching(subtractRight,root);
         if(res.size()>0) {
           for(MatchResult::iterator kk=res.begin();kk!=res.end();++kk) {
             // match found
             SgExpression* op=isSgExpression((*kk)["$OriginalOp"]);
             SgExpression* valueNode=isSgExpression((*kk)["$Val"]);
             SgExpression* remainsNode=isSgExpression((*kk)["$Remains"]);

             if(getTrace() && op && remainsNode) {
               cout<<"Rule algebraic matched: "<<op->unparseToString()<<" => -"<<remainsNode->unparseToString()<<endl;
             }

             bool algebraicIdentityTransformation=false;
             if(valueNode && isSgSubtractOp(op)) {
               if(isValueZero(valueNode)) {
                 algebraicIdentityTransformation=true;
               } else {
                 //not normalized
                 //cout<<"DEBUG: Found unsupported value-type in alebraic multiply-transformation rule :"<<cnt<<": "<<op->unparseToString()<<endl;
               }
             }
             if(algebraicIdentityTransformation) {
               if(getTrace()) {
                 cout<<"Rule algebraic: "<<op->unparseToString()<<" => -"<<remainsNode->unparseToString()<<endl;
               }
               SgExpression* newMinusOp=SageBuilder::buildMinusOp(remainsNode);
               SgNodeHelper::replaceExpression(op,newMinusOp,false);
               transformationApplied=true; 
               someTransformationApplied=true;
               _rewriteStatistics.numZeroSubEConversion++;
               cnt++;
             }
           }
         }
       } while(transformationApplied);
     }



     if(ruleAlgebraic) {
       int cnt=0;
       do {

         // the following rules guarantee convergence
         transformationApplied=false;
         //MatchResult res=m.performMatching("$MultiplyOp=SgMultiplyOp($Remains,$Val=SgFloatVal|$Val=SgDoubleVal|$Val=SgIntVal)",root);

         // E*1.0=>E
         string mulRightVal="$IdentityOp=SgMultiplyOp($Remains,$Val=SgDoubleVal)|$IdentityOp=SgMultiplyOp($Remains,$Val=SgFloatVal)|$IdentityOp=SgMultiplyOp($Remains,$Val=SgIntVal)";
         // 1.0*E=>E
         string mulLeftVal="$IdentityOp=SgMultiplyOp($Val=SgDoubleVal,$Remains)|$IdentityOp=SgMultiplyOp($Val=SgFloatVal,$Remains)|$IdentityOp=SgMultiplyOp($Val=SgIntVal,$Remains)";
         // E+0.0=>E
         string addRightVal="$IdentityOp=SgAddOp($Remains,$Val=SgDoubleVal)|$IdentityOp=SgAddOp($Remains,$Val=SgFloatVal)|$IdentityOp=SgAddOp($Remains,$Val=SgIntVal)";
         // 0.0+E=>E
         string addLeftVal="$IdentityOp=SgAddOp($Val=SgDoubleVal,$Remains)|$IdentityOp=SgAddOp($Val=SgFloatVal,$Remains)|$IdentityOp=SgAddOp($Val=SgIntVal,$Remains)";

         MatchResult res=m.performMatching(mulRightVal+"|"+mulLeftVal+"|"+addRightVal+"|"+addLeftVal,root);
         if(res.size()>0) {
           for(MatchResult::iterator kk=res.begin();kk!=res.end();++kk) {
             // match found
#if 0
             for(SingleMatchVarBindings::iterator vars_iter=(*kk).begin();vars_iter!=(*kk).end();++vars_iter) {
               SgNode* matchedTerm=(*vars_iter).second;
               std::cout << "DEBUG:::  VAR: " << (*vars_iter).first << "=" << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
             }
#endif
             SgExpression* valueNode=isSgExpression((*kk)["$Val"]);
             SgExpression* op=isSgExpression((*kk)["$IdentityOp"]);
             SgExpression* remainsNode=isSgExpression((*kk)["$Remains"]);

             bool algebraicIdentityTransformation=false;
             if(valueNode && isSgMultiplyOp(op)) {
               if(isValueOne(valueNode)) {
                 algebraicIdentityTransformation=true;
                 _rewriteStatistics.numMultiplyOneElim++;
               } else {
                 //not normalize
                 //cout<<"WARNING: Found unsupported value-type in alebraic multiply-transformation rule :"<<cnt<<": "<<op->unparseToString()<<endl;
               }
             }
             if(valueNode && isSgAddOp(op)) {
               if(isValueZero(valueNode)) {
                 algebraicIdentityTransformation=true;
                 _rewriteStatistics.numAddZeroElim++;
               } else {
                 //not normalized
                 //cout<<"DEBUG: Found unsupported value-type in alebraic multiply-transformation rule :"<<cnt<<": "<<op->unparseToString()<<endl;
               }
             }
             if(algebraicIdentityTransformation) {
               if(getTrace()) {
                 cout<<"Rule algebraic: "<<op->unparseToString()<<" => "<<remainsNode->unparseToString()<<endl;
               }
               SgNodeHelper::replaceExpression(op,remainsNode,false);
               transformationApplied=true; 
               someTransformationApplied=true;
               cnt++;
             }
           }
         }
       } while(transformationApplied);
     }

     if(ruleAddReorder) {
       do {
         // the following rules guarantee convergence

         // REWRITE: re-ordering (normalization) of expressions
         // Rewrite-rule 1: SgAddOp(SgAddOp($Remains,$Other),$IntVal=SgIntVal) => SgAddOp(SgAddOp($Remains,$IntVal),$Other)
         //                 where $Other!=SgIntVal && $Other!=SgFloatVal && $Other!=SgDoubleVal; ($Other notin {SgIntVal,SgFloatVal,SgDoubleVal})
         transformationApplied=false;
         // (R + O) + Int =>   (R + Int) + O
         MatchResult res=m.performMatching("$BinaryOp1=SgAddOp(SgAddOp($Remains,$Other),$IntVal=SgIntVal)",root);
         if(res.size()>0) {
           for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
             // match found
             SgExpression* other=isSgExpression((*i)["$Other"]);
             if(other) {
               if(!isSgIntVal(other) && !isSgFloatVal(other) && !isSgDoubleVal(other)) {
                 //SgNode* op1=(*i)["$BinaryOp1"];
                 SgExpression* val=isSgExpression((*i)["$IntVal"]);
                 //cout<<"FOUND: "<<op1->unparseToString()<<endl;
                 if(getTrace()) {
                   cout<<"Rule AddOpReorder: "<<((*i)["$BinaryOp1"])->unparseToString()<<" => ";
                 }
                 // replace op1-rhs with op2-rhs
                 SgExpression* other_copy=SageInterface::copyExpression(other);
                 SgExpression* val_copy=SageInterface::copyExpression(val);
                 SgNodeHelper::replaceExpression(other,val_copy,false);
                 SgNodeHelper::replaceExpression(val,other_copy,false);
                 //cout<<"REPLACED: "<<op1->unparseToString()<<endl;
                 transformationApplied=true;
                 someTransformationApplied=true;
                 if(getTrace())
                   cout<<((*i)["$BinaryOp1"])->unparseToString()<<endl;
                 _rewriteStatistics.numAddOpReordering++;
               }
             }
           }
         }
       } while(transformationApplied);
     }

     // REWRITE: constant folding of constant integer expressions (float values are not folded)
     do {
       // Rewrite-rule 2: SgAddOp($IntVal1=SgIntVal,$IntVal2=SgIntVal) => SgIntVal
       //                 where SgIntVal.val=$IntVal1.val+$IntVal2.val
       transformationApplied=false;
       MatchResult res=m.performMatching(
                                         "$BinaryOp1=SgAddOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
                                       | $BinaryOp1=SgSubtractOp($IntVal1=SgIntVal,$IntVal2=SgIntVal) \
                                       | $BinaryOp1=SgMultiplyOp($IntVal1=SgIntVal,$IntVal2=SgIntVal) \
                                       | $BinaryOp1=SgDivideOp($IntVal1=SgIntVal,$IntVal2=SgIntVal) \
                                         ",root);
       if(res.size()>0) {
         for(MatchResult::iterator i=res.begin();i!=res.end();++i) {
           // match found
           SgExpression* op1=isSgExpression((*i)["$BinaryOp1"]);
           SgIntVal* val1=isSgIntVal((*i)["$IntVal1"]);
           SgIntVal* val2=isSgIntVal((*i)["$IntVal2"]);
           //cout<<"FOUND CONST: "<<op1->unparseToString()<<endl;
           int rawval1=val1->get_value();
           int rawval2=val2->get_value();
         // replace with folded value (using integer semantics)
           switch(op1->variantT()) {
           case V_SgAddOp:
             SgNodeHelper::replaceExpression(op1,SageBuilder::buildIntVal(rawval1+rawval2),false);
             break;
           case V_SgSubtractOp:
             SgNodeHelper::replaceExpression(op1,SageBuilder::buildIntVal(rawval1-rawval2),false);
           break;
           case V_SgMultiplyOp:
             SgNodeHelper::replaceExpression(op1,SageBuilder::buildIntVal(rawval1*rawval2),false);
             break;
           case V_SgDivideOp:
             SgNodeHelper::replaceExpression(op1,SageBuilder::buildIntVal(rawval1/rawval2),false);
             break;
           default:
             cerr<<"Error: rewrite phase: unsopported operator in matched expression. Bailing out."<<endl;
             exit(1);
           }
           transformationApplied=true;
           someTransformationApplied=true;
           _rewriteStatistics.numConstantFolding++;
         }
       }
     } while(transformationApplied);

     //eliminateSuperfluousCasts(root);

     //if(someTransformationApplied) cout<<"DEBUG: transformed: "<<root->unparseToString()<<endl;
  } while(someTransformationApplied);
  if(getRuleCommutativeSort()) {
    establishCommutativeOrder(root,variableIdMapping);
  }
   //cout<<"DEBUG: "<<AstTerm::astTermWithNullValuesToString(root)<<endl;
}
