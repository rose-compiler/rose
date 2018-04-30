
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
#include "TypeTransformer.h"

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
#include "CppStdUtilities.h"
#include <utility>
#include <functional>
#include <regex>
#include <algorithm>
#include <list>
#include "TFTransformation.h"

using namespace std;

class TestTraversal : public AstSimpleProcessing {
public:
  virtual void visit(SgNode* node) { /* do nothing */ };
};


string toolName="typeforge";

bool isComment(string s) {
  return s.size()>=2 && s[0]=='/' && s[1]=='/';
}

SgType* checkType(SgInitializedName* varInitName,string typeName) {
  SgType* varInitType=varInitName->get_type();
  SgType* baseType=varInitType->findBaseType();
  if(baseType) {
    if(SgNamedType* namedType=isSgNamedType(baseType)) {
      string nameTypeString=namedType->get_name();
      if(nameTypeString==typeName) {
        cout<<"DEBUG: Found user-defined type:"<<typeName<<endl;
        return baseType;
      }
    }
  }
  return nullptr;
}

// search for user defined types in function parameters and variable declarations in provided function
SgType* findUserDefinedTypeByName(SgFunctionDefinition* funDef, string userDefinedTypeName) {
  // (1) formal function parameters, (2) declared variables
  SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
  for(auto varInitName : initNamePtrList) {
    if(SgType* type=checkType(varInitName,userDefinedTypeName))
      return type;
  }
  RoseAst ast(funDef);
  for (auto node : ast) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
      SgInitializedName* varInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
      if(SgType* type=checkType(varInitName,userDefinedTypeName))
        return type;
    }
  }
  return nullptr;
}

// experimental "parser" for building types from string
SgType* buildTypeFromStringSpec(string type,SgFunctionDefinition* funDef) {
  SgType* newType=nullptr;
  std::regex e("[_A-Za-z]+|\\*|&|const|\\s");
  // default constructor = end-of-sequence
  std::regex_token_iterator<std::string::iterator> rend;
  std::regex_token_iterator<std::string::iterator> a ( type.begin(), type.end(), e );
  bool buildConstType=false;
  while (a!=rend) {
    string typePart=*a++;
    if(typePart=="float") {
      newType=SageBuilder::buildFloatType();
    } else if(typePart=="double") {
      newType=SageBuilder::buildDoubleType();
    } else if(typePart=="long double") {
      newType=SageBuilder::buildLongDoubleType();
    } else if(typePart==" ") {
    continue;
    } else if(typePart=="*") {
      if(newType==nullptr) goto parseerror;
      newType=SageBuilder::buildPointerType(newType);
    } else if(typePart=="&") {
      if(newType==nullptr) goto parseerror;
      newType=SageBuilder::buildReferenceType(newType);
    } else if(typePart=="const") {
      buildConstType=true;
    } else if(std::regex_match(typePart, std::regex("^[_A-Za-z]+$"))) {
      // found a type name
      if(funDef) {
        //cout<<"DEBUG: found type name:"<<typePart<<endl;
        SgScopeStatement* scope=funDef->get_scope();
        // check whether provided type name is a name of a user-defined type
        SgType* userDefinedType=findUserDefinedTypeByName(funDef,typePart);
        if(userDefinedType) {
          //cout<<"DEBUG: --> found user defined type :"<<typePart<<endl;
          newType=userDefinedType;
        } else {
          newType=SageBuilder::buildOpaqueType(typePart, scope);
        }
      } else {
        cout<<"WARNING: no function def for scope. not building new type:"<<typePart<<endl;
      }
    } else {
    parseerror:
      cerr<<"Error: unsupported type: "<<type<<", unresolved:"<<typePart<<"."<<endl;
      exit(1);
    }
    if(buildConstType && newType) {
      newType=SageBuilder::buildConstType(newType);
      buildConstType=false;
    }
  }
  return newType;
}

list<SgInitializedName*> findVariablesByType(SgNode* root, SgType* type) {
  list<SgInitializedName*> list;
  RoseAst ast(root);
  for (auto node : ast) {
    if(SgInitializedName* initName=isSgInitializedName(node)) {
      SgType* varInitNameType=initName->get_type();
      if(varInitNameType==type) {
        cout<<"DEBUG: found variable by type! :"<<initName->unparseToString()<<endl;
        list.push_back(initName);
      }
    }
  }
  return list;
}

