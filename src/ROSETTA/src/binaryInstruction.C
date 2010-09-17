/*-------------------------------------------------------------------------------------------------------------------------------
 * NOTICE: Some of these classes and their data members are documented in *.docs files in docs/testDoxygen where '*' is the
 *         name of the class.  Be sure to update that documentation if you change this file!  The practice used in this file
 *         is that doxygen-documented entities are marked here with a "doxygen" comment as a reminder.
 *-----------------------------------------------------------------------------------------------------------------------------*/

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



     NEW_TERMINAL_MACRO(AsmPowerpcInstruction, "AsmPowerpcInstruction", "AsmPowerpcInstructionTag" );
     AsmPowerpcInstruction.setFunctionPrototype("HEADER_BINARY_POWERPC_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     AsmPowerpcInstruction.setDataPrototype("PowerpcInstructionKind", "kind", "= powerpc_unknown_instruction",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmPowerpcInstruction.setPredeclarationString("HEADER_BINARY_POWERPC_INSTRUCTION_PREDECLARATION",
                                                   "../Grammar/BinaryInstruction.code");



  // This is currently a AsmFunctionCall plus other unspecified uses of Asmx86Instruction, it may be refied later.
     NEW_NONTERMINAL_MACRO(AsmInstruction,
                           Asmx86Instruction | AsmArmInstruction | AsmPowerpcInstruction,
                           "AsmInstruction", "AsmInstructionTag", true );



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

     NEW_NONTERMINAL_MACRO ( AsmBinaryExpression, AsmBinaryAdd               | AsmBinarySubtract      | AsmBinaryMultiply           |
                                                  AsmBinaryDivide            | AsmBinaryMod           | AsmBinaryAddPreupdate       |
                                                  AsmBinarySubtractPreupdate | AsmBinaryAddPostupdate | AsmBinarySubtractPostupdate |
                                                  AsmBinaryLsl               | AsmBinaryLsr           | AsmBinaryAsr                |
                                                  AsmBinaryRor, "AsmBinaryExpression", "AsmBinaryExpressionTag", false );

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
     NEW_TERMINAL_MACRO ( AsmPowerpcRegisterReferenceExpression , "AsmPowerpcRegisterReferenceExpression", "AsmPowerpcRegisterReferenceExpressionTag" );
     NEW_NONTERMINAL_MACRO ( AsmRegisterReferenceExpression ,
                             Asmx86RegisterReferenceExpression | AsmArmRegisterReferenceExpression | AsmPowerpcRegisterReferenceExpression,
                            "AsmRegisterReferenceExpression", "AsmRegisterReferenceExpressionTag" , false);

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
                             AsmCommonSubExpression | AsmExprListExp, "AsmExpression", "AsmExpressionTag", false );

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
     AsmTypeByte.setDataPrototype           ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeWord.setDataPrototype           ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeDoubleWord.setDataPrototype     ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeQuadWord.setDataPrototype       ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeDoubleQuadWord.setDataPrototype ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeSingleFloat.setDataPrototype    ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeDoubleFloat.setDataPrototype    ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmTypeVector.setDataPrototype         ("int","elementCount","= 0",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmTypeVector.setDataPrototype         ("SgAsmType*","elementType","= NULL",CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmType80bitFloat.setDataPrototype     ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmType128bitFloat.setDataPrototype    ("static $CLASSNAME*","builtin_type","",NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL || TYPE_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     NEW_NONTERMINAL_MACRO ( AsmType, AsmTypeByte        | AsmTypeWord           | AsmTypeDoubleWord  |
                                      AsmTypeQuadWord    | AsmTypeDoubleQuadWord | AsmType80bitFloat  |
                                      AsmType128bitFloat | AsmTypeSingleFloat    | AsmTypeDoubleFloat |
                                      AsmTypeVector, "AsmType", "AsmTypeTag", false /* canHaveInstances = false */ );

     NEW_TERMINAL_MACRO ( AsmGenericDLL,           "AsmGenericDLL",           "AsmGenericDLLTag");
     NEW_TERMINAL_MACRO ( AsmPEImportHNTEntryList, "AsmPEImportHNTEntryList", "AsmPEImportHNTEntryListTag" );
     NEW_TERMINAL_MACRO ( AsmPEImportILTEntryList, "AsmPEImportILTEntryList", "AsmPEImportILTEntryListTag" );
     NEW_TERMINAL_MACRO ( AsmPEImportDirectoryList, "AsmPEImportDirectoryList", "AsmPEImportDirectoryListTag" );

     NEW_TERMINAL_MACRO ( AsmGenericFormat,      "AsmGenericFormat",      "AsmGenericFormatTag" );

     NEW_TERMINAL_MACRO ( AsmGenericFile,        "AsmGenericFile",        "AsmGenericFileTag" );

  // Support for different types of header for binary executable file formats.
     NEW_TERMINAL_MACRO ( AsmElfFileHeader,      "AsmElfFileHeader",      "AsmElfFileHeaderTag"   );
     NEW_TERMINAL_MACRO ( AsmPEFileHeader,       "AsmPEFileHeader",       "AsmPEFileHeaderTag"    );
     NEW_TERMINAL_MACRO ( AsmNEFileHeader,       "AsmNEFileHeader",       "AsmNEFileHeaderTag"    );
     NEW_TERMINAL_MACRO ( AsmLEFileHeader,       "AsmLEFileHeader",       "AsmLEFileHeaderTag"    );
     NEW_TERMINAL_MACRO ( AsmDOSFileHeader,      "AsmDOSFileHeader",      "AsmDOSFileHeaderTag"    );
     NEW_NONTERMINAL_MACRO ( AsmGenericHeader, AsmPEFileHeader  | AsmLEFileHeader |  AsmNEFileHeader | AsmDOSFileHeader |  AsmElfFileHeader, "AsmGenericHeader",    "AsmGenericHeaderTag", true /* canHaveInstances = true */ );

  // A lot of IR nodes are derived from the AsmGenericSection.
     NEW_TERMINAL_MACRO(AsmElfRelocSection,   "AsmElfRelocSection",   "AsmElfRelocSectionTag");
     NEW_TERMINAL_MACRO(AsmElfDynamicSection, "AsmElfDynamicSection", "AsmElfDynamicSectionTag");
     NEW_TERMINAL_MACRO(AsmElfSymbolSection,  "AsmElfSymbolSection",  "AsmElfSymbolSectionTag");
     NEW_TERMINAL_MACRO(AsmElfStringSection,  "AsmElfStringSection",  "AsmElfStringSectionTag");
     NEW_TERMINAL_MACRO(AsmElfEHFrameSection, "AsmElfEHFrameSection", "AsmElfEHFrameSection");
     NEW_TERMINAL_MACRO(AsmElfNoteSection,    "AsmElfNoteSection",    "AsmElfNoteSection");
     NEW_TERMINAL_MACRO(AsmElfSymverSection,  "AsmElfSymverSection",  "AsmElfSymverSection");
     NEW_TERMINAL_MACRO(AsmElfSymverDefinedSection,"AsmElfSymverDefinedSection","AsmElfSymverDefinedSection");
     NEW_TERMINAL_MACRO(AsmElfSymverNeededSection,"AsmElfSymverNeededSection","AsmElfSymverNeededSection");

  // DQ (9/9/2008): Added support for String Table (part of Robb's work)
     NEW_TERMINAL_MACRO ( AsmElfStrtab,          "AsmElfStrtab",          "AsmElfStrtabTag"          );
     NEW_TERMINAL_MACRO ( AsmCoffStrtab,         "AsmCoffStrtab",         "AsmCoffStrtabTag"         );
     NEW_NONTERMINAL_MACRO( AsmGenericStrtab, AsmElfStrtab | AsmCoffStrtab, "AsmGenericStrtab", "AsmGenericStrtabTag", false);

     NEW_NONTERMINAL_MACRO ( AsmElfSection,
                             AsmElfSymbolSection | AsmElfRelocSection | AsmElfDynamicSection | AsmElfStringSection |
                             AsmElfNoteSection   | AsmElfEHFrameSection | 
                             AsmElfSymverSection | AsmElfSymverDefinedSection | AsmElfSymverNeededSection,
                             "AsmElfSection", "AsmElfSectionTag", true /* canHaveInstances = true */ );

     NEW_TERMINAL_MACRO    ( AsmElfSectionTable,  "AsmElfSectionTable",  "AsmElfSectionTableTag"  );
     NEW_TERMINAL_MACRO    ( AsmElfSegmentTable,  "AsmElfSegmentTable",  "AsmElfSegmentTableTag"  );

     NEW_TERMINAL_MACRO    ( AsmPEImportSection, "AsmPEImportSection", "AsmPEImportSectionTag" );
     NEW_TERMINAL_MACRO    ( AsmPEExportSection, "AsmPEExportSection", "AsmPEExportSectionTag" );
     NEW_TERMINAL_MACRO    ( AsmPEStringSection, "AsmPEStringSection", "AsmPEStringSectionTag");
     NEW_NONTERMINAL_MACRO ( AsmPESection, AsmPEImportSection | AsmPEExportSection | AsmPEStringSection,
                             "AsmPESection",       "AsmPESectionTag", true /* canHaveInstances = true */ );

     NEW_TERMINAL_MACRO    ( AsmPESectionTable,  "AsmPESectionTable",  "AsmPESectionTableTag"  );
     NEW_TERMINAL_MACRO    ( AsmCoffSymbolTable, "AsmCoffSymbolTable", "AsmCoffSymbolTableTag" );
     NEW_TERMINAL_MACRO    ( AsmDOSExtendedHeader, "AsmDOSExtendedHeader", "AsmDOSExtendedHeaderTag" );

     NEW_TERMINAL_MACRO    ( AsmNESection,           "AsmNESection",           "AsmNESectionTag"      );
     NEW_TERMINAL_MACRO    ( AsmNESectionTable,      "AsmNESectionTable",      "AsmNESectionTableTag" );
     NEW_TERMINAL_MACRO    ( AsmNENameTable,         "AsmNENameTable",         "AsmNENameTableTag"    );
     NEW_TERMINAL_MACRO    ( AsmNEModuleTable,       "AsmNEModuleTable",       "AsmNEModuleTableTag"  );
     NEW_TERMINAL_MACRO    ( AsmNEStringTable,       "AsmNEStringTable",       "AsmNEStringTableTag"  );
     NEW_TERMINAL_MACRO    ( AsmNEEntryTable,        "AsmNEEntryTable",        "AsmNEEntryTableTag"   );
     NEW_TERMINAL_MACRO    ( AsmNERelocTable,        "AsmNERelocTable",        "AsmNERelocTableTag"   );

     NEW_TERMINAL_MACRO    ( AsmLESection,      "AsmLESection",      "AsmLESectionTag"      );
     NEW_TERMINAL_MACRO    ( AsmLESectionTable, "AsmLESectionTable", "AsmLESectionTableTag" );
     NEW_TERMINAL_MACRO    ( AsmLENameTable,    "AsmLENameTable",    "AsmLENameTableTag"    );
     NEW_TERMINAL_MACRO    ( AsmLEPageTable,    "AsmLEPageTable",    "AsmLEPageTableTag"    );
     NEW_TERMINAL_MACRO    ( AsmLEEntryTable,   "AsmLEEntryTable",   "AsmLEEntryTableTag"   );
     NEW_TERMINAL_MACRO    ( AsmLERelocTable,   "AsmLERelocTable",   "AsmLERelocTableTag"   );

     NEW_NONTERMINAL_MACRO ( AsmGenericSection,
            AsmGenericHeader |
            AsmElfSection    | AsmElfSectionTable | AsmElfSegmentTable     |
            AsmPESection     | AsmPESectionTable  | AsmDOSExtendedHeader   | AsmCoffSymbolTable  |
            AsmNESection     | AsmNESectionTable  | AsmNENameTable         | AsmNEModuleTable    |
            AsmNEStringTable | AsmNEEntryTable    | AsmNERelocTable        |
            AsmLESection     | AsmLESectionTable  | AsmLENameTable         | AsmLEPageTable      | AsmLEEntryTable  | AsmLERelocTable,
           "AsmGenericSection",    "AsmGenericSectionTag", true /* canHaveInstances = true */ );

  // Support for Symbols in the binary executable.
     NEW_TERMINAL_MACRO    ( AsmCoffSymbol,   "AsmCoffSymbol", "AsmCoffSymbolTag"    );
     NEW_TERMINAL_MACRO    ( AsmElfSymbol,    "AsmElfSymbol",  "AsmElfSymbolTag"    );
     NEW_NONTERMINAL_MACRO ( AsmGenericSymbol, AsmCoffSymbol  | AsmElfSymbol, "AsmGenericSymbol",    "AsmGenericSymbolTag", false /* canHaveInstances = false */ );

  // These are supporting IR nodes to be gathered into (derived from) the AsmGenericBinaryFileSupport class
     NEW_TERMINAL_MACRO    ( AsmElfSectionTableEntry,     "AsmElfSectionTableEntry",     "AsmElfSectionTableEntryTag"     );
     NEW_TERMINAL_MACRO    ( AsmElfSegmentTableEntry,     "AsmElfSegmentTableEntry",     "AsmElfSegmentTableEntryTag"     );
     NEW_TERMINAL_MACRO    ( AsmElfSegmentTableEntryList, "AsmElfSegmentTableEntryList", "AsmElfSegmentTableEntryListTag" );
     NEW_TERMINAL_MACRO    ( AsmElfRelocEntry,            "AsmElfRelocEntry",            "AsmElfRelocEntryTag"            );
     NEW_TERMINAL_MACRO    ( AsmElfRelocEntryList,        "AsmElfRelocEntryList",        "AsmElfRelocEntryListTag"        );
     NEW_TERMINAL_MACRO    ( AsmElfDynamicEntry,          "AsmElfDynamicEntry",          "AsmElfDynamicEntryTag"          );
     NEW_TERMINAL_MACRO    ( AsmElfDynamicEntryList,      "AsmElfDynamicEntryList",      "AsmElfDynamicEntryListTag"      );
     NEW_TERMINAL_MACRO    ( AsmElfEHFrameEntryCI,        "AsmElfEHFrameEntryCI",        "AsmElfEHFrameEntryCITag"        );
     NEW_TERMINAL_MACRO    ( AsmElfEHFrameEntryCIList,    "AsmElfEHFrameEntryCIList",    "AsmElfEHFrameEntryCIListTag"    );
     NEW_TERMINAL_MACRO    ( AsmElfEHFrameEntryFD,        "AsmElfEHFrameEntryFD",        "AsmElfEHFrameEntryFDTag"        );
     NEW_TERMINAL_MACRO    ( AsmElfEHFrameEntryFDList,    "AsmElfEHFrameEntryFDList",    "AsmElfEHFrameEntryFDListTag"    );
     NEW_TERMINAL_MACRO    ( AsmElfNoteEntry,             "AsmElfNoteEntry",             "AsmElfNoteEntryTag"             );
     NEW_TERMINAL_MACRO    ( AsmElfNoteEntryList,         "AsmElfNoteEntryList",         "AsmElfNoteEntryListTag"         );
     NEW_TERMINAL_MACRO    ( AsmElfSymverEntry,    "AsmElfSymverEntry",    "AsmElfSymverEntryTag"    );
     NEW_TERMINAL_MACRO    ( AsmElfSymverEntryList,"AsmElfSymverEntryList","AsmElfSymverEntryListTag");
     NEW_TERMINAL_MACRO    ( AsmElfSymverDefinedEntry,             "AsmElfSymverDefinedEntry",             "AsmElfSymverDefinedEntryTag"             );
     NEW_TERMINAL_MACRO    ( AsmElfSymverDefinedEntryList,         "AsmElfSymverDefinedEntryList",         "AsmElfSymverDefinedEntryListTag"         );
     NEW_TERMINAL_MACRO    ( AsmElfSymverDefinedAux,             "AsmElfSymverDefinedAux",             "AsmElfSymverDefinedAuxTag"             );
     NEW_TERMINAL_MACRO    ( AsmElfSymverDefinedAuxList,         "AsmElfSymverDefinedAuxList",         "AsmElfSymverDefinedAuxListTag"         );
     NEW_TERMINAL_MACRO    ( AsmElfSymverNeededEntry,             "AsmElfSymverNeededEntry",             "AsmElfSymverNeededEntryTag"             );
     NEW_TERMINAL_MACRO    ( AsmElfSymverNeededEntryList,         "AsmElfSymverNeededEntryList",         "AsmElfSymverNeededEntryListTag"         );
     NEW_TERMINAL_MACRO    ( AsmElfSymverNeededAux,             "AsmElfSymverNeededAux",             "AsmElfSymverNeededAuxTag"             );
     NEW_TERMINAL_MACRO    ( AsmElfSymverNeededAuxList,         "AsmElfSymverNeededAuxList",         "AsmElfSymverNeededAuxListTag"         );

     NEW_TERMINAL_MACRO    ( AsmPERVASizePair,       "AsmPERVASizePair",       "AsmPERVASizePairTag"       );
     NEW_TERMINAL_MACRO    ( AsmPEExportDirectory,   "AsmPEExportDirectory",   "AsmPEExportDirectoryTag"   );
     NEW_TERMINAL_MACRO    ( AsmPEExportEntry,       "AsmPEExportEntry",       "AsmPEExportEntryTag"       );
     NEW_TERMINAL_MACRO    ( AsmPEImportDirectory,   "AsmPEImportDirectory",   "AsmPEImportDirectoryTag"   );
     NEW_TERMINAL_MACRO    ( AsmPEImportILTEntry,    "AsmPEImportILTEntry",    "AsmPEImportILTEntryTag"    );
     NEW_TERMINAL_MACRO    ( AsmPEImportHNTEntry,    "AsmPEImportHNTEntry",    "AsmPEImportHNTEntryTag"    );
     NEW_TERMINAL_MACRO    ( AsmPEImportLookupTable, "AsmPEImportLookupTable", "AsmPEImportLookupTableTag" );
     NEW_TERMINAL_MACRO    ( AsmPESectionTableEntry, "AsmPESectionTableEntry", "AsmPESectionTableEntryTag" );

     NEW_TERMINAL_MACRO    ( AsmNEEntryPoint,        "AsmNEEntryPoint",        "AsmNEEntryPointTag"        );
     NEW_TERMINAL_MACRO    ( AsmNERelocEntry,        "AsmNERelocEntry",        "AsmNERelocEntryTag"        );
     NEW_TERMINAL_MACRO    ( AsmNESectionTableEntry, "AsmNESectionTableEntry", "AsmNESectionTableEntryTag" );

     NEW_TERMINAL_MACRO    ( AsmLEPageTableEntry,    "AsmLEPageTableEntry",   "AsmLEPageTableEntryTag"     );
     NEW_TERMINAL_MACRO    ( AsmLEEntryPoint,        "AsmLEEntryPoint",       "AsmLEEntryPointTag"         );
     NEW_TERMINAL_MACRO    ( AsmLESectionTableEntry, "AsmLESectionTableEntry","AsmLESectionTableEntryTag"  );

  // List objects required because IR design does not mix children with list elements.
     NEW_TERMINAL_MACRO ( AsmGenericSectionList, "AsmGenericSectionList", "AsmGenericSectionListTag" );
     NEW_TERMINAL_MACRO ( AsmGenericHeaderList,  "AsmGenericHeaderList",  "AsmGenericHeaderListTag"  );
     NEW_TERMINAL_MACRO ( AsmGenericSymbolList,  "AsmGenericSymbolList",  "AsmGenericSymbolListTag"  );
     NEW_TERMINAL_MACRO ( AsmElfSymbolList,      "AsmElfSymbolList",      "AsmElfSymbolListTag"      );
     NEW_TERMINAL_MACRO ( AsmCoffSymbolList,     "AsmCoffSymbolList",     "AsmCoffSymbolListTag"     );
     NEW_TERMINAL_MACRO ( AsmGenericDLLList,     "AsmGenericDLLList",     "AsmGenericDLLListTag"     );
     NEW_TERMINAL_MACRO ( AsmPERVASizePairList,  "AsmPERVASizePairList",  "AsmPERVASizePairListTag"  );
     NEW_TERMINAL_MACRO (AsmPEExportEntryList,   "AsmPEExportEntryList",  "AsmPEExportEntryListTag"  );


  // DQ (8/2/2008): These were removed from the design by Robb (segments and sections are now the same: as sections).
  // NEW_TERMINAL_MACRO ( AsmGenericSegmentList, "AsmGenericSegmentList", "AsmGenericSegmentListTag" );
  // NEW_TERMINAL_MACRO ( AsmElfSegment,         "AsmElfSegment",         "AsmElfSegmentTag" );
  // NEW_NONTERMINAL_MACRO ( AsmGenericSegment, AsmElfSegment, "AsmGenericSegment", "AsmGenericSegmentTag", false );
  // NEW_NONTERMINAL_MACRO ( AsmGenericSupport, AsmGenericFile        | AsmGenericHeader | AsmGenericSection      |
  //                                            AsmGenericSegment     | AsmGenericFormat |
  //                                            AsmGenericSectionList | AsmGenericSegmentList,
  //                         "AsmGenericSupport",    "AsmGenericSupportTag", false );

#if 1
  // New IR nodes to support string table transformations in Binary
     NEW_TERMINAL_MACRO ( AsmBasicString,        "AsmBasicString",        "AsmBasicStringTag"        );
     NEW_TERMINAL_MACRO ( AsmStoredString,       "AsmStoredString",       "AsmStoredStringTag"       );
     NEW_NONTERMINAL_MACRO ( AsmGenericString, AsmBasicString | AsmStoredString, "AsmGenericString", "AsmGenericStringTag", false /* canHaveInstances = false */ );

     NEW_TERMINAL_MACRO ( AsmStringStorage,   "AsmStringStorage",   "AsmStringStorageTag"   );
#endif

// DQ (11/3/2008): Later this might go into a separate file.
// ***************************************************************************************
// DWARF Support for mapping information in the binary executable back to the source code.
// ***************************************************************************************

     NEW_TERMINAL_MACRO ( AsmDwarfMacro, "AsmDwarfMacro", "AsmDwarfMacroTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfLine,  "AsmDwarfLine",  "AsmDwarfLineTag"  );
     NEW_TERMINAL_MACRO ( AsmDwarfMacroList, "AsmDwarfMacroList", "AsmDwarfMacroListTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfLineList,  "AsmDwarfLineList",  "AsmDwarfLineListTag"  );

  // These are a subset (but most of) the possible Dwarf Tags, I have skipped some of the
  // architecture specific tags (except for GNU).  There are about 63 new IR nodes here.
  // These are kinds of language constructs that are stored in the ".debug_info" section
  // when dwarf debug information is available (generated by the compiler) in the binary
  // executable.
     NEW_TERMINAL_MACRO ( AsmDwarfArrayType, "AsmDwarfArrayType", "AsmDwarfArrayTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfClassType, "AsmDwarfClassType", "AsmDwarfClassTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfEntryPoint, "AsmDwarfEntryPoint", "AsmDwarfEntryPointTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfEnumerationType, "AsmDwarfEnumerationType", "AsmDwarfEnumerationTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfFormalParameter, "AsmDwarfFormalParameter", "AsmDwarfFormalParameterTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfImportedDeclaration, "AsmDwarfImportedDeclaration", "AsmDwarfImportedDeclarationTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfLabel, "AsmDwarfLabel", "AsmDwarfLabelTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfLexicalBlock, "AsmDwarfLexicalBlock", "AsmDwarfLexicalBlockTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfMember, "AsmDwarfMember", "AsmDwarfMemberTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfPointerType, "AsmDwarfPointerType", "AsmDwarfPointerTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfReferenceType, "AsmDwarfReferenceType", "AsmDwarfReferenceTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfCompilationUnit,  "AsmDwarfCompilationUnit",  "AsmDwarfCompilationUnitTag"  );
     NEW_TERMINAL_MACRO ( AsmDwarfStringType, "AsmDwarfStringType", "AsmDwarfStringTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfStructureType, "AsmDwarfStructureType", "AsmDwarfStructureTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfSubroutineType, "AsmDwarfSubroutineType", "AsmDwarfSubroutineTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfTypedef, "AsmDwarfTypedef", "AsmDwarfTypedefTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfUnionType, "AsmDwarfUnionType", "AsmDwarfUnionTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfUnspecifiedParameters, "AsmDwarfUnspecifiedParameters", "AsmDwarfUnspecifiedParametersTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfVariant, "AsmDwarfVariant", "AsmDwarfVariantTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfCommonBlock, "AsmDwarfCommonBlock", "AsmDwarfCommonBlockTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfCommonInclusion, "AsmDwarfCommonInclusion", "AsmDwarfCommonInclusionTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfInheritance, "AsmDwarfInheritance", "AsmDwarfInheritanceTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfInlinedSubroutine, "AsmDwarfInlinedSubroutine", "AsmDwarfInlinedSubroutineTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfModule, "AsmDwarfModule", "AsmDwarfModuleTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfPtrToMemberType, "AsmDwarfPtrToMemberType", "AsmDwarfPtrToMemberTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfSetType, "AsmDwarfSetType", "AsmDwarfSetTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfSubrangeType, "AsmDwarfSubrangeType", "AsmDwarfSubrangeTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfWithStmt, "AsmDwarfWithStmt", "AsmDwarfWithStmtTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfAccessDeclaration, "AsmDwarfAccessDeclaration", "AsmDwarfAccessDeclarationTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfBaseType, "AsmDwarfBaseType", "AsmDwarfBaseTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfCatchBlock, "AsmDwarfCatchBlock", "AsmDwarfCatchBlockTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfConstType, "AsmDwarfConstType", "AsmDwarfConstTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfConstant, "AsmDwarfConstant", "AsmDwarfConstantTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfEnumerator, "AsmDwarfEnumerator", "AsmDwarfEnumeratorTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfFileType, "AsmDwarfFileType", "AsmDwarfFileTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfFriend, "AsmDwarfFriend", "AsmDwarfFriendTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfNamelist, "AsmDwarfNamelist", "AsmDwarfNamelistTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfNamelistItem, "AsmDwarfNamelistItem", "AsmDwarfNamelistItemTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfPackedType, "AsmDwarfPackedType", "AsmDwarfPackedTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfSubprogram, "AsmDwarfSubprogram", "AsmDwarfSubprogramTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfTemplateTypeParameter, "AsmDwarfTemplateTypeParameter", "AsmDwarfTemplateTypeParameterTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfTemplateValueParameter, "AsmDwarfTemplateValueParameter", "AsmDwarfTemplateValueParameterTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfThrownType, "AsmDwarfThrownType", "AsmDwarfThrownTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfTryBlock, "AsmDwarfTryBlock", "AsmDwarfTryBlockTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfVariantPart, "AsmDwarfVariantPart", "AsmDwarfVariantPartTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfVariable, "AsmDwarfVariable", "AsmDwarfVariableTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfVolatileType, "AsmDwarfVolatileType", "AsmDwarfVolatileTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfDwarfProcedure, "AsmDwarfDwarfProcedure", "AsmDwarfDwarfProcedureTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfRestrictType, "AsmDwarfRestrictType", "AsmDwarfRestrictTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfInterfaceType, "AsmDwarfInterfaceType", "AsmDwarfInterfaceTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfNamespace, "AsmDwarfNamespace", "AsmDwarfNamespaceTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfImportedModule, "AsmDwarfImportedModule", "AsmDwarfImportedModuleTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfUnspecifiedType, "AsmDwarfUnspecifiedType", "AsmDwarfUnspecifiedTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfPartialUnit, "AsmDwarfPartialUnit", "AsmDwarfPartialUnitTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfImportedUnit, "AsmDwarfImportedUnit", "AsmDwarfImportedUnitTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfMutableType, "AsmDwarfMutableType", "AsmDwarfMutableTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfCondition, "AsmDwarfCondition", "AsmDwarfConditionTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfSharedType, "AsmDwarfSharedType", "AsmDwarfSharedTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfFormatLabel, "AsmDwarfFormatLabel", "AsmDwarfFormatLabelTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfFunctionTemplate, "AsmDwarfFunctionTemplate", "AsmDwarfFunctionTemplateTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfClassTemplate, "AsmDwarfClassTemplate", "AsmDwarfClassTemplateTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfUpcSharedType, "AsmDwarfUpcSharedType", "AsmDwarfUpcSharedTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfUpcStrictType, "AsmDwarfUpcStrictType", "AsmDwarfUpcStrictTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfUpcRelaxedType, "AsmDwarfUpcRelaxedType", "AsmDwarfUpcRelaxedTypeTag" );
     NEW_TERMINAL_MACRO ( AsmDwarfUnknownConstruct, "AsmDwarfUnknownConstruct", "AsmDwarfUnknownConstructTag" );

     NEW_NONTERMINAL_MACRO ( AsmDwarfConstruct,
               AsmDwarfArrayType | AsmDwarfClassType | AsmDwarfEntryPoint | AsmDwarfEnumerationType | AsmDwarfFormalParameter | AsmDwarfImportedDeclaration |
               AsmDwarfLabel | AsmDwarfLexicalBlock | AsmDwarfMember | AsmDwarfPointerType | AsmDwarfReferenceType | AsmDwarfCompilationUnit | AsmDwarfStringType |
               AsmDwarfStructureType | AsmDwarfSubroutineType | AsmDwarfTypedef | AsmDwarfUnionType | AsmDwarfUnspecifiedParameters | AsmDwarfVariant |
               AsmDwarfCommonBlock | AsmDwarfCommonInclusion | AsmDwarfInheritance | AsmDwarfInlinedSubroutine | AsmDwarfModule | AsmDwarfPtrToMemberType |
               AsmDwarfSetType | AsmDwarfSubrangeType | AsmDwarfWithStmt | AsmDwarfAccessDeclaration | AsmDwarfBaseType | AsmDwarfCatchBlock | AsmDwarfConstType |
               AsmDwarfConstant | AsmDwarfEnumerator | AsmDwarfFileType | AsmDwarfFriend | AsmDwarfNamelist | AsmDwarfNamelistItem | AsmDwarfPackedType |
               AsmDwarfSubprogram | AsmDwarfTemplateTypeParameter | AsmDwarfTemplateValueParameter | AsmDwarfThrownType | AsmDwarfTryBlock | AsmDwarfVariantPart |
               AsmDwarfVariable | AsmDwarfVolatileType | AsmDwarfDwarfProcedure | AsmDwarfRestrictType | AsmDwarfInterfaceType | AsmDwarfNamespace |
               AsmDwarfImportedModule | AsmDwarfUnspecifiedType | AsmDwarfPartialUnit | AsmDwarfImportedUnit | AsmDwarfMutableType | AsmDwarfCondition |
               AsmDwarfSharedType | AsmDwarfFormatLabel | AsmDwarfFunctionTemplate | AsmDwarfClassTemplate | AsmDwarfUpcSharedType | AsmDwarfUpcStrictType |
               AsmDwarfUpcRelaxedType | AsmDwarfUnknownConstruct, "AsmDwarfConstruct", "AsmDwarfConstructTag", false /* canHaveInstances = false */ );

     NEW_TERMINAL_MACRO ( AsmDwarfConstructList, "AsmDwarfConstructList", "AsmDwarfConstructListTag" );

     NEW_TERMINAL_MACRO ( AsmDwarfCompilationUnitList, "AsmDwarfCompilationUnitList",  "AsmDwarfCompilationUnitListTag"  );

     NEW_NONTERMINAL_MACRO ( AsmDwarfInformation,
               AsmDwarfMacro | AsmDwarfMacroList | AsmDwarfLine | AsmDwarfLineList | AsmDwarfCompilationUnitList |
               AsmDwarfConstruct | AsmDwarfConstructList, "AsmDwarfInformation", "AsmDwarfInformationTag", false /* canHaveInstances = false */ );

// ***************************************************************************************
//                     END OF DWARF SPECIFIC IR NODE DEFINITIONS
// ***************************************************************************************



  // Root of class hierarchy for binary file support
     NEW_NONTERMINAL_MACRO ( AsmExecutableFileFormat,
               AsmGenericDLL           | AsmGenericFormat        | AsmGenericDLLList           | AsmElfEHFrameEntryFD     |
               AsmGenericFile          | AsmGenericSection       | AsmGenericSymbol            | AsmGenericStrtab         |
               AsmGenericSymbolList    | AsmGenericSectionList   | AsmGenericHeaderList        | AsmGenericString         |
               AsmElfSectionTableEntry | AsmElfSegmentTableEntry | AsmElfSymbolList            | AsmPEImportILTEntry      |
               AsmElfRelocEntry        | AsmElfRelocEntryList    | AsmPEExportEntry            | AsmPEExportEntryList     |
               AsmElfDynamicEntry      | AsmElfDynamicEntryList  | AsmElfSegmentTableEntryList | AsmStringStorage         |
               AsmElfNoteEntry         | AsmElfNoteEntryList     |
               AsmElfSymverEntry       | AsmElfSymverEntryList   |
               AsmElfSymverDefinedEntry| AsmElfSymverDefinedEntryList |AsmElfSymverDefinedAux  | AsmElfSymverDefinedAuxList|
               AsmElfSymverNeededEntry | AsmElfSymverNeededEntryList  |AsmElfSymverNeededAux   | AsmElfSymverNeededAuxList |

               AsmPEImportDirectory    | AsmPEImportHNTEntry     | AsmPESectionTableEntry      | AsmPEExportDirectory     |
               AsmPERVASizePair        | AsmCoffSymbolList       | AsmPERVASizePairList        | AsmElfEHFrameEntryCI     |
               AsmPEImportHNTEntryList | AsmPEImportILTEntryList | AsmPEImportLookupTable      | AsmPEImportDirectoryList |
               AsmNEEntryPoint         | AsmNERelocEntry         | AsmNESectionTableEntry      | AsmElfEHFrameEntryCIList |
               AsmLEPageTableEntry     | AsmLEEntryPoint         | AsmLESectionTableEntry      | AsmElfEHFrameEntryFDList |
               AsmDwarfInformation, "AsmExecutableFileFormat", "AsmExecutableFileFormatTag", false /* canHaveInstances = false */);


  // This is the IR node for a binary executable that loosely corresponds to the SgFile IR node for
  // source code. The kinds of information that we want to save for each is really quire different.
     NEW_TERMINAL_MACRO ( AsmInterpretation, "AsmInterpretation", "AsmInterpretationTag" );
     NEW_TERMINAL_MACRO ( AsmInterpretationList, "AsmInterpretationList", "AsmInterpretationListTag" );
     NEW_TERMINAL_MACRO ( AsmGenericFileList, "AsmGenericFileList", "AsmGenericFileListTag" );

     NEW_NONTERMINAL_MACRO (AsmNode,
                            AsmStatement | AsmExpression | AsmInterpretation | AsmOperandList | AsmType |
                            AsmExecutableFileFormat | AsmInterpretationList | AsmGenericFileList,
                            "AsmNode","AsmNodeTag", false);

  // DQ (3/15/2007): Added support forbinaries (along lines of suggestions by Thomas Dullien)
  // AsmInstructionBase.setFunctionPrototype        ( "HEADER", "../Grammar/Common.code");
     AsmNode.setFunctionPrototype        ( "HEADER_BINARY", "../Grammar/BinaryInstruction.code");

  // Asm instructions should be able to have attached attributes (comments) like other IR nodes in ROSE
     AsmNode.setDataPrototype     ( "AttachedPreprocessingInfoType*", "attachedPreprocessingInfoPtr", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, COPY_DATA);

#if 1
  // DQ (11/10/2008): Added attribute via ROSETTA (changed to pointer to AstAttributeMechanism)
  // Modified implementation to only be at specific IR nodes (now supporting all binary IR nodes).
     AsmNode.setDataPrototype("AstAttributeMechanism*","asmAttributeMechanism","= NULL",
            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

  // Reuse the same definition as in SgSupport.
     AsmNode.setFunctionPrototype      ( "HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
     AsmNode.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
#endif

  // DQ (8/28/2008): Change "unsigned int" to "addr_t"
     AsmStatement.setDataPrototype("rose_addr_t","address","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
  // DQ (8/23/2008): Moved the comment string from the AsmInstruction to the AsmStatement
     AsmStatement.setDataPrototype("std::string","comment","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     //AsmStatement.setDataPrototype("SgAsmNode*","parent","= NULL",
     //                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);



     // Most platform and non-platform dependent machine instructions will map to this IR node
     AsmInstruction.setFunctionPrototype( "HEADER_BINARY_INSTRUCTION", "../Grammar/BinaryInstruction.code");
     AsmInstruction.setDataPrototype("std::string","mnemonic","= \"\"",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("SgUnsignedCharList","raw_bytes","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("SgAsmOperandList*","operandList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmInstruction.setDataPrototype("SgAsmStatementPtrList","sources","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     // Instruction basic block. One entry point (first instruction) and one exit point (last instruction).
     AsmBlock.setFunctionPrototype( "HEADER_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmBlock.setDataPrototype("rose_addr_t","next_block_true_address","= 0",// [tps 05Apr07] needed for the control_flow_graph
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("rose_addr_t","next_block_false_address","= 0",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("rose_addr_t","id","= 0",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmBlock.setDataPrototype("SgAsmStatementPtrList","statementList","", //in order of execution
                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("SgAddressList", "cached_successors", "",
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmBlock.setDataPrototype("bool", "complete_successors", "= false", // are all successors known and cached?
                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


     AsmOperandList.setFunctionPrototype        ( "HEADER_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");
     AsmOperandList.setDataPrototype("SgAsmExpressionPtrList","operands","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmInterpretation.setFunctionPrototype("HEADER_INTERPRETATION", "../Grammar/BinaryInstruction.code");
     AsmInterpretation.setPredeclarationString("HEADER_INTERPRETATION_PREDECLARATION", "../Grammar/BinaryInstruction.code");
     AsmInterpretation.setAutomaticGenerationOfConstructor(false);
  // DQ (11/6/2008): Moved Dwarf support to AsmInterpretation from SgBinaryFile. Moved ahead of the
  // SgAsmGenericHeader so that maps (of instructions to source) built in the Dwarf section can be
  // used in analysis in the instruction sections. since Dwarf is meant to be read-only (at least
  // for now) this is a simpler design and avoids redundant traversals.
     AsmInterpretation.setDataPrototype("SgAsmDwarfCompilationUnitList*","dwarf_info","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmInterpretation.setDataPrototype("SgAsmGenericHeaderList*", "headers", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // This is where the instructions are put...(put it last so the instructions are traversed last, after the binary file format)
     AsmInterpretation.setDataPrototype("SgAsmBlock*","global_block","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmInterpretationList.setDataPrototype("SgAsmInterpretationPtrList", "interpretations", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



  // *****************************************************
  //           NEW BINARY FILE FORMAT IR NODES
  // *****************************************************

  // This is a location where we can put some types that are used everywhere in the binary file format support.
     AsmExecutableFileFormat.setFunctionPrototype ( "HEADER_EXECUTABLE_FILE_FORMAT", "../Grammar/BinaryInstruction.code");

     AsmGenericString.setFunctionPrototype ( "HEADER_GENERIC_STRING", "../Grammar/BinaryInstruction.code");

  // std::string p_string;
     AsmBasicString.setFunctionPrototype ( "HEADER_BASIC_STRING", "../Grammar/BinaryInstruction.code");
     AsmBasicString.setDataPrototype("std::string","string","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmStoredString.setFunctionPrototype ( "HEADER_STORED_STRING", "../Grammar/BinaryInstruction.code");
     AsmStoredString.setDataPrototype("SgAsmStringStorage*","storage","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmStringStorage.setFunctionPrototype ( "HEADER_STRING_STORAGE", "../Grammar/BinaryInstruction.code");
     AsmStringStorage.setDataPrototype("SgAsmGenericStrtab*","strtab","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmStringStorage.setDataPrototype("std::string","string","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmStringStorage.setDataPrototype("rose_addr_t","offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfFileHeader.setFunctionPrototype ( "HEADER_ELF_HEADER", "../Grammar/BinaryInstruction.code");
     AsmElfFileHeader.setDataPrototype("unsigned char","e_ident_file_class","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned char","e_ident_data_encoding","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned char","e_ident_file_version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgUnsignedCharList","e_ident_padding","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_machine","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_ehsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","phextrasz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_phnum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","shextrasz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_shnum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_shstrndx","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (8/17/2008): Note that SgAsmElfSectionTable is a SgAsmGenericSection, so it is traversed from
  // the SgAsmGenericSectionList, so set to: NO_TRAVERSAL.
     AsmElfFileHeader.setDataPrototype("SgAsmElfSectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (8/17/2008): This points to an IR node that is in the SgAsmGenericFile::section list so we can't
  // traverse it here or we will be traversing this subtree twice in a single traversal (the dot graphs
  // show this nicely).  So set as: NO_TRAVERSAL
     AsmElfFileHeader.setDataPrototype("SgAsmElfSegmentTable*","segment_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



  /* An AsmElfSectionTable is a synthesized section representing the ELF Section Table. The section table entry info parsed
   * from the file is stored with its corresponding section rather than in the AsmElfSectionTable. We can reconstruct the
   * section table since sections have unique ID numbers that are their original indices in the ELF Section Table. */
     AsmElfSectionTable.setFunctionPrototype ( "HEADER_ELF_SECTION_TABLE", "../Grammar/BinaryInstruction.code");



     AsmElfSectionTableEntry.setFunctionPrototype("HEADER_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTableEntry.setDataPrototype("unsigned","sh_name","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgAsmElfSectionTableEntry::SectionType","sh_type","= SHT_PROGBITS",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_link","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_info","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("uint64_t","sh_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t","sh_addr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t","sh_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t","sh_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t","sh_addralign","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("rose_addr_t","sh_entsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgUnsignedCharList","extra","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfSection.setFunctionPrototype("HEADER_ELF_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSection.setDataPrototype("SgAsmElfSection*","linked_section","= NULL", //accessors must be virtual
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSection.setDataPrototype("SgAsmElfSectionTableEntry*","section_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmElfSection.setDataPrototype("SgAsmElfSegmentTableEntry*","segment_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmElfStringSection.setFunctionPrototype("HEADER_ELF_STRING_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfStringSection.setAutomaticGenerationOfDestructor(false);
     AsmElfStringSection.setDataPrototype("SgAsmElfStrtab*", "strtab", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfStrtab.setFunctionPrototype      ( "HEADER_ELF_STRING_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfStrtab.setAutomaticGenerationOfDestructor(false);

     AsmPEStringSection.setFunctionPrototype("HEADER_PE_STRING_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPEStringSection.setAutomaticGenerationOfDestructor(false);
     AsmPEStringSection.setDataPrototype("SgAsmCoffStrtab*", "strtab", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmCoffStrtab.setFunctionPrototype      ( "HEADER_COFF_STRING_TABLE", "../Grammar/BinaryInstruction.code");
     AsmCoffStrtab.setAutomaticGenerationOfDestructor(false);



     AsmElfRelocSection.setFunctionPrototype("HEADER_ELF_RELOC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfRelocSection.setDataPrototype("bool", "uses_addend", "= true",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocSection.setDataPrototype("SgAsmElfSection*","target_section", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocSection.setDataPrototype("SgAsmElfRelocEntryList*", "entries", "= NULL", 
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmElfRelocEntry.setFunctionPrototype("HEADER_ELF_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfRelocEntry.setDataPrototype("rose_addr_t", "r_offset", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocEntry.setDataPrototype("rose_addr_t", "r_addend", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocEntry.setDataPrototype("unsigned long", "sym", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocEntry.setDataPrototype("SgAsmElfRelocEntry::RelocType", "type", "= R_386_NONE",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfRelocEntry.setDataPrototype("SgUnsignedCharList","extra","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfRelocEntryList.setDataPrototype("SgAsmElfRelocEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfDynamicSection.setFunctionPrototype ( "HEADER_ELF_DYNAMIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicSection.setDataPrototype("SgAsmElfDynamicEntryList*","entries","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmElfDynamicEntry.setFunctionPrototype ( "HEADER_ELF_DYNAMIC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicEntry.setDataPrototype("SgAsmElfDynamicEntry::EntryType","d_tag","= SgAsmElfDynamicEntry::DT_NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicEntry.setDataPrototype("rose_rva_t", "d_val", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicEntry.setDataPrototype("SgUnsignedCharList","extra","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfDynamicEntryList.setDataPrototype("SgAsmElfDynamicEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /* ELF Notes */
     AsmElfNoteSection.setFunctionPrototype("HEADER_ELF_NOTE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfNoteSection.setDataPrototype("SgAsmElfNoteEntryList*", "entries", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmElfNoteEntry.setFunctionPrototype("HEADER_ELF_NOTE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfNoteEntry.setDataPrototype("unsigned", "type", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfNoteEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfNoteEntry.setDataPrototype("SgUnsignedCharList","payload","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfNoteEntryList.setDataPrototype("SgAsmElfNoteEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /* ELF Error Handling Frame Section (.eh_frame) */
     AsmElfEHFrameSection.setFunctionPrototype("HEADER_ELF_EH_FRAME_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfEHFrameSection.setDataPrototype("SgAsmElfEHFrameEntryCIList*", "ci_entries", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmElfEHFrameEntryCIList.setDataPrototype("SgAsmElfEHFrameEntryCIPtrList", "entries", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfEHFrameEntryFDList.setDataPrototype("SgAsmElfEHFrameEntryFDPtrList", "entries", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /* ELF Error Handling Frame Entry, Common Information Entry Format */
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



     /* ELF Error Handling Frame Entry, Frame Description Entry Format */
     AsmElfEHFrameEntryFD.setFunctionPrototype("HEADER_ELF_EH_FRAME_ENTRY_FD", "../Grammar/BinaryInstruction.code");
     AsmElfEHFrameEntryFD.setDataPrototype("rose_rva_t", "begin_rva", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryFD.setDataPrototype("rose_addr_t", "size", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryFD.setDataPrototype("SgUnsignedCharList", "augmentation_data", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfEHFrameEntryFD.setDataPrototype("SgUnsignedCharList", "instructions", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


     /* The GNU Symbol Version Table (.gnu.version section).  The section points to an SgAsmElfSymverEntryPtrList containing a
      * list of SgAsmElfSymverEntry objects. */
     AsmElfSymverSection.setFunctionPrototype("HEADER_ELF_SYMVER_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymverSection.setDataPrototype("SgAsmElfSymverEntryList*", "entries", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmElfSymverEntryList.setDataPrototype("SgAsmElfSymverEntryPtrList", "entries", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmElfSymverEntry.setFunctionPrototype("HEADER_ELF_SYMVER_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSymverEntry.setDataPrototype("size_t", "value", "= 0",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /* The GNU Symbol Version Definitions (.gnu.version_d section).  The SgAsmElfSymverDefinedSection points to a list of
      * entries (SgAsmSymverDefinedEntry), which in turn point to a list of auxilliary members (SgAsmSymverDefinedAux). See
      * SgAsmSymverDefinedSection::parse() for a good description of the disk format. */
     AsmElfSymverDefinedSection.setFunctionPrototype("HEADER_ELF_SYMVER_DEFINED_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymverDefinedSection.setDataPrototype("SgAsmElfSymverDefinedEntryList*", "entries", "= NULL",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmElfSymverDefinedEntryList.setDataPrototype("SgAsmElfSymverDefinedEntryPtrList", "entries", "",
                                                   NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                   NO_DELETE);

     AsmElfSymverDefinedAuxList.setDataPrototype("SgAsmElfSymverDefinedAuxPtrList", "entries", "",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

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

     AsmElfSymverDefinedAux.setFunctionPrototype("HEADER_ELF_SYMVER_DEFINED_AUX", "../Grammar/BinaryInstruction.code");
     AsmElfSymverDefinedAux.setDataPrototype("SgAsmGenericString*","name","= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     /* The GNU Symbol Version Requirements Table (.gnu.version_r section).  The format of this table is similar to the GNU
      * Symbol Version Definitions Table described above, namey that the table object (SgAsmElfSymverNeededSection) points to
      * a list of entries (SgAsmElfSymverNeededEntry), which of which point to a list of auxilliary information
      * (SgAsmElfSymverNeededAux). */
     AsmElfSymverNeededSection.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymverNeededSection.setDataPrototype("SgAsmElfSymverNeededEntryList*", "entries", "= NULL",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmElfSymverNeededEntryList.setDataPrototype("SgAsmElfSymverNeededEntryPtrList", "entries", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmElfSymverNeededAuxList.setDataPrototype("SgAsmElfSymverNeededAuxPtrList", "entries", "",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmElfSymverNeededEntry.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSymverNeededEntry.setDataPrototype("size_t", "version", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededEntry.setDataPrototype("SgAsmGenericString*", "file_name", "= 0",     
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededEntry.setDataPrototype("SgAsmElfSymverNeededAuxList*", "entries", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmElfSymverNeededAux.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_AUX", "../Grammar/BinaryInstruction.code");
     AsmElfSymverNeededAux.setDataPrototype("uint32_t", "hash", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededAux.setDataPrototype("int", "flags", "= 0",     
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededAux.setDataPrototype("size_t", "other", "= 0",     
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymverNeededAux.setDataPrototype("SgAsmGenericString*","name","= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     
     

  /* An AsmElfSegmentTable is a synthesized section representing the ELF Segment Table. The segment table entry info parsed
   * from the file is stored with its corresponding AsmElfSection rather than in the AsmElfSegmentTable. We can reconstruct the
   * segment table since the segment table entries store their table index. */
     AsmElfSegmentTable.setFunctionPrototype ( "HEADER_ELF_SEGMENT_TABLE", "../Grammar/BinaryInstruction.code");



     AsmElfSegmentTableEntry.setFunctionPrototype("HEADER_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTableEntry.setDataPrototype("size_t", "index", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentType","type",
                                              "= SgAsmElfSegmentTableEntry::PT_LOAD",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentFlags","flags",
                                              "= SgAsmElfSegmentTableEntry::PF_NONE",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t","offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t","vaddr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t","paddr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t","filesz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t","memsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t","align","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgUnsignedCharList","extra","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfSegmentTableEntryList.setFunctionPrototype("HEADER_ELF_SEGMENT_TABLE_ENTRY_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTableEntryList.setDataPrototype("SgAsmElfSegmentTableEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmElfSymbolSection.setFunctionPrototype("HEADER_ELF_SYMBOL_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolSection.setDataPrototype("SgAsmElfSymbolList*","symbols","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmElfSymbolSection.setDataPrototype("bool", "is_dynamic", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmElfSymbolList.setFunctionPrototype ( "HEADER_ELF_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolList.setDataPrototype("SgAsmElfSymbolPtrList","symbols","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmElfSymbol.setFunctionPrototype("HEADER_ELF_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmElfSymbol.setDataPrototype("unsigned char","st_info","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned char","st_res1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned","st_shndx","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("rose_addr_t","st_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("SgUnsignedCharList","extra","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmPEFileHeader.setFunctionPrototype("HEADER_PE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmPEFileHeader.setAutomaticGenerationOfDestructor(false);
     AsmPEFileHeader.setDataPrototype("unsigned","e_cpu_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_nsections","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_time","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("rose_addr_t","e_coff_symtab","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("rose_addr_t","e_nt_hdr_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_coff_nsyms","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_opt_magic","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_lmajor","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_lminor","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_code_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_data_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_bss_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("rose_rva_t","e_code_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("rose_rva_t","e_data_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_section_align","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_file_align","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_os_major","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_os_minor","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_user_major","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_user_minor","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_subsys_major","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_subsys_minor","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_reserved9","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_image_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_header_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_file_checksum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_subsystem","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_dll_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_stack_reserve_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_stack_commit_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_heap_reserve_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_heap_commit_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_loader_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_num_rvasize_pairs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmPERVASizePairList*","rvasize_pairs","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (8/17/2008): Note that SgAsmPESectionTable is a SgAsmGenericSection, so it is traversed from
  // the SgAsmGenericSectionList, so set to: NO_TRAVERSAL.
     AsmPEFileHeader.setDataPrototype("SgAsmPESectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (8/17/2008): Note that SgAsmCoffSymbolTable is a SgAsmGenericSection, so it is traversed from
  // the SgAsmGenericSectionList, so set to: NO_TRAVERSAL.
     AsmPEFileHeader.setDataPrototype("SgAsmCoffSymbolTable*","coff_symtab","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmPERVASizePair.setFunctionPrototype ( "HEADER_PE_RVA_SIZE_PAIR", "../Grammar/BinaryInstruction.code");
     AsmPERVASizePair.setDataPrototype("rose_rva_t","e_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPERVASizePair.setDataPrototype("rose_addr_t","e_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPERVASizePair.setDataPrototype("SgAsmGenericSection*", "section", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmPERVASizePairList.setFunctionPrototype ( "HEADER_PE_RVA_SIZE_PAIR_LIST", "../Grammar/BinaryInstruction.code");
     AsmPERVASizePairList.setDataPrototype("SgAsmPERVASizePairPtrList","pairs","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmPESection.setFunctionPrototype ( "HEADER_PE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPESection.setDataPrototype("SgAsmPESectionTableEntry*","section_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmPESectionTable.setFunctionPrototype ( "HEADER_PE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");



     AsmPESectionTableEntry.setFunctionPrototype ( "HEADER_PE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmPESectionTableEntry.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("rose_addr_t","virtual_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("rose_addr_t","rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("rose_addr_t","physical_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("rose_addr_t","physical_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned","coff_line_nums","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned","n_relocs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned","n_coff_line_nums","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmPEImportSection.setFunctionPrototype ( "HEADER_PE_IMPORT_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPEImportSection.setDataPrototype ("SgAsmPEImportDirectoryList*","import_directories","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmPEExportEntryList.setDataPrototype("SgAsmPEExportEntryPtrList", "exports", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmPEExportSection.setFunctionPrototype("HEADER_PE_EXPORT_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPEExportSection.setDataPrototype("SgAsmPEExportDirectory*", "export_dir", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEExportSection.setDataPrototype("SgAsmPEExportEntryList*", "exports", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmPEExportEntry.setFunctionPrototype("HEADER_PE_EXPORT_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmPEExportEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEExportEntry.setDataPrototype("unsigned", "ordinal", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportEntry.setDataPrototype("rose_rva_t", "export_rva", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExportEntry.setDataPrototype("SgAsmGenericString*", "forwarder", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


  // std::vector<COFFSymbol*> symbols;
  // ExecSection *strtab;                /* Section containing symbol names */
     AsmCoffSymbolTable.setFunctionPrototype ( "HEADER_PE_COFF_SYMBOL_TABLE", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbolTable.setDataPrototype("SgAsmCoffSymbolList*","symbols","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (8/17/2008): Note that strtab is a SgAsmGenericSection, so it is traversed from
  // the SgAsmGenericSectionList, so set to: NO_TRAVERSAL.
     AsmCoffSymbolTable.setDataPrototype("SgAsmGenericSection*","strtab","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // std::string         st_name;        // The original name; super.name might be modified
  // addr_t              st_name_offset;
  // int                 st_section_num;
  // unsigned            st_type, st_storage_class, st_num_aux_entries;
  // const unsigned char *aux_data;      // Auxilliary data from table entries that follow
  // size_t              aux_size;       // Size (bytes) of auxilliary data
     AsmCoffSymbol.setFunctionPrototype ( "HEADER_PE_COFF_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbol.setDataPrototype("std::string","st_name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("rose_addr_t","st_name_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("int","st_section_num","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (8/28/2008): Check against the specification
     AsmCoffSymbol.setDataPrototype("unsigned","st_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("unsigned","st_storage_class","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("unsigned","st_num_aux_entries","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("SgUnsignedCharList","aux_data","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This is the IR node to hold a list of symbols (needed because we can't mix children from data
  // members with children from containers).
     AsmCoffSymbolList.setFunctionPrototype ( "HEADER_PE_COFF_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbolList.setDataPrototype("SgAsmCoffSymbolPtrList","symbols","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmPEImportDirectory.setFunctionPrototype ( "HEADER_PE_IMPORT_DIRECTORY", "../Grammar/BinaryInstruction.code");
     AsmPEImportDirectory.setDataPrototype("int","idx","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("rose_rva_t","ilt_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("SgAsmPEImportLookupTable*", "ilt", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("time_t","time","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("unsigned","forwarder_chain","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("rose_rva_t","dll_name_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("SgAsmGenericString*", "dll_name", "=0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("rose_rva_t","iat_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("SgAsmPEImportLookupTable*", "iat", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmPEImportILTEntry.setFunctionPrototype("HEADER_PE_IMPORT_ILT_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmPEImportILTEntry.setDataPrototype("SgAsmPEImportILTEntry::ILTEntryType", "entry_type", "=ILT_ORDINAL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportILTEntry.setDataPrototype("unsigned","ordinal","= 0",                   /*if entry_type==ILT_ORDINAL*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportILTEntry.setDataPrototype("rose_rva_t","hnt_entry_rva","= 0",           /*if entry_type==ILT_HNT_ENTRY_RVA*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportILTEntry.setDataPrototype("SgAsmPEImportHNTEntry*","hnt_entry","= 0",   /*if entry_type==ILT_HNT_ENTRY_RVA*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportILTEntry.setDataPrototype("rose_rva_t","bound_rva","= 0",               /*if entry_type==ILT_BOUND_RVA*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportILTEntry.setDataPrototype("uint64_t","extra_bits","= 0",                /*non-zero reserved bits, all entry types*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmPEImportLookupTable.setFunctionPrototype("HEADER_PE_IMPORT_LOOKUP_TABLE", "../Grammar/BinaryInstruction.code");
     AsmPEImportLookupTable.setDataPrototype("bool", "is_iat", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportLookupTable.setDataPrototype("SgAsmPEImportILTEntryList*","entries","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmPEImportHNTEntry.setFunctionPrototype("HEADER_PE_IMPORT_HNT_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmPEImportHNTEntry.setDataPrototype("unsigned","hint","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportHNTEntry.setDataPrototype("SgAsmGenericString*","name","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportHNTEntry.setDataPrototype("unsigned char","padding","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);




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
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);





     AsmPEImportHNTEntryList.setDataPrototype("SgAsmPEImportHNTEntryPtrList","hintnames","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmPEImportILTEntryList.setDataPrototype("SgAsmPEImportILTEntryPtrList","vector","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmPEImportDirectoryList.setDataPrototype("SgAsmPEImportDirectoryPtrList","vector","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmNEFileHeader.setFunctionPrototype ( "HEADER_NE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmNEFileHeader.setDataPrototype("unsigned","e_linker_major","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_linker_minor","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_checksum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_flags1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_autodata_sn","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_bss_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_stack_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_csip","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_sssp","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_nsections","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_nmodrefs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_nnonresnames","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_nmovable_entries","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_sector_align","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_nresources","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_exetype","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_flags2","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_res1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("unsigned","e_winvers","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_entrytab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_entrytab_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_sectab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_rsrctab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_resnametab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_modreftab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_importnametab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_nonresnametab_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_fastload_sector","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("rose_addr_t","e_fastload_nsectors","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmDOSExtendedHeader*","dos2_header","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (8/17/2008): Note that SgAsmNESectionTable is a SgAsmGenericSection, so it is traversed from
  // the SgAsmGenericSectionList, so set to: NO_TRAVERSAL.
     AsmNEFileHeader.setDataPrototype("SgAsmNESectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNENameTable*","resname_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNENameTable*","nonresname_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNEModuleTable*","module_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNEEntryTable*","entry_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


  // NESectionTableEntry *st_entry;
  // NERelocTable *reloc_table;
     AsmNESection.setFunctionPrototype ( "HEADER_NE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmNESection.setDataPrototype("SgAsmNESectionTableEntry*","st_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNESection.setDataPrototype("SgAsmNERelocTable*","reloc_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


  // unsigned    flags, sector;
  // addr_t      physical_size, virtual_size;
     AsmNESectionTable.setFunctionPrototype ( "HEADER_NE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNESectionTable.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("unsigned","sector","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("rose_addr_t","physical_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("rose_addr_t","virtual_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



  // std::vector<std::string> names; /*first name is module name; remainder are symbols within the module*/
  // std::vector<unsigned> ordinals; /*first entry is ignored but present in file*/
     AsmNENameTable.setFunctionPrototype ( "HEADER_NE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNENameTable.setDataPrototype("SgStringList","names","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNENameTable.setDataPrototype("SgUnsignedList","ordinals","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // NEStringTable *strtab;
  // std::vector<addr_t> name_offsets;
  // std::vector<std::string> names;
     AsmNEModuleTable.setFunctionPrototype ( "HEADER_NE_MODULE_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEModuleTable.setDataPrototype("SgAsmNEStringTable*","strtab","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmNEModuleTable.setDataPrototype("SgAddressList","name_offsets","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEModuleTable.setDataPrototype("SgStringList","names","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // No data members
     AsmNEStringTable.setFunctionPrototype ( "HEADER_NE_STRING_TABLE", "../Grammar/BinaryInstruction.code");

  // std::vector<size_t> bundle_sizes;
  // std::vector<NEEntryPoint> entries;
     AsmNEEntryTable.setFunctionPrototype ( "HEADER_NE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEEntryTable.setDataPrototype("SgSizeTList","bundle_sizes","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEEntryTable.setDataPrototype("SgAsmNEEntryPointPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // std::vector<NERelocEntry> entries;
     AsmNERelocTable.setFunctionPrototype ( "HEADER_NE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNERelocTable.setDataPrototype("SgAsmNERelocEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



  // NEEntryFlags flags;         /* bit flags */
  // unsigned int3f;             /* always 0x3fxx */
  // unsigned section_idx;       /* zero indicates unused entry */
  // unsigned section_offset;    /* byte offset into section */
     AsmNEEntryPoint.setFunctionPrototype ( "HEADER_NE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmNEEntryPoint.setDataPrototype("SgAsmNEEntryPoint::NEEntryFlags","flags","= SgAsmNEEntryPoint::EF_ZERO",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEEntryPoint.setDataPrototype("unsigned","int3f","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEEntryPoint.setDataPrototype("unsigned","section_idx","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEEntryPoint.setDataPrototype("unsigned","section_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // NERelocSrcType      src_type;       /* low nibble of first byte of relocation record */
  // NERelocModifiers    modifier;       /* high nibble of first byte */
  // NERelocTgtType      tgt_type;       /* low two bits of second byte */
  // NERelocFlags        flags;          /* high six bits of second byte */
  // addr_t              src_offset;
  // iref_type iref;
  // iord_type iord;
  // iname_type iname;
  // osfixup_type osfixup;
     AsmNERelocEntry.setFunctionPrototype ( "HEADER_NE_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocSrcType","src_type","= SgAsmNERelocEntry::RF_SRCTYPE_8OFF",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocModifiers","modifier","= SgAsmNERelocEntry::RF_MODIFIER_SINGLE",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocTgtType","tgt_type","= SgAsmNERelocEntry::RF_TGTTYPE_IREF",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocFlags","flags","= SgAsmNERelocEntry::RF_ADDITIVE",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("rose_addr_t","src_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Note that these call default constructors to initialize their data
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iref_type","iref","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iord_type","iord","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iname_type","iname","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::osfixup_type","osfixup","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // unsigned    flags, sector;
  // addr_t      physical_size, virtual_size;
     AsmNESectionTableEntry.setFunctionPrototype ( "HEADER_NE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmNESectionTableEntry.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTableEntry.setDataPrototype("unsigned","sector","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTableEntry.setDataPrototype("rose_addr_t","physical_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTableEntry.setDataPrototype("rose_addr_t","virtual_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmLEFileHeader.setFunctionPrototype ( "HEADER_LE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmLEFileHeader.setDataPrototype("unsigned","e_byte_order","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_word_order","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_format_level","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_cpu_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_os_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_module_version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_eip_section","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_esp_section","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_last_page_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_page_offset_shift","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_fixup_sect_cksum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_loader_sect_cksum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_secttab_nentries","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_rsrctab_nentries","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_fmtdirtab_nentries","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_import_modtab_nentries","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_preload_npages","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_nonresnametab_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_nonresnametab_cksum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_auto_ds_section","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_debug_info_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_num_instance_preload","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_num_instance_demand","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("unsigned","e_heap_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_npages","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_eip","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_esp","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_page_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_fixup_sect_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_loader_sect_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_secttab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_pagetab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_iterpages_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_rsrctab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_resnametab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_entrytab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_fmtdirtab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_fixup_pagetab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_fixup_rectab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_import_modtab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_import_proctab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_ppcksumtab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_data_pages_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_nonresnametab_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("rose_addr_t","e_debug_info_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmDOSExtendedHeader*","dos2_header","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // DQ (8/17/2008): Note that SgAsmLESectionTable is a SgAsmGenericSection, so it is traversed from
  // the SgAsmGenericSectionList, so set to: NO_TRAVERSAL.
     AsmLEFileHeader.setDataPrototype("SgAsmLESectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLEPageTable*","page_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLENameTable*","resname_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLENameTable*","nonresname_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLEEntryTable*","entry_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLERelocTable*","reloc_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



  // LESectionTableEntry *st_entry;
     AsmLESection.setFunctionPrototype ( "HEADER_LE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmLESection.setDataPrototype("SgAsmLESectionTableEntry*","st_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // No data members
     AsmLESectionTable.setFunctionPrototype ( "HEADER_LE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");

  // std::vector<std::string> names;  // first name is module name; remainder are symbols within the module
  // std::vector<unsigned> ordinals;  // first entry is ignored but present in file
     AsmLENameTable.setFunctionPrototype ( "HEADER_LE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLENameTable.setDataPrototype("SgStringList","names","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLENameTable.setDataPrototype("SgUnsignedList","ordinals","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // std::vector<LEPageTableEntry*> entries;
     AsmLEPageTable.setFunctionPrototype ( "HEADER_LE_PAGE_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLEPageTable.setDataPrototype("SgAsmLEPageTableEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // std::vector<size_t> bundle_sizes;
  // std::vector<LEEntryPoint> entries;
     AsmLEEntryTable.setFunctionPrototype ( "HEADER_LE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLEEntryTable.setDataPrototype("SgSizeTList","bundle_sizes","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryTable.setDataPrototype("SgAsmLEEntryPointPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // std::vector<LERelocEntry> entries;
     AsmLERelocTable.setFunctionPrototype ( "HEADER_LE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLERelocTable.setDataPrototype("SgAsmLERelocEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // unsigned    pageno;
  // unsigned    flags;
     AsmLEPageTableEntry.setFunctionPrototype ( "HEADER_LE_PAGE_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmLEPageTableEntry.setDataPrototype("unsigned","pageno","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEPageTableEntry.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // std::vector<LEEntryPoint> entries;
  // unsigned flags, objnum, entry_type, res1;
  // addr_t entry_offset;
     AsmLEEntryPoint.setFunctionPrototype ( "HEADER_LE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmLEEntryPoint.setDataPrototype("SgAsmLEEntryPointPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned","objnum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned","entry_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned","res1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("rose_addr_t","entry_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // unsigned    flags, pagemap_index, pagemap_nentries, res1;
  // addr_t      mapped_size, base_addr;
     AsmLESectionTableEntry.setFunctionPrototype ( "HEADER_LE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmLESectionTableEntry.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("unsigned","pagemap_index","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("unsigned","pagemap_nentries","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("unsigned","res1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("rose_addr_t","mapped_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("rose_addr_t","base_addr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmDOSFileHeader.setFunctionPrototype ( "HEADER_DOS_HEADER", "../Grammar/BinaryInstruction.code");
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_last_page_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_total_pages","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_nrelocs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_header_paragraphs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_minalloc","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_maxalloc","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_ss","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_sp","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_cksum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_ip","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_cs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("uint16_t","e_overlay","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("rose_addr_t","e_relocs_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned","e_res1", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*","relocs","= NULL", //relocation info
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*","rm_section","= NULL", //real-mode section
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmDOSExtendedHeader.setFunctionPrototype ( "HEADER_DOS_EXTENDED_HEADER", "../Grammar/BinaryInstruction.code");
     AsmDOSExtendedHeader.setDataPrototype("unsigned","e_res1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned","e_oemid","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned","e_oeminfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned","e_res2","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned","e_res3","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned","e_res4","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned","e_res5","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("unsigned","e_res6","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSExtendedHeader.setDataPrototype("rose_addr_t","e_lfanew","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



  // Need a separate IR node to hold the list of SgAsmGenericSection pointers.
     AsmGenericSectionList.setDataPrototype("SgAsmGenericSectionPtrList","sections","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



     AsmGenericSection.setFunctionPrototype("HEADER_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmGenericSection.setPredeclarationString("HEADER_GENERIC_SECTION_PREDECLARATION", "../Grammar/BinaryInstruction.code");
  // Later we will want to turn this back on so that this IR node is consistant with the others (if appropriate).
     AsmGenericSection.setAutomaticGenerationOfDestructor(false);
  /* The file to which this section belongs */
     AsmGenericSection.setDataPrototype("SgAsmGenericFile*","file","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("SgAsmGenericHeader*","header","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Current size of section in bytes (may be different than original size, which is kept in p_data.size()) */
     AsmGenericSection.setDataPrototype("rose_addr_t","size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Starting file offset of the section */
     AsmGenericSection.setDataPrototype("rose_addr_t","offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Required file alignment; zero and one both imply byte alignment */
     AsmGenericSection.setDataPrototype("rose_addr_t", "file_alignment", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Original content of just this section; points into file's content */
     AsmGenericSection.setDataPrototype("SgFileContentList","data","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* General contents of the section */
     AsmGenericSection.setDataPrototype("SgAsmGenericSection::SectionPurpose","purpose","= SgAsmGenericSection::SP_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Section was created by the format reader; not specified in file */
     AsmGenericSection.setDataPrototype("bool","synthesized","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Non-unique section ID (unique for ELF) or negative */
     AsmGenericSection.setDataPrototype("int","id","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Non-unique name of section (may be empty string) */
     AsmGenericSection.setDataPrototype("SgAsmGenericString*","name","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t","mapped_preferred_rva","= 0",     /*doxygen*/
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t","mapped_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_alignment", "= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool","mapped_rperm","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool","mapped_wperm","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool","mapped_xperm","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool","contains_code","= false",               /*doxygen*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("rose_addr_t","mapped_actual_va","= 0",         /*doxygen*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



  // DQ (8/17/2008): SgAsmGenericHeader are derived from the SgAsmGenericSection and so already appear
  // in the AsmGenericFile::sections list, so set to: NO_TRAVERSAL.
  // RPM (8/30/2008): AST-wise, headers are children of SgAsmGenericFile, and the children of each header are
  // the sections that belong to that header. So we now do need to traverse the headers.
     AsmGenericHeaderList.setDataPrototype("SgAsmGenericHeaderPtrList","headers","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmGenericDLLList.setFunctionPrototype ( "HEADER_GENERIC_DLL_LIST", "../Grammar/BinaryInstruction.code");

  // DQ (8/23/2008): I think we should consider traversing this or building an ELF version
  // of the list so that we can traverse it similarly to the PE DLL list.
     AsmGenericDLLList.setDataPrototype("SgAsmGenericDLLPtrList","dlls","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmGenericSymbolList.setFunctionPrototype ( "HEADER_GENERIC_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");

  // DQ (8/17/2008): Set to NO_TRAVERSAL to avoid visiting symbols twice (also travered from
  // SgAsmElfSymbolList and SgAsmPESymbolList for ELF or PE formats).
     AsmGenericSymbolList.setDataPrototype("SgAsmGenericSymbolPtrList","symbols","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmGenericHeader.setFunctionPrototype("HEADER_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");
     AsmGenericHeader.setAutomaticGenerationOfDestructor(false);
     /* General info about the executable format */
     AsmGenericHeader.setDataPrototype("SgAsmGenericFormat*","exec_format","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     /* Optional magic number in file byte order */
     AsmGenericHeader.setDataPrototype("SgCharList","magic","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     /* Machine for which this header and its sections, etc. was compiled */
     AsmGenericHeader.setDataPrototype("SgAsmGenericFormat::InsSetArchitecture","isa","= SgAsmGenericFormat::ISA_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     /* Base virtual address used by all "relative virtual addresses" (RVA) */
     AsmGenericHeader.setDataPrototype("rose_addr_t","base_va","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     /* Code entry point wrt base_va */
     AsmGenericHeader.setDataPrototype("SgRVAList","entry_rvas","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericHeader.setDataPrototype("SgAsmGenericDLLList*","dlls","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmGenericHeader.setDataPrototype("SgAsmGenericSymbolList*","symbols","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmGenericHeader.setDataPrototype("SgAsmGenericSectionList*","sections","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);




     AsmGenericFile.setFunctionPrototype ( "HEADER_GENERIC_FILE", "../Grammar/BinaryInstruction.code");
     AsmGenericFile.setPredeclarationString("HEADER_GENERIC_FILE_PREDECLARATION", "../Grammar/BinaryInstruction.code");
  // Later we will want to turn this back on so that this IR node is consistant with the others (if appropriate).
     AsmGenericFile.setAutomaticGenerationOfConstructor(false);
     AsmGenericFile.setAutomaticGenerationOfDestructor(false);
     AsmGenericFile.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* File descriptor opened for read-only (or negative) */
     AsmGenericFile.setDataPrototype("int","fd","= -1",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (6/14/2008): This data member's type is not yet correctly implemented needs to reference stat or stat64.
  // RPM (2008-08-22): We don't need this any more; we only used it for get_size(), which now uses STL vector.
  // Need to discuss this with Robb.
  /* File attributes at time of file open (valid if fd>=0) */
     AsmGenericFile.setDataPrototype("SgAsmGenericFormat::fileDetails","sb","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Content of file mapped into memory */
     AsmGenericFile.setDataPrototype("SgFileContentList","data","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* All known header sections for this file */
     AsmGenericFile.setDataPrototype("SgAsmGenericHeaderList*","headers","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  /* All known holes (areas otherwise unreferenced by parsing) */
     AsmGenericFile.setDataPrototype("SgAsmGenericSectionList*", "holes", "= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("bool", "truncate_zeros", "= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("bool", "tracking_references", "= true", 
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("ExtentMap", "referenced_extents", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmGenericFileList.setDataPrototype("SgAsmGenericFilePtrList", "files", "",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


  // This data structure represents the ExecFile from file: ExecGeneric.h
  // ExecFamily          family;                         // General format: ELF, PE, etc.
  // ExecPurpose         purpose;                        // executable, library, etc.
  // ByteOrder           sex;
  // unsigned            version;                        // actual file format version number stored in file
  // bool                is_current_version;             // is 'version' considered to be the current, supported version
  // ExecABI             abi;                            // application binary interface
  // unsigned            abi_version;                    // version of the ABI targeted by this file
  // size_t              word_size;                      // in bytes (e.g., Elf32 is 4; Elf64 is 8)
     AsmGenericFormat.setFunctionPrototype ( "HEADER_GENERIC_FORMAT", "../Grammar/BinaryInstruction.code");
  /* General format: ELF, PE, etc. */
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecFamily","family","= SgAsmGenericFormat::FAMILY_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* executable, library, etc. */
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecPurpose","purpose","= SgAsmGenericFormat::PURPOSE_EXECUTABLE",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* No comment available */
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ByteOrder","sex","= SgAsmGenericFormat::ORDER_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* actual file format version number stored in file */
     AsmGenericFormat.setDataPrototype("unsigned","version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* is 'version' considered to be the current, supported version */
     AsmGenericFormat.setDataPrototype("bool","is_current_version","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* application binary interface */
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecABI","abi","= SgAsmGenericFormat::ABI_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* version of the ABI targeted by this file */
     AsmGenericFormat.setDataPrototype("unsigned","abi_version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Initialize to ZERO first and then set as required, in bytes (e.g., Elf32 is 4; Elf64 is 8) */
     AsmGenericFormat.setDataPrototype("size_t","word_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/3/2008): Added new IR nodes data members.
  // SymbolDefState      def_state;    // Undefined, created but not allocated, created and allocated, etc.
  // SymbolBinding       binding;      // local, global, etc.
  // SymbolType          type;         // file, section, variable, function, etc.
  // addr_t              value;        // symbol value or address if defined
  // addr_t              size;         // size of symbol if defined
  // ExecSection         *bound;       // section when defined locally
  // std::string         name;         // Symbol name may be the empty string
     AsmGenericSymbol.setFunctionPrototype ( "HEADER_GENERIC_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmGenericSymbol.setAutomaticGenerationOfConstructor(false);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolDefState","def_state","= SgAsmGenericSymbol::SYM_UNDEFINED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolBinding","binding","= SgAsmGenericSymbol::SYM_NO_BINDING",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolType","type","= SgAsmGenericSymbol::SYM_NO_TYPE",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("rose_addr_t","value","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("rose_addr_t","size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSection*","bound","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericString*","name","= 0",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmGenericDLL.setFunctionPrototype ( "HEADER_GENERIC_DLL", "../Grammar/BinaryInstruction.code");
     AsmGenericDLL.setDataPrototype("SgAsmGenericString*","name","= 0",
                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmGenericDLL.setDataPrototype("SgStringList","symbols","",
                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);



     AsmGenericStrtab.setFunctionPrototype("HEADER_GENERIC_STRTAB", "../Grammar/BinaryInstruction.code");
     AsmGenericStrtab.setAutomaticGenerationOfDestructor(false);
     AsmGenericStrtab.setDataPrototype("SgAsmGenericSection*", "container", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericStrtab.setDataPrototype("SgAsmGenericStrtab::referenced_t","storage_list","",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericStrtab.setDataPrototype("ExtentMap","freelist","", /*space avail. for new strings*/
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericStrtab.setDataPrototype("SgAsmStringStorage*","dont_free","= NULL", /*never free this storage*/
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericStrtab.setDataPrototype("size_t","num_freed","= 0", /*number of strings freed thus far*/
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

// *****************************************************
//           END OF BINARY FILE FORMAT IR NODES
// *****************************************************







     // There are several sorts of declarations within a binary
     AsmDeclaration.setFunctionPrototype("HEADER_BINARY_DECLARATION", "../Grammar/BinaryInstruction.code");
     AsmDataStructureDeclaration.setFunctionPrototype("HEADER_BINARY_DATA_STRUCTURE", "../Grammar/BinaryInstruction.code");
     // DQ (3/15/2007): I can't seem to get this to compile so I will leave it out for now!
     // Binaries have some easily resolved data structures so we use this to represent these
     // AsmDataStructureDeclaration.setDataPrototype("std::list<SgAsmDeclaration*>","declarationList","",
     //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);




     // Binaries have many easily resolved functions so we use this to represent these
     AsmFunctionDeclaration.setFunctionPrototype("HEADER_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
     AsmFunctionDeclaration.setDataPrototype("std::string","name","= \"\"",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("unsigned", "reason", "= SgAsmFunctionDeclaration::FUNC_NONE", /*bit flags*/
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmFunctionDeclaration::function_kind_enum",
                                             "function_kind","= SgAsmFunctionDeclaration::e_unknown",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
     AsmFunctionDeclaration.setDataPrototype("std::string","name_md5","= \"\"",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmStatementPtrList","statementList","",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmStatementPtrList","dest","",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("rose_addr_t", "entry_va", "= 0",  /*entry point virtual address*/
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // DQ (5/3/2010): Added symbol table support to the binary analysis within ROSE.  Values that
     // are addresses or references to data will have symbols in a function symbol table.  All other 
     // values are assumed to be literals and will not have associated symbols.
     AsmFunctionDeclaration.setDataPrototype("SgSymbolTable*", "symbol_table", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE,
                                             NO_COPY_DATA);



  // These are used as data members in AsmDataStructureDeclaration
     AsmFieldDeclaration.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Not clear if we want to store the offset explicitly
     AsmFieldDeclaration.setDataPrototype("uint64_t","offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmByteValueExpression.setDataPrototype("uint8_t","value","= 0x0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmWordValueExpression.setDataPrototype("uint16_t","value","= 0x0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDoubleWordValueExpression.setDataPrototype("uint32_t","value","= 0x0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmQuadWordValueExpression.setDataPrototype("uint64_t","value","= 0x0",
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
     AsmValueExpression.setDataPrototype("unsigned short", "bit_offset", "= 0",         /*doxygen*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmValueExpression.setDataPrototype("unsigned short", "bit_size", "= 0",           /*doxygen*/
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (5/3/2010): Added symbol table support to the binary analysis within ROSE.  Values that
  // are addresses or references to data will have symbols in a function symbol table.  All other 
  // values are assumed to be literals and will not have associated symbols.
     AsmValueExpression.setDataPrototype("SgSymbol*", "symbol", "= NULL",
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



     AsmRegisterReferenceExpression.setFunctionPrototype("HEADER_BINARY_REGISTER_REFERENCE_EXPRESSION",
                                                         "../Grammar/BinaryInstruction.code");
     // added by tps on 3Apr07 and removed on 16Jan08
     //AsmRegisterReferenceExpression.setDataPrototype("SgAsmExpression*","offset","= NULL",
     //                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     // Added by JJW on 2-12-2008
     AsmRegisterReferenceExpression.setDataPrototype("SgAsmType*","type","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     Asmx86RegisterReferenceExpression.setFunctionPrototype("HEADER_BINARY_X86_REGISTER_REFERENCE_EXPRESSION",
                                                            "../Grammar/BinaryInstruction.code");
     Asmx86RegisterReferenceExpression.setDataPrototype("X86RegisterClass","register_class","= x86_regclass_unknown",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Asmx86RegisterReferenceExpression.setDataPrototype("int","register_number","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Asmx86RegisterReferenceExpression.setDataPrototype("X86PositionInRegister","position_in_register","= x86_regpos_unknown",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmArmRegisterReferenceExpression.setFunctionPrototype("HEADER_BINARY_ARM_REGISTER_REFERENCE_EXPRESSION",
                                                            "../Grammar/BinaryInstruction.code");
     AsmArmRegisterReferenceExpression.setDataPrototype("SgAsmArmRegisterReferenceExpression::arm_register_enum",
                                                        "arm_register_code",
                                                        "= SgAsmArmRegisterReferenceExpression::undefined_arm_register",
                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmPowerpcRegisterReferenceExpression.setFunctionPrototype("HEADER_BINARY_POWERPC_REGISTER_REFERENCE_EXPRESSION",
                                                                "../Grammar/BinaryInstruction.code");
     AsmPowerpcRegisterReferenceExpression.setDataPrototype("PowerpcRegisterClass","register_class","= powerpc_regclass_unknown",
                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     // GPR, FPR, SPR, TBR, etc. number, or condition register field or bit number
     AsmPowerpcRegisterReferenceExpression.setDataPrototype("int","register_number","= 0",
                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPowerpcRegisterReferenceExpression.setDataPrototype("PowerpcConditionRegisterAccessGranularity",
                                                            "conditionRegisterGranularity", "= powerpc_condreggranularity_whole",
                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



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



     /* FIXME: p_replacement is only set by RoseBin_IDAPRO_buildTree::resolveRecursivelyExpression() and appears to be used
      *        only in a couple of files in src/midend/binaryAnalsyses (and elsewhere only for converting a SgAsmExpression to
      *        a string). It seems to hold the name of a function, such as "_malloc" or "malloc@plt" for branch instructions.
      *        It should be possible to obtain the function name by looking up the instruction at the branch target and then
      *        following parent links in the AST until we reach the SgAsmFunctionDeclaration node, which has a get_name()
      *        method. [RPM 2009-07-16]. */
     AsmExpression.setFunctionPrototype("HEADER_BINARY_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmExpression.setDataPrototype("std::string", "replacement", "= \"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmExpression.setDataPrototype("std::string", "comment", "= \"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



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


// DQ (11/3/2008): Later this might go into a separate file.
// ***************************************************************************************
// DWARF Support for mapping information in the binary executable back to the source code.
// ***************************************************************************************

  // This is the base class for all the AsmDwarf IR nodes
     AsmDwarfInformation.setFunctionPrototype     ( "HEADER_DWARF_INFORMATION", "../Grammar/BinaryInstruction.code");

  // This is the Dwarf CU:
/*  Dwarf_Debug cc_dbg;
    Dwarf_Word cc_length;
    Dwarf_Small cc_length_size;
    Dwarf_Small cc_extension_size;
    Dwarf_Half cc_version_stamp;
    Dwarf_Sword cc_abbrev_offset;
    Dwarf_Small cc_address_size;
    Dwarf_Word cc_debug_info_offset;
    Dwarf_Byte_Ptr cc_last_abbrev_ptr;
    Dwarf_Hash_Table cc_abbrev_hash_table;
    Dwarf_CU_Context cc_next;
    unsigned char cc_offset_length;
*/
     AsmDwarfCompilationUnit.setFunctionPrototype ( "HEADER_DWARF_COMPILATION_UNIT", "../Grammar/BinaryInstruction.code");
  // AsmDwarfCompilationUnit.setDataPrototype("std::string","name","= \"\"",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("std::string","producer","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("std::string","language","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t","low_pc","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t","hi_pc","= 0x0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("int","version_stamp","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t","abbrev_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t","address_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("uint64_t","offset_length","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfLineList*","line_info","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfConstructList*","language_constructs","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfMacroList*","macro_info","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfCompilationUnitList.setFunctionPrototype  ( "HEADER_DWARF_COMPILATION_UNIT_LIST", "../Grammar/BinaryInstruction.code");
     AsmDwarfCompilationUnitList.setDataPrototype("SgAsmDwarfCompilationUnitPtrList","cu_list","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfMacroList.setFunctionPrototype ( "HEADER_DWARF_MACRO_LIST", "../Grammar/BinaryInstruction.code");
     AsmDwarfMacroList.setDataPrototype("SgAsmDwarfMacroPtrList","macro_list","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfMacro.setFunctionPrototype     ( "HEADER_DWARF_MACRO", "../Grammar/BinaryInstruction.code");
     AsmDwarfMacro.setDataPrototype("std::string","macro_string","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmDwarfLineList.setFunctionPrototype  ( "HEADER_DWARF_LINE_LIST", "../Grammar/BinaryInstruction.code");
     AsmDwarfLineList.setDataPrototype("SgAsmDwarfLinePtrList","line_list","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#if 0
  // I am having trouble making these proper data members so just use function to return them, so that they are computed dynamically.
     AsmDwarfLineList.setDataPrototype("SgInstructionAddressSourcePositionMapPtrList","instructionToSourceMap","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfLineList.setDataPrototype("SgSourcePositionInstructionAddressMapPtrList","sourceToInstructionMap","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

     AsmDwarfLine.setFunctionPrototype      ( "HEADER_DWARF_LINE", "../Grammar/BinaryInstruction.code");
     AsmDwarfLine.setDataPrototype("uint64_t","address","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfLine.setDataPrototype("int","file_id","= Sg_File_Info::NULL_FILE_ID",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfLine.setDataPrototype("int","line","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfLine.setDataPrototype("int","column","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmDwarfConstruct.setFunctionPrototype ( "HEADER_DWARF_CONSTRUCT", "../Grammar/BinaryInstruction.code");

  // For now we will store the nesting level (just to support debugging), then it will be removed.
     AsmDwarfConstruct.setDataPrototype("int","nesting_level","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmDwarfConstruct.setDataPrototype("uint64_t","offset","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDwarfConstruct.setDataPrototype("uint64_t","overall_offset","= 0",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // A lot of constructs have a name so put the name into the base class
     AsmDwarfConstruct.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This is a data member that can be filled in via a separate analysis (we would have to read the line information before hand).
     AsmDwarfConstruct.setDataPrototype("SgAsmDwarfLine*","source_position","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmDwarfConstructList.setFunctionPrototype ( "HEADER_DWARF_CONSTRUCT_LIST", "../Grammar/BinaryInstruction.code");
     AsmDwarfConstructList.setDataPrototype("SgAsmDwarfConstructPtrList","list","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // These are the Dwarf IR nodes that require support for children
     AsmDwarfSubprogram.setFunctionPrototype ( "HEADER_DWARF_SUBPROGRAM", "../Grammar/BinaryInstruction.code");
     AsmDwarfSubprogram.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfStructureType.setFunctionPrototype ( "HEADER_DWARF_STRUCTURE_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfStructureType.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfArrayType.setFunctionPrototype ( "HEADER_DWARF_ARRAY_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfArrayType.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfLexicalBlock.setFunctionPrototype ( "HEADER_DWARF_LEXICAL_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmDwarfLexicalBlock.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfInlinedSubroutine.setFunctionPrototype ( "HEADER_DWARF_INLINED_SUBROUTINE", "../Grammar/BinaryInstruction.code");
     AsmDwarfInlinedSubroutine.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfEnumerationType.setFunctionPrototype ( "HEADER_DWARF_ENUMERATION_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfEnumerationType.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfSubroutineType.setFunctionPrototype ( "HEADER_DWARF_SUBROUTINE_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfSubroutineType.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfUnionType.setFunctionPrototype ( "HEADER_DWARF_UNION_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfUnionType.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfNamespace.setFunctionPrototype ( "HEADER_DWARF_NAMESPACE", "../Grammar/BinaryInstruction.code");
     AsmDwarfNamespace.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfClassType.setFunctionPrototype ( "HEADER_DWARF_CLASS_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfClassType.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmDwarfCommonBlock.setFunctionPrototype ( "HEADER_DWARF_COMMON_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmDwarfCommonBlock.setDataPrototype("SgAsmDwarfConstructList*","body","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


// ***************************************************************************************
//                     END OF DWARF SPECIFIC IR NODE DEFINITIONS
// ***************************************************************************************




  // ***********************************************************************
  // ***********************************************************************
  //                       Source Code Definition
  // ***********************************************************************
  // ***********************************************************************

     AsmNode.setFunctionSource         ( "SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");

  // Binary File Format
     AsmExecutableFileFormat.setFunctionSource ( "SOURCE_EXECUTABLE_FILE_FORMAT", "../Grammar/BinaryInstruction.code");

  // DQ (9/9/2008): This is the class has has the std::string data member
     AsmBasicString.setFunctionSource ( "SOURCE_BASIC_STRING", "../Grammar/BinaryInstruction.code");

     AsmGenericHeader.setFunctionSource ( "SOURCE_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");
     AsmGenericSection.setFunctionSource ( "SOURCE_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmGenericFormat.setFunctionSource ( "SOURCE_GENERIC_FORMAT", "../Grammar/BinaryInstruction.code");
     AsmGenericSymbol.setFunctionSource ( "SOURCE_GENERIC_SYMBOL", "../Grammar/BinaryInstruction.code");
  // AsmGenericSectionList.setFunctionSource ( "SOURCE_GENERIC_SECTION_LIST", "../Grammar/BinaryInstruction.code");
     AsmGenericSymbolList.setFunctionSource ( "SOURCE_GENERIC_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmGenericDLL.setFunctionSource ( "SOURCE_GENERIC_DLL", "../Grammar/BinaryInstruction.code");
     AsmGenericDLLList.setFunctionSource ( "SOURCE_GENERIC_DLL_LIST", "../Grammar/BinaryInstruction.code");
     AsmGenericString.setFunctionSource ( "SOURCE_GENERIC_STRING", "../Grammar/BinaryInstruction.code");

     AsmElfFileHeader.setFunctionSource ( "SOURCE_ELF_HEADER", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTable.setFunctionSource ( "SOURCE_ELF_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTableEntry.setFunctionSource ( "SOURCE_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfStringSection.setFunctionSource("SOURCE_ELF_STRING_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSection.setFunctionSource ( "SOURCE_ELF_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfStrtab.setFunctionSource ( "SOURCE_ELF_STRING_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicSection.setFunctionSource ( "SOURCE_ELF_DYNAMIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicEntry.setFunctionSource ( "SOURCE_ELF_DYNAMIC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTable.setFunctionSource ( "SOURCE_ELF_SEGMENT_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTableEntry.setFunctionSource ( "SOURCE_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
  // AsmElfSegmentEntry.setFunctionSource ( "SOURCE_ELF_SEGMENT_ENTRY", "../Grammar/BinaryInstruction.code");
  // AsmElfSegmentEntryList.setFunctionSource ( "SOURCE_ELF_SEGMENT_ENTRY_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolSection.setFunctionSource ( "SOURCE_ELF_SYMBOL_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolList.setFunctionSource ( "SOURCE_ELF_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSymbol.setFunctionSource ( "SOURCE_ELF_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmStoredString.setFunctionSource ( "SOURCE_STORED_STRING", "../Grammar/BinaryInstruction.code");
     AsmStringStorage.setFunctionSource ( "SOURCE_STRING_STORAGE", "../Grammar/BinaryInstruction.code");

     AsmPERVASizePair.setFunctionSource ( "SOURCE_PE_RVA_SIZE_PAIR", "../Grammar/BinaryInstruction.code");
     AsmPEFileHeader.setFunctionSource ( "SOURCE_PE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmPESection.setFunctionSource ( "SOURCE_PE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPESectionTable.setFunctionSource ( "SOURCE_PE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbolTable.setFunctionSource ( "SOURCE_PE_COFF_SYMBOL_TABLE", "../Grammar/BinaryInstruction.code");
     AsmPEImportDirectory.setFunctionSource ( "SOURCE_PE_IMPORT_DIRECTORY", "../Grammar/BinaryInstruction.code");
     AsmPESectionTableEntry.setFunctionSource ( "SOURCE_PE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbol.setFunctionSource ( "SOURCE_PE_COFF_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmCoffStrtab.setFunctionSource("SOURCE_COFF_STRING_TABLE", "../Grammar/BinaryInstruction.code");

     AsmNEFileHeader.setFunctionSource ( "SOURCE_NE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmNESection.setFunctionSource ( "SOURCE_NE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmNESectionTable.setFunctionSource ( "SOURCE_NE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNENameTable.setFunctionSource ( "SOURCE_NE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEModuleTable.setFunctionSource ( "SOURCE_NE_MODULE_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEStringTable.setFunctionSource ( "SOURCE_NE_STRING_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEEntryTable.setFunctionSource ( "SOURCE_NE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNERelocTable.setFunctionSource ( "SOURCE_NE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEEntryPoint.setFunctionSource ( "SOURCE_NE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmNERelocEntry.setFunctionSource ( "SOURCE_NE_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmNESectionTableEntry.setFunctionSource ( "SOURCE_NE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");

     AsmLEFileHeader.setFunctionSource        ( "SOURCE_LE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmLESection.setFunctionSource           ( "SOURCE_LE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmLESectionTable.setFunctionSource      ( "SOURCE_LE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLENameTable.setFunctionSource         ( "SOURCE_LE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLEPageTable.setFunctionSource         ( "SOURCE_LE_PAGE_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLEEntryTable.setFunctionSource        ( "SOURCE_LE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLERelocTable.setFunctionSource        ( "SOURCE_LE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLEPageTableEntry.setFunctionSource    ( "SOURCE_LE_PAGE_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmLEEntryPoint.setFunctionSource        ( "SOURCE_LE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmLESectionTableEntry.setFunctionSource ( "SOURCE_LE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");

     AsmDOSFileHeader.setFunctionSource       ( "SOURCE_DOS_HEADER", "../Grammar/BinaryInstruction.code");


  // Non binary File IR node support

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
     AsmPowerpcRegisterReferenceExpression.setFunctionSource ( "SOURCE_BINARY_POWERPC_REGISTER_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmMemoryReferenceExpression.setFunctionSource ( "SOURCE_BINARY_MEMORY_REFERENCE_EXPRESSION", "../Grammar/BinaryInstruction.code");

     AsmByteValueExpression.setFunctionSource( "SOURCE_BINARY_BYTE_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmDoubleFloatValueExpression.setFunctionSource( "SOURCE_BINARY_DOUBLE_FLOAT_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmDoubleWordValueExpression.setFunctionSource( "SOURCE_BINARY_DOUBLE_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmQuadWordValueExpression.setFunctionSource( "SOURCE_BINARY_QUAD_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmSingleFloatValueExpression.setFunctionSource( "SOURCE_BINARY_SINGLE_FLOAT_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmVectorValueExpression.setFunctionSource( "SOURCE_BINARY_VECTOR_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");
     AsmWordValueExpression.setFunctionSource( "SOURCE_BINARY_WORD_VALUE_EXPRESSION", "../Grammar/BinaryInstruction.code");


#if USE_OLD_BINARY_EXECUTABLE_IR_NODES
     AsmSectionHeaderList.setFunctionSource ( "SOURCE_BINARY_FILE_SECTION_HEADER_LIST", "../Grammar/BinaryInstruction.code");
     AsmProgramHeaderList.setFunctionSource ( "SOURCE_BINARY_FILE_PROGRAM_HEADER_LIST", "../Grammar/BinaryInstruction.code");
#endif

// DQ (11/3/2008): Later this might go into a separate file.
// ***************************************************************************************
// DWARF Support for mapping information in the binary executable back to the source code.
// ***************************************************************************************

     AsmDwarfInformation.setFunctionSource       ( "SOURCE_DWARF_INFORMATION", "../Grammar/BinaryInstruction.code");
     AsmDwarfCompilationUnit.setFunctionSource   ( "SOURCE_DWARF_COMPILATION_UNIT", "../Grammar/BinaryInstruction.code");
     AsmDwarfMacro.setFunctionSource             ( "SOURCE_DWARF_MACRO", "../Grammar/BinaryInstruction.code");
     AsmDwarfLine.setFunctionSource              ( "SOURCE_DWARF_LINE", "../Grammar/BinaryInstruction.code");
     AsmDwarfMacroList.setFunctionSource         ( "SOURCE_DWARF_MACRO_LIST", "../Grammar/BinaryInstruction.code");
     AsmDwarfLineList.setFunctionSource          ( "SOURCE_DWARF_LINE_LIST", "../Grammar/BinaryInstruction.code");

     AsmDwarfConstruct.setFunctionSource         ( "SOURCE_DWARF_CONSTRUCT", "../Grammar/BinaryInstruction.code");
     AsmDwarfConstructList.setFunctionSource     ( "SOURCE_DWARF_CONSTRUCT_LIST", "../Grammar/BinaryInstruction.code");

     AsmDwarfCompilationUnitList.setFunctionSource ( "SOURCE_DWARF_COMPILATION_UNIT_LIST", "../Grammar/BinaryInstruction.code");

  // These are the Dwarf IR nodes that require support for children
     AsmDwarfSubprogram.setFunctionSource        ( "SOURCE_DWARF_SUBPROGRAM", "../Grammar/BinaryInstruction.code");
     AsmDwarfStructureType.setFunctionSource     ( "SOURCE_DWARF_STRUCTURE_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfArrayType.setFunctionSource         ( "SOURCE_DWARF_ARRAY_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfLexicalBlock.setFunctionSource      ( "SOURCE_DWARF_LEXICAL_BLOCK", "../Grammar/BinaryInstruction.code");
     AsmDwarfInlinedSubroutine.setFunctionSource ( "SOURCE_DWARF_INLINED_SUBROUTINE", "../Grammar/BinaryInstruction.code");
     AsmDwarfEnumerationType.setFunctionSource   ( "SOURCE_DWARF_ENUMERATION_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfSubroutineType.setFunctionSource    ( "SOURCE_DWARF_SUBROUTINE_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfUnionType.setFunctionSource         ( "SOURCE_DWARF_UNION_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfNamespace.setFunctionSource         ( "SOURCE_DWARF_NAMESPACE", "../Grammar/BinaryInstruction.code");
     AsmDwarfClassType.setFunctionSource         ( "SOURCE_DWARF_CLASS_TYPE", "../Grammar/BinaryInstruction.code");
     AsmDwarfCommonBlock.setFunctionSource       ( "SOURCE_DWARF_COMMON_BLOCK", "../Grammar/BinaryInstruction.code");

// ***************************************************************************************
//                     END OF DWARF SPECIFIC IR NODE DEFINITIONS
// ***************************************************************************************


   }
