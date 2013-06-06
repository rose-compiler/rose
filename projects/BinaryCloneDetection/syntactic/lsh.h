#include <string>
#include <iostream>
#include <map>
#include <exception>
#include <stdint.h>
#include <cmath>
#include <stdio.h>

#include "sqlite3x.h"
#include <boost/random.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <cstring>

#include "vectorCompression.h"
#include "callLSH.h" // To get insert_into_clusters and insert_into_postprocessed_clusters

extern bool debug_messages;



template <typename T>
class scoped_array_with_size {
  boost::scoped_array<T> sa;
  size_t theSize;

  public:
  scoped_array_with_size(): sa(), theSize(0) {}
  scoped_array_with_size(size_t s): sa(new T[s]), theSize(s) {}

  void allocate(size_t s) {
    sa.reset(new T[s]);
    theSize = s;
  }
  size_t size() const {return theSize;}
  T* get() const {return sa.get();}

  T& operator[](size_t i) {return sa[i];}
  const T& operator[](size_t i) const {return sa[i];}

  private:
  scoped_array_with_size(const scoped_array_with_size<T>&); // Not copyable
};

struct VectorEntry {
  // CREATE TABLE vectors(row_number INTEGER PRIMARY KEY, function_id INTEGER,  index_within_function INTEGER, line INTEGER, offset INTEGER, sum_of_counts INTEGER, counts BLOB, instr_seq BLOB);
  uint32_t functionId;
  uint32_t indexWithinFunction;
  uint32_t rowNumber;
  uint64_t line;
  uint32_t offset;
  scoped_array_with_size<uint8_t> compressedCounts;
  char instrSeqMD5[16];
  VectorEntry() {} // Mark entry as invalid
};


// This is from callLSH.C, with the corresponding version for postprocessed_clusters added

void insert_into_clusters(
    sqlite3x::sqlite3_connection& con,
    int cluster,  
    int function_id, 
    int index_within_function,
    int vectors_row, 
    double dist 
    );


void insert_into_postprocessed_clusters(
    sqlite3x::sqlite3_connection& con,
    int cluster,  
    int function_id, 
    int index_within_function,
    int vectors_row, 
    double dist 
    );


void get_run_parameters(sqlite3x::sqlite3_connection& con, int& windowSize, int& stride);


void compute_vector_range(const scoped_array_with_size<VectorEntry>& vectors, size_t numVectorElements, scoped_array_with_size<uint16_t>& upperBounds, size_t& totalRange); 

class HammingHashFunctionSet {
  size_t k, l, numVectorElements, hashTableNumBuckets;
  scoped_array_with_size<uint16_t> upperBounds;
  size_t totalRange;
  scoped_array_with_size<size_t> hashFunctionVectorIndexes;
  scoped_array_with_size<size_t> hashFunctionVectorCompareValues;
  scoped_array_with_size<size_t> hashFunctionCoeffs;

  public:
  HammingHashFunctionSet(const scoped_array_with_size<VectorEntry>& vectors, size_t k, size_t l, double /* r */,
      size_t numVectorElements, size_t hashTableNumBuckets);
  private:
  void create_hash_functions();

  public:
  void compute_hashes(const scoped_array_with_size<uint8_t>& compressedCounts, const boost::scoped_array<size_t>& hashes) const;

};

class StableDistributionHashFunctionSet {
  double r;
  size_t k, l, numVectorElements, hashTableNumBuckets;
  scoped_array_with_size<float> hashFunctionMatrix;
  scoped_array_with_size<float> hashFunctionBiases;
  scoped_array_with_size<size_t> hashFunctionCoeffs;

  public:
  StableDistributionHashFunctionSet(const scoped_array_with_size<VectorEntry>& /* vectors */, size_t k, size_t l, double r,
      size_t numVectorElements, size_t hashTableNumBuckets);
  private:
  void create_hash_functions();

  public:
  void compute_hashes(const scoped_array_with_size<uint8_t>& compressedCounts, const boost::scoped_array<size_t>& hashes) const;

};

template <typename Value>
class HashTableGroup {
  const size_t numHashTables, numBuckets, maxBucketSize, singleHashTableSize;
  boost::scoped_array<Value> table;
  boost::scoped_array<size_t> bucketSizes; // Values are 0..maxBucketSize-1 for non-full buckets, maxBucketSize for a full bucket that hasn't overflowed yet, and maxBucketSize+1 for a bucket that has overflowed into later buckets

