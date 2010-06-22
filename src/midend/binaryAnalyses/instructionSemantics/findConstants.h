/* The primary purpose of this header file is to define a "policy" for instruction semantics such as for the
 * X86InstructionSemantics class. This particular policy is for tracking the flow of constant values. */

#ifndef findConstants_H
#define findConstants_H

#include "x86InstructionSemantics.h"
#include "integerOps.h"
#include "flowEquations.h"

#include <cassert>
#include <cstdio>
#include <boost/lexical_cast.hpp>

/** A LatticeElement contains the value of a register or memory location. The value can either be one of three types:
 *  1. An unknown value (X).  Each potential unknown value is given a unique identifying "name" (a positive integer) which
 *     distinguishes it from all other unknown values, whether they are equal or unequal.
 *
 *  2. A known offset from an unknown value (X+N). These are represented as the name of X plus the offset N. For instance, if
 *     register "ax" has an unknown value "X" named "v123" and we execute "add ax,0x4" then the result will be that register "ax"
 *     will have the value v123+4, or X+4.
 *
 *  3. A known constant, C.  All known constants have a zero name and the value is stored in the "offset" field. */
template <size_t Len>                         // Sums are modulo 2^Len
struct LatticeElement {
    bool isTop;
    uint64_t name;                            // 0 for constants, a nonzero ID number for everything else
    SgAsmx86Instruction* definingInstruction; // Functionally dependent on name (mostly for debugging)
    bool negate;                              // Switch between name+offset and -name+offset; should be false for constants
    uint64_t offset;                          // Offset from name

    /* Constructs a "top" lattice element */
    LatticeElement()
        : isTop(true), name(0), definingInstruction(NULL), negate(false), offset(0)
        {}

    /* Construct a named lattice element (no offset) */
    static LatticeElement nonconstant(uint64_t name, SgAsmx86Instruction* definingInstruction) {
        return LatticeElement(name, definingInstruction, false, 0);
    }

    /* Construct a non-top, named lattice element with optional offset. */
    LatticeElement(uint64_t name, SgAsmx86Instruction* definingInstruction, bool negate, uint64_t offset)
        : isTop(false), name(name), definingInstruction(definingInstruction), negate(negate),
          offset(offset & (IntegerOps::GenMask<uint64_t, Len>::value))
        {}

    /* Construct a named lattice element with optional offset. */
    LatticeElement(bool isTop, uint64_t name, SgAsmx86Instruction* definingInstruction, bool negate, uint64_t offset)
        : isTop(isTop), name(name), definingInstruction(definingInstruction), negate(negate),
          offset(offset & (IntegerOps::GenMask<uint64_t, Len>::value))
        {}

    /* Construct a constant lattice element */
    static LatticeElement constant(uint64_t c, SgAsmx86Instruction* definingInstruction) {
        return LatticeElement(0, definingInstruction, false, c);
    }

    friend bool operator==(const LatticeElement& a, const LatticeElement& b) {
        if (a.isTop != b.isTop) return false;
        if (a.isTop) return true;
        return a.name == b.name && (a.name == 0 || a.negate == b.negate) && a.offset == b.offset;
    }
    friend bool operator<(const LatticeElement& a, const LatticeElement& b) { // Arbitrary order
        if (a.isTop < b.isTop) return true;
        if (b.isTop < a.isTop) return false;
        if (a.name < b.name) return true;
        if (b.name < a.name) return false;
        if (a.name != 0 && a.negate < b.negate) return true;
        if (a.name != 0 && b.negate < a.negate) return false;
        return a.offset < b.offset;
    }
    void merge(const LatticeElement& elt, uint64_t newName, SgAsmx86Instruction* def) {
        if (elt.isTop) return;
        if (this->isTop) {
            *this = elt;
            return;
        }
        if (*this == elt)
            return;
        this->isTop = false;
        this->name = newName;
        this->definingInstruction = def;
        this->negate = false;
        this->offset = 0;
        return;
    }
};

/** Shows values like this (all integers are hexadecimal, offsets are signed):
 *    1. Unknown value:                      v5b9
 *    2. Known offset from unknown value:    v5b9+45
 *    3. Constant:                           0xfffffffc (-0x4)
 *    4. Top elements:                       <top>
 */
template <size_t Len>
std::ostream& operator<<(std::ostream& o, const LatticeElement<Len>& e)
{
    if (e.isTop) {
        o << "<top>";
    } else {
        uint64_t sign_bit = (uint64_t)1 << (Len-1);  /* e.g., 80000000 */
        uint64_t val_mask = sign_bit - 1;            /* e.g., 7fffffff */
        uint64_t negative = (e.offset & sign_bit) ? (~e.offset & val_mask) + 1 : 0; /*magnitude of negative value*/

        if (e.name!=0) {
            /* This is a named value rather than a constant. */
            const char *sign = e.negate ? "-" : "";
            o <<sign <<"v" <<std::dec <<e.name;
            if (negative) {
                o <<"-0x" <<std::hex <<negative;
            } else if (e.offset) {
                o <<"+0x" <<std::hex <<e.offset;
            }
        } else {
            /* This is a constant */
            ROSE_ASSERT(!e.negate);
            o  <<"0x" <<std::hex <<e.offset;
            if (negative)
                o <<" (-0x" <<std::hex <<negative <<")";
        }
        if (e.definingInstruction!=NULL)
            o << " [from " << unparseInstructionWithAddress(e.definingInstruction) << "]";
    }
    return o;
}

/** Counter to generate unique names for XVariables (and thereby, LatticeElements). */
extern uint64_t xvarNameCounter;

/** Instruction on which we are currently working. Set by FindConstantsPolicy::startInstruction, cleared by
 *  FindConstantsPolicy::finishInstruction, and accessed by the XVariable constructor. */
extern SgAsmx86Instruction* currentInstruction;

template <size_t Len>
struct XVariable: public Variable { /*Variable defined in flowEquations.h*/
    LatticeElement<Len> value;
    uint64_t myName;
    SgAsmx86Instruction* def;
    XVariable()
        : value(), myName(++xvarNameCounter), def(currentInstruction)
        {}

    /** Give the variable a new value, keeping track of changes via the Variable superclass. */
    void set(const LatticeElement<Len>& le) {
        LatticeElement<Len> newValue = value;
        newValue.merge(le, myName, def);
        if (value == newValue)
            return;
        value = newValue;
        this->pushChanges();
    }

    /** Gets the variable's current value. */
    LatticeElement<Len> get() const {
        return value;
    }
};

