#include "VxUtilFuncs.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include "sage3basic.h"
#include "SgNodeHelper.h"
#include "sageInterface.h"

//#include <codethorn/ExprAnalyzer.h>


/**
 * getFunctionDefinitionFromDeclaration
 *
 * If you have a function declaration, often you want to get the definition from it.  But that's kind of a pain.
 * Hence this function.
 **/
SgFunctionDefinition* VxUtilFuncs::getFunctionDefinitionFromDeclaration(const SgFunctionDeclaration* funcDecl) {
  //Get the defining declaration (we don't know if funcDecl is the defining or nonDefining declaration
  SgFunctionDeclaration* firstDecl = isSgFunctionDeclaration(funcDecl->get_firstNondefiningDeclaration()); 
  ROSE_ASSERT(firstDecl != NULL);
  SgFunctionDeclaration* funcDefDecl = isSgFunctionDeclaration(firstDecl->get_definingDeclaration()); 
  if(funcDefDecl == NULL) {
    return NULL;
  }

  //Get the definition from the defining declaration
  SgFunctionDefinition* funcDef = isSgFunctionDefinition(funcDefDecl->get_definition());  
  return funcDef;
}

/**
 * getClassDefinitionFromDeclaration
 *
 * If you have a class declaration, often you want to get the definition from it.  But that's kind of a pain.
 * Hence this function.
 **/
SgClassDefinition* VxUtilFuncs::getClassDefinitionFromDeclaration(const SgClassDeclaration* funcDecl) {
  SgClassDeclaration* funcDefDecl = isSgClassDeclaration(funcDecl->get_definingDeclaration()); 
  if(funcDefDecl == NULL) {
    return NULL;
  }

  //Get the definition from the defining declaration
  SgClassDefinition* funcDef = isSgClassDefinition(funcDefDecl->get_definition());  
  return funcDef;
}

/**
 * isHeaderFile
 *
 * Gets the file_info for the node passed in and tried to see if it's
 * extension ended in .h* or not
 *
 **/
bool VxUtilFuncs::isHeaderFile(const std::string& path) {
    boost::filesystem::path filePath = boost::filesystem::weakly_canonical(path);
    std::string extension = filePath.extension().string();
    
    if(boost::iequals(extension.substr(0, 2), ".h")) {
        return true;
    }
    return false;
}

/**
 * isImplFile
 *
 * Gets the file_info for the node passed in and tried to see if it's
 * extension ended in .c* or not
 *
 **/
bool VxUtilFuncs::isImplFile(const std::string& path) {
    boost::filesystem::path filePath = boost::filesystem::weakly_canonical(path);
    std::string extension = filePath.extension().string();
    
    //case insensitive compare
    if(boost::iequals(extension.substr(0, 2), ".c")) {
        return true;
    }
    return false;
}


/**
 * isHeaderFile
 *
 * Gets the file_info for the node passed in and tried to see if it's
 * extension ended in .h* or not
 *
 **/
bool VxUtilFuncs::isHeaderFile(SgLocatedNode* inNode) {
    boost::filesystem::path filePath = boost::filesystem::weakly_canonical(inNode->get_file_info()->get_filename());
    std::string extension = filePath.extension().string();
    
    if(boost::iequals(extension.substr(0, 2), ".h")) {
        return true;
    }
    return false;
}

/**
 * isImplFile
 *
 * Gets the file_info for the node passed in and tried to see if it's
 * extension ended in .c* or not
 *
 **/
bool VxUtilFuncs::isImplFile(SgLocatedNode* inNode) {
    boost::filesystem::path filePath = boost::filesystem::weakly_canonical(inNode->get_file_info()->get_filename());
    std::string extension = filePath.extension().string();
    
    //case insensitive compare
    if(boost::iequals(extension.substr(0, 2), ".c")) {
        return true;
    }
    return false;
}


