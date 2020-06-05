#ifndef ROSE_BinaryAnalysis_InstructionCache_H
#define ROSE_BinaryAnalysis_InstructionCache_H
#include <rosePublicConfig.h>
#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT) && __cplusplus >= 201103L
#include <memory>
#include <unordered_map>

namespace Rose {
namespace BinaryAnalysis {

class Disassembler; // from Disassembler.h, but we only need the forward here.
class InstructionCache;
class InstructionPtr;
class LockedInstruction;
class ManagedInstruction;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ManagedInstruction
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Wrapper for AST that can be evicted.
 *
 *  This is primarily an internal object. See @ref InstructionPtr instead.
 *
 *  This object either points to an @ref SgAsmInstruction AST or to enough information to recreate it. Every one of these
 *  objects is owned by an @ref InstructionCache. It has a very limited public API: essentially just an arrow operator
 *  that returns a pointer to the AST. The arrow operator recreates the AST if necessary. */
class ManagedInstruction {
private:
    // Every ManagedInstruction is owned by a cache. The user might create additional shared pointers to this object, but as long
    // as any ManagedInstruction object exists and can potentially be in the ABSENT state, we need to have a cache that can
    // reconstruct the AST.
    InstructionCache *cache; // not null, set by constructor and never changed

    // Protects all following data members
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;

    // As is typical of cache-like objects, most of the data members are mutable because the some of the member functions that
    // modify them are conceptually const. For example, the operator-> is simply a dereference from the caller's point of void
    // and is thus const, but under the covers it needs to be able to convert this object from the ABSENT state to the PRESENT
    // state.

    // "time" of last dereference. This informs the cache eviction algorithm.
    mutable size_t lastAccess;

    // C++11 doesn't have discriminated unions like C++17, so we do it the hard way.
    enum State {
        ABSENT,                          // the pointer is non-null but the AST is not present 
        PRESENT                          // the AST is present or is a null pointer
    };
    mutable State state;
    union U {
        SgAsmInstruction *ast;         // the AST or null when in the PRESENT state
        rose_addr_t va;                 // the instruction starting address when in the ABSENT state.
    };
    mutable U u;

private:
    friend class InstructionCache;

    ManagedInstruction() = delete;
    ManagedInstruction(const ManagedInstruction&) = delete;
    ManagedInstruction& operator=(const ManagedInstruction&) = delete;

    ManagedInstruction(InstructionCache *cache, rose_addr_t va)
        : cache{cache}, state{ABSENT}, u{.va = va} {
        ASSERT_not_null(cache);
    }

    explicit ManagedInstruction(InstructionCache *cache)
        : cache{cache}, state{PRESENT}, u{.ast = nullptr} {
        ASSERT_not_null(cache);
    }

    // There is no safe way to do this with implicit locking. Any reference we would return could be held indefinitely by the
    // caller and there's no way we can automatically lock it.
    SgAsmInstruction& operator*() const = delete;

public:
    /** Access an instruction member.
     *
     *  Assuming this is not a null pointer, this method reconstructs the instruction AST if necessary, notifies the associated
     *  cache that the instruction has been used, and returns a new instruction pointer whose only purpose is to cause the
     *  instruction to be temporarily locked in the cache so some other thread doesn't evict it while we're using it. If the AST
     *  is null then a null pointer is returned with no attempt to create an AST.
     *
     *  Thread safety: This method is thread safe. */
    LockedInstruction operator->() const;               // hot

private:
    friend class InstructionPtr;

    // True if the underlying instructon is a null pointer.
    bool isNull() const;                                // hot

    // Create a locking pointer around the AST, and mark the AST as having been accessed.
    LockedInstruction lock() const;                     // hot

    // Make sure the AST is present and return a special pointer that causes it to be locked in the cache. The function is const
    // because it's typically called from a const context (pointer dereference) and from the user's point of void is constant even
    // though under the covers it's creating a new AST and swapping it into this object.
    LockedInstruction makePresentNS() const;            // hot

