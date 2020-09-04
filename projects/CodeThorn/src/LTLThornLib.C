#include "sage3basic.h"

#include <string>     // std::string, std::stoi
#include <regex>

#include "IOAnalyzer.h"
#include "ReadWriteAnalyzer.h"
#include "CounterexampleGenerator.h"
#include "CodeThornLib.h"
#include "LTLThornLib.h"
#include "Miscellaneous2.h"
#include "CppStdUtilities.h"

#include "ltlthorn-lib/SpotConnection.h"
#include "ltlthorn-lib/CounterexampleAnalyzer.h"
#include "ltlthorn-lib/Solver10.h"
#include "ltlthorn-lib/Solver11.h"
#include "ltlthorn-lib/Solver12.h"
#include "CodeThornException.h"

#include <execinfo.h>
#include <unistd.h>
#include <sys/time.h>
#if defined(__unix__) || defined(__unix) || defined(unix)
#include <sys/resource.h>
#endif

using namespace std;

using namespace CodeThorn;
using namespace boost;

#include "Diagnostics.h"
using namespace Sawyer::Message;


namespace CodeThorn {
  void LtlRersMapping::addInput(char c,int v) {
    _inputMappingCharInt[c]=v;
    _inputMappingIntChar[v]=c;
  }
  void LtlRersMapping::addOutput(char c,int v) {
    _outputMappingCharInt[c]=v;
    _outputMappingIntChar[v]=c;
  }

  void LtlRersMapping::addInputAsciiValueSetWithOffsetA(std::string setString) {
    set<int> valueSet=CodeThorn::Parse::integerSet(setString);
    for(auto val : valueSet) {
      addInput('A'+val-1,val);
    }
  }
  void LtlRersMapping::addOutputAsciiValueSetWithOffsetA(std::string setString) {
    set<int> valueSet=CodeThorn::Parse::integerSet(setString);
    for(auto val : valueSet) {
      addOutput('A'+val-1,val);
    }
  }

  std::set<char> LtlRersMapping::getInputCharSet() {
    std::set<char> set;
    for(auto entry : _inputMappingCharInt) {
      set.insert(entry.first);
    }
    return set;
  }
  std::set<int> LtlRersMapping::getInputValueSet() {
    std::set<int> set;
    for(auto entry : _inputMappingCharInt) {
      set.insert(entry.second);
    }
    return set;
  }
  std::set<char> LtlRersMapping::getOutputCharSet() {
    std::set<char> set;
    for(auto entry : _outputMappingCharInt) {
      set.insert(entry.first);
    }
    return set;
  }
  std::set<int> LtlRersMapping::getOutputValueSet() {
    std::set<int> set;
    for(auto entry : _outputMappingCharInt) {
      set.insert(entry.second);
    }
    return set;
  }
  int LtlRersMapping::getValue(char c) {
    auto iterI=_inputMappingCharInt.find(c);
    if(iterI!=_inputMappingCharInt.end())
      return (*iterI).second;
    auto iterO=_outputMappingCharInt.find(c);
    if(iterO!=_outputMappingCharInt.end())
      return (*iterO).second;
    throw CodeThorn::Exception(string("LtlRersMapping::getValue unknown char: ")+c);
  }
  char LtlRersMapping::getChar(int value) {
    auto iterI=_inputMappingIntChar.find(value);
    if(iterI!=_inputMappingIntChar.end())
      return (*iterI).second;
    auto iterO=_outputMappingIntChar.find(value);
    if(iterO!=_outputMappingIntChar.end())
      return (*iterO).second;
    stringstream ss;
    ss<<value;
    throw CodeThorn::Exception(string("LtlRersMapping::getChar unknown value: ")+ss.str());
  }
  std::string LtlRersMapping::getIOString(int value) {
    auto iterI=_inputMappingIntChar.find(value);
    if(iterI!=_inputMappingIntChar.end())
      return string("i")+(*iterI).second;
    auto iterO=_outputMappingIntChar.find(value);
    if(iterO!=_outputMappingIntChar.end())
      return string("o")+(*iterO).second;
    stringstream ss;
    ss<<value;
    throw CodeThorn::Exception(string("LtlRersMapping::getIOString unknown value: ")+ss.str());
  }
  bool LtlRersMapping::isInput(char c) {
    return _inputMappingCharInt.find(c)!=_inputMappingCharInt.end();
  }
  bool LtlRersMapping::isInput(int value) {
    return _inputMappingIntChar.find(value)!=_inputMappingIntChar.end();
  }
  bool LtlRersMapping::isOutput(char c) {
    return _outputMappingCharInt.find(c)!=_inputMappingCharInt.end();
  }
  bool LtlRersMapping::isOutput(int value) {
    return _outputMappingIntChar.find(value)!=_inputMappingIntChar.end();
  }
}

