// stats test

#include "rose.h"
#include "ProgramStats.h"

void ProgramStatistics::computeOpStats(ComputationInfo& ci, SgNode* node) {
  RoseAst ast(node);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
	if(isSgAddOp(*i)||isSgDivideOp(*i)||isSgSubtractOp(*i)||isSgModOp(*i)||isSgMultiplyOp(*i)) {
	  ci.numArithOp[CIT_TOTAL]++;
	  if(SgExpression* expr=isSgExpression(*i)) {
		SgType* typenode=expr->get_type();
		if(typenode) {
		  if(isSgTypeFloat(typenode)||isSgTypeDouble(typenode)||isSgTypeLongDouble(typenode)) {
			ci.numArithOp[CIT_FLOAT]++;
		  } else if(isSgTypeInt(typenode)
					||isSgTypeLong(typenode)
					||isSgTypeLongDouble(typenode)
					||isSgTypeLongLong(typenode)
					||isSgTypeShort(typenode)
					||isSgTypeSignedChar(typenode)
					||isSgTypeSignedInt(typenode)
					||isSgTypeSignedLong(typenode)
					||isSgTypeSignedLongLong(typenode)
					||isSgTypeSignedShort(typenode)
					||isSgTypeUnsignedChar(typenode)
					||isSgTypeUnsignedInt(typenode)
					||isSgTypeUnsignedLong(typenode)
					||isSgTypeUnsignedLongLong(typenode)
					||isSgTypeUnsignedShort(typenode)
					) {
			ci.numArithOp[CIT_INT]++;
		  }
		}
	  }
	}
	if(isSgBitAndOp(*i)||isSgBitOrOp(*i)||isSgBitXorOp(*i))
	  ci.numBitOp++;
  }
}

ComputationInfo ProgramStatistics::computeComputationInfo(Label lab, VariableIdMapping* vidm) {
  SgNode* node=labeler->getNode(lab);
  ComputationInfo ci;
  //cout<<ci.toString()<<endl;
  if(isSgVariableDeclaration(node)||isSgExpression(node)||isSgInitializedName(node)) {
	VariableIdSet readMemLocSet=AnalysisAbstractionLayer::useVariables(node, *vidm);
	VariableIdSet writeMemLocSet=AnalysisAbstractionLayer::defVariables(node, *vidm);
	ci.numReadMemLoc[CIT_TOTAL]=readMemLocSet.size();
	ci.numWriteMemLoc[CIT_TOTAL]=writeMemLocSet.size();
	//if(writeMemLocSet.size()>0) cout<<"found SOMETHING!"<<ci.numWriteMemLoc[CIT_TOTAL]<<endl;
  }
  computeOpStats(ci,node);
  
  // check whether this is a condition node
  if(labeler->isConditionLabel(lab))
	ci.numConditionTest++;

  // if there is more than one predecessor then we mark those predecessors as jmp (exluding back-edges!)
  Flow inEdges=icfg->inEdges(lab);
  inEdges.deleteEdges(EDGE_BACKWARD); // only exists for loop-conditions, join-nodes of branch-stmts may not have any backward-edge
  if(inEdges.size()>1) {
	for(EdgeSet::iterator i=inEdges.begin();i!=inEdges.end();++i) {
	  computationInfo[(*i).source].numJmp++;
	}
  }
  Flow specialEdges=icfg->edgesOfType(EDGE_BACKWARD);
  Flow callEdges=icfg->edgesOfType(EDGE_CALL);
  Flow callreturnEdges=icfg->edgesOfType(EDGE_CALLRETURN);
  specialEdges+=callEdges;
  specialEdges+=callreturnEdges;
  for(EdgeSet::iterator i=specialEdges.begin();i!=specialEdges.end();++i) {
	computationInfo[(*i).source].numJmp++;
  }
  return ci;
}

ProgramStatistics::ProgramStatistics(VariableIdMapping* vidm, Labeler* labeler, Flow* icfg, string useDefAstAttributeName)
  :vidm(vidm),
   labeler(labeler),
   icfg(icfg),
   useDefAstAttributeName(useDefAstAttributeName)
{
  computationInfo.resize(labeler->numberOfLabels());
}

void ProgramStatistics::computeStatistics() {
  LabelSet labSet=icfg->nodeLabels();
  //long labelNum=labeler->numberOfLabels();
  for(LabelSet::iterator i=labSet.begin();i!=labSet.end();++i) {
	Label lab=*i;
	//	UDAstAttribute* node0=dynamic_cast<UDAstAttribute*>(node->getAttribute(useDefAstAttributeName));
	cout<<"computing for Label "<<lab<<endl;
	computationInfo[lab]=computeComputationInfo(lab,vidm);
	//cout<<"TEST:"<<computationInfo[lab].numWriteMemLoc[CIT_TOTAL]<<endl;
#if 0
	UDAstAttribute* node0=dynamic_cast<UDAstAttribute*>(node->getAttribute(useDefAstAttributeName));
	cout<<endl;
#endif
  }
  

}

void ProgramStatistics::printStatistics() {
  LabelSet labSet=icfg->nodeLabels();
  // print results
  cout<< "-----------------------------------"<<endl;	
  cout<< "Program statistics:"<<endl;
  cout<< "-----------------------------------"<<endl;	
  for(LabelSet::iterator i=labSet.begin();i!=labSet.end();++i) {
	cout<<"L"<<*i<<":"<<computationInfo[*i].toString()<<endl;
  }
  cout<< "-----------------------------------"<<endl;	
}

string ProgramStatistics::generateNodeResourceUsageDotString(Label lab) {
  stringstream dot;
  dot<<lab;
  string labelstart="[label=";
  string labelend="]";
  string tablestart="<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">";
  string rowstart="<TR>";
  string rowend="</TR>";
  string labentry="<TR><TD COLSPAN=\"5\" BGCOLOR=\"firebrick4\"><font color=\"white\">L"+dot.str()+"</font></TD></TR>";
  string entries=computationInfo[lab].toDot();
  //string entry2="<TD PORT=\"second\" BGCOLOR=\"blue\">FLOAT</TD>";
  string tableend="</TABLE>>";
  string result=dot.str()+labelstart+tablestart+labentry+rowstart+entries+rowend+tableend+labelend;
  return result;
}

void ProgramStatistics::generateResourceUsageICFGDotFile(string dotfilename) {
  // generate ICFG visualization
  cout << "generating "<<dotfilename<<endl;
  icfg->setDotOptionDisplayLabel(false);
  icfg->setDotOptionDisplayStmt(false);
  icfg->setDotOptionHeaderFooter(false);
  string icfgEdges=icfg->toDot(labeler);

  LabelSet labSet=icfg->nodeLabels();
  string resourceUsage;
  for(LabelSet::iterator i=labSet.begin();i!=labSet.end();++i) {
	resourceUsage+=generateNodeResourceUsageDotString(*i);
	resourceUsage+=";\n";
  }
  string icfgAndResourceUsage="digraph ResourceUsage {\n"+resourceUsage+icfgEdges+"}\n";
  write_file(dotfilename,icfgAndResourceUsage);
}
