#include "sage3basic.h"

#include "CFAnalysis.h"
#include "Labeler.h"
#include "AstTerm.h"
#include <boost/foreach.hpp>
#include "CodeThornException.h"
#include "Sawyer/GraphTraversal.h"

using namespace CodeThorn;
using namespace std;

Edge::Edge() {
  // all default constructed values for the 4 private member variables are as intended
}
Edge::Edge(Label source0,Label target0):_source(source0),_target(target0),_annotation(""){
  // _types is an empty set by default (we may want to use EDGE_UNKNOWN instead)
}
Edge::Edge(Label source0,EdgeType et,Label target0):_source(source0),_target(target0),_annotation(""){
  _types.insert(et);
}
Edge::Edge(Label source0,set<EdgeType> tset,Label target0):_source(source0),_target(target0),_annotation(""){
  _types=tset;
}

bool Edge::isValid() const {
  return _source.isValid() && _target.isValid();
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
  else {
    // some checks that ensure that no nodes are merged that cannot
    // coexist. For arbitrary CFG transformations EDGE_PATH should be
    // used
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
    _types.erase(*iter);
  }
}

string Edge::toString() const {
  stringstream ss;
  ss << "Edge"<<"("<<source()<<","<<typesToString()<<","<<target()<<")";
  return ss.str();
}
string Edge::toStringNoType() const {
  stringstream ss;
  ss << "("<<source()<<","<<target()<<")";
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
  return ""; // dead code.
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
  ss<<source()<<"->"<<target();
  ss<<" [label=\""<<typesToString()<<"\"";
  ss<<" color="<<color<<" ";
  ss<<"]";
  return ss.str();
}
string Edge::toDotColored() const {
  stringstream ss;
  ss<<source()<<"->"<<target();
  ss<<" [label=\""<<typesToString()<<"\"";
  ss<<" color="<<color()<<" ";
  ss<<"]";
  return ss.str();
}

string Edge::toDotAnnotationOnly() const {
  stringstream ss;
  ss<<source()<<"->"<<target();
  ss<<" [label=\""<<getAnnotation()<<"\"";
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
  return e1.source()==e2.source() && e1.typesCode()==e2.typesCode() && e1.target()==e2.target() && e1.getAnnotation() == e2.getAnnotation();
}
bool CodeThorn::operator!=(const Edge& e1, const Edge& e2) {
  return !(e1==e2);
}
bool CodeThorn::operator<(const Edge& e1, const Edge& e2) {
  assert(&e1);
  assert(&e2);
  if(e1.source()!=e2.source())
    return e1.source()<e2.source();
  if(e1.target()!=e2.target())
    return e1.target()<e2.target();
  if(e1.typesCode()!=e2.typesCode())
    return e1.typesCode()<e2.typesCode();
  return e1.getAnnotation()<e2.getAnnotation();
}

long Edge::typesCode() const {
  long h=1;
  for(set<EdgeType>::iterator i=_types.begin();i!=_types.end();++i) {
    h+=(1<<*i);
  }
  int m = _types.size();
  int k = 0;
  for (string::iterator i=getAnnotation().begin(); i!=getAnnotation().end(); ++i) {
    h+=(static_cast<unsigned char>(*i)<<(k*8 + m));
    k++;
  }
  return h;
}

long Edge::hash() const {
  return typesCode();
}

Flow::Flow() {
  resetDotOptions(); 
}

Label Flow::getStartLabel() {
  if(_startLabelSet.size()==1) {
    return *_startLabelSet.begin();
  } else if(_startLabelSet.size()==0) {
    cerr<<"Flow::getStartLabel: start label requested, but no start label available.";
    exit(1);
  } else {
    cout<<"WARNING: start label requested, but more than one start label available. Choosing randomly one of the registered start labels."<<endl;
    return *_startLabelSet.begin();
  }
}
void Flow::setStartLabel(Label label) {
  LabelSet ls;
  ls.insert(label);
  _startLabelSet=ls;
}

void Flow::addStartLabel(Label label) {
  _startLabelSet.insert(label);
}

void Flow::setStartLabelSet(LabelSet labelSet) {
  _startLabelSet = labelSet;
}

LabelSet Flow::getStartLabelSet() {
  return _startLabelSet;
}

CodeThorn::Flow Flow::reverseFlow() {
  Flow reverseFlow;
  for(Flow::iterator i=begin();i!=end();++i) {
    reverseFlow.insert(Edge((*i).target(),(*i).getTypes(),(*i).source()));
  }
  return reverseFlow;
}

