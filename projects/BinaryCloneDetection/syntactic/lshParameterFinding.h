#include "lsh.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include "computerangesFunc.h"

using namespace boost::program_options;
using namespace sqlite3x;
using boost::lexical_cast;


float computeFunctionP(float w, float c){

  if ( c < 10e-6 )

    return 1.;
  else {
    float x = w / c;
    return 1 - erfc(x / 1.41421356237309504880) - 1.12837916709551257390 / 1.41421356237309504880 / x * (1 - expf(-((x) * (x)) / 2));
  }
}


long int computeMForULSH(long int k, float successProbability){
  {if (!((k & 1) == 0)) {fprintf(stderr, "ASSERT failed on line %d, file %s.\n", 333, "SelfTuning.cpp"); exit(1);}};
  float mu = 1 - (powf(computeFunctionP(4.0, 1), k / 2));
  float P = successProbability;
  float d = (1-mu)/(1-P)*1/logf(1/mu) * (powf(mu, -1/(1-mu)));
  float y = logf(d);
  long int m = ((long int)(ceilf(1 - y/logf(mu) - 1/(1-mu))));
  while ((powf(mu, m-1)) * (1 + m * (1-mu)) > 1 - P){
    m++;
  }
  return m;
}



struct group_probability
{
  double portion_of_total;
  int low;
  int high;

  group_probability(double p, int l, int h)
    : portion_of_total(p), low(l), high(h)
    { };
};



static void read_n_elements_from_vectors(sqlite3_connection& con, scoped_array_with_size<VectorEntry>& vectors, int low, int high, int number_of_elements_to_get, int& counter)
{
  size_t eltCount = 0;

  try {
    sqlite3_command cmd(con,
        high == -1
        ? "SELECT count(row_number) from vectors where sum_of_counts >= ? order by random() "
        : "SELECT count(row_number) from vectors where sum_of_counts >= ? and sum_of_counts <= ? order by random() ");

    cmd.bind(1,boost::lexical_cast<std::string>(low));
    if (high != -1){
      cmd.bind(2, boost::lexical_cast<std::string>(high));

    }
    eltCount = cmd.executeint64();
  } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}

  if (eltCount == 0) {
    std::cerr << "No vectors found -- invalid database?" << std::endl;
    exit (1);
  }
  if(eltCount<5000)
    number_of_elements_to_get = eltCount;
  std::cerr << "Found " << eltCount << " vectors " << number_of_elements_to_get << std::endl;

  vectors.allocate(number_of_elements_to_get);

  try {
    sqlite3_command cmd(con,
        high == -1 ? "SELECT row_number, function_id, index_within_function, line, offset, counts, instr_seq  from vectors where sum_of_counts >= ?"
        " order by random() limit ?"     
        : "SELECT  row_number, function_id, index_within_function, line, offset, counts, instr_seq from vectors where sum_of_counts >= ? and sum_of_counts <= ?"
        " order by random() limit ?"
        );
    cmd.bind(1, boost::lexical_cast<std::string>(low));


    if (high != -1){
      cmd.bind(2, boost::lexical_cast<std::string>(high));
      cmd.bind(3, boost::lexical_cast<std::string>( number_of_elements_to_get  ));

    }else
      cmd.bind(2, boost::lexical_cast<std::string>( number_of_elements_to_get ));


    //sqlite3_command cmd(con, "select row_number, function_id, index_within_function, line, offset, counts, instr_seq from vectors order by random() limit ?");
    //cmd.bind(1,total_number_of_elements_to_get);
    sqlite3_reader r = cmd.executereader();
    while (r.read()) {
      long long rowNumber = r.getint64(0);
      int functionId = r.getint(1);
      int indexWithinFunction = r.getint(2);
      long long line = r.getint64(3);
      int offset = r.getint(4);
      std::string compressedCounts = r.getblob(5);
      std::string instrSeqMD5 = r.getblob(6);
      if (rowNumber <= 0 ) {
        std::cerr << "Row number in database is out of range" << std::endl;
        abort();
      }
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
      if (rowNumber % 100000 == 0) std::cerr << "Got row " << rowNumber << std::endl;

      counter++;
    }
  } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}

  for(size_t i =0; i < vectors.size(); i++)
  {
    if(vectors[i].compressedCounts.size() == 0)
      std::cout << "1:It is empty " << std::endl;

  };

};


