#include "rose.h"
#include "rose_getline.h"
#include "rose_strtoull.h"
#include "Diagnostics.h"
#include <sawyer/CommandLine.h>
#include <sawyer/ProgressBar.h>
#include <set>

typedef rose_addr_t Value;
typedef std::set<Value> Set;

using namespace rose;
using namespace rose::Diagnostics;

struct Settings {
    unsigned nBits;
    Settings(): nBits(32) {}
};

static Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    SwitchGroup switches;

    // Standard stuff
    switches.insert(Switch("help", 'h')
                    .action(showHelpAndExit(0))
                    .doc("Show this documentation."));
    switches.insert(Switch("log", 'L')
                    .action(configureDiagnostics("log", Sawyer::Message::mfacilities))
                    .argument("config")
                    .whichValue(SAVE_ALL)
                    .doc("Configures diagnostics.  Use \"@s{log}=help\" and \"@s{log}=list\" to get started."));
    switches.insert(Switch("version", 'V')
                    .action(showVersionAndExit(version_message(), 0))
                    .doc("Shows version information for various ROSE components and then exits."));

    // Switches specific to this tool
    switches.insert(Switch("width")
                    .argument("nbits", nonNegativeIntegerParser(settings.nBits))
                    .doc("Width of values in bits. All calculations are done in modulo arithmetic. The default is " +
                         StringUtility::plural(settings.nBits, "bits")));

    Parser parser;
    parser
        .purpose("maximize bijection of two sets")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{setA} @v{setB}")
        .doc("description",
             "Reads two sets of integers, one set from each of the two files on the command line, and chooses an integer delta, "
             "which when added to each of the integers from the first set matches values in the second set.  The delta is "
             "chosen so as to maximize the number of values that match.");

    return parser.with(switches).parse(argc, argv).apply();
}

static Set
readSet(const std::string &fileName, Value mask) {
    Set set;
    ASSERT_require2(fileName.size()==strlen(fileName.c_str()), "NUL characters not allowed in file names");
    FILE *f = fopen(fileName.c_str(), "r");
    if (!f)
        throw std::runtime_error(fileName + ": " + strerror(errno));
    char *line = NULL;
    size_t linesz = 0, linenum = 1;
    while (rose_getline(&line, &linesz, f)>0) {
        errno = 0;
        char *rest;
        Value val = rose_strtoull(line, &rest, 0);
        while (*rest && isspace(*rest))
            ++rest;
        if (errno || *rest)
            throw std::runtime_error(fileName+":"+StringUtility::numberToString(linenum)+": syntax error in number: "+line);
        val &= mask;
        set.insert(val);
        ++linenum;
    }
    fclose(f);
    if (line)
        free(line);
    return set;
}

static size_t
countBijection(const Set &s1, const Set &s2, Value delta, Value mask) {
    size_t n = 0;
    BOOST_FOREACH (Value v1, s1) {
        Value v2 = (v1 + delta) & mask;
        if (s2.find(v2)!=s2.end())
            ++n;
    }
    return n;
}

int
main(int argc, char *argv[]) {
    Diagnostics::initialize();
    Settings settings;
    Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv, settings /*out*/);
    std::vector<std::string> positionalArgs = cmdline.unreachedArgs();
    if (2!=positionalArgs.size())
        throw std::runtime_error("incorrect usage; see --help for details");
    if (0==settings.nBits)
        throw std::runtime_error("value width cannot be zero");
    if (settings.nBits > 8*sizeof(Value))
        throw std::runtime_error("value width cannot be larger than " + StringUtility::addrToString(8*sizeof(Value)));
    Value mask = IntegerOps::genMask<Value>(settings.nBits);

    Set s1 = readSet(positionalArgs[0], mask);
    Set s2 = readSet(positionalArgs[1], mask);

    // Try each possible delta
    typedef Sawyer::Container::Map<Value /*delta*/, size_t /*count*/> Deltas;
    Deltas deltas;
    Sawyer::ProgressBar<size_t> progress(s1.size()*s2.size(), mlog[INFO]);
    BOOST_FOREACH (Value v1, s1) {
        BOOST_FOREACH (Value v2, s2) {
            ++progress;
            Value delta = (v2-v1) & mask;
            if (!deltas.exists(delta))
                deltas.insert(delta, countBijection(s1, s2, delta, mask));
        }
    }

    // Invert the deltas map and sort
    typedef Sawyer::Container::Map<size_t /*count*/, Set /*deltas*/> InvertedDelta;
    InvertedDelta results;
    BOOST_FOREACH (const Deltas::Node &dn, deltas.nodes()) {
        results.insertMaybeDefault(dn.value());
        results[dn.value()].insert(dn.key());
    }

    // Show results
    BOOST_REVERSE_FOREACH (const InvertedDelta::Node &result, results.nodes()) {
        std::cout <<result.key() <<"(" <<(100.0*result.key()/s1.size())<<"%):";
        BOOST_FOREACH (Value delta, result.value())
            std::cout <<" " <<StringUtility::addrToString(delta);
        std::cout <<"\n";
    }

    return 0;
}

