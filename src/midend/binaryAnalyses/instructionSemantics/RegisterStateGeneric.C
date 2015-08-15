#include <sage3basic.h>
#include <RegisterStateGeneric.h>

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

void
RegisterStateGeneric::clear()
{
    registers.clear();
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
            val = get_protoval()->number_(regs[i].get_nbits(), 0);
        } else {
            val = get_protoval()->undefined_(regs[i].get_nbits());
            if (!name.empty())
                val->set_comment(name+"_0");
        }
        registers[RegStore(regs[i])].push_back(RegPair(regs[i], val));
    }
}

static bool
has_null_value(const RegisterStateGeneric::RegPair &rp)
{
    return rp.value == NULL;
}

void
RegisterStateGeneric::get_nonoverlapping_parts(const Extent &overlap, const RegPair &rp, RiscOperators *ops,
                                               RegPairs *pairs/*out*/)
{
    if (overlap.first() > rp.desc.get_offset()) { // LSB part of existing register does not overlap
        RegisterDescriptor nonoverlap_desc(rp.desc.get_major(), rp.desc.get_minor(),
                                           rp.desc.get_offset(),
                                           overlap.first() - rp.desc.get_offset());
        SValuePtr nonoverlap_val = ops->extract(rp.value, 0, nonoverlap_desc.get_nbits());
        pairs->push_back(RegPair(nonoverlap_desc, nonoverlap_val));
    }
    if (overlap.last()+1 < rp.desc.get_offset() + rp.desc.get_nbits()) { // MSB part of existing reg does not overlap
        size_t nonoverlap_first = overlap.last() + 1; // bit offset for register
        size_t nonoverlap_nbits = (rp.desc.get_offset() + rp.desc.get_nbits()) - (overlap.last() + 1);
        RegisterDescriptor nonoverlap_desc(rp.desc.get_major(), rp.desc.get_minor(), nonoverlap_first, nonoverlap_nbits);
        size_t lo_bit = nonoverlap_first - rp.desc.get_offset(); // lo bit of value to extract
        SValuePtr nonoverlap_val = ops->extract(rp.value, lo_bit, lo_bit+nonoverlap_nbits);
        pairs->push_back(RegPair(nonoverlap_desc, nonoverlap_val));
    }
}

