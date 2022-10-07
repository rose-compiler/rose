#include "sage3basic.h"
#include "CodeThornCommandLineOptions.h"
#include "Visualizer.h"
#include "SgNodeHelper.h"
#include "CommandLineOptions.h"
#include "AstAnnotator.h"
#include "AbstractValue.h"
#include "Miscellaneous2.h"
#include <sstream>

#include "rose_config.h"
#ifdef HAVE_SPOT
// SPOT includes
#include "tgba/tgba.hh"
#include "tgba/succiter.hh"
#include "tgba/state.hh"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace CodeThorn;

class AssertionAttribute : public DFAstAttribute {
public:
  AssertionAttribute(string preCondition):_precond(preCondition) {}
  string toString() { return _precond; }
private:
  string _precond;
};

AssertionExtractor::AssertionExtractor(CTAnalysis* analyzer)
{
  setLabeler(analyzer->getLabeler());
  setVariableIdMapping(analyzer->getVariableIdMapping());
  setEStateSet(analyzer->getEStateSet());
  long num=labeler->numberOfLabels();
  assertions.resize(num);
}

void AssertionExtractor::setLabeler(Labeler* x) { labeler=x; }
void AssertionExtractor::setVariableIdMapping(VariableIdMapping* x) { variableIdMapping=x; }
void AssertionExtractor::setEStateSet(EStateSet* x) { estateSet=x; }

void AssertionExtractor::computeLabelVectorOfEStates() {
  for(EStateSet::iterator i=estateSet->begin();i!=estateSet->end();++i) {
    Label lab=(*i)->label();
    PStatePtr p=(*i)->pstate();
    if(assertions[lab.getId()]!="")
      assertions[lab.getId()]+="||";
    assertions[lab.getId()]+="(";
    {
      bool isFirst=true;
      for(PState::const_iterator j=p->begin();j!=p->end();++j) {
        // iterating on the map
        AbstractValue varId=(*j).first;
        if(p->varIsConst(varId)) {
          if(!isFirst) {
            assertions[lab.getId()]+=" && ";
          } else {
            isFirst=false;
          }
          assertions[lab.getId()]+=varId.toString(variableIdMapping)+"=="+p->varValueToString(varId);
        }
      }
      assertions[lab.getId()]+=")";
    }
  }
#if 0
  std::cout<<"Computed Assertions:"<<endl;
  for(size_t i=0;i<assertions[i].size();++i) {
    std::cout<<"@"<<Labeler::labelToString(i)<<": "<<assertions[i]<<std::endl;
  }
#endif
}

void AssertionExtractor::annotateAst() {
  for(size_t i=0;i<assertions.size();i++) {
    if(!labeler->isBlockEndLabel(i)&&!labeler->isFunctionCallReturnLabel(i)&&!labeler->isFunctionExitLabel(i)) {
      SgNode* node=labeler->getNode(i);
      if(node->attributeExists("ctgen-pre-condition"))
        cout << "WARNING: pre-condition already exists. skipping."<<endl;
      else {
        if(assertions[i]!="") {
          node->setAttribute("ctgen-pre-condition",new AssertionAttribute("GENERATED_ASSERT("+assertions[i]+")"));
        }
      }
    }
  }
}

Visualizer::Visualizer():
  labeler(0),
  variableIdMapping(0),
  flow(0),
  estateSet(0),
  transitionGraph(0),
  tg1(false),
  tg2(false),
  optionTransitionGraphDotHtmlNode(true)
{}

//! The analyzer provides all necessary information
Visualizer::Visualizer(CTAnalysis* analyzer):
  tg1(false),
  tg2(false),
  optionTransitionGraphDotHtmlNode(true)
{
  setLabeler(analyzer->getLabeler());
  setVariableIdMapping(analyzer->getVariableIdMapping());
  setFlow(analyzer->getFlow());
  setEStateSet(analyzer->getEStateSet());
  setTransitionGraph(analyzer->getTransitionGraph());
  _ctOpt=analyzer->getOptions();
}

void Visualizer::setOptionTransitionGraphDotHtmlNode(bool x) {optionTransitionGraphDotHtmlNode=x;}
void Visualizer::setLabeler(Labeler* x) { labeler=x; }
void Visualizer::setVariableIdMapping(VariableIdMapping* x) { variableIdMapping=x; }
void Visualizer::setFlow(Flow* x) { flow=x; }
void Visualizer::setEStateSet(EStateSet* x) { estateSet=x; }
void Visualizer::setTransitionGraph(TransitionGraph* x) { transitionGraph=x; }

