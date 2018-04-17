#include "rose.h"

#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string.h>
#include <map>
#include <boost/algorithm/string.hpp>
using namespace std;

#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"

#include "constantPropagation.h"

int numFails = 0, numPass = 0;

class evaluateAnalysisStates : public UnstructuredPassIntraAnalysis
   {
     public:
          ConstantPropagationAnalysis* div;
          string indent;
          map<string, map<SgName, ConstantPropagationLattice> > expectations;
          int total_expectations;
  
          evaluateAnalysisStates(ConstantPropagationAnalysis* div_, string indent_);
          void visit(const Function& func, const DataflowNode& n, NodeState& state);
   };


evaluateAnalysisStates::evaluateAnalysisStates(ConstantPropagationAnalysis* div_, string indent_)
   : div(div_), indent(indent_), total_expectations(0)
   {
     expectations["testFunc1"]["a"] = ConstantPropagationLattice(ConstantPropagationLattice::constantValue,1);
     expectations["testFunc1"]["b"] = ConstantPropagationLattice(ConstantPropagationLattice::constantValue,4);
     expectations["testFunc1"]["c"] = ConstantPropagationLattice(ConstantPropagationLattice::constantValue,5);

     for (map<string, map<SgName, ConstantPropagationLattice> >::iterator i = expectations.begin(); i != expectations.end(); ++i)
          total_expectations += i->second.size();

     cout << "Total expectations: " << total_expectations << endl;
   }

void
evaluateAnalysisStates::visit(const Function& func, const DataflowNode& n, NodeState& state)
   {
     SgFunctionCallExp *fnCall = isSgFunctionCallExp(n.getNode());
     if (!fnCall)
          return;

     if (!fnCall->getAssociatedFunctionSymbol()) 
          return;

     string funcName = fnCall->getAssociatedFunctionSymbol()->get_name().getString();
     if (funcName.find("testFunc") == string::npos)
          return;

     FiniteVarsExprsProductLattice *lat = dynamic_cast<FiniteVarsExprsProductLattice *>(state.getLatticeAbove(div)[0]);
     cout << indent << "Lattice before call to " << funcName << ": " << lat->str() << endl;

     set<varID> allVars = lat->getAllVars();
     for (set<varID>::iterator i = allVars.begin(); i != allVars.end(); ++i)
        {
          string name = i->str();
          cout << "Variable " << name << " ";

          if (expectations[funcName].find(name) == expectations[funcName].end())
             {
               cout << "unspecified" << endl;
               continue;
             }

          Lattice *got = lat->getVarLattice(*i);
          ROSE_ASSERT(got);
          if (expectations[funcName][name] != got)
             {
               cout << "mismatched: " << got->str() << " was not the expected " << expectations[funcName][name].str();
               numFails++;
             }
            else
             {
               cout << "matched";
               numPass++;
             }
          cout << endl;
        }
   }


int
main( int argc, char * argv[] ) 
   {
     printf("========== S T A R T ==========\n");
	
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
	
     initAnalysis(project);
     Dbg::init("Divisibility Analysis Test", ".", "index.html");

	
     liveDeadAnalysisDebugLevel = 1;
     analysisDebugLevel = 1;
     if (liveDeadAnalysisDebugLevel)
        {
          printf("*********************************************************************\n");
          printf("*****************   Constant Propagation Analysis   *****************\n");
          printf("*********************************************************************\n");
        }

    // liveness analysis is used to generate FiniteVarsExprsProductLattice only for live variables at a CFG point
    // &ldva can be set to NULL then all visible variables and expressions will be used to generate FiniteVarsExprsProductLattice
     LiveDeadVarsAnalysis ldva(project);
     UnstructuredPassInterDataflow ciipd_ldva(&ldva);
     ciipd_ldva.runAnalysis();

    // prepare call graph
     CallGraphBuilder cgb(project);
     cgb.buildCallGraph();
     SgIncidenceDirectedGraph* graph = cgb.getGraph(); 

    // use constant propagation within the context insensitive interprocedural dataflow driver
     analysisDebugLevel = 1;
     ConstantPropagationAnalysis cpA(&ldva);
     //ConstantPropagationAnalysis cpA(NULL);
     ContextInsensitiveInterProceduralDataflow cpInter(&cpA, graph);
     cpInter.runAnalysis();

    // verify the results
     evaluateAnalysisStates eas(&cpA, "    ");
     UnstructuredPassInterAnalysis upia_eas(eas);
     upia_eas.runAnalysis();

// A better verification method using pragma strings embedded in the input test code
  // grab live-in information from a Pragma Declaration
  Rose_STL_Container <SgNode*> nodeList = NodeQuery::querySubTree(project, V_SgPragmaDeclaration);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgPragmaDeclaration* pdecl= isSgPragmaDeclaration((*i));
    ROSE_ASSERT (pdecl != NULL);
    // skip irrelevant pragmas
    if (SageInterface::extractPragmaKeyword(pdecl) != "rose")
      continue;
   // NOTE: it is not ConstantPropagationLattice here!!
   // grab the first lattice attached to the pragma statement for this analysis
    VarsExprsProductLattice* lattice = dynamic_cast <VarsExprsProductLattice *>(NodeState::getLatticeAbove(&cpA, pdecl,0)[0]);
    ROSE_ASSERT (lattice != NULL);
    string lattice_str = lattice->str();
    boost::erase_all(lattice_str, " ");
    boost::erase_all(lattice_str, "\n");
//    cout <<lattice_str<<endl;
    std::string pragma_str = pdecl->get_pragma()->get_pragma ();
    pragma_str.erase (0,5);
    boost::erase_all(pragma_str, "\n");
    // cout <<pragma_str <<endl;


    boost::erase_all(pragma_str, " ");

    if (lattice_str == pragma_str)
    {
     cout<<"Verified!"<<endl;
    }
    else
    {
     cout<<"Analysis results are not identical to the reference results from the pragma !"<<endl;
     cout<<"=======pragma reference results========"<<endl; 
     cout <<pragma_str <<endl;
     cout<<"-------analysis results--------"<<endl; 
     cout <<lattice_str<<endl;
     assert (false);
    }

  }



// Liao 1/6/2012, optionally dump dot graph of the analysis result
     Dbg::dotGraphGenerator(&cpA);
     if (numFails == 0 && numPass == eas.total_expectations)
          printf("PASS: %d / %d\n", numPass, eas.total_expectations);
       else
          printf("FAIL!: %d / %d\n", numPass, eas.total_expectations);

     printf("==========  E  N  D  ==========\n");
	
  // Unparse and compile the project (so this can be used for testing)
     return /*backend(project) +*/ numFails;
   }



