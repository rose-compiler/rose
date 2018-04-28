#include "sage3basic.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"
#include "TFTransformation.h"
#include "AstTerm.h"

using namespace std;

void TFTransformation::transformHancockAccess(SgNode* root) {
  RoseAst ast(root);
  std::string matchexpression="$Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp(SgArrowExp($WORK,$DS),$E1),$E2),$RHS)";
  matchexpression+="| $Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2),$RHS)";
  //    matchexpression+="| $Root=SgAssignOp(SgVarRefExp,$RHS)"; bug: causes crash
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  // print result in readable form for demo purposes
  if(trace) std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    statementTransformations++;
    if(trace) std::cout << "---------------------------------------------------------"<<endl;
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
      if(trace) std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
      // transform RHS:
      std::string matchexpressionRHSAccess="$RHSPattern=SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2)";
      AstMatching mRHS;
      SgNode* root2=(*i)["$RHS"];
      if(trace) cout<<"RHS-MATCHING ROOT2: "<<root2->unparseToString()<<endl;
      MatchResult rRHS=mRHS.performMatching(matchexpressionRHSAccess,root2);
      for(MatchResult::iterator j=rRHS.begin();j!=rRHS.end();++j) {
        readTransformations++;
        if(trace) {
          std::cout << "MATCH-RHS: \n"; 
          cout<< "RHS-PATTERN:"<<(*j)["$RHSPattern"]<<" : "<</*(*j)["RHSPattern"]->unparseToString()<<*/endl;
          //cout<< "RHS-WORK:"<<(*j)["$WORK"]<<" : "<<(*j)["$WORK"]->unparseToString()<<endl;
          cout<< "RHS-DS:"<<(*j)["$DS"]<<" : "<<(*j)["$DS"]->unparseToString()<<endl;
          cout<< "RHS-E1:"<<(*j)["$E1"]<<" : "<<(*j)["$E1"]->unparseToString()<<endl;
          cout<< "RHS-E2:"<<(*j)["$E2"]<<" : "<<(*j)["$E2"]->unparseToString()<<endl;
        }
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

        //        string work=(*j)["$WORK"]->unparseToString();
        string ds=(*j)["$DS"]->unparseToString();
        string e1=(*j)["$E1"]->unparseToString();
        string e2=(*j)["$E2"]->unparseToString();
#if 1
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
#endif
        mRHS.printMarkedLocations();
        mRHS.printMatchOperationsSequence();
      }
    }    
#endif

    // transform LHS: work -> dV[E1][E2] = RHS; ==> work -> dV.set(E1,E2,RHS);
    
    string newCode0;
    string oldCode="/* OLD: "+oldCode0+"; */\n";
    if((*i)["$LHS"]) {
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
      // var=$RHS
      string rhs=(*i)["$RHS"]->unparseToString();
      newCode0=((*i)["$LHS"])->unparseToString()+rhs;
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
  m.printMarkedLocations();
  m.printMatchOperationsSequence();
  
  cout<<"Transformation statistics:"<<endl;
  cout<<"Number of statement transformations: "<<statementTransformations<<endl;
  cout<<"STATS: Number of read access transformations: "<<readTransformations<<endl;
  cout<<"STATS: Number of write access transformations: "<<writeTransformations<<endl;
  int totalTransformations=readTransformations+writeTransformations;
  cout<<"Total number of transformations: "<<totalTransformations<<endl;
}
