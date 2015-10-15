// List function instructions with source code

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "DwarfLineMapper.h"
#include <cerrno>

using namespace rose;

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE [FUNCTION]\n"
              <<"  This command lists function instructions interspersed with source code if source code is available.\n"
              <<"\n"
              <<"    --[no-]assembly\n"
              <<"            Show the assembly listing for this function.  The default is to show the listing.\n"
              <<"    --[no-]color\n"
              <<"            Emit ANSI escape codes to colorize source code. The default is to emit escape codes if standard\n"
              <<"            output is a terminal.\n"
              <<"    --quiet\n"
              <<"            Turn all output off. This is usually only useful when it is followed by switches that turn\n"
              <<"            some things back on again.\n"
              <<"    --[no-]source\n"
              <<"            Show the source code if available.  If --assembly and --source are both specified, then the\n"
              <<"            source code is listed with assembly interspersed.  The default is to show source code.\n"
              <<"    --[no-]source-names\n"
              <<"            List the names of the source files where this function was defined if that information is\n"
              <<"            available in the database. The default is to show this list.\n"
              <<"    --[no-]summary\n"
              <<"            Show summary information about the function such as its name, address, number of instructions,\n"
              <<"            etc. The default is to show this information.\n"
              <<"    --[no-]tests\n"
              <<"            Show a list of tests that were run for this function, including which input group was used,\n"
              <<"            which output group was produced, how many instructions were executed, how long the test ran,\n"
              <<"            the final status, etc.  The default is to show this information.\n"
              <<"    --trace[=IGROUP,...]\n"
              <<"    --no-trace\n"
              <<"            Annotate the listing with test trace events if they are available in the database.  If --trace\n"
              <<"            is specified with no input group ID numbers, then traces from all tests of this function are\n"
              <<"            used.  Traces for other functions that happened to call this function are not shown.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    FUNCTION\n"
              <<"            A function ID number, a function entry address, a function name, the base name of a specimen, or\n"
              <<"            an absolute name of a specimen. If FUNCTION matches more than a single function, then those\n"
              <<"            function ID numbers are listed.  If no FUNCTION is specified then a list of all functions is\n"
              <<"            presented.\n";
    exit(exit_status);
}

struct Switches {
    Switches()
        : show_summary(true), show_source_names(true), show_tests(true), show_assembly(true), show_source(true),
          show_trace(false), colorize(isatty(1)) {}
    bool show_summary, show_source_names, show_tests, show_assembly, show_source, show_trace;
    std::set<int/*igroup_id*/> traces;
    bool colorize;
} opt;

struct AssemblyCode {
    AssemblyCode(int pos, rose_addr_t addr, const std::string &assembly, int func_id=-1, const std::string &func_name="")
        : pos(pos), addr(addr), assembly(assembly), func_id(func_id), func_name(func_name) {}
    int pos; // index of instruction within function
    rose_addr_t addr;
    std::string assembly;
    int func_id;
    std::string func_name;
};


typedef std::map<rose_addr_t, AssemblyCode> Instructions;

struct Code {
    std::string source_code;
    Instructions assembly_code;
};

typedef BinaryAnalysis::DwarfLineMapper::SrcInfo SourcePosition;
typedef std::map<SourcePosition, Code> Listing;

struct OutputEventKey {
    int igroup_id;
    rose_addr_t mem_addr;
    OutputEventKey(): igroup_id(-1), mem_addr(0) {}
    OutputEventKey(int igroup_id, rose_addr_t mem_addr): igroup_id(igroup_id), mem_addr(mem_addr) {}
    bool operator<(const OutputEventKey &other) const {
        return igroup_id < other.igroup_id || (igroup_id==other.igroup_id && mem_addr<other.mem_addr);
    }
};

struct OutputEventValue {
    int pos; // event index
    unsigned value; // output value
    OutputEventValue(): pos(-1), value(0) {}
    OutputEventValue(int pos, unsigned value): pos(pos), value(value) {}
};

// Associates an input_id and memory address pair with the last output event number for that pair.  This allows us to
// determine whether an particular output event is the final write to this address.
typedef std::map<OutputEventKey, OutputEventValue> FinalOutputEvents;
FinalOutputEvents final_output_events;

