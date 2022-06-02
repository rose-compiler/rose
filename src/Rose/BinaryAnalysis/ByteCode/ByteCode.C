#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/ByteCode/ByteCode.h>

using namespace Rose::BinaryAnalysis::Partitioner2;
using PoolEntry = SgAsmJvmConstantPoolEntry;
using AddressSegment = Sawyer::Container::AddressSegment<rose_addr_t,uint8_t>;
using std::cout;
using std::endl;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

void Class::partition()
{
  /* QUESTION for Robb ??? */
  const int nBits = 32;

  Partitioner2::Partitioner partitioner{};

  for (auto constMethod : methods()) {
    bool needNewBlock{true};
    BasicBlockPtr block;

    // Allow const_cast only here: TODO: consider fixing this (adding basic blocks)?
    Method* method = const_cast<Method*>(constMethod);

    auto instructions = method->instructions()->get_instructions();
    if (instructions.size() > 0) {
      auto va = instructions[0]->get_address();
      auto function = Partitioner2::Function::instance(va, method->name());

      for (auto insn : instructions) {
        va = insn->get_address();
        if (needNewBlock) {
          block = Partitioner2::BasicBlock::instance(va, partitioner);
          function->insertBasicBlock(va);
          method->append(block);
          needNewBlock = false;
        }
        block->append(partitioner, insn);

        if (insn->terminatesBasicBlock()) {
          cout << "... adding successor block\n";
          if (constMethod != methods().back()) {
            block->insertSuccessor(block->fallthroughVa(), nBits, EdgeType::E_NORMAL, Confidence::PROVED);
          }
          needNewBlock = true;
        }
      }
    }
  }
}

} // namespace
} // namespace
} // namespace

#endif
