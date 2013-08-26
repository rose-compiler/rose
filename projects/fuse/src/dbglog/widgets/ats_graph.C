#include "sage3basic.h"
#include "widgets.h"
#include "ats_graph.h"
#include "saveDotAnalysis.h"
#include "partitions.h"
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>

using namespace std;
using namespace dbglog;

namespace fuse {

int ats2DotDebugLevel=0;
/***************************************
 *** ATS Graph Visualizer for dbglog ***
 ***************************************/

void printAnchor_atsGraph(std::ostream &o, anchor a, string label, string indent);
// Helper function to print Part information
// doAnchorRankSame - if true, the anchors of this Part are placed on the same horizontal rank in the generated dot graph
void printPart_atsGraph(std::ostream &o, partDotInfoPtr info, PartPtr part, bool doAnchorRankSame, string indent);
void printPartEdge_atsGraph(std::ostream &o, map<PartPtr, partDotInfoPtr>& partInfo, PartEdgePtr e, 
                            bool isInEdge, string indent);

/*************************
 ***** Ctxt2PartsMap *****
 *************************/

Ctxt2PartsMap_atsGraph::Ctxt2PartsMap_atsGraph(bool crossAnalysisBoundary, Ctxt2PartsMap_Leaf_GeneratorPtr lgen) : 
  Ctxt2PartsMap(crossAnalysisBoundary, lgen) {}
Ctxt2PartsMap_atsGraph::Ctxt2PartsMap_atsGraph(bool crossAnalysisBoundary, const std::list<std::list<PartContextPtr> >& key, PartPtr part, Ctxt2PartsMap_Leaf_GeneratorPtr lgen) :
  Ctxt2PartsMap(crossAnalysisBoundary, key, part, lgen) {}

// Prints out the hierarchical sub-graphs of Parts denoted by this map to the given output stream
// partInfo: maps Parts to the information required to display them in the dot graph
// subgraphName: name of the subgraph that contains the current level in the map
void Ctxt2PartsMap_atsGraph::map2dot(std::ostream& o, std::map<PartPtr, partDotInfoPtr>& partInfo, std::string subgraphName, std::string indent) const {
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
    //o << indent << "  rankdir=LR;"<<endl;
    string label = c->first.get()->str();
    boost::replace_all(label, "\n", "\\n");
    o << indent << "  label = \""<<label<<"\";"<<endl;
    c->second->map2dot(o, partInfo, subsubgraphName.str(), indent+"    ");
    o << indent << "}"<<endl;
  }
}

/***************************************
 ***** Ctxt2PartsMap_Leaf_atsGraph *****
 ***************************************/
class partStr {
  public:
  string part;
  string anchor;
  partStr(string part, string anchor) : part(part), anchor(anchor) {}
};

// allAnchors - If true, the anchor field of the returned partStr object contains 
//              a space-separated list of all the anchors associated with this Part.
//              If false, it contains just one representative anchor.
partStr getPartStr(std::map<PartPtr, partDotInfoPtr>& partInfo, PartPtr p, bool allAnchors) {
  partDotInfo_atsGraphPtr pi = boost::dynamic_pointer_cast<partDotInfo_atsGraph>(partInfo[p]);

  assert(pi);
  assert(pi->anchors.size()>0);
  ostringstream partStream;       partStream       << "clusterPart"<<pi->partID;
  ostringstream partAnchorStream; 
  if(allAnchors) {
    for(list<anchor>::iterator a=pi->anchors.begin(); a!=pi->anchors.end(); a++) {
      if(a!=pi->anchors.begin()) partAnchorStream << " ";
      partAnchorStream << "a"<<a->getID();
    }
  }
  else
    partAnchorStream << "a"<<pi->anchors.front().getID();
  
  return partStr(partStream.str(), partAnchorStream.str());
}

