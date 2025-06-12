#ifndef LOCALRWSETGENERATOR_H
#define LOCALRWSETGENERATOR_H

#include <CallGraph.h>

#include <Rose/AST/NodeId.h>
#include <Sawyer/Graph.h>
#include <Sawyer/Message.h>
#include <boost/unordered_map.hpp>
#include <vector>
#include <limits>
#include <unordered_map>
#include <set>
#include <nlohmann/json.hpp>

#include "ReadWriteSetRecords.h"

#define TOOL_VERSION_STRING "0.1.0"


class LocalRWSetGenerator 
{

public:    

  LocalRWSetGenerator() : commandLine("") {};

  /** 
   * Preferred constructor.  So we can save the command line the sets were generated with.
   **/
  LocalRWSetGenerator(std::string& inCommandLine) : commandLine(inCommandLine) {};
  

  /**
   * generateAccessNameStrings
   *
   * \brief Generates the name string for an SgInitializedName
   * 
   *  1. If a variable name starts with '!' an error occurred on determining 
   *     that varialbe and the read/write sets for the function are INVALID.  
   *     For example:
   *     "!ERROR: Call to undefined function: ::vxsimHostPrintf Invalid R/W sets!"
   *     Despite this the rest of the R/W set will be generated as well as possible.
   *  2. Local variable and function arguments get their function name surrounded 
   *     by "@".  Because we combine the sets of called functions into the caller's 
   *     set, we need some way to designate which function those locals belonged to.  
   *     So a local variable references look like this: "::sqr": ["@::sqr@x" ],
   *  3. Member variables are prepended by the class they come from with
   *     '$' symbols, for the same reason as the @ signs.  
   *
   * \param[in] funcDef: The function definition the read/write was 
   *                     found in.  Mostly used for scope info
   * \param[in] coarseName: The SgInitialized name to generate a string for
   *
   **/
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
  std::string generateAccessNameStrings(SgInitializedName* coarseName, ReadWriteSets::Globality globality);
  std::string generateAccessNameStrings(SgFunctionDeclaration* funcDecl, SgThisExp* thisExp);

//  std::string generateAccessNameStrings(SgFunctionDefinition* funcDef, 
//                                        SgInitializedName* coarseName);
    
  
  
  /**
   * This code was mostly taken from sageInterface.C SageInterface::convertRefToInitializedName
   * It was originally written by Leo.  But his version doesn't give back as much information
   * as I need.
   *
   * This further improvement returns every step in a call like
   * this->a.b.c, rather than just either this, or c, as the original
   * did.
   *
   * Variable references can be introduced by SgVarRef, SgPntrArrRefExp,
   * SgInitializedName, SgMemberFunctionRef etc. This function will
   * convert them all to  a top level SgInitializedName, except an
   * thisExp. It makes an AccessSetRecord for each one and returns it.
   *
   * HOWEVER, in some cases the reference returned IS NOT a
   * SgInitializedName!  The normal case is something like foo.bar()  In
   * this case Qing's code considered bar() a reference and passes it
   * back, but it's NOT an SgInitializedName.  So we just ignore that, 
   * and a couple of other cases...
   *
   * \param[in] funcDef: The function where this statement comes from.  Required for determining
   *                     the scope of the reference
   * \param[in] current: The SgNode we're trying to turn into one or more
   *                     InitializedNames
   * \param[in] accessOrigin: The original statement.  Used for making notestring, maybe delte
   * \param[inout] thisFuncThis: A single thisExp to use for all this references
   *
   * \return: A set of AccessSetRecords: I really didn't want to return a vector, but it's the best way to
   * get both sides of a binary op, and merge everything easily
   */
  std::set<ReadWriteSets::AccessSetRecord> recursivelyMakeAccessRecord(SgFunctionDefinition* funcDef, SgNode* current, SgNode* accessOrigin, SgThisExp** thisFuncThis);

  /**
   * This function attempts to determine the AccessType of a given access.
   * It has to be done recursively.  For example, if an array is seen,
   * we recursively check the element type of the array.
   *
   * \param[in] curType: Most coarse grain view of the variable written
   * to.  So foo.bar.a, we start from foo.  Then recurively walk foo,
   * then bar, looking for pointers.  Usually type is determined by "a"
   * but not always.
   **/
  ReadWriteSets::VarType determineType(SgType* curType);

  /**
   * Collect the Read/Write sets of a single function, without recusion.
   *
   * \param[in] funcDef: The function definition to process
   * \param[out] readSet: All the variables read
   * \param[out] writeSet: All the variable written
   *
   **/
  void collectRWSetsNoRecursion(SgFunctionDefinition* funcDef, 
                                ReadWriteSets::FunctionReadWriteRecord& record);    

