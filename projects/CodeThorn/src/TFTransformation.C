#include "sage3basic.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"
#include "TFTransformation.h"
#include "AstTerm.h"

using namespace std;

SgType* getElementType(SgType* type) {
  if(SgPointerType* ptrType=isSgPointerType(type)) {
    return getElementType(ptrType->get_base_type());
  } else {
    return type;
  }
}

// SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2) ==> $DS+".get("+$E1+","+$E2+")";
void TFTransformation::transformRhs(SgType* accessType, SgNode* rhsRoot) {
  // transform RHS:
  std::string matchexpressionRHSAccess="$ArrayAccessPattern=SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2)";
  AstMatching mRHS;
  MatchResult rRHS=mRHS.performMatching(matchexpressionRHSAccess,rhsRoot);
  for(MatchResult::iterator j=rRHS.begin();j!=rRHS.end();++j) {
    SgExpression* matchedPattern=isSgExpression((*j)["$ArrayAccessPattern"]);
    SgType* rhsType=nullptr;
    if(SgVarRefExp* varRefExp=isSgVarRefExp((*j)["$DS"])) {
      rhsType=varRefExp->get_type();
      rhsType=getElementType(rhsType);
    } else {
      rhsType=matchedPattern->get_type();
    }
    if(true ||trace) {
      cout<<"RHS-MATCHING ROOT: "<<matchedPattern->unparseToString()<<endl;
      cout<<"RHS-MATCHING TYPE: "<<rhsType->unparseToString()<<endl;
      cout<<"RHS-MATCHING ROOTAST: "<<AstTerm::astTermWithNullValuesToString(matchedPattern)<<endl;
    }
    if(rhsType==accessType) {
      readTransformations++;
      string ds=(*j)["$DS"]->unparseToString();
      string e1=(*j)["$E1"]->unparseToString();
      string e2=(*j)["$E2"]->unparseToString();
      string oldCode0=(*j)["$ArrayAccessPattern"]->unparseToString();
      string newCode0=ds+".get("+e1+","+e2+")";
      string newCode=newCode0; // ';' is unparsed as part of the statement that contains the assignop
      SgNodeHelper::replaceAstWithString((*j)["$ArrayAccessPattern"], newCode);
      std::cout << std::endl;
      std::string lineCol=SgNodeHelper::sourceLineColumnToString((*j)["$ArrayAccessPattern"]);
      if(trace) {
        cout <<"RHS-TRANSFORMATION: "<<lineCol<<" OLD:"<<oldCode0<<endl;
        cout <<"RHS-TRANSFORMATION: "<<lineCol<<" NEW:"<<newCode0<<endl;
      }
      //mRHS.printMarkedLocations();
      //mRHS.printMatchOperationsSequence();
    } else {
      cout<<"DEBUG: rhs matches, but type does not. skipping."<<rhsType->unparseToString()<<"!="<<accessType->unparseToString()<<endl;
    }
  }
}

void TFTransformation::checkAndTransformVarAssignments(SgType* accessType,SgNode* root) {
  RoseAst ast(root);
  std::string matchexpression;
  matchexpression+="$Root=SgAssignOp(SgVarRefExp,$RHS)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  // print result in readable form for demo purposes
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    statementTransformations++;
    SgNode* rhsRoot=(*i)["$RHS"];
    cout<<"DEBUG: transforming variable assignment: "<<isSgExpression(rhsRoot)->unparseToString()<<endl;
    transformRhs(accessType,rhsRoot);
  }
}

void TFTransformation::checkAndTransformNonAssignments(SgType* accessType,SgNode* root) {
  RoseAst ast(root);
  if(!isSgAssignOp(root)&&isSgExpression(root)) {
    cout<<"DEBUG: transforming non-assignments: "<<isSgExpression(root)->unparseToString()<<endl;
    transformRhs(accessType,root);
  }
}