bool VxUtilFuncs::isFileScope(const SgDeclarationStatement* sgDeclaration) {
  const SgDeclarationModifier& declMod = sgDeclaration->get_declarationModifier();
  const SgStorageModifier& storageMod = declMod.get_storageModifier();
  if(isSgGlobal(sgDeclaration->get_scope()) && storageMod.isStatic()) {
    return true;
  }

  //I think it's only file scope if the namespace right before global is anonymous
  //So iterate up to Global, and prevScope should be the outter most
  //namespace.  If it is a namespace, check if it's anonymous.
  //Otherwise it's not filescope
  SgStatement* scope = sgDeclaration->get_scope();
  SgStatement* prevScope = NULL;
  while(scope && !isSgGlobal(scope)) {
      prevScope = scope;
      scope = scope->get_scope();
  }
  
  if(prevScope) {
      SgNamespaceDefinitionStatement* nspaceDef = isSgNamespaceDefinitionStatement(prevScope);
      if(nspaceDef) {
          SgNamespaceDeclarationStatement* nspaceDecl = nspaceDef->get_namespaceDeclaration();
          if(nspaceDecl) {
              if(nspaceDecl->get_isUnnamedNamespace()) {
                  return true;
              }
          }
      }
  }
  
  return false;
}

/**
 *  getExpandedFunctionName
 *
 *  Returns a fully detailed function name with all argument types 
 *  so this name can be used to differentiate overloaded functions.
 *  This is the equivlent of a mangled name, but more human readable
 *
 * \param[in] funcDecl: A function declaration to full name of 
 *
 **/
std::string VxUtilFuncs::getExpandedFunctionName(const SgFunctionDeclaration* fdecl) 
{
    // This object is used to unparse type properly
    SgUnparse_Info * uinfo = new SgUnparse_Info();
    uinfo->set_SkipComments();
    uinfo->set_SkipWhitespaces();
    uinfo->set_SkipEnumDefinition();
    uinfo->set_SkipClassDefinition();
    uinfo->set_SkipFunctionDefinition();
    uinfo->set_SkipBasicBlock();
    uinfo->set_isTypeFirstPart();

    //    Return type not necessary for disambiguation
    //    SgType * type = fdecl->get_type()->get_return_type();
    std::ostringstream oss;
 
    oss << fdecl->get_qualified_name().getString() << "(";
    bool first = true;
    for (auto t : fdecl->get_type()->get_argument_list()->get_arguments()) {
        
        if(first) {
            first = false;
        } else {
            oss << ",";
        }
        oss << globalUnparseToString(t, uinfo);
    }
    oss << ")";
    return oss.str();

}

    
/** \brief We represent file scope functions by slapping the filename on the front of the function name
 *
 * Normally the internal function name is just the VxUtilFuncs::getExpandedFunctionName.  However, if 
 * a function has file scope (C static or C++ anonymous namespace), such a function may collide with 
 * another file scoped function with the same name in the a different function.,
 * SO, we slap the filename on the front of the name of the function.  Luckily, in a file scope function,
 * we always have the filename, because they can only be called from the function they are defined in.
 **/
std::string VxUtilFuncs::compileInternalFunctionName(const SgFunctionDeclaration* sgFunctionDeclaration, std::string filename) {
  if(VxUtilFuncs::isFileScope(sgFunctionDeclaration)) {
    return filename + " " + VxUtilFuncs::getExpandedFunctionName(sgFunctionDeclaration);
  }
  return VxUtilFuncs::getExpandedFunctionName(sgFunctionDeclaration);
}


/**
 *  getTrueFilePath
 *
 *  Get the path of the file containing the provided Declaration
 *  This is a special case to deal with when a function is defined as a 
 *  template.  Instantiated templates are always compilerGenerated, so have
 *  no file.  So we have to get the associated template and get its file.
 *
 * \param[in] funcDecl: A function declaration to get the filename of
 *
 **/
