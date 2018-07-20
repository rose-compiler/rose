#include "sage3basic.h"
#include "TFSpecFrontEnd.h"
#include "TFTransformation.h"
#include <iostream>
#include "CppStdUtilities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "AstProcessing.h"
#include "AstMatching.h"
#include "TFTypeTransformer.h"
#include "TFSpecFrontEnd.h"
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
#include <ToolConfig.hpp>

using namespace std;
using json = nlohmann::json;

bool isComment(string s) {
  return s.size()>=2 && s[0]=='/' && s[1]=='/';
}

bool nathan_checkSuffix(string s, string suffix){
  return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

string nathan_convertJSON(string fileName,TFTypeTransformer& tt){
  string tfString = "";
  ToolConfig config(fileName);
  vector<ToolAction>& actions = config.getActions();
  for(auto act: actions){
    string action = act.getActionType();
    if(action == "replace_vartype" || action == "replace_varbasetype" || action == "change_vartype" || action == "change_varbasetype"){
      tfString = tfString + action + ";" + act.getScope() + ";" + act.getName() + ";" + act.getToType() + "\n";
    }
    else if(action == "replace_type" || action == "replace_basetype" || action == "change_type" || action == "change_basetype"){
      tfString = tfString + action + ";" + act.getScope() + ";" + act.getFromType() + "=>" + act.getToType() + "\n";
    }
    else if(action == "transform"){
      tfString = tfString + action + ";" + act.getScope() + ";" + act.getFromType() + ";" + act.getName() + "\n";
    }
  }
  ofstream out(fileName + ".tf");
  out << tfString;
  out.close();
  tt.nathan_setConfig(config, fileName);
  return fileName + ".tf";
}

SgType* checkType(SgInitializedName* varInitName, string typeName) {
  SgType* varInitType=varInitName->get_type();
  SgType* baseType=varInitType->findBaseType();
  if(baseType) {
    if(SgNamedType* namedType=isSgNamedType(baseType)) {
      string nameTypeString=namedType->get_name();
      if(nameTypeString==typeName) {
        //cout<<"DEBUG: Found user-defined type:"<<typeName<<endl;
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
SgType* buildTypeFromStringSpec(string type, SgScopeStatement* providedScope) {
  SgType* newType=nullptr;
  std::regex e("[_A-Za-z]+|\\*|&|const");
  // default constructor = end-of-sequence
  std::regex_token_iterator<std::string::iterator> rend;
  std::regex_token_iterator<std::string::iterator> a ( type.begin(), type.end(), e );
  bool buildConstType=false;
  bool isLongType=false;
  bool isShortType=false;
  while (a!=rend) {
    string typePart=*a++;
    if(typePart=="float") {
      if(isLongType||isShortType) {
        cerr<<"Error: wrong type: float cannot be short or long."<<endl;
        exit(1);
      }
      newType=SageBuilder::buildFloatType();
    } else if(typePart=="double") {
      if(!isLongType)
        newType=SageBuilder::buildDoubleType();
      else
        newType=SageBuilder::buildLongDoubleType();
    } else if(typePart=="short") {
      isShortType=true;
    } else if(typePart=="long") {
      isLongType=true;
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
      if(SgFunctionDefinition* funDef=isSgFunctionDefinition(providedScope)) {
        //cout<<"DEBUG: found type name:"<<typePart<<endl;
        SgScopeStatement* funScope=funDef->get_scope();
        // check whether provided type name is a name of a user-defined type
        SgType* userDefinedType=findUserDefinedTypeByName(funDef,typePart);
        if(userDefinedType) {
          //cout<<"DEBUG: --> found user defined type :"<<typePart<<endl;
          newType=userDefinedType;
        } else {
          newType=SageBuilder::buildOpaqueType(typePart, funScope);
        }
      } else {
        // use provided scope if no funDef is provided
        newType=SageBuilder::buildOpaqueType(typePart, providedScope);
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
        //cout<<"DEBUG: found variable by type! :"<<initName->unparseToString()<<endl;
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

bool TFSpecFrontEnd::run(std::string specFileName, SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation) {
  CppStdUtilities::DataFileVector lines;
  string tempFileName = "";
  if(nathan_checkSuffix(specFileName, ".json")){
    tempFileName = nathan_convertJSON(specFileName, tt);
    specFileName = tempFileName;
  }
  bool fileOK=CppStdUtilities::readDataFile(specFileName,lines);
  RoseAst completeAst(root);
  if(fileOK) {
    int lineNr=0;
    for (auto line : lines) {
      lineNr++;
      std::vector<std::string> splitLine=CppStdUtilities::splitByRegex(line,";");
      //std::vector<std::string> splitLine=CppStdUtilities::splitByComma(line);
      string commandName,functionName,varName,typeName;
      size_t numEntries=splitLine.size();
      if(numEntries<=2 || numEntries>=5) {
	cerr<<"Error: wrong input format in file "<<specFileName<<". Wrong number of entries in line "<<lineNr<<"."<<endl;
	return true;
      }
      commandName=splitLine[0];
      functionName=splitLine[1];
      if(isComment(commandName)) {
	// line is commented out (skip)
	cout<<"Skipping line "<<lineNr<<endl;
	continue;
      }
      if(commandName=="change_vartype" || commandName=="change_varbasetype" || commandName=="replace_vartype" || commandName=="replace_varbasetype") {
	varName=splitLine[2];
	if(numEntries==4) {
	  typeName=splitLine[3];
	} else {
	  typeName="float";
	}
        bool transformBase = false;
        if(commandName == "replace_varbasetype"){
          transformBase = true;
        }
        SgFunctionDefinition* funDef;
        SgType* newType;
        if(functionName=="$global") {
          funDef=nullptr; // denote global scope
          SgGlobal* globalScope = root->get_globalScopeAcrossFiles();
          newType = buildTypeFromStringSpec(typeName,globalScope);
        } else {
          funDef=completeAst.findFunctionByName(functionName);
          if(funDef==0) {
            cerr<<"Error: function "<<functionName<<" does not exist in file."<<endl;
            return true;
          }
	  newType=buildTypeFromStringSpec(typeName,funDef);
        }
	if(newType==nullptr) {
	  cerr<<"Error: unknown type "<<typeName<<" in command file "<<specFileName<<" in line "<<lineNr<<"."<<endl;
	  return true;
	} else {
	  tt.addToTransformationList(_list,newType,funDef,varName,transformBase,nullptr);
	}
      } else if(commandName=="replace_type" || commandName=="change_type") {
	if(numEntries!=3) {
	  cerr<<"Error: wrong number of arguments in line "<<lineNr<<"."<<endl;
	  return true;
	}
	if(tt.getTraceFlag()) cout<<"TRACE: replace_type mode: "<< "in line "<<lineNr<<"."<<endl;
        bool onlyGlobalVars=false;
        std::list<SgVariableDeclaration*> listOfGlobalVars;
	string functionSpec=splitLine[1];

	string functionName;
        std::vector<std::string> functionConstructSpecList;
        std::vector<std::string> functionSpecSplit;
        if(functionSpec=="$global") {
          onlyGlobalVars=true;
        } else {
          functionSpecSplit=CppStdUtilities::splitByRegex(functionSpec,":");
          if(functionSpecSplit.size()!=2) { cerr<<"Error: wrong function specifier in line "<<lineNr<<":"<<functionSpec<<endl; exit(1);}
          functionName=functionSpecSplit[0];
          functionConstructSpecList=CppStdUtilities::splitByRegex(functionSpecSplit[1],",");
        }

	string typeReplaceSpec=splitLine[2];
	std::vector<std::string> typeReplaceSpecSplit=CppStdUtilities::splitByRegex(typeReplaceSpec,"\\s*=>\\s*");
	if(typeReplaceSpecSplit.size()!=2) { cerr<<"Error: wrong type replace specifier in line "<<lineNr<<":"<<typeReplaceSpec<<endl; exit(1);}
	string oldTypeSpec=typeReplaceSpecSplit[0];
	string newTypeSpec=typeReplaceSpecSplit[1];
	if(tt.getTraceFlag()) cout<<"TRACE: line "<<lineNr<<":"<<functionSpec<<" "<<oldTypeSpec<<" "<<newTypeSpec<<" ptrlevel:"<<pointerLevelOfType(newTypeSpec)<<" ref:"<<isReferenceType(newTypeSpec)<<" constref:"<<isConstReferenceType(newTypeSpec)<<endl;
        if(onlyGlobalVars) {
          listOfGlobalVars=SgNodeHelper::listOfGlobalVars(root);
          if(listOfGlobalVars.size()>0) {
            cout<<"Found "<<listOfGlobalVars.size() <<" global variables."<<endl;
            SgScopeStatement* globalScope=(*listOfGlobalVars.begin())->get_scope(); // obtain global scope from first var
            SgType* oldBuiltType=buildTypeFromStringSpec(oldTypeSpec,globalScope);
            SgType* newBuiltType=buildTypeFromStringSpec(newTypeSpec,globalScope);
            for(auto varDecl : listOfGlobalVars) {
              SgInitializedName* varInitName=SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
              SgType* varInitNameType=varInitName->get_type();
              if(varInitNameType==oldBuiltType) {
                varInitName->set_type(newBuiltType);
                numTypeReplace++;
              }
            }
          }
          //cout<<"Error: option 'global' not supported yet."<<endl;
          //exit(1);
        } else {
          std::list<SgFunctionDefinition*> listOfFunctionDefinitions;
          if(functionName=="*") {
            // transformation is specified to be applied to all functions, create list of all functions
            listOfFunctionDefinitions=SgNodeHelper::listOfFunctionDefinitions(root);
          } else {
            SgFunctionDefinition* funDef=completeAst.findFunctionByName(functionName);
            if(funDef==nullptr) {
              cout<<"WARNING: function "<<functionName<<" does not exist."<<endl;
            } else {
              listOfFunctionDefinitions.push_back(funDef);
            }
          }
          for (auto funDef : listOfFunctionDefinitions) {
            SgType* oldBuiltType=buildTypeFromStringSpec(oldTypeSpec,funDef);
            SgType* newBuiltType=buildTypeFromStringSpec(newTypeSpec,funDef);
            //cout<<"DEBUG: BUILT TYPES:"<<oldBuiltType->unparseToString()<<" => "<<newBuiltType->unparseToString()<<endl;
            
            for(auto functionConstructSpec : functionConstructSpecList) {
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
                exit(1);
              }
              //tt.addToTransformationList(list,newType,funDef,varName);
            } // end of loop on functionConstructSpecList
          }
        } 
      } else if(commandName=="replace_basetype" || commandName=="change_basetype") {
	if(numEntries!=3) {
	  cerr<<"Error: wrong number of arguments in line "<<lineNr<<"."<<endl;
	  return true;
	}
        bool transformBase = false;
        if(commandName == "replace_basetype" || commandName == "change_basetype"){
	  if(tt.getTraceFlag()) cout<<"TRACE: replace_basetype mode: "<< "in line "<<lineNr<<"."<<endl;
          transformBase = true;
        }else{
          if(tt.getTraceFlag()) cout<<"TRACE: replace_type mode: "<< "in line "<<lineNr<<"."<<endl;
        }
        bool onlyGlobalVars=false;
        std::list<SgVariableDeclaration*> listOfGlobalVars;
	string functionSpec=splitLine[1];

	string functionName;
        std::vector<std::string> functionConstructSpecList;
        std::vector<std::string> functionSpecSplit;
        if(functionSpec=="$global") {
          onlyGlobalVars=true;
        } else {
          functionSpecSplit=CppStdUtilities::splitByRegex(functionSpec,":");
          if(functionSpecSplit.size()!=2) { cerr<<"Error: wrong function specifier in line "<<lineNr<<":"<<functionSpec<<endl; exit(1);}
          functionName=functionSpecSplit[0];
          functionConstructSpecList=CppStdUtilities::splitByRegex(functionSpecSplit[1],",");
        }

	string typeReplaceSpec=splitLine[2];
	std::vector<std::string> typeReplaceSpecSplit=CppStdUtilities::splitByRegex(typeReplaceSpec,"\\s*=>\\s*");
	if(typeReplaceSpecSplit.size()!=2) { cerr<<"Error: wrong type replace specifier in line "<<lineNr<<":"<<typeReplaceSpec<<endl; exit(1);}
	string oldTypeSpec=typeReplaceSpecSplit[0];
	string newTypeSpec=typeReplaceSpecSplit[1];
	if(tt.getTraceFlag()) cout<<"TRACE: line "<<lineNr<<":"<<functionSpec<<" "<<oldTypeSpec<<" "<<newTypeSpec<<" ptrlevel:"<<pointerLevelOfType(newTypeSpec)<<" ref:"<<isReferenceType(newTypeSpec)<<" constref:"<<isConstReferenceType(newTypeSpec)<<endl;
        if(onlyGlobalVars) {
          listOfGlobalVars=SgNodeHelper::listOfGlobalVars(root);
          if(listOfGlobalVars.size()>0) {
            cout<<"Found "<<listOfGlobalVars.size() <<" global variables."<<endl;
            SgScopeStatement* globalScope=(*listOfGlobalVars.begin())->get_scope(); // obtain global scope from first var
            SgType* oldBuiltType=buildTypeFromStringSpec(oldTypeSpec,globalScope);
            SgType* newBuiltType=buildTypeFromStringSpec(newTypeSpec,globalScope);
            tt.addToTransformationList(_list,newBuiltType,nullptr,"",transformBase,oldBuiltType);
          }
        } else {
          std::list<SgFunctionDefinition*> listOfFunctionDefinitions;
          if(functionName=="*") {
            // transformation is specified to be applied to all functions, create list of all functions
            listOfFunctionDefinitions=SgNodeHelper::listOfFunctionDefinitions(root);
          } else {
            SgFunctionDefinition* funDef=completeAst.findFunctionByName(functionName);
            if(funDef==nullptr) {
              cout<<"WARNING: function "<<functionName<<" does not exist."<<endl;
            } else {
              listOfFunctionDefinitions.push_back(funDef);
            }
          }
          for (auto funDef : listOfFunctionDefinitions) {
            SgType* oldBuiltType=buildTypeFromStringSpec(oldTypeSpec,funDef);
            SgType* newBuiltType=buildTypeFromStringSpec(newTypeSpec,funDef);
            //cout<<"DEBUG: BUILT TYPES:"<<oldBuiltType->unparseToString()<<" => "<<newBuiltType->unparseToString()<<endl;
            for(auto functionConstructSpec : functionConstructSpecList) {
              tt.addToTransformationList(_list,newBuiltType,funDef,"TYPEFORGE"+functionConstructSpec,transformBase,oldBuiltType);
            }
          }
        }
      } 
      else if(commandName=="transform") {
        if(splitLine.size()!=4) {
          cerr<<"Error in line "<<lineNr<<": wrong number of arguments: "<<splitLine.size()<<" (should be 4)."<<endl;
        }
        if(tt.getTraceFlag()) cout<<"TRACE: transform mode: "<< "in line "<<lineNr<<"."<<endl;
        string functionName=splitLine[1];
        string accessTypeName=splitLine[2];
        string transformationName=splitLine[3];
        std::list<SgFunctionDefinition*> listOfFunctionDefinitions;
        if(functionName=="*") {
          // transformation is specified to be applied to all functions, create list of all functions
          listOfFunctionDefinitions=SgNodeHelper::listOfFunctionDefinitions(root);
        } else {
          SgFunctionDefinition* funDef=completeAst.findFunctionByName(functionName);
          if(funDef==nullptr) {
            cerr<<"Error: line "<<lineNr<<": function "<<functionName<<" not found."<<endl;
            return true;
          } else {
            listOfFunctionDefinitions.push_back(funDef);
          }
        }
        for (auto funDef : listOfFunctionDefinitions) {
          SgType* accessType=buildTypeFromStringSpec(accessTypeName,funDef);
          if(tt.getTraceFlag()) { cout<<"TRACE: transformation: "<<transformationName<<endl;}
          if(transformationName=="readwrite_access_transformation") {
            tfTransformation.transformHancockAccess(accessType,funDef);
          } else if(transformationName=="arrayofstructs_access_transformation") {
	    //cout<<"ASTTERM:"<<AstTerm::astTermWithNullValuesToString(funDef);
            tfTransformation.transformArrayOfStructsAccesses(accessType,funDef);
          } else if(transformationName=="ad_intermediate_instrumentation") {
            tfTransformation.instrumentADIntermediate(funDef);
          }
        }
      } else {
        cerr<<"Error: unknown command "<<commandName<<" in line "<<lineNr<<"."<<endl;
        return true;
      }
    }
    if(tempFileName != ""){
      remove(tempFileName.c_str());
    }
    return false;
  } else {
    cerr<<"Error: could not access file "<<specFileName<<endl;
    return true;
  }
  return true;
}

int TFSpecFrontEnd::getNumTypeReplace() {
  return numTypeReplace;
}

TFTypeTransformer::VarTypeVarNameTupleList
TFSpecFrontEnd::getTransformationList() {
  return _list;
}
