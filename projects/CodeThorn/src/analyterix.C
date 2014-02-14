// Author: Markus Schordan, 2013.

#include "rose.h"

#include "inliner.h"

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
#include "AnalysisAbstractionLayer.h"
#include "AType.h"
#include "SgNodeHelper.h"
#include "DFAstAttributeConversion.h"
#include "FIConstAnalysis.h"
#include <boost/foreach.hpp>

#include <vector>
#include <set>
#include <list>
#include <string>

#include "limits.h"
#include <cmath>
#include "assert.h"

// ROSE analyses
#include "VariableRenaming.h"

using namespace std;
using namespace CodeThorn;
using namespace AType;
using namespace DFAstAttributeConversion;

#include "ReachabilityResults.h"

string option_prefix;
bool option_stats=false;
bool option_rdanalysis=false;
bool option_roserdanalysis=false;
bool option_fi_constanalysis=false;
const char* csvConstResultFileName=0;

//boost::program_options::variables_map args;

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

void rdAnalysis(SgProject* root) {
  boolOptions.registerOption("semantic-fold",false); // temporary
  boolOptions.registerOption("post-semantic-fold",false); // temporary

  RDAnalyzer* rdAnalyzer=new RDAnalyzer();
  rdAnalyzer->initialize(root);
  rdAnalyzer->initializeGlobalVariables(root);

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

  Flow* flow=rdAnalyzer->getFlow();
#if 1
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

  cout << "INFO: annotating analysis results as comments."<<endl;
  AstAnnotator ara(rdAnalyzer->getLabeler());
  ara.annotateAstAttributesAsCommentsBeforeStatements(root, "rd-analysis-in");
  ara.annotateAstAttributesAsCommentsAfterStatements(root, "rd-analysis-out");
  cout << "INFO: generating annotated source code."<<endl;
  root->unparse(0,0);
}

void printRoseInfo(SgProject* project) {
  project->display("PROJECT NODE");
  int fileNum=project->numberOfFiles();
  for(int i=0;i<fileNum;i++) {
    std::stringstream ss;
    SgFile* file=(*project)[i];
    ROSE_ASSERT(file);
    ss<<"FILE NODE Nr. "<<i;
    file->display(ss.str());
  }
}

void generateRoseRDDotFile(VariableRenaming* varRen,string filename) {
  ofstream myfile;
  myfile.open(filename.c_str());
  myfile<<"digraph RD1 {"<<endl;
  std::cout << "Propagated Def Table:" << endl;
  VariableRenaming::DefUseTable& defTable=varRen->getPropDefTable();
    BOOST_FOREACH(VariableRenaming::DefUseTable::value_type& node, defTable)
    {
	  SgNode* astNode=node.first;
	  std::cout << "  Def Table for [" << node.first->class_name() << ":" << astNode << "]:"<< astNode->unparseToString() << std::endl;
	  myfile<<"N"<<astNode<<"[label=\""<<astNode->unparseToString()<<"\"];"<<endl;
        BOOST_FOREACH(VariableRenaming::TableEntry::value_type& entry, defTable[astNode])
        {
            std::cout << "    Defs for [" << varRen->keyToString(entry.first) << "]:" << std::endl;
            BOOST_FOREACH(VariableRenaming::NodeVec::value_type& iter, entry.second)
            {
                std::cout << "      -[" << iter->class_name() << ":" << iter << "]" << std::endl;
				std::vector<SgInitializedName*> defNameVec=entry.first;
				ROSE_ASSERT(defNameVec.size()==1);
				myfile<<"N"<<astNode<<"->"<<"N"<<iter<<"[label=\""<<varRen->keyToString(entry.first)<<"\"];"<<endl;
            }
        }
    }
	myfile<<"}"<<endl;
	myfile.close();
}

/*
typedef std::map<int, SgNode*> NumNodeRenameEntry;
NumNodeRenameEntry getUsesAtNodeForName(SgNode* node, const VarName& var);
NumNodeRenameEntry getDefsAtNodeForName(SgNode* node, const VarName& var);
typedef boost::unordered_map<VarName, NumNodeRenameEntry> NumNodeRenameTable;
NumNodeRenameTable getUsesAtNode(SgNode* node);
NumNodeRenameTable getDefsAtNode(SgNode* node);
NumNodeRenameTable getDefsForSubtree(SgNode* node);
static VarName getVarName(SgNode* node);
NodeVec getAllUsesForDef(const VarName& var, int num);
*/

