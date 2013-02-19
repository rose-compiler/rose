#include "callLSH.h"
#include "computerangesFunc.h"
#include "vectorCompression.h"
#include "lshParameterFinding.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <math.h>
#include <iostream>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;
using namespace sqlite3x;
namespace fs = boost::filesystem;


static 
void insert_into_clusters(
    sqlite3_connection& con,
    int cluster,  
    int function_id, 
    int index_within_function,
    int vectors_row, 
    int dist 
    );




static void executeLSHCode(sqlite3_connection& con, const string& databaseName, const string& Exec, const string& paramFileName, const Range& range);
static void executeLSHCodeLLNL(sqlite3_connection& con, const std::string& databaseName, const string& Exec, const string& paramFileName, const Range& range, int norm, double similarity, double false_negative_rate, int num_vectors);



#include <vector>
#include <math.h>
#include <iostream>
#include <stdlib.h>



double getL(int k, double similarity_threshold, double false_negative_rate, double groupLow)
{
     double distance = similarity_threshold==1.  ? 1. : sqrt(2*groupLow*(1.-similarity_threshold));

     return ceil(log(false_negative_rate)/log(1-pow(1-distance/8000,k)));
}




double pfL2(double c, double r)
{
   double Pi = 4.0*atan(1.0);
   return  -erf(-r/(c*sqrt(2)))-2*c*(1-exp(- (pow(r,2)*0.5/pow(c,2) )))/(sqrt(2*Pi)*r) ;

};

double getL2(double k, double r, double similarity_threshold, double false_negative_rate)
{

     return ceil(log(false_negative_rate)/log(1- pow(pfL2(1.,r),k) ));
}

double computeQualityL2(  double k,  double r, double similarity_threshold, double false_negative_rate, double groupLow)
{
     double distance = similarity_threshold==1.  ? 1. : sqrt(2*groupLow*(1.-similarity_threshold));
     double quality=0;
     for (int c = 0; c <= 100; c++) // X is the distance
     {
       double y = 1-pow((1- pow(pfL2(c/35,r),k)),getL2(k,r,similarity_threshold,false_negative_rate));
       //std::cout << fabs(y) << std::endl;
       quality+=fabs(distance <= c ? 1-y : -y);
     }

     return quality;
     

};


double computeQuality(double k, double l, double similarity_threshold, double groupLow)
{
  double distance = similarity_threshold==1.  ? 1. : sqrt(2*groupLow*(1.-similarity_threshold));


  double quality=0;
  for (int x =1; x <= 101; x++) // X is the distance
  {
    double y = 1-pow(1-pow(1-(double)x/8000,k),l);
//    std::cout << fabs( distance <= x ? 1-y : -y ) << std::endl;
    quality+=fabs(distance <= x ? 1-y : -y);
  }

  return quality;


};




size_t computeSizeOfMemoryUsage(int num_buckets, int num_elem_per_bucket, int num_vectors, int l)
{
  int estimatedCompressedVecSize = 120;
  
  //Contribtions from hashtables
  size_t hashtableContrib = num_buckets*l*( num_elem_per_bucket*sizeof(size_t)
      + sizeof(size_t));
  //Contribution from storing the vectors
  size_t vectorContrib    = (estimatedCompressedVecSize+sizeof(VectorEntry) )*num_vectors;

  return vectorContrib+hashtableContrib;

};

/*
int costFunction( int num_buckets, int num_elem_per_bucket, int num_vectors, int k, int l,  size_t available_memory=160000000000)
{
  //size_t totalMemUsage = computeSizeOfMemoryUsage(num_buckets, num_elem_per_bucket, num_vectors, l);

   return -1; 

  
}
*/
struct parameters{
  size_t k;
  size_t l;
  size_t num_buckets;
  size_t num_elem_per_bucket;

  parameters(size_t kVal, size_t lVal, size_t num_bucketsVal, size_t num_elem_per_bucketVal)
    : k(kVal), l(lVal), num_buckets(num_bucketsVal), num_elem_per_bucket(num_elem_per_bucketVal)
    {};
  
};