    // Evicts the AST from memory, deleting it from this object and replacing it with only the instruction address. The
    // instruction address, together with the information stored in the cache, is enough to recreate the AST if we ever need it
    // again.
    void evict();
    
    // Update the last access time used by the cache eviction algorithm.  The function is const because it's typically called
    // in a const context (pointer dereferencing).
    void updateTimerNS() const;                         // hot

    // Take the AST and its ownership away from this object, returning the AST. Throws an exception if the AST is locked, since
    // its not possible for the returned raw pointer and the cache to share ownership.
    SgAsmInstruction* take();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LockedInstruction
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Smart pointer that locks an instruction AST.
 *
 *  This class is primarily used internally to implement temporary instruction locks in order to prevent the instruction from being
 *  evicted by an @ref InstructionCache.  Users will generally interact directly with @ref InstructionPtr instead, and these @ref
 *  LockedInstruction pointers are returned as temporaries.
 *
 *  A @ref LockedInstruction pointer is like a shared-ownership smart pointer except instead of adjusting a reference count it
 *  adjusts a lock count. Any @ref SgAsmInstruction AST node that has a positive lock count will not be evicted from an @ref
 *  InstructionCache. */
class LockedInstruction {
private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_; // protects all following data members
    SgAsmInstruction *insn;

public:
    /** Construct a null pointer. */
    LockedInstruction();

    /** Point to a specific instruction.
     *
     *  The lock count for the instruction is incremented if @p insn is non-null. */
    explicit LockedInstruction(SgAsmInstruction *insn); // hot

    /** Point to a specific instruction.
     *
     *  The instruction is loaded into memory (if it had been evicted) and is then locked into the cache by incrementing its lock
     *  count. */
    explicit LockedInstruction(const InstructionPtr &insn);

    /** Copy constructor.
     *
     *  The copy constructor increments the lock count on the instruction if @p other is non-null. */
    LockedInstruction(const LockedInstruction &other);

    /** Assignment.
     *
     *  The lock counts for @p this and @p other are adjusted appropriately.
     *
     *  Thread safety: This method is thread safe. */
    LockedInstruction& operator=(const LockedInstruction &other);

    /** Destructor.
     *
     *  The destructor decrements the lock count if @p this was non-null. */
    ~LockedInstruction();                               // hot

    /** Reset to null.
     *
     *  After this call, the pointer will be a null pointer. If it had been pointing to anything previously, then the lock count
     *  for that object is decremented.
     *
     *  Thread safety: This method is thread safe. */
    void reset();

    /** Dereference.
     *
     *  Dereferences this pointer to return a reference to the underlying @ref SgAsmInstruction node. This pointer must not be a
     *  null pointer.
     *
     *  Thread safety: This method is thread safe. */
    SgAsmInstruction& operator*() const;

    /** Dereference.
     *
     *  Dereference this pointer to return a pointer to the @ref SgAsmInstruction node. This pointer must not be a null pointer.
     *
     *  Thread safety: This method is thread safe. */
    SgAsmInstruction* operator->() const;               // hot

    /** Get the raw pointer.
     *
     *  Thread safety: This method is thread safe. */
    SgAsmInstruction* get() const;