// Prints out the hierarchical sub-graphs of Parts denoted by this map to the given output stream
// partInfo: maps Parts to the information required to display them in the dot graph
// subgraphName: name of the subgraph that contains the current level in the map
void Ctxt2PartsMap_Leaf_atsGraph::map2dot(std::ostream& o, std::map<PartPtr, partDotInfoPtr>& partInfo, std::string subgraphName, std::string indent) const {
  scope reg(txt()<<"Ctxt2PartsMap_Leaf_atsGraph::map2dot() subgraphName="<<subgraphName<<" #m="<<m.size(), scope::medium, ats2DotDebugLevel, 2);
  
  if(m.size()==0) return;
  
  int i=0;
  for(map<PartContextPtr, set<PartPtr> >::const_iterator c=m.begin(); c!=m.end(); c++, i++) {
    ostringstream subsubgraphName; subsubgraphName << subgraphName<<i;
    o << indent << "subgraph "<<subsubgraphName.str()<<"{"<<endl;
    o << indent << "  color=black;"<<endl;
    o << indent << "  fillcolor=lightgrey;"<<endl;
    o << indent << "  style=filled;"<<endl;
    //o << indent << "  rankdir=TD;"<<endl;
    string label = c->first.get()->str();
    boost::replace_all(label, "\n", "\\n");
    o << indent << "  label = \""<<label<<"\";"<<endl;
    // Sets of all the outgoing and incoming function call states
    set<PartPtr> funcCallsOut, funcCallsIn;
    
    // Iterate over all the states in this context, printing them and recording the function calls
    for(set<PartPtr>::iterator p=c->second.begin(); p!=c->second.end(); p++) {
      if(ats2DotDebugLevel>=2) dbg << "part="<<p->get()->str();
      
      if(partInfo.find(*p) == partInfo.end()) continue;
      
      set<CFGNode> matchNodes;
      if(p->get()->mayIncomingFuncCall(matchNodes))      funcCallsIn.insert(*p);
      else if(p->get()->mayOutgoingFuncCall(matchNodes)) funcCallsOut.insert(*p);
      else if((*p)->maySgNodeAny<SgFunctionParameterList>() || (*p)->maySgNodeAny<SgFunctionDefinition>())
        printPart_atsGraph(o, partInfo[*p], *p, false, indent+"  ");
      else 
        printPart_atsGraph(o, partInfo[*p], *p, true, indent+"  ");
    }
    
    // Add invisible edges between matching in-out function call states
    int inIdx=0;
    for(set<PartPtr>::iterator in=funcCallsIn.begin(); in!=funcCallsIn.end(); in++, inIdx++) {
      partStr psIn = getPartStr(partInfo, *in, true);
      
      o << indent << "    subgraph "<<subsubgraphName.str()<<"_Call"<<inIdx<<"{"<<endl;
      o << indent << "      style=invis;"<<endl;
      //o << indent << inPartAnchor.str()<<endl;
      printPart_atsGraph(o, partInfo[*in], *in, false, indent+"      ");
      
      ostringstream sink;
      int numMatchedCalls=0;
      for(set<PartPtr>::iterator out=funcCallsOut.begin(); out!=funcCallsOut.end(); out++) {
        if(in->get()->mayMatchFuncCall(*out)) {
          partStr psOut = getPartStr(partInfo, *out, true);
          
          printPart_atsGraph(o, partInfo[*out], *out, false, indent+"      ");
          
          /*o << indent << "      " << psOut.anchor << " -> " << psIn.anchor << 
              "\t[lhead="<<psOut.part<<", "<<
                 "ltain="<<psIn.part<<", "<<
                 "style=\"solid\", color=\"green\"];" << endl;
                  //"style=invis];" << endl;*/
          //o << indent << outPartAnchor.str() <<endl;
          sink << psOut.anchor << (numMatchedCalls==0? "": " ");
          numMatchedCalls++;
        }
      }
      o << indent << "      { rank=sink; "<<psIn.anchor<<" }"<<endl;
      o << indent << "      { rank=source; "<<sink.str()<<" }"<<endl;
      o << indent << "    }" <<endl;
    }
      
    // Set the entry nodes to the minimum position
    bool sourceDiscovered=false;
    for(set<PartPtr>::iterator p=c->second.begin(); p!=c->second.end(); p++) {
      if(partInfo.find(*p) == partInfo.end()) continue;
      
      if((*p)->maySgNodeAny<SgFunctionParameterList>()) {
        partStr ps = getPartStr(partInfo, *p, true);
          
        if(!sourceDiscovered) {
          o << indent << "  { rank=source; ";
          sourceDiscovered = true;
        }
        o << ps.anchor<<"; ";
      }
    }
    if(sourceDiscovered) o << "};"<<endl;
    
    // Set the exit nodes to the maximum position
    bool sinkDiscovered=false;
    for(set<PartPtr>::iterator p=c->second.begin(); p!=c->second.end(); p++) {
      if(partInfo.find(*p) == partInfo.end()) continue;
      
      if((*p)->maySgNodeAny<SgFunctionDefinition>()) {
        partStr ps = getPartStr(partInfo, *p, true);
        
        if(!sinkDiscovered) {
          o << indent << "  { rank=sink; ";
          sinkDiscovered = true;
        }
        o << ps.anchor<<"; ";
      }
    }
    if(sinkDiscovered) o << "};"<<endl;
    
    o << indent << "}"<< endl;
  }
}

