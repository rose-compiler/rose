// Adds functions to the database.

#include "sage3basic.h"
#include "CloneDetectionLib.h"

#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <sstream>
#include <iomanip>

static SqlDatabase::TransactionPtr transaction;

using namespace boost;

typedef boost::adjacency_list< boost::vecS,
                               boost::vecS,
                               boost::directedS > DirectedGraph;

using namespace CloneDetection;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE SPECIMEN\n"
              <<"  This command takes data from a results database and produces graphs and tables. \n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

struct ResilienceToOptimization {
    std::string db_group;

    std::string first_group;
    std::string second_group;

    double recall;
    double specificity;
    double precision;
};

/** Create table that shows recall, precision, and specificity for pairs of optimization levels for the same specimen. */
void
create_resilience_graph(std::string db_name)
{
    SqlDatabase::StatementPtr stmt = transaction->statement("select db_group, recall, specificity, precision"
                                                            " from resilience_to_optimization ORDER BY db_group");
    std::vector<ResilienceToOptimization> rates;
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!= stmt->end(); ++row) {
        ResilienceToOptimization res;

        res.db_group = row.get<std::string>(0);

        res.first_group  = res.db_group.substr(1,2);
        res.second_group = res.db_group.substr(4,6);

        res.recall       = row.get<double>(1);
        res.specificity  = row.get<double>(2);
        res.precision    = row.get<double>(3);

        rates.push_back(res);
    }

    //overall rates - only one row should exist in the database
    SqlDatabase::StatementPtr overall_rate_stmt = transaction->statement(" select "
                                                                         " recall_min, recall_max, recall_mean,"
                                                                         " recall_standard_deviation, specificity_min,"
                                                                         " specificity_max, specificity_mean,"
                                                                         " specificity_standard_deviation, precision_min,"
                                                                         " precision_max, precision_mean,"
                                                                         " precision_standard_deviation"
                                                                         " from resilience_to_optimization_rate");

    double recall_min = 0.0,      recall_max = 0.0,      recall_mean = 0.0,      recall_standard_deviation = 0.0;
    double specificity_min = 0.0, specificity_max = 0.0, specificity_mean = 0.0, specificity_standard_deviation = 0.0;
    double precision_min = 0.0,   precision_max = 0.0,   precision_mean = 0.0,   precision_standard_deviation = 0.0;

    for (SqlDatabase::Statement::iterator row=overall_rate_stmt->begin(); row!= overall_rate_stmt->end(); ++row) {
        recall_min   = row.get<double>(0);
        recall_max   = row.get<double>(1);
        recall_mean  = row.get<double>(2);
        recall_standard_deviation = row.get<double>(3);

        specificity_min   = row.get<double>(4);
        specificity_max   = row.get<double>(5);
        specificity_mean  = row.get<double>(6);
        specificity_standard_deviation = row.get<double>(7);

        precision_min   = row.get<double>(8);
        precision_max   = row.get<double>(9);
        precision_mean  = row.get<double>(10);
        precision_standard_deviation = row.get<double>(11);
    }

    int ncol = 5;

    //create graph
    std::ofstream ofile;
    ofile.open("resilience_graph.tex");

    ofile<<"\\begin{table}"<<std::endl;
    ofile<<"\\caption{ }"<<std::endl;
    ofile<<"\\begin{tabular}{|c|r|r|r|r|}}";

    ofile << "|}" << std::endl;
    ofile << " compares & precision  & recall & specificity \\\\ " << std::endl;
    ofile << "\\hline ";

    for (int col=0; col < ncol-1; col++)
        ofile << "\t &";
    ofile << "\t\\\\\n";

    ofile << std::fixed << std::setprecision(2);
    for (std::vector<ResilienceToOptimization>::iterator it = rates.begin(); it != rates.end(); ++it) {
        ofile << "\\hline ";
        ofile << " & ";

        ofile << it->first_group;
        ofile << " vs ";
        ofile << it->second_group;
        ofile << " & ";
        ofile << it->precision;
        ofile << " & ";
        ofile << it->recall;
        ofile << " & ";
        ofile << it->specificity;

        ofile << " \\\\"<<std::endl;
    }

    ofile << "\\hline"         << std::endl;
    ofile << "\\end{tabular} " << std::endl;

    ofile << "\\caption{ ";

    { //the overall rates
        ofile << " Mean";
        ofile << " precision " << precision_mean << " standard deviation " << precision_standard_deviation;
        ofile << " ( min "     << precision_min  << " max " << precision_max << " ). ";

        ofile << " Mean";
        ofile << " specificity " << specificity_mean << " standard deviation " << specificity_standard_deviation;
        ofile << " ( min "       << specificity_min  << " max " << specificity_max << " ). ";

        ofile << " Mean";
        ofile << " recall " << recall_mean << " standard deviation " << recall_standard_deviation;
        ofile << " ( min "   << recall_min  << " max " << recall_max << " ). ";
    }

    ofile << "}" << std::endl;

    ofile << "\\end{table}"    << std::endl;

    ofile.close();
}

struct PercentSimilar {
    double similarity_middle;
    double percent;
};

/** Plot of simliarity vs how many percent of the functions has the similarity. */
void
plot_similarity_vs_frequency()
{
    SqlDatabase::StatementPtr stmt = transaction->statement("select similarity_middle, percent from aggregate_percent"
                                                            " order by similarity_middle");
    std::vector<PercentSimilar> percent_similars;
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!= stmt->end(); ++row) {
        PercentSimilar ps;
        ps.similarity_middle = row.get<double>(0)*100;
        ps.percent           = row.get<double>(1);
        percent_similars.push_back(ps);
    }

    std::ofstream ofile;
    ofile.open("similarity_vs_frequency.tex");

    ofile << std::fixed << std::setprecision(2);
    ofile << "\\begin{tikzpicture}" << std::endl;
    ofile << "   \\begin{axis}["    << std::endl;
    ofile << "      height=9cm,"    << std::endl;
    ofile << "      width=9cm,"     << std::endl;
    ofile << "      grid=major,"    << std::endl;
    ofile << "   ]"                 << std::endl;

    ofile << "   \\addplot coordinates {"        << std::endl;

    for (std::vector<PercentSimilar>::iterator it = percent_similars.begin();
         it != percent_similars.end();
         ++it) {
        ofile << "            ( " << it->similarity_middle << "," << it->percent << ")" << std::endl;
    }

    ofile << "   };"                             << std::endl;

    ofile << "   \\end{axis}"                    << std::endl;
    ofile << "\\end{tikzpicture}"                << std::endl;

    ofile.close();
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

    int argno = 1;

    std::vector<std::string> signature_components;

    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        std::cout << argv[argno] << std::endl;
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);

    std::string db_name(argv[argno++]);
    std::cout << "Connecting to db:" << db_name << std::endl;
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(db_name);
    transaction = conn->transaction();

    std::cout << "creating resilience graph" << std::endl;
    create_resilience_graph(db_name);
    plot_similarity_vs_frequency();
    std::cout << "done creating resilience graph" << std::endl;

    //transaction->commit();
    return 0;
}