void generateRoseRDDotFile2(VariableRenaming* varRen,string filename) {
  ofstream myfile;
  myfile.open(filename.c_str());
  myfile<<"digraph RD1 {"<<endl;
  std::cout << "Propagated Def Table:" << endl;
  VariableRenaming::DefUseTable& defTable=varRen->getPropDefTable();
    BOOST_FOREACH(VariableRenaming::DefUseTable::value_type& node, defTable)
    {
	  SgNode* astNode=node.first;
	  std::cout << "  Def Table for [" << node.first->class_name() << ":" << astNode << "]:"<< astNode->unparseToString() << std::endl;
	  myfile<<"N"<<astNode<<"[label=\""<<astNode->unparseToString()<<"\"];"<<endl;
        BOOST_FOREACH(VariableRenaming::TableEntry::value_type& entry, defTable[astNode])
        {
            std::cout << "    Defs for [" << varRen->keyToString(entry.first) << "]:" << std::endl;
            BOOST_FOREACH(VariableRenaming::NodeVec::value_type& iter, entry.second)
            {
                std::cout << "      -[" << iter->class_name() << ":" << iter << "]" << std::endl;
				std::vector<SgInitializedName*> defNameVec=entry.first;
				ROSE_ASSERT(defNameVec.size()==1);
				myfile<<"N"<<astNode<<"->"<<"N"<<iter<<"[label=\""<<varRen->keyToString(entry.first)<<"\"];"<<endl;
            }
        }
    }
	myfile<<"}"<<endl;
	myfile.close();
}

void printCodeStatistics(SgNode* root) {
  SgProject* project=isSgProject(root);
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(project);
  VariableIdSet setOfUsedVars=AnalysisAbstractionLayer::usedVariablesInsideFunctions(project,&variableIdMapping);
  cout<<"----------------------------------------------------------------------"<<endl;
  cout<<"Statistics:"<<endl;
  cout<<"Number of functions          : "<<SgNodeHelper::listOfFunctionDefinitions(project).size()<<endl;
  cout<<"Number of global variables   : "<<SgNodeHelper::listOfGlobalVars(project).size()<<endl;
  cout<<"Number of global variableIds : "<<AnalysisAbstractionLayer::globalVariables(project,&variableIdMapping).size()<<endl;
  cout<<"Number of used variables     : "<<setOfUsedVars.size()<<endl;
  cout<<"----------------------------------------------------------------------"<<endl;
  cout<<"VariableIdMapping-size       : "<<variableIdMapping.getVariableIdSet().size()<<endl;
  cout<<"----------------------------------------------------------------------"<<endl;
}