void TFTransformation::transformArrayAssignments(SgType* accessType,SgNode* root) {
  RoseAst ast(root);
  std::string matchexpression;
  // $WORK->$ARR[$IDX1,$IDX2]=$RHS
  matchexpression+="$Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp(SgArrowExp($WORK,$ARR),$IDX1),$IDX2),$RHS)";
  // $ARR[$IDX1,$IDX2]=$RHS
  matchexpression+="| $Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp($ARR,$IDX1),$IDX2),$RHS)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    statementTransformations++;
    if(trace) std::cout << "MATCH-LHS: \n"; 
    //SgNode* n=(*i)["X"];
    for(SingleMatchVarBindings::iterator vars_iter=(*i).begin();vars_iter!=(*i).end();++vars_iter) {
      SgNode* matchedTerm=(*vars_iter).second;
      if(trace) std::cout << "  VAR: " << (*vars_iter).first << "=" << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
    string oldCode0=(*i)["$Root"]->unparseToString();
    SgNode* rhsRoot=(*i)["$RHS"];
    transformRhs(accessType,rhsRoot);
    // transform LHS: work -> dV[IDX1][IDX2] = RHS; ==> work -> dV.set(IDX1,IDX2,RHS);
    string newCode0;
    string oldCode;
    if((*i)["$LHS"]) {
      SgExpression* lhsExp=isSgExpression((*i)["$LHS"]);
      string lhsTypeName=lhsExp->get_type()->unparseToString();
      SgType* lhsType=lhsExp->get_type();
      cout<<"DEBUG: LHS-TYPE:"<<lhsExp->unparseToString()<<":"<<lhsType->unparseToString();
      if(SgArrayType* lhsArrayType=isSgArrayType(lhsType)) {
        SgType* lhsBaseType=lhsArrayType->get_base_type();
        cout<<": basetype:"<<lhsBaseType->unparseToString();
      }
      cout<<endl;
      if(lhsType==accessType) {
        SgNode* workLhs=(*i)["$WORK"];
        string work;
        if(workLhs) {
          work=workLhs->unparseToString();
        }
        string ds=(*i)["$ARR"]->unparseToString();
        string e1=(*i)["$IDX1"]->unparseToString();
        string e2=(*i)["$IDX2"]->unparseToString();
        string rhs=(*i)["$RHS"]->unparseToString();

        writeTransformations++;
        if(workLhs)
          newCode0=work+" -> "+ds+".set("+e1+","+e2+","+rhs+")";
        else
          newCode0=ds+".set("+e1+","+e2+","+rhs+")";
      } else {
        cout<<"DEBUG: lhs-matches, but type does not. skipping."<<lhsTypeName<<"!="<<accessType->unparseToString()<<endl;
      }
      string newCode="      "+newCode0; // ';' is unparsed as part of the statement that contains the assignop
      string oldCode2;
      if(oldCode0.size()>0 && oldCode[0]=='/') {
        /* old code comment is already generated */
      } else {
        //oldCode2="/* OLD: "+oldCode0+"*/;\n"; TODO: must strip preceding comment generated by previous transformation
      }
      SgNodeHelper::replaceAstWithString((*i)["$Root"], oldCode2+newCode);

      std::string lineCol=SgNodeHelper::sourceLineColumnToString((*i)["$Root"]);
      if(trace) {
        cout <<"TRANSFORMATION: "<<lineCol<<" OLD:"<<oldCode0<<endl;
        cout <<"TRANSFORMATION: "<<lineCol<<" NEW:"<<newCode0<<endl;
      }
    }
  }
}

