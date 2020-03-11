/* data race detection - refactoring in progress */

#include "sage3basic.h"
#include "DataRaceDetection.h"
#include "Specialization.h"
#include "EquivalenceChecking.h"
#include "AstTerm.h"
#include "OmpSupport.h"
#include "CodeThornCommandLineOptions.h"

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;
using namespace Sawyer::Message;
using namespace OmpSupport;

Sawyer::Message::Facility DataRaceDetection::logger;

DataRaceDetection::DataRaceDetection() {
  static bool initialized = false;
  if (!initialized) {
    initialized = true;
    logger = Sawyer::Message::Facility("CodeThorn::DataRaceDetection", Rose::Diagnostics::destination);
    Rose::Diagnostics::mfacilities.insertAndAdjust(logger);
  }
}

DataRaceDetection::Options::Options():active(false),
                                      dataRaceFail(false),
                                      maxFloatingPointOperations(0),
                                      useConstSubstitutionRule(false),
                                      visualizeReadWriteSets(false),
                                      printUpdateInfos(false)
{
}

void DataRaceDetection::handleCommandLineOptions(Analyzer& analyzer) {
  //cout<<"DEBUG: initializing data race detection"<<endl;
  if(args.getBool("data-race-fail")) {
    args.setOption("data-race",true);
  }
  if(args.isDefined("data-race-csv")) {
    options.dataRaceCsvFileName=args.getString("data-race-csv");
    args.setOption("data-race",true);
  }
  if(args.getBool("data-race")) {
    options.active=true;
    //cout<<"INFO: ignoring lhs-array accesses"<<endl;
    analyzer.setSkipArrayAccesses(true);
    options.useConstSubstitutionRule=args.getBool("rule-const-subst");
    options.maxFloatingPointOperations=0; // not used yet
  }
  if (args.getBool("visualize-read-write-sets")) {
    options.visualizeReadWriteSets=true;
  }
  if(args.getBool("print-update-infos")) {
    options.printUpdateInfos=true;
  }
  options.useConstSubstitutionRule=args.getBool("rule-const-subst");
  if(args.isDefined("max-extracted-updates")) {
    options.maxNumberOfExtractedUpdates=args.getInt("max-extracted-updates");
  }
}

void DataRaceDetection::setCsvFileName(string fileName) {
  options.dataRaceCsvFileName=fileName;
}

void DataRaceDetection::reportResult(int verifyUpdateSequenceRaceConditionsResult,
                                     int verifyUpdateSequenceRaceConditionsParLoopNum,
                                     int verifyUpdateSequenceRaceConditionsTotalLoopNum) {
  stringstream text;
  if(verifyUpdateSequenceRaceConditionsResult==-1) {
    text<<"sequential";
  } else if(verifyUpdateSequenceRaceConditionsResult==-2) {
    // not supported yet
    text<<"unknown";
  } else if(verifyUpdateSequenceRaceConditionsResult==0) {
    text<<"no";
  } else {
    text<<"yes";
  }
  text<<","<<verifyUpdateSequenceRaceConditionsResult;
  text<<","<<verifyUpdateSequenceRaceConditionsParLoopNum;
  text<<","<<verifyUpdateSequenceRaceConditionsTotalLoopNum;
  text<<endl;
  
  if(options.dataRaceCsvFileName!="") {
    CodeThorn::write_file(options.dataRaceCsvFileName,text.str());
  } else {
    // if no output file is provided print on std out
    cout << "Data Race Detection: ";
    cout << text.str();
  }
}