SValuePtr
RegisterStateGeneric::readRegister(const RegisterDescriptor &reg, RiscOperators *ops)
{
    ASSERT_require(reg.is_valid());

    // Fast case: the state does not store this register or any register that might overlap with this register.
    Registers::iterator ri = registers.find(reg);
    if (ri==registers.end()) {
        size_t nbits = reg.get_nbits();
        SValuePtr newval = get_protoval()->undefined_(nbits);
        std::string regname = regdict->lookup(reg);
        if (!regname.empty())
            newval->set_comment(regname + "_0");
        registers[reg].push_back(RegPair(reg, newval));
        return newval;
    }

    // Get a list of all the (parts of) registers that might overlap with this register.
    const Extent need_extent(reg.get_offset(), reg.get_nbits());
    std::vector<SValuePtr> overlaps(reg.get_nbits()); // overlapping parts of other registers by destination bit offset
    std::vector<RegPair> nonoverlaps; // non-overlapping parts of overlapping registers
    for (RegPairs::iterator rvi=ri->second.begin(); rvi!=ri->second.end(); ++rvi) {
        Extent have_extent(rvi->desc.get_offset(), rvi->desc.get_nbits());
        if (need_extent==have_extent) {
            // exact match; no need to traverse further because a RegisterStateGeneric never stores overlapping values
            ASSERT_require(nonoverlaps.empty());
            return rvi->value;
        } else {
            const Extent overlap = need_extent.intersect(have_extent);
            if (overlap==need_extent) {
                // The stored register contains all the bits we need to read, and then some.  Extract only what we need.
                // No need to traverse further because we never store overlapping values.
                ASSERT_require(nonoverlaps.empty());
                const size_t lo_bit = need_extent.first() - have_extent.first();
                return ops->extract(rvi->value, lo_bit, lo_bit+need_extent.size());
            } else if (!overlap.empty()) {
                ASSERT_require(overlap.first() >= have_extent.first());
                size_t extractBegin = overlap.first() - have_extent.first();
                size_t extractEnd = extractBegin + overlap.size();
                ASSERT_require(extractEnd > extractBegin);
                ASSERT_require(extractEnd <= rvi->value->get_width());
                const SValuePtr overlap_val = ops->extract(rvi->value, extractBegin, extractEnd);
                ASSERT_require(overlap_val->get_width() == overlap.size());
                ASSERT_require(overlap.first() >= need_extent.first());
                size_t offsetWrtResult = overlap.first() - need_extent.first();
                overlaps[offsetWrtResult] = overlap_val;

                if (coalesceOnRead_) {
                    // If any part of the existing register is not represented by the register being read, then we need to save
                    // that part.
                    get_nonoverlapping_parts(overlap, *rvi, ops, &nonoverlaps /*out*/);

                    // Mark the overlapping register by setting it to null so we can remove it later.
                    rvi->value = SValuePtr();
                }
            }
        }
    }

    // Remove the overlapping registers that we marked above, and replace them with their non-overlapping parts.
    if (coalesceOnRead_) {
        ri->second.erase(std::remove_if(ri->second.begin(), ri->second.end(), has_null_value), ri->second.end());
        ri->second.insert(ri->second.end(), nonoverlaps.begin(), nonoverlaps.end());
    }

    // Compute the return value by concatenating the overlapping parts and filling in any missing parts.
    SValuePtr retval;
    size_t offset = 0;
    while (offset<reg.get_nbits()) {
        SValuePtr part;
        if (overlaps[offset]!=NULL) {
            part = overlaps[offset];
        } else {
            size_t next_offset = offset+1;
            while (next_offset<reg.get_nbits() && overlaps[next_offset]==NULL) ++next_offset;
            size_t nbits = next_offset - offset;
            part = get_protoval()->undefined_(nbits);
            if (!coalesceOnRead_) {
                // This part of the return value doesn't exist in the register state, so we need to add it.  When
                // coalesceOnRead is set we'll insert the whole value at once at the very end.
                RegisterDescriptor subreg(reg.get_major(), reg.get_minor(), reg.get_offset()+offset, nbits);
                ri->second.push_back(RegPair(subreg, part));
            }
        }
        retval = retval==NULL ? part : ops->concat(retval, part);
        offset += part->get_width();
    }

    // Insert the return value.  When coalesceOnRead is set then no part of the register we just read overlaps with anything
    // already stored because we've removed those parts of registers that overlap.  In other words, there's already a hole in
    // which we can insert the value we're about to return.
    if (coalesceOnRead_)
        ri->second.push_back(RegPair(reg, retval));

    ASSERT_require(reg.get_nbits()==retval->get_width());
    return retval;
}

void
RegisterStateGeneric::updateWriteProperties(const RegisterDescriptor &reg, RegisterProperty prop) {
    insertProperties(reg, prop);
    if (prop == WRITTEN)
        eraseProperties(reg, READ_AFTER_WRITE);
}

void
RegisterStateGeneric::updateReadProperties(const RegisterDescriptor &reg) {
    insertProperties(reg, READ);
    BitProperties &props = properties_.insertMaybeDefault(reg);
    BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
    BOOST_FOREACH (BitProperties::Node &node, props.findAll(where)) {
        if (!node.value().exists(WRITTEN)) {
            node.value().insert(READ_BEFORE_WRITE);
            if (!node.value().exists(INITIALIZED))
                node.value().insert(READ_UNINITIALIZED);
        } else {
            node.value().insert(READ_AFTER_WRITE);
        }
    }
}

void
RegisterStateGeneric::writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    ASSERT_not_null(value);
    ASSERT_require2(reg.get_nbits()==value->get_width(), "value written to register must be the same width as the register");
    erase_register(reg, ops);
    Registers::iterator ri = registers.find(reg);
    if (ri == registers.end()) {
        registers[reg].push_back(RegPair(reg, value));
    } else {
        ri->second.push_back(RegPair(reg, value));
    }
}

