#ifndef TYPEFORGE_COMMAND_LIST_H
#define TYPEFORGE_COMMAND_LIST_H

#include "sage3basic.h"

#include "AstTerm.h"

namespace Typeforge {

class TransformDirective;
class TFTypeTransformer;
class TFTransformation;

class Command {
  protected:
    bool base;
    bool listing;
    int commandNumber;

    Command(bool changeBase, bool justList, int number);

  public:
    virtual int run(RoseAst completeAst, TFTransformation & tfTransformation) = 0;
};

class TypeCommand : public Command {
  std::string location;
  std::string funName;
  std::string newType;
  std::string oldType;
  public:
    TypeCommand(std::string loc, std::string fun, std::string toType, std::string fromType, bool base, bool listing, int number);

    int run(RoseAst completeAst, TFTransformation & tfTransformation);
};

class HandleCommand : public Command {
  std::string handle;
  std::string newType;
  public:
    HandleCommand(std::string nodeHandle, std::string toType, bool base, bool listing, int number);
    int run(RoseAst completeAst, TFTransformation & tfTransformation);
};

class TransformCommand : public Command {
  std::string functionName;
  std::string accessTypeName;
  std::string transformationName;
  public:
    TransformCommand(std::string funName, std::string typeName, std::string transformName, int number);
    int run(RoseAst completeAst, TFTransformation & tfTransformation);
};

class IncludeCommand : public Command {
  std::string functionName;
  std::string includeName;
  public:
    IncludeCommand(std::string funName, std::string inName, int number);
    int run(RoseAst completeAst, TFTransformation & tfTransformation);
};

class PragmaCommand : public Command {
  std::string fromMatch;
  std::string toReplace;
  public:
    PragmaCommand(std::string from, std::string to, int number);
    int run(RoseAst completeAst, TFTransformation & tfTransformation);
};

class CommandList {
  public:
    int runCommands(TFTransformation & tfTransformation);

    void addTypeCommand(std::string location, std::string funName, std::string newType, std::string oldType, bool base, bool listing);
    void addHandleCommand(std::string handle, std::string newType, bool base, bool listing);
    void addTransformCommand(std::string funName, std::string typeName, std::string transformName);
    void addIncludeCommand(std::string funName, std::string includeName);
    void addPragmaCommand(std::string fromMatch, std::string toReplace);

  private:
    void addCommand(Command * cmd);

    std::vector< Command * > commandsList;
    std::list< TransformDirective * > _list;

  friend class Command;
};

}

#endif
