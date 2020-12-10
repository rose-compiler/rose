#include <rosePublicConfig.h>
#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT) && __cplusplus >= 201103L
#include <sage3basic.h>
#include <BinaryInstructionCache.h>
#include <Disassembler.h>

namespace Rose {
namespace BinaryAnalysis {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ManagedInstruction
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmInstruction*
ManagedInstruction::take() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (ABSENT == state)
        makePresentNS();
    if (u.ast && u.ast->cacheLockCount() != 0)
        throw InstructionCache::Exception("cannot take ownership of a locked AST");

    SgAsmInstruction *retval = u.ast;
    if (retval) {
        state = ABSENT;
        u.va = retval->get_address();
    }

    return retval;
}

void
ManagedInstruction::evict() {
    // FIXME: This currently does nothing.  A debug mode of operation could mark the SgAsmInstruction as being deleted but not
    // actually delete it--rather trap all member function invocations.
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LockedInstruction
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LockedInstruction::LockedInstruction()
        : insn(nullptr) {}

LockedInstruction::LockedInstruction(const InstructionPtr &insn) {
    if (insn) {
        *this = insn.lock();
    } else {
        reset();
    }
}

LockedInstruction::LockedInstruction(const LockedInstruction &other)
    : insn(other.insn) {
    if (insn)
        insn->adjustCacheLockCount(+1);
}

LockedInstruction&
LockedInstruction::operator=(const LockedInstruction &other) {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    if (insn != other.insn) {
        if (insn)
            insn->adjustCacheLockCount(-1);
        insn = other.insn;
        if (insn)
            insn->adjustCacheLockCount(+1);
    }
    return *this;
}

void
LockedInstruction::reset() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (insn) {
        insn->adjustCacheLockCount(-1);
        insn = nullptr;
    }
}

SgAsmInstruction*
LockedInstruction::get() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return insn;
}

SgAsmInstruction&
LockedInstruction::operator*() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_not_null(insn);
    return *insn;
}

LockedInstruction::operator bool() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return insn != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InstructionPtr
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
InstructionPtr
InstructionPtr::instance(InstructionCache *cache, rose_addr_t va) {
  InstructionPtr retval;
  retval.mi_ = std::shared_ptr<ManagedInstruction>(new ManagedInstruction(cache, va));
  return retval;
}

// class method
InstructionPtr
InstructionPtr::instance(InstructionCache *cache) {
  InstructionPtr retval;
  retval.mi_ = std::shared_ptr<ManagedInstruction>(new ManagedInstruction(cache));
  return retval;
}

void
InstructionPtr::reset() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    mi_.reset();
}

SgAsmInstruction*
InstructionPtr::take() {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    if (ManagedInstruction *mi = mi_.get()) {
        return mi->take();
    } else {
        return nullptr;
    }
}

LockedInstruction
InstructionPtr::lock() const {
    InstructionPtr retval;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return mi_ ? (*mi_.get()).lock() : LockedInstruction();
}

bool InstructionPtr::operator==(const InstructionPtr &other) const {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    return mi_ == other.mi_;
}

bool InstructionPtr::operator!=(const InstructionPtr &other) const {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    return mi_ != other.mi_;
}

bool InstructionPtr::operator<=(const InstructionPtr &other) const {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    return mi_ <= other.mi_;
}

bool InstructionPtr::operator>=(const InstructionPtr &other) const {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    return mi_ >= other.mi_;
}

bool InstructionPtr::operator<(const InstructionPtr &other) const {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    return mi_ < other.mi_;
}

bool InstructionPtr::operator>(const InstructionPtr &other) const {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    return mi_ > other.mi_;
}

bool InstructionPtr::operator==(const std::nullptr_t) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return mi_ && !(*mi_).isNull() ? false : true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InstructionCache
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

InstructionPtr
InstructionCache::get(rose_addr_t va) {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    auto found = insns_.find(va);
    if (found != insns_.end())
        return found->second;

    // By not actually doing the decoding yet, this statement executes fast and we're not penalized for holding the mutex the
    // whole time. By time we do the decoding, the only locking that's necessary is to lock the individual pointer-like
    // objects--no need to lock the whole cache.
    auto mi = memory_->at(va).require(MemoryMap::EXECUTABLE).exists() ?
              InstructionPtr::instance(this, va) :
              InstructionPtr::instance(this);
    insns_[va] = mi;
    return mi;
}

LockedInstruction
InstructionCache::lock(rose_addr_t va) {
    return get(va).lock();
}

SgAsmInstruction*
InstructionCache::decode(rose_addr_t va) {
  ASSERT_not_null(decoder_);
  ASSERT_not_null(memory_);

  // We can assume that the memory map is not changing, thus the access is thread safe.
  if (!memory_->at(va).require(MemoryMap::EXECUTABLE).exists())
      return nullptr; // invalid memory location

  // The following code assumes that the disassembleOne is thread safe. Decoders implement thread safety either by having no
  // mutable state (the best), having no mutable global state and protecting each disassembler with an object mutex (not
  // great), or having mutable global state and protecting all instances of that disassembler type with a single global mutex
  // (the worst).
  SgAsmInstruction *insn = nullptr;
  try {
      insn = decoder_->disassembleOne(memory_, va);
  } catch (const Disassembler::Exception &e) {
      insn = decoder_->makeUnknownInstruction(e);
  }
  ASSERT_not_null(insn);
  return insn;
}

void
InstructionCache::evict() {
    // FIXME: does nothing at this point. Eventually it will choose the best ASTs to evict and remove them to try to keep the
    // total amount of memory used below a certain user-specified threshold. Remember to update the thread safety documentatoin
    // for this class once it's implemented.
}

} // namespace
} // namespace
#endif
