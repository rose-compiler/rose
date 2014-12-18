// Author: Markus Schordan, 2013, 2014.

#include "rose.h"

#include "inliner.h"

#include <iostream>
#include <vector>
#include <set>
#include <list>
#include <string>
#include <cmath>

#include "SgNodeHelper.h"
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>

#include "Timer.h"
#include "RoseAst.h"

#include "limits.h"
#include "assert.h"

// ROSE analyses
#include "VariableRenaming.h"

using namespace std;

string option_prefix;

//#include "CommandLineOptions.h"
boost::program_options::variables_map args;

void generateCode(SgProject* root);
void forwardCodeTransformation(SgFunctionDefinition* funDef);

void printRoseInfo(SgProject* project) {
  project->display("PROJECT NODE");
  int fileNum=project->numberOfFiles();
  for(int i=0;i<fileNum;i++) {
    std::stringstream ss;
    SgFile* file=(*project)[i];
    ROSE_ASSERT(file);
    ss<<"FILE NODE Nr. "<<i;
    file->display(ss.str());
  }
}

bool isPointerType(SgNode* node) {
  return isSgPointerType(node);
}

class TransformationSequence : protected AstPrePostProcessing {
protected:
  virtual void preOrderVisit(SgNode *astNode);
  virtual void postOrderVisit(SgNode *astNode);
public:
  void reset();
  void create(SgNode*);
  void apply();
private:
  list<SgNode*> transformationSequence;
  list<SgNode*> transformationSequenceCommit;
};

void TransformationSequence::reset() {
  transformationSequence.clear();
}

void TransformationSequence::preOrderVisit(SgNode *astNode) {
}

void TransformationSequence::postOrderVisit(SgNode *astNode) {
  //cout<<"SgNode:"<<astNode->class_name()<<endl;
  if(isSgAssignOp(astNode)) {
    SgNode* lhs;
    lhs=SgNodeHelper::getLhs(astNode);
    transformationSequence.push_back(lhs);
  }
  if(isSgPlusAssignOp(astNode)
     || isSgMinusAssignOp(astNode)
     || isSgMultAssignOp(astNode)
     || isSgDivAssignOp(astNode)
     || isSgModAssignOp(astNode) // %=
     || isSgAndAssignOp(astNode) // &=
     || isSgIorAssignOp(astNode) // |=
     || isSgXorAssignOp(astNode) // ^=
     || isSgLshiftAssignOp(astNode) // <<=
     || isSgRshiftAssignOp(astNode) // >>=
     ) {
    SgNode* lhs=SgNodeHelper::getLhs(astNode);
    transformationSequence.push_back(lhs);
  }
  if(isSgPlusPlusOp(astNode) || isSgMinusMinusOp(astNode)) {
    SgNode* operand=SgNodeHelper::getUnaryOpChild(astNode);
    transformationSequence.push_back(operand);
  }
  if(isSgDeleteExp(astNode)) {
    transformationSequenceCommit.push_back(astNode);
  }
}

// computes the list of nodes for which the bs-memory-mod transformation must be applied
void TransformationSequence::create(SgNode* node) {
  traverse(node);
}

// TODO: handle global variables and member variables in member functions
bool isLocalVariable(SgExpression* exp) {
  return isSgVarRefExp(exp);
}

