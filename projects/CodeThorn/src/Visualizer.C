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
Visualizer::Visualizer(Labeler* l, VariableIdMapping* vim, Flow* f, PStateSet* ss, EStateSet* ess, TransitionGraph* tg):
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
void Visualizer::setLabeler(Labeler* x) { labeler=x; }
void Visualizer::setVariableIdMapping(VariableIdMapping* x) { variableIdMapping=x; }
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
	ss<<pstate->toString(variableIdMapping);
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
	ss<<estateIdStringWithTemporaries(estate);
  }
  if((tg1&&boolOptions["tg1-estate-properties"])||(tg2&&boolOptions["tg2-estate-properties"])) {
	ss<<estate->toString(variableIdMapping);
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

	if(labeler->isStdInLabel((*j)->label())) bgcolor="dodgerblue";
	if(labeler->isStdOutLabel((*j)->label())) bgcolor="orange";
	if(labeler->isStdErrLabel((*j)->label())) bgcolor="orangered";
	if(SgNodeHelper::Pattern::matchAssertExpr(labeler->getNode((*j)->label()))) {bgcolor="black";textcolor="white";}
	// check for start state
	if(transitionGraph->getStartLabel()==(*j)->label()) {bgcolor="white";} 

	// should not be necessary!
#if 0
	if((*j)->io.op==InputOutput::STDIN_VAR) bgcolor="dodgerblue";
	if((*j)->io.op==InputOutput::STDOUT_VAR) bgcolor="orange";
	if((*j)->io.op==InputOutput::STDERR_VAR) bgcolor="orangered";
#endif

	//if((*j)->io.op==InputOutput::FAILED_ASSERT) {bgcolor="black";textcolor="white";}
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
  ss<<"node [shape=box style=filled color=lightgrey];"<<endl;
  for(TransitionGraph::iterator j=transitionGraph->begin();j!=transitionGraph->end();++j) {
	//if((*j).target->io.op==InputOutput::FAILED_ASSERT) continue;
	ss <<"\""<<estateToString((*j).source)<<"\""<< "->" <<"\""<<estateToString((*j).target)<<"\"";
    ss <<" [label=\""<<SgNodeHelper::nodeToString(labeler->getNode((*j).edge.source));
	ss <<"["<<(*j).edge.typesToString()<<"]";
	ss <<"\" ";
	ss <<" color="<<(*j).edge.color()<<" ";
	ss <<" stype="<<(*j).edge.dotEdgeStyle()<<" ";
	ss <<"]"<<";"<<endl;
  }
  tg1=false;
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
	if((*j).target->io.op==InputOutput::FAILED_ASSERT) continue;
	ss <<"L"<<Labeler::labelToString(source->label())<<":";
	ss <<"\"P"<<estateIdStringWithTemporaries(source)<<"\"";
	ss <<"->";
	ss <<"L"<<Labeler::labelToString(target->label())<<":";
	ss <<"\"P"<<estateIdStringWithTemporaries(target)<<"\"";

	ss<<"[";
	ss<<"color="<<(*j).edge.color();
	ss<<" ";
	ss<<"style="<<(*j).edge.dotEdgeStyle();
	ss<<"]";
	ss<<";"<<endl;
    //ss <<" [label=\""<<SgNodeHelper::nodeToString(getLabeler()->getNode((*j).edge.source))<<"\"]"<<";"<<endl;
  }
  ss<<"}\n";
  tg2=false;
  return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// END OF VISUALIZER
////////////////////////////////////////////////////////////////////////////////////////////////////
