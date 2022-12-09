#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <iostream>

#include <Rose/BinaryAnalysis/ByteCode/Analysis.h>

#define DEBUG_PRINT 0

using namespace Rose::BinaryAnalysis::Partitioner2;
using PoolEntry = SgAsmJvmConstantPoolEntry;
using AddressSegment = Sawyer::Container::AddressSegment<rose_addr_t,uint8_t>;
using std::cout;
using std::endl;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

std::set<rose_addr_t> Method::targets() const {
  std::set<rose_addr_t> retval{};
  for (auto insn : instructions()->get_instructions()) {
    bool complete;
    auto successors = insn->getSuccessors(complete);
      for (auto successor : successors.values()) {
#if DEBUG_PRINT
        cout << "... Method::targets():adding successor target va:" << successor << endl;
#endif
        retval.insert(successor);
      }
  }
  return retval;
}


void Class::partition()
{
  const size_t nBits = 64;

  auto partitioner = Partitioner2::Partitioner::instance();

  for (auto constMethod : methods()) {
    bool needNewBlock{true};
    BasicBlockPtr block;

    // Allow const_cast only here: TODO: consider fixing this (adding basic blocks)?
    Method* method = const_cast<Method*>(constMethod);
    std::set<rose_addr_t> targets = method->targets();

    auto instructions = method->instructions()->get_instructions();
    if (instructions.size() > 0) {
      auto va = instructions[0]->get_address();
      auto function = Partitioner2::Function::instance(va, method->name());

      for (auto insn : instructions) {
        va = insn->get_address();

        // A new block is needed if this instruction is a target of a branch and nonterminal
        if (targets.find(va) != targets.end() && !insn->terminatesBasicBlock()) {
          // But a new block is not needed if this is the first instruction in the block
          if (!block->isEmpty() && va != block->address()) {
#if DEBUG_PRINT
            cout << "... splitting block after:" << block->instructions().back()->get_address()
                 << " va:" << va
                 << " fallthrough:" << block->fallthroughVa()
                 << " kind:" << insn->get_anyKind()
                 << " :" << insn->description()
                 << endl;
#endif
            // If the instruction doesn't have a branch target, add fall through successor
            if (!block->instructions().back()->branchTarget()) {
#if DEBUG_PRINT
              cout << "... adding successor fall-through edge from va:"
                   << block->instructions().back()->get_address() << " to:" << block->fallthroughVa() << endl;
#endif
              block->insertSuccessor(block->fallthroughVa(), nBits, EdgeType::E_NORMAL, Confidence::PROVED);
            }
            needNewBlock = true;
          }
        }

        if (needNewBlock) {
          block = Partitioner2::BasicBlock::instance(va, partitioner);
          function->insertBasicBlock(va);
          method->append(block);
          needNewBlock = false;
        }
        block->append(partitioner, insn);

        // Add successors if this instruction terminates the block
        if (insn->terminatesBasicBlock() && insn != instructions.back()) {
          bool complete;
          auto successors = insn->getSuccessors(complete); // complete is a return value
          for (auto successor : successors.values()) {
#if DEBUG_PRINT
            cout << "... adding successor edge from va:" << va << " to:" << successor << endl;
#endif
            block->insertSuccessor(successor, nBits, EdgeType::E_NORMAL, Confidence::PROVED);
          }
          // Set properties of the block
          SgAsmInstruction* last = block->instructions().back();
          if (last->isFunctionReturnFast(block->instructions())) {
            block->isFunctionReturn(true);
          }
          else if (last->isFunctionCallFast(block->instructions(), nullptr, nullptr)) {
            block->isFunctionCall(true);
          }
          needNewBlock = true;
        }
      }
    }
  }
}

void Class::digraph()
{
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
  for (size_t midx = 0; midx < methods().size(); midx++) {
    auto method = methods()[midx];
    for (size_t bidx = 0; bidx < method->blocks().size(); bidx++) {
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

  for (size_t midx = 0; midx < methods().size(); midx++) {
    auto method = methods()[midx];

    std::map<rose_addr_t,int> vaToBlock{};
    for (size_t bidx = 0; bidx < method->blocks().size(); bidx++) {
      auto block = method->blocks()[bidx];
      for (auto insn : block->instructions()) {
        vaToBlock.emplace(insn->get_address(),bidx);
      }
    }

    // Edge from method to first block
    auto blockHead = method->blocks()[0];
    dotFile << "  \"" << method->name() << "\" -> "
            << "block_" << midx << "_0:" << blockHead->instructions()[0]->get_address() << endl;

    for (size_t bidx = 0; bidx < method->blocks().size(); bidx++) {
      auto block = method->blocks()[bidx];
      auto tail = block->instructions().back();
      // Successors edges from the block
      if (block->successors().isCached()) {
        for (auto successor : block->successors().get()) {
          if (auto targetVa = successor.expr()->toUnsigned()) {
            rose_addr_t va = targetVa.get();
            dotFile << "  block_" << midx << "_" << bidx << ":" << tail->get_address()
                    << " -> block_" << midx << "_" << vaToBlock[va] << ":" << va << endl;
          }
        }
      }
    }
  }
  dotFile << "}\n";
  dotFile.close();
}

} // namespace
} // namespace
} // namespace

#endif
