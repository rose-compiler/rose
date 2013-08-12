#include "lsh.h"
#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <ext/hash_map>
#include "rose.h"
#include "createCloneDetectionVectorsBinary.h"

//MD5 sum header files
#include <openssl/md5.h>
#include <stdio.h>
#include <string.h>
using namespace std;
using namespace __gnu_cxx;

void read_vector_data(const SqlDatabase::TransactionPtr &tx, scoped_array_with_size<VectorEntry>& vectors,
                      const std::vector<int> function_to_look_for, map<string, std::vector<int> >& internTable,
                      const int groupLow, const int groupHigh, bool useCounts);

void find_exact_clones(const SqlDatabase::TransactionPtr &tx, int stride, int windowSize,
                       std::vector<int>& functionsThatWeAreInterestedIn, const size_t numStridesThatMustBeDifferent,
                       scoped_array_with_size<VectorEntry>& vectors, map<string, std::vector<int> >& internTable, bool useCounts);
