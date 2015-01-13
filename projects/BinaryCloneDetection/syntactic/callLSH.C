#include "rose.h"

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
namespace fs = boost::filesystem;


static void executeLSHCode(const SqlDatabase::TransactionPtr&, const string& databaseName, const string& Exec,
                           const string& paramFileName, const CloneRange& range);
static void executeLSHCodeLLNL(const SqlDatabase::TransactionPtr&, const std::string& databaseName, const string& Exec,
                               const string& paramFileName, const CloneRange& range, int norm, double similarity,
                               double false_negative_rate, int num_vectors);

#include <vector>
#include <math.h>
#include <iostream>
#include <stdlib.h>

double
getL(int k, double similarity_threshold, double false_negative_rate, double groupLow)
{
    double distance = similarity_threshold==1.  ? 1. : sqrt(2*groupLow*(1.-similarity_threshold));
    return ceil(log(false_negative_rate)/log(1-pow(1-distance/8000,k)));
}

double
pfL2(double c, double r)
{
    double Pi = 4.0*atan(1.0);
    return  -erf(-r/(c*sqrt(2)))-2*c*(1-exp(- (pow(r,2)*0.5/pow(c,2) )))/(sqrt(2*Pi)*r) ;
}

double
getL2(double k, double r, double similarity_threshold, double false_negative_rate)
{
    return ceil(log(false_negative_rate)/log(1- pow(pfL2(1.,r),k)));
}

double
computeQualityL2(double k, double r, double similarity_threshold, double false_negative_rate, double groupLow)
{
    double distance = similarity_threshold==1.  ? 1. : sqrt(2*groupLow*(1.-similarity_threshold));
    double quality=0;
    for (int c = 0; c <= 100; c++) { // X is the distance
        double y = 1-pow((1- pow(pfL2(c/35,r),k)),getL2(k,r,similarity_threshold,false_negative_rate));
        quality+=fabs(distance <= c ? 1-y : -y);
    }
    return quality;
}

double
computeQuality(double k, double l, double similarity_threshold, double groupLow)
{
    double distance = similarity_threshold==1.  ? 1. : sqrt(2*groupLow*(1.-similarity_threshold));
    double quality=0;
    for (int x =1; x <= 101; x++) { // X is the distance
        double y = 1-pow(1-pow(1-(double)x/8000,k),l);
        quality+=fabs(distance <= x ? 1-y : -y);
    }
    return quality;
}

size_t
computeSizeOfMemoryUsage(int num_buckets, int num_elem_per_bucket, int num_vectors, int l)
{
    int estimatedCompressedVecSize = 120;
  
    //Contribtions from hashtables
    size_t hashtableContrib = num_buckets*l*( num_elem_per_bucket*sizeof(size_t)
                                              + sizeof(size_t));
    //Contribution from storing the vectors
    size_t vectorContrib = (estimatedCompressedVecSize+sizeof(VectorEntry) )*num_vectors;

    return vectorContrib+hashtableContrib;
}

struct parameters {
    size_t k;
    size_t l;
    size_t num_buckets;
    size_t num_elem_per_bucket;

    parameters(size_t kVal, size_t lVal, size_t num_bucketsVal, size_t num_elem_per_bucketVal)
        : k(kVal), l(lVal), num_buckets(num_bucketsVal), num_elem_per_bucket(num_elem_per_bucketVal)
        {};
};

