#include <string>
#include <iostream>
#include "sqlite3x.h"
#include <math.h>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include "computerangesFunc.h"

using namespace boost::program_options;
using namespace std;
using namespace sqlite3x;


struct group_probability
{
  double portion_of_total;
  int low;
  int high;

  group_probability(double p, int l, int h)
    : portion_of_total(p), low(l), high(h)
    { };
};

int main(int argc, char* argv[])
{
  std::string database;
  double distBound = 1.;
  try {
    options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce a help message")
      ("database", value< string >(&database), "The sqlite database that we are to use")
      ("distance,d", value< double >(&distBound), "The maximum distance that is allowed in a clone pair")

      ;

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);



    if (vm.count("help")) {
      cout << desc;            
      exit(0);
    }

    if (database == "") {
      std::cerr << "Missing options. Call as: " << argv[0] << " --database <database-name> [other parameters]" 
        << std::endl;
      exit(1);
    }

  }
  catch(exception& e) {
    cout << e.what() << "\n";
  }

  cout << "database: " << database << std::endl;
  cout << "distance: " << distBound<< std::endl;

  
  sqlite3_connection con(database.c_str());

  try {
    con.executenonquery("create table IF NOT EXISTS groups(low INTEGER, high INTEGER, num_elements INTEGER)");
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {
    con.executenonquery("delete from groups");
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }





  vector<Range> ranges = computeranges(distBound, 50, 100000);

  
  map<size_t, int> groupSizes;
  std::cout << "Looking for the biggest group" << std::endl;
  for (size_t i = 0; i < ranges.size(); ++i) {
    try {
      sqlite3_command cmd(con,
          ranges[i].high == -1 ? "SELECT count(row_number) from vectors where sum_of_counts >= ?"
          : "SELECT count(row_number) from vectors where sum_of_counts >= ? and sum_of_counts <= ?");
      cmd.bind(1, boost::lexical_cast<string>(ranges[i].low));
      if (ranges[i].high != -1) cmd.bind(2, boost::lexical_cast<string>(ranges[i].high));
      int numElementsInGroup = cmd.executeint();
      groupSizes[i] = numElementsInGroup;
      
      try{
        string query = "INSERT into groups(low,high,num_elements) VALUES(?,?,?) ";

        sqlite3_command cmd(con, query /* selectQuery.str() */ );

        cmd.bind(1, boost::lexical_cast<string>(ranges[i].low));
        cmd.bind(2, boost::lexical_cast<string>(ranges[i].high));
        cmd.bind(3, boost::lexical_cast<string>(numElementsInGroup));

        cmd.executenonquery();
      } catch(exception &ex) {
        cerr << "Exception Occurred: " << ex.what() << endl;
      }

    } catch (exception& e) {cerr << "Caught: " << e.what() << endl;}
  }

  return 0;
};