void Visualizer::setOptionMemorySubGraphs(bool flag) { optionMemorySubGraphs=flag; }
bool Visualizer::getOptionMemorySubGraphs() { return optionMemorySubGraphs; }

string Visualizer::estateToString(EStatePtr estate) {
  stringstream ss;
  bool pstateAddressSeparator=false;
  if((tg1&&_ctOpt.visualization.tg1EStateAddress)||(tg2&&_ctOpt.visualization.tg2EStateAddress)) {
    ss<<"@"<<estate;
    pstateAddressSeparator=true;
  }    
  if((tg1&&_ctOpt.visualization.tg1EStateId)||(tg2&&_ctOpt.visualization.tg2EStateId)) {
    if(pstateAddressSeparator) {
      ss<<":";
    }
    ss<<estateIdStringWithTemporaries(estate);
  }
  if((tg1&&_ctOpt.visualization.tg1EStateProperties)||(tg2&&_ctOpt.visualization.tg2EStateProperties)) {
    ss<<estate->toString(variableIdMapping);
  } 
  return ss.str();
}


string Visualizer::estateToDotString(EStatePtr estate) {
  return string("\""+SgNodeHelper::doubleQuotedEscapedString(estateToString(estate))+"\"");
}

string Visualizer::transitionGraphDotHtmlNode(Label lab) {
  string s;
  s+="L"+Labeler::labelToString(lab)+" [shape=none, margin=0, label=";
  s+="<\n";
  s+="<TABLE BORDER=\"0\"  CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n";
  s+="<TR>\n";
  s+="<TD ROWSPAN=\"1\" BGCOLOR=\"brown\">";
  //s+="\""+SgNodeHelper::nodeToString(labeler->getNode(lab))+"\"";
  s+="<FONT COLOR=\"white\">" "L"+Labeler::labelToString(lab)+"</FONT>";
  s+="</TD>\n";

  string sinline;
  set<EStatePtr> estateSetOfLabel=transitionGraph->estateSetOfLabel(lab);
  for(set<EStatePtr>::iterator j=estateSetOfLabel.begin();j!=estateSetOfLabel.end();++j) {
    // decide on color first
    string textcolor="black";
    string bgcolor="lightgrey";

    if((*j)->isConst(variableIdMapping)) bgcolor="mediumpurple2";
    if(IOLabeler* iolabeler=dynamic_cast<IOLabeler*>(labeler)) {
      if(iolabeler->isStdInLabel((*j)->label())) bgcolor="dodgerblue";
      if(iolabeler->isStdOutLabel((*j)->label())) bgcolor="orange";
      if(iolabeler->isStdErrLabel((*j)->label())) bgcolor="orangered";
      if((*j)->io.isFailedAssertIO()) {
	bgcolor="black";textcolor="red";
	// FAILEDASSERTVIS
	continue;
      }
    }
    if(SgNodeHelper::Pattern::matchAssertExpr(labeler->getNode((*j)->label()))) {bgcolor="black";textcolor="white";}

    // check for start state
    if(transitionGraph->getStartLabel()==(*j)->label()) {bgcolor="white";} 

    sinline+="<TD BGCOLOR=\""+bgcolor+"\" PORT=\"P"+estateIdStringWithTemporaries(*j)+"\">";
    sinline+="<FONT COLOR=\""+textcolor+"\">"+estateToString(*j)+"</FONT>";
    sinline+="</TD>";
  }
  if(sinline=="") {
    // sinline="<TD>empty</TD>";
    // instead of generating empty nodes we do not generate anything for empty nodes
    return "";
  }
  s+=sinline+"</TR>\n";
  s+="</TABLE>";
  s+=">];\n";
  return s;
}

string Visualizer::dotEStateAddressString(EStatePtr estate) {
  stringstream ss;
  ss<<"s"<<estate;
  return ss.str();
}

string Visualizer::dotEStateMemoryString(EStatePtr estate) {
  string prefix=dotClusterName(estate);
  return estate->pstate()->toDotString(prefix,variableIdMapping);
}

std::string Visualizer::dotClusterName(EStatePtr estate) {
  return "cluster_"+this->dotEStateAddressString(estate);
}