parameters
selectParameters(double false_negative_rate, double similarity_threshold, int groupLow, int num_elem_per_bucket,
                 int num_vectors,  int norm, size_t available_memory=1600000000)
{
    std::cout << "Norm is: " << norm << std::endl;
    if (norm != 1 && norm != 2)
        exit(1);
    //Calculate how many buckets are needed
    size_t num_buckets=(size_t)ceil(num_vectors*2./num_elem_per_bucket);
    if (num_buckets<=2)
        num_buckets=3;
   
    double r = 4.;
    std::vector<size_t> kVals;
    std::vector<size_t> lVals;
    std::vector<double> qualityVals;
    double bestQuality=0;
    for (int k = 1 ; k <= ((norm==1) ? 1000 : 30 ); k += ((norm ==1) ? 50 : 1)) {
        double l = norm ==1
                   ? getL(k, similarity_threshold, false_negative_rate, groupLow)
                   : getL2(k, r, similarity_threshold, false_negative_rate);
        double quality = norm == 1
                         ? computeQuality(k,l,similarity_threshold, groupLow)
                         : computeQualityL2(k,r,similarity_threshold,false_negative_rate,groupLow) ;
        if (quality > bestQuality)
            bestQuality = quality;
        kVals.push_back(k);
        lVals.push_back((int)l) ;
        qualityVals.push_back(quality);
        std::cout << "k " << k << " l " << l <<  " Quality " << quality << std::endl;
    }

    double allowedDistanceFromTopQuality = 0.1;
    int bestIndex=0;
    for (size_t i =0; i < qualityVals.size() ; i++) {
        if (qualityVals[i]>=(1.0-allowedDistanceFromTopQuality)*bestQuality) {
            bestIndex=i;
            break;
        }
        if (computeSizeOfMemoryUsage(num_buckets, num_elem_per_bucket, num_vectors, lVals[i]) > available_memory) {
            bestIndex = i-1;
            break;
        }
    }
    return parameters(kVals[bestIndex],lVals[bestIndex],num_buckets, num_elem_per_bucket);
}

