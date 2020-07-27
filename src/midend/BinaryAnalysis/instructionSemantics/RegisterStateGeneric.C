#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <RegisterStateGeneric.h>

#include <boost/algorithm/string/erase.hpp>
#include <boost/format.hpp>
#include <Diagnostics.h>
#include <FormatRestorer.h>
#include <boost/format.hpp>

// Define this if you want extra consistency checking before and after each mutator.  This slows things down considerably but
// can be useful for narrowing down logic errors in the implementation.
//#define RegisterStateGeneric_ExtraAssertions

// Define this if you want the readRegister behavior as it existed before 2015-09-24. This behavior was wrong in certain ways
// because it didn't always cause registers to spring into existence the first time they were read.
//#define RegisterStateGeneric_20150924

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

using namespace Rose::Diagnostics;

static bool
sortByOffset(const RegisterStateGeneric::RegPair &a, const RegisterStateGeneric::RegPair &b) {
    return a.desc.offset() < b.desc.offset();
}

void
RegisterStateGeneric::clear()
{
    registers_.clear();
    eraseWriters();
}

void
RegisterStateGeneric::zero()
{
    // We're initializing with a concrete value, so it really doesn't matter whether we initialize large registers
    // or small registers.  Constant folding will adjust things as necessary when we start reading registers.
    std::vector<RegisterDescriptor> regs = regdict->get_largest_registers();
    initialize_nonoverlapping(regs, true);
}

void
RegisterStateGeneric::initialize_large()
{
    ASSERT_not_null(regdict);
    std::vector<RegisterDescriptor> regs = regdict->get_largest_registers();
    initialize_nonoverlapping(regs, false);
}

void
RegisterStateGeneric::initialize_small()
{
    std::vector<RegisterDescriptor> regs = regdict->get_smallest_registers();
    initialize_nonoverlapping(regs, false);
}

void
RegisterStateGeneric::initialize_nonoverlapping(const std::vector<RegisterDescriptor> &regs, bool initialize_to_zero)
{
    clear();
    for (size_t i=0; i<regs.size(); ++i) {
        std::string name = regdict->lookup(regs[i]);
        SValuePtr val;
        if (initialize_to_zero) {
            val = protoval()->number_(regs[i].nBits(), 0);
        } else {
            val = protoval()->undefined_(regs[i].nBits());
            if (!name.empty() && val->get_comment().empty())
                val->set_comment(name+"_0");
        }
        registers_.insertMaybeDefault(regs[i]).push_back(RegPair(regs[i], val));
    }
}

static bool
has_null_value(const RegisterStateGeneric::RegPair &rp)
{
    return rp.value == NULL;
}

void
RegisterStateGeneric::assertStorageConditions(const std::string &when, RegisterDescriptor reg) const {
#if !defined(NDEBUG) && defined(RegisterStateGeneric_ExtraAssertions)
#if 1 // DEBUGGING [Robb P. Matzke 2015-09-28]
    static volatile size_t ncalls = 0;
    ++ncalls;
#endif
    std::ostringstream error;
    BOOST_FOREACH (const Registers::Node &rnode, registers_.nodes()) {
        Sawyer::Container::IntervalSet<BitRange> foundLocations;
        BOOST_FOREACH (const RegPair &regpair, rnode.value()) {
            if (!regpair.desc.is_valid()) {
                error <<"invalid register descriptor";
            } else if (regpair.desc.get_major() != rnode.key().majr || regpair.desc.minorNumber() != rnode.key().minr) {
                error <<"register is in wrong list; register=" <<regpair.desc.get_major() <<"." <<regpair.desc.minorNumber()
                      <<", list=" <<rnode.key().majr <<"." <<rnode.key().minr;
            } else if (regpair.value == NULL) {
                error <<"value is null for register " <<regpair.desc;
            } else if (regpair.value->get_width() != regpair.desc.nBits()) {
                error <<"value width (" <<regpair.value->get_width() <<") is incorrect for register " <<regpair.desc;
            } else if (foundLocations.isOverlapping(regpair.location())) {
                error <<"register " <<regpair.desc <<" is stored multiple times in the list";
            }
            foundLocations.insert(regpair.location());
            if (!error.str().empty())
                break;
        }
        if (!error.str().empty()) {
            mlog[FATAL] <<when <<" register " <<reg <<":\n";
            mlog[FATAL] <<"  " <<error.str() <<"\n";
            mlog[FATAL] <<"  related registers:\n";
            BOOST_FOREACH (const RegPair &regpair, rnode.value()) {
                mlog[FATAL] <<"    " <<regpair.desc;
                if (regpair.value == NULL)
                    mlog[FATAL] <<"\tnull value";
                mlog[FATAL] <<"\n";
            }
            abort();
        }
    }
#endif
}

