// Author: Markus Schordan, 2013, 2014.

#include "rose.h"

#include "inliner.h"

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"
#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
#include "AstAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"
#include "ProgramStats.h"
#include "AnalysisAbstractionLayer.h"
#include "AType.h"
#include "SgNodeHelper.h"
#include "DFAstAttributeConversion.h"
#include "FIConstAnalysis.h"
#include <boost/foreach.hpp>

#include "addressTakenAnalysis.h"
#include "defUseQuery.h"
#include "Timer.h"
#include "AnalysisAbstractionLayer.h"
#include "RoseAst.h"

#include <vector>
#include <set>
#include <list>
#include <string>

#include "limits.h"
#include <cmath>
#include "assert.h"

// ROSE analyses
#include "VariableRenaming.h"

using namespace std;
using namespace CodeThorn;
using namespace AType;
using namespace DFAstAttributeConversion;
using namespace AnalysisAbstractionLayer;

#include "PropertyValueTable.h"

string option_prefix;
bool option_stats=false;

//boost::program_options::variables_map args;

void generateCode(SgProject* root);
void forwardCodeTransformation(SgFunctionDefinition* funDef);

int countSubTreeNodes(SgNode* root) {
  int num=0;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    num++;
  }
  return num;
}

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
};

void TransformationSequence::reset() {
  transformationSequence.clear();
}

void TransformationSequence::preOrderVisit(SgNode *astNode) {
}

void TransformationSequence::postOrderVisit(SgNode *astNode) {
  if(isSgAssignOp(astNode)) {
    // TODO: getLhs
    transformationSequence.push_back(astNode);
  }
  if(isSgPlusAssignOp(astNode)
     || isSgMinusAssignOp(astNode)
     || isSgMultAssignOp(astNode)
     || isSgDivAssignOp(astNode)
     // %=
     // &=
     // |=
     // ^=
     // <<=
     // >>=
     ) {
    // TODO: getUnaryChild
    transformationSequence.push_back(astNode);
  }
  // TODO: inc/dec operators
  // delete operator
}

// computes the list of nodes for which the bs-memory-mod transformation must be applied
void TransformationSequence::create(SgNode* node) {
  traverse(node);
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
      cout<<"DEBUG: Applying transformation "<<s<<endl;
      SgNodeHelper::replaceAstWithString(*i,s);
    }
  }
}

void forwardCodeTransformation(SgFunctionDefinition* funDef) {
  TransformationSequence ts;
  ts.create(funDef);
  ts.apply();
}

// clones a function definition, but also the corresponding declaration
SgFunctionDefinition* cloneFunctionDefinition(SgFunctionDefinition* originalFunDef, string prefix, string suffix) {
    SgFunctionDeclaration* originalFunDecl=originalFunDef->get_declaration();
    SgFunctionDeclaration* clonedFunDecl=isSgFunctionDeclaration(SageInterface::deepCopyNode(originalFunDecl));
    clonedFunDecl->set_name(SgName(prefix+clonedFunDecl->get_name().getString()+suffix));
    SgScopeStatement* originalFunctionParentScope=originalFunDecl->get_scope();
    SageInterface::appendStatement(clonedFunDecl, originalFunctionParentScope);
    SgFunctionDefinition* clonedFunDef=clonedFunDecl->get_definition();
    return clonedFunDef;
}

void generateCode(SgProject* root) {
  list<SgFunctionDefinition*> functionDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=functionDefs.begin();i!=functionDefs.end();++i) {
    SgFunctionDefinition* forwardFunDef=*i;
    //forwardFunDef=cloneFunctionDefinition(*i,"__forward_","");
    // forwardFunDef->fixupCopy()
    forwardCodeTransformation(forwardFunDef);
    // insert the transformed forward function into original program
    //SgFunctionDefinition* reverseFunDef=cloneFunc(*i,"__reverse_","");
    //string revCode=generateReverseCode(*i, REV2);
    //SgNodeHelper::replaceAstWithString(reverseFunDef,revCode);
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
    po::options_description desc
      ("reverser V0.1\n"
       "Written by Markus Schordan\n"
       "Supported options");
  
    desc.add_options()
      ("help,h", "produce this help message.")
      ("rose-help", "show help for compiler frontend options.")
      ("ast-file-node-display", "show project and file node dumps (using display()).")
      ("version,v", "display the version.")
      ("prefix",po::value< string >(), "set prefix [arg] for all generated files.")
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
    if (args.count("prefix")) {
      option_prefix=args["prefix"].as<string>().c_str();
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
