// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Cycle Detection Analysis
// Author: Thomas Panas
// Date: 15-August-2008

#include "rose.h"
#include "compass.h"

using namespace std;

extern const Compass::CheckerUsingGraphProcessing* const cycleDetectionChecker;

namespace CompassAnalyses
{ 
  namespace CycleDetection
  { 
    /*! \brief Cycle Detection: Add your description here 
     */
    const std::string checkerName      = "CycleDetection";

    // Descriptions should not include the newline character "\n".
    const std::string shortDescription = "This checker looks for cycles in a function!";
    const std::string longDescription  = "This checker looks for cycles in a function!";

    // Specification of Checker Output Implementation
    class CheckerOutput: public Compass::OutputViolationBase
    { 
    public:
      CheckerOutput(SgNode* node,std::string s);
    };

    // Specification of Checker Traversal Implementation

    class Traversal
      : public Compass::GraphProcessingWithRunFunction //RoseBin_DataFlowAbstract
    {
      Compass::OutputObject* output;
      // Checker specific parameters should be allocated here.
      bool debug;
      std::vector<SgDirectedGraphNode*> successors;
      std::set<SgDirectedGraphNode*> visited;
      std::map<SgDirectedGraphNode*,SgDirectedGraphNode*> cycleFound;

    public:
      Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

      // Change the implementation of this function if you are using inherited attributes.
      void *initialInheritedAttribute() const { return NULL; }

      // The implementation of the run function has to match the traversal being called.
      bool run(string& name, SgDirectedGraphNode* node, SgDirectedGraphNode* previous);

      void run(SgNode*);

      bool runEdge(SgDirectedGraphNode* node, SgDirectedGraphNode* next) {
        return false;
      }
      bool checkIfValidCycle(SgDirectedGraphNode* node, SgDirectedGraphNode* next);
               
      void init(RoseBin_Graph* vg) {
        vizzGraph = vg;
      }

    };
  }
}

CompassAnalyses::CycleDetection::
CheckerOutput::CheckerOutput ( SgNode* node,std::string violation)
  : OutputViolationBase(node,::cycleDetectionChecker->checkerName,violation)
{}

CompassAnalyses::CycleDetection::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["CycleDetection.YourParameter"]);
  debug=false;

}

void
CompassAnalyses::CycleDetection::
Traversal::run ( SgNode* node )
{
}


// Checker main run function and metadata

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  //  CompassAnalyses::CycleDetection::Traversal(params, output).run(Compass::projectPrerequisite.getProject());

  // DQ (9/3/2008): Using new SgBinaryFile IR node.
  // SgAsmFile* file = Compass::projectPrerequisite.getProject()->get_file(0).get_binaryFile();
  SgBinaryFile* binaryFile = isSgBinaryFile(Compass::projectPrerequisite.getProject()->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;

  if (file==NULL)
    return;

  CompassAnalyses::CycleDetection::Traversal checker(params, output);
  checker.init(Compass::binDataFlowPrerequisite.getGraph());

  bool interprocedural = false;
  RoseBin_DataFlowAnalysis* dfanalysis = Compass::binDataFlowPrerequisite.getBinDataFlowInfo();
  vector<SgDirectedGraphNode*> rootNodes;
  dfanalysis->getRootNodes(rootNodes);
  dfanalysis->init();
  
  dfanalysis->traverseGraph(rootNodes, &checker, interprocedural);
}


// Remove this function if your checker is not an AST traversal
static Compass::GraphProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::CycleDetection::Traversal(params, output);
}

static Compass::PrerequisiteList getPrerequisites() {
  Compass::PrerequisiteList defusePre;
  defusePre.push_back(&Compass::projectPrerequisite);
  defusePre.push_back(&Compass::binDataFlowPrerequisite);
  return defusePre;
}


extern const Compass::CheckerUsingGraphProcessing* const cycleDetectionChecker =
  new Compass::CheckerUsingGraphProcessing(
                       "CycleDetection",
                       // Descriptions should not include the newline character "\n".
                       "Short description not written yet!",
                       "Long description not written yet!",
                       Compass::X86Assembly,
                       getPrerequisites(),
                       run,
                       createTraversal);
   




/**************************************************************************
 * If we think that a cycle exists. We want to make sure.
 * Therefore, we traverse the graph from the next node to the previous node
 * and check if such a path exists.
 **************************************************************************/
