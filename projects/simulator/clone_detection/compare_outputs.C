// Compares output groups from functions using some criteria other than exact match.
//
// Reads the semantic_outputvalue table to find matching output groups, then modifies the effective_outputgroup column
// of the semantic_fio table so that two functions whose outputs matched according to the criteria herein have the same
// output group ID in that column.

#include "CloneDetectionOutputs.h"
#include "CloneDetectionProgress.h"
#include "sqlite3x.h"

#include <cstring>
#include <iostream>
#include <set>

using namespace sqlite3x;
using namespace CloneDetection;

static std::string argv0;

static void
usage(int exit_status, const std::string &mesg="")
{
    if (!mesg.empty())
        std::cerr <<argv0 <<": " <<mesg <<"\n";
    std::cerr <<"usage: " <<argv0 <<" DATABASE_NAME\n"
              <<"    DATABASE_NAME is the name of the SQLite3 database\n";
    exit(exit_status);
}

static bool
equal(const OutputGroup &a, const OutputGroup &b)
{
    // Both must have terminated the same way in order to be equal
    if (a.fault!=b.fault)
        return false;

    // Treat output values as sets
    std::set<uint32_t> a_vals(a.values.begin(), a.values.end());
    std::set<uint32_t> b_vals(b.values.begin(), b.values.end());
    if (a_vals.size()!=b_vals.size() || !std::equal(a_vals.begin(), a_vals.end(), b_vals.begin()))
        return false;

    // Don't worry about function calls or syscalls
    return true;
}

int
main(int argc, char *argv[])
{
    std::ios::sync_with_stdio();
    argv0 = argv[0];
    size_t slash = argv0.rfind('/');
    if (slash!=std::string::npos)
        argv0 = argv0.substr(slash+1);
    if (0==argv0.substr(0, 3).compare("lt-"))
        argv0 = argv0.substr(3);

    int argno;
    for (argno=1; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h") || !strcmp(argv[argno], "-?")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else {
            usage(1, std::string("unknown switch: ")+argv[argno]);
        }
    }
    if (argno+1!=argc)
        usage(1);
    std::string dbname = argv[argno++];

    sqlite3_connection db(dbname.c_str());
    OutputGroups output_groups;
    load_output_groups(db, NULL, output_groups/*out*/);
    Progress progress1((output_groups.size()-1)*(output_groups.size()-2)/2);

    // Compare output groups and build an equality map using transitivity property to lowest numbered output
    std::map<int, int> eqmap;
    std::vector<bool> avoid(output_groups.size(), false);
    for (size_t i=0; i<output_groups.size(); ++i) {
        for (size_t j=i+1; j<output_groups.size(); ++j) {
            progress1.show();
            if (!avoid[j] && equal(output_groups[i], output_groups[j])) {
                eqmap[j] = i;
                avoid[j] = true;
            }
        }
    }
    progress1.clear();

    // Rewrite the values of the semantic_fio table.  This is going to be a bit convoluted since SQLite3 doesn't
    // support column renaming.  We also have to be explicit about the columns that already exist, rather than
    // generically copying them.
    std::cerr <<"saving new table...\n";
    sqlite3_transaction lock(db, sqlite3_transaction::LOCK_IMMEDIATE);
    Progress progress2(db.executeint("select count(*) from semantic_fio"));
    db.executenonquery("drop table if exists compare_outputs_tmp");
    db.executenonquery("create table compare_outputs_tmp as select * from semantic_fio limit 0");
    sqlite3_command cmd1(db,
                         "insert into compare_outputs_tmp"
                         // 1        2              3                  4                     5
                         " (func_id, inputgroup_id, pointers_consumed, nonpointers_consumed, instructions_executed,"
                         // 6                     7                   8             9
                         " actual_outputgroup, effective_outputgroup, elapsed_time, cpu_time)"
                         " values (?,?,?,?,?,?,?,?,?)");
    sqlite3_command cmd2(db, "select * from semantic_fio");     // these two lines must be two separate C++ statements
    sqlite3_reader c2 = cmd2.executereader();                   // otherwise sqlite3x seg faults.
    while (c2.read()) {
        progress2.show();
        cmd1.bind(1, c2.getint(0)); // bind is 1-origin; get*() are zero-origin; what a stupid API
        cmd1.bind(2, c2.getint(1));
        cmd1.bind(3, c2.getint(2));
        cmd1.bind(4, c2.getint(3));
        cmd1.bind(5, c2.getint(4));
        cmd1.bind(6, c2.getint(5));
        std::map<int, int>::iterator found = eqmap.find(c2.getint(6));
        if (found==eqmap.end()) {
            cmd1.bind(7, c2.getint(6));
        } else {
            cmd1.bind(7, found->second);
        }
        cmd1.bind(8, c2.getdouble(7));
        cmd1.bind(9, c2.getdouble(8));
        cmd1.executenonquery();
    }
    progress2.clear();
    std::cerr <<"renaming table...\n";
    db.executenonquery("drop table if exists semantic_fio_old");
    db.executenonquery("alter table semantic_fio rename to semantic_fio_old");
    db.executenonquery("alter table compare_outputs_tmp rename to semantic_fio");
    lock.commit();

    return 0;
}