parameters selectParameters(double false_negative_rate, double similarity_threshold, int groupLow, int num_elem_per_bucket, int num_vectors,  int norm, size_t available_memory=1600000000)
{
 
  std::cout << "Norm is: " << norm << std::endl;
  if(norm != 1 && norm != 2) exit(1);
   //Calculate how many buckets are needed
   size_t num_buckets=(size_t)ceil(num_vectors*2./num_elem_per_bucket);
   if(num_buckets<=2) num_buckets=3;
   
   double r = 4.;
   
   std::vector<size_t> kVals;
   std::vector<size_t> lVals;
   std::vector<double> qualityVals;
  
   double bestQuality=0;
   for(int k = 1 ; k<=  ( (norm ==1)? 1000 : 30 ); k+=  ( (norm ==1)? 50 : 1 ) )
   {
     double l = norm ==1 ? getL(k, similarity_threshold, false_negative_rate, groupLow)
         : getL2(k, r, similarity_threshold, false_negative_rate);

     double quality = norm == 1 ? computeQuality(k,l,similarity_threshold, groupLow)
       : computeQualityL2(k,r,similarity_threshold,false_negative_rate,groupLow) ;

     if(quality > bestQuality)
       bestQuality = quality;

     kVals.push_back(k);
     lVals.push_back((int)l) ;
     qualityVals.push_back(quality);
     std::cout << "k " << k << " l " << l <<  " Quality " << quality << std::endl;
   }


   double allowedDistanceFromTopQuality = 0.1;
   int bestIndex=0;
   for( int i =0; i < qualityVals.size() ; i++ )
   {
     if(qualityVals[i]>=(1.-allowedDistanceFromTopQuality)*bestQuality /*&& ( lVals[i]>=20 || (i == qualityVals.size()-1) )*/ )
     {
       bestIndex=i;
       break;
     }
     
     if( computeSizeOfMemoryUsage(num_buckets, num_elem_per_bucket, num_vectors, lVals[i]) > available_memory )
     {
       bestIndex = i-1;
       break;
     }

   }


   
   return parameters(kVals[bestIndex],lVals[bestIndex],num_buckets, num_elem_per_bucket);
};