/** A pointer to an XVariable. */
template <size_t Len>
struct XVariablePtr {
    XVariable<Len>* var;
    XVariablePtr()
        : var(NULL)
        {}
    XVariablePtr(XVariable<Len>* var)
        : var(var)
        {}
    operator XVariable<Len>*() const {
        return var;
    }
    XVariable<Len>* operator->() const {
        return var;
    }
    static XVariablePtr bottom() {
        struct BottomConstraint: public Constraint {
            XVariablePtr<Len> var;
            BottomConstraint(XVariablePtr<Len> var)
                : var(var)
                {}
            virtual void run() const {
                var->set(LatticeElement<Len>::nonconstant(var->myName, var->def));
            }
            virtual void markDependencies() {}
        };
        XVariablePtr<Len> var = new XVariable<Len>();
        (new BottomConstraint(var))->activate();
        return var;
    }
};

template <size_t Len>
std::ostream& operator<<(std::ostream& o, XVariablePtr<Len> v) {
    o << v->value;
    return o;
}

/** Information about the contents of memory at a given address. The contents and address are both lattice elements and
 *  therefore can have unknown values. */
struct MemoryWrite {
    LatticeElement<32> address;
    LatticeElement<32> data;
    unsigned int len;
    friend bool operator==(const MemoryWrite& a, const MemoryWrite& b) {
        return a.address == b.address && a.data == b.data && a.len == b.len;
    }
    friend bool operator<(const MemoryWrite& a, const MemoryWrite& b) {
        return a.address < b.address;
    }
};

bool mayAlias(const MemoryWrite&, const MemoryWrite&);
bool mustAlias(const MemoryWrite&, const MemoryWrite&);

template <size_t From, size_t To>
XVariablePtr<To> extendByMSB(XVariablePtr<From>);

template <size_t From, size_t To, size_t Len>
XVariablePtr<To - From> extract(XVariablePtr<Len>);

/** A set of values stored in memory. */
/* FIXME: Why are addresses and data always 32 bits? Will this work for a 64-bit architecture? [RPM 2009-02-03] */
struct MemoryWriteSet {
    bool isTop;
    std::vector<MemoryWrite> writes;         /* Always sorted by address */

    MemoryWriteSet()
        : isTop(true), writes()
        {}

    /** Add a value to memory. Any existing writes that may overlap with the new data are removed from this MemoryWriteSet. */
    void addWrite(LatticeElement<32> address, LatticeElement<32> data, unsigned int len) {
        isTop = false;
        MemoryWrite mw;
        mw.address = address;
        mw.data = data;
        mw.len = len;
        std::vector<MemoryWrite> newWrites;
        for (size_t i = 0; i < writes.size(); ++i) {
            if (!mayAlias(writes[i], mw))
                newWrites.push_back(writes[i]);
        }
        newWrites.push_back(mw);
        writes = newWrites;
        std::sort(writes.begin(), writes.end());
    }

    /** Obtains the value stored at the specified memory address, returning true if the address is defined or false otherwise. */
    template <size_t Len> // In bits
    bool getValueAtAddress(LatticeElement<32> address, LatticeElement<Len>& result, uint32_t resultName,
                           SgAsmx86Instruction* resultDef) const {
        /* Construct the MemoryWrite object for the address in question since it's needed by mustAlias() */
        MemoryWrite mw;
        mw.address = address;
        mw.data = LatticeElement<32>::constant(0, resultDef);
        mw.len = Len / 8;

        /* Scan vector until we find a match and then return that value. */
        for (size_t i = 0; i < writes.size(); ++i) {
            if (mustAlias(writes[i], mw)) {
                //std::cout << "Found data " << writes[i].data << " for address " << address << std::endl;
                const LatticeElement<32>& data = writes[i].data;
                result = LatticeElement<Len>(data.isTop, data.name, data.definingInstruction, data.negate, data.offset);
                return true;
            }
        }

        /* No match found */
        result = isTop ? LatticeElement<Len>() : LatticeElement<Len>::nonconstant(resultName, resultDef);
        return false;
    }

    static MemoryWriteSet bottom() {
        MemoryWriteSet mws;
        mws.isTop = false;
        mws.writes.clear();
        return mws;
    }

    bool mergeIn(const MemoryWriteSet& o) { // Returns to determine whether changes were made
        if (o.isTop)
            return false;
        if (this->isTop) {
            *this = o;
            return !o.isTop;
        }
        bool result = !writes.empty();
        *this = bottom(); // FIXME [JJW]
        return result;
    }

    friend bool operator==(const MemoryWriteSet& a, const MemoryWriteSet& b) {
        return a.isTop == b.isTop && (a.isTop || a.writes == b.writes);
    }
};

struct MemoryVariable: public Variable {
    MemoryWriteSet mws;
    MemoryVariable()
        : mws()
        {}
    void set(const MemoryWriteSet& s) {
        if (mws.mergeIn(s)) this->pushChanges();
    }
    const MemoryWriteSet& get() const {
        return mws;
    }
};

template <size_t OutputLen>
struct NullaryConstraint: public Constraint {
    XVariablePtr<OutputLen> result;
    NullaryConstraint(XVariablePtr<OutputLen> result): result(result) {}
    virtual void run() const {
        uint64_t newVal = this->compute();
        result->set(LatticeElement<OutputLen>::constant(newVal, result->def));
    }
    virtual void markDependencies() {}
    virtual uint64_t compute() const = 0;
};

template <size_t InputLen, size_t OutputLen>
struct UnaryConstraint: public Constraint {
    XVariablePtr<OutputLen> result;
    XVariablePtr<InputLen> var;
    UnaryConstraint(XVariablePtr<OutputLen> result, XVariablePtr<InputLen> var)
        : result(result), var(var)
        {}
    virtual void run() const {
        LatticeElement<InputLen> le = var->get();
        if (le.isTop) {result->set(LatticeElement<OutputLen>()); return;}
        if (le.name != 0) {result->set(LatticeElement<OutputLen>::nonconstant(result->myName, result->def)); return;}
        uint64_t newVal = this->compute(le.offset);
        result->set(LatticeElement<OutputLen>::constant(newVal, result->def));
    }
    virtual void markDependencies() {
        addDependency(var);
    }
    virtual uint64_t compute(uint64_t a) const = 0;
};

#define UNARY_COMPUTATION(name, InLen, OutLen, Formula)                                                                        \
    XVariablePtr<(OutLen)> name(XVariablePtr<(InLen)> a) {                                                                     \
        XVariablePtr<(OutLen)> result = new XVariable<(OutLen)>();                                                             \
        struct IC: public UnaryConstraint<(InLen), (OutLen)> {                                                                 \
            IC(XVariablePtr<(OutLen)> result, XVariablePtr<(InLen)> var1)                                                      \
                : UnaryConstraint<(InLen), (OutLen)>(result, var1)                                                             \
                {}                                                                                                             \
            virtual uint64_t compute(uint64_t a) const {                                                                       \
                Formula                                                                                                        \
            }                                                                                                                  \
        };                                                                                                                     \
        (new IC(result, a))->activate();                                                                                       \
        return result;                                                                                                         \
    }

