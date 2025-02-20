#ifndef SELECTIVE_TESTING_DEPENDENCE_ANALYSIS
#define SELECTIVE_TESTING_DEPENDENCE_ANALYSIS

#include <fstream>
#include <string>
#include <CommandOptions.h>
#include "dependence_table.h"

class SgNode;
class SgProject;

namespace AstUtilInterface {
// Analyze the dependences among all the components of a software.
// The syntax for specifying a dependence is x1 : y1 y2 ... ym; which
// specifies that x1 is dependent on y1, y2, ..., ym, where x1, y1, ..., ym are
// strings that uniquely identifes each component of the whole program.
class WholeProgramDependenceAnalysis  {
  public:
    DebugLog Log = DebugLog("-debug-dep-analysis");

    WholeProgramDependenceAnalysis(int argc, const char** argv); 
    WholeProgramDependenceAnalysis() 
      : sageProject(0), main_table(false), annot_table(true) {}

    // Read the dependences from an external file.
    void CollectPastResults(std::istream& input_file, std::istream* annot_file = 0); 

    // Compute new dependences by parsing the files from command-line.
    // Returns whether the computed information is accurate (no unknowns). 
    void ComputeDependences(); 

    // Output the new dependences into a new file. 
    void OutputDependences(std::ostream& output); 
    // Output the saved side effects of functions into a new file. 
    void OutputAnnotations(std::ostream& output); 

    // Output the new dependences into a new file. 
    void OutputDependencesInGUI(std::ostream& output) 
       { main_table.OutputDependencesInGUI(output); }

    // Compute new dependences for the given AST  node
    void ComputeDependences(SgNode* input, SgNode* root);

     // Read the dependences from an external file.
    void CollectPastResults(std::istream& input_file) 
       { main_table.CollectFromFile(input_file); } 

    
 private:
    DependenceTable main_table, annot_table;
    SgProject* sageProject = 0;
};

}; /* name space AstUtilInterface*/

#endif