bool DataRaceDetection::run(Analyzer& analyzer) {
  if(options.active) {
    try {
      SAR_MODE sarMode=SAR_SSA;
      Specialization speci;
      ArrayUpdatesSequence arrayUpdates;
      RewriteSystem rewriteSystem;   
      int verifyUpdateSequenceRaceConditionsResult=-2;
      int verifyUpdateSequenceRaceConditionsTotalLoopNum=-1;
      int verifyUpdateSequenceRaceConditionsParLoopNum=-1;
      
      analyzer.setSkipUnknownFunctionCalls(true);
      analyzer.setSkipArrayAccesses(true);

      // perform data race detection
      if (options.visualizeReadWriteSets) {
        setVisualizeReadWriteAccesses(true);
      }
      logger[TRACE]<<"STATUS: performing array analysis on STG."<<endl;
      logger[TRACE]<<"STATUS: identifying array-update operations in STG and transforming them."<<endl;
      
      speci.setMaxNumberOfExtractedUpdates(options.maxNumberOfExtractedUpdates);
      speci.dataRaceDetection=true;
      speci.extractArrayUpdateOperations(&analyzer,
                                         arrayUpdates,
                                         rewriteSystem,
                                         options.useConstSubstitutionRule
                                         );
      // TODO: SUBST remove substitution if faster without
      speci.substituteArrayRefs(arrayUpdates, analyzer.getVariableIdMapping(), sarMode, rewriteSystem);
      
      SgNode* root=analyzer.startFunRoot;
      VariableId parallelIterationVar;
      LoopInfoSet loopInfoSet=DataRaceDetection::determineLoopInfoSet(root,analyzer.getVariableIdMapping(), analyzer.getLabeler());
      logger[TRACE]<<"INFO: number of iteration vars: "<<loopInfoSet.size()<<endl;
      verifyUpdateSequenceRaceConditionsTotalLoopNum=loopInfoSet.size();
      verifyUpdateSequenceRaceConditionsParLoopNum=DataRaceDetection::numParLoops(loopInfoSet, analyzer.getVariableIdMapping());
      verifyUpdateSequenceRaceConditionsResult=checkDataRaces(loopInfoSet,arrayUpdates,analyzer.getVariableIdMapping());
      // if no data race is found, but only an incomplete STG computed due to resource constraints, then report unknown (-2)
      if(options.printUpdateInfos) {
        speci.printUpdateInfos(arrayUpdates,analyzer.getVariableIdMapping());
      }

      if(verifyUpdateSequenceRaceConditionsResult==false && analyzer.isIncompleteSTGReady()) {
        logger[TRACE]<<"DEBUG: INCOMPLETE AST AND NO DATA RACE WAS FOUND => UNKNOWN"<<endl;
        verifyUpdateSequenceRaceConditionsResult=-2;
      }
      // cannot handle programs without parallel loop yet
      if(verifyUpdateSequenceRaceConditionsParLoopNum==0) {
        verifyUpdateSequenceRaceConditionsResult=-2;
      }
      reportResult(verifyUpdateSequenceRaceConditionsResult,
                   verifyUpdateSequenceRaceConditionsParLoopNum,
                   verifyUpdateSequenceRaceConditionsTotalLoopNum);
    } catch(const CodeThorn::Exception& e) {
      logger[TRACE] << "CodeThorn::Exception raised & catched inside data race detection (generating 'unknown'): " << e.what() << endl;
      reportResult(-2,0,0);
      return 1;
    }
    return true;
  } else {
    return false;
  }
}

bool DataRaceDetection::isOmpParallelFor(SgForStatement* node) {
  SgNode* parentNode1=node->get_parent();
  ROSE_ASSERT(parentNode1);
  if(isSgOmpSimdStatement(parentNode1)) {
    return true;
  } else if(SgOmpForStatement* ompForStmt=isSgOmpForStatement(parentNode1)) {
    SgNode* parentNode2=ompForStmt->get_parent();
    ROSE_ASSERT(parentNode2);
    return isSgOmpParallelStatement(parentNode2);
  }
  return false;
}

