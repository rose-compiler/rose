#include "sage3basic.h"

#include "AstTerm.h"
#include "SgNodeHelper.h"
#include "AstProcessing.h"
#include "AstMatching.h"

#include "Typeforge/SpecFrontEnd.hpp"
#include "Typeforge/TFTransformation.hpp"
#include "Typeforge/CppStdUtilities.hpp"
#include "Typeforge/TFTypeTransformer.hpp"
#include "Typeforge/CastStats.hpp"
#include "Typeforge/ToolConfig.hpp"
#include "Typeforge/CommandList.hpp"

#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <utility>
#include <functional>
#include <regex>
#include <algorithm>

#define CHANGE_EVERY_TYPE  "change_every_type"
#define CHANGE_EVERY_BASE  "change_every_basetype"
#define CHANGE_VAR_TYPE    "change_var_type"
#define CHANGE_VAR_BASE    "change_var_basetype"
#define CHANGE_HANDLE_TYPE "change_handle_type"
#define CHANGE_HANDLE_BASE "change_handle_basetype"
#define CHANGE_SET_TYPE    "change_set_type"
#define CHANGE_SET_BASE    "change_set_basetype"
#define LIST_CHANGES_TYPE  "list_changes_type"
#define LIST_CHANGES_BASE  "list_changes_basetype"
#define SET_CHANGES_TYPE   "set_changes_type"
#define SET_CHANGES_BASE   "set_changes_basetype"
#define TRANSFORM          "transform"
#define ADD_INCLUDE        "add_include"
#define REPLACE_PRAGMA     "replace_pragma"
#define FIND_SETS          "find_sets"
#define ADD_SPEC           "import"
#define AD_INST            "ad_intermediate_instrumentation"

#ifndef DEBUG__SpecFrontEnd
#  define DEBUG__SpecFrontEnd 0
#endif
#ifndef DEBUG__SpecFrontEnd__parse
#  define DEBUG__SpecFrontEnd__parse DEBUG__SpecFrontEnd
#endif
#ifndef DEBUG__SpecFrontEnd__readJSONFile
#  define DEBUG__SpecFrontEnd__readJSONFile DEBUG__SpecFrontEnd
#endif

