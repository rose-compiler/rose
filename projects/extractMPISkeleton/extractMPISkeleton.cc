#include "APIReader.h"
#include "APIDepFinder.h"
#include "DangerousOperationFinder.h"
#include "GenericDepAttrib.h"
#include "Outline.h"
#include "processPragmas.h"

#include <Outliner.hh>
#include <staticSingleAssignment.h>

class APIDepChecker : public NodeChecker {
  private:

    APISpecs *specs;

  public:

    APIDepChecker(APISpecs *s) : specs(s) {}

    bool check(SgNode *n) {
        AstAttribute *attr = n->getAttribute("APIDep");
        //std::cout << "Check! " << n->unparseToString() << std::endl;
        // Note: the def-use analysis will point us at the defining
        // *statement* for a given variable. If that's an API call, it
        // could be a problem, but we'll need to know more to know
        // whether the definition is dependent to ommitted data.
        if(attr) {
            if(debug) {
                std::cout << "Attribute: " << attr->toString() << std::endl;
                std::cout << "  " << n->unparseToString() << std::endl;
            }
            SgFunctionCallExp *fc = isSgFunctionCallExp(n);
            bool omitted = false;
            if(fc) {
                SgExpression *fe = fc->get_function();
                if(debug)
                    std::cout << "  Function: "
                              << fe->unparseToString()
                              << std::endl;
                SgFunctionRefExp *fr = isSgFunctionRefExp(fe);
                if(fr) {
                    std::string name = fr->get_symbol()->get_name().getString();
                    APISpec *spec = lookupFunction(specs, name);
                    if(spec) {
                        ArgTreatments *ts = spec->getFunction(name);
                        ArgTreatments::iterator argAttr = ts->begin();
                        for(; argAttr != ts->end(); argAttr++) {
                            GenericDepAttribute *depAttr =
                                (GenericDepAttribute *)(*argAttr);
                            omitted = spec->shouldOmit(depAttr);
                        }
                    }
                }
            }
            return omitted;
        } else {
            return false;
        }
    }
};

int main(int argc, char **argv) {

    bool outline = false;  // if true,   'outlines' non-skeleton code
                           // otherwise, removes non-skeleton code
    bool genPDF = false;   // if true, generate a PDF dump of the entire
                           // AST of the input program

    // Local Command Line Processing:
    Rose_STL_Container<std::string> l =
      CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
    if ( CommandlineProcessing::isOption(l,"-skel:","(o|outline)",true) ) {
      std::cout << "Outlining on." << std::endl;
      outline = true;
    }
    if ( CommandlineProcessing::isOption(l,"-skel:","(d|debug)",true) ) {
        debug = true;
    }
    if ( CommandlineProcessing::isOption(l,"-skel:","(p|pdf)",true) ) {
        genPDF = true;
    }

    std::string spec_fname;
    APISpecs apiSpecs;
    if ( CommandlineProcessing::isOptionWithParameter(l, "-skel:",
                                                      "(s|spec)",
                                                      spec_fname,
                                                      true) ) {
        std::cout << "Using API specification: " << spec_fname << std::endl;
        apiSpecs = readAPISpecCollection(spec_fname);
    } else {
        std::cout << "Warning: no API specification given." << std::endl;
    }

    SgProject* project = frontend(l);

    // Run the Backstroke Def-Use analysis
    if(debug) std::cout << "Running backstroke def-use analysis" << std::endl;
    StaticSingleAssignment ssa(project);
    ssa.run(true, true);

    // Find the dependencies of API calls:
    if(debug) std::cout << "Running dependency finder" << std::endl;

    APIDepFinder *df = new APIDepFinder(&ssa, &apiSpecs);
    df->traverse(project);
    df->finalize(project);

    // Print warnings for dangerous operators:
    // (dangerous = operator may crash program [/0, segfault] on some inputs)

    if(debug) std::cout << "Running dependency checker" << std::endl;
    APIDepChecker *checker = new APIDepChecker(&apiSpecs);
    DangerousOperationFinder *dof =
        new DangerousOperationFinder(&ssa, checker);
    dof->traverse(project);

    // Create skeletons from the code (& generates report):
    if(debug) std::cout << "Create Skeleton" << std::endl;
    skeletonizeCode(&apiSpecs, project, outline, genPDF);

    // Process Pragmas
    if(debug) std::cout << "Processing Pragmas" << std::endl;
    processPragmas(project);

    // Generating modified code:
    if(debug) std::cout << "Generating modified code" << std::endl;
    project->skipfinalCompileStep(true);
    backend(project);

    // Clean up:
    delete dof;
    delete checker;
    delete df;

    return 0;
}