void Flow::resetDotOptions() {
  _dotOptionDisplayLabel=true;
  _dotOptionDisplayStmt=true;
  _dotOptionEdgeAnnotationsOnly=false;
  _dotOptionFixedColor=false;
  _fixedColor="black";
  _fixedNodeColor="white";
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

bool Flow::contains(Edge e) {
  return (find(e) != end());
}

Flow::iterator Flow::find(Edge e) {
#ifdef USE_SAWYER_GRAPH
  SawyerCfg::VertexIterator source = _sawyerFlowGraph.findVertexKey(e.source());
  if (source != _sawyerFlowGraph.vertices().end()) {
    boost::iterator_range<SawyerCfg::EdgeIterator> outEdges = (*source).outEdges();
    for (SawyerCfg::EdgeIterator i=outEdges.begin(); i!=outEdges.end(); ++i) {
      EdgeData eData = (*i).value();
      if (eData.edgeTypes == e.types() && eData.annotation == e.getAnnotation()) {
        if ((*((*i).target())).value() == e.target()) {
          return Flow::iterator(i);
        }
      }
    }
  }
  return Flow::iterator(_sawyerFlowGraph.edges().end());
#else 
  return _edgeSet.find(e);
#endif
}

bool Flow::contains(Label l) {
#ifdef USE_SAWYER_GRAPH
  return (_sawyerFlowGraph.findVertexKey(l) != _sawyerFlowGraph.vertices().end());
#else
  for (Flow::iterator i=begin(); i!=end(); ++i) {
    if ((*i).source() == l || (*i).target() == l) {
      return true;
    }
  }
  return false;
#endif
}

pair<Flow::iterator, bool> Flow::insert(Edge e) {
#ifdef USE_SAWYER_GRAPH
  EdgeData edgeData = EdgeData(e.types(), e.getAnnotation());
  Flow::iterator previousEdge = find(e);
  if (previousEdge != end()) {
    return pair<Flow::iterator, bool>(previousEdge, false);
  } else {
    Flow::iterator iter = Flow::iterator(_sawyerFlowGraph.insertEdgeWithVertices(e.source(), e.target(), edgeData));
    return pair<Flow::iterator, bool>(iter, true);
  }
#else
  return _edgeSet.insert(e);
#endif
}

void Flow::erase(Flow::iterator iter) {
#ifdef USE_SAWYER_GRAPH
  _sawyerFlowGraph.eraseEdgeWithVertices(iter);
#else
  _edgeSet.erase(iter);
#endif
}

size_t Flow::erase(Edge e) {
#ifdef USE_SAWYER_GRAPH
  Flow::iterator existingEdge = find(e);
  if (existingEdge != end()) {
    erase(existingEdge);
    return 1;
  } else {
    return 0;
  }
#else
  return _edgeSet.erase(e);
#endif
}

size_t Flow::size() {
#ifdef USE_SAWYER_GRAPH
  return _sawyerFlowGraph.nEdges();
#else
  return _edgeSet.size();
#endif
}

Flow::iterator Flow::begin() {
#ifdef USE_SAWYER_GRAPH
  return Flow::iterator(_sawyerFlowGraph.edges().begin());
#else
  return _edgeSet.begin();
#endif
}

Flow::iterator Flow::end() {
#ifdef USE_SAWYER_GRAPH
  return Flow::iterator(_sawyerFlowGraph.edges().end());
#else
  return _edgeSet.end();
#endif
}

Flow::node_iterator Flow::nodes_begin() {
#ifdef USE_SAWYER_GRAPH
  return _sawyerFlowGraph.vertexValues().begin();
#else
  throw CodeThorn::Exception("Nodes iterator not implemented because STL set is used as underlying datastructure.");
#endif
}

Flow::node_iterator Flow::nodes_end() {
#ifdef USE_SAWYER_GRAPH
  return _sawyerFlowGraph.vertexValues().end();
#else
  throw CodeThorn::Exception("Nodes iterator not implemented because STL set is used as underlying datastructure.");
#endif
}

Flow::const_node_iterator Flow::nodes_begin() const {
#ifdef USE_SAWYER_GRAPH
  return _sawyerFlowGraph.vertexValues().begin();
#else
  throw CodeThorn::Exception("Nodes iterator not implemented because STL set is used as underlying datastructure.");
#endif
}

Flow::const_node_iterator Flow::nodes_end() const {
#ifdef USE_SAWYER_GRAPH
  return _sawyerFlowGraph.vertexValues().end();
#else
  throw CodeThorn::Exception("Nodes iterator not implemented because STL set is used as underlying datastructure.");
#endif
}

Flow::const_node_iterator Flow::nodes_cbegin() const {
  return nodes_begin();
}

Flow::const_node_iterator Flow::nodes_cend() const {
  return nodes_end();
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

void Flow::setDotOptionEdgeAnnotationsOnly(bool opt) {
  _dotOptionEdgeAnnotationsOnly=opt;
}

void Flow::setDotOptionFixedColor(bool opt) {
  _dotOptionFixedColor=opt;
}

void Flow::setDotFixedColor(string color) {
  setDotOptionFixedColor(true);
  _fixedColor=color;
}

void Flow::setDotFixedNodeColor(string color) {
  _fixedNodeColor=color;
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
  //  cout<<"toDot:: Flow label-set size: "<<nlabs.size()<<endl;
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
      if(labeler->isBlockBeginLabel(*i)) {
        ss<<"{";
      } else if(labeler->isBlockEndLabel(*i)) {
        ss<<"}";
      } else if(labeler->isFunctionEntryLabel(*i)) {
        ss<<"Entry:";
        ss<<SgNodeHelper::nodeToString(node);
      } else if(labeler->isFunctionExitLabel(*i)) {
        ss<<"Exit:";
      } else if(labeler->isFunctionCallLabel(*i)) {
        ss<<"Call:";
        ss<<SgNodeHelper::nodeToString(node);
      } else if(labeler->isFunctionCallReturnLabel(*i)) {
        ss<<"CallReturn:";
      } else if(SgCaseOptionStmt* caseStmt=isSgCaseOptionStmt(node)) {
        ss<<"case "<<caseStmt->get_key()->unparseToString();
        if(SgExpression* expr=caseStmt->get_key_range_end()) {
          ss<<" ... "<<expr->unparseToString();
        }
        ss<<":";
      } else if(isSgTryStmt(node)) {
        ss<<"try: ";
      } else if(isSgDefaultOptionStmt(node)) {
        ss<<"default:";
      } else if(labeler->isJoinLabel(*i)) {
        ss<<"Join for fork "<<labeler->forkLabel(node);
      } else if(labeler->isForkLabel(*i)) {
        ss<<"Fork: ";
      } else if(labeler->isWorkshareLabel(*i)) {
        ss<<"Workshare: ";
        if (isSgOmpForStatement(node)) {
          ss << "OMP for";
        } else if (isSgOmpSectionsStatement(node)) {
          ss << "OMP sections";
        }
      } else if(labeler->isBarrierLabel(*i)) {
        if (isSgOmpBarrierStatement(node)) {
          ss<<"Barrier";
        } else {
          ss<<"Barrier for workshare "<<labeler->workshareLabel(node);
        }
      } else if(isSgOmpBodyStatement(node)) {
        ss<<node->class_name();
      } else {
        ss<<SgNodeHelper::nodeToString(node);
      }
    }
    if(_dotOptionDisplayLabel||_dotOptionDisplayStmt) {
      ss << "\"";
      if (labeler) {
        SgNode* node=labeler->getNode(*i);
        if(SgNodeHelper::isCond(node)) {
          ss << " shape=oval style=filled ";
          ss<<"color=yellow ";
        } else {
          ss << " shape=box ";
        }
      } else {
        if (_fixedNodeColor != "white") {
          ss << " fillcolor=\""<<_fixedNodeColor<<"\" style=filled";
        }
      }
      ss << "];\n";
    }
  }
  for(Flow::iterator i=begin();i!=end();++i) {
    Edge e=*i;
    if (_dotOptionEdgeAnnotationsOnly) {
      ss<<e.toDotAnnotationOnly()<<";\n";
    } else {
      ss<<(_dotOptionFixedColor?e.toDotFixedColor(_fixedColor):e.toDotColored())<<";\n";
    }
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
#ifdef USE_SAWYER_GRAPH
      erase(i);
      ++i;
#else
      erase(i++);
#endif
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
#ifdef USE_SAWYER_GRAPH
    erase(*i);
    ++i;
#else
    erase(i++); // MS: it is paramount to pass a copy of the iterator, and perform a post-increment.
#endif
    numDeleted++;
  }
  return numDeleted;
}

