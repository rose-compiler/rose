#include "sage3basic.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"
#include "TFTransformation.h"
#include "AstTerm.h"

using namespace std;

// SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2) ==> $DS+".get("+$E1+","+$E2+")";
void TFTransformation::transformRhs(string exprTypeName, SgNode* rhsRoot) {
  if(trace) std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
  // transform RHS:
  std::string matchexpressionRHSAccess="$RHSPattern=SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2)";
  AstMatching mRHS;
  if(true ||trace) {
    cout<<"RHS-MATCHING ROOT2: "<<rhsRoot->unparseToString()<<endl;
    cout<<"RHS-MATCHING TYPE : "<<AstTerm::astTermWithNullValuesAndTypesToString(rhsRoot)<<endl;
  }
  MatchResult rRHS=mRHS.performMatching(matchexpressionRHSAccess,rhsRoot);
  for(MatchResult::iterator j=rRHS.begin();j!=rRHS.end();++j) {
    // rhsTypeName is not the type of var[i][j]
    string rhsTypeName=isSgExpression((*j)["$RHSPattern"])->get_type()->unparseToString();
    
    if(true || rhsTypeName==exprTypeName) {
      readTransformations++;
      if(trace) {
        std::cout << "MATCH-RHS: \n"; 
        cout<< "RHS-PATTERN:"<<(*j)["$RHSPattern"]<<" : "<</*(*j)["RHSPattern"]->unparseToString()<<*/endl;
        //cout<< "RHS-WORK:"<<(*j)["$WORK"]<<" : "<<(*j)["$WORK"]->unparseToString()<<endl;
        cout<< "RHS-DS:"<<(*j)["$DS"]<<" : "<<(*j)["$DS"]->unparseToString()<<endl;
        cout<< "RHS-E1:"<<(*j)["$E1"]<<" : "<<(*j)["$E1"]->unparseToString()<<endl;
        cout<< "RHS-E2:"<<(*j)["$E2"]<<" : "<<(*j)["$E2"]->unparseToString()<<endl;
      }
#if 0
      // fix: check for addressOf operator
      SgNode* p1=((*j)["$E2"])->get_parent();
      cout<<"DEDEBUG "<<p1->unparseToString()<<endl;
      cout<<"DEDEBUG:TERM:"<<AstTerm::astTermWithNullValuesToString(p1);
      SgNode* p2=p1->get_parent();
      cout<<"DEDEBUG:TERMp2:"<<AstTerm::astTermWithNullValuesToString(p1);
      if(isSgAddressOfOp(p2)) {
        // skip transformation here (would be wrong)
        cout<<"DEBUG: detected adress of operator. Skipping transformation."<<endl;
        continue;
      }
#endif
      //        string work=(*j)["$WORK"]->unparseToString();
      string ds=(*j)["$DS"]->unparseToString();
      string e1=(*j)["$E1"]->unparseToString();
      string e2=(*j)["$E2"]->unparseToString();
      string oldCode0=(*j)["$RHSPattern"]->unparseToString();
      string newCode0=ds+".get("+e1+","+e2+")";
      string newCode=newCode0; // ';' is unparsed as part of the statement that contains the assignop
      SgNodeHelper::replaceAstWithString((*j)["$RHSPattern"], newCode);
      std::cout << std::endl;
      std::string lineCol=SgNodeHelper::sourceLineColumnToString((*j)["$RHSPattern"]);
      if(trace) {
        cout <<"RHS-TRANSFORMATION: "<<lineCol<<" OLD:"<<oldCode0<<endl;
        cout <<"RHS-TRANSFORMATION: "<<lineCol<<" NEW:"<<newCode0<<endl;
      }
      //mRHS.printMarkedLocations();
      //mRHS.printMatchOperationsSequence();
    } else {
      cout<<"DEBUG: rhs matches, but type does not. skipping."<<rhsTypeName<<"!="<<exprTypeName<<endl;
    }
  }
}


void TFTransformation::checkAndTransformVarAssignments(string exprTypeName,SgNode* root) {
  RoseAst ast(root);
  std::string matchexpression;
  matchexpression+="$Root=SgAssignOp(SgVarRefExp,$RHS)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  // print result in readable form for demo purposes
  if(trace) std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
#if 1
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    statementTransformations++;
    SgNode* rhsRoot=(*i)["$RHS"];
    cout<<"DEBUG: transforming variable assignment: "<<isSgExpression(rhsRoot)->unparseToString()<<endl;
     transformRhs(exprTypeName,rhsRoot);
  }
#endif
}

SgType* getExprType(SgExpression* exp) {
  SgType* type=exp->get_type();
  return type;
}

string exprTypeToString(SgExpression* exp) {
  return getExprType(exp)->unparseToString();
}
		    