    /** Test whether this pointer is non-null.
     *
     *  Returns true if this pointer is non-null, false if null.
     *
     *  Thread safety: This method is thread safe. */
    explicit operator bool() const;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InstructionPtr
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Pointer to an instruction.
 *
 *  Since instruction ASTs are easily recomputed from the bytes that form the instruction, it should be possible to delete such
 *  ASTs and reconstruct them on demand (when the smart pointer is dereferenced), thus implementing a form of instruction
 *  caching. Unfortunately, ROSE doesn't have clear rules about who owns each node of an AST, and that makes it impossible to
 *  implement AST deletion in a safe manner (which is why most analysis authors never delete any AST nodes).
 *
 *  This class and the associated rules and exceptions (below) attempt to rectify this situation as much as possible for the
 *  limited case of instruction ASTs.  Instruction ASTs fall into two categories: (1) those ASTs that are managed through these
 *  smart pointers, known as "managed ASTs", and (2) those ASTs that are not managed through these smart pointers, known as
 *  "unmanaged ASTs".  A managed AST can be pointed to (at the root or any descendent node) in one of these ways:
 *
 *  @li The root of a managed AST can be pointed to by any number of smart pointers. The smart pointers implement a form of shared
 *  ownership.
 *
 *  @li Any node of a managed AST can be pointed to by a raw pointer provided that there exists at least one smart pointer
 *  pointing to the root of that AST and at least one of those smart pointers is in a locked state (see below).
 *
 *  @li The nodes of managed AST can have raw pointers to their immediate children, and each child can each have a raw pointer to
 *  its immediate parent.
 *
 *  Locking an AST prevents it from being deleted, such as preventing it from being evicted from an instruction cache by some
 *  other thread.  All locks are applied to the root of the AST, which in turn affects the whole AST. Locks are recursive: an
 *  AST can be locked numerous times and will become unlocked only when all locks are relinguished.  The two types are locks
 *  are RAII locks and temporary locks. Both types get unlocked automatically and are exception safe.
 *
 *  RAII locks are an explicit form of locking an AST and are implemented by the @ref InstructionGuard type.  This class has a
 *  constructor that takes a smart pointer argument. The lock is held until the guard variable is deleted, such as when it goes
 *  out of scope.
 *
 *  @code
 *  // Example of an RAII lock
 *  InstructionPtr insn = ...; // some instruction AST
 *  {
 *      InstructionGuard lock(insn); // the instruction is now locked
 *      ::traverse(::preorder, insn.raw()); // this SageIII function is safe to call
 *  } // the instruction is now unlocked (if this was the only lock)
 *  @endcode
 *
 *  The other type of locks, temporary locks, are fully automatic in that they get created and destroyed implicitly, but they
 *  last only for the duration of a C++ expression.
 *
 *  @code
 *  // Example of a temporary lock
 *  InstructionPtr insn = ...; // some instruction AST
 *  std::cout <<insn->get_operandsList()->get_operands().size(); // lock is held for lifetime of expression
 *  @endcode
 *
 *  Here are some implementation details that might clear things up...  An @ref InstructionPtr is basically like an
 *  std::shared_ptr except it has no @c operator* since it cannot be made safe in this situation, and its @ref operator-> "arrow
 *  operator" returns a different type.  The C++ arrow operator is recursive in nature, and we use that to our benefit. The @ref
 *  InstructionPtr arrow operator returns a reference to a @ref ManagedInstruction object (which is stored in the @ref
 *  InstructionCache). The @ref ManagedInstruction arrow operator ensures that the AST exists, recreating it if necessary, and
 *  returns a new @ref LockedInstruction object.  The @ref LockedInstruction arrow operator finally returns a @ref
 *  SgAsmInstruction pointer.
 *
 *  Thread safety: The smart pointers themselves and the associated locks are thread safe. The objects to which they point and
 *  the AST as a whole is generally not thread safe. Note that this is more strict than what the standard requires for
 *  std::shared_ptr. */
class InstructionPtr {
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_; // protects all following data members
    std::shared_ptr<ManagedInstruction> mi_;

public:
    /** Construct a null pointer. */
    InstructionPtr() {}

    /** Copy constructor. */
    InstructionPtr(const InstructionPtr &other)
        : mi_(other.mi_) {}


    /** Assignment operator.
     *
     *  Thread safety: This function is thread safe. */
    InstructionPtr& operator=(const InstructionPtr &other); // hot

    /** Clear the pointer.
     *
     *  Sets the pointer back to being a null pointer.
     *
     *  Thread safety: This function is thread safe. */
    void reset();

    // Dereferences are inherently unsafe because we have no opportunity to lock the instruction in a way that we can then unlock
    // it, and we have no control over the lifetime of the reference that we would return.
    SgAsmInstruction& operator*() const = delete;

