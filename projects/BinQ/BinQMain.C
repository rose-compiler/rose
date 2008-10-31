/*
 *
 */

#include "rose.h"
#include <vector>
#include <qrose.h>
#include "BinQGui.h"
#include <boost/program_options.hpp>
#include <iostream>

using namespace qrs;
using namespace boost::program_options;
using namespace boost;
using namespace std;


int main( int argc, char **argv )
{
#if 0
  RoseBin_Def::RoseAssemblyLanguage = RoseBin_Def::x86;
  fprintf(stderr, "Starting binCompass frontend...\n");
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  fprintf(stderr, "End binCompass frontend...\n\n\n");
#endif


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
      std::cerr << "Missing options. Call as: BinQ --fileA <file A> --fileB <file B>" 
        << std::endl;
      exit(1);

    }

    fileA = vm["fileA"].as<string >();
    fileB = vm["fileB"].as<string >();
    cout << "File A: " << fileA << " File B: " << fileB << std::endl;


  }
  catch(exception& e) {
    cout << e.what() << "\n";
  }



  BinQGUI binGui(fileA,fileB);
  binGui.run();
  return QROSE::exec();
}
