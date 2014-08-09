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
bool option_rdanalysis=false;
bool option_roserdanalysis=false;
bool option_fi_constanalysis=false;
const char* csvConstResultFileName=0;

//boost::program_options::variables_map args;


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


// experimental

bool isPointerType(SgNode* node) {
  return isSgPointerType(node);
}

class Code {
public:
  Code();
  Code(string s);
  string toString();
  void append(Code& code);
  void prepend(Code& code);
  void appendCode(string code);
  void prependCode(string code);
private:
  string code;
};

Code::Code() {
}

Code::Code(string s) {
  code=s;
}

void Code::append(Code& code) { 
  appendCode(code.toString());
}

void Code::prepend(Code& code) { 
  prependCode(code.toString());
}

string Code::toString() {
  return code;
}

void Code::appendCode(string s) {
  code+=s;
}

void Code::prependCode(string s) {
  code=s+code;
}

class CodeGenerator {
public:
  virtual string generateForwardCopyAst(SgNode* node);
  virtual string generateForwardAddressOpAndPPushInAssignment(SgAssignOp* assignOp);
  virtual string generateReversePPopHandler(SgNode* node);
  virtual string generateReverseAssign(SgAssignOp* node);
  virtual string generateTypedTopPopValue(SgType* mytype);
  virtual string generateSizeTopPopValue(SgType* mytype);
  virtual string generatePPop();
  virtual Code generateForwardIfStmt(SgNode* cond,Code thenC, Code elseC);
  virtual Code generateReverseIfStmt(SgNode* cond,Code thenC, Code elseC);
  virtual Code generateForwardWhileStmt(SgNode* cond, Code body);
  virtual Code generateReverseWhileStmt(SgNode* cond, Code body);
  virtual Code transformBlockAppendPush(Code code,int pushValue);
  string unparseCond(SgNode* cond);
private:
};

string CodeGenerator::unparseCond(SgNode* cond) {
 string condString;
 condString=cond->unparseToString(); // TODO: handle assignments in cond
 if(condString[condString.size()-1]==';')
   condString.erase(condString.size()-1); // C++11: condString.pop_back()
 return condString;
}

Code CodeGenerator::transformBlockAppendPush(Code block,int pushValue) {
  stringstream pushValueString;
  pushValueString<<pushValue;
  return Code("{"+block.toString()+"revstack.push("+pushValueString.str()+");}\n");
}

Code CodeGenerator::generateForwardIfStmt(SgNode* cond,Code thenC, Code elseC) {
  string condString=unparseCond(cond);
  string s="if("+condString+")\n "+thenC.toString()+" else "+elseC.toString();
  return Code(s);
}

Code CodeGenerator::generateForwardWhileStmt(SgNode* cond,Code bodyC) {
  static int num=0;
  string condString=unparseCond(cond);
  string var="cnt";
  stringstream ss;
  ss<<var<<num;
  string uniquevarname=ss.str();
  bodyC.prependCode(uniquevarname+"++;\n");
  string s="int "+uniquevarname+"=0;\n"+"while("+condString+")\n{"+bodyC.toString()+"}\n"+"revstack.push("+uniquevarname+");\n";
  num++;
  return Code(s);
}

Code CodeGenerator::generateReverseWhileStmt(SgNode* cond,Code bodyC) {
  static int num=0;
  string var="rcnt";
  stringstream ss;
  ss<<var<<num;
  string uniquevarname=ss.str();
  bodyC.prependCode(uniquevarname+"--;\n");
  string s="int "+uniquevarname+"=revstack.tpop();\n"+"while("+uniquevarname+">0)\n{"+bodyC.toString()+"}\n";
  num++;
  return Code(s);
}

Code CodeGenerator::generateReverseIfStmt(SgNode* cond,Code thenC, Code elseC) {
  // TODO: handle assignments in cond
  string condString="revstack.tpop()==0";
  string s="if("+condString+")\n "+"{"+thenC.toString()+"}"+" else "+"{"+elseC.toString()+"}";
  return Code(s);
}