bool 
CompassAnalyses::CycleDetection::Traversal::checkIfValidCycle(SgDirectedGraphNode* node_n,
                                                              SgDirectedGraphNode* next_n){
  // traverse the graph from next to node
  bool foundCycle=false;
  std::vector<SgDirectedGraphNode*> successors_f;
  std::set<SgDirectedGraphNode*> visited_f;
  vector<SgDirectedGraphNode*> worklist;
  worklist.push_back(next_n);
  visited_f.insert(next_n);
  while (!worklist.empty()) {
    SgDirectedGraphNode* current = worklist.back();
    worklist.pop_back();
    successors_f.clear();
    if (debug)
      std::cerr << "current node " << current << " worklist size : " << 
	worklist.size() << "  visited size : " << visited_f.size() << std::endl;

    vizzGraph->getSuccessors(current, successors_f);    
    vector<SgDirectedGraphNode*>::iterator succ = successors_f.begin();
    for (;succ!=successors_f.end();++succ) {
      SgDirectedGraphNode* next = *succ;      
      if (debug)
	std::cerr << "worklist:  next node " << next << std::endl;
      if (sameParents(current,next)) { 
	if (next==next_n) {
	  // we have found a cycle
	  foundCycle=true;
	  worklist.clear();
	  if (debug)
	    std::cerr << "worklist: reached origin; abort. " <<  std::endl;
	  break;
	} else {
	  // cycle not found. If this node is in the same function
	  // and it has not been visited before, then we put it in 
	  // the worklist
	  std::set<SgDirectedGraphNode*>::iterator 
	    it =visited_f.find(next);
	  if (it==visited_f.end()) {
	    worklist.push_back(next);
	    visited_f.insert(next);
	    if (debug)
	      std::cerr << "worklist: push_back next node. " <<  std::endl;
	  } else {	
	    if (debug)
	      std::cerr << "worklist: no cycle. " <<  std::endl;
	  }

	}
      } else {
	if (debug)
	  std::cerr << "worklist:  different parents "  << std::endl;
      }
    } // for
  } // while
  return foundCycle;
}

/**************************************************************************
 * Main function. This function is run on each node that is being traversed
 * in the graph. For each node, we determine the successors and check
 * if those have been previously seen. If yes, a cycle may exist.
 **************************************************************************/
bool 
CompassAnalyses::CycleDetection::Traversal::run(string& name, SgDirectedGraphNode* node,
                                                SgDirectedGraphNode* previous){
  // check known function calls and resolve variables
  ROSE_ASSERT(node);

  //cerr << " cycledetection->run " << node->get_name() << endl;
  SgAsmFunctionDeclaration* func = isSgAsmFunctionDeclaration(node->get_SgNode());
  if (func) {
    // if the node is a function, we clear the visited nodes
    // this should speed up our search
    visited.clear();
    return false;
  }
  successors.clear();
  ROSE_ASSERT(vizzGraph);
  vizzGraph->getSuccessors(node, successors);    
  vector<SgDirectedGraphNode*>::iterator succ = successors.begin();
  for (;succ!=successors.end();++succ) {
    // for each successor do...
    SgDirectedGraphNode* next = *succ;
    // if the node is an instruction, we check if it was visited
    // if not, we add it to the visited set, otherwise a cycle is present
    std::set<SgDirectedGraphNode*>::iterator it =visited.find(next);
    if (it!=visited.end()) {
      // found this node in visited list
      SgAsmx86Instruction* nodeSg = isSgAsmx86Instruction(node->get_SgNode());
      SgAsmx86Instruction* nextSg = isSgAsmx86Instruction(next->get_SgNode());
      if (debug) {
        std::string outputText = "Found possible cycle between  ";
        outputText+=toString(nodeSg->get_kind()) + " (";
        outputText+=RoseBin_support::HexToString(nodeSg->get_address()) + ") and ";
        outputText+=toString(nextSg->get_kind()) + " (";
        outputText+=RoseBin_support::HexToString(nextSg->get_address()) + ")";
        std::cerr << outputText << std::endl;
        output->addOutput(new CheckerOutput(nodeSg, outputText));
      }
      bool validCycle = checkIfValidCycle(node,next);
      if (validCycle) {
        std::string outputText = "Found cycle between  ";
        outputText+=toString(nodeSg->get_kind()) + " (";
        outputText+=RoseBin_support::HexToString(nodeSg->get_address()) + ") and ";
        outputText+=toString(nextSg->get_kind()) + " (";
        outputText+=RoseBin_support::HexToString(nextSg->get_address()) + ")";
        std::cerr << outputText << std::endl;
        output->addOutput(new CheckerOutput(nodeSg, outputText));
	cycleFound[node]=next;
      } else {
	if (debug)
	  std::cerr << "This is not a cyclic node "  << std::endl;
      }
    }
  }
  visited.insert(node);
  return false;
}