// Helper function to print Part anchor information
void printAnchor_atsGraph(std::ostream &o, anchor a, string label, string indent)
{
  std::string nodeColor = "black";
  std::string nodeStyle = "solid";
  std::string nodeShape = "box";
  
  o << indent << "a"<<a.getID() << " "<<
               "[label=\""<<label<<"\", "<<//\"a"<<a.getID()<<"\", "<<
                "color=\"" << nodeColor << "\", "<<
                "fillcolor=\"white\", "<<
                "style=\"" << nodeStyle << "\", "<<
                "shape=\"" << nodeShape << "\", "<<
                "href=\"javascript:"<<a.getLinkJS()<<"\"];\n";
}

// Helper function to print Part information
// doAnchorRankSame - if true, the anchors of this Part are placed on the same horizontal rank in the generated dot graph
void printPart_atsGraph(std::ostream &o, partDotInfoPtr info, PartPtr part, bool doAnchorRankSame, string indent)
{
  //assert(boost::dynamic_pointer_cast<partDotInfo_atsGraph>(info));
  
  o << indent << "subgraph clusterPart"<<boost::dynamic_pointer_cast<partDotInfo_atsGraph>(info)->partID<<"{"<<endl;
  o << indent << "  color=black;"<<endl;
  o << indent << "  fillcolor=lightsteelblue;"<<endl;
  o << indent << "  style=filled;"<<endl;
  //o << indent << "  rankdir=LR;"<<endl;
  o << indent << "  label=\"";
  set<CFGNode> nodes = part->CFGNodes();
  ostringstream label;
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
  
  o << labelMultLineStr <<"\";"<<endl;
  
  // Print all the anchors inside this Part
  list<anchor>& anchors = boost::dynamic_pointer_cast<partDotInfo_atsGraph>(info)->anchors;
  int j=0;
  for(list<anchor>::iterator a=anchors.begin(); a!=anchors.end(); a++, j++) {
    printAnchor_atsGraph(o, *a, txt()<<"*"<<j<<"*", indent+"    ");
    
    /* // Add an invisible edge from this anchor to the one that immediately follows it to 
    // ensure a left-to-right order among them
    list<anchor>::iterator nextA = a; nextA++;
    if(nextA!=anchors.end())
      o << indent << "    a"<<a->getID() << " -> a"<<nextA->getID() << " [style=invis]" << endl;*/
  }
  
  if(doAnchorRankSame) {
    // Ensure that all anchors in this part are on the same horizontal level
    o << indent << "    {rank=same ";
    for(list<anchor>::iterator a=anchors.begin(); a!=anchors.end(); a++)
      o << "a" << a->getID() << " ";
    o << "}"<<endl;
  }
  
  o << indent << "}" <<endl;
}

// Printer for edges between Part clusters
void printPartEdge_atsGraph(std::ostream &o, map<PartPtr, partDotInfoPtr>& partInfo, PartEdgePtr e, 
                            bool isInEdge, string indent)
{
  /*map<CFGNode, boost::shared_ptr<SgValueExp> > pv = e->getPredicateValue();
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
  }*/
  
  /*partStr psSource = getPartStr(partInfo, e->source());
  partStr psTarget = getPartStr(partInfo, e->target());
  
  o << indent << psSource.anchor << " -> "<<psTarget.anchor << 
       "\t[lhead="<<psSource.part<<", "<<
         " ltail="<<psTarget.part<<", "<<
         " label=\"" << escapeString(values.str()) << "\","<<
         " style=\"" << (isInEdge ? "dotted" : "solid") << "\", " << 
         " color=\"" << color << "\"];\n";*/
}

