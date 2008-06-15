
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
                             AsmTypeSingleFloat | AsmTypeDoubleFloat | AsmTypeVector,
                             "AsmType", "AsmTypeTag", false );

#define NEW_ASM_EXECUTABLE_FORMAT_IR_NODES 1
#if NEW_ASM_EXECUTABLE_FORMAT_IR_NODES

  // Derive the SgAsmElfHeader from the SgAsmGenericHeader
     NEW_TERMINAL_MACRO    ( AsmElfHeader,    "AsmElfHeader", "AsmElfHeaderTag" );
     NEW_NONTERMINAL_MACRO ( AsmGenericHeader, AsmElfHeader, "AsmGenericHeader", "AsmGenericHeaderTag", false );

     NEW_TERMINAL_MACRO    ( AsmElfSymbolSection, "AsmElfSymbolSection", "AsmElfSymbolSectionTag" );
     NEW_NONTERMINAL_MACRO ( AsmElfSection, AsmElfSymbolSection, "AsmElfSection",      "AsmElfSectionTag", false );

  // NEW_TERMINAL_MACRO    ( AsmElfSection,      "AsmElfSection",      "AsmElfSectionTag"      );
     NEW_TERMINAL_MACRO    ( AsmElfSectionTable, "AsmElfSectionTable", "AsmElfSectionTableTag" );
     NEW_TERMINAL_MACRO    ( AsmElfSegmentTable, "AsmElfSegmentTable", "AsmElfSegmentTableTag" );
     NEW_NONTERMINAL_MACRO ( AsmGenericSection, AsmElfSection | AsmElfSectionTable | AsmElfSegmentTable,
                             "AsmGenericSection", "AsmGenericSectionTag", false );

  // Generic executable file format supporting IR nodes: ExecSegment, ExecSection, ExecHeader, ExecFile
  // NEW_TERMINAL_MACRO ( AsmGenericSegment,     "AsmGenericSegment",     "AsmGenericSegmentTag" );
  // NEW_TERMINAL_MACRO ( AsmGenericSection,     "AsmGenericSection",     "AsmGenericSectionTag" );
  // NEW_TERMINAL_MACRO ( AsmGenericHeader,      "AsmGenericHeader",      "AsmGenericHeaderTag"  );
     NEW_TERMINAL_MACRO ( AsmGenericFile,        "AsmGenericFile",        "AsmGenericFileTag"    );
     NEW_TERMINAL_MACRO ( AsmGenericFormat,      "AsmGenericFormat",      "AsmGenericFormatTag"    );
     NEW_TERMINAL_MACRO ( AsmGenericArchitecture,"AsmGenericArchitecture","AsmGenericArchitectureTag"    );

     NEW_TERMINAL_MACRO ( AsmGenericSectionList, "AsmGenericSectionList", "AsmGenericSectionListTag" );
     NEW_TERMINAL_MACRO ( AsmGenericSegmentList, "AsmGenericSegmentList", "AsmGenericSegmentListTag" );

     NEW_TERMINAL_MACRO ( AsmElfSegment,         "AsmElfSegment",         "AsmElfSegmentTag" );
     NEW_NONTERMINAL_MACRO ( AsmGenericSegment, AsmElfSegment, "AsmGenericSegment", "AsmGenericSegmentTag", false );

     NEW_NONTERMINAL_MACRO ( AsmGenericSupport, AsmGenericFile        | AsmGenericHeader | AsmGenericSection      | 
                                                AsmGenericSegment     | AsmGenericFormat | AsmGenericArchitecture | 
                                                AsmGenericSectionList | AsmGenericSegmentList,
                             "AsmGenericSupport",    "AsmGenericSupportTag", false );

  // ELF executable file format supporting IR nodes
  // ElfFileHeader, ElfSectionTable, ElfSectionTableEntry, ElfSection, 
  //                ElfSegmentTable, ElfSegmentTableEntry, ElfDynamicSegment, ElfDynamicEntry, 
  // ElfSymbolSection, ElfSymbol
  // NEW_TERMINAL_MACRO ( AsmElfFile,              "AsmElfFile",              "AsmElfFileTag"    );
  // NEW_TERMINAL_MACRO ( AsmElfSectionTable,      "AsmElfSectionTable",      "AsmElfSectionTableTag"    );
     NEW_TERMINAL_MACRO ( AsmElfSectionTableEntry, "AsmElfSectionTableEntry", "AsmElfSectionTableEntryTag"    );
  // NEW_TERMINAL_MACRO ( AsmElfSection,           "AsmElfSection",           "AsmElfSectionTag"    );
  // NEW_TERMINAL_MACRO ( AsmElfSegmentTable,      "AsmElfSegmentTable",      "AsmElfSegmentTableTag"    );
     NEW_TERMINAL_MACRO ( AsmElfSegmentTableEntry, "AsmElfSegmentTableEntry", "AsmElfSectionEntryTag"    );
  // NEW_TERMINAL_MACRO ( AsmElfDynamicSegment,    "AsmElfDynamicSegment",    "AsmElfDynamicSegmentTag"  );
  // NEW_TERMINAL_MACRO ( AsmElfSegment,           "AsmElfDynamicSegment",    "AsmElfDynamicSegmentTag"  );
  // NEW_TERMINAL_MACRO ( AsmElfDynamicEntry,      "AsmElfDynamicEntry",      "AsmElfDynamicEntryTag"    );
     NEW_TERMINAL_MACRO ( AsmElfSegmentEntry,      "AsmElfSegmentEntry",      "AsmElfSegmentEntryTag"    );
  // NEW_TERMINAL_MACRO ( AsmElfSymbolSection,     "AsmElfSymbolSection",     "AsmElfSymbolSectionTag"   );
     NEW_TERMINAL_MACRO ( AsmElfSymbolList,        "AsmElfSymbolList",        "AsmElfSymbolListTag"      );
     NEW_TERMINAL_MACRO ( AsmElfSymbol,            "AsmElfSymbol",            "AsmElfSymbolTag"          );

     NEW_TERMINAL_MACRO ( AsmElfSegmentEntryList, "AsmElfSegmentEntryList", "AsmElfSegmentEntryListTag"  );

     NEW_NONTERMINAL_MACRO ( AsmElfSupport, AsmElfSectionTableEntry | AsmElfSegmentTableEntry | AsmElfSegmentEntry | 
                                            AsmElfSegmentEntryList | AsmElfSymbolList | AsmElfSymbol,
                             "AsmElfSupport", "AsmElfSupportTag", false );

  // Windows PE executable file format supporting IR nodes
     NEW_TERMINAL_MACRO ( AsmPEFile,    "AsmPEFile",    "AsmPEFileTag" );
     NEW_TERMINAL_MACRO ( AsmPESection, "AsmPESection", "AsmPESectionTag" );

     NEW_NONTERMINAL_MACRO ( AsmPESupport, AsmPEFile | AsmPESection, "AsmPESupport",    "AsmPESupportTag", false );

  // Parent of all executable format support IR nodes
     NEW_NONTERMINAL_MACRO ( AsmExecutableFileFormat, AsmGenericSupport | AsmElfSupport | AsmPESupport,
                             "AsmExecutableFileFormat",    "AsmExecutableFileFormatTag", false );

