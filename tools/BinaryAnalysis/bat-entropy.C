static const char *purpose = "entropy computed over a sliding window";
static const char *description =
    "Slides a window across virtual memory and computes the entropy at each position. Command-line switches control "
    "the size of the window, the number of bytes per symbol, and the amount by which to shift the window at each step.";

#include <rose.h>
#include <CommandLine.h>                                // rose
#include <Partitioner2/Engine.h>                        // rose

#include <batSupport.h>
#include <boost/format.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static Sawyer::Message::Facility mlog;
static const char *OVERFLOW_SUFFIX = "+++";

struct Settings {
    SerialIo::Format stateFormat;
    size_t symbolSize;                                  // size of each symbol in bytes
    size_t windowSize;                                  // size of each window in symbols
    size_t translation;                                 // window translation at each step in symbols
    rose_addr_t alignment;                              // how to align the first window in bytes; 0 means same as symbolSize
    AddressInterval where;                              // what part of memory to examine
    size_t barLength;                                   // for bar charts, total number of columns
    size_t nBuckets;                                    // number of buckets for histograms
    double scale;

    Settings()
        : stateFormat(SerialIo::BINARY), symbolSize(1), windowSize(1024), translation(1),
          alignment(0), where(AddressInterval::whole()), barLength(0), nBuckets(0), scale(1.0) {}
};

