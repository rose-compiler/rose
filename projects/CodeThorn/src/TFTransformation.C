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
      string lhsTypeName=lhsExp->get_type()->unparseToString(); // used in error message
      SgType* lhsType=lhsExp->get_type();
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
        continue;
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

  //$VAR1[$IDX1][$IDX2].$VAR2[$IDX3] => $VAR1.$VAR2[$IDX3]($IDX1,$IDX2) where name($var2)=="u"
  matchexpression+="$AccessPattern=SgPntrArrRefExp(SgDotExp(SgPntrArrRefExp(SgPntrArrRefExp($VAR1,$IDX1),$IDX2),$VAR2),$IDX3)";
  //$VAR1[$IDX1][$IDX2].$VAR2 => $VAR1.$VAR2($IDX1,$IDX2) where name($var2) in {"rho","p"}
  matchexpression+="| $AccessPattern=SgDotExp(SgPntrArrRefExp(SgPntrArrRefExp($VAR1,$IDX1),$IDX2),$VAR2)";

  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  for(MatchResult::iterator j=r.begin();j!=r.end();++j) {
    // resolve cases that both match expressions can be applied, but
    // only one should be used if and only if the pattern2's parent is
    // pattern1.
    SgExpression* matchedPattern=isSgExpression((*j)["$AccessPattern"]);
    SgType* rhsType=nullptr;
    if(SgVarRefExp* varRefExp=isSgVarRefExp((*j)["$VAR1"])) {
      rhsType=varRefExp->get_type();
      rhsType=getElementType(rhsType);
    } else {
      rhsType=matchedPattern->get_type();
    }
    if(trace) {
      cout<<"ARRAY STRUCT MATCHING ROOT: "<<matchedPattern->unparseToString()<<endl;
      cout<<"ARRAY STRUCT TYPE: "<<rhsType->unparseToString()<<endl;
      cout<<"ARRAY STRUCT ROOTAST: "<<AstTerm::astTermWithNullValuesToString(matchedPattern)<<endl;
    }
    if(rhsType==accessType) {
      string var1=(*j)["$VAR1"]->unparseToString();
      string var2=(*j)["$VAR2"]->unparseToString();
      string idx1=(*j)["$IDX1"]->unparseToString();
      string idx2=(*j)["$IDX2"]->unparseToString();
      string oldCode0=matchedPattern->unparseToString();
      string newCode0;
      // check is 1st or 2nd match expression was matched (2nd match expression implies assignment of IDX3)
      if((*j)["$IDX3"]==nullptr) {
        // rule 2
        if(var2!="rho" && var2!="p" && var2!="mass" && var2!="energy" && var2!="rho_E") {
          continue;
        }
        newCode0=var1+"."+var2+"("+idx1+","+idx2+")";
      } else {
        // rule 1; not rule 1 is also applied for rule 2 matches, but
        // test on 'u' filters those because it is the same struct's
        // other data member name (disjunctive sets of data members).
        if(var2!="u" && var2!="momentum" && var2!="rho_u") {
          continue;
        }
        string idx3=(*j)["$IDX3"]->unparseToString();
        newCode0=var1+"."+var2+"["+idx3+"]("+idx1+","+idx2+")";
      }
      arrayOfStructsTransformations++;
      string newCode=newCode0; // ';' is unparsed as part of the statement that contains the assignop
      SgNodeHelper::replaceAstWithString(matchedPattern, newCode);
      std::string lineCol=SgNodeHelper::sourceLineColumnToString(matchedPattern);
      //mRHS.printMarkedLocations();
      //mRHS.printMatchOperationsSequence();
    } else {
      cout<<"DEBUG: rhs matches, but type does not. skipping."<<rhsType->unparseToString()<<"!="<<accessType->unparseToString()<<endl;
    }
  }
}

// used to avoid transforming assignments in for-initializer expression
bool isWithinBlockStmt(SgExpression* exp) {
  SgNode* current=exp;
  while(isSgExpression(current)||isSgExprStatement(current)) {
    current=current->get_parent();
  };
  return isSgBasicBlock(current);
}

