// List function instructions with source code

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "DwarfLineMapper.h"
#include <cerrno>

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE [FUNCTION]\n"
              <<"  This command lists function instructions interspersed with source code if source code is available.\n"
              <<"\n"
              <<"    --[no-]assembly\n"
              <<"            Show the assembly listing for this function.  The default is to show the listing.\n"
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
    Switches(): show_summary(true), show_source_names(true), show_tests(true), show_assembly(true), show_source(true) {}
    bool show_summary, show_source_names, show_tests, show_assembly, show_source;
};

typedef std::map<int/*index*/, std::string/*assembly*/> Instructions;

struct Code {
    std::string source_code;
    Instructions assembly_code;
};

typedef BinaryAnalysis::DwarfLineMapper::SrcInfo SourcePosition;
typedef std::map<SourcePosition, Code> Listing;

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
                                                             //  8            9            10
                                                             "   func.digest, cmd.hashkey, cmd.begin_time"
                                                             " from semantic_functions as func"
                                                             " join semantic_files as file1 on func.specimen_id = file1.id"
                                                             " join semantic_files as file2 on func.file_id = file2.id"
                                                             " join semantic_history as cmd on func.cmd = cmd.hashkey"
                                                             " where func.id = ?")->bind(0, func_id)->begin();
    std::cout <<"Function ID:                      " <<func_id <<"\n"
              <<"Entry virtual address:            " <<StringUtility::addrToString(geninfo.get<rose_addr_t>(0)) <<"\n"
              <<"Function name:                    " <<geninfo.get<std::string>(1) <<"\n"
              <<"Binary specimen name:             " <<geninfo.get<std::string>(2) <<"\n";
    if (0!=geninfo.get<std::string>(2).compare(geninfo.get<std::string>(3)))
        std::cout <<"Binary file name:                 " <<geninfo.get<std::string>(3) <<"\n";
    std::cout <<"Number of instructions:           " <<geninfo.get<size_t>(4) <<"\n"
              <<"Number of bytes for instructions: " <<geninfo.get<size_t>(5) <<"\n"
              <<"Number of bytes for static data:  " <<geninfo.get<size_t>(6) <<"\n"
              <<"Total number of bytes:            " <<geninfo.get<size_t>(7) <<"\n" // not necessarily the sum isize + dsize
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
    SqlDatabase::Table<int, size_t, size_t, size_t, int64_t, std::string, double, double, int64_t> fio;
    fio.insert(tx->statement("select"
                             " fio.igroup_id, fio.integers_consumed, fio.pointers_consumed,"
                             " fio.instructions_executed, fio.ogroup_id, fault.name, fio.elapsed_time, fio.cpu_time, fio.cmd"
                             " from semantic_fio as fio"
                             " join semantic_faults as fault on fio.status = fault.id"
                             " where func_id = ?"
                             " order by igroup_id")->bind(0, func_id));
    std::cout <<"Tests run for this function:\n";
    fio.headers("IGroup", "Ints", "Ptrs", "Insns", "OGroup", "Status", "Elapsed Time",
                "CPU Time", "Command");
    fio.line_prefix("    ");
    fio.print(std::cout);
}

static void
list_assembly(const SqlDatabase::TransactionPtr &tx, int func_id)
{
    SqlDatabase::StatementPtr stmt = tx->statement("select assembly from semantic_instructions where func_id = ?"
                                                   " order by position")->bind(0, func_id);
    for (SqlDatabase::Statement::iterator insn=stmt->begin(); insn!=stmt->end(); ++insn)
        std::cout <<insn.get<std::string>(0) <<"\n";
}

