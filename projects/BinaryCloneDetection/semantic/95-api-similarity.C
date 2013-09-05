#include <cerrno>
#include "sage3basic.h"
#include "CloneDetectionLib.h"

std::string argv0;

static SqlDatabase::TransactionPtr transaction;



static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE \n"
              <<"This command computes the API Call similarity.\n"
              <<"\n"
              <<"  These switches control how api call traces are compared:\n"
              <<"    --call-depth=-1|0|..|MAXINT\n"
              <<"            Controls which API Calls of the trace is considered part of the similarity computation.\n"
              <<"            Options are: -1 is all calls, 0 is only direct calls, or a custom depth from 1\n"
              <<"            to some arbitrary depth.\n"
              <<"    --ignore-inline-candidates\n"
              <<"            Ignore functions present in only one of the traces that has a semantic equivalent in both\n"
              <<"            functions compilation unit.\n"
              <<"    --ignore-no-compares\n"
              <<"            Ignore functions that in the semantic clone detection never succeeded for any test.\n"
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

    bool ignore_inline_candidates = false;
    bool ignore_no_compares = false;
    int  call_depth = 0;

    

    int argno = 1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")){
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--ignore-inline-candidates")) {
            ignore_inline_candidates = true;
        } else if (!strcmp(argv[argno], "--ignore-no-compares")) {
          ignore_no_compares = false;
        } else if (!strncmp(argv[argno], "--call-depth=",13)) {
          call_depth = strtol(argv[argno]+13, NULL, 0);
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+1!=argc)
        usage(1);
    SqlDatabase::ConnectionPtr conn = SqlDatabase::Connection::create(argv[argno++]);
    transaction = conn->transaction();

    return 0;
} 
