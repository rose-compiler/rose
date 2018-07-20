#ifndef TYPEFORGE_COMMAND_LIST_H
#define TYPEFORGE_COMMAND_LIST_H

#include "TFCommandList.h"
#include "sage3basic.h"
#include "TFSpecFrontEnd.h"
#include "TFTransformation.h"
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
#include "abstract_handle.h"
#include "roseAdapter.h"

class Command{
  protected:
    bool base;
    bool listing;
    int commandNumber;
    Command(bool changeBase, bool justList, int number);
  public:
    virtual int run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list) = 0;
};

class TypeCommand : public Command{
  std::string location;
  std::string funName;
  std::string newType;
  std::string oldType;
  public:
    TypeCommand(std::string loc, std::string fun, std::string toType, std::string fromType, bool base, bool listing, int number);
    int run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list);
};

class VarTypeCommand : public Command{
  std::string varName;
  std::string funName;
  std::string newType;
  public:
    VarTypeCommand(std::string name, std::string fun, std::string toType, bool base, bool listing, int number);
    int run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list);
};

class HandleCommand : public Command{
  std::string handle;
  std::string newType;
  public:
    HandleCommand(std::string nodeHandle, std::string toType, bool base, bool listing, int number);
    int run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list);
};

class TransformCommand : public Command{
  std::string functionName;
  std::string accessTypeName;
  std::string transformationName;
  public:
    TransformCommand(std::string funName, std::string typeName, std::string transformName, int number);
    int run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list);
};

class CommandList{
  public:
    CommandList(std::string spec);
  
    int runCommands(SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation);
  
    void addVarTypeCommand(std::string varName, std::string funName, std::string newType, bool base, bool listing);
    void addTypeCommand(std::string location, std::string funName, std::string newType, std::string oldType, bool base, bool listing);
    void addHandleCommand(std::string handle, std::string newType, bool base, bool listing);
    void addTransformCommand(std::string funName, std::string typeName, std::string transformName);
    void nextCommand();
    TFTypeTransformer::VarTypeVarNameTupleList getTransformationList();
  private:
    std::vector<Command*> commandsList;
    TFTypeTransformer::VarTypeVarNameTupleList _list;
    int nextCommandNumber = 1;
};

#endif