void
RegisterStateGeneric::scanAccessedLocations(RegisterDescriptor reg, RiscOperators *ops,
                                            RegPairs &accessedParts /*out*/, RegPairs &preservedParts /*out*/) const {
    BitRange accessedLocation = BitRange::baseSize(reg.offset(), reg.nBits());
    const RegPairs &pairList = registers_.getOrDefault(reg);
    BOOST_FOREACH (const RegPair &regpair, pairList) {
        BitRange storedLocation = regpair.location();   // the thing that's already stored in this state
        BitRange overlap = storedLocation & accessedLocation;
        if (overlap.isEmpty())
            continue;

        // Low-order bits of stored part that are not needed for this access
        if (overlap.least() > storedLocation.least()) {
            size_t nbits = overlap.least() - storedLocation.least();
            RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), storedLocation.least(), nbits);
            preservedParts.push_back(RegPair(subreg, ops->unsignedExtend(regpair.value, nbits)));
        }

        // Bits of the part that are needed for this access.
        {
            RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), overlap.least(), overlap.size());
            size_t extractBegin = overlap.least() - storedLocation.least();
            size_t extractEnd = extractBegin + overlap.size();
            accessedParts.push_back(RegPair(subreg, ops->extract(regpair.value, extractBegin, extractEnd)));
        }

        // High-order bits of stored part that are not needed for this access
        if (overlap.greatest() < storedLocation.greatest()) {
            size_t nbits = storedLocation.greatest() - overlap.greatest();
            RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), overlap.greatest()+1, nbits);
            size_t extractBegin = overlap.greatest()+1 - storedLocation.least();
            size_t extractEnd = extractBegin + nbits;
            preservedParts.push_back(RegPair(subreg, ops->extract(regpair.value, extractBegin, extractEnd)));
        }
    }
}

void
RegisterStateGeneric::clearOverlappingLocations(RegisterDescriptor reg) {
    BitRange accessedLocation = BitRange::baseSize(reg.offset(), reg.nBits());
    RegPairs emptyPairList;
    RegPairs &pairList = registers_.getOrElse(reg, emptyPairList);
    BOOST_FOREACH (RegPair &regpair, pairList) {
        BitRange storedLocation = regpair.location();
        BitRange overlap = storedLocation & accessedLocation;
        if (!overlap.isEmpty())
            regpair.value = SValuePtr();
    }
}

