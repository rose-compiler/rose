#include "sage3basic.h"

#include "SgNodeHelper.h"

#include "Typeforge/CommandList.hpp"
#include "Typeforge/SpecFrontEnd.hpp"
#include "Typeforge/TFTransformation.hpp"
#include "Typeforge/CppStdUtilities.hpp"
#include "Typeforge/Analysis.hpp"
#include "Typeforge/TFTypeTransformer.hpp"

#include <iostream>
#include <vector>
#include <regex>

#include <boost/algorithm/string.hpp>

#ifndef DEBUG__buildTypeFromStringSpec
#  define DEBUG__buildTypeFromStringSpec 0
#endif
#ifndef DEBUG__TypeCommand__run
#  define DEBUG__TypeCommand__run 0
#endif
#ifndef DEBUG__TypeCommand_changeVariableType
#  define DEBUG__TypeCommand_changeVariableType 0
#endif
#ifndef DEBUG__HandleCommand__run
#  define DEBUG__HandleCommand__run 0
#endif
#ifndef DEBUG__SetTypeCommand__run
#  define DEBUG__SetTypeCommand__run 0
#endif

namespace Typeforge {

using namespace std;

// Returns base type of varInitName if the name ofthe base type matches typename. Else returns nullptr.
static SgType* checkType(SgInitializedName* varInitName, string typeName) {
  SgType* varInitType=varInitName->get_type();
  SgType* baseType=varInitType->findBaseType();
  if(baseType) {
    if(SgNamedType* namedType=isSgNamedType(baseType)) {
      string nameTypeString=namedType->get_name();
      if(nameTypeString==typeName) {
        return baseType;
      }
    }
  }
  return nullptr;
}

// Returns the type that matches the userDefinedTypeName inside the given function if it exists. Else returns nullptr.
static SgType* findUserDefinedTypeByName(SgFunctionDefinition* funDef, string userDefinedTypeName) {
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

// Returns the SgType* that mathces the type defined by the string in the given scope. If no type matches will exit.
static SgType* buildTypeFromStringSpec(string type, SgScopeStatement* providedScope) {
#if DEBUG__buildTypeFromStringSpec
  std::cout << "ENTER buildTypeFromStringSpec" << std::endl;
#endif
  SgType* newType=nullptr;
  regex e1("[_A-Za-z:]+|\\*|&|const|<|>");
  regex_token_iterator<string::iterator> rend;
  regex_token_iterator<string::iterator> a ( type.begin(), type.end(), e1 );
  bool buildConstType=false;
  bool isLongType=false;
  bool isShortType=false;
  while (a!=rend) {
    string typePart=*a++;
    if(typePart=="<" || typePart==">") {
      cerr<<"Error: unsupported type "<<type<<endl;
      cerr<<"Note: Parameterized types are not supported as type names. Try to use a typename introduced by a typedef instead."<<endl;
      exit(1);
    } else if(typePart=="float") {
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
    } else if(regex_match(typePart, regex("^[_A-Za-z:]+$"))) {
      if(newType!=nullptr) goto parseerror;
      // TV: I kept this logic even if I don't know why it exists...
      if(SgFunctionDefinition* funDef=isSgFunctionDefinition(providedScope)) {
        SgType* userDefinedType=findUserDefinedTypeByName(funDef,typePart);
        if(userDefinedType) {
          newType=userDefinedType;
        }
      }
      // TV: new logic handling qualified named and building opaque types in the global scope
      if (newType == nullptr) {
        SgScopeStatement * globalScope = SageInterface::getGlobalScope(providedScope);
        ROSE_ASSERT(globalScope != NULL);
        // TODO handle scoping: split `typePart` using `::` then lookup/create namespaces
        newType=SageBuilder::buildOpaqueType(typePart, globalScope);
#if DEBUG__buildTypeFromStringSpec
        std::cout << " - newType = " << newType << " ( " << newType->class_name() << "): " << newType->unparseToString() << "" << std::endl;
#endif
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


Command::Command(bool changeBase, bool justList, int number) :
  base(changeBase),
  listing(justList),
  commandNumber(number)
{}

//Type command will take all instances of oldType and change them to newType inside the given function(or $global)
//and for specified location(ret,args,body)
TypeCommand::TypeCommand(string loc, string fun, string toType, string fromType, bool base, bool listing, int number) : Command(base, listing, number){
  location = loc;
  funName  = fun;
  newType  = toType;
  oldType  = fromType;
}
 
int TypeCommand::run(RoseAst completeAst, TFTransformation & tfTransformation) {
#if DEBUG__TypeCommand__run
  std::cout << "TypeCommand::run()" << std::endl;
  std::cout << "  location = " << location << std::endl;
  std::cout << "  funName  = " << funName  << std::endl;
  std::cout << "  newType  = " << newType  << std::endl;
  std::cout << "  oldType  = " << oldType  << std::endl;
#endif
  if (::Typeforge::transformer.getTraceFlag()) {
    std::cout << "TRACE: TypeCommand::run started." << std::endl;
  }

  SgGlobal* glob = ::Typeforge::project->get_globalScopeAcrossFiles();
  SgType* otype = buildTypeFromStringSpec(oldType, glob);
  SgType* ntype = buildTypeFromStringSpec(newType, glob);

  std::vector<SgVariableDeclaration *> vdecls;
  ::Typeforge::typechain.getGlobals(vdecls, location);
#if DEBUG__TypeCommand__run
  std::cout << "  # vdecls = " << vdecls.size() << std::endl;
#endif
  ::Typeforge::typechain.getFields(vdecls, location);
#if DEBUG__TypeCommand__run
  std::cout << "  # vdecls = " << vdecls.size() << std::endl;
#endif
  ::Typeforge::typechain.getLocals(vdecls, location);
#if DEBUG__TypeCommand__run
  std::cout << "  # vdecls = " << vdecls.size() << std::endl;
#endif
  for (auto n : vdecls) {
#if DEBUG__TypeCommand__run
    std::cout << "    n = " << n << " (" << n->class_name() << ")" << std::endl;
#endif
    bool cannot_be_changed = !SgNodeHelper::nodeCanBeChanged(n);
#if DEBUG__TypeCommand__run
    std::cout << "    cannot_be_changed = " << ( cannot_be_changed ? "true" : "false" ) << std::endl;
#endif
    if (isSgTemplateVariableDeclaration(n) || cannot_be_changed) continue;

    SgType * ctype = ::Typeforge::typechain.getType(n);
    assert(ctype != nullptr);
#if DEBUG__TypeCommand__run
    std::cout << "    ctype = " << ctype << " (" << ctype->class_name() << ")" << std::endl;
#endif
    if (Typeforge::isTypeBasedOn(ctype, otype, base)) {
      ::Typeforge::transformer.changeType(n, ntype, base, listing);
    }
  }

  std::vector<SgFunctionDeclaration *> fdecls;
  ::Typeforge::typechain.getFunctions(fdecls, location);
  ::Typeforge::typechain.getMethods(fdecls, location);
  for (auto n : fdecls) {
    if ( isSgTemplateFunctionDeclaration(n) ||
         isSgTemplateMemberFunctionDeclaration(n) ||
         !SgNodeHelper::nodeCanBeChanged(n)
    ) continue;

    if (Typeforge::isTypeBasedOn(::Typeforge::typechain.getType(n), otype, base)) {
      ::Typeforge::transformer.changeType(n, ntype, base, listing);
    }
  }

  std::vector<SgInitializedName *> inames;
  ::Typeforge::typechain.getParameters(inames, location);
  for (auto n : inames) {
    if (!SgNodeHelper::nodeCanBeChanged(n)) continue;

    if (Typeforge::isTypeBasedOn(::Typeforge::typechain.getType(n), otype, base)) {
      ::Typeforge::transformer.changeType(n, ntype, base, listing);
    }
  }

  std::vector<SgFunctionCallExp *> calls;
  ::Typeforge::typechain.getCallExp(calls, location);
  for (auto n : calls) {
    if (!SgNodeHelper::nodeCanBeChanged(n)) continue;

    if (Typeforge::isTypeBasedOn(::Typeforge::typechain.getType(n), otype, base)) {
      ::Typeforge::transformer.changeType(n, ntype, base, listing);
    }
  }

  return false;
}

//Replaces the type of the variable specified by the handlde with newType
HandleCommand::HandleCommand(string nodeHandle, string toType, bool base, bool listing, int number) :
  Command(base, listing, number)
{
  handle  = nodeHandle;
  newType = toType; 
}

int HandleCommand::run(RoseAst completeAst, TFTransformation& tfTransformation) {
#if DEBUG__HandleCommand__run
    std::cout << "HandleCommand::run" << std::endl;
    std::cout << "  handle = " << handle << std::endl;
#endif

  SgNode * n = ::Typeforge::typechain.getNode(handle);
  if (n == nullptr) {
    // TODO possible when working with multiple files:
    //   (1) list change,
    //   (2) external tool select,
    //   (3) apply transform
    // Can also happen for user provided handles so we might need some warnings!
    return 0;
  }

#if DEBUG__HandleCommand__run
  std::cout << "  n      = " << n << " ( " << n->class_name() << ")" << std::endl;
#endif

  if (SgVariableDeclaration* varDec = isSgVariableDeclaration(n)) {
    SgScopeStatement * scope = varDec->get_scope();
    SgType * newBuiltType = buildTypeFromStringSpec(newType, scope);
    ::Typeforge::transformer.addTransformation(varDec, newBuiltType, base);

  } else if (SgInitializedName* initName = isSgInitializedName(n)) {
    SgScopeStatement * scope = isSgDeclarationStatement(initName->get_parent())->get_scope();
    SgType * newBuiltType = buildTypeFromStringSpec(newType, scope);
    ::Typeforge::transformer.addTransformation(initName, newBuiltType, base);

  } else if (SgFunctionDeclaration* funDec = isSgFunctionDeclaration(n)) {
    SgFunctionDefinition * funDef = funDec->get_definition();
    SgType * newBuiltType = buildTypeFromStringSpec(newType, funDef);
    ::Typeforge::transformer.addTransformation(funDec, newBuiltType, base);

  } else if (SgExpression * expr = isSgExpression(n)) {
    SgScopeStatement * scope = SageInterface::getScope(expr);
    assert(scope != nullptr);
    SgType * newBuiltType = buildTypeFromStringSpec(newType, scope);
    ::Typeforge::transformer.addTransformation(expr, newBuiltType, base);

  } else {
    assert(false);
  }

  return 0;
}

//Performs specified transformation
TransformCommand::TransformCommand(string funName, string typeName, string transformName, int number) : Command(false, false, number){
  functionName = funName;
  accessTypeName = typeName;
  transformationName = transformName;
}

int TransformCommand::run(RoseAst completeAst, TFTransformation& tfTransformation){
  list<SgFunctionDefinition*> listOfFunctionDefinitions;
  if(functionName=="*") {
    listOfFunctionDefinitions=SgNodeHelper::listOfFunctionDefinitions(::Typeforge::project);
  } else {
    SgFunctionDefinition* funDef=completeAst.findFunctionByName(functionName);
    if(funDef==nullptr) {
      cerr<<"Error: Command "<<commandNumber<<": function "<<functionName<<" not found."<<endl;
      return true;
    } else {
      listOfFunctionDefinitions.push_back(funDef);
    }
  }

  for (auto funDef : listOfFunctionDefinitions) {
    SgType* accessType=buildTypeFromStringSpec(accessTypeName,funDef);
    if(::Typeforge::transformer.getTraceFlag()) { cout<<"TRACE: transformation: "<<transformationName<<endl;}
    if(transformationName=="readwrite_access_transformation") {
      tfTransformation.addReadWriteTransformation(funDef, accessType);
    } else if(transformationName=="arrayofstructs_access_transformation") {
      tfTransformation.addArrayStructTransformation(funDef, accessType);
    } else if(transformationName=="ad_intermediate_instrumentation") {
      tfTransformation.addADTransformation(funDef);
    }
  }
  return false; 
}

//Adds includes to file
IncludeCommand::IncludeCommand(string funName, string inName, int number) : Command(false, false, number){
  functionName = funName;
  includeName = inName;
}

int IncludeCommand::run(RoseAst completeAst, TFTransformation& tfTransformation) {
  SgFilePtrList listOfFiles;
  if(functionName=="*") {
    listOfFiles = ::Typeforge::project->get_files();
  } else {
    SgNode* currentNode=completeAst.findFunctionByName(functionName);
    while(currentNode != nullptr && !isSgFile(currentNode)){
      currentNode = currentNode->get_parent();
    }
    if(currentNode==nullptr) {
      return false;
    } else {
      if(SgFile* file = isSgFile(currentNode)){
        listOfFiles.push_back(file);
      }
    }
  }
  for (auto file : listOfFiles) {
    if(SgSourceFile* source = isSgSourceFile(file)){
      tfTransformation.addIncludeTransformation(includeName, false, source);
    }  
  }
  return false; 
}

//Will replace #pragma with given insetion
PragmaCommand::PragmaCommand(string from, string to, int number) : Command(false, false, number){
  fromMatch = from;
  toReplace = to;
}

int PragmaCommand::run(RoseAst completeAst, TFTransformation& tfTransformation) {
  tfTransformation.addPragmaTransformation(fromMatch, toReplace);
  return false;
}

// Call the run command on all commands stored in the list.

int CommandList::runCommands(TFTransformation& tfTransformation){
  RoseAst completeAst(::Typeforge::project);
  for(auto command : commandsList) {
    if (command->run(completeAst, tfTransformation)) return true;
  }
  return false;
}

//Set of methods for adding commands to the command list

void CommandList::addTypeCommand(string location, string funName, string newType, string oldType, bool base, bool listing){
  TypeCommand* newCommand = new TypeCommand(location, funName, newType, oldType, base, listing, commandsList.size());
  commandsList.push_back(newCommand);
}

void CommandList::addHandleCommand(string handle, string newType, bool base, bool listing){
  HandleCommand* newCommand = new HandleCommand(handle, newType, base, listing, commandsList.size());
  commandsList.push_back(newCommand);
}

void CommandList::addTransformCommand(string funName, string typeName, string transformName){
  TransformCommand* newCommand = new TransformCommand(funName, typeName, transformName, commandsList.size());
  commandsList.push_back(newCommand);
}

void CommandList::addIncludeCommand(string funName, string includeName){
  IncludeCommand* newCommand = new IncludeCommand(funName, includeName, commandsList.size());
  commandsList.push_back(newCommand);
}

void CommandList::addPragmaCommand(string fromMatch, string toReplace){
  PragmaCommand* newCommand = new PragmaCommand(fromMatch, toReplace, commandsList.size());
  commandsList.push_back(newCommand);
}

}

