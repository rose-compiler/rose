// Tests Rose::BinaryAnalysis::SourceLocations (at least the part other than parsing ELF files).

#include <rose.h>
#include <BinarySourceLocations.h>
#include <SourceLocation.h>
#include <sstream>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

static void
emptyTests(const SourceLocations &locations) {
    SourceLocation loc1("/dev/zero", 0);
    ASSERT_always_require(locations.get(100).isEmpty());
    ASSERT_always_require(locations.get(loc1).isEmpty());
    ASSERT_always_require(locations(100).isEmpty());
    ASSERT_always_forbid(locations(loc1));
    ASSERT_always_forbid(locations.firstAddress(loc1));
    ASSERT_always_require(locations.allFileNames().empty());
    ASSERT_always_require(locations.nextSourceLocation().isEmpty());
    ASSERT_always_require(locations.nextSourceLocation(loc1).isEmpty());

    {
        std::ostringstream ss;
        locations.printSrcToAddr(ss, "  ");
        ASSERT_always_require(ss.str() == "");
    }

    {
        std::ostringstream ss;
        locations.printAddrToSrc(ss, "  ");
        ASSERT_always_require(ss.str() == "");
    }

    {
        std::ostringstream ss;
        locations.print(ss, "  ");
        ASSERT_always_require(ss.str() == "");
    }

    {
        std::ostringstream ss;
        ss <<locations;
        ASSERT_always_require(ss.str() == "");
    }
}

static void
insertFirstItem(SourceLocations &locations, const SourceLocation &loc1, const rose_addr_t va1) {
    locations.insert(loc1, va1);
    ASSERT_always_require(locations.get(loc1).size() == 1);
    ASSERT_always_require(locations.get(loc1).exists(va1));
    ASSERT_always_require(locations(loc1).orElse(0) == va1);
    ASSERT_always_require(locations(va1) == loc1);
    ASSERT_always_require(locations.firstAddress(loc1).orElse(0) == va1);
    ASSERT_always_require(locations.nextSourceLocation() == loc1);
    ASSERT_always_require(locations.nextSourceLocation(loc1).isEmpty());

    // Erase some non-existing things and make sure the first item stays there
    const SourceLocation loc2("/dev/null", 100);
    const rose_addr_t va2 = 0x08040011;
    locations.erase(loc2, va1);
    ASSERT_always_require(locations.get(loc1).size() == 1);
    ASSERT_always_require(locations.get(loc1).exists(va1));
    ASSERT_always_require(locations(loc1).orElse(0) == va1);
    ASSERT_always_require(locations(va1) == loc1);
    ASSERT_always_require(locations.firstAddress(loc1).orElse(0) == va1);
    ASSERT_always_require(locations.nextSourceLocation() == loc1);
    ASSERT_always_require(locations.nextSourceLocation(loc1).isEmpty());

    locations.erase(loc1, va2);
    ASSERT_always_require(locations.get(loc1).size() == 1);
    ASSERT_always_require(locations.get(loc1).exists(va1));
    ASSERT_always_require(locations(loc1).orElse(0) == va1);
    ASSERT_always_require(locations(va1) == loc1);
    ASSERT_always_require(locations.firstAddress(loc1).orElse(0) == va1);
    ASSERT_always_require(locations.nextSourceLocation() == loc1);
    ASSERT_always_require(locations.nextSourceLocation(loc1).isEmpty());

    locations.erase(loc2);
    ASSERT_always_require(locations.get(loc1).size() == 1);
    ASSERT_always_require(locations.get(loc1).exists(va1));
    ASSERT_always_require(locations(loc1).orElse(0) == va1);
    ASSERT_always_require(locations(va1) == loc1);
    ASSERT_always_require(locations.firstAddress(loc1).orElse(0) == va1);
    ASSERT_always_require(locations.nextSourceLocation() == loc1);
    ASSERT_always_require(locations.nextSourceLocation(loc1).isEmpty());

    locations.erase(va2);
    ASSERT_always_require(locations.get(loc1).size() == 1);
    ASSERT_always_require(locations.get(loc1).exists(va1));
    ASSERT_always_require(locations(loc1).orElse(0) == va1);
    ASSERT_always_require(locations(va1) == loc1);
    ASSERT_always_require(locations.firstAddress(loc1).orElse(0) == va1);
    ASSERT_always_require(locations.nextSourceLocation() == loc1);
    ASSERT_always_require(locations.nextSourceLocation(loc1).isEmpty());
}

// Insert a second item having the same source location as the first item.
static void
insertSecondItemSameSource(SourceLocations &locations, const SourceLocation &loc1, const rose_addr_t va2, const rose_addr_t va1) {
    locations.insert(loc1, va2);
    ASSERT_always_require(locations.get(loc1).size() == 2);
    ASSERT_always_require(locations.get(loc1).exists(va2));
    ASSERT_always_require(locations(loc1).orElse(911) == va1);
    ASSERT_always_require(locations(va2) == loc1);
    ASSERT_always_require(locations.firstAddress(loc1).orElse(911) == va1);
    SourceLocation loc = locations.nextSourceLocation();
    ASSERT_always_require(loc == loc1);
    ASSERT_always_require(locations.nextSourceLocation(loc1).isEmpty());
}

// Insert an item having the same address as another item. Since addresses can have only one source location, the first
// binding to the first location should be removed.
static void
insertSecondItemSameAddr(SourceLocations &locations, const SourceLocation &loc2, const rose_addr_t va1, const SourceLocation &loc1) {
    locations.insert(loc2, va1);
    ASSERT_always_require(locations.get(loc2).size() == 1);
    ASSERT_always_require(locations.get(loc2).exists(va1));
    ASSERT_always_require(locations.get(loc1).size() == 1);
    ASSERT_always_forbid(locations.get(loc1).exists(va1));
    ASSERT_always_require(locations.firstAddress(loc1).get() != va1);
    ASSERT_always_require(locations.get(va1) == loc2);
    SourceLocation loc = locations.nextSourceLocation();
    ASSERT_always_require(loc == loc1);
    ASSERT_always_require(locations.nextSourceLocation(loc) == loc2); // same name but higher line number
}

int main() {
    SourceLocations locations;
    emptyTests(locations);

    const SourceLocation loc1("/dev/urandom", 100);
    const SourceLocation loc2("/dev/urandom", 200);
    const rose_addr_t va1 = 0x08040010;
    const rose_addr_t va2 = 0x08040011;

    insertFirstItem(locations, loc1, va1);
    insertSecondItemSameSource(locations, loc1, va2, va1);
    insertSecondItemSameAddr(locations, loc2, va1, loc1);
}