/*
  transform assignments:
  $var1->$var2[$E1][$E2] = $RHS where basetype($var2)==TYPE 
    ==> $var1->$var2.set($E1,$E2,transformRhs($RHS))
  SgVarRefExp=$RHS
    ==> transformRhs($RHS)
  $RHS (e.g. function call: transform all parameters)
    ==> transformRhs($RHS)
  $var[$E1,$E2] = $RHS 
    where type($var)==HancockWorkArrays**
          ||type($var)==FluxVector**
    ==> $var.set($E1,$E2,transform($RHS))

transform on lhs or rhs:
  $var[$E1][$E2] where type($var)==HancockWorkArrays** 
                       && (name($var)==v_max_x || (name($var)==v_max_y))
    ==> $var($E1,$E2)
  $var1[$E1][$E2].$var2 => $var1.$var2($E1,$E2) where name($var2) in {"rho","p"}
  $var1[$E1][$E2].$var2[$E3] => $var1.$var2($E1,$E2) where name($var2)=="a"

transformRhs(exp):
  $var[$E1][$E2] where elementType(var)==TYPE
    ==> $var.get($E1,$E2)

 */
void TFTransformation::transformHancockAccess(string exprTypeName,SgNode* root) {
  RoseAst ast(root);
  std::string matchexpression;
  matchexpression+="$Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp(SgArrowExp($WORK,$DS),$E1),$E2),$RHS)";
  matchexpression+="| $Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2),$RHS)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  // print result in readable form for demo purposes
  if(trace) std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    statementTransformations++;
    if(trace) std::cout << "MATCH-LHS: \n"; 
    //SgNode* n=(*i)["X"];
    for(SingleMatchVarBindings::iterator vars_iter=(*i).begin();vars_iter!=(*i).end();++vars_iter) {
      SgNode* matchedTerm=(*vars_iter).second;
      if(trace) std::cout << "  VAR: " << (*vars_iter).first << "=" << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
    string oldCode0=(*i)["$Root"]->unparseToString();
    if((*i)["$LHS"]) {
      writeTransformations++;
      SgNode* workLhs=(*i)["$WORK"];
      if(workLhs) {
        if(trace)
          cout<< "WORK:"<<workLhs<<" : "<<workLhs->unparseToString()<<endl;
      }
      if(trace) {
        cout<< "DS:"<<(*i)["$DS"]<<" : "<<(*i)["$DS"]->unparseToString()<<endl;
        cout<< "E1:"<<(*i)["$E1"]<<" : "<<(*i)["$E1"]->unparseToString()<<endl;
        cout<< "E2:"<<(*i)["$E2"]<<" : "<<(*i)["$E2"]->unparseToString()<<endl;
        cout<< "RHS:"<<(*i)["$RHS"]<<" : "<<(*i)["$RHS"]->unparseToString()<<endl;
        // need to store old code before rhs-transformations are applied
        cout<< "OLDCODE0-ROOT:"<<oldCode0<<endl;
      }
    }
#if 1
    {
      SgNode* rhsRoot=(*i)["$RHS"];
      transformRhs(exprTypeName,rhsRoot);
    }    
#endif

    // transform LHS: work -> dV[E1][E2] = RHS; ==> work -> dV.set(E1,E2,RHS);
    
    string newCode0;
    string oldCode="/* OLD: "+oldCode0+"; */\n";
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
      if(true || lhsTypeName==exprTypeName) {
        SgNode* workLhs=(*i)["$WORK"];
        string work;
        if(workLhs) {
          work=workLhs->unparseToString();
        }
        string ds=(*i)["$DS"]->unparseToString();
        string e1=(*i)["$E1"]->unparseToString();
        string e2=(*i)["$E2"]->unparseToString();
        string rhs=(*i)["$RHS"]->unparseToString();

        if(workLhs)
          newCode0=work+" -> "+ds+".set("+e1+","+e2+","+rhs+")";
        else
        newCode0=ds+".set("+e1+","+e2+","+rhs+")";
      } else {
        cout<<"DEBUG: lhs-matches, but type does not. skipping."<<lhsTypeName<<"!="<<exprTypeName<<endl;
        // var=$RHS
        //  string rhs=(*i)["$RHS"]->unparseToString();
        //newCode0=((*i)["$LHS"])->unparseToString()+rhs;
      }
      string newCode="      "+newCode0; // ';' is unparsed as part of the statement that contains the assignop
      SgNodeHelper::replaceAstWithString((*i)["$Root"], oldCode+newCode);
      std::cout << std::endl;
      std::string lineCol=SgNodeHelper::sourceLineColumnToString((*i)["$Root"]);
      if(trace) {
        cout <<"TRANSFORMATION: "<<lineCol<<" OLD:"<<oldCode0<<endl;
        cout <<"TRANSFORMATION: "<<lineCol<<" NEW:"<<newCode0<<endl;
      }
    }
  }

  checkAndTransformVarAssignments(exprTypeName,root);

  //m.printMarkedLocations();
  //m.printMatchOperationsSequence();
  cout<<"Transformation statistics:"<<endl;
  cout<<"Number of statement transformations: "<<statementTransformations<<endl;
  cout<<"STATS: Number of read access transformations: "<<readTransformations<<endl;
  cout<<"STATS: Number of write access transformations: "<<writeTransformations<<endl;
  int totalTransformations=readTransformations+writeTransformations;
  cout<<"Total number of transformations: "<<totalTransformations<<endl;
}
