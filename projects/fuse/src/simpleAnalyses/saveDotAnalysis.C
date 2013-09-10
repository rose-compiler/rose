#include "sage3basic.h"
#include "saveDotAnalysis.h"
#include "partitions.h"
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>
using namespace std;
using namespace dbglog;
namespace fuse {
/***********************
 *** SaveDotAnalysis ***
 ***********************/

int saveDotAnalysisDebugLevel=1;

// Helper function to print Part information
void printPart(std::ostream &o, map<PartPtr, partDotInfoPtr>& partInfo, PartPtr part, string indent);
// Edge printer
void printEdge(std::ostream &o, map<PartPtr, partDotInfoPtr>& partInfo, PartEdgePtr e, bool isInEdge, string indent);
  
// ---------------------------------------------
// Generates a DOT graph that represents the given partition graph

void ats2dot(std::string fName, std::string graphName, set<PartPtr>& startParts, set<PartPtr>& endParts)
{
  ofstream out((fName+".dot").c_str(), std::ofstream::out);
  ats2dot(out, graphName, startParts, endParts);
  out.close();
  
  ostringstream cmd;
  cmd << "dot -Tpng "<<fName<<".dot -o "<<fName<<".png";
  system(cmd.str().c_str());
}

void ats2dot_bw(std::string fName, std::string graphName, set<PartPtr>& startParts, set<PartPtr>& endParts)
{
  ofstream out((fName+".dot").c_str(), std::ofstream::out);
  ats2dot_bw(out, graphName, startParts, endParts);
  out.close();
  
  ostringstream cmd;
  cmd << "dot -Tpng "<<fName<<".dot -o "<<fName<<".png";
  system(cmd.str().c_str());
}

// Given a map from PartPtrs to their unique IDs, return the unique ID of the given PartPtr, adding a 
// new ID for it if it is not already in the map
int getPartUID(map<PartPtr, partDotInfoPtr>& partInfo, PartPtr part)
{
  map<PartPtr, partDotInfoPtr>::iterator i=partInfo.find(part);
  // If the part exists in the map, return its ID
  if(i != partInfo.end())
    return i->second->partID;
  // Otherwise, add a fresh ID and return it
  else {
    int ID = partInfo.size();
    partInfo[part] = boost::make_shared<partDotInfo>(ID);
    return ID;
  }
}

/************************
 ***** DummyContext *****
 ************************/

// Returns a list of PartContextPtr objects that denote more detailed context information about
// this PartContext's internal contexts. If there aren't any, the function may just return a list containing
// this PartContext itself.
list<PartContextPtr> DummyContext::getSubPartContexts() const {
  std::list<PartContextPtr> listOfMe;
  listOfMe.push_back(makePtr<DummyContext>());
  return listOfMe;
}

bool DummyContext::operator==(const PartContextPtr& that) const { return true; }
bool DummyContext::operator< (const PartContextPtr& that) const { return false; }
  
std::string DummyContext::str(std::string indent) { return ""; }

/*************************
 ***** Ctxt2PartsMap *****
 *************************/

Ctxt2PartsMap::Ctxt2PartsMap(bool crossAnalysisBoundary, Ctxt2PartsMap_Leaf_GeneratorPtr lgen) : 
      lgen(lgen), crossAnalysisBoundary(crossAnalysisBoundary)
{
  l = lgen->newLeaf();
}

Ctxt2PartsMap::Ctxt2PartsMap(bool crossAnalysisBoundary, const list<list<PartContextPtr> >& key, 
        PartPtr part, Ctxt2PartsMap_Leaf_GeneratorPtr lgen) :
  lgen(lgen), crossAnalysisBoundary(crossAnalysisBoundary)
{
  l = lgen->newLeaf();
  crossAnalysisBoundary = false;
  insert(key, part);
}

// Given a key, pulls off the PartContextPtr at its very front and returns the resulting key along
// with a flag that indicates whether the front element of the top-level list was removed in the process
Ctxt2PartsMap::SubKey Ctxt2PartsMap::getNextSubKey(const list<list<PartContextPtr> >& key) {
  assert(key.size()>0);
  assert(key.begin()->size()>0);
  
  SubKey ret;
  ret.front = key.begin()->front();
  ret.back = key;
  
  // Pop the front PartContext from the first context in the new key
  ret.back.begin()->pop_front();
  
  // If the new key's first element is now empty, remove it as well and record this fact
  if(ret.back.begin()->size()==0) {
    ret.back.pop_front();
    ret.crossAnalysisBoundary = true;
  } else 
    ret.crossAnalysisBoundary = false;
  
  return ret;
}

// Returns the total number of PartContextPtr within key, across all the second-level lists
int Ctxt2PartsMap::getNumSubKeys(const list<list<PartContextPtr> >& key)
{
  int total=0;
  for(list<list<PartContextPtr> >::const_iterator i=key.begin(); i!=key.end(); i++) {
    assert(i->size() > 0);
    total += i->size();
  }
  return total;
}

// Stores the given PartPtr under the given Context key.
void Ctxt2PartsMap::insert(const list<list<PartContextPtr> >& key, PartPtr part) {
  assert(getNumSubKeys(key)>0);
  //cout << "Ctxt2PartsMap::insert() #key="<<key.size()<<endl;
  //list<list<PartContextPtr> > subKey = key;
  //subKey.pop_front();
  
  //cout << "#key="<<key.size()<<" #subKey="<<subKey.size()<<endl;
  
  //if(m.find(curKey) == m.end() || m[curKey].find(loc) == m[curKey].end()) {
  /*if(m.find(curKey) == m.end()) {
    if(key.size()==2) m[curKey] = new Ctxt2PartsMap_Leaf();
    else              m[curKey] = new Ctxt2PartsMap();
  }*/
  
  if(getNumSubKeys(key)==1) l->insert(key, part);
  else {
    SubKey sk = getNextSubKey(key);
    crossAnalysisBoundary = sk.crossAnalysisBoundary;
    if(m.find(sk.front) == m.end()) m[sk.front] = new Ctxt2PartsMap(false, lgen);
    m[sk.front]->insert(sk.back, part);
  }
}

// Returns the Part mapped under the given Context key
set<PartPtr> Ctxt2PartsMap::get(const list<list<PartContextPtr> >& key) {
  assert(getNumSubKeys(key)>0);

  if(getNumSubKeys(key)==1)
    return l->get(key);
  else {
    SubKey sk = getNextSubKey(key);
    if(m.find(sk.front) == m.end())
      return set<PartPtr>();
    else
      return m[sk.front]->get(sk.back);
  }
}

// Prints out the hierarchical sub-graphs of Parts denoted by this map to the given output stream
// partInfo: maps Parts to the information required to display them in the dot graph
// subgraphName: name of the subgraph that contains the current level in the map
void Ctxt2PartsMap::map2dot(std::ostream& o, map<PartPtr, partDotInfoPtr>& partInfo, std::string subgraphName, std::string indent) const {
  //cout << indent << "Ctxt2PartsMap::map2dot() subgraphName="<<subgraphName<<" #m="<<m.size()<<endl;
  l->map2dot(o, partInfo, subgraphName+"_L", indent+"    ");
  if(m.size()==0) return;
  
  int i=0;
  for(std::map<PartContextPtr, Ctxt2PartsMap*>::const_iterator c=m.begin(); c!=m.end(); c++, i++) {
    ostringstream subsubgraphName; subsubgraphName << subgraphName<<"_N"<<i;
    o << indent << "subgraph "<<subsubgraphName.str()<<"{"<<endl;
    o << indent << "  color="<<(crossAnalysisBoundary?"red":"black")<<";"<<endl;
    o << indent << "  fillcolor=lightgrey;"<<endl;
    o << indent << "  style=filled;"<<endl;
    o << indent << "  rankdir=LR;"<<endl;
    string label = c->first.get()->str();
    //cout << indent << "  i="<<i<<", label="<<label<<endl;
    //std::replace(label.begin(), label.end(), string("\n"), string("\\n"));
    boost::replace_all(label, "\n", "\\n");
    o << indent << "  label = \""<<label<<"\";"<<endl;
    c->second->map2dot(o, partInfo, subsubgraphName.str(), indent+"    ");
    o << indent << "}"<<endl;
  }
  
  // Make sure that all the sub-graphs are arranged horizontally
  /*o << "  { rank=same; ";
  i=0;
  for(std::map<PartContextPtr, Ctxt2PartsMap*>::const_iterator c=m.begin(); c!=m.end(); c++, i++) {
    ostringstream subsubgraphName; subsubgraphName << subgraphName<<"_"<<i;
    o << subsubgraphName.str()<<"; ";
  }
  o << "};"<<endl;*/
}

std::string Ctxt2PartsMap::str(std::string indent) {
  ostringstream oss;
  oss << "Ctxt2PartsMap["<<endl;
  oss << indent << "    " << l->str(indent+"    ") << endl;
  
  for(map<PartContextPtr, Ctxt2PartsMap* >::iterator i=m.begin(); i!=m.end(); i++) {
    oss << indent << i->first.get()->str(indent+"  ") << " =&gt; " << endl;
    oss << indent << "    " << i->second->str(indent+"    ") << endl;
  }
  /*if(m.size()>1) {
    map<PartContextPtr, Ctxt2PartsMap* >::iterator i=m.begin();
    i++;
    for(; i!=m.end(); i++) {
    for(map<PartContextPtr, Ctxt2PartsMap* >::iterator j=m.begin(); j!=i; j++) {
      oss << indent << "==" << (i->first==j->first) << " i="<<i->first.get()->str(indent+"  ")<<" j="<<j->first.get()->str(indent+"  ")<<endl;
    } }
  }*/
  oss << "]";
  return oss.str();
}

/******************************
 ***** Ctxt2PartsMap_Leaf *****
 ******************************/

Ctxt2PartsMap_Leaf::Ctxt2PartsMap_Leaf(const list<list<PartContextPtr> >& key, PartPtr part) {
  insert(key, part);
}

// Stores the given PartPtr under the given Context key.
void Ctxt2PartsMap_Leaf::insert(const list<list<PartContextPtr> >& key, PartPtr part) {
  assert(Ctxt2PartsMap::getNumSubKeys(key)==1);
  Ctxt2PartsMap::SubKey sk = Ctxt2PartsMap::getNextSubKey(key);
  //cout << "Ctxt2PartsMap_Leaf::insert() <<#m="<<m.size()<<" key="<<(sk.front.get()->str())<<endl;
  m[sk.front].insert(part);
  //cout << "Ctxt2PartsMap_Leaf::insert() >>#m="<<m.size()<<endl;
}

// Returns the Part mapped under the given Context key
set<PartPtr> Ctxt2PartsMap_Leaf::get(const list<list<PartContextPtr> >& key) {
  assert(Ctxt2PartsMap::getNumSubKeys(key)==1);
  //if(m.find(*(key.begin())) == m.end() || m[*(key.begin())].find(loc) == m[*(key.begin())].end()) {
  Ctxt2PartsMap::SubKey sk = Ctxt2PartsMap::getNextSubKey(key);
  if(m.find(sk.front) == m.end())
    return set<PartPtr>();
  else
    return m[sk.front];
}

// Prints out the hierarchical sub-graphs of Parts denoted by this map to the given output stream
// partInfo: maps Parts to the information required to display them in the dot graph
// subgraphName: name of the subgraph that contains the current level in the map
void Ctxt2PartsMap_Leaf::map2dot(std::ostream& o, map<PartPtr, partDotInfoPtr>& partInfo, std::string subgraphName, std::string indent) const {
  //cout << indent << "Ctxt2PartsMap_Leaf::map2dot() subgraphName="<<subgraphName<<" #m="<<m.size()<<endl;
  
  if(m.size()==0) return;
  
  int i=0;
  for(map<PartContextPtr, set<PartPtr> >::const_iterator c=m.begin(); c!=m.end(); c++, i++) {
    ostringstream subsubgraphName; subsubgraphName << subgraphName<<i;
    o << indent << "subgraph "<<subsubgraphName.str()<<"{"<<endl;
    o << indent << "  color=black;"<<endl;
    o << indent << "  fillcolor=lightgrey;"<<endl;
    o << indent << "  style=filled;"<<endl;
    o << indent << "  rankdir=TD;"<<endl;
    //o << "    ordering=out;"<<endl;
    string label = c->first.get()->str();
    //std::replace(label.begin(), label.end(), string("\n"), string("\\n"));
    boost::replace_all(label, "\n", "\\n");
    o << indent << "  label = \""<<label<<"\";"<<endl;
    // Sets of all the outgoing and incoming function call states
    set<PartPtr> funcCallsOut, funcCallsIn;
    
    // Iterate over all the states in this context, printing them and recording the function calls
    for(set<PartPtr>::iterator p=c->second.begin(); p!=c->second.end(); p++) {
      printPart(o, partInfo, *p, indent+"  ");
      set<CFGNode> matchNodes;
      if(p->get()->mayIncomingFuncCall(matchNodes)) funcCallsIn.insert(*p);
      if(p->get()->mayOutgoingFuncCall(matchNodes)) funcCallsOut.insert(*p);
    }
    
    // Add invisible edges between matching in-out function call states
    int inIdx=0;
    for(set<PartPtr>::iterator in=funcCallsIn.begin();   in!=funcCallsIn.end();   in++, inIdx++) {
      o << indent << "subgraph "<<subsubgraphName.str()<<"_Call"<<inIdx<<"{"<<endl;
      o << indent << "  style=invis;"<<endl;
      o << indent << getPartUID(partInfo, *in)<<endl;
      ostringstream sink;
      int numMatchedCalls=0;
      for(set<PartPtr>::iterator out=funcCallsOut.begin(); out!=funcCallsOut.end(); out++) {
        if(in->get()->mayMatchFuncCall(*out)) {
          o << indent << getPartUID(partInfo, *out) << " -> " << getPartUID(partInfo, *in) << "[style=invis]" << endl;
          o << indent << getPartUID(partInfo, *out)<<endl;
          sink << getPartUID(partInfo, *out)<<(numMatchedCalls==0? "": ",");
          numMatchedCalls++;
        }
      }
      o << indent << "  { rank=sink; "<<getPartUID(partInfo, *in)<<" }"<<endl;
      o << indent << "  { rank=source; "<<sink.str()<<" }"<<endl;
      o << indent << "}" <<endl;
    }
      
    // Set the entry nodes to the minimum position
    bool sourceDiscovered=false;
    for(set<PartPtr>::iterator p=c->second.begin(); p!=c->second.end(); p++) {
      if((*p)->maySgNodeAny<SgFunctionParameterList>()) {
        if(!sourceDiscovered) {
          o << indent << "  { rank=source; ";
          sourceDiscovered = true;
        }
        o << getPartUID(partInfo, *p)<<"; ";
      }
    }
    if(sourceDiscovered) o << "};"<<endl;
    
    // Set the exit nodes to the maximum position
    bool sinkDiscovered=false;
    for(set<PartPtr>::iterator p=c->second.begin(); p!=c->second.end(); p++) {
      if((*p)->maySgNodeAny<SgFunctionDefinition>()) {
        if(!sinkDiscovered) {
          o << indent << "  { rank=sink; ";
          sinkDiscovered = true;
        }
        o << getPartUID(partInfo, *p)<<"; ";
      }
    }
    if(sinkDiscovered) o << "};"<<endl;
    
    o << indent << "}"<< endl;
  }
}

std::string Ctxt2PartsMap_Leaf::str(std::string indent) {
  ostringstream oss;
  oss << "Ctxt2PartsMap_Leaf["<<endl;
  for(map<PartContextPtr, set<PartPtr> >::iterator i=m.begin(); i!=m.end(); i++) {
    oss << indent << i->first.get()->str(indent+"  ") << " =&gt; " << endl;
    for(set<PartPtr>::iterator j=i->second.begin(); j!=i->second.end(); j++)
      oss << indent << "    " << j->get()->str(indent+"    ") << endl;
  }
  oss << "]";
  return oss.str();
}


// Returns an empty Ctxt2PartsMap that can support keys of this length
//Ctxt2PartsMap* createC2PMap(const list<list<PartContextPtr> >& key)
/*Ctxt2PartsMap* createC2PMap(const list<list<PartContextPtr> >& key)
{
  assert(key.size()>0);
  Ctxt2PartsMap* c2pMap = NULL;
  if(key.size()==1) c2pMap = new Ctxt2PartsMap_Leaf();
  else              c2pMap = new Ctxt2PartsMap();
  return c2pMap;
}*/

class Ctxt2PartsMap_Leaf_Generator_Base : public Ctxt2PartsMap_Leaf_Generator {
  public:
  Ctxt2PartsMap_Leaf* newLeaf() const { return new Ctxt2PartsMap_Leaf(); }
};

std::ostream & ats2dot(std::ostream &o, std::string graphName, set<PartPtr>& startParts, set<PartPtr>& endParts)
{
  scope reg("ats2dot", scope::high, saveDotAnalysisDebugLevel, 1);
  o << "digraph " << graphName << " {"<<endl;
  
  // Maps parts to their unique IDs
  map<PartPtr, partDotInfoPtr> partInfo;
  
  // Stores all the text generated to specify edges
  ostringstream edgesStr;
  
  //cout << "------------------------------------------------"<<endl;
  
  // Maps contexts to the set of parts in each context
  Ctxt2PartsMap ctxt2parts(false, boost::make_shared<Ctxt2PartsMap_Leaf_Generator_Base>());
  for(fw_partEdgeIterator state(startParts); state!=fw_partEdgeIterator::end(); state++) {
    PartPtr part = state.getPart();
    scope reg2(txt()<<"ats2dot: part="<<getPartUID(partInfo, part)<<"="<<part->str(), scope::medium, saveDotAnalysisDebugLevel, 1);
    if(saveDotAnalysisDebugLevel>=1) {
      dbg << "context="<<part->getContext()->str()<<endl;
      dbg << "pedge="<<state.getPartEdge()->str()<<endl;
    }
    
    list<list<PartContextPtr> > key = part->getContext()->getDetailedPartContexts();
    if(saveDotAnalysisDebugLevel>=1) dbg << "#key="<<key.size()<<endl;
    if(key.size()==0) {
      DummyContext d;
      key.push_back(d.getSubPartContexts());
    }
    
    /*cout << "key = " << endl;
    for(list<list<PartContextPtr> >::iterator i=key.begin(); i!=key.end(); i++) {
    for(list<PartContextPtr>::iterator j=i->begin(); j!=i->end(); j++) {
      if(j==i->begin()) cout << "[]";
      cout << (*j)->str("    ") << endl;
    } }
    cout << getPartUID(partInfo, *state)<<": state = "<<(*state)->str()<<endl;*/
    //if(!ctxt2parts) ctxt2parts = createC2PMap(key);
    ctxt2parts.insert(key, part);
    /*cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    cout << ctxt2parts.str("");
    cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"<<endl;*/
    
    if(state.getPartEdge()->source() && state.getPartEdge()->target())
      printEdge(edgesStr, partInfo, state.getPartEdge(), false, "  ");
    
    /*list<PartEdgePtr> outEdges = part->outEdges();
    indent ind;
    for(list<PartEdgePtr>::iterator e=outEdges.begin(); e!=outEdges.end(); e++) {
      / *dbg << "edge ="<<getPartUID(partInfo, (*e)->source())<<"="<<(*e)->source()->str()<< " =&gt; " <<
                            getPartUID(partInfo, (*e)->target())<<"="<<(*e)->target()->str()<<endl;* /
      printEdge(edgesStr, partInfo, *e, false, "  ");* /

      // If the edges's source and target have different contexts, the target must be an entry node of its context
      //ctxt2parts[(*e)->target()->getContext()].insert((*e)->target());
      / *if((*e)->source()->getContext() != (*e)->target()->getContext())
        ctxt2EntryParts[(*e)->target()->getContext()].insert((*e)->target());* /
       
      // If the target of this edge has not yet been visited, add it to the iterator
      / *if(visited.find((*e)->target()) != visited.end())
        state.add((*e)->target());* /
    }*/
  }
  
  /*{
  scope reg("ctxt2parts", scope::medium, 1, 1);
  dbg << ctxt2parts.str("");
  }*/
  
  ctxt2parts.map2dot(o, partInfo);
  // Print out all the contexts and the parts within them
  /*int i=0;
  for(map<ContextPtr, set<PartPtr> >::iterator c=ctxt2parts.begin(); c!=ctxt2parts.end(); c++, i++) {
    o << "subgraph cluster_"<<i<<"{"<<endl;
    o << "    color=lightgrey;"<<endl;
    o << "    style=filled;"<<endl;
    o << "    label = \""<<c->first.get()->str()<<"\";"<<endl;
    for(set<PartPtr>::iterator p=c->second.begin(); p!=c->second.end(); p++) {
      printPart(o, partInfo, *p);
    }

    // Make sure that all the entry parts are on top of their context
    / *o << "    { rank=min; ";
    map<ContextPtr, set<PartPtr> >::iterator entr = ctxt2EntryParts.find(c->first);
    for(set<PartPtr>::iterator p=entr->second.begin(); p!=entr->second.end(); p++)
      o << getPartUID(partInfo, *p)<<"; ";
    o << "};"<<endl;
    
    o << "}"<<endl;
  }*/
  
  o << edgesStr.str();
  
  o << "}"<<endl;
  return o;
}

std::ostream & ats2dot_bw(std::ostream &o, std::string graphName, set<PartPtr>& startParts, set<PartPtr>& endParts)
{
  scope reg("ats2dot_bw", scope::high, saveDotAnalysisDebugLevel, 1);
  o << "digraph " << graphName << " {"<<endl;
  
  //dbg << "#endParts="<<endParts.size()<<endl;
  // Maps parts to their unique IDs
  map<PartPtr, partDotInfoPtr> partInfo;
  
  // Stores all the text generated to specify edges
  ostringstream edgesStr;
  
  //cout << "------------------------------------------------"<<endl;
  
  // Maps contexts to the set of parts in each context
  Ctxt2PartsMap ctxt2parts(false, boost::make_shared<Ctxt2PartsMap_Leaf_Generator_Base>());
  
  for(bw_partEdgeIterator state(endParts); state!=bw_partEdgeIterator::end(); state++) {
    PartPtr part = state.getPart();
    scope reg(txt()<<"ats2dot: part="<<getPartUID(partInfo, part)<<"="<<part->str(), scope::medium, saveDotAnalysisDebugLevel, 1);
    if(saveDotAnalysisDebugLevel>=1) {
      dbg << "state="<<state.str()<<endl;
      dbg << "*state="<<part->str()<<" context="<<part->getContext()->str()<<endl;
      dbg << "pedge="<<state.getPartEdge()->str()<<endl;
    }
    
    list<list<PartContextPtr> > key = part->getContext()->getDetailedPartContexts();
    if(key.size()==0) {
      DummyContext d;
      key.push_back(d.getSubPartContexts());
    }
    ctxt2parts.insert(key, part);
    
    if(state.getPartEdge()->source() && state.getPartEdge()->target())
      printEdge(edgesStr, partInfo, state.getPartEdge(), false, "  ");
  }
  ctxt2parts.map2dot(o, partInfo);
  o << edgesStr.str();
  
  o << "}"<<endl;
  return o;
}

// Helper function to print Part information
void printPart(std::ostream &o, map<PartPtr, partDotInfoPtr>& partInfo, PartPtr part, string indent)
{
  std::string nodeColor = "black";
  // Control nodes
  if(part->outEdges().size()>1)
    nodeColor = "purple4";

  std::string nodeStyle = "solid";
  // Function entry/exit
  set<CFGNode> matchNodes;
  if(part->mayFuncEntry(matchNodes) || 
     part->mayFuncExit(matchNodes))
     nodeStyle = "dotted";
  
  std::string nodeShape = "box";
  if(part->mustOutgoingFuncCall(matchNodes))
     nodeShape = "invtrapezium";
  else if(part->mustIncomingFuncCall(matchNodes))
     nodeShape = "trapezium";
  
  o << indent << getPartUID(partInfo, part) << " [label=\"";
  ostringstream label;
  set<CFGNode> nodes = part->CFGNodes();
  for(set<CFGNode>::iterator n=nodes.begin(); n!=nodes.end(); n++) {
    if(n!=nodes.begin()) label << "\\n";
    if(isSgFunctionDefinition(n->getNode())) {
      Function func(isSgFunctionDefinition(n->getNode()));
      label << func.get_name().getString() << "() END";
    } else
      label << CFGNode2Str(*n);
  }
  
  // Compress the label horizontally to ensure that it is not too wide by adding line-breaks
  string labelStr = label.str();
  unsigned int lineWidth = 30;
  string labelMultLineStr = "";
  unsigned int i=0;
  while(i<labelStr.length()-lineWidth) {
    // Look for the next line-break
    unsigned int nextLB = labelStr.find_first_of("\n", i);
    // If the next line is shorter than lineWidth, add it to labelMulLineStr and move on to the next line
    if(nextLB-i < lineWidth) {
      labelMultLineStr += labelStr.substr(i, nextLB-i+1);
      i = nextLB+1;
    // If the next line is longer than lineWidth, add just lineWidth characters to labelMulLineStr
    } else {
      // If it is not much longer than lineWidth, don't break it up
      if(i>=labelStr.length()-lineWidth*1.25) break;
      labelMultLineStr += labelStr.substr(i, lineWidth) + "\\n";
      i += lineWidth;
    }
  }
  // Add the last line in labelStr to labelMulLineStr
  labelMultLineStr += labelStr.substr(i, labelStr.length()-i);
  
  o << labelMultLineStr <<"\", color=\"" << nodeColor << "\", fillcolor=\"white\", style=\"" << nodeStyle << "\", shape=\"" << nodeShape << "\"];\n";
}

// Edge printer
void printEdge(std::ostream &o, map<PartPtr, partDotInfoPtr>& partInfo, PartEdgePtr e, bool isInEdge, string indent)
{
  map<CFGNode, boost::shared_ptr<SgValueExp> > pv = e->getPredicateValue();
  string color = "black";
  ostringstream values;
  
  if(pv.size()>0) {
    // Flags that record whether all the SgValues were boolean with a true/false value.
    bool booleanTrue = true;
    bool booleanFalse = true;
    for(map<CFGNode, boost::shared_ptr<SgValueExp> >::iterator v=pv.begin(); v!=pv.end(); v++) {
      if(v!=pv.begin()) values << ", ";
      values << v->second->unparseToString();
      if(ValueObject::isValueBoolCompatible(v->second)) {
        booleanTrue  = booleanTrue  &&  ValueObject::SgValue2Bool(v->second);
        booleanFalse = booleanFalse && !ValueObject::SgValue2Bool(v->second);
      }
    }

    assert(!(booleanTrue && booleanFalse));
    if(booleanTrue)  color = "blue3";
    if(booleanFalse) color = "crimson";
  }
  
  o << indent << getPartUID(partInfo, e->source()) << " -> " << getPartUID(partInfo, e->target()) << 
       " [label=\"" << escapeString(values.str()) << "\","<<
       " style=\"" << (isInEdge ? "dotted" : "solid") << "\", " << 
       " color=\"" << color << "\"];\n";
}

}; // namespace fuse
