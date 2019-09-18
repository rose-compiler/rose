// Tests the BitPattern<> class
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "rose.h"
#include "BitPattern.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace Rose;

template<typename T>
void
require(const std::string &testname, const BitPattern<T> &bp, const std::string &answer)
{
    std::ostringstream ss;
    ss <<bp;
    if (0!=ss.str().compare(answer)) {
        std::cerr <<"test '" <<testname <<"' failed\n"
                  <<"  expected: '" <<answer <<"'\n"
                  <<"  obtained: '" <<ss.str() <<"'\n";
        abort();
    }
}

void default_ctor()
{
    BitPattern<uint8_t> p1;
    require("ctor <uint8_t>", p1, "empty");

    BitPattern<uint16_t> p2;
    require("ctor <uint16_t>", p2, "empty");

    BitPattern<uint32_t> p3;
    require("ctor <uint32_t>", p3, "empty");

    BitPattern<uint64_t> p4;
    require("ctor <uint64_t>", p4, "empty");
}

void pattern_mask_ctor()
{
    BitPattern<uint8_t> p1(0x1c, 0x14, 0);
    require("pattern/mask ctor <uint8_t>", p1, "0x14/0x1c");

    BitPattern<uint16_t> p2(0, 0, 0);
    require("pattern/mask ctor <uint16_t>", p2, "empty");

    BitPattern<uint32_t> p3(0x00ff00ff, 0x003c003c, 1);
    require("pattern/mask ctor <uint32_t>", p3, "{0x00000000,0x003c003c}/{0x00000000,0x00ff00ff}");
}

int main()
{
    default_ctor();
    pattern_mask_ctor();
    return 0;
}

#endif
