// Author: Marc Jasper, 2016.

#include "ParProExplorer.h"
#include "rose_config.h"

#include "LtsminConnection.h"
#include "ParProAutomataGenerator.h"


using namespace CodeThorn;
using namespace CodeThorn;
using namespace std;

ParProExplorer::ParProExplorer(vector<Flow*>& cfas, EdgeAnnotationMap& annotationMap):
_parProLtlMiner(ParProLtlMiner(this)),
_cfas(cfas),
_annotationMap(annotationMap),
_properties(new PropertyValueTable()),
_numVerified(0),
_numFalsified(0),
_componentSelection(PAR_PRO_COMPONENTS_ALL),
_randomSubsetMode(PAR_PRO_NUM_SUBSETS_NONE),
_numDifferentSubsets(-1),
_numRandomComponents(3),
_ltlMode(PAR_PRO_LTL_MODE_NONE),
_includeLtlResults(false),
_miningsPerSubsystem(50),
_minNumComponents(3),
_numRequiredVerifiable(10),
_numRequiredFalsifiable(10),
_numberOfThreadsToUse(1), 
_visualize(false),
_useLtsMin(false),
_parallelCompositionOnly(false) {
}

void ParProExplorer::setFixedComponentSubsets(list<set<int> > fixedSubsets) {
     _fixedComponentSubsets = fixedSubsets;
     _currentFixedSubset = _fixedComponentSubsets.begin();
}

PropertyValueTable* ParProExplorer::ltlAnalysis(ParallelSystem system) {
  PropertyValueTable* result = NULL;
  if (_ltlMode == PAR_PRO_LTL_MODE_NONE) {
    return new PropertyValueTable();  // return an empty table because no LTL analysis was selected
  }  
  if (_ltlMode == PAR_PRO_LTL_MODE_CHECK) {
    if(_useLtsMin) {
      LtsminConnection ltsminConnection(_annotationMap, _ltlInputFilename);
      return ltsminConnection.checkLtlPropertiesParPro(system.components());
    } else {
      SpotConnection spotConnection(_ltlInputFilename);
      bool withCounterexample = false;
      bool spuriousNoAnswers = false;
      if (system.hasStg()) {
	if (_visualize) {
#if HAVE_SPOT
	  ParProSpotTgba* spotTgba = spotConnection.toTgba(*(system.stg()));
	  Visualizer visualizer;
	  string dotTgba = visualizer.spotTgbaToDot(*spotTgba);
	  delete spotTgba;
	  spotTgba = NULL;
	  string outputFilename = "spotTgba_no_approx.dot";
	  write_file(outputFilename, dotTgba);
	  cout << "generated " << outputFilename <<"."<<endl;
#endif
	}  
	spotConnection.checkLtlPropertiesParPro(*(system.stg()), withCounterexample, spuriousNoAnswers, system.getAnnotations());
	result = spotConnection.getLtlResults();
      } else {
	if (system.hasStgOverApprox()) {
	  if (_visualize) {
#if HAVE_SPOT
	    ParProSpotTgba* spotTgba = spotConnection.toTgba(*(system.stgOverApprox()));
	    Visualizer visualizer;
	    string dotTgba = visualizer.spotTgbaToDot(*spotTgba);
	    delete spotTgba;
	    spotTgba = NULL;
	    string outputFilename = "spotTgba_over_approx.dot";
	    write_file(outputFilename, dotTgba);
	    cout << "generated " << outputFilename <<"."<<endl;
#endif
	  }  
	  spotConnection.checkLtlPropertiesParPro(*(system.stgOverApprox()), withCounterexample, spuriousNoAnswers, system.getAnnotations());
	}
	if (system.hasStgUnderApprox()) {
	  if (_visualize) {
#if HAVE_SPOT
	    ParProSpotTgba* spotTgba = spotConnection.toTgba(*(system.stgUnderApprox()));
	    Visualizer visualizer;
	    string dotTgba = visualizer.spotTgbaToDot(*spotTgba);
	    delete spotTgba;
	    spotTgba = NULL;
	    string outputFilename = "spotTgba_under_approx.dot";
	    write_file(outputFilename, dotTgba);
	    cout << "generated " << outputFilename <<"."<<endl;
#endif
	  }
	  spotConnection.checkLtlPropertiesParPro(*(system.stgUnderApprox()), withCounterexample, spuriousNoAnswers, system.getAnnotations());
	}
      }
      result = spotConnection.getLtlResults();
    }
  } else if (_ltlMode == PAR_PRO_LTL_MODE_MINE) {
    if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_NONE) {
      if (_useLtsMin) {
	result = _parProLtlMiner.minePropertiesLtsMin(system, _minNumComponents, _numRequiredVerifiable, _numRequiredFalsifiable);
      } else {
	cerr << "ERROR: ltl mining on the entire parallel system is currently only supported based on the LTSmin backend." << endl;
	ROSE_ASSERT(0);
      }
    } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_FINITE) {
      result = _parProLtlMiner.mineProperties(system, _minNumComponents);
    } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_INFINITE) {
      int yetToVerify = _numRequiredVerifiable - _numVerified;
      int yetToFalsify = _numRequiredFalsifiable - _numFalsified;
      ROSE_ASSERT(yetToFalsify >= 0 && yetToVerify >= 0);
      if (_useLtsMin) {
	result = _parProLtlMiner.minePropertiesLtsMin(system, _minNumComponents, yetToVerify, yetToFalsify);
      } else {
	result = _parProLtlMiner.mineProperties(system, _minNumComponents, yetToVerify, yetToFalsify);
      }
    }
  }
  return result;
}