#define UNARY_COMPUTATION_SPECIAL(name, InLen1, OutLen, Formula)                                                               \
    XVariablePtr<(OutLen)> name(XVariablePtr<(InLen1)> a) {                                                                    \
        XVariablePtr<(OutLen)> result = new XVariable<(OutLen)>();                                                             \
        struct IC: public UnaryConstraint<(InLen1), (OutLen)> {                                                                \
            IC(XVariablePtr<(OutLen)> result, XVariablePtr<(InLen1)> var1)                                                     \
                : UnaryConstraint<(InLen1), (OutLen)>(result, var1)                                                            \
                {}                                                                                                             \
            virtual void run() const {                                                                                         \
                LatticeElement<(InLen1)> le1 = UnaryConstraint<(InLen1), (OutLen)>::var->get();                                \
                XVariablePtr<(OutLen)> result = UnaryConstraint<(InLen1), (OutLen)>::result;                                   \
                if (le1.isTop) {result->set(LatticeElement<(OutLen)>()); return;}                                              \
                Formula                                                                                                        \
            }                                                                                                                  \
            virtual uint64_t compute(uint64_t) const {abort();}                                                                \
        };                                                                                                                     \
        (new IC(result, a))->activate();                                                                                       \
        return result;                                                                                                         \
    }

template <size_t InputLen1, size_t InputLen2, size_t OutputLen>
struct BinaryConstraint: public Constraint {
    XVariablePtr<OutputLen> result;
    XVariablePtr<InputLen1> var1;
    XVariablePtr<InputLen2> var2;

    BinaryConstraint(XVariablePtr<OutputLen> result, XVariablePtr<InputLen1> var1, XVariablePtr<InputLen2> var2)
        : result(result), var1(var1), var2(var2)
        {}

    virtual void run() const {
        LatticeElement<InputLen1> le1 = var1->get();
        LatticeElement<InputLen2> le2 = var2->get();
        if (le1.isTop || le2.isTop) {
            result->set(LatticeElement<OutputLen>());
            return;
        }
        if (le1.name != 0 || le2.name != 0) {
            result->set(LatticeElement<OutputLen>::nonconstant(result->myName, result->def));
            return;
        }
        uint64_t newVal = this->compute(le1.offset, le2.offset);
        result->set(LatticeElement<OutputLen>::constant(newVal, result->def));
    }

    virtual void markDependencies() {
        addDependency(var1);
        addDependency(var2);
    }
    virtual uint64_t compute(uint64_t a, uint64_t b) const = 0;
};

#define BINARY_COMPUTATION(name, InLen1, InLen2, OutLen, Formula)                                                              \
    XVariablePtr<(OutLen)> name(XVariablePtr<(InLen1)> a, XVariablePtr<(InLen2)> b) {                                          \
        XVariablePtr<(OutLen)> result = new XVariable<(OutLen)>();                                                             \
        struct IC: public BinaryConstraint<(InLen1), (InLen2), (OutLen)> {                                                     \
            IC(XVariablePtr<(OutLen)> result, XVariablePtr<(InLen1)> var1, XVariablePtr<(InLen2)> var2)                        \
                : BinaryConstraint<(InLen1), (InLen2), (OutLen)>(result, var1, var2)                                           \
                {}                                                                                                             \
            virtual uint64_t compute(uint64_t a, uint64_t b) const {                                                           \
                Formula                                                                                                        \
            }                                                                                                                  \
        };                                                                                                                     \
        (new IC(result, a, b))->activate();                                                                                    \
        return result;                                                                                                         \
    }

#define BINARY_COMPUTATION_SPECIAL(name, InLen1, InLen2, OutLen, Formula)                                                      \
    XVariablePtr<(OutLen)> name(XVariablePtr<(InLen1)> a, XVariablePtr<(InLen2)> b) {                                          \
        XVariablePtr<(OutLen)> result = new XVariable<(OutLen)>();                                                             \
        struct IC: public BinaryConstraint<(InLen1), (InLen2), (OutLen)> {                                                     \
            IC(XVariablePtr<(OutLen)> result, XVariablePtr<(InLen1)> var1, XVariablePtr<(InLen2)> var2)                        \
                : BinaryConstraint<(InLen1), (InLen2), (OutLen)>(result, var1, var2)                                           \
                {}                                                                                                             \
            virtual void run() const {                                                                                         \
                LatticeElement<(InLen1)> le1 = BinaryConstraint<(InLen1), (InLen2), (OutLen)>::var1->get();                    \
                LatticeElement<(InLen2)> le2 = BinaryConstraint<(InLen1), (InLen2), (OutLen)>::var2->get();                    \
                XVariablePtr<(OutLen)> result = BinaryConstraint<(InLen1), (InLen2), (OutLen)>::result;                        \
                if (le1.isTop || le2.isTop) {                                                                                  \
                    result->set(LatticeElement<(OutLen)>());                                                                   \
                    return;                                                                                                    \
                }                                                                                                              \
                Formula                                                                                                        \
            }                                                                                                                  \
            virtual uint64_t compute(uint64_t, uint64_t) const {                                                               \
                abort();                                                                                                       \
            }                                                                                                                  \
        };                                                                                                                     \
        (new IC(result, a, b))->activate();                                                                                    \
        return result;                                                                                                         \
    }

template <size_t InputLen1, size_t InputLen2, size_t InputLen3, size_t OutputLen>
struct TernaryConstraint: public Constraint {
    XVariablePtr<OutputLen> result;
    XVariablePtr<InputLen1> var1;
    XVariablePtr<InputLen2> var2;
    XVariablePtr<InputLen3> var3;

    TernaryConstraint(XVariablePtr<OutputLen> result,
                      XVariablePtr<InputLen1> var1, XVariablePtr<InputLen2> var2, XVariablePtr<InputLen3> var3)
        : result(result), var1(var1), var2(var2), var3(var3)
        {}

    virtual void run() const {
        LatticeElement<InputLen1> le1 = var1->get();
        LatticeElement<InputLen2> le2 = var2->get();
        LatticeElement<InputLen3> le3 = var3->get();
        if (le1.isTop || le2.isTop || le3.isTop) {
            result->set(LatticeElement<OutputLen>());
            return;
        }
        if (le1.name != 0 || le2.name != 0 || le3.name != 0) {
            result->set(LatticeElement<OutputLen>::nonconstant(result->myName, result->def));
            return;
        }
        uint64_t newVal = this->compute(le1.offset, le2.offset, le3.offset);
        result->set(LatticeElement<OutputLen>::constant(newVal, result->def));
    }

    virtual void markDependencies() {
        addDependency(var1);
        addDependency(var2);
        addDependency(var3);
    }

    virtual uint64_t compute(uint64_t a, uint64_t b, uint64_t c) const = 0;
};

