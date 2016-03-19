#include "sage3basic.h"
#include "RewriteSystem.h"
#include "Timer.h"

using namespace std;
using namespace SPRAY;

RewriteStatistics RewriteSystem::getRewriteStatistics() {
  return dump1_stats;
}

RewriteStatistics::RewriteStatistics() {
  init();
}
void RewriteStatistics::init() {
  numElimMinusOperator=0;
  numElimAssignOperator=0;
  numAddOpReordering=0;
  numConstantFolding=0;
  numVariableElim=0;
  numArrayUpdates=0;
  numConstExprElim=0;
}
void RewriteStatistics::reset() { 
  init();
}
RewriteStatistics RewriteSystem::getStatistics() {
  return dump1_stats;
}

string RewriteStatistics::toString() {
  stringstream ss;
  ss<<"Array updates  : "<<numArrayUpdates<<endl;
  ss<<"Elim minus op  : "<<numElimMinusOperator<<endl;
  ss<<"Elim assign op : "<<numElimAssignOperator<<endl;
  ss<<"Add op reorder : "<<numAddOpReordering<<endl;
  ss<<"Const fold     : "<<numConstantFolding<<endl;
  ss<<"Variable elim  : "<<numVariableElim<<endl;
  ss<<"Const expr elim: "<<numConstExprElim<<endl;
  return ss.str();
}

string RewriteStatistics::toCsvString() {
  stringstream ss;
  ss<<numArrayUpdates
    <<","<<numElimMinusOperator
    <<","<<numElimAssignOperator
    <<","<<numAddOpReordering
    <<","<<numConstantFolding
    <<","<<numVariableElim
    <<","<<numConstExprElim
    ;
  return ss.str();
}

void RewriteSystem::resetStatistics() {
  dump1_stats.reset();
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
  cout<<"INFO: transforming "<<assignOpNum<<" compound assignment expressions: started."<<endl;
  size_t assignOpNr=1;
  Timer timer;
  double buildTime=0.0, replaceTime=0.0;
  for(AssignOpListType::iterator i=assignOpList.begin();i!=assignOpList.end();++i) {
    //cout<<"INFO: normalizing compound assign op "<<assignOpNr<<" of "<<assignOpNum<<endl;
    timer.start();
    SgExpression* newRoot=isSgExpression(buildRewriteCompoundAssignment(*i,variableIdMapping));
    buildTime+=timer.getElapsedTimeInMilliSec();

    if(newRoot) {
      timer.start();
      SgNodeHelper::replaceExpression(*i,newRoot);
      replaceTime+=timer.getElapsedTimeInMilliSec();
      assignOpNr++;
    } else {
      cout<<"WARNING: not an expression. transformation not applied: "<<(*i)->unparseToString()<<endl;
    }
    //cout<<"Buildtime: "<<buildTime<<" Replacetime: "<<replaceTime<<endl;
  }
  cout<<"INFO: transforming "<<assignOpNum<<" compound assignment expressions: done."<<endl;
}

SgNode* RewriteSystem::buildRewriteCompoundAssignment(SgNode* root, VariableIdMapping* variableIdMapping) {
  // Rewrite-rule 0: $Left OP= $Right => $Left = $Left OP $Right
  if(isSgCompoundAssignOp(root)) {
    dump1_stats.numElimAssignOperator++;
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
    default: /* ignore all other cases - all other expr remain unmodified */
      return 0;
    }
  }
  return 0;
}