SValuePtr
RegisterStateGeneric::readRegister(RegisterDescriptor reg, const SValuePtr &dflt, RiscOperators *ops) {
    ASSERT_forbid(reg.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_require2(reg.nBits() == dflt->get_width(), "value being read must be same size as register" +
                    (boost::format(": %|u| -> %|u|") % reg.nBits() % dflt->get_width()).str());
    ASSERT_not_null(ops);
    assertStorageConditions("at start of read", reg);
    BitRange accessedLocation = BitRange::baseSize(reg.offset(), reg.nBits());
#ifdef RegisterStateGeneric_20150924
    const bool adjustLocations = false;
#else
    const bool adjustLocations = accessModifiesExistingLocations_;
#endif

    // Fast case: the state does not store this register or any register that might overlap with this register.
    if (!registers_.exists(reg)) {
        if (!accessCreatesLocations_)
            return dflt;
        SValuePtr newval = dflt->copy();
        std::string regname = regdict->lookup(reg);
        boost::erase_all(regname, "[");
        boost::erase_all(regname, "]");
        if (!regname.empty() && newval->get_comment().empty())
            newval->set_comment(regname + "_0");
        registers_.insertMaybeDefault(reg).push_back(RegPair(reg, newval));
        assertStorageConditions("at end of read", reg);
        return newval;
    }

    // Iterate over the storage/value pairs to figure out what parts of the register are already in existing storage locations,
    // and which parts of those overlapping storage locations are not accessed.
    RegPairs accessedParts;                             // parts of existing overlapping locations we access
    RegPairs preservedParts;                            // parts of existing overlapping locations we don't access
    RegPairs &pairList = registers_.insertMaybeDefault(reg);
    scanAccessedLocations(reg, ops, accessedParts /*out*/, preservedParts /*out*/);
    if (adjustLocations)
        clearOverlappingLocations(reg);

    // Figure out which part of the access does not exist in the state
    typedef Sawyer::Container::IntervalSet<BitRange> Locations;
    Locations newLocations;
    newLocations.insert(accessedLocation);
    BOOST_FOREACH (const RegPair &regpair, accessedParts)
        newLocations -= regpair.location();

    // Create values for the parts of the accessed register that weren't stored in the state, but don't store them yet.
    RegPairs newParts;
    BOOST_FOREACH (const BitRange &newLocation, newLocations.intervals()) {
        RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), newLocation.least(), newLocation.size());
        ASSERT_require(newLocation.least() >= reg.offset());
        SValuePtr newval = ops->extract(dflt,
                                        newLocation.least()-reg.offset(),
                                        newLocation.greatest()+1-reg.offset());
        newParts.push_back(RegPair(subreg, newval));
    }

    // Construct the return value by combining all the parts we found or created.
    SValuePtr retval;
    RegPairs retvalParts = accessedParts;
    retvalParts.insert(retvalParts.end(), newParts.begin(), newParts.end());
    std::sort(retvalParts.begin(), retvalParts.end(), sortByOffset);
    BOOST_FOREACH (const RegPair &regpair, retvalParts)
        retval = retval ? ops->concat(retval, regpair.value) : regpair.value;
    ASSERT_require(retval->get_width() == reg.nBits());

    // Update the register state -- write parts that didn't exist and maybe combine or split some locations.
    if (adjustLocations) {
        // Remove parts of the pair list we're about to replace.
        pairList.erase(std::remove_if(pairList.begin(), pairList.end(), has_null_value), pairList.end());
        pairList.insert(pairList.end(), preservedParts.begin(), preservedParts.end()); // re-insert preserved parts
        if (accessCreatesLocations_) {
            // Combine all the accessed locations (just removed above) into a single location corresponding exactly to the
            // return value.
            pairList.push_back(RegPair(reg, retval));
        } else {
            // Add back the parts that were originally present and which we accessed, but not any new parts. They're added back
            // in as large chunks as possible.
            std::sort(accessedParts.begin(), accessedParts.end(), sortByOffset);
            for (size_t i = 0; i<accessedParts.size(); ++i /*also incremented in body*/) {
                // Find largest contiguous value starting at i.
                SValuePtr part = accessedParts[i].value;
                size_t firstOffset = accessedParts[i].desc.offset();
                size_t nextOffset = firstOffset + accessedParts[i].desc.nBits();
                while (i+1 < accessedParts.size() && accessedParts[i+1].desc.offset() == nextOffset) {
                    ++i;
                    nextOffset += accessedParts[i].desc.nBits();
                    part = ops->concat(part, accessedParts[i].value);
                }
                RegisterDescriptor tmpReg(reg.majorNumber(), reg.minorNumber(), firstOffset, nextOffset-firstOffset);
                ASSERT_require(tmpReg.nBits() == part->get_width());
                pairList.push_back(RegPair(tmpReg, part));
            }
        }
    } else if (accessCreatesLocations_) {
        // Don't adjust existing locations, just add locations that were not existing.
        pairList.insert(pairList.end(), newParts.begin(), newParts.end());
    }

    assertStorageConditions("at end of read", reg);
    return retval;
}

BaseSemantics::SValuePtr
RegisterStateGeneric::peekRegister(RegisterDescriptor reg, const SValuePtr &dflt, RiscOperators *ops) {
    ASSERT_forbid(reg.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_require2(reg.nBits() == dflt->get_width(), "value being read must be same size as register" +
                    (boost::format(": %|u| -> %|u|") % reg.nBits() % dflt->get_width()).str());
    ASSERT_not_null(ops);
    assertStorageConditions("at start of read", reg);
    BitRange accessedLocation = BitRange::baseSize(reg.offset(), reg.nBits());

    if (!registers_.exists(reg))
        return dflt;                                    // no part of the register is stored in the state

    // Iterate over the storage/value pairs to figure out what parts of the register are already in existing storage locations,
    // and which parts of those overlapping storage locations are not accessed.
    RegPairs accessedParts;                             // parts of existing overlapping locations we access
    RegPairs preservedParts;                            // parts of existing overlapping locations we don't access
    scanAccessedLocations(reg, ops, accessedParts /*out*/, preservedParts /*out*/);

    // Figure out which part of the access does not exist in the state
    typedef Sawyer::Container::IntervalSet<BitRange> Locations;
    Locations newLocations;
    newLocations.insert(accessedLocation);
    BOOST_FOREACH (const RegPair &regpair, accessedParts)
        newLocations -= regpair.location();

    // Create values for the parts of the accessed register that weren't stored in the state
    RegPairs newParts;
    BOOST_FOREACH (const BitRange &newLocation, newLocations.intervals()) {
        RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), newLocation.least(), newLocation.size());
        ASSERT_require(newLocation.least() >= reg.offset());
        SValuePtr newval = ops->extract(dflt,
                                        newLocation.least()-reg.offset(),
                                        newLocation.greatest()+1-reg.offset());
        newParts.push_back(RegPair(subreg, newval));
    }

    // Construct the return value by combining all the parts we found or created.
    SValuePtr retval;
    RegPairs retvalParts = accessedParts;
    retvalParts.insert(retvalParts.end(), newParts.begin(), newParts.end());
    std::sort(retvalParts.begin(), retvalParts.end(), sortByOffset);
    BOOST_FOREACH (const RegPair &regpair, retvalParts)
        retval = retval ? ops->concat(retval, regpair.value) : regpair.value;
    ASSERT_require(retval->get_width() == reg.nBits());

    assertStorageConditions("at end of peek", reg);
    return retval;
}

