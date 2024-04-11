#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch32.h>
#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>
#include <Rose/BinaryAnalysis/InstructionEnumsJvm.h>
#include <Rose/BinaryAnalysis/InstructionEnumsM68k.h>
#include <Rose/BinaryAnalysis/InstructionEnumsMips.h>
#include <Rose/BinaryAnalysis/InstructionEnumsPowerpc.h>
#include <Rose/BinaryAnalysis/InstructionEnumsX86.h>
#include <Rose/StringUtility/Escape.h>

#include <Sawyer/Assert.h>

namespace Rose {
namespace BinaryAnalysis {

RegisterDictionary::RegisterDictionary() {}

RegisterDictionary::RegisterDictionary(const std::string &name)
    : name_(name) {}

RegisterDictionary::RegisterDictionary(const RegisterDictionary &other)
    : SharedObject(other) {
    *this = other;
}

RegisterDictionary::Ptr
RegisterDictionary::instance(const std::string &name) {
    return Ptr(new RegisterDictionary(name));
}

RegisterDictionary::Ptr
RegisterDictionary::instanceNull() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary::Ptr regs;
    if (!regs) {
        regs =RegisterDictionary::instance("null");
        regs->insert("pc", 0, 0, 0, 8);                 // program counter
        regs->insert("sp", 0, 1, 0, 8);                 // stack pointer
    }
    return regs;
}

const std::string&
RegisterDictionary::name() const {
    return name_;
}

void
RegisterDictionary::name(const std::string &s) {
    name_ = s;
}

const RegisterDictionary::Entries &
RegisterDictionary::registers() const {
    return forward_;
}

void
RegisterDictionary::insert(const std::string &name, RegisterDescriptor reg) { //
    /* Erase the name from the reverse lookup map, indexed by the old descriptor. */
    Entries::iterator fi = forward_.find(name);
    if (fi != forward_.end()) {
        Reverse::iterator ri = reverse_.find(fi->second);
        ASSERT_require(ri != reverse_.end());
        std::vector<std::string>::iterator vi=std::find(ri->second.begin(), ri->second.end(), name);
        ASSERT_require(vi != ri->second.end());
        ri->second.erase(vi);
    }

    /* Insert or replace old descriptor with a new one and insert reverse lookup info. */
    forward_[name] = reg;
    reverse_[reg].push_back(name);
}

void
RegisterDictionary::insert(const std::string &name, unsigned majr, unsigned minr, unsigned offset, unsigned nbits) {
    insert(name, RegisterDescriptor(majr, minr, offset, nbits));
}

void
RegisterDictionary::insert(const Ptr &other) {
    ASSERT_not_null(other);
    const Entries &entries = other->registers();
    for (Entries::const_iterator ei = entries.begin(); ei != entries.end(); ++ei)
        insert(ei->first, ei->second);

    instructionPointer_ = other->instructionPointer_;
    stackPointer_ = other->stackPointer_;
    stackFrame_ = other->stackFrame_;
    stackSegment_ = other->stackSegment_;
    callReturn_ = other->callReturn_;
}

void
RegisterDictionary::resize(const std::string &name, unsigned new_nbits) {
    RegisterDescriptor reg = findOrThrow(name);
    reg.nBits(new_nbits);
    insert(name, reg);
}

RegisterDescriptor
RegisterDictionary::find(const std::string &name) const {
    Entries::const_iterator fi = forward_.find(name);
    return forward_.end() == fi ? RegisterDescriptor() : fi->second;
}

RegisterDescriptor
RegisterDictionary::findOrThrow(const std::string &name) const {
    Entries::const_iterator fi = forward_.find(name);
    if (forward_.end() == fi)
        throw Exception("register " + name + " not found");
    return fi->second;
}

const std::string&
RegisterDictionary::lookup(RegisterDescriptor reg) const {
    Reverse::const_iterator ri = reverse_.find(reg);
    if (ri != reverse_.end()) {
        for (size_t i = ri->second.size(); i > 0; --i) {
            const std::string &name = ri->second[i-1];
            Entries::const_iterator fi = forward_.find(name);
            ASSERT_require(fi != forward_.end());
            if (fi->second == reg)
                return name;
        }
    }

    static const std::string empty;
    return empty;
}

bool
RegisterDictionary::exists(RegisterDescriptor rdesc) const {
    Reverse::const_iterator found = reverse_.find(rdesc);
    return found == reverse_.end();
}
RegisterDescriptor
RegisterDictionary::findLargestRegister(unsigned major, unsigned minor, size_t maxWidth) const {
    RegisterDescriptor retval;
    for (Entries::const_iterator iter = forward_.begin(); iter != forward_.end(); ++iter) {
        RegisterDescriptor reg = iter->second;
        if (major == reg.majorNumber() && minor == reg.minorNumber()) {
            if (maxWidth > 0 && reg.nBits() > maxWidth) {
                // ignore
            } else if (retval.isEmpty()) {
                retval = reg;
            } else if (retval.nBits() < reg.nBits()) {
                retval = reg;
            }
        }
    }
    return retval;
}

RegisterParts
RegisterDictionary::getAllParts() const {
    RegisterParts retval;
    for (const Entries::value_type &node: forward_)
        retval.insert(node.second);
    return retval;
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::getDescriptors() const {
    const Entries &entries = registers();
    RegisterDescriptors retval;
    retval.reserve(entries.size());
    for (Entries::const_iterator ei = entries.begin(); ei != entries.end(); ++ei)
        retval.push_back(ei->second);
    return retval;
}

// Returns lowest possible non-negative value not present in v
static unsigned
firstUnused(std::vector<unsigned> &v /*in,out*/) {
    if (v.empty())
        return 0;
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
    if (v.back() + 1 == v.size())
        return v.size();
    for (size_t i=0; i<v.size(); ++i) {
        if (v[i] != i)
            return i;
    }
    ASSERT_not_reachable("should have returned by now");
}

unsigned
RegisterDictionary::firstUnusedMajor() const {
    std::vector<unsigned> used;
    for (const Entries::value_type &entry: forward_) {
        if (used.empty() || used.back() != entry.second.majorNumber())
            used.push_back(entry.second.majorNumber());
    }
    return firstUnused(used);
}

unsigned
RegisterDictionary::firstUnusedMinor(unsigned majr) const {
    std::vector<unsigned> used;
    for (const Entries::value_type &entry: forward_) {
        if (entry.second.majorNumber() == majr)
            used.push_back(entry.second.minorNumber());
    }
    return firstUnused(used);
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::getLargestRegisters() const {
    SortBySize order(SortBySize::ASCENDING);
    return filterNonoverlapping(getDescriptors(), order, true);
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::getSmallestRegisters() const {
    SortBySize order(SortBySize::DESCENDING);
    return filterNonoverlapping(getDescriptors(), order, true);
}

void
RegisterDictionary::print(std::ostream &o) const {
    o <<"RegisterDictionary \"" <<name_ <<"\" contains " <<forward_.size() <<" " <<(1 == forward_.size()?"entry":"entries") <<"\n";
    for (Entries::const_iterator ri = forward_.begin(); ri != forward_.end(); ++ri)
        o <<"  \"" <<ri->first <<"\" " <<ri->second <<"\n";

    for (Reverse::const_iterator ri = reverse_.begin(); ri != reverse_.end(); ++ri) {
        o <<"  " <<ri->first;
        for (std::vector<std::string>::const_iterator vi = ri->second.begin(); vi != ri->second.end(); ++vi) {
            o <<" " <<*vi;
        }
        o <<"\n";
    }
}

size_t
RegisterDictionary::size() const {
    return forward_.size();
}


Sawyer::Optional<std::string>
RegisterDictionary::name(RegisterDescriptor reg) const {
    const std::string nm = lookup(reg);
    if (nm.empty()) {
        return {};
    } else {
        return nm;
    }
}

std::string
RegisterDictionary::nameOrQuad(RegisterDescriptor reg) const {
    if (const auto s = name(reg)) {
        return *s;
    } else {
        return reg.toString();
    }
}

std::string
RegisterDictionary::nameAndQuad(RegisterDescriptor reg) const {
    const std::string q = reg.toString();
    const auto s = name(reg);
    return s ? "\"" + StringUtility::cEscape(*s) + "\" " + q : q;
}

std::string
RegisterDictionary::quadAndName(RegisterDescriptor reg) const {
    const std::string q = reg.toString();
    const auto s = name(reg);
    return s ? q + " \"" + StringUtility::cEscape(*s) + "\"" : q;
}

std::ostream&
operator<<(std::ostream &out, const RegisterDictionary &dict) {
    dict.print(out);
    return out;
}

RegisterDescriptor
RegisterDictionary::instructionPointerRegister() const {
    return instructionPointer_;
}

void
RegisterDictionary::instructionPointerRegister(RegisterDescriptor reg) {
    instructionPointer_ = reg;
}

void
RegisterDictionary::instructionPointerRegister(const std::string &name) {
    instructionPointer_ = findOrThrow(name);
}

RegisterDescriptor
RegisterDictionary::stackPointerRegister() const {
    return stackPointer_;
}

void
RegisterDictionary::stackPointerRegister(RegisterDescriptor reg) {
    stackPointer_ = reg;
}

void
RegisterDictionary::stackPointerRegister(const std::string &name) {
    stackPointer_ = findOrThrow(name);
}

RegisterDescriptor
RegisterDictionary::stackFrameRegister() const {
    return stackFrame_;
}

void
RegisterDictionary::stackFrameRegister(RegisterDescriptor reg) {
    stackFrame_ = reg;
}

void
RegisterDictionary::stackFrameRegister(const std::string &name) {
    stackFrame_ = findOrThrow(name);
}

RegisterDescriptor
RegisterDictionary::stackSegmentRegister() const {
    return stackSegment_;
}

void
RegisterDictionary::stackSegmentRegister(RegisterDescriptor reg) {
    stackSegment_ = reg;
}

void
RegisterDictionary::stackSegmentRegister(const std::string &name) {
    stackSegment_ = findOrThrow(name);
}

RegisterDescriptor
RegisterDictionary::callReturnRegister() const {
    return callReturn_;
}

void
RegisterDictionary::callReturnRegister(RegisterDescriptor reg) {
    callReturn_ = reg;
}

void
RegisterDictionary::callReturnRegister(const std::string &name) {
    callReturn_ = findOrThrow(name);
}

} // namespace
} // namespace

#endif
