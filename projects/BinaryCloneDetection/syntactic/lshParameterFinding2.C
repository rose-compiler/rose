#include "lshParameterFinding.h"

float
computeFunctionP(float w, float c)
{
    if ( c < 10e-6 ) {
        return 1.;
    } else {
        float x = w / c;
        return (1 - erfc(x / 1.41421356237309504880) - 1.12837916709551257390 / 1.41421356237309504880 / x *
                (1 - expf(-((x) * (x)) / 2)));
    }
}

long int
computeMForULSH(long int k, float successProbability)
{
    if (!((k & 1) == 0)) {
        fprintf(stderr, "ASSERT failed on line %d, file %s.\n", 333, "SelfTuning.cpp");
        exit(1);
    };
    float mu = 1 - (powf(computeFunctionP(4.0, 1), k / 2));
    float P = successProbability;
    float d = (1-mu)/(1-P)*1/logf(1/mu) * (powf(mu, -1/(1-mu)));
    float y = logf(d);
    long int m = ((long int)(ceilf(1 - y/logf(mu) - 1/(1-mu))));
    while ((powf(mu, m-1)) * (1 + m * (1-mu)) > 1 - P) {
        m++;
    }
    return m;
}

static void
read_n_elements_from_vectors(const SqlDatabase::TransactionPtr &tx, scoped_array_with_size<VectorEntry>& vectors,
                             int low, int high, int number_of_elements_to_get, int& counter)
{
    size_t eltCount = 0;

    std::string sql1 = std::string("select count(*) from vectors where sum_of_counts >= ?") +
                       (high != -1 ? " and sum_of_counts <= ?" : "") +
                       " order by random()";
    SqlDatabase::StatementPtr cmd1 = tx->statement(sql1);
    cmd1->bind(0, low);
    if (high != -1)
        cmd1->bind(1, high);
    eltCount = cmd1->execute_int();

    if (eltCount == 0) {
        std::cerr << "No vectors found -- invalid database?" << std::endl;
        exit (1);
    }
    if(eltCount<5000)
        number_of_elements_to_get = eltCount;
    std::cerr << "Found " << eltCount << " vectors " << number_of_elements_to_get << std::endl;

    vectors.allocate(number_of_elements_to_get);

    //                                     0            1                      2     3       4           5
    std::string sql2 = std::string("select function_id, index_within_function, line, offset, counts_b64, instr_seq_b64") +
                       " from vectors where sum_of_counts >= ?" +
                       (high != -1 ? " and sum_of_counts <= ?" : "") +
                       " order by random() limit ?";
    SqlDatabase::StatementPtr cmd2 = tx->statement(sql2);
    cmd2->bind(0, low);
    if (high != -1){
        cmd2->bind(1, high);
        cmd2->bind(2, number_of_elements_to_get);
    } else {
        cmd2->bind(1, number_of_elements_to_get);
    }

    for (SqlDatabase::Statement::iterator r=cmd2->begin(); r!=cmd2->end(); ++r) {
        int functionId = r.get<int>(0);
        int indexWithinFunction = r.get<int>(1);
        long long line = r.get<int64_t>(2);
        int offset = r.get<int>(3);
        std::vector<uint8_t> counts = StringUtility::decode_base64(r.get<std::string>(4));
        std::string compressedCounts(&counts[0], &counts[0]+counts.size());
        std::vector<uint8_t> md5 = StringUtility::decode_base64(r.get<std::string>(5));
        std::string instrSeqMD5(&md5[0], &md5[0]+md5.size());
        VectorEntry& ve = vectors[counter];
        vectors[counter].functionId = functionId;
        vectors[counter].indexWithinFunction = indexWithinFunction;
        vectors[counter].line = line;
        vectors[counter].offset = offset;
        vectors[counter].compressedCounts.allocate(compressedCounts.size());
        memcpy(vectors[counter].compressedCounts.get(), compressedCounts.data(), compressedCounts.size());
        if(compressedCounts.size() == 0)
            std::cout << "Compressed counts is 0 " << std::endl;
        if(vectors[counter].compressedCounts.size() == 0)
            std::cout << "Compressed counts is 0 " << std::endl;

        if (instrSeqMD5.size() != 16) {
            std::cerr << "Found MD5 with length other than 16" << std::endl;
            abort();
        }
        memcpy(ve.instrSeqMD5, instrSeqMD5.data(), 16);

        counter++;
    }

    for (size_t i =0; i < vectors.size(); i++) {
        if(vectors[i].compressedCounts.size() == 0)
            std::cout << "1:It is empty " << std::endl;
    }
}

double
computeSimilarity(const boost::scoped_array<size_t>& elem1, const boost::scoped_array<size_t>& elem2, int size)
{
    int equal=0;
    for (int i = 0 ; i < size ; i++) {
        if (elem1[i] == elem2[i])
            equal++;
    }
    return equal/(double)size;
}

