#include "findExactClones.h"

void
read_vector_data(const SqlDatabase::TransactionPtr &tx, scoped_array_with_size<VectorEntry>& vectors,
                 const std::vector<int> function_to_look_for, map<string, std::vector<int> >& internTable,
                 const int groupLow, const int groupHigh, bool useCounts)
{
    size_t eltCount = 0;
    string findNumElement = function_to_look_for.size() > 0
                            ? "select count(*) from vectors where instr_seq_b64 in"
                            " (select distinct instr_seq_b64 from vectors where function_id in"
                            " (select function_id from function_to_look_for) ) "
                            " order by id"
                            : ( groupHigh == -1
                                ? "select count(*) from vectors where sum_of_counts >= ?"
                                : "select count(*) from vectors where sum_of_counts >= ? and sum_of_counts <= ?");
    std::cout << "findNumElements " << findNumElement << " low  " << groupLow << " high " << groupHigh << std::endl;
    SqlDatabase::StatementPtr cmd1 = tx->statement(findNumElement);
    if (function_to_look_for.size() == 0) {
        cmd1->bind(0, groupLow);
        if (groupHigh != -1)
            cmd1->bind(1, groupHigh);
    }
    eltCount = cmd1->execute_int();
    if (eltCount == 0) {
        cerr << "No vectors found -- invalid database?" << endl;
        exit (1);
    }

    cerr << "Found " << eltCount << " vectors" << endl;
    vectors.allocate(eltCount);
    std::list<string> instSeq;
    string findVectorsToCheck = function_to_look_for.size() > 0
                                ? "select id, function_id, index_within_function, line, last_insn_va, counts_b64,"
                                " instr_seq_b64 from vectors where instr_seq_b64 in"
                                " (select distinct instr_seq_b64 from vectors"
                                " where function_id in (select function_id from function_to_look_for))"
                                " order by id"
                                : ( groupHigh == -1
                                    ? "select id, function_id, index_within_function, line, last_insn_va, counts_b64,"
                                    " instr_seq_b64  from vectors where sum_of_counts >= ?"
                                    : "select id, function_id, index_within_function, line, last_insn_va, counts_b64,"
                                    " instr_seq_b64 from vectors where sum_of_counts >= ? and sum_of_counts <= ?");
    SqlDatabase::StatementPtr cmd2 = tx->statement(findVectorsToCheck);
    if (function_to_look_for.size() == 0) {
        cmd2->bind(0, groupLow);
        if (groupHigh != -1)
            cmd2->bind(1, groupHigh);
    }
    size_t indexInVectors=0;
    for (SqlDatabase::Statement::iterator r=cmd2->begin(); r!=cmd2->end(); ++r) {
        long long rowNumber = r.get<int64_t>(0);
        int functionId = r.get<int>(1);
        int indexWithinFunction = r.get<int>(2);
        long long line = r.get<int64_t>(3);
        int offset = r.get<int>(4);
        std::vector<uint8_t> counts = StringUtility::decode_base64(r.get<std::string>(5));
        string compressedCounts(&counts[0], &counts[0]+counts.size());
        std::vector<uint8_t> md5 = StringUtility::decode_base64(r.get<std::string>(6));
        string instrSeqMD5(&md5[0], &md5[0]+md5.size());
        if (rowNumber <= 0 || indexInVectors > eltCount) {
            cerr << "Row number in database is out of range" << endl;
            abort();
        }
        std::string index = useCounts ? compressedCounts : instrSeqMD5;
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

        if (rowNumber % 100000 == 0)
            cerr << "Got row " << rowNumber << endl;
    }
}

void
find_exact_clones(const SqlDatabase::TransactionPtr &tx, int stride, int windowSize,
                  std::vector<int>& functionsThatWeAreInterestedIn, const size_t numStridesThatMustBeDifferent,
                  scoped_array_with_size<VectorEntry>& vectors, map<string, std::vector<int> >& internTable, bool useCounts)
{
    size_t clusterNum = 0, postprocessedClusterNum = 0;
    for (map<string, std::vector<int> >::iterator iItr = internTable.begin(); iItr != internTable.end(); ++iItr) {
        if (iItr->second.size() > 1) {
            //Postprocessed-clusters is the interesting cluster representation for the exact clone detection
            for (size_t i =0; i < iItr->second.size() ; i++) {
                int indexInVectors = iItr->second[i];
                bool interestedInCluster = false;
                VectorEntry& ve = vectors[indexInVectors];

                if (functionsThatWeAreInterestedIn.size()>0 && (interestedInCluster == false))
                    continue;
                insert_into_clusters(tx, clusterNum, ve.functionId, ve.indexWithinFunction, ve.rowNumber,0);
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
                if (first || ve.functionId != lastFunctionId ||
                    ve.indexWithinFunction >= lastIndexWithinFunction + numStridesThatMustBeDifferent) {
                    lastFunctionId = ve.functionId;
                    lastIndexWithinFunction = ve.indexWithinFunction;
                    postprocessedClusterElements.push_back(indexInVectors);
                }
                first = false;
            }

            if (postprocessedClusterElements.size() >= 2) { //insert post processed data 
                for (vector<int >::const_iterator j = postprocessedClusterElements.begin();
                     j != postprocessedClusterElements.end();
                     ++j) {
                    const VectorEntry& ve = vectors[*j];
                    insert_into_postprocessed_clusters(tx, postprocessedClusterNum, ve.functionId, ve.indexWithinFunction,
                                                       ve.rowNumber, 0);
                    insertedIntoPostprocessed = true;
                }
                if (postprocessedClusterNum % 2000 == 0) {
                    cerr << "postprocessed cluster " << postprocessedClusterNum << " has "
                         << postprocessedClusterElements.size() << " elements" << endl;
                }
                ++postprocessedClusterNum;
            }
            clusterNum++;
            if (insertedIntoPostprocessed)
                postprocessedClusterNum++;
            if (clusterNum %20000 == 0)
                cerr << "Inserting cluster " << clusterNum << endl;
        }
    }
}
