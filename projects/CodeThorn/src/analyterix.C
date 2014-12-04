// Author: Markus Schordan, 2013, 2014.

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

#include "addressTakenAnalysis.h"
#include "defUseQuery.h"
#include "Timer.h"
#include "AnalysisAbstractionLayer.h"

#include <vector>
#include <set>
#include <list>
#include <string>

#include "limits.h"
#include <cmath>
#include "assert.h"

#include "GeneralAnalyzerBase.h"
#include "GeneralAnalyzer.h"

// ROSE analyses
#include "VariableRenaming.h"

using namespace std;
using namespace CodeThorn;
using namespace AType;
using namespace DFAstAttributeConversion;
using namespace AnalysisAbstractionLayer;

#include "PropertyValueTable.h"

string option_prefix;
bool option_stats=false;
bool option_rdanalysis=false;
bool option_generalanalysis=false;
bool option_roserdanalysis=false;
bool option_fi_constanalysis=false;
const char* csvConstResultFileName=0;

//boost::program_options::variables_map args;


int countSubTreeNodes(SgNode* root) {
  int num=0;
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    num++;
  }
  return num;
}

vector<VariantT> variantVector;

void storeTreeStructure(SgNode* root) {
  RoseAst ast(root);
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    variantVector.push_back((*i)->variantT());
  }
}

int checkTreeStructure(SgNode* root) {
  int num=0;
  RoseAst ast(root);
  vector<VariantT>::iterator vecIter=variantVector.begin();
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    //cout<<"SubTree:"<<(*i)->unparseToString()<<endl;
    if((*i)->variantT()!=(*vecIter)) {
      cout<<"Error: AST structure changed!"<<endl;
      cout<<"SubTree:"<<(*i)->unparseToString()<<endl;
      exit(1);
    }
  }
  return num;
}

void myRewrite(SgExpression* oldExp, SgExpression* newExp,bool keepOld) {
  if (oldExp==newExp)
    return;
  SgNode* parent=oldExp->get_parent();
  if(!parent) {
    cout<<"Rewrite: parent pointer is null: @"<<oldExp->class_name()<<endl;
    return;
  }
  ROSE_ASSERT(parent!=oldExp);
  cout<<"REWRITE: parent: "<<parent->unparseToString()<<"["<<parent->class_name()<<"]"<<" : @node:"<<oldExp->class_name()<<endl;

  // not supported yet
  if(isSgInitializedName(parent)) {
    cerr<<"Rewrite: SgInitializedName: not supported yet."<<endl;
    return;
  }
  if(isSgExprListExp(parent)) {
    cerr<<"Rewrite: isSgExprListExp: not supported yet."<<endl;
    return;    
  }
  if(isSgConditionalExp(parent)) {
    cerr<<"Rewrite: isSgConditionalExp: not supported yet."<<endl;
    return;
  }
  if(isSgAssignInitializer(parent)) {
    cerr<<"Rewrite: isSgAssignInitializer: not supported yet."<<endl;
    return;
  }

  // supported
  if(SgExprStatement* p=isSgExprStatement(parent)) {
    newExp->set_parent(parent);
    p->set_expression(newExp);
    return;
  } 
  if(SgReturnStmt* p=isSgReturnStmt(parent)) {
    newExp->set_parent(parent);
    p->set_expression(newExp);
    return;
  }
  //  if(SgBinaryOp* p=isSgBinaryOp(parent)) {
  if(SgBinaryOp* p=dynamic_cast<SgBinaryOp*>(parent)) {
    if(SgExpression* lhs=p->get_lhs_operand()) {
      if(oldExp==lhs) {
        newExp->set_parent(parent);
        p->set_lhs_operand(newExp);
        return;
      }
    }
    if(SgExpression* rhs=p->get_rhs_operand()) {
      if(oldExp==rhs) {
        newExp->set_parent(parent);
        p->set_rhs_operand(newExp);
        return;
      }
    }
    cout<<"Error: child of binary op is lost."<<endl;
  }
  return;

  if(SgUnaryOp* p=isSgUnaryOp(parent)) {
    if (oldExp==p->get_operand_i()) {
      newExp->set_parent(parent);
      p->set_operand_i(newExp);
      return;
    } else {
      cerr<<"Error: unary operand does not match with child."<<endl;
      exit(1);
    }
  }
  cerr<<"Error: rewrite unsupported. parent: "<<parent->class_name()<<" node: "<<oldExp->class_name()<<endl;
  cerr<<"parent isBinaryOp:"<<(isSgBinaryOp(parent))<<endl;
  exit(1);
}

