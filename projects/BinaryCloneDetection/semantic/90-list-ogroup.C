// List output group

#include "sage3basic.h"
#include "CloneDetectionLib.h"
#include "DwarfLineMapper.h"
#include <cerrno>

using namespace CloneDetection;
std::string argv0;

static void
usage(int exit_status)
{
    std::cerr <<"usage: " <<argv0 <<" [SWITCHES] [--] DATABASE [OGROUP|FUNCTION]\n"
              <<"  This command lists function instructions interspersed with source code if source code is available.\n"
              <<"\n"
              <<"    --summary|--details\n"
              <<"            The --summary switch causes only summary information about an output group to be shown, while\n"
              <<"            the --details switch (the default) also lists the values of the output group.\n"
              <<"\n"
              <<"    DATABASE\n"
              <<"            The name of the database to which we are connecting.  For SQLite3 databases this is just a local\n"
              <<"            file name that will be created if it doesn't exist; for other database drivers this is a URL\n"
              <<"            containing the driver type and all necessary connection parameters.\n"
              <<"    OGROUP\n"
              <<"            Hash key (64-bit random ID) for output group. This is the output group that is displayed.\n"
              <<"    FUNCTION\n"
              <<"            A function ID number whose output group hash keys are shown.\n";
    exit(exit_status);
}

struct Switches {
    Switches(): show_details(true) {}
    bool show_details;
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
    int argno=1;
    for (/*void*/; argno<argc && '-'==argv[argno][0]; ++argno) {
        if (!strcmp(argv[argno], "--")) {
            ++argno;
            break;
        } else if (!strcmp(argv[argno], "--help") || !strcmp(argv[argno], "-h")) {
            usage(0);
        } else if (!strcmp(argv[argno], "--details")) {
            opt.show_details = true;
        } else if (!strcmp(argv[argno], "--summary") || !strcmp(argv[argno], "--summarize")) {
            opt.show_details = false;
        } else {
            std::cerr <<argv0 <<": unknown switch: " <<argv[argno] <<"\n"
                      <<argv0 <<": see --help for more info\n";
            exit(1);
        }
    };
    if (argno+2!=argc)
        usage(0);
    SqlDatabase::TransactionPtr tx = SqlDatabase::Connection::create(argv[argno++])->transaction();

    // Try to load the output group.
    char *ogroup_spec=argv[argno++], *rest;
    errno = 0;
    int64_t ogroup_id = strtoll(ogroup_spec, &rest, 0);
    if (errno || rest==ogroup_spec || *rest) {
        std::cerr <<argv0 <<": invalid output group ID or function ID: " <<ogroup_id <<"\n";
        exit(1);
    }
    OutputGroups ogroups;
    if (ogroups.load(tx, ogroup_id)) {
        ogroups.lookup(ogroup_id)->print(std::cout);
    } else {
        // Try to load a function
        assert(!"not implemented");
    }

    // no commit -- database not modified; otherwise be sure to also add CloneDetection::finish_command()
    return 0;
}