void
RegisterStateGeneric::writeRegister(RegisterDescriptor reg, const SValuePtr &value, RiscOperators *ops)
{
    ASSERT_not_null(value);
    ASSERT_require2(reg.nBits()==value->get_width(), "value written to register must be the same width as the register" +
                    (boost::format(": %|u| -> %|u|") % value->get_width() % reg.nBits()).str());
    ASSERT_not_null(ops);
    assertStorageConditions("at start of write", reg);
    BitRange accessedLocation = BitRange::baseSize(reg.offset(), reg.nBits());

    // Fast case: the state does not store this register or any register that might overlap with this register.
    if (!registers_.exists(reg)) {
        if (!accessCreatesLocations_)
            throw RegisterNotPresent(reg);
        registers_.insertMaybeDefault(reg).push_back(RegPair(reg, value));
        assertStorageConditions("at end of write", reg);
        return;
    }

    // Check that we're allowed to add storage locations if necessary.
    if (!accessCreatesLocations_) {
        size_t nBitsFound = 0;
        BOOST_FOREACH (const RegPair &regpair, registers_.getOrDefault(reg))
            nBitsFound += (regpair.location() & accessedLocation).size();
        ASSERT_require(nBitsFound <= accessedLocation.size());
        if (nBitsFound < accessedLocation.size())
            throw RegisterNotPresent(reg);
    }

    // Iterate over the storage/value pairs to figure out what parts of the register are already in existing storage locations,
    // and which parts of those overlapping storage locations are not accessed.
    RegPairs accessedParts;                             // parts of existing overlapping locations we access
    RegPairs preservedParts;                            // parts of existing overlapping locations we don't access
    RegPairs &pairList = registers_.insertMaybeDefault(reg);
    scanAccessedLocations(reg, ops, accessedParts /*out*/, preservedParts /*out*/);
    if (accessModifiesExistingLocations_)
        clearOverlappingLocations(reg);

    // Figure out which part of the access does not exist in the state
    typedef Sawyer::Container::IntervalSet<BitRange> Locations;
    Locations newLocations;
    newLocations.insert(accessedLocation);
    BOOST_FOREACH (const RegPair &regpair, accessedParts)
        newLocations -= regpair.location();

    // Update the register state by writing to the accessed area.
    if (accessModifiesExistingLocations_) {
        // Combine all the accessed locations into a single location corresponding exactly to the written value, and split
        // those previously existing locations that partly overlap the written location.
        pairList.erase(std::remove_if(pairList.begin(), pairList.end(), has_null_value), pairList.end());
        pairList.insert(pairList.end(), preservedParts.begin(), preservedParts.end());
        pairList.push_back(RegPair(reg, value));
    } else {
        // Don't insert/erase locations that existed already -- only change their values.
        BOOST_FOREACH (RegPair &regpair, pairList) {
            BitRange storedLocation = regpair.location();
            if (BitRange overlap = storedLocation & accessedLocation) {
                SValuePtr valueToWrite;
                if (overlap.least() > storedLocation.least()) {
                    size_t nbits = overlap.least() - storedLocation.least();
                    SValuePtr loValue = ops->unsignedExtend(regpair.value, nbits);
                    valueToWrite = loValue;
                }
                {
                    size_t nbits = overlap.size();
                    size_t extractBegin = overlap.least() - accessedLocation.least();
                    size_t extractEnd = extractBegin + nbits;
                    SValuePtr midValue = ops->extract(value, extractBegin, extractEnd);
                    valueToWrite = valueToWrite ? ops->concat(valueToWrite, midValue) : midValue;
                }
                if (overlap.greatest() < storedLocation.greatest()) {
                    size_t nbits = storedLocation.greatest() - overlap.greatest();
                    size_t extractBegin = overlap.greatest()+1 - storedLocation.least();
                    size_t extractEnd = extractBegin + nbits;
                    SValuePtr hiValue = ops->extract(regpair.value, extractBegin, extractEnd);
                    valueToWrite = valueToWrite ? ops->concat(valueToWrite, hiValue) : hiValue;
                }
                ASSERT_not_null(valueToWrite);
                ASSERT_require(valueToWrite->get_width() == regpair.desc.nBits());
                regpair.value = valueToWrite;
            }
        }

        // Insert the parts that didn't exist before
        BOOST_FOREACH (const BitRange &newLocation, newLocations.intervals()) {
            size_t extractBegin = newLocation.least() - accessedLocation.least();
            size_t extractEnd = extractBegin + newLocation.size();
            SValuePtr valueToWrite = ops->extract(value, extractBegin, extractEnd);
            ASSERT_require(valueToWrite->get_width() == newLocation.size());
            RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), newLocation.least(), newLocation.size());
            pairList.push_back(RegPair(subreg, valueToWrite));
        }
    }
    assertStorageConditions("at end of write", reg);
}

