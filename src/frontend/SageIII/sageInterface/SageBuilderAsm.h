#ifndef ROSE_SageBuilderAsm_H
#define ROSE_SageBuilderAsm_H

namespace Rose {

/** Functions to build binary analysis ASTs. */
namespace SageBuilderAsm {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build files

/** Build a new binary composite object.
 *
 *  A SgBinaryComposite is the top-level node for binary analysis and holds two lists: a list of file headers representing the
 *  various executable containers that have been parsed, and a list of interpretations that organize thos headers into
 *  compatible units.  For instance, a Microsoft Windows executable file will have two headers (DOS and PE) and two
 *  interpretations (one for the DOS code and data and another for the PE code and data).  If dynamic linking is performed,
 *  then even more headers will be present, but their code and data will likely be inserted into one of the existing
 *  interpretations.   An interpretation (SgAsmInterpretation) is analogous to a process.
 *
 *  ASTs normally have only one SgBinaryComposite, but an AST can have more than one if the AST is used to analyze two or more
 *  binaries.  For instance, if we have two related versions of the i586 ELF "login" program and we want to run some analysis
 *  that compares the two programs, then we probably want two SgBinaryComposite nodes in the AST. Each SgBinaryComposite will
 *  have a SgAsmInterpretation to represent the i586 "login" process.  Doing it this way will prevent headers and code from one
 *  version to be confused with headers and code from the other version.
 *
 *  A SgProject node will be created if one has not been created already. The segments/sections from the file are mapped into
 *  process virtual memory in the SgAsmInterpretation but are not disassembled. */
SgBinaryComposite* buildBinaryComposite(const std::string &fileName);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build data types
//
// These data type builders always return the same IR node and are thread-safe.

// Generic data types
SgAsmIntegerType* buildTypeU(size_t nBits);             /**< Unsigned type of specified size. */
SgAsmIntegerType* buildTypeI(size_t nBits);             /**< Signed type of specified size. */
SgAsmIntegerType* buildTypeU1();                        /**< 1-bit unsigned (Boolean). */
SgAsmIntegerType* buildTypeU4();                        /**< 4-bit unsigned. */
SgAsmIntegerType* buildTypeU8();                        /**< 8-bit unsigned. */
SgAsmIntegerType* buildTypeU16();                       /**< 16-bit unsigned. */
SgAsmIntegerType* buildTypeU32();                       /**< 32-bit unsigned. */
SgAsmIntegerType* buildTypeU64();                       /**< 64-bit unsigned. */
SgAsmIntegerType* buildTypeI8();                        /**< 8-bit signed. */
SgAsmIntegerType* buildTypeI16();                       /**< 16-bit signed. */
SgAsmIntegerType* buildTypeI32();                       /**< 32-bit signed. */
SgAsmIntegerType* buildTypeI64();                       /**< 64-bit signed. */
SgAsmFloatType* buildIeee754Binary32();                 /**< 32-bit IEEE-754 floating-point. */
SgAsmFloatType* buildIeee754Binary64();                 /**< 64-bit IEEE-754 floating-point. */
SgAsmFloatType* buildIeee754Binary80();                 /**< 80-bit IEEE-754 floating-point (as in x86). */
SgAsmVectorType* buildTypeVector(size_t, SgAsmType*);   /**< Fixed-size, packed array. */

// Architecture-specific data types
SgAsmIntegerType *buildTypeX86Byte();                   /**< 8-bit unsigned. */
SgAsmIntegerType *buildTypeX86Word();                   /**< 16-bit unsigned. */
SgAsmIntegerType *buildTypeX86DoubleWord();             /**< 32-bit unsigned. */
SgAsmIntegerType *buildTypeX86QuadWord();               /**< 64-bit unsigned. */
SgAsmFloatType *buildTypeX86Float32();                  /**< 32-bit IEEE-754 floating-point. */
SgAsmFloatType *buildTypeX86Float64();                  /**< 64-bit IEEE-754 floating-point. */
SgAsmFloatType *buildTypeX86Float80();                  /**< 80-bit IEEE-754 floating-point. */
SgAsmVectorType *buildTypeX86DoubleQuadWord();          /**< Vector of two 64-bit unsigned integers. */
SgAsmFloatType *buildTypeM68kFloat96();                 /**< Motorola M68k 96-bit float (16-bits are always zero). */
SgAsmFloatType *buildTypeM68kFloat80();                 /**< Motorola M68k 96-bit float w/out 16-bit constant zero field. */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic value builders
SgAsmIntegerValueExpression* buildValueInteger(uint64_t value, SgAsmType*);
SgAsmIntegerValueExpression* buildValueInteger(const Sawyer::Container::BitVector&, SgAsmType*);
SgAsmFloatValueExpression* buildValueFloat(double value, SgAsmType*);
SgAsmFloatValueExpression* buildValueFloat(const Sawyer::Container::BitVector&, SgAsmType*);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Building integer values.

// Build integer values generically (one of these per buildType function above)
SgAsmIntegerValueExpression* buildValueU1(bool);
SgAsmIntegerValueExpression* buildValueU8(uint8_t);
SgAsmIntegerValueExpression* buildValueU16(uint16_t);
SgAsmIntegerValueExpression* buildValueU32(uint32_t);
SgAsmIntegerValueExpression* buildValueU64(uint64_t);
SgAsmIntegerValueExpression* buildValueI8(int8_t);
SgAsmIntegerValueExpression* buildValueI16(int16_t);
SgAsmIntegerValueExpression* buildValueI32(int32_t);
SgAsmIntegerValueExpression* buildValueI64(int64_t);
SgAsmFloatValueExpression* buildValueIeee754Binary32(double);
SgAsmFloatValueExpression* buildValueIeee754Binary64(double);

// Building integer values with x86-specific type names
SgAsmIntegerValueExpression* buildValueX86Byte(uint8_t);
SgAsmIntegerValueExpression* buildValueX86Word(uint16_t);
SgAsmIntegerValueExpression* buildValueX86DWord(uint32_t);
SgAsmIntegerValueExpression* buildValueX86QWord(uint64_t);
SgAsmFloatValueExpression* buildValueX86Float32(double);
SgAsmFloatValueExpression* buildValueX86Float64(double);
SgAsmFloatValueExpression* buildValueX86Float80(double);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
SgAsmBinaryAdd* buildAddExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinarySubtract* buildSubtractExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinaryAddPreupdate* buildAddPreupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinarySubtractPreupdate* buildSubtractPreupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinaryAddPostupdate* buildAddPostupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinarySubtractPostupdate* buildSubtractPostupdateExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinaryMultiply* buildMultiplyExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinaryLsl* buildLslExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinaryLsr* buildLsrExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinaryAsr* buildAsrExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmBinaryRor* buildRorExpression(SgAsmExpression *lhs, SgAsmExpression *rhs, SgAsmType *type=NULL);
SgAsmUnaryRrx* buildRrxExpression(SgAsmExpression *lhs, SgAsmType *type=NULL);
SgAsmUnaryArmSpecialRegisterList *buildArmSpecialRegisterList(SgAsmExpression *lhs);
SgAsmExprListExp* buildExprListExpression();
void appendExpression(SgAsmExprListExp*, SgAsmExpression*);
SgAsmMemoryReferenceExpression* buildMemoryReferenceExpression(SgAsmExpression *addr, SgAsmExpression *segment=NULL,
                                                               SgAsmType *type=NULL);
SgAsmRiscOperation* buildRiscOperation(SgAsmRiscOperation::RiscOperator);
SgAsmRiscOperation* buildRiscOperation(SgAsmRiscOperation::RiscOperator, SgAsmExpression*);
SgAsmRiscOperation* buildRiscOperation(SgAsmRiscOperation::RiscOperator, SgAsmExpression*, SgAsmExpression*);
SgAsmRiscOperation* buildRiscOperation(SgAsmRiscOperation::RiscOperator, SgAsmExpression*, SgAsmExpression*, SgAsmExpression*);
SgAsmRiscOperation* buildRiscOperation(SgAsmRiscOperation::RiscOperator, SgAsmExpression*, SgAsmExpression*, SgAsmExpression*,
                                       SgAsmExpression*);


template <typename Insn>
inline Insn* appendOperand(Insn* insn, SgAsmExpression* op) {
    SgAsmOperandList* operands = insn->get_operandList();
    operands->get_operands().push_back(op);
    op->set_parent(operands);
    return insn; // For chaining this operation
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build instruction
SgAsmX86Instruction* buildX86Instruction(Rose::BinaryAnalysis::X86InstructionKind);
SgAsmX86Instruction* buildX86Instruction(Rose::BinaryAnalysis::X86InstructionKind, SgAsmExpression *operand);
SgAsmX86Instruction* buildX86Instruction(Rose::BinaryAnalysis::X86InstructionKind, SgAsmExpression *lhs, SgAsmExpression *rhs);
SgAsmX86Instruction *buildX86MultibyteNopInstruction(size_t nBytes);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build basic blocks
SgAsmBlock* buildBasicBlock(const std::vector<SgAsmInstruction*>&);
SgAsmFunction* buildFunction(rose_addr_t entryVa, const std::vector<SgAsmBlock*>&);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Build data blocks
SgAsmStaticData* buildStaticData(rose_addr_t startVa, const SgUnsignedCharList &rawData);
SgAsmBlock* buildDataBlock(SgAsmStaticData*);


} // namespace
} // namespace

#endif
