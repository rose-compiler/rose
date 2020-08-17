/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

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
#include "tgba/succiter.hh"
#include "tgba/state.hh"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace CodeThorn;
using namespace CodeThorn;

class AssertionAttribute : public DFAstAttribute {
public:
  AssertionAttribute(string preCondition):_precond(preCondition) {}
  string toString() { return _precond; }
private:
  string _precond;
};

AssertionExtractor::AssertionExtractor(Analyzer* analyzer)
{
  setLabeler(analyzer->getLabeler());
  setVariableIdMapping(analyzer->getVariableIdMapping());
  setPStateSet(analyzer->getPStateSet());
  setEStateSet(analyzer->getEStateSet());
  long num=labeler->numberOfLabels();
  assertions.resize(num);
}

void AssertionExtractor::setLabeler(Labeler* x) { labeler=x; }
void AssertionExtractor::setVariableIdMapping(VariableIdMapping* x) { variableIdMapping=x; }
void AssertionExtractor::setPStateSet(PStateSet* x) { pstateSet=x; }
void AssertionExtractor::setEStateSet(EStateSet* x) { estateSet=x; }

void AssertionExtractor::computeLabelVectorOfEStates() {
  for(EStateSet::iterator i=estateSet->begin();i!=estateSet->end();++i) {
    Label lab=(*i)->label();
    const PState* p=(*i)->pstate();
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
      const ConstraintSet* cset=(*i)->constraints();
      string constraintstring=cset->toAssertionString(variableIdMapping);
      if(!isFirst && constraintstring!="") {
        assertions[lab.getId()]+=" && ";
      } else {
        isFirst=false;
      }
      assertions[lab.getId()]+=constraintstring;
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
  pstateSet(0),
  estateSet(0),
  transitionGraph(0),
  tg1(false),
  tg2(false),
  optionTransitionGraphDotHtmlNode(true)
{}

//! The analyzer provides all necessary information
Visualizer::Visualizer(Analyzer* analyzer):
  tg1(false),
  tg2(false),
  optionTransitionGraphDotHtmlNode(true)
{
  setLabeler(analyzer->getLabeler());
  setVariableIdMapping(analyzer->getVariableIdMapping());
  setFlow(analyzer->getFlow());
  setPStateSet(analyzer->getPStateSet());
  setEStateSet(analyzer->getEStateSet());
  setTransitionGraph(analyzer->getTransitionGraph());
}

  //! For providing specific information. For some visualizations not all information is required. The respective set-function can be used as well to set specific program information (this allows to also visualize computed subsets of information (such as post-processed transition graphs etc.).
Visualizer::Visualizer(IOLabeler* l, VariableIdMapping* vim, Flow* f, PStateSet* ss, EStateSet* ess, TransitionGraph* tg):
  labeler(l),
  variableIdMapping(vim),
  flow(f),
  pstateSet(ss),
  estateSet(ess),
  transitionGraph(tg),
  tg1(false),
  tg2(false),
  optionTransitionGraphDotHtmlNode(true)
{}

void Visualizer::setOptionTransitionGraphDotHtmlNode(bool x) {optionTransitionGraphDotHtmlNode=x;}
void Visualizer::setLabeler(IOLabeler* x) { labeler=x; }
void Visualizer::setVariableIdMapping(VariableIdMapping* x) { variableIdMapping=x; }
void Visualizer::setFlow(Flow* x) { flow=x; }
void Visualizer::setPStateSet(PStateSet* x) { pstateSet=x; }
void Visualizer::setEStateSet(EStateSet* x) { estateSet=x; }
void Visualizer::setTransitionGraph(TransitionGraph* x) { transitionGraph=x; }

void Visualizer::setOptionMemorySubGraphs(bool flag) { optionMemorySubGraphs=flag; }
bool Visualizer::getOptionMemorySubGraphs() { return optionMemorySubGraphs; }

/*! 
 * \author Marc Jasper
 * \date 2016.
 */
string Visualizer::cfasToDotSubgraphs(vector<Flow*> cfas) {
  // define a color scheme
  int numColors = 16;
  vector<string> colors(numColors);
  colors[0] = "#6699FF";
  colors[1] = "#7F66FF";
  colors[2] = "#CC66FF";
  colors[3] = "#FF66E6";

  colors[4] = "#66E6FF";
  colors[5] = "#2970FF";
  colors[6] = "#004EEB";
  colors[7] = "#FF6699";

  colors[8] = "#66FFCC";
  colors[9] = "#EB9C00";
  colors[10] = "#FFB829";
  colors[11] = "#FF7F66";

  colors[12] = "#66FF7F";
  colors[13] = "#99FF66";
  colors[14] = "#E6FF66";
  colors[15] = "#FFCC66";

  stringstream ss;
  ss << "digraph G {" << endl;
  for (unsigned int i = 0; i < cfas.size(); ++i) {
    Flow* cfa = cfas[i];
    cfa->setDotOptionHeaderFooter(false);
    cfa->setDotOptionDisplayLabel(true);
    cfa->setDotOptionDisplayStmt(false);
    cfa->setDotOptionEdgeAnnotationsOnly(true);
    cfa->setDotFixedNodeColor(colors[(i % numColors)]);
    ss << "  subgraph component" << i << " {" << endl;
    ss << cfa->toDot(NULL);
    ss << "  }" << endl;
  }
  ss << "}" << endl;
  return ss.str();
}

string Visualizer::pstateToString(const PState* pstate) {
  stringstream ss;
  bool pstateAddressSeparator=false;
  if((tg1&&args.getBool("tg1-pstate-address"))||(tg2&&args.getBool("tg2-pstate-address"))) {
    ss<<"@"<<pstate;
    pstateAddressSeparator=true;
  }    
  if((tg1&&args.getBool("tg1-pstate-id"))||(tg2&&args.getBool("tg2-pstate-id"))) {
    if(pstateAddressSeparator)
      ss<<":";
    ss<<"S"<<pstateSet->pstateId(pstate);
  }
  if((tg1&&args.getBool("tg1-pstate-properties"))||(tg2&&args.getBool("tg2-pstate-properties"))) {
    ss<<pstate->toString(variableIdMapping);
  } 
  return ss.str();
}

string Visualizer::estateToString(const EState* estate) {
  stringstream ss;
  bool pstateAddressSeparator=false;
  if((tg1&&args.getBool("tg1-estate-address"))||(tg2&&args.getBool("tg2-estate-address"))) {
    ss<<"@"<<estate;
    pstateAddressSeparator=true;
  }    
  if((tg1&&args.getBool("tg1-estate-id"))||(tg2&&args.getBool("tg2-estate-id"))) {
    if(pstateAddressSeparator) {
      ss<<":";
    }
    ss<<estateIdStringWithTemporaries(estate);
  }
  if((tg1&&args.getBool("tg1-estate-properties"))||(tg2&&args.getBool("tg2-estate-properties"))) {
    ss<<estate->toString(variableIdMapping);
  } 
  if((tg1&&args.getBool("tg1-estate-predicate"))||(tg2&&args.getBool("tg2-estate-predicate"))) {
    string s=estate->predicateToString(variableIdMapping);
    // replace ASCII with HTML characters
    s=CodeThorn::replace_string(s,",","&and;");
    s=CodeThorn::replace_string(s,"!=","&ne;");
    s=CodeThorn::replace_string(s,"==","=");
    ss<<s;
  }
  return ss.str();
}

string Visualizer::pstateToDotString(const PState* pstate) {
  return string("\""+SgNodeHelper::doubleQuotedEscapedString(pstateToString(pstate))+"\"");
}

string Visualizer::estateToDotString(const EState* estate) {
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
  set<const EState*> estateSetOfLabel=transitionGraph->estateSetOfLabel(lab);
  for(set<const EState*>::iterator j=estateSetOfLabel.begin();j!=estateSetOfLabel.end();++j) {
    // decide on color first
    string textcolor="black";
    string bgcolor="lightgrey";

    if((*j)->isConst(variableIdMapping)) bgcolor="mediumpurple2";
    if(labeler->isStdInLabel((*j)->label())) bgcolor="dodgerblue";
    if(labeler->isStdOutLabel((*j)->label())) bgcolor="orange";
    if(labeler->isStdErrLabel((*j)->label())) bgcolor="orangered";

    if(SgNodeHelper::Pattern::matchAssertExpr(labeler->getNode((*j)->label()))) {bgcolor="black";textcolor="white";}
    if((*j)->io.isFailedAssertIO()) {
      bgcolor="black";textcolor="red";
      // FAILEDASSERTVIS
      continue;
    }

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

string Visualizer::dotEStateAddressString(const EState* estate) {
  stringstream ss;
  ss<<"s"<<estate;
  return ss.str();
}

string Visualizer::dotEStateMemoryString(const EState* estate) {
  string prefix=dotClusterName(estate);
  return estate->pstate()->toDotString(prefix,variableIdMapping);
}

std::string Visualizer::dotClusterName(const EState* estate) {
  return "cluster_"+this->dotEStateAddressString(estate);
}

string Visualizer::transitionGraphToDot() {
  tg1=true;
  EStatePtrSet allEStates=transitionGraph->estateSet();
  stringstream ss;
  ss<<"compound=true;"<<endl; // required for cluster edges to appear
  ss<<"node [shape=box style=filled color=lightgrey];"<<endl;
  // generate all graph node ids with label strings
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

      // deactivated because putting all cluster nodes at the same
      // rank triggers a dot assertion to fail when a node is shared.
#if 0
      
      ss<<"{ rank = same; "; // rank start
      string prefix=dotClusterName(s);
      auto idStringsSet=s->pstate()->getDotNodeIdStrings(prefix);
      for(auto id : idStringsSet) {
        ss<<"\""<<id<<"\""<<";"<<endl;
      }
      ss<<dotEStateAddressString(s)<<"[color=brown label=< <FONT COLOR=\"white\">" "L"+Labeler::labelToString(s->label())+"</FONT> >];"<<endl;
      ss<< " }"<<endl; // rank end
#endif
      ss<<dotEStateAddressString(s)<<"[color=brown label=< <FONT COLOR=\"white\">" "L"+Labeler::labelToString(s->label())+"</FONT> >];"<<endl;
      ss<<dotEStateAddressString(s)<<endl; // hook for cluster edges
      ss<<dotEStateMemoryString(s);
      ss<<"}"<<endl; // end of subgraph
    } 
  }

  for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {

    // // FAILEDASSERTVIS: the next check allows to turn off edges of failing assert to target node (text=red, background=black)
    if((*j)->target->io.op==InputOutput::FAILED_ASSERT) continue;

    ss <<dotEStateAddressString((*j)->source)<< "->"<<dotEStateAddressString((*j)->target);
    ss <<" [label=\""<<SgNodeHelper::nodeToString(labeler->getNode((*j)->edge.source()));
    ss <<"["<<(*j)->edge.typesToString()<<"]";
    ss <<"\" ";
    ss <<" color="<<(*j)->edge.color()<<" ";
    ss <<" stype="<<(*j)->edge.dotEdgeStyle()<<" ";
    if(getOptionMemorySubGraphs()) {
      // change head and tail of arrows for clusters
      ss<<" ltail="<<dotClusterName((*j)->source);
      ss<<" lhead="<<dotClusterName((*j)->target);
    }
    ss<<" penwidth=3.0 weight=1.0"; // bold cfg edges
    ss <<"]"<<";"<<endl;
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
  ss << transitionGraphWithIOToDot(concreteEStates, true, args.getBool("keep-error-states"), false);
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
  for(set<const EState*>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
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
      if(args.getBool("rersmode") && !args.getBool("rers-numeric")) {
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
      if(labeler->isStdInLabel(lab))
        color="dodgerblue";
      if(labeler->isStdOutLabel(lab))
        color="orange";
      if(labeler->isStdErrLabel(lab))
        color="orangered";
      if((*i)->io.op==InputOutput::FAILED_ASSERT||SgNodeHelper::Pattern::matchAssertExpr(labeler->getNode(lab)))
         color="black";
      ss<<" color="<<color<<" style=\"filled\" fontsize=24 ";
      if((*i)->io.isStdErrIO())
        ss <<" fontcolor=orangered "; // do not show input value in stdErr states
      ss<<"];";
      ss<<endl;
    }
    
    stringstream newedges;
    // generate constraint on each edge of following state
    TransitionGraph::TransitionPtrSet outTrans=transitionGraph->outEdges(*i);
#if 0 // debug only
    if ((*i)->io.isStdInIO() && (*i)->isRersTopified(variableIdMapping)) {
      int inputVal = (*i)->determineUniqueIOValue().getIntValue();
      cout << "DEBUG: abstract input " << inputVal << " has "<<  outTrans.size() << " successors." << endl;
    }
#endif
    for(TransitionGraph::TransitionPtrSet::iterator j=outTrans.begin();
    j!=outTrans.end();
    ++j) { 
      const EState* target = (*j)->target;
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
        newedges<<" [label=\"";
        newedges<<(*j)->target->constraints()->toString()<<"\"";
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
  set<const EState*> estatePtrSet=transitionGraph->estateSet();
  for(set<const EState*>::iterator i=estatePtrSet.begin();i!=estatePtrSet.end();++i) {
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
    if(args.getBool("rersmode") && !args.getBool("rers-numeric")) {
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
    stringstream newedges;
    // generate constraint on each edge of following state
    TransitionGraph::TransitionPtrSet outTrans=transitionGraph->outEdges(*i);
    for(TransitionGraph::TransitionPtrSet::iterator j=outTrans.begin();
    j!=outTrans.end();
    ++j) {
      newedges<<"n"<<(*j)->source<<"->"<<"n"<<(*j)->target;
      if(number.isTop()) {
    newedges<<" [label=\"";
    newedges<<(*j)->target->constraints()->toString()<<"\"";
    if((*j)->source==(*j)->target)
      newedges<<" color=black "; // self-edge-color
    newedges<<"];"<<endl;
      }
      newedges<<endl;
    }
    
    // determine color based on IO type
    string color="grey";
    if(labeler->isStdInLabel(lab))
      color="dodgerblue";
    if(labeler->isStdOutLabel(lab))
      color="orange";
    if(labeler->isStdErrLabel(lab))
      color="orangered";


    if((*i)->io.op==InputOutput::FAILED_ASSERT||SgNodeHelper::Pattern::matchAssertExpr(labeler->getNode(lab)))
       color="black";
    ss<<" color="<<color<<" style=\"filled\"";
    ss<<"];";
    ss<<endl;
    ss<<newedges.str();
  }
  return ss.str();
}


string Visualizer::estateIdStringWithTemporaries(const EState* estate) {
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
    const EState* source=(*j)->source;
    const EState* target=(*j)->target;

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

#ifdef HAVE_SPOT
struct spot_state_compare {
  bool operator() (spot::state* const& lhs, spot::state* const& rhs) const {
    if (lhs->compare(rhs) < 0) {
      return true;
    } else {
      return false;
    }
  }
};

/*! 
 * \author Marc Jasper
 * \date 2016.
 */
string Visualizer::spotTgbaToDot(spot::tgba& tgba) {
  stringstream ss;
  ss << "digraph G {" << endl;
  spot::state* initState = tgba.get_init_state();
  list<spot::state*> worklist;
  set<spot::state*, spot_state_compare> added;
  worklist.push_back(initState);
  added.insert(initState);
  while (!worklist.empty()) {
    spot::state* next = worklist.front();
    ss <<"  "<< "\""<<tgba.format_state(next)<<"\" [ label=\"\" ]" << endl;
    worklist.pop_front();
    spot::tgba_succ_iterator* outEdgesIter = tgba.succ_iter(next, NULL, NULL);
    outEdgesIter->first();
    while(!outEdgesIter->done()) {
      spot::state* successor = outEdgesIter->current_state();
      ss <<"  "<< "\""<<tgba.format_state(next)<<"\""<<" -> "<<"\""<<tgba.format_state(successor)<<"\"";
      ss <<" [ label=\""<<tgba.transition_annotation(outEdgesIter)<<"\" ]" << endl;
      if (added.find(successor) == added.end()) {
	worklist.push_back(successor);
	added.insert(successor);
      }
      outEdgesIter->next();
    }
    delete outEdgesIter;
  }
  ss << "}" << endl;
  return ss.str();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// END OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////
