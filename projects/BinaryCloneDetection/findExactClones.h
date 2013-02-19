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
using namespace sqlite3x;
using namespace std;
using namespace __gnu_cxx;

static void read_vector_data(sqlite3_connection& con, scoped_array_with_size<VectorEntry>& vectors, const std::vector<int> function_to_look_for,
                 map<string, std::vector<int> >& internTable, const int groupLow, const int groupHigh, bool useCounts
    ) {

  
  size_t eltCount = 0;

  try {

    string findNumElement = function_to_look_for.size() > 0 ? "select count(row_number) from vectors where instr_seq in"
      " (select distinct instr_seq from vectors where function_id in (select function_id from function_to_look_for) ) "
      " ORDER BY row_number" :
      ( groupHigh == -1 ? "SELECT count(*) from vectors where sum_of_counts >= ?" : 
        "SELECT count(*) from vectors where sum_of_counts >= ? and sum_of_counts <= ?" );

    std::cout << "findNumElements " << findNumElement << " low  " << groupLow << " high " << groupHigh << std::endl; 
    sqlite3_command cmd(con, findNumElement );
    if(function_to_look_for.size() == 0 )
    {
      cmd.bind(1,boost::lexical_cast<string>(groupLow));
      if( groupHigh != -1 )
        cmd.bind(2,boost::lexical_cast<string>(groupHigh));
    }

    eltCount = cmd.executeint64();

  } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}

  if (eltCount == 0) {
    cerr << "No vectors found -- invalid database?" << endl;
    exit (1);
  }

  cerr << "Found " << eltCount << " vectors" << endl;

  vectors.allocate(eltCount);


  std::list<string> instSeq;



  try {

    string findVectorsToCheck =
      function_to_look_for.size() > 0 ?  
      " select row_number, function_id, index_within_function, line, offset, counts, instr_seq from vectors where instr_seq in"
      " (select distinct instr_seq from vectors where function_id in (select function_id from function_to_look_for) )"
      " ORDER BY row_number" :
      ( groupHigh == -1 ? "SELECT row_number, function_id, index_within_function, line, offset, counts, instr_seq  from vectors where sum_of_counts >= ?" : 
        "SELECT row_number, function_id, index_within_function, line, offset, counts, instr_seq from vectors where sum_of_counts >= ? and sum_of_counts <= ?" );
   sqlite3_command cmd(con, findVectorsToCheck );

    if(function_to_look_for.size() == 0 )
    {
      cmd.bind(1,boost::lexical_cast<string>(groupLow));
      if( groupHigh != -1 )
        cmd.bind(2,boost::lexical_cast<string>(groupHigh));
    }

    sqlite3_reader r = cmd.executereader();

    size_t indexInVectors=0;

    while (r.read()) {
      long long rowNumber = r.getint64(0);
      int functionId = r.getint(1);
      int indexWithinFunction = r.getint(2);
      long long line = r.getint64(3);
      int offset = r.getint(4);
      string compressedCounts = r.getblob(5);
      string instrSeqMD5 = r.getblob(6);

      if (rowNumber <= 0 || indexInVectors > eltCount) {
        cerr << "Row number in database is out of range" << endl;
        abort();
      }


      std::string index;
      if(useCounts == true)
        index = compressedCounts;
      else
        index = instrSeqMD5;
        
      
      VectorEntry& ve = vectors[indexInVectors];
      ve.rowNumber  = rowNumber;
      ve.functionId = functionId;
      ve.indexWithinFunction = indexWithinFunction;
      ve.line = line;
      ve.offset = offset;
      ve.compressedCounts.allocate(compressedCounts.size());
      memcpy(ve.compressedCounts.get(), compressedCounts.data(), compressedCounts.size());
      if (instrSeqMD5.size() != 16) {
        cerr << "Found MD5 with length other than 16" << endl;
        abort();
      }
     memcpy(ve.instrSeqMD5, instrSeqMD5.data(), 16);

     internTable[instrSeqMD5].push_back(indexInVectors);
     indexInVectors++;
         
     if (rowNumber % 100000 == 0) cerr << "Got row " << rowNumber << endl;
    }
  } catch (exception& e) {cerr << "Exception: " << e.what() << endl;}
}

void find_exact_clones(sqlite3_connection& con,  int stride, int windowSize, std::vector<int>& functionsThatWeAreInterestedIn,
   const size_t numStridesThatMustBeDifferent, scoped_array_with_size<VectorEntry>& vectors,   map<string, std::vector<int> >& internTable, bool useCounts
   )
{

  try {
    sqlite3_transaction trans2(con);

    size_t clusterNum = 0, postprocessedClusterNum = 0;
    for( map<string, std::vector<int> >::iterator iItr = internTable.begin();
        iItr != internTable.end(); ++iItr)
    {

      
      if(iItr->second.size() > 1 )
     {

        //Postprocessed-clusters is the interesting cluster representation for the
        //exact clone detection
        for(size_t i =0; i < iItr->second.size() ; i++)
        {

          int indexInVectors = iItr->second[i];
//          bool insertedIntoClusters = true;
          bool interestedInCluster = false;

          VectorEntry& ve = vectors[indexInVectors];

#if 0          
          for(size_t j = 0; j < functionsThatWeAreInterestedIn.size(); j++)
            for(size_t k = 0; k < iItr->second.size(); k++)
              if ( ve.functionId == functionsThatWeAreInterestedIn[j] )
              {
                interestedInCluster = true;
                break;
              }
       
#endif
    
          if(functionsThatWeAreInterestedIn.size()>0 && (interestedInCluster == false) )
            continue;
          insert_into_clusters(con, clusterNum, ve.functionId, ve.indexWithinFunction, ve.rowNumber,0);


          
        }
        

        //Do postprocessing
        bool first =true;
        //The next two variables will be initialized in the first iteration of the next for look
        size_t lastFunctionId = 0;
        size_t lastIndexWithinFunction =0;
        bool insertedIntoPostprocessed = false;

        std::vector<int> postprocessedClusterElements;
        for (size_t j = 0; j < iItr->second.size(); ++j) {
          int indexInVectors = iItr->second[j];
          const VectorEntry& ve = vectors[indexInVectors];

          
          if (first || ve.functionId != lastFunctionId || ve.indexWithinFunction >= lastIndexWithinFunction + numStridesThatMustBeDifferent) {
            lastFunctionId = ve.functionId;
            lastIndexWithinFunction = ve.indexWithinFunction;
            postprocessedClusterElements.push_back(indexInVectors);

          }
          first = false;
        }
       
        if (postprocessedClusterElements.size() >= 2) { //insert post processed data 
          for (vector<int >::const_iterator j = postprocessedClusterElements.begin(); j != postprocessedClusterElements.end(); ++j) {
            const VectorEntry& ve = vectors[*j];
            insert_into_postprocessed_clusters(con, postprocessedClusterNum, ve.functionId, ve.indexWithinFunction, ve.rowNumber, 0);
            insertedIntoPostprocessed = true;

          }
          if (postprocessedClusterNum % 2000 == 0) {
            cerr << "postprocessed cluster " << postprocessedClusterNum << " has " << postprocessedClusterElements.size() << " elements" << endl;
          }
          ++postprocessedClusterNum;
        }
        clusterNum++;
        if(insertedIntoPostprocessed) postprocessedClusterNum++;
          
        if (clusterNum %20000 == 0) 
        {
          cerr << "Inserting cluster " << clusterNum << endl;
        }

      };



    }
    trans2.commit();

  } catch(exception &ex) {
    cerr << "Exception Occured: " << ex.what() << endl;
  }


};


