/******************************************************************************************************************************
 * NOTE:  For any given IR class, please keep all its parts as close together as possible.  Its bad enough that we're
 *        defining classes in a non-C-like language -- and it's painful when a definition is scattered all over this file. We
 *        already have four places to look to get the definition of an IR class:
 *              (1) here, in this source file, built with ROSETTA functions
 *              (2) additional declarations between HEADER_* markers in ../Grammar/BinaryInstruction.code (which we can't
 *                  even parse as a C++ file, making IDEs useless there).
 *              (3) function definitions between SOURCE_* markers in ../Grammar/BinaryInstruction.code, which for some
 *                  reason are not next to the corresponding HEADER_ markers.  Please migrate these definitions to *.C files
 *                  whenever you have an opportunity, so that IDEs can see the definitions.
 *              (4) Doxygen documentation in ../../../docs/testDoxygen directory since doxygen can't parse the IR classes
 *                  from this source file.  Documented entities are marked with a "DOXYGEN" comment here to make it more
 *                  obvious when documentation is present.
 *
 * NOTE:  First part of the file is for classes describing instructions.  Middle part is for classes describing binary
 *        containers (organized by container type: ELF, DOS, PE, ...)  Last part is the base classes.  Unlike C++, the classes
 *        defined in this file need to be defined from the most specific (derived classes) to the most general (base class).
 *
 * NOTE:  Please use three blank lines between IR node definitions to help make this file more readable.  Unless those IR
 *        nodes are so closely related to one another that it's better to keep them close.
 *-----------------------------------------------------------------------------------------------------------------------------*/

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"