int main(int argc, char* argv[]) {
  try {
    if(argc==1) {
      cout << "Error: wrong command line options."<<endl;
      exit(1);
    }
    // Command line option handling.
    namespace po = boost::program_options;
    po::options_description desc
      ("analyterix V0.1\n"
       "Written by Markus Schordan\n"
       "Supported options");
  
    desc.add_options()
      ("help,h", "produce this help message.")
      ("rose-help", "show help for compiler frontend options.")
      ("ast-file-node-display", "show project and file node dumps (using display()).")
      ("version,v", "display the version.")
      ("stats", "display code statistics.")
      ("rd-analysis", "perform reaching definitions analysis.")
      ("rose-rd-analysis", "perform rose-core reaching definitions analysis.")
      ("fi-constanalysis", "perform flow-insensitive constant analysis.")
      ("varidmapping", "prints variableIdMapping")
      ("write-varidmapping", "writes variableIdMapping to a file variableIdMapping.csv")
      ("csv-fi-constanalysis",po::value< string >(), "generate csv-file [arg] with const-analysis data.")
      ("prefix",po::value< string >(), "set prefix for all generated files.")
      ;
  //    ("int-option",po::value< int >(),"option info")


    po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), args);
    po::notify(args);

    if (args.count("help")) {
      cout << "analyterix <filename> [OPTIONS]"<<endl;
      cout << desc << "\n";
      return 0;
    }
    if (args.count("rose-help")) {
      argv[1] = strdup("--help");
    }

    if (args.count("version")) {
      cout << "analyterix version 0.1\n";
      cout << "Written by Markus Schordan 2014\n";
      return 0;
    }
    if (args.count("prefix")) {
      option_prefix=args["prefix"].as<string>().c_str();
    }

    if(args.count("stats")) {
      option_stats=true;
    }
    if(args.count("rd-analysis")) {
      option_rdanalysis=true;
    }
    if(args.count("rose-rd-analysis")) {
      option_roserdanalysis=true;
    }
    if(args.count("fi-constanalysis")) {
      option_fi_constanalysis=true;
    }
    if (args.count("csv-fi-constanalysis")) {
      csvConstResultFileName=args["csv-fi-constanalysis"].as<string>().c_str();
      option_fi_constanalysis=true;
    }
  

    // clean up string-options in argv
    for (int i=1; i<argc; ++i) {
      if (string(argv[i]) == "--prefix" 
          || string(argv[i]) == "--csv-const-result"
          ) {
        // do not confuse ROSE frontend
        argv[i] = strdup("");
        assert(i+1<argc);
        argv[i+1] = strdup("");
      }
    }

  cout << "INIT: Parsing and creating AST."<<endl;
  SgProject* root = frontend(argc,argv);
  //  AstTests::runAllTests(root);
  // inline all functions

  if(option_stats) {
    printCodeStatistics(root);
  }

  if(args.count("ast-file-node-display")) {
    printRoseInfo(root);
  }

  cout<<"STATUS: computing variableid mapping"<<endl;
  VariableIdMapping variableIdMapping;
  variableIdMapping.computeVariableSymbolMapping(root);
  cout<<"VariableIdMapping size: "<<variableIdMapping.getVariableIdSet().size()<<endl;
  Labeler* labeler=new Labeler(root);
  //cout<<"Labelling:\n"<<labeler->toString()<<endl;
  IOLabeler* iolabeler=new IOLabeler(root,&variableIdMapping);
  //cout<<"IOLabelling:\n"<<iolabeler->toString()<<endl;

  if (args.count("varidmapping")) {
    variableIdMapping.toStream(cout);
  }

  if(option_fi_constanalysis) {
    VarConstSetMap varConstSetMap;
    FIConstAnalysis fiConstAnalysis(&variableIdMapping);
    fiConstAnalysis.runAnalysis(root);
    fiConstAnalysis.attachAstAttributes(labeler,"const-analysis-inout"); // not iolabeler
    if(csvConstResultFileName) {
      cout<<"INFO: generating const CSV file "<<option_prefix+csvConstResultFileName<<endl;
      fiConstAnalysis.writeCvsConstResult(variableIdMapping, option_prefix+csvConstResultFileName);
    }
#if 1
    cout << "INFO: annotating analysis results as comments."<<endl;
    AstAnnotator ara(labeler);
    ara.annotateAstAttributesAsCommentsBeforeStatements(root, "const-analysis-inout");
    ara.annotateAstAttributesAsCommentsAfterStatements(root, "const-analysis-inout");
    cout << "INFO: generating annotated source code."<<endl;
    root->unparse(0,0);
#endif
  }
  if(option_rdanalysis) {
    cout<<"STATUS: Performing RD analysis."<<endl;
    rdAnalysis(root);
  }
  if(option_roserdanalysis) {
      cout << "INFO: generating rose-rd dot file (1/2)."<<endl;
      VariableRenaming varRen(root);
      varRen.run();
      varRen.toFilteredDOT("rose-rd1.dot");
	  //      varRen.printOriginalDefTable();
      //varRen.printRenameTable();
	  generateRoseRDDotFile(&varRen,"rose-rd2.dot");
  }
  cout<< "STATUS: finished."<<endl;

  // main function try-catch
  } catch(char* str) {
    cerr << "*Exception raised: " << str << endl;
    return 1;
  } catch(const char* str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  } catch(string str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  }
  return 0;
}