string Visualizer::transitionGraphToDot() {
  tg1=true;
  EStatePtrSet allEStates=transitionGraph->estateSet();
  stringstream ss;
  ss<<"compound=true;"<<endl; // required for cluster edges to appear
  ss<<"node [shape=box style=filled color=lightgrey];"<<endl;
  ss<<"graph [ordering=out];"<<endl;
  // generate all graph node ids with label strings
  int numInvisibleLayoutEdges=2; // only used for memory subgraphs

  for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {
    // // FAILEDASSERTVIS: the next check allows to turn off edges of failing assert to target node (text=red, background=black)
    if((*j)->target->io.op==InputOutput::FAILED_ASSERT) continue;

    if(getOptionMemorySubGraphs()) {
      ss <<dotEStateAddressString((*j)->source)<<"_"<<numInvisibleLayoutEdges<< "->"<<dotEStateAddressString((*j)->target); // inter-cluster edge
    } else {
      ss <<dotEStateAddressString((*j)->source)<<"->"<<dotEStateAddressString((*j)->target); // inter-cluster edge
    }
    ss <<" [label=\""<<SgNodeHelper::nodeToString(labeler->getNode((*j)->edge.source()));
    ss <<"["<<(*j)->edge.typesToString()<<"]";
    ss <<"\" ";
    ss <<" color="<<(*j)->edge.color()<<" "; // inter-cluster edge color
    ss <<" stype="<<(*j)->edge.dotEdgeStyle()<<" ";
    if(getOptionMemorySubGraphs()) {
      // change head and tail of arrows for clusters
      ss<<" ltail="<<dotClusterName((*j)->source);
      ss<<" lhead="<<dotClusterName((*j)->target);
    }
    ss<<" penwidth=3.0 weight=1.0"; // bold cfg edges
    ss <<"]"<<";"<<endl;
  }

  if(!getOptionMemorySubGraphs()) {
    // default behavior
    for(auto s : allEStates) {
      ss<<dotEStateAddressString(s)<<" [label="<<estateToDotString(s)<<"];"<<endl;
    }
  } else {
    // memory subgraphs visualization
    // note: for empty clusters: DUMMY_0 [shape=point style=invis]
    // note: minlen=1 on the edges necessary if clusters are only connected vertically (otherwise arrows are collapsed)
    std::set<string> nodes;
    for(auto s : allEStates) {
      ss<<"subgraph "<<dotClusterName(s)<<" {"<<endl;
      ss<<"style=filled; color=lightgrey; node [style=filled,color=white];"<<endl;
      //ss<<"label=\"@"<<s<<"\";"<<endl;
      ss<<"label="<<estateIdStringWithTemporaries(s)<<";"<<endl;

      // generate fake invisible edges inside cluster for better layout
      for(int i=0;i<numInvisibleLayoutEdges;i++) {
	string fakeEdgeSource=dotEStateAddressString(s);
	if(i!=0) {
	  fakeEdgeSource+=("_"+std::to_string(i));
	  ss<<fakeEdgeSource<<" [label=\"\" style = invis]"<<endl;
	}
	ss<< fakeEdgeSource<<"->"<<dotEStateAddressString(s)<<"_"<<i+1<<" [ style=invis ]"<<endl;
      }
      // special case: last node of invisible layout edges (note: numInvisibleLayoutEdges == i+1)
      ss<<dotEStateAddressString(s)<<"_"<<numInvisibleLayoutEdges<<" [label=\"\" style = invis]"<<endl;

      ss<<dotEStateAddressString(s)<<"[color=brown label=< <FONT COLOR=\"white\">" "L"+Labeler::labelToString(s->label())+"</FONT> >];"<<endl;
      ss<<dotEStateAddressString(s)<<endl; // hook for cluster edges
      ss<<dotEStateMemoryString(s);

      ss<<"}"<<endl; // end of subgraph
    } 
  }

  tg1=false;
  return ss.str();
}

/*! 
 * \author Marc Jasper
 * \date 2014, 2015.
 */
string Visualizer::abstractTransitionGraphToDot() {
  stringstream ss;
  string result;
  // group all states into concrete and abstract ones
  EStatePtrSet allEStates=transitionGraph->estateSet();
  EStatePtrSet concreteEStates;
  EStatePtrSet abstractEStates;
  for(EStatePtrSet::iterator i=allEStates.begin(); i!=allEStates.end(); ++i) {
    if ((*i)->isRersTopified(variableIdMapping)) {
      abstractEStates.insert(*i);
    } else {
      concreteEStates.insert(*i);
    } 
  }
  ss << transitionGraphWithIOToDot(concreteEStates, true, _ctOpt.keepErrorStates, false);
  ss << "subgraph cluster_abstractStates {" << endl;
  ss << transitionGraphWithIOToDot(abstractEStates, true, false, true);
  ss << "}" << endl;
  return ss.str();
}