std::string VxUtilFuncs::getTrueFilePath(SgFunctionDeclaration* inFuncDecl) {
  //We need to figure out where the function originates, but that's different if it's a template so handle both cases
  std::string filepathStr;
  SgTemplateInstantiationFunctionDecl* tempIDecl = isSgTemplateInstantiationFunctionDecl(inFuncDecl);
  SgTemplateInstantiationMemberFunctionDecl* tempIMDecl = isSgTemplateInstantiationMemberFunctionDecl(inFuncDecl);
  if(tempIDecl) {  //is template
    SgTemplateFunctionDeclaration* tempDecl = isSgTemplateFunctionDeclaration(tempIDecl->get_templateDeclaration());
    SgFunctionDeclaration* defFuncDecl = isSgTemplateFunctionDeclaration(tempDecl->get_definingDeclaration());
    filepathStr = defFuncDecl->get_file_info()->get_filename();
  } else if(tempIMDecl) { //is template member
    SgTemplateMemberFunctionDeclaration* tempDecl = isSgTemplateMemberFunctionDeclaration(tempIMDecl->get_templateDeclaration());
    SgFunctionDeclaration* defFuncDecl = isSgTemplateMemberFunctionDeclaration(tempDecl->get_definingDeclaration());
    //Not sure under what circumstances this is null, compiler
    //generated builtin template functions? I got it for _M_construct < const char * > 
    if(defFuncDecl) {
        filepathStr = defFuncDecl->get_file_info()->get_filename();
    } else {
        Sawyer::Message::mlog[Sawyer::Message::Common::WARN] << "Cannot identify defining file for " << inFuncDecl->get_name().getString() << std::endl;       
        filepathStr = tempDecl->get_file_info()->get_filename();    
    }
    
  } else { //is NOT template
    filepathStr = inFuncDecl->get_definingDeclaration()->get_file_info()->get_filename();
  }
  boost::filesystem::path filePath = boost::filesystem::weakly_canonical(filepathStr);  
  return filePath.native();

}

std::string VxUtilFuncs::returnFirstNamespace(std::string namestr, std::string delimiter = "::") 
{
    size_t pos = 0;
    size_t prevpos = 0;
    std::string token;
    while ((pos = namestr.find(delimiter, prevpos)) != std::string::npos) {
        token = namestr.substr(prevpos, pos-prevpos);
        if(token != "") {
            return token;
        }
        prevpos = pos + delimiter.size();
    }
    return "";
}


/**
 * isInProject
 * 
 * Determines if a given function is from this project, or from someplace else (like the stl).
 * This is handy for filtering out functions we don't need to worry about.  We don't need
 * to unit test external functions, and we can reasonably assume they won't change our project's
 * global or member variables.
 * 
 **/
bool VxUtilFuncs::isInProject(SgFunctionDeclaration* inFuncDecl) {
    std::string funcName = inFuncDecl->get_name().getString();
    std::string firstNamespace = returnFirstNamespace(funcName, "::");
    //Special case, std and boost use a lot of header-defined
    //functions, and are never in the project
    if(firstNamespace == "std" || firstNamespace == "boost") {
        return false;
    }
    
    

  SgProject* projectRoot = SageInterface::getProject(inFuncDecl);
  std::string rootDirStr(projectRoot->get_applicationRootDirectory());
  if(rootDirStr == "") {
    return true;  //User didn't specify a root, so nothing can be filtered
  }
  //If the function isn't defined at all, assume it's not in our project?
  if(inFuncDecl->get_definingDeclaration()  == NULL) {
    return false;  
  }
  std::string trueFilePath = getTrueFilePath(inFuncDecl);
  if(trueFilePath == "compilerGenerated") {
      //Apparently a builtin templated function.
      return false;
  }
  
  boost::filesystem::path filePath = boost::filesystem::weakly_canonical(trueFilePath);  
  boost::filesystem::path rootDir  = boost::filesystem::weakly_canonical(rootDirStr);

  if(filePath.native().substr(0, rootDir.native().size()) == rootDir) {
    return true;
  }
  return false;

}

/**
 *  Turn absolute path into relative path
 **/
std::string VxUtilFuncs::getRelativePath(const std::string& rootPathStr, const std::string& filePathStr) {
  boost::filesystem::path rootPath(rootPathStr);
  boost::filesystem::path filePath(filePathStr);
  boost::filesystem::path relativePath = boost::filesystem::relative(filePath, rootPath);
  return relativePath.native();
}

