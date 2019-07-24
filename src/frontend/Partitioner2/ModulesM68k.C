#include "sage3basic.h"
#include <Partitioner2/ModulesM68k.h>
#include <Partitioner2/Utility.h>
#include <SageBuilderAsm.h>
#include <boost/foreach.hpp>
#include <set>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesM68k {

using namespace Rose::Diagnostics;

bool
MatchLink::match(const Partitioner &partitioner, rose_addr_t anchor) {
    if (anchor & 1)
        return false;                               // m68k instructions must be 16-bit aligned
    static const RegisterDescriptor REG_A6(m68k_regclass_addr, 6, 0, 32);
    if (SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(partitioner.discoverInstruction(anchor))) {
        const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
        SgAsmDirectRegisterExpression *rre = NULL;
        SgAsmIntegerValueExpression *offset = NULL;
        if (insn->get_kind()==m68k_link && args.size()==2 &&
            (rre=isSgAsmDirectRegisterExpression(args[0])) && rre->get_descriptor()==REG_A6 &&
            (offset=isSgAsmIntegerValueExpression(args[1])) && offset->get_signedValue()<=0) {
            function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
            function_->reasonComment("matched LINK A6, <non_positive_integer>");
            return true;
        }
    }
    return false;
}

// Find padding that appears before the entry address of a function that aligns the entry address on a 4-byte boundary.
// For m68k, padding is either 2-byte TRAPF instructions (0x51 0xfc) or zero bytes.  Patterns we've seen are 51 fc, 51 fc 00
// 51 fc, 00 00, 51 fc 51 fc, but we'll allow any combination.
rose_addr_t
MatchFunctionPadding::match(const Partitioner &partitioner, rose_addr_t anchor) {
    MemoryMap::Ptr m = partitioner.memoryMap();
    if (0==anchor)
        return anchor;

    // Read backward from the anchor, skipping over padding as we go
    rose_addr_t padMin = anchor;
    uint8_t buf[2];                                     // reading two bytes at a time
    while (AddressInterval accessed = m->at(padMin-1).limit(2).require(MemoryMap::EXECUTABLE)
           .read(buf, Sawyer::Container::MATCH_BACKWARD)) {

        // Match zero byte or (0x51 0xfc) pair
        AddressInterval matched;
        if (2==accessed.size() && (0x51==buf[0] && 0xfc==buf[1])) {
            matched = accessed;
        } else if (2==accessed.size() && 0==buf[1]) {
            matched = AddressInterval::baseSize(accessed.least()+1, 1);
        } else if (1==accessed.size() && 0==buf[0]) {
            matched = accessed;
        } else {
            break;
        }

        // Make sure that what we matched is not already part of some other instruction
        if (!partitioner.instructionsOverlapping(matched).empty())
            break;

        // This match appears to be valid padding
        padMin = matched.least();
        if (0==padMin)
            break;                                      // avoid overflow
    }
    
    return padMin;
}