void CodeThorn::initDiagnosticsLTL() {
  IOAnalyzer::initDiagnostics();
  ReadWriteAnalyzer::initDiagnostics();
  CounterexampleGenerator::initDiagnostics();
}

bool CodeThorn::readAndParseLTLRersMappingFile(string ltlRersMappingFileName, CodeThorn::LtlRersMapping& rersLtlMapping) {
  CppStdUtilities::DataFileVector dataFileVector;
  bool readStatus=CppStdUtilities::readDataFile(ltlRersMappingFileName,dataFileVector);
  if(readStatus==false)
    return readStatus;
  int lineNr=1;
  for(std::string line : dataFileVector) {
    std::vector<std::string> lineEntries=CppStdUtilities::splitByRegex(line,",|\\t|\\s+");
    if(lineEntries.size()!=2) {
      cerr<<"Error: format error in rers mapping file. Not exactly two entries in line "<<lineNr<<endl;
      exit(1);
    }
    string ioString=lineEntries[0];
    int value=std::stoi(lineEntries[1]);
    cout<<"DEBUG: Parsing mapping file: line "<<lineNr<<": "<<ioString<<" <=> "<<value<<endl;
    if(ioString.size()==2&&ioString[0]=='i') {
      rersLtlMapping.addInput(ioString[1],value);
    } else if(ioString.size()==2&&ioString[0]=='o') {
      rersLtlMapping.addOutput(ioString[1],value);
    } else {
      cout<<"WARNING: unknown entry in rers mapping file line "<<lineNr<<": "<<ioString<<" (ignoring it)"<<endl;
    }
    lineNr++;
  }
  cout<<"DEBUG:";CppStdUtilities::printContainer(rersLtlMapping.getInputCharSet(),",",cout);
  cout<<"DEBUG:";CppStdUtilities::printContainer(rersLtlMapping.getInputValueSet(),",",cout);
  cout<<"DEBUG:";CppStdUtilities::printContainer(rersLtlMapping.getOutputCharSet(),",",cout);
  cout<<"DEBUG:";CppStdUtilities::printContainer(rersLtlMapping.getOutputValueSet(),",",cout);
  return true;
}

void CodeThorn::runLTLAnalysis(CodeThornOptions& ctOpt, LTLOptions& ltlOpt,IOAnalyzer* analyzer, TimingCollector& tc) {
  long pstateSetSize=analyzer->getPStateSet()->size();
  long pstateSetBytes=analyzer->getPStateSet()->memorySize();
  long pstateSetMaxCollisions=analyzer->getPStateSet()->maxCollisions();
  long pstateSetLoadFactor=analyzer->getPStateSet()->loadFactor();
  long eStateSetSize=analyzer->getEStateSet()->size();
  long eStateSetBytes=analyzer->getEStateSet()->memorySize();
  long eStateSetMaxCollisions=analyzer->getEStateSet()->maxCollisions();
  double eStateSetLoadFactor=analyzer->getEStateSet()->loadFactor();
  long transitionGraphSize=analyzer->getTransitionGraph()->size();
  long transitionGraphBytes=transitionGraphSize*sizeof(Transition);
  long numOfconstraintSets=analyzer->getConstraintSetMaintainer()->numberOf();
  long constraintSetsBytes=analyzer->getConstraintSetMaintainer()->memorySize();
  long constraintSetsMaxCollisions=analyzer->getConstraintSetMaintainer()->maxCollisions();
  double constraintSetsLoadFactor=analyzer->getConstraintSetMaintainer()->loadFactor();
  long numOfStdinEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDIN_VAR));
  long numOfStdoutVarEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_VAR));
  long numOfStdoutConstEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDOUT_CONST));
  long numOfStderrEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::STDERR_VAR));
  long numOfFailedAssertEStates=(analyzer->getEStateSet()->numberOfIoTypeEStates(InputOutput::FAILED_ASSERT));
  long numOfConstEStates=0;//(analyzer->getEStateSet()->numberOfConstEStates(analyzer->getVariableIdMapping()));
  long numOfStdoutEStates=numOfStdoutVarEStates+numOfStdoutConstEStates;