void
RegisterStateGeneric::erase_register(const RegisterDescriptor &reg, RiscOperators *ops)
{
    ASSERT_require(reg.is_valid());

    // Fast case: the state does not store this register or any register that might overlap with this register
    Registers::iterator ri = registers.find(reg);
    if (ri == registers.end())
        return;                                         // no part of register is stored in this state

    // Look for existing registers that overlap with this register and remove them.  If the overlap was only partial, then we
    // need to eventually add the non-overlapping part back into the list.
    RegPairs nonoverlaps; // the non-overlapping parts of overlapping registers
    Extent need_extent(reg.get_offset(), reg.get_nbits());
    for (RegPairs::iterator rvi=ri->second.begin(); rvi!=ri->second.end(); ++rvi) {
        Extent have_extent(rvi->desc.get_offset(), rvi->desc.get_nbits());
        Extent overlap = need_extent.intersect(have_extent);
        if (!overlap.empty()) {
            get_nonoverlapping_parts(overlap, *rvi, ops, &nonoverlaps);
            rvi->value = SValuePtr(); // mark pair for removal by setting it to null
        }
    }

    // Remove marked pairs, then add the non-overlapping parts.
    ri->second.erase(std::remove_if(ri->second.begin(), ri->second.end(), has_null_value), ri->second.end());
    ri->second.insert(ri->second.end(), nonoverlaps.begin(), nonoverlaps.end());
}

RegisterStateGeneric::RegPairs
RegisterStateGeneric::get_stored_registers() const
{
    RegPairs retval;
    for (Registers::const_iterator ri=registers.begin(); ri!=registers.end(); ++ri)
        retval.insert(retval.end(), ri->second.begin(), ri->second.end());
    return retval;
}

void
RegisterStateGeneric::traverse(Visitor &visitor)
{
    for (Registers::iterator ri=registers.begin(); ri!=registers.end(); ++ri) {
        for (RegPairs::iterator rpi=ri->second.begin(); rpi!=ri->second.end(); ++rpi) {
            SValuePtr newval = (visitor)(rpi->desc, rpi->value);
            if (newval!=NULL) {
                ASSERT_require(newval->get_width()==rpi->desc.get_nbits());
                rpi->value = newval;
            }
        }
    }
}

void
RegisterStateGeneric::deep_copy_values()
{
    for (Registers::iterator ri=registers.begin(); ri!=registers.end(); ++ri) {
        for (RegPairs::iterator pi=ri->second.begin(); pi!=ri->second.end(); ++pi)
            pi->value = pi->value->copy();
    }
}

bool
RegisterStateGeneric::is_partly_stored(const RegisterDescriptor &desc) const
{
    Extent want(desc.get_offset(), desc.get_nbits());
    Registers::const_iterator ri = registers.find(RegStore(desc));
    if (ri!=registers.end()) {
        for (RegPairs::const_iterator pi=ri->second.begin(); pi!=ri->second.end(); ++pi) {
            const RegisterDescriptor &desc = pi->desc;
            Extent have(desc.get_offset(), desc.get_nbits());
            if (want.overlaps(have))
                return true;
        }
    }
    return false;
}

bool
RegisterStateGeneric::is_wholly_stored(const RegisterDescriptor &desc) const
{
    Extent want(desc.get_offset(), desc.get_nbits());
    ExtentMap have = stored_parts(desc);
    return 1==have.nranges() && have.minmax()==want;
}

bool
RegisterStateGeneric::is_exactly_stored(const RegisterDescriptor &desc) const
{
    Registers::const_iterator ri = registers.find(RegStore(desc));
    if (ri!=registers.end()) {
        for (RegPairs::const_iterator pi=ri->second.begin(); pi!=ri->second.end(); ++pi) {
            if (desc==pi->desc)
                return true;
        }
    }
    return false;
}

ExtentMap
RegisterStateGeneric::stored_parts(const RegisterDescriptor &desc) const
{
    ExtentMap retval;
    Extent want(desc.get_offset(), desc.get_nbits());
    Registers::const_iterator ri = registers.find(RegStore(desc));
    if (ri!=registers.end()) {
        for (RegPairs::const_iterator pi=ri->second.begin(); pi!=ri->second.end(); ++pi) {
            const RegisterDescriptor &desc = pi->desc;
            Extent have(desc.get_offset(), desc.get_nbits());
            retval.insert(want.intersect(have));
        }
    }
    return retval;
}

