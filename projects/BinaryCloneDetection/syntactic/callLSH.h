#ifndef DEF_OPEERATE_ON_CLUSTERS
#define DEF_OPEERATE_ON_CLUSTERS

#include "rose.h"
#include "SqlDatabase.h"

class OperateOnClusters {
    SqlDatabase::TransactionPtr tx;
    std::string databaseName;
    int norm;
    double similarity;

    void read_file(std::string);

    //hash function size
    size_t k;
    //hash table count
    size_t l;

public:
    OperateOnClusters(const std::string& database, int p, double s, size_t hash_function_size , size_t hash_table_count);
    void analyzeClusters();
};

#endif
