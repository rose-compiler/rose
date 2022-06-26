#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <iostream>

#include <Rose/BinaryAnalysis/ByteCode/ByteCode.h>

#define DEBUG_ON 0

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
  const size_t nBits = 64;

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
#if DEBUG_ON
          std::cout << "--> terminates (post call) with " << insn->description() << std::endl;
#endif
          if (insn != instructions.back()) {
            auto target = insn->branchTarget();
            if (insn->branchTarget()) {
#if DEBUG_ON
              cout << "... adding successor target edge from va:" << va << " to:" << insn->branchTarget().get() << endl;
#endif
              block->insertSuccessor(insn->branchTarget().get(), nBits, EdgeType::E_NORMAL, Confidence::PROVED);
            }
            else {
#if DEBUG_ON
              cout << "... adding successor block edge from va:" << va << " to:" << block->fallthroughVa() << endl;
#endif
              block->insertSuccessor(block->fallthroughVa(), nBits, EdgeType::E_NORMAL, Confidence::PROVED);
            }
            needNewBlock = true;
          }
          // Set properties of the block
          SgAsmInstruction* last = block->instructions().back();
          if (last->isFunctionReturnFast(block->instructions())) {
            block->isFunctionReturn(true);
          }
          else if (last->isFunctionCallFast(block->instructions(), nullptr, nullptr)) {
            block->isFunctionCall(true);
          }
        }
      }
    }
  }
}

void Class::digraph()
{
  int mindex{0};

  std::ofstream dotFile;
  dotFile.open(name() + ".dot");
  dotFile << "digraph g {" << endl;

  dotFile << "\n";
  dotFile << "  fontname=\"Helvetica,Arial,sans-serif\"\n";
  dotFile << "  node [fontname=\"Helvetica,Arial,sans-serif\"]\n";
  dotFile << "  edge [fontname=\"Helvetica,Arial,sans-serif\"]\n";
  dotFile << "  node [fontsize = \"16\" shape = \"ellipse\"]\n";
  dotFile << "  graph [rankdir = \"LR\"]\n\n";

  // Edge from class to each method
  for (auto method : methods()) {
    dotFile << "  " << name() << " -> \"" << method->name() << "\";\n";
  }
  dotFile << endl;

  // Blocks for each method
  for (int midx = 0; midx < methods().size(); midx++) {
    auto method = methods()[midx];
    for (int bidx = 0; bidx < method->blocks().size(); bidx++) {
      auto block = method->blocks()[bidx];
      dotFile << "  block_" << midx << "_" << bidx << " [\n    label=\"";
      for (auto insn : block->instructions()) {
        dotFile << "<" << insn->get_address() << ">" << insn->get_mnemonic() << ":" << insn->get_address();
        if (insn != block->instructions().back()) dotFile << "|";
      }
      dotFile << "\"\n    shape = \"record\"\n  ];" << endl;
    }
  }
  dotFile << endl;

  for (int midx = 0; midx < methods().size(); midx++) {
    auto method = methods()[midx];

    std::map<rose_addr_t,int> vaToBlock{};
    for (int bidx = 0; bidx < method->blocks().size(); bidx++) {
      auto block = method->blocks()[bidx];
      for (auto insn : block->instructions()) {
        vaToBlock.emplace(insn->get_address(),bidx);
      }
    }

    // Edge from method to first block
    auto blockHead = method->blocks()[0];
    dotFile << "  \"" << method->name() << "\" -> "
            << "block_" << midx << "_0:" << blockHead->instructions()[0]->get_address() << endl;

    for (int bidx = 0; bidx < method->blocks().size(); bidx++) {
      auto block = method->blocks()[bidx];
      auto tail = block->instructions().back();
      bool complete;

#if 0
      // Successors from block
      if (block->successors().isCached()) {
        for (auto successor : block->successors().get()) {
          if (auto targetVa = successor.expr()->toUnsigned()) {
            // TODO: Note the cast to uint32_t, why not set nBits to 64?
            rose_addr_t va = (uint32_t) targetVa.get();
            dotFile << "  block_" << midx << "_" << bidx << ":" << tail->get_address()
                    << " -> block_" << midx << "_" << vaToBlock[va] << ":" << va << endl;
          }
        }
      }
#else
      // Successor edges
      auto successors = tail->getSuccessors(complete); // complete is a return value
      for (auto successor : successors.values()) {
        dotFile << "  block_" << midx << "_" << bidx << ":" << tail->get_address()
                << " -> block_" << midx << "_" << vaToBlock[successor] << ":" << successor << endl;
      }

#if 0
      // Fall-through successor edge
      if (!complete && (bidx+1 < method->blocks().size())) {
        auto nextVa = method->blocks()[bidx+1]->address();
        dotFile << "  block_" << midx << "_" << bidx << ":" << tail->get_address()
                << " -> block_" << midx << "_" << bidx+1 << ":" << nextVa << endl;
      }
#endif
#endif
    }
  }
  dotFile << "}\n";
  dotFile.close();
}

} // namespace
} // namespace
} // namespace

#endif
