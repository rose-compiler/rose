
#include "sage3basic.h"
#include "TransitionGraph.h"
#include "Analyzer.h"
#include "CodeThornException.h"
#include "CodeThornCommandLineOptions.h"

using namespace CodeThorn;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
Transition::Transition() {}
Transition::Transition(const EState* source,Edge edge, const EState* target)
  :source(source),edge(edge),target(target) {
}

string Transition::toString() const {
  string s1=source->toString();
  string s2=edge.toString();
  string s3=target->toString();
  return string("(")+s1+", "+s2+", "+s3+")";
}

size_t Transition::memorySize() const {
  return sizeof(*this);
}

TransitionHashFun::TransitionHashFun() {
}

long TransitionHashFun::operator()(Transition* s) const {
  unsigned int hash=1;
  hash=((((long)s->source)+1)<<8)+(long)s->target*(long)s->edge.hash();
  return long(hash);
}

TransitionEqualToPred::TransitionEqualToPred() {}
bool TransitionEqualToPred::operator()(Transition* t1, Transition* t2) const {
  return t1->source==t2->source && t1->target==t2->target && t1->edge==t2->edge;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
TransitionGraph::TransitionGraph():
  _startLabel(Label()),
  _numberOfNodes(0),
  _preciseSTG(true), 
  _completeSTG(true),
  _modeLTLDriven(false),
  _forceQuitExploration(false) {
}

LabelSet TransitionGraph::labelSetOfIoOperations(InputOutput::OpType op) {
  LabelSet lset;
  // the target node records the effect of the edge-operation on the source node.
  for(TransitionGraph::iterator i=begin();i!=end();++i) {
    if((*i)->target->io.op==op) {
      lset.insert((*i)->source->label());
    }
  }
  return lset;
} 

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::reduceEStates(set<const EState*> toReduce) {
  for(set<const EState*>::const_iterator i=toReduce.begin();i!=toReduce.end();++i) { 
    reduceEState(*i);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::reduceEStates2(set<const EState*> toReduce) {
  size_t todo=toReduce.size();
  if(args.getBool("post-semantic-fold"))
    cout << "STATUS: remaining states to fold: "<<todo<<endl;
  for(set<const EState*>::const_iterator i=toReduce.begin();i!=toReduce.end();++i) { 
    reduceEState2(*i);
    todo--;
    if(todo%10000==0 && args.getBool("post-semantic-fold")) {
      cout << "STATUS: remaining states to fold: "<<todo<<endl;
    }
  }
}

/*! 
 * \author Markus Schordan
 * \date 2019 (linear algorithm for IO reduction)
 */         
void TransitionGraph::reduceEStates3(function<bool(const EState*)> predicate) {
  const size_t reportingInterval=10000;
  EStatePtrSet states=estateSet();
  size_t todo=states.size();
  size_t numReduced=0;
  cout << "STATUS: remaining states to check for reduction: "<<todo<<endl;
  if (todo==0) {
    return;
  }
  for(auto i:states) { 
    if(!(predicate(i) || i == getStartEState())) {
      reduceEState2(i);
      numReduced++;
    }
    todo--;
    if(todo%reportingInterval==0) {
      cout << "STATUS: remaining states to check for reduction: "<<todo<<endl;
    }
  }
  cout<<"STATUS: reduced "<<numReduced<<" states."<<endl;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */

TransitionGraph::TransitionPtrSet TransitionGraph::inEdges(const EState* estate) {
  ROSE_ASSERT(estate);
#if 1
  return _inEdges[estate];
#else
  TransitionGraph::TransitionPtrSet in;
  for(TransitionGraph::const_iterator i=begin();i!=end();++i) {
    if(estate==(*i).target)
      in.insert(&(*i)); // TODO: CHECK
  }
  TransitionGraph::TransitionPtrSet in2;
  if(!(in==_inEdges[estate])) {
    cerr<<"DEBUG: inEdges mismatch."<<endl;
    cerr << "set1:";
    for(TransitionGraph::TransitionPtrSet::iterator i=in.begin();i!=in.end();++i) {
      cerr << " "<<(*i)->toString()<<endl;
    }
    cerr <<endl;
    in2=_inEdges[estate];
    cerr << "set2:";
    for(TransitionGraph::TransitionPtrSet::iterator i=in2.begin();i!=in2.end();++i) {
      cerr << " "<<(*i)->toString()<<endl;
    }
    cerr <<endl;
    cerr<<"------------------------------------------------"<<endl;
    exit(1);
  }
  return in;
#endif
}


/*! 
  * \author Markus Schordan
  * \date 2012.
 */
// MS: we definitely need to cache all the results or use a proper graph structure
TransitionGraph::TransitionPtrSet TransitionGraph::outEdges(const EState* estate) {
  ROSE_ASSERT(estate);
  if(getModeLTLDriven()) {
    ROSE_ASSERT(_analyzer);
    if (_forceQuitExploration) {
      return TransitionGraph::TransitionPtrSet();
    }
    if(_outEdges[estate].size()==0) {
      ROSE_ASSERT(_analyzer);
      Analyzer::SubSolverResultType subSolverResult;
      if(_analyzer) {
	subSolverResult = _analyzer->subSolver(estate);
      } else {
	throw CodeThorn::Exception("Used analyzer must be an instance of \"IOAnalyzer\" in order to run the sub solver.");
      }
      EStateWorkList& deferedWorkList=subSolverResult.first;
      EStatePtrSet& existingEStateSet=subSolverResult.second;
      EStatePtrSet succNodes;
      for(EStateWorkList::iterator i=deferedWorkList.begin();i!=deferedWorkList.end();++i) {
        succNodes.insert(*i);
      }
      for(EStatePtrSet::iterator i=existingEStateSet.begin();i!=existingEStateSet.end();++i) {
        succNodes.insert(*i);
      }
      //cout<<"DEBUG: succ:"<<deferedWorkList.size()<<","<<existingEStateSet.size()<<":"<<succNodes.size()<<endl;
      for(EStatePtrSet::iterator j=succNodes.begin();j!=succNodes.end();++j) {
        ROSE_ASSERT(*j);
        Edge newEdge(estate->label(),EDGE_PATH,(*j)->label());
        Transition t(estate,newEdge,*j);
        add(t);
      }
    }
    //if(_outEdges[estate].size()>0) cerr<<"DEBUG: #out-edges="<<_outEdges[estate].size()<<endl;
  }
  return _outEdges[estate];
}

EStatePtrSet TransitionGraph::pred(const EState* estate) {
  EStatePtrSet predNodes;
  TransitionPtrSet tset=inEdges(estate);
  for(TransitionPtrSet::iterator i=tset.begin();i!=tset.end();++i) {
    predNodes.insert((*i)->source);
  }
  return predNodes;
}

EStatePtrSet TransitionGraph::succ(const EState* estate) {
  EStatePtrSet succNodes;
  TransitionPtrSet tset=outEdges(estate);
  for(TransitionPtrSet::iterator i=tset.begin();i!=tset.end();++i) {
    succNodes.insert((*i)->target);
  }
  return succNodes;
}



/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::reduceEState(const EState* estate) {
  assert(0);
  /* description of essential operations:
   *   inedges: (n_i,b)
   *   outedges: (b,n_j) 
   *   insert(ni,t,nj) where t=union(t(n_i))+union(t(n_j))+{EDGE_PATH}
   *   remove(n_i,b)
   *   remove(b,n_j)
   *   delete b
   
   * ea: (n1,cfge,n2) == ((l1,p1,c1,io1),(l1,t12,l2),(l2,p2,c2,io2))
   * eb: (n2,cfge,n3) == ((l2,p2,c2,io2),(l2,t23,l3),(l3,p3,c3,io3))
   * ==> (n1,cfge',n3) == ((l1,p1,c1,io1),(l1,{t12,t13,EDGE_PATH}},l3),(l3,p3,c3,io3))
   
   */
  TransitionGraph::TransitionPtrSet in=inEdges(estate);
  TransitionGraph::TransitionPtrSet out=outEdges(estate);
  if(in.size()!=0) {
    //cout<< "INFO: would be eliminating: node: "<<estate<<", #in="<<in.size()<<", #out="<<out.size()<<endl;
  } else {
    cout<< "INFO: not eliminating node because #in==0: node: "<<estate<<", #in="<<in.size()<<", #out="<<out.size()<<endl;
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator==(const Transition& t1, const Transition& t2) {
  return t1.source==t2.source && t1.edge==t2.edge && t1.target==t2.target;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator!=(const Transition& t1, const Transition& t2) {
  return !(t1==t2);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool CodeThorn::operator<(const Transition& t1, const Transition& t2) {
  if(t1.source!=t2.source)
    return t1.source<t2.source;
  if(t1.edge!=t2.edge)
    return t1.edge<t2.edge;
  return t1.target<t2.target;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::add(Transition trans) {
  const Transition* transp=processNewOrExisting(trans);
  assert(transp!=0);
#pragma omp critical(TRANSGRAPH)
  {
    _outEdges[trans.source].insert(transp);
    _inEdges[trans.target].insert(transp);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::erase(TransitionGraph::iterator transiter) {
  const Transition* transp=determine(**transiter);
  assert(transp!=0);
  _outEdges[(*transiter)->source].erase(transp);
  _inEdges[(*transiter)->target].erase(transp);
  HSetMaintainer<Transition,TransitionHashFun,TransitionEqualToPred>::erase(transiter);
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::erase(const Transition trans) {
  const Transition* transp=determine(trans);
  assert(transp!=0);
  _outEdges[trans.source].erase(transp);
  _inEdges[trans.target].erase(transp);
  size_t num=HSetMaintainer<Transition,TransitionHashFun,TransitionEqualToPred>::erase(const_cast<Transition*>(transp));
  assert(num==1);
}

void TransitionGraph::eliminateEState(const EState* estate) {
  TransitionGraph::TransitionPtrSet in=inEdges(estate);
  for(TransitionPtrSet::iterator i=in.begin();i!=in.end();++i)
    erase(**i);
  TransitionGraph::TransitionPtrSet out=outEdges(estate);
  for(TransitionPtrSet::iterator i=out.begin();i!=out.end();++i)
    erase(**i);
  // we only represent edges, nothing else to do
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
string TransitionGraph::toString() const {
  string s;
  size_t cnt=0;
  for(TransitionGraph::const_iterator i=begin();i!=end();++i) {
    s+=(*i)->toString()+"\n";
    cnt++;
  }
  assert(cnt==size());
  return s;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
set<const EState*> TransitionGraph::estateSetOfLabel(Label lab) {
  set<const EState*> estateSet;
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
    if((*j)->source->label()==lab)
      estateSet.insert((*j)->source);
    if((*j)->target->label()==lab)
      estateSet.insert((*j)->target);
  }
  return estateSet;
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
bool TransitionGraph::checkConsistency() {
  bool ok=true;
  size_t cnt=0;
  TransitionGraph* tg=this;
  for(TransitionGraph::const_iterator i=tg->begin();i!=tg->end();++i) {
    cnt++;
  }
  if(cnt!=tg->size()) {
    cerr<< "Error: TransitionGraph: size()==" <<tg->size()<< ", count=="<<cnt<<endl;
    ok=false;
  }
  assert(cnt==tg->size());
  //cout << "checkTransitionGraph:"<<ok<<" size:"<<size()<<endl;
  return ok;
}

const Transition* TransitionGraph::hasSelfEdge(const EState* estate) {
  TransitionPtrSet in=inEdges(estate);
  for(TransitionPtrSet::iterator i=in.begin();i!=in.end();++i) {
    if((*i)->source==estate)
      return *i;
  }
  // checking in edges only is sufficient because a self edge must be both
  return 0;
}
/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::reduceEState2(const EState* estate) {
  /* description of essential operations:
   *   inedges: (n_i,b)
   *   outedges: (b,n_j) 
   *   insert(ni,t,nj) where t=union(t(n_i))+union(t(n_j))+{EDGE_PATH}
   *   remove(n_i,b)
   *   remove(b,n_j)
   *   delete b
   
   * ea: (n1,cfge,n2) == ((l1,p1,c1,io1),(l1,t12,l2),(l2,p2,c2,io2))
   * eb: (n2,cfge,n3) == ((l2,p2,c2,io2),(l2,t23,l3),(l3,p3,c3,io3))
   * ==> (n1,cfge',n3) == ((l1,p1,c1,io1),(l1,{t12,t13,EDGE_PATH}},l3),(l3,p3,c3,io3))
   
   */
  ROSE_ASSERT(estate);
  // check self-edge
  if(const Transition* trans=hasSelfEdge(estate)) {
    this->erase(*trans);
  }
  TransitionGraph::TransitionPtrSet in=inEdges(estate);
  TransitionGraph::TransitionPtrSet out=outEdges(estate);
  if(in.size()!=0 && out.size()!=0 ) {
    set<Transition> newTransitions;
    for(TransitionPtrSet::iterator i=in.begin();i!=in.end();++i) {
      for(TransitionPtrSet::iterator j=out.begin();j!=out.end();++j) {
        if((*i)->source!=estate && (*j)->target!=estate) {
          Edge newEdge((*i)->source->label(),EDGE_PATH,(*j)->target->label());
          Transition t((*i)->source,newEdge,(*j)->target);
          newTransitions.insert(t);
        //assert(newTransitions.find(t)!=newTransitions.end());
        }
      }
    }
    //cout << "DEBUG: number of new transitions: "<<newTransitions.size()<<endl;

    TransitionPtrSet all=in;
    for(TransitionPtrSet::iterator j=out.begin();j!=out.end();++j) {
      all.insert(*j);
    }
    // 1. remove all old transitions
    for(TransitionPtrSet::iterator i=all.begin();i!=all.end();++i) {
        this->erase(**i);
    }

    // 2. add new transitions
    for(set<Transition>::iterator k=newTransitions.begin();k!=newTransitions.end();++k) {
      //check if a single transition to the target already exists, delete this shorter path
      TransitionPtrSet outEdgesTransSource = outEdges((*k).source);
      for (TransitionPtrSet::iterator iter = outEdgesTransSource.begin(); iter != outEdgesTransSource.end(); ++iter) {
        if ((*iter)->target == (*k).target) {
          erase(**iter);
        }
      }
      this->add(*k);
      //assert(find(*k)!=end());
    }
    eliminateEState(estate);
    assert(newTransitions.size()<=in.size()*out.size());
  } else {
    // need to eliminate node instead
    eliminateEState(estate);
  }
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
// later, we may want to maintain this set with every graph-operation (turning the linear access to constant)
set<const EState*> TransitionGraph::estateSet() {
  _recomputedestateSet.clear();
  for(TransitionGraph::iterator j=begin();j!=end();++j) {
      _recomputedestateSet.insert((*j)->source);
      _recomputedestateSet.insert((*j)->target);
  }
  return _recomputedestateSet;
}

// author: Marc Jasper, 2015. 
long TransitionGraph::numberOfObservableStates(bool includeIn, bool includeOut, bool includeErr) {
  long result = 0;
  EStatePtrSet allStates = estateSet();
  for (EStatePtrSet::iterator i=allStates.begin(); i!=allStates.end(); ++i) {
    if ((includeIn && (*i)->io.isStdInIO()) || (includeOut && (*i)->io.isStdOutIO())
            || (includeErr && ((*i)->io.isStdErrIO()||(*i)->io.isFailedAssertIO())) ) {
      result++;
    }
  }
  return result;
}

int TransitionGraph::eliminateBackEdges() {
  const EState* startState=getStartEState();
  set<const EState*> visited;
  visited.insert(startState);
  TransitionPtrSet backEdges; // default empty
  determineBackEdges(startState, visited, backEdges);
  for(TransitionPtrSet::iterator i=backEdges.begin();i!=backEdges.end();++i) {
    
  }
  return backEdges.size();
}
void TransitionGraph::determineBackEdges(const EState* state, set<const EState*>& visited, TransitionPtrSet& tpSet) {
  TransitionPtrSet succPtrs=outEdges(state);
  for(TransitionPtrSet::iterator i=succPtrs.begin();i!=succPtrs.end();++i) {
    if(visited.find((*i)->target)!=visited.end()) {
      // target node exists in visited-set
      tpSet.insert(*i);
      return;
    }
    visited.insert((*i)->target);
    determineBackEdges((*i)->target,visited,tpSet);
  }
  return;
}

void TransitionGraph::setIsPrecise(bool v) {
  _preciseSTG=v;
}

void TransitionGraph::setIsComplete(bool v) {
  _completeSTG=v;
}

bool TransitionGraph::isPrecise() {
  return _preciseSTG;
}

bool TransitionGraph::isComplete() {
  return _completeSTG;
}

void TransitionGraph::setForceQuitExploration(bool v) {
  _forceQuitExploration=v;
  setIsComplete(false);
}

size_t TransitionGraph::memorySize() const {
  size_t mem = HSetMaintainer<Transition,TransitionHashFun,TransitionEqualToPred>::memorySize();
  // The size of the Transition objects has been counted by the HSetMaintainer already.
  // However, the additional pointers in the _inEdges and _outEdges maps need to be considered too.
  for (map<const EState*,TransitionPtrSet >::const_iterator i=_inEdges.begin(); i!=_inEdges.end(); ++i) {
    for (TransitionPtrSet::const_iterator k=(*i).second.begin(); k!=(*i).second.end(); ++k) {
      mem+=sizeof(*k);
    }
    mem+=sizeof(*i);
  }
  for (map<const EState*,TransitionPtrSet >::const_iterator i=_outEdges.begin(); i!=_outEdges.end(); ++i) {
    for (TransitionPtrSet::const_iterator k=(*i).second.begin(); k!=(*i).second.end(); ++k) {
      mem+=sizeof(*k);
    }
    mem+=sizeof(*i);
  }
  for(set<const EState*>::const_iterator i=_recomputedestateSet.begin(); i!= _recomputedestateSet.end(); ++i) {
    mem+=sizeof(*i);
  }
  return mem + sizeof(*this);  // TODO: check if sizeof(base class HSetMaintainer) is now counted twice
}

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
void TransitionGraph::setStartEState(const EState* estate) {
  ROSE_ASSERT(getModeLTLDriven());
  _startEState=estate;
}

const EState* TransitionGraph::getStartEState() {
  if(getModeLTLDriven()) {
    return _startEState;
  }
  for(TransitionGraph::iterator i=begin();i!=end();++i) {
    if((*i)->source->label()==getStartLabel()) {
      return (*i)->source;
    }
  }
  return 0;
}

Transition TransitionGraph::getStartTransition() {
  // we ensure that all start transitions share the same start label
  TransitionGraph::iterator foundElementIter=end();
  for(TransitionGraph::iterator i=begin();i!=end();++i) {
    if((*i)->source->label()==getStartLabel()) {
      if(foundElementIter!=end()) {
        cerr<< "Error: TransitionGraph: non-unique start transition."<<endl;
        exit(1);
      }
      foundElementIter=i;
    }
  }
  if(foundElementIter!=end())
    return **foundElementIter;
  else {
    throw CodeThorn::Exception("TransitionGraph: no start transition found.");
  }
}
void TransitionGraph::printStgSize(std::string optionalComment) {
  long inStates = numberOfObservableStates(true, false, false);
  long outStates = numberOfObservableStates(false, true, false);
  long errStates = numberOfObservableStates(false, false, true);
  cout << "STATUS: STG size ";
  if (optionalComment != "") {
    cout << "(" << optionalComment << "): ";
  }
  cout << "#transitions: " << size();
  cout << ", #states: " << estateSet().size()
       << " (" << inStates << " in / " << outStates << " out / " << errStates << " err)"
       << endl;
}

void TransitionGraph::csvToStream(std::stringstream& csvOutput) {
  long inStates = numberOfObservableStates(true, false, false);
  long outStates = numberOfObservableStates(false, true, false);
  long errStates = numberOfObservableStates(false, false, true);
  csvOutput << size() <<","<< estateSet().size() <<","<< inStates <<","<< outStates <<","<< errStates;
}

