#ifndef SELECTIVE_TESTING_DEPENDENCE_ANALYSIS
#define SELECTIVE_TESTING_DEPENDENCE_ANALYSIS

#include <fstream>
#include <string>
#include <CommandOptions.h>
#include "dependence_table.h"
#include <Rose/FlowAnalysisInterface.h>

class SgNode;
class SgProject;

namespace AstUtilInterface {
//
// Read a table that records the dependences among all the components of a software.
// The syntax for specifying a dependence is x1 : y1 y2 ... ym; which
// specifies that x1 is dependent on y1, y2, ..., ym, where x1, y1, ..., ym are
// strings that uniquely identifes each component of the whole program.
class CollectDependences{
  protected:
    bool update_annotations_ = false;
    std::string local_read_string(std::istream& input_file);
    void save_annotation(const DependenceEntry& e); 
    
  public:
    DebugLog Log = DebugLog("-debug-dep-table");
    CollectDependences( bool update_annotations=false) 
      : update_annotations_(update_annotations) {}
    void set_update_annotations(bool what) { update_annotations_ = what; }
    // Read the system dependence table from an input file.
    void CollectFromFile(std::istream& input_file, DependenceTable& main_table);
};

// Analyze the dependences among all the components of a software.
// The syntax for specifying a dependence is x1 : y1 y2 ... ym; which
// specifies that x1 is dependent on y1, y2, ..., ym, where x1, y1, ..., ym are
// strings that uniquely identifes each component of the whole program.

// Make the analysis portable across differeng graph representations.
template <class NodeIterator, class EdgeIterator>
class WholeProgramDependenceAnalysis : public CollectDependences {
    // Use the Log variable to enable logging of the analysis process.
    DebugLog Log = DebugLog("-debug-dep-analysis");

  public:
    /* Here connect to the portable ROSE FlowGraph Create interface */
    typedef Rose::FlowGraphInterface::FlowGraphCreateInterface<SgNode*,OperatorSideEffect, NodeIterator,EdgeIterator> GraphCreate;

    WholeProgramDependenceAnalysis(GraphCreate* _main_table, SgProject *_root)
      : main_table(_main_table), sageProject(_root) 
        { assert(main_table != 0); }

    void set_save_annotations(DependenceTable* _annot_table) { 
          annot_table = _annot_table; 
      } 
    
    // Returns the saved dependences.
    GraphCreate* getDependences() { return main_table; }
    // Returns the saved side effects of functions.
    DependenceTable* getAnnotations() { return annot_table; }

    // Compute new dependences by parsing the files from command-line.
    // Returns whether the computed information is accurate (no unknowns). 
    bool ComputeDependences(); 

    // Compute new dependences for the given AST  node
    void ComputeDependences(SgNode* input, SgNode* root);

 private:
    GraphCreate *main_table = 0;
    DependenceTable *annot_table = 0;
    SgProject* sageProject = 0;
};
}; /* name space AstUtilInterface*/


/* Here implement the portable ROSE analysis interface */
// Internally automatically select the desired analysis based on 
template <class NodeIterator, class EdgeIterator> 
class Rose::FlowGraphInterface::CallGraphAnalysis
      <NodeIterator, EdgeIterator, Rose::FlowGraphInterface::AnalysisSupportOption::AstSideEffect> { 
 typedef FlowGraphCreateInterface<SgNode*,SgNode*, NodeIterator,EdgeIterator> GraphCreate;
 bool performAnalysis(const SgNode* ast, GraphCreate& graph) { 
      AstUtilInterface::WholeProgramDependenceAnalysis<NodeIterator,EdgeIterator> op(&graph,0, ast); 
      return op.ComputeDependences();
   }
 };


#endif
