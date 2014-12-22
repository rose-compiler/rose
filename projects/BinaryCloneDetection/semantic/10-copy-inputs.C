// Copy inputs from one database to another

#include "sage3basic.h"
#include "CloneDetectionLib.h"

using namespace CloneDetection;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" SOURCE_DATABASE DESTINATION_DATABASE\n";
    exit(exit_status);
}

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

    // Parse switches
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
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
    if (argno+2 != argc)
        usage(1);
    std::string srcDatabaseUrl = argv[argno++];
    std::string dstDatabaseUrl = argv[argno++];

    // Open databases
    SqlDatabase::TransactionPtr srcTx = SqlDatabase::Connection::create(srcDatabaseUrl)->transaction();
    SqlDatabase::TransactionPtr dstTx = SqlDatabase::Connection::create(dstDatabaseUrl)->transaction();

    // How many input groups already exist in the destination file.  We need to remap the input group ID numbers as
    // they are copied from one database to the other.
    size_t igroupIdOffset = dstTx->statement("select coalesce(max(igroup_id), -1) + 1 from semantic_inputvalues")->execute_int();

    // Get the list of commands that have already been run.
    std::set<int64_t> cmds;
    SqlDatabase::StatementPtr cmdsQuery = dstTx->statement("select hashkey from semantic_history");
    for (SqlDatabase::Statement::iterator row=cmdsQuery->begin(); row!=cmdsQuery->end(); ++row)
        cmds.insert(row.get<int64_t>(0));

    // What do we need to copy?
    std::string toCopyConstraint = cmds.empty() ? std::string() : " where cmd not " + SqlDatabase::in(cmds);
    size_t nRowsToCopy = srcTx->statement("select count(*) from semantic_inputvalues" + toCopyConstraint)->execute_int();
    std::cerr <<argv0 <<": need to copy " <<StringUtility::plural(nRowsToCopy, "table rows") <<"\n";
    Progress progress(nRowsToCopy);

    //                                                                 0          1         2    3    4
    SqlDatabase::StatementPtr srcInputQuery = srcTx->statement("select igroup_id, queue_id, pos, val, cmd"
                                                               " from semantic_inputvalues" + toCopyConstraint);
    SqlDatabase::StatementPtr dstInputInsert = dstTx->statement("insert into semantic_inputvalues (igroup_id, queue_id,"
                                                                " pos, val, cmd) values (?, ?, ?, ?, ?)");
    //                                                                   0           1         2         3
    SqlDatabase::StatementPtr srcCommandQuery = srcTx->statement("select begin_time, end_time, notation, command"
                                                                 " from semantic_history where hashkey = ?");
    SqlDatabase::StatementPtr dstCommandInsert = dstTx->statement("insert into semantic_history (begin_time, end_time,"
                                                                  " notation, command, hashkey) values (?, ?, ?, ?, ?)");

    // Copy everything that needs to be copied
    for (SqlDatabase::Statement::iterator inputRow=srcInputQuery->begin(); inputRow!=srcInputQuery->end(); ++inputRow) {
        int64_t cmdId = inputRow.get<int64_t>(4);

        // Copy the command from the source semantic_history to the destination table if necessary
        if (cmds.find(cmdId)==cmds.end()) {
            SqlDatabase::Statement::iterator cmdRow = srcCommandQuery->bind(0, cmdId)->begin();
            assert(!cmdRow.at_eof());
            dstCommandInsert
                ->bind(0, cmdRow.get<time_t>(0))        // begin_time
                ->bind(1, cmdRow.get<time_t>(1))        // end_time
                ->bind(2, cmdRow.get<std::string>(2))   // notation
                ->bind(3, cmdRow.get<std::string>(3))   // command
                ->bind(4, cmdId)
                ->execute();
            cmds.insert(cmdId);
        }

        // Copy the row from the source semantic_inputvalues table to the destination table
        dstInputInsert
            ->bind(0, inputRow.get<int>(0) + igroupIdOffset) // igroup_id
            ->bind(1, inputRow.get<int>(1))             // queue_id
            ->bind(2, inputRow.get<int>(2))             // pos
            ->bind(3, inputRow.get<int64_t>(3))         // val
            ->bind(4, cmdId)
            ->execute();

        ++progress;
    }

    // Clean up
    dstTx->commit();
    return 0;
}

