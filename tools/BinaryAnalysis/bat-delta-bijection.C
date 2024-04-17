const char *purpose = "find best offset mapping one set to another";
const char *description =
    "This program takes two sets of integers, sets A and B, and chooses an integer delta, d, which maximizes the "
    "number of values from the set {A+d} which also exist in B.  The program can be invoked two ways:"

    "@bullet{When invoked with two file names, the numbers from the first file compose set A and those from the "
    "second are set B.  Each line of the files should be an integer that can be parsed with the 'strtoull' function.}"

    "@bullet{When invoked with one file name it looks for lines that begin with the string \"function 0x\" followed "
    "by a hexadecimal address, and end with the word \"missing\" or \"exists\".  Those addresses that are \"exists\" "
    "form set A and the others form set B.}"

    "The basic idea is that the starting virtual address for a raw buffer whose memory position is unknown can often "
    "be guessed by comparing function call sites with function entry addresses.  The function entry addresses "
    "(which are generally incorrect since we don't know the buffer's true address), form set A and the call "
    "targets form set B.  If call targets are mostly position dependent (constant addresses) then one of the best "
    "deltas detected by this program will likely be the delta that must be added to the address where we "
    "originally loaded the buffer. That is, adjusting the buffer's position in memory by delta would cause the "
    "function entry addresses to line up with the call target addresses.";

#include <rose.h>
#include <batSupport.h>

#include <Rose/BitOps.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <rose_getline.h>
#include <rose_strtoull.h>

#include <Sawyer/CommandLine.h>
#include <Sawyer/Map.h>
#include <Sawyer/ProgressBar.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <errno.h>
#include <fstream>
#include <regex.h>
#include <set>
#include <string>
#include <vector>

using Value = rose_addr_t;
using Set = std::set<Value>;

using namespace Rose;
using namespace Rose::Diagnostics;

struct Settings {
    unsigned nBits = 32;
};

static std::vector<boost::filesystem::path>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen= Rose::CommandLine::genericSwitches();

    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("width")
                .argument("nbits", nonNegativeIntegerParser(settings.nBits))
                .doc("Width of values in bits. All calculations are done in modulo arithmetic. The default is " +
                     StringUtility::plural(settings.nBits, "bits")));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("synopsis",
               "@prop{programName} [@v{switches}] @v{setA} @v{setB}\n\n"
               "@prop{programName} [@v{switches}] @v{function_list}");
    parser.with(tool);
    parser.with(gen);
    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();

    if (args.size() != 1 && args.size() != 2) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
    if (0==settings.nBits) {
        mlog[FATAL] <<"value width cannot be zero\n";
        exit(1);
    }
    if (settings.nBits > 8*sizeof(Value)) {
        mlog[FATAL] <<"value width cannot be larger than " + StringUtility::addrToString(8*sizeof(Value)) <<"\n";
        exit(1);
    }

    return std::vector<boost::filesystem::path>(args.begin(), args.end());
}

static void
readFunctions(const boost::filesystem::path &fileName, const Value mask, Set &set1 /*out*/, Set &set2 /*out*/) {
    std::regex re("^function (0x[0-9a-fA-F]+): (exists|missing)");
    std::ifstream f(fileName.c_str());

    if (!f) {
        mlog[FATAL] <<"cannot open file: " <<fileName <<"\n";
        exit(1);
    }
    while (f) {
        const std::string line = rose_getline(f);
        if (line.empty())
            break;

        std::smatch found;
        if (std::regex_search(line, found, re)) {
            if (found.str(2) == "exists") {
                set1.insert(*StringUtility::toNumber<Value>(found.str(1)) & mask);
            } else {
                ASSERT_require(found.str(2) == "missing");
                set2.insert(*StringUtility::toNumber<Value>(found.str(1)) & mask);
            }
        }
    }
}

static Set
readSet(const boost::filesystem::path &fileName, const Value mask) {
    Set set;
    std::ifstream f(fileName.c_str());
    if (!f) {
        mlog[FATAL] <<"cannot open file: " <<fileName <<"\n";
        exit(1);
    }

    size_t lineNumber = 1;
    while (f) {
        const std::string line = rose_getline(f);
        if (line.empty())
            break;

        if (auto val = StringUtility::toNumber<Value>(boost::trim_copy(line))) {
            set.insert(*val & mask);
            ++lineNumber;
        } else {
            mlog[FATAL] <<"expected a number at line " <<lineNumber <<" in " <<fileName <<": " <<val.unwrapError() <<"\n";
            exit(1);
        }
    }
    return set;
}

static size_t
countMatches(const Set &s1, Set s2, Value delta, Value mask) {
    size_t n = 0;
    for (Value v1: s1) {
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
    ROSE_INITIALIZE;

    // Parse command line
    Settings settings;
    std::vector<boost::filesystem::path> args = parseCommandLine(argc, argv, settings /*out*/);
    Value mask = BitOps::lowMask<Value>(settings.nBits);

    // Load number sets
    Set setA, setB;
    if (1 == args.size()) {
        readFunctions(args[0], mask, setA /*out*/, setB /*out*/);
    } else {
        ASSERT_require(2 == args.size());
        setA = readSet(args[0], mask);
        setB = readSet(args[1], mask);
    }
    mlog[INFO] <<"set A contains " <<StringUtility::plural(setA.size(), "values") <<"\n";
    mlog[INFO] <<"set B contains " <<StringUtility::plural(setB.size(), "values") <<"\n";

    // Try each possible delta
    using Deltas = Sawyer::Container::Map<Value /*delta*/, size_t /*count*/>;
    Deltas deltas;
    Sawyer::ProgressBar<size_t> progress(setA.size()*setB.size(), mlog[MARCH]);
    for (Value a: setA) {
        for (Value b: setB) {
            ++progress;
            Value delta = (b -a ) & mask;
            if (!deltas.exists(delta))
                deltas.insert(delta, countMatches(setA, setB, delta, mask));
        }
    }

    // Invert the deltas map and sort
    using InvertedDelta = Sawyer::Container::Map<size_t /*count*/, Set /*deltas*/>;
    InvertedDelta results;
    for (const Deltas::Node &dn: deltas.nodes()) {
        results.insertMaybeDefault(dn.value());
        results[dn.value()].insert(dn.key());
    }

    // Show results
    std::cout <<"NMatch(%Match) Deltas...\n";
    for (const InvertedDelta::Node &result: boost::adaptors::reverse(results.nodes())) {
        std::cout <<result.key() <<"(" <<(100.0*result.key()/setB.size())<<"%):";
        for (Value delta: result.value())
            std::cout <<" " <<StringUtility::addrToString(delta);
        std::cout <<"\n";
    }

    return 0;
}