double
computeSimilarity(const boost::scoped_array<uint16_t>& elem1, const boost::scoped_array<uint16_t>& elem2, int size)
{
    int equal=0;
    for (int i = 0 ; i < size ; i++) {
        if (elem1[i] == elem2[i])
            equal++;
    }
    return equal/(double)size;
}

double
computeSimilarityDisplay(const boost::scoped_array<size_t>& elem1, const boost::scoped_array<size_t>& elem2, int size)
{
    int equal=0;
    std::cout << "The size " << size << " " ;
    for (int i = 0 ; i < size ; i++) {
        if (elem1[i] == elem2[i])
            equal++;
        std::cout << i << ":" << elem1[i] <<  ":" << elem2[i] << " ";
    }
    std::cout << std::endl;
    return equal/(double)size;
}

std::pair<size_t, size_t>
runLSH(const SqlDatabase::TransactionPtr &tx, const std::string database, size_t l, size_t k,
       const size_t total_number_of_elements_to_get, const size_t hashTableNumBuckets, const size_t hashTableElementsPerBucket,
       const double distBound, const double r, const int norm, const double similarity_threshold, const size_t low,
       const size_t high)
{
    scoped_array_with_size<VectorEntry> vectors;
    int counter =0;
    read_n_elements_from_vectors(tx, vectors, low, high, total_number_of_elements_to_get,counter);
    for (size_t i =0; i < vectors.size(); i++) {
        if(vectors[i].compressedCounts.size() == 0)
            std::cout << "It is empty " << std::endl;
    }

    if (vectors[0].compressedCounts.size() == 0) {
        std::cerr << "Vector slot 0 is empty" << std::endl;
        abort();
    }
    boost::scoped_array< boost::scoped_array<size_t> > hash_for_vectors(new boost::scoped_array<size_t>[vectors.size()]);
    double allowed_negative_rate = 0.10;

    while(true) {
        compute_hash_functions<HammingHashFunctionSet>(vectors, hash_for_vectors, k, l, r, hashTableNumBuckets,
                                                       getUncompressedSizeOfVector(vectors[0].compressedCounts.get(),
                                                                                   vectors[0].compressedCounts.size()));
        negative_rate neg_rate(0,0,0,0,0,0,0,0);
        std::cout << " k is  " << k << " and l is " << l << std::endl;
        switch (norm) {
            case 1: {
                neg_rate = compute_parameter_quality<L1DistanceObject>(similarity_threshold, distBound, vectors,
                                                                       hash_for_vectors, l, hashTableNumBuckets,
                                                                       hashTableElementsPerBucket);

                break;
            }
            case 2: {
                neg_rate = compute_parameter_quality<L2DistanceObject>(similarity_threshold, distBound, vectors,
                                                                       hash_for_vectors, l, hashTableNumBuckets,
                                                                       hashTableElementsPerBucket);
                break;
            }
            default:
                std::cerr << "Bad value for --norm" << std::endl;
                abort(); // Should have been caught earlier
        }

        std::cerr << "l: " << l << " k: " << k << std::endl;
        std::cout << "false_negatives " << neg_rate.false_negatives << std::endl;
        std::cout << "conflicts " << neg_rate.conflicts << std::endl;
        std::cout << "biggest bucket size " << neg_rate.biggest_bucket_size << std::endl;

        std::cout << "total_neg_tests " << neg_rate.total_neg_tests << std::endl;
        std::cout << "total_conflicts " << neg_rate.total_conflicts << std::endl;

        std::cout << "positives       " << neg_rate.positives << std::endl;
        std::cout << "exact positives " << neg_rate.exact_positives << std::endl;
        std::cout << "total tests     " << neg_rate.total_tests <<std::endl; 

        std::cout << "allowed_negative_rate " << allowed_negative_rate << std::endl;

        std::cout << "false_neg_rate  " << neg_rate.false_negatives/ neg_rate.total_neg_tests << std::endl;
        std::cout << "conflicts_rate  " << neg_rate.conflicts/ neg_rate.total_conflicts << std::endl;

        if (neg_rate.false_negatives != 0) {
            l+=1;
            continue;
        }

        //Experiment in adding the factor also when the false negs are too high
        if ((neg_rate.conflicts/ neg_rate.total_conflicts) > allowed_negative_rate ||
            (neg_rate.biggest_bucket_size > neg_rate.exact_positives+0.02*vectors.size())) {
            k=2*k;
            continue;
        }

        break;
    }

    std::cout << "k:" << k << std::endl;
    std::cout << "l:" << l << std::endl;

    return std::pair<size_t,size_t>(k,l);
}