string Visualizer::transitionGraphWithIOToDot(EStatePtrSet displayedEStates,
                                              bool uniteOutputFromAbstractStates, bool includeErrorStates, bool allignAbstractStates) {
  stringstream ss;
  EStatePtrSet estatePtrSet = displayedEStates;
  set<int> outputValues;
  EStatePtrSet abstractInputStates;
  for(set<EStatePtr>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
    if ( !includeErrorStates && ((*i)->io.isStdErrIO() || (*i)->io.isFailedAssertIO()) ){
      continue;
    }
    if(allignAbstractStates && (*i)->io.isStdInIO() && (*i)->isRersTopified(variableIdMapping)) {
      abstractInputStates.insert(*i);
    }
    //represent the current EState
    bool displayCurrentState = true;
    if ( uniteOutputFromAbstractStates && (*i)->io.isStdOutIO() && (*i)->isRersTopified(variableIdMapping) ) {
      int outputVal = (*i)->determineUniqueIOValue().getIntValue();
      pair<set<int>::iterator, bool> notYetSeen = outputValues.insert(outputVal);
      if (notYetSeen.second == true) {
        stringstream temp;
        temp << "abstractOutput" << outputVal;
        string outputStateName = temp.str();
        ss<< outputStateName <<" [label=";
      } else {
        displayCurrentState=false;
      }
    } else {
      ss<<"n"<<*i<<" [label=";
    }
    AbstractValue number=(*i)->determineUniqueIOValue();
    if (displayCurrentState) {
      // generate number which is used in IO operation
      string name="\"";
      if(_ctOpt.rers.rersMode && !_ctOpt.rers.rersNumeric) {
        if(!number.isTop() && !number.isBot()) {
          // convert number to letter
          int num=number.getIntValue();
          num+='A'-1;
          char numc=num;
          stringstream ss;
          ss<<numc;
          name+=ss.str();
        } else {
          name+=number.toString();
        }
      } else {
        name+=number.toString();
      }
      name+="\"";
      ss<<name;
      // determine color based on IO type
      Label lab=(*i)->label();
      string color="grey";

      if(IOLabeler* iolabeler=dynamic_cast<IOLabeler*>(labeler)) {
	if(iolabeler->isStdInLabel(lab))
	  color="dodgerblue";
	if(iolabeler->isStdOutLabel(lab))
	  color="orange";
	if(iolabeler->isStdErrLabel(lab))
	  color="orangered";
	if((*i)->io.op==InputOutput::FAILED_ASSERT||SgNodeHelper::Pattern::matchAssertExpr(iolabeler->getNode(lab)))
	  color="black";
	if((*i)->io.isStdErrIO())
	  ss <<" fontcolor=orangered "; // do not show input value in stdErr states
      }
      ss<<" color="<<color<<" style=\"filled\" fontsize=24 ";
      ss<<"];";
      ss<<endl;
    }
    
    stringstream newedges;
    // generate constraint on each edge of following state
    TransitionGraph::TransitionPtrSet outTrans=transitionGraph->outEdges(*i);
#if 0 // debug only
    if ((*i)->io.isStdInIO() && (*i)->isRersTopified(variableIdMapping)) {
      int inputVal = (*i)->determineUniqueIOValue().getIntValue();
      //cout << "DEBUG: abstract input " << inputVal << " has "<<  outTrans.size() << " successors." << endl;
    }
#endif
    for(TransitionGraph::TransitionPtrSet::iterator j=outTrans.begin();
    j!=outTrans.end();
    ++j) { 
      EStatePtr target = (*j)->target;
      if ( !includeErrorStates && (target->io.isStdErrIO() || target->io.isFailedAssertIO()) ){
        continue;
      }
      if ( uniteOutputFromAbstractStates && (*i)->io.isStdOutIO() && (*i)->isRersTopified(variableIdMapping) ) {
        int outputVal = (*i)->determineUniqueIOValue().getIntValue();
        newedges<< "abstractOutput" << outputVal<<"->";
      } else {
        newedges<<"n"<<(*j)->source<<"->";
      }
      if ( uniteOutputFromAbstractStates && target->io.isStdOutIO() && target->isRersTopified(variableIdMapping) ) {
        int targetOutputVal = (*j)->target->determineUniqueIOValue().getIntValue();
        newedges<< "abstractOutput" << targetOutputVal;
      } else {
        newedges<<"n"<<(*j)->target;
      }
      if(number.isTop()) {
        newedges<<" [label=\""<<"top"<<"\"";
      if((*j)->source==(*j)->target)
        newedges<<" color=black "; // self-edge-color
        newedges<<"];"<<endl;
      }
      newedges<<endl;
    }
    ss<<newedges.str();
  }

  // allign abstract input and output states
  if (allignAbstractStates) {
    ss << "{ rank=same;";
    for (EStatePtrSet::iterator i = abstractInputStates.begin(); i != abstractInputStates.end(); ++i) {
      ss << " n" << (*i); 
    }
    ss << " }" << endl;
    ss << "{ rank=same;";
    for (set<int>::iterator i = outputValues.begin(); i != outputValues.end(); ++i) {
      ss << " abstractOutput" << (*i); 
    }
    ss << " }" << endl;
  }
  return ss.str();
}

