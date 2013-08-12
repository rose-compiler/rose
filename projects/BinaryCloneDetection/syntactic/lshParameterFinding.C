#include "lshParameterFinding.h"


int
main(int argc, char* argv[])
{
    std::string database;
    size_t l = 1, k = 6;
    size_t total_number_of_elements_to_get = 5000;
    size_t hashTableNumBuckets = 13000000, hashTableElementsPerBucket = 20;
    double distBound = 1.;
    double r = 4.;
    int norm = 2;
    double similarity_threshold=1.;

    size_t low=-1;
    size_t high=-1;
 
    try {
        options_description desc("Allowed options");
        desc.add_options()
            ("help", "Produce a help message")
            ("nodelete", "Do not delete previous entries in vectors")
            ("database", value< std::string >(&database), "The sqlite database that we are to use")
            ("hash-function-size,k", value< size_t >(&k), "The number of elements in a single hash function")
            ("hash-table-count,l", value< size_t >(&l), "The number of separate hash tables to create")
            ("buckets,b", value< size_t >(&hashTableNumBuckets),
             "The number of buckets in each hash table (buckets may store multiple elements)")
            ("groupLow,g", value< size_t >(&low), "The lowest count of elements")
            ("groupHigh,G", value< size_t >(&high), "The highest count of elements")
            ("similarity,t", value< double >(&similarity_threshold), "The similarity threshold that is allowed in a clone pair")
            ("distance,d", value< double >(&distBound), "The maximum distance that is allowed in a clone pair")
            ("interval-size,r", value< double >(&r), "The divisor for the l_2 hash function family")
            ("norm,p", value< int >(&norm), "Exponent in p-norm to use (1 or 2)")
            ("number-of-samples,s", value< size_t >(&total_number_of_elements_to_get),
             "The number of vectors to use when sampling for hash functions");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        distBound = similarity_threshold==1.  ? 1. : sqrt(2*low*(1.-similarity_threshold));
        std::cerr << "similarity " << similarity_threshold << " distBound " << distBound << std::endl;

        if (vm.count("help")) {
            std::cerr << desc << std::endl;
            exit(0);
        }

        if (database == "") {
            std::cerr << "Missing options. Call as: " << argv[0] << " --database <database-name> [other parameters]" 
                      << std::endl;
            exit(1);
        }

        if (hashTableNumBuckets >= (1ULL << 32)) {
            std::cerr << "Number of buckets must be less than 2**32" << std::endl;
            exit (1);
        }

        if (norm != 1 && norm != 2) {
            std::cerr << "Norm must be either 1 or 2" << std::endl;
            exit (1);
        }

        std::cerr << "norm: l_" << norm << std::endl;
        std::cerr << "database: " << database << std::endl;
        std::cerr << "k: " << k << std::endl;
        std::cerr << "l: " << l << std::endl;
        std::cerr << "buckets: " << hashTableNumBuckets << std::endl;
        std::cerr << "bucket size: " << hashTableElementsPerBucket << std::endl;
        std::cerr << "distance: " << distBound << std::endl;
        std::cerr << "r: " << r << std::endl;
        std::cerr << "number-of-vectors:" << total_number_of_elements_to_get << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        exit (1);
    }

    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(database)->transaction();
    runLSH(tx, database, l, k, total_number_of_elements_to_get,
           hashTableNumBuckets, hashTableElementsPerBucket, distBound, r,
           norm, similarity_threshold, low, high);

    //Finding groups
    std::vector<CloneRange> ranges = computeranges(distBound, 50, 100000);

    std::vector<int> rangeSize;
    int total_number_of_elements_in_groups=0;
    int maxNumElementsInGroup = -1;
    int maxNumElementIdx = -1;
    for (size_t i = 0; i < ranges.size(); ++i) {
        std::string sql = std::string("select count(*) from vectors where sum_of_counts >= ?") +
                          (ranges[i].high != -1 ? "and sum_of_counts <= ?" : "");
        SqlDatabase::StatementPtr cmd = tx->statement(sql);
        cmd->bind(0, ranges[i].low);
        if (ranges[i].high != -1)
            cmd->bind(1, ranges[i].high);
        int numElementsInGroup = cmd->execute_int();
        total_number_of_elements_in_groups += numElementsInGroup;
        rangeSize.push_back(numElementsInGroup);

        if (numElementsInGroup > maxNumElementsInGroup) {
            maxNumElementsInGroup = numElementsInGroup;
            maxNumElementIdx = i;
        }
    }

    tx->commit();
    return 0;
}
