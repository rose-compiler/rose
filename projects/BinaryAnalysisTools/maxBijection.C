#include "rose.h"
#include "rose_getline.h"
#include "rose_strtoull.h"
#include "Diagnostics.h"
#include <Sawyer/CommandLine.h>
#include <Sawyer/ProgressBar.h>
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
    SwitchGroup generic = CommandlineProcessing::genericSwitches();

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("width")
                .argument("nbits", nonNegativeIntegerParser(settings.nBits))
                .doc("Width of values in bits. All calculations are done in modulo arithmetic. The default is " +
                         StringUtility::plural(settings.nBits, "bits")));

    Parser parser;
    parser
        .purpose("find best offset mapping one set to another")
        .doc("synopsis",
             "@prop{programName} [@v{switches}] @v{setA} @v{setB}\n\n"
             "@prop{programName} [@v{switches}] @v{function_list}")
        .doc("description",
             "This program takes two sets of integers, sets A and B, and chooses an integer delta, d, which maximizes the "
             "number of values from the set {A+d} which also exist in B.  The program can be invoked two ways:"

             "@bullet{When invoked with two file names, the numbers from the first file compose set A and those from the "
             "second are set B.  Each line of the files should be an integer that can be parsed with the 'strtoull' function.}"

             "@bullet{When invoked with one file name it looks for lines that begin with the string \"function 0x\" followed "
             "by a hexadecimal address, and end with the word \"missing\" or \"exists\".  Those "
             "addresses that are \"exists\" form set A and the others form set B.}"

             "The basic idea is that the starting virtual address for a raw buffer whose memory position is unknown can often "
             "be guessed by comparing function call sites with function entry addresses.  The function entry addresses "
             "(which are generally incorrect since we don't know the buffer's true address), form set A and the call "
             "targets form set B.  If call targets are mostly position dependent (constant addresses) then one of the best "
             "deltas detected by this program will likely be the delta that must be added to the address where we "
             "originally loaded the buffer. That is, adjusting the buffer's position in memory by delta would cause the "
             "function entry addresses to line up with the call target addresses.");

    return parser.with(generic).with(tool).parse(argc, argv).apply();
}

static void
readFunctions(const std::string &fileName, Value mask, Set &set1 /*out*/, Set &set2 /*out*/) {
    ASSERT_require2(fileName.size()==strlen(fileName.c_str()), "NUL characters not allowed in file names");
    FILE *f = fopen(fileName.c_str(), "r");
    if (!f)
        throw std::runtime_error(fileName + ": " + strerror(errno));
    char *line = NULL;
    size_t linesz = 0;
    while (rose_getline(&line, &linesz, f)>0) {
        if (boost::starts_with(line, "function 0x")) {
            char *s=line+11, *rest;
            errno = 0;
            Value val = rose_strtoull(s, &rest, 16);
            for (size_t i=strlen(rest); i>0 && isspace(rest[i-1]); --i)
                rest[i-1] = '\0';
            if (boost::ends_with(rest, ": exists")) {
                set1.insert(val & mask);
            } else if (boost::ends_with(rest, ": missing")) {
                set2.insert(val & mask);
            }
        }
    }
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
countMatches(const Set &s1, Set s2, Value delta, Value mask) {
    size_t n = 0;
    BOOST_FOREACH (Value v1, s1) {
        Value v2 = (v1 + delta) & mask;
        if (s2.erase(v2)) {
            ++n;
            if (s2.empty())
                break;
        }
    }
    return n;
}

int
main(int argc, char *argv[]) {
    Diagnostics::initialize();

    // Parse command line
    Settings settings;
    Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv, settings /*out*/);
    std::vector<std::string> positionalArgs = cmdline.unreachedArgs();
    if (0==settings.nBits)
        throw std::runtime_error("value width cannot be zero");
    if (settings.nBits > 8*sizeof(Value))
        throw std::runtime_error("value width cannot be larger than " + StringUtility::addrToString(8*sizeof(Value)));
    Value mask = IntegerOps::genMask<Value>(settings.nBits);

    // Load number sets
    Set setA, setB;
    if (1==positionalArgs.size()) {
        readFunctions(positionalArgs[0], mask, setA /*out*/, setB /*out*/);
    } else if (2==positionalArgs.size()) {
        setA = readSet(positionalArgs[0], mask);
        setB = readSet(positionalArgs[1], mask);
    } else {
        throw std::runtime_error("incorrect usage; see --help for details");
    }
    mlog[INFO] <<"set A contains " <<StringUtility::plural(setA.size(), "values") <<"\n";
    mlog[INFO] <<"set B contains " <<StringUtility::plural(setB.size(), "values") <<"\n";

    // Try each possible delta
    typedef Sawyer::Container::Map<Value /*delta*/, size_t /*count*/> Deltas;
    Deltas deltas;
    Sawyer::ProgressBar<size_t> progress(setA.size()*setB.size(), mlog[MARCH]);
    BOOST_FOREACH (Value a, setA) {
        BOOST_FOREACH (Value b, setB) {
            ++progress;
            Value delta = (b-a) & mask;
            if (!deltas.exists(delta))
                deltas.insert(delta, countMatches(setA, setB, delta, mask));
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
    std::cout <<"NMatch(%Match) Deltas...\n";
    BOOST_REVERSE_FOREACH (const InvertedDelta::Node &result, results.nodes()) {
        std::cout <<result.key() <<"(" <<(100.0*result.key()/setB.size())<<"%):";
        BOOST_FOREACH (Value delta, result.value())
            std::cout <<" " <<StringUtility::addrToString(delta);
        std::cout <<"\n";
    }

    return 0;
}