string Visualizer::transitionGraphWithIOToDot() {
  stringstream ss;
#if 0
  for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {
    // do not generate input->output edges here because we generate them later
    if(!(labeler->isStdInLabel((*j).source->label()) && labeler->isStdOutLabel((*j).source->label()))) {
      ss<<"n"<<(*j).source<<"->"<<"n"<<(*j).target<<";"<<endl;
    }
  }
#endif
  set<EStatePtr> estatePtrSet=transitionGraph->estateSet();
  for(set<EStatePtr>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
    ss<<"n"<<*i<<" [label=";
    Label lab=(*i)->label();
    string name="\"";
#if 0
    stringstream ss2;
    ss2<<lab;
    name+="L"+ss2.str()+":";

    // determine whether it is a call node (C) or a call-return node (R)
    if(labeler->isFunctionCallLabel(lab))
      name+="C:";
    if(labeler->isFunctionCallReturnLabel(lab))
      name+="R:";
#endif
    // generate number which is used in IO operation
    AbstractValue number=(*i)->determineUniqueIOValue();
    if(_ctOpt.rers.rersMode && !_ctOpt.rers.rersNumeric) {
      if(!number.isTop() && !number.isBot()) {
        // convert number to letter
        int num=number.getIntValue();
        num+='A'-1; // TODO: introduce RERS mapping 2020
        char numc=num;
        stringstream ss;
        ss<<numc;
        name+=ss.str();
      } else {
        name+=number.toString();
      }
    } else {
      name+=number.toString();
    }
    name+="\"";
    ss<<name;
    stringstream newedges;
    // generate constraint on each edge of following state
    TransitionGraph::TransitionPtrSet outTrans=transitionGraph->outEdges(*i);
    for(TransitionGraph::TransitionPtrSet::iterator j=outTrans.begin();
    j!=outTrans.end();
    ++j) {
      newedges<<"n"<<(*j)->source<<"->"<<"n"<<(*j)->target;
      if(number.isTop()) {
	newedges<<" [label=\""<<"top"<<"\"";
    if((*j)->source==(*j)->target)
      newedges<<" color=black "; // self-edge-color
    newedges<<"];"<<endl;
      }
      newedges<<endl;
    }
    
    // determine color based on IO type
    string color="grey";
    if(IOLabeler* iolabeler=dynamic_cast<IOLabeler*>(labeler)) {
      if(iolabeler->isStdInLabel(lab))
	color="dodgerblue";
      if(iolabeler->isStdOutLabel(lab))
	color="orange";
      if(iolabeler->isStdErrLabel(lab))
	color="orangered";
    }
    if((*i)->io.op==InputOutput::FAILED_ASSERT||SgNodeHelper::Pattern::matchAssertExpr(labeler->getNode(lab)))
      color="black";
    ss<<" color="<<color<<" style=\"filled\"";
    ss<<"];";
    ss<<endl;
    ss<<newedges.str();
  }
  return ss.str();
}


string Visualizer::estateIdStringWithTemporaries(EStatePtr estate) {
  stringstream ss;
  EStateId estateId=estateSet->estateId(estate);
  if(estateId!=NO_ESTATE) {
    ss<<"ES"<<estateSet->estateId(estate);
  } else {
    ss<<"TES"<<estate;
  }
  return ss.str();
}

