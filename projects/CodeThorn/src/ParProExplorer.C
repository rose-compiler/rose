// Author: Marc Jasper, 2016.

#include "ParProExplorer.h"

using namespace SPRAY;
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
_visualize(false) {
}

PropertyValueTable* ParProExplorer::ltlAnalysis(ParallelSystem system) {
  PropertyValueTable* result = NULL;
  if (_ltlMode == PAR_PRO_LTL_MODE_NONE) {
    return new PropertyValueTable();  // return an empty table because no LTL analysis was selected
  }  
  if (_ltlMode == PAR_PRO_LTL_MODE_CHECK) {
    SpotConnection spotConnection(_ltlInputFilename);
    bool withCounterexample = false;
    bool spuriousNoAnswers = false;
    if (system.hasStg()) {
      spotConnection.checkLtlPropertiesParPro(*(system.stg()), withCounterexample, spuriousNoAnswers, system.getAnnotations());
      result = spotConnection.getLtlResults();
    } else {
      if (system.hasStgOverApprox()) {
	if (_visualize) {
	  ParProSpotTgba* spotTgba = spotConnection.toTgba(*(system.stgOverApprox()));
	  Visualizer visualizer;
	  string dotTgba = visualizer.spotTgbaToDot(*spotTgba);
	  delete spotTgba;
	  spotTgba = NULL;
	  string outputFilename = "spotTgba_over_approx.dot";
	  write_file(outputFilename, dotTgba);
	  cout << "generated " << outputFilename <<"."<<endl;
        }  
	spotConnection.checkLtlPropertiesParPro(*(system.stgOverApprox()), withCounterexample, spuriousNoAnswers, system.getAnnotations());
      }
      if (system.hasStgUnderApprox()) {
	if (_visualize) {
	  ParProSpotTgba* spotTgba = spotConnection.toTgba(*(system.stgUnderApprox()));
	  Visualizer visualizer;
	  string dotTgba = visualizer.spotTgbaToDot(*spotTgba);
	  delete spotTgba;
	  spotTgba = NULL;
	  string outputFilename = "spotTgba_under_approx.dot";
	  write_file(outputFilename, dotTgba);
	  cout << "generated " << outputFilename <<"."<<endl;
	}
	spotConnection.checkLtlPropertiesParPro(*(system.stgUnderApprox()), withCounterexample, spuriousNoAnswers, system.getAnnotations());
      }
    }
    result = spotConnection.getLtlResults();
  } else if (_ltlMode == PAR_PRO_LTL_MODE_MINE) {
    if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_NONE) {
      cerr << "ERROR: ltl mining on the entire parallel system is currently not supported yet." << endl;
      ROSE_ASSERT(0);
    } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_FINITE) {
      result = _parProLtlMiner.mineProperties(system, _minNumComponents);
    } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_INFINITE) {
      int yetToVerify = _numRequiredVerifiable - _numVerified;
      int yetToFalsify = _numRequiredFalsifiable - _numFalsified;
      ROSE_ASSERT(yetToFalsify >= 0 && yetToVerify >= 0);
      result = _parProLtlMiner.mineProperties(system, _minNumComponents, yetToVerify, yetToFalsify);
    }
  }
  return result;
}