void ParProExplorer::explore() {

  /*
  if(_useLtsMin) {
    ParallelSystem system = exploreOnce();
    LtsminConnection ltsMinConnection;
    string testProperty = "( (((action == \"c0_t7\") -> (!(action == \"c0_t1\") W (action == \"c0_t0__c1_t2\"))) W (action == \"c1_t3\")) )";
    //string testProperty = "( (!((action == \"c0_t9\") || (action == \"c1_t3\")) W (action == \"c1_t4\")) )";
    ltsMinConnection.checkPropertyParPro(testProperty, system.components());
    exit(0);
  }
  */
  vector<Flow*> dotGraphs;
  NumberGenerator dotGraphStateNumbers(1);
  if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_INFINITE) {
    if (_ltlMode != PAR_PRO_LTL_MODE_MINE) {
      cout << "ERROR: An unlimited number of analyses using random subsets has been selected, but LTLs are not generated (choose mode --ltl-mode=mine)."<<endl;
      ROSE_ASSERT(0);
    }
    long counter = 0;
    long nextReportedCount = 10000;
    while(_numVerified < _numRequiredVerifiable || _numFalsified < _numRequiredFalsifiable) {
      if ((_miningsPerSubsystem * counter) >= nextReportedCount) {
	cout << "STATUS: " << (_miningsPerSubsystem * counter) << " LTLs tried" << endl; 
	nextReportedCount += 10000;
      }
      ParallelSystem system = exploreOnce();
      PropertyValueTable* intermediateResult = ltlAnalysis(system);
      _properties->append( *intermediateResult );
      if (!_storeComputedSystems) {
	system.deleteStgs();
      }
      delete intermediateResult;
      int numVerifiedOld = _numVerified;
      int numFalsifiedOld = _numFalsified;
      recalculateNumVerifiedFalsified();
      if (_numVerified != numVerifiedOld || _numFalsified != numFalsifiedOld) {
	cout << "STATUS: verifiable: "<<_numVerified<<"   falsified: "<<_numFalsified << endl;
      }
      ++counter;
    }
    _properties->shuffle();
  } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_FINITE) {
    if (_componentSelection == PAR_PRO_COMPONENTS_SUBSET_FIXED) {
      while (_currentFixedSubset != _fixedComponentSubsets.end()) {
	ParallelSystem system = exploreOnce();
	if (_visualize) {
	  addToVisOutput(system, dotGraphs, dotGraphStateNumbers);
	}
	if (!_parallelCompositionOnly) {
	  _properties->append( *(ltlAnalysis(system)) );
	}
	++_currentFixedSubset;
      }
    } else {
      for (int i = 0; i < _numDifferentSubsets; i++) {
	ParallelSystem system = exploreOnce();
	if (_visualize) {
	  addToVisOutput(system, dotGraphs, dotGraphStateNumbers);
	}
	if (!_parallelCompositionOnly) {	
	  PropertyValueTable* intermediateResult = ltlAnalysis(system);
	  _properties->append( *intermediateResult );
	  delete intermediateResult;	
	}
	if (!_storeComputedSystems) {
	  system.deleteStgs();
	}
	recalculateNumVerifiedFalsified();
	if ( _ltlMode == PAR_PRO_LTL_MODE_MINE) {
	  cout << "STATUS: verifiable: "<<_numVerified<<"   falsified: "<<_numFalsified << endl;
	}
	if ( _ltlMode == PAR_PRO_LTL_MODE_MINE
	     && (_numVerified >= _numRequiredVerifiable && _numFalsified >= _numRequiredFalsifiable) ) {
	  break;
	}
      }
      _properties->shuffle();
    }
  } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_NONE) {
    ParallelSystem system = exploreOnce();
    _properties->append( *(ltlAnalysis(system)) );
    if (_visualize) {
      addToVisOutput(system, dotGraphs, dotGraphStateNumbers);
    }
  }
  if (_visualize) {
    Visualizer visualizer;
    string dotFlow = visualizer.cfasToDotSubgraphs(dotGraphs);
    string outputFilename = "all_analyzed_systems_enumerated_states.dot";
    write_file(outputFilename, dotFlow);
    cout << "generated " << outputFilename <<"."<<endl;
  }
}

