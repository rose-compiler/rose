#ifndef SELECTIVE_TESTING_DEPENDENCE_ANALYSIS
#define SELECTIVE_TESTING_DEPENDENCE_ANALYSIS

#include <fstream>
#include <string>
#include <CommandOptions.h>
#include "dependence_table.h"
#include "AstUtilInterface.h"

class SgNode;
class SgProject;

namespace AstUtilInterface {
// Analyze the dependences among all the components of a software.
// The syntax for specifying a dependence is x1 : y1 y2 ... ym; which
// specifies that x1 is dependent on y1, y2, ..., ym, where x1, y1, ..., ym are
// strings that uniquely identifes each component of the whole program.
class WholeProgramDependenceAnalysis : public DependenceTable {
  public:
    DebugLog Log = DebugLog("-debug-dep-analysis");

    WholeProgramDependenceAnalysis(int argc, const char** argv); 

    // Read the dependences from an external file.
    void CollectPastResults(std::istream& input_file); 

    // Compute new dependences by parsing the files from command-line.
    // Returns whether the computed information is accurate (no unknowns). 
    void ComputeDependences(); 

    // Output the new dependences into a new file. 
    void OutputDependences(std::ostream& output); 

    // Output the new dependences into a new file. 
    void OutputDependencesInGUI(std::ostream& output) 
       { DependenceTable::OutputDependencesInGUI(output); }

    // Compute new dependences for the given AST  node
    void ComputeDependences(SgNode* input, SgNode* root);
    
    //! The operator op accesses the given memory reference in nature of the given relation.
    bool SaveOperatorSideEffect(SgNode* op, const AstNodePtr& varref, AstUtilInterface::OperatorSideEffect relation, SgNode* sig = 0); 
    //
    //! Erase existing annotation for the given operator.
    void ClearOperatorSideEffect(SgNode* op); 

 protected:
    virtual void save_dependence(const DependenceEntry& e) override;
  private:
    SgProject* sageProject = 0;
};

}; /* name space AstUtilInterface*/

#endif
