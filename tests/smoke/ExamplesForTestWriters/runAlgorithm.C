static const char *purpose = "demo checker for smoke tests";

static const char *description =
    "This is a stupid example that pretends to run some algorithm on an optional specimen. In actuality, it does nothing "
    "but check its command-line and print a message.";



#include <rose.h>
#include <Diagnostics.h>
#include <Sawyer/CommandLine.h>

using namespace rose;
namespace cl = Sawyer::CommandLine;

int
main(int argc, char *argv[]) {
    std::string algorithm = "none";

    // Build a relatively complete man page while we're at it. You don't have to use this style. In fact, if you have a lot of
    // switches I would recommend something a little more readable.
    std::vector<std::string> args = cl::Parser()
                                    .purpose(purpose)
                                    .version(std::string(ROSE_SCM_VERSION_ID).substr(0,8), ROSE_CONFIGURE_DATE)
                                    .chapter(1, "ROSE Command-line Tools")
                                    .doc("Synopsis", "@prop{programName} [@v{switches}] @v{files}")
                                    .doc("Description", description)

                                    .with(cl::Switch("help", 'h')
                                          .action(cl::showHelpAndExit(0))
                                          .doc("Show this documentation."))

                                    .with(cl::Switch("algorithm")
                                          .argument("algo", cl::anyParser(algorithm))
                                          .doc("Name of the algorithm to run on the input files. Any string you like."))

                                    .parse(argc, argv).apply().unreachedArgs();

    std::cout <<"ran algorithm " <<algorithm <<" on " <<StringUtility::plural(args.size(), "inputs") <<"\n";
}
