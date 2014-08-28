// Returns the list of tests that remain to be executed.  Each line of output is one test and contains the following
// tab-separated fields:
//   0. The ID number of the function
//   1. The input group number

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include <cerrno>

std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" DATABASE\n"
              <<"  This command produces a list of tests that are not present in the database. Each line of output\n"
              <<"  corresponds to one test and contains the file ID for the specimen, the function ID, and the input\n"
              <<"  group number separated from one another by TAB characters.\n"
              <<"\n"
              <<"    --entry=ADDR\n"
              <<"            Select only functions having the specified entry address, which may be specified in decimal\n"
              <<"            hexadecimal, or octal using the usual C syntax.  This switch may appear more than once to select\n"
              <<"            functions that have any of the specified addresses.\n"
              <<"    --file=ID[,...]\n"
              <<"            Select only functions that are defined in one of the specified binary files.  The ID list is a\n"
              <<"            comma-separated list of file identifiers.  Invoking with --file=list will produce a list of all\n"
              <<"            files that contain functions.  This switch may appear more than once and its effect is cumulative.\n"
              <<"    --function=ID[,...]\n"
              <<"            Select only functions with the specified ID numbers.  This switch may appear more than once and\n"
              <<"            its effect is cumulative.  If no IDs are specified then all IDs are considered.\n"
              <<"    --function=TABLE.COLUMN\n"
              <<"            Select only the functions whose IDs are mentioned in the specified table column.  The normal\n"
              <<"            behavior is to use \"semantic_functions.id\".\n"
              <<"    --name=NAME\n"
              <<"            Select only functions having the specified names.  This switch may appear more than once to\n"
              <<"            select functions that have any of the specified names.\n"
              <<"    --nfuzz=N\n"
              <<"            Restricts the number of input groups to be at most N.  The default is to consider all existing\n"
              <<"            input groups.\n"
              <<"    --first-fuzz=N\n"
              <<"            Lowest numbered fuzz test to consider.  The default is zero.\n"
              <<"    --size=NINSNS\n"
              <<"            Select only functions that have at least NINSNS instructions.\n"
              <<"    --specimen=ID[,...]\n"
              <<"            Select only functions belonging to one of the specified specimens.  A \"specimen\" is a binary\n"
              <<"            file whose name was given to the 11-add-functions command, and does not include dynamically\n"
              <<"            linked libraries (unless such a library was also given as an argument to 11-add-functions).\n"
              <<"            The ID list is a comma-separated list of specimen identifiers. Invoking with --specimen=list\n"
              <<"            will produce a list of specimen ID numbers and their file names.  This switch may appear more\n"
              <<"            than once and its effect is cumulative.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n";
    exit(exit_status);
}

struct Switches {
    Switches(): ninsns(0), nfuzz(0), first_fuzz(0), nfuzz_set(false), list_specimens(false), list_files(false) {}
    std::set<rose_addr_t> entry_vas;
    std::set<std::string> names;
    std::set<int> specimens, files, functions;
    std::string function_table, function_column;
    size_t ninsns, nfuzz, first_fuzz;
    bool nfuzz_set; // is nfuzz value valid?
    bool list_specimens, list_files;
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

