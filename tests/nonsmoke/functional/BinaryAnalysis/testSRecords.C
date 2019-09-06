#include <rose.h>
#include <SRecord.h>

#include <sstream>

using namespace Rose::BinaryAnalysis;

static MemoryMap::Ptr
sRecordsToMemory(const std::string &input) {
    // Parse the input
    std::istringstream in(input);
    std::vector<SRecord> srecs = SRecord::parse(in);
    size_t nErrors = 0;
    BOOST_FOREACH (const SRecord &srec, srecs) {
        if (!srec.error().empty()) {
            std::cerr <<srec.error() <<"\n";
            ++nErrors;
        }
    }
    ASSERT_always_require(nErrors == 0);

    // Create a memory map
    MemoryMap::Ptr map = MemoryMap::instance();
    SRecord::load(srecs, map);
    return map;
}

static std::string
memoryToSRecords(const MemoryMap::Ptr &mem, SRecord::Syntax syntax) {
    std::string retval;
    std::vector<SRecord> srecs = SRecord::create(mem, syntax);
    BOOST_FOREACH (const SRecord &srec, srecs)
        retval += srec.toString() + "\n";
    return retval;
}

static void
test(const std::string &s1, SRecord::Syntax syntax) {
    MemoryMap::Ptr mem1 = sRecordsToMemory(s1);
    std::string s2 = memoryToSRecords(mem1, syntax); // probably not equal to s1
    MemoryMap::Ptr mem2 = sRecordsToMemory(s2);
    std::string s3 = memoryToSRecords(mem2, syntax);
    ASSERT_always_require2(s2 == s3, "s2:\n" + s2 + "s3:\n" + s3);
}

int main() {
    std::string s1 =
        "S00F000068656C6C6F202020202000003C\n"
        "S11F00007C0802A6900100049421FFF07C6C1B787C8C23783C6000003863000026\n"
        "S11F001C4BFFFFE5398000007D83637880010014382100107C0803A64E800020E9\n"
        "S111003848656C6C6F20776F726C642E0A0042\n"
        "S5030003F9\n"
        "S9030000FC\n";
    
    std::string s2 =
        ":10010000214601360121470136007EFE09D2190140\n"
        ":100110002146017E17C20001FF5F16002148011928\n"
        ":10012000194E79234623965778239EDA3F01B2CAA7\n"
        ":100130003F0156702B5E712B722B732146013421C7\n"
        ":00000001FF\n";

    test(s1, SRecord::SREC_MOTOROLA);
    test(s2, SRecord::SREC_INTEL);

    test(s1, SRecord::SREC_INTEL);
    test(s2, SRecord::SREC_MOTOROLA);
}
