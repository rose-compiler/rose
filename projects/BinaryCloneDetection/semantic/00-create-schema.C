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
    struct Faults {
        SqlDatabase::StatementPtr stmt;
        Faults(const SqlDatabase::TransactionPtr &tx) {
            stmt = tx->statement("insert into semantic_faults (id, name, description) values (?, ?, ?)");
        }
        void insert(AnalysisFault::Fault fault) {
            stmt->bind(0, fault);
            stmt->bind(1, AnalysisFault::fault_name(fault));
            stmt->bind(2, AnalysisFault::fault_desc(fault));
            stmt->execute();
        }
    } faults(tx);
    faults.insert(AnalysisFault::NONE);
    faults.insert(AnalysisFault::DISASSEMBLY);
    faults.insert(AnalysisFault::INSN_LIMIT);
    faults.insert(AnalysisFault::HALT);
    faults.insert(AnalysisFault::INTERRUPT);
    faults.insert(AnalysisFault::SEMANTICS);
    faults.insert(AnalysisFault::SMTSOLVER);
    faults.insert(AnalysisFault::INPUT_LIMIT);
    faults.insert(AnalysisFault::BAD_STACK);

    // Populate the semantic_fio_events table
    struct Events {
        SqlDatabase::StatementPtr stmt;
        Events(const SqlDatabase::TransactionPtr &tx) {
            stmt = tx->statement("insert into semantic_fio_events (id, name, description) values (?, ?, ?)");
        }
        void insert(TracerEvent event, const std::string &name, const std::string &desc) {
            stmt->bind(0, event)->bind(1, name)->bind(2, desc)->execute();
        }
    } events(tx);
    events.insert(EV_REACHED,           "reached",              "this address was executed");
    events.insert(EV_BRANCHED,          "branched",             "branch taken");
    events.insert(EV_RETURNED,          "returned",             "function forced to return early");
    events.insert(EV_CONSUME_INPUT,     "consume input",        "consumed an integer value from the input group");
    events.insert(EV_MEM_WRITE,         "memory write",         "potential output value created");
    events.insert(EV_FAULT,             "fault",                "test failed; event value is the fault ID");

    // Populate the semantic_input_queues table
    struct Queues {
        SqlDatabase::StatementPtr stmt;
        Queues(const SqlDatabase::TransactionPtr &tx) {
            stmt = tx->statement("insert into semantic_input_queues (id, name, description) values (?, ?, ?)");
        }
        void insert(InputQueueName qn, const std::string &desc) {
            stmt->bind(0, (int)qn)->bind(1, InputGroup::queue_name(qn))->bind(2, desc)->execute();
        }
    } queues(tx);
    queues.insert(IQ_ARGUMENT,  "arguments of the function being analyzed");
    queues.insert(IQ_LOCAL,     "local variables appearing on the stack");
    queues.insert(IQ_GLOBAL,    "global variables");
    queues.insert(IQ_FUNCTION,  "function black box return values");
    queues.insert(IQ_POINTER,   "variables of pointer type not in lower-numbered queues");
    queues.insert(IQ_MEMHASH,   "memory not in lower-numbered queues");
    queues.insert(IQ_INTEGER,   "locations not not in lower-numbered queues");

    finish_command(tx, cmd_id);
    tx->commit();
    return 0;
}