RegisterStateGeneric::RegPairs
RegisterStateGeneric::overlappingRegisters(const RegisterDescriptor &needle) const {
    ASSERT_require(needle.is_valid());
    RegPairs retval;
    BitRange want = BitRange::baseSize(needle.get_offset(), needle.get_nbits());
    Registers::const_iterator ri = registers.find(needle);
    if (ri != registers.end()) {
        BOOST_FOREACH (const RegPair &regValuePair, ri->second) {
            const RegisterDescriptor haystack = regValuePair.desc;
            BitRange have = BitRange::baseSize(haystack.get_offset(), haystack.get_nbits());
            if (have.isOverlapping(want))
                retval.push_back(regValuePair);
        }
    }
    return retval;
}

bool
RegisterStateGeneric::insertWriters(const RegisterDescriptor &desc, const AddressSet &writerVas) {
    if (writerVas.isEmpty())
        return false;
    BitAddressSet &parts = writers_.insertMaybeDefault(desc);
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    return parts.insert(where, writerVas);
}

void
RegisterStateGeneric::eraseWriters(const RegisterDescriptor &desc, const AddressSet &writerVas) {
    if (writerVas.isEmpty() || !writers_.exists(desc))
        return;
    BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    parts.erase(where, writerVas);
    if (parts.isEmpty())
        writers_.erase(desc);
}

void
RegisterStateGeneric::setWriters(const RegisterDescriptor &desc, const AddressSet &writerVas) {
    if (writerVas.isEmpty()) {
        eraseWriters(desc);
    } else {
        BitAddressSet &parts = writers_.insertMaybeDefault(desc);
        BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
        parts.replace(where, writerVas);
    }
}

// [Robb P. Matzke 2015-08-07]: deprecated
void
RegisterStateGeneric::set_latest_writer(const RegisterDescriptor &desc, rose_addr_t writer_va)
{
    setWriters(desc, writer_va);
}

void
RegisterStateGeneric::eraseWriters(const RegisterDescriptor &desc) {
    if (!writers_.exists(desc))
        return;
    BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    parts.erase(where);
    if (parts.isEmpty())
        writers_.erase(desc);
}

// [Robb P. Matzke 2015-08-07]: deprecated
void
RegisterStateGeneric::clear_latest_writer(const RegisterDescriptor &desc)
{
    eraseWriters(desc);
}

void
RegisterStateGeneric::eraseWriters() {
    writers_.clear();
}

// [Robb P. Matzke 2015-08-07]: deprecated
void
RegisterStateGeneric::clear_latest_writers()
{
    eraseWriters();
}

bool
RegisterStateGeneric::hasWritersAny(const RegisterDescriptor &desc) const {
    if (!writers_.exists(desc))
        return false;
    const BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    return parts.isOverlapping(where);
}

bool
RegisterStateGeneric::hasWritersAll(const RegisterDescriptor &desc) const {
    if (!writers_.exists(desc))
        return false;
    const BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    return parts.contains(where);
}

RegisterStateGeneric::AddressSet
RegisterStateGeneric::getWritersUnion(const RegisterDescriptor &desc) const {
    if (!writers_.exists(desc))
        return AddressSet();
    const BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    return parts.getUnion(where);
}

