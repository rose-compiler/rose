#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "rose.h"

#include "APIReader.h"
#include "APIDepFinder.h"
#include "DangerousOperationFinder.h"
#include "APIDepChecker.h"
#include "GenericDepAttrib.h"
#include "Outline.h"
#include "processPragmas.h"
#include "annotatePragmas.h"
#include "Utils.h"

#include <Outliner.hh>
#include <staticSingleAssignment.h>

int main(int argc, char **argv) {

    bool outline = false;  // if true,   'outlines' non-skeleton code
                           // otherwise, removes non-skeleton code
    bool genPDF = false;   // if true, generate a PDF dump of the entire
                           // AST of the input program

    //
    // Local Command Line Processing:
    //
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
        std::cout << "Warning: no API specification given."     << std::endl;
    }
    
    std::string sig_fname;
    if( CommandlineProcessing::isOptionWithParameter(l, "-skel:",
                                                     "(g|sig)",
                                                     sig_fname,
                                                     true) ){
        std::cout << "Using signature file: " << sig_fname << std::endl;
    } else {
        std::cout << "Warning: No signature given." << std::endl;
    }
    // We want this to be the transitive closure of the signatures
    // TODO: Change the data format so that no accidents can happen?
    Signatures sigs;
    if( sig_fname != "" ){
       sigs = readSignatures(sig_fname, debug);
       if (debug) std::cout << "Using signatures: " << std::endl
                            << sigs
                            << "End signatures"     << std::endl;
    }

    // Now we attempt to augument the APISpec with the data from the
    // signatures file
    buildAPISpecFromSignature( apiSpecs
                             , sigs );
    //
    // parse and create project
    //
    SgProject* const project = frontend(l);

    //
    // Run the SSA analysis
    //
    if(debug) std::cout << "Running SSA analysis"               << std::endl;
    StaticSingleAssignment ssa(project);
    ssa.run(true, true);

    ClassHierarchyWrapper chw(project);

    // Store all known function definitions, indexed by mangled name,
    // because declarations don't reliably link to them. For example,
    // some member function definitions come via
    // SgMemberFunctionDeclaration, which would be missed by a node query.
    if(debug) std::cout << "Build SgSymbol |-> SgFunctionDeclaration map"
                                                                << std::endl;
    const std::map <SgSymbol*, SgFunctionDeclaration*> declTable;
    buildSymbolToDeclMap(project, declTable);

    // Find the dependencies of API calls:
    if(debug) std::cout << "Running dependency finder"          << std::endl;
    APIDepFinder df(&ssa, declTable, &chw, &apiSpecs, sig_fname == "" ? NULL : &sigs);
    df.traverseInputFiles(project);//, postorder);

    // Print warnings for dangerous operators:
    // (dangerous = operator may crash program [/0, segfault] on some inputs)
    if(debug) std::cout << "Running dependency checker"         << std::endl;
    APIDepChecker checker(&apiSpecs);
    DangerousOperationFinder dof(&ssa, &checker);
    dof.traverseInputFiles(project); //, postorder);

    // Annotate Pragmas: add attributes to AST so that skeletonizing plays
    // well with pragmas; must be done before skeletonization.
    if(debug) std::cout << "PreProcessing Pragmas"              << std::endl;
    annotatePragmas(project);

    // Create skeletons from the code (& generates report):
    if(debug) std::cout << "Create Skeleton"                    << std::endl;
    skeletonizeCode(&apiSpecs, project, outline, genPDF);

    // Process Pragmas
    if(debug) std::cout << "Processing Pragmas"                 << std::endl;
    processPragmas(project, outline, NULL);

    // Generating modified code:
    if(debug) std::cout << "Generating modified code"           << std::endl;
    project->skipfinalCompileStep(true);
    backend(project);

    return 0;
}
