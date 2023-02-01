#include "sage3basic.h"

#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <boost/lexical_cast.hpp>

using namespace Rose::BinaryAnalysis;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace SS = Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;

static void
test01(const BS::RiscOperators::Ptr &ops) {
    std::cout <<"\n"
              <<"================================================================\n"
              <<"Merge a register existing in state 1 into state 2 that does not\n"
              <<"have that register. This should result in a simple copy of the\n"
              <<"register value from state 1 to state 2\n"
              <<"\n";

    const RegisterDescriptor A(0, 0, 2, 1);

    // Source state1
    auto state1 = ops->currentState();
    state1->clear();
    auto a = ops->number_(A.nBits(), 1);
    state1->writeRegister(A, a, ops.get());

    // Source state2 (and eventual destination)
    auto state2 = state1->clone();
    state2->clear();

    // Merge state 1 into state 2
    std::cout <<"initial state1:\n" <<(*state1 + "  ")
              <<"initial state2:\n" <<(*state2 + "  ");
    bool changed = state2->merge(state1, ops.get());
    std::cout <<"merged state1 into state2 to obtain new state2:\n" <<(*state2 + "  ");

    // Test
    ASSERT_always_require(changed);
    auto a1 = state1->peekRegister(A, ops->undefined_(A.nBits()), ops.get());
    auto a2 = state2->peekRegister(A, ops->undefined_(A.nBits()), ops.get());
    ASSERT_always_require2(a1->must_equal(a2),
                           "  a1=" + boost::lexical_cast<std::string>(*a1) +
                           "\n    a2=" + boost::lexical_cast<std::string>(*a2));
}

static void
test02(const BS::RiscOperators::Ptr &ops) {
    std::cout <<"\n"
              <<"================================================================\n"
              <<"Merge register A in state 1 into state 2 that already contains\n"
              <<"register B that overlaps A. The low- and high-order bits of B\n"
              <<"should be unchanged, while the overlapping part is merged.\n"
              <<"\n";

    const RegisterDescriptor A(0, 0, 1, 1);             // small register in middle of big register
    const RegisterDescriptor B(0, 0, 0, 3);             // big enclosing register
    const RegisterDescriptor Blo(0, 0, 0, 1);           // low-order bits of B not in A
    const RegisterDescriptor Bhi(0, 0, 2, 1);           // high-order bits of B not in A

    // Source state1
    auto state1 = ops->currentState();
    state1->clear();
    auto a = ops->number_(A.nBits(), 1);
    state1->writeRegister(A, a, ops.get());

    // Source state2 (and eventual destination state)
    auto state2 = state1->clone();
    state2->clear();
    auto b = ops->number_(B.nBits(), 2);
    state2->writeRegister(B, b, ops.get());

    // Merge state 1 into state 2
    std::cout <<"initial state1:\n" <<(*state1 + "  ")
              <<"initial state2:\n" <<(*state2 + "  ");
    bool changed = state2->merge(state1, ops.get());
    std::cout <<"merged state1 into state2 to obtain new state2:\n" <<(*state2 + "  ");

    // Test
    ASSERT_always_require(!changed);
    auto lo = state2->peekRegister(Blo, ops->undefined_(Blo.nBits()), ops.get());
    auto hi = state2->peekRegister(Bhi, ops->undefined_(Bhi.nBits()), ops.get());
    auto mid1 = state1->peekRegister(A, ops->undefined_(A.nBits()), ops.get());
    auto mid2 = state2->peekRegister(A, ops->undefined_(A.nBits()), ops.get());
    ASSERT_always_require(lo->must_equal(ops->extract(b, 0, 1)));
    ASSERT_always_require(hi->must_equal(ops->extract(b, 2, 3)));
    ASSERT_always_require(mid1->must_equal(mid2));
}


static void
test03(const BS::RiscOperators::Ptr &ops) {
    std::cout <<"\n"
              <<"================================================================\n"
              <<"Merge register B in state 1 into state 2 that already contains\n"
              <<"register A that is a subset of register B. The low- and high-\n"
              <<"order bits of B in state 2 after the merge are just copied from\n"
              <<"state 2, while the overlapping part is merged with state 1.\n"
              <<"\n";

    const RegisterDescriptor A(0, 0, 1, 1);             // small register in middle of big register
    const RegisterDescriptor B(0, 0, 0, 3);             // big enclosing register
    const RegisterDescriptor Blo(0, 0, 0, 1);           // low-order bits of B not in A
    const RegisterDescriptor Bhi(0, 0, 2, 1);           // high-order bits of B not in A

    // Source state1
    auto state1 = ops->currentState();
    state1->clear();
    auto b = ops->number_(B.nBits(), 7);
    state1->writeRegister(B, b, ops.get());

    // Source state2 (and eventual destination state)
    auto state2 = state1->clone();
    state2->clear();
    auto a = ops->number_(A.nBits(), 1);
    state2->writeRegister(A, a, ops.get());

    // Merge state 1 into state 2
    std::cout <<"initial state1:\n" <<(*state1 + "  ")
              <<"initial state2:\n" <<(*state2 + "  ");
    bool changed = state2->merge(state1, ops.get());
    std::cout <<"merged state1 into state2 to obtain new state2:\n" <<(*state2 + "  ");

    // Test
    ASSERT_always_require(changed);
    auto lo = state2->peekRegister(Blo, ops->undefined_(Blo.nBits()), ops.get());
    auto hi = state2->peekRegister(Bhi, ops->undefined_(Bhi.nBits()), ops.get());
    auto mid1 = state1->peekRegister(A, ops->undefined_(A.nBits()), ops.get());
    auto mid2 = state2->peekRegister(A, ops->undefined_(A.nBits()), ops.get());
    ASSERT_always_require(lo->must_equal(ops->extract(b, 0, 1)));
    ASSERT_always_require(hi->must_equal(ops->extract(b, 2, 3)));
    ASSERT_always_require(mid1->must_equal(mid2));
}

int
main() {
    ROSE_INITIALIZE;

    auto regdict = RegisterDictionary::instanceAmd64();
    auto protoval = SS::SValue::instance();
    auto registers = SS::RegisterState::instance(protoval, regdict);
    auto memory = SS::MemoryState::instance(protoval, protoval);
    auto state = SS::State::instance(registers, memory);
    auto ops = SS::RiscOperators::instanceFromState(state);

    test01(ops);
    test02(ops);
    test03(ops);
}

#endif
