/*********************************
 * Author: Markus Schordan, 2012 *
 *********************************/

#include "sage3basic.h"

#include <list>
#include <sstream>
#include "SgNodeHelper.h"
#include "limits.h"
#include "RoseAst.h"
#include "CodeThornException.h"

using namespace std;

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
SgVariableSymbol* SgNodeHelper::isFunctionParameterVariableSymbol(SgNode* node) {
  if(node) {
    if(SgVariableSymbol* varsym=isSgVariableSymbol(node))
      if(SgInitializedName* initname=varsym->get_declaration())
        if(SgDeclarationStatement* declstmt=initname->get_declaration())
          if(SgFunctionParameterList* fpl=isSgFunctionParameterList(declstmt))
            if(fpl && !SgNodeHelper::isForwardFunctionDeclaration(fpl->get_parent()))
              return varsym;
  }
  return 0;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isVariableSymbolInFunctionForwardDeclaration(SgNode* varsym) {
  if(!(varsym && isSgVariableSymbol(varsym)))
    return false;
  SgInitializedName* initname=isSgVariableSymbol(varsym)->get_declaration();
  SgDeclarationStatement* declstmt=initname->get_declaration();
  SgFunctionParameterList* fpl=isSgFunctionParameterList(declstmt);
  return fpl && SgNodeHelper::isForwardFunctionDeclaration(fpl->get_parent());
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgDeclarationStatement* SgNodeHelper::findVariableDeclarationWithVariableSymbol(SgNode* node) {
  if(SgVariableSymbol* varsym=isSgVariableSymbol(node)) {
    if(SgNodeHelper::isVariableSymbolInFunctionForwardDeclaration(node)) {
      // in this case no Ast node representing a declaration exists in the ROSE AST.
      return 0;
    }
    SgInitializedName* initname=varsym->get_declaration();
    ROSE_ASSERT(initname);
    SgDeclarationStatement* declstmt=initname->get_declaration();
    ROSE_ASSERT(declstmt);
    return declstmt;
  } else {
    throw CodeThorn::Exception("SgNodeHelper::getSgVariableDeclarationOfSgVariableSymbol : parameter not a SgVariableSymbol");
  }
  return 0; // non-reachable
}

SgFunctionDeclaration* SgNodeHelper::findFunctionDeclarationWithFunctionSymbol(SgNode* node) {
  if(SgFunctionSymbol* funcsym = isSgFunctionSymbol(node)) {
    SgFunctionDeclaration* decl = funcsym->get_declaration();
    ROSE_ASSERT(decl);
    return decl;
  } else {
    throw CodeThorn::Exception("SgNodeHelper::getSgFunctionDeclarationOfSgFunctionSymbol : parameter not a SgFunctionSymbol");
  }
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
std::string SgNodeHelper::sourceFilenameToString(SgNode* node) {
  Sg_File_Info* fi=node->get_file_info();
  std::stringstream ss;
  ss<<fi->get_filenameString();
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
std::string SgNodeHelper::sourceLineColumnToString(SgNode* node) {
  return sourceLineColumnToString(node, ":");
}

/*! 
  * \author Markus Schordan
  * \date 2019.
 */
std::string SgNodeHelper::sourceLineColumnToString(SgNode* node, string separator) {
  std::stringstream ss;
  Sg_File_Info* fi=node->get_file_info();
  ss<<fi->get_line();
  ss<<separator;
  ss<<fi->get_col();
  return ss.str();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
std::string SgNodeHelper::sourceFilenameLineColumnToString(SgNode* node) {
  string filename=SgNodeHelper::sourceFilenameToString(node);
  string lc=SgNodeHelper::sourceLineColumnToString(node);
  std::stringstream ss;
  ss<<filename;
  ss<<":";
  ss<<lc;
  return ss.str();
}

std::string SgNodeHelper::lineColumnNodeToString(SgNode* node) {
  return SgNodeHelper::sourceLineColumnToString(node)+": "+SgNodeHelper::nodeToString(node);
}


/*! 
  * \author Markus Schordan
  * \date 2014.
 */
vector<SgVarRefExp*> SgNodeHelper::determineVariablesInSubtree(SgNode* node) {
  vector<SgVarRefExp*> varVec;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVarRefExp* v=isSgVarRefExp(*i)) {
      varVec.push_back(v);
    }
  }
  return varVec;
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
size_t SgNodeHelper::determineChildIndex(SgNode* child) {
  SgNode* parent=child->get_parent();
  if(parent==0)
    return -1;
  else
    return parent->get_childIndex(child);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgVarRefExp* SgNodeHelper::Pattern::matchSingleVarScanf(SgNode* node) {
  SgNode* nextNodeToAnalyze1=node;
  if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) ) {
    string fName=SgNodeHelper::getFunctionName(funCall);
    SgExpressionPtrList& actualParams=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(fName=="scanf") {
      if(actualParams.size()==2) {
        SgAddressOfOp* addressOp=isSgAddressOfOp(actualParams[1]);
        if(!addressOp) {
          throw CodeThorn::Exception("SgNodeHelper::Pattern::matchSingleVarScanf: unsupported scanf argument #2 (no address operator found). Required form: scanf(\"%d\",&v).");
        }
        SgVarRefExp* varRefExp=isSgVarRefExp(SgNodeHelper::getFirstChild(addressOp));
        if(!varRefExp) {
          throw CodeThorn::Exception("SgNodeHelper::Pattern::matchSingleVarScanf: unsupported scanf argument #2 (no variable found). Required form: scanf(\"%d\",&v).");
        }
        // matched: SgAddressOfOp(SgVarRefExp())
        return varRefExp;
      } else {
        throw CodeThorn::Exception("SgNodeHelper::Pattern::matchSingleVarScanf: unsupported number of arguments of scanf.Exactly one variable of the form scanf(\"%d\",&v) is required.");
      }
    }
  }
  return 0;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::Pattern::OutputTarget::isKnown() {
  return outType!=SgNodeHelper::Pattern::OutputTarget::UNKNOWNOPERATION && outType!=SgNodeHelper::Pattern::OutputTarget::UNKNOWNPRINTF;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNodeHelper::Pattern::OutputTarget SgNodeHelper::Pattern::matchSingleVarOrValuePrintf(SgNode* node) {
  OutputTarget outputTarget;
  SgNode* nextNodeToAnalyze1=node;
  if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) ) {
    string fName=SgNodeHelper::getFunctionName(funCall);
    SgExpressionPtrList& actualParams=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(fName=="printf") {
      if(actualParams.size()==2) {
        if(SgVarRefExp* varRefExp=isSgVarRefExp(actualParams[1])) {
          outputTarget.outType=SgNodeHelper::Pattern::OutputTarget::VAR;
          outputTarget.varRef=varRefExp;
          return outputTarget;
        }           
        if(SgIntVal* intValNode=isSgIntVal(actualParams[1])) {
          outputTarget.outType=SgNodeHelper::Pattern::OutputTarget::INT;
          outputTarget.intVal=intValNode->get_value();
          return outputTarget;
        }
      }
      outputTarget.outType=SgNodeHelper::Pattern::OutputTarget::UNKNOWNPRINTF;
      return outputTarget;
    }
  }    
  outputTarget.outType=SgNodeHelper::Pattern::OutputTarget::UNKNOWNOPERATION;
  return outputTarget;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgVarRefExp* SgNodeHelper::Pattern::matchSingleVarPrintf(SgNode* node) {
  SgNode* nextNodeToAnalyze1=node;
  if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) ) {
    string fName=SgNodeHelper::getFunctionName(funCall);
    SgExpressionPtrList& actualParams=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(fName=="printf") {
      if(actualParams.size()==2) {
        SgVarRefExp* varRefExp=isSgVarRefExp(actualParams[1]);
        if(!varRefExp) {
          throw CodeThorn::Exception(string("Error: unsupported print argument #2 (no variable found). Required form of printf(\"...%d...\",v).")+" Source: "+node->unparseToString());
        }
        return varRefExp;
      } else {
        throw CodeThorn::Exception("Error: unsupported number of printf arguments. Required form of printf(\"...%d...\",v).");
      }
    }
  }    
  return 0;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgVarRefExp* SgNodeHelper::Pattern::matchSingleVarFPrintf(SgNode* node, bool showWarnings) {
  SgNode* nextNodeToAnalyze1=node;
  if(SgFunctionCallExp* funCall=SgNodeHelper::Pattern::matchFunctionCall(nextNodeToAnalyze1) ) {
    string fName=SgNodeHelper::getFunctionName(funCall);
    SgExpressionPtrList& actualParams=SgNodeHelper::getFunctionCallActualParameterList(funCall);
    if(fName=="fprintf") {
      if(actualParams.size()==3) {
        SgVarRefExp* varRefExp=isSgVarRefExp(actualParams[2]);
        if(!varRefExp) {
          if(showWarnings) {
            cerr<<"WARNING: unsupported fprint argument #3 (no variable found). Required form of fprintf(stream,\"...%d...\",v)."<<endl;
          }
          return 0;
        }
        return varRefExp;
      } else {
        if(showWarnings) {
          cerr<<"WARNING: unsupported number of fprintf arguments. Required form of fprintf(stream,\"...%d...\",v)."<<endl;
        }
        return 0;
      }
    }
  }
  return 0;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgExpression* SgNodeHelper::getInitializerExpressionOfVariableDeclaration(SgVariableDeclaration* decl) {
  SgInitializedName* initName=SgNodeHelper::getInitializedNameOfVariableDeclaration(decl);
  SgInitializer* initializer=initName->get_initializer();
  // check if it is a declaration without initializer
  if(!initializer)
    return 0;
  if(SgAssignInitializer* assignInitializer=isSgAssignInitializer(initializer)) {
    SgExpression* expr=assignInitializer->get_operand_i();
    return expr;
  } else {
    return 0;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
  * \brief Returns the SgInitializedName of a variable declaration or throws an exception
 */
SgInitializedName* SgNodeHelper::getInitializedNameOfVariableDeclaration(SgVariableDeclaration* decl) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  if(initName0) {
    if(SgInitializedName* initName=isSgInitializedName(initName0)) {
      return initName;
    } else {
      throw CodeThorn::Exception("Error: AST structure failure: no variable found (@initializedName).");
    }
  } else {
    throw CodeThorn::Exception("Error: AST structure failure: no variable found.");
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string SgNodeHelper::symbolToString(SgSymbol* symbol) {
  SgName nameObject=symbol->get_name();
  string nameString=nameObject.getString();
  return nameString;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
list<SgGlobal*> SgNodeHelper::listOfSgGlobal(SgProject* project) {
  list<SgGlobal*> globalList;
  int numFiles=project->numberOfFiles();
  for(int i=0;i<numFiles;++i) {
    SgFile* file=(*project)[i];
    if(SgSourceFile* sourceFile=isSgSourceFile(file)) {
      SgGlobal* global=sourceFile->get_globalScope();
      globalList.push_back(global);
    } else {
      throw CodeThorn::Exception("Error: Ast structure failure: file is not a source file.");
    }
  }
  return globalList;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
list<SgVariableDeclaration*> SgNodeHelper::listOfGlobalVars(SgProject* project) {
  list<SgVariableDeclaration*> globalVarDeclList;
  list<SgGlobal*> globalList=SgNodeHelper::listOfSgGlobal(project);
  for(list<SgGlobal*>::iterator i=globalList.begin();i!=globalList.end();++i) {
    list<SgVariableDeclaration*> varDeclList=SgNodeHelper::listOfGlobalVars(*i);
    globalVarDeclList.splice(globalVarDeclList.end(),varDeclList); // we are *moving* objects (not copying)
  }
  return globalVarDeclList;
}

#if __cplusplus > 199711L
list<SgVariableDeclaration*> SgNodeHelper::listOfGlobalFields(SgProject* project) {
  list<SgVariableDeclaration*> globalFieldDeclList;
  list<SgGlobal*> globalList=SgNodeHelper::listOfSgGlobal(project);
  for(list<SgGlobal*>::iterator i=globalList.begin();i!=globalList.end();++i) {
    list<SgVariableDeclaration*> varDeclList=SgNodeHelper::listOfGlobalFields(*i);
    globalFieldDeclList.splice(globalFieldDeclList.end(),varDeclList); // we are *moving* objects (not copying)
  }
  return globalFieldDeclList;
}
#endif

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
list<SgFunctionDefinition*> SgNodeHelper::listOfFunctionDefinitions(SgNode* node) {
  list<SgFunctionDefinition*> funDefList;
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgFunctionDefinition* funDef=isSgFunctionDefinition(*i)) {
      funDefList.push_back(funDef);
    }
  }
  return funDefList;
}


/*! 
  * \author Tristan Vanderbruggen
  * \date 2019.
 */
#if __cplusplus > 199711L
list<SgFunctionDeclaration*> SgNodeHelper::listOfFunctionDeclarations(SgNode* node) {
  list<SgFunctionDeclaration*> funDeclList;

  if (node == nullptr) {
    node = SageInterface::getProject();
  }
  assert(node != nullptr);

  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    SgFunctionDeclaration * funDecl = isSgFunctionDeclaration(*i);
    if (funDecl == NULL) continue;
    funDeclList.push_back(funDecl);
  }
  return funDeclList;
}
#endif


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
list<SgVarRefExp*> SgNodeHelper::listOfUsedVarsInFunctions(SgProject* project) {
  list<SgVarRefExp*> varRefExpList;
  list<SgFunctionDefinition*> funDefList=SgNodeHelper::listOfFunctionDefinitions(project);
  for(list<SgFunctionDefinition*>::iterator i=funDefList.begin();i!=funDefList.end();++i) {
    RoseAst ast(*i);
    for(RoseAst::iterator j=ast.begin();j!=ast.end();++j) {
      if(SgVarRefExp* varRefExp=isSgVarRefExp(*j)) {
        varRefExpList.push_back(varRefExp);
      }
    }
  }
  return varRefExpList;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
list<SgVariableDeclaration*> SgNodeHelper::listOfGlobalVars(SgGlobal* global) {
  list<SgVariableDeclaration*> varDeclList;
  SgDeclarationStatementPtrList& declStmtList=global->get_declarations();
  for(SgDeclarationStatementPtrList::iterator i=declStmtList.begin();i!=declStmtList.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      if(!varDecl->isForward()) {
        varDeclList.push_back(varDecl);
      }
    }
  }
  return varDeclList;
}

#if __cplusplus > 199711L
list<SgVariableDeclaration*> SgNodeHelper::listOfGlobalFields(SgGlobal* global) {
  list<SgVariableDeclaration*> fields;
  auto s_decls = global->get_declarations();
  for(auto s : s_decls) {
    SgClassDeclaration * xdecl = isSgClassDeclaration(s);
    SgClassDefinition * xdefn = xdecl ? xdecl->get_definition() : nullptr;
    if (xdefn != nullptr) {
      auto v_decls = xdefn->getDeclarationList();
      for (auto v : v_decls) {
        SgVariableDeclaration * vdecl = isSgVariableDeclaration(v);
        if (vdecl != nullptr) {
          fields.push_back(vdecl);
        }
      }
    }
  }
  return fields;
}
#endif

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgSymbol*
SgNodeHelper::getSymbolOfVariableDeclaration(SgVariableDeclaration* decl) {  
  SgInitializedName* initName=SgNodeHelper::getInitializedNameOfVariableDeclaration(decl);
  if(initName) {
    SgSymbol* initDeclVar=initName->search_for_symbol_from_symbol_table();
    return initDeclVar;
  } else {
    return 0;
  }
}

SgFunctionSymbol* SgNodeHelper::getSymbolOfFunctionDeclaration(SgFunctionDeclaration* decl) {
  SgSymbol* symbol = decl->search_for_symbol_from_symbol_table();
  ROSE_ASSERT(symbol);
  ROSE_ASSERT(isSgFunctionSymbol(symbol));
  return isSgFunctionSymbol(symbol);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
  * \brief Returns a unique UniqueVariableSymbol (SgSymbol*) for a variale in a variable declaration (can be used as ID)

 */
SgSymbol*
SgNodeHelper::getSymbolOfVariable(SgVarRefExp* varRefExp) {
  SgVariableSymbol* varSym=varRefExp->get_symbol();
  if(varSym) {
    // schroder3 (2016-08-23): Check for incorrect AST structure which is an indicator that the wrong
    //  symbol is referenced. This is known to happen inside catch blocks if the SgVarRefExp refers to
    //  the caught variable.
    SgNode* initName = varSym->get_declaration();
    if(initName) {
      SgNode* initNameParent = initName->get_parent();
      if(initNameParent->get_childIndex(initName) > initNameParent->get_numberOfTraversalSuccessors()) {
        // Incorrect AST structure: Node is not a child of its parent node. This is currently only known to happen
        //  inside a catch block:
        SgCatchOptionStmt* catchStmt = isSgCatchOptionStmt(initNameParent->get_parent());
        ROSE_ASSERT(catchStmt);
        // Get the correct symbol:
        SgVariableDeclaration* decl = catchStmt->get_condition();
        ROSE_ASSERT(decl);
        SgVariableSymbol* correctVarSym = isSgVariableSymbol(SgNodeHelper::getSymbolOfVariableDeclaration(decl));
        ROSE_ASSERT(correctVarSym);
        ROSE_ASSERT(symbolToString(correctVarSym) == symbolToString(varSym));
        varSym = correctVarSym;
      }
    }
  }
  if(!varSym) {
    throw CodeThorn::Exception("SgNodeHelper::getSymbolofVariable: SgVariableSymbol of varRefExp==0");
#if 0
    SgInitializedName* varInitName=varSym->get_declaration(); // schroder3: varSym is 0 here!
    if(varInitName==0) {
      //cout << "DEBUG: *only* varSym available."<<endl;
      return varSym;
    }
    SgSymbol* symbol=getSymbolOfInitializedName(varInitName);
    if(symbol==0) {
      // MS: Fall back solution: try to find a symbol using the declaration 
      //     (that's sometimes necessary when coming from a SgVariableSymbol)
      SgDeclarationStatement* varInitNameDecl=varInitName->get_declaration();
      if(SgVariableDeclaration* decl=isSgVariableDeclaration(varInitNameDecl)) {
        return SgNodeHelper::getSymbolOfVariableDeclaration(decl);
      } else {
        // we give up
        cerr << "WARNING: getSymbolOfVariable: no symbol found."<<endl;
        return 0;
      }
    } else {
      return symbol;
    }
#endif
  }
  return varSym;
}

// Returns a unique symbol for a function (can be used as ID)
SgFunctionSymbol* SgNodeHelper::getSymbolOfFunction(SgFunctionRefExp* funcRefExp) {
  SgFunctionSymbol* funcSymbol = funcRefExp->get_symbol();
  ROSE_ASSERT(funcSymbol);
  return funcSymbol;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgSymbol*
SgNodeHelper::getSymbolOfInitializedName(SgInitializedName* initName) {
  //if(initName->get_prev_decl_item()==0 && initName->get_symbol_from_symbol_table()==0) {
    //cerr<<"WARNING: SgInitializedName: symbol-look-up would fail: get_name()=="<<initName->get_name()<< " .. skipping."<<endl;
    //return 0;
    //}
  // schroder3 (2016-07-22): If the init name is in a constructor initializer list then is does not have a valid symbol.
  if(SgCtorInitializerList* ctorInitializerList = isSgCtorInitializerList(initName->get_parent())) {
    ROSE_ASSERT(initName->search_for_symbol_from_symbol_table() == 0);
    //  Find the declaration of the corresponding member and use the declaration's init name instead:
    SgMemberFunctionDeclaration* parentConstructorDecl = isSgMemberFunctionDeclaration(ctorInitializerList->get_parent());
    ROSE_ASSERT(parentConstructorDecl);
    SgClassDefinition* correspondingClass = isSgClassDefinition(parentConstructorDecl->get_class_scope());
    ROSE_ASSERT(correspondingClass);
    // Is init name a member initializer or a call of (base) class constructor?
    if(isSgConstructorInitializer(initName->get_initializer())) {
      // Init name "is" a call of a (base) class constructor. Finding the declaration of this constructor
      //  is currently not supported (TODO).
    }
    else {
      // Find the corresponding member declaration:
      SgDeclarationStatementPtrList& members = correspondingClass->get_members();
      bool declFound = false;
      for(SgDeclarationStatementPtrList::const_iterator i = members.begin(); i != members.end(); ++i) {
        if(SgVariableDeclaration* varDecl = isSgVariableDeclaration(*i)) {
          SgInitializedName* currMemberInitName = getInitializedNameOfVariableDeclaration(varDecl);
          ROSE_ASSERT(currMemberInitName);
          if(currMemberInitName->get_name() == initName->get_name()) {
            // Found the corresponding member declaration. Use the member declaration's init name to get
            //  the symbol:
            initName = currMemberInitName;
            declFound = true;
            break;
          }
        }
      }
      if(!declFound) {
        throw CodeThorn::Exception("Error: Unable to find declaration of member \"" + initName->get_name().getString()
                                + "\" that is referenced in constructor initializer list.");
      }
    }
  }
  // Return the symbol of the init name:
  SgSymbol* varsym=initName->search_for_symbol_from_symbol_table();
  return varsym;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
list<SgClassDeclaration*> SgNodeHelper::classDeclarationNestingSequence(SgDeclarationStatement* node) {
  list<SgClassDeclaration*> cdlist;
  SgNode* node2=node;
  while(node2) {
    if(SgClassDeclaration* cd=isSgClassDeclaration(node2))
      cdlist.push_front(cd);
    node2=node2->get_parent();
  }
  return cdlist;
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
// MS: TODO: this implementation is complicated and needs to be structured better
string SgNodeHelper::uniqueLongVariableName(SgNode* node) {
  if(!(isSgVarRefExp(node)||isSgVariableDeclaration(node)||isSgVariableSymbol(node))) {
    string s="Error: uniqueVariableName: unsupported node type: "+node->class_name();
    throw CodeThorn::Exception(s);
  }
  SgSymbol* sym=0;
  bool found=false;
  string name="?";
  string filename="?";
  string classnestingname="?";
  string scopesequencenumber="?";

  if(SgVariableSymbol* varsym=SgNodeHelper::isFunctionParameterVariableSymbol(node)) {
    name=SgNodeHelper::symbolToString(varsym);
    found=true;
    sym=varsym;
  } else {
    if(SgVariableSymbol* varsym=isSgVariableSymbol(node)) {
      SgInitializedName* initname=varsym->get_declaration();
      // schroder3 (2016-08-23): Commented out assertion because lambda closure variables do
      //  not have a SgInitializedName node.
      // ROSE_ASSERT(initname);
      if(initname) {
#if 1
        node=initname->get_declaration();
        ROSE_ASSERT(node);
#else
        // this way we would be using variable definitions
        SgNode* node1=initname->get_definition();
        if(!node1)
          node=initname->get_declaration();
        else
          node=node1;
#endif
      }
    }
    if(SgVarRefExp* varRef=isSgVarRefExp(node)) {
      SgVariableSymbol* varsym=isSgVariableSymbol(SgNodeHelper::getSymbolOfVariable(varRef));
      ROSE_ASSERT(varsym);
      node=findVariableDeclarationWithVariableSymbol(varsym);
      ROSE_ASSERT(node);
    }
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
      filename=SgNodeHelper::sourceFilenameToString(varDecl);
      sym=isSgVariableSymbol(SgNodeHelper::getSymbolOfVariableDeclaration(varDecl));
      ROSE_ASSERT(sym);
      found=true;
      name=SgNodeHelper::symbolToString(sym);
      
      // class nesting name
      classnestingname="[";
      list<SgClassDeclaration*> typenestingsequence=SgNodeHelper::classDeclarationNestingSequence(varDecl);
      for(list<SgClassDeclaration*>::iterator i=typenestingsequence.begin();i!=typenestingsequence.end();++i) {
        SgClassDeclaration* decl=*i;
        if(i!=typenestingsequence.begin())
          classnestingname+="::";
        classnestingname+=decl->get_name();
      }
      classnestingname+="]";
      // scope sequence number
      stringstream ss;
      ss << SgNodeHelper::scopeSequenceNumber(varDecl);
      scopesequencenumber=ss.str();
    } else {
      name=std::string("??")+node->sage_class_name()+"??";
      return name;
    }
  } // end of FunctionParameter-check
  if(found) {
    if(sym==0) {
      throw CodeThorn::Exception("SgNodeHelper::uniqueVariableName: sym==0.");
    }

    // NOTE: in case of a function parameter varDecl is represented by the function declaration

    // we search from the SgSymbol (which is somewhere found in the AST). Even if it is in the symbol table
    // we will still find the right function!
    SgFunctionDefinition* funDef=SgNodeHelper::correspondingSgFunctionDefinition(sym);
    string funName; // intentionally empty when we are in global scope and no funDef is found.
    if(funDef)
      funName=SgNodeHelper::getFunctionName(funDef);
    stringstream ss;
    ss << SgNodeHelper::scopeNestingLevel(sym);
    string scopeLevel=ss.str();
    //name=SageInterface::generateUniqueName(sym,true);
    string longName=string("$")+filename+string("$")+funName+"$"+scopeLevel+"/"+scopesequencenumber+"$"+classnestingname+"$"+name;
    return longName;
  } else {
    throw CodeThorn::Exception("SgNodeHelper::uniqueVariableName: improper node operation ("+node->class_name());
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgFunctionDefinition* SgNodeHelper::correspondingSgFunctionDefinition(SgNode* node) {
  if(node==0)
    return 0;
  while(!isSgFunctionDefinition(node)) {
    node=SgNodeHelper::getParent(node);
    if(node==0)
      return 0;
  }
  return isSgFunctionDefinition(node);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
int SgNodeHelper::scopeNestingLevel(SgNode* node) {
  if(node==0)
    return 0;
  while(!isSgBasicBlock(node)) {
    node=SgNodeHelper::getParent(node);
    if(node==0)
      return 0;
  }
  return 1+scopeNestingLevel(SgNodeHelper::getParent(node));
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
int SgNodeHelper::scopeSequenceNumber(SgNode* node) {
  /* this is quite an expensive function (searches the AST upwards and within one container). However, the purpose is to provide
     a meaningful (=human readable) scope-sequence number. Alternatively it could also be provided for each scope as precomputed value
     attached as ast-attribute.
     1) find scope we are in
     2) determine scope number from parent container
     scopes are numbered 0 .. n
     returns the scope-number of the provided node's scope within the outer scope
     returns -1 if the provided node has no outer scope
  */
  size_t cnum=-1;
  // ad1)
  while(!isSgScopeStatement(node)) {
    node=node->get_parent();
  }
  // check if no outer scope existed
  if(node==0)
    return -1;
  // ad2)
  SgNode* parent=node->get_parent();
  size_t num=parent->get_numberOfTraversalSuccessors();
  for(size_t i=0; i<num;++i) {
    SgNode* child=parent->get_traversalSuccessorByIndex(i);
    if(isSgScopeStatement(child)) {
      ++cnum;
      if(child==node)
        return cnum;
    }
  }
  throw CodeThorn::Exception("SgNodeHelper::scopeSequenceNumber: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isForwardFunctionDeclaration(SgNode* node) {
  if(SgFunctionDeclaration* funDecl=isSgFunctionDeclaration(node)) {
    SgFunctionDefinition* funDef=funDecl->get_definition();
    if(funDef)
      return false;
  }
  return true;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgFunctionDefinition* SgNodeHelper::determineFunctionDefinition(SgFunctionCallExp* funCall) {
  if(SgFunctionDeclaration* funDecl=funCall->getAssociatedFunctionDeclaration()) {
    if(SgDeclarationStatement* defFunDecl=funDecl->get_definingDeclaration()) {
      if(SgFunctionDeclaration* funDecl2=isSgFunctionDeclaration(defFunDecl)) {
        if(SgFunctionDefinition* funDef=funDecl2->get_definition()) {
          return funDef;
        } else {
          return 0;
        }
      }
    }
  }
  return 0;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string SgNodeHelper::getLabelName(SgNode* node) {
  if(!isSgLabelStatement(node))
    throw CodeThorn::Exception("SgNodeHelper::getLabelName: improper node operation.");
  string labelName=node->unparseToString();
  // strip off trailing ":"
  return labelName.substr(0,labelName.size()-1);
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgExpressionPtrList& SgNodeHelper::getFunctionCallActualParameterList(SgNode* node) {
  if(!isSgFunctionCallExp(node))
    throw CodeThorn::Exception("SgNodeHelper::getFunctionCallActualParameterList: improper node operation.");
  return isSgExprListExp(node->get_traversalSuccessorByIndex(1))->get_expressions();
}

// schroder3 (2016-07-27): Returns the callee of the given call expression
SgExpression* SgNodeHelper::getCalleeOfCall(/*const*/ SgFunctionCallExp* call) {
  SgNode* firstChildOfCallExp = call->get_traversalSuccessorByIndex(0);
  if(SgExpression* calleeExpression = isSgExpression(firstChildOfCallExp)) {
    return calleeExpression;
  }
  else {
    ROSE_ASSERT(false);
    return 0;
  }
}

// schroder3 (2016-06-24): Returns the function type of the callee of the given call expression
SgFunctionType* SgNodeHelper::getCalleeFunctionType(/*const*/ SgFunctionCallExp* call) {
  SgExpression* callee = getCalleeOfCall(call);
  // The callee should have a (underlying) function type:
  SgFunctionType* calleeFunctionType = isCallableExpression(callee);
  ROSE_ASSERT(calleeFunctionType);
  return calleeFunctionType;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgInitializedNamePtrList& SgNodeHelper::getFunctionDefinitionFormalParameterList(SgNode* node) {
  SgFunctionDefinition* funDef=isSgFunctionDefinition(node);
  if(!funDef)
    throw CodeThorn::Exception("SgNodeHelper::getFunctionDefinitionFormalParameterList: improper node operation.");
  SgFunctionDeclaration* funDecl=funDef->get_declaration();
  return funDecl->get_args();
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
SgType* SgNodeHelper::getFunctionReturnType(SgNode* node) {
  SgFunctionDefinition* funDef=isSgFunctionDefinition(node);
  if(!funDef)
    throw CodeThorn::Exception("SgNodeHelper::getFunctionReturnType: improper node operation.");
  SgFunctionDeclaration* funDecl=funDef->get_declaration();
  return funDecl->get_orig_return_type();
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
set<SgVariableDeclaration*> SgNodeHelper::localVariableDeclarationsOfFunction(SgFunctionDefinition* funDef) {
  set<SgVariableDeclaration*> localVarDecls;
  RoseAst ast(funDef);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(*i)) {
      localVarDecls.insert(varDecl);
    }
  }
  return localVarDecls;
}

//! schroder3 (2016-07-22): Returns the closest function definition that contains the given node
SgFunctionDefinition* SgNodeHelper::getClosestParentFunctionDefinitionOfLocatedNode(SgLocatedNode* locatedNode) {
  SgNode* node = locatedNode;
  while((node = node->get_parent())) {
    if(SgFunctionDefinition* funcDef = isSgFunctionDefinition(node)) {
      return funcDef;
    }
  }
  return 0;
}

// schroder3 (2016-07-22): Modified version of SageInterface::isPointerType(...) that
//  returns the underlying pointer type.
const SgPointerType* SgNodeHelper::isPointerType(const SgType* t) {
  if(isSgPointerType(t)) {
    return isSgPointerType(t);
  }
  else if(isSgTypedefType(t)) {
    return SgNodeHelper::isPointerType(isSgTypedefType(t)->get_base_type());
  }
  else if(isSgModifierType(t)) {
    return SgNodeHelper::isPointerType(isSgModifierType(t)->get_base_type());
  }
  else {
    return 0;
  }
}

// schroder3 (2016-08-17): Added support for rvalue reference types. See isLvalueReferenceType(...)
//  if only lvalue references are desired.
// schroder3 (2016-07-22): Modified version of SageInterface::isReferenceType(...) that
//  returns the underlying reference type.
//
// Returns the underlying (without typedefs and modifiers) rvalue OR lvalue reference type if the
//  given type is such a reference type. Returns 0 otherwise.
const SgType* SgNodeHelper::isReferenceType(const SgType* t) {
  if(isSgReferenceType(t) /* <- tests for lvalue reference */ || isSgRvalueReferenceType(t)) {
    return t;
  }
  else if(isSgTypedefType(t)) {
    return SgNodeHelper::isReferenceType(isSgTypedefType(t)->get_base_type());
  }
  else if(isSgModifierType(t)) {
    return SgNodeHelper::isReferenceType(isSgModifierType(t)->get_base_type());
  }
  else {
    return 0;
  }
}

// schroder3 (2016-08-22): Modified version of SageInterface::isReferenceType(...) that
//  returns the underlying LVALUE reference type.
const SgReferenceType* SgNodeHelper::isLvalueReferenceType(const SgType* t) {
  if(const SgType* underlyingRvalueOrLvalueRef = isReferenceType(t)) {
    return isSgReferenceType(underlyingRvalueOrLvalueRef); // isSgReferenceType tests for lvalue reference
  }
  else {
    return 0;
  }
}

// schroder3 (2016-08-22): Returns the underlying RVALUE reference type if the given type is a
//  rvalue reference type. Returns 0 otherwise.
const SgRvalueReferenceType* SgNodeHelper::isRvalueReferenceType(const SgType* t) {
  if(const SgType* underlyingRvalueOrLvalueRef = isReferenceType(t)) {
    return isSgRvalueReferenceType(underlyingRvalueOrLvalueRef);
  }
  else {
    return 0;
  }
}

// schroder3 (2016-08-22): Wrapper around SgReferenceType::get_base_type(...) and
//  SgRvalueReferenceType::get_base_type(...) that works for both reference types.
//  (This is a workaround for the missing mutual base class of SgReferenceType and
//  SgRvalueReferenceType.)
SgType* SgNodeHelper::getReferenceBaseType(const SgType* t) {
  if(const SgReferenceType* lvalueReferenceType = isLvalueReferenceType(t)) {
    return lvalueReferenceType->get_base_type();
  }
  else if(const SgRvalueReferenceType* rvalueReferenceType = isRvalueReferenceType(t)) {
    return rvalueReferenceType->get_base_type();
  }
  else {
    throw CodeThorn::Exception("SgNodeHelper::getReferenceBaseType(...): Parameter is not a"
        " lvalue or rvalue reference type.");
  }
}

// schroder3 (2016-07-26): Returns the given type as a SgPointerType if it is a
//  function pointer type. Returns 0 otherwise.
const SgPointerType* SgNodeHelper::isFunctionPointerType(const SgType* type) {
  if(const SgPointerType* pointerType = SgNodeHelper::isPointerType(type)) {
    if(isSgFunctionType(pointerType->get_base_type())) {
      return pointerType;
    }
  }
  return 0;
}

// schroder3 (2016-07-26): Returns the (underlying) function type of the given type if the given
//  type is eligible for function-to-pointer conversion. Returns 0 otherwise.
const SgFunctionType* SgNodeHelper::isTypeEligibleForFunctionToPointerConversion(const SgType* type) {
  // Only lvalues of function (reference) type are eligible for function-to-pointer
  //  conversion.
  if(const SgType* refType = SgNodeHelper::isReferenceType(type)) {
    type = getReferenceBaseType(refType);
  }

  return isSgFunctionType(type);
}

// schroder3 (2016-07-27): Returns the underlying function type of the given expression if it
//  is callable. Returns 0 otherwise.
//
//  A special handling of expressions of class type where the corresponding class has a call
//   operator ("operator()(...)") is not necessary because ROSE normalizes calls like "A a; a(1);"
//   to "A a; a.operator()(1);". This also applies to calls of lambdas.
SgFunctionType* SgNodeHelper::isCallableExpression(/*const*/ SgExpression* expr) {
  return isCallableType(expr->get_type());
}

// schroder3 (2016-07-27): Returns the underlying function type if the given type
//  is callable i.e. a expression of this type could be called. Returns 0 otherwise.
SgFunctionType* SgNodeHelper::isCallableType(/*const*/ SgType* type) {
  // It is possible to call a reference to a pointer to a function:
  if(const SgType* referenceType = isReferenceType(type)) {
    type = getReferenceBaseType(referenceType);
  }
  if(const SgPointerType* pointerType = isPointerType(type)) {
    type = pointerType->get_base_type();
  }
  return isSgFunctionType(type);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgFunctionCallExp* SgNodeHelper::Pattern::matchFunctionCall(SgNode* node) {
  if(SgFunctionCallExp* fce=SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(node))
    return fce;
  if(SgFunctionCallExp* fce=SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(node))
    return fce;
  if(SgFunctionCallExp* fce=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(node))
    return fce;
  if(SgFunctionCallExp* fce=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(node))
    return fce;
  if(SgFunctionCallExp* fce=SgNodeHelper::Pattern::matchFunctionCallExpInVariableDeclaration(node))
    return fce;
  return 0;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgFunctionCallExp* SgNodeHelper::Pattern::matchExprStmtFunctionCallExp(SgNode* node) {
  if(SgNode* sexp=isSgExprStatement(node))
    if(SgFunctionCallExp* fcp=isSgFunctionCallExp(SgNodeHelper::getExprStmtChild(sexp)))
      return fcp;
  return 0;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgFunctionCallExp* SgNodeHelper::Pattern::matchReturnStmtFunctionCallExp(SgNode* node) {
  if(SgNode* rexp=isSgReturnStmt(node))
    if(SgFunctionCallExp* fcp=isSgFunctionCallExp(SgNodeHelper::getFirstChild(rexp)))
      return fcp;
  return 0;
}


/*! 
  * \author Markus Schordan
  * \date 2015.
 */
SgFunctionCallExp* SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp(SgNode* node) {
  std::pair<SgVarRefExp*,SgFunctionCallExp*>  p=SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(node);
  return p.second;
}

/*! 
  * \author Markus Schordan
  * \date 2015.
 */
std::pair<SgVarRefExp*,SgFunctionCallExp*> SgNodeHelper::Pattern::matchExprStmtAssignOpVarRefExpFunctionCallExp2(SgNode* node) {
  if(SgNode* sexp=isSgExprStatement(node)) {
    if(SgNode* assignOp=isSgAssignOp(SgNodeHelper::getExprStmtChild(sexp))) {
      SgNode* lhs=SgNodeHelper::getLhs(assignOp);
      SgNode* rhs=SgNodeHelper::getRhs(assignOp);
      if(SgVarRefExp* var=isSgVarRefExp(lhs)) {
        /* the result of a function call may be casted. skip those
           casts to find the actual function call node.
        */
        while(isSgCastExp(rhs)) {
          rhs=SgNodeHelper::getFirstChild(rhs);
        }
        if(SgFunctionCallExp* fcp=isSgFunctionCallExp(rhs)) {
          return std::make_pair(var,fcp);
        }
      }
    }
  }
  return std::make_pair((SgVarRefExp*)0,(SgFunctionCallExp*)0);
}

SgVariableDeclaration* SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall(SgNode* node) {
  std::pair<SgVariableDeclaration*,SgFunctionCallExp*> pair=SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall2(node);
  return pair.first;
}

SgFunctionCallExp* SgNodeHelper::Pattern::matchFunctionCallExpInVariableDeclaration(SgNode* node) {
  std::pair<SgVariableDeclaration*,SgFunctionCallExp*> pair=SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall2(node);
  return pair.second;
}

std::pair<SgVariableDeclaration*,SgFunctionCallExp*> SgNodeHelper::Pattern::matchVariableDeclarationWithFunctionCall2(SgNode* node) {
  if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(node)) {
    SgExpression* initializer=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(varDecl);
    if(SgFunctionCallExp* funCall=isSgFunctionCallExp(initializer)) {
      return std::make_pair(varDecl,funCall);
    }
  }
  return std::make_pair((SgVariableDeclaration*)0,(SgFunctionCallExp*)0);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::Pattern::matchAssertExpr(SgNode* node) {
  if(isSgExprStatement(node)) {
    node=SgNodeHelper::getExprStmtChild(node);
  }
  // TODO: refine this to also check for name, paramters, etc.
  if(isSgConditionalExp(node))
    return true;
  return false;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
set<SgNode*> SgNodeHelper::loopRelevantBreakStmtNodes(SgNode* node) {
  set<SgNode*> breakNodes;
  RoseAst ast(node);
  RoseAst::iterator i=ast.begin();
  ++i; // go to first child
  while(i!=ast.end()) {
    if(isSgBreakStmt(*i))
      breakNodes.insert(*i);
    if(isSgForStatement(*i)||isSgWhileStmt(*i)||isSgDoWhileStmt(*i)||isSgSwitchStatement(*i))
      i.skipChildrenOnForward();
    ++i;
  }
  return breakNodes;
}

/*! 
  * \author Markus Schordan
  * \date 2017.
 */
set<SgContinueStmt*> SgNodeHelper::loopRelevantContinueStmtNodes(SgNode* node) {
  set<SgContinueStmt*> continueNodes;
  RoseAst ast(node);
  RoseAst::iterator i=ast.begin();
  ++i; // go to first child
  while(i!=ast.end()) {
    if(SgContinueStmt* cs=isSgContinueStmt(*i))
      continueNodes.insert(cs);
    if(isSgForStatement(*i)||isSgWhileStmt(*i)||isSgDoWhileStmt(*i)||isSgSwitchStatement(*i))
      i.skipChildrenOnForward();
    ++i;
  }
  return continueNodes;
}

/*! 
  * \author Markus Schordan
  * \date 2018.
 */
set<SgCaseOptionStmt*> SgNodeHelper::switchRelevantCaseStmtNodes(SgNode* node) {
  set<SgCaseOptionStmt*> caseNodes;
  RoseAst ast(node);
  RoseAst::iterator i=ast.begin();
  while(i!=ast.end()) {
    if(SgCaseOptionStmt* caseStmt=isSgCaseOptionStmt(*i))
      caseNodes.insert(caseStmt);
    // exclude nested switch stmts
    if(isSgSwitchStatement(*i))
      i.skipChildrenOnForward();
    ++i;
  }
  return caseNodes;
}

/*! 
  * \author Markus Schordan
  * \date 2018.
 */
SgDefaultOptionStmt* SgNodeHelper::switchRelevantDefaultStmtNode(SgNode* node) {
  RoseAst ast(node);
  RoseAst::iterator i=ast.begin();
  while(i!=ast.end()) {
    if(SgDefaultOptionStmt* defStmt=isSgDefaultOptionStmt(*i))
      return defStmt;
    // exclude nested switch stmts
    if(isSgSwitchStatement(*i))
      i.skipChildrenOnForward();
    ++i;
  }
  return 0; // nullptr
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isAstRoot(SgNode* node) {
  return !node->get_parent();
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getParent(SgNode* node) {
  SgNode* origNode=node;
  node=node->get_parent();
  if(node==0 && !isSgProject(origNode)) {
    throw CodeThorn::Exception("SgNodeHelper::getParent: improper node operation (@"+origNode->class_name()+")");
  }
  return node;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isLoopCond(SgNode* node) {
  SgNode* parent=node->get_parent();
  if(isLoopStmt(parent))
    return SgNodeHelper::getCond(parent)==node && node!=0;
  else
    return false;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isLoopStmt(SgNode* node) {
  return isSgWhileStmt(node)||isSgDoWhileStmt(node)||isSgForStatement(node);
}

/*! 
  * \author Markus Schordan
  * \date 2019.
 */
bool SgNodeHelper::isCondInBranchStmt(SgNode* node) {
  SgNode* parent=node->get_parent();
  if(isSgExprStatement(parent)) {
    parent=parent->get_parent();
  }
  if(isCondStmt(parent))
    return SgNodeHelper::getCond(parent)==node && node!=0;
  else
    return false;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isCond(SgNode* node) {
  SgNode* parent=node->get_parent();
  if(isSgExprStatement(parent)) {
    parent=parent->get_parent();
  }
  if(isCondStmtOrExpr(parent))
    return SgNodeHelper::getCond(parent)==node && node!=0;
  else
    return false;
}

/*! 
  * \author Markus Schordan
  * \date 2016.
 */
bool SgNodeHelper::isCondStmt(SgNode* node) {
  return isSgIfStmt(node)||isSgWhileStmt(node)||isSgDoWhileStmt(node)||isSgForStatement(node)||isSgSwitchStatement(node);
}

/*! 
  * \author Markus Schordan
  * \date 2016.
 */
bool SgNodeHelper::isCondStmtOrExpr(SgNode* node) {
  return isCondStmt(node)||isSgConditionalExp(node);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isPrefixIncDecOp(SgNode* node) {
  if(isSgPlusPlusOp(node)||isSgMinusMinusOp(node))
    return static_cast<SgUnaryOp*>(node)->isLValue()==true;
  else
    return false;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isPostfixIncDecOp(SgNode* node) {
  if(isSgPlusPlusOp(node)||isSgMinusMinusOp(node))
    return static_cast<SgUnaryOp*>(node)->isLValue()==false;
  else
    return false;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgStatementPtrList& SgNodeHelper::getForInitList(SgNode* node) {
  if(SgForStatement* forstmt=isSgForStatement(node)) {
    return forstmt->get_init_stmt();
  }
  // SgForInitStatement
  throw CodeThorn::Exception("SgNodeHelper::getForInitList: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgExpression* SgNodeHelper::getForIncExpr(SgNode* node) {
  if(SgForStatement* forstmt=isSgForStatement(node)) {
    return forstmt->get_increment();
  }
  // SgForInitStatement
  throw CodeThorn::Exception("SgNodeHelper::getForIncExpr: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool SgNodeHelper::isForIncExpr(SgNode* node) {
  if(SgForStatement* sgFor=isSgForStatement(SgNodeHelper::getParent(node))) {
    if(node==sgFor->get_increment()) {
      return true;
    }
  }
  return false;
}



/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getCond(SgNode* node) {
  if(SgConditionalExp*  condexp=isSgConditionalExp(node)) {
    return condexp->get_conditional_exp();
  } else if(SgIfStmt* ifstmt=isSgIfStmt(node)) {
    return ifstmt->get_conditional();
  } else if(SgWhileStmt* whilestmt=isSgWhileStmt(node)) {
    return whilestmt->get_condition();
  } else if(SgDoWhileStmt* dowhilestmt=isSgDoWhileStmt(node)) {
    return dowhilestmt->get_condition();
  } else if(SgForStatement* forstmt=isSgForStatement(node)) {
    return forstmt->get_test();
  } else if(SgSwitchStatement* switchstmt=isSgSwitchStatement(node)) {
    return switchstmt->get_item_selector();
  } else {
    throw CodeThorn::Exception("SgNodeHelper::getCond: improper node operation.");
  }
}

/*! 
  * \author Markus Schordan
  * \date 2016.
 */
void SgNodeHelper::setCond(SgStatement* stmt, SgNode* cond) {
  if(SgStatement* stmtCond=isSgStatement(cond)) {
    if(SgIfStmt* ifstmt=isSgIfStmt(stmt)) {
      return ifstmt->set_conditional(stmtCond);
    } else if(SgWhileStmt* whilestmt=isSgWhileStmt(stmt)) {
      return whilestmt->set_condition(stmtCond);
    } else if(SgDoWhileStmt* dowhilestmt=isSgDoWhileStmt(stmt)) {
      return dowhilestmt->set_condition(stmtCond);
    } else if(SgForStatement* forstmt=isSgForStatement(stmt)) {
      return forstmt->set_test(stmtCond);
    } else if(SgSwitchStatement* switchstmt=isSgSwitchStatement(stmt)) {
      return switchstmt->set_item_selector(stmtCond);
    } else {
      throw CodeThorn::Exception("SgNodeHelper::setCond: improper node operation (unknown branching construct).");
    }
  } else {
    cerr<<"Error: ConditionType: node type: "<<cond->class_name()<<endl;
    throw CodeThorn::Exception("SgNodeHelper::setCond: improper node operation (wrong condition type).");
  }
}

string SgNodeHelper::unparseCond(SgNode* cond) {
  if(SgNodeHelper::isCond(cond)) {
    string condString;
    condString=cond->unparseToString();
    if(condString[condString.size()-1]==';')
      condString.erase(condString.size()-1); // C++11: condString.pop_back()
    return condString;
  } else {
    throw CodeThorn::Exception("SgNodeHelper::unparseCond: improper node operation.");
  }
}



/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getTrueBranch(SgNode* node) {
  if(SgIfStmt* ifstmt=isSgIfStmt(node)) {
    return ifstmt->get_true_body();
  }
  if(SgConditionalExp*  condexp=isSgConditionalExp(node)) {
    return condexp->get_true_exp();
  }
  throw CodeThorn::Exception("SgNodeHelper::getTrueBranch: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getFalseBranch(SgNode* node) {
  if(SgIfStmt* ifstmt=isSgIfStmt(node)) {
    return ifstmt->get_false_body();
  }
  if(SgConditionalExp*  condexp=isSgConditionalExp(node)) {
    return condexp->get_false_exp();
  }
  throw CodeThorn::Exception("SgNodeHelper::getFalseBranch: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getLoopBody(SgNode* node) {
  if(SgWhileStmt* whilestmt=isSgWhileStmt(node)) {
    return whilestmt->get_body();
  }
  if(SgDoWhileStmt* dowhilestmt=isSgDoWhileStmt(node)) {
    return dowhilestmt->get_body();
  }
  if(SgForStatement* forstmt=isSgForStatement(node)) {
    return forstmt->get_loop_body();
  }
  throw CodeThorn::Exception("SgNodeHelper::getLoopBody: improper node operation.");
}



/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getFirstOfBlock(SgNode* node) {
  if(SgBasicBlock* block=isSgBasicBlock(node)) {
    int len=SgNodeHelper::numChildren(block);
    if(len>0)
      return node->get_traversalSuccessorByIndex(0);
  }
  // MS: note, the child could be 0 as well. Therefore we do not return 0, but throw an exception.
  throw CodeThorn::Exception("SgNodeHelper::getFirstBlock: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getLastOfBlock(SgNode* node) {
  if(SgBasicBlock* block=isSgBasicBlock(node)) {
    int len=SgNodeHelper::numChildren(block);
    if(len>0)
      return node->get_traversalSuccessorByIndex(len-1);
  }
  // MS: note, the child could be 0 as well. Therefore we do not return 0, but throw an exception.
  throw CodeThorn::Exception("SgNodeHelper::getLastOfBlock: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2013.
 */
// TODO: refactor with new function SgNodeHelper::replaceString
void replaceInString(string toReplace, string with, string& str) {
  size_t index = 0;
  while (index<str.size()) {
    /* find the substring to replace. */
    index = str.find(toReplace, index);
    if (index == string::npos) break;
    
    /* replace the substring */
    str.replace(index, toReplace.size(), with);
    /* advance index forward past the replaced string */
    index += with.size();
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string SgNodeHelper::doubleQuotedEscapedString(string s1) {
  string s2=s1;
  char s[2]={'"',0};
  replaceInString(string(s),"\\\"",s2);
  return s2;
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
string SgNodeHelper::doubleQuotedEscapedHTMLString(string s1) {
  string s2=SgNodeHelper::doubleQuotedEscapedString(s1);
  // not clear yet how to get these special characters to display in labels inside tables in dot.
  replaceInString("<="," LE ",s2); // le
  replaceInString(">="," GE ",s2); // ge
  replaceInString("<"," LT ",s2); // lt
  replaceInString(">"," GT ",s2); //gt
  replaceInString("&"," ADDR ",s2); //&
  return s2;
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string SgNodeHelper::nodeToString(SgNode* node) {
  if(isSgBasicBlock(node))
    return "{"; // MS: TODO: that's temporary and will be removed.
  if(isSgFunctionDefinition(node)||isSgFunctionDeclaration(node))
    return SgNodeHelper::getFunctionName(node);
  string s=node->unparseToString();
  return doubleQuotedEscapedString(s);
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string SgNodeHelper::getFunctionName(SgNode* node) {
  SgFunctionDeclaration* fundecl=0;
  if(SgFunctionDefinition* fundef=isSgFunctionDefinition(node)) {
    node=fundef->get_declaration();
  }
  if(SgFunctionCallExp* funCall=isSgFunctionCallExp(node)) {
    // MS: conditional update of variable 'node' is intentional for following if
    node=funCall->getAssociatedFunctionDeclaration();
    // in case of function pointers 0 is returned
    if(node==0)
      return "";
  }
  if(SgFunctionDeclaration* tmpfundecl=isSgFunctionDeclaration(node)) {
    fundecl=tmpfundecl;
  }
  if(fundecl) {
    SgName fname=fundecl->get_name();
    return fname.getString();
  }
  throw CodeThorn::Exception("SgNodeHelper::getFunctionName: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getExprStmtChild(SgNode* node) {
  if(!isSgExprStatement(node)) {
    throw CodeThorn::Exception("SgNodeHelper::getExprStmtChild: improper type ("+node->class_name()+")");
  }
  return SgNodeHelper::getFirstChild(node);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getExprRootChild(SgNode* node) {
  if(!isSgExpressionRoot(node)) {
    throw CodeThorn::Exception("SgNodeHelper::getExprRootChild: improper type ("+node->class_name()+")");
  }
  return SgNodeHelper::getFirstChild(node);
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
bool SgNodeHelper::isArrayElementAssignment(SgNode* node) {
  if(isSgCompoundAssignOp(node)||isSgAssignOp(node)) {
    SgNode* lhs=getLhs(node);
    if(isSgPntrArrRefExp(lhs))
      return true;
  }
  return false;
}

/*! 
  * \author Markus Schordan
  * \date 2014.
 */
bool SgNodeHelper::isFloatingPointAssignment(SgNode* node) {
  if(isSgCompoundAssignOp(node)||isSgAssignOp(node)) {
    SgExpression* lhs=isSgExpression(getLhs(node));
    ROSE_ASSERT(lhs);
    SgType* type=lhs->get_type();
    if(isFloatingPointType(type))
      return true;
  }
  return false;
}

bool SgNodeHelper::isFloatingPointType(SgType* type) {
  return isSgTypeFloat(type) || isSgTypeDouble(type) || isSgTypeLongDouble(type);
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getUnaryOpChild(SgNode* node) {
  if(!dynamic_cast<SgUnaryOp*>(node)) {
    throw CodeThorn::Exception("Error: improper type in getUnaryOpChild ("+node->class_name()+")");
  }
  return SgNodeHelper::getFirstChild(node);
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getFirstChild(SgNode* node) {
  int len=SgNodeHelper::numChildren(node);
  if(len>0)
    return node->get_traversalSuccessorByIndex(0);
  else {
    // MS: note, the child could be 0 as well. Therefore we do not return 0, but throw an exception.
    throw CodeThorn::Exception("SgNodeHelper::getFirstChild: improper node operation.");
  }
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getLhs(SgNode* node) {
  if(dynamic_cast<SgBinaryOp*>(node)) 
    return node->get_traversalSuccessorByIndex(0);
  else 
    throw CodeThorn::Exception("SgNodeHelper::getLhs: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
SgNode* SgNodeHelper::getRhs(SgNode* node) {
  if(dynamic_cast<SgBinaryOp*>(node)) 
    return node->get_traversalSuccessorByIndex(1);
  else 
    throw CodeThorn::Exception("SgNodeHelper::getRhs: improper node operation.");
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
int SgNodeHelper::numChildren(SgNode* node) {
  if(node) {
    size_t len=node->get_numberOfTraversalSuccessors();
    if(len<=(size_t)INT_MAX)
      return (int)len;
    else
      throw CodeThorn::Exception("SgNodeHelper::numChildren: number of children beyond max int.");
  } else {
    return 0; // if node==0 we return 0 as number of children
  }
}

void SgNodeHelper::replaceExpression(SgExpression* e1, SgExpression* e2, bool mode) {
  SgExpression* pe=isSgExpression(e1->get_parent());
  SgExprStatement* ps=isSgExprStatement(e1->get_parent());
  if(pe && mode==false) {
    pe->replace_expression(e1,e2);
    e2->set_parent(pe);
  } else if(ps && mode==false) {
    ps->replace_expression(e1,e2);
    e2->set_parent(ps);
  } else {
    SageInterface::replaceExpression(e1,e2,mode); // this function is more general but very slow
  }
}

void SgNodeHelper::replaceAstWithString(SgNode* node, string s) {
  AstUnparseAttribute* substituteNameAttribute=new AstUnparseAttribute(s,AstUnparseAttribute::e_replace);
  node->setAttribute("AstUnparseAttribute",substituteNameAttribute);
}

bool SgNodeHelper::isArrayAccess(SgNode* node) {
  return isSgPntrArrRefExp(node)!=0;
}

bool SgNodeHelper::isPointerVariable(SgVarRefExp* var) {
  if(var==0)
    return false;
  SgType* type=var->get_type();
  return isSgPointerType(type)!=0;
}

bool SgNodeHelper::isArrayDeclaration(SgVariableDeclaration* decl) {
  if (isAggregateDeclaration(decl))
    return true;
  SgType* type = decl->get_variables()[0]->get_type();
  return isSgArrayType(type);
}

bool SgNodeHelper::isAggregateDeclaration(SgVariableDeclaration* decl) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  ROSE_ASSERT(initName0);
  if(SgInitializedName* initName=isSgInitializedName(initName0)) {
    SgInitializer* arrayInitializer=initName->get_initializer();
    return isSgAggregateInitializer(arrayInitializer);
  }
  return false;
}

SgExpressionPtrList& SgNodeHelper::getInitializerListOfAggregateDeclaration(SgVariableDeclaration* decl) {
  SgNode* initName0=decl->get_traversalSuccessorByIndex(1); // get-InitializedName
  ROSE_ASSERT(initName0);
  if(SgInitializedName* initName=isSgInitializedName(initName0)) {
    // array initializer
    SgInitializer* initializer=initName->get_initializer();
    if(SgAggregateInitializer* arrayInit=isSgAggregateInitializer(initializer)) {
      SgExprListExp* rhsOfArrayInit=arrayInit->get_initializers();
      SgExpressionPtrList& exprPtrList=rhsOfArrayInit->get_expressions();
      return exprPtrList;
    }
  }
  throw CodeThorn::Exception("SgNodeHelper::getInitializerListOfAggregateDeclaration: getInitializerListOfArrayVariable failed.");
}

SgNodeHelper::PragmaList
SgNodeHelper::collectPragmaLines(string pragmaName,SgNode* root) {
  PragmaList l;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    std::list<SgPragmaDeclaration*> pragmaNodes;
    ROSE_ASSERT(pragmaNodes.size()==0);
    // SgPragmaDeclaration(SgPragma),..., SgStatement
    // collects consecutive pragma declarations
    while(i!=ast.end()&&(isSgPragmaDeclaration(*i)||isSgPragma(*i))) {
      if(SgPragmaDeclaration* pragmaDecl=isSgPragmaDeclaration(*i)) {
        pragmaNodes.push_back(pragmaDecl);
      }
      ++i;
    }
    if(i!=ast.end()) {
      for(std::list<SgPragmaDeclaration*>::iterator p=pragmaNodes.begin();p!=pragmaNodes.end();++p) {
        string str=SgNodeHelper::getPragmaDeclarationString(*p);
        SgNodeHelper::replaceString(str,"#pragma ","");
        if(SgNodeHelper::isPrefix(pragmaName,str)) {
          SgPragmaDeclaration* lastPragmaDecl=pragmaNodes.back();
          // ensure we did not collect pragmas at the end of a block
          if(!(isLastChildOf(lastPragmaDecl,lastPragmaDecl->get_parent()))) {
            if(SgStatement* assocStmt=isSgStatement(*i)) {
              SgNodeHelper::replaceString(str,pragmaName+" ","");
              //cout<<"PRAGMA REVERSE: "<<str<<" : "<<(assocStmt)->unparseToString()<<endl;
              l.push_back(make_pair(str,assocStmt));
            } else {
              throw CodeThorn::Exception("SgNodeHelper::collectPragmaLines "+SgNodeHelper::sourceLineColumnToString(*p)+": pragma not associated with a method or statement.");
            }
          } else {
              throw CodeThorn::Exception("SgNodeHelper::collectPragmaLines "+SgNodeHelper::sourceLineColumnToString(*p)+": pragma at end of block. This is not allowed.");
          }
        }
      }
    }
  }
  return l;
}

std::string SgNodeHelper::getPragmaDeclarationString(SgPragmaDeclaration* pragmaDecl) {
  SgPragma* pragma=pragmaDecl->get_pragma();
  ROSE_ASSERT(pragma);
  return pragma->get_pragma();
}

void SgNodeHelper::replaceString(std::string& str, const std::string& from, const std::string& to) {
  if(from.empty())
    return;
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

bool SgNodeHelper::isPrefix( const std::string& prefix, const std::string& s )
{
  return std::equal(
                    prefix.begin(),
                    prefix.begin() + prefix.size(),
                    s.begin() 
                    );
}

bool SgNodeHelper::isLastChildOf(SgNode* elem, SgNode* parent) {
  std::vector<SgNode*> children=parent->get_traversalSuccessorContainer();
  return elem==children.back();
}

#if __cplusplus > 199711L
bool SgNodeHelper::hasOmpNoWait(SgOmpClauseBodyStatement *ompNode) {
  for (auto c : ompNode->get_clauses()) {
    if (isSgOmpNowaitClause(c)) {
      return true;
    }
  }
  return false;
}

SgNodeHelper::OmpSectionList SgNodeHelper::getOmpSectionList(SgOmpSectionsStatement *sectionsStmt) {
  auto bb = isSgBasicBlock(sectionsStmt->get_traversalSuccessorByIndex(0));
  OmpSectionList l;
  for (auto stmt : bb->get_statements()) {
    if (auto s = isSgOmpSectionStatement(stmt)) {
      l.push_back(s);
    }
  }
  return l;
}
#endif

#if __cplusplus > 199711L
namespace SgNodeHelper {

template <typename N>
bool node_can_be_changed(N * node);

template <>
bool node_can_be_changed<SgLocatedNode>(SgLocatedNode * lnode) {
  return ! SageInterface::insideSystemHeader(lnode) &&
         ! lnode->isCompilerGenerated();
}

template <>
bool node_can_be_changed<SgLocatedNodeSupport>(SgLocatedNodeSupport * lnode_s) {
  return SgNodeHelper::node_can_be_changed<SgLocatedNode>(lnode_s);
}

template <>
bool node_can_be_changed<SgStatement>(SgStatement * stmt) {
  return SgNodeHelper::node_can_be_changed<SgLocatedNode>(stmt);
}

template <>
bool node_can_be_changed<SgDeclarationStatement>(SgDeclarationStatement * decl) {
  return SgNodeHelper::node_can_be_changed<SgStatement>(decl);
}

template <>
bool node_can_be_changed<SgFunctionDeclaration>(SgFunctionDeclaration * fdecl) {
  std::string fname = fdecl->get_name().getString();
  return fname.find("__builtin_") != 0 && SgNodeHelper::node_can_be_changed<SgDeclarationStatement>(fdecl);
}

template <>
bool node_can_be_changed<SgVariableDeclaration>(SgVariableDeclaration * vdecl) {
  return SgNodeHelper::node_can_be_changed<SgDeclarationStatement>(vdecl);
}

template <>
bool node_can_be_changed<SgScopeStatement>(SgScopeStatement * scope) {
  return SgNodeHelper::node_can_be_changed<SgStatement>(scope);
}

template <>
bool node_can_be_changed<SgFunctionDefinition>(SgFunctionDefinition * fdefn) {
  return SgNodeHelper::node_can_be_changed<SgScopeStatement>(fdefn);
}

template <>
bool node_can_be_changed<SgInitializedName>(SgInitializedName * iname) {
  return SgNodeHelper::node_can_be_changed<SgLocatedNodeSupport>(iname);
}

bool nodeCanBeChanged(SgLocatedNode * lnode) {
  // TODO big switch statement...
  SgFunctionDeclaration * fdecl = isSgFunctionDeclaration(lnode);
  if (fdecl != nullptr) {
    return SgNodeHelper::node_can_be_changed(fdecl);
  } else {
    return SgNodeHelper::node_can_be_changed(lnode);
  }
}

}
#endif