bool checkAstExpressionRewrite(SgNode* root) {
  // 1 determine all root nodes of expressions in given AST
  int numOrig=countSubTreeNodes(root);
  storeTreeStructure(root);
  AstTests::runAllTests(isSgProject(root));
  cout<<"STATUS: AST is consistent."<<endl;
  RoseAst ast(root);
  typedef list<SgExpression*> ExprList;
  typedef list<ExprList> ExprListList;
  list<list<SgExpression*> > exprListList;
  for(RoseAst::iterator i=ast.begin();i!=ast.end();++i) {
    if(isSgExpression(*i)) {
      list<SgExpression*> subExprList;
      RoseAst subAst(*i);
      for(RoseAst::iterator j=subAst.begin().withoutNullValues();j!=subAst.end();++j) {
        SgNode* node=*j;
        ROSE_ASSERT(node);
        if(isSgExpression(node)) {
          cout<<"DEBUG: "<<node->unparseToString()<<endl;
          if(!isSgAssignInitializer(node) && (!isSgEnumVal(node)))
            //  if(!dynamic_cast<SgAssignInitializer*>(node) && !dynamic_cast<SgEnumVal*>(node)) {
            //if(isSgExpression(*j)) 
            if(node->get_parent()==0) {
              cout<<"Error: parent==0: "<<node->class_name()<<endl;
            }
            subExprList.push_back(isSgExpression(node));
        } else {
          cout<<"ERROR: Non-expression inside an expression!?."<<endl;
        }
      }
      exprListList.push_back(subExprList);
    }
  }
  cout<<"STATUS: checking rewrite on "<<exprListList.size()<<" expressions."<<endl;
  // print
  for(ExprListList::iterator i=exprListList.begin();i!=exprListList.end();++i) {
    ExprList exprList=*i;
    if((*i).begin()!=(*i).end()) {
      SgNode* node=(*(*i).begin());
      cout<<"CHECKING EXPR:"<<node->unparseToString()<<":"<<node->class_name()<<endl;
      for(ExprList::reverse_iterator j=exprList.rbegin();j!=exprList.rend();++j) {
        ROSE_ASSERT(*j);
        cout<<"CHECKING SUBEXPR:"<<(*j)->unparseToString()<<":"<<(*j)->class_name()<<endl;
        SgExpression* exprCopy1=SageInterface::copyExpression(*j);
        if(!(exprCopy1)) {
          cout<<"ERROR: expr1 copy is null (skipping replace)"<<endl;
          break;
        }
        SgExpression* exprCopy2=SageInterface::copyExpression(*j);
        if(!(exprCopy1)) {
          cout<<"ERROR: expr2 copy is null (skipping replace)"<<endl;
          break;
        }
        cout<<"Rewriting now.."<<endl;
        //SageInterface::replaceExpression(*j,exprCopy1,false);
        //SageInterface::replaceExpression(exprCopy1,exprCopy2,false);
        myRewrite(*j,exprCopy1,false);
        myRewrite(exprCopy1,exprCopy2,false);
        AstTests::runAllTests(isSgProject(root));
        int numRewritten=countSubTreeNodes(root);
        if(numOrig!=numRewritten) {
          cout<<"ERROR: Ast size has changed - orig: "<<numOrig<<" now:"<<"numRewritten"<<endl;
          exit(1);
        }
        //checkTreeStructure(origAst);
        cout<<"STATUS: AST is consistent."<<endl;
      }
    }
  }
  return true;
}


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

#include "PropertyState.h"
#include "IntervalPropertyState.h"

class IntervalPropertyStateFactory : public PropertyStateFactory {
public:
    PropertyState* create() {
      return new IntervalPropertyState(); 
    }
};

