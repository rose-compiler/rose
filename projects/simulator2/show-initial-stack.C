#include <rose.h>
#include <Sawyer/IntervalMap.h>

using namespace rose::StringUtility;

int
main(int argc, char *argv[], char *envp[]) {

    // Figure out where things are
    typedef Sawyer::Container::IntervalMap<AddressInterval, std::string> WhatsHere;
    WhatsHere whatsHere;
    whatsHere.insert(AddressInterval::baseSize((rose_addr_t)&argc, sizeof argc), "argc");
    whatsHere.insert(AddressInterval::baseSize((rose_addr_t)&argv, sizeof(char**)), "argv");
    for (int i=0; i<=argc; ++i)
        whatsHere.insert(AddressInterval::baseSize((rose_addr_t)(argv+i), sizeof(argv[i])), "argv["+numberToString(i)+"]");
    for (int i=0; i<argc; ++i) {
        size_t len = strlen(argv[i]) + 1;
        whatsHere.insert(AddressInterval::baseSize((rose_addr_t)argv[i], len), "argv["+numberToString(i)+"][*]");
    }
    whatsHere.insert(AddressInterval::baseSize((rose_addr_t)&envp, sizeof(char**)), "envp");

    // Print them
    BOOST_REVERSE_FOREACH (const WhatsHere::Node &node, whatsHere.nodes())
        std::cout <<node.key() <<"\t" <<node.value() <<"\n";
}
