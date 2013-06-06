#include "findExactClones.h"


using namespace boost::program_options;
using namespace std;
int main(int argc, char* argv[])
{
  std::string database;


  std::vector<int> functionsThatWeAreInterestedIn;
  bool useCounts = false;
    try {
        options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce a help message")

            ("counts", "use counts as hash table entry")

            ("database,q", value< string >()->composing(), 
                 "the sqlite database that we are to use")

            ("function_id,f", value<vector<int> >(&functionsThatWeAreInterestedIn)->composing(), 
             "specify the file_ids that we are interested in")
                    ;

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);


        if (vm.count("help")) {
            cout << desc;            
			exit(0);
        }
        if (vm.count("counts")) {
          useCounts = true;
          std::cout << "useCounts: true" << std::endl;
        }else
          std::cout << "useCounts: false" << std::endl;


		if (vm.count("database")!=1  ) {
		  std::cerr << "Missing options. Call as: findClones --database <database-name>" 
			<< std::endl;
		  exit(1);

		}

		database = vm["database"].as<string >();
		cout << "database: " << database << std::endl;


    }
    catch(exception& e) {
        cout << e.what() << "\n";
    }

    scoped_array_with_size<VectorEntry> vectors;
    
    
  sqlite3_connection con(database.c_str());




  cerr << "About to delete from clusters" << endl;
  try{
    con.executenonquery("delete from clusters");
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }
  cerr << "... done" << endl;

  cerr << "About to delete from postprocessed_clusters" << endl;
  try{
    con.executenonquery("delete from postprocessed_clusters");
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }

  int windowSize = 0;
  int stride = 0;
  get_run_parameters(con, windowSize, stride);


  
  const size_t numStridesThatMustBeDifferent = windowSize / (stride * 2);


  try {
    sqlite3_command(con, "create temporary table function_to_look_for(function_id integer)").executenonquery();
  } catch (exception& ex) {cerr << "Exception Occurred 1 : " << ex.what() << endl;}

  try {

    for (size_t i=0; i < functionsThatWeAreInterestedIn.size() ; i++ )
    {
         sqlite3_command(con, "insert into function_to_look_for(function_id) values("+boost::lexical_cast<string>(functionsThatWeAreInterestedIn[i])+")").executenonquery();
    }

  } catch (exception& ex) {cerr << "Exception Occurred 1 : " << ex.what() << endl;}

  

  int groupLow=0;
  int groupHigh=-1;
  map<string, std::vector<int> > internTable;


  read_vector_data(con,vectors,  functionsThatWeAreInterestedIn, internTable, groupLow, groupHigh,useCounts);

  find_exact_clones(con, stride, windowSize, functionsThatWeAreInterestedIn, numStridesThatMustBeDifferent, vectors, internTable, useCounts);

#if 0
  if( functionsThatWeAreInterestedIn.size() == 0 )
  {
    long long threshold  = 10000000;
  long long sum=0;

    for( ; i < grouping.size() ; i++ ){

      sum+=grouping[i].second;

      if( sum >= threshold || i+1 == grouping.size() )
      {
        groupHigh=grouping[i].first;
        std::cout << "Looking for group low : " << groupLow << " groupHigh: " << groupHigh << std::endl;
        hash_map<void*, std::vector<VectorEntry> > internTable;

        read_vector_data(con,vectors, functionsThatWeAreInterestedIn, internTable, groupLow, groupHigh,useCounts);

        find_exact_clones(con, stride, windowSize, functionsThatWeAreInterestedIn, numStridesThatMustBeDifferent, internTable, useCounts);
        sum=0;
        if(i+1 < grouping.size())
          groupLow=grouping[i+1].first;
      }
    }

  }else{
        std::cout << "Looking for group low : " << groupLow << " groupHigh: " << groupHigh << std::endl;

        hash_map<void*, std::vector<VectorEntry> > internTable;

        read_vector_data(con,vectors, functionsThatWeAreInterestedIn, internTable, groupLow, groupHigh, useCounts);

        find_exact_clones(con, stride, windowSize, functionsThatWeAreInterestedIn, numStridesThatMustBeDifferent, internTable, useCounts);

  }
#endif
  //op.calculate_false_positives();

  return 0;
};