// Printer for edges between individual anchor nodes 
void printAnchorEdge_atsGraph(std::ostream &o, anchor fromAnchor, PartPtr fromPart, 
                                               anchor toAnchor,   PartPtr toPart, string indent)
{
  if(toAnchor.isLocated()) {
    string style = "solid";
    string color = "black";
    int weight=100;
    
    // If this edge crosses function boundaries, reduce its weight
    set<CFGNode> matchNodes;
    if(toPart->mayIncomingFuncCall(matchNodes) || fromPart->mayOutgoingFuncCall(matchNodes))
      weight = 1;
    
    /*dbg << "a"<<e.getFrom().getID() << " -> a" << e.getTo().getID() << 
         " [style=\"" << style << "\", " << 
          " color=\"" << color << "\"];\n";*/
    o << indent << "a"<<fromAnchor.getID() << " -> a" << toAnchor.getID() << 
         " [style=\"" << style << "\", " << 
          " color=\"" << color << "\", weight="<<weight<<"];\n";
  }
}

class Ctxt2PartsMap_Leaf_Generator_atsGraph : public Ctxt2PartsMap_Leaf_Generator {
  public:
  Ctxt2PartsMap_Leaf* newLeaf() const { return new Ctxt2PartsMap_Leaf_atsGraph(); }
};

/********************
 ***** atsGraph *****
 ********************/

// startPart(s) - the Parts from which the iteration of the ATS should start. The ATS graph performs a forward
//    iteration through the ATS so these should be the entry Part(s).
// partAnchors - maps each Part in the ATS to the anchors that point to blocks associated with it
// dirAligned - true if the edges between anchors are pointing in the same direction as the ATS flow of control
//    and false if they point in the opposite direcction
atsGraph::atsGraph(PartPtr startPart, boost::shared_ptr<map<PartPtr, list<anchor> > > partAnchors, bool dirAligned, int curDebugLevel, int targetDebugLevel) :
  graph(curDebugLevel, targetDebugLevel), partAnchors(partAnchors), dirAligned(dirAligned)
{
  if(!active) return;

  startParts.insert(startPart);
}

atsGraph::atsGraph(std::set<PartPtr>& startParts, boost::shared_ptr<map<PartPtr, list<anchor> > > partAnchors, bool dirAligned, int curDebugLevel, int targetDebugLevel) :
  graph(curDebugLevel, targetDebugLevel), startParts(startParts), partAnchors(partAnchors), dirAligned(dirAligned)
{
  if(!active) return;

}

atsGraph::~atsGraph() {
  if(!active) return;
  
  int partID=0;
  for(map<PartPtr, list<anchor> >::iterator pa=partAnchors->begin(); pa!=partAnchors->end(); pa++, partID++) {
    partInfo[pa->first] = boost::make_shared<partDotInfo_atsGraph>(partID, pa->second);
    
    PartPtr p = pa->first;
    if(ats2DotDebugLevel>=2) dbg << "Adding "<<p->str()<<endl;
  }
  
  for(map<PartPtr, list<anchor> >::iterator pa=partAnchors->begin(); pa!=partAnchors->end(); pa++) {
    // Each anchor must be associated with exactly one Part
    for(list<anchor>::iterator a=pa->second.begin(); a!=pa->second.end(); a++) {
      assert(anchor2Parts.find(*a) == anchor2Parts.end());
      anchor2Parts[*a] = pa->first;
    }
  }

  // Ensure that every Part has at least one anchor associated with it to make it possible to establish
  // edges between Part clusters (graphviz can only set up edges between nodes and edges between clusters
  // are based on edges between their members)
  for(map<PartPtr, partDotInfoPtr>::iterator pi=partInfo.begin(); pi!=partInfo.end(); pi++) {
    if(boost::dynamic_pointer_cast<partDotInfo_atsGraph>(pi->second)->anchors.size()==0) {
      boost::dynamic_pointer_cast<partDotInfo_atsGraph>(pi->second)->anchors.push_back(anchor());
      PartPtr p = pi->first;
      if(ats2DotDebugLevel>=2) dbg << "Blank mapping to "<<p->str()<<endl;
    }
  }
  
  if(!graphOutput) {
    outputCanvizDotGraph(genDotGraph());
  }
}