static void read_vector_data(sqlite3_connection& con, scoped_array_with_size<VectorEntry>& vectors, int total_number_of_elements_to_get, int distBound) {
  vectors.allocate(total_number_of_elements_to_get);


  size_t eltCount = 0;
  try {
    eltCount = lexical_cast<size_t>(con.executestring("select count(row_number) from vectors"));
  } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}

  if (eltCount == 0) {
    std::cerr << "No vectors found -- invalid database?" << std::endl;
    exit (1);
  }

  //Finding groups
  //double distance = sqrt((1. - distBound) * 50.);
  std::vector<Range> ranges = computeranges(distBound, 50, 100000);

  std::vector<int> rangeSize;

  int total_number_of_elements_in_groups=0;
  int maxNumElementsInGroup = -1;
  int maxNumElementIdx = -1;

  for (size_t i = 0; i < ranges.size(); ++i) {
    try {
      sqlite3_command cmd(con,
          ranges[i].high == -1 ? "SELECT count(row_number) from vectors where sum_of_counts >= ?"
          : "SELECT count(row_number) from vectors where sum_of_counts >= ? and sum_of_counts <= ?");
      cmd.bind(1, boost::lexical_cast<std::string>(ranges[i].low));
      if (ranges[i].high != -1) cmd.bind(2, boost::lexical_cast<std::string>(ranges[i].high));
      int numElementsInGroup = cmd.executeint();
      total_number_of_elements_in_groups+=numElementsInGroup;
      rangeSize.push_back(numElementsInGroup);
      if (numElementsInGroup > maxNumElementsInGroup) {
        maxNumElementsInGroup = numElementsInGroup;
        maxNumElementIdx = i;
      }

    }    catch (std::exception& e) {std::cerr << "Caught: " << e.what() << std::endl;}
  }
  int counter = 0 ;
 
  read_n_elements_from_vectors(con, vectors, ranges[maxNumElementIdx].low, ranges[maxNumElementIdx].high, total_number_of_elements_in_groups,counter);


#if 0
  for (size_t i = 0; i < ranges.size(); ++i) {
    int number_of_elements_to_get; 
    try {
      int numElementsInGroup = rangeSize[i];
      double groupProb = numElementsInGroup/(double)total_number_of_elements_in_groups;

      if( groupProb <  1/(double)total_number_of_elements_to_get ) continue;
      number_of_elements_to_get = floor(groupProb*total_number_of_elements_to_get);
   
      
    } catch (std::exception& e) {std::cerr << "Caught: " << e.what() << std::endl;}


    if(counter+number_of_elements_to_get >= total_number_of_elements_to_get)
    {
      std::cout << "The fetch exceeds the total_number_of_elements_to_get " << total_number_of_elements_to_get << 
                   " and is " << counter+number_of_elements_to_get << std::endl;
      abort();

    }
    read_n_elements_from_vectors(con, vectors,ranges[i].low, ranges[i].high, number_of_elements_to_get, counter);

  }


  int remaining_elements = total_number_of_elements_to_get-(counter+1);
  if(remaining_elements>0)
      read_n_elements_from_vectors(con, vectors,0, -1, remaining_elements, counter);
#endif
  
#if 0
  try {
    eltCount = lexical_cast<size_t>(con.executestring("select count(row_number) from vectors"));
  } catch (std::exception& e) {std::cerr << "Exception: " << e.what() << std::endl;}

  if (eltCount == 0) {
    std::cerr << "No vectors found -- invalid database?" << std::endl;
    exit (1);
  }

  std::cerr << "Found " << eltCount << " vectors" << std::endl;
#endif

}

#if 0
void compute_hash_for_vector(size_t index, const scoped_array_with_size<uint8_t>& compressedCounts) {
  boost::scoped_array<size_t> hashes(new size_t[l]);
  hashFunctions.compute_hashes(compressedCounts, hashes);
}
#endif