void
RegisterStateGeneric::updateWriteProperties(RegisterDescriptor reg, InputOutputProperty prop) {
    insertProperties(reg, prop);
    if (prop == IO_WRITE)
        eraseProperties(reg, IO_READ_AFTER_WRITE);
}

void
RegisterStateGeneric::updateReadProperties(RegisterDescriptor reg) {
    insertProperties(reg, IO_READ);
    BitProperties &props = properties_.insertMaybeDefault(reg);
    BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
    BOOST_FOREACH (BitProperties::Node &node, props.findAll(where)) {
        if (!node.value().exists(IO_WRITE)) {
            node.value().insert(IO_READ_BEFORE_WRITE);
            if (!node.value().exists(IO_INIT))
                node.value().insert(IO_READ_UNINITIALIZED);
        } else {
            node.value().insert(IO_READ_AFTER_WRITE);
        }
    }
}

void
RegisterStateGeneric::erase_register(RegisterDescriptor reg, RiscOperators *ops)
{
    ASSERT_forbid(reg.isEmpty());
    ASSERT_not_null(ops);
    assertStorageConditions("at start of erase", reg);
    BitRange accessedLocation = BitRange::baseSize(reg.offset(), reg.nBits());

    // Fast case: the state does not store this register or any register that might overlap with this register
    if (!registers_.exists(reg))
        return;                                         // no part of register is stored in this state
    RegPairs &pairList = registers_[reg];

    // Look for existing registers that overlap with this register and remove them.  If the overlap was only partial, then we
    // need to eventually add the non-overlapping part back into the list.
    RegPairs nonoverlaps; // the non-overlapping parts of overlapping registers
    Extent need_extent(reg.offset(), reg.nBits());
    BOOST_FOREACH (RegPair &reg_val, pairList) {
        BitRange haveLocation = reg_val.location();
        if (BitRange intersection = accessedLocation & haveLocation) {
            if (haveLocation.least() < intersection.least()) {
                size_t leftSize = intersection.least() - haveLocation.least();
                RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), haveLocation.least(), leftSize);
                nonoverlaps.push_back(RegPair(subreg, ops->unsignedExtend(reg_val.value, leftSize)));
            }
            if (intersection.greatest() < haveLocation.greatest()) {
                size_t rightSize = haveLocation.greatest() - intersection.greatest();
                size_t lobit = intersection.greatest()+1 - haveLocation.least();
                RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), intersection.greatest()+1, rightSize);
                nonoverlaps.push_back(RegPair(subreg, ops->extract(reg_val.value, lobit, lobit+rightSize)));
            }
            reg_val.value = SValuePtr();
        }
    }

    // Remove marked pairs, then add the non-overlapping parts.
    pairList.erase(std::remove_if(pairList.begin(), pairList.end(), has_null_value), pairList.end());
    pairList.insert(pairList.end(), nonoverlaps.begin(), nonoverlaps.end());
    assertStorageConditions("at end of erase", reg);
}