// Events for a single address. */
struct Event {
    typedef std::map<int64_t/*input*/, size_t/*count*/> Inputs;
    typedef std::vector<Inputs> InputQueues; // indexed by queue number
    typedef std::map<CloneDetection::AnalysisFault::Fault, size_t/*count*/> Faults;
    typedef std::map<rose_addr_t/*target*/, size_t/*count*/> Branches;
    typedef std::vector<std::pair<OutputEventKey, OutputEventValue> > Outputs;

    Event(): nexecuted(0), nreturns(0), ninputs(0), nfaults(0), nbranches(0) {}

    int func_id;                // function for event address (not the analyzed function that emitted the event)
    size_t nexecuted;           // number of times this basic block was executed
    size_t nreturns;            // number of times the inner-most function was forced to return early
    size_t ninputs;             // number of inputs consumed here
    InputQueues inputs;         // input value distribution per queue
    size_t nfaults;             // 0 or 1 unless we've accumulated multiple tests
    Faults faults;              // fault distribution
    size_t nbranches;           // number of times we branched
    Branches branches;          // branch target distribution
    Outputs outputs;            // output values, saving only the last one for each memory address
};

typedef std::map<rose_addr_t/*va*/, Event> Events;

static int
find_function_or_exit(const SqlDatabase::TransactionPtr &tx, char *func_spec)
{
    char *rest;
    errno = 0;
    int func_id = -1;
    int func_spec_i = strtol(func_spec, &rest, 0);
    if (errno || rest==func_spec || *rest)
        func_spec_i = -1;
    if (-1==func_id && -1!=func_spec_i &&
        1==tx->statement("select count(*) from semantic_functions where id = ?")->bind(0, func_spec_i)->execute_int())
        func_id = func_spec_i;
    if (-1==func_id) {
        SqlDatabase::StatementPtr stmt1a = tx->statement("select func.id, func.entry_va, func.name, func.ninsns, file.name"
                                                         " from semantic_functions as func"
                                                         " join semantic_files as file on func.file_id = file.id"
                                                         " where entry_va = ?")->bind(0, func_spec_i);
        SqlDatabase::StatementPtr stmt1b = tx->statement("select func.id, func.entry_va, func.name, func.ninsns, file.name"
                                                         " from semantic_functions as func"
                                                         " join semantic_files as file on func.file_id = file.id"
                                                         " where func.name = ?")->bind(0, func_spec);
        SqlDatabase::StatementPtr stmt1c = tx->statement("select func.id, func.entry_va, func.name, func.ninsns, file.name"
                                                         " from semantic_functions as func"
                                                         " join semantic_files as file on func.file_id = file.id"
                                                         " where file.name like"
                                                         " '%/"+SqlDatabase::escape(func_spec, tx->driver(), false)+"'");
        SqlDatabase::Table<int, rose_addr_t, std::string, size_t, std::string> functions;
        if (func_spec_i!=-1)
            functions.insert(stmt1a);
        functions.insert(stmt1b);
        functions.insert(stmt1c);
        functions.headers("ID", "Entry VA", "Function Name", "NInsns", "Specimen Name");
        functions.renderers().r1 = &SqlDatabase::addr32Renderer;
        if (functions.empty()) {
            std::cout <<argv0 <<": no function found by ID, address, or name: " <<func_spec <<"\n";
            exit(0);
        } else if (1==functions.size()) {
            func_id = functions[0].v0;
        } else {
            std::cout <<argv0 <<": function specification is ambiguous: " <<func_spec <<"\n";
            functions.print(std::cout);
            exit(0);
        }
    }
    assert(func_id>=0);
    return func_id;
}