double computeSimilarity( const boost::scoped_array<size_t>&  elem1,  const boost::scoped_array<size_t>& elem2, int size  )
{

  int equal=0;
  for( int i = 0 ; i < size ; i++)
  {
//    std::cout << elem1[i] << ":" << elem2[i] << " ";
//    if(elem2[i]) equal++;
    if(elem1[i] == elem2[i]) equal++;
  }
//  std::cout << std::endl;
 
  return equal/(double)size;
};
double computeSimilarity( const boost::scoped_array<uint16_t>&  elem1,  const boost::scoped_array<uint16_t>& elem2, int size  )
{

  int equal=0;
  for( int i = 0 ; i < size ; i++)
  {
//    std::cout << elem1[i] << ":" << elem2[i] << " ";
    if(elem1[i] == elem2[i]) equal++;
  }
//  std::cout << std::endl;
 
  return equal/(double)size;
};

double computeSimilarityDisplay( const boost::scoped_array<size_t>&  elem1,  const boost::scoped_array<size_t>& elem2, int size  )
{

  int equal=0;
  std::cout << "The size " << size << " " ;
  for( int i = 0 ; i < size ; i++)
  {
//    std::cout << elem1[i] << ":" << elem2[i] << " ";
    if(elem1[i] == elem2[i]) equal++;
    std::cout << i << ":" << elem1[i] <<  ":" << elem2[i] << " ";
  }
  std::cout << std::endl;
 
  
  return equal/(double)size;
};


template< typename HashFunctionGenerator >
void compute_hash_functions( const scoped_array_with_size<VectorEntry>& vectors, boost::scoped_array< boost::scoped_array<size_t> >& hash_for_vectors,
    const size_t k, const size_t l, const double r, const size_t b, size_t number_of_vec_elements )
{
  HashFunctionGenerator hashFunctions(vectors, k, l, r, number_of_vec_elements, b);

  for(size_t i = 0; i < vectors.size(); ++i)
    hash_for_vectors[i].reset(new size_t[l]);
 
  boost::scoped_array<size_t> hashes(new size_t[l]);
 
  for (size_t i = 0; i < vectors.size(); ++i) {
    const VectorEntry& ve = vectors[i];
    if (ve.compressedCounts.size() == 0) {
      std::cout << "Empty entry" << std::endl;
      
      continue; // Empty entry
    }
    hashFunctions.compute_hashes(ve.compressedCounts, hashes);
    
    for (size_t j = 0; j < l; ++j) {
     hash_for_vectors[i][j] = hashes[j];
    }
  }

}

struct negative_rate
{
   int positives;
   int exact_positives;
   int total_tests;
   int total_conflicts;
   int total_neg_tests;
   double conflicts;
   double false_negatives;
   size_t biggest_bucket_size;
   negative_rate(int pos, int exact_pos, int tot_tests, int tot_pos_tests, int tot_neg_tests, double conflicts, double false_neg, 
       size_t bbs)
     : positives(pos), exact_positives(exact_pos), total_tests(tot_tests), total_conflicts(tot_pos_tests), total_neg_tests(tot_neg_tests), conflicts(conflicts), false_negatives(false_neg), biggest_bucket_size(bbs)
   {}
};

template< typename DistanceFunc>
negative_rate compute_parameter_quality(const double s_0, const double distanceBound, 
     const scoped_array_with_size<VectorEntry>& vectors,  const  boost::scoped_array< boost::scoped_array<size_t> >& hash_for_vectors,
     const size_t l, size_t numBuckets,size_t  maxBucketSize){
  DistanceFunc lshDist;

  HashTableGroup<size_t> hashTables(l, numBuckets, maxBucketSize);
  //Loop to insert into the hash table according to distance

  for (size_t i = 0; i < vectors.size(); ++i) {
    const VectorEntry& ve = vectors[i];
    if (ve.compressedCounts.size() == 0) continue; // Empty entry
    for (size_t j = 0; j < l; ++j) {
      hashTables.insert(j, hash_for_vectors[i][j], i);
    }
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
      //if(i==j) continue;

#if 0
      boost::scoped_array<uint16_t> uncompressedVectorI2(new uint16_t[numVectorElements]);
      decompressVector(vectors[j].compressedCounts.get(), vectors[j].compressedCounts.size(), uncompressedVectorI2.get());
#endif


      double dist       = lshDist(vectors[j].compressedCounts, uncompressedVectorI    );
      double similarity = computeSimilarity(hash_for_vectors[i], hash_for_vectors[j],l);

#if 0
      double exact_sim  = computeSimilarity(uncompressedVectorI, uncompressedVectorI2, numVectorElements);
      
      if(exact_sim >= s_0 )
        exact_positives++;
#endif
      total_tests++;
      
      if( dist < distanceBound ) // The distance is within the distance bound
      {
        total_neg_tests++;
        if( similarity == 0 ) // Not found to be similar in hash functions
        {

          //std::cout << dist << " distBound " << distanceBound << std::endl;
          //std::cout << "Adding to false_negatives" << std::endl;
          //std::cout << "similarity " << similarity << " dist " << dist << std::endl;
          false_negatives += 1.0;
          //computeSimilarityDisplay(hash_for_vectors[i], hash_for_vectors[j],l);

          
        }
      }else{
        total_conflicts++;

        if( similarity > s_0  ) // A hash table has found the vectors to be similar in at least
                                //one hash table
        {
          //std::cout << "Adding to conflicts" << std::endl;
          //std::cout << "similarity " << similarity << " s_0 " << s_0 << std::endl;

          conflicts += 1;
        }
 
        positives++;

      }
    };
  };

  return negative_rate(positives,exact_positives, total_tests, total_conflicts, total_neg_tests, conflicts, false_negatives, biggest_bucket);
  
};


