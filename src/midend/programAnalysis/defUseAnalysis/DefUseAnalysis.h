/******************************************
 * Category: DFA
 * DefUse Analysis Declaration
 * created by tps in Feb 2007
 *****************************************/

#ifndef __DefUseAnalysis_HXX_LOADED__
#define __DefUseAnalysis_HXX_LOADED__
#include <string>

#include "filteredCFG.h"
#include "DFAnalysis.h"
#include "support.h"
#include "DFAFilter.h"

#include <iostream>

// CH (4/9/2010): Use boost::unordered instead
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#if 0
#ifdef _MSC_VER
#include <hash_map>
#include <hash_set>
#if 0
template <> struct hash <SgNode*> {
    size_t operator()(SgNode* const & n) const {
      return (size_t) n;
    }
    size_t operator()(SgNode* const & n1, SgNode* const & n2) const {
      ROSE_ASSERT(n1);
      ROSE_ASSERT(n2);
      return ( n1==n2);
    }
  };
#endif
#else
#include <ext/hash_map>
#include <ext/hash_set>

namespace rose_hash {
  template <> struct hash <SgNode*> {
    size_t operator()(SgNode* const & n) const {
      return (size_t) n;
    }
    size_t operator()(SgNode* const & n1, SgNode* const & n2) const {
      ROSE_ASSERT(n1);
      ROSE_ASSERT(n2);
      return ( n1==n2);
    }
  };
}
#endif

#endif 


class DefUseAnalysis : public DFAnalysis, Support {
 private:
  SgProject* project;
  bool DEBUG_MODE;
  bool DEBUG_MODE_EXTRA;
  std::vector<SgInitializedName*> globalVarList;

  bool visualizationEnabled;

  // def-use-specific --------------------
  typedef std::vector < std::pair<SgInitializedName* , SgNode*> > multitype;
  //  typedef std::multimap < SgInitializedName* , SgNode* > multitype;

  typedef std::map< SgNode* , multitype > tabletype;
  // typedef std::map< SgNode* , int > convtype;
// CH (4/9/2010): Use boost::unordered instead  
//#ifdef _MSC_VER
#if 0
  typedef hash_map< SgNode* , int > convtype;
#else
  //typedef __gnu_cxx::hash_map< SgNode* , int > convtype;
  typedef rose_hash::unordered_map< SgNode* , int > convtype;
#endif

  // local functions ---------------------
  void find_all_global_variables();
  bool start_traversal_of_functions();
  bool searchMap(const tabletype* ltable, SgNode* node);
  bool searchVizzMap(SgNode* node);
  std::string getInitName(SgNode* sgNode);

  // the main table of all entries
  tabletype table;
  tabletype usetable;
  // table for indirect definitions
  //ideftype idefTable;
  // the helper table for visualization
  convtype vizzhelp;
  static int sgNodeCounter ;
  int nrOfNodesVisited;

  // functions to be printed in DFAtoDOT
  std::vector <FilteredCFGNode < IsDFAFilter > > dfaFunctions;

  void addAnyElement(tabletype* tabl, SgNode* sgNode, SgInitializedName* initName, SgNode* defNode);
  void mapAnyUnion(tabletype* tabl, SgNode* before, SgNode* other, SgNode* current);
  void printAnyMap(tabletype* tabl);


 public:
  DefUseAnalysis(SgProject* proj): project(proj), 
    DEBUG_MODE(false), DEBUG_MODE_EXTRA(false){
    //visualizationEnabled=true;
    //table.clear();
    //usetable.clear();
    //globalVarList.clear();
    //vizzhelp.clear();
    //sgNodeCounter=0;
  };
  virtual ~DefUseAnalysis() {}

  std::map< SgNode* , multitype  > getDefMap() { return table;}
  std::map< SgNode* , multitype  > getUseMap() { return usetable;}
  void setMaps(std::map< SgNode* , multitype  > def,
          std::map< SgNode* , multitype > use) {
    table = def;
    usetable = use;
  }
       
  // def-use-public-functions -----------
  int run();
  int run(bool debug);
  multitype getDefMultiMapFor(SgNode* node);
  multitype  getUseMultiMapFor(SgNode* node);
  std::vector < SgNode* > getAnyFor(const multitype* mul, SgInitializedName* initName);
  std::vector < SgNode* > getDefFor(SgNode* node, SgInitializedName* initName);
  std::vector < SgNode* > getUseFor(SgNode* node, SgInitializedName* initName);
  bool isNodeGlobalVariable(SgInitializedName* node);
  std::vector <SgInitializedName*> getGlobalVariables();
  // the following one is used for parallel traversal
  int start_traversal_of_one_function(SgFunctionDefinition* proc);

  // helpers -----------------------------
  bool searchMap(SgNode* node);
  int getDefSize();
  int getUseSize();
  void printMultiMap(const multitype* type);
  void printDefMap();  
  void printUseMap();

  bool addID(SgNode* sgNode);
  void addDefElement(SgNode* sgNode, SgInitializedName* initName, SgNode* defNode);
  void addUseElement(SgNode* sgNode, SgInitializedName* initName, SgNode* defNode);
  void replaceElement(SgNode* sgNode, SgInitializedName* initName);
  void mapDefUnion(SgNode* before, SgNode* other, SgNode* current);
  void mapUseUnion(SgNode* before, SgNode* other, SgNode* current);

  void clearUseOfElement(SgNode* sgNode, SgInitializedName* initName);

  int getIntForSgNode(SgNode* node);
  void dfaToDOT();
  //void addIDefElement(SgNode* sgNode, SgInitializedName* initName);

  // clear the tables if necessary
  void flush() {
   table.clear();
   usetable.clear();
   globalVarList.clear();
   vizzhelp.clear();
   sgNodeCounter=1;
   //  nrOfNodesVisited=0;
  }

  void flushHelp() {
   vizzhelp.clear();
   sgNodeCounter=1;
  }

  void disableVisualization() {
    visualizationEnabled=false;
  }

  void flushDefuse() {
   table.clear();
   usetable.clear();
   //   vizzhelp.clear();
   //sgNodeCounter=1;
  }
};

#endif
