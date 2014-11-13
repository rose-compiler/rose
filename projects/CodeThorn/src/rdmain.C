// Author: Markus Schordan, 2013.

#include "rose.h"

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalyzer.h"
#include "RDLattice.h"
#include "DFAnalyzer.h"
#include "WorkList.h"
#include "RDAnalyzer.h"
#include "RDAstAttribute.h"
#include "AstAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"
#include "ProgramStats.h"
#include "DFAstAttributeConversion.h"

#include "DFTransferFunctions.hpp"

using namespace std;
using namespace CodeThorn;
using namespace DFAstAttributeConversion;

template<typename T>
void printAttributes(Labeler* labeler, VariableIdMapping* vim, string attributeName) {
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
    Label lab=i;
    SgNode* node=labeler->getNode(i);
    //    cout<<"@Label "<<lab<<":";
    T* node0=dynamic_cast<T*>(node->getAttribute(attributeName));
    if(node0)
      node0->toStream(cout,vim);
    else
      cout<<" none.";
    cout<<endl;
  }
}

list<SgExpression*> varRefExpOfusedVar(SgNode* root, VariableId varId, VariableIdMapping* variableIdMapping ) {
  list<SgExpression*> varRefList;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(SgVarRefExp* varRefExp=isSgVarRefExp(*i)) {
      if(varId==variableIdMapping->variableId(varRefExp)) {
        varRefList.push_back(varRefExp);
      }
    }
  }
  return varRefList;
}

// requires available expressions analysis
void substituteUsesWithAvailableExpRhsOfDef(string udAttributeName, SgNode* root, Labeler* labeler, VariableIdMapping* variableIdMapping) {
  RoseAst ast(root);
  long labelNum=labeler->numberOfLabels();
  for(long i=0;i<labelNum;++i) {
    Label lab=i;
    SgNode* node=labeler->getNode(lab);
    UDAstAttribute* udAttr=dynamic_cast<UDAstAttribute*>(node->getAttribute(udAttributeName));
    if(udAttr) {
      VariableIdSet usedVars=udAttr->useVariables(*variableIdMapping);
      if(usedVars.size()>0) {
        //cout<<"Found used vars."<<endl;
        // foreach uvar in usedVars do if(def(uvar)==1) replace(ast(uvar),rhs(def(uvar))) od
        for(VariableIdSet::iterator i=usedVars.begin();i!=usedVars.end();++i) {
          LabelSet varDefs=udAttr->definitionsOfVariable(*i);
          if(varDefs.size()==1) {
            // 1) determine definition rhs
            Label def=*varDefs.begin(); // guaranteed to be one
            SgNode* defRootNode=labeler->getNode(def);
            // only substitute variables
            if(variableIdMapping->hasIntegerType(*i)||variableIdMapping->hasFloatingPointType(*i)) {
              //cout<<"Found UD Attribute with one def. variable:"<<variableIdMapping->uniqueShortVariableName(*i)<<" ";
              //cout<<"DEF:"<<defRootNode->unparseToString()<<endl;
              // somewhat combersome to determime the rhs of the def
              // 1) declaration initializer
              // 2) assignment
              SgExpression* rhsExp=0;
              if(isSgExprStatement(defRootNode)) {
                defRootNode=SgNodeHelper::getExprStmtChild(defRootNode);
              }
              if(SgVariableDeclaration* decl=isSgVariableDeclaration(defRootNode)) {
                SgExpression* exp=SgNodeHelper::getInitializerExpressionOfVariableDeclaration(decl);
                if(exp) {
                  rhsExp=exp;
                }
              }
              if(SgAssignOp* assignOp=isSgAssignOp(defRootNode)) {
                rhsExp=isSgExpression(SgNodeHelper::getRhs(assignOp));
              }
              if(rhsExp) {
                list<SgExpression*> uses=varRefExpOfusedVar(node, *i, variableIdMapping);
                for(list<SgExpression*>::iterator i=uses.begin();i!=uses.end();++i) {
                  cout<<"Substituting:"<<(*i)->unparseToString()<<" by "<<rhsExp->unparseToString()<<endl;
                  SgNodeHelper::replaceExpression(*i,SageInterface::copyExpression(rhsExp),true); // must be true (otherwise internal error)
                }
              }
              // determine rhs of assignment
            }
          }
        }
      }
    }
  }
}