  public:
  HashTableGroup(size_t numHashTables, size_t numBuckets, size_t maxBucketSize):
    numHashTables(numHashTables), numBuckets(numBuckets), maxBucketSize(maxBucketSize),
    singleHashTableSize(numBuckets * maxBucketSize),
    table(new Value[numHashTables * numBuckets * maxBucketSize]),
    bucketSizes(new size_t[numHashTables * numBuckets]) {
      memset(bucketSizes.get(), 0, sizeof(size_t) * numHashTables * numBuckets);
    }

  void insert(size_t tableNum, size_t hashCode, const Value& v) {
    size_t actualBucket = hashCode;
    size_t* bucketSizePtr = &bucketSizes[tableNum * numBuckets + actualBucket];
    while (*bucketSizePtr >= maxBucketSize) {
      // std::cerr << "Bucket " << actualBucket << " is overflowed -- skipping on insert" << std::endl;
      *bucketSizePtr = maxBucketSize + 1; // Mark bucket as overflowed if it wasn't already
      ++actualBucket;
      ++bucketSizePtr;
      if (actualBucket >= numBuckets) {
        actualBucket -= numBuckets;
        bucketSizePtr -= numBuckets;
      }
      if (actualBucket == hashCode) {
        std::cerr << "Hash table full" << std::endl;
        abort();
      }
    }
    assert (*bucketSizePtr < maxBucketSize);
    table[tableNum * singleHashTableSize + actualBucket * maxBucketSize + *bucketSizePtr] = v;
    ++(*bucketSizePtr);
    // std::cerr << "Vector " << i << " hash number " << j << " is " << thisHash << std::endl;
  }

  void append_bucket_contents(size_t tableNum, size_t hashCode, std::vector<Value>& contents) const {
    size_t bucketNum = hashCode;
    while (true) { // Iterate through all overflowed buckets and the first non-full bucket found
      const Value* const bucketBase = &table[tableNum * singleHashTableSize + bucketNum * maxBucketSize];
      const size_t rawBucketSize = bucketSizes[tableNum * numBuckets + bucketNum];
      const bool bucketOverflowed = (rawBucketSize > maxBucketSize);
      // The ?: expression here prevents copying too many elements from overflowed buckets
      const size_t bucketSize = bucketOverflowed ? maxBucketSize : rawBucketSize;
      contents.insert(contents.end(), bucketBase, bucketBase + bucketSize);
      if (bucketOverflowed) {
        // std::cerr << "Need to scan bucket after full bucket " << bucketNum << std::endl;
        ++bucketNum;
        if (bucketNum >= numBuckets) bucketNum -= numBuckets;
        continue;
      } else {
        break;
      }
    }
  }

  size_t get_biggest_bucket() const {
    size_t currentMax =0;

    for (size_t i = 0; i < numHashTables; ++i) {
         for (size_t j = 0; j < numBuckets; ++j) {
        size_t totalBucketSize = 0;
        size_t j2 = j;
        while (true) {
          const size_t rawBucketSize = bucketSizes[i * numBuckets + j2];
          const bool bucketOverflowed = (rawBucketSize > maxBucketSize);
          const size_t bucketSize = bucketOverflowed ? maxBucketSize : rawBucketSize;
          totalBucketSize += bucketSize;

          
          if (!bucketOverflowed) break;
          j2 = (j2 + 1) % numBuckets;
          if (j2 == j) {
            std::cerr << "All hash table buckets are overfull; this is not allowed" << std::endl;
            abort();
          }
        }

        if(totalBucketSize>currentMax) currentMax = totalBucketSize;

      }
    }
    return currentMax;
  }
  void print_distribution_statistics(std::ostream& o) const {
    o << "Statistics for " << numHashTables << " hash table(s)\n";
    for (size_t i = 0; i < numHashTables; ++i) {
      o << "Table " << i << ":\n";
      std::map<size_t, size_t> sizes;
      for (size_t j = 0; j < numBuckets; ++j) {
        size_t totalBucketSize = 0;
        size_t j2 = j;
        while (true) {
          const size_t rawBucketSize = bucketSizes[i * numBuckets + j2];
          const bool bucketOverflowed = (rawBucketSize > maxBucketSize);
          const size_t bucketSize = bucketOverflowed ? maxBucketSize : rawBucketSize;
          totalBucketSize += bucketSize;
          if (!bucketOverflowed) break;
          j2 = (j2 + 1) % numBuckets;
          if (j2 == j) {
            std::cerr << "All hash table buckets are overfull; this is not allowed" << std::endl;
            abort();
          }
        }
        ++sizes[totalBucketSize];
      }
      for (std::map<size_t, size_t>::reverse_iterator it = sizes.rbegin();
           it != sizes.rend(); ++it) {
        o << "Spill size " << it->first << " occurs for " << it->second << "bucket(s)\n";
      }
    }
  }

};

