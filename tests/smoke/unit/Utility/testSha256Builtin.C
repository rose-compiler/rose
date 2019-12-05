#include <rose.h>
#include <Combinatorics.h>

#include <boost/lexical_cast.hpp>
#include <CommandLine.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/Message.h>

using namespace Rose;
using namespace Sawyer::Message::Common;

enum Command { HASH, OUTPUT };

static Sawyer::Message::Facility mlog;

static std::pair<Command, size_t>
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;
    Parser p = Rose::CommandLine::createEmptyParser("test builtin SHA-256",
                                                    "Generate data and either output it or hash it. The default is to generate and "
                                                    "hash. The same data is generated each time for a particular size, which allows "
                                                    "you to either hash the data internally or send it to some external tool for "
                                                    "comparison.");
    p.doc("Synopsis", "@v{programName} [output|hash] @v{message_size}");
    p.with(Rose::CommandLine::genericSwitches());
    p.errorStream(mlog[FATAL]);
    std::vector<std::string> args = p.parse(argc, argv).apply().unreachedArgs();

    if (args.size() == 1) {
        return std::make_pair(HASH, boost::lexical_cast<size_t>(args[0]));
    } else if (args.size() == 2) {
        Command command;
        if ("hash" == args[0]) {
            command = HASH;
        } else if ("output" == args[0]) {
            command = OUTPUT;
        } else {
            mlog[FATAL] <<"unknown command \"" <<StringUtility::cEscape(args[0]) <<"\"\n";
            exit(1);
        }
        return std::make_pair(command, boost::lexical_cast<size_t>(args[1]));
    } else {
        mlog[FATAL] <<"incorrect usage; see --help\n";
        exit(1);
    }
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
    std::pair<Command, size_t> comsz = parseCommandLine(argc, argv);
    std::vector<uint8_t> data = buildData(comsz.second);

    switch (comsz.first) {
        case HASH: {
            Combinatorics::HasherSha256Builtin hasher;
            if (!data.empty())
                hasher.insert(&data[0], data.size());
            std::cout <<hasher <<"\n";
            break;
        }

        case OUTPUT:
            BOOST_FOREACH (uint8_t byte, data)
                std::cout <<reinterpret_cast<unsigned char>(byte);
            break;
    }
}
