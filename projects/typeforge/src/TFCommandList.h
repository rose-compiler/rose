#ifndef TYPEFORGE_COMMAND_LIST_H
#define TYPEFORGE_COMMAND_LIST_H

#include "sage3basic.h"
#include "AstTerm.h"
#include "TFTransformation.h"
#include "TFTypeTransformer.h"

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

class IncludeCommand : public Command{
  std::string functionName;
  std::string includeName;
  public:
    IncludeCommand(std::string funName, std::string inName, int number);
    int run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list);
};

class PragmaCommand : public Command{
  std::string fromMatch;
  std::string toReplace;
  public:
    PragmaCommand(std::string from, std::string to, int number);
    int run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list);
};

class FileCommand : public Command{
  std::string fileName;
  public:
    FileCommand(std::string file, int number);
    int run(SgProject* root, RoseAst completeAst, TFTypeTransformer& tt, TFTransformation& tfTransformation, TFTypeTransformer::VarTypeVarNameTupleList& _list);
};

class CommandList{
  public:
    CommandList();
  
    int runCommands(SgProject* root, TFTypeTransformer& tt, TFTransformation& tfTransformation);
  
    void addVarTypeCommand(std::string varName, std::string funName, std::string newType, bool base, bool listing);
    void addTypeCommand(std::string location, std::string funName, std::string newType, std::string oldType, bool base, bool listing);
    void addHandleCommand(std::string handle, std::string newType, bool base, bool listing);
    void addTransformCommand(std::string funName, std::string typeName, std::string transformName);
    void addIncludeCommand(std::string funName, std::string includeName);
    void addPragmaCommand(std::string fromMatch, std::string toReplace);
    void addFileCommand(std::string fileName);
    void nextCommand();
    TFTypeTransformer::VarTypeVarNameTupleList getTransformationList();
  private:
    std::vector<Command*> commandsList;
    TFTypeTransformer::VarTypeVarNameTupleList _list;
    int nextCommandNumber = 1;
};

#endif