    Switches opt;
    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strncmp(argv[argno], "--entry=", 8)) {
            opt.entry_vas.insert(strtoull(argv[argno]+8, NULL, 0));
        } else if (!strcmp(argv[argno], "--file=list") || !strcmp(argv[argno], "--files=list")) {
            opt.list_files = true;
        } else if (!strncmp(argv[argno], "--file=", 7) || !strncmp(argv[argno], "--files=", 8)) {
            std::vector<std::string> ids = StringUtility::split(",", strchr(argv[argno], '=')+1, (size_t)-1, true);
            for (size_t i=0; i<ids.size(); ++i) {
                const char *s = ids[i].c_str();
                char *rest;
                errno = 0;
                int id = strtoul(s, &rest, 0);
                if (errno || rest==s || *rest) {
                    std::cerr <<argv0 <<": invalid file ID: " <<ids[i] <<"\n";
                    exit(1);
                }
                opt.files.insert(id);
            }
        } else if (!strncmp(argv[argno], "--function=", 11) || !strncmp(argv[argno], "--functions=", 12)) {
            std::vector<std::string> ids = StringUtility::split(",", strchr(argv[argno], '=')+1, (size_t)-1, true);
            if (ids.size()==1 && isalpha(ids[0][0]) && ids[0].find_first_of('.')!=std::string::npos) {
                std::vector<std::string> words = StringUtility::split(".", ids[0]);
                if (words.size()!=2 ||
                    !SqlDatabase::is_valid_table_name(words[0]) || !SqlDatabase::is_valid_table_name(words[1])) {
                    std::cerr <<argv0 <<": --function switch needs either IDs or a database TABLE.COLUMN\n";
                    exit(1);
                }
                opt.function_table = words[0];
                opt.function_column = words[1];
            } else {
                for (size_t i=0; i<ids.size(); ++i) {
                    const char *s = ids[i].c_str();
                    char *rest;
                    errno = 0;
                    int id = strtoul(s, &rest, 0);
                    if (errno || rest==s || *rest) {
                        std::cerr <<argv0 <<": invalid function ID: " <<ids[i] <<"\n";
                        exit(1);
                    }
                    opt.functions.insert(id);
                }
            }
        } else if (!strncmp(argv[argno], "--first-fuzz=", 13)) {
            opt.first_fuzz = strtoul(argv[argno]+13, NULL, 0);
        } else if (!strncmp(argv[argno], "--name=", 7)) {
            opt.names.insert(argv[argno]+7);
        } else if (!strncmp(argv[argno], "--nfuzz=", 8)) {
            opt.nfuzz = strtoul(argv[argno]+8, NULL, 0);
            opt.nfuzz_set = true;
        } else if (!strncmp(argv[argno], "--size=", 7)) {
            opt.ninsns = strtoul(argv[argno]+7, NULL, 0);
        } else if (!strcmp(argv[argno], "--specimen=list") || !strcmp(argv[argno], "--specimens=list")) {
            opt.list_specimens = true;
        } else if (!strncmp(argv[argno], "--specimen=", 11) || !strncmp(argv[argno], "--specimens=", 12)) {
            std::vector<std::string> ids = StringUtility::split(",", strchr(argv[argno], '=')+1, (size_t)-1, true);
            for (size_t i=0; i<ids.size(); ++i) {
                const char *s = ids[i].c_str();
                char *rest;
                errno = 0;
                int id = strtoul(s, &rest, 0);
                if (errno || rest==s || *rest) {
                    std::cerr <<argv0 <<": invalid specimen ID: " <<ids[i] <<"\n";
                    exit(1);
                }
                opt.specimens.insert(id);
            }
        } else {
            std::cerr <<argv0 <<": unrecognized switch: " <<argv[argno] <<"\n"
                      <<"see \"" <<argv0 <<" --help\" for usage info.\n";
            exit(1);
        }
    }
    if (argno+1!=argc)
        usage(1);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno++])->transaction();

    // List the ID numbers and names for all specimen files
    if (opt.list_specimens) {
        SqlDatabase::Table<int, std::string> specimens;
        specimens.insert(tx->statement("select file.id, file.name"
                                       " from (select distinct specimen_id as id from semantic_functions) as specimen"
                                       " join semantic_files as file on specimen.id = file.id"
                                       " order by file.name"));
        specimens.headers("File ID", "Specimen Name");
        specimens.print(std::cout);
        return 0;
    }

    // List the ID numbers and names for all files containing functions
    if (opt.list_files) {
        SqlDatabase::Table<int, std::string> files;
        files.insert(tx->statement("select file.id, file.name"
                                   " from (select distinct file_id as id from semantic_functions) as used"
                                   " join semantic_files as file on used.id = file.id"
                                   " order by file.name"));
        files.headers("File ID", "Binary File Name");
        files.print(std::cout);
        return 0;
    }

    // Sanity checks
    if (!opt.functions.empty() && !opt.function_table.empty()) {
        std::cerr <<argv0 <<": --function=ID and --function=TABLE are mutually exclusive\n";
        exit(1);
    }
    if (0==tx->statement("select count(*) from semantic_functions")->execute_int()) {
        std::cerr <<argv0 <<": database has no functions; nothing to test\n";
        return 0;
    }
    if (0==tx->statement("select count(*) from semantic_inputvalues")->execute_int()) {
        std::cerr <<argv0 <<": database has no input groups; nothing to test\n";
        return 0;
    }

    // Create table tmp_functions containing IDs for selected functions and their specimen IDs
    std::vector<std::string> constraints;
    if (!opt.entry_vas.empty())
        constraints.push_back("func.entry_va " + SqlDatabase::in(opt.entry_vas));
    if (!opt.names.empty())
        constraints.push_back("func.name " + SqlDatabase::in_strings(opt.names, tx->driver()));
    if (!opt.specimens.empty())
        constraints.push_back("func.specimen_id " + SqlDatabase::in(opt.specimens));
    if (!opt.files.empty())
        constraints.push_back("func.file_id " + SqlDatabase::in(opt.files));
    if (!opt.functions.empty())
        constraints.push_back("func.id " + SqlDatabase::in(opt.functions));
    if (opt.ninsns>0)
        constraints.push_back("func.ninsns >= " + StringUtility::numberToString(opt.ninsns));
    std::string sql1 = "select func.id, func.specimen_id from semantic_functions as func";
    if (!opt.function_table.empty())
        sql1 += " join "+opt.function_table+" as flist on func.id = flist."+opt.function_column;
    if (!constraints.empty())
        sql1 += " where " + StringUtility::join(" and ", constraints);
    tx->execute("create temporary table tmp_functions as " + sql1);

    // Create table tmp_inputgroups containing IDs for selected input groups
    std::string sql2 = "select distinct igroup_id from semantic_inputvalues where igroup_id >= " +
                       StringUtility::numberToString(opt.first_fuzz);
    if (opt.nfuzz_set)
        sql2 += " and igroup_id < " + StringUtility::numberToString(opt.first_fuzz+opt.nfuzz);
    tx->execute("create temporary table tmp_inputgroups as " + sql2);

    // Create tmp_pending as the cross product of functions and inputgroups except for those already tested
    tx->execute("create temporary table tmp_pending as"
                "    select func.specimen_id as specimen_id, func.id as func_id, igroup.igroup_id as igroup_id"
                "      from tmp_functions as func"
                "      join tmp_inputgroups as igroup"
                "      on igroup.igroup_id is not null" // "on" clause and "is not null" (rather than "true") for portability
                "  except"
                "    select func.specimen_id, func.id, fio.igroup_id"
                "      from semantic_fio as fio"
                "      join semantic_functions as func on fio.func_id=func.id");
    SqlDatabase::StatementPtr stmt = tx->statement("select distinct specimen_id, func_id, igroup_id"
                                                   " from tmp_pending"
                                                   " order by specimen_id, igroup_id, func_id");
    for (SqlDatabase::Statement::iterator row=stmt->begin(); row!=stmt->end(); ++row)
        std::cout <<row.get<int>(0) <<"\t" <<row.get<int>(1) <<"\t" <<row.get<int>(2) <<"\n";

    // no need to commit, but if we change this in the future, be sure to add begin_command()/finish_command()
    return 0;
}