std::pair<size_t, size_t> runLSH(sqlite3_connection& con, const std::string database, size_t l, size_t k, const size_t total_number_of_elements_to_get,
    const size_t hashTableNumBuckets, const size_t hashTableElementsPerBucket, const double distBound, const double r,
    const int norm, const double similarity_threshold, const size_t low, const size_t high
    )
{

  
  scoped_array_with_size<VectorEntry> vectors;
  //read_vector_data(con, vectors,total_number_of_elements_to_get, distBound);
  int counter =0;
  read_n_elements_from_vectors(con, vectors, low, high, total_number_of_elements_to_get,counter);
  for(size_t i =0; i < vectors.size(); i++)
  {
    if(vectors[i].compressedCounts.size() == 0)
      std::cout << "It is empty " << std::endl;

  };

  if (vectors[0].compressedCounts.size() == 0) {
    std::cerr << "Vector slot 0 is empty" << std::endl;
    abort();
  }
  boost::scoped_array< boost::scoped_array<size_t> > hash_for_vectors(new boost::scoped_array<size_t>[vectors.size()] )  ;

  double allowed_negative_rate = 0.10;
  int changed_false_negatives = 0;
  int changed_conflicts = 0;


  while(true)
  {

    compute_hash_functions<HammingHashFunctionSet>(vectors, hash_for_vectors, k, l, r, hashTableNumBuckets,
        getUncompressedSizeOfVector(vectors[0].compressedCounts.get(),vectors[0].compressedCounts.size()));
    negative_rate neg_rate(0,0,0,0,0,0,0,0);
    
//    long int m = computeMForULSH(k, similarity_threshold);
 //   l = m * (m-1) / 2;


    
    std::cout << " k is  " << k << " and l is " << l << std::endl;


    switch (norm) {
      case 1:
        {
          neg_rate = compute_parameter_quality<L1DistanceObject>(similarity_threshold, distBound, 
              vectors, hash_for_vectors, l, hashTableNumBuckets, hashTableElementsPerBucket);

          break;
        }

      case 2:
        {
          neg_rate = compute_parameter_quality<L2DistanceObject>(similarity_threshold, distBound, 
              vectors, hash_for_vectors, l, hashTableNumBuckets, hashTableElementsPerBucket);

          break;
        }

      default: std::cerr << "Bad value for --norm" << std::endl; abort(); // Should have been caught earlier
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

    //uint64_t diff = changed_conflicts - changed_false_negatives;
    //uint64_t allowed_diff = 2;
    
    //    if( (neg_rate.false_negatives/ neg_rate.total_neg_tests) > allowed_negative_rate
    //        || (diff>allowed_diff ) )
    if( neg_rate.false_negatives != 0 )

    {
      l+=1;
      continue;
    }

    //Experiment in adding the factor also when the false negs are too high
    if( (neg_rate.conflicts/ neg_rate.total_conflicts) > allowed_negative_rate
        || ( neg_rate.biggest_bucket_size > neg_rate.exact_positives+0.02*vectors.size() ))
    {
      k=2*k;
      continue;
    }

    break;
  }

  std::cout << "k:" << k << std::endl;
  std::cout << "l:" << l << std::endl;

  return std::pair<size_t,size_t>(k,l);
};