void TransformationSequence::apply() {
  for(list<SgNode*>::iterator i=transformationSequence.begin();i!=transformationSequence.end();++i) {
    if(isSgAssignOp(*i)) {
      SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(*i));
      SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(*i));
      ROSE_ASSERT(lhs && rhs);
      string s=string("rts.assign(&(")+lhs->unparseToString()+")"
        +", "
        +rhs->unparseToString()
        +")";
      cout<<"DEBUG: Applying transformation assignop: "<<s<<endl;
      SgNodeHelper::replaceAstWithString(*i,s);
    } else {
      SgExpression* exp=isSgExpression(*i);
      ROSE_ASSERT(exp);
      if(!isLocalVariable(exp)) {
        string s;
        if(isPointerType(exp->get_type())) {
          s=string("(*rts.avpushptr((void**)&(")+exp->unparseToString()+")))";
        } else {
          s=string("(*rts.avpush(&(")+exp->unparseToString()+")))";
        }
        cout<<"DEBUG: Applying transformation on operand: "<<s<<endl;
        SgNodeHelper::replaceAstWithString(*i,s);
      } else {
        cout<<"DEBUG: Detected local variable."<<endl;
      }
    }
  }
  for(list<SgNode*>::iterator i=transformationSequenceCommit.begin();i!=transformationSequenceCommit.end();++i) {
    // split delete operation in: 1) destructor call, 2) register for commit
    // TODO: implement as proper AST manipulation
    SgNode* operand=SgNodeHelper::getFirstChild(*i);
    SgExpression* deleteOperand=isSgExpression(operand);
    SgType* deleteOperandType=deleteOperand->get_type();
    //SgType* deleteOperandType2=deleteOperandType->findBaseType();
    SgDeclarationStatement* decl=deleteOperandType->getAssociatedDeclaration();
    string typeName;
    string destructorCall;
    if(SgClassDeclaration* classDecl=isSgClassDeclaration(decl)) {
      SgSymbol* symbol=classDecl->get_symbol_from_symbol_table();
      SgName name=symbol->get_name();
      typeName=name;
      destructorCall=string(operand->unparseToString())+"->"+"~"+typeName+"();";
    } else {
      if(SgExpression* exp=isSgExpression(operand)) {
        typeName=exp->get_type()->unparseToString();
        typeName.erase(typeName.size()-1); // remove trailing "*" (must be pointer)
        cout<<"INFO: delete on non-class type "<<typeName<<endl;
        destructorCall="";
      } else {
        cerr<<"Error: unknown operand or type in delete operation."<<endl;
        exit(1);
      }
    }

    string registerCall=string("rts.registerForCommit((void*)")+operand->unparseToString()+")";
    string code=destructorCall+registerCall;
    SgNodeHelper::replaceAstWithString(*i,code);
  }
}

void forwardCodeTransformation(SgFunctionDefinition* funDef) {
  TransformationSequence ts;
  ts.create(funDef);
  ts.apply();
}

void generateCode(SgProject* root) {
  list<SgFunctionDefinition*> functionDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=functionDefs.begin();i!=functionDefs.end();++i) {
    SgFunctionDefinition* forwardFunDef=*i;
    forwardCodeTransformation(forwardFunDef);
  }
  root->unparse(0,0);
}

int main(int argc, char* argv[]) {
  try {
    if(argc==1) {
      cout << "Error: wrong command line options."<<endl;
      exit(1);
    }
    // Command line option handling.
    namespace po = boost::program_options;
    boost::program_options::options_description desc
      ("reverser V0.1\n"
       "Written by Markus Schordan\n"
       "Supported options");
  
    desc.add_options()
      ("help,h", "produce this help message.")
      ("rose-help", "show help for compiler frontend options.")
      ("ast-file-node-display", "show project and file node dumps (using display()).")
      ("version,v", "display the version.")
      ;

    po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), args);
    po::notify(args);

    if (args.count("help")) {
      cout << "reverser <filename> [OPTIONS]"<<endl;
      cout << desc << "\n";
      return 0;
    }
    if (args.count("rose-help")) {
      argv[1] = strdup("--help");
    }

    if (args.count("version")) {
      cout << "backstroke version 2.0\n";
      cout << "Written by Markus Schordan 2014\n";
      return 0;
    }

    // clean up string-options in argv
    for (int i=1; i<argc; ++i) {
      if (string(argv[i]) == "--prefix" 
          ) {
        // do not confuse ROSE frontend
        argv[i] = strdup("");
        assert(i+1<argc);
        argv[i+1] = strdup("");
      }
    }

  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* root = frontend(argc,argv);
  //  AstTests::runAllTests(root);
  // inline all functions

  if(args.count("ast-file-node-display")) {
    printRoseInfo(root);
  }

  generateCode(root);
  cout<< "STATUS: finished."<<endl;

  // main function try-catch
  } catch(char* str) {
    cerr << "*Exception raised: " << str << endl;
    return 1;
  } catch(const char* str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  } catch(string str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  }
  return 0;
}
