#include <boost/foreach.hpp>


#include <rose.h>
#include "ssexpr.h"
#include "APISpec.h"
#include "APIReader.h"
#include "SignatureReader.h"
#include <iostream>
#define foreach BOOST_FOREACH
// Looks for a command line like:
// ./foo --bar -baz -- 1.sig 2.sig 3.sig
// Starting from the -- it will unconditionally grab
// each thing on the command line and treat it as a file name
std::vector<std::string> getFileNames(int argc, char** argv)
{
  int i = 0;
  for(; i < argc; ++i){
    if(strcmp(argv[i],"--") == 0){
      // all arguments after this point are to be interpreted
      // as file names containing signature data
      ++i; // consume the "--"
      break;
    }
  }
  // grab the file names
  std::vector<std::string> fnames;
  for(; i < argc; ++i){
     fnames.push_back(argv[i]);
  }
  return fnames;
}

int main(int argc, char **argv) {

    bool debug  = false;   // if true, generate lots of debugging output
    const std::string summarize("-summarize:");

    //
    // Local Command Line Processing:
    //
    // TODO: I don't think we actually need rose here
    Rose_STL_Container<std::string> l =
      CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
    if ( CommandlineProcessing::isOption(l,summarize,"(d|debug)",true) ) {
        debug = true;
    }

    std::string spec_fname;
    APISpecs apiSpecs;
    if ( CommandlineProcessing::isOptionWithParameter(l, summarize,
                                                      "(s|spec)",
                                                      spec_fname,
                                                      true) ) {
        std::cout << "Using API specification: " << spec_fname << std::endl;
        apiSpecs = readAPISpecCollection(spec_fname);
    } else {
        std::cout << "Warning: no API specification given."     << std::endl;
    }

    std::string sig_fname;
    if( CommandlineProcessing::isOptionWithParameter(l, summarize,
                                                     "(o|output)",
                                                     sig_fname,
                                                     true) ){
      std::cout << "Using output filename: " << sig_fname << std::endl;
    } else {
        std::cout << "Warning: No output filename given. Using standard out." << std::endl;
    }

    std::vector<std::string> sigFNames = getFileNames(argc, argv);

    if(debug) std::cout << "Signature file names:" << std::endl;
    foreach(const std::string & sigFName, sigFNames){
       if(debug) std::cout << "    " << sigFName << std::endl;
    }

    Signatures allSigs;
    foreach(const std::string & sigFName, sigFNames){
       const Signatures sigs(readSignatures(sigFName, debug));
       foreach(const Signatures::value_type & pair, sigs){
         // FIXME: Check if pair.first is already a key in allSigs and freakout
         // if it is. Why? Because that means we have non-unique function definitions.
         // ie., foo is defined twice. Are mangled names enough?
         allSigs.insert(pair);
       }
    }

    if (debug) std::cout << "Using signatures: " << std::endl
                         << allSigs
                         << "End signatures"     << std::endl;

    // Now do a dependency propagation, ie., if A calls B and B calls MPI_Send
    // then we want to say that A (indirectly) calls MPI_Send
    const Signatures transitive_sigs(transitiveClosure(allSigs));
    if (debug) std::cout << "Transitive closure of signatures: " << std::endl
                         << transitive_sigs
                         << "End Tranistive signatures" << std::endl;
    if( sig_fname != "" ){
      std::ofstream out(std::string(sig_fname).c_str());
#if __cplusplus < 201103L
      std::auto_ptr<SExpr> sx(toSExpr(transitive_sigs));
#else
      std::unique_ptr<SExpr> sx(toSExpr(transitive_sigs));
#endif
      out << *sx << std::endl;
    }
    return 0;
}
