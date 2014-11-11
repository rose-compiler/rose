#include "sage3basic.h"

#include "CFAnalyzer.h"
#include "Labeler.h"
#include "AstTerm.h"
#include <boost/foreach.hpp>

using namespace CodeThorn;

Edge::Edge():source(0),target(0){
}
Edge::Edge(Label source0,Label target0):source(source0),target(target0){
  // _types is an empty set by default (we may want to use EDGE_UNKNOWN instead)
}
Edge::Edge(Label source0,EdgeType et,Label target0):source(source0),target(target0){
  _types.insert(et);
}
Edge::Edge(Label source0,set<EdgeType> tset,Label target0):source(source0),target(target0){
  _types=tset;
}

bool Edge::isType(EdgeType et) const {
  if(et==EDGE_UNKNOWN) {
    return _types.size()==0;
  } else {
    return _types.find(et)!=_types.end();
  }
}

set<EdgeType> Edge::types() const {
  return _types;
}

void Edge::addType(EdgeType et) {
  // perform some consistency checks
  bool ok=true;
  // for EDGE_PATH we allow any combination, otherwise we check
  if(et==EDGE_PATH) {
    _types.clear();
    _types.insert(EDGE_PATH);
    return;
  }
  if(isType(EDGE_PATH))
    return;
  if(!isType(EDGE_PATH) && !(et==EDGE_PATH) && !boolOptions["semantic-fold"] && !boolOptions["post-semantic-fold"]) {
    switch(et) {
    case EDGE_FORWARD: if(isType(EDGE_BACKWARD)) ok=false;break;
    case EDGE_BACKWARD: if(isType(EDGE_FORWARD)) ok=false;break;
    case EDGE_TRUE: if(isType(EDGE_FALSE)) ok=false;break;
    case EDGE_FALSE: if(isType(EDGE_TRUE)) ok=false;break;
    default:
      ;// anything else is ok
    }
  }
  if(ok)
    _types.insert(et);
  else {
    cerr << "Error: inconsistent icfg-edge annotation: edge-annot: " << et << ", set: "<<toString() <<endl;
    exit(1);
  }
}

void Edge::addTypes(set<EdgeType> ets) {
  if(ets.find(EDGE_PATH)!=ets.end()) {
    addType(EDGE_PATH);
  }
  for(set<EdgeType>::iterator i=ets.begin();i!=ets.end();++i) {
    addType(*i);
  }
}

void Edge::removeType(EdgeType et) {
  set<EdgeType>::const_iterator iter =_types.find(et);
  if(iter!=_types.end()) {
    _types.erase(iter);
  }
}

string Edge::toString() const {
  stringstream ss;
  ss << "Edge"<<"("<<source<<","<<typesToString()<<","<<target<<")";
  return ss.str();
}
string Edge::toStringNoType() const {
  stringstream ss;
  ss << "("<<source<<","<<target<<")";
  return ss.str();
}

string Edge::typesToString() const {
  stringstream ss;
  ss<<"{";
  if(_types.size()==0) {
    ss<<typeToString(EDGE_UNKNOWN);
  } else {
    for(set<EdgeType>::iterator i=_types.begin();
        i!=_types.end();
        ++i) {
      if(i!=_types.begin())
        ss<< ", ";
      ss<<typeToString(*i);
    }
  }
  ss<<"}";
  return ss.str();
}

string Edge::typeToString(EdgeType et) {
  switch(et) {
  case EDGE_UNKNOWN: return "unknown";
  case EDGE_FORWARD: return "forward"; // forward edges are obvious.
  case EDGE_BACKWARD: return "backward";
  case EDGE_TRUE: return "true";
  case EDGE_FALSE: return "false";
  case EDGE_LOCAL: return "local";
  case EDGE_EXTERNAL: return "external";
  case EDGE_CALL: return "call";
  case EDGE_CALLRETURN: return "callreturn";
  case EDGE_PATH: return "PATH";
  default:
    cerr<<"Error: Edge-type is not of 'enum EdgeType'.";
    exit(1);
  }
  return ""; // dead code. just to provide some return value to avoid false positive compiler warnings
}

string Edge::color() const {
  if(isType(EDGE_BACKWARD)) 
    return "blue";
  else if(isType(EDGE_TRUE)) 
    return "green";
  else if(isType(EDGE_FALSE)) 
    return "red";
  return "black";
}


// color: true/false has higher priority than forward/backward.
string Edge::toDotFixedColor(string color) const {
  stringstream ss;
  ss<<source<<"->"<<target;
  ss<<" [label=\""<<typesToString()<<"\"";
  ss<<" color="<<color<<" ";
  ss<<"]";
  return ss.str();
}
string Edge::toDotColored() const {
  stringstream ss;
  ss<<source<<"->"<<target;
  ss<<" [label=\""<<typesToString()<<"\"";
  ss<<" color="<<color()<<" ";
  ss<<"]";
  return ss.str();
}