RegisterStateGeneric::RegPairs
RegisterStateGeneric::get_stored_registers() const
{
    RegPairs retval;
    BOOST_FOREACH (const RegPairs &pairlist, registers_.values())
        retval.insert(retval.end(), pairlist.begin(), pairlist.end());
    return retval;
}

void
RegisterStateGeneric::traverse(Visitor &visitor)
{
    BOOST_FOREACH (RegPairs &pairlist, registers_.values()) {
        BOOST_FOREACH (RegPair &pair, pairlist) {
            if (SValuePtr newval = (visitor)(pair.desc, pair.value)) {
                ASSERT_require(newval->get_width() == pair.desc.nBits());
                pair.value = newval;
            }
        }
    }
}

void
RegisterStateGeneric::deep_copy_values()
{
    BOOST_FOREACH (RegPairs &pairlist, registers_.values()) {
        BOOST_FOREACH (RegPair &pair, pairlist)
            pair.value = pair.value->copy();
    }
}

bool
RegisterStateGeneric::is_partly_stored(RegisterDescriptor desc) const
{
    BitRange want = BitRange::baseSize(desc.offset(), desc.nBits());
    BOOST_FOREACH (const RegPair &pair, registers_.getOrDefault(desc)) {
        if (want & pair.location())
            return true;
    }
    return false;
}

bool
RegisterStateGeneric::is_wholly_stored(RegisterDescriptor desc) const
{
    Sawyer::Container::IntervalSet<BitRange> desired;
    desired.insert(BitRange::baseSize(desc.offset(), desc.nBits()));
    BOOST_FOREACH (const RegPair &pair, registers_.getOrDefault(desc))
        desired -= pair.location();
    return desired.isEmpty();
}

bool
RegisterStateGeneric::is_exactly_stored(RegisterDescriptor desc) const
{
    BOOST_FOREACH (const RegPair &pair, registers_.getOrDefault(desc)) {
        if (desc == pair.desc)
            return true;
    }
    return false;
}

ExtentMap
RegisterStateGeneric::stored_parts(RegisterDescriptor desc) const
{
    ExtentMap retval;
    Extent want(desc.offset(), desc.nBits());
    BOOST_FOREACH (const RegPair &pair, registers_.getOrDefault(desc)) {
        Extent have(pair.desc.offset(), pair.desc.nBits());
        retval.insert(want.intersect(have));
    }
    return retval;
}

RegisterStateGeneric::RegPairs
RegisterStateGeneric::overlappingRegisters(RegisterDescriptor needle) const {
    ASSERT_forbid(needle.isEmpty());
    BitRange needleBits = BitRange::baseSize(needle.offset(), needle.nBits());
    RegPairs retval;
    BOOST_FOREACH (const RegPair &pair, registers_.getOrDefault(needle)) {
        if (needleBits & pair.location())
            retval.push_back(pair);
    }
    return retval;
}

bool
RegisterStateGeneric::insertWriters(RegisterDescriptor desc, const AddressSet &writerVas) {
    if (writerVas.isEmpty())
        return false;
    BitAddressSet &parts = writers_.insertMaybeDefault(desc);
    BitRange where = BitRange::baseSize(desc.offset(), desc.nBits());
    return parts.insert(where, writerVas);
}

void
RegisterStateGeneric::eraseWriters(RegisterDescriptor desc, const AddressSet &writerVas) {
    if (writerVas.isEmpty() || !writers_.exists(desc))
        return;
    BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.offset(), desc.nBits());
    parts.erase(where, writerVas);
    if (parts.isEmpty())
        writers_.erase(desc);
}

void
RegisterStateGeneric::setWriters(RegisterDescriptor desc, const AddressSet &writerVas) {
    if (writerVas.isEmpty()) {
        eraseWriters(desc);
    } else {
        BitAddressSet &parts = writers_.insertMaybeDefault(desc);
        BitRange where = BitRange::baseSize(desc.offset(), desc.nBits());
        parts.replace(where, writerVas);
    }
}

void
RegisterStateGeneric::eraseWriters(RegisterDescriptor desc) {
    if (!writers_.exists(desc))
        return;
    BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.offset(), desc.nBits());
    parts.erase(where);
    if (parts.isEmpty())
        writers_.erase(desc);
}

void
RegisterStateGeneric::eraseWriters() {
    writers_.clear();
}

bool
RegisterStateGeneric::hasWritersAny(RegisterDescriptor desc) const {
    if (!writers_.exists(desc))
        return false;
    const BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.offset(), desc.nBits());
    return parts.isOverlapping(where);
}

