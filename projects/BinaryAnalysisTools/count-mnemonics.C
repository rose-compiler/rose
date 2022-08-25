#include <rose.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>

#include <boost/lexical_cast.hpp>

using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

static bool countUnknown = false;
static bool countErrors = false;

/*static*/ std::string
trim(const std::string &s, const std::string &suffix1, const std::string &suffix2 = "") {
    if (!suffix1.empty() && boost::ends_with(s, suffix1) && s.size() > suffix1.size())
        return s.substr(0, s.size() - suffix1.size());
    if (!suffix2.empty() && boost::ends_with(s, suffix2) && s.size() > suffix2.size())
        return s.substr(0, s.size() - suffix2.size());
    return s;
}

static std::string
baseMnemonic(SgAsmInstruction *insn_) {
    ASSERT_not_null(insn_);
    std::string s = insn_->get_mnemonic();

#ifdef ROSE_ENABLE_ASM_AARCH32
    auto insn = isSgAsmAarch32Instruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_condition()) {
        case ARM_CC_INVALID:
            return s;
        case ARM_CC_EQ:
            return trim(s, "eq");
	case ARM_CC_NE:
            return trim(s, "ne");
	case ARM_CC_HS:
            return trim(s, "hs", "cs");
	case ARM_CC_LO:
            return trim(s, "lo", "cc");
	case ARM_CC_MI:
            return trim(s, "mi");
        case ARM_CC_PL:
            return trim(s, "pl");
        case ARM_CC_VS:
            return trim(s, "vs");
	case ARM_CC_VC:
            return trim(s, "vc");
	case ARM_CC_HI:
            return trim(s, "hi");
	case ARM_CC_LS:
            return trim(s, "ls");
	case ARM_CC_GE:
            return trim(s, "ge");
	case ARM_CC_LT:
            return trim(s, "lt");
	case ARM_CC_GT:
            return trim(s, "gt");
	case ARM_CC_LE:
            return trim(s, "le");
	case ARM_CC_AL:
            return s;
    }
    ASSERT_not_reachable("unhandled insn condition: " + boost::lexical_cast<std::string>(insn->get_condition()));
#else
    return s;
#endif
}

int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;

    ASSERT_always_require(argc >= 2);
    std::string isa = argv[1];
    std::map<std::string, size_t> histogram;
    using HE = std::pair<std::string, size_t>;          // no lambda auto before C++14
    size_t nErrors = 0;

    for (int i = 2; i < argc; ++i) {
        P2::Engine engine;
        engine.settings().disassembler.isaName = isa;
        MemoryMap::Ptr map = engine.loadSpecimens(argv[i]);
        if (map->isEmpty())
            continue;

        Disassembler::Base *disassembler = engine.obtainDisassembler();
        rose_addr_t va = map->hull().least();
        while (map->atOrAfter(va).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
            va = alignUp(va, disassembler->instructionAlignment());
            try {
                SgAsmInstruction *insn = disassembler->disassembleOne(map, va);
                ASSERT_not_null(insn);
                if (countUnknown || insn->get_mnemonic() != "unknown")
                    ++histogram[baseMnemonic(insn)];
                va += insn->get_size();
#if 0 // [Robb Matzke 2021-01-26]: broken
                deleteAST(insn);
#endif
            } catch (const Disassembler::Exception &e) {
                if (countErrors)
                    ++nErrors;
                ++va;
            }
        }
    }

    // Sort the histogram by descending number of occurrances.
    std::vector<std::pair<std::string, size_t>> sorted(histogram.begin(), histogram.end());
    std::sort(sorted.begin(), sorted.end(), [](const HE &a, const HE &b) {
            return a.second > b.second;
        });
    size_t total = 0;
    for (const auto &pair: sorted)
        total += pair.second;

    // Show the results
    size_t runningTotal = 0;
    for (const auto &pair: sorted) {
        runningTotal += pair.second;
        std::cout <<(boost::format("| %-32s | %d | %7.3f%% | %7.3f%% |\n")
                     %pair.first
                     %pair.second
                     %(100.0 * pair.second / total)
                     %(100.0 * runningTotal / total));
    }
    if (nErrors > 0) {
        runningTotal += nErrors;
        std::cout <<(boost::format("| %-32s | %d | %7.3f%% | %7.3f%% |\n")
                     %"Errors"
                     %nErrors
                     %(100.0 * nErrors / total)
                     %(100.0 * runningTotal / total));
        total += nErrors;
    }
    std::cout <<(boost::format("| %-32s | %d | %7.3f%% | %7.3f%% |\n")
                 %"Total"
                 %total
                 %100.0
                 %100.0);
}
