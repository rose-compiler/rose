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
#include "TFCommandList.h"
#include <boost/filesystem.hpp>

#define CHANGE_EVERY_TYPE  "change_every_type"
#define CHANGE_EVERY_BASE  "change_every_basetype"
#define CHANGE_VAR_TYPE    "change_var_type"
#define CHANGE_VAR_BASE    "change_var_basetype"
#define CHANGE_HANDLE_TYPE "change_handle_type"
#define CHANGE_HANDLE_BASE "change_handle_basetype"
#define LIST_CHANGES_TYPE  "list_changes_type"
#define LIST_CHANGES_BASE  "list_changes_basetype"
#define TRANSFORM          "transform"
#define ADD_INCLUDE        "add_include"
#define REPLACE_PRAGMA     "replace_pragma"
#define FIND_SETS          "find_sets"
#define ADD_SPEC           "import"

using namespace std;

bool isComment(string s) {
  return s.size()>=2 && s[0]=='/' && s[1]=='/';
}

bool nathan_checkExtension(string filePath, string extension){
  boost::filesystem::path pathObj(filePath);
  if(pathObj.has_extension()){
    if(pathObj.extension().string() == extension) return true;
  }
  return false;
}

int TFSpecFrontEnd::convertJSON(string fileName){
  ToolConfig* config = new ToolConfig(fileName);
  vector<ToolAction>& actions = config->getActions();
  for(auto act: actions){
    string handle = act.getHandle();
    string action = act.getActionType();
    bool base = false;
    bool deprecatedBase    = ((action == "replace_varbasetype") || (action == "change_varbasetype") || (action == "replace_basetype") || (action == "change_basetype") || (action == "list_basereplacements"));
    bool deprecatedVar     = ((action == "replace_vartype") || (action == "replace_varbasetype") || (action == "change_vartype") || (action == "change_varbasetype"));
    bool deprecatedEvery   = ((action == "replace_type") || (action == "replace_basetype") || (action == "change_type") || (action == "change_basetype"));
    bool deprecatedList    = ((action == "list_replacements") || (action == "list_basereplacements"));
    bool deprecatedInclude = ((action == "introduce_include"));
    if(action == CHANGE_VAR_BASE || action == CHANGE_EVERY_BASE || action == LIST_CHANGES_BASE || deprecatedBase) base = true;
    if(action == CHANGE_VAR_TYPE || action == CHANGE_VAR_BASE || deprecatedVar){
      if(handle != ""){
        commandList.addHandleCommand(handle, act.getToType(), base, false);
      }else{
        commandList.addVarTypeCommand(act.getName(), act.getScope(), act.getToType(), base, false);
      }
    }
    else if(action == CHANGE_EVERY_TYPE || action == CHANGE_EVERY_BASE || deprecatedEvery){
      string functionName = "$global";
      std::vector<std::string> functionConstructSpecList = {""};
      std::vector<std::string> functionSpecSplit;
      if(act.getScope() != "$global") {
        functionSpecSplit=CppStdUtilities::splitByRegex(act.getScope(),":");
        if(functionSpecSplit.size()!=2) { cerr<<"Error: wrong function specifier "<<act.getScope()<<endl; exit(1);}
        functionName=functionSpecSplit[0];
        functionConstructSpecList=CppStdUtilities::splitByRegex(functionSpecSplit[1],",");
      } 
      for(auto functionConstructSpec : functionConstructSpecList) {
        commandList.addTypeCommand(functionConstructSpec, functionName, act.getToType(), act.getFromType(), base, false);
      }
    }
    else if(action == TRANSFORM){
      commandList.addTransformCommand(act.getScope(), act.getFromType(), act.getName());
    }
    else if(action == LIST_CHANGES_TYPE || action == LIST_CHANGES_BASE || deprecatedList){
      string scope = act.getScope();
      commandList.addFileCommand(act.getName());
      if(scope == "" || scope == "$global"){
        if(scope == "") scope = "*";
        commandList.addTypeCommand("", "$global", act.getToType(), act.getFromType(), base, true);
      }
      if(scope != "$global"){
        commandList.addTypeCommand("body", scope, act.getToType(), act.getFromType(), base, true);
        commandList.addTypeCommand("args", scope, act.getToType(), act.getFromType(), base, true);
        commandList.addTypeCommand("ret", scope, act.getToType(), act.getFromType(), base, true);
      }
      commandList.addFileCommand("");
    }
    else if(action == ADD_INCLUDE || deprecatedInclude){
      commandList.addIncludeCommand(act.getScope(), act.getName());
    }
    else if(action == REPLACE_PRAGMA){
      commandList.addPragmaCommand(act.getFromType(), act.getToType());
    }
    else if(action == ADD_SPEC){
      parse(act.getName());
    }
    else{
      cout<<"Unrecognized Action "<<action<<endl;
    }
    commandList.nextCommand();
  }
cout<<"parsed\n";
  return 0;
}