void RewriteSystem::rewriteCompoundAssignments(SgNode*& root, VariableIdMapping* variableIdMapping) {
#if 0
  SgNode* root2=*(&root);
  root=buildRewriteCompoundAssignment(root2,variableIdMapping);
  return;
#else
  // Rewrite-rule 0: $Left OP= $Right => $Left = $Left OP $Right
  if(isSgCompoundAssignOp(root)) {
    dump1_stats.numElimAssignOperator++;
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

 
 // rewrites an AST
 // requirements: all variables have been replaced by constants
 // uses AstMatching to match patterns.
void RewriteSystem::rewriteAst(SgNode*& root, VariableIdMapping* variableIdMapping, bool rewriteTrace, bool ruleAddReorder, bool performCompoundAssignmentsElimination) {
   //  cout<<"Rewriting AST:"<<endl;
   bool someTransformationApplied=false;
   bool transformationApplied=false;
   AstMatching m;
   // outer loop (overall fixpoint on all transformations)
   /* Transformations:
      1) eliminate unary operator -(integer) in tree
      2) normalize expressions (reordering of inner nodes and leave nodes)
      3) constant folding (leave nodes)
   */

   if(performCompoundAssignmentsElimination)
   {
     rewriteCompoundAssignments(root,variableIdMapping);
   }

   do{
     someTransformationApplied=false;


 do {
     // Rewrite-rule 1: $UnaryOpSg=MinusOp($IntVal1=SgIntVal) => SgIntVal.val=-$Intval.val
     transformationApplied=false;
     MatchResult res=m.performMatching(
                                       "$UnaryOp=SgMinusOp($IntVal=SgIntVal)\
                                       ",root);
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
           cerr<<"Error: rewrite phase: unsopported operator in matched unary expression. Bailing out."<<endl;
           exit(1);
         }
         transformationApplied=true;
         someTransformationApplied=true;
         dump1_stats.numElimMinusOperator++;
       }
     }
  } while(transformationApplied); // a loop will eliminate -(-(5)) to 5

 if(ruleAddReorder) {
   do {
     // the following rules guarantee convergence
     
     // REWRITE: re-ordering (normalization) of expressions
     // Rewrite-rule 1: SgAddOp(SgAddOp($Remains,$Other),$IntVal=SgIntVal) => SgAddOp(SgAddOp($Remains,$IntVal),$Other) 
     //                 where $Other!=SgIntVal && $Other!=SgFloatVal && $Other!=SgDoubleVal; ($Other notin {SgIntVal,SgFloatVal,SgDoubleVal})
     transformationApplied=false;
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
             if(rewriteTrace)
               cout<<"Rule AddOpReorder: "<<((*i)["$BinaryOp1"])->unparseToString()<<" => ";
             // replace op1-rhs with op2-rhs
             SgExpression* other_copy=SageInterface::copyExpression(other);
             SgExpression* val_copy=SageInterface::copyExpression(val);
             SgNodeHelper::replaceExpression(other,val_copy,false);
             SgNodeHelper::replaceExpression(val,other_copy,false);
             //cout<<"REPLACED: "<<op1->unparseToString()<<endl;
             transformationApplied=true;
             someTransformationApplied=true;
             if(rewriteTrace)
               cout<<((*i)["$BinaryOp1"])->unparseToString()<<endl;
             dump1_stats.numAddOpReordering++;
           }       
         }
       }
     }
   } while(transformationApplied);
 }

   // REWRITE: constant folding of constant integer (!) expressions
   // we intentionally avoid folding of float values
 do {
     // Rewrite-rule 2: SgAddOp($IntVal1=SgIntVal,$IntVal2=SgIntVal) => SgIntVal
     //                 where SgIntVal.val=$IntVal1.val+$IntVal2.val
     transformationApplied=false;
     MatchResult res=m.performMatching(
                                       "$BinaryOp1=SgAddOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
                                       |$BinaryOp1=SgSubtractOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
                                       |$BinaryOp1=SgMultiplyOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
                                       |$BinaryOp1=SgDivideOp($IntVal1=SgIntVal,$IntVal2=SgIntVal)\
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
         dump1_stats.numConstantFolding++;
       }
     }
  } while(transformationApplied);
 //if(someTransformationApplied) cout<<"DEBUG: transformed: "<<root->unparseToString()<<endl;
   } while(someTransformationApplied);
}