//Transformation 2dArrayOfStructs to StructWithArray:
void TFTransformation::transformArrayOfStructsAccesses(SgType* accessType,SgNode* root) {
  RoseAst ast(root);
  std::string matchexpression;

  //$var1[$IDX1][$IDX2].$var2 => $var1.$var2($IDX1,$IDX2) where name($var2) in {"rho","p"}
  matchexpression+="$AccessPattern=SgDotExp(SgPntrArrRefExp(SgPntrArrRefExp($VAR1,$IDX1),$IDX2),$VAR2)";
  //$var1[$IDX1][$IDX2].$var2[$E3] => $var1.$var2[$E3]($IDX1,$IDX2) where name($var2)=="u"
  //"| SgPntrArrRefExp(SgDotExp(SgPntrArrRefExp(SgPntrArrRefExp(SgVarRefExp,SgIntVal),SgVarRefExp),SgVarRefExp),SgIntVal)";;

  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  for(MatchResult::iterator j=r.begin();j!=r.end();++j) {
    SgExpression* matchedPattern=isSgExpression((*j)["$AccessPattern"]);
    SgType* rhsType=nullptr;
    if(SgVarRefExp* varRefExp=isSgVarRefExp((*j)["$VAR1"])) {
      rhsType=varRefExp->get_type();
      rhsType=getElementType(rhsType);
    } else {
      rhsType=matchedPattern->get_type();
    }
    if(true ||trace) {
      cout<<"ACCESS-MATCHING ROOT: "<<matchedPattern->unparseToString()<<endl;
      cout<<"ACCESS-MATCHING TYPE: "<<rhsType->unparseToString()<<endl;
      cout<<"ACCESS-MATCHING ROOTAST: "<<AstTerm::astTermWithNullValuesToString(matchedPattern)<<endl;
    }
    if(rhsType==accessType) {
      arrayStructTransformations++;
      cout<<"DEBUG1"<<endl;
      string var1=(*j)["$VAR1"]->unparseToString();
      string var2=(*j)["$VAR2"]->unparseToString();
      cout<<"DEBUG2"<<endl;
      // check for struct variables (TODO: allow to specify vars as additional constraints on the basetype specifier)
      if(var2!="rho" && var2!="p") {
        cout<<"DEBUG2Exit"<<endl;
        continue;
      }
      cout<<"DEBUG3"<<endl;
      string e1=(*j)["$IDX1"]->unparseToString();
      string e2=(*j)["$IDX2"]->unparseToString();
      cout<<"DEBUG4"<<endl;
      string oldCode0=(*j)["$AccessPattern"]->unparseToString();
      string newCode0=var1+"."+var2+"("+e1+","+e2+")";
      string newCode=newCode0; // ';' is unparsed as part of the statement that contains the assignop
      cout<<"DEBUG5"<<endl;
      SgNodeHelper::replaceAstWithString((*j)["$AccessPattern"], newCode);
      cout<<"DEBUG6"<<endl;
      std::string lineCol=SgNodeHelper::sourceLineColumnToString((*j)["$AccessPattern"]);
      if(trace) {
        cout <<"RHS-TRANSFORMATION: "<<lineCol<<" OLD:"<<oldCode0<<endl;
        cout <<"RHS-TRANSFORMATION: "<<lineCol<<" NEW:"<<newCode0<<endl;
      }
      //mRHS.printMarkedLocations();
      //mRHS.printMatchOperationsSequence();
    } else {
      cout<<"DEBUG: rhs matches, but type does not. skipping."<<rhsType->unparseToString()<<"!="<<accessType->unparseToString()<<endl;
    }
  }
}

/*
  transform assignments:
  $var1->$var2[$IDX1][$IDX2] = $RHS where basetype($var2)==TYPE 
    ==> $var1->$var2.set($IDX1,$IDX2,transformRhs($RHS))
  SgVarRefExp=$RHS
    ==> transformRhs($RHS)
  $RHS (e.g. function call: transform all parameters)
    ==> transformRhs($RHS)
  $var[$IDX1,$IDX2] = $RHS 
    where type($var)==HancockWorkArrays**
          ||type($var)==FluxVector**
    ==> $var.set($IDX1,$IDX2,transform($RHS))

transform on lhs or rhs:
  $var[$IDX1][$IDX2] where type($var)==HancockWorkArrays** 
                       && (name($var)==v_max_x || (name($var)==v_max_y))
    ==> $var($IDX1,$IDX2)

  Transformation 2dArrayOfStructs to StructWithArray:
  $var1[$IDX1][$IDX2].$var2 => $var1.$var2($IDX1,$IDX2) where name($var2) in {"rho","p"}
  $var1[$IDX1][$IDX2].$var2[$E3] => $var1.$var2[$E3]($IDX1,$IDX2) where name($var2)=="u"

transformRhs(exp):
  $var[$IDX1][$IDX2] where elementType(var)==TYPE
    ==> $var.get($IDX1,$IDX2)

 */
void TFTransformation::transformHancockAccess(SgType* accessType,SgNode* root) {
  transformArrayAssignments(accessType,root);
  checkAndTransformVarAssignments(accessType,root);
  checkAndTransformNonAssignments(accessType,root);
  //m.printMarkedLocations();
  //m.printMatchOperationsSequence();
}