#define TERNARY_COMPUTATION(name, InLen1, InLen2, InLen3, OutLen, Formula)                                                     \
    XVariablePtr<(OutLen)> name(XVariablePtr<(InLen1)> a, XVariablePtr<(InLen2)> b, XVariablePtr<(InLen3)> c) {                \
        XVariable<(OutLen)>* result = new XVariable<(OutLen)>();                                                               \
        struct IC: public TernaryConstraint<(InLen1), (InLen2), (InLen3), (OutLen)> {                                          \
            IC(XVariablePtr<(OutLen)> result,                                                                                  \
               XVariablePtr<(InLen1)> var1, XVariablePtr<(InLen2)> var2, XVariablePtr<(InLen3)> var3)                          \
                : TernaryConstraint<(InLen1), (InLen2), (InLen3), (OutLen)>(result, var1, var2, var3)                          \
                {}                                                                                                             \
            virtual uint64_t compute(uint64_t a, uint64_t b, uint64_t c) const {                                               \
                Formula                                                                                                        \
            }                                                                                                                  \
        };                                                                                                                     \
        (new IC(result, a, b, c))->activate();                                                                                 \
        return result;                                                                                                         \
    }

#define TERNARY_COMPUTATION_SPECIAL(name, InLen1, InLen2, InLen3, OutLen, Formula)                                             \
    XVariablePtr<(OutLen)> name(XVariablePtr<(InLen1)> a, XVariablePtr<(InLen2)> b, XVariablePtr<(InLen3)> c) {                \
        XVariablePtr<(OutLen)> result = new XVariable<(OutLen)>();                                                             \
        struct IC: public TernaryConstraint<(InLen1), (InLen2), (InLen3), (OutLen)> {                                          \
            IC(XVariablePtr<(OutLen)> result,                                                                                  \
               XVariablePtr<(InLen1)> var1, XVariablePtr<(InLen2)> var2, XVariablePtr<(InLen3)> var3)                          \
                : TernaryConstraint<(InLen1), (InLen2), (InLen3), (OutLen)>(result, var1, var2, var3)                          \
                {}                                                                                                             \
            virtual void run() const {                                                                                         \
                LatticeElement<(InLen1)> le1 = TernaryConstraint<(InLen1), (InLen2), (InLen3), (OutLen)>::var1->get();         \
                LatticeElement<(InLen2)> le2 = TernaryConstraint<(InLen1), (InLen2), (InLen3), (OutLen)>::var2->get();         \
                LatticeElement<(InLen3)> le3 = TernaryConstraint<(InLen1), (InLen2), (InLen3), (OutLen)>::var3->get();         \
                XVariablePtr<(OutLen)> result = TernaryConstraint<(InLen1), (InLen2), (InLen3), (OutLen)>::result;             \
                if (le1.isTop || le2.isTop || le3.isTop) {                                                                     \
                    result->set(LatticeElement<(OutLen)>());                                                                   \
                    return;                                                                                                    \
                }                                                                                                              \
                Formula                                                                                                        \
            }                                                                                                                  \
            virtual uint64_t compute(uint64_t, uint64_t, uint64_t) const {                                                     \
                abort();                                                                                                       \
            }                                                                                                                  \
        };                                                                                                                     \
        (new IC(result, a, b, c))->activate();                                                                                 \
        return result;                                                                                                         \
    }


template <size_t Len>
struct MergeConstraint: public Constraint {
    XVariablePtr<Len> result;
    XVariablePtr<Len> var1;
    MergeConstraint(XVariablePtr<Len> result, XVariablePtr<Len> var1)
        : result(result), var1(var1)
        {}
    virtual void run() const {
        result->set(var1->get());
    }
    virtual void markDependencies() {
        addDependency(var1);
    }
};

struct MemoryMergeConstraint: public Constraint {
    MemoryVariable* result;
    MemoryVariable* var1;
    MemoryMergeConstraint(MemoryVariable* result, MemoryVariable* var1)
        : result(result), var1(var1)
        {}
    virtual void run() const {
        result->set(var1->get());
    }
    virtual void markDependencies() {
        addDependency(var1);
    }
};

struct RegisterSet {
    XVariablePtr<32> gpr[8];
    XVariablePtr<16> segreg[6];
    XVariablePtr<1> flag[16];
    MemoryVariable* memoryWrites; // Undefined elements are bottom, no two elements can satisfy mayAlias
    
    RegisterSet() {
        for (size_t i = 0; i < 8; ++i)
            gpr[i] = new XVariable<32>();
        for (size_t i = 0; i < 6; ++i)
            segreg[i] = new XVariable<16>();
        for (size_t i = 0; i < 16; ++i)
            flag[i] = new XVariable<1>();
        memoryWrites = new MemoryVariable();
    }

    void setToBottom() {
        for (size_t i = 0; i < 8; ++i)
            gpr[i] = XVariablePtr<32>::bottom();
        for (size_t i = 0; i < 6; ++i)
            segreg[i] = XVariablePtr<16>::bottom();
        for (size_t i = 0; i < 16; ++i)
            flag[i] = XVariablePtr<1>::bottom();
        memoryWrites->set(MemoryWriteSet::bottom());
    }

    void mergeIn(const RegisterSet& rs) {
        for (size_t i = 0; i < 8; ++i)
            (new MergeConstraint<32>(gpr[i], rs.gpr[i]))->activate();
        for (size_t i = 0; i < 6; ++i)
            (new MergeConstraint<16>(segreg[i], rs.segreg[i]))->activate();
        for (size_t i = 0; i < 16; ++i)
            (new MergeConstraint<1>(flag[i], rs.flag[i]))->activate();
        (new MemoryMergeConstraint(memoryWrites, rs.memoryWrites))->activate();
    }

    /** Show register set, but only the parts that differ from @p base. */
    std::string diff(const RegisterSet &orig, std::string prefix="") {
        std::ostringstream s;
        for (size_t i=0; i<8; i++) {
            if (!(orig.gpr[i]->get()==gpr[i]->get())) {
                s <<prefix <<gprToString((X86GeneralPurposeRegister)i) <<" = " <<gpr[i] <<"\n";
            }
        }
        for (size_t i=0; i<6; i++) {
            if (!(orig.segreg[i]->get()==segreg[i]->get())) {
                s <<prefix <<segregToString((X86SegmentRegister)i) <<" = " <<segreg[i] <<"\n";
            }
        }
        for (size_t i=0; i<16; i++) {
            if (!(orig.flag[i]->get()==flag[i]->get())) {
                s <<prefix <<flagToString((X86Flag)i) << " = " <<flag[i] <<"\n";
            }
        }
        /* Show memory in this register set that is different than the original */
        if (!memoryWrites->get().isTop) {
            for (size_t i=0; i<memoryWrites->get().writes.size(); i++) {
                LatticeElement<32> addr = memoryWrites->get().writes[i].address;
                LatticeElement<32> orig_data;
                if (!orig.memoryWrites->get().getValueAtAddress(addr, orig_data/*out*/, 0, NULL) ||
                    !(orig_data==memoryWrites->get().writes[i].data)) {
                    s <<prefix <<"mem @ " <<addr <<" = " <<memoryWrites->get().writes[i].data <<"\n";
                }
            }
        }
        

        return s.str();
    }
    
};

