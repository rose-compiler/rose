/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Visualizer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////

Visualizer::Visualizer():
  optionStateId(true),
  optionStateProperties(false),
  optionEStateLabel(false),
  optionEStateId(false),
  optionEStateProperties(false),
  optionTransitionGraphDotHtmlNode(true),
  labeler(0),
  flow(0),
  stateSet(0),
  eStateSet(0),
  transitionGraph(0)
{}

//! The analyzer provides all necessary information
Visualizer::Visualizer(Analyzer* analyzer):
  optionStateId(true),
  optionStateProperties(false),
  optionEStateLabel(false),
  optionEStateId(true),
  optionEStateProperties(false),
  optionTransitionGraphDotHtmlNode(true) 
{
  setLabeler(analyzer->getLabeler());
  setFlow(analyzer->getFlow());
  setStateSet(analyzer->getStateSet());
  setEStateSet(analyzer->getEStateSet());
  setTransitionGraph(analyzer->getTransitionGraph());
}

  //! For providing specific information. For some visualizations not all information is required. The respective set-function can be used as well to set specific program information (this allows to also visualize computed subsets of information (such as post-processed transition graphs etc.).
Visualizer::Visualizer(Labeler* l, Flow* f, StateSet* ss, EStateSet* ess, TransitionGraph* tg):
  optionStateId(true),
  optionStateProperties(false),
  optionEStateLabel(false),
  optionEStateId(true),
  optionEStateProperties(false),
  optionTransitionGraphDotHtmlNode(true),
  labeler(l),
  flow(f),
  stateSet(ss),
  eStateSet(ess),
  transitionGraph(tg)
{}

void Visualizer::setOptionStateId(bool x) {optionStateId=x;}
void Visualizer::setOptionStateProperties(bool x) {optionStateProperties=x;}
void Visualizer::setOptionEStateLabel(bool x) {optionEStateLabel=x;}
void Visualizer::setOptionEStateId(bool x) {optionEStateId=x;}
void Visualizer::setOptionEStateProperties(bool x) {optionEStateProperties=x;}
void Visualizer::setOptionTransitionGraphDotHtmlNode(bool x) {optionTransitionGraphDotHtmlNode=x;}
void Visualizer::setLabeler(Labeler* x) { labeler=x; }
void Visualizer::setFlow(Flow* x) { flow=x; }
void Visualizer::setStateSet(StateSet* x) { stateSet=x; }
void Visualizer::setEStateSet(EStateSet* x) { eStateSet=x; }
void Visualizer::setTransitionGraph(TransitionGraph* x) { transitionGraph=x; }

string Visualizer::stateToString(const State* state) {
  stringstream ss;
  if(optionStateId) {
	ss<<"PS"<<stateSet->stateId(state);
  } else {
	ss<< state;
  }
  return ss.str();
}

string Visualizer::eStateToString(const EState* eState) {
  stringstream ss;
  if(optionEStateId) {
	ss<<"ES"<<eStateSet->eStateId(eState);
  } else {
	ss<< eState;
  }
  return ss.str();
}

string Visualizer::transitionGraphToDot() {
  stringstream ss;
  for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {
	ss <<"\""<<eStateToString((*j).source)<<"\""<< "->" <<"\""<<eStateToString((*j).target)<<"\"";
    ss <<" [label=\""<<SgNodeHelper::nodeToString(labeler->getNode((*j).edge.source))<<"\"]"<<";"<<endl;
  }
  return ss.str();
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
  set<const EState*> eStateSet=transitionGraph->eStateSetOfLabel(lab);
  for(set<const EState*>::iterator j=eStateSet.begin();j!=eStateSet.end();++j) {
	// decide on color first
	string color="lightgrey";
	if((*j)->io.op==InputOutput::IN_VAR) color="dodgerblue";
	if((*j)->io.op==InputOutput::OUT_VAR) color="orange";
	sinline+="<TD BGCOLOR=\""+color+"\" PORT=\""+eStateToString(*j)+"\">";
	sinline+=eStateToString(*j);
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

string Visualizer::foldedTransitionGraphToDot() {
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
	ss <<"L"<<Labeler::labelToString(source->label)<<":"<<"\""<<eStateToString(source)<<"\""
	   <<"->"
	   <<"L"<<Labeler::labelToString(target->label)<<":"<<"\""<<eStateToString(target)<<"\"";
	if((*j).edge.type==EDGE_TRUE) ss<<"[color=green]";
	if((*j).edge.type==EDGE_FALSE) ss<<"[color=red]";
	if((*j).edge.type==EDGE_BACKWARD) ss<<"[color=blue]";
	ss << ";"<<endl;
    //ss <<" [label=\""<<SgNodeHelper::nodeToString(getLabeler()->getNode((*j).edge.source))<<"\"]"<<";"<<endl;
  }
  ss<<"}\n";
  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// END OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////