// Generates and returns the dot graph code for this graph
string atsGraph::genDotGraph()
{
  scope s("dot", scope::medium, ats2DotDebugLevel, 1);
  
  ostringstream o;
  
  o << "digraph atsGraph {"<<endl;
  o << "  compound=true;"<<endl;
  o << "  rankdir="<<(dirAligned? "TD": "DT")<<";"<<endl;
  // Stores all the text generated to specify edges
  ostringstream partEdgesStr;
  
  // Maps contexts to the set of parts in each context
  Ctxt2PartsMap_atsGraph ctxt2parts(false, boost::make_shared<Ctxt2PartsMap_Leaf_Generator_atsGraph>());
  for(fw_partEdgeIterator state(startParts); state!=fw_partEdgeIterator::end(); state++) {
    PartPtr part = state.getPart();
    
    list<list<PartContextPtr> > key = part->getContext()->getDetailedPartContexts();
    if(key.size()==0) {
      DummyContext d;
      key.push_back(d.getSubPartContexts());
    }
    ctxt2parts.insert(key, part);
    
    // Generate edges between part clusters
    if(state.getPartEdge()->source() && state.getPartEdge()->target()) {
      printPartEdge_atsGraph(partEdgesStr, partInfo, state.getPartEdge(), false, "  ");
      //list<anchors>& sAnchors = boost::dynamic_pointer_cast<partDotInfo_atsGraph>(partInfo[state.getPartEdge()->source()])->anchors;
      //list<anchors>& tAnchors = boost::dynamic_pointer_cast<partDotInfo_atsGraph>(partInfo[state.getPartEdge()->target()])->anchors;
    }
  }
  
  ctxt2parts.map2dot(o, partInfo);

  o << partEdgesStr.str();
  
  // Generate edges between anchor nodes
  
  // Between the time when an edge was inserted into edges and now, the anchors on both sides of each
  // edge should have been located (attached to a concrete location in the output). This means that
  // some of the edges are now redundant (e.g. multiple forward edges from one location that end up
  // arriving at the same location). We thus create a new set of edges based on the original list.
  // The set's equality checks will eliminate all duplicates.

  set<graphEdge> uniqueEdges;
  map<anchor, set<anchor> > fromto;
  for(list<graphEdge>::iterator e=edges.begin(); e!=edges.end(); e++) {
    graphEdge edge = *e;
    uniqueEdges.insert(edge);
    fromto[edge.getFrom()].insert(edge.getTo());
  }
  
  {
  scope se("Edges", scope::medium, ats2DotDebugLevel, 1);
  for(set<graphEdge>::iterator e=uniqueEdges.begin(); e!=uniqueEdges.end(); e++) {
  //for(map<anchor, set<anchor> >::iterator from=uniqueEdges.begin(); from!=uniqueEdges.end(); from++) {
  //for(set<anchor>::iterator to=from->second.begin(); to!=from->second.end(); to++) {
    graphEdge edge = *e;
    if(ats2DotDebugLevel>=1) dbg << "edge "<<edge.getFrom().str()<<" => "<<edge.getTo().str()<<endl;
    printAnchorEdge_atsGraph(o, edge.getFrom(), anchor2Parts[edge.getFrom()], 
                                edge.getTo(),   anchor2Parts[edge.getTo()], "  ");
    
    // All the anchors associated with the from anchor's Part
/*    assert(boost::dynamic_pointer_cast<partDotInfo_atsGraph>(partInfo[anchor2Parts[edge.getFrom()]]));
    list<anchor>& fromPartAnchors = boost::dynamic_pointer_cast<partDotInfo_atsGraph>(partInfo[anchor2Parts[edge.getFrom()]])->anchors;
    for(list<anchor>::iterator a=fromPartAnchors.begin(); a!=fromPartAnchors.end(); a++) {
      if(*a != edge.getFrom()) {
        for(set<anchor>::iterator to=fromto[*a].begin(); to!=fromto[*a].end(); to++)
          o << "    a"<<edge.getFrom().getID() << " -> a"<<to->getID() << " [style=\"solid\", color=\"red\"];"<<endl;// [style=invis]" << endl;
      }
    }*/
  }
  }
  
  o << "}"<<endl;
  return o.str();
}

}; // namespace fuse