    /** Dereference.
     *
     *  Dereference this pointer to obtain a locked pointer.
     *
     *  Thread safety:  This function is thread safe. */
    LockedInstruction operator->() const;               // hot

    /** Test whether pointer is non-null.
     *
     *  Returns true if non-null, false if null.
     *
     *  Thread safety: This function is thread safe. */
    explicit operator bool() const;                     // hot

    /** Return a locking pointer.
     *
     *  Returns a pointer that causes the underlying instruction AST to be locked in memory for at least the lifetime of the
     *  returned pointer. */
    LockedInstruction lock() const;

    /** Give ownership to caller.
     *
     *  The AST and its ownership is transferred to the caller. This operation fails by throwing an exception if the AST is
     *  currently locked. This pointer still exists and any operation that would normally reinstantiate the AST (such as
     *  pointer dereference) will do so.
     *
     *  Thread safety: This function is thread safe. */
    SgAsmInstruction* take();

    /** Comparison.
     *
     *  Thread safety: This function is thread safe.
     *
     * @{ */
    bool operator==(const InstructionPtr &other) const;
    bool operator!=(const InstructionPtr &other) const;
    bool operator<=(const InstructionPtr &other) const;
    bool operator>=(const InstructionPtr &other) const;
    bool operator<(const InstructionPtr &other) const;
    bool operator>(const InstructionPtr &other) const;
    bool operator==(std::nullptr_t) const;
    bool operator!=(std::nullptr_t) const;              // hot
    /** @} */

private:
    friend class InstructionCache;

    // Construct pointer to a ManagedInstruction that exists in an instruction cache. */
    static InstructionPtr instance(InstructionCache *cache, rose_addr_t va);
    static InstructionPtr instance(InstructionCache *cache);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InstructionCache
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Cache of instruction ASTs.
 *
 *  A cache is responible for handing out pointers to managed instruction ASTs and evicting those ASTs from memory when they
 *  haven't been used recently and it's safe to do so.  Each smart pointer handed out by the cache acts mostly like a raw
 *  pointer to a @ref SgAsmInstruction node from the user's point of view, but under the covers, the smart pointers coordinate
 *  with the cache so the cache knows when to evict ASTs. */
class InstructionCache: public Sawyer::SharedObject {
public:
    /** Exceptions thrown by this layer. */
    class Exception: public Rose::Exception {
    public:
        Exception(const std::string &mesg)
            : Rose::Exception(mesg) {}
        ~Exception() throw() {}
    };

private:
    MemoryMap::Ptr memory_; // not null, constant for life of object
    Disassembler *decoder_; // not null, constant for life of object

    mutable SAWYER_THREAD_TRAITS::Mutex mutex_; // protects all following data members
    std::unordered_map<rose_addr_t, InstructionPtr> insns_;

    InstructionCache(const InstructionCache&) = delete;
    InstructionCache& operator=(const InstructionCache&) = delete;

public:
    /** Construct a new instruction cache.
     *
     *  Each instruction cache is for a specific virtual memory and instruction decoder.  The memory mapping and content should
     *  not change under the cache since doing so could cause reconstructed evicted instructions to be different than the
     *  original instruction. */
    InstructionCache(const MemoryMap::Ptr &memory, Disassembler *decoder)
        : memory_(memory), decoder_(decoder) {
        ASSERT_not_null(memory);
        ASSERT_not_null(decoder);
    }

    /** Property: memory map providing the opcodes for the instructions.
     *
     *  The memory map is provided at construction time and neither the mapping nor the data should change (see constructor).
     *
     *  Thread safety: This function is thread safe. */
    MemoryMap::Ptr memoryMap() const {
        return memory_; // mo lock necessary since memory_ can never change
    }

    /** Property: the decoder used to construct the instruction ASTs from data in memory.
     *
     *  Thread safety: This function is thread safe. */
    Disassembler* decoder() const {
        return decoder_; // no lock necessary since decoder_ can never change.
    }
    
