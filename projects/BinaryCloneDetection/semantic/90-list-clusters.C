// Show semantic clusters


#include "sage3basic.h"
#include "CloneDetectionLib.h"
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE\n"
              <<"  This command lists information about similarity clusters.\n"
              <<"\n"
              <<"    --summary|--details\n"
              <<"            The --summary switch causes only summary information about clusters to be shown, while the\n"
              <<"            --details switch (the default) also lists the contents of each cluster.\n"
              <<"    --table=NAME\n"
              <<"            The name of the table containing cluster information.  This table should have at least\n"
              <<"            two columns to associate functions with clusters: an integer \"id\" cluster ID column and\n"
              <<"            an integer \"func_id\" column.  The default is to use the \"semantic_clusters\" table.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

struct Switches {
    Switches(): tablename("semantic_clusters"), show_details(true) {}
    std::string tablename;
    bool show_details;
};

int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    {
        size_t slash = argv0.rfind('/');
        argv0 = slash==std::string::npos ? argv0 : argv0.substr(slash+1);
        if (0==argv0.substr(0, 3).compare("lt-"))
            argv0 = argv0.substr(3);
    }

    Switches opt;
    int argno=1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--details")) {
            opt.show_details = true;
        } else if (!strcmp(argv[argno], "--summary") || !strcmp(argv[argno], "--summarize")) {
            opt.show_details = false;
        } else if (!strncmp(argv[argno], "--table=", 8)) {
            opt.tablename = argv[argno]+8;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+1!=argc)
        usage(0);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno++])->transaction();

    // Load the query results into memory and make some adjustments for how the table gets printed
    SqlDatabase::StatementPtr q1 = tx->statement("select"
                                                 // 0          1        2              3          4
                                                 " cluster.id, func.id, func.entry_va, func.name, file.name"
                                                 " from "+opt.tablename+" as cluster"
                                                 " join semantic_functions as func on cluster.func_id=func.id"
                                                 " join semantic_files as file on func.specimen_id=file.id"
                                                 " order by cluster.id, func.name, func.id");

    typedef SqlDatabase::Table<int, int, rose_addr_t, std::string, std::string> ClusterTable;
    ClusterTable table(q1);
    table.headers("ClusterID", "FunctionID", "EntryVA", "FuncName", "Specimen");
    table.renderers().r2 = &SqlDatabase::addr32Renderer;

    struct ClusterSeparator: SqlDatabase::PrePostRow<ClusterTable> {
        int prev_cluster_id;
        void operator()(std::ostream &out, const ClusterTable *table, size_t rownum, const std::vector<size_t> &widths) {
            int cluster_id = (*table)[rownum].v0;
            if (0==rownum) {
                prev_cluster_id = cluster_id;
            } else if (cluster_id!=prev_cluster_id) {
                table->print_rowsep(out, widths);
                prev_cluster_id = cluster_id;
            }
        }
    } clusterSeparator;
    table.prepost(&clusterSeparator, NULL);

    // Accumulate cluster statistics
    std::set<int> functions;
    size_t nclusters=0, cluster_size=0;
    std::vector<size_t> cluster_sizes;
    int prev_cluster_id = -1;
    for (size_t i=0; i<table.size(); ++i) {
        int cluster_id = table[i].v0;
        int func_id = table[i].v1;
        std::string funcname = table[i].v3;
        std::string filename = table[i].v4;
        functions.insert(func_id);
        if (cluster_id!=prev_cluster_id) {
            if (prev_cluster_id>=0) {
                cluster_sizes.push_back(cluster_size);
                ++nclusters;
            }
            prev_cluster_id = cluster_id;
            cluster_size = 0;
        }
        ++cluster_size;
    }
    if (prev_cluster_id>=0) {
        cluster_sizes.push_back(cluster_size);
        ++nclusters;
    }
    if (0==nclusters) {
        std::cout <<"No clusters\n";
        return 0;
    }
    std::sort(cluster_sizes.begin(), cluster_sizes.end());
    double average_size = 0;
    std::map<size_t, size_t> distribution;
    for (size_t i=0; i<cluster_sizes.size(); ++i) {
        average_size += cluster_sizes[i];
        ++distribution[cluster_sizes[i]];
    }
    size_t max_dist = 0;
    for (std::map<size_t, size_t>::const_iterator di=distribution.begin(); di!=distribution.end(); ++di)
        max_dist = std::max(max_dist, di->second);
    average_size /= cluster_sizes.size();
    double median_size = cluster_sizes[cluster_sizes.size()/2];
    if (0==cluster_sizes.size()%2)
        median_size = (median_size + cluster_sizes[cluster_sizes.size()/2-1]) / 2.0;
    size_t ntested_functions = tx->statement("select count(*) from"
                                             " (select distinct func_id from semantic_fio) as x")->execute_int();
    double inclusion_ratio = (double)functions.size() / ntested_functions;

    // Show cluster statistics
    std::cout <<"Number of clusters (non-singleton maximal cliques):  " <<nclusters <<"\n"
              <<"Number of unique functions across all clusters:      " <<functions.size() <<"\n"
              <<"Percent of tested functions represented in clusters: " <<100.0*inclusion_ratio <<"\n"
              <<"Size of smallest non-singleton cluster:              " <<cluster_sizes.front() <<"\n"
              <<"Size of largest cluster:                             " <<cluster_sizes.back() <<"\n"
              <<"Average cluster size:                                " <<average_size <<"\n"
              <<"Median cluster size:                                 " <<median_size <<"\n";
    printf("Cluster size distribution:\n    %7s %s\n", "Size", "Instances");
    for (std::map<size_t, size_t>::const_iterator di=distribution.begin(); di!=distribution.end(); ++di) {
        static const int width = 80;
        size_t nchars = round((double)width * di->second/max_dist);
        printf("    %7zu %9zu |%-*s|\n", di->first, di->second, width, std::string(nchars, '=').c_str());
    }

    // Show the clusters
    if (opt.show_details) {
        std::cout <<"\n";
        table.print(std::cout);
    }

    // no commit -- database not modified; otherwise be sure to also add CloneDetection::finish_command()
    return 0;
}