LoopInfoSet DataRaceDetection::determineLoopInfoSet(SgNode* root, VariableIdMapping* variableIdMapping, Labeler* labeler) {
  //cout<<"INFO: loop info set and determine iteration vars."<<endl;
  LoopInfoSet loopInfoSet;
  RoseAst ast(root);
  AstMatching m;

  // (i) match all for-stmts and (ii) filter canonical ones
  string matchexpression="$FORSTMT=SgForStatement(_,_,..)";
  MatchResult r=m.performMatching(matchexpression,root);
  //cout << "DEBUG: Matched for loops: "<<r.size()<<endl;
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    LoopInfo loopInfo;
    SgNode* forNode=(*i)["$FORSTMT"];
    //cout << "DEBUG: Detected for loops: "<<forNode->unparseToString()<<endl;

    //cout<<"DEBUG: MATCH: "<<forNode->unparseToString()<<AstTerm::astTermWithNullValuesToString(forNode)<<endl;
    ROSE_ASSERT(isSgForStatement(forNode));
    SgInitializedName* ivar=0;
    SgExpression* lb=0;
    SgExpression* ub=0;
    SgExpression* step=0;
    SgStatement* body=0;
    bool hasIncrementalIterationSpace=false;
    bool isInclusiveUpperBound=false;
    bool isCanonicalOmpForLoop=SageInterface::isCanonicalForLoop(forNode, &ivar, &lb, &ub, &step, &body, &hasIncrementalIterationSpace, &isInclusiveUpperBound);
    if(isCanonicalOmpForLoop) {
      ROSE_ASSERT(ivar);
      SgInitializedName* node=0;
      if(isCanonicalOmpForLoop) {
        node=ivar;
      }
      ROSE_ASSERT(node);
#if 0      
      // WORKAROUND 1
      // TODO: investigate why the for pointer is not stored in the same match-result
      if(forNode==0) {
        forNode=node; // init
        while(!isSgForStatement(forNode)||isSgProject(forNode))
        forNode=forNode->get_parent();
      }
      ROSE_ASSERT(!isSgProject(forNode));
#endif
      loopInfo.iterationVarId=variableIdMapping->variableId(node);
      loopInfo.forStmt=isSgForStatement(forNode);
      loopInfo.iterationVarType=isOmpParallelFor(loopInfo.forStmt)?ITERVAR_PAR:ITERVAR_SEQ;
      if(loopInfo.forStmt) {
        const SgStatementPtrList& stmtList=loopInfo.forStmt->get_init_stmt();
        ROSE_ASSERT(stmtList.size()==1);
        loopInfo.initStmt=stmtList[0];
        loopInfo.condExpr=loopInfo.forStmt->get_test_expr();
        loopInfo.computeLoopLabelSet(labeler);
        loopInfo.computeOuterLoopsVarIds(variableIdMapping);
        loopInfo.isOmpCanonical=true;
      } else {
        cerr<<"WARNING: no for statement found."<<endl;
        if(forNode) {
          cerr<<"for-loop:"<<forNode->unparseToString()<<endl;
        } else {
          cerr<<"for-loop: 0"<<endl;
        }
      }
    } else {
      loopInfo.forStmt=isSgForStatement(forNode);
      loopInfo.isOmpCanonical=false;
    }
    loopInfoSet.push_back(loopInfo);
  }
  logger[TRACE]<<"INFO: found "<<DataRaceDetection::numParLoops(loopInfoSet,variableIdMapping)<<" parallel loops."<<endl;
  return loopInfoSet;
}

// finds the list of pragmas (in traversal order) with the prefix 'prefix' (e.g. '#pragma omp parallel' is found for prefix 'omp')
list<SgPragmaDeclaration*> DataRaceDetection::findPragmaDeclarations(SgNode* root, string pragmaKeyWord) {
  list<SgPragmaDeclaration*> pragmaList;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin(); i!=ast.end();++i) {
    if(SgPragmaDeclaration* pragmaDecl=isSgPragmaDeclaration(*i)) {
      string foundPragmaKeyWord=SageInterface::extractPragmaKeyword(pragmaDecl);
      //cout<<"DEBUG: PRAGMAKEYWORD:"<<foundPragmaKeyWord<<endl;
      if(pragmaKeyWord==foundPragmaKeyWord || "end"+pragmaKeyWord==foundPragmaKeyWord) {
        pragmaList.push_back(pragmaDecl);
      }
    }
  }
  return pragmaList;
}