/**
 *  getNodeRelativePath
 *
 *  Get the path of the file containing the provided node, if the user has provided
 *  an applicationRootDirectory.  If not the absolute path will be returned.
 *
 * \param[in] node: A node from a file
 *
 **/
std::string VxUtilFuncs::getNodeRelativePath(SgNode* node) {
  SgProject* projectRoot = SageInterface::getProject(node);
  std::string rootDirStr(projectRoot->get_applicationRootDirectory());
  
  boost::filesystem::path filePath(SageInterface::getEnclosingSourceFile(node)->get_file_info()->get_filenameString());
  filePath = boost::filesystem::weakly_canonical(filePath);
  
  //If the user didn't define an application root, just return the absolute path
  if(rootDirStr == "") {
    return filePath.native();
  }
  return getRelativePath(rootDirStr, filePath.native());
}

/**
 *  getFuncDeclRelativePath
 *
 *  Get the path of the file containing the provided Declaration, if the user 
 *  has provided an applicationRootDirectory.  If not the absolute path will 
 *  be returned.
 *  This is a special case to deal with when a function is defined as a 
 *  template.  Instantiated templates are always compilerGenerated, so have
 *  no file.  So we have to get the associated template and get its file.
 *
 * \param[in] funcDecl: A function declaration to get the filename of
 *
 **/
std::string VxUtilFuncs::getFuncDeclRelativePath(SgFunctionDeclaration* inFuncDecl) {
  SgProject* projectRoot = SageInterface::getProject(inFuncDecl);
  std::string rootDirStr(projectRoot->get_applicationRootDirectory());
  //Force everything canonical before compare
  boost::filesystem::path filePath = boost::filesystem::weakly_canonical(getTrueFilePath(inFuncDecl));  
  if(rootDirStr == "") {
    return filePath.native();  //User didn't specify a root, so no relativity
  }
  boost::filesystem::path rootDir  = boost::filesystem::weakly_canonical(rootDirStr);
  return getRelativePath(rootDir.native(), filePath.native());
}