struct L1DistanceObject {
  double operator()(const scoped_array_with_size<uint8_t>& a, const boost::scoped_array<uint16_t>& b) const;
};

struct L2DistanceObject {
  double operator()(const scoped_array_with_size<uint8_t>& a, const boost::scoped_array<uint16_t>& b) const;
};

class LSHTableBase {
  public:
    virtual ~LSHTableBase() {}
    virtual std::vector<std::pair<size_t, double> > query(size_t i) const = 0;
};

template <typename HashFunctionGenerator, typename DistanceFunc>
class LSHTable: public LSHTableBase {
  const scoped_array_with_size<VectorEntry>& vectors;
  HashFunctionGenerator hashFunctions;
  DistanceFunc distance;
  HashTableGroup<size_t> hashTables;
  size_t l, numVectorElements;
  double distBound;

  public:
  LSHTable(const scoped_array_with_size<VectorEntry>& vectors, const DistanceFunc& distance,
      size_t k, size_t l, double r,
      size_t numVectorElements, size_t numBuckets, size_t maxBucketSize,
      double distBound):
    vectors(vectors), hashFunctions(vectors, k, l, r, numVectorElements, numBuckets),
    distance(distance), hashTables(l, numBuckets, maxBucketSize),
    l(l), numVectorElements(numVectorElements),
    distBound(distBound) {

      //Loop to insert into the hash table according to distance
      for (size_t i = 0; i < vectors.size(); ++i) {
        const VectorEntry& ve = vectors[i];
        if (ve.compressedCounts.size() == 0) continue; // Empty entry
        this->insert(i, ve.compressedCounts);
        if (i % 100000 == 0 && debug_messages ) std::cerr << "Finished inserting vector " << i << std::endl;
      }
      hashTables.print_distribution_statistics(std::cerr);
    }

  private:
  void insert(size_t index, const scoped_array_with_size<uint8_t>& compressedCounts) {
    boost::scoped_array<size_t> hashes(new size_t[l]);
    hashFunctions.compute_hashes(compressedCounts, hashes);
    for (size_t j = 0; j < l; ++j) {
      hashTables.insert(j, hashes[j], index);
    }
  }

  public:
  std::vector<std::pair<size_t, double> > query(size_t i) const { // Pairs are vector number, distance
    std::vector<std::pair<size_t, double> > clusterElements;
    clusterElements.push_back(std::make_pair(i, 0));
    const VectorEntry& ve = vectors[i];
    boost::scoped_array<size_t> hashes(new size_t[l]);
    hashFunctions.compute_hashes(ve.compressedCounts, hashes);
    std::vector<size_t> bucketContents;
    for (size_t hashNum = 0; hashNum < l; ++hashNum) { //Loop over hash tables
      size_t bucketNum = hashes[hashNum];
      hashTables.append_bucket_contents(hashNum, bucketNum, bucketContents);
    }
    boost::scoped_array<uint16_t> uncompressedVectorI(new uint16_t[numVectorElements]);
    decompressVector(ve.compressedCounts.get(), ve.compressedCounts.size(), uncompressedVectorI.get());
    // Remove duplicates to avoid distance computations
    std::sort(bucketContents.begin(), bucketContents.end());
    bucketContents.erase(std::unique(bucketContents.begin(), bucketContents.end()), bucketContents.end());
    for (size_t i = 0; i < bucketContents.size(); ++i) {
      size_t entry = bucketContents[i];
      const VectorEntry& ve2 = vectors[entry];
      double dist = distance(ve2.compressedCounts, uncompressedVectorI);
      if (dist > distBound) continue;
      clusterElements.push_back(std::make_pair(entry, dist));
    }
    return clusterElements;
  }
};

