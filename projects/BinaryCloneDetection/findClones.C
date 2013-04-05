#include "callLSH.h"
#include <string>
#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
using namespace boost::program_options;
using namespace std;
int main(int argc, char* argv[])
{
  std::string database;
  int norm = 1;
  double similarity_threshold=1.;

  size_t k;
  size_t l;
    try {
        options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce a help message")
            ("database,q", value< string >()->composing(), 
                 "the sqlite database that we are to use")
            ("norm,p", value< int >(&norm), "Exponent in p-norm to use (1 or 2 or 3 (MIT implementation) )")
            ("hash-function-size,k", value< size_t >(&k), "The number of elements in a single hash function")
            ("hash-table-count,l", value< size_t >(&l), "The number of separate hash tables to create")
            ("similarity,t", value< double >(&similarity_threshold), "The similarity threshold that is allowed in a clone pair")

        ;

//        variables_map vm;
//        store(command_line_parser(argc, argv).options(desc)
//              .run(), vm);

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

        if (vm.count("help")) {
            cout << desc;            
			exit(0);
        }
		
		if (vm.count("database")!=1  ) {
		  std::cerr << "Missing options. Call as: findClones --database <database-name>" 
			<< std::endl;
		  exit(1);

		}

		database = vm["database"].as<string >();
                similarity_threshold = vm["similarity"].as<double>();
		cout << "database: " << database << std::endl;


    }
    catch(exception& e) {
        cout << e.what() << "\n";
    }

 
    std::cout << "The similarity threshold is " << similarity_threshold << std::endl;
  OperateOnClusters op(database, norm, similarity_threshold, k , l);
  
  op.analyzeClusters();
  //op.calculate_false_positives();

  return 0;
};



