
#ifndef PAR_PRO_LTL_MINER_H
#define PAR_PRO_LTL_MINER_H

#include "sage3basic.h"

// CodeThorn includes
#include "ParProEState.h"

// CodeThorn includes
#include "SpotConnection.h"
#include "ParProAnalyzer.h"
#include "PropertyValueTable.h"
#include "LtsminConnection.h"

// BOOST includes
#include "boost/lexical_cast.hpp"
#include "boost/unordered_set.hpp"
#include "boost/regex.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/regex.hpp"

namespace CodeThorn {

  class ParProExplorer;

  /*! 
   * \brief Sub-system of parallel process graphs (a.k.a. synchronized labeled transition
   systems). Also stores pointers to respective (approximated) transition graphs.
   * \author Marc Jasper
   * \date 2016, 2017.
   */
  class ParallelSystem {
  public:
    ParallelSystem();
    void deleteStgs();

    std::set<string> getAnnotations() const;
    std::set<int> getComponentIds() const;
    std::set<string> getAnnotations(std::list<int> componentIds);
    size_t size() const { return _components.size(); }
    void addComponent(int id, Flow* cfa);
    std::string toString() const;

    std::map<int, Flow*> components() const { return _components; }  // TODO: remove const and add other functions to query the component ids
    EdgeAnnotationMap edgeAnnotationMap();
    void setComponents(std::map<int, Flow*> components) { _components=components; } 
    bool hasStg() const { return (_stg != NULL); }
    ParProTransitionGraph* stg() { return _stg; }
    void setStg(ParProTransitionGraph* stg) { _stg=stg; }
    bool hasStgOverApprox() const { return (_stgOverApprox != NULL); }
    ParProTransitionGraph* stgOverApprox() { return _stgOverApprox; }
    void setStgOverApprox(ParProTransitionGraph* stgOverApprox) { _stgOverApprox=stgOverApprox; }
    bool hasStgUnderApprox() const { return (_stgUnderApprox != NULL); }
    ParProTransitionGraph* stgUnderApprox() { return _stgUnderApprox; }
    void setStgUnderApprox(ParProTransitionGraph* stgUnderApprox) { _stgUnderApprox=stgUnderApprox; }

  private:
    std::map<int, Flow*> _components;
    ParProTransitionGraph* _stg;
    ParProTransitionGraph* _stgOverApprox;
    ParProTransitionGraph* _stgUnderApprox;
  };

  bool operator<(const ParallelSystem& p1, const ParallelSystem& p2);
  bool operator==(const ParallelSystem& p1, const ParallelSystem& p2);
  bool operator!=(const ParallelSystem& p1, const ParallelSystem& p2);

  /*! 
   * \author Marc Jasper
   * \date 2016.
   */
  class ParallelSystemHashFun {
  public:
    ParallelSystemHashFun() {}
    long operator()(ParallelSystem* p) const {
      unsigned int hash=1;
      map<int, Flow*> components = p->components();
      for(map<int, Flow*>::iterator i=components.begin();i!=components.end();++i) {
	hash=((hash<<8)+((long)((*i).first)))^hash;
      }
      return long(hash);
    }
  };
  
  /*! 
   * \author Marc Jasper
   * \date 2016.
   */
  class ParallelSystemEqualToPred {
  public:
    ParallelSystemEqualToPred() {}
    bool operator()(ParallelSystem* s1, ParallelSystem* s2) const {
      if(s1->components().size()!=s2->components().size()) {
	return false;
      } else {
	map<int, Flow*> s1Components = s1->components();
	for(map<int, Flow*>::iterator i1=s1Components.begin(), i2=s2->components().begin();i1!=s1Components.end();(++i1,++i2)) {
	  if((*i1).first!=(*i2).first)
	    return false;
	}
      }
      return true;
    }
  };

  /*! 
   * \author Marc Jasper
   * \date 2016.
   */
  class ParallelSystemSet : public HSetMaintainer<ParallelSystem,ParallelSystemHashFun,ParallelSystemEqualToPred> {
  public:
    typedef HSetMaintainer<ParProEState,ParProEStateHashFun,ParProEStateEqualToPred>::ProcessingResult PSProcessingResult;
  };

  typedef boost::unordered_map<const ParallelSystem*, std::list<const ParallelSystem*> > ParallelSystemDag;

  /*! 
   * \brief Mines randomly generated valid and violated LTL properties on parallel process graphs.
   * \author Marc Jasper
   * \date 2016, 2017.
   */
  class ParProLtlMiner {
  public:
  ParProLtlMiner(ParProExplorer* explorer) : _numberOfMiningsPerSubsystem(10), _parProExplorer(explorer) {}

    PropertyValueTable* mineProperties(ParallelSystem& system, int minNumComponents);
    PropertyValueTable* mineProperties(ParallelSystem& system, int minNumComponents, int minNumVerifiable, int minNumFalsifiable);
    PropertyValueTable* minePropertiesLtsMin(ParallelSystem& system, int minNumComponents, int minNumVerifiable, int minNumFalsifiable);
    void setNumberOfMiningsPerSubsystem(unsigned int numMinings) { _numberOfMiningsPerSubsystem = numMinings; }
    void setNumberOfComponentsForLtlAnnotations(unsigned int numComponentsLtl) { _numComponentsForLtlAnnotations = numComponentsLtl; }
    void setStoreComputedSystems(bool storeSystems) { _storeComputedSystems = storeSystems; }

  private:
    std::string randomLtlFormula(std::vector<std::string> atomicPropositions, int maxProductions); // deprecated
    std::pair<std::string, std::string> randomLtlFormula(std::set<std::string> atomicPropositions);
    std::pair<std::string, std::string> randomTemporalFormula(std::set<std::string>& atomicPropositions, int temporalDepth, bool withOuterParens=true);
    std::pair<std::string, std::string> randomFrequentEventFormula(std::set<std::string>& atomicPropositions, int temporalDepth, bool withOuterParens=true);
    std::pair<std::string, std::string> randomRareEventFormula(std::set<std::string>& atomicPropositions, int temporalDepth, bool withOuterParens=true);
    std::string randomAtomicProposition(std::set<std::string>& atomicPropositions);
    // version that stores computed parallel systems
    bool passesFilter(std::string ltlProperty, PropertyValue correctValue, 
		      const ParallelSystem* system, int minNumComponents);
    void exploreSubsystemsAndAddToWorklist(const ParallelSystem* system, 
					   ComponentApproximation approxMode, 
					   std::list<const ParallelSystem*>& worklist);
    void initiateSubsystemsOf(const ParallelSystem* system);
    // version that recomputed parallel systems every time (lower memory consumption)
    bool passesFilter(std::string ltlProperty, PropertyValue correctValue, 
		      ParallelSystem& system, int minNumComponents);
    void exploreSubsystemsAndAddToWorklist(ParallelSystem& system, 
					   ComponentApproximation approxMode, 
					   std::list<ParallelSystem>& worklist);
    list<ParallelSystem> initiateSubsystemsOf(ParallelSystem& system);
    bool passesFilterLtsMin(string ltlProperty, PropertyValue correctValue, ParallelSystem& system, int minNumComponents);
    
    unsigned int _numComponentsForLtlAnnotations;
    unsigned int _numberOfMiningsPerSubsystem;
    ParProExplorer* _parProExplorer;

    bool _storeComputedSystems;
    ParallelSystemSet _subsystems;
    ParallelSystemDag _subsystemsOf;
    SpotConnection _spotConnection;
    LtsminConnection _ltsminConnection;
  };

} // end of namespace CodeThorn

#endif
