// Shows clone detection results.  The only reason we have this is because SQLite3 insists that all integers should be printed
// in decimal form, but it's more convenient to see function entry addresses in hexadecimal.

#include "rose.h"
#include "DwarfLineMapper.h"
#include "FormatRestorer.h"
#include "sqlite3x.h"
using namespace sqlite3x;


#include <cerrno>
#include <cstdarg>

#define ALL_CLUSTERS    (-1)
#define BUSY_TIMEOUT    (60*1000) /*1 minute*/
static std::string argv0;

static void
usage(int exit_status, const std::string &mesg="")
{
    if (!mesg.empty())
        std::cerr <<argv0 <<": " <<mesg <<"\n";
    std::cerr <<"usage: " <<argv0 <<" DATABASE_NAME\n"
              <<"           Show all clusters of similar functions\n"
              <<"       " <<argv0 <<" DATABASE_NAME [syntactic|semantic|combined] clusters\n"
              <<"       " <<argv0 <<" DATABASE_NAME [syntactic|semantic|combined] cgclusters\n"
              <<"           Show all syntactic, semantic, or combined (intersection of syntactic and semantic) clusters\n"
              <<"           using either normal clusters or callgraph clusters.\n"
              <<"       " <<argv0 <<" DATABASE_NAME [syntactic|semantic|combined] cluster CLUSTER_ID\n"
              <<"       " <<argv0 <<" DATABASE_NAME [syntactic|semantic|combined] cgcluster CLUSTER_ID\n"
              <<"           Show a single cluster\n"
              <<"       " <<argv0 <<" DATABASE_NAME functions\n"
              <<"           List basic info about all functions\n"
              <<"       " <<argv0 <<" DATABASE_NAME function FUNCTION_ID\n"
              <<"           Show all information about a single function\n"
              <<"       " <<argv0 <<" DATABASE_NAME list FUNCTION_ID\n"
              <<"           List the function with source code if available.\n";
    exit(exit_status);
}

static void
die(const char *mesg, ...)
{
    va_list ap;
    va_start(ap, mesg);
    std::cerr <<argv0 <<": ";
    vfprintf(stderr, mesg, ap);
    va_end(ap);
    exit(1);
}

struct Row {
    int cluster_id, function_id;
    uint64_t entry_va;
    std::string funcname, filename;
    Row(int cluster_id, int function_id, uint64_t entry_va, const std::string &funcname, const std::string &filename)
        : cluster_id(cluster_id), function_id(function_id), entry_va(entry_va), funcname(funcname), filename(filename) {}
};

static void
show_cluster(const std::string &dbname, const std::string &tabname, int cluster_id)
{
    typedef std::vector<Row> Rows;

    // Open the database and suck in the information we need
    sqlite3_connection db(dbname.c_str());
    db.busy_timeout(BUSY_TIMEOUT);
    sqlite3_command cmd1(db,
                         "select clusters.cluster_id, funcs.id, funcs.entry_va, funcs.funcname, files.name"
                         " from " + tabname + " as clusters"
                         " join semantic_functions as funcs on clusters.func_id = funcs.id"
                         " join semantic_files as files on funcs.file_id = files.id"
                         + (ALL_CLUSTERS==cluster_id ? "" : " where cluster_id = ?") +
                         " order by clusters.cluster_id, clusters.func_id");
    if (cluster_id!=ALL_CLUSTERS)
        cmd1.bind(1, cluster_id);
    sqlite3_reader cursor = cmd1.executereader();
    Rows rows;
    while (cursor.read())
        rows.push_back(Row(cursor.getint(0), cursor.getint(1), cursor.getint64(2), cursor.getstring(3), cursor.getstring(4)));
    if (rows.size() > 7) // keep output clean; most people can recognize up to seven items without counting
        std::cout <<rows.size() <<" functions in these clusters\n";

    // Column headers
    std::vector<std::string> headers(5, "");
    headers[0] = "cluster_id";
    headers[1] = "function_id";
    headers[2] = "entry_va";
    headers[3] = "funcname";
    headers[4] = "filename";

    // Figure out how wide to make the columns
    std::vector<int> width(5);
    for (size_t i=0; i<5; ++i)
        width[i] = headers[i].size();
    char buf[64];
    int n;
    for (size_t i=0; i<rows.size(); ++i) {
        n = snprintf(buf, sizeof buf, "%d", rows[i].cluster_id);
        width[0] = std::max(width[0], n);
        n = snprintf(buf, sizeof buf, "%d", rows[i].function_id);
        width[1] = std::max(width[1], n);
        n = snprintf(buf, sizeof buf, "0x%08"PRIx64, rows[i].entry_va);
        width[2] = std::max(width[2], n);
        width[3] = std::max(width[3], (int)rows[i].funcname.size());
        width[4] = std::max(width[4], (int)rows[i].filename.size());
    }

    // Print everything
    for (size_t i=0; i<5; ++i)
        std::cout <<(i>0?" ":"") <<std::setw(width[i]) <<headers[i];
    std::cout <<"\n";
    for (size_t i=0; i<5; ++i)
        std::cout <<(i>0?" ":"") <<std::string(width[i], '-');
    std::cout <<"\n";
    for (size_t i=0; i<rows.size(); ++i) {
        snprintf(buf, sizeof buf, "0x%08"PRIx64, rows[i].entry_va);
        buf[sizeof(buf)-1] = '\0';
        std::cout <<std::setw(width[0]) <<std::right <<rows[i].cluster_id <<" "
                  <<std::setw(width[1]) <<std::right <<rows[i].function_id <<" "
                  <<std::setw(width[2]) <<std::right <<buf <<" "
                  <<std::setw(width[3]) <<std::left  <<rows[i].funcname <<" "
                  <<std::setw(width[4]) <<std::left  <<rows[i].filename <<"\n";
    }
}

