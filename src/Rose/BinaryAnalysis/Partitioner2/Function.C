#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>

#include <Rose/BinaryAnalysis/Partitioner2/DataBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Exception.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <integerOps.h>                                 // rose

#include <SgAsmFunction.h>
#include <SgAsmInstruction.h>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

Function::~Function() {}

Function::Function()
    : entryVa_(0), reasons_(0), isFrozen_(false) {}

Function::Function(rose_addr_t entryVa, const std::string &name, unsigned reasons)
    : entryVa_(entryVa), name_(name), reasons_(reasons), isFrozen_(false) {
    bblockVas_.insert(entryVa);
}

Function::Ptr
Function::instance(rose_addr_t entryVa, const std::string &name, unsigned reasons) {
    return Ptr(new Function(entryVa, name, reasons));
}

Function::Ptr
Function::instance(rose_addr_t entryVa, unsigned reasons) {
    return Ptr(new Function(entryVa, "", reasons));
}

rose_addr_t
Function::address() const {
    return entryVa_;
}

const std::string&
Function::name() const {
    return name_;
}

void
Function::name(const std::string &name) {
    name_ = name;
}

const std::string&
Function::demangledName() const {
    return demangledName_.empty() ? name_ : demangledName_;
}

void
Function::demangledName(const std::string &name) {
    demangledName_ = name;
}

const std::string&
Function::comment() const {
    return comment_;
}

void
Function::comment(const std::string &s) {
    comment_ = s;
}

const SourceLocation&
Function::sourceLocation() const {
    return sourceLocation_;
}

void
Function::sourceLocation(const SourceLocation &loc) {
    sourceLocation_ = loc;
}

unsigned
Function::reasons() const {
    return reasons_;
}

void
Function::reasons(unsigned reasons) {
    reasons_ = reasons;
}

void
Function::insertReasons(unsigned reasons) {
    reasons_ = (reasons_ & 0xffff0000) | reasons;
}

void
Function::eraseReasons(unsigned reasons) {
    reasons_ &= ~((0xffff0000 & reasons) | ((reasons & 0xffff) != 0 ? 0xffff : 0x0));
}

const std::string&
Function::reasonComment() const {
    return reasonComment_;
}

void
Function::reasonComment(const std::string &s) {
    reasonComment_ = s;
}

const std::set<rose_addr_t>&
Function::basicBlockAddresses() const {
    return bblockVas_;
}

bool
Function::ownsBasicBlock(rose_addr_t bblockVa) const {
    return bblockVas_.find(bblockVa) != bblockVas_.end();
}

bool
Function::insertBasicBlock(rose_addr_t bblockVa) {
    ASSERT_forbid(isFrozen_);
    bool wasInserted = bblockVas_.insert(bblockVa).second;
    if (wasInserted)
        clearCache();
    return wasInserted;
}

void
Function::eraseBasicBlock(rose_addr_t bblockVa) {        // no-op if not existing
    ASSERT_forbid(isFrozen_);
    ASSERT_forbid2(bblockVa==entryVa_, "function entry block cannot be removed");
    clearCache();
    bblockVas_.erase(bblockVa);
}

const std::vector<DataBlock::Ptr>&
Function::dataBlocks() const {
    return dblocks_;
}

bool
Function::insertDataBlock(const DataBlock::Ptr &dblock) {
    if (isFrozen_)
        throw Exception(printableName() + " is frozen or attached to the CFG/AUM");
    if (dblock==NULL)
        return false;
    bool retval = insertUnique(dblocks_, dblock, sortDataBlocks);
    if (retval)
        clearCache();
    return retval;
}

DataBlock::Ptr
Function::eraseDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_forbid2(isFrozen(), "function must be modifiable to erase data block");
    DataBlock::Ptr retval;
    if (dblock) {
        std::vector<DataBlock::Ptr>::iterator lb = std::lower_bound(dblocks_.begin(), dblocks_.end(), dblock, sortDataBlocks);
        if (lb!=dblocks_.end() && (*lb)->extent() == dblock->extent()) {
            retval = *lb;
            dblocks_.erase(lb);
            clearCache();
        }
    }
    return retval;
}

DataBlock::Ptr
Function::dataBlockExists(const DataBlock::Ptr &dblock) const {
    Sawyer::Optional<DataBlock::Ptr> found;
    if (dblock)
        found = getUnique(dblocks_, dblock, sortDataBlocks);
    return found ? *found : DataBlock::Ptr();
}

AddressIntervalSet
Function::dataAddresses() const {
    AddressIntervalSet retval;
    for (const DataBlock::Ptr &db: dblocks_)
        retval.insert(db->extent());
    return retval;
}

bool
Function::isFrozen() const {
    return isFrozen_;
}

void
Function::replaceOrInsertDataBlock(const DataBlock::Ptr &dblock) {
    ASSERT_not_null(dblock);
    replaceOrInsert(dblocks_, dblock, sortDataBlocks);
}

std::string
Function::printableName() const {
    std::string s = "function " + StringUtility::addrToString(address());
    const std::string &name = demangledName();
    if (!name.empty())
        s += " \"" + StringUtility::cEscape(name) + "\"";
    return s;
}

bool
Function::isThunk() const {
    if (0 == (reasons_ & SgAsmFunction::FUNC_THUNK))
        return false;
    if (bblockVas_.size() != 1)
        return false;
    return true;
}

size_t
Function::nBasicBlocks() const {
    return bblockVas_.size();
}

size_t
Function::nDataBlocks() const {
    return dblocks_.size();
}

BS::SValue::Ptr
Function::stackDeltaOverride() const {
    return stackDeltaOverride_;
}

void
Function::stackDeltaOverride(const BS::SValue::Ptr &delta) {
    stackDeltaOverride_ = delta;
}

BS::SValue::Ptr
Function::stackDelta() const {
    if (stackDeltaOverride_ != NULL)
        return stackDeltaOverride_;
    return stackDeltaAnalysis_.functionStackDelta();
}

int64_t
Function::stackDeltaConcrete() const {
    BS::SValue::Ptr v = stackDelta();
    if (v && v->toSigned()) {
        return v->toSigned().get();
    } else {
        return SgAsmInstruction::INVALID_STACK_DELTA;
    }
}

const StackDelta::Analysis&
Function::stackDeltaAnalysis() const {
    return stackDeltaAnalysis_;
}

StackDelta::Analysis&
Function::stackDeltaAnalysis() {
    return stackDeltaAnalysis_;
}

const CallingConvention::Analysis&
Function::callingConventionAnalysis() const {
    return ccAnalysis_;
}

CallingConvention::Analysis&
Function::callingConventionAnalysis() {
    return ccAnalysis_;
}

CallingConvention::Definition::Ptr
Function::callingConventionDefinition() {
    return ccDefinition_;
}

void
Function::callingConventionDefinition(const CallingConvention::Definition::Ptr &ccdef) {
    ccDefinition_ = ccdef;
}

const Sawyer::Cached<bool>&
Function::isNoop() const {
    return isNoop_;
}

void
Function::freeze() {
    isFrozen_ = true;
}

void
Function::thaw() {
    isFrozen_ = false;
}

} // namespace
} // namespace
} // namespace

#endif
