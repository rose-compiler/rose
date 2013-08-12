#ifndef lshParameterFinding_H
#define lshParameterFinding_H

#include "lsh.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include "computerangesFunc.h"

using namespace boost::program_options;

float computeFunctionP(float w, float c);
long int computeMForULSH(long int k, float successProbability);

struct group_probability {
    double portion_of_total;
    int low;
    int high;

    group_probability(double p, int l, int h)
        : portion_of_total(p), low(l), high(h)
        {}
};

double computeSimilarity(const boost::scoped_array<size_t>& elem1, const boost::scoped_array<size_t>& elem2, int size);

double computeSimilarity(const boost::scoped_array<uint16_t>& elem1, const boost::scoped_array<uint16_t>& elem2, int size);

double computeSimilarityDisplay(const boost::scoped_array<size_t>& elem1, const boost::scoped_array<size_t>& elem2, int size);

template <typename HashFunctionGenerator>
void
compute_hash_functions(const scoped_array_with_size<VectorEntry>& vectors,
                       boost::scoped_array <boost::scoped_array<size_t> >& hash_for_vectors,
                       const size_t k, const size_t l, const double r, const size_t b, size_t number_of_vec_elements)
{
    HashFunctionGenerator hashFunctions(vectors, k, l, r, number_of_vec_elements, b);

    for (size_t i = 0; i < vectors.size(); ++i)
        hash_for_vectors[i].reset(new size_t[l]);
 
    boost::scoped_array<size_t> hashes(new size_t[l]);
 
    for (size_t i = 0; i < vectors.size(); ++i) {
        const VectorEntry& ve = vectors[i];
        if (ve.compressedCounts.size() == 0) {
            std::cout << "Empty entry" << std::endl;
            continue; // Empty entry
        }
        hashFunctions.compute_hashes(ve.compressedCounts, hashes);
        for (size_t j = 0; j < l; ++j)
            hash_for_vectors[i][j] = hashes[j];
    }
}

struct negative_rate {
    int positives;
    int exact_positives;
    int total_tests;
    int total_conflicts;
    int total_neg_tests;
    double conflicts;
    double false_negatives;
    size_t biggest_bucket_size;
    negative_rate(int pos, int exact_pos, int tot_tests, int tot_pos_tests, int tot_neg_tests, double conflicts,
                  double false_neg, size_t bbs)
        : positives(pos), exact_positives(exact_pos), total_tests(tot_tests), total_conflicts(tot_pos_tests),
          total_neg_tests(tot_neg_tests), conflicts(conflicts), false_negatives(false_neg), biggest_bucket_size(bbs)
        {}
};

template< typename DistanceFunc>
negative_rate
compute_parameter_quality(const double s_0, const double distanceBound, const scoped_array_with_size<VectorEntry>& vectors,
                          const  boost::scoped_array< boost::scoped_array<size_t> >& hash_for_vectors, const size_t l,
                          size_t numBuckets,size_t  maxBucketSize)
{
    DistanceFunc lshDist;

    HashTableGroup<size_t> hashTables(l, numBuckets, maxBucketSize);
    //Loop to insert into the hash table according to distance
    for (size_t i = 0; i < vectors.size(); ++i) {
        const VectorEntry& ve = vectors[i];
        if (ve.compressedCounts.size() == 0) continue; // Empty entry
        for (size_t j = 0; j < l; ++j)
            hashTables.insert(j, hash_for_vectors[i][j], i);
    }

    size_t biggest_bucket = hashTables.get_biggest_bucket();
    double conflicts = 0;
    double false_negatives = 0;
    int positives = 0;
    int total_tests =0;
    int exact_positives =0; 
    int total_conflicts=0;
    int total_neg_tests=0;
    size_t numVectorElements = getUncompressedSizeOfVector(vectors[0].compressedCounts.get(), vectors[0].compressedCounts.size());

    for (size_t i = 0; i < vectors.size(); ++i) {
        boost::scoped_array<uint16_t> uncompressedVectorI(new uint16_t[numVectorElements]);
        const VectorEntry& ve = vectors[i];
        decompressVector(ve.compressedCounts.get(), ve.compressedCounts.size(), uncompressedVectorI.get());

        for (size_t j = i+1; j < vectors.size(); ++j) {
            double dist       = lshDist(vectors[j].compressedCounts, uncompressedVectorI);
            double similarity = computeSimilarity(hash_for_vectors[i], hash_for_vectors[j],l);
            total_tests++;
      
            if (dist < distanceBound) { // The distance is within the distance bound
                total_neg_tests++;
                if (similarity == 0) // Not found to be similar in hash functions
                    false_negatives += 1.0;
            } else {
                total_conflicts++;

                if (similarity > s_0 ) // A hash table has found the vectors to be similar in at least one hash table
                    conflicts += 1;
                positives++;
            }
        }
    }

    return negative_rate(positives, exact_positives, total_tests, total_conflicts, total_neg_tests, conflicts,
                         false_negatives, biggest_bucket);
  
}

std::pair<size_t, size_t> runLSH(const SqlDatabase::TransactionPtr &tx, const std::string database, size_t l, size_t k,
                                 const size_t total_number_of_elements_to_get, const size_t hashTableNumBuckets,
                                 const size_t hashTableElementsPerBucket, const double distBound, const double r,
                                 const int norm, const double similarity_threshold, const size_t low, const size_t high);

#endif
