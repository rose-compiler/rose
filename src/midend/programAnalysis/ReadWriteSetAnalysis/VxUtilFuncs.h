#ifndef VX_UTIL_FUNCS_H
#define VX_UTIL_FUNCS_H

#include <limits>

namespace VxUtilFuncs 
{

/**
 * getFunctionDefinitionFromDeclaration
 *
 * If you have a function declaration, often you want to get the definition from it.  But that's kind of a pain.
 * Hence this function.
 **/
  SgFunctionDefinition* getFunctionDefinitionFromDeclaration(const SgFunctionDeclaration* funcDecl);

/**
 * getClassDefinitionFromDeclaration
 *
 * If you have a class declaration, often you want to get the definition from it.  But that's kind of a pain.
 * Hence this function.
 **/
  SgClassDefinition* getClassDefinitionFromDeclaration(const SgClassDeclaration* funcDecl);


/**
 * isHeaderFile
 *
 * Gets the file_info for the node passed in and tried to see if it's
 * extension ended in .h* or not
 *
 * Note, NOT RELIABLE FOR SgFunctionDeclarations, which might be
 * compiler generated
 **/
    bool isHeaderFile(const std::string& path);
    
/**
 * isImplFile
 *
 * Gets the file_info for the node passed in and tried to see if it's
 * extension ended in .c* or not
 *
 * Note, NOT RELIABLE FOR SgFunctionDeclarations, which might be
 * compiler generated
 **/
    bool isImplFile(const std::string& path);

/**
 * isHeaderFile
 *
 * Gets the file_info for the node passed in and tried to see if it's
 * extension ended in .h* or not
 *
 * Note, NOT RELIABLE FOR SgFunctionDeclarations, which might be
 * compiler generated
 **/
    bool isHeaderFile(SgLocatedNode* inNode);
    
/**
 * isImplFile
 *
 * Gets the file_info for the node passed in and tried to see if it's
 * extension ended in .c* or not
 *
 * Note, NOT RELIABLE FOR SgFunctionDeclarations, which might be
 * compiler generated
 **/
    bool isImplFile(SgLocatedNode* inNode);
/**
 * isFileScope
 *
 * Checks if a function had file-only scope.  (Is C static or C++ anonymous namespace
 **/
  bool isFileScope(const SgDeclarationStatement* sgDeclaration);
    
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
  std::string getExpandedFunctionName(const SgFunctionDeclaration* node);

/** \brief We represent file scope functions by slapping the filename on the front of the function name
 *
 * Normally the internal function name is just the VxUtilFuncs::getExpandedFunctionName.  However, if 
 * a function has file scope (C static or C++ anonymous namespace), such a function may collide with 
 * another file scoped function with the same name in the a different function.,
 * SO, we slap the filename on the front of the name of the function.  Luckily, in a file scope function,
 * we always have the filename, because they can only be called from the function they are defined in.
 **/
  std::string compileInternalFunctionName(const SgFunctionDeclaration* sgFunctionDeclaration, std::string filename);
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
  std::string getTrueFilePath(SgFunctionDeclaration* node);


/**
 *  returnFirstNamespace
 *
 *  Basically, get the first token of a string delimited by
 *  "delimiter"  Normally used for getting the std in std::string, for
 *  example.
 *
 * \param[in] namestr: The string to parse
 * \param[in] delimiter: The string to us as a delimiter
 * \return The first token
 *
 **/
  std::string returnFirstNamespace(std::string namestr, std::string delimiter );
    

/**
 * isInProject
 * 
 * Determines if a given function is from this project, or from someplace else (like the stl).
 * This is handy for filtering out functions we don't need to worry about.  We don't need
 * to unit test external functions, and we can reasonably assume they won't change our project's
 * global or member variables.
 * 
 **/
  bool isInProject(SgFunctionDeclaration* inFuncDecl);
/**
 *  getRelativePath
 *
 *  Given a root and a file path, return the file path relative to the root.
 *
 *  \param[in] rootPathStr: The root of the project
 *  \param[in] filePath: The absolute path to a file we want to make relative.
 **/
  std::string getRelativePath(const std::string& rootPathStr, const std::string& filePathStr);

/**
 *  getNodeRelativePath
 *
 *  Get the path of the file containing the provided node, if the user has provided
 *  an applicationRootDirectory.  If not the absolute path will be returned.
 *
 * \param[in] node: A node from a file
 *
 **/
  std::string getNodeRelativePath(SgNode* node);


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
  std::string getFuncDeclRelativePath(SgFunctionDeclaration* inFuncDecl);



/**
 * extractFromPossibleCast
 *
 * Sometimes the object you're looking for may or may not be inside a cast.  
 * Rather than writing the code to manually check every time, this function will
 * check if obj is a cast, and if it is, return it's operand.
 **/
  SgNode* extractFromPossibleCast(SgNode* obj);

/**
 * extractParentFromPossibleCast
 *
 * Sometimes the you want a node's parent, but if the parent is a cast
 * you need to step up again. 
 * Rather than writing the code to manually check every time, this function will
 * check if the parent is a cast, and if it is, return it's the cast's
 * parent..
 **/
  SgNode* extractParentFromPossibleCast(SgNode* obj);
  