void ParProExplorer::explore() {
  if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_INFINITE) {
    if (_ltlMode != PAR_PRO_LTL_MODE_MINE) {
      cout << "ERROR: An unlimited number of analyses using random subsets has been selected, but LTLs are not generated (choose mode --ltl-mode=mine)."<<endl;
      ROSE_ASSERT(0);
    }
    while(_numVerified < _numRequiredVerifiable || _numFalsified < _numRequiredFalsifiable) {
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
    }
    _properties->shuffle();
  } else if (_randomSubsetMode == PAR_PRO_NUM_SUBSETS_FINITE) {
    if (_componentSelection == PAR_PRO_COMPONENTS_SUBSET_FIXED) {
      ParallelSystem system = exploreOnce();
      _properties->append( *(ltlAnalysis(system)) );
    } else {
      for (int i = 0; i < _numDifferentSubsets; i++) {
	ParallelSystem system = exploreOnce();
	PropertyValueTable* intermediateResult = ltlAnalysis(system);
	_properties->append( *intermediateResult );
	if (!_storeComputedSystems) {
	  system.deleteStgs();
	}
	delete intermediateResult;	
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
    ParProAnalyzer parProAnalyzer(_cfas);
    parProAnalyzer.setAnnotationMap(_annotationMap);
    parProAnalyzer.initializeSolver();
    parProAnalyzer.runSolver();
    ParProTransitionGraph* stg = parProAnalyzer.getTransitionGraph();
    system.setStg(stg);
  } else if (_componentSelection == PAR_PRO_COMPONENTS_SUBSET_FIXED) {
    for (set<int>::iterator i=_fixedComponentIds.begin(); i!=_fixedComponentIds.end(); i++) {
      system.addComponent(*i, _cfas[*i]);
    }
    computeStgApprox(system, COMPONENTS_OVER_APPROX);
    computeStgApprox(system, COMPONENTS_UNDER_APPROX);
  } else if (_componentSelection == PAR_PRO_COMPONENTS_SUBSET_RANDOM) {
    set<int> randomIds = randomSetNonNegativeInts(_numRandomComponents, (((int)_cfas.size()) - 1)); 
    for (set<int>::iterator i=randomIds.begin(); i!=randomIds.end(); i++) {
      system.addComponent(*i, _cfas[*i]);
    }
    computeStgApprox(system, COMPONENTS_OVER_APPROX);
    computeStgApprox(system, COMPONENTS_UNDER_APPROX);
  }
  if (_visualize) {
    Visualizer visualizer;
    if (system.hasStg()) {
      string dotStg = visualizer.parProTransitionGraphToDot(system.stg());
      string outputFilename = "stgParallelProgram_no_approx.dot";
      write_file(outputFilename, dotStg);
      cout << "generated " << outputFilename <<"."<<endl;
    }
    if (system.hasStgOverApprox()) {
      string dotStg = visualizer.parProTransitionGraphToDot(system.stgOverApprox());
      string outputFilename = "stgParallelProgram_over_approx.dot";
      write_file(outputFilename, dotStg);
      cout << "generated " << outputFilename <<"."<<endl;
    }
    if (system.hasStgUnderApprox()) {
      string dotStg = visualizer.parProTransitionGraphToDot(system.stgUnderApprox());
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
  ROSE_ASSERT(approxMode != COMPONENTS_NO_APPROX);
  if (approxMode == COMPONENTS_OVER_APPROX) {
    ROSE_ASSERT(!system.hasStgOverApprox());
  } else {
    ROSE_ASSERT(!system.hasStgUnderApprox());
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
  parProAnalyzer.setAnnotationMap(_annotationMap);
  parProAnalyzer.setComponentApproximation(approxMode);
  parProAnalyzer.initializeSolver();
  parProAnalyzer.runSolver();
 if (approxMode == COMPONENTS_OVER_APPROX) {
   ParProTransitionGraph* stg = parProAnalyzer.getTransitionGraph();
   stg->setIsPrecise(false);
   stg->setIsComplete(true);
   system.setStgOverApprox(stg);
 } else {
   ParProTransitionGraph* stg = parProAnalyzer.getTransitionGraph();
   stg->setIsPrecise(true);
   stg->setIsComplete(false);
   system.setStgUnderApprox(stg);
 }
}

void ParProExplorer::recalculateNumVerifiedFalsified() {
  _numVerified = _properties->entriesWithValue(PROPERTY_VALUE_YES);
  _numFalsified = _properties->entriesWithValue(PROPERTY_VALUE_NO);
}