    /** Obtain the instruction at a specified address.
     *
     *  Returns a smart pointer for the instruction at the specified address.  If the byte at the adress is not executable then
     *  a null pointer is returned (it's easy to change memory permissions in ROSE--they don't have to remain the same as how
     *  the ELF or PE container parsing instantiated them). If the address is valid but the decoder cannot form a valid
     *  instruction from the opcode in memory, then a special, non-null "unknown" instruction is returned.
     *
     *  Thread safety: This function is thread safe. */
    InstructionPtr get(rose_addr_t va);

    /** Shortcut to obtain a lock.
     *
     *  This is the same as calling @ref get and then locking the return value. */
    LockedInstruction lock(rose_addr_t va);

    /** Garbage collection.
     *
     *  Runs one iteration of the cache eviction algorithm. */
    void evict();

private:
    friend class ManagedInstruction;

    // Decode a single instruction at the specified address. This function is thread safe.
    SgAsmInstruction* decode(rose_addr_t);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InstructionGuard
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** RAII lock guard.
 *
 *  Locks the specified instruction in memory for the lifetime of this object. */
class InstructionGuard {
    // The InstructionGuard was originally slightly more complicated, but the intruction of the automatic temporary locking
    // made it a lot simpler! All we need to do is hold onto a locked instruction pointer. However, we keep this class around because
    // it's better documentation for the programmer's intent than simply holding a locked pointer.
    LockedInstruction lock;

public:
    /** Constuct the object and acquire locks. */
    explicit InstructionGuard(const InstructionPtr &insn)
        : lock(insn) {}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline definitions for hot functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline InstructionPtr&
InstructionPtr::operator=(const InstructionPtr &other) {
    SAWYER_THREAD_TRAITS::LockGuard2 lock(mutex_, other.mutex_);
    mi_ = other.mi_;
    return *this;
}

inline LockedInstruction
InstructionPtr::operator->() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_not_null(mi_);
    ManagedInstruction &mi = *mi_.get();
    return mi.lock();
}

inline LockedInstruction
ManagedInstruction::operator->() const {
    return lock();
}

inline LockedInstruction
ManagedInstruction::lock() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    updateTimerNS();
    return makePresentNS();
}

inline void
ManagedInstruction::updateTimerNS() const {
    static size_t nextTimer = 0;
    lastAccess = ++nextTimer;
}

inline LockedInstruction
ManagedInstruction::makePresentNS() const {
    if (ABSENT == state) {                              // unlikely
        SgAsmInstruction *decoded = cache->decode(u.va);
        ASSERT_not_null(decoded); // at worst, the decoder will return an unknown instruction
        state = PRESENT; // no-throw
        u.ast = decoded; // no-throw
    }
    return LockedInstruction{u.ast};
}

inline
LockedInstruction::LockedInstruction(SgAsmInstruction *insn)
    : insn(insn) {
    if (insn)
        insn->adjustCacheLockCount(+1);                 // ROSETTA generated, thus cannot be inlined
}

inline
LockedInstruction::~LockedInstruction() {
  if (insn)
      insn->adjustCacheLockCount(-1);
}

inline SgAsmInstruction*
LockedInstruction::operator->() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    ASSERT_not_null(insn);
    return insn;
}

inline
InstructionPtr::operator bool() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return mi_.get() ? !(*mi_).isNull() : false;
}

inline bool
ManagedInstruction::isNull() const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    // A null pointer can be in the absent state only if it was never yet in the present state. This is because all we know
    // about an absent pointer is it's address, not whether we can create an instruction AST at that address. Therefore, we
    // have to try to create the AST.
    makePresentNS();
    return u.ast == nullptr;
}

inline bool
InstructionPtr::operator!=(const std::nullptr_t) const {
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
    return mi_ && !(*mi_).isNull()? true : false;
}




} // namespace
} // namespacd
#endif
#endif
