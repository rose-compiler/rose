#include "sage3basic.h"
#include "CommandList.h"
#include "SpecFrontEnd.h"
#include "TFTransformation.h"
#include "CppStdUtilities.h"
#include <iostream>
#include <vector>
#include "SgNodeHelper.h"
#include "Analysis.h"
#include <regex>
#include "CppStdUtilities.h"
#include <boost/algorithm/string.hpp>
#include "TFTypeTransformer.h"

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

static void changeVariableType(
  SgType * toType,
  SgFunctionDeclaration * funDecl,
  std::string location,
  bool base,
  SgType * fromType,
  bool listing
) {
#if DEBUG__TypeCommand_changeVariableType
  std::cout << "ENTER TypeCommand::changeVariableType" << std::endl;
  std::cout << "    $ funDecl       = " << funDecl << " (" << ( funDecl != nullptr ? funDecl->class_name() : "" ) << ")" << std::endl;
  std::cout << "    $ location      = " << location << std::endl;
  std::cout << "    $ toType        = " << toType  << " (" << ( toType != nullptr ? toType->class_name() : "" ) << ")" << std::endl;
  std::cout << "    $ base          = " << base << std::endl;
  std::cout << "    $ fromType      = " << fromType << " (" << ( fromType != nullptr ? fromType->class_name() : "" ) << ")" << std::endl;
  std::cout << "    $ listing       = " << listing << std::endl;
#endif

  assert(location.find("TYPEFORGE::") == 0);
  assert(fromType != nullptr);

  // Process type changes inside of a function

  if (funDecl != nullptr) {
    if (!SgNodeHelper::nodeCanBeChanged(funDecl)) return;

    if (funDecl->get_definingDeclaration() != nullptr) {
      funDecl = isSgFunctionDeclaration(funDecl->get_definingDeclaration());
    }
    assert(funDecl != nullptr);

    // Function Parameter

    if (location == "TYPEFORGE::args") {
#if DEBUG__TypeCommand_changeVariableType
      std::cout << " SCAN function parameters" << std::endl;
#endif
      SgFunctionDeclaration * dfdecl = isSgFunctionDeclaration(funDecl->get_definingDeclaration());
      if (dfdecl == nullptr) {
        dfdecl = funDecl;
      }
      for (auto varInitName : dfdecl->get_args()) {
#if DEBUG__TypeCommand_changeVariableType
        std::cout << "   * varInitName    = " << std::hex << varInitName << " (" << varInitName->class_name() << ")" << std::endl;
        std::cout << "       ->get_name() = " << varInitName->get_name() << std::endl;
#endif
        if (Typeforge::isTypeBasedOn(::Typeforge::typechain.getType(varInitName), fromType, base)) {
          ::Typeforge::transformer.changeType(varInitName, toType, base, listing);
        }
      }
    }

    /////////////////////////////////////
    // Return Type                     //
    /////////////////////////////////////

    if (location == "TYPEFORGE::ret") {
#if DEBUG__TypeCommand_changeVariableType
      std::cout << " SCAN return type" << std::endl;
#endif

      funDecl = isSgFunctionDeclaration(funDecl->get_firstNondefiningDeclaration());
      assert(funDecl != nullptr);

      SgFunctionType * funType = funDecl->get_type();
      SgType * funRetType = funType->get_return_type();
      if (Typeforge::isTypeBasedOn(funRetType, fromType, base)) {
        ::Typeforge::transformer.changeType(funDecl, toType, base, listing);
      }
    }

    /////////////////////////////////////
    // Function definition             //
    /////////////////////////////////////

    SgFunctionDefinition* funDef = funDecl->get_definition();

    if (funDef != NULL && location == "TYPEFORGE::body") {

      if (!SgNodeHelper::nodeCanBeChanged(funDef)) return;

#if DEBUG__TypeCommand_changeVariableType
      std::cout << " SCAN function body" << std::endl;
#endif

      RoseAst ast(funDef);
      for (RoseAst::iterator i = ast.begin(); i != ast.end(); ++i) {
        if (SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
          if (isSgTemplateVariableDeclaration(varDecl) || !SgNodeHelper::nodeCanBeChanged(varDecl)) continue;

          if (Typeforge::isTypeBasedOn(::Typeforge::typechain.getType(varDecl), fromType, base)) {
            ::Typeforge::transformer.changeType(varDecl, toType, base, listing);
          }
        }
      }
    }
  } else if (location == "TYPEFORGE::global") {

    /////////////////////////////////////
    // Change type of global variables //
    /////////////////////////////////////

#if DEBUG__TypeCommand_changeVariableType
    std::cout << " SCAN global variables" << std::endl;
#endif

    list<SgVariableDeclaration*> listOfVars = SgNodeHelper::listOfGlobalVars(::Typeforge::project);
    listOfVars.splice(listOfVars.end(), SgNodeHelper::listOfGlobalFields(::Typeforge::project));
    for (auto varDecl: listOfVars) {
      if (isSgTemplateVariableDeclaration(varDecl) || !SgNodeHelper::nodeCanBeChanged(varDecl)) continue;

      if (Typeforge::isTypeBasedOn(::Typeforge::typechain.getType(varDecl), fromType, base)) {
        ::Typeforge::transformer.changeType(varDecl, toType, base, listing);
      }
    }
  }
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

#if 1
  SgGlobal* glob = ::Typeforge::project->get_globalScopeAcrossFiles();
  SgType* otype = buildTypeFromStringSpec(oldType, glob);
  SgType* ntype = buildTypeFromStringSpec(newType, glob);

  std::vector<SgVariableDeclaration *> vdecls;
  ::Typeforge::typechain.getGlobals(vdecls, location);
  ::Typeforge::typechain.getFields(vdecls, location);
  ::Typeforge::typechain.getLocals(vdecls, location);
  for (auto n : vdecls) {
    if (isSgTemplateVariableDeclaration(n) || !SgNodeHelper::nodeCanBeChanged(n)) continue;

    if (Typeforge::isTypeBasedOn(::Typeforge::typechain.getType(n), otype, base)) {
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

  return false;
#else
  if (funName == "$global") {
    SgGlobal* globalScope = ::Typeforge::project->get_globalScopeAcrossFiles();
    SgType* oldBuiltType = buildTypeFromStringSpec(oldType, globalScope);
    SgType* newBuiltType = buildTypeFromStringSpec(newType, globalScope);
    changeVariableType(newBuiltType, nullptr, "TYPEFORGE::global", base, oldBuiltType, listing);
    return false;
  } else {
    list<SgFunctionDeclaration*> listOfFunctionDeclarations;
    if(funName=="*") {
      listOfFunctionDeclarations=SgNodeHelper::listOfFunctionDeclarations(::Typeforge::project);
    } else {
      listOfFunctionDeclarations=completeAst.findFunctionDeclarationsByName(funName);
      if(listOfFunctionDeclarations.empty()) {
        cerr<<"Error: Command "<<commandNumber<<": function "<<funName<<" does not exist."<<endl;
        return true;
      }
    }
    for (auto funDecl : listOfFunctionDeclarations) {
      SgType* oldBuiltType=buildTypeFromStringSpec(oldType,funDecl->get_scope());
      SgType* newBuiltType=buildTypeFromStringSpec(newType,funDecl->get_scope());
      if (::Typeforge::transformer.getTraceFlag()) {
        std::cout << "TRACE: TypeCommand::run : adding type transformation to list: " << oldBuiltType->unparseToString() << " ==> " << newBuiltType->unparseToString() << std::endl;
      }
      if (funDecl->get_firstNondefiningDeclaration()) {
        funDecl = isSgFunctionDeclaration(funDecl->get_firstNondefiningDeclaration());
      }
      changeVariableType(newBuiltType, funDecl, "TYPEFORGE::" + location, base, oldBuiltType, listing);
    }
    return false;
  }
#endif
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
  assert(n != nullptr); // TODO possible when working with multiple files: (1) list change, (2) external tool select, (3) apply transform ; can also happen for user provided handles so we might need some warnings!

#if DEBUG__HandleCommand__run
  std::cout << "  n      = " << n << " ( " << n->class_name() << ")" << std::endl;
#endif

  if(SgVariableDeclaration* varDec = isSgVariableDeclaration(n)) {
    SgScopeStatement * scope = varDec->get_scope();
    SgType * newBuiltType = buildTypeFromStringSpec(newType, scope);
    ::Typeforge::transformer.addTransformation(varDec, newBuiltType, base);

  } else if (SgInitializedName* initName = isSgInitializedName(n)) {
    SgScopeStatement * scope = isSgDeclarationStatement(initName->get_parent())->get_scope();
    SgType * newBuiltType = buildTypeFromStringSpec(newType, scope);
    ::Typeforge::transformer.addTransformation(initName, newBuiltType, base);

  } else if(SgFunctionDeclaration* funDec = isSgFunctionDeclaration(n)) {
    SgFunctionDefinition * funDef = funDec->get_definition();
    SgType * newBuiltType = buildTypeFromStringSpec(newType, funDef);
    ::Typeforge::transformer.addTransformation(funDec, newBuiltType, base);
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