bool TFSpecFrontEnd::parse(std::string specFileName) {  
  CppStdUtilities::DataFileVector lines;
  bool fileOK=CppStdUtilities::readDataFile(specFileName,lines);
  if(nathan_checkExtension(specFileName, ".json")){
    convertJSON(specFileName);
    return false;
  }
  else if(fileOK) {
    int lineNr=0;
    for (auto line : lines) {
      lineNr++;
      if(isComment(line)) {
	// line is commented out (skip)
	cout<<"Skipping line "<<lineNr<<endl;
	continue;
      }
      std::vector<std::string> splitLine=CppStdUtilities::splitByRegex(line,";");
      string commandName,functionName,varName,typeName;
      size_t numEntries=splitLine.size();
      if(numEntries<=2) {
	cerr<<"Error: wrong input format in file "<<specFileName<<". Wrong number of entries in line "<<lineNr<<"."<<endl;
	return true;
      }
      commandName=splitLine[0];
      functionName=splitLine[1];
      
      if(commandName== CHANGE_VAR_TYPE || commandName== CHANGE_VAR_BASE) {
	varName=splitLine[2];
	if(numEntries==4) {
	  typeName=splitLine[3];
	} else {
	  typeName="float";
	}
        bool transformBase = false;
        if(commandName == CHANGE_VAR_BASE){
          transformBase = true;
        }
        commandList.addVarTypeCommand(varName, functionName, typeName, transformBase, false);
      } else if(commandName == CHANGE_EVERY_TYPE || commandName== CHANGE_EVERY_BASE) {
	if(numEntries!=3) {
	  cerr<<"Error: wrong number of arguments in line "<<lineNr<<"."<<endl;
	  return true;
	}
        bool transformBase = false;
        if(commandName == CHANGE_EVERY_BASE){
          transformBase = true;
        }
        
	string functionSpec=splitLine[1];

	string functionName = "$global";
        std::vector<std::string> functionConstructSpecList = {""};
        std::vector<std::string> functionSpecSplit;
        if(functionSpec != "$global") {
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

        for(auto functionConstructSpec : functionConstructSpecList) {
          commandList.addTypeCommand(functionConstructSpec, functionName, newTypeSpec, oldTypeSpec, transformBase, false);
        }
      } 
      else if(commandName== TRANSFORM) {
        if(splitLine.size()!=4) {
          cerr<<"Error in line "<<lineNr<<": wrong number of arguments: "<<splitLine.size()<<" (should be 4)."<<endl;
        }
        commandList.addTransformCommand(splitLine[1], splitLine[2], splitLine[3]);
      }
      else if(commandName == CHANGE_HANDLE_TYPE || commandName == CHANGE_HANDLE_BASE){
        bool base = false;
        if(commandName == CHANGE_HANDLE_BASE) base = true;
        commandList.addHandleCommand(splitLine[1], splitLine[2], base, false);
      }
      else if(commandName == LIST_CHANGES_TYPE || commandName == LIST_CHANGES_BASE){
        bool base = false;
        if(commandName == LIST_CHANGES_BASE) base = true;
        commandList.addFileCommand(splitLine[4]);
        if(splitLine[1] == "" || splitLine[1] == "$global"){
          if(splitLine[1] == "") splitLine[1] = "*";
          commandList.addTypeCommand("", "$global", splitLine[3], splitLine[2], base, true);
        }
        if(splitLine[1] != "$global"){
          commandList.addTypeCommand("body", splitLine[1], splitLine[3], splitLine[2], base, true);
          commandList.addTypeCommand("args", splitLine[1], splitLine[3], splitLine[2], base, true);
          commandList.addTypeCommand("ret", splitLine[1], splitLine[3], splitLine[2], base, true);
        }
        commandList.addFileCommand("");
      }
      else if(commandName == ADD_INCLUDE){
        commandList.addIncludeCommand(splitLine[1], splitLine[2]);
      }
      else if(commandName == REPLACE_PRAGMA){
        for(unsigned int i = 3; i < splitLine.size(); i++) splitLine[2] = splitLine[2] + ";" + splitLine[i];
        commandList.addPragmaCommand(splitLine[1], splitLine[2]);
      }
      else if(commandName == ADD_SPEC){
        parse(splitLine[1]);
      }
      else {
        cerr<<"Error: unknown command "<<commandName<<" in line "<<lineNr<<"."<<endl;
        return true;
      }
      commandList.nextCommand();
    }
    return false;
  } else {
    cerr<<"Error: could not access file "<<specFileName<<endl;
    return true;
  }
  return true;
}

int TFSpecFrontEnd::run(SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation){
  int temp = commandList.runCommands(root, tt, tfTransformation);
  _list = commandList.getTransformationList();
  return temp;
}

int TFSpecFrontEnd::getNumTypeReplace() {
  return numTypeReplace;
}

TFTypeTransformer::VarTypeVarNameTupleList
TFSpecFrontEnd::getTransformationList() {
  return _list;
}
