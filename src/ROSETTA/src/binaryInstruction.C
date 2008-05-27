
#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"

void
Grammar::setUpBinaryInstructions ()
   {

  // DQ (3/14/2007): Added support in IR for binaries.
  // This function sets up the IR nodes to support the representation of a binary file (or machine instructions).
  // It can be either platform specific or non-platform specific.

  // tps (09/21/07): example code to handle ARM binaries

     NEW_TERMINAL_MACRO ( AsmBlock        , "AsmBlock",        "AsmBlockTag" );
     NEW_TERMINAL_MACRO ( AsmOperandList  , "AsmOperandList",  "AsmOperandListTag" );


     NEW_TERMINAL_MACRO ( AsmArmInstruction, "AsmArmInstruction", "AsmArmInstructionTag" );
     // AsmArmInstruction.setFunctionPrototype           ( "HEADER_BINARY_ARM_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     AsmArmInstruction.setDataPrototype                    ("ArmInstructionKind","kind","= arm_unknown_instruction",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmArmInstruction.setDataPrototype                    ("ArmInstructionCondition","condition","= arm_cond_unknown",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmArmInstruction.setDataPrototype                    ("int","positionOfConditionInMnemonic","= -1",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmArmInstruction.setPredeclarationString     ("HEADER_BINARY_ARM_INSTRUCTION_PREDECLARATION" , "../Grammar/BinaryInstruction.code");

     NEW_TERMINAL_MACRO ( Asmx86Instruction, "Asmx86Instruction", "Asmx86InstructionTag" );
     Asmx86Instruction.setFunctionPrototype           ( "HEADER_BINARY_X86_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     Asmx86Instruction.setDataPrototype                    ("X86InstructionKind","kind","= x86_unknown_instruction",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     Asmx86Instruction.setDataPrototype                    ("X86InstructionSize","baseSize","= x86_insnsize_none",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     Asmx86Instruction.setDataPrototype                    ("X86InstructionSize","operandSize","= x86_insnsize_none",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     Asmx86Instruction.setDataPrototype                    ("X86InstructionSize","addressSize","= x86_insnsize_none",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     Asmx86Instruction.setDataPrototype                    ("bool","lockPrefix","= false",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     Asmx86Instruction.setDataPrototype                    ("X86BranchPrediction","branchPrediction","= x86_branch_prediction_none",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     Asmx86Instruction.setDataPrototype                    ("X86SegmentRegister","segmentOverride","= x86_segreg_none",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     Asmx86Instruction.setPredeclarationString     ("HEADER_BINARY_X86_INSTRUCTION_PREDECLARATION" , "../Grammar/BinaryInstruction.code");


  // This is currently a AsmFunctionCall plus other unspecified uses of Asmx86Instruction, it may be refied later.
     NEW_NONTERMINAL_MACRO ( AsmInstruction, Asmx86Instruction | AsmArmInstruction
			     , "AsmInstruction", "AsmInstructionTag", true );


     NEW_TERMINAL_MACRO ( AsmDataStructureDeclaration , "AsmDataStructureDeclaration", "AsmDataStructureDeclarationTag" );
     NEW_TERMINAL_MACRO ( AsmFunctionDeclaration      , "AsmFunctionDeclaration",      "AsmFunctionDeclarationTag" );
     NEW_TERMINAL_MACRO ( AsmFieldDeclaration         , "AsmFieldDeclaration",         "AsmFieldDeclarationTag" );

     NEW_NONTERMINAL_MACRO ( AsmDeclaration, AsmDataStructureDeclaration | AsmFunctionDeclaration | 
                             AsmFieldDeclaration, "AsmDeclaration", "AsmDeclarationTag", false );

     NEW_NONTERMINAL_MACRO ( AsmStatement, AsmDeclaration | AsmBlock | AsmInstruction, "AsmStatement", "AsmStatementTag", false );

  // DQ (3/30/2007): Fixed up the names where were "AsmUnaryAdd"
     NEW_TERMINAL_MACRO ( AsmBinaryAdd    ,  "AsmBinaryAdd",  "AsmBinaryAddTag" );
     NEW_TERMINAL_MACRO ( AsmBinarySubtract, "AsmBinarySubtract", "AsmBinarySubtractTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryMultiply, "AsmBinaryMultiply", "AsmBinaryMultiplyTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryDivide,   "AsmBinaryDivide", "AsmBinaryDivideTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryMod,      "AsmBinaryMod", "AsmBinaryModTag" );

  // ARM addressing mode operations
     NEW_TERMINAL_MACRO ( AsmBinaryAddPreupdate    ,  "AsmBinaryAddPreupdate",  "AsmBinaryAddPreupdateTag" );
     NEW_TERMINAL_MACRO ( AsmBinarySubtractPreupdate, "AsmBinarySubtractPreupdate", "AsmBinarySubtractPreupdateTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryAddPostupdate    ,  "AsmBinaryAddPostupdate",  "AsmBinaryAddPostupdateTag" );
     NEW_TERMINAL_MACRO ( AsmBinarySubtractPostupdate, "AsmBinarySubtractPostupdate", "AsmBinarySubtractPostupdateTag" );

     NEW_TERMINAL_MACRO ( AsmBinaryLsl, "AsmBinaryLsl", "AsmBinaryLslTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryLsr, "AsmBinaryLsr", "AsmBinaryLsrTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryAsr, "AsmBinaryAsr", "AsmBinaryAsrTag" );
     NEW_TERMINAL_MACRO ( AsmBinaryRor, "AsmBinaryRor", "AsmBinaryRorTag" );

     NEW_NONTERMINAL_MACRO ( AsmBinaryExpression, AsmBinaryAdd | AsmBinarySubtract | AsmBinaryMultiply | 
                             AsmBinaryDivide | AsmBinaryMod | AsmBinaryAddPreupdate | AsmBinarySubtractPreupdate | AsmBinaryAddPostupdate | AsmBinarySubtractPostupdate | AsmBinaryLsl | AsmBinaryLsr | AsmBinaryAsr | AsmBinaryRor, "AsmBinaryExpression", "AsmBinaryExpressionTag", false );

     NEW_TERMINAL_MACRO ( AsmUnaryPlus , "AsmUnaryPlus",  "AsmUnaryPlusTag" );
     NEW_TERMINAL_MACRO ( AsmUnaryMinus, "AsmUnaryMinus", "AsmUnaryMinusTag" );
     NEW_TERMINAL_MACRO ( AsmUnaryRrx, "AsmUnaryRrx", "AsmUnaryRrxTag" );
     NEW_TERMINAL_MACRO ( AsmUnaryArmSpecialRegisterList, "AsmUnaryArmSpecialRegisterList", "AsmUnaryArmSpecialRegisterListTag" );

     NEW_NONTERMINAL_MACRO ( AsmUnaryExpression, AsmUnaryPlus | AsmUnaryMinus | AsmUnaryRrx | AsmUnaryArmSpecialRegisterList, "AsmUnaryExpression", "AsmUnaryExpressionTag", false );

     NEW_TERMINAL_MACRO ( AsmMemoryReferenceExpression   , "AsmMemoryReferenceExpression",   "AsmMemoryReferenceExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmControlFlagsExpression      , "AsmControlFlagsExpression",      "AsmControlFlagsExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmCommonSubExpression         , "AsmCommonSubExpression",         "AsmCommonSubExpressionTag" );
     NEW_TERMINAL_MACRO ( Asmx86RegisterReferenceExpression , "Asmx86RegisterReferenceExpression", "Asmx86RegisterReferenceExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmArmRegisterReferenceExpression , "AsmArmRegisterReferenceExpression", "AsmArmRegisterReferenceExpressionTag" );
     NEW_NONTERMINAL_MACRO ( AsmRegisterReferenceExpression , Asmx86RegisterReferenceExpression | AsmArmRegisterReferenceExpression, "AsmRegisterReferenceExpression", "AsmRegisterReferenceExpressionTag" , false);

     //     NEW_TERMINAL_MACRO ( AsmArmRegisterReferenceExpression , "AsmArmRegisterReferenceExpression", "AsmArmRegisterReferenceExpressionTag" );

     NEW_TERMINAL_MACRO ( AsmByteValueExpression        , "AsmByteValueExpression",        "AsmByteValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmWordValueExpression        , "AsmWordValueExpression",        "AsmWordValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmDoubleWordValueExpression  , "AsmDoubleWordValueExpression",  "AsmDoubleWordValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmQuadWordValueExpression    , "AsmQuadWordValueExpression",    "AsmQuadWordValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmSingleFloatValueExpression , "AsmSingleFloatValueExpression", "AsmSingleFloatValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmDoubleFloatValueExpression , "AsmDoubleFloatValueExpression", "AsmDoubleFloatValueExpressionTag" );
     NEW_TERMINAL_MACRO ( AsmVectorValueExpression      , "AsmVectorValueExpression",      "AsmVectorValueExpressionTag" );

     NEW_NONTERMINAL_MACRO ( AsmValueExpression, AsmByteValueExpression | AsmWordValueExpression | 
                             AsmDoubleWordValueExpression | AsmQuadWordValueExpression | 
                             AsmSingleFloatValueExpression | AsmDoubleFloatValueExpression | 
                             AsmVectorValueExpression, "AsmValueExpression", "AsmValueExpressionTag", false );

     NEW_TERMINAL_MACRO (AsmExprListExp,            "AsmExprListExp",            "AsmExprListExpTag" );
     AsmExprListExp.setDataPrototype("SgAsmExpressionPtrList", "expressions", "",
				     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


     NEW_NONTERMINAL_MACRO ( AsmExpression, AsmValueExpression | AsmBinaryExpression | AsmUnaryExpression | 
                             AsmMemoryReferenceExpression | AsmRegisterReferenceExpression | AsmControlFlagsExpression | 
                             AsmCommonSubExpression | AsmExprListExp
			     //| AsmArmRegisterReferenceExpression
			     , "AsmExpression", "AsmExpressionTag", false );

  // These are attached to the SgAsmNode object (the root for the IR used for binaries)
     NEW_TERMINAL_MACRO ( AsmTypeByte        , "AsmTypeByte",        "AsmTypeByteTag" );
     NEW_TERMINAL_MACRO ( AsmTypeWord        , "AsmTypeWord",        "AsmTypeWordTag" );
     NEW_TERMINAL_MACRO ( AsmTypeDoubleWord  , "AsmTypeDoubleWord",  "AsmTypeDoubleWordTag" );
     NEW_TERMINAL_MACRO ( AsmTypeQuadWord    , "AsmTypeQuadWord",    "AsmTypeQuadWordTag" );
     NEW_TERMINAL_MACRO ( AsmTypeDoubleQuadWord, "AsmTypeDoubleQuadWord",    "AsmTypeDoubleQuadWordTag" );
     NEW_TERMINAL_MACRO ( AsmTypeSingleFloat , "AsmTypeSingleFloat", "AsmTypeSingleFloatTag" );
     NEW_TERMINAL_MACRO ( AsmTypeDoubleFloat , "AsmTypeDoubleFloat", "AsmTypeDoubleFloatTag" );
     NEW_TERMINAL_MACRO ( AsmType80bitFloat ,  "AsmType80bitFloat",  "AsmType80bitFloatTag" );
     NEW_TERMINAL_MACRO ( AsmType128bitFloat , "AsmType128bitFloat", "AsmType128bitFloatTag" );
     NEW_TERMINAL_MACRO ( AsmTypeVector      , "AsmTypeVector",      "AsmTypeVectorTag" );
     AsmTypeByte.setDataPrototype                    ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeWord.setDataPrototype                    ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeDoubleWord.setDataPrototype              ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeQuadWord.setDataPrototype                ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeDoubleQuadWord.setDataPrototype          ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeSingleFloat.setDataPrototype             ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeDoubleFloat.setDataPrototype             ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeVector.setDataPrototype                  ("int","elementCount","",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmTypeVector.setDataPrototype                  ("SgAsmType*","elementType","",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmType80bitFloat.setDataPrototype             ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmType128bitFloat.setDataPrototype            ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     NEW_NONTERMINAL_MACRO ( AsmType, AsmTypeByte | AsmTypeWord | AsmTypeDoubleWord | AsmTypeQuadWord | AsmTypeDoubleQuadWord | AsmType80bitFloat | AsmType128bitFloat |
                             AsmTypeSingleFloat | AsmTypeDoubleFloat | AsmTypeVector, "AsmType", "AsmTypeTag", false );

     NEW_TERMINAL_MACRO ( AsmFile                     , "AsmFile",                     "AsmFileTag" );

  // DQ (1/6/2008): Added ELF program header and section header support to AST.
     NEW_TERMINAL_MACRO ( AsmProgramHeader, "AsmProgramHeader", "AsmProgramHeaderTag" );
     NEW_TERMINAL_MACRO ( AsmSectionHeader, "AsmSectionHeader", "AsmSectionHeaderTag" );

  // DQ (1/6/2008): These store the lists of AsmProgramHeader and AsmSectionHeader objects.
  // AsmFile can not have traversed data members and lists, so this give it two additional data members.
     NEW_TERMINAL_MACRO ( AsmProgramHeaderList, "AsmProgramHeaderList", "AsmProgramHeaderListTag" );
     NEW_TERMINAL_MACRO ( AsmSectionHeaderList, "AsmSectionHeaderList", "AsmSectionHeaderListTag" );

  // NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmOperandList | AsmType, "AsmNode","AsmNodeTag");
     NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmProgramHeader | AsmSectionHeader | AsmProgramHeaderList | AsmSectionHeaderList | AsmOperandList | AsmType, "AsmNode","AsmNodeTag", false);

  // DQ (3/15/2007): Added support forbinaries (along lines of suggestions by Thomas Dullien)
  // AsmInstructionBase.setFunctionPrototype        ( "HEADER", "../Grammar/Common.code");
     AsmNode.setFunctionPrototype        ( "HEADER_BINARY", "../Grammar/BinaryInstruction.code");

  // Asm instructions should be able to have attached attributes like other IR nodes in ROSE
     AsmNode.setDataPrototype     ( "AttachedPreprocessingInfoType*", "attachedPreprocessingInfoPtr", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, COPY_DATA);

     AsmStatement.setDataPrototype("unsigned int","address","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     //AsmStatement.setDataPrototype("SgAsmNode*","parent","= NULL",
     //                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

  // Most platform and non-platform dependent machine instructions will map to this IR node
     AsmInstruction.setFunctionPrototype        ( "HEADER_BINARY_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     AsmInstruction.setDataPrototype("std::string","mnemonic","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
  // AsmInstruction.setDataPrototype("long","basic_block_id","= -1",
  //                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("std::string","raw_bytes","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("std::string","comment","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("SgAsmOperandList*","operandList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("SgAsmStatementPtrList","sources","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Block of instructions (helps define depth to the AST)
     AsmBlock.setFunctionPrototype              ( "HEADER_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
     // added by tps, 05Apr07 ... need this for the control_flow_graph
     AsmBlock.setDataPrototype("unsigned int","next_block_true_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("unsigned int","next_block_false_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("unsigned int","id","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);

     AsmOperandList.setFunctionPrototype        ( "HEADER_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");
     AsmOperandList.setDataPrototype("SgAsmExpressionPtrList","operands","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmBlock.setDataPrototype("SgAsmStatementPtrList","statementList","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmBlock.setDataPrototype("bool","externallyVisible"," = true", // Can this block be called into from random code?
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmFile.setFunctionPrototype ( "HEADER_BINARY_FILE", "../Grammar/BinaryInstruction.code");
     AsmFile.setDataPrototype("std::string","name","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmBlock*","global_block","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // AsmFile.setDataPrototype("std::vector<unsigned int>", "magic_number_vector","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // AsmFile.setDataPrototype("SgAsmFile::magic_number_vector_type", "magic_number_vector","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("std::string", "magic_number_string","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmFile.setDataPrototype("SgAsmFile::elf_class_kind_enum", "binary_class_type","= SgAsmFile::e_class_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_data_encoding_enum", "data_encoding","= SgAsmFile::e_data_encoding_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_version_enum", "version","= SgAsmFile::e_version_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("int", "magicNumberPaddingStartIndex","= -1",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_OS_ABI_identification_enum", "operating_system","= SgAsmFile::e_OS_ABI_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_object_file_type_enum", "object_file_type","= SgAsmFile::e_file_type_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmFile::elf_machine_architecture_enum", "machine_architecture","= SgAsmFile::e_machine_architecture_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "associated_entry_point","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "program_header_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "section_header_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "processor_specific_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "elf_header_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "program_header_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "program_header_entry_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "number_of_program_headers","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "section_header_entry_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "number_of_section_headers","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "section_header_string_table_index","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmProgramHeaderList*", "programHeaderList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmSectionHeaderList*", "sectionHeaderList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmSectionHeaderList.setFunctionPrototype ( "HEADER_BINARY_FILE_SECTION_HEADER_LIST", "../Grammar/BinaryInstruction.code");
#if 1
     AsmSectionHeaderList.setDataPrototype("SgAsmSectionHeaderPtrList","section_headers","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
     AsmSectionHeaderList.setAutomaticGenerationOfConstructor(FALSE);
     AsmSectionHeaderList.editSubstitute       ( "HEADER_LIST_DECLARATIONS", "HEADER_LIST_DECLARATIONS", "../Grammar/Statement.code" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_DATA_TYPE", "SgAsmSectionHeaderPtrList" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_NAME", "section_headers" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_FUNCTION_RETURN_TYPE", "void" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_FUNCTION_NAME", "section_header" );
     AsmSectionHeaderList.editSubstitute       ( "LIST_ELEMENT_DATA_TYPE", "SgStatement*" );
     AsmSectionHeaderList.setDataPrototype    ( "SgAsmSectionHeaderPtrList", "section_headers", "",
				      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

     AsmProgramHeaderList.setFunctionPrototype ( "HEADER_BINARY_FILE_PROGRAM_HEADER_LIST", "../Grammar/BinaryInstruction.code");
     AsmProgramHeaderList.setDataPrototype("SgAsmProgramHeaderPtrList","program_headers","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmProgramHeader.setFunctionPrototype ( "HEADER_BINARY_FILE_PROGRAM_HEADER", "../Grammar/BinaryInstruction.code");
  // AsmProgramHeader.setDataPrototype("std::string","name","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // AsmProgramHeader.setDataPrototype("unsigned long","name_string_index","= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","starting_file_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","starting_virtual_memory_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","starting_physical_memory_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","file_image_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","memory_image_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","segment_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmProgramHeader.setDataPrototype("unsigned long","alignment","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmSectionHeader.setFunctionPrototype ( "HEADER_BINARY_FILE_SECTION_HEADER", "../Grammar/BinaryInstruction.code");
     AsmSectionHeader.setDataPrototype("std::string","name","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","name_string_index","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","starting_memory_address","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","starting_file_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","table_index_link","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","info","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","address_alignment","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSectionHeader.setDataPrototype("unsigned long","table_entry_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // There are several sorts of declarations within a binary
     AsmDeclaration.setFunctionPrototype        ( "HEADER_BINARY_DECLARATION", "../Grammar/BinaryInstruction.code");

     AsmDataStructureDeclaration.setFunctionPrototype ( "HEADER_BINARY_DATA_STRUCTURE", "../Grammar/BinaryInstruction.code");
  // DQ (3/15/2007): I can't seem to get this to compile so I will leave it out for now!
  // Binaries have some easily resolved data structures so we use this to represent these
  // AsmDataStructureDeclaration.setDataPrototype("std::list<SgAsmDeclaration*>","declarationList","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);

  // Binaries have many easily resolved functions so we use this to represent these
     AsmFunctionDeclaration.setFunctionPrototype        ( "HEADER_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
     AsmFunctionDeclaration.setDataPrototype("std::string","name","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmFunctionDeclaration::function_kind_enum","function_kind","= SgAsmFunctionDeclaration::e_unknown",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
  // AsmFunctionDeclaration.setDataPrototype("int","function_kind","= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  //   AsmFunctionDeclaration.setDataPrototype("SgAsmBlockPtrList","body","",
  //                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("std::string","name_md5","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmStatementPtrList","statementList","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmStatementPtrList","dest","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // These are used as data members in AsmDataStructureDeclaration
     AsmFieldDeclaration.setDataPrototype("std::string","name","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Not clear if we want to store the offset explicitly
     AsmFieldDeclaration.setDataPrototype("unsigned long","offset","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmByteValueExpression.setDataPrototype("unsigned char","value","= 0x0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmWordValueExpression.setDataPrototype("unsigned short","value","= 0x0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDoubleWordValueExpression.setDataPrototype("unsigned int","value","= 0x0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // This should maybe be "unsigned long long"
     AsmQuadWordValueExpression.setDataPrototype("unsigned long long int","value","= 0x0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmSingleFloatValueExpression.setDataPrototype("float","value","= 0.0F",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDoubleFloatValueExpression.setDataPrototype("double","value","= 0.0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmVectorValueExpression.setDataPrototype("unsigned int","size","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // For now we will let this be a SgType, but we have to figure how what the binary types will go in the IR.
  // Also we don't traverse types (they are considered special attributes to the IR nodes that define the AST).
     AsmVectorValueExpression.setDataPrototype("SgAsmType*","type","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Store the expression tree from any constant folding (this can be ignored until later)
     AsmValueExpression.setDataPrototype("SgAsmValueExpression*","unfolded_expression_tree","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // added by tps on 5Apr07
     AsmExpression.setDataPrototype("std::string","replacement","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmBinaryExpression.setDataPrototype("SgAsmExpression*","lhs","= NULL",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmBinaryExpression.setDataPrototype("SgAsmExpression*","rhs","= NULL",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmUnaryExpression.setDataPrototype("SgAsmExpression*","operand","= NULL",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmMemoryReferenceExpression.setFunctionPrototype ( "HEADER_BINARY_MEMORY_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*","address","= NULL",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     // added by tps on 16Jan08
     AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*","segment","= NULL",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // Added by JJW on 2-12-2008
     AsmMemoryReferenceExpression.setDataPrototype("SgAsmType*","type","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmRegisterReferenceExpression.setFunctionPrototype ( "HEADER_BINARY_REGISTER_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     Asmx86RegisterReferenceExpression.setFunctionPrototype ( "HEADER_BINARY_X86_REGISTER_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmArmRegisterReferenceExpression.setFunctionPrototype ( "HEADER_BINARY_ARM_REGISTER_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");

     Asmx86RegisterReferenceExpression.setDataPrototype("X86RegisterClass","register_class","",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Asmx86RegisterReferenceExpression.setDataPrototype("int","register_number","",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Asmx86RegisterReferenceExpression.setDataPrototype("X86PositionInRegister","position_in_register","= x86_regpos_unknown",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     
     AsmArmRegisterReferenceExpression.setDataPrototype("SgAsmArmRegisterReferenceExpression::arm_register_enum","arm_register_code","= SgAsmArmRegisterReferenceExpression::undefined_arm_register",
                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     
  // added by tps on 3Apr07 and removed on 16Jan08
     //     AsmRegisterReferenceExpression.setDataPrototype("SgAsmExpression*","offset","= NULL",
     //                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // Added by JJW on 2-12-2008
     AsmRegisterReferenceExpression.setDataPrototype("SgAsmType*","type","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



  // This might better be an STL std::vector<bool> type.
     AsmControlFlagsExpression.setDataPrototype("unsigned long","bit_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Not sure what to do with this, but if it is in the database we should build them
     AsmCommonSubExpression.setDataPrototype("SgAsmExpression*","subexpression","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // Attach source code to the IR nodes (taken from ../Grammar/BinaryInstruction.code)
     AsmType.setFunctionPrototype            ( "HEADER_BINARY_TYPE", "../Grammar/BinaryInstruction.code");
     AsmTypeByte.setFunctionPrototype        ( "HEADER_BINARY_TYPE_BYTE", "../Grammar/BinaryInstruction.code");
     AsmTypeWord.setFunctionPrototype        ( "HEADER_BINARY_TYPE_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeDoubleWord.setFunctionPrototype  ( "HEADER_BINARY_TYPE_DOUBLE_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeQuadWord.setFunctionPrototype    ( "HEADER_BINARY_TYPE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeDoubleQuadWord.setFunctionPrototype( "HEADER_BINARY_TYPE_DOUBLE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeSingleFloat.setFunctionPrototype ( "HEADER_BINARY_TYPE_SINGLE_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmTypeDoubleFloat.setFunctionPrototype ( "HEADER_BINARY_TYPE_DOUBLE_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmType80bitFloat.setFunctionPrototype  ( "HEADER_BINARY_TYPE_80bit_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmType128bitFloat.setFunctionPrototype ( "HEADER_BINARY_TYPE_128bit_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmTypeVector.setFunctionPrototype      ( "HEADER_BINARY_TYPE_VECTOR", "../Grammar/BinaryInstruction.code");


     AsmExpression.setFunctionPrototype      ( "HEADER_BINARY_EXPRESSION", "../Grammar/BinaryInstruction.code");

     AsmByteValueExpression.setFunctionPrototype( "HEADER_BINARY_BYTE_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmDoubleFloatValueExpression.setFunctionPrototype( "HEADER_BINARY_DOUBLE_FLOAT_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmDoubleWordValueExpression.setFunctionPrototype( "HEADER_BINARY_DOUBLE_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmQuadWordValueExpression.setFunctionPrototype( "HEADER_BINARY_QUAD_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmSingleFloatValueExpression.setFunctionPrototype( "HEADER_BINARY_SINGLE_FLOAT_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmVectorValueExpression.setFunctionPrototype( "HEADER_BINARY_VECTOR_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmWordValueExpression.setFunctionPrototype( "HEADER_BINARY_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");

     AsmNode.setDataPrototype("AstAttributeMechanism*","attributeMechanism","= NULL",
                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmNode.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     AsmNode.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");





  // ***********************************************************************
  // ***********************************************************************
  //                       Source Code Definition
  // ***********************************************************************
  // ***********************************************************************

     AsmBlock.setFunctionSource                    ( "SOURCE_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmOperandList.setFunctionSource              ( "SOURCE_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");
     AsmDataStructureDeclaration.setFunctionSource ( "SOURCE_BINARY_DATA_STRUCTURE", "../Grammar/BinaryInstruction.code");
     AsmInstruction.setFunctionSource              ( "SOURCE_BINARY_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     AsmFunctionDeclaration.setFunctionSource      ( "SOURCE_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
     AsmNode.setFunctionSource                     ( "SOURCE_BINARY_NODE", "../Grammar/BinaryInstruction.code");
     Asmx86Instruction.setFunctionSource           ( "SOURCE_BINARY_X86_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     // AsmArmInstruction.setFunctionSource           ( "SOURCE_BINARY_ARM_INSTRUCTION", "../Grammar/BinaryInstruction.code");

     AsmType.setFunctionSource            ( "SOURCE_BINARY_TYPE", "../Grammar/BinaryInstruction.code");
     AsmTypeByte.setFunctionSource        ( "SOURCE_BINARY_TYPE_BYTE", "../Grammar/BinaryInstruction.code");
     AsmTypeWord.setFunctionSource        ( "SOURCE_BINARY_TYPE_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeDoubleWord.setFunctionSource  ( "SOURCE_BINARY_TYPE_DOUBLE_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeQuadWord.setFunctionSource    ( "SOURCE_BINARY_TYPE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeDoubleQuadWord.setFunctionSource( "SOURCE_BINARY_TYPE_DOUBLE_QUAD_WORD", "../Grammar/BinaryInstruction.code");
     AsmTypeSingleFloat.setFunctionSource ( "SOURCE_BINARY_TYPE_SINGLE_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmTypeDoubleFloat.setFunctionSource ( "SOURCE_BINARY_TYPE_DOUBLE_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmTypeVector.setFunctionSource      ( "SOURCE_BINARY_TYPE_VECTOR", "../Grammar/BinaryInstruction.code");
     AsmType80bitFloat.setFunctionSource  ( "SOURCE_BINARY_TYPE_80bit_FLOAT", "../Grammar/BinaryInstruction.code");
     AsmType128bitFloat.setFunctionSource ( "SOURCE_BINARY_TYPE_128bit_FLOAT", "../Grammar/BinaryInstruction.code");

     AsmExpression.setFunctionSource               ( "SOURCE_BINARY_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmRegisterReferenceExpression.setFunctionSource ( "SOURCE_BINARY_REGISTER_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     Asmx86RegisterReferenceExpression.setFunctionSource ( "SOURCE_BINARY_X86_REGISTER_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmArmRegisterReferenceExpression.setFunctionSource ( "SOURCE_BINARY_ARM_REGISTER_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmMemoryReferenceExpression.setFunctionSource ( "SOURCE_BINARY_MEMORY_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");

     AsmByteValueExpression.setFunctionSource( "SOURCE_BINARY_BYTE_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmDoubleFloatValueExpression.setFunctionSource( "SOURCE_BINARY_DOUBLE_FLOAT_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmDoubleWordValueExpression.setFunctionSource( "SOURCE_BINARY_DOUBLE_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmQuadWordValueExpression.setFunctionSource( "SOURCE_BINARY_QUAD_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmSingleFloatValueExpression.setFunctionSource( "SOURCE_BINARY_SINGLE_FLOAT_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmVectorValueExpression.setFunctionSource( "SOURCE_BINARY_VECTOR_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmWordValueExpression.setFunctionSource( "SOURCE_BINARY_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");


     AsmSectionHeaderList.setFunctionSource ( "SOURCE_BINARY_FILE_SECTION_HEADER_LIST", "../Grammar/BinaryInstruction.code");
     AsmProgramHeaderList.setFunctionSource ( "SOURCE_BINARY_FILE_PROGRAM_HEADER_LIST", "../Grammar/BinaryInstruction.code");

   }