static void 
callExact(sqlite3_connection& con, const std::string databaseName, const string& Exec)
{


  try {
    con.executenonquery("create table IF NOT EXISTS detection_parameters(similarity_threshold FLOAT, false_negative_rate FLOAT)");
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {con.executenonquery("delete from detection_parameters");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  try {
    sqlite3_command cmd(con, "insert into detection_parameters(similarity_threshold, false_negative_rate) values (?, ?)");
    cmd.bind(1, 1.0);
    cmd.bind(2, 0);
    cmd.executenonquery();
  } catch (exception& e) {cerr << "Exception checking run parameters " << e.what() << endl;}



  
  std::cout << "Start running exact clone detection" << std::endl;

  int groupLow =0;
  int groupHigh=-1;
  
    pid_t p = fork();

  if (p == -1) { // Error
    perror("fork: ");
    exit (1);
  } if (p == 0) { // Child



    vector<char*> args;
    args.push_back(strdup(Exec.c_str()));
/*    args.push_back("-g");
    args.push_back(strdup(boost::lexical_cast<string>(groupLow).c_str()));
    args.push_back("-G");
    args.push_back(strdup(boost::lexical_cast<string>(groupHigh).c_str()));*/
    args.push_back("--database");
    args.push_back(strdup(databaseName.c_str()));
    args.push_back(0);

    ostringstream outStr; 
    for(vector<char*>::iterator iItr = args.begin(); iItr != args.end();
        ++iItr )
    {
      outStr << *iItr << " ";
    }     
    ;
    std::cout << "Calling " << outStr.str() << std::endl;


    execv(Exec.c_str(), &args[0]);

    perror("execv: ");
    exit (1);
  } else { // Parent
    int status;
    if (waitpid(p, &status, 0) == -1) {
      perror("waitpid");
      abort();
    }
    cerr << "Status: " << status << endl;
    cerr << "Done waiting for Exact Clone Detection" << endl;
  }

 
};


static void
callLSH(sqlite3_connection& con, const std::string databaseName, double similarity_threshold, const string& Exec, int norm, size_t hash_function_size, size_t hash_table_count)
{

  double distance = sqrt((1. - similarity_threshold) * 50.);
  double false_negative_rate = ( similarity_threshold != 1.0) ? 0.0100 : 0;

  vector<Range> ranges = computeranges(distance, 50, 100000);

  int maxNumElementsInGroup = -1;
  int maxNumElementIdx = -1;



  try {
    con.executenonquery("create table IF NOT EXISTS detection_parameters(similarity_threshold FLOAT, false_negative_rate FLOAT)");
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }

  try {con.executenonquery("delete from detection_parameters");}
  catch(exception &ex) {cerr << "Exception Occurred: " << ex.what() << endl;}

  try {
    sqlite3_command cmd(con, "insert into detection_parameters(similarity_threshold, false_negative_rate) values (?, ?)");
    cmd.bind(1, similarity_threshold);
    cmd.bind(2, false_negative_rate);
    cmd.executenonquery();
  } catch (exception& e) {cerr << "Exception checking run parameters " << e.what() << endl;}



  
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
          std::cerr << "The current group from " << ranges[i].low << " to " << ranges[i].high << " is of size " << numElementsInGroup 
                    << std::endl;
	  if (numElementsInGroup > maxNumElementsInGroup) {
		maxNumElementsInGroup = numElementsInGroup;
		maxNumElementIdx = i;
	  }
	} catch (exception& e) {cerr << "Caught: " << e.what() << endl;}
  }

  std::cout << "Biggest group found " << ranges[maxNumElementIdx].low << " " << ranges[maxNumElementIdx].high << std::endl;
  


//  std::pair<size_t, size_t> runLSH();
  char tempDirName[] = "/tmp/paramdirXXXXXX";
  char* mkdtempResult = mkdtemp(tempDirName);
  if (!mkdtempResult) {
	perror("mkdtemp: ");
	exit (1);
  }
  string paramFileName = string(tempDirName) + "/params";
  paramFileName = "/tmp/lshparamdirE40hF1/params";
  std::cout << "Number of groups :" << ranges.size() << std::endl;


  
  try {
	for (size_t i = 0; i < ranges.size(); ++i) {
	  size_t group = (i == 0) ? maxNumElementIdx : (i <= maxNumElementIdx) ? i - 1 : i;
	  if (groupSizes[group] > 1) {
		std::cout << "Executing LSH code low " << ranges[group].low  
				  << " high " << ranges[group].high << " group  " << group << " size " << groupSizes[group] << std::endl;


                if(norm == 3)
                   executeLSHCode(con, databaseName, Exec, paramFileName, ranges[group]);
                else
                {


                  executeLSHCodeLLNL(con, databaseName, Exec, paramFileName, ranges[group],norm,similarity_threshold, false_negative_rate,groupSizes[group]);
                }

	  }

	}
  } catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }

  unlink(paramFileName.c_str());
  rmdir(tempDirName);
}