std::ostream&
operator<<(std::ostream& o, const RegisterSet& rs);

struct FindConstantsPolicy {
    std::map<uint64_t, RegisterSet> rsets;
    RegisterSet currentRset, *initialRset;
    uint32_t addr;
    XVariablePtr<32> newIp; // To determine if it is a constant
    
    FindConstantsPolicy()
        : initialRset(NULL), addr(0)
        {}

    /* Use this constructor when performing constant propagation analysis on a function where you want the entry point of the
     * function to use the specified initial values. See FindConstantsPolicy::startInstruction() for how this is used. */
    FindConstantsPolicy(RegisterSet *initial_rs)
        : initialRset(initial_rs), addr(0) {}


    XVariablePtr<32> readGPR(X86GeneralPurposeRegister r) {
        return currentRset.gpr[r];
    }

    void writeGPR(X86GeneralPurposeRegister r, XVariablePtr<32> value) {
        currentRset.gpr[r] = value;
    }

    XVariablePtr<16> readSegreg(X86SegmentRegister sr) {
        return currentRset.segreg[sr];
    }

    void writeSegreg(X86SegmentRegister sr, XVariablePtr<16> val) {
        currentRset.segreg[sr] = val;
    }

    XVariablePtr<32> readIP() {
        return newIp;
    }

    void writeIP(XVariablePtr<32> n) {
        newIp = n;
    }

    XVariablePtr<1> readFlag(X86Flag f) {
        return currentRset.flag[f];
    }

    void writeFlag(X86Flag f, XVariablePtr<1> value) {
        currentRset.flag[f] = value;
    }

    /* Only used by number<>(uint64_t) below, but gcc-4.0.1 on OSX (Apple Inc. build 5484) does not like this struct to be
     * defined inside that function.  It results in strange warnings about an unrelated function in
     * x86InstructionSemantics.C having no return value. [RPM 2010-05-27] */
    template <size_t Len>
    struct NumberConstraint: public NullaryConstraint<Len> {
        uint64_t val;
        NumberConstraint(XVariablePtr<Len> var, uint64_t val)
            : NullaryConstraint<Len>(var), val(val)
            {}
        virtual uint64_t compute() const {
            return val;
        }
    };
        
    template <size_t Len>
    XVariablePtr<Len> number(uint64_t n) {
        XVariablePtr<Len> var = new XVariable<Len>();
        (new NumberConstraint<Len>(var, n))->activate();
        return var;
    }

    // Only safe when MSBs don't matter (i.e., you can't extract bits from something and then use this to put in zeros -- the
    // original bits will probably appear again)
    template <size_t Len, size_t Len2>
    static UNARY_COMPUTATION_SPECIAL(extendByMSB, Len, Len2, {
            result->set(LatticeElement<Len2>(le1.name, le1.definingInstruction, le1.negate, le1.offset));
        })

    template <size_t From, size_t To, size_t Len>
    static UNARY_COMPUTATION_SPECIAL(extract, Len, To - From, {
            if (From == 0) {
                result->set(LatticeElement<To - From>(le1.name, le1.definingInstruction, le1.negate, le1.offset));
                return;
            }
            if (le1.name != 0) {
                result->set(LatticeElement<To - From>::nonconstant(result->myName, result->def));
                return;
            }
            result->set(LatticeElement<To - From>::constant((le1.offset >> From) & (IntegerOps::SHL1<uint64_t, To - From>::value - 1),
                                                            result->def));
        })


    template <size_t Len1, size_t Len2>
    BINARY_COMPUTATION(concat, Len1, Len2, Len1 + Len2, {
            return a | (b << Len1);
        })

    XVariablePtr<1> false_() {
        return number<1>(0);
    }
    XVariablePtr<1> true_() {
        return number<1>(1);
    }
    XVariablePtr<1> undefined_() {
        return new XVariable<1>();
    }

    template <size_t Len>
    UNARY_COMPUTATION_SPECIAL(invert, Len, Len, {
            if (le1.name == 0)
                result->set(LatticeElement<Len>::constant(~le1.offset, result->def));
            else
                result->set(LatticeElement<Len>(le1.name, le1.definingInstruction, !le1.negate, ~le1.offset));
        })

    template <size_t Len>
    UNARY_COMPUTATION_SPECIAL(negate, Len, Len, {
            if (le1.name == 0)
                result->set(LatticeElement<Len>::constant(-le1.offset, result->def));
            else
                result->set(LatticeElement<Len>(le1.name, le1.definingInstruction, !le1.negate, -le1.offset));
        })

    template <size_t Len>
    BINARY_COMPUTATION(and_, Len, Len, Len, {return (a & b);})

    template <size_t Len>
    BINARY_COMPUTATION(or_, Len, Len, Len, {return (a | b);})

    template <size_t Len>
    BINARY_COMPUTATION_SPECIAL(xor_, Len, Len, Len, {
            if (le1 == le2) {
                result->set(LatticeElement<Len>::constant(0, result->def));
                return;
            }
            if (le1.name == 0 && le2.name == 0) {
                result->set(LatticeElement<Len>::constant(le1.offset ^ le2.offset, result->def));
                return;
            }
            result->set(LatticeElement<Len>::nonconstant(result->myName, result->def));
        })

    template <size_t From, size_t To>
    UNARY_COMPUTATION(signExtend, From, To, {return (IntegerOps::signExtend<From, To>(a));})

    template <size_t Len>
    XVariablePtr<Len> ite(XVariablePtr<1> sel, XVariablePtr<Len> ifTrue, XVariablePtr<Len> ifFalse) {
        XVariablePtr<Len> result = new XVariable<Len>();
        struct IteConstraint: public Constraint {
            XVariablePtr<Len> result;
            XVariablePtr<Len> ifTrue;
            XVariablePtr<Len> ifFalse;
            XVariablePtr<1> sel;
            IteConstraint(XVariablePtr<Len> result, XVariablePtr<1> sel, XVariablePtr<Len> ifTrue, XVariablePtr<Len> ifFalse)
                : result(result), ifTrue(ifTrue), ifFalse(ifFalse), sel(sel)
                {}
            virtual void run() const {
                LatticeElement<Len> res;
                if (sel->get().name != 0 || (sel->get().name == 0 && sel->get().offset == 1)) {
                    res.merge(ifTrue->get(), result->myName, result->def);
                }
                if (sel->get().name != 0 || (sel->get().name == 0 && sel->get().offset == 0)) {
                    res.merge(ifFalse->get(), result->myName, result->def);
                }
                result->set(res);
            }
            virtual void markDependencies() {
                addDependency(sel);
                addDependency(ifTrue);
                addDependency(ifFalse);
            }
        };
        (new IteConstraint(result, sel, ifTrue, ifFalse))->activate();
        return result;
    }

    template <size_t Len>
    UNARY_COMPUTATION(equalToZero, Len, 1, {return (a == 0);})