 /** 
  * getUniqueDeclaration
  *
  * We use FunctionDeclarations to talk about functions, but there may be multiple declarations in
  * an analysis.  The "First Non-defining declaration" is always unique however.  So we always want that.
  **/
  SgFunctionDeclaration* getUniqueDeclaration(SgFunctionCallExp* func);
  SgFunctionDeclaration* getUniqueDeclaration(SgFunctionRefExp* func);
  SgFunctionDeclaration* getUniqueDeclaration(SgMemberFunctionRefExp* func);
  SgFunctionDeclaration* getUniqueDeclaration(SgFunctionDefinition* func);
  SgFunctionDeclaration* getUniqueDeclaration(SgTemplateMemberFunctionRefExp* func);

 /**
  * extractRefArgFromCall
  *
  * Call to extract a variable or function reference from an argument list
  **/
  SgNode* extractRefArgFromCall(const SgFunctionCallExp* wdCallExp, size_t argnum);

  SgVarRefExp* extractVarRefFromAnything(SgNode* obj);
 
 /**
  * toString
  * \brief Converts the argument to a string
  **/
 template<typename T>
   std::string toString(T arg) {
   std::stringstream ss;
   ss << arg;
   return ss.str();
 }

 /**
  * extractPriorityValue
  *
  * Searches from SgNode down until it can find an SgIntVal to extract
  * a value from.  The SgIntVal is returned.
  *
  * If one cannot be found NULL is returned
  **/
 SgIntVal* extractPriorityValue(SgNode* rootNode);

 /**
  * searchForFunctionRefAndExtractDecl
  *
  * Searches from SgNode down until it can find an SgFunctionRefExp or SgMemberFunctionRefExp
  * to extract an SgFunctionDeclaration from.  The SgFunctionDeclaration is returned.
  *
  * If one cannot be found NULL is returned
  **/
 SgFunctionDeclaration* searchForFunctionRefAndExtractDecl(SgNode* rootNode);

/**
 * Makes a string to represent why a given function failed to be mockable 
 * param[in] message: A human readable explination of the problem
 * param[in] sgNode: The problem node, used to get the file location
 *                   (if possible)
 * param[in] reason: The problem node as a string from the code.  If
 *   the failure was type based, it's a type name, it can be a function
 *   name, etc.
 **/
    std::string makeNoteString(const std::string& message, SgNode* sgNode, const std::string& reason);



  //Recursively descends though typedefs until a real type is reached
    SgType* getTypedefBaseType(SgType* origTypedef);
  
  /**
   *  This function steps up the AST looking for an ancestor of the particular type.
   *  If this works well it should go into SageInterface
   *
   *  \param[in] sgNode  : The node to start the search from
   *  \param[in] targetT : The variantT of the type of node we're looking for.
   **/
  SgNode* getAncestorOfType(SgNode* sgNode, enum VariantT targetT);
  

/**
 * This function attempts to insert a node into a read or writeset
 * or a message explaining why it can't. This can be quite a fraught 
 * process. 
 *
 * \param[in] funcDef: The function definition the read/write was 
 *                     found in.  Mostly used for scope info
 * \param[in] current: The node given as a read or write.  Not always 
 *                     sensible.
 *
 **/
std::string generateAccessNameStrings(SgInitializedName* coarseName);
std::string generateAccessNameStrings(SgFunctionDeclaration* funcDecl, SgThisExp* thisExp);


}
#endif //VX_UTIL_FUNCS