// returns the number of detected data races
int DataRaceDetection::checkDataRaces(LoopInfoSet& loopInfoSet, 
                                      ArrayUpdatesSequence& arrayUpdates, 
                                      VariableIdMapping* variableIdMapping) {
  int errorCount=0;
  logger[TRACE]<<"checking race conditions."<<endl;
  logger[INFO]<<"number of parallel loops: "<<numParLoops(loopInfoSet,variableIdMapping)<<endl;
  
  // only used when USE_ALL_ITER_VARS is defined. See also "_checkAllLoops".
  VariableIdSet allIterVars;
  for(LoopInfoSet::iterator lis=loopInfoSet.begin();lis!=loopInfoSet.end();++lis) {
    allIterVars.insert((*lis).iterationVarId);
  }
  // check each parallel loop
  for(LoopInfoSet::iterator lis=loopInfoSet.begin();lis!=loopInfoSet.end();++lis) {
      if((*lis).iterationVarType==ITERVAR_PAR || _checkAllLoops) {
      VariableId parVariable;
      parVariable=(*lis).iterationVarId;
      if(_checkAllLoops) {
        logger[INFO]<<"checking loop: "<<variableIdMapping->variableName(parVariable)<<endl;
      } else {
        logger[INFO]<<"checking parallel loop: "<<variableIdMapping->variableName(parVariable)<<endl;
      }
      IndexToReadWriteDataMap indexToReadWriteDataMap;
      populateReadWriteDataIndex(*lis, indexToReadWriteDataMap, arrayUpdates, variableIdMapping);
      displayReadWriteDataIndex(indexToReadWriteDataMap, variableIdMapping);  // requires log level "debug"
      // perform data race check
      errorCount += numberOfRacyThreadPairs(indexToReadWriteDataMap, variableIdMapping);
      if (errorCount > 0 && !_checkAllLoops) {
	break; // found at least one data race, skip the remaining loops
      }
    } // if parallel loop
  } // foreach loop
  return errorCount;
}

void DataRaceDetection::populateReadWriteDataIndex(LoopInfo& li, IndexToReadWriteDataMap& indexToReadWriteDataMap, 
                                                   ArrayUpdatesSequence& arrayUpdates, 
                                                   VariableIdMapping* variableIdMapping) {
  for(ArrayUpdatesSequence::iterator i=arrayUpdates.begin();i!=arrayUpdates.end();++i) {
    const EState* estate=(*i).first;
    if (li.isInAssociatedLoop(estate)) {
      const PState* pstate=estate->pstate();
      SgExpression* exp=(*i).second;
      IndexVector index = extractIndexVector(li, pstate);
      // check index and report unknown if negative (TODO: recompute negative indices)
      for (auto idx : index) {
        if(idx<0) {
          stringstream ss;
          ss<<idx;
          throw CodeThorn::Exception("Negative array index detected: "+ss.str());
        }
      }
      addAccessesFromExpressionToIndex(exp, index, indexToReadWriteDataMap, variableIdMapping);
    }
  } // array sequence iter
}


IndexVector DataRaceDetection::extractIndexVector(LoopInfo& li, const PState* pstate) {
  IndexVector index;
  VariableId parVariable=li.iterationVarId;
  // use all vars for indexing or only outer+par loop variables
  for(VariableIdSet::iterator ol=li.outerLoopsVarIds.begin();ol!=li.outerLoopsVarIds.end();++ol) {
    VariableId otherVarId=*ol;
    ROSE_ASSERT(otherVarId.isValid());
    if(!pstate->varValue(otherVarId).isTop()&&pstate->varValue(otherVarId).isConstInt()) {
      int otherIntVal=pstate->varValue(otherVarId).getIntValue();
      index.push_back(otherIntVal);
    }
  }
  if(!pstate->varValue(parVariable).isTop()&&pstate->varValue(parVariable).isConstInt()) {
    int parIntVal=pstate->varValue(parVariable).getIntValue();
    index.push_back(parIntVal);
  }
  return index;
}

bool DataRaceDetection::isSharedArrayAccess(SgPntrArrRefExp* useRef) {
  SgNode* lhsExp=SgNodeHelper::getLhs(useRef);
  //cout<<"DEBUG: checking lhs: "<<lhsExp->unparseToString()<<endl;
  if(SgVarRefExp* varRefExp=isSgVarRefExp(lhsExp)) {
    return isSharedVariable(varRefExp);
  }
  return true;
}

bool DataRaceDetection::isSharedVariable(SgVarRefExp* varRefExp) {
  if(varRefExp) {
    omp_construct_enum sharingProperty=OmpSupport::getDataSharingAttribute(varRefExp);
    bool isShared=(sharingProperty==OmpSupport::e_shared);
    //cout<<"Var: "<<varRefExp->unparseToString()<<" shared: "<<isShared<<endl;
    return isShared;
  }
  return true;
}

