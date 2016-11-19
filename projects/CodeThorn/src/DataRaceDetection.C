/* data race detection - refactoring in progress */

#include "Analyzer.h"
#include "CommandLineOptions.h"
#include "DataRaceDetection.h"

void initDataRaceDetection(Analyzer& analyzer) {
  //cout<<"DEBUG: initializing data race detection"<<endl;
  if(boolOptions["data-race-fail"]||args.count("data-race-csv")) {
    boolOptions.setOption("data-race",true);
  }
  if(boolOptions["data-race"]) {
    cout<<"INFO: ignoring lhs-array accesses"<<endl;
    analyzer.setSkipArrayAccesses(true);
  }
}

bool runDataRaceDetection(Analyzer& analyzer) {
  SAR_MODE sarMode=SAR_SSA;
  Specialization speci;
  ArrayUpdatesSequence arrayUpdates;
  RewriteSystem rewriteSystem;
  bool useConstSubstitutionRule=boolOptions["rule-const-subst"];
  int verifyUpdateSequenceRaceConditionsResult=-1;
  int verifyUpdateSequenceRaceConditionsTotalLoopNum=-1;
  int verifyUpdateSequenceRaceConditionsParLoopNum=-1;

  if(boolOptions["data-race"]) {
    analyzer.setSkipSelectedFunctionCalls(true);
    analyzer.setSkipArrayAccesses(true);
    // perform data race detection
    if (boolOptions["visualize-read-write-sets"]) {
      speci.setVisualizeReadWriteAccesses(true);
    }
    ArrayUpdatesSequence arrayUpdates;
    cout<<"STATUS: performing array analysis on STG."<<endl;
    cout<<"STATUS: identifying array-update operations in STG and transforming them."<<endl;
    //bool useConstSubstitutionRule=boolOptions["rule-const-subst"];
    
    speci.extractArrayUpdateOperations(&analyzer,
                                       arrayUpdates,
                                       rewriteSystem,
                                       useConstSubstitutionRule
                                       );
    speci.substituteArrayRefs(arrayUpdates, analyzer.getVariableIdMapping(), sarMode);

    SgNode* root=analyzer.startFunRoot;
    VariableId parallelIterationVar;
    LoopInfoSet loopInfoSet=EquivalenceChecking::determineLoopInfoSet(root,analyzer.getVariableIdMapping(), analyzer.getLabeler());
    cout<<"INFO: number of iteration vars: "<<loopInfoSet.size()<<endl;
    verifyUpdateSequenceRaceConditionsTotalLoopNum=loopInfoSet.size();
    verifyUpdateSequenceRaceConditionsParLoopNum=Specialization::numParLoops(loopInfoSet, analyzer.getVariableIdMapping());
    verifyUpdateSequenceRaceConditionsResult=speci.verifyUpdateSequenceRaceConditions(loopInfoSet,arrayUpdates,analyzer.getVariableIdMapping());
    if(boolOptions["print-update-infos"]) {
      speci.printUpdateInfos(arrayUpdates,analyzer.getVariableIdMapping());
    }
    speci.createSsaNumbering(arrayUpdates, analyzer.getVariableIdMapping());

    cout << "Data Race Detection:"<<endl;
    stringstream text;
    if(verifyUpdateSequenceRaceConditionsResult==-1) {
      text<<"sequential";
    } else if(verifyUpdateSequenceRaceConditionsResult==0) {
      text<<"pass";
    } else {
        text<<"fail";
    }
    text<<","<<verifyUpdateSequenceRaceConditionsResult;
    text<<","<<verifyUpdateSequenceRaceConditionsParLoopNum;
    text<<","<<verifyUpdateSequenceRaceConditionsTotalLoopNum;
    text<<endl;

    cout << text.str();
    if(args.count("data-race-csv")) {
      string fileName=args["data-race-csv"].as<string>();
      string fileContent=text.str();
      CodeThorn::write_file(fileName,fileContent);
    }
    return true;
  } else {
    return false;
  }
}