// A "switch" statement looks like this:
//   0x1000e062: 30 7b 1a 06             |0{..    |   movea.w [pc+0x00000006+d1*0x00000002], a0
//   0x1000e066: 4e fb 88 02             |N...    |   jmp    pc+0x00000002+a0*0x00000001
// The offset table appears immediately after the JMP (thus pc+6 in the MOVEA) and the code for the various cases starts
// immediately after the offset table.
bool
SwitchSuccessors::operator()(bool chain, const Args &args) {
    ASSERT_not_null(args.bblock);
    if (!chain)
        return false;
    size_t nInsns = args.bblock->nInstructions();
    if (nInsns < 2)
        return chain;
    SgAsmM68kInstruction *movea = isSgAsmM68kInstruction(args.bblock->instructions()[nInsns-2]);
    SgAsmM68kInstruction *jmp = isSgAsmM68kInstruction(args.bblock->instructions().back());
    if (!movea || movea->get_kind()!=m68k_movea || !jmp || jmp->get_kind()!=m68k_jmp)
        return chain;
    const SgAsmExpressionPtrList &movea_args = movea->get_operandList()->get_operands();
    const SgAsmExpressionPtrList &jmp_args = jmp->get_operandList()->get_operands();
    if (movea_args.size()!=2 || jmp_args.size()!=1)
        return chain;
    const RegisterDescriptor REG_PC = args.partitioner.instructionProvider().instructionPointerRegister();

    // MOVEA first argument is: (+ (+ PC 6) (* Dx 2))
    //   prog variables: sum1 ---^  ^ ^^ ^   ^ ^^ ^---- scale1
    //                   sum2 ------' || |   | ''------ reg2
    //                   reg1 --------'' |   '--------- prod1
    //                   offset1 --------'
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(movea_args[0]);
    SgAsmBinaryAdd *sum1 = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
    SgAsmBinaryAdd *sum2 = sum1 ? isSgAsmBinaryAdd(sum1->get_lhs()) : NULL;
    SgAsmDirectRegisterExpression *reg1 = sum2 ? isSgAsmDirectRegisterExpression(sum2->get_lhs()) : NULL;
    SgAsmIntegerValueExpression *offset1 = sum2 ? isSgAsmIntegerValueExpression(sum2->get_rhs()) : NULL;
    SgAsmBinaryMultiply *prod1 = sum1 ? isSgAsmBinaryMultiply(sum1->get_rhs()) : NULL;
    SgAsmDirectRegisterExpression *reg2 = prod1 ? isSgAsmDirectRegisterExpression(prod1->get_lhs()) : NULL;
    SgAsmIntegerValueExpression *scale1 = prod1 ? isSgAsmIntegerValueExpression(prod1->get_rhs()) : NULL;
    if (!reg1 || reg1->get_descriptor()!=REG_PC || !offset1 || offset1->get_absoluteValue()!=6 ||
        !reg2 || !scale1 || scale1->get_absoluteValue()!=2)
        return chain;

    // MOVEA second argument is: Ax
    //           reg3  ----------^^
    SgAsmDirectRegisterExpression *reg3 = isSgAsmDirectRegisterExpression(movea_args[1]);
    if (!reg3)
        return chain;

    // JMP argument is:    (+ (+ PC 2) (* Ax 1))
    //            sum3 -----^  ^ ^^ ^   ^ ^^ ^------ scale2
    //            sum4 --------' || |   | ''-------- reg5
    //            reg4 ----------'' |   '----------- prod2
    //            offset2 ----------'
    SgAsmBinaryAdd *sum3 = isSgAsmBinaryAdd(jmp_args[0]);
    SgAsmBinaryAdd *sum4 = sum3 ? isSgAsmBinaryAdd(sum3->get_lhs()) : NULL;
    SgAsmDirectRegisterExpression *reg4 = sum4 ? isSgAsmDirectRegisterExpression(sum4->get_lhs()) : NULL;
    SgAsmIntegerValueExpression *offset2 = sum4 ? isSgAsmIntegerValueExpression(sum4->get_rhs()) : NULL;
    SgAsmBinaryMultiply *prod2 = sum3 ? isSgAsmBinaryMultiply(sum3->get_rhs()) : NULL;
    SgAsmDirectRegisterExpression *reg5 = prod2 ? isSgAsmDirectRegisterExpression(prod2->get_lhs()) : NULL;
    SgAsmIntegerValueExpression *scale2 = prod2 ? isSgAsmIntegerValueExpression(prod2->get_rhs()) : NULL;
    if (!reg4 || reg4->get_descriptor()!=REG_PC || !offset2 || offset2->get_absoluteValue()!=2 ||
        !reg5 || !scale2 || scale2->get_absoluteValue()!=1)
        return chain;

    // Make sure the destination of the MOVEA is the index register for the JMP
    if (reg3->get_descriptor() != reg5->get_descriptor())
        return chain;

    // At this point we've matched the MOVEA/JMP pair and we know that the offset table follows the JMP instruction.  Read
    // offsets from the table until the table runs into one of our successors.
    rose_addr_t startOfOffsetTable = jmp->get_address() + jmp->get_size();
    size_t tableIdx = 0;
    rose_addr_t leastCodeVa = (rose_addr_t)(-1);
    std::set<rose_addr_t> codeVas;
    MemoryMap::Ptr map = args.partitioner.memoryMap();
    while (1) {
        // Where is the offset in memory?  It must be between the end of the JMP instruction (watch out for overflow) and the
        // lowest address for a switch case.
        rose_addr_t offsetVa = startOfOffsetTable + tableIdx*2;
        if (offsetVa < startOfOffsetTable || offsetVa+2 >= leastCodeVa)
            break;
        AddressInterval offsetExtent = AddressInterval::baseSize(offsetVa, 2);

        // If the memory containing the offset is already used by some other basic block then it probably isn't an offset, but
        // rather an instruction.  This sometimes terminates an offset table where the first thing after the offset table isn't
        // a case that's pointed to by the offset table.  But it will only catch those situations if we already know about such
        // a basic block.  This seems to work okay for well-formed (non-obfuscated) code when the recursive basic block
        // discovery follows higher-address blocks before lower-address blocks.
        if (args.partitioner.placeholderExists(offsetVa) || !args.partitioner.basicBlocksOverlapping(offsetExtent).empty())
            break;
        
        // Read the offset from the offset table.  Something went wrong if we can't read it because we know that the code for
        // the switch cases follows the table.
        uint16_t offsetBE;
        if (2!=map->at(offsetVa).limit(2).require(MemoryMap::EXECUTABLE).read((uint8_t*)&offsetBE).size()) {
            mlog[WARN] <<"short read entry[" <<tableIdx <<"] " <<StringUtility::addrToString(offsetVa)
                       <<" in offset table " <<StringUtility::addrToString(startOfOffsetTable) <<"\n";
            break;
        }

        // M68k offsets are 16-bit big-endian that are sign extended to 32 bits
        rose_addr_t offset = ByteOrder::be_to_host(offsetBE);
        offset = IntegerOps::signExtend2(offset, 16, 32);

        // Case code address is computed from the JMP instruction. The case code must be after the (current) end of the offset
        // table, which is also after the JMP instruction.
        rose_addr_t codeVa = (jmp->get_address() + 4 + offset) & IntegerOps::GenMask<rose_addr_t, 32>::value;
        if (codeVa < offsetVa+2)
            break;

        codeVas.insert(codeVa);
        leastCodeVa = std::min(leastCodeVa, codeVa);
        ++tableIdx;
    }
    if (0==tableIdx)
        return chain;                                   // we failed to find any entries in the offset table

    // We now know the size of the offset table and the addresses of all the switch cases.  Throw away the successors that were
    // already computed (it was probably just a single indeterminate successor) and replace them with the ones we found.
    args.bblock->successors().clear();
    BOOST_FOREACH (rose_addr_t va, codeVas)
        args.bblock->insertSuccessor(va, 32);

    // Create a data block for the offset table and attach it to the basic block
    SgAsmType *tableEntryType = SageBuilderAsm::buildTypeU16();
    SgAsmType *tableType = SageBuilderAsm::buildTypeVector(tableIdx, tableEntryType);
    DataBlock::Ptr offsetTable = DataBlock::instance(startOfOffsetTable, tableType);
    offsetTable->comment("m68k 'switch' statement's 'case' address table");
    args.bblock->insertDataBlock(offsetTable);

    return chain;
}