Flow Flow::inEdges(Label label) {
  Flow flow;
#ifdef USE_SAWYER_GRAPH
  SawyerCfg::VertexIterator vertexIter = _sawyerFlowGraph.findVertexKey(label);
  ROSE_ASSERT(vertexIter != _sawyerFlowGraph.vertices().end());
  Flow::iterator begin = Flow::iterator((*vertexIter).inEdges().begin());
  Flow::iterator end = Flow::iterator((*vertexIter).inEdges().end());
  for (Flow::iterator i=begin; i!=end; ++i) {
      Edge inEdge = *i;
      flow.insert(inEdge);
  }
#else
  for(Flow::iterator i=begin();i!=end();++i) {
    if((*i).target()==label)
      flow.insert(*i);
  }
#endif
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}

Edge Flow::outEdgeOfType(Label label, EdgeType type) {
  Flow flow=outEdgesOfType(label,type);
  if(flow.size()==1) {
    return *flow.begin();
  } else {
    Edge invalidEdge;
    return invalidEdge;
  }
}

Flow Flow::outEdges(Label label) {
  Flow flow;
#ifdef USE_SAWYER_GRAPH
  SawyerCfg::VertexIterator vertexIter = _sawyerFlowGraph.findVertexKey(label);
  ROSE_ASSERT(vertexIter != _sawyerFlowGraph.vertices().end());
  Flow::iterator begin = Flow::iterator((*vertexIter).outEdges().begin());
  Flow::iterator end = Flow::iterator((*vertexIter).outEdges().end());
  for (Flow::iterator i=begin; i!=end; ++i) {
    Edge outEdge = *i;
    flow.insert(outEdge);
  }
#else
  for(Flow::iterator i=begin();i!=end();++i) {
    if((*i).source()==label)
      flow.insert(*i);
  }
#endif
  flow.setDotOptionDisplayLabel(_dotOptionDisplayLabel);
  flow.setDotOptionDisplayStmt(_dotOptionDisplayStmt);
  return flow;
}