// Show some general info about the function
static void
show_summary(const SqlDatabase::TransactionPtr &tx, int func_id)
{
    SqlDatabase::Statement::iterator geninfo = tx->statement("select"
                                                             //  0              1          2           3
                                                             "   func.entry_va, func.name, file1.name, file2.name,"
                                                             //  4            5           6           7
                                                             "   func.ninsns, func.isize, func.dsize, func.size,"
                                                             //  8            9            10              11
                                                             "   func.digest, cmd.hashkey, cmd.begin_time, func.specimen_id,"
                                                             //  12
                                                             "   func.file_id"
                                                             " from semantic_functions as func"
                                                             " join semantic_files as file1 on func.specimen_id = file1.id"
                                                             " join semantic_files as file2 on func.file_id = file2.id"
                                                             " join semantic_history as cmd on func.cmd = cmd.hashkey"
                                                             " where func.id = ?")->bind(0, func_id)->begin();
    double returns_value = CloneDetection::function_returns_value(tx, func_id);

    std::cout <<"Function ID:                      " <<func_id <<"\n"
              <<"Entry virtual address:            " <<StringUtility::addrToString(geninfo.get<rose_addr_t>(0)) <<"\n"
              <<"Function name:                    " <<geninfo.get<std::string>(1) <<"\n"
              <<"Binary specimen name:             " <<geninfo.get<std::string>(2) <<" (id=" <<geninfo.get<int>(11) <<")\n";
    if (0!=geninfo.get<std::string>(2).compare(geninfo.get<std::string>(3)))
        std::cout <<"Binary file name:                 " <<geninfo.get<std::string>(3) <<" (id=" <<geninfo.get<int>(12) <<")\n";
    std::cout <<"Number of instructions:           " <<geninfo.get<size_t>(4) <<"\n"
              <<"Number of bytes for instructions: " <<geninfo.get<size_t>(5) <<"\n"
              <<"Number of bytes for static data:  " <<geninfo.get<size_t>(6) <<"\n"
              <<"Total number of bytes:            " <<geninfo.get<size_t>(7) <<"\n" // not necessarily the sum isize + dsize
              <<"Function returns a value:         " <<round(100.0*returns_value) <<"% probability\n"
              <<"Function static digest:           " <<geninfo.get<std::string>(8) <<"\n"
              <<"Command that inserted function:   " <<geninfo.get<int64_t>(9) <<" (command hashkey)\n"
              <<"Time that function was inserted:  " <<SqlDatabase::humanTimeRenderer(geninfo.get<time_t>(10), 0) <<"\n";


    size_t ntests = tx->statement("select count(*) from semantic_fio where func_id=?")->bind(0, func_id)->execute_int();
    if (0==ntests) {
        std::cout <<"Number of tests for function:     " <<ntests <<"\n";
    } else {
        SqlDatabase::StatementPtr stmt = tx->statement("select fault.name, count(*), 100.0*count(*)/?"
                                                       " from semantic_fio as fio"
                                                       " join semantic_faults as fault on fio.status = fault.id"
                                                       " where func_id = ?"
                                                       " group by fault.id, fault.name"
                                                       " order by fault.id")->bind(0, ntests)->bind(1, func_id);
        SqlDatabase::Table<std::string, size_t, double> statuses(stmt);
        if (statuses.size()==1) {
            std::cout <<"Number of tests for function:     " <<ntests <<" (all had status " <<statuses[0].v0 <<")\n";
        } else {
            std::cout <<"Number of tests for function:     " <<ntests <<"\n";
            statuses.headers("Status", "NTests", "Percent");
            statuses.line_prefix("    ");
            statuses.print(std::cout);
        }
    }
}

// Show the names of the source code files for this function.
static void
show_source_names(const SqlDatabase::TransactionPtr &tx, int func_id)
{
    SqlDatabase::Table<int, std::string> srcfiles(tx->statement("select distinct file.id, file.name"
                                                                " from semantic_instructions as insn"
                                                                " join semantic_files as file on insn.src_file_id = file.id"
                                                                " where insn.func_id = ?"
                                                                " order by file.name")->bind(0, func_id));
    if (1==srcfiles.size()) {
        std::cout <<"Source file name:                 " <<srcfiles[0].v1 <<" (id=" <<srcfiles[0].v0 <<")\n";
    } else if (!srcfiles.empty()) {
        std::cout <<"Number of source files:           " <<srcfiles.size() <<"\n";
        srcfiles.headers("FileID", "Name");
        srcfiles.line_prefix("    ");
        srcfiles.print(std::cout);
    }
}

// List tests that were run for this function
static void
show_tests(const SqlDatabase::TransactionPtr &tx, int func_id)
{
    SqlDatabase::Table<int, size_t, size_t, size_t, size_t, size_t, size_t, size_t, int64_t,
                       std::string, double, double, int64_t> fio;
    fio.insert(tx->statement("select"
                             "   fio.igroup_id, fio.arguments_consumed, fio.locals_consumed, fio.globals_consumed,"
                             "   fio.functions_consumed, fio.integers_consumed, fio.pointers_consumed,"
                             "   fio.instructions_executed, fio.ogroup_id,"
                             "   fault.name, fio.elapsed_time, fio.cpu_time, fio.cmd"
                             " from semantic_fio as fio"
                             " join semantic_faults as fault on fio.status = fault.id"
                             " where func_id = ?"
                             " order by igroup_id")->bind(0, func_id));
    std::cout <<"Tests run for this function:\n";
    fio.headers("IGroup", "Args", "Locals", "Globals", "Funcs", "Ints", "Ptrs", "Insns", "OGroup", "Status", "Elapsed Time",
                "CPU Time", "Command");
    fio.line_prefix("    ");
    fio.print(std::cout);
}

