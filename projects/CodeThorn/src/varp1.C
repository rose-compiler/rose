// Author: Markus Schordan, 2013.
// Example AstMatcher : used for demonstrating and testing the matcher mechanism

#include <iostream>
#include <fstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"

#include "Timer.h"

using namespace std;

class TestTraversal : public AstSimpleProcessing {
public:
  virtual void visit(SgNode* node) { /* do nothing */ };
};

void write_file(std::string filename, std::string data) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);
  myfile << data;
  myfile.close();
}

int main (int argc, char* argv[])
{
  rose::global_options.set_frontend_notes(false);
  rose::global_options.set_frontend_warnings(false);
  rose::global_options.set_backend_warnings(false);

  int readTransformations=0;
  int writeTransformations=0;
  int statementTransformations=0;

  vector<string> argvList(argv, argv+argc);
  argvList.push_back("-rose:skipfinalCompileStep");
  // Build the AST used by ROSE
  //SgProject* sageProject = frontend(argc,argv);
  SgProject* sageProject=frontend (argvList); 

  
  // Run internal consistency tests on AST
  //AstTests::runAllTests(sageProject);
  //AstDOTGeneration dotGen;
  //dotGen.generate(sageProject,"matcher",AstDOTGeneration::TOPDOWN);
  SgNode* root;
  root=sageProject;
#if 0
  std::cout << "TERM INFO OUTPUT: START\n";
  std::cout << astTermToMultiLineString(root,0);
  std::cout << "TERM INFO OUTPUT: END\n";
#endif
  RoseAst ast(root);
  std::string matchexpression="$Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp(SgArrowExp($WORK,$DS),$E1),$E2),$RHS)";
  matchexpression+="| $Root=SgAssignOp($LHS=SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2),$RHS)";
  //  matchexpression+="| $Root=SgAssignOp(SgVarRefExp,$RHS)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  // print result in readable form for demo purposes
  std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    statementTransformations++;
    std::cout << "---------------------------------------------------------"<<endl;
    std::cout << "MATCH-LHS: \n"; 
    //SgNode* n=(*i)["X"];
    for(SingleMatchVarBindings::iterator vars_iter=(*i).begin();vars_iter!=(*i).end();++vars_iter) {
      SgNode* matchedTerm=(*vars_iter).second;
      std::cout << "  VAR: " << (*vars_iter).first << "=" << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
    string oldCode0=(*i)["$Root"]->unparseToString();
    if((*i)["$LHS"]) {
      writeTransformations++;
      SgNode* workLhs=(*i)["$WORK"];
      if(workLhs) {
        cout<< "WORK:"<<workLhs<<" : "<<workLhs->unparseToString()<<endl;
      }
      cout<< "DS:"<<(*i)["$DS"]<<" : "<<(*i)["$DS"]->unparseToString()<<endl;
      cout<< "E1:"<<(*i)["$E1"]<<" : "<<(*i)["$E1"]->unparseToString()<<endl;
      cout<< "E2:"<<(*i)["$E2"]<<" : "<<(*i)["$E2"]->unparseToString()<<endl;
      cout<< "RHS:"<<(*i)["$RHS"]<<" : "<<(*i)["$RHS"]->unparseToString()<<endl;
      // need to store old code before rhs-transformations are applied
      cout<< "OLDCODE0-ROOT:"<<oldCode0<<endl;
    }
#if 1
    {
      std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -"<<endl;
      // transform RHS:
      std::string matchexpressionRHSAccess="$RHSPattern=SgPntrArrRefExp(SgPntrArrRefExp($DS,$E1),$E2)";
      AstMatching mRHS;
      SgNode* root2=(*i)["$RHS"];
      cout<<"RHS-MATCHING ROOT2: "<<root2->unparseToString()<<endl;
      MatchResult rRHS=mRHS.performMatching(matchexpressionRHSAccess,root2);
      for(MatchResult::iterator j=rRHS.begin();j!=rRHS.end();++j) {
        readTransformations++;
        std::cout << "MATCH-RHS: \n"; 
        cout<< "RHS-PATTERN:"<<(*j)["$RHSPattern"]<<" : "<</*(*j)["RHSPattern"]->unparseToString()<<*/endl;
        //cout<< "RHS-WORK:"<<(*j)["$WORK"]<<" : "<<(*j)["$WORK"]->unparseToString()<<endl;
        cout<< "RHS-DS:"<<(*j)["$DS"]<<" : "<<(*j)["$DS"]->unparseToString()<<endl;
        cout<< "RHS-E1:"<<(*j)["$E1"]<<" : "<<(*j)["$E1"]->unparseToString()<<endl;
        cout<< "RHS-E2:"<<(*j)["$E2"]<<" : "<<(*j)["$E2"]->unparseToString()<<endl;
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
        cout <<"RHS-TRANSFORMATION: "<<lineCol<<" OLD:"<<oldCode0<<endl;
        cout <<"RHS-TRANSFORMATION: "<<lineCol<<" NEW:"<<newCode0<<endl;
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
    cout <<"TRANSFORMATION: "<<lineCol<<" OLD:"<<oldCode0<<endl;
    cout <<"TRANSFORMATION: "<<lineCol<<" NEW:"<<newCode0<<endl;
  }
  m.printMarkedLocations();
  m.printMatchOperationsSequence();
  
  cout<<"Transformation statistics:"<<endl;
  cout<<"Number of statement transformations: "<<statementTransformations<<endl;
  cout<<"Number of read access transformations: "<<readTransformations<<endl;
  cout<<"Number of write access transformations: "<<writeTransformations<<endl;
  int totalTransformations=readTransformations+writeTransformations;
  cout<<"Total number of transformations: "<<totalTransformations<<endl;
  write_file("astterm.txt",AstTerm::astTermToMultiLineString(root,2));
  write_file("astterm.dot",AstTerm::astTermWithNullValuesToDot(root));
  backend(sageProject);
}