RegisterStateGeneric::AddressSet
RegisterStateGeneric::getWritersIntersection(const RegisterDescriptor &desc) const {
    if (!writers_.exists(desc))
        return AddressSet();
    const BitAddressSet &parts = writers_[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    return parts.getIntersection(where);
}

// [Robb P. Matzke 2015-08-07]: deprecated, use getWritersUnion instead
std::set<rose_addr_t>
RegisterStateGeneric::get_latest_writers(const RegisterDescriptor &desc) const
{
    AddressSet writerVas = getWritersUnion(desc);
    std::set<rose_addr_t> retval(writerVas.values().begin(), writerVas.values().end());
    return retval;
}

bool
RegisterStateGeneric::hasPropertyAny(const RegisterDescriptor &reg, RegisterProperty prop) const {
    if (!properties_.exists(reg))
        return false;
    const BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
    return bitProps.existsAnywhere(where, prop);
}

bool
RegisterStateGeneric::hasPropertyAll(const RegisterDescriptor &reg, RegisterProperty prop) const {
    if (!properties_.exists(reg))
        return false;
    const BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
    return bitProps.existsEverywhere(where, prop);
}

RegisterStateGeneric::PropertySet
RegisterStateGeneric::getPropertiesUnion(const RegisterDescriptor &reg) const {
    if (!properties_.exists(reg))
        return PropertySet();
    const BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
    return bitProps.getUnion(where);
}

RegisterStateGeneric::PropertySet
RegisterStateGeneric::getPropertiesIntersection(const RegisterDescriptor &reg) const {
    if (!properties_.exists(reg))
        return PropertySet();
    const BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
    return bitProps.getIntersection(where);
}

bool
RegisterStateGeneric::insertProperties(const RegisterDescriptor &reg, const PropertySet &props) {
    if (props.isEmpty())
        return false;
    BitProperties &bitProps = properties_.insertMaybeDefault(reg);
    BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
    return bitProps.insert(where, props);
}

bool
RegisterStateGeneric::eraseProperties(const RegisterDescriptor &reg, const PropertySet &props) {
    if (props.isEmpty() || !properties_.exists(reg))
        return false;
    BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
    bool changed = bitProps.erase(where, props);
    if (bitProps.isEmpty())
        properties_.erase(reg);
    return changed;
}

void
RegisterStateGeneric::setProperties(const RegisterDescriptor &reg, const PropertySet &props) {
    if (props.isEmpty()) {
        eraseProperties(reg);
    } else {
        BitProperties &bitProps = properties_.insertMaybeDefault(reg);
        BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
        bitProps.replace(where, props);
    }
}

void
RegisterStateGeneric::eraseProperties(const RegisterDescriptor &reg) {
    if (!properties_.exists(reg))
        return;
    BitProperties &bitProps = properties_[reg];
    BitRange where = BitRange::baseSize(reg.get_offset(), reg.get_nbits());
    bitProps.erase(where);
    if (bitProps.isEmpty())
        properties_.erase(reg);
}

void
RegisterStateGeneric::eraseProperties() {
    properties_.clear();
}

std::vector<RegisterDescriptor>
RegisterStateGeneric::findProperties(const PropertySet &required, const PropertySet &prohibited) const {
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
        const RegisterDescriptor &otherReg = otherRegVal.desc;
        const BaseSemantics::SValuePtr &otherValue = otherRegVal.value;
        if (is_partly_stored(otherReg)) {
            BaseSemantics::SValuePtr thisValue = readRegister(otherReg, ops);
            if (BaseSemantics::SValuePtr mergedValue = thisValue->merge(otherValue, ops->get_solver())) {
                writeRegister(otherReg, mergedValue, ops);
                changed = true;
            }
        } else {
            writeRegister(otherReg, otherValue, ops);
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

static bool
sortByOffset(const RegisterStateGeneric::RegPair &a, const RegisterStateGeneric::RegPair &b) {
    return a.desc.get_offset() < b.desc.get_offset();
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
        for (Registers::const_iterator ri=registers.begin(); ri!=registers.end(); ++ri) {
            RegPairs regPairs = ri->second;
            std::sort(regPairs.begin(), regPairs.end(), sortByOffset);
            for (RegPairs::const_iterator rvi=regPairs.begin(); rvi!=regPairs.end(); ++rvi) {
                std::string regname = regnames(rvi->desc);
                if (!fmt.get_suppress_initial_values() || rvi->value->get_comment().empty() ||
                    0!=rvi->value->get_comment().compare(regname+"_0")) {
                    if (0==i) {
                        maxlen = std::max(maxlen, regname.size());
                    } else {
                        stream <<fmt.get_line_prefix() <<std::setw(maxlen) <<std::left <<regname;
                        oflags.restore();
                        if (fmt.get_show_latest_writers()) {
                            // FIXME[Robb P. Matzke 2015-08-12]: This doesn't take into account that different writer sets can
                            // exist for different parts of the register.
                            AddressSet writers = getWritersUnion(rvi->desc);
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
                            PropertySet props = getPropertiesUnion(rvi->desc);
                            if (props.exists(READ_BEFORE_WRITE)) {
                                stream <<" read-before-write";
                            } else if (props.exists(WRITTEN) && props.exists(READ)) {
                                // nothing
                            } else if (props.exists(READ)) {
                                stream <<" read-only";
                            } else if (props.exists(WRITTEN)) {
                                stream <<" write-only";
                            }
                        }

                        stream <<" = ";
                        rvi->value->print(stream, fmt);
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