static void
show_all(const std::string &dbname)
{
    std::cout <<"Syntactic clusters (from \"syntactic_clusters\" table):\n";
    show_cluster(dbname, "syntactic_clusters", ALL_CLUSTERS);
    std::cout <<"\nSemantic clusters (from \"semantic_clusters\" table):\n";
    show_cluster(dbname, "semantic_clusters", ALL_CLUSTERS);
    std::cout <<"\nCombined clusters (intersection of syntactic and semantic clusters):\n";
    show_cluster(dbname, "combined_clusters", ALL_CLUSTERS);
}

static void
show_function(const std::string &dbname, int function_id)
{
    sqlite3_connection db(dbname.c_str());
    db.busy_timeout(BUSY_TIMEOUT);

    // Print some info about the function itself
    sqlite3_command cmd1(db,
                         "select funcs.entry_va, funcs.funcname, files.name, funcs.listing"
                         " from semantic_functions as funcs"
                         " join semantic_files as files on funcs.file_id = files.id"
                         " where funcs.id = ?");
    cmd1.bind(1, function_id);
    sqlite3_reader c1 = cmd1.executereader();
    if (!c1.read())
        die("function id not found: %d", function_id);
    rose_addr_t entry_va = c1.getint64(0);
    std::string funcname = c1.getstring(1);
    std::string filename = c1.getstring(2);
    std::string listing = c1.getstring(3);
    if (c1.read())
        die("duplicate function id: %d", function_id);
    std::cout <<"Function ID:               " <<function_id <<"\n"
              <<"Specimen name:             " <<filename <<"\n"
              <<"Function name:             " <<funcname <<"\n"
              <<"Entry virtual address:     " <<entry_va <<"\n";

    // Inputs and outputs
    sqlite3_command cmd2(db, "select inputgroup_id, effective_outputgroup from semantic_fio where func_id = ?"
                         " order by inputgroup_id");
    sqlite3_command cmd3(db, "select vtype, pos, val from semantic_inputvalues where id=? order by vtype,pos");
    sqlite3_command cmd4(db, "select output.val, fault.name"
                         " from semantic_outputvalues as output"
                         " left join semantic_faults as fault on output.val = fault.id"
                         " where output.id=? order by output.val");

    cmd2.bind(1, function_id);
    sqlite3_reader c2 = cmd2.executereader();
    for (size_t i_run=0; c2.read(); ++i_run) {
        std::cout <<"\nRun #" <<i_run <<":\n";
        int inputgroup_id = c2.getint(0);
        int outputgroup_id = c2.getint(1);

        std::cout <<"  used input sequence #" <<inputgroup_id;
        cmd3.bind(1, inputgroup_id);
        sqlite3_reader c3 = cmd3.executereader();
        while (c3.read()) {
            std::string vtype = c3.getstring(0);
            int pos = c3.getint(1);
            uint64_t val = c3.getint64(2);
            assert(!vtype.empty());
            if ('P'==vtype[0]) {
                std::cout <<(0==pos ? "\n    Pointers:     " : ", ") <<StringUtility::addrToString(val);
            } else {
                std::cout <<(0==pos ? "\n    Non-pointers: " : ", ") <<val;
            }
        }
        std::cout <<"\n";
        
        std::cout <<"  generated output set #" <<outputgroup_id <<"\n    Values:       ";
        cmd4.bind(1, outputgroup_id);
        sqlite3_reader c4 = cmd4.executereader();
        for (size_t i_output=0; c4.read(); ++i_output) {
            uint64_t val = c4.getint64(0);
            std::string faultname = c4.getstring(1);
            if (i_output!=0)
                std::cout <<", ";
            if (!faultname.empty()) {
                std::cout <<faultname;
            } else if (val < 256) {
                std::cout <<val;
            } else {
                std::cout <<StringUtility::addrToString(val);
            }
        }
        std::cout <<"\n";
    }
    std::cout <<"\n\n" <<listing;
}