/**
 * extractFromPossibleCast
 *
 * Sometimes the object you're looking for may or may not be inside a cast.  
 * Rather than writing the code to manually check every time, this function will
 * check if obj is a cast, and if it is, return it's operand.
 **/
 SgNode* VxUtilFuncs::extractFromPossibleCast(SgNode* obj) {
   if(obj->variantT() != V_SgCastExp) {
     return obj;
   } 
   SgCastExp* cast = isSgCastExp(obj);
   return cast->get_operand();
 }

 /** 
  * getUniqueDeclaration
  *
  * We use FunctionDeclarations to talk about functions, but there may be multiple declarations in
  * an analysis.  The "First Non-defining declaration" is always unique however.  So we always want that.
  **/
 SgFunctionDeclaration* VxUtilFuncs::getUniqueDeclaration(SgFunctionCallExp* func) {
   SgFunctionDeclaration* funcDecl = func->getAssociatedFunctionDeclaration(); 
   if(funcDecl) {
     return isSgFunctionDeclaration(funcDecl->get_firstNondefiningDeclaration());
   } else {
     SgUnparse_Info info; 
     Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "Could not resolve call to : " << func->unparseToString(&info) << std::endl;
     Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "      From: " << func->get_file_info()->displayString() << std::endl;  
     return NULL;
   }
 }
 SgFunctionDeclaration* VxUtilFuncs::getUniqueDeclaration(SgFunctionRefExp* func) {
   SgFunctionDeclaration* funcDecl = func->getAssociatedFunctionDeclaration(); 
   if(funcDecl) {
     return isSgFunctionDeclaration(funcDecl->get_firstNondefiningDeclaration());
   } else {
     SgUnparse_Info info; 
     Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "Could not resolve call to : " << func->unparseToString(&info) << std::endl;
     Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "  " << func->get_file_info()->displayString() << std::endl;  
     return NULL;
   }
   //   return isSgFunctionDeclaration(func->getAssociatedFunctionDeclaration()->get_firstNondefiningDeclaration());
 }
 SgFunctionDeclaration* VxUtilFuncs::getUniqueDeclaration(SgMemberFunctionRefExp* func) {
   SgMemberFunctionDeclaration* funcDecl = func->getAssociatedMemberFunctionDeclaration(); 
   if(funcDecl) {
     return isSgFunctionDeclaration(funcDecl->get_firstNondefiningDeclaration());
   } else {
     SgUnparse_Info info; 
     Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "Could not resolve call to : " << func->unparseToString(&info) << std::endl;
     Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "  " << func->get_file_info()->displayString() << std::endl;  
     return NULL;
   }
   //   return isSgFunctionDeclaration(func->getAssociatedFunctionDeclaration()->get_firstNondefiningDeclaration());
 }
 SgFunctionDeclaration* VxUtilFuncs::getUniqueDeclaration(SgFunctionDefinition* func) {
   return isSgFunctionDeclaration(func->get_declaration()->get_firstNondefiningDeclaration());
 }
 SgFunctionDeclaration* VxUtilFuncs::getUniqueDeclaration(SgTemplateMemberFunctionRefExp* func) {
   SgTemplateMemberFunctionDeclaration* funcDecl = func->getAssociatedMemberFunctionDeclaration(); 
   if(funcDecl) {
     return isSgFunctionDeclaration(funcDecl->get_firstNondefiningDeclaration());
   } else {
     SgUnparse_Info info; 
     Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "Could not resolve call to : " << func->unparseToString(&info) << std::endl;
     Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << "  " << func->get_file_info()->displayString() << std::endl;  
     return NULL;
   }
   //   return isSgFunctionDeclaration(func->getAssociatedFunctionDeclaration()->get_firstNondefiningDeclaration());
 }


 /**
  * extractRefArgFromCall
  *
  * Call to extract a variable or function reference from an argument list
  **/
 SgNode* VxUtilFuncs::extractRefArgFromCall(const SgFunctionCallExp* wdCallExp, size_t argnum) {
   SgExprListExp* wdCallArgs = wdCallExp->get_args();
   SgExpressionPtrList & wdCallArgList = wdCallArgs->get_expressions();
   SgNode* retVal = VxUtilFuncs::extractFromPossibleCast(wdCallArgList[argnum]); //Get the reference to the start function 
   return retVal;
 }

 SgVarRefExp* VxUtilFuncs::extractVarRefFromAnything(SgNode* obj) {
     SgVarRefExp* varRef = isSgVarRefExp(obj);
     if(varRef) {
         return varRef;  
     }
     SgCastExp* cast = isSgCastExp(obj);
     if(cast) {
         return extractVarRefFromAnything(cast->get_operand());
     }
     SgDotExp* dot = isSgDotExp(obj);
     if(dot) {
         return extractVarRefFromAnything(dot->get_rhs_operand());
     }
     SgArrowExp* arrow = isSgArrowExp(obj);
     if(arrow) {
         return extractVarRefFromAnything(arrow->get_rhs_operand());
     }
     SgPntrArrRefExp* pntrArr = isSgPntrArrRefExp(obj);  //An array, like SEM_ID foo[3]
     if(pntrArr) {
         return extractVarRefFromAnything(pntrArr->get_lhs_operand());
     }
     SgPointerDerefExp* derefExp = isSgPointerDerefExp(obj);
     if(derefExp) {
         return extractVarRefFromAnything(derefExp->get_operand());       
     }
     Sawyer::Message::mlog[Sawyer::Message::Common::ERROR] << "Could not extract Variable Reference from unhandled type:" << obj->class_name() << std::endl;
     Sawyer::Message::mlog[Sawyer::Message::Common::ERROR] << "  From: " << obj->get_file_info()->displayString() << std::endl;   
     return NULL;
 }

 /**
  * extractPriorityValue
  *
  * Searches from SgNode down until it can find an SgIntVal to extract
  * a value from.  The SgIntVal is returned.
  *
  * If one cannot be found NULL is returned
  **/
  SgIntVal* VxUtilFuncs::extractPriorityValue(SgNode* rootNode)
  {    
      if(rootNode->variantT() == V_SgIntVal) {
          SgIntVal* sgIntVal = isSgIntVal(rootNode);
          ROSE_ASSERT(sgIntVal);
          return sgIntVal;
      }

      std::vector< SgNode * > children = rootNode->get_traversalSuccessorContainer();
     
      BOOST_FOREACH(SgNode *child, children) {
          SgIntVal* childVal = extractPriorityValue(child);
          if(childVal)
              return childVal;
      }

      return NULL;  //Couldn't find an SgIntVal, so null
  }

 /**
  * searchForFunctionRefAndExtractDecl
  *
  * Searches from SgNode down until it can find an SgFunctionRefExp or SgMemberFunctionRefExp
  * to extract an SgFunctionDeclaration from.  The SgFunctionDeclaration is returned.
  *
  * If one cannot be found NULL is returned
  **/
  SgFunctionDeclaration* VxUtilFuncs::searchForFunctionRefAndExtractDecl(SgNode* rootNode)
  { 
      if(rootNode->variantT() == V_SgFunctionRefExp) {
          SgFunctionRefExp* sgFuncRef = isSgFunctionRefExp(rootNode);
          ROSE_ASSERT(sgFuncRef);
          SgFunctionDeclaration* sgFuncDecl = VxUtilFuncs::getUniqueDeclaration(sgFuncRef);
          ROSE_ASSERT(sgFuncDecl);
          return sgFuncDecl;
      }
      if(rootNode->variantT() == V_SgMemberFunctionRefExp) {
          SgMemberFunctionRefExp* sgFuncRef = isSgMemberFunctionRefExp(rootNode);
          ROSE_ASSERT(sgFuncRef);
          SgFunctionDeclaration* sgFuncDecl = VxUtilFuncs::getUniqueDeclaration(sgFuncRef);
          ROSE_ASSERT(sgFuncDecl);
          return sgFuncDecl;
      }


      std::vector< SgNode * > children = rootNode->get_traversalSuccessorContainer();
     
      BOOST_FOREACH(SgNode *child, children) {
          SgFunctionDeclaration* childVal = searchForFunctionRefAndExtractDecl(child);
          if(childVal)
              return childVal;
      }

      return NULL;  //Couldn't find an SgIntVal, so null
  }


