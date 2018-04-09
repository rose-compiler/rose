
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>

#include "rose.h"
#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "Timer.h"
#include "CommandLineOptions.h"
#include "AstProcessing.h"
#include "AstMatching.h"
#include "Sawyer/Graph.h"

//preparation for using the Sawyer command line parser
//#define USE_SAWYER_COMMANDLINE
#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

#include "CastStats.h"
#include "CastTransformer.h"
#include "CastGraphVis.h"

using namespace std;

class TestTraversal : public AstSimpleProcessing {
public:
  virtual void visit(SgNode* node) { /* do nothing */ };
};

void makeAllCastsExplicit(SgProject* root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgCastExp* castExp=isSgCastExp(*i)) {
      if(castExp->isCompilerGenerated()) {
	castExp->unsetCompilerGenerated();
      }
    }
  }
}


void annotateImplicitCastsAsComments(SgProject* root) {
  RoseAst ast(root);
  std::string matchexpression="$CastNode=SgCastExp($CastOpChild)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,root);
  //std::cout << "Number of matched patterns with bound variables: " << r.size() << std::endl;
  list<string> report;
  int statementTransformations=0;
  for(MatchResult::reverse_iterator i=r.rbegin();i!=r.rend();++i) {
    statementTransformations++;
    SgCastExp* castExp=isSgCastExp((*i)["$CastNode"]);
    ROSE_ASSERT(castExp);
    SgExpression* childNode=isSgExpression((*i)["$CastOpChild"]);
    ROSE_ASSERT(childNode);
    if(castExp->isCompilerGenerated()) {
      SgType* castType=castExp->get_type();
      string castTypeString=castType->unparseToString();
      SgType* castedType=childNode->get_type();
      string castedTypeString=castedType->unparseToString();
      string reportLine="compiler generated cast: "
        +SgNodeHelper::sourceLineColumnToString(castExp->get_parent())
        +": "+castTypeString+" <== "+castedTypeString;
      if(castType==castedType) {
        reportLine+=" [ no change in type. ]";
      }
      // line are created in reverse order
      report.push_front(reportLine); 
      
      string newSourceCode;
      newSourceCode="/*CAST("+castTypeString+")*/";
      newSourceCode+=castExp->unparseToString();
      castExp->unsetCompilerGenerated(); // otherwise it is not replaced
      SgNodeHelper::replaceAstWithString(castExp,newSourceCode);
    }
  }
  for(list<string>::iterator i=report.begin();i!=report.end();++i) {
    cout<<*i<<endl;
  }
  //m.printMarkedLocations();
  //m.printMatchOperationsSequence();
  cout<<"Number of compiler generated casts: "<<statementTransformations<<endl;
}

void changeVariableType(SgProject* root, string varNameToFind, SgType* type) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      SgInitializedName* varInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
      if(varInitName) {
	SgSymbol* varSym=SgNodeHelper::getSymbolOfInitializedName(varInitName);
	if(varSym) {
	  string varName=SgNodeHelper::symbolToString(varSym);
	  if(varName==varNameToFind) {
	    cout<<"STATUS: found declaration of var "<<varNameToFind<<". Changed type to "<<type->unparseToString()<<"."<<endl;
	    SgTypeFloat* ft=SageBuilder::buildFloatType();
	    varInitName->set_type(ft);
	  }
	}
      }
    }
  }
}

string toolName="typeforge";

int main (int argc, char* argv[])
{
  ROSE_INITIALIZE;
  Rose::global_options.set_frontend_notes(false);
  Rose::global_options.set_frontend_warnings(false);
  Rose::global_options.set_backend_warnings(false);

  // Command line option handling.
#ifdef USE_SAWYER_COMMANDLINE
    namespace po = Sawyer::CommandLine::Boost;
#else
    namespace po = boost::program_options;
#endif

  po::options_description desc
    ("Supported options");

  desc.add_options()
    ("help,h", "produce this help message.")
    ("version,v", "display the version.")
    ("annotate", "annotate implicit casts as comments.")
    ("explicit", "make all imlicit casts explicit.")
    ("stats", "print statistics on casts of built-in floating point types.")
    ("dot-type-graph", "generate typegraph in dot file 'typegraph.dot'.")
    ("float-var", po::value< string >()," change type of var [arg] to float.")
    ("double-var", po::value< string >()," change type of var [arg] to double.")
    ("long-double-var", po::value< string >()," change type of var [arg] to long double.")
    ;

  po::store(po::command_line_parser(argc, argv).
            options(desc).allow_unregistered().run(), args);
  po::notify(args);

  if (args.count("help")) {
    cout << toolName <<" <filename> [OPTIONS]"<<endl;
    cout << desc << "\n";
    exit(0);
  }

  if(args.isUserProvided("version")) {
    cout<<toolName<<" version 0.1.0"<<endl;
    return 0;
  }

  for (int i=1; i<argc; ++i) {
    if (string(argv[i]) == "--float-var"
        || string(argv[i]) == "--double-var"
	|| string(argv[i]) == "--long-double-var"
        ) {
      argv[i] = strdup("");
      assert(i+1<argc);
      argv[i+1] = strdup("");
    }
  }
  vector<string> argvList(argv, argv+argc);
  argvList.push_back("-rose:skipfinalCompileStep");
  SgProject* sageProject=frontend (argvList); 

  if(args.isUserProvided("explicit")) {
    makeAllCastsExplicit(sageProject);
    cout<<"Converted all implicit casts to explicit casts."<<endl;
  }

  if(args.isUserProvided("stats")) {
    CastStats castStats;
    castStats.computeStats(sageProject);
    cout<<castStats.toString();
    return 0;
  }

  if(args.isUserProvided("annotate")) {
    annotateImplicitCastsAsComments(sageProject);
    cout<<"Annotated program with comments."<<endl;
    backend(sageProject);
    return 0;
  }
  
  if(args.isUserProvided("dot-type-graph")) {
    generateTypeGraph(sageProject);
    return 0;
  }

  if(args.isUserProvided("float-var")) {
    cout<<"Changing variable type."<<endl;
    string varName=args.getString("float-var");
    changeVariableType(sageProject, varName, SageBuilder::buildFloatType());
  }
  if(args.isUserProvided("double-var")) {
    cout<<"Changing variable type."<<endl;
    string varName=args.getString("double-var");
    changeVariableType(sageProject, varName, SageBuilder::buildDoubleType());
  } 
  if(args.isUserProvided("long-double-var")) {
    cout<<"Changing variable type."<<endl;
    string varName=args.getString("long-double-var");
    changeVariableType(sageProject, varName, SageBuilder::buildLongDoubleType());
  }

  bool transform=args.isUserProvided("float-var")||args.isUserProvided("double-var")||args.isUserProvided("long-double-var");
  if(transform) {
    // make all floating point casts explicit
    makeAllCastsExplicit(sageProject);
    // transform all casts now
    CastTransformer ct;
    ct.traverseWithinCommandLineFiles(sageProject);
  }

  backend(sageProject);

  return 0;
}