std::vector<Function::Ptr>
findInterruptFunctions(const Partitioner &partitioner, rose_addr_t vectorVa) {
    std::vector<Function::Ptr> functions;
    std::set<rose_addr_t> functionVas;
    for (size_t i=0; i<256; ++i) {
        rose_addr_t elmtVa = vectorVa + 4*i;
        uint32_t functionVa;
        if (4 == partitioner.memoryMap()->at(elmtVa).limit(4).read((uint8_t*)&functionVa).size()) {
            functionVa = ByteOrder::be_to_host(functionVa);
            std::string name;
            unsigned reasons = SgAsmFunction::FUNC_EXCEPTION_HANDLER;
            switch (i) {
                case 0: continue; // this vector entry is the initial stack pointer, not a function address
                case 1: name = "reset_handler"; reasons |= SgAsmFunction::FUNC_ENTRY_POINT; break;
                case 2: name = "access_fault_handler"; break;
                case 3: name = "address_error_handler"; break;
                case 4: name = "illegal_insn_handler"; break;
                case 5: name = "integer_divide_by_zero_handler"; break;
                case 6: name = "chk_insn_handler"; break;
                case 7: name = "trap_insn_handler"; break;
                case 8: name = "privilege_violation_handler"; break;
                case 9: name = "trace_handler"; break;
                case 10: name = "line_1010_emulator"; break;
                case 11: name = "line_1111_emulator"; break;
                case 12: name = "non_pc_breakpoint_debug_handler"; break;
                case 13: name = "pc_breakpoing_debug_handler"; break;
                case 14: name = "format_error_handler"; break;
                case 15: name = "uninitialized_interrupt_handler"; break;
                case 24: name = "spurious_interrupt_handler"; break;
                case 25: name = "level_1_interrupt_autovector_handler"; break;
                case 26: name = "level_2_interrupt_autovector_handler"; break;
                case 27: name = "level_3_interrupt_autovector_handler"; break;
                case 28: name = "level_4_interrupt_autovector_handler"; break;
                case 29: name = "level_5_interrupt_autovector_handler"; break;
                case 30: name = "level_6_interrupt_autovector_handler"; break;
                case 31: name = "level_7_interrupt_autovector_handler"; break;
                case 32:
                case 33:
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47: name = "trap_" + StringUtility::numberToString(i-32) + "_insn_handler"; break;
                case 48: name = "fp_branch_on_unordered_condition_handler"; break;
                case 49: name = "fp_inexact_result_handler"; break;
                case 50: name = "fp_divide_by_zero_handler"; break;
                case 51: name = "fp_underflow_handler"; break;
                case 52: name = "fp_operand_error_handler"; break;
                case 53: name = "fp_overflow_handler"; break;
                case 54: name = "fp_signaling_nan_handler"; break;
                case 55: name = "fp_input_denormalized_handler"; break;
                case 56: name = "mmu_configuration_error_handler"; break;
                case 57: name = "mmu_illegal_operation_handler"; break;
                case 58: name = "mmu_access_level_violation_handler"; break;
                case 61: name = "unsupported_instruction_handler"; break;
                default:
                    name = "interrupt_vector_"+StringUtility::numberToString(i)+"_handler"; break;
            }
                        
            Function::Ptr function = Function::instance(functionVa, name, reasons);
            function->reasonComment("vector " + StringUtility::addrToString(vectorVa) +
                                    " entry " + StringUtility::numberToString(i));
            if (Sawyer::Optional<Function::Ptr> found = getUnique(functions, function, sortFunctionsByAddress)) {
                // Multiple vector entries point to the same function, so give it a rather generic name
                found.get()->name("interrupt_vector_function");
            } else {
                insertUnique(functions, function, sortFunctionsByAddress);
            }
        }
    }
    return functions;
}

} // namespace
} // namespace
} // namespace
} // namespace