/**
 * Makes a string to represent why a given function failed to be mockable 
 * param[in] message: A human readable explination of the problem
 * param[in] sgNode: The problem node, used to get the file location
 *                   (if possible)
 * param[in] reason: The problem node as a string from the code.  If
 *   the failure was type based, it's a type name, it can be a function
 *   name, etc.
 **/
std::string VxUtilFuncs::makeNoteString(const std::string& message, SgNode* sgNode, const std::string& reason) 
{ 
    std::stringstream ss;
    ss << "\"" << message << "\", ";
    ss << "\"" << reason << "\"";
    ss << ", ";
    if(sgNode) {    
        Sg_File_Info* finfo = sgNode->get_file_info(); 
        if(finfo) {
          int linenum = finfo->get_line();//attempt to get good line #
          for(int count = 0; count < 2 && linenum == 0; ++count) {
              sgNode = sgNode->get_parent();
              finfo = sgNode->get_file_info();
              linenum = finfo->get_line();  
          }
          
          ss << " \"" << VxUtilFuncs::getNodeRelativePath(sgNode) << ":" << finfo->get_line() << "\"";
        } else {
          ss << " \"" << sgNode->unparseToString() << "\"";
        }
    }

    return ss.str();
} 

SgType* VxUtilFuncs::getTypedefBaseType(SgType* origTypedef) {
    
    SgType* thisType = origTypedef;
    while(thisType->variantT() == V_SgTypedefType) {
        SgTypedefType* thisTypedef = dynamic_cast<SgTypedefType*>(thisType);
        ROSE_ASSERT(thisTypedef != NULL);
        thisType = thisTypedef->get_base_type();
    }
    return thisType;
}