static void
list_combined(const SqlDatabase::TransactionPtr &tx, int func_id, bool show_assembly)
{
    // Get lines of source code and add them as keys in the Listing map. Include a few extra lines for context.
    std::map<int/*fileid*/, std::string/*filename*/> file_names;
    Listing listing;
    SqlDatabase::StatementPtr stmt3 = tx->statement("select"
                                                    "   file.name, insn.src_file_id,"
                                                    "   min(insn.src_line)-5 as minline, max(insn.src_line)+5 as maxline"
                                                    " from semantic_instructions as insn"
                                                    " join semantic_files as file on insn.src_file_id = file.id"
                                                    " where insn.func_id = ?"
                                                    " group by insn.src_file_id, file.name")->bind(0, func_id);
    for (SqlDatabase::Statement::iterator row=stmt3->begin(); row!=stmt3->end(); ++row) {
        std::string file_name = row.get<std::string>(0);
        int file_id = row.get<int>(1);
        file_names[file_id] = file_name;
        int minline = row.get<int>(2);
        int maxline = row.get<int>(3);
        SqlDatabase::StatementPtr stmt4 = tx->statement("select linenum, line from semantic_sources"
                                                        " where file_id=? and linenum>=? and linenum<=?"
                                                        " order by linenum")
                                          ->bind(0, file_id)->bind(1, minline)->bind(2, maxline);
        for (SqlDatabase::Statement::iterator srcinfo=stmt4->begin(); srcinfo!=stmt4->end(); ++srcinfo)
            listing[SourcePosition(file_id, srcinfo.get<int>(0))].source_code = srcinfo.get<std::string>(1);
    }

    // Get lines of assembly code and insert them into the correct place in the Listing.
    if (show_assembly) {
        SqlDatabase::StatementPtr stmt5 = tx->statement("select src_file_id, src_line, position, assembly"
                                                        " from semantic_instructions"
                                                        " where func_id = ?"
                                                        " order by position")->bind(0, func_id);
        for (SqlDatabase::Statement::iterator row=stmt5->begin(); row!=stmt5->end(); ++row) {
            int file_id = row.get<int>(0);
            int line_num = row.get<int>(1);
            int position = row.get<int>(2);
            std::string assembly = row.get<std::string>(3);
            listing[SourcePosition(file_id, line_num)].assembly_code.insert(std::make_pair(position, assembly));
        }

        // Print the listing
        std::cout <<"WARNING: This listing should be read cautiously. It is ordered according to the\n"
                  <<"         source code with assembly lines following the source code line from which\n"
                  <<"         they came.  However, the compiler does not always generate machine\n"
                  <<"         instructions in the same order as source code.  When a discontinuity\n"
                  <<"         occurs in the assembly instruction listing, it will be marked by a \"#\"\n"
                  <<"         character.  The assembly instructions are also numbered according to\n"
                  <<"         their relative positions in the binary function.\n"
                  <<"\n"
                  <<" /---------------------------- Source file ID\n"
                  <<" |     /---------------------- Source line number\n"
                  <<" |     |   /------------------ Instruction out-of-order indicator\n"
                  <<" |     |   |  /--------------- Instruction position index\n"
                  <<" |     |   |  |      /-------- Instruction virtual address\n"
                  <<"vvvv vvvvv v vv vvvvvvvvvv\n";
    }

    // Show the listing
    int prev_position = -1;
    std::set<int> seen_files;
    for (Listing::iterator li=listing.begin(); li!=listing.end(); ++li) {
        int file_id = li->first.file_id;
        if (file_id>=0) {
            if (seen_files.insert(file_id).second)
                std::cout <<std::setw(4) <<std::right <<file_id <<".file  |" <<file_names[file_id] <<"\n";
            std::cout <<std::setw(4) <<std::right <<file_id <<"." <<std::setw(6) <<std::left <<li->first.line_num
                      <<"|" <<li->second.source_code <<"\n";
        }
        for (Instructions::iterator ii=li->second.assembly_code.begin(); ii!=li->second.assembly_code.end(); ++ii) {
            std::cout <<"           " <<(prev_position+1==ii->first ? "|" : "#")
                      <<std::setw(3) <<std::right <<ii->first <<" " <<ii->second <<"\n";
            prev_position = ii->first;
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

    Switches opt;
    int argno=1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--assembly")) {
            opt.show_assembly = true;
        } else if (!strcmp(argv[argno], "--no-assembly")) {
            opt.show_assembly = false;
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
        } else if (!strcmp(argv[argno], "--no-tests")) {
            opt.show_tests = false;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+1!=argc && argno+2!=argc)
        usage(0);
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
