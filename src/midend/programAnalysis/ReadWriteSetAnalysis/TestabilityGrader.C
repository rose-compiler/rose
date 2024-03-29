#include <sage3basic.h>
#include "TestabilityGrader.h"


/**
 * mergeFileIntoCache
 *
 * TestabilityGrader is normally called with a number of json files that must be merged into one cache for easy access.  
 * This function does the inital merge set.
 *
 * \param[in] inCache : This is the "cache" that was just read from a json file and is to be merged in 
 **/
void TestabilityGrader::mergeFileIntoCache(const std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>& inSet) {
  for (auto& thisRecord : inSet) {
    std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>::iterator isInCache = startCache.find(thisRecord);
    // a Function already exists in the cache, attempt to merge the
    // two functions.  (This happens with functions defined in .h
    // files a lot.)
    if(isInCache != startCache.end()) {
      std::cerr << "Function Name collision! Two files contain a matching function name, this shouldn't happen!" << std::endl;
      std::cerr << "Function Name: " << isInCache->internalFunctionName << " filename: " << isInCache->filename << std::endl;
      std::cerr << "Function Name: " << thisRecord.internalFunctionName << " filename: " << thisRecord.filename << std::endl;
    }

    startCache.insert(thisRecord);
  }
      
}

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
 * \param[out] maxVarType : Expected to be 0 during the initial call!
 * \param[out] maxAccessType : Expected to be 0 during the initial call!
 * \param[inout] workingCache: The set of functions seen so far during this recursion, so we don't infinitely loop
 **/
void TestabilityGrader::recursivelyEvaluateFunctionRecords(const ReadWriteSets::FunctionReadWriteRecord& record,
                                                           ReadWriteSets::Globality& maxGlobality,
                                                           ReadWriteSets::VarType& maxVarType,
                                                           ReadWriteSets::AccessType& maxAccessType,
                                                           std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, 
                                                           ReadWriteSets::FunctionReadWriteRecord_hash>& workingCache) {

  //NodeId isn't really valid in this context, but if this is a valid record, it will have one.
  //So check to ensure a bad record wasn't passed in by mistake
  ROSE_ASSERT(record.nodeId != "");

  const auto& inWorkingSet = workingCache.find(record);
  if(inWorkingSet != workingCache.end()) {
    //function already in the working set, no need to continue
    maxGlobality = inWorkingSet->globality;
    maxVarType = inWorkingSet->varType;
    maxAccessType = inWorkingSet->accessType;
    return;
  }

  const auto& inCompletedSet = completedCache.find(record);
  if(inWorkingSet != completedCache.end()) {
    //function already in the completed set, no need to continue
    maxGlobality = inCompletedSet->globality;
    maxVarType = inCompletedSet->varType;
    maxAccessType = inCompletedSet->accessType;
    return;
  }
  
  //Done with checks, start working.  funcRecord goes in workingCache
  //First get the globality and accessType for the local RWSets
  maxTestability(record, maxGlobality, maxVarType, maxAccessType);
  ReadWriteSets::FunctionReadWriteRecord localRecord(record);
  localRecord.globality = maxGlobality; 
  localRecord.varType = maxVarType;
  localRecord.accessType = maxAccessType;
  workingCache.insert(localRecord);

  //Need to merge every RWSet entry from the called function into the current record
  
  for(const std::string& calledFuncName : localRecord.calledFunctions) {
    //Gotta make a record to lookup in the cache, only internalFunctionName is required for lookup
    ReadWriteSets::FunctionReadWriteRecord tmpRecord;
    tmpRecord.internalFunctionName = calledFuncName;
    
    const auto& startCacheRecord = startCache.find(tmpRecord);
    if(startCacheRecord == startCache.end()) {
      continue;
    }
    ReadWriteSets::Globality tmpGlobality = ReadWriteSets::LOCALS;
    ReadWriteSets::VarType tmpVarType = ReadWriteSets::PRIMITIVES;
    ReadWriteSets::AccessType tmpAccessType = ReadWriteSets::NORMAL;
    recursivelyEvaluateFunctionRecords(*startCacheRecord, tmpGlobality, tmpVarType, tmpAccessType, workingCache);
    if(maxGlobality < tmpGlobality)
      maxGlobality = tmpGlobality;
    if(maxVarType < tmpVarType)
      maxVarType = tmpVarType;
    if(maxAccessType < tmpAccessType)
      maxAccessType = tmpAccessType;

    //Merge in all the accesses from the called functions into this function's RWSets
    ReadWriteSets::recursiveMerge(localRecord.readSet, startCacheRecord->readSet);
    ReadWriteSets::recursiveMerge(localRecord.writeSet, startCacheRecord->writeSet);
    
  } 
  localRecord.globality = maxGlobality;
  localRecord.varType = maxVarType;
  localRecord.accessType = maxAccessType;

  completedCache.insert(localRecord);
  
}