string Edge::dotEdgeStyle() const {
  if(isType(EDGE_EXTERNAL)) 
    return "dotted";
  else if(isType(EDGE_CALL) || isType(EDGE_CALLRETURN)) 
    return "dashed";
  return "solid";
}
InterEdge::InterEdge(Label call, Label entry, Label exit, Label callReturn):
  call(call),
  entry(entry),
  exit(exit),
  callReturn(callReturn){
  }
string InterEdge::toString() const {
  stringstream ss;
  ss << "("<<call<<","<<entry<<","<<exit<<","<<callReturn<<")";
  return ss.str();
}

string InterFlow::toString() const {
  string res;
  res+="{";
  for(InterFlow::iterator i=begin();i!=end();++i) {
    if(i!=begin())
      res+=",";
    res+=(*i).toString();
  }
  res+="}";
  return res;
}

bool CodeThorn::operator<(const InterEdge& e1, const InterEdge& e2) {
  if(e1.call!=e2.call) 
    return e1.call<e2.call;
  if(e1.entry!=e2.entry)
    return e1.entry<e2.entry;
  if(e1.exit!=e2.exit)
    return e1.exit<e2.exit;
  return e1.callReturn<e2.callReturn;
}

bool CodeThorn::operator==(const InterEdge& e1, const InterEdge& e2) {
  return e1.call==e2.call
    && e1.entry==e2.entry
    && e1.exit==e2.exit
    && e1.callReturn==e2.callReturn
    ;
}

bool CodeThorn::operator!=(const InterEdge& e1, const InterEdge& e2) {
  return !(e1==e2);
}

bool CodeThorn::operator==(const Edge& e1, const Edge& e2) {
  assert(&e1);
  assert(&e2);
  return e1.source==e2.source && e1.typesCode()==e2.typesCode() && e1.target==e2.target;
}
bool CodeThorn::operator!=(const Edge& e1, const Edge& e2) {
  return !(e1==e2);
}
bool CodeThorn::operator<(const Edge& e1, const Edge& e2) {
  assert(&e1);
  assert(&e2);
  if(e1.source!=e2.source)
    return e1.source<e2.source;
  if(e1.target!=e2.target)
    return e1.target<e2.target;
  return e1.typesCode()<e2.typesCode();
}

long Edge::typesCode() const {
  long h=1;
  for(set<EdgeType>::iterator i=_types.begin();i!=_types.end();++i) {
    h+=(1<<*i);
  }
  return h;
}

long Edge::hash() const {
  return typesCode();
}

Flow::Flow():_boostified(false) {
  resetDotOptions();
}

void Flow::boostify() {
  cout<<"STATUS: converting ICFG to boost graph representation ... "<<endl;
  edge_t e; bool b;
  for(Flow::iterator i=begin();i!=end();++i) {
    tie(e,b)=add_edge((*i).source,(*i).target,_flowGraph);
    _flowGraph[e]=(*i).getTypes();
  }
  _boostified=true;
  cout<<"STATUS: converting ICFG to boost graph representation: DONE."<<endl;
}


void Flow::resetDotOptions() {
  _dotOptionDisplayLabel=true;
  _dotOptionDisplayStmt=true;
  _dotOptionFixedColor=false;
  _fixedColor="black";
  _dotOptionHeaderFooter=true;
}

string Flow::toString() {
  stringstream ss;
  ss<<"{";
  for(Flow::iterator i=begin();i!=end();++i) {
    if(i!=begin())
      ss<<",";
    if(_stringNoType)
      ss<<(*i).toStringNoType();
    else
      ss<<(*i).toString();
  }
  ss<<"}";
  return ss.str();
}

Flow Flow::operator+(Flow& s2) {
  Flow result;
  result=*this;
  for(Flow::iterator i2=s2.begin();i2!=s2.end();++i2)
    result.insert(*i2);
  return result;
}
 
Flow& Flow::operator+=(Flow& s2) {
  for(Flow::iterator i2=s2.begin();i2!=s2.end();++i2)
    insert(*i2);
  return *this;
}

void Flow::setDotOptionDisplayLabel(bool opt) {
  _dotOptionDisplayLabel=opt;
}
void Flow::setDotOptionDisplayStmt(bool opt) {
  _dotOptionDisplayStmt=opt;
}

void Flow::setDotOptionFixedColor(bool opt) {
  _dotOptionFixedColor=opt;
}

void Flow::setDotFixedColor(string color) {
  setDotOptionFixedColor(true);
  _fixedColor=color;
}

void Flow::setDotOptionHeaderFooter(bool opt) {
  _dotOptionHeaderFooter=opt;
}

void Flow::setTextOptionPrintType(bool opt) {
  _stringNoType=!opt;
}