PropertyValueTable* ParProExplorer::propertyValueTable() {
  return _properties;
}

ParallelSystem ParProExplorer::exploreOnce() {
  ParallelSystem system;
  if (_componentSelection == PAR_PRO_COMPONENTS_ALL) {
    int currentId = 0;
    for (vector<Flow*>::iterator i=_cfas.begin(); i!=_cfas.end(); ++i) {
      system.addComponent(currentId, *i);
      currentId++;
    }
    if(!_useLtsMin) {
	computeStgApprox(system, COMPONENTS_NO_APPROX);
      if (_parallelCompositionOnly) {
	cout << "STATUS: " << system.stg()->numStates() << " distinct states exist in the parallel composition." << endl;
      } 
    }
  } else if (_componentSelection == PAR_PRO_COMPONENTS_SUBSET_FIXED) {
    ROSE_ASSERT(_currentFixedSubset != _fixedComponentSubsets.end());
    set<int> fixedSubset = *_currentFixedSubset;
    for (set<int>::iterator i=fixedSubset.begin(); i!=fixedSubset.end(); i++) {
      system.addComponent(*i, _cfas[*i]);
    }
    if(!_useLtsMin) {
      if (_parallelCompositionOnly || _cfas.size() == (unsigned) _numRandomComponents) {
	computeStgApprox(system, COMPONENTS_NO_APPROX);
	cout << "STATUS: " << system.stg()->numStates() << " distinct states exist in the parallel composition." << endl;
      } else {
	computeStgApprox(system, COMPONENTS_OVER_APPROX);
	computeStgApprox(system, COMPONENTS_UNDER_APPROX);
      }
    }
  } else if (_componentSelection == PAR_PRO_COMPONENTS_SUBSET_RANDOM) {
    set<int> randomIds = randomSetNonNegativeInts(_numRandomComponents, (((int)_cfas.size()) - 1)); 
    for (set<int>::iterator i=randomIds.begin(); i!=randomIds.end(); i++) {
      system.addComponent(*i, _cfas[*i]);
    }
    if(!_useLtsMin) {
      if (_parallelCompositionOnly || _cfas.size() == (unsigned) _numRandomComponents) {
	computeStgApprox(system, COMPONENTS_NO_APPROX);
      } else {
	computeStgApprox(system, COMPONENTS_OVER_APPROX);
	computeStgApprox(system, COMPONENTS_UNDER_APPROX);
      }
    }
  }
  if (_visualize) {
    if (system.hasStg()) {
      string dotStg = system.stg()->toDot();
      string outputFilename = "stgParallelProgram_no_approx.dot";
      write_file(outputFilename, dotStg);
      cout << "generated " << outputFilename <<"."<<endl;
    }
    if (system.hasStgOverApprox()) {
      string dotStg = system.stgOverApprox()->toDot();
      string outputFilename = "stgParallelProgram_over_approx.dot";
      write_file(outputFilename, dotStg);
      cout << "generated " << outputFilename <<"."<<endl;
    }
    if (system.hasStgUnderApprox()) {
      string dotStg = system.stgUnderApprox()->toDot();
      string outputFilename = "stgParallelProgram_under_approx.dot";
      write_file(outputFilename, dotStg);
      cout << "generated " << outputFilename <<"."<<endl;
    }
  }
  return system;
}