string CodeGenerator::generateForwardCopyAst(SgNode* node) { 
  return node->unparseToString();
}

string CodeGenerator::generateSizeTopPopValue(SgType* mytype) {
  //return "size_tpop(sizeof("+mytype->unparseToString()+"))";
  return "revstack."+mytype->unparseToString()+"_tpop()";
}

string CodeGenerator::generateTypedTopPopValue(SgType* mytype) {
  string typeString=mytype->unparseToString();
  return "revstack."+typeString+"_vpop()";
}

string CodeGenerator::generatePPop() {
  return "revstack.ppop()";
}
// generate: *(ptype_push(&lhs))=rhs;
string CodeGenerator::generateForwardAddressOpAndPPushInAssignment(SgAssignOp* assignOp) {
  SgNode* lhs=SgNodeHelper::getLhs(assignOp);
  SgNode* rhs=SgNodeHelper::getRhs(assignOp);
  SgType* lhsType=isSgExpression(lhs)->get_type();
  string lhsTypeName=lhsType->unparseToString();
  string pushType=lhsTypeName;
  if(isPointerType(lhsType))
    pushType="ptr";
  string s="*(revstack.pvpush_"+pushType+"(("+lhsTypeName+"*)&"+lhs->unparseToString()+"))"+" = "+rhs->unparseToString()+";";
  return s;
}

// generate:  *(ppop)=(type)vpop();
string CodeGenerator::generateReversePPopHandler(SgNode* node) {
  if(SgExpression* exp=isSgExpression(node)) {
    SgType* mytype=exp->get_type();
    string typestring=mytype->unparseToString();
    //string s=string("*(revstack.ppop())=")+"("+mytype->unparseToString()+")"+generateSizeTopPopValue(mytype)+";";
    string s=string("")+"{ int* p=revstack.ppop();int val="+"("+mytype->unparseToString()+")"+generateSizeTopPopValue(mytype)+";*p=val;}";
    return s;
  } else {
    cerr<<"Error: node is not of type expression. Cannot determine type: "<<node->unparseToString()<<endl;
    exit(1);
  }
}

enum GMode {FWD, REV1, REV2};
enum GVariant {INCREMENTAL,AVPAIR};

#if 0
struct Mode {
  Mode(GMode gmode):mode(gmode),variant(INCREMENTAL){}
  Mode(GMode gmode, GVariant v):mode(gmode),variant(v){}
  GMode mode;
  GVariant variant;
};
#endif

string CodeGenerator::generateReverseAssign(SgAssignOp* node) {
  SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(node));
  SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(node));
  ROSE_ASSERT(lhs && rhs);
  string assignCode=lhs->unparseToString()+"="+"vpop();\n";
  return assignCode;
}