void DataRaceDetection::addAccessesFromExpressionToIndex(SgExpression* exp, IndexVector& index,
                                                      IndexToReadWriteDataMap& indexToReadWriteDataMap, 
                                                      VariableIdMapping* variableIdMapping) {  
  SgExpression* lhs=isSgExpression(SgNodeHelper::getLhs(exp));
  SgExpression* rhs=isSgExpression(SgNodeHelper::getRhs(exp));
  //ROSE_ASSERT(isSgPntrArrRefExp(lhs)||SgNodeHelper::isFloatingPointAssignment(exp)); // only for equivalence checking valid
        
  //cout<<"EXP: "<<exp->unparseToString()<<", lhs:"<<lhs->unparseToString()<<" :: "<<endl;
  // read-set
  RoseAst rhsast(rhs);
  for (RoseAst::iterator j=rhsast.begin(); j!=rhsast.end(); ++j) {
    if(SgPntrArrRefExp* useRef=isSgPntrArrRefExp(*j)) {
      if(isSharedArrayAccess(useRef)) {
        ArrayElementAccessData access(useRef,variableIdMapping);
        if(access.hasNegativeIndex()) {
          throw CodeThorn::Exception("addAccessesFromExpressionToIndex: Negative array index detected.");
        }
        indexToReadWriteDataMap[index].readArrayAccessSet.insert(access);
      }
      j.skipChildrenOnForward();
    } else if(SgVarRefExp* useRef=isSgVarRefExp(*j)) {
      ROSE_ASSERT(useRef);
      if(isSharedVariable(useRef)) {
        VariableId varId=variableIdMapping->variableId(useRef);
        indexToReadWriteDataMap[index].readVarIdSet.insert(varId);
      }
      j.skipChildrenOnForward();
    } else {
      //cout<<"INFO: UpdateExtraction: ignored expression on rhs:"<<(*j)->unparseToString()<<endl;
    }
  }
  if(SgPntrArrRefExp* arr=isSgPntrArrRefExp(lhs)) {
    if(isSharedArrayAccess(arr)) {
      ArrayElementAccessData access(arr,variableIdMapping);
      if(access.hasNegativeIndex()) {
        throw CodeThorn::Exception("addAccessesFromExpressionToIndex: Negative array index detected.");
      }
      indexToReadWriteDataMap[index].writeArrayAccessSet.insert(access);
    }
  } else if(SgVarRefExp* var=isSgVarRefExp(lhs)) {
    if(isSharedVariable(var)) {
      VariableId varId=variableIdMapping->variableId(var);
      indexToReadWriteDataMap[index].writeVarIdSet.insert(varId);
    }
  } else {
    cerr<<"Error: addAccessFromExpressoinToIndex: unknown LHS."<<endl;
    exit(1);
  }
}

void DataRaceDetection::displayReadWriteDataIndex(IndexToReadWriteDataMap& indexToReadWriteDataMap, 
                                               VariableIdMapping* variableIdMapping) {
  logger[DEBUG]<<"indexToReadWriteDataMap size: "<<indexToReadWriteDataMap.size()<<endl;
  for(IndexToReadWriteDataMap::iterator imap=indexToReadWriteDataMap.begin();
      imap!=indexToReadWriteDataMap.end();
      ++imap) {
    IndexVector index=(*imap).first;
    logger[DEBUG]<<"INDEX: ";
    for(IndexVector::iterator iv=index.begin();iv!=index.end();++iv) {
      if(iv!=index.begin())
        logger[DEBUG]<<",";
      logger[DEBUG]<<*iv;
    }
    logger[DEBUG]<<endl;
    logger[DEBUG]<<" R-SET: ";
    for(VariableIdSet::const_iterator i=indexToReadWriteDataMap[index].readVarIdSet.begin();i!=indexToReadWriteDataMap[index].readVarIdSet.end();++i) {
      logger[DEBUG]<<(*i).toString(variableIdMapping)<<" ";
    }
    logger[DEBUG]<<endl;
    logger[DEBUG]<<" W-SET: ";
    for(VariableIdSet::const_iterator i=indexToReadWriteDataMap[index].writeVarIdSet.begin();i!=indexToReadWriteDataMap[index].writeVarIdSet.end();++i) {
      logger[DEBUG]<<(*i).toString(variableIdMapping)<<" ";
    }
    logger[DEBUG]<<endl;
    logger[DEBUG]<<" ARRAY R-SET: ";
    for(ArrayElementAccessDataSet::const_iterator i=indexToReadWriteDataMap[index].readArrayAccessSet.begin();i!=indexToReadWriteDataMap[index].readArrayAccessSet.end();++i) {
      logger[DEBUG]<<(*i).toString(variableIdMapping)<<" ";
    }
    logger[DEBUG]<<endl;
    logger[DEBUG]<<" ARRAY  W-SET: ";
    for(ArrayElementAccessDataSet::const_iterator i=indexToReadWriteDataMap[index].writeArrayAccessSet.begin();i!=indexToReadWriteDataMap[index].writeArrayAccessSet.end();++i) {
      logger[DEBUG]<<(*i).toString(variableIdMapping)<<" ";
    }
    logger[DEBUG]<<endl;
    logger[DEBUG]<<"#read-array-accesses:"<<indexToReadWriteDataMap[index].readArrayAccessSet.size()<<"#read-var-accesses:"<<indexToReadWriteDataMap[index].readVarIdSet.size()<<endl;
    logger[DEBUG]<<"#write-array-accesses:"<<indexToReadWriteDataMap[index].writeArrayAccessSet.size()<<"#write-var-accesses:"<<indexToReadWriteDataMap[index].writeVarIdSet.size()<<endl;
  } // imap
}