#if defined(__unix__) || defined(__unix) || defined(unix)
  // Unix-specific solution to finding the peak phyisical memory consumption (rss).
  // Not necessarily supported by every OS.
  struct rusage resourceUsage;
  getrusage(RUSAGE_SELF, &resourceUsage);
  long totalMemory=resourceUsage.ru_maxrss * 1024;
#else
  long totalMemory=pstateSetBytes+eStateSetBytes+transitionGraphBytes+constraintSetsBytes;
#endif

  double totalRunTime=tc.frontEndRunTime+tc.initRunTime+tc.analysisRunTime;

  long pstateSetSizeInf = 0;
  long eStateSetSizeInf = 0;
  long transitionGraphSizeInf = 0;
  long eStateSetSizeStgInf = 0;
  double infPathsOnlyTime = 0;
  double stdIoOnlyTime = 0;

 
  TimeMeasurement& timer=tc.timer;
  if(ltlOpt.stdIOOnly) {
    SAWYER_MESG(logger[TRACE]) << "STATUS: bypassing all non standard I/O states. (P2)"<<endl;
    timer.start();
    if (ltlOpt.keepErrorStates) {
      analyzer->reduceStgToInOutAssertStates();
    } else {
      analyzer->reduceStgToInOutStates();
    }
    if(ltlOpt.inifinitePathsOnly) {
      analyzer->pruneLeaves();
    }
    stdIoOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
  } else {
    if(ltlOpt.inifinitePathsOnly) {
      assert (!ltlOpt.keepErrorStates);
      cout << "recursively removing all leaves (1)."<<endl;
      timer.start();
      analyzer->pruneLeaves();
      infPathsOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
      pstateSetSizeInf=analyzer->getPStateSet()->size();
      eStateSetSizeInf = analyzer->getEStateSet()->size();
      transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
      eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
    }
  }

  long eStateSetSizeIoOnly = 0;
  long transitionGraphSizeIoOnly = 0;
  double spotLtlAnalysisTime = 0;

  stringstream statisticsSizeAndLtl;
  stringstream statisticsCegpra;

  if (ltlOpt.ltlFormulaeFile.size()>0) {
    logger[INFO] <<"STG size: "<<analyzer->getTransitionGraph()->size()<<endl;
    string ltl_filename = ltlOpt.ltlFormulaeFile;
    if(ctOpt.rers.rersMode) {  //reduce the graph accordingly, if not already done
      if (!ltlOpt.inifinitePathsOnly
          && !ltlOpt.keepErrorStates
          &&!analyzer->getModeLTLDriven()) {
        cout<< "STATUS: recursively removing all leaves (due to RERS-mode (2))."<<endl;
        timer.start();
        analyzer->pruneLeaves();
        infPathsOnlyTime = timer.getTimeDurationAndStop().milliSeconds();

        pstateSetSizeInf=analyzer->getPStateSet()->size();
        eStateSetSizeInf = analyzer->getEStateSet()->size();
        transitionGraphSizeInf = analyzer->getTransitionGraph()->size();
        eStateSetSizeStgInf = (analyzer->getTransitionGraph())->estateSet().size();
      }
      if (!ltlOpt.stdIOOnly &&!analyzer->getModeLTLDriven()) {
        cout << "STATUS: bypassing all non standard I/O states (due to RERS-mode) (P1)."<<endl;
        timer.start();
        analyzer->getTransitionGraph()->printStgSize("before reducing non-I/O states");
        if (ltlOpt.keepErrorStates) {
          analyzer->reduceStgToInOutAssertStates();
        } else {
          analyzer->reduceStgToInOutStates();
        }
        stdIoOnlyTime = timer.getTimeDurationAndStop().milliSeconds();
        analyzer->getTransitionGraph()->printStgSize("after reducing non-I/O states");
      }
    }
    if(ltlOpt.noInputInputTransitions) {  //delete transitions that indicate two input states without an output in between
      analyzer->removeInputInputTransitions();
      analyzer->getTransitionGraph()->printStgSize("after reducing input->input transitions");
    }
    bool withCounterexample = false;
    if(ltlOpt.withCounterExamples || ltlOpt.withLTLCounterExamples) {  //output a counter-example input sequence for falsified formulae
      withCounterexample = true;
    }

    timer.start();
    LtlRersMapping ltlRersMapping;
    std::set<int> ltlInAlphabet;// = analyzer->getInputVarValues();
    //take fixed ltl input alphabet if specified, instead of the input values used for stg computation
    if (ltlOpt.ltlInAlphabet.size()>0) {
      ltlRersMapping.addInputAsciiValueSetWithOffsetA(ltlOpt.ltlInAlphabet);
    }
    std::set<int> ltlOutAlphabet;
    if (ltlOpt.ltlOutAlphabet.size()>0) {
      ltlRersMapping.addOutputAsciiValueSetWithOffsetA(ltlOpt.ltlOutAlphabet);
    }
    if(ltlOpt.ltlRersMappingFileName.size()>0) {
      // load and parse file into ltlInAlphabet and ltlOutAlphabet
      // input/output alphabet
      if(!readAndParseLTLRersMappingFile(ltlOpt.ltlRersMappingFileName,ltlRersMapping)) {
        cerr<<"Error: could not open RERS mapping file "<<ltlOpt.ltlRersMappingFileName<<endl;
        exit(1);
      }
      // set input/output alphabets here as well
    }
    ltlInAlphabet=ltlRersMapping.getInputValueSet();
    ltlOutAlphabet=ltlRersMapping.getOutputValueSet();
    if(ltlInAlphabet.size()==0) {
      cerr<<"Error: no LTL input alphabet provided."<<endl;
      exit(1);
    }
    if(ltlOutAlphabet.size()==0) {
      cerr<<"Error: no LTL output alphabet provided."<<endl;
      exit(1);
    }

    PropertyValueTable* ltlResults=nullptr;
    SpotConnection spotConnection(ltl_filename);
    spotConnection.setModeLTLDriven(analyzer->getModeLTLDriven());
    if (analyzer->getModeLTLDriven()) {
      analyzer->setSpotConnection(&spotConnection);
    }

    SAWYER_MESG(logger[TRACE]) << "STATUS: generating LTL results"<<endl;
    bool spuriousNoAnswers = false;
    SAWYER_MESG(logger[TRACE]) << "LTL: check properties."<<endl;
    if (ltlOpt.propertyNrToCheck!=-1) {
      int propertyNum = ltlOpt.propertyNrToCheck;
      spotConnection.checkSingleProperty(propertyNum, *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
    } else {
      spotConnection.checkLtlProperties( *(analyzer->getTransitionGraph()), ltlInAlphabet, ltlOutAlphabet, withCounterexample, spuriousNoAnswers);
    }
    spotLtlAnalysisTime=timer.getTimeDurationAndStop().milliSeconds();
    SAWYER_MESG(logger[TRACE]) << "LTL: get results from spot connection."<<endl;
    ltlResults = spotConnection.getLtlResults();
    SAWYER_MESG(logger[TRACE]) << "LTL: results computed."<<endl;

    if (ltlOpt.cegpra.ltlPropertyNr!=-1 || ltlOpt.cegpra.checkAllProperties) {
      if (ltlOpt.cegpra.csvStatsFileName.size()>0) {
        statisticsCegpra << "init,";
        analyzer->getTransitionGraph()->printStgSize("initial abstract model");
        analyzer->getTransitionGraph()->csvToStream(statisticsCegpra);
        statisticsCegpra << ",na,na";
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
        statisticsCegpra << "," << ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
      }
      CounterexampleAnalyzer ceAnalyzer(analyzer, &statisticsCegpra);
      if (ltlOpt.cegpra.maxIterations!=-1) {
        ceAnalyzer.setMaxCounterexamples(ltlOpt.cegpra.maxIterations);
      }
      if (ltlOpt.cegpra.checkAllProperties) {
        ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(spotConnection, ltlInAlphabet, ltlOutAlphabet);
      } else {
        // cegpra for single LTL property
        //ROSE_ASSERT(ltlOpt.cegpra.ltlPropertyNr!=-1);
        int property = ltlOpt.cegpra.ltlPropertyNr;
        ltlResults = ceAnalyzer.cegarPrefixAnalysisForLtl(property, spotConnection, ltlInAlphabet, ltlOutAlphabet);
      }
    }

    if(ctOpt.status) {
      ltlResults-> printResults("YES (verified)", "NO (falsified)", "ltl_property_", withCounterexample);
      analyzer->printStatusMessageLine("==============================================================");
      ltlResults->printResultsStatistics();
      analyzer->printStatusMessageLine("==============================================================");
    }
    if (ltlOpt.spotVerificationResultsCSVFileName.size()>0) {  //write results to a file instead of displaying them directly
      std::string csv_filename = ltlOpt.spotVerificationResultsCSVFileName;
      SAWYER_MESG(logger[TRACE]) << "STATUS: writing ltl results to file: " << csv_filename << endl;
      ltlResults->writeFile(csv_filename.c_str(), false, 0, withCounterexample);
    }
    if (ltlOpt.ltlStatisticsCSVFileName.size()>0) {
      analyzer->getTransitionGraph()->printStgSize("final model");
      analyzer->getTransitionGraph()->csvToStream(statisticsSizeAndLtl);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_YES);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_NO);
      statisticsSizeAndLtl <<","<< ltlResults->entriesWithValue(PROPERTY_VALUE_UNKNOWN);
    }
