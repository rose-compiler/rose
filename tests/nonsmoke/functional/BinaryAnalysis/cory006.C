// Demonstrates how to disassemble things that are between functions, such as the three JMP instructions in this code assuming
// that those JMPs are not otherwise part of the CFG.
// 
//   push ebp
//   mov ebp, esp
//   nop
//   leave
//   ret
//   jmp f1
//   jmp f1
//   jmp f1
//   push ebp
//   mov ebp, esp
//   nop
//   leave
//   ret

#include <rose.h>
#include <AsmUnparser.h>
#include <BinaryControlFlow.h>
#include <Partitioner2/Engine.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
namespace P2 = rose::BinaryAnalysis::Partitioner2;

static bool
isGoodBasicBlock(const P2::BasicBlock::Ptr &bb) {
    return bb != NULL;                                  // could be more choosy here ;-)
}

int
main(int argc, char *argv[]) {

    // This paragraph initializes the ROSE library, generates the man page for this tool, does command-line parsing for quite a
    // few switches including "--help", loads various specimen resources (ELF/PE, running process, raw memory dumps, etc),
    // disassembles, and partitions.  We could have called Engine::frontend() and done it all in one function call, but then we
    // wouldn't have a Partitioner2::Partitioner object that we need below.
    std::string purpose = "demonstrate inter-function disassembly";
    std::string description =
        "Disassembles and partitions the specimen(s), then tries to disassemble things between the functions.";
    P2::Engine engine;
    std::vector<std::string> specimens = engine.parseCommandLine(argc, argv, purpose, description).unreachedArgs();
    P2::Partitioner partitioner = engine.partition(specimens);

    // The partitioner's address usage map (AUM) describes what part of memory has been disassembled as instructions or
    // data. We're interested in the unused parts between the lowest and highest disassembled addresses, so we loop over those
    // parts.  The hull() is the entire used interval -- lowest to highest addresses used regardless of the unused areas in the
    // middle.  An AddressInterval evaluated in boolean context returns false if it's empty.
    rose_addr_t va = partitioner.aum().hull().least();
    while (AddressInterval unused = partitioner.aum().nextUnused(va)) {

        // Is the unused area beyond the last thing compiled?  We're only interested in the stuff between functions.  This
        // check also means that unused.greatest()+1 will not overflow, which simplifies later code. Overflows are easy to
        // trigger when the specimen's word size is the same as ROSE's word size.
        if (unused.least() > partitioner.aum().hull().greatest())
            break;

        // The unused address might be in the middle of some very large unmapped area of memory, or perhaps in an area that
        // doesn't have execute permission (the partitioner will only disassemble at addresses that we've marked as
        // executable). A naive implementation would just increment to the next address and try again, but that could take a
        // very long time.  This "if" statement will give us the next executable address that falls within the unused interval
        // if possible. The address is assigned to "va" if possible.
        if (!engine.memoryMap().within(unused).require(MemoryMap::EXECUTABLE).next().assignTo(va)) {
            va = unused.greatest() + 1;                 // won't overflow because of check above
            continue;
        }

        // "va" now points to an executable address that the partitioner doesn't know about yet.
        ASSERT_require(engine.memoryMap().at(va).require(MemoryMap::EXECUTABLE).exists());
        ASSERT_forbid(partitioner.aum().instructionExists(va));
        std::cout <<"unused address " <<StringUtility::addrToString(va) <<"\n";

        // Cause the partitioner to discover (disassemble) one basic block. This doesn't add the basic block to the
        // partitioner or change the partitioner in any way.  If the BB isn't something we want to keep then just forget about
        // it and garbage collection will reclaim the memory.
        P2::BasicBlock::Ptr bb = partitioner.discoverBasicBlock(va);
        if (!isGoodBasicBlock(bb)) {
            ++va;
            continue;
        }
        std::cout <<"  disassembled " <<bb->printableName() <<"\n";

        // Inform the partitioner that we wish to keep this BB.
        partitioner.attachBasicBlock(bb);

        // This BB was not reachable by any previous CFG edge, therefore it doesn't belong to any function. In order for it to
        // show up in the eventual AST we need to add it to some function (the ROSE AST has a requirement that every basic
        // block belongs to a function, although the partitioner can easily cope with the other case). The easiest way in this
        // situation is to just create a new function whose entry block is this BB.  Creating a function doesn't modify the
        // partitioner in any way, so we need to also attach the function to the partitioner.
        P2::Function::Ptr function = P2::Function::instance(va, SgAsmFunction::FUNC_USERDEF);
        function->insertBasicBlock(va);                 // allowed only before attaching function to partitioner
        partitioner.attachOrMergeFunction(function);

        // This basic block might be the first block of a whole bunch that are connected by as yet undiscovered CFG edges. We
        // can recursively discover and attach all those blocks with one Engine method.  There are also Partitioner methods to
        // do similar things, but they're lower level.
        engine.runPartitionerRecursive(partitioner);
    }

    // We've probably added a bunch more functions and basic blocks to the partitioner, but we haven't yet assigned the basic
    // blocks discovered by Engine::runPartitionerRecursive to any functions.  We might also need to assign function labels
    // from ELF/PE information, re-run some analysis, etc., so do that now.
    engine.runPartitionerFinal(partitioner);

    // Most ROSE analysis is performed on an abstract syntax tree, so generate one.  If the specime is an ELF or PE container
    // then the returned global block will also be attached somewhere below a SgProject node, otherwise the returned global
    // block is the root of the AST and there is no project (e.g., like when the specimen is a raw memory dump).
    SgAsmBlock *gblock = P2::Modules::buildAst(partitioner, engine.interpretation());

    // Generate an assembly listing. These unparser properties are all optional, but they result in more informative assembly
    // listings.
    AsmUnparser unparser;
    unparser.set_registers(partitioner.instructionProvider().registerDictionary());
    unparser.add_control_flow_graph(ControlFlow().build_block_cfg_from_ast<ControlFlow::BlockGraph>(gblock));
    unparser.staticDataDisassembler.init(engine.disassembler());
    unparser.unparse(std::cout, gblock);
}