static void 
callExact(const SqlDatabase::TransactionPtr &tx, const std::string databaseName, const string& Exec)
{
    // FIXME: We can't pass parameters to the exec'd process this way because the parent's SQL statements are
    // being executed in a transaction -- they won't be visible in the child. [Robb P. Matzke 2013-08-12]
    tx->execute("delete from detection_parameters");
    tx->statement("insert into detection_parameters (similarity_threshold, false_negative_rate) values (?, ?)")
        ->bind(0, 1.0)
        ->bind(1, 0)
        ->execute();

    std::cout << "Start running exact clone detection" << std::endl;
    pid_t p = fork();
    if (p == -1) { // Error
        perror("fork: ");
        exit (1);
    }
    if (p == 0) { // Child
        vector<char*> args;
        args.push_back(strdup(Exec.c_str()));
        args.push_back(strdup("--database"));
        args.push_back(strdup(databaseName.c_str()));
        args.push_back(0);

        ostringstream outStr; 
        for (vector<char*>::iterator iItr = args.begin(); iItr != args.end(); ++iItr)
            outStr << *iItr << " ";
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
}

static void
callLSH(const SqlDatabase::TransactionPtr &tx, const std::string databaseName, double similarity_threshold, const string& Exec,
        int norm, size_t hash_function_size, size_t hash_table_count)
{
    double distance = sqrt((1. - similarity_threshold) * 50.);
    double false_negative_rate = ( similarity_threshold != 1.0) ? 0.0100 : 0;
    vector<CloneRange> ranges = computeranges(distance, 50, 100000);
    int maxNumElementsInGroup = -1;
    int maxNumElementIdx = -1;

    // FIXME: We can't pass parameters to the exec'd process this way because the parent's SQL statements are
    // being executed in a transaction -- they won't be visible in the child. [Robb P. Matzke 2013-08-12]
    tx->execute("delete from detection_parameters");
    tx->statement("insert into detection_parameters (similarity_threshold, false_negative_rate) values (?, ?)")
        ->bind(0, similarity_threshold)
        ->bind(1, false_negative_rate)
        ->execute();
  
    map<size_t, int> groupSizes;
    std::cout << "Looking for the biggest group" << std::endl;
    for (size_t i = 0; i < ranges.size(); ++i) {
        std::string sql = std::string("select count(*) from vectors where sum_of_counts >= ?") +
                          (ranges[i].high != -1 ? " and sum_of_counts <= ?" : "");
        SqlDatabase::StatementPtr cmd = tx->statement(sql);
        cmd->bind(0, ranges[i].low);
        if (ranges[i].high != -1)
            cmd->bind(1, ranges[i].high);
        int numElementsInGroup = cmd->execute_int();
        groupSizes[i] = numElementsInGroup;
        std::cerr << "The current group from " << ranges[i].low << " to " << ranges[i].high
                  << " is of size " << numElementsInGroup << std::endl;
        if (numElementsInGroup > maxNumElementsInGroup) {
            maxNumElementsInGroup = numElementsInGroup;
            maxNumElementIdx = i;
        }
    }

    std::cout << "Biggest group found " << ranges[maxNumElementIdx].low << " " << ranges[maxNumElementIdx].high << std::endl;
    char tempDirName[] = "/tmp/paramdirXXXXXX";
    char* mkdtempResult = mkdtemp(tempDirName);
    if (!mkdtempResult) {
	perror("mkdtemp: ");
	exit (1);
    }
    string paramFileName = string(tempDirName) + "/params";
    paramFileName = "/tmp/lshparamdirE40hF1/params";
    std::cout << "Number of groups :" << ranges.size() << std::endl;

    for (int i = 0; i < (int)ranges.size(); ++i) {
        size_t group = (i == 0) ? maxNumElementIdx : (i <= maxNumElementIdx) ? i - 1 : i;
        if (groupSizes[group] > 1) {
            std::cout << "Executing LSH code low " << ranges[group].low  
                      << " high " << ranges[group].high << " group  " << group << " size " << groupSizes[group] << std::endl;
            if(norm == 3) {
                executeLSHCode(tx, databaseName, Exec, paramFileName, ranges[group]);
            } else {
                executeLSHCodeLLNL(tx, databaseName, Exec, paramFileName, ranges[group], norm, similarity_threshold,
                                   false_negative_rate, groupSizes[group]);
            }
        }
    }
    unlink(paramFileName.c_str());
    rmdir(tempDirName);
}

static void
postprocess(const SqlDatabase::TransactionPtr &tx)
{
    int windowSize = tx->statement("select window_size from run_parameters limit 1")->execute_int();
    int stride = tx->statement("select stride from run_parameters limit 1")->execute_int();
    assert(windowSize != 0);
    assert(stride != 0);

    cerr << "About to delete from postprocessed_clusters" << endl;
    tx->execute("delete from postprocessed_clusters");
    cerr << "... done" << endl;

    cerr << "About to postprocess" << endl;
    SqlDatabase::StatementPtr cmd = tx->statement("select cluster, function_id, index_within_function, vectors_row"
                                                  " from clusters order by cluster, function_id, index_within_function");
    SqlDatabase::StatementPtr insertCmd = tx->statement("insert into postprocessed_clusters"
                                                        " select * from clusters where row_number = ?");
    const size_t numStridesThatMustBeDifferent = windowSize / (stride * 2);
    string last_cluster = "";
    string last_func_id = "";
    size_t last_index_within_function = 0;
    vector<string> rows_in_this_cluster;
    bool first = true;
    for (SqlDatabase::Statement::iterator postproc_reader=cmd->begin(); postproc_reader!=cmd->end(); ++postproc_reader) {
        string cluster = postproc_reader.get<std::string>(0);
        string function_id = postproc_reader.get<std::string>(1);
        size_t index_within_function = postproc_reader.get<size_t>(2);
        string cluster_row_number = postproc_reader.get<std::string>(3);
        bool differentFunction = cluster != last_cluster || function_id != last_func_id;
        bool endingCluster = differentFunction;
        bool beginningNewCluster = first || differentFunction;
        first = false;
        if (endingCluster) {
            if (rows_in_this_cluster.size() > 1) { // Skip clusters that have only one element left
                for (size_t i = 0; i < rows_in_this_cluster.size(); ++i) {
                    insertCmd->bind(0, rows_in_this_cluster[i]);
                    insertCmd->execute();
                }
            }
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
    cerr << "... done" << endl;
}

static void
executeLSHCode(const SqlDatabase::TransactionPtr &tx, const std::string& databaseName, const string& Exec,
               const string& paramFileName, const CloneRange& range)
{
    pid_t p = fork();
    if (p == -1) { // Error
	perror("fork: ");
	exit (1);
    }
    if (p == 0) { // Child
	vector<char*> args;
        // FIXME: memory leaks [Robb P. Matzke 2013-08-12]
	args.push_back(strdup(Exec.c_str()));
	args.push_back(strdup("-g"));
	args.push_back(strdup(boost::lexical_cast<string>(range.low).c_str()));
	args.push_back(strdup("-G"));
	args.push_back(strdup(boost::lexical_cast<string>(range.high).c_str()));
	args.push_back(strdup("-R"));
	args.push_back(strdup("0.7"));
	args.push_back(strdup("-M"));
	args.push_back(strdup("12000000000"));
	args.push_back(strdup("-b"));
	args.push_back(strdup("2"));
	args.push_back(strdup("-f"));
	args.push_back(strdup(databaseName.c_str()));
	args.push_back(strdup("-p"));
	args.push_back(strdup(paramFileName.c_str()));
	args.push_back(0);

        ostringstream outStr; 
        for(vector<char*>::iterator iItr = args.begin(); iItr != args.end(); ++iItr)
            outStr << *iItr << " ";
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
	cerr << "Done waiting for LSH" << endl;
    }
}

static void
executeLSHCodeLLNL(const SqlDatabase::TransactionPtr &tx, const std::string& databaseName, const string& Exec,
                   const string& paramFileName, const CloneRange& range, int norm, double similarity,
                   double false_negative_rate, int num_vectors)
{
    pid_t p = fork();
    if (p == -1) { // Error
        perror("fork: ");
        exit (1);
    }
    if (p == 0) { // Child
        size_t num_elem_per_bucket=20;
        parameters params = selectParameters(false_negative_rate, similarity, range.low, num_elem_per_bucket, 
                                             num_vectors,norm);

        vector<char*> args;
        // FIXME: memory leaks [Robb P. Matzke 2013-08-12]
        args.push_back(strdup(Exec.c_str()));
        args.push_back(strdup("-g"));
        args.push_back(strdup(boost::lexical_cast<string>(range.low).c_str()));
        args.push_back(strdup("-G"));
        args.push_back(strdup(boost::lexical_cast<string>(range.high).c_str()));
        args.push_back(strdup("-t"));
        args.push_back(strdup(boost::lexical_cast<string>(similarity).c_str()));
        args.push_back(strdup("-b"));
        args.push_back(strdup(boost::lexical_cast<string>( params.num_buckets ).c_str()) );
        args.push_back(strdup("-s"));
        args.push_back(strdup(boost::lexical_cast<string>(params.num_elem_per_bucket).c_str()));
        args.push_back(strdup("--database"));
        args.push_back(strdup(databaseName.c_str()));
        args.push_back(strdup("--nodelete"));
        args.push_back(strdup("-k"));
        args.push_back(strdup(boost::lexical_cast<string>(params.k).c_str()));
        args.push_back(strdup("-l"));
        args.push_back(strdup(boost::lexical_cast<string>(params.l).c_str()));
        args.push_back(strdup("-p"));
        args.push_back(strdup(boost::lexical_cast<string>(norm).c_str()));

        args.push_back(0);

        ostringstream outStr; 
        for (vector<char*>::iterator iItr = args.begin(); iItr != args.end(); ++iItr)
            outStr << *iItr << " ";
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
        cerr << "Done waiting for LSH" << endl;
    }
}

inline double
tvToDouble(const timeval& tv)
{
    return tv.tv_sec + tv.tv_usec * 1.e-6;
}

void
insert_timing(const SqlDatabase::TransactionPtr &tx, std::string property_name, const timeval& before, const timeval& after,
              const rusage& ru_before, const rusage& ru_after)
{
    SqlDatabase::StatementPtr cmd = tx->statement("insert into timing"
                                                  // 0              1                2               3              4
                                                  " (property_name, total_wallclock, total_usertime, total_systime, wallclock,"
                                                  // 5        6
                                                  " usertime, systime)"
                                                  " values (?,?,?,?,?,?,?)");
    cmd->bind(0, property_name);
    cmd->bind(1, 0);
    cmd->bind(2, tvToDouble(ru_after.ru_utime));
    cmd->bind(3, tvToDouble(ru_after.ru_stime));
    cmd->bind(4, (tvToDouble(after) - tvToDouble(before)));
    cmd->bind(5, (tvToDouble(ru_after.ru_utime) - tvToDouble(ru_before.ru_utime)));
    cmd->bind(6, (tvToDouble(ru_after.ru_stime) - tvToDouble(ru_before.ru_stime)));
    cmd->execute();
}

void 
OperateOnClusters::analyzeClusters()
{
    // FIXME: child process won't see changes since we're executing in a transaction [Robb P. Matzke 2013-08-12]
    tx->execute("delete from clusters" );
    cerr << "About to delete from postprocessed_clusters" << endl;
    tx->execute("delete from postprocessed_clusters");

    string Exec;
    if (norm == 3){
        Exec = ABS_BUILDDIR "/lsh2/lsh2";
    } else {
        Exec = ABS_BUILDDIR "/lshCloneDetection";
    }
    std::cout << "Running LSH " << Exec << std::endl;

    struct timeval before, after;
    struct rusage ru_before, ru_after;
    struct rusage ru_before_children, ru_after_children;

    {
	gettimeofday(&before, NULL);
	getrusage(RUSAGE_SELF, &ru_before);
	getrusage(RUSAGE_CHILDREN, &ru_before_children);

        std::cout << "Similarity is " << similarity << std::endl;
        if (similarity == 1.0) {
            Exec = ABS_BUILDDIR "/exactCloneDetection";
            callExact(tx, databaseName,Exec);
        } else {
            callLSH(tx, databaseName, similarity, Exec,norm, k,l); // FIXME: update similarity value
        }

	gettimeofday(&after, NULL);
	getrusage(RUSAGE_SELF, &ru_after);
	getrusage(RUSAGE_CHILDREN, &ru_after_children);

	insert_timing(tx, "LSH-current", before,after, ru_before, ru_after);
	insert_timing(tx, "LSH-children", before,after, ru_before_children, ru_after_children);
	std::cout << "End running LSH" << std::endl;
    } 

    {
	gettimeofday(&before, NULL);
	getrusage(RUSAGE_SELF, &ru_before);
        if (norm == 3)
            postprocess(tx);
	gettimeofday(&after, NULL);
	getrusage(RUSAGE_SELF, &ru_after);
	insert_timing(tx, "post-processing", before, after, ru_before, ru_after);
    }

    SqlDatabase::StatementPtr cmd2 = tx->statement("select count(distinct cluster) from clusters");
    SqlDatabase::Statement::iterator entriesForFile = cmd2->begin();
    int num_clusters =0;
    if (entriesForFile!=cmd2->end())
        num_clusters = entriesForFile.get<int>(0);
    std::cerr << "------------------------------------------------" << std::endl;
    std::cerr << "Found " << num_clusters << " clusters " << std::endl;
}

OperateOnClusters::OperateOnClusters(const string& database, int p, double s, size_t hash_function_size, size_t hash_table_count) 
    : databaseName(database), norm(p), similarity(s), k(hash_function_size), l(hash_table_count) {
    tx = SqlDatabase::Connection::create(database)->transaction();
};


