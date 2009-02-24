/*
 *
 */
#include "rose.h"
#include <vector>
#include <qrose.h>
#include "compilerFlagsGui.h"
#include <boost/program_options.hpp>

using namespace qrs;
using namespace boost::program_options;
using namespace boost;
using namespace std;


int main( int argc, char **argv )
{
  QROSE::init(argc,argv);


  std::string dbA,dbB;

  try {
    options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce a help message")
      ("dbA,a", value< string >()->composing(), 
       "db A to be diffed")
      ("dbB,b", value< string >()->composing(), 
       "db B to be diffed")
      ;

    variables_map vm;
    store(command_line_parser(argc, argv).options(desc)
        .run(), vm);


    if (vm.count("help")) {
      cout << desc;            
      exit(0);
    }

    if (vm.count("dbA")!=1 ||vm.count("dbB")!=1 ) {
      std::cerr << "Missing options. Call as: astEquivalence --dbA <db A> --dbB <db B>" 
        << std::endl;
      exit(1);

    }

    dbA = vm["dbA"].as<string >();
    dbB = vm["dbB"].as<string >();
    cout << "File A: " << dbA << " File B: " << dbB << std::endl;


  }
  catch(std::exception& e) {
    cout << e.what() << "\n";
  }



  BinaryCloneGui binaryCloneGui(dbA, dbB);
//  binaryCloneGui.run();
  return QROSE::exec();
}
