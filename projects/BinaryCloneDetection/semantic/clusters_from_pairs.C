#include "rose.h"
#include "sqlite3x.h"
using namespace sqlite3x;       // sqlite3x classes are all prefixed with sqlite3 already, so the name space is somewhat redundant

static void
usage(const std::string &arg0, int exit_status)
{
    size_t slash = arg0.rfind('/');
    std::string name = slash==std::string::npos ? arg0 : arg0.substr(slash+1);
    if (0==name.substr(0, 3).compare("lt-"))
        name = name.substr(3);

    std::cerr <<"usage: " <<name <<" DATABASE PAIRS_TABLE CLUSTER_TABLE\n"
              <<"  reads the named PAIRS_TABLE from the named DATABASE and generates\n"
              <<"  a CLUSTER_TABLE (recreating it if it already existed).  The PAIRS_TABLE\n"
              <<"  should have at least two columns, the first two of which are pairs of\n"
              <<"  integers indicating a symmetric relationship between two items identified\n"
              <<"  by the two integers.  The resulting CLUSTER_TABLE will have two columns:\n"
              <<"  a zero-origin integer cluster ID number, and an item number from the\n"
              <<"  PAIRS_TABLE.\n";
    exit(exit_status);
}

typedef std::pair<int, int> Pair;               // Identifiers of two objects that are in the same cluster
typedef std::set<int> Cluster;                  // A cluster of objects that appeared as pairs
typedef std::list<Cluster> Clusters;            // The collection of all clusters

struct ClusterContains {
    int value;
    ClusterContains(int value): value(value) {}
    bool operator()(const Cluster &cluster) {
        return std::find(cluster.begin(), cluster.end(), value) != cluster.end();
    }
};

int
main(int argc, char *argv[])
{
    // Parse the command-line
    int argno;
    for (argno=1; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h") || !strcmp(argv[argno], "-?")) {
            usage(argv[0], 0);
        } else {
            std::cerr <<argv[0] <<": unknown command-line switch: " <<argv[argno] <<"\n";
            usage(argv[0], 1);
        }
    }
    if (argno+3 != argc)
        usage(argv[0], 1);
    const char* dbname        = argv[argno+0];
    std::string pairs_table   = argv[argno+1];
    std::string cluster_table = argv[argno+2];

    sqlite3_connection db(dbname);

    // Read the PAIRS_TABLE and construct clusters
    Clusters clusters;
    sqlite3_command cmd1(db, "select * from " + pairs_table);
    sqlite3_reader cursor = cmd1.executereader();
    while (cursor.read()) {
        Pair pair(cursor.getint(0), cursor.getint(1));
        Clusters::iterator c1 = std::find_if(clusters.begin(), clusters.end(), ClusterContains(pair.first));
        Clusters::iterator c2 = std::find_if(clusters.begin(), clusters.end(), ClusterContains(pair.second));
        if (c1==clusters.end() && c2==clusters.end()) {
            // neither function is already in a cluster, so create a new cluster containing only these two.
            Cluster c;
            c.insert(pair.first);
            c.insert(pair.second);
            clusters.push_back(c);
        } else if (c1!=clusters.end() && c1==c2) {
            // both functions are already in the same cluster, so do nothing
        } else if (c2==clusters.end()) {
            // first function is in a cluster, but the second is not; add second to first's cluster
            c1->insert(pair.second);
        } else if (c1==clusters.end()) {
            // second function is in a cluster, but the first is not; add first to second's cluster
            c2->insert(pair.first);
        } else {
            // functions are in two different clusters. Move all of the second cluster into the first.
            c1->insert(c2->begin(), c2->end());
            clusters.erase(c2);
        }
    }

    // Create the output table
    db.executenonquery("drop table if exists " + cluster_table);
    db.executenonquery("create table " + cluster_table + " ("
                       "  cluster_id integer,"
                       "  func_id integer)");

    // Populate the output table
    int cluster_id = 0;
    sqlite3_transaction lock(db, sqlite3_transaction::LOCK_IMMEDIATE);
    sqlite3_command cmd2(db, "insert into " + cluster_table + "(cluster_id, func_id) values (?,?)");
    for (Clusters::iterator ci=clusters.begin(); ci!=clusters.end(); ++ci, ++cluster_id) {
        for (Cluster::iterator fi=ci->begin(); fi!=ci->end(); ++fi) {
            cmd2.bind(1, cluster_id);
            cmd2.bind(2, *fi);
            cmd2.executenonquery();
        }
    }
    lock.commit();

    return 0;
}