namespace Typeforge {

using namespace std;

static bool isComment(string s) {
  return s.size()>=2 && s[0]=='/' && s[1]=='/';
}

static bool checkTypeforgeExtension(string filePath, string extension){
  boost::filesystem::path pathObj(filePath);
  if(pathObj.has_extension()){
    if(pathObj.extension().string() == extension) return true;
  }
  return false;
}

void SpecFrontEnd::parse(std::vector<std::string> const & filenames, CommandList & commandList) {
  for (auto filename : filenames) {
    parse(filename, commandList);
  }
}

bool SpecFrontEnd::parse(std::string const & filename, CommandList & commandList) {  
  if (checkTypeforgeExtension(filename, ".tf")) {
#if DEBUG__SpecFrontEnd__parse
    std::cout << "Reading TF file " << filename << std::endl;
#endif
    return readTFFile(filename, commandList);
  } else {
#if DEBUG__SpecFrontEnd__parse
    std::cout << "Reading JSON file " << filename << std::endl;
#endif
    return readJSONFile(filename, commandList);
   }
}

//Read in json and generate command list
bool SpecFrontEnd::readJSONFile(string const & fileName, CommandList & commandList){
  {
    std::ifstream f(fileName.c_str());
    if (!f.good()) {
      std::cerr << "Cannot open the file " << fileName << ". It is advised to use absolute files." << std::endl;
      exit(1);
    }
  }

#if DEBUG__SpecFrontEnd__readJSONFile
  std::cout << "SpecFrontEnd::readJSONFile" << std::endl;
  std::cout << "  fileName = " << fileName << std::endl;
#endif

  ToolConfig * config = new ToolConfig(fileName);
  auto const & actions = config->getActions();

#if DEBUG__SpecFrontEnd__readJSONFile
  std::cout << "  actions.size() = " << actions.size() << std::endl;
#endif

  for (auto const & act: actions) {

    std::string const & handle = act.second.getHandle();
    std::string const & action = act.second.getActionType();

#if DEBUG__SpecFrontEnd__readJSONFile
    std::cout << "  handle = " << handle << std::endl;
    std::cout << "  action = " << action << std::endl;
#endif

    bool deprecatedBase    = ((action == "replace_varbasetype") || (action == "change_varbasetype") || (action == "replace_basetype") || (action == "change_basetype") || (action == "list_basereplacements"));
    bool base = (
      action == CHANGE_VAR_BASE ||
      action == CHANGE_EVERY_BASE ||
      action == LIST_CHANGES_BASE ||
      action == CHANGE_SET_BASE ||
      deprecatedBase
    );

    bool deprecatedVar     = ((action == "replace_vartype") || (action == "replace_varbasetype") || (action == "change_vartype") || (action == "change_varbasetype"));
    bool deprecatedEvery   = ((action == "replace_type") || (action == "replace_basetype") || (action == "change_type") || (action == "change_basetype"));
    bool deprecatedList    = ((action == "list_replacements") || (action == "list_basereplacements"));
    bool deprecatedInclude = ((action == "introduce_include"));

    if (
      action == CHANGE_VAR_TYPE ||
      action == CHANGE_VAR_BASE ||
      deprecatedVar
    ) {
      assert(handle != "");
      commandList.addHandleCommand(handle, act.second.getToType(), base, false);
    } else if (
      action == CHANGE_EVERY_TYPE ||
      action == CHANGE_EVERY_BASE ||
      deprecatedEvery
    ) {
      string functionName = "$global";
      std::vector<std::string> functionConstructSpecList = {""};
      std::vector<std::string> functionSpecSplit;
      if(act.second.getScope() != "$global") {
        // parse func_name:body,args,ret
        functionSpecSplit = CppStdUtilities::splitByRegex(act.second.getScope(),":");
        if (functionSpecSplit.size()!=2) {
          std::cerr << "Error: wrong function specifier " << act.second.getScope() << std::endl;
          exit(1);
        }
        functionName = functionSpecSplit[0];
        functionConstructSpecList = CppStdUtilities::splitByRegex(functionSpecSplit[1],",");
      } 
      for(auto functionConstructSpec : functionConstructSpecList) {
        commandList.addTypeCommand(functionConstructSpec, functionName, act.second.getToType(), act.second.getFromType(), base, false);
      }
    } else if (
      action == TRANSFORM
    ) {
      commandList.addTransformCommand(act.second.getScope(), act.second.getFromType(), act.second.getName());
    } else if (
      action == AD_INST
    ) {
      commandList.addTransformCommand(act.second.getScope(), act.second.getFromType(), action);
    } else if (
      action == LIST_CHANGES_TYPE ||
      action == LIST_CHANGES_BASE ||
      deprecatedList
    ) {
      string scope = act.second.getScope();
      if (scope == "") scope = "*";

      if (scope == "*" || scope == "$global") {
        commandList.addTypeCommand("", "$global", act.second.getToType(), act.second.getFromType(), base, true);
      }

      if (scope != "$global"){
        commandList.addTypeCommand("body", scope, act.second.getToType(), act.second.getFromType(), base, true);
        commandList.addTypeCommand("args", scope, act.second.getToType(), act.second.getFromType(), base, true);
        commandList.addTypeCommand("ret", scope, act.second.getToType(), act.second.getFromType(), base, true);
      }

    } else if (
      action == ADD_INCLUDE ||
      deprecatedInclude
    ) {
      commandList.addIncludeCommand(act.second.getScope(), act.second.getName());
    } else if (
      action == REPLACE_PRAGMA
    ) {
      commandList.addPragmaCommand(act.second.getFromType(), act.second.getToType());
    } else if (
      action == ADD_SPEC
    ) {
      parse(act.second.getName(), commandList);
    } else {
      cout<<"Error: Unrecognized Action "<<action<<endl;
      return true;
    }
  }
  return false;
}

bool SpecFrontEnd::readTFFile(string const & specFileName, CommandList & commandList){
  CppStdUtilities::DataFileVector lines;
  bool fileOK=CppStdUtilities::readDataFile(specFileName,lines);
  if(fileOK) {
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
        assert(false); // commandList.addVarTypeCommand(varName, functionName, typeName, transformBase, false);
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
          bool listing=false;
          commandList.addTypeCommand(functionConstructSpec, functionName, newTypeSpec, oldTypeSpec, transformBase, listing);
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
        if(splitLine[1] == "" || splitLine[1] == "$global"){
          if(splitLine[1] == "") splitLine[1] = "*";
          commandList.addTypeCommand("", "$global", splitLine[3], splitLine[2], base, true);
        }
        if(splitLine[1] != "$global"){
          commandList.addTypeCommand("body", splitLine[1], splitLine[3], splitLine[2], base, true);
          commandList.addTypeCommand("args", splitLine[1], splitLine[3], splitLine[2], base, true);
          commandList.addTypeCommand("ret", splitLine[1], splitLine[3], splitLine[2], base, true);
        }
      }
      else if(commandName == ADD_INCLUDE){
        commandList.addIncludeCommand(splitLine[1], splitLine[2]);
      }
      else if(commandName == REPLACE_PRAGMA){
        for(unsigned int i = 3; i < splitLine.size(); i++) splitLine[2] = splitLine[2] + ";" + splitLine[i];
        commandList.addPragmaCommand(splitLine[1], splitLine[2]);
      }
      else if(commandName == ADD_SPEC){
        parse(splitLine[1], commandList);
      }
      else {
        cerr<<"Error: unknown command "<<commandName<<" in line "<<lineNr<<"."<<endl;
        return true;
      }
    }
    return false;
  } else {
    cerr<<"Error: could not access file "<<specFileName<<endl;
    return true;
  }
  return true;
}

}