// test
Code generateFRCode(SgNode* node, Code code, GMode mode) {
  if(node==0) {
    Code code0;
    return code0;
  }
  CodeGenerator& cg=*new CodeGenerator();
  //cerr<<"P1:"<<node->sage_class_name()<<endl;

  if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
    if(mode==FWD) {
      return Code(varDecl->unparseToString());
    }
    if(mode==REV1) {
      // generate variable declaration (order needs to be reversed)
      return Code(varDecl->unparseToString());
    }
    if(mode==REV2) {
      // nothing to do
      return Code();
    }
    cerr<<"Error: wrong mode."<<endl;
    exit(1);
  }
  if(SgFunctionDeclaration* fdecl=isSgFunctionDeclaration(node)) {
    Code code;
    if(false && SgNodeHelper::isForwardFunctionDeclaration(fdecl)) {
      string dstring=fdecl->unparseToString();
      code=Code(dstring);
      return code;
    } else {
      string dstring=fdecl->unparseToString();
      cout<<"FDECL:"<<dstring<<endl;
      //code=generateFRCode(fdecl->get_definition(),code, mode); TODO: currently starting at fundefs
      code.prependCode(dstring);
      return code;
    }
  }
  if(SgFunctionDefinition* fdef=isSgFunctionDefinition(node)) {
    SgType* frtype=SgNodeHelper::getFunctionReturnType(node);
    string returnTypeString=frtype->unparseToString();
    string funNameString=SgNodeHelper::getFunctionName(node);
    SgInitializedNamePtrList& paramList=SgNodeHelper::getFunctionDefinitionFormalParameterList(node);
    string params;
    for(SgInitializedNamePtrList::iterator i=paramList.begin();i!=paramList.end();++i) {
      if(i!=paramList.begin())
        params+=",";
      params+=(*i)->unparseToString();
    }
    string fdefBodyCode=generateFRCode(fdef->get_body(),code,mode).toString();
    switch(mode) {
    case FWD: {
      string funSignature=returnTypeString+" "+funNameString+"_forward"+"("+params+")";
      return Code(string(funSignature+" {\n"+fdefBodyCode+"}\n"));
    }
    case REV1:
    case REV2:
      {
      return Code("void __reverse"+funNameString+"() {\n"+fdefBodyCode+"}\n");
    }
    default:;
    }
  }

  if(isSgIfStmt(node)) {
    SgNode* cond=SgNodeHelper::getCond(node);
    SgNode* thenB=SgNodeHelper::getTrueBranch(node);
    SgNode* elseB=SgNodeHelper::getFalseBranch(node);
    Code codeThen1=generateFRCode(thenB,code, mode);
    Code codeElse1=generateFRCode(elseB,code, mode);
    if(mode==FWD) {
      Code codeThen2=cg.transformBlockAppendPush(codeThen1,0);
      Code codeElse2=cg.transformBlockAppendPush(codeElse1,1);
      return cg.generateForwardIfStmt(cond,codeThen2,codeElse2);
    };
    if(mode==REV1||mode==REV2) {
      Code code;
      return cg.generateReverseIfStmt(cond,codeThen1,codeElse1);
    }
  }
  if(isSgWhileStmt(node)) {
    Code code;
    SgNode* cond=SgNodeHelper::getCond(node);    
    SgNode* body=SgNodeHelper::getLoopBody(node);
    Code codeBody=generateFRCode(body,code, mode);
    if(mode==FWD)
      return cg.generateForwardWhileStmt(cond,codeBody);
    if(mode==REV1||mode==REV2)
      return cg.generateReverseWhileStmt(cond,codeBody);
  }
  if(SgBasicBlock* block=isSgBasicBlock(node)) {
    RoseAst ast(block);
    RoseAst::iterator j=ast.begin();
    ++j;
    string s;
    for(;j!=ast.end();++j) {
      Code code;
      Code code2=generateFRCode(*j,code,mode);
      string scode=code2.toString();
      j.skipChildrenOnForward();
      if(mode==FWD)
        s+=scode;
      if(mode==REV1||mode==REV2)
        s=scode+s;
    }
    return Code(s);
  }
  // assign statement
  if(SgAssignOp* assignOp=isSgAssignOp(node)) {
    Code code;
    switch(mode) {
    case FWD: code.appendCode(cg.generateForwardAddressOpAndPPushInAssignment(assignOp)+"\n");return code;
    case REV1: 
      code.prependCode(cg.generateReverseAssign(assignOp));return code;
    case REV2:
      code.prependCode("vapop();"); return code;
    default:;
    }
    return Code("unknown-code");
  }
  if(isSgExprStatement(node)) {
    Code code;
    return generateFRCode(SgNodeHelper::getExprStmtChild(node), code, mode);
  }
  if(SgReturnStmt* exp=isSgReturnStmt(node)) {
    Code code;
    switch(mode) {
    case FWD: return Code(exp->unparseToString());
    case REV1:
    case REV2:
      return code;
    default:;
    }
  }
  cerr<<"Error: UNSUPPORTED:"<<node->sage_class_name()<<"::"<<node->unparseToString()<<endl;
  exit(1);
  return code;
}

