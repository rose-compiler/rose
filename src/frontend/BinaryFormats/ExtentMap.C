// Use AddressInterval or similar for all new code. I plan to eventually remove ExtentMap one. [Robb Matzke 2021-09-14]

/* The ExtentMap class. This class is similar std::map<rose_addr_t,rose_addr_t> where the two addresses are the starting
 * offset and size.  The main difference is that if two adjacent extents are added to the map they will be condensed into a
 * single extent.  This class is used to keep track of what parts of a binary file have been parsed, and is also used to
 * manage string table free lists, among other things. */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>

#include <boost/foreach.hpp>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

Extent toExtent(const AddressInterval &x) {
    return x.isEmpty() ? Extent() : Extent::inin(x.least(), x.greatest());
}

AddressInterval toAddressInterval(const Extent &x) {
    return x.empty() ? AddressInterval() : AddressInterval::hull(x.first(), x.last());
}

ExtentMap toExtentMap(const AddressIntervalSet &x) {
    ExtentMap retval;
    BOOST_FOREACH (const AddressInterval &interval, x.intervals())
        retval.insert(toExtent(interval));
    return retval;
}

AddressIntervalSet toAddressIntervalSet(const ExtentMap &x) {
    AddressIntervalSet retval;
    for (ExtentMap::const_iterator iter=x.begin(); iter!=x.end(); ++iter)
        retval.insert(toAddressInterval(iter->first));
    return retval;
}

std::ostream& operator<<(std::ostream &out, const AddressInterval &x) {
    if (x.isEmpty()) {
        out <<"empty";
    } else if (x.isSingleton()) {
        out <<StringUtility::addrToString(x.least());
    } else {
        out <<"[" <<StringUtility::addrToString(x.least()) <<"," <<StringUtility::addrToString(x.greatest()) <<"]";
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, const AddressIntervalSet &x) {
    out <<"{";
    BOOST_FOREACH (const AddressInterval &interval, x.intervals())
        out <<" " <<interval;
    out <<" }";
    return out;
}

/* Class method comparing two extents. The return value is one of the following letters, depending on how extent A is related
 * to extent B:
 *     C (congruent):  A and B are congruent
 *     L (left):       A is left of B
 *     R (right):      A is right of B
 *     O (outer):      A contains B, but A and B are not congruent
 *     I (inner):      A is contained by B, but A and B are not congruent
 *     B (beginning):  A overlaps with the beginning of B but does not contain B
 *     E (ending):     A overlaps with the end of B but does not contain B */
char
ExtentMap::category(const Extent &a, const Extent &b)
{
    if (a.relaxed_first()==b.relaxed_first() && a.size()==b.size())
        return 'C';
    if (a.relaxed_first()+a.size() <= b.relaxed_first())
        return 'L';
    if (a.relaxed_first() >= b.relaxed_first()+b.size())
        return 'R';
    if (a.relaxed_first() <= b.relaxed_first() && a.relaxed_first()+a.size() >= b.relaxed_first()+b.size())
        return 'O';
    if (a.relaxed_first() >= b.relaxed_first() && a.relaxed_first()+a.size() <= b.relaxed_first()+b.size())
        return 'I';
    if (a.relaxed_first() <= b.relaxed_first()) /*already know a.first+a.size > b.first*/
        return 'B';
    return 'E';
}

/* Allocate an extent of the specified size (best fit first) from the extent map, removing the returned extent from the map. */
Extent
ExtentMap::allocate_best_fit(rose_addr_t size)
{
    iterator found = best_fit(size, begin());
    if (found==end())
        throw std::bad_alloc();
    Extent retval(found->first.first(), size);
    erase(retval);
    return retval;
}

/* Allocate an extent of the specified size (first fit) from the extent map, removing the returned extent from the map. */
Extent
ExtentMap::allocate_first_fit(rose_addr_t size)
{
    iterator found = first_fit(size, begin());
    if (found==end())
        throw std::bad_alloc();
    Extent retval(found->first.first(), size);
    erase(retval);
    return retval;
}

/* Allocate the specified extent, which must be in the free list. */
void
ExtentMap::allocate_at(const Extent &request)
{
    ROSE_ASSERT(subtract_from(request).size()==0); /*entire request should be on free list*/
    erase(request);
}

void
ExtentMap::dump_extents(std::ostream &o, const std::string &prefix, const std::string &label) const
{
    using namespace StringUtility;
    size_t idx=0;
    for (const_iterator i=begin(); i!=end(); ++i, ++idx) {
        o <<prefix <<(label.empty()?std::string("Extent"):label) <<"[" <<idx <<"]"
          <<" = offset " <<unsignedToHex(i->first.first())
          <<" for " <<unsignedToHex(i->first.size()) <<(1==i->first.size()?" byte":" bytes")
          <<" ending at " <<unsignedToHex(i->first.first() + i->first.size()) <<"\n";
    }
}
    

/* Print info about an extent map. This is a little different format than the ostream "<<" operator. */
void
ExtentMap::dump_extents(FILE *f, const char *prefix, const char *label, bool pad) const
{
    char p[4096];
    size_t idx=0;
    for (const_iterator i=begin(); i!=end(); ++i, ++idx) {
        if (!label) label = "Extent";
        snprintf(p, sizeof(p), "%s%s[%zd]", prefix, label, idx);
        int w = pad ? std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p)) : 1;
        fprintf(f, "%s%-*s = offset 0x%08" PRIx64 " (%" PRIu64 "),"
                " for 0x%08" PRIx64 " (%" PRIu64 ") byte%s,"
                " ending at 0x%08" PRIx64 " (%" PRIu64 ")\n",
                p, w, "", i->first.first(), i->first.first(),
                i->first.size(), i->first.size(), 1==i->first.size()?"":"s", 
                i->first.first()+i->first.size(), i->first.first()+i->first.size());
    }
}

#endif
