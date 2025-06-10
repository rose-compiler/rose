#ifndef TESTABILITYGRADER_H
#define TESTABILITYGRADER_H
#include <unordered_set>
#include "ReadWriteSetRecords.h"

/**
 * \class TestabilityGrader
 *
 * \brief TestabilityGrader goes through the R/W set json files generated
 *        by LocalRWSetGenerator and recursively goes through
 *        each function's calledFunctions to rate the full testability of 
 *        the function.  It does NOT use ROSE.  
 *
 **/


class TestabilityGrader 
{
public:    

  TestabilityGrader() {};

  /**
   * mergeFileIntoCache
   *
   * TestabilityGrader is normally called with a number of json files that must be merged into one cache for easy access.  
   * This function does the inital merge set.
   *
   * \param[in] inCache : This is the "cache" that was just read from a json file and is to be merged in 
   **/
  void mergeFileIntoCache(const std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& inCache);

  /**
   * Evaluates each FunctionRecord recursively, that is, it also gets the 
   * maxGlobality and maxAccessType from each calledFunction.
   * 
   * This is performed recursively, so THREE caches are involved:
   *   1. startCache: The cache read from the input files and combined.  It is assumed to have come from files 
   *                  where the files were processed individually and the accessRecords have been evaluated, 
   *                  but functions have not.
   *   2. completedCache : This contains functions that HAVE been processed.  This is to avoid unecessary work.
   *   3. workingCache: This is passed in and should only contain functions that are currently being processed.
   *                    This is avoid infinite recursion.  The working cache functions aren't complete, but if 
   *                    we see a function in here, we know we can stop recursing. 
   *
   * \param[record] The function we are evaluating (calledFunctions will be recursed into)
   * \param[out] maxGlobality  : Expected to be 0 during the initial call!
   * \param[out] maxAccessType : Expected to be 0 during the initial call!
   * \param[in] ignoreFunctions : Functions to ignore when it comes to RWSets because they have issue that are known to not be enlightening.
   *                              (Example, ... like printLog are UNKNOWN, but don't affect the caller)
   * \param[inout] localPtrFuncs : Functions that access a local pointer, inside this function we will add functions that call a
   *                               function that access a local pointer 
   * \param[inout] workingCache: The set of functions seen so far during this recursion, so we don't infinately loop
   **/
  void recursivelyEvaluateFunctionRecords(const ReadWriteSets::FunctionReadWriteRecord& record,
                                          ReadWriteSets::Globality& maxGlobality, 
                                          ReadWriteSets::VarType& maxVarType,
                                          ReadWriteSets::AccessType& maxAccessType,
                                          const std::set<std::string>& ignoreFunctions,
                                          std::unordered_set< std::string > localPtrFuncs,
                                          std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& workingCache);


  /** 
   * This is a cache functions -> variables read (fully qualified
   * names).  The functions are NOT rated because called functions 
   * have not been recusively rated.  And each R/W set is local only.
   *        
   * Each variable name must be fully qualified.  If the variable is
   * local, it will be preceeded by the fully qualified function name,
   * like this ('@' a delimit the function name):
   * @::foo@var
   * 
   * function -> ReadSet-> AccessSetRecord
   **/
  std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash> startCache;

  /** 
   * This is a cache functions -> variables read (fully qualified
   * names).  The functions ARE rated, before a FunctionReadWriteRecord
   * can be inserted into this cache, the called Functions must be evaluated. 
   *        
   * Each variable name must be fully qualified.  If the variable is
   * local, it will be preceeded by the fully qualified function name,
   * like this ('@' a delimit the function name):
   * @::foo@var
   * 
   * function -> ReadSet-> AccessSetRecord
   **/
  std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash> completedCache;


};




#endif //TESTABILITYGRADER