// Create and populate the tmp_events table.
static void
gather_events(const SqlDatabase::TransactionPtr &tx, int func_id)
{
    tx->execute("create temporary table tmp_events as select * from semantic_fio_trace limit 0");
    if (opt.show_trace) {
        std::string sql = "insert into tmp_events select * from semantic_fio_trace where func_id = ?";
        std::vector<std::string> igroups;
        for (std::set<int>::const_iterator i=opt.traces.begin(); i!=opt.traces.end(); ++i)
            igroups.push_back(StringUtility::numberToString(*i));
        if (!igroups.empty())
            sql += " and igroup_id in (" + StringUtility::join(", ", igroups) + ")";
        tx->statement(sql)->bind(0, func_id)->execute();
    }
}


// Load all events into memory.  Events are emitted for a particular function ID being analyzed, but if the 25-run-test
// --follow-calls was specified, then events for that function ID might be at instructions that are outside that function.
// We need to make note of those functions so that we can load all their instructions.
static void
load_events(const SqlDatabase::TransactionPtr &tx, int func_id, Events &events/*in,out*/)
{
    int specimen_id = tx->statement("select specimen_id from semantic_functions where id = ?")
                      ->bind(0, func_id)->execute_int();
    SqlDatabase::StatementPtr stmt = tx->statement("select"
                                                   // 0          1               2            3          4
                                                   " event.addr, event.event_id, event.minor, event.val, func.id,"
                                                   // 5               6
                                                   " event.igroup_id, event.pos"
                                                   " from tmp_events as event"
                                                   " join semantic_instructions as insn on event.addr = insn.address"
                                                   " join semantic_functions as func on insn.func_id = func.id"
                                                   " where func.specimen_id = ?"
                                                   " order by igroup_id, pos");
    stmt->bind(0, specimen_id);
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        rose_addr_t addr = row.get<rose_addr_t>(0);
        int event_id = row.get<int>(1);
        int minor = row.get<int>(2);
        int64_t val = row.get<int64_t>(3);
        events[addr].func_id = row.get<int>(4); // the hard-to-get ID, not the one stored in the events func_id column.
        int igroup_id = row.get<int>(5);
        int pos = row.get<int>(6);
        switch (event_id) {
            case CloneDetection::EV_REACHED: {
                ++events[addr].nexecuted;
                break;
            }
            case CloneDetection::EV_BRANCHED: {
                ++events[addr].nbranches;
                ++events[addr].branches[val];
                break;
            }
            case CloneDetection::EV_RETURNED: {
                ++events[addr].nreturns;
                break;
            }
            case CloneDetection::EV_CONSUME_INPUT: {
                ++events[addr].ninputs;
                assert(minor>=0);
                if ((size_t)minor>=events[addr].inputs.size())
                    events[addr].inputs.resize(minor+1);
                ++events[addr].inputs[minor][val];
                break;
            }
            case CloneDetection::EV_FAULT: {
                CloneDetection::AnalysisFault::Fault fault = (CloneDetection::AnalysisFault::Fault)minor;
                ++events[addr].nfaults;
                ++events[addr].faults[fault];
                break;
            }
            case CloneDetection::EV_MEM_WRITE: {
                OutputEventKey output_key(igroup_id, val);
                OutputEventValue output_val(pos, minor);
                // Track final writes to each address
                final_output_events[output_key] = output_val;
                // Append event to the appropriate instruction
                events[addr].outputs.push_back(std::make_pair(output_key, output_val));
            }
            default:
                /*void*/
                break;
        }
    }
}

// Create the tmp_insns table to hold all the instructions for the function-to-be-listed and all the instructions of all
// the functions that are mentioned in events.
static void
gather_instructions(const SqlDatabase::TransactionPtr tx, int func_id, const Events &events)
{
    std::set<std::string> func_ids;
    func_ids.insert(StringUtility::numberToString(func_id));
    for (Events::const_iterator ei=events.begin(); ei!=events.end(); ++ei)
        func_ids.insert(StringUtility::numberToString(ei->second.func_id));
    std::string sql = "create temporary table tmp_insns as"
                      " select * from semantic_instructions"
                      " where func_id in ("+StringUtility::join_range(", ", func_ids.begin(), func_ids.end())+")";
    tx->execute(sql);
}