  /**
   * This function fills in the read and write set caches for a particular function.
   * It fills in the global caches directly, so they are not taken as function
   * arguments.
   * Steps: 
   * 1. Get global caches
   * 2. Collect the R/W sets for funcDef only (no recursion)
   * 3. Collect the R/W sets for each function funcDef calls and insert them 
   *    into the R/W set as well
   *
   * param[in] funcDef:  The function we are determining the purity of
   **/
  void collectFunctionReadWriteSets(SgFunctionDefinition* funcDef);


  /**
   * Gets all functions, runs collectFunctionReadWriteSets on each (which may 
   * do recursive calls).  The sub calls fill in the global cache directly,
   * so there are no real arguments.
   *
   * param[in] root:  SgProject root of this project.
   * param[in] commandLine: Passed in just to save it in the FunctionRecords.
   **/
  void collectReadWriteSets(SgProject *root);


  /**
   * Walks through the cache inserting it all into a nlohmann json object.
   * It's possible that if I used a std::string to represent the function
   * instead of an SgFunctionDeclaration nlohmann could convert the whole
   * cache directly to json, but using an SgFunctionDeclaration is 
   * convienent when actually making the cache.
   * This just reads directly from the global cache, so no arguments needed
   *
   **/
  nlohmann::json convertCacheToJson();

  /**
   * Opens the output file, calls the function to convert the cache to json.
   * Writes out.
   *
   * param[in] outFilename:  The file to write the cache out to as json
   **/
  void outputCache(std::string& outFilename);



  /**
   * Return a const reference to the entire cache of read/write sets
   * for the all functions analyzed.  Useful for iterating over.
   *
   **/
  const std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& getRWSetCache() {
    return rwSetCache;
  };

  /**
   * Return a const reference to the readSet of a single function
   * \param[in]: sgFunctionDeclaration: The function to get a read set of
   **/
  const std::set<ReadWriteSets::AccessSetRecord>& getReadSet(SgFunctionDeclaration* sgFunctionDeclaration);
  /**
   * Return a const reference to the writeSet of a single function
   * \param[in]: sgFunctionDeclaration: The function to get a write set of
   **/
  const std::set<ReadWriteSets::AccessSetRecord>& getWriteSet(SgFunctionDeclaration* sgFunctionDeclaration);

  const std::unordered_set<std::string>& getRequiredTemplateInstantiations() const {
    return requiredTemplateInstantiations;
  }


private:

  //! \brief Helper function to determine if a variable is used as a function pointer
  bool isFunctionPointer(SgVarRefExp* inVarRef);


  /** Check if parent is a Dot or Arrow type
   * 
   * Dot and arrow types can be a pain when they are repeated.  (Like,
   * a.b.c.d();)  The original idea to recurse in d and then step back
   * out setting each accessType to FIELD_ACCESS or ARROW on the way
   * out doesn't work if there are more than one because I (kind of)
   * end up reversing the tree.  Rose parses like this:
   *            .
   *           / \
   *         .    d()
   *        / \
   8       .   c
   *      / \
   *     a   b
   * But my form of this is a[b[c[d()]]]
   * We recurse down to a.b, and set a as a field access, but we don't
   * know about b.  We don't know that b is a Dot expression as well
   * until we return out of recursion a bit.  But we've already
   * created b at that point with the wrong access type.  
   *
   * So this function checks if the parent SgNode is a dot or arrow,
   * so when we're on a.b we can check b.c and create b as a FIELD_ACCESS
   **/
  ReadWriteSets::AccessType checkParentAccessTypeForBinaryOps(SgNode* current);
  

  /** 
   * This is a cache functions -> variables read (fully qualified
   * names).  It is not recursive
   *        
   * Each variable name must be fully qualified.  If the variable is
   * local, it will be preceeded by the fully qualified function name,
   * like this ('@' a delimit the function name):
   * @::foo@var
   * 
   * file -> function -> ReadSet-> Variablename
   **/
  std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash> rwSetCache;


  /**
   *  In C++, a template usage by pointer or reference may not require that the template actually be instantiated.  
   *  When that happens, we can't classify the type, or unit test the function properly.  We can force a template to be
   *  instantated with a declaration like "template class MyClass<int>"
   *
   *  But these cases can be detected by LocalRWSetGenerator, so when they are seen, a properly instantiation will be saved 
   *  here, and later written to a file so it can be placed in total_headers.hh, and be useful.
   **/
  std::unordered_set<std::string> requiredTemplateInstantiations;
  
  //! \brief command line used to generate all this.  Needs to match for NodeIds to be valid.
  std::string commandLine;

  friend std::ostream& operator<< (std::ostream& os, const LocalRWSetGenerator& rwset);

};

std::ostream& operator<< (std::ostream& os, const LocalRWSetGenerator& rwset);


#endif  //LOCALRWSETGENERATOR
