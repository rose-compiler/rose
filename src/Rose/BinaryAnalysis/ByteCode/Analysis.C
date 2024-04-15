#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ByteCode/Analysis.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>

#include <SgAsmInstruction.h>
#include <SgAsmInstructionList.h>
#include <Cxx_GrammarDowncast.h>

#include <iostream>

using namespace Rose::BinaryAnalysis::Partitioner2;
using PoolEntry = SgAsmJvmConstantPoolEntry;
using AddressSegment = Sawyer::Container::AddressSegment<rose_addr_t,uint8_t>;
using Rose::Diagnostics::DEBUG;
using Rose::Diagnostics::INFO;
using Rose::StringUtility::addrToString;

namespace Rose {
namespace BinaryAnalysis {
namespace ByteCode {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Method
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Method::Method(rose_addr_t va) : classAddr_{va} {}

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
    auto successors = insn->architecture()->getSuccessors(insn, complete /*out*/);
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

void Class::partition(const PartitionerPtr &partitioner, std::map<std::string,rose_addr_t> &discoveredFunctions) const
{
  const size_t nBits = 64;

  for (auto constMethod : methods()) {
    rose_addr_t va{0};
    FunctionPtr function{};
    BasicBlockPtr block{};
    bool needNewBlock{true};
    bool insertFallthroughSuccessors{true};

    // Allow const_cast only here: TODO: consider fixing this (adding basic blocks)?
    Method* method = const_cast<Method*>(constMethod);

    // Annotate the instructions
    method->annotate();

    auto instructions = method->instructions()->get_instructions();
    if (instructions.size() > 0) {
      // The address of the Partitioner2::Function is the address of the first basic block
      va = instructions[0]->get_address();
    } else {
      // A Java interface has no instructions, use the class address instead
      va = address();
    }

    // Determine if this method/function has been seen before (e.g., ".ctor" of parent class)
    if (partitioner->placeholderExists(va)) continue;

    // Create the (Partitioner2) function
    std::string functionName = name() + typeSeparator() + method->name();
    function = Partitioner2::Function::instance(va, functionName);

    // Add newly discovered function to the list
    if (discoveredFunctions.find(functionName) == discoveredFunctions.end()) {
      discoveredFunctions[functionName] = va;
    }
    else {
      // This occurs for a nested/inner class with a constructor having an input parameter
      // of a dynamic type (I think, for example, see T8_NestMembersAttribute.java).
      // Also dotnet (see T1_Test1.cs).
      mlog[Diagnostics::WARN] << "Class::partition(): discovered duplicate function: " << functionName << "\n";
    }

    std::set<rose_addr_t> targets = method->targets();

    for (auto astInsn : instructions) {
      // A copy of the instruction must be made if it is linked to ROSE's AST
      SgTreeCopy deep;
      SgAsmInstruction* insn = isSgAsmInstruction(astInsn->copy(deep));
      ASSERT_not_null(insn);
      ASSERT_require(insn != astInsn);
      ASSERT_require(insn->get_address() == astInsn->get_address());

      // A new block is needed if this instruction is a target of a branch and nonterminal
      va = insn->get_address();
      if (targets.find(va) != targets.end()) {
        // But a new block is not needed if this is the first instruction in the block
        if (block && !block->isEmpty() && va != block->address()) {
          mlog[DEBUG] << "Splitting block after: " << addrToString(block->instructions().back()->get_address())
                      << " va: " << addrToString(va)
                      << " fallthrough: " << addrToString(block->fallthroughVa())
                      << " kind:" << insn->get_anyKind() << " :" << insn->get_mnemonic() << "\n";

          // If the instruction doesn't have a branch target, add fall through successor
          if (!partitioner->architecture()->branchTarget(block->instructions().back())) {
            mlog[DEBUG] << "Adding successor fall-through edge from va: "
                        << addrToString(block->instructions().back()->get_address())
                        << " to: " << addrToString(block->fallthroughVa()) << "\n";
            if (insertFallthroughSuccessors) {
              block->insertSuccessor(block->fallthroughVa(), nBits, EdgeType::E_NORMAL, Confidence::PROVED);
            } else {
              insertFallthroughSuccessors = true;
            }
          }
          needNewBlock = true;
        }
      }

      // If needed, create a new block and insert it into the function
      if (needNewBlock) {
        if (block && !block->isEmpty() && va != block->address()) {
          // Attach old block only if its address differs from the new instruction's
          partitioner->attachBasicBlock(block);
        }
        block = Partitioner2::BasicBlock::instance(va, partitioner);
        function->insertBasicBlock(va);
        method->append(block);
        needNewBlock = false;
      }

      // Warning: this instruction can't be linked into ROSE's AST (parent must be null)
      block->append(partitioner, insn);

      // If this instruction terminates the block, add successors and set block properties
      if (partitioner->architecture()->terminatesBasicBlock(insn)) {
        bool complete;
        auto successors = partitioner->architecture()->getSuccessors(insn, complete/*out*/);
        for (auto successor : successors.values()) {
          mlog[DEBUG] << "Adding successor edge from va: " << addrToString(va) << " to: " << addrToString(successor) << "\n";
          block->insertSuccessor(successor, nBits, EdgeType::E_NORMAL, Confidence::PROVED);
        }

        // Set properties of the block
        SgAsmInstruction* last = block->instructions().back();
        if (partitioner->architecture()->isFunctionReturnFast(block->instructions())) {
          block->isFunctionReturn(true);
        }
        else if (partitioner->architecture()->isFunctionCallFast(block->instructions(), nullptr, nullptr)) {
          block->isFunctionCall(true);

          // Fully resoved function name is stored in the comment of the call instruction
          std::string callee = last->get_comment();

          // Insert a partitioner function for a system call when first seen
          if (discoveredFunctions.find(callee) == discoveredFunctions.end()) {
            if (method->isSystemReserved(callee)) {
              rose_addr_t reservedVa = Container::nextSystemReservedVa();
              auto reservedFunction = Partitioner2::Function::instance(reservedVa, callee);
              auto reservedBlock = Partitioner2::BasicBlock::instance(reservedVa, partitioner);
              reservedFunction->insertBasicBlock(reservedVa);
              partitioner->attachBasicBlock(reservedBlock);
              partitioner->attachFunction(reservedFunction);

              // It's been discovered
              discoveredFunctions[callee] = reservedVa;
            }
          }

          if (discoveredFunctions.find(callee) != discoveredFunctions.end()) {
            auto itr = discoveredFunctions.find(callee);
            mlog[DEBUG] << "Adding call edge from va: " << addrToString(va) << " to: " << addrToString(itr->second)
                        << " : " << callee << "\n";
            block->insertSuccessor(itr->second, nBits, EdgeType::E_FUNCTION_CALL, Confidence::PROVED);
          }
          else {
            mlog[DEBUG] << "Failed to find function, NOT adding call edge from va: " <<addrToString(va) <<" to: " <<callee <<"\n";
            // No fallthrough successor should be added to allow edge to indeterminate vertex to be created
            insertFallthroughSuccessors = false;
          }
        }
        needNewBlock = true;
      }
    }

    // If this is an interface an empty block will need to be created
    if (block == nullptr) {
        block = Partitioner2::BasicBlock::instance(va, partitioner);
        function->insertBasicBlock(va);
        method->append(block);
    }

    // Attach function return block and function to the partitioner
    partitioner->attachBasicBlock(block);
    partitioner->attachFunction(function);
  }
}

void Class::digraph() const
{
  std::ofstream dotFile;
  mlog[INFO] << "Opening dot file " + name() + ".dot\n";
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
Namespace::partition(const PartitionerPtr &partitioner, std::map<std::string,rose_addr_t> &discoveredFunctions) const {
  for (auto cls: classes()) {
    cls->partition(partitioner, discoveredFunctions);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Container
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
rose_addr_t
Container::nextSystemReservedVa_{static_cast<rose_addr_t>(-1)};

rose_addr_t Container::nextSystemReservedVa() {
  rose_addr_t va{nextSystemReservedVa_};
  nextSystemReservedVa_ -= 1024;
  return va;
}

void
Container::partition(const PartitionerPtr &partitioner) const {
  // Both Cil and Jvm need call return edges to be added by Partitioner::attachBasicBlock()
  partitioner->autoAddCallReturnEdges(true);

  // Do the partitioning and keep track of functions discovered during the process
  std::map<std::string,rose_addr_t> discoveredFunctions{};
  for (auto nmSpace: namespaces_) {
    nmSpace->partition(partitioner, discoveredFunctions);
  }

  // Attach empty functions as targets for invoke of system functions
  for (auto discovered: discoveredFunctions) {
    rose_addr_t va = discovered.second;
    std::string name = discovered.first;

    // Create and attach system functions if placeholder doesn't exist already
    if (isSystemReserved(name)) {
      if (!partitioner->placeholderExists(va)) {
        auto function = Partitioner2::Function::instance(va, name);
        auto block = Partitioner2::BasicBlock::instance(va, partitioner);
        function->insertBasicBlock(va);
        partitioner->attachBasicBlock(block);
        partitioner->attachFunction(function);
      }
    }
  }

  // partitioner->dumpCfg(std::cout, "Worker:", true, false);
}

} // namespace
} // namespace
} // namespace

#endif