string Flow::toDot(Labeler* labeler) {
  stringstream ss;
  if(_dotOptionHeaderFooter)
    ss<<"digraph G {\n";
  LabelSet nlabs=nodeLabels();
  cout<<"toDot:: Flow label-set size: "<<nlabs.size()<<endl;
  for(LabelSet::iterator i=nlabs.begin();i!=nlabs.end();++i) {
    if(_dotOptionDisplayLabel) {
      ss << *i;
      ss << " [label=\"";
      ss << Labeler::labelToString(*i);
      if(_dotOptionDisplayStmt)
        ss << ": ";
    }
    if(_dotOptionDisplayStmt) {
      SgNode* node=labeler->getNode(*i);
      if(labeler->isFunctionEntryLabel(*i))
        ss<<"Entry:";
      if(labeler->isFunctionExitLabel(*i))
        ss<<"Exit:";
      if(labeler->isFunctionCallLabel(*i))
        ss<<"Call:";
      if(labeler->isFunctionCallReturnLabel(*i))
        ss<<"CallReturn:";
      ss<<SgNodeHelper::nodeToString(node);
    }
    if(_dotOptionDisplayLabel||_dotOptionDisplayStmt)
      ss << "\"";
    if(_dotOptionDisplayLabel||_dotOptionDisplayStmt) {
      SgNode* node=labeler->getNode(*i);
      if(SgNodeHelper::isCond(node)) {
        ss << " shape=oval style=filled ";
        ss<<"color=yellow "; 
      } else {
        ss << " shape=box ";
      }
      ss << "];\n";
    }
  }
  for(Flow::iterator i=begin();i!=end();++i) {
    Edge e=*i;
    ss<<(_dotOptionFixedColor?e.toDotFixedColor(_fixedColor):e.toDotColored())<<";\n";
  }
  if(_dotOptionHeaderFooter)
    ss<<"}";
  return ss.str();
}

size_t Flow::deleteEdges(EdgeType edgeType) {
  Flow::iterator i=begin();
  size_t numDeleted=0;
  while(i!=end()) {
    if((*i).isType(edgeType)) {
      erase(i++);
      numDeleted++;
    } else {
      ++i;
    }
  }
  return numDeleted;
}

/*! 
  * \author Markus Schordan
  * \date 2013.
 */
size_t Flow::deleteEdges(Flow& edges) {
  // MS: this function is supposed to allow a subset of edges of the very same graph as parameter
  // hence, we must be careful about iterator semantics
  size_t numDeleted=0;
  Flow::iterator i=edges.begin();
  while(i!=end()) {
    erase(i++); // MS: it is paramount to pass a copy of the iterator, and perform a post-increment.
      numDeleted++;
  }
  return numDeleted;
}

Flow Flow::inEdges(Label label) {
  Flow flow;
  for(Flow::iterator i=begin();i!=end();++i) {
    if((*i).target==label)
      flow.insert(*i);
  }
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}

Flow Flow::outEdges(Label label) {
  Flow flow;
  if(!_boostified) {
    for(Flow::iterator i=begin();i!=end();++i) {
      if((*i).source==label)
        flow.insert(*i);
    }
  } else {
    typedef graph_traits<FlowGraph> GraphTraits;
    //    typename property_map<FlowGraph, vertex_index_t>::type 
    // index = get(vertex_index, _flowGraph);
    GraphTraits::out_edge_iterator out_i, out_end;
    GraphTraits::edge_descriptor e;
    for (tie(out_i, out_end) = out_edges(label, _flowGraph); 
         out_i != out_end; ++out_i) {
      e = *out_i;
      Label src = source(e, _flowGraph), targ = target(e, _flowGraph);
      flow.insert(Edge(src,_flowGraph[e],targ));
    }
  }
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}

Flow Flow::outEdgesOfType(Label label, EdgeType edgeType) {
  Flow flow;
  for(Flow::iterator i=begin();i!=end();++i) {
    if((*i).source==label && (*i).isType(edgeType))
      flow.insert(*i);
  }
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}

Flow Flow::edgesOfType(EdgeType edgeType) {
  Flow flow;
  for(Flow::iterator i=begin();i!=end();++i) {
    if((*i).isType(edgeType))
      flow.insert(*i);
  }
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}

LabelSet Flow::nodeLabels() {
  LabelSet s;
  for(Flow::iterator i=begin();i!=end();++i) {
    Edge e=*i;
    s.insert(e.source);
    s.insert(e.target);
  }
  return s;
}

LabelSet Flow::sourceLabels() {
  LabelSet s;
  for(Flow::iterator i=begin();i!=end();++i) {
    Edge e=*i;
    s.insert(e.source);
  }
  return s;
}

LabelSet Flow::targetLabels() {
  LabelSet s;
  for(Flow::iterator i=begin();i!=end();++i) {
    Edge e=*i;
    s.insert(e.target);
  }
  return s;
}

LabelSet Flow::pred(Label label) {
    Flow flow=inEdges(label);
    return flow.sourceLabels();
}

LabelSet Flow::succ(Label label) {
  Flow flow=outEdges(label);
  return flow.targetLabels();
}