    template <size_t Len, size_t SCLen>
    UNARY_COMPUTATION(generateMask, SCLen, Len, {return IntegerOps::genMask<uint64_t>(a);})

#if 1
    template <size_t Len>
    BINARY_COMPUTATION_SPECIAL(add, Len, Len, Len, {
            if (le1.name == 0 || le2.name == 0) {
                result->set(LatticeElement<Len>(le1.name + le2.name, result->def,
                                                le1.negate || le2.negate, le1.offset + le2.offset));
                return;
            }
            if (le1.name == le2.name && le1.negate == !le2.negate) {
                result->set(LatticeElement<Len>::constant(le1.offset + le2.offset, result->def));
                return;
            }
            result->set(LatticeElement<Len>::nonconstant(result->myName, result->def));
        })
#else
    template <size_t Len>
    BINARY_COMPUTATION(add, Len, Len, Len, {return (a & b);})

#endif


    template <size_t Len>
    TERNARY_COMPUTATION_SPECIAL(add3, Len, Len, 1, Len, {
            if ((le1.name == 0) + (le2.name == 0) + (le3.name == 0)) {
                result->set(LatticeElement<Len>(le1.name + le2.name + le3.name, result->def,
                                                le1.negate || le2.negate || le3.negate,
                                                le1.offset + le2.offset + le3.offset));
                return;
            }
            if (le1.name == le2.name && le3.name == 0 && le1.negate == !le2.negate) {
                result->set(LatticeElement<Len>::constant(le1.offset + le2.offset + le3.offset, result->def));
                return;
            }
            result->set(LatticeElement<Len>::nonconstant(result->myName, result->def));
        })

    template <size_t Len>
    TERNARY_COMPUTATION(xor3, Len, Len, Len, Len, {return (a ^ b ^ c);})

    template <size_t Len>
    XVariablePtr<Len> addWithCarries(XVariablePtr<Len> a, XVariablePtr<Len> b, XVariablePtr<1> carryIn,
                                     XVariablePtr<Len>& carries) { // Full case
        XVariablePtr<Len + 1> aa = extendByMSB<Len, Len + 1>(a);
        XVariablePtr<Len + 1> bb = extendByMSB<Len, Len + 1>(b);
        XVariablePtr<Len + 1> result = add3(aa, bb, carryIn);
        carries = extract<1, Len + 1>(xor3(aa, bb, result));
        return extract<0, Len>(result);
    }

    template <size_t Len, size_t SALen>
    BINARY_COMPUTATION(rotateLeft, Len, SALen, Len, {
            return IntegerOps::rotateLeft<Len>(a, b);
        })

    template <size_t Len, size_t SALen>
    BINARY_COMPUTATION(rotateRight, Len, SALen, Len, {
            return IntegerOps::rotateRight<Len>(a, b);
        })

    template <size_t Len, size_t SALen>
    BINARY_COMPUTATION(shiftLeft, Len, SALen, Len, {
            return IntegerOps::shiftLeft<Len>(a, b);
        })

    template <size_t Len, size_t SALen>
    BINARY_COMPUTATION(shiftRight, Len, SALen, Len, {
            return IntegerOps::shiftRightLogical<Len>(a, b);
        })

    template <size_t Len, size_t SALen>
    BINARY_COMPUTATION(shiftRightArithmetic, Len, SALen, Len, {
            return IntegerOps::shiftRightArithmetic<Len>(a, b);
        })

    template <size_t Len1, size_t Len2>
    BINARY_COMPUTATION(signedMultiply, Len1, Len2, Len1 + Len2, {
            return (IntegerOps::signExtend<Len1, 64>(a) * IntegerOps::signExtend<Len2, 64>(b));
        })

    template <size_t Len1, size_t Len2>
    BINARY_COMPUTATION(unsignedMultiply, Len1, Len2, Len1 + Len2, {
            return (a * b);
        })

    template <size_t Len1, size_t Len2>
    BINARY_COMPUTATION(signedDivide, Len1, Len2, Len1, {
            return (IntegerOps::signExtend<Len1, 64>(a) / IntegerOps::signExtend<Len2, 64>(b));
        })

    template <size_t Len1, size_t Len2>
    BINARY_COMPUTATION(signedModulo, Len1, Len2, Len2, {
            return (IntegerOps::signExtend<Len1, 64>(a) % IntegerOps::signExtend<Len2, 64>(b))
                ;})

    template <size_t Len1, size_t Len2>
    BINARY_COMPUTATION(unsignedDivide, Len1, Len2, Len1, {
            if (0==b) throw std::string("division by zero");
            return (a / b);
        })

    template <size_t Len1, size_t Len2>
    BINARY_COMPUTATION(unsignedModulo, Len1, Len2, Len2, {
            return (a % b);
        })

    template <size_t Len>
    UNARY_COMPUTATION(leastSignificantSetBit, Len, Len, {
            for (int i = 0; i < (int)Len; ++i) {
                if (a & IntegerOps::shl1<uint64_t>(i))
                    return i;
            }
            return 0;
        })

    template <size_t Len>
    UNARY_COMPUTATION(mostSignificantSetBit, Len, Len, {
            for (int i = (int)Len - 1; i >= 0; --i) {
                if (a & IntegerOps::shl1<uint64_t>(i))
                    return i;
            }
            return 0;
        })

    XVariablePtr<32> filterIndirectJumpTarget(XVariablePtr<32> x) {
        return x;
    }
    XVariablePtr<32> filterCallTarget(XVariablePtr<32> x) {
        return x;
    }
    XVariablePtr<32> filterReturnTarget(XVariablePtr<32> x) {
        return x;
    }

    template <size_t Len> // In bits
    XVariablePtr<Len> readMemory(X86SegmentRegister segreg, XVariablePtr<32> addr, XVariablePtr<1> cond) {
        struct ReadMemoryConstraint: public Constraint {
            XVariablePtr<Len> result;
            MemoryVariable* memory;
            XVariablePtr<32> addr;
            virtual void run() const {
                LatticeElement<Len> resultRaw;
                const MemoryWriteSet &mws = memory->get();
                mws.getValueAtAddress<Len>(addr->get(), resultRaw, result->myName, result->def);
                result->set(resultRaw);
            }
            virtual void markDependencies() {
                addDependency(memory);
                addDependency(addr);
            }
        };
        ReadMemoryConstraint* c = new ReadMemoryConstraint();
        c->result = new XVariable<Len>();
        c->memory = currentRset.memoryWrites;
        c->addr = addr;
        c->activate();
        return c->result;
    }

    template <size_t Len>
    static MemoryVariable* memoryWriteHelper(MemoryVariable* memory, XVariablePtr<32> address, XVariablePtr<Len> data) {
        struct C: public Constraint {
            MemoryVariable* memory;
            XVariablePtr<32> address;
            XVariablePtr<Len> data;
            MemoryVariable* memoryOut;
            virtual void run() const {
                MemoryWriteSet mws = memory->get();
                mws.addWrite(address->get(), extendByMSB<Len, 32>(data)->get(), Len / 8);
                memoryOut->set(mws);
            }
            virtual void markDependencies() {addDependency(memory); addDependency(address); addDependency(data);}
        };
        C* c = new C();
        c->memory = memory;
        c->address = address;
        c->data = data;
        c->memoryOut = new MemoryVariable();
        c->activate();
        return c->memoryOut;
    }