#ifdef USE_SAWYER_GRAPH
boost::iterator_range<Flow::iterator> Flow::inEdgesIterator(Label label) {
  SawyerCfg::VertexIterator vertexIter = _sawyerFlowGraph.findVertexKey(label);
  ROSE_ASSERT(vertexIter != _sawyerFlowGraph.vertices().end());
  boost::iterator_range<SawyerCfg::EdgeIterator> edges =(*vertexIter).inEdges();
  return boost::iterator_range<Flow::iterator>(Flow::iterator(edges.begin()), Flow::iterator(edges.end()));
}

boost::iterator_range<Flow::iterator> Flow::outEdgesIterator(Label label) {
  SawyerCfg::VertexIterator vertexIter = _sawyerFlowGraph.findVertexKey(label);
  ROSE_ASSERT(vertexIter != _sawyerFlowGraph.vertices().end());
  boost::iterator_range<SawyerCfg::EdgeIterator> edges =(*vertexIter).outEdges();
  return boost::iterator_range<Flow::iterator>(Flow::iterator(edges.begin()), Flow::iterator(edges.end()));
}

// schroder3 (2016-08-16): Returns a topological sorted list of CFG-edges
std::list<Edge> Flow::getTopologicalSortedEdgeList(Label startLabel) {
  std::list<Edge> topologicalSortedEdges;
  SawyerCfg::VertexIterator startLabelIter = _sawyerFlowGraph.findVertexValue(startLabel);
  // Depth first post-order traversal over the edges:
  using namespace Sawyer::Container::Algorithm;
  DepthFirstForwardGraphTraversal<SawyerCfg> depthFirstTraversal(_sawyerFlowGraph, startLabelIter, LEAVE_EDGE);
  for(; depthFirstTraversal; ++depthFirstTraversal) {
    // Use the edge iterator's dereference operator to construct the Edge object:
    Edge edge = *iterator(depthFirstTraversal.edge());
    topologicalSortedEdges.push_front(edge);
  }
  return topologicalSortedEdges;
}
#endif

Flow Flow::outEdgesOfType(Label label, EdgeType edgeType) {
  Flow flow;
  for(Flow::iterator i=begin();i!=end();++i) {
    if((*i).source()==label && (*i).isType(edgeType))
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
    s.insert(e.source());
    s.insert(e.target());
  }
  return s;
}

LabelSet Flow::sourceLabels() {
  LabelSet s;
  for(Flow::iterator i=begin();i!=end();++i) {
    s.insert((*i).source());
  }
  return s;
}

LabelSet Flow::targetLabels() {
  LabelSet s;
  for(Flow::iterator i=begin();i!=end();++i) {
    Edge e=*i;
    s.insert(e.target());
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

set<string> Flow::getAllAnnotations() {
  set<string> result;
  for(Flow::iterator i=begin();i!=end();++i) {
    result.insert((*i).getAnnotation());
  }
  return result;
}

#ifdef USE_SAWYER_GRAPH
EdgeTypeSet Flow::iterator::getTypes() {
  EdgeTypeSet result = ((*this).SawyerCfg::EdgeIterator::operator->())->value().edgeTypes;
  return result;
}

void Flow::iterator::setTypes(EdgeTypeSet types) {
  ((*this).SawyerCfg::EdgeIterator::operator->())->value().edgeTypes=types;
}

string Flow::iterator::getAnnotation() {
  string result = ((*this).SawyerCfg::EdgeIterator::operator->())->value().annotation;
  return result;
}

Label Flow::iterator::source() {
  Label result = ((*this).SawyerCfg::EdgeIterator::operator->())->source()->value();
  return result;
}

Label Flow::iterator::target() {
  Label result = ((*this).SawyerCfg::EdgeIterator::operator->())->target()->value();
  return result;
}

Edge Flow::iterator::operator*() {
  Edge result = Edge(source(), getTypes(), target());
  result.setAnnotation(getAnnotation());
  return result;
}

#endif

