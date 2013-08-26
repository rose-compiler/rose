#include "sage3basic.h"
#include "factor_trans_system.h"
#include <algorithm>

using namespace std;
using namespace dbglog;
namespace fuse {

int factorTransSystemDebugLevel=0;

FactorTransStatePtr NULLFactorTransState;
FactorTransEdgePtr NULLFactorTransEdge;

// Given a set of multiplication terms, returns a set of numbers that are the products of upto 
// numTerms of these terms
set<int>* getAllMultPermutations(set<int> multTerms, int numTerms) {
  set<int>* valsPrior = new set<int>();
  valsPrior->insert(1);
  set<int>* valsNext = new set<int>();

  for(int i=0; i<numTerms; i++) {
    for(set<int>::iterator v=valsPrior->begin(); v!=valsPrior->end(); v++) {
    for(set<int>::iterator f=multTerms.begin(); f!=multTerms.end(); f++) {
      valsNext->insert(*v * *f);
    } }

    // Swap valsNext and valsPrior and empty the old contents of valsNext
    swap(valsPrior, valsNext);
    valsNext->clear();
  }

  // valsPrior now contains all the values that are numRollbackSteps ahead of the curVal we started with.

  delete valsNext;
  return valsPrior;
}

// Given a set of divion terms and an initial number, returns a set of numbers that are the results of evenly 
// dividing out upto numTerms of these terms from this number
set<int>* getAllDivPermutations(int val, set<int> divTerms, int numTerms) {
  set<int>* valsPrior = new set<int>();
  valsPrior->insert(val);
  set<int>* valsNext = new set<int>();

  for(int i=0; i<numTerms; i++) {
    for(set<int>::iterator v=valsPrior->begin(); v!=valsPrior->end(); v++) {
    for(set<int>::iterator t=divTerms.begin(); t!=divTerms.end(); t++) {
      // If the current transition factor divides the current value, divide the factor out of the value
      // and add the result to valsNext;
      if(*v % *t == 0) {
        valsNext->insert(*v / *t);
        //dbg << "inserting "<<(*v / *t)<<endl;
      }
    } }

    // Swap valsNext and valsPrior and empty the old contents of valsNext
    swap(valsPrior, valsNext);
    valsNext->clear();
    //dbg << "i="<<i<<": -----------------------------"<<endl;
  }

  // valsPrior now contains all the values that are numTerms behind the val we started with.
  
  delete valsNext;
  return valsPrior;
}

// Returns the prime decomposition of the given number, in order of increasing prime factors
list<int> getPrimeDecomp(int n) {
  list<int> primes;
  int z = 2;

  while(z*z <= n) {
    if(n % z == 0) {
      primes.push_back(z);
      n /= z;
    } else
      z++;
  }
  primes.push_back(n);
  return primes;
}

// Returns a string representation of a list of numbers with the given separator
string list2Str(const list<int>& l, string sep) {
  ostringstream oss;
  list<int>::const_iterator i=l.begin();
  while(i!=l.end()) {
    oss << *i;
    i++;
    if(i!=l.end())
      oss << sep;
  }
  return oss.str();
}

// Returns a string representation of a set of numbers with the given separator
string set2Str(const set<int>& l, string sep) {
  ostringstream oss;
  set<int>::const_iterator i=l.begin();
  while(i!=l.end()) {
    oss << *i;
    i++;
    if(i!=l.end())
      oss << sep;
  }
  return oss.str();
}

/******************************
 ***** PartPtrPartContext *****
 ******************************/

// Returns a list of PartContextPtr objects that denote more detailed context information about
// this PartContext's internal contexts. If there aren't any, the function may just return a list containing
// this PartContext itself.
std::list<PartContextPtr> PartPtrPartContext::getSubPartContexts() const { 
  std::list<PartContextPtr> listOfMe;
  listOfMe.push_back(makePtr<PartPtrPartContext>(part));
  return listOfMe;
}

// The default implementation corresponds to all Parts having the same context.
bool PartPtrPartContext::operator==(const PartContextPtr& that_arg) const { 
  PartPtrPartContextPtr that = dynamicPtrCast<PartPtrPartContext>(that_arg);
  assert(that);
  return part == that->part;
}
bool PartPtrPartContext::operator< (const PartContextPtr& that_arg) const {
  PartPtrPartContextPtr that = dynamicPtrCast<PartPtrPartContext>(that_arg);
  assert(that);
  return part < that->part;
}
std::string PartPtrPartContext::str(std::string indent) {
  std::ostringstream oss;
  //oss << "[PartPtrPartContext: "<<part->str(indent)<<"]";
  FactorTransStatePtr state = dynamicPtrCast<FactorTransState>(part); assert(state);
  oss << "[V="<<list2Str(getPrimeDecomp(state->curVal), ":")<<" #step="<<state->numSteps<<" #dStep="<<state->numStepsInDerivedGraph<<
         "]";
  return oss.str();
}

/****************************
 ***** FactorTransState *****
 ****************************/
  
// Constructor used to create the states of the base transition system (not a refinement of any other system)
FactorTransState::FactorTransState(const set<int>& transitionFactors, const std::set<int>& subGraphTransFactors, 
                                   int initVal, int numSteps, ComposedAnalysis* analysis): 
  Part(analysis, NULLPart),
  transitionFactors(transitionFactors), 
  baseCommonFactor(1), myCommonFactor(1),
  curVal(initVal), numSteps(numSteps),
  numStepsInDerivedGraph(0), 
  subGraphTransFactors(subGraphTransFactors)
{ 
  /*cout << "FactorTransState::FactorTransState() &transitionFactors="<<(&transitionFactors)<<endl;
  cout << "FactorTransState::FactorTransState() &this->transitionFactors="<<(&(this->transitionFactors))<<endl;*/
}

// Create a State from the given base Part, while adding the extra factor to common factors
// to make sure that this Part refines the base Part
FactorTransState::FactorTransState(PartPtr base, const std::set<int>& transitionFactors, 
                                   int numStepsInDerivedGraph, const std::set<int>& subGraphTransFactors, //std::set<int>::iterator sgtFactorIt,
                                   int curVal, int numSteps, int myCommonFactor, ComposedAnalysis* analysis) : 
  Part(analysis, base), transitionFactors(transitionFactors), 
  numStepsInDerivedGraph(numStepsInDerivedGraph), subGraphTransFactors(subGraphTransFactors)//, sgtFactorIt(sgtFactorIt)
{
  //dbg << "FactorTransState::FactorTransState() base="<<base->str()<<endl;
  FactorTransStatePtr that = dynamicPtrCast<FactorTransState>(base);
  assert(that);
  
  //dbg << "FactorTransState::FactorTransState() that="<<that->str()<<endl;
  this->baseCommonFactor = that->baseCommonFactor * that->myCommonFactor;
  this->myCommonFactor   = myCommonFactor;
  this->curVal           = curVal;
  this->numSteps         = numSteps;
  //dbg << "FactorTransState::FactorTransState() final this="<<str()<<endl;
}

FactorTransState::FactorTransState(const FactorTransState& that) :
  Part((const Part&)that), transitionFactors(that.transitionFactors), subGraphTransFactors(that.subGraphTransFactors)
{
  baseCommonFactor       = that.baseCommonFactor;
  myCommonFactor         = that.myCommonFactor;
  curVal                 = that.curVal;
  numSteps               = that.numSteps;
  numStepsInDerivedGraph = that.numStepsInDerivedGraph;
  //sgtFactorIt            = that.sgtFactorIt;
}

FactorTransState::FactorTransState(const FactorTransStatePtr& that) :
  Part((const Part&)*(that.get())), transitionFactors(that->transitionFactors), subGraphTransFactors(that->subGraphTransFactors)
{
  baseCommonFactor       = that->baseCommonFactor;
  myCommonFactor         = that->myCommonFactor;
  curVal                 = that->curVal;
  numSteps               = that->numSteps;
  numStepsInDerivedGraph = that->numStepsInDerivedGraph;
  //sgtFactorIt            = that->sgtFactorIt;
}


/*void FactorTransState::operator=(const FactorTransState& that) {
  transitionFactors = that.transitionFactors;
  baseCommonFactor  = that.baseCommonFactor;
  myCommonFactor    = that.myCommonFactor;
  numSteps          = that.numSteps;
  curVal            = that.curVal;
}

void FactorTransState::operator=(FactorTransStatePtr that) {
  transitionFactors = that->transitionFactors;
  baseCommonFactor  = that->baseCommonFactor;
  myCommonFactor    = that->myCommonFactor;
  numSteps          = that->numSteps;
  curVal            = that->curVal;
}*/

std::list<PartEdgePtr> FactorTransState::outEdges() {
  // If this part has a parent edge, then it can get its outEdges from the parent and then add its own commonFactor
  if(getParent()) {
    return outEdges_derived();
  // If this part has no parent, it must compute its outEdges using math
  } else {
    return outEdges_base();
  }
}

// Version of outEdges for transition systems that are not derived from other ones (have a NULL parent)
std::list<PartEdgePtr> FactorTransState::outEdges_base() {
  assert(baseCommonFactor==1);
  assert(myCommonFactor==1);
  
  list<PartEdgePtr> edges;
  scope reg("FactorTransState::outEdges_base", scope::medium, 1, factorTransSystemDebugLevel);
  if(factorTransSystemDebugLevel>=1) dbg << "#transitionFactors="<<transitionFactors.size()<<", numSteps="<<numSteps<<", maxSteps="<<dynamic_cast<FactorTransSystemAnalysis*>(analysis)->maxSteps<<", numRollbackSteps="<<dynamic_cast<FactorTransSystemAnalysis*>(analysis)->numRollbackSteps<<endl;
  // If we have not yet reached the maximum number of steps, advance
  if(numSteps < dynamic_cast<FactorTransSystemAnalysis*>(analysis)->maxSteps-1) {
    for(set<int>::iterator i=transitionFactors.begin(); i!=transitionFactors.end(); i++) {
      edges.push_back(makePtr<FactorTransEdge>(transitionFactors, subGraphTransFactors, curVal, curVal * *i, numSteps, numSteps+1, analysis));
      if(factorTransSystemDebugLevel>=1) dbg << "curVal="<<curVal<<" * *i="<<(*i)<<" = "<<(curVal * *i)<<endl;
    }
  // Otherwise, if we have reached the maximum, roll back a fixed number of steps
  } else {
    // In every iteration we'll take the values in inVals_prior, divide them by any multipliers in transitionFactors
    // that they are divisible by and insert the results into inVals_next. We'll then move inVals_next to 
    // inVals_prior and repeat this for a numRollbackSteps number of iterations
    set<int>* inVals = getAllDivPermutations(curVal, transitionFactors, 
                                             dynamic_cast<FactorTransSystemAnalysis*>(analysis)->numRollbackSteps);

    // inVals_prior now contains all the values that are numRollbackSteps behind the curVal we started with.
    // Now, create the edges from curVal to those values
    if(factorTransSystemDebugLevel>=1) dbg << "inVals.size()="<<inVals->size()<<endl;
    for(set<int>::iterator v=inVals->begin(); v!=inVals->end(); v++) {
      edges.push_back(makePtr<FactorTransEdge>(transitionFactors, subGraphTransFactors, curVal, *v, numSteps,
                                         dynamic_cast<FactorTransSystemAnalysis*>(analysis)->maxSteps -                                  
                                         dynamic_cast<FactorTransSystemAnalysis*>(analysis)->numRollbackSteps - 1, analysis));
      if(factorTransSystemDebugLevel>=1) dbg << "curVal="<<curVal<<" *v = "<<*v<<endl;
    }

    delete inVals;
  }
  return edges;
}

// Version of outEdges for transition systems that are derived from other ones (have a non-NULL parent)
std::list<PartEdgePtr> FactorTransState::outEdges_derived() {
  list<PartEdgePtr> edges;
  scope reg("FactorTransState::outEdges_derived", scope::medium, 1, factorTransSystemDebugLevel);
  if(factorTransSystemDebugLevel>=1) dbg << "    "<<str("    ")<<endl;
  
  list<PartEdgePtr> baseOutEdges = getParent()->outEdges();
  // If we've reached the end of the sub-graph associated with the parent state
  if(numStepsInDerivedGraph == dynamic_cast<FactorTransSystemAnalysis*>(analysis)->maxNumStepsInDerivedGraph-1) {
    if(factorTransSystemDebugLevel>=1) dbg << "Reached End"<<endl;
    for(list<PartEdgePtr>::iterator e=baseOutEdges.begin(); e!=baseOutEdges.end(); e++) {
      FactorTransEdgePtr baseEdge = dynamicPtrCast<FactorTransEdge>(*e); assert(baseEdge);
      PartEdgePtr pedge = makePtr<FactorTransEdge>(*e, 
                                         transitionFactors, subGraphTransFactors,
                                         baseEdge->srcVal, baseEdge->srcNumSteps, numStepsInDerivedGraph,   myCommonFactor, //sgtFactorIt, 
                                         baseEdge->tgtVal, baseEdge->tgtNumSteps, 0,                        1, //sgtFactorIt, 
                                         /*1,*/ analysis);
      if(factorTransSystemDebugLevel>=1) dbg << "    pedge="<<pedge->str("            ")<<endl;
      
      edges.push_back(pedge);
    }
  // Otherwise, if we're still in the middle of the sub-graph
  } else {
    if(factorTransSystemDebugLevel>=1) dbg << "Not Reached End"<<endl;
    //for(list<PartEdgePtr>::iterator e=baseOutEdges.begin(); e!=baseOutEdges.end(); e++) {
      for(set<int>::iterator f=subGraphTransFactors.begin(); f!=subGraphTransFactors.end(); f++) {
        PartEdgePtr pedge = makePtr<FactorTransEdge>(getParent()->inEdgeFromAny(), transitionFactors, subGraphTransFactors,
                                           curVal, numSteps, numStepsInDerivedGraph,   myCommonFactor, //sgtFactorIt, 
                                           curVal, numSteps, numStepsInDerivedGraph+1, myCommonFactor * *f, // sgtFactorIt, 
                                           /*myCommonFactor * *f, */analysis);
        if(factorTransSystemDebugLevel>=1) dbg << "    Factor="<<*f<<": pedge="<<pedge->str("            ")<<endl;
        edges.push_back(pedge);
      }
    //}
  }
  return edges;
}

std::list<PartEdgePtr> FactorTransState::inEdges() {
  // If this part has a parent edge, then it can get its inEdges from the parent and then add its own commonFactor
  if(getParent()) {
    return inEdges_derived();
  // If this part has no parent, it must compute its inEdges using math
  } else {
    return inEdges_base();
  }
}

// Version of inEdges for transition systems that are not derived from other ones (have a NULL parent)
std::list<PartEdgePtr> FactorTransState::inEdges_base() {
  assert(baseCommonFactor==1);
  assert(myCommonFactor==1);
  
  list<PartEdgePtr> edges;
  scope reg("FactorTransState::inEdges_base", scope::medium, 1, factorTransSystemDebugLevel);
  if(factorTransSystemDebugLevel>=1) dbg << "this="<<str()<<endl;
  // If we're not at a location in the transition graph to which we may roll back to when we reach the 
  // maximum number of transitions.
  if(numSteps != dynamic_cast<FactorTransSystemAnalysis*>(analysis)->maxSteps -                                  
                 dynamic_cast<FactorTransSystemAnalysis*>(analysis)->numRollbackSteps-1) {
    // Only roll backwards if we're not at the starting node
    if(numSteps>0) {
      // For each transition factor that divides curVal, divide it out of curVal and create a corresponding
      // transition edge
      for(set<int>::iterator t=transitionFactors.begin(); t!=transitionFactors.end(); t++) {
        if(curVal % *t == 0) {
          edges.push_back(makePtr<FactorTransEdge>(transitionFactors, subGraphTransFactors,
                                                    curVal / *t, curVal, numSteps-1, numSteps, analysis));
          if(factorTransSystemDebugLevel>=1) dbg << "curVal="<<curVal<<" / *t="<<(*t)<<" = "<<(curVal / *t)<<endl;
        }
      }
    }
  // Otherwise, roll forward numRollbackSteps steps to all the states that could have rolled back
  // to this state
  } else {
    if(factorTransSystemDebugLevel>=1) dbg << "<u>Unrolling forward</u>"<<endl;
    // In every iteration we'll take the values in outVals_prior, multiply them by all the multipliers in 
    // transitionFactors and insert the results into outVals_next. We'll then move outVals_next to 
    // outVals_prior and repeat this for a numRollbackSteps number of iterations
    set<int>* outVals = getAllMultPermutations(transitionFactors, 
                                               dynamic_cast<FactorTransSystemAnalysis*>(analysis)->numRollbackSteps);

    // outVals_prior now contains all the values that are numRollbackSteps ahead of the curVal we started with.
    // Now, create the edges those values to curVal
    for(set<int>::iterator v=outVals->begin(); v!=outVals->end(); v++) {
      edges.push_back(makePtr<FactorTransEdge>(transitionFactors, subGraphTransFactors,
                                         *v, curVal, 
                                         dynamic_cast<FactorTransSystemAnalysis*>(analysis)->maxSteps - 1,
                                         numSteps, analysis));
      if(factorTransSystemDebugLevel>=1) dbg << "curVal="<<curVal<<" *v = "<<*v<<endl;
    }

    delete outVals;
  }
  return edges;
}

// Version of inEdges for transition systems that are derived from other ones (have a non-NULL parent)
std::list<PartEdgePtr> FactorTransState::inEdges_derived() {
  list<PartEdgePtr> edges;
  scope reg("FactorTransState::inEdges_derived", scope::medium, 1, factorTransSystemDebugLevel);
  if(factorTransSystemDebugLevel>=1) dbg << "#transitionFactors="<<transitionFactors.size()<<endl;

  // If we've reached the start of the sub-graph associated with the parent state
  if(numStepsInDerivedGraph == 0) {
    /*list<PartEdgePtr> baseInEdges = getParent()->inEdges();
    for(list<PartEdgePtr>::iterator e=baseInEdges.begin(); e!=baseInEdges.end(); e++) {
      edges.push_back(makePtr<FactorTransEdge>(*e, transitionFactors, myCommonFactor, analysis));
    }*/
    
    // In every iteration we'll take the values in outVals_prior, multiply them by all the multipliers in 
    // transitionFactors and insert the results into outVals_next. We'll then move outVals_next to 
    // outVals_prior and repeat this for a maxSteps number of iterations
    set<int>* outVals = getAllMultPermutations(subGraphTransFactors, 
                                               dynamic_cast<FactorTransSystemAnalysis*>(analysis)->maxNumStepsInDerivedGraph-1);

    // outVals_prior now contains all the values that are numRollbackSteps ahead of the curVal we started with.
    // Now, create the edges those values to curVal
    
    list<PartEdgePtr> baseInEdges = getParent()->inEdges();
    for(list<PartEdgePtr>::iterator e=baseInEdges.begin(); e!=baseInEdges.end(); e++) {
      for(set<int>::iterator v=outVals->begin(); v!=outVals->end(); v++) {
        FactorTransEdgePtr baseEdge = dynamicPtrCast<FactorTransEdge>(*e); assert(baseEdge);
        edges.push_back(makePtr<FactorTransEdge>(*e, transitionFactors, subGraphTransFactors,
                                           baseEdge->srcVal, baseEdge->srcNumSteps, 
                                              dynamic_cast<FactorTransSystemAnalysis*>(analysis)->maxNumStepsInDerivedGraph-1, 
                                              *v, // sgtFactorIt, 
                                           baseEdge->tgtVal, baseEdge->tgtNumSteps, 
                                           0, 1, //sgtFactorIt, 
                                           /*1, */analysis));
      }
    }
    
    delete outVals;
  // Otherwise, if we're still in the middle of the sub-graph
  } else {
    for(set<int>::iterator f=subGraphTransFactors.begin(); f!=subGraphTransFactors.end(); f++) {
      if(myCommonFactor % *f == 0) {
        edges.push_back(makePtr<FactorTransEdge>(getParent()->inEdgeFromAny(), transitionFactors, subGraphTransFactors,
                                           curVal, numSteps, numStepsInDerivedGraph-1, myCommonFactor / *f, //  sgtFactorIt, 
                                           curVal, numSteps, numStepsInDerivedGraph,   myCommonFactor,      // sgtFactorIt, 
                                           /*myCommonFactor / *f, */analysis));
      }
    }
  }
  
  return edges;
}

std::set<CFGNode> FactorTransState::CFGNodes() const {
  // Create a single CFGNode that contains a SgIntVal of this value
  set<CFGNode> nodes;
  ostringstream oss; oss << "V="<<list2Str(getPrimeDecomp(curVal * baseCommonFactor * myCommonFactor), ":")<<
                           " #step="<<numSteps<<" #dStep="<<numStepsInDerivedGraph;
  nodes.insert(CFGNode(SageBuilder::buildStringVal(oss.str())));
  return nodes;
}

// If this Part corresponds to a function call/return, returns the set of Parts that contain
// its corresponding return/call, respectively.
std::set<PartPtr> FactorTransState::matchingCallParts() const {
  return std::set<PartPtr>();
}

// Returns a PartEdgePtr, where the source is a wild-card part (NULLPart) and the target is this Part
PartEdgePtr FactorTransState::inEdgeFromAny() {
  // If this part has a parent edge, then it can get its inEdgesFromAny from the parent and then add its own commonFactor
  if(getParent()) {
    return makePtr<FactorTransEdge>(getParent()->inEdgeFromAny(), transitionFactors, subGraphTransFactors,
                                     -1,     -1,       -1,                     -1, //subGraphTransFactors.end(), 
                                     curVal, numSteps, numStepsInDerivedGraph, myCommonFactor, //sgtFactorIt, 
                                     /*myCommonFactor, */analysis);
  // If this part has no parent, it must compute its inEdgesFrom using math by using a negative number 
  // as the the source state
  } else {
    return makePtr<FactorTransEdge>(transitionFactors, subGraphTransFactors, -1, curVal, -1, numSteps, analysis);
  }
}

// Returns a PartEdgePtr, where the target is a wild-card part (NULLPart) and the source is this Part
PartEdgePtr FactorTransState::outEdgeToAny() {
  // If this part has a parent edge, then it can get its inEdgesFromAny from the parent and then add its own commonFactor
  if(getParent()) {
    return makePtr<FactorTransEdge>(getParent()->outEdgeToAny(), transitionFactors, subGraphTransFactors,
                                     curVal, numSteps, numStepsInDerivedGraph, myCommonFactor, //sgtFactorIt, 
                                     -1,     -1,       -1,                     -1, //subGraphTransFactors.end(),                          
                                     /*myCommonFactor, */analysis);
  // If this part has no parent, it must compute its inEdgesFrom using math by using a negative number 
  // as the the target state
  } else {
    return makePtr<FactorTransEdge>(transitionFactors, subGraphTransFactors, curVal, -1, numSteps, -1, analysis);
  }
}

// Returns the specific context of this Part. Can return the NULLPartContextPtr if this
// Part doesn't implement a non-trivial context.
PartContextPtr FactorTransState::getPartContext() const {
  if(getParent()) return makePtr<PartPtrPartContext>(getParent());
  else            return NULLPartContextPtr;
}

bool FactorTransState::equal(const PartPtr& o) const {
  FactorTransStatePtr that = dynamicPtrCast<FactorTransState>(o);
  assert(that);
  assert(analysis == that->analysis);

  /*scope reg("FactorTransState::==", scope::medium, 1, 1);
  dbg << "this="<<const_cast<FactorTransState*>(this)->str()<<endl;
  dbg << "that="<<that.get()->str()<<endl;
  dbg << ((transitionFactors == that->transitionFactors &&
         commonFactors   == that->commonFactors &&
          curVal         == that->curVal)?
           "EQUAL": "NOT EQUAL")<<endl;*/

  return getParent()            == that->getParent() &&
         baseCommonFactor       == that->baseCommonFactor &&
         myCommonFactor         == that->myCommonFactor &&
         curVal                 == that->curVal && 
         numSteps               == that->numSteps && 
         numStepsInDerivedGraph == that->numStepsInDerivedGraph && 
         //sgtFactorIt            == that->sgtFactorIt && 
         transitionFactors      == that->transitionFactors &&
         subGraphTransFactors   == that->subGraphTransFactors;
}

bool FactorTransState::less(const PartPtr& o)  const {
  FactorTransStatePtr that = dynamicPtrCast<FactorTransState>(o);
  assert(that);
  assert(analysis == that->analysis);

  /*scope reg("FactorTransState::<", scope::medium, 1, 1);
  dbg << "this="<<const_cast<FactorTransState*>(this)->str()<<endl;
  dbg << "that="<<that.get()->str()<<endl;
  dbg << "getParent() < that->getParent(): "<<(getParent() < that->getParent())<<endl;
  dbg << "getParent() == that->getParent(): "<<(getParent() == that->getParent())<<endl;
  dbg << "transitionFactors < that->transitionFactors: "<<(transitionFactors < that->transitionFactors)<<endl;
  dbg << "transitionFactors == that->transitionFactors: "<<(transitionFactors == that->transitionFactors)<<endl;
  dbg << "baseCommonFactor < that->baseCommonFactor: "<<(baseCommonFactor < that->baseCommonFactor)<<endl;
  dbg << "baseCommonFactor == that->baseCommonFactor: "<<(baseCommonFactor == that->baseCommonFactor)<<endl;
  dbg << "myCommonFactor == that->myCommonFactor: "<<(myCommonFactor == that->myCommonFactor)<<endl;
  dbg << "myCommonFactor < that->myCommonFactor: "<<(myCommonFactor < that->myCommonFactor)<<endl;
  dbg << "curVal("<<curVal<<" < that->curVal("<<that->curVal<<"): "<<(curVal < that->curVal)<<endl;*/

  if(getParent() < that->getParent()) return true;
  if(getParent() != that->getParent()) return false;

  if(baseCommonFactor  < that->baseCommonFactor) return true;
  if(baseCommonFactor != that->baseCommonFactor) return false;
  if(myCommonFactor  < that->myCommonFactor) return true;
  if(myCommonFactor != that->myCommonFactor) return false;
  if(curVal  < that->curVal) return true;
  if(curVal != that->curVal) return false;
  if(numSteps  < that->numSteps) return true;
  if(numSteps != that->numSteps) return false;
  if(numStepsInDerivedGraph  < that->numStepsInDerivedGraph) return true;
  if(numStepsInDerivedGraph != that->numStepsInDerivedGraph) return false;
  /*if(sgtFactorIt  < that->sgtFactorIt) return true;
  if(sgtFactorIt != that->sgtFactorIt) return false;*/
  if(transitionFactors  < that->transitionFactors) return true;
  if(transitionFactors != that->transitionFactors) return false;
  return subGraphTransFactors < that->subGraphTransFactors;
}

std::string FactorTransState::str(std::string indent) {
  ostringstream oss;
  oss << "[FTState: v="<<curVal<<"="<<(curVal * baseCommonFactor * myCommonFactor) << 
                 ", baseComFac="<<list2Str(getPrimeDecomp(baseCommonFactor), ":")<<
                 ", myComFac="<<list2Str(getPrimeDecomp(myCommonFactor), ":")<<
                 ", #Steps=" << numSteps << 
                 ", #StInDG="<<numStepsInDerivedGraph<<
                 ", transFacs=" << set2Str(transitionFactors, ",") << 
                 ", subGTransFacs=" << set2Str(subGraphTransFactors, ",") << 
                 ", parent="<<getParent() << 
                 //", *sgtFactorIt="<<(subGraphTransFactors.size()>0? *sgtFactorIt: -1)<<
         "]";
  return oss.str();
}

/***************************
 ***** FactorTransEdge *****
 ***************************/

// Constructor used to create the states of the base transition system (not a refinement of any other system)
FactorTransEdge::FactorTransEdge(const set<int>& transitionFactors, const std::set<int>& subGraphTransFactors, 
                                 int srcVal, int tgtVal,                          
                                 int srcNumSteps, int tgtNumSteps, ComposedAnalysis* analysis) :
        PartEdge(analysis, NULLPartEdge), transitionFactors(transitionFactors), 
        srcVal(srcVal), tgtVal(tgtVal),
        srcNumSteps(srcNumSteps), tgtNumSteps(tgtNumSteps),
        subGraphTransFactors(subGraphTransFactors)
{
  //myCommonFactor = 1;
  srcNumStepsInDerivedGraph = 0;
  tgtNumStepsInDerivedGraph = 0; 
  srcCommonFactor = 1;
  tgtCommonFactor = 1;
}

// Create a State from the given base PartEdge, while adding the extra factor to common factors
// to make sure that this PartEdge refines the base PartEdge
FactorTransEdge::FactorTransEdge(PartEdgePtr base, const std::set<int>& transitionFactors, 
                                 const std::set<int>& subGraphTransFactors, 
                                 int srcVal, int srcNumSteps, int srcNumStepsInDerivedGraph, int srcCommonFactor, //std::set<int>::iterator srcSgtFactorIt, 
                                 int tgtVal, int tgtNumSteps, int tgtNumStepsInDerivedGraph, int tgtCommonFactor, //std::set<int>::iterator tgtSgtFactorIt, 
                                 /*int myCommonFactor, */ComposedAnalysis* analysis) :
  PartEdge(analysis, base), transitionFactors(transitionFactors),
  subGraphTransFactors(subGraphTransFactors)
{
  FactorTransEdgePtr that = dynamicPtrCast<FactorTransEdge>(base);
  assert(that);
  
  //this->myCommonFactor            = myCommonFactor;
  this->srcVal                    = srcVal;
  this->tgtVal                    = tgtVal;
  this->srcNumSteps               = srcNumSteps;
  this->tgtNumSteps               = tgtNumSteps;
  this->srcNumStepsInDerivedGraph = srcNumStepsInDerivedGraph;
  //this->srcSgtFactorIt            = that->srcSgtFactorIt;
  this->srcCommonFactor           = srcCommonFactor;
  this->tgtNumStepsInDerivedGraph = tgtNumStepsInDerivedGraph;
  //this->tgtSgtFactorIt            = that->tgtSgtFactorIt;
  this->tgtCommonFactor           = tgtCommonFactor;
}

FactorTransEdge::FactorTransEdge(const FactorTransEdge& that) : 
  PartEdge((const PartEdge&)that), transitionFactors(that.transitionFactors), subGraphTransFactors(that.subGraphTransFactors)
{
  //myCommonFactor            = that.myCommonFactor;
  srcVal                    = that.srcVal;
  tgtVal                    = that.tgtVal;
  srcNumSteps               = that.srcNumSteps;
  tgtNumSteps               = that.tgtNumSteps;
  srcNumStepsInDerivedGraph = that.srcNumStepsInDerivedGraph;
  //srcSgtFactorIt            = that.srcSgtFactorIt;
  srcCommonFactor           = that.srcCommonFactor;
  tgtNumStepsInDerivedGraph = that.tgtNumStepsInDerivedGraph;
  //tgtSgtFactorIt            = that.tgtSgtFactorIt;
  tgtCommonFactor           = that.tgtCommonFactor;
}

FactorTransEdge::FactorTransEdge(const FactorTransEdgePtr& that) : 
  PartEdge((const PartEdge&)(*that.get())), transitionFactors(that->transitionFactors), subGraphTransFactors(that->subGraphTransFactors)
{
  //myCommonFactor            = that->myCommonFactor;
  srcVal                    = that->srcVal;
  tgtVal                    = that->tgtVal;
  srcNumSteps               = that->srcNumSteps;
  tgtNumSteps               = that->tgtNumSteps;
  srcNumStepsInDerivedGraph = that->srcNumStepsInDerivedGraph;
  //srcSgtFactorIt            = that->srcSgtFactorIt;
  srcCommonFactor           = that->srcCommonFactor;
  tgtNumStepsInDerivedGraph = that->tgtNumStepsInDerivedGraph;
  //tgtSgtFactorIt            = that->tgtSgtFactorIt;
  this->tgtCommonFactor     = that->tgtCommonFactor;
}

PartPtr FactorTransEdge::source() const
{
  if(getParent()) {
    if(getParent()->source()) return makePtr<FactorTransState>(getParent()->source(), transitionFactors,
                                                                srcNumStepsInDerivedGraph, subGraphTransFactors,
                                                                srcVal, srcNumSteps, srcCommonFactor, analysis);
    // If the source is NULL simply because this edge is "inside" a base part
    else if(srcVal!=-1) return makePtr<FactorTransState>(getParent()->target(), transitionFactors,
                                                          srcNumStepsInDerivedGraph, subGraphTransFactors,
                                                          srcVal, srcNumSteps, srcCommonFactor, analysis);
    else                      return NULLPart;
  } else if(srcVal>=0) return makePtr<FactorTransState>(transitionFactors, subGraphTransFactors, srcVal, srcNumSteps, analysis);
    else               return NULLPart;
}

PartPtr FactorTransEdge::target() const
{
  if(getParent()) {
    if(getParent()->target()) return makePtr<FactorTransState>(getParent()->target(), transitionFactors,
                                                                tgtNumStepsInDerivedGraph, subGraphTransFactors,
                                                                tgtVal, tgtNumSteps, tgtCommonFactor, analysis);
    // If the target is NULL simply because this edge is "inside" a base part
    else if(tgtVal!=-1) return makePtr<FactorTransState>(getParent()->source(), transitionFactors,
                                                          tgtNumStepsInDerivedGraph, subGraphTransFactors,
                                                          tgtVal, tgtNumSteps, tgtCommonFactor, analysis);
    else                      return NULLPart;
  } else if(tgtVal>=0) return makePtr<FactorTransState>(transitionFactors, subGraphTransFactors,
                                                         tgtVal, tgtNumSteps, analysis);
   else               return NULLPart;
}

// If the source Part corresponds to a conditional of some sort (if, switch, while test, etc.)
// it must evaluate some predicate and depending on its value continue, execution along one of the
// outgoing edges. The value associated with each outgoing edge is fixed and known statically.
// getPredicateValue() returns the value associated with this particular edge. Since a single 
// Part may correspond to multiple CFGNodes getPredicateValue() returns a map from each CFG node
// within its source part that corresponds to a conditional to the value of its predicate along 
// this edge. 
std::map<CFGNode, boost::shared_ptr<SgValueExp> > FactorTransEdge::getPredicateValue()
{
  if(getParent()) return getParent()->getPredicateValue();
  else            return map<CFGNode, boost::shared_ptr<SgValueExp> >();
}

bool FactorTransEdge::equal(const PartEdgePtr& o) const
{
  FactorTransEdgePtr that = dynamicPtrCast<FactorTransEdge>(o);
  assert(that);
  
  return //myCommonFactor            == that->myCommonFactor && 
         srcVal                    == that->srcVal &&
         tgtVal                    == that->tgtVal &&
         srcNumSteps               == that->srcNumSteps &&
         tgtNumSteps               == that->tgtNumSteps &&
         srcNumStepsInDerivedGraph == that->srcNumStepsInDerivedGraph &&
         tgtNumStepsInDerivedGraph == that->tgtNumStepsInDerivedGraph &&
         /*srcSgtFactorIt            == that->srcSgtFactorIt &&
         tgtSgtFactorIt            == that->tgtSgtFactorIt &&*/
         srcCommonFactor           == that->srcCommonFactor &&
         tgtCommonFactor           == that->tgtCommonFactor &&
         transitionFactors         == that->transitionFactors &&
         subGraphTransFactors      == that->subGraphTransFactors;
}

bool FactorTransEdge::less(const PartEdgePtr& o)  const {
  FactorTransEdgePtr that = dynamicPtrCast<FactorTransEdge>(o);
  assert(that);
  
  /*scope reg("FactorTransEdge::<", scope::medium, 1, 1);
  dbg << "this="<<const_cast<FactorTransEdge*>(this)->str()<<endl;
  dbg << "that="<<that.get()->str()<<endl;
  dbg << "getParent() < that->getParent(): "<<(getParent() < that->getParent())<<endl;
  dbg << "getParent() == that->getParent(): "<<(getParent() == that->getParent())<<endl;
  dbg << "myCommonFactor < that->myCommonFactor: "<<(myCommonFactor < that->myCommonFactor)<<endl;
  dbg << "myCommonFactor == that->myCommonFactor: "<<(myCommonFactor == that->myCommonFactor)<<endl;
  dbg << "srcVal < that->srcVal: "<<(srcVal < that->srcVal)<<endl;
  dbg << "srcVal == that->srcVal: "<<(srcVal == that->srcVal)<<endl;
  dbg << "tgtVal < that->tgtVal: "<<(tgtVal < that->tgtVal)<<endl;
  dbg << "tgtVal == that->tgtVal: "<<(tgtVal == that->tgtVal)<<endl;
  dbg << "srcNumSteps < that->srcNumSteps: "<<(srcNumSteps < that->srcNumSteps)<<endl;
  dbg << "srcNumSteps("<<srcNumSteps<<") == that->srcNumSteps("<<that->srcNumSteps<<"): "<<(srcNumSteps == that->srcNumSteps)<<endl;
  dbg << "tgtNumSteps < that->tgtNumSteps: "<<(tgtNumSteps < that->tgtNumSteps)<<endl;
  dbg << "tgtNumSteps("<<tgtNumSteps<<") == that->tgtNumSteps("<<that->tgtNumSteps<<"): "<<(tgtNumSteps == that->tgtNumSteps)<<endl;
  dbg << "transitionFactors < that->transitionFactors: "<<(transitionFactors < that->transitionFactors)<<endl;*/
  
  if(getParent() < that->getParent()) return true;
  if(getParent() != that->getParent()) return false;
  /*if(myCommonFactor  < that->myCommonFactor) return true;
  if(myCommonFactor != that->myCommonFactor) return false;*/
  if(srcVal  < that->srcVal) return true;
  if(srcVal != that->srcVal) return false;
  if(tgtVal  < that->tgtVal) return true;
  if(tgtVal != that->tgtVal) return false;
  if(srcNumSteps  < that->srcNumSteps) return true;
  if(srcNumSteps != that->srcNumSteps) return false;
  if(tgtNumSteps  < that->tgtNumSteps) return true;
  if(tgtNumSteps != that->tgtNumSteps) return false;
  if(srcNumStepsInDerivedGraph  < that->srcNumStepsInDerivedGraph) return true;
  if(srcNumStepsInDerivedGraph != that->srcNumStepsInDerivedGraph) return false;
  if(tgtNumStepsInDerivedGraph  < that->tgtNumStepsInDerivedGraph) return true;
  if(tgtNumStepsInDerivedGraph != that->tgtNumStepsInDerivedGraph) return false;
  /*if(srcSgtFactorIt  < that->srcSgtFactorIt) return true;
  if(srcSgtFactorIt != that->srcSgtFactorIt) return false;
  if(tgtSgtFactorIt  < that->tgtSgtFactorIt) return true;
  if(tgtSgtFactorIt != that->tgtSgtFactorIt) return false;*/
  if(srcCommonFactor  < that->srcCommonFactor) return true;
  if(srcCommonFactor != that->srcCommonFactor) return false;
  if(tgtCommonFactor  < that->tgtCommonFactor) return true;
  if(tgtCommonFactor != that->tgtCommonFactor) return false;
  if(transitionFactors  < that->transitionFactors) return true;
  if(transitionFactors != that->transitionFactors) return false;
  return subGraphTransFactors < that->subGraphTransFactors;
}

// Pretty print for the object
std::string FactorTransEdge::str(std::string indent) {
  ostringstream oss;
  int fullSrcCommonFactor = 1, fullTgtCommonFactor = 1;
  if(getParent()) {
    FactorTransEdgePtr parent = dynamicPtrCast<FactorTransEdge>(getParent());
    while(parent) {
      assert(parent);
      fullSrcCommonFactor *= parent->srcCommonFactor;
      fullTgtCommonFactor *= parent->tgtCommonFactor;
      parent = dynamicPtrCast<FactorTransEdge>(parent->getParent());
    }
  }
  oss << "[FTEdge: src=" << list2Str(getPrimeDecomp(srcVal*fullSrcCommonFactor), ":") << 
                ", tgt=" << list2Str(getPrimeDecomp(tgtVal*fullTgtCommonFactor), ":") << 
                ", src#Steps=" << srcNumSteps << 
                ", tgt#Steps=" << tgtNumSteps << 
                ", src#SInDG=" << srcNumStepsInDerivedGraph << 
                ", tgt#SInDG=" << tgtNumStepsInDerivedGraph << 
                /*", *srcSgtFactorIt=" << (subGraphTransFactors.size()>0? *srcSgtFactorIt: -1) << 
                ", *tgtSgtFactorIt=" << (subGraphTransFactors.size()>0? *tgtSgtFactorIt: -1) << */
                ", srcComFac=" << list2Str(getPrimeDecomp(srcCommonFactor), ":") << 
                ", tgtComFac=" << list2Str(getPrimeDecomp(tgtCommonFactor), ":") << 
                ", transFacs=" << set2Str(transitionFactors, ",") << 
                ", subGTransFacs=" << set2Str(subGraphTransFactors, ",") << 
                ", parent="<<getParent() << 
         "]";
  return oss.str();
}

/*************************************
 ***** FactorTransSystemAnalysis *****
 *************************************/

FactorTransSystemAnalysis::FactorTransSystemAnalysis(
        bool firstAnalysis, 
        const std::set<int>& transitionFactors, 
        /*int myCommonFactor, */int maxSteps, int numRollbackSteps,
        int maxNumStepsInDerivedGraph, const std::set<int>& subGraphTransFactors) : 
  firstAnalysis(firstAnalysis), transitionFactors(transitionFactors), 
  /*myCommonFactor(myCommonFactor), */maxSteps(maxSteps), numRollbackSteps(numRollbackSteps),
  maxNumStepsInDerivedGraph(maxNumStepsInDerivedGraph), subGraphTransFactors(subGraphTransFactors)
{
  //cout << "FactorTransSystemAnalysis::FactorTransSystemAnalysis() &transitionFactors="<<(&transitionFactors)<<", #transitionFactors="<<transitionFactors.size()<<endl;
  //cout << "FactorTransSystemAnalysis::FactorTransSystemAnalysis() &this->transitionFactors="<<(&this->transitionFactors)<<", #this->transitionFactors="<<this->transitionFactors.size()<<endl;
}

// Returns a shared pointer to a freshly-allocated copy of this ComposedAnalysis object
ComposedAnalysisPtr FactorTransSystemAnalysis::copy()
{ return boost::make_shared<FactorTransSystemAnalysis>(firstAnalysis, transitionFactors, 
                                                       /*myCommonFactor, */maxSteps, numRollbackSteps,
                                                       maxNumStepsInDerivedGraph, subGraphTransFactors); }

// Return the anchor Parts of the application
set<PartPtr> FactorTransSystemAnalysis::GetStartAStates_Spec()
{
  set<PartPtr> startStates;
  
  if(firstAnalysis) {
    startStates.insert(makePtr<FactorTransState>(transitionFactors, subGraphTransFactors, 1, 0, this));
  } else {
    /*dbg << "getComposer()-&gt;GetStartAState(this)="<<getComposer()->GetStartAState(this)<<endl;
    dbg << "getComposer()-&gt;GetStartAState(this)="<<getComposer()->GetStartAState(this)->str()<<endl;*/
    //dbg << "myCommonFactor="<<myCommonFactor<<endl;
    set<PartPtr> baseStartStates = getComposer()->GetStartAStates(this);
    for(set<PartPtr>::iterator s=baseStartStates.begin(); s!=baseStartStates.end(); s++)
      startStates.insert(makePtr<FactorTransState>(*s, transitionFactors, 0, subGraphTransFactors, 1, 0, 1, this));
  }
  
  return startStates;
}

set<PartPtr> FactorTransSystemAnalysis::GetEndAStates_Spec()
{
  set<PartPtr> endStates;
  
  // If this is the first analysis, use math to compute the values at the terminal points of the transition system
  if(firstAnalysis) {
    // In every iteration we'll take the values in outVals_prior, multiply them by all the multipliers in 
    // transitionFactors and insert the results into outVals_next. We'll then move outVals_next to 
    // outVals_prior and repeat this for a maxSteps number of iterations
    set<int>* outVals = getAllMultPermutations(transitionFactors, maxSteps-1);
    
    // Now, create the edges those values to curVal
    for(set<int>::iterator v=outVals->begin(); v!=outVals->end(); v++) {
      endStates.insert(makePtr<FactorTransState>(transitionFactors, subGraphTransFactors, *v, maxSteps-1, this));
    }
    
    delete outVals;
  // Else, if this is not the first analysis, build the endStates from the end states provided by the server analysis
  } else {
    set<int>* inVals = getAllMultPermutations(subGraphTransFactors, maxNumStepsInDerivedGraph-1);
    
    set<PartPtr> baseEndStates = getComposer()->GetEndAStates(this);
    for(set<PartPtr>::iterator s=baseEndStates.begin(); s!=baseEndStates.end(); s++) {
      FactorTransStatePtr baseState = dynamicPtrCast<FactorTransState>(*s); assert(baseState);
      for(set<int>::iterator v=inVals->begin(); v!=inVals->end(); v++)
        endStates.insert(makePtr<FactorTransState>(baseState, transitionFactors, maxNumStepsInDerivedGraph-1, 
                                                    subGraphTransFactors, 
                                                    baseState->curVal, baseState->numSteps, *v, this));
    }
    
    delete inVals;
  }
  return endStates;
}

}; // namespace fuse
