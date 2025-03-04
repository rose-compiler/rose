#ifndef ROSE_Extent_H
#define ROSE_Extent_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>

#include <rangemap.h>                                   // rose

#include <string>
#include <stdio.h>

// Deprecated. Use Rose::BinaryAnalysis::AddressInterval for all new code.
typedef Range<Rose::BinaryAnalysis::Address> Extent;

// Deprecated. Use Rose::BinaryAnalysis::AddressIntervalSet instead for all new code.
class ExtentMap: public RangeMap<Extent> {
public:
        ExtentMap(): RangeMap<Extent>() {}
        template<class Other> ExtentMap(const Other &other): RangeMap<Extent>(other) {}
        static char category(const Extent &a, const Extent &b);
        ExtentMap subtract_from(const Extent &e) const {
                return invert_within<ExtentMap>(e);
        }
        void allocate_at(const Extent &request);
        Extent allocate_best_fit(const Rose::BinaryAnalysis::Address size);
        Extent allocate_first_fit(const Rose::BinaryAnalysis::Address size);
        void dump_extents(std::ostream&, const std::string &prefix="", const std::string &label="") const;
        void dump_extents(FILE *f, const char *prefix, const char *label, bool pad=true) const;
};

// deprecated. Conversion functions, to be deleted when the old types are no longer needed.
Extent toExtent(const Rose::BinaryAnalysis::AddressInterval&);
Rose::BinaryAnalysis::AddressInterval toAddressInterval(const Extent&);
ExtentMap toExtentMap(const Rose::BinaryAnalysis::AddressIntervalSet&);
Rose::BinaryAnalysis::AddressIntervalSet toAddressIntervalSet(const ExtentMap&);

#endif
#endif
