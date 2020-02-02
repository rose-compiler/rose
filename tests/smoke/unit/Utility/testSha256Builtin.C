#include <rose.h>
#include <Combinatorics.h>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <CommandLine.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Interval.h>
#include <Sawyer/Message.h>

using namespace Rose;
using namespace Sawyer::Message::Common;

typedef Sawyer::Container::Interval<size_t> Sizes;

static Sawyer::Message::Facility mlog;

static std::pair<std::string, Sizes>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser p = Rose::CommandLine::createEmptyParser("test builtin SHA-256",
                                                    "Generate data and either output it or hash it. The default is to generate and "
                                                    "hash. The same data is generated each time for a particular size, which allows "
                                                    "you to either hash the data internally or send it to some external tool for "
                                                    "comparison.");
    p.doc("Synopsis", "@v{programName} [@v{switches}] @v{message_min_size} [@v{message_max_size}]");

    std::string outputPrefix;
    SwitchGroup tool("Tool-specific switches");
    tool.insert(Switch("output", 'o')
                .argument("prefix", anyParser(outputPrefix))
                .doc("Send the actual data to a file whose name is @v{prefix}-@v{NNNNNNNN}.dat. where @v{NNNNNNNN} is the decimal size "
                     "of the file in bytes. If more than one size is being tested then multiple files will be created."));

    p.with(Rose::CommandLine::genericSwitches());
    p.with(tool);
    p.errorStream(mlog[FATAL]);
    std::vector<std::string> args = p.parse(argc, argv).apply().unreachedArgs();

    Sizes sizes;
    if (args.size() == 1) {
        sizes = boost::lexical_cast<size_t>(args[0]);
    } else if (args.size() == 2) {
        sizes = Sizes::hull(boost::lexical_cast<size_t>(args[0]), boost::lexical_cast<size_t>(args[1]));
    } else {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }

    return std::make_pair(outputPrefix, sizes);
}

static std::vector<uint8_t>
buildData(size_t nBytes) {
    std::vector<uint8_t> data;
    data.reserve(nBytes);
    for (size_t i=0; i<nBytes; ++i)
        data.push_back(i & 0xff);
    return data;
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("tool-specific diagnostics");
    std::pair<std::string, Sizes> comsz = parseCommandLine(argc, argv);

    BOOST_FOREACH (size_t nBytes, comsz.second) {
        std::vector<uint8_t> data = buildData(nBytes);
        if (comsz.first.empty()) {
            Combinatorics::HasherSha256Builtin hasher;
            if (!data.empty())
                hasher.insert(&data[0], data.size());
            std::cout <<hasher <<"\n";
        } else {
            std::string filename = (boost::format("%s-%08d.dat") % comsz.first % nBytes).str();
            std::ofstream out(filename.c_str(), std::ios_base::binary);
            BOOST_FOREACH (uint8_t byte, data)
                out <<reinterpret_cast<unsigned char>(byte);
        }
    }
}

