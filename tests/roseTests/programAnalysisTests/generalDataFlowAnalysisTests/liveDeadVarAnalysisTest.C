// Liao, 12/6/2011
#include "rose.h"

#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

// TODO group them into one header
#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"
#include "liveDeadVarAnalysis.h"

int numFails = 0, numPass = 0;

//------------ function level dot graph output ----------------
class analysisStatesToDOT : public UnstructuredPassIntraAnalysis
{
  private:
    LiveDeadVarsAnalysis* lda; // reference to the source analysis
    void printEdge(const DataflowEdge& e); // print data flow edge
    void printNode(const DataflowNode& n, std::string state_string); // print date flow node
    void visit(const Function& func, const DataflowNode& n, NodeState& state); // visitor function
  public:
    std::ostream* ostr; 
    analysisStatesToDOT (LiveDeadVarsAnalysis* l):  lda(l){ };
};

void analysisStatesToDOT::printEdge (const DataflowEdge& e)
{
  (*ostr) << e.source().id() << " -> " << e.target().id() << " [label=\"" << escapeString(e.toString ()) <<
    "\", style=\"" << "solid" << "\"];\n";
}

 void analysisStatesToDOT::printNode(const DataflowNode& n, std::string state_string)
{
        std::string id = n.id();  // node id
        std::string nodeColor = "black";

        if (isSgStatement(n.getNode()))
            nodeColor = "blue";
        else if (isSgExpression(n.getNode()))
            nodeColor = "green";
        else if (isSgInitializedName(n.getNode()))
            nodeColor = "red";

        // node_id [label="", color="", style=""]
        (*ostr) << id << " [label=\""  << escapeString(n.toString()) <<"\\n" << escapeString (state_string) << "\", color=\"" << nodeColor <<
            "\", style=\"" << (n.isInteresting()? "solid" : "dotted") << "\"];\n";
}

// This will be visited only once? Not sure, better check
void
analysisStatesToDOT::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
  std::string state_str = state.str( lda, " ");
  printNode(n, state_str);
  std::vector < DataflowEdge> outEdges = n.outEdges();
  for (unsigned int i = 0; i < outEdges.size(); ++i)
  {
    printEdge(outEdges[i]);
  }
}

//------------ call graph level  driver ----------------
class IntraAnalysisResultsToDotFiles: public UnstructuredPassInterAnalysis
{
  public:
    // internal function level dot generation, a name for the output file
    IntraAnalysisResultsToDotFiles(IntraProceduralAnalysis & funcToDot)
      : InterProceduralAnalysis(&funcToDot), UnstructuredPassInterAnalysis(funcToDot){ }
    // For each function, call function level Dot file generator to generate separate dot file
    void runAnalysis() ;
};

void IntraAnalysisResultsToDotFiles::runAnalysis()
{
  set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
  // Go through functions one by one, call an intra-procedural analysis on each of them
  // iterate over all functions with bodies
  for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)
  {
     
    FunctionState* fState = *it;
    // compose the output file name as filename_mangled_function_name.dot
    Function *func = & (fState->getFunc());
    assert (func != NULL);
    SgFunctionDefinition* proc = func->get_definition();
    string file_name = StringUtility::stripPathFromFileName(proc->get_file_info()->get_filename());
    string file_func_name= file_name+ "_"+proc->get_mangled_name().getString();

    string full_output = file_func_name +"_cfg.dot";
    std::ofstream ostr(full_output.c_str());

    ostr << "digraph " << "mygraph" << " {\n";
    analysisStatesToDOT* dot_analysis = dynamic_cast <analysisStatesToDOT*> ( intraAnalysis);
    assert (dot_analysis != NULL);
    dot_analysis->ostr = &ostr;
    dot_analysis->runAnalysis(fState->func, &(fState->state));
    ostr << "}\n";
  }

}


//-----------------------------------------------------------
int
main( int argc, char * argv[] ) 
   {
     printf("========== S T A R T ==========\n");
     SgProject* project = frontend(argc,argv);
	
     initAnalysis(project);
     Dbg::init("Live dead variable analysis Test", ".", "index.html");

  /* analysisDebugLevel = 0;

     SaveDotAnalysis sda;
     UnstructuredPassInterAnalysis upia_sda(sda);
     upia_sda.runAnalysis();
   */
	
     liveDeadAnalysisDebugLevel = 1;
     analysisDebugLevel = 1;
     if (liveDeadAnalysisDebugLevel)
        {
          printf("*********************************************************************\n");
          printf("**********   Live Dead Variable Analysis (Liveness Analysis)   ******\n");
          printf("*********************************************************************\n");
        }

     LiveDeadVarsAnalysis ldva(project);
     UnstructuredPassInterDataflow ciipd_ldva(&ldva);
     ciipd_ldva.runAnalysis();
   // Output the dot graph
     analysisStatesToDOT eas( &ldva);
     IntraAnalysisResultsToDotFiles upia_eas(eas);
     upia_eas.runAnalysis();
    
      return 0;
   }