// Parse the command-line and return the name of the input file if any (the ROSE binary state).
static boost::filesystem::path
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup gen = Rose::CommandLine::genericSwitches();
    gen.insert(Bat::stateFileFormatSwitch(settings.stateFormat));

    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");

    tool.insert(Switch("symbol-size")
                .argument("bytes", nonNegativeIntegerParser(settings.symbolSize))
                .doc("Number of bytes per symbol. The default is " + StringUtility::plural(settings.symbolSize, "bytes") + "."));

    tool.insert(Switch("window-size")
                .argument("symbols", nonNegativeIntegerParser(settings.windowSize))
                .doc("Size of sliding window measured in number of symbols. The default is " +
                     StringUtility::plural(settings.windowSize, "symbols") + "."));

    tool.insert(Switch("translate")
                .argument("symbols", nonNegativeIntegerParser(settings.translation))
                .doc("Amount by which to slide the window at each step, measured in units of symbol size. The default is " +
                     StringUtility::plural(settings.translation, "symbols") + "."));

    tool.insert(Switch("alignment")
                .argument("bytes", nonNegativeIntegerParser(settings.alignment))
                .doc("How to align the first window, measured in bytes. This applies to the first window for each contiguous "
                     "region of virtual memory. If the start of the contiguous region is less than a multiple of the window "
                     "alignment, then the window is shifted to a higher address to cause it to be aligned. A value of zero "
                     "means that the alignment should be the same as the symbol size. The default is " +
                     StringUtility::plural(settings.alignment, "bytes") + "."));

    tool.insert(Switch("where")
                .argument("interval", P2::addressIntervalParser(settings.where))
                .doc("Specifies the range of addresses that should be analyzed.  Only windows that exist entirely "
                     "within this range are analyzed. The default is the entire address space. " +
                     P2::AddressIntervalParser::docString() + "."));

    tool.insert(Switch("bar")
                .argument("n", nonNegativeIntegerParser(settings.barLength))
                .doc("The width of the bars in the bar chart ASCII art. Zero means don't display bars. The default is " +
                     StringUtility::plural(settings.barLength, "characters") + "."));

    tool.insert(Switch("histogram")
                .argument("n", nonNegativeIntegerParser(settings.nBuckets), "32")
                .doc("Instead of showing one line per window, redraw the entire screen per window. The symbols in the "
                     "window are hashed and assigned to @v{n} buckets and the buckets are displayed one per line. The value "
                     "for each bucket is the window entropy divided by the number of buckets. The @s{bar} switch controls "
                     "the width of the bar graphs for each bucket. Since dividing the window entropy by the number of buckets "
                     "can result in very short bars, the @s{scale} switch can be used to increase their length."));

    tool.insert(Switch("scale")
                .argument("factor", realNumberParser(settings.scale))
                .doc("Causes all values to be scaled by the specified factor. This can be used to increase the length of "
                     "the bars in a histogram. Any bar that would become longer than the maximum length has the string "
                     "\"" + StringUtility::cEscape(OVERFLOW_SUFFIX) + "\" appended."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{rba-state}]");
    parser.with(gen).with(tool);
    std::vector<std::string> args = parser.parse(argc, argv).apply().unreachedArgs();

    if (0 == settings.symbolSize) {
        mlog[FATAL] <<"invalid symbol size: 0 bytes\n";
        exit(1);
    }
    if (0 == settings.windowSize) {
        mlog[FATAL] <<"invalid window size: 0 symbols\n";
        exit(1);
    }
    if (0 == settings.translation) {
        mlog[FATAL] <<"invalid window translation: 0 symbols\n";
        exit(1);
    }
    if (0 == settings.alignment)
        settings.alignment = settings.symbolSize;

    if (settings.scale <= 0.0) {
        mlog[FATAL] <<"invalid scale factor: " <<settings.scale <<"; must be positive\n";
        exit(1);
    }

    double possibleSymbols = pow(2, 8*settings.symbolSize);
    if (settings.windowSize <  possibleSymbols * 2) {
        mlog[WARN] <<"expect poor results when window is not significantly larger than the total possible number of symbols\n"
                   <<"window size is " <<StringUtility::plural(settings.windowSize, "symbols") <<" but number of "
                   <<"possible symbols is " <<possibleSymbols <<"\n";
    }

    if (args.size() > 1) {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    return args.empty() ? boost::filesystem::path("-") : args[0];
}

class Window {
    typedef Sawyer::Container::Map<std::vector<uint8_t>, size_t> Symbols;
    Symbols symbols_;
    AddressInterval location_;

public:
    bool isEmpty() const {
        return location_.isEmpty();
    }

    void clear() {
        symbols_.clear();
        location_ = AddressInterval();
    }

    AddressInterval location() const {
        return location_;
    }

    size_t nSymbols() const {
        return symbols_.isEmpty() ? 0 : location_.size() / symbols_.least().size(); // assumes all symbols same size
    }

    void insert(const MemoryMap::Ptr &map, rose_addr_t va, size_t nSymbols, size_t symbolSize) {
        ASSERT_require(symbolSize > 0);
        ASSERT_require2(symbols_.isEmpty() || symbols_.least().size() == symbolSize, "inconsistent symbols size");
        AddressInterval newInterval = AddressInterval::baseSize(va, nSymbols*symbolSize);
        ASSERT_require2(!location_.isOverlapping(newInterval), "window already contains some of this data");
        ASSERT_require2(location_.isAdjacent(newInterval), "new data is not adjacent to existing window data");
        location_ = location_.hull(newInterval);

        std::vector<uint8_t> buf(nSymbols * symbolSize);
        size_t nRead = map->at(va).read(buf).size();
        ASSERT_always_require2(nRead == buf.size(), "short read");
        for (size_t i = 0; i < nSymbols; ++i) {
            std::vector<uint8_t> symbol(buf.begin() + i * symbolSize, buf.begin() + (i+1) * symbolSize);
            ++symbols_.insertMaybe(symbol, 0);
        }
    }

    void erase(const MemoryMap::Ptr &map, rose_addr_t va, size_t nSymbols, size_t symbolSize) {
        ASSERT_require(symbolSize > 0);
        ASSERT_require2(symbols_.isEmpty() || symbols_.least().size() == symbolSize, "inconsistent symbols size");
        if (nSymbols > 0) {
            AddressInterval erasingInterval = AddressInterval::baseSize(va, nSymbols*symbolSize);
            ASSERT_require2(location_.isContaining(erasingInterval), "cannot erase what isn't present");
            if (erasingInterval == location_) {
                clear();
            } else if (location_.least() == erasingInterval.least()) {
                location_ = AddressInterval::hull(erasingInterval.greatest()+1, location_.greatest());
            } else if (location_.greatest() == erasingInterval.greatest()) {
                location_ = AddressInterval::hull(location_.least(), erasingInterval.least()-1);
            } else {
                ASSERT_not_reachable("cannot erase from the middle of a window");
            }

            std::vector<uint8_t> buf(nSymbols * symbolSize);
            size_t nRead = map->at(va).read(buf).size();
            ASSERT_always_require2(nRead == buf.size(), "short read");
            for (size_t i = 0; i < nSymbols; ++i) {
                std::vector<uint8_t> symbol(buf.begin() + i * symbolSize, buf.begin() + (i+1) * symbolSize);
                if (0 == --symbols_[symbol])
                    symbols_.erase(symbol);
            }
        }
    }

    double entropy() const {
        ASSERT_forbid(isEmpty());                       // entropy is not defined
        double e = 0.0;
        const size_t symbolSize = symbols_.least().size(); // bytes per symbol
        ASSERT_require(symbolSize > 0);
        double totalSymbols = pow(2.0, 8*symbolSize);   // number of possible distinct symbols
        BOOST_FOREACH (size_t n, symbols_.values()) {
            ASSERT_forbid(0 == n);
            double symbolProbability = double(n) / nSymbols();
            e -= symbolProbability * log(symbolProbability) / log(totalSymbols);
        }
        return e;
    }

    static size_t hash(const std::vector<uint8_t> &symbol) {
#if 0 // [Robb Matzke 2019-07-18]
        // hashing
        ASSERT_forbid(symbol.empty());
        Rose::Combinatorics::HasherFnv hasher;
        hasher.insert(&symbol[0], symbol.size());
        return hasher.partial();
#elif 0
        // little endian
        size_t retval = 0;
        for (size_t i=0; i < sizeof(retval) && i < symbol.size(); ++i)
            retval |= (size_t)symbol[i] << (8*i);
        return retval;
#else
        // big endian
        size_t retval = 0;
        for (size_t i=0; i < sizeof(retval) && i < symbol.size(); ++i)
            retval |= (size_t)symbol[i] <<(8 * (sizeof(retval)-(i+1)));
        return retval;
#endif
    }

    struct Bucket {
        size_t nSymbols;                                // n. distinct symbols represented by this bucket
        size_t total;                                   // total count for this bucket

        Bucket()
            : nSymbols(0), total(0) {}

        Bucket(size_t nSymbols, size_t total)
            : nSymbols(nSymbols), total(total) {}
    };

    typedef std::vector<Bucket> Buckets;
    Buckets bucketize(size_t nBuckets) const {
        Buckets buckets(nBuckets);
        BOOST_FOREACH (const Symbols::Node &node, symbols_.nodes()) {
#if 0 // [Robb Matzke 2019-07-19]
            size_t idx = hash(node.key()) % nBuckets;
#else
            // use the high bits
            size_t nBits = ceil(log2(nBuckets));
            size_t idx = (hash(node.key()) >> (8*sizeof(size_t) - nBits)) % nBuckets;
#endif
            ++buckets[idx].nSymbols;
            buckets[idx].total += node.value();
        }
        return buckets;
    }
};

static std::string makeBar(double value, double value2, double scale, size_t totalWidth) {
    value = std::max(0.0, value);

    size_t barLength = round(value * scale * totalWidth);
    std::string suffix;
    if (barLength > totalWidth) {
        barLength = totalWidth;
        suffix = OVERFLOW_SUFFIX;
    } else {
        suffix = std::string(strlen(OVERFLOW_SUFFIX), ' ');
    }

    // The primary value as a bar
    std::string bar = std::string(barLength, '=') + std::string(totalWidth - barLength, ' ') + '|';

    // Secondary value as a dot, but only if it's not the same as the primary value
    if (value2 >= 0.0) {
        size_t v2idx = round(value2 * scale * totalWidth);
        if (v2idx < bar.size() && v2idx+1 != barLength)
            bar[v2idx] = '.';
    }

    bar[0]                                   = '|';
    bar[(int)round(0.25*totalWidth)] = '|';
    bar[(int)round(0.50*totalWidth)] = '|';
    bar[(int)round(0.75*totalWidth)] = '|';

    return bar + suffix;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("measuring entropy in a sliding window");
    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    P2::Engine engine;
    Settings settings;
    boost::filesystem::path inputFileName = parseCommandLine(argc, argv, engine, settings);
    P2::Partitioner partitioner = engine.loadPartitioner(inputFileName, settings.stateFormat);
    MemoryMap::Ptr map = partitioner.memoryMap();
    ASSERT_not_null(map);

    // Look at only part of the map
    map = map->shallowCopy();
    map->within(settings.where).keep();

    if (settings.nBuckets > 0)
        std::cout <<"\033[2J";                          // clear screen

    std::vector<double> bucketAverages;
    Window window;
    rose_addr_t va = 0;
    while (map->atOrAfter(va).next().assignTo(va)) {

        // If this is the first window, align it
        if (window.isEmpty()) {
            if (va % settings.alignment != 0) {
                rose_addr_t alignedVa = alignUp(va, settings.alignment);
                if (alignedVa <= va)
                    break;                              // overflow
                va = alignedVa;
                continue;                               // try again now that it's aligned
            }
        }

        // If there's not enough data for the window, shift it to a higher address and try again.
        size_t nAvail = map->at(va).available().size();
        if (nAvail < settings.windowSize * settings.symbolSize) {
            window.clear();
            rose_addr_t nextVa = va + nAvail + 1;
            if (nextVa <= va)
                break;                                  // overflow
            va = nextVa;
            continue;
        }

        // Fill the window with data. If the window is empty, we can just fill it, otherwise we erase the old shifted-out data
        // and insert the new shifted-in data.
        if (window.isEmpty() || window.location().greatest() < va) {
            window.clear();
            window.insert(map, va, settings.windowSize, settings.symbolSize);

        } else {
            size_t symbolsShifted = (va - window.location().least()) / settings.symbolSize;
            ASSERT_require(symbolsShifted > 0);
            window.erase(map, window.location().least(), symbolsShifted, settings.symbolSize);
            ASSERT_require(window.location().least() == va);
            window.insert(map, window.location().greatest()+1, symbolsShifted, settings.symbolSize);
        }

        // Print results
        if (0 == settings.nBuckets) {
            // One line per window
            double e = window.entropy();
            std::cout <<StringUtility::addrToString(window.location().least()) <<" " <<(boost::format("%8.6f") % e);
            if (settings.barLength > 0)
                std::cout <<" " <<makeBar(e, -1, settings.scale, settings.barLength);
            std::cout <<"\n";
        } else {
            // One screen per window
            Window::Buckets buckets = window.bucketize(settings.nBuckets);
            bucketAverages.reserve(buckets.size());

            std::cout <<"\033[1;1H";                    // move cursor to top left of screen
            std::cout <<"window " <<StringUtility::addrToString(va) <<":\n";
            for (size_t i=0; i<buckets.size(); ++i) {
                size_t nSymbolsInBucket = buckets[i].nSymbols;
                double value = nSymbolsInBucket > 0 ? double(buckets[i].total) / (nSymbolsInBucket * window.nSymbols()) : 0.0;

                if (i >= bucketAverages.size()) {
                    bucketAverages.push_back(value);
                } else {
                    static const double weight = 0.001;
                    bucketAverages[i] = weight * value + (1-weight) * bucketAverages[i];
                }

                std::cout <<(boost::format("B%|-3| %|4| %|7.3f|%% %|1|\n")
                             % i
                             % nSymbolsInBucket
                             % (100*value)
                             % makeBar(value, bucketAverages[i], settings.scale, settings.barLength));
            }
        }

        // Calculate shifted window address
        rose_addr_t nextVa = va + settings.translation * settings.symbolSize;
        if (nextVa <= va)
            break;                                      // overflow
        va = nextVa;
    }
}
