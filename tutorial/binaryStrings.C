//! [headers]
#include <rose.h>

#include <boost/foreach.hpp>
#include <Partitioner2/Engine.h>
#include <BinaryString.h>

using namespace rose::BinaryAnalysis;
//! [headers]

int
main(int argc, char *argv[]) {
    //! [commandline]
    std::string purpose = "finds static strings in a binary specimen";
    std::string description =
        "This tool disassembles a binary specimen and then scans the "
        "read-only parts of memory to find static strings. It looks for "
        "C-style NUL-termianted printable ASCII strings, zero-terminated "
        "UTF-16 little-endian strings, two-byte little-endian length-encoded "
        "ASCII strings, and some other common formats.";

    Partitioner2::Engine engine;
    std::vector<std::string> specimen =
        engine.parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    //! [commandline]

    //! [load]
    MemoryMap map = engine.loadSpecimens(specimen);
    ByteOrder::Endianness sex = engine.obtainDisassembler()->get_sex();
    //! [load]

    //! [analysis]
    Strings::StringFinder finder;       // the string analyzer
    finder.settings().minLength = 5;    // no strings shorter than 5 characters
    finder.settings().maxLength = 8192; // no strings longer than 8k characters
    finder.insertCommonEncoders(sex);   // match common encodings of strings
    finder.find(map.require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE));
    //! [analysis]

    //! [output]
    // Output, or just do "std::cout <<finder" if you're not picky.
    BOOST_FOREACH (const Strings::EncodedString &string, finder.strings()) {
        std::cout <<"string at " <<string.address() <<" for " <<string.size() <<" bytes\n";
        std::cout <<"encoding: " <<string.encoder()->name() <<"\n";
        std::cout <<"narrow value: \"" <<StringUtility::cEscape(string.narrow()) <<"\"\n";
    }
    //! [output]
}
