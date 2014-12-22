#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/lexical_cast.hpp>

#include "sage3basic.h"
#include "CloneDetectionLib.h"

namespace FailureEvaluation {
    extern const char *failure_schema;
};

using namespace CloneDetection;
using namespace FailureEvaluation;
using namespace boost::accumulators;

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] \n"
              <<"This command computes the API Call similarity.\n"
              <<"\n"
              <<"  These switches control which databases get their indexes dropped:\n"
              <<"    --prefix=STRING\n"
              <<"            Prefix for databases.\n"
              <<"\n";
    exit(exit_status);
}

std::vector<std::string>
get_database_names(std::string prefix)
{
    std::vector<std::string> db_names;

    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create("postgresql:///template1");
    SqlDatabase::TransactionPtr transaction = conn->transaction();

    SqlDatabase::StatementPtr stmt = transaction->statement("select datname from pg_database where datname LIKE '" +
                                                            prefix + "%'");

    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row)
        db_names.push_back(row.get<std::string>(0));
    return db_names;
}

/** Get all database groups based on the prefix. The prefix is everything but the last 5 characters. */
std::map<std::string, std::set<std::string> >
get_database_groups(std::string prefix)
{
    std::vector<std::string> dbs = get_database_names(prefix);

    // Group databases based upon prefix
    std::map<std::string, std::set<std::string> > db_groups;
    for (unsigned int i = 0; i < dbs.size(); ++i) {
        std::string cur_db = dbs[i];
        std::string cur_db_prefix = cur_db.substr(0,cur_db.size()-5);
        std::map<std::string, std::set<std::string> >::iterator it=db_groups.find(cur_db_prefix);

        if (it == db_groups.end()) {
            std::set<std::string> new_set;
            new_set.insert(cur_db);
            db_groups[cur_db_prefix] = new_set;
        } else {
            it->second.insert(cur_db);
        }
    }
    return db_groups;
}

int main(int argc, char *argv[])
{
    std::string prefix = "as_";
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--prefix=",9)) {
            prefix = boost::lexical_cast<std::string>(argv[argno]+9);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    }

    std::cout << argno << " " << argc << std::endl;
    if (argno!=argc)
        usage(1);

    std::map<std::string, std::set<std::string> >  db_groups = get_database_groups(prefix);
    for (std::map<std::string, std::set<std::string> >::iterator it = db_groups.begin(); it != db_groups.end(); ++it) {
        for (std::set<std::string >::iterator m_it = it->second.begin(); m_it != it->second.end(); ++m_it) {
            SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create("postgresql:///"+*m_it);
            SqlDatabase::TransactionPtr transaction = conn->transaction();

            transaction->execute("drop index IF EXISTS fr_call_index;");
            //transaction->execute("drop index IF EXISTS idx_ogroups_hashkey");
            transaction->execute("drop index IF EXISTS semantic_funcsim_func1_id");
            transaction->execute("drop index IF EXISTS semantic_funcsim_func2_id");
            transaction->execute("drop index IF EXISTS semantic_funcsim_similarity");
            transaction->execute("drop index IF EXISTS tmp_called_functions_func_id");
            transaction->execute("drop index IF EXISTS fr_tmp_called_index");
            transaction->execute("drop index IF EXISTS fr_tmp_interesting_funcs_index");
            transaction->execute("drop index IF EXISTS fr_tmp_library_funcs_index");
            transaction->execute("drop index IF EXISTS fr_tmp_library_funcs_name_index");

            transaction->execute("drop table IF EXISTS tmp_called_functions");
            transaction->execute("drop table IF EXISTS tmp_tested_funcs");
            transaction->execute("drop table IF EXISTS tmp_plt_func_names ");
            transaction->execute("drop table IF EXISTS tmp_library_funcs ");
            transaction->execute("drop table IF EXISTS tmp_interesting_funcs");
            transaction->execute("drop table IF EXISTS tmp_called_functions");
            transaction->execute("drop table IF EXISTS tmp_all_called_funcs");

            transaction->commit();
        }
    }
    return 0;
}