/*! 
 * \author Marc Jasper
 * \date 2016.
 */
string Visualizer::visualizeReadWriteAccesses(IndexToReadWriteDataMap& indexToReadWriteDataMap, VariableIdMapping* variableIdMapping, 
					      ArrayElementAccessDataSet& readWriteRaces, ArrayElementAccessDataSet& writeWriteRaces, 
					      bool arrayElementsAsPoints, bool useClusters, bool prominentRaceWarnings) {
      stringstream dot;
      map<string, string> clusters;
      string racePointSizeStr = "0.5";
      dot << "digraph G {" << endl;
      for(IndexToReadWriteDataMap::iterator imap=indexToReadWriteDataMap.begin();
          imap!=indexToReadWriteDataMap.end();
          ++imap) {
        IndexVector index=(*imap).first;
	int parallel_loop_id = *(--index.end());
      	dot << boost::lexical_cast<string>(parallel_loop_id)<<" [fontsize=16]" << endl;
        for(ArrayElementAccessDataSet::const_iterator i=indexToReadWriteDataMap[index].readArrayAccessSet.begin();i!=indexToReadWriteDataMap[index].readArrayAccessSet.end();++i) {
	  string node_shape;
	  string edge_color;
	  // highlight data races
	  if (readWriteRaces.find(*i) != readWriteRaces.end()) {
	    if (prominentRaceWarnings) {
	      node_shape = "[label = \"R/W race\", shape = doubleoctagon, style=filled, fillcolor = red, fontcolor=white, fontsize=18]";
	      edge_color = "red";
	    } else {
	      if (arrayElementsAsPoints) {
		node_shape = "[label = \"\", shape = point, fillcolor=red, color=red, style=filled, width="+racePointSizeStr+", height="+racePointSizeStr+"]";
	      } else {
		node_shape = "[shape = rectangle, fillcolor=red, color=red, style=filled, width="+racePointSizeStr+", height="+racePointSizeStr+"]";
	      }
	      edge_color = "blue";	      
	    }
	  } else {
	    if (arrayElementsAsPoints) {
	      node_shape = "[label = \"\", shape = point]";
	    } else {
	      node_shape = "[shape = rectangle]";
	    }
            edge_color = "blue";
	  }
	  // record read edge
          dot<<"  "<<"\""<<(*i).toStringNoSsaSubscript(variableIdMapping)<<"\""<<" -> "<<parallel_loop_id<<"[color="<<edge_color<<"]"<<endl;
	  // add details for the node representing the array element access
	  string varName = variableIdMapping->variableName(i->getVariable());
	  map<string, string>::iterator iter = clusters.find(varName);
	  if (iter != clusters.end()) {
	    iter->second += "\"" + (*i).toStringNoSsaSubscript(variableIdMapping) + "\"" + node_shape + "\n";
	  } else {
	    string clusterstring;
	    if (useClusters) {
	      clusterstring = "subgraph cluster_" + varName + " { \n";
	    }
	    clusterstring += "\"" + (*i).toStringNoSsaSubscript(variableIdMapping) + "\"" + node_shape + "\n";
	    clusters[varName] = clusterstring;
	  }
        }
        for(ArrayElementAccessDataSet::const_iterator i=indexToReadWriteDataMap[index].writeArrayAccessSet.begin();i!=indexToReadWriteDataMap[index].writeArrayAccessSet.end();++i) {
	  string node_shape;
	  string edge_color;
	  if (readWriteRaces.find(*i) != readWriteRaces.end()) {
	    if (prominentRaceWarnings) {
	      node_shape = "[label = \"R/W race\", shape = doubleoctagon, style=filled, fillcolor = red, fontcolor=white, fontsize=18]";
	      edge_color = "red";
	    } else {
	      if (arrayElementsAsPoints) {
		node_shape = "[label = \"\", shape = point, fillcolor=red, color=red, style=filled, width="+racePointSizeStr+", height="+racePointSizeStr+"]";
	      } else {
		node_shape = "[shape = rectangle, fillcolor=red, color=red, style=filled, width="+racePointSizeStr+", height="+racePointSizeStr+"]";
	      }
	      edge_color = "\"#47A11D\"";	      
	    }
	  } else if (writeWriteRaces.find(*i) != writeWriteRaces.end()) {
	    if (prominentRaceWarnings) {
	      node_shape = "[label = \"W/W race\", shape = doubleoctagon, style=filled, fillcolor = red, fontcolor=white, fontsize=18]";
	      edge_color = "red";
	    } else {
	      if (arrayElementsAsPoints) {
		node_shape = "[label = \"\", shape = point, fillcolor=red, color=red, style=filled, width="+racePointSizeStr+", height="+racePointSizeStr+"]";
	      } else {
		node_shape = "[shape = rectangle, fillcolor=red, color=red, style=filled, width="+racePointSizeStr+", height="+racePointSizeStr+"]";
	      }
	      edge_color = "\"#47A11D\"";	      
	    }
	  } else {
	    if (arrayElementsAsPoints) {
	      node_shape = "[label = \"\", shape = point]";
	    } else {
	      node_shape = "[shape = rectangle]";
	    }
	    edge_color = "\"#47A11D\"";
	  }
	  // record write edge
          dot<<"  "<<parallel_loop_id<<" -> "<<"\""<<(*i).toStringNoSsaSubscript(variableIdMapping)<<"\""<<"[color="<<edge_color<<"]"<<endl;
	  // add details for the node representing the array element access
	  string varName = variableIdMapping->variableName(i->getVariable());
	  map<string, string>::iterator iter = clusters.find(varName);
	  if (iter != clusters.end()) {
	    iter->second += "\"" + (*i).toStringNoSsaSubscript(variableIdMapping) + "\"" + node_shape + "\n";
	  } else {
	    string clusterstring;
	    if (useClusters) {
	      clusterstring = "subgraph cluster_" + varName + " { \n";
	    }
	    clusterstring += "\"" + (*i).toStringNoSsaSubscript(variableIdMapping) + "\"" + node_shape + "\n";
	    clusters[varName] = clusterstring;
	  }
        }
      }
      for (map<string, string>::iterator k = clusters.begin(); k !=clusters.end(); k++) {
	if (useClusters) {
	  (k->second) += "} \n";
	}
	dot << k->second << endl;
      }
      dot << "}" << endl;
      return dot.str();
 }