// some simple string type tests (until parser is available)
size_t pointerLevelOfType(string type) {
  return std::count(type.begin(),type.end(),'*');
}
bool isReferenceType(string type) {
  return 1==std::count(type.begin(),type.end(),'&');
}
bool isConstReferenceType(string type) {
  size_t nPos = type.find("const", 0);
  return nPos!=string::npos && isReferenceType(type);
}

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
    ("trace", "print cast operations as they are performed.")
    ("dot-type-graph", "generate typegraph in dot file 'typegraph.dot'.")
    ("command-file", po::value< string >()," name of file where each line specifies how to change a variable's type: type-name function-name var-name.")
    ("csv-stats-file", po::value< string >()," generate file [args] with transformation statistics.")
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
    cout<<toolName<<" version 0.3.0"<<endl;
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
  TypeTransformer tt;

  if(args.isUserProvided("explicit")) {
    tt.makeAllCastsExplicit(sageProject);
    cout<<"Converted all implicit casts to explicit casts."<<endl;
    backend(sageProject);
    return 0;
  }

  if(args.isUserProvided("stats")) {
    CastStats castStats;
    castStats.computeStats(sageProject);
    cout<<castStats.toString();
    return 0;
  }

  if(args.isUserProvided("annotate")) {
    tt.annotateImplicitCastsAsComments(sageProject);
    cout<<"Annotated program with comments."<<endl;
    backend(sageProject);
    return 0;
  }
  
  if(args.isUserProvided("dot-type-graph")) {
    string dotFileName="typegraph.dot";
    if(generateTypeGraph(sageProject,dotFileName)) {
      cout<<"Generated file "<<dotFileName<<endl;
    } else {
      cerr<<"Error: could not write file "<<dotFileName<<endl;
    }
    return 0;
  }

  if(args.isUserProvided("trace")) {
    tt.setTraceFlag(true);
  }

  if(args.isUserProvided("command-file")) {
    TFTransformation tfTransformation;
    tfTransformation.trace=tt.getTraceFlag();
    int numTypeReplace=0;
    string commandFileName=args.getString("command-file");
    CppStdUtilities::DataFileVector lines;
    bool fileOK=CppStdUtilities::readDataFile(commandFileName,lines);
    RoseAst completeAst(sageProject);
    if(fileOK) {
      TypeTransformer::VarTypeVarNameTupleList list;
      int lineNr=0;
      for (auto line : lines) {
        lineNr++;
        //std::vector<std::string> splitLine=CppStdUtilities::splitByRegex(line,";");
        std::vector<std::string> splitLine=CppStdUtilities::splitByComma(line);
        string commandName,functionName,varName,typeName;
        size_t numEntries=splitLine.size();
        if(numEntries<=2 || numEntries>=5) {
          cerr<<"Error: wrong input format in file "<<commandFileName<<". Wrong number of entries in line "<<lineNr<<"."<<endl;
          return 1;
        }
        commandName=splitLine[0];
        functionName=splitLine[1];
        if(isComment(commandName)) {
          // line is commented out (skip)
          cout<<"Skipping line "<<lineNr<<endl;
          continue;
        }
        if(commandName=="replace_vartype") {
          varName=splitLine[2];
          if(numEntries==4) {
            typeName=splitLine[3];
          } else {
            typeName="float";
          }
          SgFunctionDefinition* funDef=completeAst.findFunctionByName(functionName);
          if(funDef==0) {
            cerr<<"Error: function "<<functionName<<" does not exist in file."<<endl;
            return 1;
          }
          SgType* newType=buildTypeFromStringSpec(typeName,funDef);
          if(newType==nullptr) {
            cerr<<"Error: unknown type "<<typeName<<" in command file "<<commandFileName<<" in line "<<lineNr<<"."<<endl;
            return 1;
          } else {
            tt.addToTransformationList(list,newType,funDef,varName);
          }
        } else if(commandName=="replace_type") {
          if(numEntries!=3) {
            cerr<<"Error: wrong number of arguments in line "<<lineNr<<"."<<endl;
            return 1;
          }
          if(tt.getTraceFlag()) cout<<"TRACE: replace_type mode: "<< "in line "<<lineNr<<"."<<endl;
          string functionSpec=splitLine[1];
          std::vector<std::string> functionSpecSplit=CppStdUtilities::splitByRegex(functionSpec,":");
          if(functionSpecSplit.size()!=2) { cerr<<"Error: wrong function specifier in line "<<lineNr<<":"<<functionSpec<<endl; exit(1);}
          string functionName=functionSpecSplit[0];
          string functionConstructSpec=functionSpecSplit[1];
          string typeReplaceSpec=splitLine[2];
          std::vector<std::string> typeReplaceSpecSplit=CppStdUtilities::splitByRegex(typeReplaceSpec,"\\s*=>\\s*");
          if(typeReplaceSpecSplit.size()!=2) { cerr<<"Error: wrong type replace specifier in line "<<lineNr<<":"<<typeReplaceSpec<<endl; exit(1);}
          string oldTypeSpec=typeReplaceSpecSplit[0];
          string newTypeSpec=typeReplaceSpecSplit[1];
          if(tt.getTraceFlag()) cout<<"TRACE: line "<<lineNr<<":"<<functionName<<" "<<functionConstructSpec<<" "<<oldTypeSpec<<" "<<newTypeSpec<<" ptrlevel:"<<pointerLevelOfType(newTypeSpec)<<" ref:"<<isReferenceType(newTypeSpec)<<" constref:"<<isConstReferenceType(newTypeSpec)<<endl;
          SgFunctionDefinition* funDef=completeAst.findFunctionByName(functionName);
          SgType* oldBuiltType=buildTypeFromStringSpec(oldTypeSpec,funDef);
          SgType* newBuiltType=buildTypeFromStringSpec(newTypeSpec,funDef);
          cout<<"DEBUG: BUILT TYPES:"<<oldBuiltType->unparseToString()<<" => "<<newBuiltType->unparseToString()<<endl;

          if(functionConstructSpec=="args") {
            // change types of arguments
            SgInitializedNamePtrList& initNamePtrList=SgNodeHelper::getFunctionDefinitionFormalParameterList(funDef);
            for(auto varInitName : initNamePtrList) {
              SgType* varInitNameType=varInitName->get_type();
              if(varInitNameType==oldBuiltType) {
                varInitName->set_type(newBuiltType);
                numTypeReplace++;
              }
            }
          } else if(functionConstructSpec=="ret") {
            // change type of return type if it matches provided type. If no type is provided always change.
            SgType* funRetType=SgNodeHelper::getFunctionReturnType(funDef); // uses get_orig_return_type
            SgFunctionDeclaration* funDecl=funDef->get_declaration();
            if(funRetType==oldBuiltType||oldBuiltType==nullptr) {
              SgFunctionType* funType=funDecl->get_type();
              funType->set_orig_return_type(newBuiltType);
              numTypeReplace++;
            }
          } else if(functionConstructSpec=="body") {
            // finds all variables in body of function and replaces type
            std::list<SgInitializedName*> varInitList=findVariablesByType(funDef, oldBuiltType);
            for (auto initName : varInitList) {
              initName->set_type(newBuiltType);
              numTypeReplace++;
            }
          } else {
            cerr<<"Error: line "<<lineNr<<": unknown function construct specifier "<<functionConstructSpec<<"."<<endl;
          }
          //tt.addToTransformationList(list,newType,funDef,varName);
        } else if(commandName=="transform") {
          if(splitLine.size()!=4) {
            cerr<<"Error in line "<<lineNr<<": wrong number of arguments: "<<splitLine.size()<<" (should be 4)."<<endl;
          }
          if(tt.getTraceFlag()) cout<<"TRACE: transform mode: "<< "in line "<<lineNr<<"."<<endl;
          string functionName=splitLine[1];
          string accessTypeName=splitLine[2];
          string transformationName=splitLine[3];
          SgFunctionDefinition* funDef=completeAst.findFunctionByName(functionName);
          if(funDef) {
            SgType* accessType=buildTypeFromStringSpec(accessTypeName,funDef);
            if(tt.getTraceFlag()) { cout<<"TRACE: transformation: "<<transformationName<<endl;}
            if(transformationName=="readwrite_access_transformation") {
              tfTransformation.transformHancockAccess(accessType,funDef);
            } else if(transformationName=="arrayofstructs_access_transformation") {
              //cout<<"ASTTERM:"<<AstTerm::astTermWithNullValuesToString(funDef);
              tfTransformation.transformArrayOfStructsAccesses(accessType,funDef);
            } else {
              cerr<<"Error in line "<<lineNr<<": unsupported transformation: "<<transformationName<<endl;
              return 1;
            }
          } else {
            cerr<<"Error: line "<<lineNr<<": function "<<functionName<<" not found."<<endl;
            return 1;
          }
        } else {
          cerr<<"Error: unknown command "<<commandName<<" in line "<<lineNr<<"."<<endl;
          return 1;
        }
      }
      tt.transformCommandLineFiles(sageProject,list);
      if(args.isUserProvided("csv-stats-file")) {
	string csvFileName=args.getString("csv-stats-file");
	tt.generateCsvTransformationStats(csvFileName,numTypeReplace,tt,tfTransformation);
      }
      tt.printTransformationStats(numTypeReplace,tt,tfTransformation);
      backend(sageProject);
      return 0;
    } else {
      cerr<<"Error: could not access file "<<commandFileName<<endl;
      return 1;
    }
  }

  if(args.isUserProvided("float-var")||args.isUserProvided("double-var")||args.isUserProvided("long-double-var")) {
    TypeTransformer::VarTypeVarNameTupleList list;
    SgFunctionDefinition* funDef=nullptr;
    if(args.isUserProvided("float-var")) {
      string varNames=args.getString("float-var");
      tt.addToTransformationList(list,SageBuilder::buildFloatType(),funDef,varNames);
    }
    if(args.isUserProvided("double-var")) {
      string varNames=args.getString("double-var");
      tt.addToTransformationList(list,SageBuilder::buildDoubleType(),funDef,varNames);
    } 
    if(args.isUserProvided("long-double-var")) {
      string varNames=args.getString("long-double-var");
      tt.addToTransformationList(list,SageBuilder::buildLongDoubleType(),funDef,varNames);
    }
    tt.transformCommandLineFiles(sageProject,list);
    backend(sageProject);
  }
  
  return 0;
}