static void
show_events(const Event &e)
{
    // Subsequent lines for early returns
    if (e.nreturns>0) {
        std::cout <<std::setw(11) <<std::right <<"RET " <<"|   forced early return "
                  <<e.nreturns <<" time" <<(1==e.nreturns?"":"s") <<"\n";
    }

    // Subsequent lines for inputs consumed
    if (e.ninputs>0) {
        std::cout <<std::setw(9) <<std::right <<e.ninputs <<"< |   ";
        size_t nlines=0;
        for (size_t q=0; q<e.inputs.size(); ++q) {
            if (e.inputs[q].empty())
                continue;
            if (0!=nlines)
                std::cout <<std::string(11, ' ') <<"|   ";
            std::cout <<CloneDetection::InputGroup::queue_name((CloneDetection::InputQueueName)q) <<" = {";
            size_t nvals = 0, col=0;
            for (Event::Inputs::const_iterator ii=e.inputs[q].begin(); ii!=e.inputs[q].end(); ++ii, ++nvals) {
                if (nvals++) {
                    std::cout <<", ";
                    col += 2;
                }
                if (col>90) {
                    std::cout <<"\n" <<std::string(11, ' ') <<"|             ";
                    col = 0;
                    ++nlines;
                }
                std::ostringstream ss;
                int64_t val = ii->first;
                size_t count = ii->second;
                if (val<=4096 && val>-4096) {
                    ss <<val;
                } else {
                    ss <<StringUtility::addrToString((uint64_t)val);
                }
                if (count>1)
                    ss <<"(" <<count <<"x)";
                std::cout <<ss.str();
                col += ss.str().size();
            }
            std::cout <<"}\n";
            ++nlines;
        }
    }

    // Subsequent lines for outputs produced
    if (!e.outputs.empty()) {
        std::cout <<std::setw(9) <<std::right <<e.outputs.size() <<"> |   ";
        int prev_igroup_id = -1;
        size_t col = 0;
        for (Event::Outputs::const_iterator oi=e.outputs.begin(); oi!=e.outputs.end(); ++oi) {
            const OutputEventKey &output_key = oi->first;
            const OutputEventValue &output_val = oi->second;
            std::ostringstream ss;
            if (output_key.igroup_id!=prev_igroup_id) {
                if (1!=opt.traces.size() && oi!=e.outputs.begin()) {
                    std::cout <<" }";
                    col += 2;
                }
                if (1==opt.traces.size()) {
                    ss <<" outputs:";
                } else {
                    ss <<" test " <<output_key.igroup_id <<" outputs:";
                }
                prev_igroup_id = output_key.igroup_id;
            }
            BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::ValueType<32> val(output_val.value);
            ss <<" mem[" <<StringUtility::addrToString(output_key.mem_addr) <<"]=" <<val;
            if (output_val.pos==final_output_events[output_key].pos)
                ss <<" (final)";
            if (col > 90) {
                std::cout <<"\n" <<std::string(11, ' ') <<"|   ";
                col = 0;
            }
            std::cout <<ss.str();
            col += ss.str().size();
        }
        std::cout <<"\n";
    }

    // Subsequent lines for control flow branches
    if (e.nbranches>0) {
        size_t col=0;
        std::cout <<std::setw(10) <<std::right <<"BR" <<" |  ";
        std::string s = " branch taken " + StringUtility::numberToString(e.nbranches) +
                        " time" + (1==e.nbranches?"":"s") + ":";
        std::cout <<s;
        col = s.size();
        for (Event::Branches::const_iterator bi=e.branches.begin(); bi!=e.branches.end(); ++bi) {
            if (col>100) {
                std::cout <<"\n" <<std::string(11, ' ') <<"|  ";
                col = 0;
            }
            std::ostringstream ss;
            ss <<" " <<StringUtility::addrToString(bi->first);
            if (bi->second!=e.nbranches)
                ss <<"(" <<bi->second <<"x)";
            std::cout <<ss.str();
            col += ss.str().size();
        }
        std::cout <<"\n";
    };

    // Subsequent lines for faults encountered
    if (e.nfaults>0) {
        std::cout <<std::setw(10) <<std::left <<"FAULT" <<" |  ";
        for (Event::Faults::const_iterator fi=e.faults.begin(); fi!=e.faults.end(); ++fi) {
            std::cout <<" " <<CloneDetection::AnalysisFault::fault_name(fi->first);
            if (e.nfaults>1)
                std::cout <<" (" <<fi->second <<"x)";
        }
        std::cout <<"\n";
    }
}