/**
 *  This function steps up the AST looking for an ancestor of the particular type.
 *  If no ancestor of ancestor of the correct type is found, nullptr
 *  is returned.
 *  If this works well it should go into SageInterface
 *
 *  \param[in] sgNode  : The node to start the search from
 *  \param[in] targetT : The variantT of the type of node we're looking for.
 *  \return Either an SgNode of type targetT, or nullptr
 **/
SgNode* VxUtilFuncs::getAncestorOfType(SgNode* sgNode, enum VariantT targetT) 
{
  ROSE_ASSERT(sgNode);
  
  SgNode* curnode = sgNode;
  if (curnode->variantT() == targetT)
  {
    return curnode;
  }
  do {
    curnode= curnode->get_parent();
  } while( (curnode!=nullptr) && (curnode->variantT() != targetT ));

  //It's either a nullptr or the right type, so return it.
  return curnode;
  
}


std::string VxUtilFuncs::generateAccessNameStrings(SgFunctionDeclaration* funcDecl, SgThisExp* thisExp)
{
    return "@" + VxUtilFuncs::getExpandedFunctionName(funcDecl) + "@" +
        "this";
}


std::string VxUtilFuncs::generateAccessNameStrings(SgInitializedName* coarseName) 
{

  SgStatement* coarseNameScope = coarseName->get_scope();
  SgType* coarseType = coarseName->get_type();
  ROSE_ASSERT(coarseType);
  //Sawyer::Message::mlog[Sawyer::Message::Common::INFO] << coarseNameScope->class_name() << std::endl;

  
  //global case
  if(isSgGlobal(coarseNameScope) || isSgNamespaceDefinitionStatement(coarseNameScope)) {
    //File scope is always within global scope
    if(isFileScope(coarseName->get_declaration())) {
      return VxUtilFuncs::getNodeRelativePath(coarseName) + " " + coarseName->unparseToString();
    }
    // If it's a class type, need to get the full access string
    if(isSgClassType(coarseType)) {
      return "::"+coarseName->unparseToString();
    } else {
      return coarseName->get_qualified_name().getString();
    }
  } else if(isSgClassDefinition(coarseNameScope)) {
    
    SgClassDefinition* clsDef = isSgClassDefinition(coarseNameScope);
    if(clsDef) {
      std::string clsName = clsDef->get_qualified_name();
      return "$" + clsName + "$" + coarseName->unparseToString();
    } else {
      return coarseName->get_qualified_name().getString();
    }
    //Otherwise, check that it's a local variable or an argument
  } else {
    SgNode* funcDefNode = getAncestorOfType(coarseName, V_SgFunctionDefinition);
    SgFunctionDeclaration* funcDecl = nullptr;
    if(funcDefNode != nullptr) {
      SgFunctionDefinition* funcDef = isSgFunctionDefinition(funcDefNode);
      funcDecl = funcDef->get_declaration();
    } else {
      SgNode* paramListNode = getAncestorOfType(coarseName, V_SgFunctionParameterList);
      if(paramListNode) {
        funcDecl = isSgFunctionDeclaration(paramListNode->get_parent());
      }
    }
    if(funcDecl == nullptr) {
      //Can't identify the globality, so don't know what else to do
      //but return and error
      return "!access unknown scope! " + coarseName->get_qualified_name().getString();
    }

    if(isSgClassType(coarseType)) {
      return "@" + VxUtilFuncs::getExpandedFunctionName(funcDecl) + "@" + 
        coarseName->unparseToString();
    } else {
      return "@" + VxUtilFuncs::getExpandedFunctionName(funcDecl) + "@" + 
        coarseName->get_qualified_name().getString();        
    }
  }
    Sawyer::Message::mlog[Sawyer::Message::Common::ERROR] <<
    "generateAccessNameStrings: " <<
    coarseName->get_file_info()->get_filename() << ":" <<
    coarseName->get_file_info()->get_line() << "-" << coarseName->get_file_info()->get_col()<<std::endl;
  Sawyer::Message::mlog[Sawyer::Message::Common::ERROR] <<
    "In generateAccessNameStrings: got to end of function:"  <<
    coarseName->class_name()<<std::endl;
  ROSE_ABORT();
}

