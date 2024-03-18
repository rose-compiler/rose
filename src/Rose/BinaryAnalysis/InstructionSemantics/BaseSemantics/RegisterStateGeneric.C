#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterStateGeneric.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/Utility.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/Diagnostics.h>
#include <Rose/FormatRestorer.h>
#include <Rose/StringUtility/NumberToString.h>
#include <ROSE_UNUSED.h>

#include <Sawyer/IntervalSet.h>

#include <boost/algorithm/string/erase.hpp>
#include <boost/format.hpp>
#include <boost/format.hpp>

// Define this if you want extra consistency checking before and after each mutator.  This slows things down considerably but
// can be useful for narrowing down logic errors in the implementation.
//#define RegisterStateGeneric_ExtraAssertions

// Define this if you want the readRegister behavior as it existed before 2015-09-24. This behavior was wrong in certain ways
// because it didn't always cause registers to spring into existence the first time they were read.
//#define RegisterStateGeneric_20150924

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

using namespace Rose::Diagnostics;

static bool
sortByOffset(const RegisterStateGeneric::RegPair &a, const RegisterStateGeneric::RegPair &b) {
    return a.desc.offset() < b.desc.offset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterStateGeneric::RegPair
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SValue::Ptr
RegisterStateGeneric::RegPair::get(RegisterDescriptor reg, RiscOperators *ops) const {
    ASSERT_require(reg.isSubsetOf(desc));
    if (reg == desc) {
        return value;
    } else {
        const size_t begin = reg.offset() - desc.offset();
        const size_t end = begin + reg.nBits();
        return ops->extract(value, begin, end);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterStateGeneric
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    std::vector<RegisterDescriptor> regs = regdict->getLargestRegisters();
    initialize_nonoverlapping(regs, true);
}

void
RegisterStateGeneric::initialize_large()
{
    ASSERT_not_null(regdict);
    std::vector<RegisterDescriptor> regs = regdict->getLargestRegisters();
    initialize_nonoverlapping(regs, false);
}

void
RegisterStateGeneric::initialize_small()
{
    std::vector<RegisterDescriptor> regs = regdict->getSmallestRegisters();
    initialize_nonoverlapping(regs, false);
}

void
RegisterStateGeneric::initialize_nonoverlapping(const std::vector<RegisterDescriptor> &regs, bool initialize_to_zero)
{
    clear();
    for (size_t i=0; i<regs.size(); ++i) {
        std::string name = regdict->lookup(regs[i]);
        SValue::Ptr val;
        if (initialize_to_zero) {
            val = protoval()->number_(regs[i].nBits(), 0);
        } else {
            val = protoval()->undefined_(regs[i].nBits());
            if (!name.empty() && val->comment().empty())
                val->comment(name+"_0");
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
    for (const Registers::Node &rnode: registers_.nodes()) {
        Sawyer::Container::IntervalSet<BitRange> foundLocations;
        for (const RegPair &regpair: rnode.value()) {
            if (!regpair.desc.is_valid()) {
                error <<"invalid register descriptor";
            } else if (regpair.desc.get_major() != rnode.key().majr || regpair.desc.minorNumber() != rnode.key().minr) {
                error <<"register is in wrong list; register=" <<regpair.desc.get_major() <<"." <<regpair.desc.minorNumber()
                      <<", list=" <<rnode.key().majr <<"." <<rnode.key().minr;
            } else if (regpair.value == NULL) {
                error <<"value is null for register " <<regpair.desc;
            } else if (regpair.value->nBits() != regpair.desc.nBits()) {
                error <<"value width (" <<regpair.value->nBits() <<") is incorrect for register " <<regpair.desc;
            } else if (foundLocations.overlaps(regpair.location())) {
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
            for (const RegPair &regpair: rnode.value()) {
                mlog[FATAL] <<"    " <<regpair.desc;
                if (regpair.value == NULL)
                    mlog[FATAL] <<"\tnull value";
                mlog[FATAL] <<"\n";
            }
            abort();
        }
    }
#else
    ROSE_UNUSED(when);
    ROSE_UNUSED(reg);
#endif
}

void
RegisterStateGeneric::scanAccessedLocations(RegisterDescriptor reg, RiscOperators *ops,
                                            RegPairs &accessedParts /*out*/, RegPairs &preservedParts /*out*/) const {
    BitRange accessedLocation = BitRange::baseSize(reg.offset(), reg.nBits());
    const RegPairs &pairList = registers_.getOrDefault(reg);
    for (const RegPair &regpair: pairList) {
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
    for (RegPair &regpair: pairList) {
        BitRange storedLocation = regpair.location();
        BitRange overlap = storedLocation & accessedLocation;
        if (!overlap.isEmpty())
            regpair.value = SValue::Ptr();
    }
}

SValue::Ptr
RegisterStateGeneric::readRegister(RegisterDescriptor reg, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_forbid(reg.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_require2(reg.nBits() == dflt->nBits(), "value being read must be same size as register" +
                    (boost::format(": %|u| -> %|u|") % reg.nBits() % dflt->nBits()).str());
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
        SValue::Ptr newval = dflt->copy();
        std::string regname = regdict->lookup(reg);
        boost::erase_all(regname, "[");
        boost::erase_all(regname, "]");
        if (!regname.empty() && newval->comment().empty())
            newval->comment(regname + "_0");
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
    for (const RegPair &regpair: accessedParts)
        newLocations -= regpair.location();

    // Create values for the parts of the accessed register that weren't stored in the state, but don't store them yet.
    RegPairs newParts;
    for (const BitRange &newLocation: newLocations.intervals()) {
        RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), newLocation.least(), newLocation.size());
        ASSERT_require(newLocation.least() >= reg.offset());
        SValue::Ptr newval = ops->extract(dflt,
                                        newLocation.least()-reg.offset(),
                                        newLocation.greatest()+1-reg.offset());
        newParts.push_back(RegPair(subreg, newval));
    }

    // Construct the return value by combining all the parts we found or created.
    SValue::Ptr retval;
    RegPairs retvalParts = accessedParts;
    retvalParts.insert(retvalParts.end(), newParts.begin(), newParts.end());
    std::sort(retvalParts.begin(), retvalParts.end(), sortByOffset);
    for (const RegPair &regpair: retvalParts)
        retval = retval ? ops->concat(retval, regpair.value) : regpair.value;
    ASSERT_require(retval->nBits() == reg.nBits());

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
                SValue::Ptr part = accessedParts[i].value;
                size_t firstOffset = accessedParts[i].desc.offset();
                size_t nextOffset = firstOffset + accessedParts[i].desc.nBits();
                while (i+1 < accessedParts.size() && accessedParts[i+1].desc.offset() == nextOffset) {
                    ++i;
                    nextOffset += accessedParts[i].desc.nBits();
                    part = ops->concat(part, accessedParts[i].value);
                }
                RegisterDescriptor tmpReg(reg.majorNumber(), reg.minorNumber(), firstOffset, nextOffset-firstOffset);
                ASSERT_require(tmpReg.nBits() == part->nBits());
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

BaseSemantics::SValue::Ptr
RegisterStateGeneric::peekRegister(RegisterDescriptor reg, const SValue::Ptr &dflt, RiscOperators *ops) {
    ASSERT_forbid(reg.isEmpty());
    ASSERT_not_null(dflt);
    ASSERT_require2(reg.nBits() == dflt->nBits(), "value being read must be same size as register" +
                    (boost::format(": %|u| -> %|u|") % reg.nBits() % dflt->nBits()).str());
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
    for (const RegPair &regpair: accessedParts)
        newLocations -= regpair.location();

    // Create values for the parts of the accessed register that weren't stored in the state
    RegPairs newParts;
    for (const BitRange &newLocation: newLocations.intervals()) {
        RegisterDescriptor subreg(reg.majorNumber(), reg.minorNumber(), newLocation.least(), newLocation.size());
        ASSERT_require(newLocation.least() >= reg.offset());
        SValue::Ptr newval = ops->extract(dflt,
                                        newLocation.least()-reg.offset(),
                                        newLocation.greatest()+1-reg.offset());
        newParts.push_back(RegPair(subreg, newval));
    }

    // Construct the return value by combining all the parts we found or created.
    SValue::Ptr retval;
    RegPairs retvalParts = accessedParts;
    retvalParts.insert(retvalParts.end(), newParts.begin(), newParts.end());
    std::sort(retvalParts.begin(), retvalParts.end(), sortByOffset);
    for (const RegPair &regpair: retvalParts)
        retval = retval ? ops->concat(retval, regpair.value) : regpair.value;
    ASSERT_require(retval->nBits() == reg.nBits());

    assertStorageConditions("at end of peek", reg);
    return retval;
}

void
RegisterStateGeneric::writeRegister(RegisterDescriptor reg, const SValue::Ptr &value, RiscOperators *ops)
{
    ASSERT_not_null(value);
    ASSERT_require2(reg.nBits()==value->nBits(), "value written to register must be the same width as the register" +
                    (boost::format(": %|u| -> %|u|") % value->nBits() % reg.nBits()).str());
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
        for (const RegPair &regpair: registers_.getOrDefault(reg))
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
    for (const RegPair &regpair: accessedParts)
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
        for (RegPair &regpair: pairList) {
            BitRange storedLocation = regpair.location();
            if (BitRange overlap = storedLocation & accessedLocation) {
                SValue::Ptr valueToWrite;
                if (overlap.least() > storedLocation.least()) {
                    size_t nbits = overlap.least() - storedLocation.least();
                    SValue::Ptr loValue = ops->unsignedExtend(regpair.value, nbits);
                    valueToWrite = loValue;
                }
                {
                    size_t nbits = overlap.size();
                    size_t extractBegin = overlap.least() - accessedLocation.least();
                    size_t extractEnd = extractBegin + nbits;
                    SValue::Ptr midValue = ops->extract(value, extractBegin, extractEnd);
                    valueToWrite = valueToWrite ? ops->concat(valueToWrite, midValue) : midValue;
                }
                if (overlap.greatest() < storedLocation.greatest()) {
                    size_t nbits = storedLocation.greatest() - overlap.greatest();
                    size_t extractBegin = overlap.greatest()+1 - storedLocation.least();
                    size_t extractEnd = extractBegin + nbits;
                    SValue::Ptr hiValue = ops->extract(regpair.value, extractBegin, extractEnd);
                    valueToWrite = valueToWrite ? ops->concat(valueToWrite, hiValue) : hiValue;
                }
                ASSERT_not_null(valueToWrite);
                ASSERT_require(valueToWrite->nBits() == regpair.desc.nBits());
                regpair.value = valueToWrite;
            }
        }

        // Insert the parts that didn't exist before
        for (const BitRange &newLocation: newLocations.intervals()) {
            size_t extractBegin = newLocation.least() - accessedLocation.least();
            size_t extractEnd = extractBegin + newLocation.size();
            SValue::Ptr valueToWrite = ops->extract(value, extractBegin, extractEnd);
            ASSERT_require(valueToWrite->nBits() == newLocation.size());
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
    for (BitProperties::Node &node: props.findAll(where)) {
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
    for (RegPair &reg_val: pairList) {
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
            reg_val.value = SValue::Ptr();
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
    for (const RegPairs &pairlist: registers_.values())
        retval.insert(retval.end(), pairlist.begin(), pairlist.end());
    return retval;
}

void
RegisterStateGeneric::traverse(Visitor &visitor)
{
    for (RegPairs &pairlist: registers_.values()) {
        for (RegPair &pair: pairlist) {
            if (SValue::Ptr newval = (visitor)(pair.desc, pair.value)) {
                ASSERT_require(newval->nBits() == pair.desc.nBits());
                pair.value = newval;
            }
        }
    }
}

void
RegisterStateGeneric::deep_copy_values()
{
    for (RegPairs &pairlist: registers_.values()) {
        for (RegPair &pair: pairlist)
            pair.value = pair.value->copy();
    }
}

bool
RegisterStateGeneric::is_partly_stored(RegisterDescriptor desc) const
{
    BitRange want = BitRange::baseSize(desc.offset(), desc.nBits());
    for (const RegPair &pair: registers_.getOrDefault(desc)) {
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
    for (const RegPair &pair: registers_.getOrDefault(desc))
        desired -= pair.location();
    return desired.isEmpty();
}

bool
RegisterStateGeneric::is_exactly_stored(RegisterDescriptor desc) const
{
    for (const RegPair &pair: registers_.getOrDefault(desc)) {
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
    for (const RegPair &pair: registers_.getOrDefault(desc)) {
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
    for (const RegPair &pair: registers_.getOrDefault(needle)) {
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
    return parts.overlaps(where);
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
    for (const RegisterProperties::Node &regNode: properties_.nodes()) {
        unsigned majr = regNode.key().majr;
        unsigned minr = regNode.key().minr;
        Bits bits;
        for (const BitProperties::Node &bitNode: regNode.value().nodes()) {
            if (bitNode.value().existsAll(required) && !bitNode.value().existsAny(prohibited))
                bits.insert(bitNode.key());
        }
        for (const BitRange &bitRange: bits.intervals()) {
            RegisterDescriptor reg(majr, minr, bitRange.least(), bitRange.size());
            retval.push_back(reg);
        }
    }
    return retval;
}

bool
RegisterStateGeneric::merge(const BaseSemantics::RegisterState::Ptr &other_, RiscOperators *ops) {
    ASSERT_not_null(ops);
    RegisterStateGeneric::Ptr other = boost::dynamic_pointer_cast<RegisterStateGeneric>(other_);
    ASSERT_not_null(other);
    using Interval = Sawyer::Container::Interval<size_t>;
    using IntervalSet = Sawyer::Container::IntervalSet<Interval>;
    bool changed = false;

    // Merge values stored in registers.
    const RegPairs otherRegVals = other->get_stored_registers();
    for (const RegPair &otherRegVal: otherRegVals) {

        // Part of otherRegVal that remains to be processed.
        IntervalSet remaining;
        remaining |= Interval::baseSize(otherRegVal.desc.offset(), otherRegVal.desc.nBits());

        // Where this register state contains a value for part of the other register, merge the values.
        const RegPairs selfRegVals = overlappingRegisters(otherRegVal.desc); // copy since write (below) changes state
        for (const RegPair &selfRegVal: selfRegVals) {
            const RegisterDescriptor reg = otherRegVal.desc & selfRegVal.desc;
            SValue::Ptr selfValue = selfRegVal.get(reg, ops);
            SValue::Ptr otherValue = otherRegVal.get(reg, ops);
            if (SValue::Ptr merged = selfValue->createOptionalMerge(otherValue, merger(), ops->solver()).orDefault()) {
                writeRegister(reg, merged, ops);
                changed = true;
            }

            const auto processed = Interval::baseSize(reg.offset(), reg.nBits());
            remaining -= processed;
        }

        // Where this register state does not contain a value for part of the other register, use the other state's value
        // without merging.
        for (const Interval &where: remaining.intervals()) {
            const RegisterDescriptor reg(otherRegVal.desc.majorNumber(), otherRegVal.desc.minorNumber(),
                                         where.least(), where.size());
            SValue::Ptr otherValue = otherRegVal.get(reg, ops);
            writeRegister(reg, otherValue, ops);
            changed = true;
        }
    }

    // Merge writer sets.
    for (const RegisterAddressSet::Node &wmNode: other->writers_.nodes()) {
        const BitAddressSet &otherWriters = wmNode.value();
        BitAddressSet &thisWriters = writers_.insertMaybeDefault(wmNode.key());
        for (const BitAddressSet::Node &otherWritten: otherWriters.nodes()) {
            bool inserted = thisWriters.insert(otherWritten.key(), otherWritten.value());
            if (inserted)
                changed = true;
        }
    }

    // Merge property sets.
    for (const RegisterProperties::Node &otherRegNode: other->properties_.nodes()) {
        const BitProperties &otherBitProps = otherRegNode.value();
        BitProperties &thisBitProps = properties_.insertMaybeDefault(otherRegNode.key());
        for (const BitProperties::Node &otherBitNode: otherBitProps.nodes()) {
            bool inserted = thisBitProps.insert(otherBitNode.key(), otherBitNode.value());
            if (inserted)
                changed = true;
        }
    }

    return changed;
}

void
RegisterStateGeneric::hash(Combinatorics::Hasher &hasher, RiscOperators *ops) const {
    // This register state automatically concatenates and splits register parts as needed. For instance, it might store x86 EAX
    // as a single 32-bit value, or two 16-bit values, or four 8-bit values, or any other combination. Some parts might even be
    // missing, such as the high 32 bits of RAX if we ony ever wrote to the low 32 bits. This complicates hashing. We could
    // simply hash exactly the parts that are stored, but a more useful hasher would return the same value whether a register
    // was stored as a single piece or broken into multiple smaller pieces.
    ASSERT_not_null(ops);
    ASSERT_not_null(regdict);
    std::vector<RegisterDescriptor> regs = regdict->getLargestRegisters();
    for (RegisterDescriptor reg: regs) {
        ExtentMap parts = stored_parts(reg);
        for (const auto &node: parts) {
            RegisterDescriptor part(reg.majorNumber(), reg.minorNumber(), node.first.first(), node.first.size());
            BaseSemantics::SValue::Ptr dflt = ops->undefined_(node.first.size());
            BaseSemantics::SValue::Ptr value = const_cast<RegisterStateGeneric*>(this)->peekRegister(part, dflt, ops); // FIXME[Robb Matzke 2021-03-26]
            hasher.insert(part.majorNumber());
            hasher.insert(part.minorNumber());
            hasher.insert(part.offset());
            hasher.insert(part.nBits());
            value->hash(hasher);
        }
    }
}

void
RegisterStateGeneric::print(std::ostream &stream, Formatter &fmt) const
{
    RegisterDictionary::Ptr regdict = fmt.registerDictionary();
    if (!regdict)
        regdict = registerDictionary();
    RegisterNames regnames(regdict);

    // First pass is to get the maximum length of the register names; second pass prints
    FormatRestorer oflags(stream);
    size_t maxlen = 6; // use at least this many columns even if register names are short.
    for (int i=0; i<2; ++i) {
        for (const RegPairs &pl: registers_.values()) {
            RegPairs regPairs = pl;
            std::sort(regPairs.begin(), regPairs.end(), sortByOffset);
            for (const RegPair &pair: regPairs) {
                std::string regname = regnames(pair.desc);
                if (!fmt.get_suppress_initial_values() || pair.value->comment().empty() ||
                    0!=pair.value->comment().compare(regname+"_0")) {
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
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterStateGeneric);
#endif

#endif