#if 0
    if(ltlResults) {
      delete ltlResults;
      ltlResults = NULL;
    }
#endif
    //temporaryTotalRunTime = totalRunTime + infPathsOnlyTime + stdIoOnlyTime + spotLtlAnalysisTime;
    //printAnalyzerStatistics(analyzer, temporaryTotalRunTime, "LTL check complete. Reduced transition system:");
  }
  double totalLtlRunTime =  infPathsOnlyTime + stdIoOnlyTime + spotLtlAnalysisTime;

  double arrayUpdateExtractionRunTime=0.0;
  double arrayUpdateSsaNumberingRunTime=0.0;
  double sortingAndIORunTime=0.0;
  double verifyUpdateSequenceRaceConditionRunTime=0.0;

  int verifyUpdateSequenceRaceConditionsResult=-1;
  int verifyUpdateSequenceRaceConditionsTotalLoopNum=-1;
  int verifyUpdateSequenceRaceConditionsParLoopNum=-1;

  /* Data race detection */
  optionallyRunDataRaceDetection(ctOpt,analyzer);

  double overallTime=totalRunTime + tc.totalInputTracesTime + totalLtlRunTime;
  analyzer->printAnalyzerStatistics(totalRunTime, "STG generation and assertion analysis complete");

  if(ctOpt.csvStatsFileName.size()>0) {
    string filename=ctOpt.csvStatsFileName;
    stringstream text;
    text<<"Sizes,"<<pstateSetSize<<", "
        <<eStateSetSize<<", "
        <<transitionGraphSize<<", "
        <<numOfconstraintSets<<", "
        << numOfStdinEStates<<", "
        << numOfStdoutEStates<<", "
        << numOfStderrEStates<<", "
        << numOfFailedAssertEStates<<", "
        << numOfConstEStates<<endl;
    text<<"Memory,"<<pstateSetBytes<<", "
        <<eStateSetBytes<<", "
        <<transitionGraphBytes<<", "
        <<constraintSetsBytes<<", "
        <<totalMemory<<endl;
    text<<"Runtime(readable),"
        <<CodeThorn::readableruntime(tc.frontEndRunTime)<<", "
        <<CodeThorn::readableruntime(tc.initRunTime)<<", "
        <<CodeThorn::readableruntime(tc.normalizationRunTime)<<", "
        <<CodeThorn::readableruntime(tc.analysisRunTime)<<", "
        <<CodeThorn::readableruntime(verifyUpdateSequenceRaceConditionRunTime)<<", "
        <<CodeThorn::readableruntime(arrayUpdateExtractionRunTime)<<", "
        <<CodeThorn::readableruntime(arrayUpdateSsaNumberingRunTime)<<", "
        <<CodeThorn::readableruntime(sortingAndIORunTime)<<", "
        <<CodeThorn::readableruntime(totalRunTime)<<", "
        <<CodeThorn::readableruntime(tc.extractAssertionTracesTime)<<", "
        <<CodeThorn::readableruntime(tc.determinePrefixDepthTime)<<", "
        <<CodeThorn::readableruntime(tc.totalInputTracesTime)<<", "
        <<CodeThorn::readableruntime(infPathsOnlyTime)<<", "
        <<CodeThorn::readableruntime(stdIoOnlyTime)<<", "
        <<CodeThorn::readableruntime(spotLtlAnalysisTime)<<", "
        <<CodeThorn::readableruntime(totalLtlRunTime)<<", "
        <<CodeThorn::readableruntime(overallTime)<<endl;
    text<<"Runtime(ms),"
        <<tc.frontEndRunTime<<", "
        <<tc.initRunTime<<", "
        <<tc.normalizationRunTime<<", "
        <<tc.analysisRunTime<<", "
        <<verifyUpdateSequenceRaceConditionRunTime<<", "
        <<arrayUpdateExtractionRunTime<<", "
        <<arrayUpdateSsaNumberingRunTime<<", "
        <<sortingAndIORunTime<<", "
        <<totalRunTime<<", "
        <<tc.extractAssertionTracesTime<<", "
        <<tc.determinePrefixDepthTime<<", "
        <<tc.totalInputTracesTime<<", "
        <<infPathsOnlyTime<<", "
        <<stdIoOnlyTime<<", "
        <<spotLtlAnalysisTime<<", "
        <<totalLtlRunTime<<", "
        <<overallTime<<endl;
    text<<"hashset-collisions,"
        <<pstateSetMaxCollisions<<", "
        <<eStateSetMaxCollisions<<", "
        <<constraintSetsMaxCollisions<<endl;
    text<<"hashset-loadfactors,"
        <<pstateSetLoadFactor<<", "
        <<eStateSetLoadFactor<<", "
        <<constraintSetsLoadFactor<<endl;
    text<<"threads,"<<analyzer->getNumberOfThreadsToUse()<<endl;
    //    text<<"abstract-and-const-states,"
    //    <<"";

    // iterations (currently only supported for sequential analysis)
    text<<"iterations,";
    if(analyzer->getNumberOfThreadsToUse()==1 && analyzer->getSolver()->getId()==5 && analyzer->getExplorationMode()==EXPL_LOOP_AWARE)
      text<<analyzer->getIterations()<<","<<analyzer->getApproximatedIterations();
    else
      text<<"-1,-1";
    text<<endl;

    // -1: test not performed, 0 (no race conditions), >0: race conditions exist
    text<<"parallelism-stats,";
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

    text<<"infinite-paths-size,"<<pstateSetSizeInf<<", "
        <<eStateSetSizeInf<<", "
        <<transitionGraphSizeInf<<", "
        <<eStateSetSizeStgInf<<endl;
    //<<numOfconstraintSetsInf<<", "
    //<< numOfStdinEStatesInf<<", "
    //<< numOfStdoutEStatesInf<<", "
    //<< numOfStderrEStatesInf<<", "
    //<< numOfFailedAssertEStatesInf<<", "
    //<< numOfConstEStatesInf<<endl;
    text<<"states & transitions after only-I/O-reduction,"
        <<eStateSetSizeIoOnly<<", "
        <<transitionGraphSizeIoOnly<<endl;
    //  text<<"input length coverage"
    // <<inputSeqLengthCovered<<endl; // MS 2020: this value is not computed

    write_file(filename,text.str());
    cout << "generated "<<filename<<endl;
  }

  if(ltlOpt.ltlStatisticsCSVFileName.size()>0) {
    // content of a line in the .csv file:
    // <#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
    string filename = ltlOpt.ltlStatisticsCSVFileName;
    write_file(filename,statisticsSizeAndLtl.str());
    cout << "generated "<<filename<<endl;
  }

  if (ltlOpt.cegpra.csvStatsFileName.size()>0) {
    // content of a line in the .csv file:
    // <analyzed_property>,<#transitions>,<#states>,<#input_states>,<#output_states>,<#error_states>,
    // <#analyzed_counterexamples>,<analysis_result(y/n/?)>,<#verified_LTL>,<#falsified_LTL>,<#unknown_LTL>
    string filename = ltlOpt.cegpra.csvStatsFileName;
    write_file(filename,statisticsCegpra.str());
    cout << "generated "<<filename<<endl;
  }
}
void CodeThorn::optionallyInitializePatternSearchSolver(CodeThornOptions& ctOpt,IOAnalyzer* analyzer,TimingCollector& timingCollector) {
    // pattern search: requires that exploration mode is set,
    // otherwise no pattern search is performed
    if(ctOpt.patSearch.explorationMode.size()>0) {
      logger[INFO] << "Pattern search exploration mode was set. Choosing solver 10." << endl;
      analyzer->setSolver(new Solver10());
      analyzer->setStartPState(*analyzer->popWorkList()->pstate());
    }
}