static void postprocess(sqlite3_connection& con) {
  int windowSize = 0;
  int stride = 0;
  try {
	windowSize = sqlite3_command(con, "select window_size from run_parameters limit 1").executeint();
  } catch (exception& ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
  try {
	stride = sqlite3_command(con, "select stride from run_parameters limit 1").executeint();
  } catch (exception& ex) {cerr << "Exception Occurred: " << ex.what() << endl;}
  assert (windowSize != 0);
  assert (stride != 0);


  cerr << "About to delete from postprocessed_clusters" << endl;
  try{
	  con.executenonquery("delete from postprocessed_clusters");
  }
  catch(exception &ex) {
	  cerr << "Exception Occurred: " << ex.what() << endl;
  }

  
  cerr << "... done" << endl;

  cerr << "About to postprocess" << endl;
  sqlite3_command cmd(con, "select cluster, function_id, index_within_function, row_number from clusters order by cluster, function_id, index_within_function");
  sqlite3_reader postproc_reader=cmd.executereader();
  const size_t numStridesThatMustBeDifferent = windowSize / (stride * 2);
  {
    string last_cluster = "";
    string last_func_id = "";
    size_t last_index_within_function = 0;
    vector<string> rows_in_this_cluster;
    bool first = true;
    while (true) {
      bool haveRecord = postproc_reader.read();
      string cluster = haveRecord ? postproc_reader.getstring(0) : "";
      string function_id = haveRecord ? postproc_reader.getstring(1) : "";
      size_t index_within_function = haveRecord ? boost::lexical_cast<size_t>(postproc_reader.getstring(2)) : 0;
      string cluster_row_number = haveRecord ? postproc_reader.getstring(3) : "";
      bool differentFunction = cluster != last_cluster || function_id != last_func_id;
      bool endingCluster = !haveRecord || differentFunction;
      bool beginningNewCluster = haveRecord && (first || differentFunction);
      first = false;
      if (endingCluster) {
        if (rows_in_this_cluster.size() > 1) { // Skip clusters that have only one element left
          for (size_t i = 0; i < rows_in_this_cluster.size(); ++i) {
            sqlite3_command insertCmd(con, "insert into postprocessed_clusters select * from clusters where row_number = ?");
            insertCmd.bind(1, rows_in_this_cluster[i]);
            try {insertCmd.executenonquery();} catch (exception& e) {cerr << "Exception: " << e.what() << endl;}
          }
        }
        if (!haveRecord) break; // Finish the loop if we are done
      }
      if (beginningNewCluster) {
        last_cluster = cluster;
        last_func_id = function_id;
        last_index_within_function = index_within_function;
        rows_in_this_cluster.clear();
      }
      bool keep = beginningNewCluster || (index_within_function >= last_index_within_function + numStridesThatMustBeDifferent);
      if (keep) {
        last_index_within_function = index_within_function;
        rows_in_this_cluster.push_back(cluster_row_number);
      }
    }
  }
  cerr << "... done" << endl;

}


static void executeLSHCode(sqlite3_connection& con, const std::string& databaseName, const string& Exec, const string& paramFileName, const Range& range) {
  // int inputToLsh[2], outputFromLsh[2];
  // pipe(inputToLsh);
  // pipe(outputFromLsh);
  pid_t p = fork();
  if (p == -1) { // Error
	perror("fork: ");
	exit (1);
  } if (p == 0) { // Child
	// close(inputToLsh[1]);
	// close(outputFromLsh[0]);
	// dup2(inputToLsh[0], 0);
	// dup2(outputFromLsh[1], 1);
	vector<char*> args;
	args.push_back(strdup(Exec.c_str()));
	args.push_back("-g");
	args.push_back(strdup(boost::lexical_cast<string>(range.low).c_str()));
	args.push_back("-G");
	args.push_back(strdup(boost::lexical_cast<string>(range.high).c_str()));
	args.push_back("-R");
	args.push_back("0.7");
	args.push_back("-M");
	args.push_back("12000000000");
	args.push_back("-b");
	args.push_back("2");
	args.push_back("-f");
	args.push_back(strdup(databaseName.c_str()));
	args.push_back("-p");
	args.push_back(strdup(paramFileName.c_str()));
	args.push_back(0);

        ostringstream outStr; 
        for(vector<char*>::iterator iItr = args.begin(); iItr != args.end();
            ++iItr )
        {
          outStr << *iItr << " ";
        }     
          ;
          std::cout << "Calling " << outStr.str() << std::endl;

        
	execv(Exec.c_str(), &args[0]);
	perror("execv: ");
	exit (1);
  } else { // Parent
#if 0
	close(inputToLsh[0]);
	close(outputFromLsh[1]);
	FILE* writeToLsh = fdopen(inputToLsh[1], "w");
	if (!writeToLsh) {
	  perror("fdopen: ");
	  exit (1);
	}
	cerr << "About to write group file" << endl;
	writeOneGroupFile(con, writeToLsh, range);
	cerr << "Wrote group file" << endl;
	fclose(writeToLsh);
	cerr << "Closed group file" << endl;
	FILE* readFromLsh = fdopen(outputFromLsh[0], "r");
	if (!readFromLsh) {
	  perror("fdopen: ");
	  exit (1);
	}
	cerr << "About to read results" << endl;
	readOneResultFile(con, readFromLsh);
	cerr << "Done reading results" << endl;
	fclose(readFromLsh);
	cerr << "Done closing results" << endl;
#endif
        int status;
	if (waitpid(p, &status, 0) == -1) {
          perror("waitpid");
          abort();
        }
        cerr << "Status: " << status << endl;
	cerr << "Done waiting for LSH" << endl;
  }
}

static void executeLSHCodeLLNL(sqlite3_connection& con, const std::string& databaseName, const string& Exec, const string& paramFileName, const Range& range, int norm, double similarity, double false_negative_rate, int num_vectors) {
  pid_t p = fork();

  if (p == -1) { // Error
    perror("fork: ");
    exit (1);
  } if (p == 0) { // Child


    size_t hashTableElementsPerBucket = 20;
    double r = 4.;

    
    size_t num_elem_per_bucket=20;


    parameters params = selectParameters(false_negative_rate, similarity, range.low, num_elem_per_bucket, 
        num_vectors,norm);



    
    
    vector<char*> args;
    args.push_back(strdup(Exec.c_str()));
    args.push_back("-g");
    args.push_back(strdup(boost::lexical_cast<string>(range.low).c_str()));
    args.push_back("-G");
    args.push_back(strdup(boost::lexical_cast<string>(range.high).c_str()));
    args.push_back("-t");
    args.push_back(strdup(boost::lexical_cast<string>(similarity).c_str()));
    args.push_back("-b");
    args.push_back(strdup(boost::lexical_cast<string>( params.num_buckets ).c_str()) );
    args.push_back("-s");
    args.push_back(strdup(boost::lexical_cast<string>(params.num_elem_per_bucket).c_str()));
    args.push_back("--database");
    args.push_back(strdup(databaseName.c_str()));
    args.push_back("--nodelete");
    args.push_back("-k");
    args.push_back(strdup(boost::lexical_cast<string>(params.k).c_str()));
    args.push_back("-l");
    args.push_back(strdup(boost::lexical_cast<string>(params.l).c_str()));
    args.push_back("-p");
    args.push_back(strdup(boost::lexical_cast<string>(norm).c_str()));

    args.push_back(0);

    ostringstream outStr; 
    for(vector<char*>::iterator iItr = args.begin(); iItr != args.end();
        ++iItr )
    {
      outStr << *iItr << " ";
    }     
    ;
    std::cout << "Calling " << outStr.str() << std::endl;


    execv(Exec.c_str(), &args[0]);

    perror("execv: ");
    exit (1);
  } else { // Parent
#if 0
    close(inputToLsh[0]);
    close(outputFromLsh[1]);
    FILE* writeToLsh = fdopen(inputToLsh[1], "w");
    if (!writeToLsh) {
      perror("fdopen: ");
      exit (1);
    }
    cerr << "About to write group file" << endl;
    writeOneGroupFile(con, writeToLsh, range);
    cerr << "Wrote group file" << endl;
    fclose(writeToLsh);
    cerr << "Closed group file" << endl;
    FILE* readFromLsh = fdopen(outputFromLsh[0], "r");
    if (!readFromLsh) {
      perror("fdopen: ");
      exit (1);
    }
    cerr << "About to read results" << endl;
    readOneResultFile(con, readFromLsh);
    cerr << "Done reading results" << endl;
    fclose(readFromLsh);
    cerr << "Done closing results" << endl;
#endif
    int status;
    if (waitpid(p, &status, 0) == -1) {
      perror("waitpid");
      abort();
    }
    cerr << "Status: " << status << endl;
    cerr << "Done waiting for LSH" << endl;
  }

  
}





inline double tvToDouble(const timeval& tv) {
  return tv.tv_sec + tv.tv_usec * 1.e-6;
}

void insert_timing(sqlite3_connection& con, 
	std::string property_name,
	const timeval& before, const timeval& after,
	const rusage& ru_before, const rusage& ru_after)
{
  try {
	string timing_insert = "INSERT INTO timing(property_name, total_wallclock, total_usertime, total_systime, wallclock, usertime, systime) VALUES(?,?,?,?,?,?,?)";
	sqlite3_command cmd(con, timing_insert.c_str());
	cmd.bind(1, property_name);
	cmd.bind(2, 0);
	cmd.bind(3, tvToDouble(ru_after.ru_utime));
	cmd.bind(4, tvToDouble(ru_after.ru_stime));
	cmd.bind(5, (tvToDouble(after) - tvToDouble(before)));
	cmd.bind(6, (tvToDouble(ru_after.ru_utime) - tvToDouble(ru_before.ru_utime)));
	cmd.bind(7, (tvToDouble(ru_after.ru_stime) - tvToDouble(ru_before.ru_stime)));
	cmd.executenonquery();
  } catch (exception& ex) {
	cerr << "Exception on timing write: " << ex.what() << endl;
  }


}


void 
OperateOnClusters::analyzeClusters()
{

  try{
    con.executenonquery("delete from clusters" );
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }

  cerr << "About to delete from postprocessed_clusters" << endl;
  try{
	  con.executenonquery("delete from postprocessed_clusters");
  }
  catch(exception &ex) {
	  cerr << "Exception Occurred: " << ex.what() << endl;
  }


  
  string Exec;
  if(norm == 3)
     Exec = ABS_TOP_BUILDDIR "/lsh2/lsh2";
  else
     Exec = ABS_TOP_BUILDDIR "/lshCloneDetection";

  std::cout << "Running LSH " << Exec << std::endl;

  struct timeval before, after;
  struct rusage ru_before, ru_after;
  struct rusage ru_before_children, ru_after_children;

  {
	gettimeofday(&before, NULL);
	getrusage(RUSAGE_SELF, &ru_before);
	getrusage(RUSAGE_CHILDREN, &ru_before_children);

	{
          std::cout << "Similarity is " << similarity << std::endl;
          if( similarity == 1.0)
          {
            Exec = ABS_TOP_BUILDDIR "/exactCloneDetection";
            callExact(con, databaseName,Exec);
            
          }else
            callLSH(con, databaseName, similarity, Exec,norm, k,l ); // FIXME: update similarity value
	}

	gettimeofday(&after, NULL);
	getrusage(RUSAGE_SELF, &ru_after);
	getrusage(RUSAGE_CHILDREN, &ru_after_children);

	insert_timing(con, "LSH-current",before,after, ru_before, ru_after );
	insert_timing(con, "LSH-children",before,after, ru_before_children, ru_after_children );
	std::cout << "End running LSH" << std::endl;
  } 

  {
	gettimeofday(&before, NULL);
	getrusage(RUSAGE_SELF, &ru_before);

        if(norm == 3)
	{
	  postprocess(con);
	}
	
	gettimeofday(&after, NULL);
	getrusage(RUSAGE_SELF, &ru_after);
	insert_timing(con,"post-processing",before,after, ru_before, ru_after );
  }

  try{
	string query = "SELECT count(distinct cluster) from clusters";

	sqlite3_command cmd2(con, query /* selectQuery.str() */ );

	sqlite3_reader entriesForFile =cmd2.executereader();

        int num_clusters =0;
	if(entriesForFile.read())
	{
	  num_clusters = entriesForFile.getint(0);
	}
        std::cerr << "------------------------------------------------" << std::endl;

        std::cerr << "Found " << num_clusters << " clusters " << std::endl;

        
  } catch(exception &ex) {
	cerr << "Exception Occurred: " << ex.what() << endl;
  }
 


}

static 
void insert_into_clusters(
    sqlite3_connection& con,
    int cluster,  
    int function_id, 
    int index_within_function,
    int vectors_row, 
    int dist 
    ) {
  std::string db_select_n = "INSERT INTO clusters(cluster, function_id, index_within_function, vectors_row, dist) VALUES(?,?,?,?,?)";
  try{
    //sqlite3_transaction trans2(con);
    {
      sqlite3_command cmd(con, db_select_n.c_str());
      cmd.bind(1,cluster);
      cmd.bind(2,function_id);
      cmd.bind(3,index_within_function);
      cmd.bind(4,vectors_row);
      cmd.bind(5,dist);
      cmd.executenonquery();
    }
    //trans2.commit();
  }
  catch(exception &ex) {
    cerr << "Exception Occurred: " << ex.what() << endl;
  }
}

OperateOnClusters::OperateOnClusters(const string& database, int p, double s, size_t hash_function_size, size_t hash_table_count) 
  : databaseName(database), norm(p), similarity(s), k(hash_function_size), l(hash_table_count)
{
   con.open( database.c_str());
};


