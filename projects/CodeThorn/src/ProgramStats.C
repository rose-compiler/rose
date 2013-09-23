// stats test

#include "rose.h"
#include "ProgramStats.h"

enum CompInfoType {CIT_TOTAL=1, CIT_INT, CIT_FLOAT, CIT_NUM};
class ComputationInfo {
public:
  ComputationInfo():
	numJmp(0),
	numConditionTest(0),
	numLogOp(0),
	numBitOp(0) {
	for(int i=CIT_TOTAL; i<CIT_NUM;++i) {
	  numArithOp[i]=0;
	  numReadMemLoc[i]=0;
	  numWriteMemLoc[i]=0;
	}
  }
  int numJmp;
  int numConditionTest;
  int numLogOp;
  int numBitOp;
  int numArithOp[CIT_NUM];
  int numReadMemLoc[CIT_NUM];
  int numWriteMemLoc[CIT_NUM];
  string toString() {
	stringstream ss;
	ss<<"Jmp:"<<numJmp<<","
	  <<"Tst:"<<numConditionTest<<","
	  <<"LogOp:"<<numLogOp<<","
	  <<"BitOp:"<<numBitOp;
	ss<< " [";
	for(int i=CIT_TOTAL; i<CIT_NUM;++i) {
	  string tt;
	  switch(i){
	  case CIT_TOTAL: tt="total";break;
	  case CIT_INT: tt="int";break;
	  case CIT_FLOAT: tt="float";break;
	  }
	  ss<<tt<<"-ArithOp:"<<numArithOp[i]<<",";
	  ss<<tt<<"-Read:"<<numReadMemLoc[i]<<",";
	  ss<<tt<<"-Write:"<<numWriteMemLoc[i]<<",";
	}
	ss<< "]";
	return ss.str();
  }
};

void computeOpStats(ComputationInfo& ci, SgNode* node) {
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


ComputationInfo computeComputationInfo(SgNode* node, VariableIdMapping* vidm) {
  ComputationInfo ci;
  cout<<"ci declared:"<<endl;
  cout<<ci.toString()<<endl;
  if(isSgVariableDeclaration(node)||isSgExpression(node)||isSgInitializedName(node)) {
	VariableIdSet readMemLocSet=AnalysisAbstractionLayer::useVariables(node, *vidm);
	VariableIdSet writeMemLocSet=AnalysisAbstractionLayer::defVariables(node, *vidm);
	ci.numReadMemLoc[CIT_TOTAL]=readMemLocSet.size();
	ci.numWriteMemLoc[CIT_TOTAL]=writeMemLocSet.size();
	if(writeMemLocSet.size()>0) cout<<"found SOMETHING!"<<ci.numWriteMemLoc[CIT_TOTAL]<<endl;
  }
  computeOpStats(ci,node);
  cout<<ci.toString()<<endl;
  return ci;
}

void computeStatistics(VariableIdMapping* vidm, Labeler* labeler, Flow* icfg, string useDefAstAttributeName) {
  LabelSet labSet=icfg->nodeLabels();
  long labelNum=labeler->numberOfLabels();
  vector<ComputationInfo> computationInfo(labelNum);
  for(LabelSet::iterator i=labSet.begin();i!=labSet.end();++i) {
	Label lab=*i;
	SgNode* node=labeler->getNode(lab);

	//	UDAstAttribute* node0=dynamic_cast<UDAstAttribute*>(node->getAttribute(useDefAstAttributeName));
	cout<<"computing for Label "<<lab<<endl;
	computationInfo[lab]=computeComputationInfo(node,vidm);
	cout<<"TEST:"<<computationInfo[lab].numWriteMemLoc[CIT_TOTAL]<<endl;
#if 0
	UDAstAttribute* node0=dynamic_cast<UDAstAttribute*>(node->getAttribute(useDefAstAttributeName));
	cout<<endl;
#endif
  }
  // print results
  cout<< "-----------------------------------"<<endl;	
  cout<< "Program statistics:"<<endl;
  cout<< "-----------------------------------"<<endl;	
  for(LabelSet::iterator i=labSet.begin();i!=labSet.end();++i) {
	cout<<"L"<<*i<<":"<<computationInfo[*i].toString()<<endl;
  }
  cout<< "-----------------------------------"<<endl;	
}

