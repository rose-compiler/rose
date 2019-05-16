// Author: Markus Schordan, 2013.

#include "rose.h"

#include <iostream>
#include "VariableIdMapping.h"
#include "Labeler.h"
#include "CFAnalysis.h"
#include "RDLattice.h"
#include "WorkList.h"
#include "RDAstAttribute.h"
#include "AstAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"
#include "ProgramStats.h"
#include "DFAstAttributeConversion.h"
#include "RDAnalysis.h"
#include "IntervalAnalysis.h"
#include "LVAnalysis.h"

#include "addressTakenAnalysis.h"
#include "FIPointerAnalysis.h"
#include "FunctionIdMapping.h"

using namespace std;
using namespace CodeThorn;
using namespace DFAstAttributeConversion;
using namespace CodeThorn;

int main(int argc, char* argv[]) {
  bool option_rd_analysis=true;
  bool option_dd_analysis=false;
  bool option_lv_analysis=false;
  bool option_interval_analysis=false;
  try {
    cout << "INIT: Parsing and creating AST."<<endl;
    SgProject* root = frontend(argc,argv);

    {
      cout<<"STATUS: running address taken analysis."<<endl;
      // compute variableId mappings
      VariableIdMapping variableIdMapping;
      variableIdMapping.computeVariableSymbolMapping(root);

      FunctionIdMapping functionIdMapping;
      CodeThorn::FIPointerAnalysis fipa(&variableIdMapping, &functionIdMapping, root);
      fipa.initialize();
      fipa.run();
      VariableIdSet vidset=fipa.getModByPointer();
      cout<<"mod-set: "<<CodeThorn::VariableIdSetPrettyPrint::str(vidset,variableIdMapping)<<endl;
    }
    
    if(option_interval_analysis)
    {
      cout << "STATUS: creating interval analyzer."<<endl;
      IntervalAnalysis* intervalAnalyzer=new IntervalAnalysis();
      cout << "STATUS: initializing interval analyzer."<<endl;
      intervalAnalyzer->initialize(root);
      cout << "STATUS: initializing interval transfer functions."<<endl;
      intervalAnalyzer->initializeTransferFunctions();
      cout << "STATUS: initializing interval global variables."<<endl;
      intervalAnalyzer->initializeGlobalVariables(root);

      
      std::string funtofind="main";
      RoseAst completeast(root);
      SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
      intervalAnalyzer->determineExtremalLabels(startFunRoot);
#if 0
      intervalAnalyzer->run();
#else
      cout << "STATUS: did not run interval analysis."<<endl;      
#endif
    }
    if(option_lv_analysis)
    {
      cout << "STATUS: creating LV analysis."<<endl;
      CodeThorn::LVAnalysis* lvAnalysis=new CodeThorn::LVAnalysis();
      cout << "STATUS: initializing LV analysis."<<endl;
      lvAnalysis->setBackwardAnalysis();
      lvAnalysis->initialize(root);
      cout << "STATUS: initializing LV transfer functions."<<endl;
      lvAnalysis->initializeTransferFunctions();
      cout << "STATUS: initializing LV global variables."<<endl;
      lvAnalysis->initializeGlobalVariables(root);
      std::string funtofind="main";
      RoseAst completeast(root);
      SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
      cout << "generating icfg_backward.dot."<<endl;
      write_file("icfg_backward.dot", lvAnalysis->getFlow()->toDot(lvAnalysis->getLabeler()));

      lvAnalysis->determineExtremalLabels(startFunRoot);
#if 1
      lvAnalysis->run();
      cout << "INFO: attaching LV-data to AST."<<endl;
      lvAnalysis->attachInInfoToAst("lv-analysis-in");
      lvAnalysis->attachOutInfoToAst("lv-analysis-out");
      AstAnnotator ara(lvAnalysis->getLabeler(),lvAnalysis->getVariableIdMapping());
      ara.annotateAstAttributesAsCommentsBeforeStatements(root, "lv-analysis-in");
      ara.annotateAstAttributesAsCommentsAfterStatements(root, "lv-analysis-out");
#else
      cout << "STATUS: did not run LV analysis."<<endl;      
#endif
      delete lvAnalysis;
    }

    if(option_rd_analysis)
    {
      cout << "STATUS: creating RD analyzer."<<endl;
      CodeThorn::RDAnalysis* rdAnalysis=new CodeThorn::RDAnalysis();
      cout << "STATUS: initializing RD analyzer."<<endl;
      rdAnalysis->initialize(root);
      cout << "STATUS: initializing RD transfer functions."<<endl;
      rdAnalysis->initializeTransferFunctions();
      cout << "STATUS: initializing RD global variables."<<endl;
      rdAnalysis->initializeGlobalVariables(root);
      
      cout << "generating icfg_forward.dot."<<endl;
      write_file("icfg_forward.dot", rdAnalysis->getFlow()->toDot(rdAnalysis->getLabeler()));
    
      std::string funtofind="main";
      RoseAst completeast(root);
      SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
      rdAnalysis->determineExtremalLabels(startFunRoot);
      rdAnalysis->run();
    
      cout << "INFO: attaching RD-data to AST."<<endl;
      rdAnalysis->attachInInfoToAst("rd-analysis-in");
      rdAnalysis->attachOutInfoToAst("rd-analysis-out");
      //printAttributes<RDAstAttribute>(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping(),"rd-analysis-in");
      cout << "INFO: annotating analysis results as comments."<<endl;
      ROSE_ASSERT(rdAnalysis->getVariableIdMapping());
      AstAnnotator ara(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping());
      ara.annotateAstAttributesAsCommentsBeforeStatements(root, "rd-analysis-in");
      ara.annotateAstAttributesAsCommentsAfterStatements(root, "rd-analysis-out");

      cout << "INFO: generating and attaching UD-data to AST."<<endl;
      createUDAstAttributeFromRDAttribute(rdAnalysis->getLabeler(),"rd-analysis-in", "ud-analysis");
#if 0
      cout << "INFO: substituting uses with rhs of defs."<<endl;
      substituteUsesWithAvailableExpRhsOfDef("ud-analysis", root, rdAnalysis->getLabeler(), rdAnalysis->getVariableIdMapping());
#endif
      if(option_dd_analysis) {
        Flow* flow=rdAnalysis->getFlow();
        cout<<"Flow label-set size: "<<flow->nodeLabels().size()<<endl;
        CFAnalysis* cfAnalyzer0=rdAnalysis->getCFAnalyzer();
        int red=cfAnalyzer0->reduceBlockBeginNodes(*flow);
        cout<<"INFO: eliminated "<<red<<" block-begin nodes in ICFG."<<endl;
        
#if 0
        cout << "INFO: computing program statistics."<<endl;
        ProgramStatistics ps(rdAnalysis->getVariableIdMapping(),
                             rdAnalysis->getLabeler(), 
                             rdAnalysis->getFlow(),
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
        write_file("icfg.dot", flow->toDot(rdAnalysis->getLabeler()));
        
        //  cout << "INFO: generating control dependence graph."<<endl;
        //Flow cdg=rdAnalysis->getCFAnalyzer()->controlDependenceGraph(*flow);

        cout << "generating datadependencegraph.dot."<<endl;
        DataDependenceVisualizer ddvis0(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        //printAttributes<UDAstAttribute>(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping(),"ud-analysis");
        //ddvis._showSourceCode=false; // for large programs
        ddvis0.generateDefUseDotGraph(root,"datadependencegraph.dot");
        flow->resetDotOptions();
        
        cout << "generating icfgdatadependencegraph.dot."<<endl;
        DataDependenceVisualizer ddvis1(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis1.includeFlowGraphEdges(flow);
        ddvis1.generateDefUseDotGraph(root,"icfgdatadependencegraph.dot");
        flow->resetDotOptions();
        
        cout << "generating icfgdatadependencegraph_clustered.dot."<<endl;
        DataDependenceVisualizer ddvis2(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis2.generateDotFunctionClusters(root,rdAnalysis->getCFAnalyzer(),"icfgdatadependencegraph_clustered.dot",true);
        
        cout << "generating icfg_clustered.dot."<<endl;
        DataDependenceVisualizer ddvis3(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis3.generateDotFunctionClusters(root,rdAnalysis->getCFAnalyzer(),"icfg_clustered.dot",false);
        
      }
    }
    cout << "INFO: generating annotated source code."<<endl;
    root->unparse(0,0);
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