string Visualizer::foldedTransitionGraphToDot() {
  tg2=true;
  stringstream ss;
  size_t gSize=transitionGraph->size();
  LabelSet labelSet=flow->nodeLabels();
  size_t labelSetSize=labelSet.size();
  size_t reportInterval=100;
  cout<<"INFO: generating folded state graph: "<<labelSetSize<<" nodes, "<<gSize<<" edges"<<endl;

  // generate graph
  ss<<"digraph html {\n";
  // generate nodes
  size_t labelNr=0;
  for(LabelSet::iterator i=labelSet.begin();i!=labelSet.end();++i) {
    ss<<transitionGraphDotHtmlNode(*i);
    if(labelSetSize>reportInterval && labelNr%reportInterval==0) {
      cout<<"INFO: generating label "<<labelNr<<" of "<<labelSetSize<<endl;
    }
    labelNr++;
  }
  // generate edges
  size_t edgeNr=0;
   for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {
    EStatePtr source=(*j)->source;
    EStatePtr target=(*j)->target;

    if(gSize>reportInterval && edgeNr%reportInterval==0) {
      cout<<"INFO: generating transition "<<edgeNr<<" of "<<gSize<<endl;
    }
    edgeNr++;
    // FAILEDASSERTVIS: the next check allows to turn off edges of failing assert to target node (text=red, background=black)
    if((*j)->target->io.op==InputOutput::FAILED_ASSERT) continue;

    ss <<"L"<<Labeler::labelToString(source->label())<<":";
    ss <<"\"P"<<estateIdStringWithTemporaries(source)<<"\"";
    ss <<"->";
    ss <<"L"<<Labeler::labelToString(target->label())<<":";
    ss <<"\"P"<<estateIdStringWithTemporaries(target)<<"\"";

    ss<<"[";
    ss<<"color="<<(*j)->edge.color();
    ss<<" ";
    ss<<"style="<<(*j)->edge.dotEdgeStyle();
    ss<<"]";
    ss<<";"<<endl;
    // ss <<" [label=\""<<SgNodeHelper::nodeToString(getLabeler()->getNode((*j).edge.source))<<"\"]"<<";"<<endl;
  }
  ss<<"}\n";
  tg2=false;
  return ss.str();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// END OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////