int main(int argc, char* argv[]) {
  try {
    cout << "INIT: Parsing and creating AST."<<endl;
    boolOptions.registerOption("semantic-fold",false); // temporary
    boolOptions.registerOption("post-semantic-fold",false); // temporary
    SgProject* root = frontend(argc,argv);
    
    RDAnalyzer* rdAnalyzer=new RDAnalyzer();
    rdAnalyzer->initialize(root);
    rdAnalyzer->initializeGlobalVariables(root);
    rdAnalyzer->initializeTransferFunctions();
    
    std::string funtofind="main";
    RoseAst completeast(root);
    SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
    rdAnalyzer->determineExtremalLabels(startFunRoot);
    rdAnalyzer->run();
    
    cout << "INFO: attaching RD-data to AST."<<endl;
    rdAnalyzer->attachInInfoToAst("rd-analysis-in");
    rdAnalyzer->attachOutInfoToAst("rd-analysis-out");
    //printAttributes<RDAstAttribute>(rdAnalyzer->getLabeler(),rdAnalyzer->getVariableIdMapping(),"rd-analysis-in");
    cout << "INFO: generating and attaching UD-data to AST."<<endl;
    createUDAstAttributeFromRDAttribute(rdAnalyzer->getLabeler(),"rd-analysis-in", "ud-analysis");
#if 0
  cout << "INFO: substituting uses with rhs of defs."<<endl;
  substituteUsesWithAvailableExpRhsOfDef("ud-analysis", root, rdAnalyzer->getLabeler(), rdAnalyzer->getVariableIdMapping());
#endif

  Flow* flow=rdAnalyzer->getFlow();
  cout<<"Flow label-set size: "<<flow->nodeLabels().size()<<endl;
  CFAnalyzer* cfAnalyzer0=rdAnalyzer->getCFAnalyzer();
  int red=cfAnalyzer0->reduceBlockBeginNodes(*flow);
  cout<<"INFO: eliminated "<<red<<" block-begin nodes in ICFG."<<endl;

#if 0
  cout << "INFO: computing program statistics."<<endl;
  ProgramStatistics ps(rdAnalyzer->getVariableIdMapping(),
                       rdAnalyzer->getLabeler(), 
                       rdAnalyzer->getFlow(),
                       "ud-analysis");
  ps.computeStatistics();
  //ps.printStatistics();
  cout << "INFO: generating resource usage visualization."<<endl;
  ps.setGenerateWithSource(false);
  ps.generateResourceUsageICFGDotFile("resourceusageicfg.dot");
  flow->resetDotOptions();
#endif
  cout << "INFO: generating visualization data."<<endl;
  // generate ICFG visualization
  cout << "generating icfg.dot."<<endl;
  write_file("icfg.dot", flow->toDot(rdAnalyzer->getLabeler()));

  //  cout << "INFO: generating control dependence graph."<<endl;
  //Flow cdg=rdAnalyzer->getCFAnalyzer()->controlDependenceGraph(*flow);

  cout << "generating datadependencegraph.dot."<<endl;
  DataDependenceVisualizer ddvis0(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
                                 "ud-analysis");
  //printAttributes<UDAstAttribute>(rdAnalyzer->getLabeler(),rdAnalyzer->getVariableIdMapping(),"ud-analysis");
  //ddvis._showSourceCode=false; // for large programs
  ddvis0.generateDefUseDotGraph(root,"datadependencegraph.dot");
  flow->resetDotOptions();

  cout << "generating icfgdatadependencegraph.dot."<<endl;
  DataDependenceVisualizer ddvis1(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
                                 "ud-analysis");
  ddvis1.includeFlowGraphEdges(flow);
  ddvis1.generateDefUseDotGraph(root,"icfgdatadependencegraph.dot");
  flow->resetDotOptions();

  cout << "generating icfgdatadependencegraph_clustered.dot."<<endl;
  DataDependenceVisualizer ddvis2(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
                                 "ud-analysis");
  ddvis2.generateDotFunctionClusters(root,rdAnalyzer->getCFAnalyzer(),"icfgdatadependencegraph_clustered.dot",true);

  cout << "generating icfg_clustered.dot."<<endl;
  DataDependenceVisualizer ddvis3(rdAnalyzer->getLabeler(),
                                 rdAnalyzer->getVariableIdMapping(),
                                 "ud-analysis");
  ddvis3.generateDotFunctionClusters(root,rdAnalyzer->getCFAnalyzer(),"icfg_clustered.dot",false);

#if 1
  cout << "INFO: annotating analysis results as comments."<<endl;
  AstAnnotator ara(rdAnalyzer->getLabeler());
  ara.annotateAstAttributesAsCommentsBeforeStatements(root, "rd-analysis-in");
  ara.annotateAstAttributesAsCommentsAfterStatements(root, "rd-analysis-out");
  cout << "INFO: generating annotated source code."<<endl;
  root->unparse(0,0);
#endif
  return 0;
  } catch(char const* s) {
    cout<<s<<endl;
    exit(1);
  } catch(string s) {
    cout<<s<<endl;
    exit(1);
  } catch(...) {
    cout<<"Error: unknown exception."<<endl;
    exit(1);
  }
}
