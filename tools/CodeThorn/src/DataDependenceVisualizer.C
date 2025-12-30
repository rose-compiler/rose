
#include "rose.h"
#include "DataDependenceVisualizer.h"
#include "VariableIdUtils.h"
#include "CFAnalysis.h"

using namespace std;
using namespace CodeThorn;

class VariableIdSetAttribute;

// public

CodeThorn::DataDependenceVisualizer::DataDependenceVisualizer(Labeler* labeler, VariableIdMapping* varIdMapping, string useDefAttributeName)
  : _showSourceCode(true),
    _labeler(labeler),
    _variableIdMapping(varIdMapping),
    _useDefAttributeName(useDefAttributeName),
    _flow(0),
    //    _dotFunctionClusters(""),
    _dotGraphName("DataDependenceGraph")
{
}

Label CodeThorn::DataDependenceVisualizer::getLabel(SgNode* stmt) {
  return _labeler->getLabel(stmt);
}

SgNode* CodeThorn::DataDependenceVisualizer::getNode(Label label) {
  return _labeler->getNode(label);
}

string CodeThorn::DataDependenceVisualizer::nodeSourceCode(Label lab) {
  if(_labeler->isFunctionEntryLabel(lab))
    return "FunctionEntry";
  if(_labeler->isFunctionExitLabel(lab))
    return "FunctionExit";
  SgNode* node=getNode(lab);
  // all other cases
  return SgNodeHelper::doubleQuotedEscapedString(node->unparseToString());
}

void CodeThorn::DataDependenceVisualizer::generateDotFunctionClusters(SgNode* /*root*/, CFAnalysis* cfanalyzer, string fileName, TopologicalSort* topSort) {

  /*
    generates a new cfg and a dot-cluster for each function and
    computes the inter-procedural edges with interFlow and adds them
    as edges between nodes of the clusters.

    temporary combersome recomputation; TODO: replace this with a proper extraction from the icfg
  */
  Flow& flow=*cfanalyzer->getIcfgFlow();
  LabelSet entryLabels=cfanalyzer->functionEntryLabels(flow);
  InterFlow& iflow=*cfanalyzer->getInterFlow();

  std::ofstream myfile;
  myfile.open(fileName.c_str(),std::ios::out);
  myfile<<"digraph "<<getDotGraphName()<<" {"<<endl;
  int k=0;
  stringstream accessToGlobalVariables; // NOT handling static vars of other functions yet
  for(LabelSet::iterator i=entryLabels.begin();i!=entryLabels.end();++i) {
    stringstream ss;
    SgNode* fdefNode=_labeler->getNode(*i);
    string functionName=SgNodeHelper::getFunctionName(fdefNode);
    ss<<"subgraph cluster_"<<(k++)
      <<" { label=\""<<functionName
      //      <<"\"style=filled; color=lightgrey; \n";
      <<"\"color=blue; \n";
    Flow flow=cfanalyzer->flow(fdefNode);
    cfanalyzer->optimizeFlow(flow); // TODO: reuse flow in cfanalyzer->getFlow() (TODO: cfanalyzer->getFunctionCfg(...))
    flow.setDotOptionHeaderFooter(false);
    ss<<flow.toDot(_labeler,topSort);
    myfile<<ss.str();
    myfile<<"}\n";
  }
  // generate interprocedural edges
  for(InterFlow::iterator i=iflow.begin();i!=iflow.end();++i) {
    if(((*i).call != Labeler::NO_LABEL) && ((*i).entry!= Labeler::NO_LABEL))
      myfile<<(*i).call<<" -> "<<(*i).entry<<";\n";
    if(((*i).exit != Labeler::NO_LABEL) && ((*i).callReturn!= Labeler::NO_LABEL)) {
      myfile<<(*i).exit<<" -> "<<(*i).callReturn<<";\n";
    }
    // generate external edge
    if(((*i).entry == Labeler::NO_LABEL) && ((*i).exit== Labeler::NO_LABEL))
      myfile<<(*i).call<<" -> "<<(*i).callReturn<<" [style=dashed label=external];\n";
    else {
      // if no local edge then generate dotted edge for better layout
      myfile<<(*i).call<<" -> "<<(*i).callReturn<<" [style=dotted arrowhead=none];\n";
    }
    //  cerr<<"WARNING: inconsistent inter-procedural cfg edge: "<<(*i).call<<" -> "<<(*i).callReturn<<endl;
  }
  myfile<<"// access to global variables\n";
  myfile<<accessToGlobalVariables.str();
  myfile<<"}"<<endl;
  myfile.close();
}

void CodeThorn::DataDependenceVisualizer::generateDot(SgNode* /*root*/, string fileName) {
  std::ofstream myfile;
  myfile.open(fileName.c_str(),std::ios::out);
  myfile<<"digraph DataDependence {"<<endl;
  if(_flow) {
    _flow->setDotOptionHeaderFooter(false);
    //_flow->setDotOptionDisplayLabel(true);
    //_flow->setDotOptionDisplayStmt(true);
    //_flow->setDotOptionFixedColor(true);
    myfile<<_flow->toDot(_labeler,0);
  }
  myfile<<"}"<<endl;
  myfile.close();
}

// private

void CodeThorn::DataDependenceVisualizer::setFunctionLabelSetSets(LabelSetSet functionLabelSetSets) {
  _functionLabelSetSets=functionLabelSetSets;
}

void CodeThorn::DataDependenceVisualizer::setDotGraphName(std::string name) {
  _dotGraphName=name;
}

std::string CodeThorn::DataDependenceVisualizer::getDotGraphName() {
  return _dotGraphName;
}

#if 0
void CodeThorn::DataDependenceVisualizer::generateDotFunctionClusters(LabelSetSet functionLabelSetSet) {
  stringstream ss;
  ss<<"// function clusters\n";
  int k=0;
  for(LabelSetSet::iterator i=functionLabelSetSet.begin();i!=functionLabelSetSet.end();++i) {
    cout<<"Generating cluster "<<k<<endl;
    ss<<"subgraph cluster_"<<(k++)
      //      <<" { style=filled; color=lightgrey; ";
      <<" { color=blue; ";
    for(LabelSet::iterator j=(*i).begin();j!=(*i).end();++j) {
      ss<<(*j)<<" ";
    }
    ss<<"}\n";
  }
  _dotFunctionClusters=ss.str();
}
#endif
