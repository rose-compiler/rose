#include <rose.h>
#include <sawyer/IntervalMap.h>

using namespace StringUtility;

int
main(int argc, char *argv[], char *envp[]) {

    // Figure out where things are
    typedef Sawyer::Container::IntervalMap<AddressInterval, std::string> WhatsHere;
    WhatsHere whatsHere;
    whatsHere.insert(AddressInterval::baseSize((rose_addr_t)&argc, sizeof argc), "argc");
    whatsHere.insert(AddressInterval::baseSize((rose_addr_t)&argv, sizeof argv), "argv");
    for (int i=0; i<=argc; ++i)
        whatsHere.insert(AddressInterval::baseSize((rose_addr_t)(argv+i), sizeof(argv[i])), "argv["+numberToString(i)+"]");
    for (int i=0; i<argc; ++i) {
        size_t len = strlen(argv[i]) + 1;
        whatsHere.insert(AddressInterval::baseSize((rose_addr_t)argv[i], len), "argv["+numberToString(i)+"][*]");
    }
    whatsHere.insert(AddressInterval::baseSize((rose_addr_t)&envp, sizeof envp), "envp");

    // Print them
    BOOST_REVERSE_FOREACH (const WhatsHere::Node &node, whatsHere.nodes())
        std::cout <<node.key() <<"\t" <<node.value() <<"\n";
}
