/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Visualizer.h"
#include "SgNodeHelper.h"
#include "CommandLineOptions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace CodeThorn;

Visualizer::Visualizer():
  tg1(false),
  tg2(false),
  optionTransitionGraphDotHtmlNode(true),
  labeler(0),
  flow(0),
  pstateSet(0),
  estateSet(0),
  transitionGraph(0)
{}

//! The analyzer provides all necessary information
Visualizer::Visualizer(Analyzer* analyzer):
  tg1(false),
  tg2(false),
  optionTransitionGraphDotHtmlNode(true)
{
  setLabeler(analyzer->getLabeler());
  setFlow(analyzer->getFlow());
  setPStateSet(analyzer->getPStateSet());
  setEStateSet(analyzer->getEStateSet());
  setTransitionGraph(analyzer->getTransitionGraph());
}

  //! For providing specific information. For some visualizations not all information is required. The respective set-function can be used as well to set specific program information (this allows to also visualize computed subsets of information (such as post-processed transition graphs etc.).
Visualizer::Visualizer(Labeler* l, Flow* f, PStateSet* ss, EStateSet* ess, TransitionGraph* tg):
  tg1(false),
  tg2(false),
  optionTransitionGraphDotHtmlNode(true),
  labeler(l),
  flow(f),
  pstateSet(ss),
  estateSet(ess),
  transitionGraph(tg)
{}

void Visualizer::setOptionTransitionGraphDotHtmlNode(bool x) {optionTransitionGraphDotHtmlNode=x;}
void Visualizer::setLabeler(Labeler* x) { labeler=x; }
void Visualizer::setFlow(Flow* x) { flow=x; }
void Visualizer::setPStateSet(PStateSet* x) { pstateSet=x; }
void Visualizer::setEStateSet(EStateSet* x) { estateSet=x; }
void Visualizer::setTransitionGraph(TransitionGraph* x) { transitionGraph=x; }

string Visualizer::pstateToString(const PState* pstate) {
  stringstream ss;
  bool pstateAddressSeparator=false;
  if((tg1&&boolOptions["tg1-pstate-address"])||(tg2&&boolOptions["tg2-pstate-address"])) {
	ss<<"@"<<pstate;
	pstateAddressSeparator=true;
  }	
  if((tg1&&boolOptions["tg1-pstate-id"])||(tg2&&boolOptions["tg2-pstate-id"])) {
	if(pstateAddressSeparator)
	  ss<<":";
	ss<<"S"<<pstateSet->pstateId(pstate);
  }
  if((tg1&&boolOptions["tg1-pstate-properties"])||(tg2&&boolOptions["tg2-pstate-properties"])) {
	ss<<pstate->toString();
  } 
  return ss.str();
}

string Visualizer::estateToString(const EState* estate) {
  stringstream ss;
  bool pstateAddressSeparator=false;
  if((tg1&&boolOptions["tg1-estate-address"])||(tg2&&boolOptions["tg2-estate-address"])) {
	ss<<"@"<<estate;
	pstateAddressSeparator=true;
  }	
  if((tg1&&boolOptions["tg1-estate-id"])||(tg2&&boolOptions["tg2-estate-id"])) {
	if(pstateAddressSeparator) {
	  ss<<":";
	}
	ss<<"ES"<<estateSet->estateId(estate);
  }
  if((tg1&&boolOptions["tg1-estate-properties"])||(tg2&&boolOptions["tg2-estate-properties"])) {
	ss<<estate->toString();
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
	if((*j)->io.op==InputOutput::STDIN_VAR) bgcolor="dodgerblue";
	if((*j)->io.op==InputOutput::STDOUT_VAR) bgcolor="orange";
	if((*j)->io.op==InputOutput::STDERR_VAR) bgcolor="orangered";
	if((*j)->io.op==InputOutput::FAILED_ASSERT) {bgcolor="black";textcolor="white";}
	sinline+="<TD BGCOLOR=\""+bgcolor+"\" PORT=\"P"+estateSet->estateIdString(*j)+"\">";
	sinline+="<FONT COLOR=\""+textcolor+"\">"+estateToString(*j)+"</FONT>";
	sinline+="</TD>";
  }
  if(sinline=="") {
	sinline="<TD>empty</TD>";
	// instead of generating empty nodes we do not generate anything for empty nodes
	return "";
  }
  s+=sinline+"</TR>\n";
  s+="</TABLE>";
  s+=">];\n";
  return s;
}

#if 0
string Visualizer::transitionGraphToDot() {
  stringstream ss;
  for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {
	ss <<"\""<<estateToString((*j).source)<<"\""<< "->" <<"\""<<estateToString((*j).target)<<"\"";
    ss <<" [label=\""<<SgNodeHelper::nodeToString(labeler->getNode((*j).edge.source))<<"\"]"<<";"<<endl;
  }
  return ss.str();
}
#endif

string Visualizer::transitionGraphToDot() {
  tg1=true;
  stringstream ss;
  for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {
	ss <<"\""<<estateToString((*j).source)<<"\""<< "->" <<"\""<<estateToString((*j).target)<<"\"";
    ss <<" [label=\""<<SgNodeHelper::nodeToString(labeler->getNode((*j).edge.source));
	ss <<"["<<(*j).edge.typeToString()<<"]";
	ss <<"\"]"<<";"<<endl;
  }
  tg1=false;
  return ss.str();
}

string Visualizer::foldedTransitionGraphToDot() {
  tg2=true;
  stringstream ss;
  ss<<"digraph html {\n";
  // generate nodes
  LabelSet labelSet=flow->nodeLabels();
  for(LabelSet::iterator i=labelSet.begin();i!=labelSet.end();++i) {
	ss<<transitionGraphDotHtmlNode(*i);
  }
  // generate edges
  for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {
	const EState* source=(*j).source;
	const EState* target=(*j).target;
	ss <<"L"<<Labeler::labelToString(source->label())<<":"<<"\"P"<<estateSet->estateId(source)<<"\""
	   <<"->"
	   <<"L"<<Labeler::labelToString(target->label())<<":"<<"\"P"<<estateSet->estateId(target)<<"\"";
	if((*j).edge.type==EDGE_TRUE) ss<<"[color=green]";
	if((*j).edge.type==EDGE_FALSE) ss<<"[color=red]";
	if((*j).edge.type==EDGE_BACKWARD) ss<<"[color=blue]";
	ss << ";"<<endl;
    //ss <<" [label=\""<<SgNodeHelper::nodeToString(getLabeler()->getNode((*j).edge.source))<<"\"]"<<";"<<endl;
  }
  ss<<"}\n";
  tg2=false;
  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// END OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////