// Show a list of all functions
static void
show_functions(const std::string &dbname)
{
    FormatRestorer saved_flags(std::cout);
    sqlite3_connection db(dbname.c_str());
    db.busy_timeout(BUSY_TIMEOUT);
    sqlite3_command cmd1(db, //  0        1                  1               2            3               4              5
                         "select func.id, fmap.syntactic_id, func.entry_va, func.isize, func.funcname, func.file_id, file.name"
                         " from semantic_functions as func"
                         " join semantic_files as file on func.file_id = file.id"
                         " join combined_functions as fmap on func.id = fmap.semantic_id"
                         " order by func.id");

    // Figure out the width for the function name column
    size_t funcname_width = 8;
    sqlite3_reader c1a = cmd1.executereader();
    while (c1a.read())
        funcname_width = std::max(funcname_width, c1a.getstring(4).size());

    // Print the data
    sqlite3_reader c1b = cmd1.executereader();
    std::cout <<"Func Synx   Entry    Insn  " <<std::setw(funcname_width) << std::left <<"Function" <<"    File\n"
              <<"  ID   ID   address  bytes " <<std::setw(funcname_width) << std::left <<"name"     <<"   ID name\n";
    while (c1b.read()) {
        std::cout <<std::setw(4) <<std::right <<c1b.getint(0) <<" "
                  <<std::setw(4) <<std::right <<c1b.getint(1) <<" "
                  <<StringUtility::addrToString(c1b.getint(2)) <<" "
                  <<std::setw(5) <<std::right <<c1b.getint(3) <<" "
                  <<std::setw(funcname_width) <<std::left <<c1b.getstring(4) <<" "
                  <<std::setw(4) <<std::right <<c1b.getint(5) <<" "
                  <<c1b.getstring(6) <<"\n";
    }
}


typedef std::map<int/*index*/, std::string/*assembly*/> Instructions;

struct Code {
    std::string source_code;
    Instructions assembly_code;
};

typedef BinaryAnalysis::DwarfLineMapper::SrcInfo SourcePosition;
typedef std::map<SourcePosition, Code> Listing;