bool
RegisterStateGeneric::hasWritersAll(RegisterDescriptor desc) const {
    if (!writers_.exists(desc))
        return false;
    const BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.offset(), desc.nBits());
    return parts.contains(where);
}

RegisterStateGeneric::AddressSet
RegisterStateGeneric::getWritersUnion(RegisterDescriptor desc) const {
    if (!writers_.exists(desc))
        return AddressSet();
    const BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.offset(), desc.nBits());
    return parts.getUnion(where);
}

RegisterStateGeneric::AddressSet
RegisterStateGeneric::getWritersIntersection(RegisterDescriptor desc) const {
    if (!writers_.exists(desc))
        return AddressSet();
    const BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.offset(), desc.nBits());
    return parts.getIntersection(where);
}

bool
RegisterStateGeneric::hasPropertyAny(RegisterDescriptor reg, InputOutputProperty prop) const {
    if (!properties_.exists(reg))
        return false;
    const BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
    return bitProps.existsAnywhere(where, prop);
}

bool
RegisterStateGeneric::hasPropertyAll(RegisterDescriptor reg, InputOutputProperty prop) const {
    if (!properties_.exists(reg))
        return false;
    const BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
    return bitProps.existsEverywhere(where, prop);
}

InputOutputPropertySet
RegisterStateGeneric::getPropertiesUnion(RegisterDescriptor reg) const {
    if (!properties_.exists(reg))
        return InputOutputPropertySet();
    const BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
    return bitProps.getUnion(where);
}

InputOutputPropertySet
RegisterStateGeneric::getPropertiesIntersection(RegisterDescriptor reg) const {
    if (!properties_.exists(reg))
        return InputOutputPropertySet();
    const BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
    return bitProps.getIntersection(where);
}

bool
RegisterStateGeneric::insertProperties(RegisterDescriptor reg, const InputOutputPropertySet &props) {
    if (props.isEmpty())
        return false;
    BitProperties &bitProps = properties_.insertMaybeDefault(reg);
    BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
    return bitProps.insert(where, props);
}

bool
RegisterStateGeneric::eraseProperties(RegisterDescriptor reg, const InputOutputPropertySet &props) {
    if (props.isEmpty() || !properties_.exists(reg))
        return false;
    BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
    bool changed = bitProps.erase(where, props);
    if (bitProps.isEmpty())
        properties_.erase(reg);
    return changed;
}

void
RegisterStateGeneric::setProperties(RegisterDescriptor reg, const InputOutputPropertySet &props) {
    if (props.isEmpty()) {
        eraseProperties(reg);
    } else {
        BitProperties &bitProps = properties_.insertMaybeDefault(reg);
        BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
        bitProps.replace(where, props);
    }
}

void
RegisterStateGeneric::eraseProperties(RegisterDescriptor reg) {
    if (!properties_.exists(reg))
        return;
    BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.offset(), reg.nBits());
    bitProps.erase(where);
    if (bitProps.isEmpty())
        properties_.erase(reg);
}

void
RegisterStateGeneric::eraseProperties() {
    properties_.clear();
}

std::vector<RegisterDescriptor>
RegisterStateGeneric::findProperties(const InputOutputPropertySet &required, const InputOutputPropertySet &prohibited) const {
    std::vector<RegisterDescriptor> retval;
    typedef Sawyer::Container::IntervalSet<BitRange> Bits;
    BOOST_FOREACH (const RegisterProperties::Node &regNode, properties_.nodes()) {
        unsigned majr = regNode.key().majr;
        unsigned minr = regNode.key().minr;
        Bits bits;
        BOOST_FOREACH (const BitProperties::Node &bitNode, regNode.value().nodes()) {
            if (bitNode.value().existsAll(required) && !bitNode.value().existsAny(prohibited))
                bits.insert(bitNode.key());
        }
        BOOST_FOREACH (const BitRange &bitRange, bits.intervals()) {
            RegisterDescriptor reg(majr, minr, bitRange.least(), bitRange.size());
            retval.push_back(reg);
        }
    }
    return retval;
}