int DataRaceDetection::numberOfRacyThreadPairs(IndexToReadWriteDataMap& indexToReadWriteDataMap, 
					    VariableIdMapping* variableIdMapping) {
  // perform the data race check
  // 1) compute vector of index-vectors for each outer-var-vector
  CheckMapType checkMap;
  populateCheckMap(checkMap, indexToReadWriteDataMap);
  // 2) check each index-vector. For each iteration of each par-loop iteration then.
  int errorCount = 0;
  VariableIdSet readWriteRaces;
  VariableIdSet writeWriteRaces;
  ArrayElementAccessDataSet arrayReadWriteRaces;
  ArrayElementAccessDataSet arrayWriteWriteRaces;
  bool drdebug=false;
  if(drdebug) cout<<"DEBUG: checkMap size: "<<checkMap.size()<<endl;
  for(CheckMapType::iterator miter=checkMap.begin();miter!=checkMap.end();++miter) {
    IndexVector outerVarIndexVector=(*miter).first;
    if(drdebug) logger[DEBUG]<<"outerVarIndexVector: "<<indexVectorToString(outerVarIndexVector)<<endl;
    ThreadVector threadVectorToCheck=(*miter).second;
    if(drdebug) logger[DEBUG]<<"vector size to check: "<<threadVectorToCheck.size()<<endl;
    for(ThreadVector::iterator tv1=threadVectorToCheck.begin();tv1!=threadVectorToCheck.end();++tv1) {
      if(drdebug) logger[DEBUG]<<"thread-vectors: tv1:"<<"["<<indexVectorToString(*tv1)<<"]"<<endl;
      ArrayElementAccessDataSet arrayWset1=indexToReadWriteDataMap[*tv1].writeArrayAccessSet;
      ArrayElementAccessDataSet arrayRset1=indexToReadWriteDataMap[*tv1].readArrayAccessSet;
      if(drdebug) logger[DEBUG]<<"tv1-arrayWset1:"<<arrayWset1.size()<<": "<<arrayElementAccessDataSetToString(arrayWset1,variableIdMapping)<<endl;
      if(drdebug) logger[DEBUG]<<"tv1-arrayRset1:"<<arrayRset1.size()<<": "<<arrayElementAccessDataSetToString(arrayRset1,variableIdMapping)<<endl;
      VariableIdSet wset1=indexToReadWriteDataMap[*tv1].writeVarIdSet;
      VariableIdSet rset1=indexToReadWriteDataMap[*tv1].readVarIdSet;
      // Compare each loop index pair only once
      ThreadVector::iterator tv2=tv1;
      ++tv2;
      for(; tv2!=threadVectorToCheck.end(); ++tv2) {
	if(drdebug) logger[DEBUG]<<"thread-vectors: tv2:"<<"["<<indexVectorToString(*tv2)<<"]"<<endl;
	ArrayElementAccessDataSet arrayRset2=indexToReadWriteDataMap[*tv2].readArrayAccessSet;
	ArrayElementAccessDataSet arrayWset2=indexToReadWriteDataMap[*tv2].writeArrayAccessSet;
	if(drdebug) logger[DEBUG]<<"arrayRset2:"<<arrayRset2.size()<<": "<<arrayElementAccessDataSetToString(arrayRset2,variableIdMapping)<<endl;
	if(drdebug) logger[DEBUG]<<"arrayWset2:"<<arrayWset2.size()<<": "<<arrayElementAccessDataSetToString(arrayWset2,variableIdMapping)<<endl;
	VariableIdSet wset2=indexToReadWriteDataMap[*tv2].writeVarIdSet;
	VariableIdSet rset2=indexToReadWriteDataMap[*tv2].readVarIdSet;
	if (dataRaceExistsInvolving1And2(wset1, rset1, wset2, rset2, writeWriteRaces, readWriteRaces)
	    || dataRaceExistsInvolving1And2(arrayWset1, arrayRset1, arrayWset2, arrayRset2, arrayWriteWriteRaces, arrayReadWriteRaces) ) {
	  errorCount++;
	  if (!_checkAllDataRaces && !_checkAllLoops) {
	    return errorCount;
	  }	  
	}
      }
      if(drdebug) logger[DEBUG]<<"------------------------"<<endl;
    }
  }
  // 3) optional: Generate a dot graph for visualizing reads and writes (including data races)
  if (getVisualizeReadWriteAccesses()) {
    string filename = "readWriteSetGraph.dot";
    Visualizer visualizer;
    string dotGraph = visualizer.visualizeReadWriteAccesses(indexToReadWriteDataMap, variableIdMapping, 
							    arrayReadWriteRaces, arrayWriteWriteRaces, 
							    !args.getBool("rw-data"), 
							    args.getBool("rw-clusters"),
							    args.getBool("rw-highlight-races"));
    write_file(filename, dotGraph);
    logger[TRACE] << "STATUS: written graph that illustrates read and write accesses to file: " << filename << endl;
  }
  return errorCount;
}