// Combined source and assembly listing for a single function
static void
list_function(const std::string &dbname, int function_id)
{
    FormatRestorer saved_flags(std::cout);
    sqlite3_connection db(dbname.c_str());
    db.busy_timeout(BUSY_TIMEOUT);

    // General function info
    sqlite3_command cmd0(db,
                         "select files.name, funcs.funcname, funcs.entry_va"
                         " from semantic_functions as funcs"
                         " join semantic_files as files on funcs.file_id = files.id"
                         " where funcs.id = ?");

    // Range of source positions associated with a binary function.  Include a few extra lines for context.
    sqlite3_command cmd1(db,
                         "select"
                         "   files.name, insns.src_file_id,"
                         "   min(insns.src_line)-5 as minline, max(insns.src_line)+5 as maxline"
                         " from semantic_instructions as insns"
                         " join semantic_files as files on insns.src_file_id = files.id"
                         " where insns.function_id = ?"
                         " group by insns.src_file_id");

    // Lines of source code from a particular source file
    sqlite3_command cmd2(db,
                         "select linenum, line from semantic_sources"
                         " where file_id=? and linenum>=? and linenum<=?"
                         " order by linenum");

    // Lines of assembly listing for a particular function
    sqlite3_command cmd3(db,
                         "select src_file_id, src_line, position, assembly"
                         " from semantic_instructions"
                         " where function_id = ?"
                         " order by position");

    // Show some general info about the function
    cmd0.bind(1, function_id);
    sqlite3_reader c0 = cmd0.executereader();
    if (!c0.read())
        die("function id not found: %d", function_id);
    std::cout <<""
              <<"WARNING: This listing should be read cautiously. It is ordered according to the\n"
              <<"         source code with assembly lines following the source code line from which\n"
              <<"         they came.  However, the compiler does not always generate machine\n"
              <<"         instructions in the same order as source code.  When a discontinuity\n"
              <<"         occurs in the assembly instruction listing, it will be marked by a \"#\"\n"
              <<"         character.  The assembly instructions are also numbered according to\n"
              <<"         their relative positions in the binary function.\n"
              <<"Function ID:                   " <<function_id <<"\n"
              <<"Binary specimen name:          " <<c0.getstring(0) <<"\n"
              <<"Function name:                 " <<c0.getstring(1) <<"\n"
              <<"Entry virtual address:         " <<StringUtility::addrToString(c0.getint(2)) <<" (" <<c0.getint(2) <<")\n";

    // Get lines of source code and add them as keys in the Listing map.
    std::map<int/*fileid*/, std::string/*filename*/> file_names;
    Listing listing;
    cmd1.bind(1, function_id);
    sqlite3_reader c1 = cmd1.executereader();
    while (c1.read()) {
        std::string file_name = c1.getstring(0);
        int file_id = c1.getint(1);
        file_names[file_id] = file_name;
        int minline = c1.getint(2);
        int maxline = c1.getint(3);
        cmd2.bind(1, file_id);
        cmd2.bind(2, minline);
        cmd2.bind(3, maxline);
        sqlite3_reader c2 = cmd2.executereader();
        while (c2.read())
            listing[SourcePosition(file_id, c2.getint(0))].source_code = c2.getstring(1);
    }

    // Get lines of assembly code and insert them into the correct place in the Listing.
    cmd3.bind(1, function_id);
    sqlite3_reader c3 = cmd3.executereader();
    while (c3.read()) {
        int file_id = c3.getint(0);
        int line_num = c3.getint(1);
        int position = c3.getint(2);
        std::string assembly = c3.getstring(3);
        listing[SourcePosition(file_id, line_num)].assembly_code.insert(std::make_pair(position, assembly));
    }

    // Print the listing
    std::cout <<" /---------------------------- Source file ID\n"
              <<" |     /---------------------- Source line number\n"
              <<" |     |   /------------------ Instruction out-of-order indicator\n"
              <<" |     |   |  /--------------- Instruction position index\n"
              <<" |     |   |  |      /-------- Instruction virtual address\n"
              <<"vvvv vvvvv v vv vvvvvvvvvv\n";
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

    if (argno>=argc)
        usage(2);
    std::string dbname = argv[argno++];

    if (0==argc-argno) {
        show_all(dbname);
        return 0;
    }

    std::string analysis = "combined";
    if (!strcmp(argv[argno], "syntactic") || !strcmp(argv[argno], "semantic") || !strcmp(argv[argno], "combined"))
        analysis = argv[argno++];
    if (0==argc-argno) {
        show_cluster(dbname, analysis+"_clusters", ALL_CLUSTERS);
        return 0;
    }

    if (argno==argc) {
        show_cluster(dbname, analysis+"_clusters", ALL_CLUSTERS);
    } else if (argno+1==argc && (!strcmp(argv[argno], "clusters") || !strcmp(argv[argno], "cgclusters"))) {
        std::string tabname = analysis + "_" + argv[argno];
        show_cluster(dbname, tabname, ALL_CLUSTERS);
    } else if (argno+1==argc && !strcmp(argv[argno], "functions")) {
        show_functions(dbname);
    } else if (argno+2==argc && !strcmp(argv[argno], "function")) {
        char *rest;
        errno = 0;
        int function_id = strtol(argv[argno+1], &rest, 0);
        if (errno || rest==argv[argno+1] || *rest)
            usage(0, "bad function ID specified");
        show_function(dbname, function_id);
    } else if (argno+2==argc && (!strcmp(argv[argno], "cluster") || !strcmp(argv[argno], "cgcluster"))) {
        std::string tabname = analysis + "_" + argv[argno] + "s";
        char *rest;
        errno = 0;
        int cluster_id = strtol(argv[argno+1], &rest, 0);
        if (errno || rest==argv[argno+1] || *rest)
            usage(0, "bad cluster ID specified");
        show_cluster(dbname, tabname, cluster_id);
    } else if (argno+2==argc && !strcmp(argv[argno], "list")) {
        char *rest;
        errno = 0;
        int function_id = strtol(argv[argno+1], &rest, 0);
        if (errno || rest==argv[argno+1] || *rest)
            usage(0, "bad function ID specified");
        list_function(dbname, function_id);
    } else {
        usage(3, std::string("unknown subcommand: ")+argv[argno]);
    }

    return 0;
}