    /** Writes @p data at the specified address. */
    template <size_t Len>
    void writeMemory(X86SegmentRegister segreg, XVariablePtr<32> addr, XVariablePtr<Len> data, XVariablePtr<1> cond) {
        currentRset.memoryWrites = memoryWriteHelper(currentRset.memoryWrites, addr, data);
    }

    /** Writes @p data at the specified address and following bytes, repeating @p repeat times. */
    template <size_t nbits>
    void writeMemory(X86SegmentRegister segreg, XVariablePtr<32> addr, XVariablePtr<nbits> data, XVariablePtr<32> repeat,
                     XVariablePtr<1> cond) {
        /* If repeat is a constant then perform the write that number of times. */
        if (0==repeat->get().name) {
            for (size_t i=0; i<repeat->get().offset; i++) {
                XVariablePtr<32> tmp_addr = add(addr, number<32>(i*nbits/8));
                currentRset.memoryWrites = memoryWriteHelper(currentRset.memoryWrites, tmp_addr, data);
            }
        } else {
            currentRset.memoryWrites = memoryWriteHelper(currentRset.memoryWrites, addr, data);
        }
    }

    void hlt() {} // FIXME

    void interrupt(uint8_t num) {
        currentRset.setToBottom();
    }

    XVariablePtr<64> rdtsc() { // FIXME
        return number<64>(0);
    }
    void startBlock(uint64_t addr) {}

    void finishBlock(uint64_t addr) {}

    /** Returns true if the specified instruction is "externally visible". The old implementation based the assessment on
     *  whether the specified instruction was the first instruction of an "externally visible" basic block. However, r4156
     *  removed SgAsmBlock::externallyVisible in favor of better algorithms to detect function boundaries, so now it's
     *  sufficient to just ask if the instruction starts a function. */
    bool isInstructionExternallyVisible(SgAsmInstruction* insn) const {
        return isFunctionEntry(insn);
    }

    /** Returns true if the specified instruction is the entry point of a function. */
    bool isFunctionEntry(SgAsmInstruction *insn) const {
        SgAsmFunctionDeclaration *fdefn = containingFunction(insn);
        ROSE_ASSERT(fdefn);
        SgAsmBlock *first_bb = isSgAsmBlock(fdefn->get_statementList()[0]);
        return first_bb->get_id()==insn->get_address();
    }
    
    /** Returns the function to which the specified instruction belongs. */
    SgAsmFunctionDeclaration *
    containingFunction(SgAsmInstruction *insn) const {
        SgAsmBlock *bb = isSgAsmBlock(insn->get_parent());
        ROSE_ASSERT(bb!=NULL);
        SgAsmFunctionDeclaration *fdefn = isSgAsmFunctionDeclaration(bb->get_parent());
        ROSE_ASSERT(fdefn!=NULL);
        return fdefn;
    }

    /** Called at the beginning of X86InstructionSemantics::processInstruction() */
    void startInstruction(SgAsmInstruction* insn) {
        addr = insn->get_address();
        newIp = number<32>(addr);
        if (isInstructionExternallyVisible(insn)) {
            if (initialRset) {
                rsets[addr] = *initialRset;
                initialRset = NULL;
            } else {
                rsets[addr].setToBottom();
            }
        }
        currentRset = rsets[addr];
        currentInstruction = isSgAsmx86Instruction(insn);
    }

    /** Called at the end of X86InstructionSemantics::processInstruction() to merge the current register set (as modified by
     *  the instruction recently processed) into the successor instructions. */
    void finishInstruction(SgAsmInstruction* insn) {
        currentInstruction = NULL;
        SgAsmx86Instruction* insnx = isSgAsmx86Instruction(insn);
        ROSE_ASSERT (insnx);
        std::vector<uint64_t> succs;
        if (newIp->get().name == 0) {
            /* We know the address of the next instruction. */
            succs.push_back(newIp->get().offset);
        } else {
            /* We don't know the address of the next instruction, so compute it from successors */
            uint64_t nextAddr = insnx->get_address() + insnx->get_raw_bytes().size();
            if (!x86InstructionIsUnconditionalBranch(insnx)) {
                succs.push_back(nextAddr);
            }
            if (isAsmBranch(insnx)) {
                uint64_t addr = 0;
                bool knownTarget = getAsmKnownBranchTarget(insnx, addr);
                if (knownTarget) {
                    succs.push_back(addr);
                }
            }
        }

        /* Merge result of processing instruction into register sets for successors */
        for (size_t i = 0; i < succs.size(); ++i) {
            uint64_t s = succs[i];
            rsets[s].mergeIn(currentRset);
        }
    }
};

/** Augment the findConstants policy to do some special things for some instructions. */
class CdeclFunctionPolicy : public FindConstantsPolicy {
    VirtualBinCFG::AuxiliaryInformation *info;
public:
    CdeclFunctionPolicy(VirtualBinCFG::AuxiliaryInformation *info, RegisterSet *rs)
        : FindConstantsPolicy(rs), info(info) {}

    /** Augments FindConstantsPolicy version. */
    void startInstruction(SgAsmInstruction* insn) {
        /* Any instruction that is a branch target should set the registerset to bottom rather than top. This isn't actually
         * the accurate thing to do, but it turns out that it works better this way for finding the signal handlers. */
        VirtualBinCFG::InstructionToAddressesMap::iterator found = info->incomingEdges.find(insn);
        if (found!=info->incomingEdges.end() && found->second.size()>1) {
            RegisterSet &rs = rsets[insn->get_address()];
            MemoryWriteSet saved = rs.memoryWrites->get();
            rs.setToBottom();
            rs.memoryWrites->set(saved);
        }

        FindConstantsPolicy::startInstruction(insn);

        /* GCC assumes that the direction flag (df) is zero on function entry. See gcc man page for -mcld switch. */
        LatticeElement<1> df = rsets[insn->get_address()].flag[x86_flag_df]->get();
        if (df.name!=0)
            writeFlag(x86_flag_df, false_());

#if 0   /*DEBUGGING: Show register set at start of instruction */
        std::cout <<"Initial RSET for [" <<unparseInstructionWithAddress(insn) <<"]\n" <<currentRset;
#endif
    }

