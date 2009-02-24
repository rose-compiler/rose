/*
 *
 */
#include "rose.h"
#include <vector>
#include <qrose.h>
#include "astEquivalenceGui.h"
#include <boost/program_options.hpp>
#include <iostream>

using namespace qrs;
using namespace boost::program_options;
using namespace boost;
using namespace std;


int main( int argc, char **argv )
{
  QROSE::init(argc,argv);

  std::string fileA,fileB;

  try {
    options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce a help message")
      ("fileA,a", value< string >()->composing(), 
       "file A to be diffed")
      ("fileB,b", value< string >()->composing(), 
       "file B to be diffed")
      ;

    variables_map vm;
    store(command_line_parser(argc, argv).options(desc)
        .run(), vm);


    if (vm.count("help")) {
      cout << desc;            
      exit(0);
    }

    if (vm.count("fileA")!=1 ||vm.count("fileB")!=1 ) {
      std::cerr << "Missing options. Call as: astEquivalence --fileA <file A> --fileB <file B>" 
        << std::endl;
      exit(1);

    }

    fileA = vm["fileA"].as<string >();
    fileB = vm["fileB"].as<string >();
    cout << "File A: " << fileA << " File B: " << fileB << std::endl;


  }
  catch(std::exception& e) {
    cout << e.what() << "\n";
  }



  
  BinaryCloneGui binaryCloneGui(fileA,fileB);

  binaryCloneGui.run();
  
  return QROSE::exec();
}