//Transformation ad_intermediate
void TFTransformation::instrumentADIntermediate(SgNode* root) {
  RoseAst ast(root);
  std::string matchexpression;
  //matchexpression+="$ASSIGNOP=SgAssignOp($VAR=SgVarRefExp,_)";
  //matchexpression+="$ASSIGNOP=SgAssignOp($VAR=SgVarRefExp,_)|$ASSIGNOP=SgPlusAssignOp($VAR=SgVarRefExp,..)";
  //matchexpression+="|$ASSIGNOP=SgAssignOp($VAR=SgPntrArrRefExp,_)|$ASSIGNOP=SgPlusAssignOp($VAR=SgPntrArrRefExp,..)";
  //matchexpression+="|$ASSIGNOP=SgAssignOp($VAR=SgPointerDerefExp,_)|$ASSIGNOP=SgPlusAssignOp($VAR=SgPointerDerefExp,..)";
  //AstMatching m;
  //MatchResult r=m.performMatching(matchexpression,root);
  //for(MatchResult::iterator j=r.begin();j!=r.end();++j) {
  //  SgExpression* assignOp=isSgExpression((*j)["$ASSIGNOP"]);
  //  SgExpression* leftExp=isSgExpression((*j)["$VAR"]);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i){
    SgExpression* assignOp = nullptr;
    SgExpression* refExp = nullptr;
    if(SgAssignOp* aop = isSgAssignOp(*i)){
      assignOp = aop;
      refExp = aop->get_lhs_operand();
    }
    else if(SgCompoundAssignOp* comOp = isSgCompoundAssignOp(*i)){
      assignOp = comOp;
      refExp = comOp->get_lhs_operand();  
    }
    else if(SgPlusPlusOp* ppOp = isSgPlusPlusOp(*i)){
      assignOp = ppOp;
      refExp = ppOp->get_operand();
    }
    else if(SgMinusMinusOp* mmOp = isSgMinusMinusOp(*i)){
      assignOp = mmOp;
      refExp = mmOp->get_operand();
    }else continue;
    SgType* varType=refExp->get_type();
    SgVarRefExp* varRefExp = nullptr;
    while(!varRefExp){
      if((varRefExp = isSgVarRefExp(refExp)));
      else if(SgPntrArrRefExp* arrRef = isSgPntrArrRefExp(refExp)) refExp = arrRef->get_lhs_operand();
      else if(SgPointerDerefExp* pntrRef = isSgPointerDerefExp(refExp)) refExp = pntrRef->get_operand();
      else continue;
    }
    if(SgNodeHelper::isFloatingPointType(varType)) {
      if(isWithinBlockStmt(assignOp)) {
        SgVariableSymbol* varRefExpSymbol=varRefExp->get_symbol();
        if(varRefExpSymbol) {
          SgName varName=varRefExpSymbol->get_name();
          string varNameString=varName;
          string instrumentationString="AD_intermediate("+varNameString+",\""+varNameString+"\", FILE_INFO, FUNC_INFO);";
          // locate root node of statement
          SgNode* stmtSearch=assignOp;
          while(!isSgStatement(stmtSearch)) {
            stmtSearch=stmtSearch->get_parent();
            if(!isSgStatement(stmtSearch)&&!isSgExpression(stmtSearch)) {
              cerr<<"Error: Unsupported expression structure at "<<SgNodeHelper::sourceLineColumnToString(assignOp)<<endl;
              exit(1);
            }
          }
          // instrument now: insert empty statement and replace it with macro call
          //cout<<"TRANSFORMATION: insert after "<< SgNodeHelper::sourceLineColumnToString(stmtSearch) <<" : "<<instrumentationString<<endl;
          string newSource=stmtSearch->unparseToString()+"\n"+instrumentationString+"\n";
          SgNodeHelper::replaceAstWithString(stmtSearch,newSource);
          adIntermediateTransformations++;
        }
      }
    }
  }
}
