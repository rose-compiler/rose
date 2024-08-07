#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/RegisterParts.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {

void
RegisterParts::erase(RegisterDescriptor reg) {
    if (map_.exists(reg)) {
        BitSet &bits = map_[reg];
        bits.erase(bitRange(reg));
        if (bits.isEmpty())
            map_.erase(reg);
    }
}

RegisterParts&
RegisterParts::operator-=(const RegisterParts &other) {
    for (const Map::Node &node: other.map_.nodes()) {
        for (const BitRange &bits: node.value().intervals())
            erase(RegisterDescriptor(node.key().get_major(), node.key().get_minor(), bits.least(), bits.size()));
    }
    return *this;
}

RegisterParts
RegisterParts::operator-(const RegisterParts &other) const {
    RegisterParts retval = *this;
    retval -= other;
    return retval;
}

RegisterParts&
RegisterParts::operator|=(const RegisterParts &other) {
    for (const Map::Node &node: other.map_.nodes())
        map_.insertMaybeDefault(node.key()).insertMultiple(node.value());
    return *this;
}

RegisterParts
RegisterParts::operator|(const RegisterParts &other) const {
    RegisterParts retval = *this;
    retval |= other;
    return retval;
}

RegisterParts&
RegisterParts::operator&=(const RegisterParts &other) {
    Map newmap;
    for (const Map::Node &node: map_.nodes()) {
        if (other.map_.exists(node.key())) {
            BitSet intersection = node.value() & other.map_[node.key()];
            if (!intersection.isEmpty())
                newmap.insert(node.key(), intersection);
        }
    }
    map_ = newmap;
    return *this;
}

RegisterParts
RegisterParts::operator&(const RegisterParts &other) const {
    RegisterParts retval = *this;
    retval &= other;
    return retval;
}

std::vector<RegisterDescriptor>
RegisterParts::extract(const RegisterDictionary::Ptr &regDict, bool extractAll) {
    std::vector<RegisterDescriptor> retval, allRegs;
    if (isEmpty())
        return retval;

    if (regDict) {
        for (const RegisterDictionary::Entries::value_type &pair: regDict->registers())
            allRegs.push_back(pair.second);
        std::sort(allRegs.begin(), allRegs.end(), RegisterDictionary::SortBySize(RegisterDictionary::SortBySize::DESCENDING));
        for (RegisterDescriptor reg: allRegs) {
            if (existsAll(reg)) {
                retval.push_back(reg);
                erase(reg);
                if (isEmpty())
                    break;
            }
        }
    }

    if (!regDict || extractAll) {
        for (const Map::Node &node: map_.nodes()) {
            for (const BitRange &bits: node.value().intervals())
                retval.push_back(RegisterDescriptor(node.key().get_major(), node.key().get_minor(), bits.least(), bits.size()));
        }
        clear();
    }

    return retval;
}

std::vector<RegisterDescriptor>
RegisterParts::listAll(const RegisterDictionary::Ptr &regDict) const {
    RegisterParts temp = *this;
    return temp.extract(regDict, true);
}

std::vector<RegisterDescriptor>
RegisterParts::listNamed(const RegisterDictionary::Ptr &regDict) const {
    RegisterParts temp = *this;
    return temp.extract(regDict, false);
}

} // namespace
} // namespace

#endif
