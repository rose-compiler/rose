
#include <iostream>
#include <fstream>
#include <sstream>
#include "rose.h"
#include "AstTerm.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"
#include <list>
#include <vector>
#include "Timer.h"
#include "CommandLineOptions.h"

//preparation for using the Sawyer command line parser
//#define USE_SAWYER_COMMANDLINE
#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

using namespace std;

stringstream ss;

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

string nodeString(SgExpression* node) {
  stringstream tempss;
  tempss<<"x"<<node;
  return tempss.str();
}

string nodeId(SgExpression* node) {
  if(isSgVarRefExp(node)) {
    return node->unparseToString(); // TODO: use variableid
  } else {
    return nodeString(node);
  }
}

string dotString(string s) {
  return SgNodeHelper::doubleQuotedEscapedString(s);
}

void addNode(SgExpression* node) {
  SgType* type=node->get_type();
  string color;
  switch(type->variantT()) {
  case V_SgTypeFloat: color="blue";break;
  case V_SgTypeDouble: color="green";break;
  case V_SgTypeLongDouble: color="red";break;
  default: color="white";
  }
  string labelInfo;
  labelInfo=string("\\n")+"type:"+type->unparseToString();

  if(isSgUnaryOp(node)||isSgBinaryOp(node)||isSgConditionalExp(node)||isSgCallExpression(node)) {
    ss<<nodeId(node)<<"[label=\"op:"<<node->class_name()+labelInfo<<"\" fillcolor="<<color<<" style=filled];"<<endl;
  } else if(isSgVarRefExp(node)) {
    ss<<nodeId(node)<<"[label=\"var:"<<node->unparseToString()+labelInfo<<"\" fillcolor="<<color<<" style=filled];"<<endl;
  } else if(isSgValueExp(node)) {
    ss<<nodeId(node)<<"[label=\"val:"<<dotString(node->unparseToString())+labelInfo<<"\" fillcolor="<<color<<" style=filled];"<<endl;
  } else {
    ss<<nodeId(node)<<"[label=\"node:"<<node->class_name()+labelInfo<<"\" fillcolor="<<color<<" style=filled];"<<endl;
  }
}

void addEdge(SgExpression* from, SgExpression* to) {
  if(from->unparseToString()=="FE_UPWARD") {
    cout<<"DEBUG:   "<<from->unparseToString()<<endl;
    cout<<"DEBUG:p :"<<from->get_parent()->unparseToString()<<endl;
    cout<<"DEBUG:pp:"<<from->get_parent()->get_parent()->unparseToString()<<endl;
  }
  ss<<nodeId(to)<<" -> "<<nodeId(from)<<"[dir=back];"<<endl;
}

void generateTypeGraph(SgProject* root) {
  RoseAst ast(root);
  ss<<"digraph G {"<<endl;
  for (RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgExpression* currentExpNode=isSgExpression(*i);
    if(currentExpNode) {
      SgExpression* parentExpNode=isSgExpression((*i)->get_parent());
      if(parentExpNode) {
	if(isSgAssignOp(parentExpNode)) {
	  // redirect assignment edge
	  SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(parentExpNode));
	  //SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(parentExpNode));
	  if(currentExpNode==lhs) {
	    std::swap(currentExpNode,parentExpNode); // invert direction of edge rhs<->assignop
	  }
	}
	addNode(currentExpNode);
	addNode(parentExpNode);
	addEdge(currentExpNode,parentExpNode);
      } else {
	addNode(currentExpNode);
      }
    }
  }
  ss<<"}"<<endl;
  write_file("typegraph.dot",ss.str());
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

string toolName="varp3";

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
    ("annotate", "annotate casts as comments.")
    ("explicit", "make all imlicit casts explicit.")
    ("type-graph", "generated dot file with type graph.")
    ("float-var", po::value< string >()," change type of var to float.")
    ("double-var", po::value< string >()," change type of var to double.")
    ("long-double-var", po::value< string >()," change type of var to long double.")
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
    cout<<toolName<<" version 0.1"<<endl;
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
  if(args.isUserProvided("annotate")) {
    annotateImplicitCastsAsComments(sageProject);
    cout<<"Annotated program with comments."<<endl;
  } else if(args.isUserProvided("float-var")) {
    cout<<"Changing variable type."<<endl;
    string varName=args.getString("float-var");
    changeVariableType(sageProject, varName, SageBuilder::buildFloatType());
  }
  if(args.isUserProvided("type-graph")) {
    generateTypeGraph(sageProject);
    exit(0);
  }

  backend(sageProject);

  return 0;
}
