#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ByteCode/Analysis.h>

#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

#include <iostream>

using namespace Rose::BinaryAnalysis::Partitioner2;
using PoolEntry = SgAsmJvmConstantPoolEntry;
using AddressSegment = Sawyer::Container::AddressSegment<rose_addr_t,uint8_t>;
using Rose::Diagnostics::DEBUG;
using Rose::StringUtility::addrToString;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Method
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Method::Method() {}

Method::~Method() {}

const std::vector<BasicBlock::Ptr>&
Method::blocks() const {
    return blocks_;
}

void
Method::append(BasicBlock::Ptr bb) {
    blocks_.push_back(bb);
}

std::set<rose_addr_t>
Method::targets() const {
  std::set<rose_addr_t> retval{};
  for (auto insn : instructions()->get_instructions()) {
    bool complete = true;
    auto successors = insn->getSuccessors(complete /*out*/);
    for (auto successor : successors.values()) {
      mlog[DEBUG] << "... Method::targets():adding successor target va: " << addrToString(successor)<< "\n";
      retval.insert(successor);
    }
  }
  return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Class::partition(const PartitionerPtr &partitioner) const
{
  const size_t nBits = 64;

  for (auto constMethod : methods()) {
    bool needNewBlock{true};
    FunctionPtr function{};
    BasicBlockPtr block{};

    // Allow const_cast only here: TODO: consider fixing this (adding basic blocks)?
    Method* method = const_cast<Method*>(constMethod);

    // Annotate the instructions
    method->annotate();

    // Determine if this method has been seen before (e.g., ".ctor" of parent class)
    auto instructions = method->instructions()->get_instructions();
    if (instructions.size() < 1) continue;
    if (partitioner->placeholderExists(instructions[0]->get_address())) continue;

    std::set<rose_addr_t> targets = method->targets();

    if (instructions.size() > 0) {
      auto va = instructions[0]->get_address();
      function = Partitioner2::Function::instance(va, method->name());

      for (auto astInsn : instructions) {
        // A copy of the instruction must be made if it is linked to ROSE's AST
        SgTreeCopy deep;
        SgAsmInstruction* insn = isSgAsmInstruction(astInsn->copy(deep));
        ASSERT_not_null(insn);
        ASSERT_require(insn != astInsn);
        ASSERT_require(insn->get_address() == astInsn->get_address());

        // A new block is needed if this instruction is a target of a branch and nonterminal
        va = insn->get_address();
        if (targets.find(va) != targets.end() && !insn->terminatesBasicBlock()) {
          // But a new block is not needed if this is the first instruction in the block
          if (block && !block->isEmpty() && va != block->address()) {
            mlog[DEBUG] << "... splitting block after: " << addrToString(block->instructions().back()->get_address())
                        << " va: " << addrToString(va)
                        << " fallthrough: " << addrToString(block->fallthroughVa())
                        << " kind:" << insn->get_anyKind() << " :" << insn->get_mnemonic() << "\n";

            // If the instruction doesn't have a branch target, add fall through successor
            if (!block->instructions().back()->branchTarget()) {
              mlog[DEBUG] << "... adding successor fall-through edge from va: "
                          << addrToString(block->instructions().back()->get_address())
                          << " to: " << addrToString(block->fallthroughVa()) << "\n";
              block->insertSuccessor(block->fallthroughVa(), nBits, EdgeType::E_NORMAL, Confidence::PROVED);
            }
            needNewBlock = true;
          }
        }

        if (needNewBlock) {
          needNewBlock = false;
          if (block && !block->isEmpty() && va != block->address()) {
            // Attach the block only if the old block's address differs from the instruction's
            partitioner->attachBasicBlock(block);
          }
          block = Partitioner2::BasicBlock::instance(va, partitioner);
          function->insertBasicBlock(va);
          method->append(block);
        }

        // Warning: this instruction can't be linked into ROSE's AST (parent must be null)
        block->append(partitioner, insn);

        // Add successors if this instruction terminates the block
        if (insn->terminatesBasicBlock() && insn != instructions.back()) {
          bool complete;
          auto successors = insn->getSuccessors(complete/*out*/);
          for (auto successor : successors.values()) {
            mlog[DEBUG] << "... adding successor edge from va: " << addrToString(va) << " to: " << addrToString(successor) << "\n";
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

    // Attach function return block
    SgAsmInstruction* last = block->instructions().back();
    ASSERT_not_null(last);
    if (last->terminatesBasicBlock() && last->isFunctionReturnFast(block->instructions())) {
      block->isFunctionReturn(true);
    }

    // Finally add terminating block and function to the CFG
    partitioner->attachBasicBlock(block);
    partitioner->attachFunction(function);
  }
  // partitioner->dumpCfg(std::cout, "Worker:", true, false);
}

void Class::digraph()
{
  std::ofstream dotFile;
  dotFile.open(name() + ".dot");
  dotFile << "digraph g {" << "\n";

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
  dotFile << "\n";

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
      dotFile << "\"\n    shape = \"record\"\n  ];" << "\n";
    }
  }
  dotFile << "\n";

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
    if (method->blocks().size() > 0) {
      auto blockHead = method->blocks()[0];
      dotFile << "  \"" << method->name() << "\" -> "
              << "block_" << midx << "_0:" << blockHead->instructions()[0]->get_address() << "\n";
    }

    for (size_t bidx = 0; bidx < method->blocks().size(); bidx++) {
      auto block = method->blocks()[bidx];
      auto tail = block->instructions().back();
      // Successors edges from the block
      if (block->successors().isCached()) {
        for (auto successor : block->successors().get()) {
          if (auto targetVa = successor.expr()->toUnsigned()) {
            rose_addr_t va = targetVa.get();
            dotFile << "  block_" << midx << "_" << bidx << ":" << tail->get_address()
                    << " -> block_" << midx << "_" << vaToBlock[va] << ":" << va << "\n";
          }
        }
      }
    }
  }
  dotFile << "}\n";
  dotFile.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
Namespace::partition(const PartitionerPtr &partitioner) const {
    for (auto cls: classes()) {
        cls->partition(partitioner);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Container
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void
Container::partition(const PartitionerPtr &partitioner) const {
    for (auto nmSpace: namespaces()) {
        nmSpace->partition(partitioner);
    }
}

} // namespace
} // namespace
} // namespace

#endif