static void
list_assembly(const SqlDatabase::TransactionPtr &tx, int func_id)
{
    Events events;
    gather_events(tx, func_id);
    load_events(tx, func_id, events);

    SqlDatabase::StatementPtr stmt = tx->statement("select address, assembly from semantic_instructions where func_id = ?"
                                                   " order by position")->bind(0, func_id);
    for (SqlDatabase::Statement::iterator insn=stmt->begin(); insn!=stmt->end(); ++insn) {
        rose_addr_t addr = insn.get<rose_addr_t>(0);
        std::string assembly = insn.get<std::string>(1);
        Events::const_iterator ei=events.find(addr);

        // Assembly line prefix
        if (ei!=events.end() && ei->second.nexecuted>0) {
            std::cout <<std::setw(9) <<std::right <<ei->second.nexecuted <<"x ";
        } else {
            std::cout <<std::string(11, ' ');
        }

        // Assembly instruction
        std::cout <<"| " <<StringUtility::addrToString(addr) <<":  " <<assembly <<"\n";

        if (ei!=events.end())
            show_events(ei->second);
    }
}

static void
gather_source_code(const SqlDatabase::TransactionPtr &tx)
{
    tx->execute("create temporary table tmp_src as"
                "  select distinct src.*"
                "    from tmp_insns as insn"
                "    join semantic_sources as src"
                "      on insn.src_file_id=src.file_id"
                "      and src.linenum >= insn.src_line-10"
                "      and src.linenum <= insn.src_line+10");
}

static void
load_source_code(const SqlDatabase::TransactionPtr &tx, Listing &listing/*in,out*/)
{
    SqlDatabase::StatementPtr stmt = tx->statement("select file_id, linenum, line from tmp_src");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
        int file_id = row.get<int>(0);
        int linenum = row.get<int>(1);
        SourcePosition srcpos(file_id, linenum);
        listing[srcpos].source_code = row.get<std::string>(2);
    }
}