void DataRaceDetection::populateCheckMap(CheckMapType& checkMap, IndexToReadWriteDataMap& indexToReadWriteDataMap) {
  for(IndexToReadWriteDataMap::iterator imap=indexToReadWriteDataMap.begin();
      imap!=indexToReadWriteDataMap.end();
      ++imap) {
    IndexVector index=(*imap).first;
    IndexVector outVarIndex;
    // if index.size()==0, it will analyze the loop independet of outer loops
    if(index.size()>0) {
      ROSE_ASSERT(index.size()>0);
      for(size_t iv1=0;iv1<index.size()-1;iv1++) {
	outVarIndex.push_back(index[iv1]);
      }
      ROSE_ASSERT(outVarIndex.size()<index.size());
    } else {
      // nothing to check
      continue;
    }
    // last index of index of par-variable
    //int parVariableValue=index[index.size()-1];
    checkMap[outVarIndex].push_back(index);
  }
}

int DataRaceDetection::numParLoops(LoopInfoSet& loopInfoSet, VariableIdMapping* variableIdMapping) {
  int checkParLoopNum=0;
  for(LoopInfoSet::iterator i=loopInfoSet.begin();i!=loopInfoSet.end();++i) {
    if((*i).iterationVarType==ITERVAR_PAR) {
      checkParLoopNum++;
      //cout<<"DEBUG: PAR-VAR:"<<variableIdMapping->variableName((*i).iterationVarId)<<endl;
    }
  }
  return checkParLoopNum;
}

void DataRaceDetection::setCheckAllLoops(bool val) {
  _checkAllLoops=val;
}
void DataRaceDetection::setCheckAllDataRaces(bool val) {
  _checkAllDataRaces=val;
}

bool DataRaceDetection::getVisualizeReadWriteAccesses() {
  return _visualizeReadWriteAccesses;
}

void DataRaceDetection::setVisualizeReadWriteAccesses(bool val) {
  _visualizeReadWriteAccesses=val;
}

std::string DataRaceDetection::indexVectorToString(IndexVector iv) {
  std::stringstream ss;
  for(IndexVector::iterator i=iv.begin();i!=iv.end();++i) {
    if(i!=iv.begin())
      ss<<", ";
    ss<<(*i);
  }
  return ss.str();
}

std::string DataRaceDetection::arrayElementAccessDataSetToString(ArrayElementAccessDataSet& ds, VariableIdMapping* vim) {
  std::stringstream ss;
  for(ArrayElementAccessDataSet::iterator i=ds.begin();i!=ds.end();++i) {
    if(i!=ds.begin())
      ss<<", ";
    ss<<(*i).toString(vim);
  }
  return ss.str();
}