bool
RegisterStateGeneric::merge(const BaseSemantics::RegisterStatePtr &other_, RiscOperators *ops) {
    ASSERT_not_null(ops);
    RegisterStateGenericPtr other = boost::dynamic_pointer_cast<RegisterStateGeneric>(other_);
    ASSERT_not_null(other);
    bool changed = false;

    // Merge values stored in registers.
    BOOST_FOREACH (const RegPair &otherRegVal, other->get_stored_registers()) {
        RegisterDescriptor otherReg = otherRegVal.desc;
        const BaseSemantics::SValuePtr &otherValue = otherRegVal.value;
        BaseSemantics::SValuePtr dflt = ops->undefined_(otherReg.nBits());
        BaseSemantics::SValuePtr thisValue = readRegister(otherReg, dflt, ops);
        if (BaseSemantics::SValuePtr merged = thisValue->createOptionalMerge(otherValue, merger(), ops->solver()).orDefault()) {
            writeRegister(otherReg, merged, ops);
            changed = true;
        }
    }

    // Merge writer sets.
    BOOST_FOREACH (const RegisterAddressSet::Node &wmNode, other->writers_.nodes()) {
        const BitAddressSet &otherWriters = wmNode.value();
        BitAddressSet &thisWriters = writers_.insertMaybeDefault(wmNode.key());
        BOOST_FOREACH (const BitAddressSet::Node &otherWritten, otherWriters.nodes()) {
            bool inserted = thisWriters.insert(otherWritten.key(), otherWritten.value());
            if (inserted)
                changed = true;
        }
    }

    // Merge property sets.
    BOOST_FOREACH (const RegisterProperties::Node &otherRegNode, other->properties_.nodes()) {
        const BitProperties &otherBitProps = otherRegNode.value();
        BitProperties &thisBitProps = properties_.insertMaybeDefault(otherRegNode.key());
        BOOST_FOREACH (const BitProperties::Node &otherBitNode, otherBitProps.nodes()) {
            bool inserted = thisBitProps.insert(otherBitNode.key(), otherBitNode.value());
            if (inserted)
                changed = true;
        }
    }

    return changed;
}

void
RegisterStateGeneric::print(std::ostream &stream, Formatter &fmt) const
{
    const RegisterDictionary *regdict = fmt.get_register_dictionary();
    if (!regdict)
        regdict = get_register_dictionary();
    RegisterNames regnames(regdict);

    // First pass is to get the maximum length of the register names; second pass prints
    FormatRestorer oflags(stream);
    size_t maxlen = 6; // use at least this many columns even if register names are short.
    for (int i=0; i<2; ++i) {
        BOOST_FOREACH (const RegPairs &pl, registers_.values()) {
            RegPairs regPairs = pl;
            std::sort(regPairs.begin(), regPairs.end(), sortByOffset);
            BOOST_FOREACH (const RegPair &pair, regPairs) {
                std::string regname = regnames(pair.desc);
                if (!fmt.get_suppress_initial_values() || pair.value->get_comment().empty() ||
                    0!=pair.value->get_comment().compare(regname+"_0")) {
                    if (0==i) {
                        maxlen = std::max(maxlen, regname.size());
                    } else {
                        stream <<fmt.get_line_prefix() <<std::setw(maxlen) <<std::left <<regname;
                        oflags.restore();
                        if (fmt.get_show_latest_writers()) {
                            // FIXME[Robb P. Matzke 2015-08-12]: This doesn't take into account that different writer sets can
                            // exist for different parts of the register.
                            AddressSet writers = getWritersUnion(pair.desc);
                            if (writers.size()==1) {
                                stream <<" [writer=" <<StringUtility::addrToString(*writers.values().begin()) <<"]";
                            } else if (!writers.isEmpty()) {
                                stream <<" [writers={";
                                for (Sawyer::Container::Set<rose_addr_t>::ConstIterator wi=writers.values().begin();
                                     wi!=writers.values().end(); ++wi) {
                                    stream <<(wi==writers.values().begin()?"":", ") <<StringUtility::addrToString(*wi);
                                }
                                stream <<"}]";
                            }
                        }

                        // FIXME[Robb P. Matzke 2015-08-12]: This doesn't take into account that different property sets can
                        // exist for different parts of the register.  It also doesn't take into account all combinations f
                        // properties -- just a few of the more common ones.
                        if (fmt.get_show_properties()) {
                            InputOutputPropertySet props = getPropertiesUnion(pair.desc);
                            if (props.exists(IO_READ_BEFORE_WRITE)) {
                                stream <<" read-before-write";
                            } else if (props.exists(IO_WRITE) && props.exists(IO_READ)) {
                                // nothing
                            } else if (props.exists(IO_READ)) {
                                stream <<" read-only";
                            } else if (props.exists(IO_WRITE)) {
                                stream <<" write-only";
                            }
                        }

                        stream <<" = ";
                        pair.value->print(stream, fmt);
                        stream <<"\n";
                    }
                }
            }
        }
    }
}



} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterStateGeneric);
#endif

#endif