static void
list_combined(const SqlDatabase::TransactionPtr &tx, int func_id, bool show_assembly)
{
    CloneDetection::FilesTable files(tx);

    Events events;
    gather_events(tx, func_id);
    load_events(tx, func_id, events/*out*/);
    gather_instructions(tx, func_id, events);

    Listing listing;
    gather_source_code(tx);
    load_source_code(tx, listing/*out*/);

    // Get lines of assembly code and insert them into the correct place in the Listing.
    if (show_assembly) {
        SqlDatabase::StatementPtr stmt = tx->statement("select"
                                                       // 0                1              2              3
                                                       " insn.src_file_id, insn.src_line, insn.position, insn.address,"
                                                       // 4             5        6
                                                       " insn.assembly, func.id, func.name"
                                                       " from tmp_insns as insn"
                                                       " join semantic_functions as func on insn.func_id = func.id"
                                                       " order by position");
        for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row) {
            int src_file_id = row.get<int>(0);
            int src_line_num = row.get<int>(1);
            SourcePosition srcpos(src_file_id, src_line_num);
            int pos = row.get<int>(2);
            rose_addr_t addr = row.get<rose_addr_t>(3);
            std::string assembly = row.get<std::string>(4);
            int func_id = row.get<int>(5);
            std::string func_name = row.get<std::string>(6);
            listing[srcpos].assembly_code.insert(std::make_pair(addr, AssemblyCode(pos, addr, assembly, func_id, func_name)));
        }

        // Listing header
        std::cout <<"WARNING: This listing should be read cautiously. It is ordered according to the\n"
                  <<"         source code with assembly lines following the source code line from which\n"
                  <<"         they came.  However, the compiler does not always generate machine\n"
                  <<"         instructions in the same order as source code.  When a discontinuity\n"
                  <<"         occurs in the assembly instruction listing, it will be marked by a \"#\"\n"
                  <<"         character.  The assembly instructions are also numbered according to\n"
                  <<"         their relative positions in the binary function.\n"
                  <<"\n"
                  <<"         The prefix area contains either source location information or test trace\n"
                  <<"         information.  Note that trace information might be incomplete because\n"
                  <<"         tracing was disabled or only partially enabled, or the trace includes\n"
                  <<"         instructions that are not present in this function listing (e.g., when\n"
                  <<"         execution follows a CALL instruction). The following notes are possible:\n"
                  <<"           * \"Nx\" where N is an integer indicates that this instruction\n"
                  <<"             was reached N times during testing.  These notes are typically\n"
                  <<"             only attached to the first instruction of a basic block and only\n"
                  <<"             if the trace contains EV_REACHED events.  Lack of an Nx notation\n"
                  <<"             doesn't necessarily mean that the basic block was not reached, it\n"
                  <<"             only means that there is no EV_REACHED event for that block.\n"
                  <<"           * \"N<\" where N is an integer indicates that the instruction\n"
                  <<"             on the previous line consumed N inputs. Information about the\n"
                  <<"             inputs is listed on the right side of this line.\n"
                  <<"           * \"N>\" where N is an integer indicates that the instruction\n"
                  <<"             on the previous line produced N memory outputs. Information about the\n"
                  <<"             outputs is listed on the right side of this line. Only the final\n"
                  <<"             write to a memory address is considered a true output, and such\n"
                  <<"             writes will be marked with the string \"final\".\n"
                  <<"           * \"BR\" indicates that the instruction on the previous line is a\n"
                  <<"             control flow branch point. The right side of the line shows more\n"
                  <<"             detailed information about how many times the branch was taken.\n"
                  <<"           * \"FAULT\" indicates that the test was terminated at the previous\n"
                  <<"             instruction. The right side of the line shows the distribution of\n"
                  <<"             faults that occurred here.\n"
                  <<"\n"
                  <<"                /------------- Prefix area\n"
                  <<" /-------------/-------------- Source file ID or assembly function ID\n"
                  <<" |     /------/--------------- Source line number or assembly instruction index\n"
                  <<" |     |   /-/---------------- Instruction out-of-order indicator\n"
                  <<" |     |   |/     /----------- Instruction virtual address\n"
                  <<" |     |   |      |\n"
                  <<"vvvv vvvvv/|      |\n"
                  <<"vvvvvvvvvv v vvvvvvvvvv\n";
    }

    // Show the listing
    int prev_func_id = -1, prev_position = -1;
    std::set<int> seen_files;
    for (Listing::iterator li=listing.begin(); li!=listing.end(); ++li) {
        int file_id = li->first.file_id;
        if (seen_files.insert(file_id).second) {
            if (file_id>=0) {
                std::cout <<"\n" <<std::setw(4) <<std::right <<file_id <<".file  |"
                          <<(opt.colorize?"\033[33;4m":"") <<files.name(file_id) <<(opt.colorize?"\033[m":"") <<"\n";
            } else {
                std::cout <<"\n" <<std::string(11, ' ') <<"|"
                          <<(opt.colorize?"\033[33;4m":"") <<"instructions not associated with a source file"
                          <<(opt.colorize?"\033[m":"") <<"\n";
            }
        }
        if (file_id>=0) {
            std::cout <<std::setw(4) <<std::right <<file_id <<"." <<std::setw(6) <<std::left <<li->first.line_num
                      <<"|"
                      <<(opt.colorize?"\033[34m":"")
                      <<StringUtility::untab(li->second.source_code)
                      <<(opt.colorize?"\033[m":"") <<"\n";
        }

        for (Instructions::iterator ii=li->second.assembly_code.begin(); ii!=li->second.assembly_code.end(); ++ii) {
            const AssemblyCode assm = ii->second;
            if (assm.func_id!=prev_func_id) {
                std::cout <<std::string(11, ' ') <<"# "
                          <<(opt.colorize?"\033[33;4m":"") <<"function " <<StringUtility::numberToString(assm.func_id);
                if (!assm.func_name.empty())
                    std::cout <<" <" <<assm.func_name <<">";
                std::cout <<(opt.colorize?"\033[m":"") <<"\n";
            }

            Events::const_iterator ei=events.find(assm.addr);
            std::cout <<std::setw(4) <<std::right <<assm.func_id <<"." <<std::setw(6) <<std::left <<assm.pos
                      <<(prev_func_id==assm.func_id && prev_position+1==assm.pos ? "|" : "#");

            if (ei!=events.end() && ei->second.nexecuted>0) {
                std::cout <<std::setw(9) <<std::right <<ei->second.nexecuted <<"x ";
            } else {
                std::cout <<std::string(11, ' ');
            }

            std::cout <<StringUtility::addrToString(assm.addr) <<":  "
                      <<(opt.colorize?"\033[32m":"") <<assm.assembly <<(opt.colorize?"\033[m":"") <<"\n";

            if (ei!=events.end())
                show_events(ei->second);

            prev_func_id = assm.func_id;
            prev_position = assm.pos;
        }
    }
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

    int argno=1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            ::usage(0);
        } else if (!strcmp(argv[argno], "--assembly")) {
            opt.show_assembly = true;
        } else if (!strcmp(argv[argno], "--no-assembly")) {
            opt.show_assembly = false;
        } else if (!strcmp(argv[argno], "--color")) {
            opt.colorize = true;
        } else if (!strcmp(argv[argno], "--no-color")) {
            opt.colorize = false;
        } else if (!strcmp(argv[argno], "--quiet")) {
            opt.show_summary = opt.show_source_names = opt.show_tests = opt.show_assembly = false;
            opt.show_source = opt.show_trace = false;
        } else if (!strcmp(argv[argno], "--source")) {
            opt.show_source = true;
        } else if (!strcmp(argv[argno], "--no-source")) {
            opt.show_source = false;
        } else if (!strcmp(argv[argno], "--source-names")) {
            opt.show_source_names = true;
        } else if (!strcmp(argv[argno], "--no-source-names")) {
            opt.show_source_names = false;
        } else if (!strcmp(argv[argno], "--summary")) {
            opt.show_summary = true;
        } else if (!strcmp(argv[argno], "--no-summary")) {
            opt.show_summary = false;
        } else if (!strcmp(argv[argno], "--tests")) {
            opt.show_tests = true;
        } else if (!strcmp(argv[argno], "--trace")) {
            opt.show_trace = true;
        } else if (!strncmp(argv[argno], "--trace=", 8)) {
            std::vector<std::string> id_strings = StringUtility::split(',', argv[argno]+8, (size_t)-1, true);
            for (size_t i=0; i<id_strings.size(); ++i) {
                const char *s = id_strings[i].c_str();
                char *rest;
                errno = 0;
                int id = strtol(s, &rest, 0);
                if (errno || rest==s) {
                    std::cerr <<argv0 <<": invalid test ID for --trace switch: " <<s <<"\n";
                    exit(1);
                }
                while (isspace(*rest)) ++rest;
                if (*rest) {
                    std::cerr <<argv0 <<": invalid test ID for --trace switch: " <<s <<"\n";
                    exit(1);
                }
                opt.traces.insert(id);
            }
            opt.show_trace = true;
        } else if (!strcmp(argv[argno], "--no-trace")) {
            opt.show_trace = false;
        } else if (!strcmp(argv[argno], "--no-tests")) {
            opt.show_tests = false;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+1!=argc && argno+2!=argc)
        ::usage(0);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno++])->transaction();
    char *func_spec = argno<argc ? argv[argno++] : NULL;

    // List all functions?
    if (!func_spec) {
        SqlDatabase::Table<int, rose_addr_t, std::string, size_t, std::string>
            functions(tx->statement("select func.id, func.entry_va, func.name, func.ninsns, file.name"
                                    " from semantic_functions as func"
                                    " join semantic_files as file on func.specimen_id = file.id"));
        functions.headers("ID", "Entry VA", "Function Name", "NInsns", "Specimen Name");
        functions.renderers().r1 = &SqlDatabase::addr32Renderer;
        size_t nrows = getenv("LINES") ? strtoul(getenv("LINES"), NULL, 0)-4 : 100;
        nrows = std::max((size_t)20, std::min((size_t)1000, nrows));
        functions.reprint_headers(nrows);
        functions.print(std::cout);
        return 0;
    }

    // Figure out the unique function ID from the specification on the command line, if possible
    int func_id = find_function_or_exit(tx, func_spec);

    bool had_output = false;
    if (opt.show_summary) {
        show_summary(tx, func_id);
        had_output = true;
    }
    if (opt.show_source_names) {
        show_source_names(tx, func_id);
        had_output = true;
    }
    if (opt.show_tests) {
        std::cout <<(had_output?"\n":"");
        show_tests(tx, func_id);
        had_output = true;
    }

    if (opt.show_source) {
        std::cout <<(had_output?"\n":"");
        list_combined(tx, func_id, opt.show_assembly);
    } else if (opt.show_assembly) {
        std::cout <<(had_output?"\n":"");
        list_assembly(tx, func_id);
    }

    // no commit -- database not modified; otherwise be sure to also add CloneDetection::finish_command()
    return 0;
}