set<int> ParProExplorer::randomSetNonNegativeInts(int size, int maxInt) {
  set<int> result;
  for (int i = 0; i < size; i++) {
    int val = rand() % (maxInt + 1);
    //draw a new number in case the randomly selected one has been drawn already
    while (result.find(val) != result.end()) {
      val = rand() % (maxInt + 1);
    }
    result.insert(val);
  }
  return result;
}

void ParProExplorer::computeStgApprox(ParallelSystem& system, ComponentApproximation approxMode) {
  if (approxMode == COMPONENTS_OVER_APPROX) {
    ROSE_ASSERT(!system.hasStgOverApprox());
  } else if (approxMode == COMPONENTS_UNDER_APPROX) {
    ROSE_ASSERT(!system.hasStgUnderApprox());
  } else {
    ROSE_ASSERT(!system.hasStg());
  }
  vector<Flow*> cfas(system.size());
  boost::unordered_map<int, int> cfaIdMap;
  int index = 0;
  map<int, Flow*> components = system.components();
  for (map<int, Flow*>::iterator i=components.begin(); i!=components.end(); ++i) {
    cfas[index] = (*i).second;
    cfaIdMap[(*i).first] = index;
    ++index;
  }
  ParProAnalyzer parProAnalyzer(cfas, cfaIdMap);
  if (approxMode == COMPONENTS_NO_APPROX) {
    EdgeAnnotationMap annotationMapSubset = system.edgeAnnotationMap();
    parProAnalyzer.setAnnotationMap(annotationMapSubset);
  } else {
    parProAnalyzer.setAnnotationMap(_annotationMap);
  }
  parProAnalyzer.setComponentApproximation(approxMode);
  parProAnalyzer.initializeSolver();
  parProAnalyzer.runSolver();
  ParProTransitionGraph* stg = parProAnalyzer.getTransitionGraph();
  if (approxMode == COMPONENTS_OVER_APPROX) {
    stg->setIsPrecise(false);
    stg->setIsComplete(true);
    system.setStgOverApprox(stg);
  } else if (approxMode == COMPONENTS_UNDER_APPROX) {
    stg->setIsPrecise(true);
    stg->setIsComplete(false);
    system.setStgUnderApprox(stg);
  } else {
    stg->setIsPrecise(true);
    stg->setIsComplete(true);
    system.setStg(stg);
    // there is no approximation, so set both approximations to the precise STG
    system.setStgOverApprox(stg);
    system.setStgUnderApprox(stg);
  }
}

void ParProExplorer::recalculateNumVerifiedFalsified() {
  _numVerified = _properties->entriesWithValue(PROPERTY_VALUE_YES);
  _numFalsified = _properties->entriesWithValue(PROPERTY_VALUE_NO);
}

void ParProExplorer::addToVisOutput(ParallelSystem& system, vector<Flow*>& dotGraphs, NumberGenerator& numGen) {
  if (system.hasStg()) {
    dotGraphs.push_back(system.stg()->toFlowEnumerateStates(numGen));
  } else {
    if (system.hasStgOverApprox()) {
      dotGraphs.push_back(system.stgOverApprox()->toFlowEnumerateStates(numGen));
    }
    if (system.hasStgUnderApprox()) {
      dotGraphs.push_back(system.stgUnderApprox()->toFlowEnumerateStates(numGen));
    }
  }
}