void generateCode(SgProject* root, string filename) {
  list<SgFunctionDefinition*> functionDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  Code fcode;
  Code forwardCode;
  for(list<SgFunctionDefinition*>::iterator i=functionDefs.begin();i!=functionDefs.end();++i) {
    Code forwardFunctionCode=generateFRCode(*i, fcode, FWD);
    forwardCode.append(forwardFunctionCode);
  }
  //cout<<"FORWARD:\n"<<forwardCode.toString();

 
  Code rcode;
  Code reverseCode;
  for(list<SgFunctionDefinition*>::iterator i=functionDefs.begin();i!=functionDefs.end();++i) {
    Code reverseFunctionCode=generateFRCode(*i, rcode, REV1);
    reverseCode.prepend(reverseFunctionCode);
  }
  
  ofstream myfile;
  myfile.open (filename.c_str());
  myfile << "#include \"RevStack.h\""<<endl;
  myfile << "RevStack revstack;"<<endl;
  myfile << "// START OF GENERATED CODE"<<endl;
  myfile << "// FORWARD CODE"<<endl;
  myfile << forwardCode.toString();
  myfile << endl;
  myfile << "// REVERSE CODE"<<endl;
  myfile << reverseCode.toString();
  //myfile << "int main() {return 0;}"<<endl;
  myfile << "// END OF GENERATED CODE"<<endl;
  myfile.close();
}

void generateCode2(SgProject* root);

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
          || string(argv[i]) == "--csv-const-result"
          || string(argv[i]) == "--out"
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

  generateCode2(root);
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

void generateForwardCode(SgFunctionDefinition* funDef);
string generateReverseCode(SgFunctionDefinition* funDef);

void generateForwardCode(SgFunctionDefinition* funDef) {
  RoseAst ast(funDef);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isSgAssignOp(*i)) {
      SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(*i));
      SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(*i));
      ROSE_ASSERT(lhs && rhs);
      string s=string("rts.assign(&(")+lhs->unparseToString()+")"
        +", "
        +rhs->unparseToString()
        +")";
      SgNodeHelper::replaceAstWithString(*i,s);
    }
  }
}

string generateReverseCode(SgFunctionDefinition* funDef, GMode mode) {
  //Code code;
  //code=generateFRCode(funDef, code, mode);
  // insert Code after forward-Function
  //return code.toString()+"\n";
  return "void __reverse_func() { rts.reverseEvent(); }\n";
}

SgFunctionDefinition* cloneFunc(SgFunctionDefinition* originalFunDef, string prefix, string suffix) {
    SgFunctionDeclaration* originalFunDecl=originalFunDef->get_declaration();
    SgFunctionDeclaration* clonedFunDecl=isSgFunctionDeclaration(SageInterface::deepCopyNode(originalFunDecl));
    clonedFunDecl->set_name(SgName(prefix+clonedFunDecl->get_name().getString()+suffix));
    SgScopeStatement* originalFunctionParentScope=originalFunDecl->get_scope();
    SageInterface::appendStatement(clonedFunDecl, originalFunctionParentScope);
    SgFunctionDefinition* clonedFunDef=clonedFunDecl->get_definition();
    return clonedFunDef;
}

void generateCode2(SgProject* root) {
  list<SgFunctionDefinition*> functionDefs=SgNodeHelper::listOfFunctionDefinitions(root);
  for(list<SgFunctionDefinition*>::iterator i=functionDefs.begin();i!=functionDefs.end();++i) {
    SgFunctionDefinition* forwardFunDef=cloneFunc(*i,"__forward_","");
    SgFunctionDefinition* reverseFunDef=cloneFunc(*i,"__reverse_","");
    // forwardFunDef->fixupCopy()
    generateForwardCode(forwardFunDef);
    // insert the transformed forward function into original program
    string revCode=generateReverseCode(*i, REV2);
    SgNodeHelper::replaceAstWithString(reverseFunDef,revCode);
    //cout<<revCode<<endl;
  }
  root->unparse(0,0);
}
