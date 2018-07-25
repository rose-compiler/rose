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

string nathan_convertJSON(string fileName,TFTypeTransformer& tt, CommandList& commandList){
  ToolConfig* config = new ToolConfig(fileName);
  vector<ToolAction>& actions = config->getActions();
  string outName = "";
  for(auto act: actions){
    string handle = act.getHandle();
    string action = act.getActionType();
    bool base = false;
    if(action == "replace_varbasetype" || action == "change_varbasetype" || action == "replace_basetype" || action == "change_basetype" || action == "list_basereplacements") base = true;
    if(handle == ""){
      if(action == "replace_vartype" || action == "replace_varbasetype" || action == "change_vartype" || action == "change_varbasetype"){
        commandList.addVarTypeCommand(act.getName(), act.getScope(), act.getToType(), base, false);
      }
      else if(action == "replace_type" || action == "replace_basetype" || action == "change_type" || action == "change_basetype"){
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
      else if(action == "transform"){
        commandList.addTransformCommand(act.getScope(), act.getFromType(), act.getName());
      }
      else if(action == "list_basereplacements" || action == "list_replacements"){
        commandList.addTypeCommand("", "$global", act.getToType(), act.getFromType(), base, true);
        commandList.addTypeCommand("body", "*", act.getToType(), act.getFromType(), base, true);
        commandList.addTypeCommand("args", "*", act.getToType(), act.getFromType(), base, true);
        commandList.addTypeCommand("ret", "*", act.getToType(), act.getFromType(), base, true);
        outName = act.getName();
      }
    }
    else{
      commandList.addHandleCommand(handle, act.getToType(), base, false);
    }
    commandList.nextCommand();
  }
  ToolConfig* newConfig = config;
  try{
    newConfig = new ToolConfig(outName);
  }catch(...){
    remove(outName.c_str());
    newConfig->getActions().clear();
  }
  tt.nathan_setConfig(newConfig);
  if(outName != "") tt.nathan_setConfigFile(outName);
  return outName;
}

bool TFSpecFrontEnd::run(std::string specFileName, SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation) {
  RoseAst completeAst(root);
  CommandList commandList(specFileName);
  string tempFileName = "";
  CppStdUtilities::DataFileVector lines;
  bool fileOK=CppStdUtilities::readDataFile(specFileName,lines);
  if(nathan_checkExtension(specFileName, ".json")){
    tempFileName = nathan_convertJSON(specFileName, tt, commandList);
    commandList.runCommands(root, tt, tfTransformation);
    _list = commandList.getTransformationList();
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
      if(numEntries<=2 || numEntries>=5) {
	cerr<<"Error: wrong input format in file "<<specFileName<<". Wrong number of entries in line "<<lineNr<<"."<<endl;
	return true;
      }
      commandName=splitLine[0];
      functionName=splitLine[1];
      
      if(commandName=="change_vartype" || commandName=="change_varbasetype" || commandName=="replace_vartype" || commandName=="replace_varbasetype") {
	varName=splitLine[2];
	if(numEntries==4) {
	  typeName=splitLine[3];
	} else {
	  typeName="float";
	}
        bool transformBase = false;
        if(commandName == "replace_varbasetype" || commandName == "change_varbasetype"){
          transformBase = true;
        }
        commandList.addVarTypeCommand(varName, functionName, typeName, transformBase, false);
      } else if(commandName == "replace_type" || commandName == "change_type" || commandName=="replace_basetype" || commandName=="change_basetype") {
	if(numEntries!=3) {
	  cerr<<"Error: wrong number of arguments in line "<<lineNr<<"."<<endl;
	  return true;
	}
        bool transformBase = false;
        if(commandName == "replace_basetype" || commandName == "change_basetype"){
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
      else if(commandName=="transform") {
        if(splitLine.size()!=4) {
          cerr<<"Error in line "<<lineNr<<": wrong number of arguments: "<<splitLine.size()<<" (should be 4)."<<endl;
        }
        if(tt.getTraceFlag()) cout<<"TRACE: transform mode: "<< "in line "<<lineNr<<"."<<endl;
        commandList.addTransformCommand(splitLine[1], splitLine[2], splitLine[3]);
      }
      else if(commandName == "handle" || commandName == "handle_base"){
        bool base = false;
        if(commandName == "handle_base") base = true;
        commandList.addHandleCommand(splitLine[1], splitLine[2], base, false);
      }
      else if(commandName == "list_replacements" || commandName == "list_basereplacements"){
        bool base = false;
        if(commandName == "list_basereplacements") base = true;
        tt.nathan_setConfigFile(splitLine[3]);
        commandList.addTypeCommand("", "$global", splitLine[2], splitLine[1], base, true);
        commandList.addTypeCommand("body", "*", splitLine[2], splitLine[1], base, true);
        commandList.addTypeCommand("args", "*", splitLine[2], splitLine[1], base, true);
        commandList.addTypeCommand("ret", "*", splitLine[2], splitLine[1], base, true);
      }
      else {
        cerr<<"Error: unknown command "<<commandName<<" in line "<<lineNr<<"."<<endl;
        return true;
      }
      commandList.nextCommand();
    }
    if(tempFileName != ""){
      remove(tempFileName.c_str());
    }
    commandList.runCommands(root, tt, tfTransformation);
    _list = commandList.getTransformationList();
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