#endif


     NEW_TERMINAL_MACRO ( AsmFile                     , "AsmFile",                     "AsmFileTag" );

  // DQ (1/6/2008): Added ELF program header and section header support to AST.
     NEW_TERMINAL_MACRO ( AsmProgramHeader, "AsmProgramHeader", "AsmProgramHeaderTag" );
     NEW_TERMINAL_MACRO ( AsmSectionHeader, "AsmSectionHeader", "AsmSectionHeaderTag" );

  // DQ (1/6/2008): These store the lists of AsmProgramHeader and AsmSectionHeader objects.
  // AsmFile can not have traversed data members and lists, so this give it two additional data members.
     NEW_TERMINAL_MACRO ( AsmProgramHeaderList, "AsmProgramHeaderList", "AsmProgramHeaderListTag" );
     NEW_TERMINAL_MACRO ( AsmSectionHeaderList, "AsmSectionHeaderList", "AsmSectionHeaderListTag" );

#if NEW_ASM_EXECUTABLE_FORMAT_IR_NODES
  // We will elimiate: AsmFile, AsmProgramHeader, AsmSectionHeader, AsmProgramHeaderList, AsmSectionHeaderList
  // NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmProgramHeader | AsmSectionHeader | AsmProgramHeaderList | AsmSectionHeaderList | AsmOperandList | AsmType | AsmExecutableFileFormat, "AsmNode","AsmNodeTag", false);
     NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmProgramHeader | AsmSectionHeader | AsmProgramHeaderList | AsmSectionHeaderList | AsmOperandList | AsmType | AsmExecutableFileFormat, "AsmNode","AsmNodeTag", false);