void
Grammar::setUpBinaryInstructions()
{
    /**************************************************************************************************************************
     *                                  Instructions.
     * Base class (SgAsmInstruction) and various subclasses, one per architecture.
     **************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmArmInstruction, "AsmArmInstruction", "AsmArmInstructionTag");
    AsmArmInstruction.setFunctionPrototype("HEADER_BINARY_ARM_INSTRUCTION", "../Grammar/BinaryInstruction.code");
    AsmArmInstruction.setDataPrototype("ArmInstructionKind", "kind", "= arm_unknown_instruction",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmArmInstruction.setDataPrototype("ArmInstructionCondition", "condition", "= arm_cond_unknown",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmArmInstruction.setDataPrototype("int", "positionOfConditionInMnemonic", "= -1",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmArmInstruction.setPredeclarationString("HEADER_BINARY_ARM_INSTRUCTION_PREDECLARATION",
                                               "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(Asmx86Instruction, "Asmx86Instruction", "Asmx86InstructionTag");
    Asmx86Instruction.setFunctionPrototype("HEADER_BINARY_X86_INSTRUCTION", "../Grammar/BinaryInstruction.code");
    Asmx86Instruction.setDataPrototype("X86InstructionKind", "kind", "= x86_unknown_instruction",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    Asmx86Instruction.setDataPrototype("X86InstructionSize", "baseSize", "= x86_insnsize_none",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    Asmx86Instruction.setDataPrototype("X86InstructionSize", "operandSize", "= x86_insnsize_none",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    Asmx86Instruction.setDataPrototype("X86InstructionSize", "addressSize", "= x86_insnsize_none",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    Asmx86Instruction.setDataPrototype("bool", "lockPrefix", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    Asmx86Instruction.setDataPrototype("X86RepeatPrefix", "repeatPrefix", "= x86_repeat_none",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    Asmx86Instruction.setDataPrototype("X86BranchPrediction", "branchPrediction", "= x86_branch_prediction_none",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    Asmx86Instruction.setDataPrototype("X86SegmentRegister", "segmentOverride", "= x86_segreg_none",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    Asmx86Instruction.setPredeclarationString("HEADER_BINARY_X86_INSTRUCTION_PREDECLARATION",
                                              "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(AsmPowerpcInstruction, "AsmPowerpcInstruction", "AsmPowerpcInstructionTag");
    AsmPowerpcInstruction.setFunctionPrototype("HEADER_BINARY_POWERPC_INSTRUCTION", "../Grammar/BinaryInstruction.code");
    AsmPowerpcInstruction.setDataPrototype("PowerpcInstructionKind", "kind", "= powerpc_unknown_instruction",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmPowerpcInstruction.setPredeclarationString("HEADER_BINARY_POWERPC_INSTRUCTION_PREDECLARATION",
                                                  "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(AsmMipsInstruction, "AsmMipsInstruction", "AsmMipsInstructionTag");
    AsmMipsInstruction.setFunctionPrototype("HEADER_BINARY_MIPS_INSTRUCTION", "../Grammar/BinaryInstruction.code");
    AsmMipsInstruction.setDataPrototype("MipsInstructionKind", "kind", "= mips_unknown_instruction",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmMipsInstruction.setPredeclarationString("HEADER_BINARY_MIPS_INSTRUCTION_PREDECLARATION",
                                               "../Grammar/BinaryInstruction.code");



    NEW_NONTERMINAL_MACRO(AsmInstruction,
                          Asmx86Instruction | AsmArmInstruction | AsmPowerpcInstruction | AsmMipsInstruction,
                          "AsmInstruction", "AsmInstructionTag", true);
    AsmInstruction.setPredeclarationString("HEADER_BINARY_INSTRUCTION_PREDECLARATION", "../Grammar/BinaryInstruction.code");
    AsmInstruction.setFunctionPrototype("HEADER_BINARY_INSTRUCTION", "../Grammar/BinaryInstruction.code");
    AsmInstruction.setFunctionSource("SOURCE_BINARY_INSTRUCTION", "../Grammar/BinaryInstruction.code");
    AsmInstruction.setDataPrototype("std::string", "mnemonic", "= \"\"",
                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmInstruction.setDataPrototype("SgUnsignedCharList", "raw_bytes", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmInstruction.setDataPrototype("SgAsmOperandList*", "operandList", "= NULL",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmInstruction.setDataPrototype("SgAsmStatementPtrList", "sources", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /**************************************************************************************************************************
     *                                  Instruction Expressions
     * Related functions and documentation can be found in src/frontend/Disassemblers/Expressions.C
     **************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmOperandList, "AsmOperandList",  "AsmOperandListTag");
    AsmOperandList.setFunctionPrototype("HEADER_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");
    AsmOperandList.setFunctionSource("SOURCE_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");
    AsmOperandList.setDataPrototype("SgAsmExpressionPtrList", "operands", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    // Binary expressions
    NEW_TERMINAL_MACRO(AsmBinaryAdd,                "AsmBinaryAdd",                "AsmBinaryAddTag");
    NEW_TERMINAL_MACRO(AsmBinarySubtract,           "AsmBinarySubtract",           "AsmBinarySubtractTag");
    NEW_TERMINAL_MACRO(AsmBinaryMultiply,           "AsmBinaryMultiply",           "AsmBinaryMultiplyTag");
    NEW_TERMINAL_MACRO(AsmBinaryDivide,             "AsmBinaryDivide",             "AsmBinaryDivideTag");
    NEW_TERMINAL_MACRO(AsmBinaryMod,                "AsmBinaryMod",                "AsmBinaryModTag");
    NEW_TERMINAL_MACRO(AsmBinaryAddPreupdate,       "AsmBinaryAddPreupdate",       "AsmBinaryAddPreupdateTag");
    NEW_TERMINAL_MACRO(AsmBinarySubtractPreupdate,  "AsmBinarySubtractPreupdate",  "AsmBinarySubtractPreupdateTag");
    NEW_TERMINAL_MACRO(AsmBinaryAddPostupdate,      "AsmBinaryAddPostupdate",      "AsmBinaryAddPostupdateTag");
    NEW_TERMINAL_MACRO(AsmBinarySubtractPostupdate, "AsmBinarySubtractPostupdate", "AsmBinarySubtractPostupdateTag");
    NEW_TERMINAL_MACRO(AsmBinaryLsl,                "AsmBinaryLsl",                "AsmBinaryLslTag");
    NEW_TERMINAL_MACRO(AsmBinaryLsr,                "AsmBinaryLsr",                "AsmBinaryLsrTag");
    NEW_TERMINAL_MACRO(AsmBinaryAsr,                "AsmBinaryAsr",                "AsmBinaryAsrTag");
    NEW_TERMINAL_MACRO(AsmBinaryRor,                "AsmBinaryRor",                "AsmBinaryRorTag");
    NEW_NONTERMINAL_MACRO(AsmBinaryExpression,
                          AsmBinaryAdd               | AsmBinarySubtract      | AsmBinaryMultiply           |
                          AsmBinaryDivide            | AsmBinaryMod           | AsmBinaryAddPreupdate       |
                          AsmBinarySubtractPreupdate | AsmBinaryAddPostupdate | AsmBinarySubtractPostupdate |
                          AsmBinaryLsl               | AsmBinaryLsr           | AsmBinaryAsr                |
                          AsmBinaryRor,
                          "AsmBinaryExpression", "AsmBinaryExpressionTag", false);
    AsmBinaryExpression.setDataPrototype("SgAsmExpression*", "lhs", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmBinaryExpression.setDataPrototype("SgAsmExpression*", "rhs", "= NULL",
                                         CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    // Unary expressions
    NEW_TERMINAL_MACRO(AsmUnaryPlus ,                  "AsmUnaryPlus",                   "AsmUnaryPlusTag");
    NEW_TERMINAL_MACRO(AsmUnaryMinus,                  "AsmUnaryMinus",                  "AsmUnaryMinusTag");
    NEW_TERMINAL_MACRO(AsmUnaryRrx,                    "AsmUnaryRrx",                    "AsmUnaryRrxTag");
    NEW_TERMINAL_MACRO(AsmUnaryArmSpecialRegisterList, "AsmUnaryArmSpecialRegisterList", "AsmUnaryArmSpecialRegisterListTag");
    NEW_NONTERMINAL_MACRO(AsmUnaryExpression,
                          AsmUnaryPlus | AsmUnaryMinus | AsmUnaryRrx | AsmUnaryArmSpecialRegisterList,
                          "AsmUnaryExpression", "AsmUnaryExpressionTag", false);
    AsmUnaryExpression.setDataPrototype("SgAsmExpression*", "operand", "= NULL",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    // References to registers
    NEW_TERMINAL_MACRO(Asmx86RegisterReferenceExpression,
                       "Asmx86RegisterReferenceExpression", "Asmx86RegisterReferenceExpressionTag");
    Asmx86RegisterReferenceExpression.setFunctionPrototype("HEADER_BINARY_X86_REGISTER_REFERENCE_EXPRESSION",
                                                           "../Grammar/BinaryInstruction.code");
    Asmx86RegisterReferenceExpression.setFunctionSource("SOURCE_BINARY_X86_REGISTER_REFERENCE_EXPRESSION",
                                                        "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(AsmArmRegisterReferenceExpression ,
                       "AsmArmRegisterReferenceExpression", "AsmArmRegisterReferenceExpressionTag");
    AsmArmRegisterReferenceExpression.setFunctionPrototype("HEADER_BINARY_ARM_REGISTER_REFERENCE_EXPRESSION",
                                                           "../Grammar/BinaryInstruction.code");
    AsmArmRegisterReferenceExpression.setDataPrototype("unsigned", "psr_mask", "=0",
                                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmPowerpcRegisterReferenceExpression,
                       "AsmPowerpcRegisterReferenceExpression", "AsmPowerpcRegisterReferenceExpressionTag");




    NEW_TERMINAL_MACRO(AsmMipsRegisterReferenceExpression,
                       "AsmMipsRegisterReferenceExpression", "AsmMipsRegisterReferenceExpressionTag");




    NEW_NONTERMINAL_MACRO(AsmRegisterReferenceExpression ,
                          Asmx86RegisterReferenceExpression | AsmArmRegisterReferenceExpression |
                          AsmPowerpcRegisterReferenceExpression | AsmMipsRegisterReferenceExpression,
                          "AsmRegisterReferenceExpression", "AsmRegisterReferenceExpressionTag" , false);
    AsmRegisterReferenceExpression.setDataPrototype("RegisterDescriptor", "descriptor", "",
                                                    CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    // Integer Constants (FIXME: These use x86 nomenclature and will likely be changed. [Robb P. Matzke 2013-02-13])
    NEW_TERMINAL_MACRO(AsmByteValueExpression, "AsmByteValueExpression", "AsmByteValueExpressionTag");
    AsmByteValueExpression.setFunctionPrototype("HEADER_BYTE_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmByteValueExpression.setFunctionSource("SOURCE_BYTE_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmByteValueExpression.setAutomaticGenerationOfConstructor(false); // we need to set the "p_type" member.

    NEW_TERMINAL_MACRO(AsmWordValueExpression, "AsmWordValueExpression", "AsmWordValueExpressionTag");
    AsmWordValueExpression.setFunctionPrototype("HEADER_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmWordValueExpression.setFunctionSource("SOURCE_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmWordValueExpression.setAutomaticGenerationOfConstructor(false); // we need to set the "p_type" member.

    NEW_TERMINAL_MACRO(AsmDoubleWordValueExpression, "AsmDoubleWordValueExpression", "AsmDoubleWordValueExpressionTag");
    AsmDoubleWordValueExpression.setFunctionPrototype("HEADER_DOUBLE_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmDoubleWordValueExpression.setFunctionSource("SOURCE_DOUBLE_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmDoubleWordValueExpression.setAutomaticGenerationOfConstructor(false); // we need to set the "p_type" member

    NEW_TERMINAL_MACRO(AsmQuadWordValueExpression, "AsmQuadWordValueExpression", "AsmQuadWordValueExpressionTag");
    AsmQuadWordValueExpression.setFunctionPrototype("HEADER_QUAD_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmQuadWordValueExpression.setFunctionSource("SOURCE_QUAD_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmQuadWordValueExpression.setAutomaticGenerationOfConstructor(false); // we need to set the "p_type" member

    NEW_NONTERMINAL_MACRO(AsmIntegerValueExpression,
                          AsmByteValueExpression | AsmWordValueExpression | AsmDoubleWordValueExpression |
                          AsmQuadWordValueExpression,
                          "AsmIntegerValueExpression", "AsmIntegerValueExpressionTag", false);
    AsmIntegerValueExpression.setFunctionPrototype("HEADER_INTEGER_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmIntegerValueExpression.setDataPrototype("SgNode*", "base_node", "=NULL",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmIntegerValueExpression.setDataPrototype("uint64_t", "relative_value", "=NULL",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmIntegerValueExpression.setDataPrototype("size_t", "significant_bits", "=0",
                                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


    // Floating point constants (FIXME: These use x86 nomenclature and will likely be changed. [Robb P. Matzke 2013-02-13])
    NEW_TERMINAL_MACRO(AsmSingleFloatValueExpression, "AsmSingleFloatValueExpression", "AsmSingleFloatValueExpressionTag");
    AsmSingleFloatValueExpression.setFunctionPrototype("HEADER_SINGLE_FLOAT_VALUE_EXPRESSION",
                                                       "../Grammar/BinaryInstruction.code");
    AsmSingleFloatValueExpression.setFunctionSource("SOURCE_SINGLE_FLOAT_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmSingleFloatValueExpression.setAutomaticGenerationOfConstructor(false); // we need to set the "p_type" member
    AsmSingleFloatValueExpression.setDataPrototype("float", "value", "= 0.0F",
                                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDoubleFloatValueExpression, "AsmDoubleFloatValueExpression", "AsmDoubleFloatValueExpressionTag");
    AsmDoubleFloatValueExpression.setFunctionPrototype("HEADER_DOUBLE_FLOAT_VALUE_EXPRESSION",
                                                       "../Grammar/BinaryInstruction.code");
    AsmDoubleFloatValueExpression.setFunctionSource("SOURCE_DOUBLE_FLOAT_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
    AsmDoubleFloatValueExpression.setAutomaticGenerationOfConstructor(false); // we need to set the "p_type" member
    AsmDoubleFloatValueExpression.setDataPrototype("double", "value", "= 0.0",
                                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);





    // Values that are addresses or references to data will have symbols in a function symbol table.  All other
    // values are assumed to be literals and will not have associated symbols.
    NEW_NONTERMINAL_MACRO(AsmValueExpression,
                          AsmIntegerValueExpression | AsmSingleFloatValueExpression | AsmDoubleFloatValueExpression,
                          "AsmValueExpression", "AsmValueExpressionTag", false);
    AsmValueExpression.setDataPrototype("SgAsmValueExpression*", "unfolded_expression_tree", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmValueExpression.setDataPrototype("unsigned short", "bit_offset", "= 0",         // DOXYGEN
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmValueExpression.setDataPrototype("unsigned short", "bit_size", "= 0",           // DOXYGEN
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmValueExpression.setDataPrototype("SgSymbol*", "symbol", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


    // References to memory
    NEW_TERMINAL_MACRO(AsmMemoryReferenceExpression, "AsmMemoryReferenceExpression", "AsmMemoryReferenceExpressionTag");
    AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*", "address", "= NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*", "segment", "= NULL",
                                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmControlFlagsExpression, "AsmControlFlagsExpression", "AsmControlFlagsExpressionTag");
    AsmControlFlagsExpression.setDataPrototype("unsigned long", "bit_flags", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmCommonSubExpression, "AsmCommonSubExpression", "AsmCommonSubExpressionTag");
    AsmCommonSubExpression.setDataPrototype("SgAsmExpression*", "subexpression", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    // Lists of expressions
    NEW_TERMINAL_MACRO(AsmExprListExp, "AsmExprListExp", "AsmExprListExpTag");
    AsmExprListExp.setDataPrototype("SgAsmExpressionPtrList", "expressions", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    // Top-level expression
    // FIXME: p_replacement is only set by RoseBin_IDAPRO_buildTree::resolveRecursivelyExpression() and appears to be used only
    //        in a couple of files in src/midend/binaryAnalsyses (and elsewhere only for converting a SgAsmExpression to a
    //        string). It seems to hold the name of a function, such as "_malloc" or "malloc@plt" for branch instructions. It
    //        should be possible to obtain the function name by looking up the instruction at the branch target and then
    //        following parent links in the AST until we reach the SgAsmFunction node, which has a get_name() method.
    //        [RPM 2009-07-16].
    NEW_NONTERMINAL_MACRO(AsmExpression,
                          AsmValueExpression           | AsmBinaryExpression            | AsmUnaryExpression        |
                          AsmMemoryReferenceExpression | AsmRegisterReferenceExpression | AsmControlFlagsExpression |
                          AsmCommonSubExpression       | AsmExprListExp,
                          "AsmExpression", "AsmExpressionTag", false);
    AsmExpression.setDataPrototype("SgAsmType*", "type", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmExpression.setDataPrototype("std::string", "replacement", "= \"\"",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmExpression.setDataPrototype("std::string", "comment", "= \"\"",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /**************************************************************************************************************************
     *                                  Data types
     * These are attached to the SgAsmNode object (the root for the IR used for binaries)
     **************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmTypeByte, "AsmTypeByte", "AsmTypeByteTag");
    AsmTypeByte.setFunctionPrototype("HEADER_BINARY_TYPE_BYTE", "../Grammar/BinaryInstruction.code");
    AsmTypeByte.setFunctionSource("SOURCE_BINARY_TYPE_BYTE", "../Grammar/BinaryInstruction.code");
    AsmTypeByte.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                 NO_DELETE, NO_COPY_DATA);      // FIXME: Should that be '|' rather than '||'? [RPM 2011-09-26]

    NEW_TERMINAL_MACRO(AsmTypeWord, "AsmTypeWord", "AsmTypeWordTag");
    AsmTypeWord.setFunctionPrototype("HEADER_BINARY_TYPE_WORD", "../Grammar/BinaryInstruction.code");
    AsmTypeWord.setFunctionSource("SOURCE_BINARY_TYPE_WORD", "../Grammar/BinaryInstruction.code");
    AsmTypeWord.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                 NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                 NO_DELETE, NO_COPY_DATA);

    NEW_TERMINAL_MACRO(AsmTypeDoubleWord, "AsmTypeDoubleWord", "AsmTypeDoubleWordTag");
    AsmTypeDoubleWord.setFunctionPrototype("HEADER_BINARY_TYPE_DOUBLE_WORD", "../Grammar/BinaryInstruction.code");
    AsmTypeDoubleWord.setFunctionSource("SOURCE_BINARY_TYPE_DOUBLE_WORD", "../Grammar/BinaryInstruction.code");
    AsmTypeDoubleWord.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                       NO_DELETE, NO_COPY_DATA);

    NEW_TERMINAL_MACRO(AsmTypeQuadWord, "AsmTypeQuadWord", "AsmTypeQuadWordTag");
    AsmTypeQuadWord.setFunctionPrototype("HEADER_BINARY_TYPE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
    AsmTypeQuadWord.setFunctionSource("SOURCE_BINARY_TYPE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
    AsmTypeQuadWord.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                     NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                     NO_DELETE, NO_COPY_DATA);

    NEW_TERMINAL_MACRO(AsmTypeDoubleQuadWord, "AsmTypeDoubleQuadWord", "AsmTypeDoubleQuadWordTag");
    AsmTypeDoubleQuadWord.setFunctionPrototype("HEADER_BINARY_TYPE_DOUBLE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
    AsmTypeDoubleQuadWord.setFunctionSource("SOURCE_BINARY_TYPE_DOUBLE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
    AsmTypeDoubleQuadWord.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                           NO_DELETE, NO_COPY_DATA);

    NEW_TERMINAL_MACRO(AsmTypeSingleFloat, "AsmTypeSingleFloat", "AsmTypeSingleFloatTag");
    AsmTypeSingleFloat.setFunctionPrototype("HEADER_BINARY_TYPE_SINGLE_FLOAT", "../Grammar/BinaryInstruction.code");
    AsmTypeSingleFloat.setFunctionSource("SOURCE_BINARY_TYPE_SINGLE_FLOAT", "../Grammar/BinaryInstruction.code");
    AsmTypeSingleFloat.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                        NO_DELETE, NO_COPY_DATA);

    NEW_TERMINAL_MACRO(AsmTypeDoubleFloat, "AsmTypeDoubleFloat", "AsmTypeDoubleFloatTag");
    AsmTypeDoubleFloat.setFunctionPrototype("HEADER_BINARY_TYPE_DOUBLE_FLOAT", "../Grammar/BinaryInstruction.code");
    AsmTypeDoubleFloat.setFunctionSource("SOURCE_BINARY_TYPE_DOUBLE_FLOAT", "../Grammar/BinaryInstruction.code");
    AsmTypeDoubleFloat.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                        NO_DELETE, NO_COPY_DATA);

    NEW_TERMINAL_MACRO(AsmType80bitFloat, "AsmType80bitFloat", "AsmType80bitFloatTag");
    AsmType80bitFloat.setFunctionPrototype("HEADER_BINARY_TYPE_80bit_FLOAT", "../Grammar/BinaryInstruction.code");
    AsmType80bitFloat.setFunctionSource("SOURCE_BINARY_TYPE_80bit_FLOAT", "../Grammar/BinaryInstruction.code");
    AsmType80bitFloat.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                       NO_DELETE, NO_COPY_DATA);

    NEW_TERMINAL_MACRO(AsmType128bitFloat, "AsmType128bitFloat", "AsmType128bitFloatTag");
    AsmType128bitFloat.setFunctionPrototype("HEADER_BINARY_TYPE_128bit_FLOAT", "../Grammar/BinaryInstruction.code");
    AsmType128bitFloat.setFunctionSource("SOURCE_BINARY_TYPE_128bit_FLOAT", "../Grammar/BinaryInstruction.code");
    AsmType128bitFloat.setDataPrototype("static $CLASSNAME*", "builtin_type", "",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL,
                                        NO_DELETE, NO_COPY_DATA);

    NEW_TERMINAL_MACRO(AsmTypeVector, "AsmTypeVector", "AsmTypeVectorTag");
    AsmTypeVector.setFunctionPrototype("HEADER_BINARY_TYPE_VECTOR", "../Grammar/BinaryInstruction.code");
    AsmTypeVector.setFunctionSource("SOURCE_BINARY_TYPE_VECTOR", "../Grammar/BinaryInstruction.code");
    AsmTypeVector.setDataPrototype("int", "elementCount", "= 0",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmTypeVector.setDataPrototype("SgAsmType*", "elementType", "= NULL",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

    NEW_NONTERMINAL_MACRO(AsmType,
                          AsmTypeByte        | AsmTypeWord           | AsmTypeDoubleWord  |
                          AsmTypeQuadWord    | AsmTypeDoubleQuadWord | AsmType80bitFloat  |
                          AsmType128bitFloat | AsmTypeSingleFloat    | AsmTypeDoubleFloat |
                          AsmTypeVector,
                          "AsmType", "AsmTypeTag", false);



    /**************************************************************************************************************************
     *                                  Collections of Instructions
     **************************************************************************************************************************/

    // A function is a collection of blocks holding instructions (basic blocks) or static data.  Instructions might have
    // references to addresses or data which are described by symbols (not to be confused with the binary's symbol table) in
    // the function's symbol table (the SgAsmFunction::symbol_table member).
     NEW_TERMINAL_MACRO(AsmFunction, "AsmFunction", "AsmFunctionTag");
     AsmFunction.setFunctionPrototype("HEADER_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
     AsmFunction.setFunctionSource("SOURCE_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
     AsmFunction.setDataPrototype("std::string", "name", "= \"\"",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunction.setDataPrototype("unsigned", "reason", "= SgAsmFunction::FUNC_NONE", /*bit flags*/
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunction.setDataPrototype("SgAsmFunction::function_kind_enum", "function_kind", "= SgAsmFunction::e_unknown",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmFunction.setDataPrototype("SgAsmFunction::MayReturn", "may_return", "= SgAsmFunction::RET_UNKNOWN",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunction.setDataPrototype("std::string", "name_md5", "= \"\"",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunction.setDataPrototype("SgAsmStatementPtrList", "statementList", "",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFunction.setDataPrototype("SgAsmStatementPtrList", "dest", "",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunction.setDataPrototype("rose_addr_t", "entry_va", "= 0",  /*entry point virtual address*/
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunction.setDataPrototype("SgSymbolTable*", "symbol_table", "= NULL",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
     AsmFunction.setDataPrototype("size_t", "cached_vertex", "= (size_t)(-1)", // see BinaryAnalysis::FunctionCall
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     // Instruction basic block. One entry point (first instruction) and one exit point (last instruction).  However,
     // SgAsmBlock is also used for other things, such as collections of functions.
     NEW_TERMINAL_MACRO(AsmBlock, "AsmBlock", "AsmBlockTag");
     AsmBlock.setFunctionPrototype("HEADER_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmBlock.setFunctionSource("SOURCE_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmBlock.setDataPrototype("rose_addr_t", "next_block_true_address", "= 0", // [tps 05Apr07] needed for the control_flow_graph
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("rose_addr_t", "next_block_false_address", "= 0",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("rose_addr_t", "id", "= 0",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmBlock.setDataPrototype("unsigned", "reason", "= SgAsmBlock::BLK_NONE", // why this block exists
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("SgAsmStatementPtrList", "statementList", "", //in order of execution
                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("SgAsmIntegerValuePtrList", "successors", "",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("bool", "successors_complete", "= false",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("SgAsmBlock*", "immediate_dominator", "=NULL",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("size_t", "cached_vertex", "= (size_t)(-1)", // see BinaryAnalysis::ControlFlow
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("double", "code_likelihood", "= 0.0",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     // Represents static data in an executable.  For now, we don't associate any type with the data because ROSE's data type
     // infrastructure (source or binary) is not capable of representing the information we need: multiple interpretations of
     // overlapping parts of memory (i.e., two or more types for the same bytes); arbitrary offsets and padding in structured
     // types; size-specific integers and floating-point types; regions of unknown type; ease of improving type information by
     // filling in more details as the type is discovered; etc.
     NEW_TERMINAL_MACRO(AsmStaticData, "AsmStaticData", "AsmStaticDataTag");
     AsmStaticData.setFunctionPrototype("HEADER_STATIC_DATA", "../Grammar/BinaryInstruction.code");
     AsmStaticData.setDataPrototype("SgUnsignedCharList", "raw_bytes", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);



     NEW_NONTERMINAL_MACRO(AsmStatement,
                           AsmFunction | AsmBlock | AsmInstruction | AsmStaticData,
                           "AsmStatement", "AsmStatementTag", false);
     AsmStatement.setDataPrototype("rose_addr_t", "address", "= 0",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmStatement.setDataPrototype("std::string", "comment", "= \"\"",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);



     /*************************************************************************************************************************
      *                                         Binary Interpretations
      * Some file formats have more than one thing in them: PE files have a DOS and Windows executable both in the same file;
      * Apple OS X files can have multiple architectures represented in a single file, etc.  The interpretation is the root of
      * an AST that represents a single, coherent sub-part of the file.
      *************************************************************************************************************************/

     // The "dwarf_info" member is first as an optimization: a single AST traversal will see the dwarf information first and
     // therefore have the necessary debugging information already available when the traversal gets to the rest of the AST.
     NEW_TERMINAL_MACRO(AsmInterpretationList, "AsmInterpretationList", "AsmInterpretationListTag");
     AsmInterpretationList.setDataPrototype("SgAsmInterpretationPtrList", "interpretations", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmInterpretation, "AsmInterpretation", "AsmInterpretationTag");
     AsmInterpretation.setFunctionPrototype("HEADER_INTERPRETATION", "../Grammar/BinaryInstruction.code");
     AsmInterpretation.setPredeclarationString("HEADER_INTERPRETATION_PREDECLARATION", "../Grammar/BinaryInstruction.code");
     AsmInterpretation.setAutomaticGenerationOfConstructor(false);
     AsmInterpretation.setDataPrototype("SgAsmDwarfCompilationUnitList*", "dwarf_info", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmInterpretation.setDataPrototype("SgAsmGenericHeaderList*", "headers", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmInterpretation.setDataPrototype("SgAsmBlock*", "global_block", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF File Header
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmElfFileHeader, "AsmElfFileHeader", "AsmElfFileHeaderTag");
     AsmElfFileHeader.setFunctionPrototype("HEADER_ELF_HEADER", "../Grammar/BinaryInstruction.code");
     AsmElfFileHeader.setFunctionSource("SOURCE_ELF_HEADER", "../Grammar/BinaryInstruction.code");
     AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_file_class", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_data_encoding", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_file_version", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgUnsignedCharList", "e_ident_padding", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "e_type", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "e_machine", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "e_flags", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "e_ehsize", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "phextrasz", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "e_phnum", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "shextrasz", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "e_shnum", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long", "e_shstrndx", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgAsmElfSectionTable*", "section_table", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgAsmElfSegmentTable*", "segment_table", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF Section Tables
      * The ELF Section Table is itself a section.  The entries of the table are stored with the section they describe rather
      * than storing them all in the SgAsmSectionTable node.  We can reconstruct the ELF Section Table since sections have
      * unique ID numbers that are their original indices in the ELF Section Table.
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmElfSectionTable, "AsmElfSectionTable", "AsmElfSectionTableTag");
     AsmElfSectionTable.setFunctionPrototype("HEADER_ELF_SECTION_TABLE", "../Grammar/BinaryInstruction.code");



     NEW_TERMINAL_MACRO(AsmElfSectionTableEntry, "AsmElfSectionTableEntry", "AsmElfSectionTableEntryTag");
     AsmElfSectionTableEntry.setFunctionPrototype("HEADER_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTableEntry.setFunctionSource("SOURCE_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTableEntry.setDataPrototype("unsigned", "sh_name", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgAsmElfSectionTableEntry::SectionType", "sh_type", "= SHT_PROGBITS",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long", "sh_link", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long", "sh_info", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("uint64_t", "sh_flags", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_addr", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_offset", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_size", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_addralign", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_entsize", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF Segment Tables
      * The ELF Segment Table is an ELF Section that has entries describing the various segments of the ELF file.  Each segment
      * is also an SgAsmElfSection and the entries of the ELF Segment Table are associated with the SgAsmElfSection they
      * describe.  The ELF Segment Table can be reconstructed by traversing the AST and finding the SgAsmElfSegmentTableEntry
      * nodes.
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmElfSegmentTable, "AsmElfSegmentTable", "AsmElfSegmentTableTag");
     AsmElfSegmentTable.setFunctionPrototype("HEADER_ELF_SEGMENT_TABLE", "../Grammar/BinaryInstruction.code");



     NEW_TERMINAL_MACRO(AsmElfSegmentTableEntryList, "AsmElfSegmentTableEntryList", "AsmElfSegmentTableEntryListTag");
     AsmElfSegmentTableEntryList.setDataPrototype("SgAsmElfSegmentTableEntryPtrList", "entries", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSegmentTableEntry, "AsmElfSegmentTableEntry", "AsmElfSegmentTableEntryTag");
     AsmElfSegmentTableEntry.setFunctionPrototype("HEADER_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTableEntry.setFunctionSource("SOURCE_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTableEntry.setDataPrototype("size_t", "index", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentType", "type",
                                              "= SgAsmElfSegmentTableEntry::PT_LOAD",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentFlags", "flags",
                                              "= SgAsmElfSegmentTableEntry::PF_NONE",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "offset", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "vaddr", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "paddr", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "filesz", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "memsz", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "align", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF Symbol Tables
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmElfSymbolSection, "AsmElfSymbolSection", "AsmElfSymbolSectionTag");
     AsmElfSymbolSection.setFunctionPrototype("HEADER_ELF_SYMBOL_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolSection.setDataPrototype("SgAsmElfSymbolList*", "symbols", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmElfSymbolSection.setDataPrototype("bool", "is_dynamic", "= false",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymbolList, "AsmElfSymbolList", "AsmElfSymbolListTag");
     AsmElfSymbolList.setDataPrototype("SgAsmElfSymbolPtrList", "symbols", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymbol, "AsmElfSymbol", "AsmElfSymbolTag");
     AsmElfSymbol.setFunctionPrototype("HEADER_ELF_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmElfSymbol.setDataPrototype("unsigned char", "st_info", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned char", "st_res1", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned", "st_shndx", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("rose_addr_t", "st_size", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("SgUnsignedCharList", "extra", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF Symbol Version Tables
      *************************************************************************************************************************/

     // The ELF Symbol Version Table (.gnu.version section).  The section points to an SgAsmElfSymverEntryPtrList containing a
     // list of SgAsmElfSymverEntry objects.
     NEW_TERMINAL_MACRO(AsmElfSymverSection, "AsmElfSymverSection", "AsmElfSymverSection");
     AsmElfSymverSection.setFunctionPrototype("HEADER_ELF_SYMVER_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymverSection.setDataPrototype("SgAsmElfSymverEntryList*", "entries", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverEntry, "AsmElfSymverEntry", "AsmElfSymverEntryTag");
     AsmElfSymverEntry.setFunctionPrototype("HEADER_ELF_SYMVER_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSymverEntry.setDataPrototype("size_t", "value", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverEntryList, "AsmElfSymverEntryList", "AsmElfSymverEntryListTag");
     AsmElfSymverEntryList.setDataPrototype("SgAsmElfSymverEntryPtrList", "entries", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /* The GNU Symbol Version Definitions (.gnu.version_d section).  The SgAsmElfSymverDefinedSection points to a list of
      * entries (SgAsmSymverDefinedEntry), which in turn point to a list of auxilliary members (SgAsmSymverDefinedAux). See
      * SgAsmSymverDefinedSection::parse() for a good description of the disk format. */
     NEW_TERMINAL_MACRO(AsmElfSymverDefinedSection, "AsmElfSymverDefinedSection", "AsmElfSymverDefinedSection");
     AsmElfSymverDefinedSection.setFunctionPrototype("HEADER_ELF_SYMVER_DEFINED_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymverDefinedSection.setDataPrototype("SgAsmElfSymverDefinedEntryList*", "entries", "= NULL",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverDefinedEntryList, "AsmElfSymverDefinedEntryList", "AsmElfSymverDefinedEntryListTag");
     AsmElfSymverDefinedEntryList.setDataPrototype("SgAsmElfSymverDefinedEntryPtrList", "entries", "",
                                                   NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                   NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverDefinedEntry, "AsmElfSymverDefinedEntry", "AsmElfSymverDefinedEntryTag");
     AsmElfSymverDefinedEntry.setFunctionPrototype("HEADER_ELF_SYMVER_DEFINED_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSymverDefinedEntry.setDataPrototype("size_t", "version", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverDefinedEntry.setDataPrototype("int", "flags", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverDefinedEntry.setDataPrototype("size_t", "index", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverDefinedEntry.setDataPrototype("uint32_t", "hash", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverDefinedEntry.setDataPrototype("SgAsmElfSymverDefinedAuxList*", "entries", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverDefinedAuxList, "AsmElfSymverDefinedAuxList", "AsmElfSymverDefinedAuxListTag");
     AsmElfSymverDefinedAuxList.setDataPrototype("SgAsmElfSymverDefinedAuxPtrList", "entries", "",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverDefinedAux, "AsmElfSymverDefinedAux", "AsmElfSymverDefinedAuxTag");
     AsmElfSymverDefinedAux.setFunctionPrototype("HEADER_ELF_SYMVER_DEFINED_AUX", "../Grammar/BinaryInstruction.code");
     AsmElfSymverDefinedAux.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /* The GNU Symbol Version Requirements Table (.gnu.version_r section).  The format of this table is similar to the GNU
      * Symbol Version Definitions Table described above, namey that the table object (SgAsmElfSymverNeededSection) points to
      * a list of entries (SgAsmElfSymverNeededEntry), which of which point to a list of auxilliary information
      * (SgAsmElfSymverNeededAux). */
     NEW_TERMINAL_MACRO(AsmElfSymverNeededSection, "AsmElfSymverNeededSection", "AsmElfSymverNeededSection");
     AsmElfSymverNeededSection.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymverNeededSection.setDataPrototype("SgAsmElfSymverNeededEntryList*", "entries", "= NULL",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverNeededEntryList, "AsmElfSymverNeededEntryList", "AsmElfSymverNeededEntryListTag");
     AsmElfSymverNeededEntryList.setDataPrototype("SgAsmElfSymverNeededEntryPtrList", "entries", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverNeededEntry, "AsmElfSymverNeededEntry", "AsmElfSymverNeededEntryTag");
     AsmElfSymverNeededEntry.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSymverNeededEntry.setDataPrototype("size_t", "version", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededEntry.setDataPrototype("SgAsmGenericString*", "file_name", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededEntry.setDataPrototype("SgAsmElfSymverNeededAuxList*", "entries", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverNeededAuxList, "AsmElfSymverNeededAuxList", "AsmElfSymverNeededAuxListTag");
     AsmElfSymverNeededAuxList.setDataPrototype("SgAsmElfSymverNeededAuxPtrList", "entries", "",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfSymverNeededAux, "AsmElfSymverNeededAux", "AsmElfSymverNeededAuxTag");
     AsmElfSymverNeededAux.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_AUX", "../Grammar/BinaryInstruction.code");
     AsmElfSymverNeededAux.setDataPrototype("uint32_t", "hash", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededAux.setDataPrototype("int", "flags", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededAux.setDataPrototype("size_t", "other", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededAux.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF Relocation Tables
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmElfRelocSection, "AsmElfRelocSection", "AsmElfRelocSectionTag");
     AsmElfRelocSection.setFunctionPrototype("HEADER_ELF_RELOC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfRelocSection.setDataPrototype("bool", "uses_addend", "= true",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocSection.setDataPrototype("SgAsmElfSection*", "target_section", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocSection.setDataPrototype("SgAsmElfRelocEntryList*", "entries", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfRelocEntryList, "AsmElfRelocEntryList", "AsmElfRelocEntryListTag");
     AsmElfRelocEntryList.setDataPrototype("SgAsmElfRelocEntryPtrList", "entries", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfRelocEntry, "AsmElfRelocEntry", "AsmElfRelocEntryTag");
     AsmElfRelocEntry.setFunctionPrototype("HEADER_ELF_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfRelocEntry.setDataPrototype("rose_addr_t", "r_offset", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocEntry.setDataPrototype("rose_addr_t", "r_addend", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocEntry.setDataPrototype("unsigned long", "sym", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocEntry.setDataPrototype("SgAsmElfRelocEntry::RelocType", "type", "= R_386_NONE",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF Dynamic Linking
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmElfDynamicSection, "AsmElfDynamicSection", "AsmElfDynamicSectionTag");
     AsmElfDynamicSection.setFunctionPrototype("HEADER_ELF_DYNAMIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicSection.setDataPrototype("SgAsmElfDynamicEntryList*", "entries", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfDynamicEntryList, "AsmElfDynamicEntryList", "AsmElfDynamicEntryListTag");
     AsmElfDynamicEntryList.setDataPrototype("SgAsmElfDynamicEntryPtrList", "entries", "",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfDynamicEntry, "AsmElfDynamicEntry", "AsmElfDynamicEntryTag");
     AsmElfDynamicEntry.setFunctionPrototype("HEADER_ELF_DYNAMIC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicEntry.setDataPrototype("SgAsmElfDynamicEntry::EntryType", "d_tag", "= SgAsmElfDynamicEntry::DT_NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicEntry.setDataPrototype("rose_rva_t", "d_val", "",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmElfDynamicEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF String Tables
      *************************************************************************************************************************/

     // String table section
     NEW_TERMINAL_MACRO(AsmElfStringSection, "AsmElfStringSection", "AsmElfStringSectionTag");
     AsmElfStringSection.setFunctionPrototype("HEADER_ELF_STRING_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfStringSection.setFunctionSource("SOURCE_ELF_STRING_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfStringSection.setAutomaticGenerationOfDestructor(false);
     AsmElfStringSection.setDataPrototype("SgAsmElfStrtab*", "strtab", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfStrtab, "AsmElfStrtab", "AsmElfStrtabTag");
     AsmElfStrtab.setFunctionPrototype("HEADER_ELF_STRING_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfStrtab.setAutomaticGenerationOfDestructor(false);



     /*************************************************************************************************************************
      *                                         ELF Notes
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmElfNoteSection, "AsmElfNoteSection", "AsmElfNoteSection");
     AsmElfNoteSection.setFunctionPrototype("HEADER_ELF_NOTE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfNoteSection.setDataPrototype("SgAsmElfNoteEntryList*", "entries", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfNoteEntryList, "AsmElfNoteEntryList", "AsmElfNoteEntryListTag");
     AsmElfNoteEntryList.setDataPrototype("SgAsmElfNoteEntryPtrList", "entries", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfNoteEntry, "AsmElfNoteEntry", "AsmElfNoteEntryTag");
     AsmElfNoteEntry.setFunctionPrototype("HEADER_ELF_NOTE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfNoteEntry.setDataPrototype("unsigned", "type", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfNoteEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmElfNoteEntry.setDataPrototype("SgUnsignedCharList", "payload", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF Exception Handling
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmElfEHFrameSection, "AsmElfEHFrameSection", "AsmElfEHFrameSection");
     AsmElfEHFrameSection.setFunctionPrototype("HEADER_ELF_EH_FRAME_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfEHFrameSection.setDataPrototype("SgAsmElfEHFrameEntryCIList*", "ci_entries", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfEHFrameEntryCIList, "AsmElfEHFrameEntryCIList", "AsmElfEHFrameEntryCIListTag");
     AsmElfEHFrameEntryCIList.setDataPrototype("SgAsmElfEHFrameEntryCIPtrList", "entries", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     // ELF Error Handling Frame Entry, Common Information Entry Format
     NEW_TERMINAL_MACRO(AsmElfEHFrameEntryCI, "AsmElfEHFrameEntryCI", "AsmElfEHFrameEntryCITag");
     AsmElfEHFrameEntryCI.setFunctionPrototype("HEADER_ELF_EH_FRAME_ENTRY_CI", "../Grammar/BinaryInstruction.code");
     AsmElfEHFrameEntryCI.setDataPrototype("int", "version", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("std::string", "augmentation_string", "= \"\"",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("uint64_t", "code_alignment_factor", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("int64_t", "data_alignment_factor", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("uint64_t", "augmentation_data_length", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("int", "lsda_encoding", "= -1",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("int", "prh_encoding", "= -1",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("unsigned", "prh_arg", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("rose_addr_t", "prh_addr", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("int", "addr_encoding", "= -1",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("bool", "sig_frame", "= false",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("SgUnsignedCharList", "instructions", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryCI.setDataPrototype("SgAsmElfEHFrameEntryFDList*", "fd_entries", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmElfEHFrameEntryFDList, "AsmElfEHFrameEntryFDList", "AsmElfEHFrameEntryFDListTag");
     AsmElfEHFrameEntryFDList.setDataPrototype("SgAsmElfEHFrameEntryFDPtrList", "entries", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /* ELF Error Handling Frame Entry, Frame Description Entry Format */
     NEW_TERMINAL_MACRO(AsmElfEHFrameEntryFD, "AsmElfEHFrameEntryFD", "AsmElfEHFrameEntryFDTag");
     AsmElfEHFrameEntryFD.setFunctionPrototype("HEADER_ELF_EH_FRAME_ENTRY_FD", "../Grammar/BinaryInstruction.code");
     AsmElfEHFrameEntryFD.setDataPrototype("rose_rva_t", "begin_rva", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryFD.setDataPrototype("rose_addr_t", "size", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryFD.setDataPrototype("SgUnsignedCharList", "augmentation_data", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryFD.setDataPrototype("SgUnsignedCharList", "instructions", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         ELF Sections
      *************************************************************************************************************************/

     NEW_NONTERMINAL_MACRO(AsmElfSection,
                           AsmElfSymbolSection | AsmElfRelocSection         | AsmElfDynamicSection       |
                           AsmElfStringSection | AsmElfNoteSection          | AsmElfEHFrameSection       |
                           AsmElfSymverSection | AsmElfSymverDefinedSection | AsmElfSymverNeededSection,
                           "AsmElfSection", "AsmElfSectionTag", true);
     AsmElfSection.setFunctionPrototype("HEADER_ELF_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSection.setFunctionSource("SOURCE_ELF_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSection.setDataPrototype("SgAsmElfSection*", "linked_section", "= NULL", //accessors must be virtual
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSection.setDataPrototype("SgAsmElfSectionTableEntry*", "section_entry", "= NULL",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmElfSection.setDataPrototype("SgAsmElfSegmentTableEntry*", "segment_entry", "= NULL",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         DOS File Header
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmDOSFileHeader, "AsmDOSFileHeader", "AsmDOSFileHeaderTag");
     AsmDOSFileHeader.setFunctionPrototype("HEADER_DOS_HEADER", "../Grammar/BinaryInstruction.code");
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_last_page_size", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_total_pages", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_nrelocs", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_header_paragraphs", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_minalloc", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_maxalloc", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_ss", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_sp", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_cksum", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_ip", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_cs", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t", "e_overlay", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("rose_addr_t", "e_relocs_offset", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned", "e_res1", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*", "relocs", "= NULL", //relocation info
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*", "rm_section", "= NULL", //real-mode section
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDOSExtendedHeader, "AsmDOSExtendedHeader", "AsmDOSExtendedHeaderTag");
     AsmDOSExtendedHeader.setFunctionPrototype("HEADER_DOS_EXTENDED_HEADER", "../Grammar/BinaryInstruction.code");
     AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res1", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_oemid", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_oeminfo", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res2", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res3", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res4", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res5", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res6", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("rose_addr_t", "e_lfanew", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         PE File Header
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmPEFileHeader, "AsmPEFileHeader", "AsmPEFileHeaderTag");
     AsmPEFileHeader.setFunctionPrototype("HEADER_PE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmPEFileHeader.setDataPrototype("unsigned", "e_cpu_type", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_nsections", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_time", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("rose_addr_t", "e_coff_symtab", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("rose_addr_t", "e_nt_hdr_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_coff_nsyms", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_flags", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_opt_magic", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_lmajor", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_lminor", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_code_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_data_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_bss_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("rose_rva_t", "e_code_rva", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("rose_rva_t", "e_data_rva", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_section_align", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_file_align", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_os_major", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_os_minor", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_user_major", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_user_minor", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_subsys_major", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_subsys_minor", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_reserved9", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_image_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_header_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_file_checksum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_subsystem", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_dll_flags", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_stack_reserve_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_stack_commit_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_heap_reserve_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_heap_commit_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_loader_flags", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned", "e_num_rvasize_pairs", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmPERVASizePairList*", "rvasize_pairs", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmPESectionTable*", "section_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmCoffSymbolTable*", "coff_symtab", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmPERVASizePairList, "AsmPERVASizePairList", "AsmPERVASizePairListTag");
     AsmPERVASizePairList.setFunctionPrototype("HEADER_PE_RVA_SIZE_PAIR_LIST", "../Grammar/BinaryInstruction.code");
     AsmPERVASizePairList.setDataPrototype("SgAsmPERVASizePairPtrList", "pairs", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmPERVASizePair, "AsmPERVASizePair", "AsmPERVASizePairTag");
     AsmPERVASizePair.setFunctionPrototype("HEADER_PE_RVA_SIZE_PAIR", "../Grammar/BinaryInstruction.code");
     AsmPERVASizePair.setDataPrototype("rose_rva_t", "e_rva", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPERVASizePair.setDataPrototype("rose_addr_t", "e_size", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPERVASizePair.setDataPrototype("SgAsmGenericSection*", "section", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         PE Import Tables
      *************************************************************************************************************************/

     // A single imported object.  Each of these nodes is created from a combination of the PE Import Lookup Table and the PE
     // Import Address Table, pointers to which are stored in the Import Directory (SgAsmPEImportDirectory).
     NEW_TERMINAL_MACRO(AsmPEImportItem, "AsmPEImportItem", "AsmPEImportItemTag");
     AsmPEImportItem.setFunctionPrototype("HEADER_PE_IMPORT_ITEM", "../Grammar/BinaryInstruction.code");
     AsmPEImportItem.setDataPrototype("bool", "by_ordinal", "=true",      // store ordinal number if ILT/IAT?
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportItem.setDataPrototype("unsigned", "ordinal", "=0",        // ordinal number
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportItem.setDataPrototype("unsigned", "hint", "=0",           // export name table hint if not by_ordinal
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportItem.setDataPrototype("SgAsmGenericString*", "name", "",  // name if not by ordinal
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEImportItem.setDataPrototype("rose_rva_t", "hintname_rva", "=0", // rva of hint/name pair
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportItem.setDataPrototype("size_t", "hintname_nalloc", "=0",  // file bytes allocated for hint/name pair
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportItem.setDataPrototype("rose_rva_t", "bound_rva", "=0",    // bound rva from IAT if not a copy of ILT
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     // A list of imported items.
     NEW_TERMINAL_MACRO(AsmPEImportItemList, "AsmPEImportItemList", "AsmPEImportItemListTag");
     AsmPEImportItemList.setDataPrototype("SgAsmPEImportItemPtrList", "vector", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     // One import directory per library.  The import directory points to the Import Lookup Table and Import Address Table.
     // ROSE no longer represents the ILT and IAT with separate IR nodes.
     NEW_TERMINAL_MACRO(AsmPEImportDirectory, "AsmPEImportDirectory", "AsmPEImportDirectoryTag");
     AsmPEImportDirectory.setFunctionPrototype("HEADER_PE_IMPORT_DIRECTORY", "../Grammar/BinaryInstruction.code");
     AsmPEImportDirectory.setDataPrototype("SgAsmGenericString*", "dll_name", "=0", // name of library from which to import
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("rose_rva_t", "dll_name_rva", "= 0", // address of the name
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("size_t", "dll_name_nalloc", "=0",   // bytes allocated in the file for the name
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("time_t", "time", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("unsigned", "forwarder_chain", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("rose_rva_t", "ilt_rva", "= 0", // starting address of the ILT
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("size_t", "ilt_nalloc", "= 0",  // bytes allocated in the file for the ILT
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("rose_rva_t", "iat_rva", "= 0", // usually in the .rdata section
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("size_t", "iat_nalloc", "=0",   // bytes allocated in the file for the IAT
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("SgAsmPEImportItemList*", "imports", "=NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


     // A list of PE Import Directories.
     NEW_TERMINAL_MACRO(AsmPEImportDirectoryList, "AsmPEImportDirectoryList", "AsmPEImportDirectoryListTag");
     AsmPEImportDirectoryList.setDataPrototype("SgAsmPEImportDirectoryPtrList", "vector", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     // A file section containing a list of PE Import Directories.  Documentation is in PeImportSection.C */
     NEW_TERMINAL_MACRO(AsmPEImportSection, "AsmPEImportSection", "AsmPEImportSectionTag");
     AsmPEImportSection.setFunctionPrototype("HEADER_PE_IMPORT_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPEImportSection.setDataPrototype ("SgAsmPEImportDirectoryList*", "import_directories", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         PE Export Tables
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmPEExportSection, "AsmPEExportSection", "AsmPEExportSectionTag");
     AsmPEExportSection.setFunctionPrototype("HEADER_PE_EXPORT_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPEExportSection.setDataPrototype("SgAsmPEExportDirectory*", "export_dir", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEExportSection.setDataPrototype("SgAsmPEExportEntryList*", "exports", "",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmPEExportDirectory, "AsmPEExportDirectory", "AsmPEExportDirectoryTag");
     AsmPEExportDirectory.setFunctionPrototype("HEADER_PE_EXPORT_DIRECTORY", "../Grammar/BinaryInstruction.code");
     AsmPEExportDirectory.setDataPrototype("unsigned", "res1", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("time_t", "timestamp", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("unsigned", "vmajor", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("unsigned", "vminor", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("rose_rva_t", "name_rva", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("unsigned", "ord_base", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("size_t", "expaddr_n", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("size_t", "nameptr_n", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("rose_rva_t", "expaddr_rva", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("rose_rva_t", "nameptr_rva", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("rose_rva_t", "ordinals_rva", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportDirectory.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmPEExportEntryList, "AsmPEExportEntryList", "AsmPEExportEntryListTag");
     AsmPEExportEntryList.setDataPrototype("SgAsmPEExportEntryPtrList", "exports", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmPEExportEntry, "AsmPEExportEntry", "AsmPEExportEntryTag");
     AsmPEExportEntry.setFunctionPrototype("HEADER_PE_EXPORT_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmPEExportEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEExportEntry.setDataPrototype("unsigned", "ordinal", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportEntry.setDataPrototype("rose_rva_t", "export_rva", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportEntry.setDataPrototype("SgAsmGenericString*", "forwarder", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         PE String Tables
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmPEStringSection, "AsmPEStringSection", "AsmPEStringSectionTag");
     AsmPEStringSection.setFunctionPrototype("HEADER_PE_STRING_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPEStringSection.setAutomaticGenerationOfDestructor(false);
     AsmPEStringSection.setDataPrototype("SgAsmCoffStrtab*", "strtab", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         PE Section Tables
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmPESectionTable, "AsmPESectionTable", "AsmPESectionTableTag");
     AsmPESectionTable.setFunctionPrototype("HEADER_PE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");



     NEW_TERMINAL_MACRO(AsmPESectionTableEntry, "AsmPESectionTableEntry", "AsmPESectionTableEntryTag");
     AsmPESectionTableEntry.setFunctionPrototype("HEADER_PE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmPESectionTableEntry.setFunctionSource("SOURCE_PE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmPESectionTableEntry.setDataPrototype("std::string", "name", "= \"\"",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "virtual_size", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "rva", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "physical_size", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "physical_offset", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned", "coff_line_nums", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned", "n_relocs", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned", "n_coff_line_nums", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         PE Sections
      *************************************************************************************************************************/

     NEW_NONTERMINAL_MACRO(AsmPESection,
                           AsmPEImportSection | AsmPEExportSection | AsmPEStringSection,
                           "AsmPESection", "AsmPESectionTag", true);
     AsmPESection.setFunctionPrototype("HEADER_PE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPESection.setDataPrototype("SgAsmPESectionTableEntry*", "section_entry", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         COFF Symbol Tables
      * The COFF format is used by Microsoft PE files.
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmCoffSymbolTable, "AsmCoffSymbolTable", "AsmCoffSymbolTableTag");
     AsmCoffSymbolTable.setFunctionPrototype("HEADER_PE_COFF_SYMBOL_TABLE", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbolTable.setDataPrototype("SgAsmGenericSection*", "strtab", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbolTable.setDataPrototype("SgAsmCoffSymbolList*", "symbols", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmCoffSymbolList, "AsmCoffSymbolList", "AsmCoffSymbolListTag");
     AsmCoffSymbolList.setDataPrototype("SgAsmCoffSymbolPtrList", "symbols", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmCoffSymbol, "AsmCoffSymbol", "AsmCoffSymbolTag");
     AsmCoffSymbol.setFunctionPrototype("HEADER_PE_COFF_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbol.setFunctionSource("SOURCE_PE_COFF_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbol.setDataPrototype("std::string", "st_name", "= \"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("rose_addr_t", "st_name_offset", "= 0",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("int", "st_section_num", "= 0",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("unsigned", "st_type", "= 0",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("unsigned", "st_storage_class", "= 0",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("unsigned", "st_num_aux_entries", "= 0",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("SgUnsignedCharList", "aux_data", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmCoffStrtab, "AsmCoffStrtab", "AsmCoffStrtabTag");
     AsmCoffStrtab.setFunctionPrototype("HEADER_COFF_STRING_TABLE", "../Grammar/BinaryInstruction.code");
     AsmCoffStrtab.setAutomaticGenerationOfDestructor(false);



     /*************************************************************************************************************************
      *                                         NE File Header
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmNEFileHeader, "AsmNEFileHeader", "AsmNEFileHeaderTag");
     AsmNEFileHeader.setFunctionPrototype("HEADER_NE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmNEFileHeader.setDataPrototype("unsigned", "e_linker_major", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_linker_minor", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_checksum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_flags1", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_autodata_sn", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_bss_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_stack_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_csip", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_sssp", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_nsections", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_nmodrefs", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_nnonresnames", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_nmovable_entries", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_sector_align", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_nresources", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_exetype", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_flags2", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_res1", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned", "e_winvers", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_entrytab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_entrytab_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_sectab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_rsrctab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_resnametab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_modreftab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_importnametab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_nonresnametab_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_fastload_sector", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_fastload_nsectors", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmDOSExtendedHeader*", "dos2_header", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNESectionTable*", "section_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNENameTable*", "resname_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNENameTable*", "nonresname_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNEModuleTable*", "module_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNEEntryTable*", "entry_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmNEEntryTable, "AsmNEEntryTable", "AsmNEEntryTableTag");
     AsmNEEntryTable.setFunctionPrototype("HEADER_NE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEEntryTable.setDataPrototype("SgSizeTList", "bundle_sizes", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEEntryTable.setDataPrototype("SgAsmNEEntryPointPtrList", "entries", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmNEEntryPoint, "AsmNEEntryPoint", "AsmNEEntryPointTag");
     AsmNEEntryPoint.setFunctionPrototype("HEADER_NE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmNEEntryPoint.setFunctionSource("SOURCE_NE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmNEEntryPoint.setDataPrototype("SgAsmNEEntryPoint::NEEntryFlags", "flags", "= SgAsmNEEntryPoint::EF_ZERO",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEEntryPoint.setDataPrototype("unsigned", "int3f", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEEntryPoint.setDataPrototype("unsigned", "section_idx", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEEntryPoint.setDataPrototype("unsigned", "section_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         NE Section Table
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmNESectionTable, "AsmNESectionTable", "AsmNESectionTableTag");
     AsmNESectionTable.setFunctionPrototype("HEADER_NE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNESectionTable.setDataPrototype("unsigned", "flags", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("unsigned", "sector", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("rose_addr_t", "physical_size", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("rose_addr_t", "virtual_size", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmNESectionTableEntry, "AsmNESectionTableEntry", "AsmNESectionTableEntryTag");
     AsmNESectionTableEntry.setFunctionPrototype("HEADER_NE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmNESectionTableEntry.setFunctionSource("SOURCE_NE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmNESectionTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTableEntry.setDataPrototype("unsigned", "sector", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTableEntry.setDataPrototype("rose_addr_t", "physical_size", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTableEntry.setDataPrototype("rose_addr_t", "virtual_size", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         NE Relocation Table
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmNERelocTable, "AsmNERelocTable", "AsmNERelocTableTag");
     AsmNERelocTable.setFunctionPrototype("HEADER_NE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNERelocTable.setDataPrototype("SgAsmNERelocEntryPtrList", "entries", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmNERelocEntry, "AsmNERelocEntry", "AsmNERelocEntryTag");
     AsmNERelocEntry.setFunctionPrototype("HEADER_NE_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmNERelocEntry.setFunctionSource("SOURCE_NE_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocSrcType", "src_type", "= SgAsmNERelocEntry::RF_SRCTYPE_8OFF",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocModifiers", "modifier", "= SgAsmNERelocEntry::RF_MODIFIER_SINGLE",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocTgtType", "tgt_type", "= SgAsmNERelocEntry::RF_TGTTYPE_IREF",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocFlags", "flags", "= SgAsmNERelocEntry::RF_ADDITIVE",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("rose_addr_t", "src_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iref_type", "iref", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iord_type", "iord", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iname_type", "iname", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::osfixup_type", "osfixup", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         NE Sections
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmNENameTable, "AsmNENameTable", "AsmNENameTableTag");
     AsmNENameTable.setFunctionPrototype("HEADER_NE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNENameTable.setDataPrototype("SgStringList", "names", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNENameTable.setDataPrototype("SgUnsignedList", "ordinals", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmNEModuleTable, "AsmNEModuleTable", "AsmNEModuleTableTag");
     AsmNEModuleTable.setFunctionPrototype("HEADER_NE_MODULE_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEModuleTable.setDataPrototype("SgAsmNEStringTable*", "strtab", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEModuleTable.setDataPrototype("SgAddressList", "name_offsets", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEModuleTable.setDataPrototype("SgStringList", "names", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmNEStringTable, "AsmNEStringTable", "AsmNEStringTableTag");
     AsmNEStringTable.setFunctionPrototype("HEADER_NE_STRING_TABLE", "../Grammar/BinaryInstruction.code");



     NEW_TERMINAL_MACRO(AsmNESection, "AsmNESection", "AsmNESectionTag");
     AsmNESection.setFunctionPrototype("HEADER_NE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmNESection.setDataPrototype("SgAsmNESectionTableEntry*", "st_entry", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNESection.setDataPrototype("SgAsmNERelocTable*", "reloc_table", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         LE File Header
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmLEFileHeader, "AsmLEFileHeader", "AsmLEFileHeaderTag");
     AsmLEFileHeader.setFunctionPrototype("HEADER_LE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmLEFileHeader.setDataPrototype("unsigned", "e_byte_order", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_word_order", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_format_level", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_cpu_type", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_os_type", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_module_version", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_flags", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_eip_section", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_esp_section", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_last_page_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_page_offset_shift", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_fixup_sect_cksum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_loader_sect_cksum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_secttab_nentries", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_rsrctab_nentries", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_fmtdirtab_nentries", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_import_modtab_nentries", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_preload_npages", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_nonresnametab_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_nonresnametab_cksum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_auto_ds_section", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_debug_info_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_num_instance_preload", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_num_instance_demand", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned", "e_heap_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_npages", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_eip", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_esp", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_page_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_fixup_sect_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_loader_sect_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_secttab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_pagetab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_iterpages_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_rsrctab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_resnametab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_entrytab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_fmtdirtab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_fixup_pagetab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_fixup_rectab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_import_modtab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_import_proctab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_ppcksumtab_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_data_pages_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_nonresnametab_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_debug_info_rfo", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmDOSExtendedHeader*", "dos2_header", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLESectionTable*", "section_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLEPageTable*", "page_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLENameTable*", "resname_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLENameTable*", "nonresname_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLEEntryTable*", "entry_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLERelocTable*", "reloc_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmLEEntryTable, "AsmLEEntryTable", "AsmLEEntryTableTag");
     AsmLEEntryTable.setFunctionPrototype("HEADER_LE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLEEntryTable.setDataPrototype("SgSizeTList", "bundle_sizes", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryTable.setDataPrototype("SgAsmLEEntryPointPtrList", "entries", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmLEEntryPoint, "AsmLEEntryPoint", "AsmLEEntryPointTag");
     AsmLEEntryPoint.setFunctionPrototype("HEADER_LE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmLEEntryPoint.setFunctionSource("SOURCE_LE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmLEEntryPoint.setDataPrototype("SgAsmLEEntryPointPtrList", "entries", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned", "flags", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned", "objnum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned", "entry_type", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned", "res1", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("rose_addr_t", "entry_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         LE Section Table
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmLESectionTable, "AsmLESectionTable", "AsmLESectionTableTag");
     AsmLESectionTable.setFunctionPrototype("HEADER_LE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");



     NEW_TERMINAL_MACRO(AsmLESectionTableEntry, "AsmLESectionTableEntry", "AsmLESectionTableEntryTag");
     AsmLESectionTableEntry.setFunctionPrototype("HEADER_LE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmLESectionTableEntry.setFunctionSource("SOURCE_LE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmLESectionTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("unsigned", "pagemap_index", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("unsigned", "pagemap_nentries", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("unsigned", "res1", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("rose_addr_t", "mapped_size", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("rose_addr_t", "base_addr", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         LE Relocation Table
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmLERelocTable, "AsmLERelocTable", "AsmLERelocTableTag");
     AsmLERelocTable.setFunctionPrototype("HEADER_LE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLERelocTable.setDataPrototype("SgAsmLERelocEntryPtrList", "entries", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         LE Sections
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmLENameTable, "AsmLENameTable", "AsmLENameTableTag");
     AsmLENameTable.setFunctionPrototype("HEADER_LE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLENameTable.setDataPrototype("SgStringList", "names", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLENameTable.setDataPrototype("SgUnsignedList", "ordinals", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmLEPageTable, "AsmLEPageTable", "AsmLEPageTableTag");
     AsmLEPageTable.setFunctionPrototype("HEADER_LE_PAGE_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLEPageTable.setDataPrototype("SgAsmLEPageTableEntryPtrList", "entries", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmLEPageTableEntry, "AsmLEPageTableEntry", "AsmLEPageTableEntryTag");
     AsmLEPageTableEntry.setFunctionPrototype("HEADER_LE_PAGE_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmLEPageTableEntry.setFunctionSource("SOURCE_LE_PAGE_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmLEPageTableEntry.setDataPrototype("unsigned", "pageno", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEPageTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmLESection, "AsmLESection", "AsmLESectionTag");
     AsmLESection.setFunctionPrototype("HEADER_LE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmLESection.setDataPrototype("SgAsmLESectionTableEntry*", "st_entry", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         DWARF
      * These are a subset (but most of) the possible Dwarf Tags, I [DQ] have skipped some of the architecture specific tags
      * (except for GNU).  There are about 63 new IR nodes here. These are kinds of language constructs that are stored in the
      * ".debug_info" section when dwarf debug information is available (generated by the compiler) in the binary executable.
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmDwarfArrayType, "AsmDwarfArrayType", "AsmDwarfArrayTypeTag");
     AsmDwarfArrayType.setFunctionPrototype("HEADER_DWARF_ARRAY_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfArrayType.setFunctionSource("SOURCE_DWARF_ARRAY_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfArrayType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfClassType, "AsmDwarfClassType", "AsmDwarfClassTypeTag");
     AsmDwarfClassType.setFunctionPrototype("HEADER_DWARF_CLASS_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfClassType.setFunctionSource("SOURCE_DWARF_CLASS_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfClassType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfEntryPoint, "AsmDwarfEntryPoint", "AsmDwarfEntryPointTag");



     NEW_TERMINAL_MACRO(AsmDwarfEnumerationType, "AsmDwarfEnumerationType", "AsmDwarfEnumerationTypeTag");
     AsmDwarfEnumerationType.setFunctionPrototype("HEADER_DWARF_ENUMERATION_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfEnumerationType.setFunctionSource("SOURCE_DWARF_ENUMERATION_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfEnumerationType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfFormalParameter, "AsmDwarfFormalParameter", "AsmDwarfFormalParameterTag");



     NEW_TERMINAL_MACRO(AsmDwarfImportedDeclaration, "AsmDwarfImportedDeclaration", "AsmDwarfImportedDeclarationTag");



     NEW_TERMINAL_MACRO(AsmDwarfLabel, "AsmDwarfLabel", "AsmDwarfLabelTag");



     NEW_TERMINAL_MACRO(AsmDwarfLexicalBlock, "AsmDwarfLexicalBlock", "AsmDwarfLexicalBlockTag");
     AsmDwarfLexicalBlock.setFunctionPrototype("HEADER_DWARF_LEXICAL_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmDwarfLexicalBlock.setFunctionSource("SOURCE_DWARF_LEXICAL_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmDwarfLexicalBlock.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfMember, "AsmDwarfMember", "AsmDwarfMemberTag");



     NEW_TERMINAL_MACRO(AsmDwarfPointerType, "AsmDwarfPointerType", "AsmDwarfPointerTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfReferenceType, "AsmDwarfReferenceType", "AsmDwarfReferenceTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfCompilationUnit, "AsmDwarfCompilationUnit", "AsmDwarfCompilationUnitTag");
     AsmDwarfCompilationUnit.setFunctionPrototype("HEADER_DWARF_COMPILATION_UNIT", "../Grammar/BinaryInstruction.code");
     AsmDwarfCompilationUnit.setFunctionSource("SOURCE_DWARF_COMPILATION_UNIT", "../Grammar/BinaryInstruction.code");
     //AsmDwarfCompilationUnit.setDataPrototype("std::string", "name", "= \"\"",
     //                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("std::string", "producer", "= \"\"",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("std::string", "language", "= \"\"",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "low_pc", "= 0x0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "hi_pc", "= 0x0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("int", "version_stamp", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "abbrev_offset", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "address_size", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "offset_length", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfLineList*", "line_info", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfConstructList*", "language_constructs", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfMacroList*", "macro_info", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfStringType, "AsmDwarfStringType", "AsmDwarfStringTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfStructureType, "AsmDwarfStructureType", "AsmDwarfStructureTypeTag");
     AsmDwarfStructureType.setFunctionPrototype("HEADER_DWARF_STRUCTURE_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfStructureType.setFunctionSource("SOURCE_DWARF_STRUCTURE_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfStructureType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfSubroutineType, "AsmDwarfSubroutineType", "AsmDwarfSubroutineTypeTag");
     AsmDwarfSubroutineType.setFunctionPrototype("HEADER_DWARF_SUBROUTINE_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfSubroutineType.setFunctionSource("SOURCE_DWARF_SUBROUTINE_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfSubroutineType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfTypedef, "AsmDwarfTypedef", "AsmDwarfTypedefTag");



     NEW_TERMINAL_MACRO(AsmDwarfUnionType, "AsmDwarfUnionType", "AsmDwarfUnionTypeTag");
     AsmDwarfUnionType.setFunctionPrototype("HEADER_DWARF_UNION_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfUnionType.setFunctionSource("SOURCE_DWARF_UNION_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfUnionType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfUnspecifiedParameters, "AsmDwarfUnspecifiedParameters", "AsmDwarfUnspecifiedParametersTag");



     NEW_TERMINAL_MACRO(AsmDwarfVariant, "AsmDwarfVariant", "AsmDwarfVariantTag");



     NEW_TERMINAL_MACRO(AsmDwarfCommonBlock, "AsmDwarfCommonBlock", "AsmDwarfCommonBlockTag");
     AsmDwarfCommonBlock.setFunctionPrototype("HEADER_DWARF_COMMON_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmDwarfCommonBlock.setFunctionSource("SOURCE_DWARF_COMMON_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmDwarfCommonBlock.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfCommonInclusion, "AsmDwarfCommonInclusion", "AsmDwarfCommonInclusionTag");



     NEW_TERMINAL_MACRO(AsmDwarfInheritance, "AsmDwarfInheritance", "AsmDwarfInheritanceTag");



     NEW_TERMINAL_MACRO(AsmDwarfInlinedSubroutine, "AsmDwarfInlinedSubroutine", "AsmDwarfInlinedSubroutineTag");
     AsmDwarfInlinedSubroutine.setFunctionPrototype("HEADER_DWARF_INLINED_SUBROUTINE", "../Grammar/BinaryInstruction.code");
     AsmDwarfInlinedSubroutine.setFunctionSource("SOURCE_DWARF_INLINED_SUBROUTINE", "../Grammar/BinaryInstruction.code");
     AsmDwarfInlinedSubroutine.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfModule, "AsmDwarfModule", "AsmDwarfModuleTag");



     NEW_TERMINAL_MACRO(AsmDwarfPtrToMemberType, "AsmDwarfPtrToMemberType", "AsmDwarfPtrToMemberTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfSetType, "AsmDwarfSetType", "AsmDwarfSetTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfSubrangeType, "AsmDwarfSubrangeType", "AsmDwarfSubrangeTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfWithStmt, "AsmDwarfWithStmt", "AsmDwarfWithStmtTag");



     NEW_TERMINAL_MACRO(AsmDwarfAccessDeclaration, "AsmDwarfAccessDeclaration", "AsmDwarfAccessDeclarationTag");



     NEW_TERMINAL_MACRO(AsmDwarfBaseType, "AsmDwarfBaseType", "AsmDwarfBaseTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfCatchBlock, "AsmDwarfCatchBlock", "AsmDwarfCatchBlockTag");



     NEW_TERMINAL_MACRO(AsmDwarfConstType, "AsmDwarfConstType", "AsmDwarfConstTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfConstant, "AsmDwarfConstant", "AsmDwarfConstantTag");



     NEW_TERMINAL_MACRO(AsmDwarfEnumerator, "AsmDwarfEnumerator", "AsmDwarfEnumeratorTag");



     NEW_TERMINAL_MACRO(AsmDwarfFileType, "AsmDwarfFileType", "AsmDwarfFileTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfFriend, "AsmDwarfFriend", "AsmDwarfFriendTag");



     NEW_TERMINAL_MACRO(AsmDwarfNamelist, "AsmDwarfNamelist", "AsmDwarfNamelistTag");



     NEW_TERMINAL_MACRO(AsmDwarfNamelistItem, "AsmDwarfNamelistItem", "AsmDwarfNamelistItemTag");



     NEW_TERMINAL_MACRO(AsmDwarfPackedType, "AsmDwarfPackedType", "AsmDwarfPackedTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfSubprogram, "AsmDwarfSubprogram", "AsmDwarfSubprogramTag");
     AsmDwarfSubprogram.setFunctionPrototype("HEADER_DWARF_SUBPROGRAM", "../Grammar/BinaryInstruction.code");
     AsmDwarfSubprogram.setFunctionSource("SOURCE_DWARF_SUBPROGRAM", "../Grammar/BinaryInstruction.code");
     AsmDwarfSubprogram.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfTemplateTypeParameter, "AsmDwarfTemplateTypeParameter", "AsmDwarfTemplateTypeParameterTag");



     NEW_TERMINAL_MACRO(AsmDwarfTemplateValueParameter, "AsmDwarfTemplateValueParameter", "AsmDwarfTemplateValueParameterTag");



     NEW_TERMINAL_MACRO(AsmDwarfThrownType, "AsmDwarfThrownType", "AsmDwarfThrownTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfTryBlock, "AsmDwarfTryBlock", "AsmDwarfTryBlockTag");



     NEW_TERMINAL_MACRO(AsmDwarfVariantPart, "AsmDwarfVariantPart", "AsmDwarfVariantPartTag");



     NEW_TERMINAL_MACRO(AsmDwarfVariable, "AsmDwarfVariable", "AsmDwarfVariableTag");



     NEW_TERMINAL_MACRO(AsmDwarfVolatileType, "AsmDwarfVolatileType", "AsmDwarfVolatileTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfDwarfProcedure, "AsmDwarfDwarfProcedure", "AsmDwarfDwarfProcedureTag");



     NEW_TERMINAL_MACRO(AsmDwarfRestrictType, "AsmDwarfRestrictType", "AsmDwarfRestrictTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfInterfaceType, "AsmDwarfInterfaceType", "AsmDwarfInterfaceTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfNamespace, "AsmDwarfNamespace", "AsmDwarfNamespaceTag");
     AsmDwarfNamespace.setFunctionPrototype("HEADER_DWARF_NAMESPACE", "../Grammar/BinaryInstruction.code");
     AsmDwarfNamespace.setFunctionSource("SOURCE_DWARF_NAMESPACE", "../Grammar/BinaryInstruction.code");
     AsmDwarfNamespace.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfImportedModule, "AsmDwarfImportedModule", "AsmDwarfImportedModuleTag");



     NEW_TERMINAL_MACRO(AsmDwarfUnspecifiedType, "AsmDwarfUnspecifiedType", "AsmDwarfUnspecifiedTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfPartialUnit, "AsmDwarfPartialUnit", "AsmDwarfPartialUnitTag");



     NEW_TERMINAL_MACRO(AsmDwarfImportedUnit, "AsmDwarfImportedUnit", "AsmDwarfImportedUnitTag");



     NEW_TERMINAL_MACRO(AsmDwarfMutableType, "AsmDwarfMutableType", "AsmDwarfMutableTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfCondition, "AsmDwarfCondition", "AsmDwarfConditionTag");



     NEW_TERMINAL_MACRO(AsmDwarfSharedType, "AsmDwarfSharedType", "AsmDwarfSharedTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfFormatLabel, "AsmDwarfFormatLabel", "AsmDwarfFormatLabelTag");



     NEW_TERMINAL_MACRO(AsmDwarfFunctionTemplate, "AsmDwarfFunctionTemplate", "AsmDwarfFunctionTemplateTag");



     NEW_TERMINAL_MACRO(AsmDwarfClassTemplate, "AsmDwarfClassTemplate", "AsmDwarfClassTemplateTag");



     NEW_TERMINAL_MACRO(AsmDwarfUpcSharedType, "AsmDwarfUpcSharedType", "AsmDwarfUpcSharedTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfUpcStrictType, "AsmDwarfUpcStrictType", "AsmDwarfUpcStrictTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfUpcRelaxedType, "AsmDwarfUpcRelaxedType", "AsmDwarfUpcRelaxedTypeTag");



     NEW_TERMINAL_MACRO(AsmDwarfUnknownConstruct, "AsmDwarfUnknownConstruct", "AsmDwarfUnknownConstructTag");



     NEW_NONTERMINAL_MACRO(AsmDwarfConstruct,
                           AsmDwarfArrayType | AsmDwarfClassType | AsmDwarfEntryPoint | AsmDwarfEnumerationType |
                           AsmDwarfFormalParameter | AsmDwarfImportedDeclaration | AsmDwarfLabel | AsmDwarfLexicalBlock |
                           AsmDwarfMember | AsmDwarfPointerType | AsmDwarfReferenceType | AsmDwarfCompilationUnit |
                           AsmDwarfStringType | AsmDwarfStructureType | AsmDwarfSubroutineType | AsmDwarfTypedef |
                           AsmDwarfUnionType | AsmDwarfUnspecifiedParameters | AsmDwarfVariant | AsmDwarfCommonBlock |
                           AsmDwarfCommonInclusion | AsmDwarfInheritance | AsmDwarfInlinedSubroutine | AsmDwarfModule |
                           AsmDwarfPtrToMemberType | AsmDwarfSetType | AsmDwarfSubrangeType | AsmDwarfWithStmt |
                           AsmDwarfAccessDeclaration | AsmDwarfBaseType | AsmDwarfCatchBlock | AsmDwarfConstType |
                           AsmDwarfConstant | AsmDwarfEnumerator | AsmDwarfFileType | AsmDwarfFriend | AsmDwarfNamelist |
                           AsmDwarfNamelistItem | AsmDwarfPackedType | AsmDwarfSubprogram | AsmDwarfTemplateTypeParameter |
                           AsmDwarfTemplateValueParameter | AsmDwarfThrownType | AsmDwarfTryBlock | AsmDwarfVariantPart |
                           AsmDwarfVariable | AsmDwarfVolatileType | AsmDwarfDwarfProcedure | AsmDwarfRestrictType |
                           AsmDwarfInterfaceType | AsmDwarfNamespace | AsmDwarfImportedModule | AsmDwarfUnspecifiedType |
                           AsmDwarfPartialUnit | AsmDwarfImportedUnit | AsmDwarfMutableType | AsmDwarfCondition |
                           AsmDwarfSharedType | AsmDwarfFormatLabel | AsmDwarfFunctionTemplate | AsmDwarfClassTemplate |
                           AsmDwarfUpcSharedType | AsmDwarfUpcStrictType | AsmDwarfUpcRelaxedType | AsmDwarfUnknownConstruct,
                           "AsmDwarfConstruct", "AsmDwarfConstructTag", false);
     AsmDwarfConstruct.setFunctionPrototype("HEADER_DWARF_CONSTRUCT", "../Grammar/BinaryInstruction.code");
     AsmDwarfConstruct.setFunctionSource("SOURCE_DWARF_CONSTRUCT", "../Grammar/BinaryInstruction.code");
     // For now we will store the nesting level (just to support debugging), then it will be removed.
     AsmDwarfConstruct.setDataPrototype("int", "nesting_level", "= 0",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfConstruct.setDataPrototype("uint64_t", "offset", "= 0",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfConstruct.setDataPrototype("uint64_t", "overall_offset", "= 0",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // A lot of constructs have a name so put the name into the base class
     AsmDwarfConstruct.setDataPrototype("std::string", "name", "= \"\"",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // This is a data member that can be filled in via a separate analysis (we would have to read the line information before
     // hand).
     AsmDwarfConstruct.setDataPrototype("SgAsmDwarfLine*", "source_position", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);




     NEW_TERMINAL_MACRO(AsmDwarfMacroList, "AsmDwarfMacroList", "AsmDwarfMacroListTag");
     AsmDwarfMacroList.setDataPrototype("SgAsmDwarfMacroPtrList", "macro_list", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfMacro, "AsmDwarfMacro", "AsmDwarfMacroTag");
     AsmDwarfMacro.setDataPrototype("std::string", "macro_string", "= \"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfLine, "AsmDwarfLine", "AsmDwarfLineTag");
     AsmDwarfLine.setDataPrototype("uint64_t", "address", "= 0",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfLine.setDataPrototype("int", "file_id", "= Sg_File_Info::NULL_FILE_ID",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfLine.setDataPrototype("int", "line", "= 0",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfLine.setDataPrototype("int", "column", "= 0",
                                   CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfLineList, "AsmDwarfLineList", "AsmDwarfLineListTag");
     AsmDwarfLineList.setFunctionPrototype("HEADER_DWARF_LINE_LIST", "../Grammar/BinaryInstruction.code");
     AsmDwarfLineList.setFunctionSource("SOURCE_DWARF_LINE_LIST", "../Grammar/BinaryInstruction.code");
     AsmDwarfLineList.setDataPrototype("SgAsmDwarfLinePtrList", "line_list", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#if 0
     // I [DQ] am having trouble making these proper data members so just use function to return them, so that they are
     // computed dynamically.
     AsmDwarfLineList.setDataPrototype("SgInstructionAddressSourcePositionMapPtrList", "instructionToSourceMap", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfLineList.setDataPrototype("SgSourcePositionInstructionAddressMapPtrList", "sourceToInstructionMap", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif



     NEW_TERMINAL_MACRO(AsmDwarfConstructList, "AsmDwarfConstructList", "AsmDwarfConstructListTag");
     AsmDwarfConstructList.setDataPrototype("SgAsmDwarfConstructPtrList", "list", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmDwarfCompilationUnitList, "AsmDwarfCompilationUnitList", "AsmDwarfCompilationUnitListTag");
     AsmDwarfCompilationUnitList.setDataPrototype("SgAsmDwarfCompilationUnitPtrList", "cu_list", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_NONTERMINAL_MACRO(AsmDwarfInformation,
                           AsmDwarfMacro | AsmDwarfMacroList | AsmDwarfLine | AsmDwarfLineList | AsmDwarfCompilationUnitList |
                           AsmDwarfConstruct | AsmDwarfConstructList,
                           "AsmDwarfInformation", "AsmDwarfInformationTag", false);



     /*************************************************************************************************************************
      *                                         Strings
      * These nodes describe how character strings (symbol names, section names, etc) are stored in a binary file.
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmStringStorage, "AsmStringStorage", "AsmStringStorageTag");
     AsmStringStorage.setFunctionPrototype("HEADER_STRING_STORAGE", "../Grammar/BinaryInstruction.code");
     AsmStringStorage.setDataPrototype("SgAsmGenericStrtab*", "strtab", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmStringStorage.setDataPrototype("std::string", "string", "= \"\"",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmStringStorage.setDataPrototype("rose_addr_t", "offset", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmBasicString, "AsmBasicString", "AsmBasicStringTag");
     AsmBasicString.setFunctionPrototype("HEADER_BASIC_STRING", "../Grammar/BinaryInstruction.code");
     AsmBasicString.setDataPrototype("std::string", "string", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmStoredString, "AsmStoredString", "AsmStoredStringTag");
     AsmStoredString.setFunctionPrototype("HEADER_STORED_STRING", "../Grammar/BinaryInstruction.code");
     AsmStoredString.setDataPrototype("SgAsmStringStorage*", "storage", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_NONTERMINAL_MACRO(AsmGenericString,
                           AsmBasicString | AsmStoredString,
                           "AsmGenericString", "AsmGenericStringTag", false);
     AsmGenericString.setFunctionPrototype("HEADER_GENERIC_STRING", "../Grammar/BinaryInstruction.code");






     /*************************************************************************************************************************
      *                                         Generic Binary IR Nodes
      * These are mostly base classes for the container-specific nodes defined above.
      *************************************************************************************************************************/

     NEW_TERMINAL_MACRO(AsmGenericHeaderList, "AsmGenericHeaderList", "AsmGenericHeaderListTag");
     AsmGenericHeaderList.setDataPrototype("SgAsmGenericHeaderPtrList", "headers", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_NONTERMINAL_MACRO(AsmGenericHeader,
                           AsmPEFileHeader | AsmLEFileHeader | AsmNEFileHeader | AsmDOSFileHeader | AsmElfFileHeader,
                           "AsmGenericHeader", "AsmGenericHeaderTag", true);
     AsmGenericHeader.setFunctionPrototype("HEADER_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");
     AsmGenericHeader.setFunctionSource("SOURCE_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");
     AsmGenericHeader.setAutomaticGenerationOfDestructor(false);
     // General info about the executable format
     AsmGenericHeader.setDataPrototype("SgAsmGenericFormat*", "exec_format", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     // Optional magic number in file byte order
     AsmGenericHeader.setDataPrototype("SgCharList", "magic", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Machine for which this header and its sections, etc. was compiled
     AsmGenericHeader.setDataPrototype("SgAsmGenericFormat::InsSetArchitecture", "isa", "= SgAsmGenericFormat::ISA_UNSPECIFIED",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Base virtual address used by all "relative virtual addresses" (RVA)
     AsmGenericHeader.setDataPrototype("rose_addr_t", "base_va", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Code entry point wrt base_va
     AsmGenericHeader.setDataPrototype("SgRVAList", "entry_rvas", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericHeader.setDataPrototype("SgAsmGenericDLLList*", "dlls", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmGenericHeader.setDataPrototype("SgAsmGenericSectionList*", "sections", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmGenericSymbolList, "AsmGenericSymbolList", "AsmGenericSymbolListTag");
     AsmGenericSymbolList.setDataPrototype("SgAsmGenericSymbolPtrList", "symbols", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_NONTERMINAL_MACRO(AsmGenericSymbol,
                           AsmCoffSymbol | AsmElfSymbol,
                           "AsmGenericSymbol", "AsmGenericSymbolTag", false);
     AsmGenericSymbol.setFunctionPrototype("HEADER_GENERIC_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmGenericSymbol.setFunctionSource("SOURCE_GENERIC_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmGenericSymbol.setAutomaticGenerationOfConstructor(false);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolDefState", "def_state", "= SgAsmGenericSymbol::SYM_UNDEFINED",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolBinding", "binding", "= SgAsmGenericSymbol::SYM_NO_BINDING",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolType", "type", "= SgAsmGenericSymbol::SYM_NO_TYPE",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("rose_addr_t", "value", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("rose_addr_t", "size", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSection*", "bound", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_NONTERMINAL_MACRO(AsmGenericStrtab,
                           AsmElfStrtab | AsmCoffStrtab,
                           "AsmGenericStrtab", "AsmGenericStrtabTag", false);
     AsmGenericStrtab.setFunctionPrototype("HEADER_GENERIC_STRTAB", "../Grammar/BinaryInstruction.code");
     AsmGenericStrtab.setAutomaticGenerationOfDestructor(false);
     AsmGenericStrtab.setDataPrototype("SgAsmGenericSection*", "container", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericStrtab.setDataPrototype("SgAsmGenericStrtab::referenced_t", "storage_list", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericStrtab.setDataPrototype("ExtentMap", "freelist", "", // space avail. for new strings
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericStrtab.setDataPrototype("SgAsmStringStorage*", "dont_free", "= NULL", // never free this storage
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericStrtab.setDataPrototype("size_t", "num_freed", "= 0", // number of strings freed thus far
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmGenericSectionList, "AsmGenericSectionList", "AsmGenericSectionListTag");
     AsmGenericSectionList.setDataPrototype("SgAsmGenericSectionPtrList", "sections", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_NONTERMINAL_MACRO(AsmGenericSection,
                           AsmGenericHeader | AsmElfSection | AsmElfSectionTable | AsmElfSegmentTable | AsmPESection |
                           AsmPESectionTable | AsmDOSExtendedHeader | AsmCoffSymbolTable | AsmNESection | AsmNESectionTable |
                           AsmNENameTable | AsmNEModuleTable | AsmNEStringTable | AsmNEEntryTable | AsmNERelocTable |
                           AsmLESection | AsmLESectionTable | AsmLENameTable | AsmLEPageTable | AsmLEEntryTable | AsmLERelocTable,
                           "AsmGenericSection", "AsmGenericSectionTag", true);
     AsmGenericSection.setFunctionPrototype("HEADER_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmGenericSection.setFunctionSource("SOURCE_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmGenericSection.setPredeclarationString("HEADER_GENERIC_SECTION_PREDECLARATION", "../Grammar/BinaryInstruction.code");
     AsmGenericSection.setAutomaticGenerationOfDestructor(false);
     AsmGenericSection.setDataPrototype("SgAsmGenericFile*", "file", "= NULL", // The file to which this section belongs
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("SgAsmGenericHeader*", "header", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Current size of section in bytes (may be different than original size, which is kept in p_data.size())
     AsmGenericSection.setDataPrototype("rose_addr_t", "size", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t", "offset", "= 0", // Starting file offset of the section
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Required file alignment; zero and one both imply byte alignment
     AsmGenericSection.setDataPrototype("rose_addr_t", "file_alignment", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Original content of just this section; points into file's content
     AsmGenericSection.setDataPrototype("SgFileContentList", "data", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // General contents of the section
     AsmGenericSection.setDataPrototype("SgAsmGenericSection::SectionPurpose", "purpose", "= SgAsmGenericSection::SP_UNSPECIFIED",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Section was created by the format reader; not specified in file
     AsmGenericSection.setDataPrototype("bool", "synthesized", "= false",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Non-unique section ID (unique for ELF) or negative
     AsmGenericSection.setDataPrototype("int", "id", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Non-unique name of section (may be empty string).  The name is not part of the AST traversal because if we
     // made it part of the traversal then we'd need to fix some of the subclasses which contain a solitary list
     // data member (ROSE cannot have IR nodes that contain a list and other data members).  Therefore, the SgAsmGenericSection
     // destructor will need to explicitly delete the name.
     AsmGenericSection.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("std::string", "short_name", "",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_preferred_rva", "= 0", // DOXYGEN
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_size", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_alignment", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool", "mapped_rperm", "= false",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool", "mapped_wperm", "= false",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool", "mapped_xperm", "= false",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool", "contains_code", "= false", // DOXYGEN
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_actual_va", "= 0", // DOXYGEN
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmGenericDLLList, "AsmGenericDLLList", "AsmGenericDLLListTag");
     AsmGenericDLLList.setDataPrototype("SgAsmGenericDLLPtrList", "dlls", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmGenericDLL, "AsmGenericDLL", "AsmGenericDLLTag");
     AsmGenericDLL.setFunctionPrototype("HEADER_GENERIC_DLL", "../Grammar/BinaryInstruction.code");
     AsmGenericDLL.setFunctionSource("SOURCE_GENERIC_DLL", "../Grammar/BinaryInstruction.code");
     AsmGenericDLL.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmGenericDLL.setDataPrototype("SgStringList", "symbols", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);



     NEW_TERMINAL_MACRO(AsmGenericFormat, "AsmGenericFormat", "AsmGenericFormatTag");
     AsmGenericFormat.setFunctionPrototype("HEADER_GENERIC_FORMAT", "../Grammar/BinaryInstruction.code");
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecFamily", "family", "= SgAsmGenericFormat::FAMILY_UNSPECIFIED",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecPurpose", "purpose", "= SgAsmGenericFormat::PURPOSE_EXECUTABLE",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ByteOrder", "sex", "= SgAsmGenericFormat::ORDER_UNSPECIFIED",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // actual file format version number stored in file
     AsmGenericFormat.setDataPrototype("unsigned", "version", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // is 'version' considered to be the current, supported version
     AsmGenericFormat.setDataPrototype("bool", "is_current_version", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // application binary interface
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecABI", "abi", "= SgAsmGenericFormat::ABI_UNSPECIFIED",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // version of the ABI targeted by this file
     AsmGenericFormat.setDataPrototype("unsigned", "abi_version", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Initialize to ZERO first and then set as required, in bytes (e.g., Elf32 is 4; Elf64 is 8)
     AsmGenericFormat.setDataPrototype("size_t", "word_size", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmGenericFileList, "AsmGenericFileList", "AsmGenericFileListTag");
     AsmGenericFileList.setDataPrototype("SgAsmGenericFilePtrList", "files", "",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     NEW_TERMINAL_MACRO(AsmGenericFile, "AsmGenericFile", "AsmGenericFileTag");
     AsmGenericFile.setFunctionPrototype("HEADER_GENERIC_FILE", "../Grammar/BinaryInstruction.code");
     AsmGenericFile.setPredeclarationString("HEADER_GENERIC_FILE_PREDECLARATION", "../Grammar/BinaryInstruction.code");
     AsmGenericFile.setAutomaticGenerationOfConstructor(false);
     AsmGenericFile.setAutomaticGenerationOfDestructor(false);
     AsmGenericFile.setDataPrototype("std::string", "name", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // File descriptor opened for read-only (or negative)
     AsmGenericFile.setDataPrototype("int", "fd", "= -1",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // File attributes at time of file open (valid if fd>=0)
     AsmGenericFile.setDataPrototype("SgAsmGenericFormat::fileDetails", "sb", "",
                                     NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // Content of file mapped into memory
     AsmGenericFile.setDataPrototype("SgFileContentList", "data", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // All known header sections for this file
     AsmGenericFile.setDataPrototype("SgAsmGenericHeaderList*", "headers", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     // All known holes (areas otherwise unreferenced by parsing)
     AsmGenericFile.setDataPrototype("SgAsmGenericSectionList*", "holes", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("bool", "truncate_zeros", "= false",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("bool", "tracking_references", "= true",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("ExtentMap", "referenced_extents", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("bool", "neuter", "= false", // if set, refuse to unparse file
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /*************************************************************************************************************************
      *                                         Generic Binary File
      * This is the root of binary file format container classes.
      *************************************************************************************************************************/

     NEW_NONTERMINAL_MACRO(AsmExecutableFileFormat,
                           AsmGenericDLL | AsmGenericFormat | AsmGenericDLLList | AsmElfEHFrameEntryFD | AsmGenericFile |
                           AsmGenericSection | AsmGenericSymbol | AsmGenericStrtab | AsmGenericSymbolList |
                           AsmGenericSectionList | AsmGenericHeaderList | AsmGenericString | AsmElfSectionTableEntry |
                           AsmElfSegmentTableEntry | AsmElfSymbolList | AsmElfRelocEntry |
                           AsmElfRelocEntryList | AsmPEExportEntry | AsmPEExportEntryList | AsmElfDynamicEntry |
                           AsmElfDynamicEntryList | AsmElfSegmentTableEntryList | AsmStringStorage | AsmElfNoteEntry |
                           AsmElfNoteEntryList | AsmElfSymverEntry | AsmElfSymverEntryList | AsmElfSymverDefinedEntry |
                           AsmElfSymverDefinedEntryList | AsmElfSymverDefinedAux | AsmElfSymverDefinedAuxList |
                           AsmElfSymverNeededEntry | AsmElfSymverNeededEntryList | AsmElfSymverNeededAux |
                           AsmElfSymverNeededAuxList | AsmPEImportDirectory | AsmPESectionTableEntry |
                           AsmPEExportDirectory | AsmPERVASizePair | AsmCoffSymbolList | AsmPERVASizePairList |
                           AsmElfEHFrameEntryCI |
                           AsmPEImportDirectoryList | AsmNEEntryPoint | AsmNERelocEntry | AsmNESectionTableEntry |
                           AsmElfEHFrameEntryCIList | AsmLEPageTableEntry | AsmLEEntryPoint | AsmLESectionTableEntry |
                           AsmElfEHFrameEntryFDList | AsmDwarfInformation | AsmPEImportItem | AsmPEImportItemList,
                           "AsmExecutableFileFormat", "AsmExecutableFileFormatTag", false);
     AsmExecutableFileFormat.setFunctionPrototype("HEADER_EXECUTABLE_FILE_FORMAT", "../Grammar/BinaryInstruction.code");



     /*************************************************************************************************************************
      *                                         Root of all binary IR classes
      *************************************************************************************************************************/

     NEW_NONTERMINAL_MACRO(AsmNode,
                           AsmStatement | AsmExpression | AsmInterpretation | AsmOperandList | AsmType |
                           AsmExecutableFileFormat | AsmInterpretationList | AsmGenericFileList,
                           "AsmNode", "AsmNodeTag", false);
     AsmNode.setFunctionPrototype("HEADER_BINARY", "../Grammar/BinaryInstruction.code");
     AsmNode.setFunctionSource("SOURCE_BINARY", "../Grammar/BinaryInstruction.code");
     AsmNode.setDataPrototype("AttachedPreprocessingInfoType*", "attachedPreprocessingInfoPtr", "= NULL",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, COPY_DATA);
     AsmNode.setDataPrototype("AstAttributeMechanism*", "asmAttributeMechanism", "= NULL",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmNode.setFunctionPrototype("HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     AsmNode.setFunctionSource("SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     AsmNode.setFunctionPrototype("HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     AsmNode.setFunctionSource("SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     AsmNode.setDataPrototype("AstAttributeMechanism*", "attributeMechanism", "= NULL",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
}
