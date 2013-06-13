// (Re)populates the database with the semantic clone detection schema.  Any conflicting tables in the database are
// dropped first.

#include "sage3basic.h"
#include "CloneDetectionLib.h"

using namespace CloneDetection;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE\n"
              <<"  This command populates the database with the semantic clone detection schema by executing the commands\n"
              <<"  from the Schema.sql file.  All existing semantic data is deleted in this process.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
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

    if (argc!=2)
        usage(1);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[1])->transaction();
    time_t begin_time = time(NULL);
    tx->execute(CloneDetection::schema); // could take a long time if the database is large
    int64_t cmd_id = start_command(tx, argc, argv, "initialized schema", begin_time);

    // Populate the semantic_faults table.
    struct FaultInserter {
        FaultInserter(const SqlDatabase::TransactionPtr &tx, int id, const char *name, const char *desc) {
            SqlDatabase::StatementPtr stmt1 = tx->statement("select count(*) from semantic_faults where id = ?");
            stmt1->bind(0, id);
            if (stmt1->execute_int()==0) {
                SqlDatabase::StatementPtr stmt2 = tx->statement("insert into semantic_faults"
                                                                " (id, name, description) values (?,?,?)");
                stmt2->bind(0, id);
                stmt2->bind(1, name);
                stmt2->bind(2, desc);
                stmt2->execute();
            }
        }
    };

#define add_fault(DB, ID, ABBR, DESC) FaultInserter(DB, ID, #ABBR, DESC)
    add_fault(tx, AnalysisFault::DISASSEMBLY, DISASSEMBLY, "disassembly failed");
    add_fault(tx, AnalysisFault::INSN_LIMIT,  INSN_LIMIT,  "simulation instruction limit reached");
    add_fault(tx, AnalysisFault::HALT,        HALT,        "x86 HLT instruction executed");
    add_fault(tx, AnalysisFault::INTERRUPT,   INTERRUPT,   "interrupt or x86 INT instruction executed");
    add_fault(tx, AnalysisFault::SEMANTICS,   SEMANTICS,   "instruction semantics error");
    add_fault(tx, AnalysisFault::SMTSOLVER,   SMTSOLVER,   "SMT solver error");
#undef  add_fault

    finish_command(tx, cmd_id);
    tx->commit();
    return 0;
}