    /* It is common for a function to align local variables on a particular boundary and this happens near the beginning of a
     * function by masking off the low-order bits of the stack pointer. For example:
     *     push ebp                   -- save stack frame
     *     mov  ebp, esp              -- create new stack frame
     *     sub  esp, 0xa8             -- allocate stack space for locals
     *     and  esp, 0xfffffff0       -- align stack on 16-byte boundary
     * The FindConstantsPolicy, when it encounters such an AND instruction with a named value (non-constant) in %esp, simply
     * gives %esp a new named value.  What we want to do instead is subtract 16 from the stack pointer, thus introducing some
     * padding between the top local variable and the bottom of the call frame. The stack after the AND looks like this:
     *     argN
     *     ...
     *     arg0
     *     return address
     *     old stack frame from [push ebp]
     *     padding we inserted from [and esp, 0xfffffff0]
     *     top local variable
     *     ...
     *     bottom local variable          <--- stack pointer points here
     *     
     * Note: The policy "and_" method is called for other instructions besides AND, some of which don't have two operands.
     */
    template<size_t Len>
    XVariablePtr<Len> and_(XVariablePtr<Len> a, XVariablePtr<Len> b) {
        XVariablePtr<Len> result = new XVariable<Len>();
        struct IC: public BinaryConstraint<Len, Len, Len> {
            IC(XVariablePtr<Len> result, XVariablePtr<Len> var1, XVariablePtr<Len> var2)
                : BinaryConstraint<Len, Len, Len>(result, var1, var2)
                {}
            virtual void run() const {
                /* "var1", "var2", and "result" here are initialized from "a", "b", and "result" above */
                LatticeElement<Len> le1 = BinaryConstraint<Len, Len, Len>::var1->get();
                LatticeElement<Len> le2 = BinaryConstraint<Len, Len, Len>::var2->get();
                XVariablePtr<Len> result = BinaryConstraint<Len, Len, Len>::result;

                /* The instruction for which this policy method is being invoked. */
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(result->def);
                ROSE_ASSERT(insn);
                SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();

                if (!le1.name && !le2.name) {
                    /* Operands are known constants, so the result will be a known constant. */
                    result->set(LatticeElement<Len>::constant(le1.offset & le2.offset, result->def));
                } else {
                    /* Is this instruction aligning the stack pointer? */
                    SgAsmx86RegisterReferenceExpression *op1 = (opands.size()==2 ?
                                                                isSgAsmx86RegisterReferenceExpression(opands[0]) : NULL);
                    if (op1 && insn->get_kind()==x86_and &&
                        op1->get_register_class()==x86_regclass_gpr && op1->get_register_number()==x86_gpr_sp &&
                        !le2.isTop) {
                        /* Yes, we're aligning the stack pointer. */
                        LatticeElement<Len> newval = le1; /* stack pointer */
                        uint32_t alignment = ~(uint32_t)le2.offset + 1; /*two's complement*/
                        //std::cout <<"ROBB: aligning stack on "<<std::dec <<alignment <<"-byte boundary"
                        //          <<" [" <<unparseInstructionWithAddress(insn) <<"]\n";
                        newval.offset -= alignment;
                        newval.definingInstruction = insn;
                        result->set(newval);
                    } else {
                        /* No, it is some other use of AND and one or both of the operands are unknown values */
                        result->set(LatticeElement<Len>());
                    }
                }
            }
            virtual uint64_t compute(uint64_t, uint64_t) const {
                abort(); /* handled by run() above */
            }
        };
        (new IC(result, a, b))->activate(); /*invokes the run() method above*/
        return result;
    }
};

/** Instruction semantics policy for API-compliant function calls.  The main thing this does differently from
 *  FindConstantsPolicy is that x86 CALL instructions initialize the register set and memory for the instruction at the next
 *  address after the CALL assuming that the called function will honor the ABI specification and will return to the address
 *  following the CALL. */
class FindConstantsABIPolicy: public FindConstantsPolicy {
public:
    /* Returns the function containing the instruction. */
    SgAsmFunctionDeclaration* find_function(SgAsmInstruction* insn) {
        SgNode* n=insn;
        while (n && !isSgAsmFunctionDeclaration(n))
            n = n->get_parent();
        return isSgAsmFunctionDeclaration(n);
    }

    /* Determines if the function contains an instruction at the specified address. */
    bool function_contains_address(SgAsmFunctionDeclaration* f, rose_addr_t va) {
        for (size_t i=0; i<f->get_statementList().size(); ++i) {
            SgAsmBlock* block = isSgAsmBlock(f->get_statementList()[i]);
            ROSE_ASSERT(block!=NULL);
            for (size_t j=0; j<block->get_statementList().size(); ++j) {
                SgAsmInstruction* insn = isSgAsmInstruction(block->get_statementList()[j]);
                ROSE_ASSERT(insn!=NULL);
                if (insn->get_address()==va)
                    return true;
            }
        }
        return false;
    }

    /* Returns true if the function at the specified address complies with the ABI */
    bool is_abi_compliant(rose_addr_t) {
        return true; /* left as an exercise for later ;-) */
    }

    /* Determines if a CALL instruction in fact calls a function. Malware sometimes uses CALL instructions for unconditional
     * jumps, in which case the instruction partitioner (Partitioner class) would have placed the call target in the same
     * function as the CALL instruction (note that recursive calls are to the entry address of the function). */
    bool is_abi_function_call(SgAsmInstruction* insn_) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(insn_);
        ROSE_ASSERT(insn!=NULL);
        if (insn->get_kind()!=x86_call && insn->get_kind()!=x86_farcall)
            return false;
        if (newIp->get().name!=0)
            return true; /*if we don't know the call target then assume it's a function call*/
        SgAsmFunctionDeclaration *caller = find_function(insn);
        rose_addr_t callee = newIp->get().offset;
        if (function_contains_address(caller, callee) && caller->get_entry_va()!=callee)
            return false; /*intra-function branch*/
        return is_abi_compliant(callee);
    }

    /* Augments superclass method so that the instruction following a CALL (provided this is really a function call and not
     * just an intra-function branch) has a register set whose callee-preserved registers are actually reserved across the
     * CALL instruction. We also preserve the stack pointer, frame pointer, and all memory. */
    void finishInstruction(SgAsmInstruction* insn_) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(insn_);
        ROSE_ASSERT(insn!=NULL);
        if (is_abi_function_call(insn)) {
            rose_addr_t call_va = insn->get_address();
            rose_addr_t next_va = insn->get_address() + insn->get_raw_bytes().size();
            RegisterSet rset;
            rset.setToBottom();
            rset.memoryWrites = rsets[call_va].memoryWrites;
            rset.gpr[x86_gpr_bx] = rsets[call_va].gpr[x86_gpr_bx];
            rset.gpr[x86_gpr_di] = rsets[call_va].gpr[x86_gpr_di];
            rset.gpr[x86_gpr_si] = rsets[call_va].gpr[x86_gpr_si];
            rset.gpr[x86_gpr_sp] = rsets[call_va].gpr[x86_gpr_sp];
            rset.gpr[x86_gpr_bp] = rsets[call_va].gpr[x86_gpr_bp];
            rsets[next_va].mergeIn(rset);
        }
        FindConstantsPolicy::finishInstruction(insn);
    }
};

#endif /* !findConstants_H */