void generalAnalysis(SgProject* root) {
  cout<<"STATUS: general analysis started."<<endl;
  cout<<"general Analysis not supported yet. Exiting."<<endl;
  exit(1);
  boolOptions.registerOption("semantic-fold",false); // temporary
  boolOptions.registerOption("post-semantic-fold",false); // temporary

  GeneralAnalyzer* generalAnalyzer=new GeneralAnalyzer();
#if 0
  IntervalPropertyStateFactory* factory=new IntervalPropertyStateFactory();
  generalAnalyzer->setFactory(factory);
  generalAnalyzer->initialize(root);
#endif
#if 0
  generalAnalyzer->initializeGlobalVariables(root);

  std::string funtofind="main";
  RoseAst completeast(root);
  SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
  generalAnalyzer->determineExtremalLabels(startFunRoot);
  generalAnalyzer->run();
#endif
  delete generalAnalyzer;
  cout<<"STATUS: general analysis finished."<<endl;
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
  ddvis0._showSourceCode=false; // for large programs
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


SgNode* normalizeAstPointer(SgNode* node) {
  if(isSgExprStatement(node))
    node=SgNodeHelper::getExprStmtChild(node);
  if(SgInitializedName* initName=isSgInitializedName(node))
    if(SgVariableDeclaration* varDecl=isSgVariableDeclaration(initName->get_parent()))
      node=varDecl;
  return node;
}
SgNode* normalizeAstPointer2(SgNode* node) {
  SgNode* node0=node;
  while(1) {
    node=normalizeAstPointer(node);
    if(isSgReturnStmt(node)||isSgCastExp(node))
      node=SgNodeHelper::getFirstChild(node);
    if(node0==node)
      break;
    else
      node0=node;
  }
  return node;
}

// errors in rose-RD analysis:
// 1) int a; int b=a; // b=a is not detected as initialization (because a has no def, but better would bespecial init (e.g. '?') for a)
//    this loses information that a variable may not be initialized
// 2) global variables are not represented properly (instead pointer to SgFunctionDefinition is stored)
void generateRoseRDDotFile2(Labeler* labeler, VariableRenaming* varRen,string filename) {
  ofstream myfile;
  myfile.open(filename.c_str());
  myfile<<"digraph RD1 {"<<endl;

  for(Labeler::iterator i=labeler->begin();i!=labeler->end();++i) {
    SgNode* node=labeler->getNode(*i);
    node=normalizeAstPointer2(node);
    if(!isSgFunctionDeclaration(node)&&!isSgFunctionDefinition(node)&&!isSgBasicBlock(node)&&!isSgWhileStmt(node)&&!isSgForStatement(node)) {
      myfile<<"// ---------------------------------------------------------"<<endl;
      myfile<<"N"<<node<<"[label=\""<<node->class_name()<<"::"<<node->unparseToString()<<"\"];"<<endl;
      VariableRenaming::NumNodeRenameTable useTable=varRen->getUsesAtNode(node);
      for(VariableRenaming::NumNodeRenameTable::iterator j=useTable.begin();j!=useTable.end();++j) {
        VariableRenaming::VarName varName=(*j).first;
        VariableRenaming::NumNodeRenameEntry numNodeRenameEntry=(*j).second;
        for(VariableRenaming::NumNodeRenameEntry::iterator k=numNodeRenameEntry.begin();k!=numNodeRenameEntry.end();++k) {
          int renNum=(*k).first;
          SgNode* renNode=(*k).second;
          renNode=normalizeAstPointer2(renNode); // required to get the variable declaration instead of only the initialized name
          myfile<<"N"<<node<<" -> "<<"N"<<renNode<<"[label=\""<<varRen->keyToString(varName)<<"-"<<renNum <<"\"];"<<endl;
          // the following name label is only required when a non-labeled node is referenced by the RD analysis
          myfile<<"N"<<renNode<<"[label=\""<<renNode->class_name()<<"::"<<renNode->unparseToString()<<"\"];"<<endl;
        }
      }
    }
  }
#if 0
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
#endif
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
      ("general-analysis", "perform general analysis.")
      ("varidmapping", "prints variableIdMapping")
      ("write-varidmapping", "writes variableIdMapping to a file variableIdMapping.csv")
      ("check-ast-expr-rewrite", "checks all expression in an ast with a generic rewrite operation.")
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
    if(args.count("general-analysis")) {
      option_generalanalysis=true;
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
#if 0
  IOLabeler* iolabeler=new IOLabeler(root,&variableIdMapping);
  //cout<<"IOLabelling:\n"<<iolabeler->toString()<<endl;
#endif

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
  if(option_generalanalysis) {
    cout<<"STATUS: Performing general analysis."<<endl;
    generalAnalysis(root);
  }
  if(option_roserdanalysis) {
      cout << "INFO: generating rose-rd dot file (1/2)."<<endl;
      VariableRenaming varRen(root);
      varRen.run();
      varRen.toFilteredDOT("rose-rd1.dot");
      //      varRen.printOriginalDefTable();
      //varRen.printRenameTable();
      generateRoseRDDotFile2(labeler,&varRen,"rose-rd2.dot");
  }
  if (args.count("check-ast-expr-rewrite")) {
    bool result=checkAstExpressionRewrite(root);
    string s="CHECK: Ast expression rewrite: ";
    if(result) {
      cout<<s<<"PASS."<<endl;
    } else {
      cout<<s<<"FAIL."<<endl;
    }
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