#else
  // NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmOperandList | AsmType, "AsmNode","AsmNodeTag");
     NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmProgramHeader | AsmSectionHeader | AsmProgramHeaderList | AsmSectionHeaderList | AsmOperandList | AsmType, "AsmNode","AsmNodeTag", false);
#endif

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
     AsmFile.setDataPrototype("SgAsmGenericHeader*","header","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#if 0
     AsmFile.setDataPrototype("SgAsmFile::elf_machine_architecture_enum", "machine_architecture","= SgAsmFile::e_machine_architecture_error",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("unsigned long", "associated_entry_point","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#else
  // DQ (6/15/2008): These are a part of the older design before using Robb's work
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
#endif

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

#if NEW_ASM_EXECUTABLE_FORMAT_IR_NODES

     AsmElfHeader.setFunctionPrototype ( "HEADER_ELF_HEADER", "../Grammar/BinaryInstruction.code");
  /* Section in which this segment lives */
     AsmElfHeader.setDataPrototype("unsigned char","e_ident_file_class","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned char","e_ident_data_encoding","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned char","e_ident_file_version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("SgCharList","e_ident_padding","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_machine","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_entry","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_phoff","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_shoff","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_ehsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_phentsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_phnum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_shentsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_shnum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("unsigned long","e_shstrndx","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("SgAsmElfSectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfHeader.setDataPrototype("SgAsmElfSegmentTable*","segment_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmElfSectionTable.setFunctionPrototype ( "HEADER_ELF_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfSection.setFunctionPrototype      ( "HEADER_ELF_SECTION",       "../Grammar/BinaryInstruction.code");

     AsmElfSectionTableEntry.setFunctionPrototype ( "HEADER_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_name","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_link","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_info","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_addr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_addralign","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_entsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    /* The ELF header can define a section table entry to be larger than the Elf*SectionTableEntry_disk struct, so any
     * extra data gets stuffed into this member, which is a pointer directly into the mapped file and is null if there
     * is no extra data. */
  // DQ (6/14/2008): I think we can let this be considered an offset into the start of the mapped file (double check on this).
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","extra","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* extra size in bytes */
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","nextra","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmElfSegmentTable.setFunctionPrototype ( "HEADER_ELF_SEGMENT_TABLE", "../Grammar/BinaryInstruction.code");

     AsmElfSegmentTableEntry.setFunctionPrototype ( "HEADER_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");

  // DQ (6/14/2008): Need to rename these data members because ROSETTA will generate "p_p_type" etc. which is unmanageable.
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","p_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","p_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","p_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","p_vaddr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","p_paddr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","p_filesz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","p_memsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","p_align","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    /* The ELF header can define a segment table entry to be larger than the Elf*SegmentTableEntry_disk struct, so any
     * extra data gets stuffed into this member, which is a pointer directly into the mapped file and is null if there
     * is no extra data. */
  // DQ (6/14/2008): I think we can let this be considered an offset into the start of the mapped file (double check on this).
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","extra","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* extra size in bytes */
     AsmElfSegmentTableEntry.setDataPrototype("unsigned long","nextra","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* The pointer to the actual segment on disk */
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmGenericSegment*","segment","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmElfSegment.setFunctionPrototype ( "HEADER_ELF_SEGMENT", "../Grammar/BinaryInstruction.code");
  /* Size in bytes of PLT relocations */
     AsmElfSegment.setDataPrototype("unsigned long","dt_pltrelsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of global offset table */
     AsmElfSegment.setDataPrototype("unsigned long","dt_pltgot","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of symbol hash table */
     AsmElfSegment.setDataPrototype("unsigned long","dt_hash","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of dynamic string table */
     AsmElfSegment.setDataPrototype("unsigned long","dt_strtab","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of symbol table */
     AsmElfSegment.setDataPrototype("unsigned long","dt_symtab","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of Rela relocations */
     AsmElfSegment.setDataPrototype("unsigned long","dt_rela","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Total size in bytes of Rela relocations */
     AsmElfSegment.setDataPrototype("unsigned long","dt_relasz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Size of one Rela relocation */
     AsmElfSegment.setDataPrototype("unsigned long","dt_relaent","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Size in bytes of string table */
     AsmElfSegment.setDataPrototype("unsigned long","dt_strsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Size in bytes of one symbol table entry */
     AsmElfSegment.setDataPrototype("unsigned long","dt_symentsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of initialization function */
     AsmElfSegment.setDataPrototype("unsigned long","dt_init","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of termination function */
     AsmElfSegment.setDataPrototype("unsigned long","dt_fini","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Type of relocation in PLT */
     AsmElfSegment.setDataPrototype("unsigned long","dt_pltrel","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of PLT relocations */
     AsmElfSegment.setDataPrototype("unsigned long","dt_jmprel","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Number of entries in dt_verneed table */
     AsmElfSegment.setDataPrototype("unsigned long","dt_verneednum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of table with needed versions */
     AsmElfSegment.setDataPrototype("unsigned long","dt_verneed","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* GNU version symbol address */
     AsmElfSegment.setDataPrototype("unsigned long","dt_versym","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegment.setDataPrototype("SgAsmElfSegmentEntryList*","other","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
    unsigned            dt_pltrelsz;                    /* Size in bytes of PLT relocations */
    addr_t              dt_pltgot;                      /* Address of global offset table */
    addr_t              dt_hash;                        /* Address of symbol hash table */
    addr_t              dt_strtab;                      /* Address of dynamic string table */
    addr_t              dt_symtab;                      /* Address of symbol table */
    addr_t              dt_rela;                        /* Address of Rela relocations */
    unsigned            dt_relasz;                      /* Total size in bytes of Rela relocations */
    unsigned            dt_relaent;                     /* Size of one Rela relocation */
    unsigned            dt_strsz;                       /* Size in bytes of string table */
    unsigned            dt_symentsz;                    /* Size in bytes of one symbol table entry */
    addr_t              dt_init;                        /* Address of initialization function */
    addr_t              dt_fini;                        /* Address of termination function */
    unsigned            dt_pltrel;                      /* Type of relocation in PLT */
    addr_t              dt_jmprel;                      /* Address of PLT relocations */
    unsigned            dt_verneednum;                  /* Number of entries in dt_verneed table */
    addr_t              dt_verneed;                     /* Address of table with needed versions */
    addr_t              dt_versym;                      /* GNU version symbol address */
    std::vector<ElfDynamicEntry> other;                 /* Other values not specifically parsed out */
#endif

     AsmElfSegmentEntry.setFunctionPrototype ( "HEADER_ELF_SEGMENT_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentEntry.setDataPrototype("unsigned long","d_tag","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentEntry.setDataPrototype("unsigned long","d_val","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
    unsigned            d_tag;
    addr_t              d_val;
#endif

     AsmElfSegmentEntryList.setFunctionPrototype ( "HEADER_ELF_SEGMENT_ENTRY_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentEntryList.setDataPrototype("SgAsmElfSegmentEntryPtrList","segment_entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmElfSymbolSection.setFunctionPrototype      ( "HEADER_ELF_SYMBOL_SECTION",       "../Grammar/BinaryInstruction.code");
     AsmElfSymbolSection.setDataPrototype("unsigned long","x_","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbolSection.setDataPrototype("SgAsmElfSymbolList*","symbols_list","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 0
    std::vector<ElfSymbol> symbols;
#endif

     AsmElfSymbolList.setFunctionPrototype ( "HEADER_ELF_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolList.setDataPrototype("SgAsmElfSymbolPtrList","symbols","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmElfSymbol.setFunctionPrototype      ( "HEADER_ELF_SYMBOL",       "../Grammar/BinaryInstruction.code");
     AsmElfSymbol.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned long","st_name","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned char","st_info","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned char","st_res1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned long","st_shndx","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned long","st_value","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned long","st_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#if 0
    std::string         name;

    /* Members defined by the ELF standard */
    addr_t              st_name;
    unsigned char       st_info, st_res1;
    unsigned            st_shndx;
    addr_t              st_value, st_size;
#endif

  // This data structure represents the ExecSegment from file: ExecGeneric.h
     AsmGenericSegment.setFunctionPrototype ( "HEADER_GENERIC_SEGMENT", "../Grammar/BinaryInstruction.code");
  /* Section in which this segment lives */
     AsmGenericSegment.setDataPrototype("SgAsmGenericSection*","section","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Optional name of segment */
     AsmGenericSegment.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Offset wrt. beginning of the section */
     AsmGenericSegment.setDataPrototype("unsigned long","offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Bytes stored in the file, sans alignment padding */
     AsmGenericSegment.setDataPrototype("unsigned long","disk_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Intended size when mapped to memory */
     AsmGenericSegment.setDataPrototype("unsigned long","mapped_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Byte value (or negative for none) of memory padding */
     AsmGenericSegment.setDataPrototype("int","mapped_padding","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Intended relative virtual address when mapped by loader */
     AsmGenericSegment.setDataPrototype("unsigned long","mapped_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Mapped by loader into memory having write permission */
     AsmGenericSegment.setDataPrototype("bool","writable","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Mapped by loader into memory having execute permission */
     AsmGenericSegment.setDataPrototype("bool","executable","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Need a separate IR node to hold the list of SgAsmGenericSection pointers.
     AsmGenericSegmentList.setDataPrototype("SgAsmGenericSegmentPtrList","segments","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This data structure represents the ExecSection from file: ExecGeneric.h
     AsmGenericSection.setFunctionPrototype ( "HEADER_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");
  /* The file to which this section belongs */
     AsmGenericSection.setDataPrototype("SgAsmGenericFile*","file","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Size of section in bytes */
     AsmGenericSection.setDataPrototype("unsigned long","size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Starting offset of the section */
     AsmGenericSection.setDataPrototype("unsigned long","offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Content of just this section; points into file's content */
  // AsmGenericSection.setDataPrototype("const unsigned char","data","= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("unsigned char","data","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* General contents of the section */
     AsmGenericSection.setDataPrototype("Exec::SectionPurpose","purpose","= Exec::SP_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Section was created by the format reader; not specified in file */
     AsmGenericSection.setDataPrototype("bool","synthesized","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Non-unique section ID (unique for ELF) or negative */
     AsmGenericSection.setDataPrototype("int","id","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Optional, non-unique name of section */
     AsmGenericSection.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* True if section should be mapped to program's address space */
     AsmGenericSection.setDataPrototype("bool","mapped","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Intended relative virtual address if `mapped' is true */
     AsmGenericSection.setDataPrototype("unsigned long","mapped_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* All segments belonging within this section */
     AsmGenericSection.setDataPrototype("SgAsmGenericSegmentPtrListPtr","segmentsList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Need a separate IR node to hold the list of SgAsmGenericSection pointers.
     AsmGenericSectionList.setDataPrototype("SgAsmGenericSectionPtrList","sections","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This data structure represents the ExecHeader from file: ExecGeneric.h
     AsmGenericHeader.setFunctionPrototype ( "HEADER_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");
  /* General info about the executable format */
     AsmGenericHeader.setDataPrototype("SgAsmGenericFormat*","fileFormat","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Machine for which this header and its sections, etc. was compiled */
     AsmGenericHeader.setDataPrototype("SgAsmGenericArchitecture*","target","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  /* Optional magic number in file byte order */
     AsmGenericHeader.setDataPrototype("SgCharList","magic","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Base virtual address used by all "relative virtual addresses" (RVA) */
     AsmGenericHeader.setDataPrototype("unsigned long","base_va","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Code entry point wrt base_va */
     AsmGenericHeader.setDataPrototype("unsigned long","entry_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This data structure represents the ExecFile from file: ExecGeneric.h
     AsmGenericFile.setFunctionPrototype ( "HEADER_GENERIC_FILE", "../Grammar/BinaryInstruction.code");
  /* File descriptor opened for read-only (or negative) */
     AsmGenericFile.setDataPrototype("int","fd","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (6/14/2008): This data member's type is not yet correctly implemented needs to reference stat or stat64.  
  // Need to discuss this with Robb.
  /* File attributes at time of file open (valid if fd>=0) */
     AsmGenericFile.setDataPrototype("unsigned long","sb","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  /* Content of file mapped into memory   (or null on file error) */
     AsmGenericFile.setDataPrototype("SgCharList","data","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* All known sections for this file */
     AsmGenericFile.setDataPrototype("SgAsmGenericSectionPtrListPtr","sections","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This data structure represents the ExecFile from file: ExecGeneric.h
     AsmGenericFormat.setFunctionPrototype ( "HEADER_GENERIC_FORMAT", "../Grammar/BinaryInstruction.code");
  /* General format: ELF, PE, etc. */
     AsmGenericFormat.setDataPrototype("Exec::ExecFamily","family","= Exec::FAMILY_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* executable, library, etc. */
     AsmGenericFormat.setDataPrototype("Exec::ExecPurpose","purpose","= Exec::PURPOSE_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* No comment available */
     AsmGenericFormat.setDataPrototype("Exec::ByteOrder","sex","= Exec::ORDER_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* actual file format version number stored in file */
     AsmGenericFormat.setDataPrototype("unsigned long","version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* is 'version' considered to be the current, supported version */
     AsmGenericFormat.setDataPrototype("bool","is_current_version","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* application binary interface */
     AsmGenericFormat.setDataPrototype("Exec::ExecABI","abi","= Exec::ABI_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* version of the ABI targeted by this file */
     AsmGenericFormat.setDataPrototype("unsigned long","abi_version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* in bytes (e.g., Elf32 is 4; Elf64 is 8) */
     AsmGenericFormat.setDataPrototype("unsigned long","word_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This data structure represents the ExecFile from file: ExecGeneric.h
     AsmGenericArchitecture.setFunctionPrototype ( "HEADER_GENERIC_ARCHITECTURE", "../Grammar/BinaryInstruction.code");
  /* Instruction set architecture */
     AsmGenericArchitecture.setDataPrototype("Exec::InsSetArchitecture","isa","= Exec::ISA_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Actual stored value if isa==ISA_OTHER */
     AsmGenericFormat.setDataPrototype("unsigned long","other","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

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
