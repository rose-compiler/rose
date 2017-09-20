/* data race detection - refactoring in progress */

#include "sage3basic.h"
#include "DataRaceDetection.h"
#include "Specialization.h"
#include "EquivalenceChecking.h"

using namespace CodeThorn;

DataRaceDetection::DataRaceDetection() {
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
}

void DataRaceDetection::setCsvFileName(string fileName) {
  options.dataRaceCsvFileName=fileName;
}

bool DataRaceDetection::run(Analyzer& analyzer) {
  if(options.active) {
    SAR_MODE sarMode=SAR_SSA;
    Specialization speci;
    ArrayUpdatesSequence arrayUpdates;
    RewriteSystem rewriteSystem;   
    int verifyUpdateSequenceRaceConditionsResult=-1;
    int verifyUpdateSequenceRaceConditionsTotalLoopNum=-1;
    int verifyUpdateSequenceRaceConditionsParLoopNum=-1;

    analyzer.setSkipSelectedFunctionCalls(true);
    analyzer.setSkipArrayAccesses(true);

    // perform data race detection
    if (options.visualizeReadWriteSets) {
      speci.setVisualizeReadWriteAccesses(true);
    }
    cout<<"STATUS: performing array analysis on STG."<<endl;
    cout<<"STATUS: identifying array-update operations in STG and transforming them."<<endl;
    
    speci.extractArrayUpdateOperations(&analyzer,
                                       arrayUpdates,
                                       rewriteSystem,
                                       options.useConstSubstitutionRule
                                       );
    speci.substituteArrayRefs(arrayUpdates, analyzer.getVariableIdMapping(), sarMode, rewriteSystem);

    SgNode* root=analyzer.startFunRoot;
    VariableId parallelIterationVar;
    LoopInfoSet loopInfoSet=EquivalenceChecking::determineLoopInfoSet(root,analyzer.getVariableIdMapping(), analyzer.getLabeler());
    cout<<"INFO: number of iteration vars: "<<loopInfoSet.size()<<endl;
    verifyUpdateSequenceRaceConditionsTotalLoopNum=loopInfoSet.size();
    verifyUpdateSequenceRaceConditionsParLoopNum=Specialization::numParLoops(loopInfoSet, analyzer.getVariableIdMapping());
    verifyUpdateSequenceRaceConditionsResult=speci.checkDataRaces(loopInfoSet,arrayUpdates,analyzer.getVariableIdMapping());
    if(options.printUpdateInfos) {
      speci.printUpdateInfos(arrayUpdates,analyzer.getVariableIdMapping());
    }
    speci.createSsaNumbering(arrayUpdates, analyzer.getVariableIdMapping());

    stringstream text;
    if(verifyUpdateSequenceRaceConditionsResult==-1) {
      text<<"sequential";
    } else 
      // not supported yet
      if(verifyUpdateSequenceRaceConditionsResult==-2) {
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
      // if no output file is proved print on std out
      cout << "Data Race Detection: ";
      cout << text.str();
    }

    return true;
  } else {
    return false;
  }
}
