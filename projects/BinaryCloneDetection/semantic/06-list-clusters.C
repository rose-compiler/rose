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
    Switches(): tablename("semantic_clusters") {}
    std::string tablename;
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


    SqlDatabase::StatementPtr q1 = tx->statement("select"
                                                 // 0          1        2              3          4
                                                 " cluster.id, func.id, func.entry_va, func.name, file.name"
                                                 " from "+opt.tablename+" as cluster"
                                                 " join semantic_functions as func on cluster.func_id=func.id"
                                                 " join semantic_files as file on func.specimen_id=file.id"
                                                 " order by cluster.id, func.id");

    size_t funcname_wd=9, filename_wd=9;
    for (SqlDatabase::Statement::iterator row=q1->begin(); row!=q1->end(); ++row) {
        funcname_wd = std::max(funcname_wd, row.get<std::string>(3).size());
        filename_wd = std::max(filename_wd, row.get<std::string>(4).size());
    }
    printf("Cluster ID | Function ID |   Entry VA | %-*s | %-*s\n",
           (int)funcname_wd, "Func Name", (int)filename_wd, "File Name");
    int prev_cluster_id = -1;
    for (SqlDatabase::Statement::iterator row=q1->begin(); row!=q1->end(); ++row) {
        int cluster_id = row.get<int>(0);
        int func_id = row.get<int>(1);
        rose_addr_t entry_va = row.get<rose_addr_t>(2);
        std::string funcname = row.get<std::string>(3);
        std::string filename = row.get<std::string>(4);
        if (cluster_id!=prev_cluster_id) {
            printf("-----------+-------------+------------+-%-*s-+-%-*s\n",
                   (int)funcname_wd, std::string(funcname_wd, '-').c_str(),
                   (int)filename_wd, std::string(filename_wd, '-').c_str());
            prev_cluster_id = cluster_id;
        }
        printf("%10d | %11d | %10s | %-*s | %-*s\n",
               cluster_id, func_id, StringUtility::addrToString(entry_va).c_str(),
               (int)funcname_wd, funcname.c_str(), (int)filename_wd, filename.c_str());

    }

    // no commit -- database not modified; otherwise be sure to also add CloneDetection::finish_command()
    return 0;
}
