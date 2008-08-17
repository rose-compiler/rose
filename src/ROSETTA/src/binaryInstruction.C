
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
     NEW_NONTERMINAL_MACRO ( AsmRegisterReferenceExpression , Asmx86RegisterReferenceExpression | AsmArmRegisterReferenceExpression, "AsmRegisterReferenceExpression", "AsmRegisterReferenceExpressionTag" , false);

  // NEW_TERMINAL_MACRO ( AsmArmRegisterReferenceExpression , "AsmArmRegisterReferenceExpression", "AsmArmRegisterReferenceExpressionTag" );

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
                                      AsmTypeVector, "AsmType", "AsmTypeTag", false );

  // Support for DLL's for different file formats (there appears to only be a PE form of DLL)
  // NEW_TERMINAL_MACRO ( AsmLEDLL, "AsmLEDLL", "AsmLEDLLTag" );

     NEW_TERMINAL_MACRO ( AsmPEDLL, "AsmPEDLL", "AsmPEDLLTag" );
     NEW_NONTERMINAL_MACRO ( AsmGenericDLL, AsmPEDLL , "AsmGenericDLL",    "AsmGenericDLLTag", false );

     NEW_TERMINAL_MACRO ( AsmPEImportHintNameList, "AsmPEImportHintNameList", "AsmPEImportHintNameListTag" );

     NEW_TERMINAL_MACRO ( AsmGenericFormat,      "AsmGenericFormat",      "AsmGenericFormatTag" );

     NEW_TERMINAL_MACRO ( AsmGenericArchitecture,"AsmGenericArchitecture","AsmGenericArchitectureTag" );
     NEW_TERMINAL_MACRO ( AsmGenericFile,        "AsmGenericFile",        "AsmGenericFileTag" );

  // Support for different types of header for binary executable file formats.
     NEW_TERMINAL_MACRO ( AsmElfFileHeader,      "AsmElfFileHeader",      "AsmElfFileHeaderTag"   );
     NEW_TERMINAL_MACRO ( AsmPEFileHeader,       "AsmPEFileHeader",       "AsmPEFileHeaderTag"    );
     NEW_TERMINAL_MACRO ( AsmNEFileHeader,       "AsmNEFileHeader",       "AsmNEFileHeaderTag"    );
     NEW_TERMINAL_MACRO ( AsmLEFileHeader,       "AsmLEFileHeader",       "AsmLEFileHeaderTag"    );
     NEW_TERMINAL_MACRO ( AsmDOSFileHeader,      "AsmDOSFileHeader",      "AsmDOSFileHeaderTag"    );
     NEW_NONTERMINAL_MACRO ( AsmGenericHeader, AsmPEFileHeader  | AsmLEFileHeader |  AsmNEFileHeader | AsmDOSFileHeader |  AsmElfFileHeader, "AsmGenericHeader",    "AsmGenericHeaderTag", false );

  // A lot of IR nodes are derived from the AsmGenericSection (segments were eliminated and became sections under Robb's recent changes).
     NEW_TERMINAL_MACRO    ( AsmElfDynamicSection,"AsmElfDynamicSection","AsmElfDynamicSectionTag");
     NEW_TERMINAL_MACRO    ( AsmElfSymbolSection, "AsmElfSymbolSection", "AsmElfSymbolSectionTag" );
     NEW_NONTERMINAL_MACRO ( AsmElfSection, AsmElfSymbolSection | AsmElfDynamicSection, "AsmElfSection", "AsmElfSectionTag", false );

     NEW_TERMINAL_MACRO    ( AsmElfSectionTable,  "AsmElfSectionTable",  "AsmElfSectionTableTag"  );
     NEW_TERMINAL_MACRO    ( AsmElfSegmentTable,  "AsmElfSegmentTable",  "AsmElfSegmentTableTag"  );

     NEW_TERMINAL_MACRO    ( AsmPEImportSection, "AsmPEImportSection", "AsmPEImportSectionTag" );
     NEW_NONTERMINAL_MACRO ( AsmPESection, AsmPEImportSection, "AsmPESection",       "AsmPESectionTag", false );

     NEW_TERMINAL_MACRO    ( AsmPESectionTable,  "AsmPESectionTable",  "AsmPESectionTableTag"  );
     NEW_TERMINAL_MACRO    ( AsmCoffSymbolTable, "AsmCoffSymbolTable", "AsmCoffSymbolTableTag" );
     NEW_TERMINAL_MACRO    ( AsmPEExtendedDOSHeader, "AsmPEExtendedDOSHeader", "AsmPEExtendedDOSHeaderTag" );

     NEW_TERMINAL_MACRO    ( AsmNESection,           "AsmNESection",           "AsmNESectionTag"      );
     NEW_TERMINAL_MACRO    ( AsmNESectionTable,      "AsmNESectionTable",      "AsmNESectionTableTag" );
     NEW_TERMINAL_MACRO    ( AsmNENameTable,         "AsmNENameTable",         "AsmNENameTableTag"    );
     NEW_TERMINAL_MACRO    ( AsmNEModuleTable,       "AsmNEModuleTable",       "AsmNEModuleTableTag"  );
     NEW_TERMINAL_MACRO    ( AsmNEStringTable,       "AsmNEStringTable",       "AsmNEStringTableTag"  );
     NEW_TERMINAL_MACRO    ( AsmNEEntryTable,        "AsmNEEntryTable",        "AsmNEEntryTableTag"   );
     NEW_TERMINAL_MACRO    ( AsmNERelocTable,        "AsmNERelocTable",        "AsmNERelocTableTag"   );
     NEW_TERMINAL_MACRO    ( AsmNEExtendedDOSHeader, "AsmNEExtendedDOSHeader", "AsmNEExtendedDOSHeaderTag" );

     NEW_TERMINAL_MACRO    ( AsmLESection,      "AsmLESection",      "AsmLESectionTag"      );
     NEW_TERMINAL_MACRO    ( AsmLESectionTable, "AsmLESectionTable", "AsmLESectionTableTag" );
     NEW_TERMINAL_MACRO    ( AsmLENameTable,    "AsmLENameTable",    "AsmLENameTableTag"    );
     NEW_TERMINAL_MACRO    ( AsmLEPageTable,    "AsmLEPageTable",    "AsmLEPageTableTag"    );
     NEW_TERMINAL_MACRO    ( AsmLEEntryTable,   "AsmLEEntryTable",   "AsmLEEntryTableTag"   );
     NEW_TERMINAL_MACRO    ( AsmLERelocTable,   "AsmLERelocTable",   "AsmLERelocTableTag"   );

     NEW_NONTERMINAL_MACRO ( AsmGenericSection, 
            AsmGenericHeader | 
            AsmElfSection    | AsmElfSectionTable | AsmElfSegmentTable     |
            AsmPESection     | AsmPESectionTable  | AsmPEExtendedDOSHeader | AsmCoffSymbolTable  |
            AsmNESection     | AsmNESectionTable  | AsmNEExtendedDOSHeader | AsmNENameTable      | AsmNEModuleTable | AsmNEStringTable | AsmNEEntryTable | AsmNERelocTable |
            AsmLESection     | AsmLESectionTable  | AsmLENameTable         | AsmLEPageTable      | AsmLEEntryTable  | AsmLERelocTable,
           "AsmGenericSection",    "AsmGenericSectionTag", false );

  // Support for Symbols in the binary executable.
     NEW_TERMINAL_MACRO    ( AsmCoffSymbol,   "AsmCoffSymbol", "AsmCoffSymbolTag"    );
     NEW_TERMINAL_MACRO    ( AsmElfSymbol,    "AsmElfSymbol",  "AsmElfSymbolTag"    );
     NEW_NONTERMINAL_MACRO ( AsmGenericSymbol, AsmCoffSymbol  | AsmElfSymbol, "AsmGenericSymbol",    "AsmGenericSymbolTag", false );

  // These are supporting IR nodes to be gathered into (derived from) the AsmGenericBinaryFileSupport class
     NEW_TERMINAL_MACRO    ( AsmElfSectionTableEntry,     "AsmElfSectionTableEntry",     "AsmElfSectionTableEntryTag"     );
     NEW_TERMINAL_MACRO    ( AsmElfSegmentTableEntry,     "AsmElfSegmentTableEntry",     "AsmElfSegmentTableEntryTag"     );
     NEW_TERMINAL_MACRO    ( AsmElfSegmentTableEntryList, "AsmElfSegmentTableEntryList", "AsmElfSegmentTableEntryListTag" );
     NEW_TERMINAL_MACRO    ( AsmElfDynamicEntry,          "AsmElfDynamicEntry",          "AsmElfDynamicEntryTag"          );
     NEW_TERMINAL_MACRO    ( AsmElfDynamicEntryList,      "AsmElfDynamicEntryList",      "AsmElfDynamicEntryListTag"      );

     NEW_TERMINAL_MACRO    ( AsmPERVASizePair,       "AsmPERVASizePair",       "AsmPERVASizePairTag"       );
     NEW_TERMINAL_MACRO    ( AsmPEImportDirectory,   "AsmPEImportDirectory",   "AsmPEImportDirectoryTag"   );
     NEW_TERMINAL_MACRO    ( AsmPEImportHintName,    "AsmPEImportHintName",    "AsmPEImportHintNameTag"    );
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


  // DQ (8/2/2008): These were removed from the design by Robb (segments and sections are now the same: as sections).
  // NEW_TERMINAL_MACRO ( AsmGenericSegmentList, "AsmGenericSegmentList", "AsmGenericSegmentListTag" );
  // NEW_TERMINAL_MACRO ( AsmElfSegment,         "AsmElfSegment",         "AsmElfSegmentTag" );
  // NEW_NONTERMINAL_MACRO ( AsmGenericSegment, AsmElfSegment, "AsmGenericSegment", "AsmGenericSegmentTag", false );
  // NEW_NONTERMINAL_MACRO ( AsmGenericSupport, AsmGenericFile        | AsmGenericHeader | AsmGenericSection      | 
  //                                            AsmGenericSegment     | AsmGenericFormat | AsmGenericArchitecture | 
  //                                            AsmGenericSectionList | AsmGenericSegmentList,
  //                         "AsmGenericSupport",    "AsmGenericSupportTag", false );

  // Root of class hierarchy for binary file support
     NEW_NONTERMINAL_MACRO ( AsmExecutableFileFormat,
               AsmGenericDLL           | AsmGenericFormat        | AsmGenericArchitecture | AsmGenericDLLList      |
               AsmGenericFile          | AsmGenericSection       | AsmGenericSymbol       | AsmGenericSymbolList   |
               AsmElfSectionTableEntry | AsmElfSegmentTableEntry | AsmElfSymbolList       | AsmElfDynamicEntry     | AsmElfDynamicEntryList | AsmElfSegmentTableEntryList |
               AsmPEImportDirectory    | AsmPEImportHintName     | AsmPESectionTableEntry | AsmPERVASizePair       | AsmCoffSymbolList      | AsmPERVASizePairList        |
               AsmNEEntryPoint         | AsmNERelocEntry         | AsmNESectionTableEntry |
               AsmLEPageTableEntry     | AsmLEEntryPoint         | AsmLESectionTableEntry | 
               AsmGenericSectionList   | AsmGenericHeaderList    | AsmPEImportHintNameList, "AsmExecutableFileFormat", "AsmExecutableFileFormatTag", false );


  // This is the IR node for a binary executable that loosely corresponds to the SgFile IR node for 
  // source code. The kinds of information that we want to save for each is really quire different.
     NEW_TERMINAL_MACRO ( AsmFile                     , "AsmFile",                     "AsmFileTag" );

#if USE_OLD_BINARY_EXECUTABLE_IR_NODES
  // DQ (8/2/2008): these might be required for now, but we need to be removed later!

#error "Dead Code!"

  // DQ (1/6/2008): Added ELF program header and section header support to AST.
     NEW_TERMINAL_MACRO ( AsmProgramHeader, "AsmProgramHeader", "AsmProgramHeaderTag" );
     NEW_TERMINAL_MACRO ( AsmSectionHeader, "AsmSectionHeader", "AsmSectionHeaderTag" );

  // DQ (1/6/2008): These store the lists of AsmProgramHeader and AsmSectionHeader objects.
  // AsmFile can not have traversed data members and lists, so this give it two additional data members.
     NEW_TERMINAL_MACRO ( AsmProgramHeaderList, "AsmProgramHeaderList", "AsmProgramHeaderListTag" );
     NEW_TERMINAL_MACRO ( AsmSectionHeaderList, "AsmSectionHeaderList", "AsmSectionHeaderListTag" );

  // We will elimiate: AsmFile, AsmProgramHeader, AsmSectionHeader, AsmProgramHeaderList, AsmSectionHeaderList
  // NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmProgramHeader | AsmSectionHeader | AsmProgramHeaderList | AsmSectionHeaderList | AsmOperandList | AsmType | AsmExecutableFileFormat, "AsmNode","AsmNodeTag", false);
  // NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmProgramHeader | AsmSectionHeader | AsmProgramHeaderList | AsmSectionHeaderList | AsmOperandList | AsmType | AsmExecutableFileFormat, "AsmNode","AsmNodeTag", false);
     NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmProgramHeader | AsmSectionHeader | AsmProgramHeaderList | AsmSectionHeaderList | AsmOperandList | AsmType | AsmExecutableFileFormat, "AsmNode","AsmNodeTag", false);
#else
     NEW_NONTERMINAL_MACRO (AsmNode, AsmStatement | AsmExpression | AsmFile | AsmOperandList | AsmType | AsmExecutableFileFormat, "AsmNode","AsmNodeTag", false);
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
     AsmInstruction.setDataPrototype("std::string","raw_bytes","= \"\"",
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

     AsmBlock.setDataPrototype("bool","externallyVisible","= true", // Can this block be called into from random code?
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



     AsmFile.setFunctionPrototype ( "HEADER_BINARY_FILE", "../Grammar/BinaryInstruction.code");

  // This is the filename of the binary executable...
     AsmFile.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This is where the instructions are put...
     AsmFile.setDataPrototype("SgAsmBlock*","global_block","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (8/16/2008): Robb suggested that this be a list since some PE files have multiple headers.
  // DQ (8/12/2008): This is the connection to Robb's work.
  // AsmFile.setDataPrototype("SgAsmGenericHeader*","header","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  // AsmFile.setDataPrototype("SgAsmGenericHeaderList*","headers","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmGenericFile*","genericFile","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (8/13/2008): Required data member for Jeremiah's ROSE/projects/assemblyToSourceAst/x86AssemblyToC.C
  // DQ (8/13/2008): This needs to be removed once the x86AssemblyToC.C file is fixed up to not require it.
  // This is redundant with the more complete information in the SgAsmGenericSections of the binary file format.
     AsmFile.setDataPrototype("unsigned long", "associated_entry_point","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

#if 0
  // DQ (8/13/2008): Removing data members not required for using Robb's work (or anywhere else in ROSE, 
  // now that we no longer use the older binary file format support).

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
     AsmFile.setDataPrototype("std::string", "magic_number_string","= \"\"",
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

  // In the new design these are sections 
     AsmFile.setDataPrototype("SgAsmProgramHeaderList*", "programHeaderList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFile.setDataPrototype("SgAsmSectionHeaderList*", "sectionHeaderList","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif
#endif

#if USE_OLD_BINARY_EXECUTABLE_IR_NODES
  // *****************************************************
  //           OLD BINARY FILE FORMAT IR NODES
  // *****************************************************

     AsmSectionHeaderList.setFunctionPrototype ( "HEADER_BINARY_FILE_SECTION_HEADER_LIST", "../Grammar/BinaryInstruction.code");
     AsmSectionHeaderList.setDataPrototype("SgAsmSectionHeaderPtrList","section_headers","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmProgramHeaderList.setFunctionPrototype ( "HEADER_BINARY_FILE_PROGRAM_HEADER_LIST", "../Grammar/BinaryInstruction.code");
     AsmProgramHeaderList.setDataPrototype("SgAsmProgramHeaderPtrList","program_headers","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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
     AsmSectionHeader.setDataPrototype("std::string","name","= \"\"",
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
#endif

  // *****************************************************
  //           NEW BINARY FILE FORMAT IR NODES
  // *****************************************************

  // This is a location where we can put some types that are used everywhere in the binary file format support.
     AsmExecutableFileFormat.setFunctionPrototype ( "HEADER_EXECUTABLE_FILE_FORMAT", "../Grammar/BinaryInstruction.code");

  // These are the native-format versions of the same members described in Elf*FileHeader_disk
  // unsigned char       e_ident_file_class, e_ident_data_encoding, e_ident_file_version, e_ident_padding[9];
  // unsigned            e_type, e_machine, e_version;
  // addr_t              e_entry, e_phoff, e_shoff;
  // unsigned            e_flags, e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;
  // Additional members not trivially present in the file header but logically part of an Elf header
  // class ElfSectionTable *section_table;
  // class ElfSegmentTable *segment_table;
     AsmElfFileHeader.setFunctionPrototype ( "HEADER_ELF_HEADER", "../Grammar/BinaryInstruction.code");
  /* Section in which this segment lives */
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
     AsmElfFileHeader.setDataPrototype("unsigned long","e_version","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_entry","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_phoff","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_shoff","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_ehsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_phentsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_phnum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_shentsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_shnum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("unsigned long","e_shstrndx","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgAsmElfSectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfFileHeader.setDataPrototype("SgAsmElfSegmentTable*","segment_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmElfSectionTable.setFunctionPrototype ( "HEADER_ELF_SECTION_TABLE", "../Grammar/BinaryInstruction.code");

  // Note that these should likely be implemented as a container, instead of reproducing the link list structure.
  // ElfSection *linked_section;
  // ElfSectionTableEntry *st_entry;
     AsmElfSection.setFunctionPrototype      ( "HEADER_ELF_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSection.setDataPrototype("SgAsmElfSection*","linked_section","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSection.setDataPrototype("SgAsmElfSectionTableEntry*","st_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // unsigned            dt_pltrelsz;                    /* Size in bytes of PLT relocations */
  // addr_t              dt_pltgot;                      /* Address of global offset table */
  // addr_t              dt_hash;                        /* Address of symbol hash table */
  // addr_t              dt_strtab;                      /* Address of dynamic string table */
  // addr_t              dt_symtab;                      /* Address of symbol table */
  // addr_t              dt_rela;                        /* Address of Rela relocations */
  // unsigned            dt_relasz;                      /* Total size in bytes of Rela relocations */
  // unsigned            dt_relaent;                     /* Size of one Rela relocation */
  // unsigned            dt_strsz;                       /* Size in bytes of string table */
  // unsigned            dt_symentsz;                    /* Size in bytes of one symbol table entry */
  // addr_t              dt_init;                        /* Address of initialization function */
  // addr_t              dt_fini;                        /* Address of termination function */
  // unsigned            dt_pltrel;                      /* Type of relocation in PLT */
  // addr_t              dt_jmprel;                      /* Address of PLT relocations */
  // unsigned            dt_verneednum;                  /* Number of entries in dt_verneed table */
  // addr_t              dt_verneed;                     /* Address of table with needed versions */
  // addr_t              dt_versym;                      /* GNU version symbol address */
  // std::vector<ElfDynamicEntry*> other_entries;        /* Other values not specifically parsed out */
  // std::vector<ElfDynamicEntry*> all_entries;          /* All parsed entries in order of appearance */
     AsmElfDynamicSection.setFunctionPrototype ( "HEADER_ELF_DYNAMIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicSection.setDataPrototype("unsigned","dt_pltrelsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_pltgot","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_hash","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_strtab","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_symtab","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_rela","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("unsigned","dt_relasz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("unsigned","dt_relaent","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("unsigned","dt_strsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("unsigned","dt_symentsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_init","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_fini","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("unsigned","dt_pltrel","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_jmprel","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("unsigned","dt_verneednum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_verneed","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_versym","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmElfDynamicEntryList*","other_entries","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicSection.setDataPrototype("SgAsmElfDynamicEntryList*","all_entries","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // unsigned            d_tag;
  // addr_t              d_val;
     AsmElfDynamicEntry.setFunctionPrototype ( "HEADER_ELF_DYNAMIC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicEntry.setDataPrototype("unsigned","d_tag","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfDynamicEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","d_val","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // AsmElfDynamicEntryList.setFunctionPrototype ( "HEADER_ELF_DYNAMIC_ENTRY_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicEntryList.setDataPrototype("SgAsmElfDynamicEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



  // unsigned             sh_name, sh_type, sh_link, sh_info;
  // uint64_t             sh_flags;
  // addr_t               sh_addr, sh_offset, sh_size, sh_addralign, sh_entsize;
  // const unsigned char* extra;
  // addr_t               nextra;
     AsmElfSectionTableEntry.setFunctionPrototype ( "HEADER_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTableEntry.setDataPrototype("unsigned","sh_name","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_link","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("unsigned long","sh_info","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("uint64_t","sh_flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","sh_addr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","sh_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","sh_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","sh_addralign","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","sh_entsize","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (6/14/2008): I think we can let this be considered an offset into the start of the mapped file (double check on this).
     AsmElfSectionTableEntry.setDataPrototype("SgUnsignedCharList","extra","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ: Note that this is the size of the "SgUnsignedCharList extra" array, so this may be redundant because we use an STL vector for "extra".
     AsmElfSectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","nextra","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // std::vector<ElfSegmentTableEntry*> entries;
     AsmElfSegmentTable.setFunctionPrototype ( "HEADER_ELF_SEGMENT_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTable.setDataPrototype("SgAsmElfSegmentTableEntryList*","entries","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // SegmentType         p_type;
  // SegmentFlags        p_flags;
  // addr_t              p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_align;
  // const unsigned char *extra;
  // addr_t              nextra;
     AsmElfSegmentTableEntry.setFunctionPrototype ( "HEADER_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");

  // DQ (6/14/2008): Need to rename these data members because ROSETTA will generate "p_p_type" etc. which is unmanageable.
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentType","type","= SgAsmElfSegmentTableEntry::PT_NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentFlags","flags","= SgAsmElfSegmentTableEntry::PF_RESERVED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","vaddr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","paddr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","filesz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","memsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","align","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    /* The ELF header can define a segment table entry to be larger than the Elf*SegmentTableEntry_disk struct, so any
     * extra data gets stuffed into this member, which is a pointer directly into the mapped file and is null if there
     * is no extra data. */
  // DQ (6/14/2008): I think we can let this be considered an offset into the start of the mapped file (double check on this).
     AsmElfSegmentTableEntry.setDataPrototype("SgUnsignedCharList","extra","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmElfSegmentTableEntryList.setFunctionPrototype ( "HEADER_ELF_SEGMENT_TABLE_ENTRY_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTableEntryList.setDataPrototype("SgAsmElfSegmentTableEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);




  // DQ (8/2/2008): This was removed from the design by Robb.
#if 0
  /* The pointer to the actual segment on disk */
  // AsmElfSegmentTableEntry.setDataPrototype("SgAsmGenericSegment*","segment","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmElfSegment.setFunctionPrototype ( "HEADER_ELF_SEGMENT", "../Grammar/BinaryInstruction.code");
  /* Size in bytes of PLT relocations */
     AsmElfSegment.setDataPrototype("unsigned long","dt_pltrelsz","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of global offset table */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_pltgot","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of symbol hash table */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_hash","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of dynamic string table */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_strtab","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of symbol table */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_symtab","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of Rela relocations */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_rela","= 0",
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
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_init","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of termination function */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_fini","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Type of relocation in PLT */
     AsmElfSegment.setDataPrototype("unsigned long","dt_pltrel","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of PLT relocations */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_jmprel","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Number of entries in dt_verneed table */
     AsmElfSegment.setDataPrototype("unsigned long","dt_verneednum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Address of table with needed versions */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_verneed","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* GNU version symbol address */
     AsmElfSegment.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dt_versym","= 0",
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

  // DQ (8/2/2008): This was removed from the design by Robb.
#if 0
     AsmElfSegmentEntry.setFunctionPrototype ( "HEADER_ELF_SEGMENT_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentEntry.setDataPrototype("unsigned long","d_tag","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSegmentEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","d_val","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


#if 0
    unsigned            d_tag;
    addr_t              d_val;
#endif

     AsmElfSegmentEntryList.setFunctionPrototype ( "HEADER_ELF_SEGMENT_ENTRY_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentEntryList.setDataPrototype("SgAsmElfSegmentEntryPtrList","segment_entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif
#endif


  // std::vector<ElfSymbol> symbols;
     AsmElfSymbolSection.setFunctionPrototype      ( "HEADER_ELF_SYMBOL_SECTION",       "../Grammar/BinaryInstruction.code");
  // AsmElfSymbolSection.setDataPrototype("unsigned long","x_","= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbolSection.setDataPrototype("SgAsmElfSymbolList*","symbols","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


     AsmElfSymbolList.setFunctionPrototype ( "HEADER_ELF_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolList.setDataPrototype("SgAsmElfSymbolPtrList","symbols","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Members defined by the ELF standard
  // addr_t              st_name;
  // unsigned char       st_info, st_res1;
  // unsigned            st_shndx;
  // addr_t              st_size; // original size for unparsing; superclass holds adjusted size
     AsmElfSymbol.setFunctionPrototype      ( "HEADER_ELF_SYMBOL",       "../Grammar/BinaryInstruction.code");
  // DQ (8/3/2008): This is now in the base class
  // AsmElfSymbol.setDataPrototype("std::string","name","= \"\"",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("SgAsmExecutableFileFormat::addr_t","st_name","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned char","st_info","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned char","st_res1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("unsigned","st_shndx","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmElfSymbol.setDataPrototype("SgAsmExecutableFileFormat::addr_t","st_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);







  // These are the native-format versions of the same members described in the PEFileHeader_disk format struct.
  // unsigned    e_cpu_type, e_nsections, e_time;
  // addr_t      e_coff_symtab, e_nt_hdr_size;
  // unsigned    e_coff_nsyms, e_flags, e_opt_magic;
  // unsigned    e_lmajor, e_lminor, e_code_size, e_data_size, e_bss_size, e_entrypoint_rva, e_code_rva, e_data_rva;
  // addr_t      e_image_base;
  // unsigned    e_section_align, e_file_align, e_os_major, e_os_minor, e_user_major, e_user_minor;
  // unsigned    e_subsys_major, e_subsys_minor, e_reserved9, e_image_size, e_header_size, e_file_checksum, e_subsystem;
  // unsigned    e_dll_flags, e_stack_reserve_size, e_stack_commit_size, e_heap_reserve_size, e_heap_commit_size;
  // unsigned    e_loader_flags, e_num_rvasize_pairs;
  // std::vector<RVASizePair> rvasize_pairs;
  // ExtendedDOSHeader *dos2_header;
  // PESectionTable *section_table;
  // COFFSymtab *coff_symtab;

     AsmPEFileHeader.setFunctionPrototype ( "HEADER_PE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmPEFileHeader.setAutomaticGenerationOfDestructor(false);

     AsmPEFileHeader.setDataPrototype("unsigned","e_cpu_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_nsections","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_time","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_coff_symtab","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_nt_hdr_size","= 0",
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
     AsmPEFileHeader.setDataPrototype("unsigned","e_entrypoint_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_code_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("unsigned","e_data_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_image_base","= 0",
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
  // AsmPEFileHeader.setDataPrototype("std::vector<RVASizePair>","rvasize_pairs","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmPERVASizePairList*","rvasize_pairs","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmPEExtendedDOSHeader*","dos2_header","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmPESectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEFileHeader.setDataPrototype("SgAsmCoffSymbolTable*","coff_symtab","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // addr_t e_rva, e_size;
     AsmPERVASizePair.setFunctionPrototype ( "HEADER_PE_RVA_SIZE_PAIR", "../Grammar/BinaryInstruction.code");
     AsmPERVASizePair.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPERVASizePair.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

     AsmPERVASizePairList.setFunctionPrototype ( "HEADER_PE_RVA_SIZE_PAIR_LIST", "../Grammar/BinaryInstruction.code");
     AsmPERVASizePairList.setDataPrototype("SgAsmPERVASizePairPtrList","pairs","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // PESectionTableEntry *st_entry;
     AsmPESection.setFunctionPrototype ( "HEADER_PE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPESection.setDataPrototype("SgAsmPESectionTableEntry*","st_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // No data members
     AsmPESectionTable.setFunctionPrototype ( "HEADER_PE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");

  // std::string name;
  // addr_t      virtual_size, rva, physical_size, physical_offset;
  // unsigned    coff_line_nums, n_relocs, n_coff_line_nums, flags;
     AsmPESectionTableEntry.setFunctionPrototype ( "HEADER_PE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmPESectionTableEntry.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","virtual_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","physical_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","physical_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned","coff_line_nums","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned","n_relocs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned","n_coff_line_nums","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPESectionTableEntry.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



  // std::vector<PEDLL*> dlls;
     AsmPEImportSection.setFunctionPrototype ( "HEADER_PE_IMPORT_SECTION", "../Grammar/BinaryInstruction.code");
  // AsmPEImportSection.setDataPrototype("SgAsmPEDLLPtrList","section_table","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportSection.setDataPrototype("SgAsmPEDLLPtrList","dlls","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // std::vector<COFFSymbol*> symbols;
  // ExecSection *strtab;                /* Section containing symbol names */
     AsmCoffSymbolTable.setFunctionPrototype ( "HEADER_PE_COFF_SYMBOL_TABLE", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbolTable.setDataPrototype("SgAsmCoffSymbolList*","symbols","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
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
     AsmCoffSymbol.setDataPrototype("SgAsmExecutableFileFormat::addr_t","st_name_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("int","st_section_num","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("unsigned","st_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("unsigned","st_storage_class","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("unsigned","st_num_aux_entries","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmCoffSymbol.setDataPrototype("SgUnsignedCharList","aux_data","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // This is the IR node to hold a list of symbols (needed because we can't mix children from data 
  // members with children from containers).
     AsmCoffSymbolList.setFunctionPrototype ( "HEADER_PE_COFF_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbolList.setDataPrototype("SgAsmCoffSymbolPtrList","symbols","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // unsigned e_res1[4], e_oemid, e_oeminfo, e_res2[10];
  // addr_t e_lfanew;
     AsmPEExtendedDOSHeader.setFunctionPrototype ( "HEADER_PE_EXTENDED_DOS_HEADER", "../Grammar/BinaryInstruction.code");
     AsmPEExtendedDOSHeader.setDataPrototype("SgUnsignedList","e_res1","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExtendedDOSHeader.setDataPrototype("unsigned","e_oemid","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExtendedDOSHeader.setDataPrototype("unsigned","e_oeminfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExtendedDOSHeader.setDataPrototype("SgUnsignedList","e_res2","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEExtendedDOSHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_lfanew","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // time_t testVariable;
  // addr_t    hintnames_rva, bindings_rva, dll_name_rva;
  // time_t    time;
  // unsigned  forwarder_chain;
     AsmPEImportDirectory.setFunctionPrototype ( "HEADER_PE_IMPORT_DIRECTORY", "../Grammar/BinaryInstruction.code");
     AsmPEImportDirectory.setDataPrototype("SgAsmExecutableFileFormat::addr_t","hintnames_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("SgAsmExecutableFileFormat::addr_t","bindings_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("SgAsmExecutableFileFormat::addr_t","dll_name_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("time_t","time","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportDirectory.setDataPrototype("unsigned","forwarder_chain","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // unsigned hint;
  // std::string name;
  // unsigned char padding;
     AsmPEImportHintName.setFunctionPrototype ( "HEADER_PE_IMPORT_HINT_NAME", "../Grammar/BinaryInstruction.code");
     AsmPEImportHintName.setDataPrototype("unsigned","hint","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportHintName.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmPEImportHintName.setDataPrototype("unsigned char","padding","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // PEImportDirectory *idir;
  // std::vector<addr_t> hintname_rvas;          /* RVAs for the hint/name pairs of the DLL functions */
  // std::vector<PEImportHintName*> hintnames;   /* The hint/name pairs */
  // std::vector<addr_t> bindings;               /* Bindings (RVA) for each function */
     AsmPEDLL.setFunctionPrototype ( "HEADER_PE_DLL", "../Grammar/BinaryInstruction.code");
     AsmPEDLL.setDataPrototype ("SgAsmPEImportDirectory*","idir","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmPEDLL.setDataPrototype ("SgAddressList","hintname_rvas","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmPEDLL.setDataPrototype ("SgAsmPEImportHintNameList*","hintnames","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmPEDLL.setDataPrototype ("SgAddressList","bindings","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);

     AsmPEImportHintNameList.setDataPrototype("SgAsmPEImportHintNamePtrList","hintnames","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


  // unsigned    e_linker_major, e_linker_minor, e_checksum, e_flags1, e_autodata_sn, e_bss_size, e_stack_size;
  // unsigned    e_csip, e_sssp, e_nsections, e_nmodrefs, e_nnonresnames, e_nmovable_entries, e_sector_align;
  // unsigned    e_nresources, e_exetype, e_flags2, e_res1, e_winvers;
  // addr_t      e_entrytab_rfo, e_entrytab_size, e_sectab_rfo, e_rsrctab_rfo, e_resnametab_rfo, e_modreftab_rfo;
  // addr_t      e_importnametab_rfo, e_nonresnametab_offset, e_fastload_sector, e_fastload_nsectors;
  // ExtendedDOSHeader *dos2_header;
  // NESectionTable *section_table;
  // NENameTable *resname_table;
  // NENameTable *nonresname_table;
  // NEModuleTable *module_table;
  // NEEntryTable *entry_table;
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
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_entrytab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_entrytab_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_sectab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_rsrctab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_resnametab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_modreftab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_importnametab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_nonresnametab_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_fastload_sector","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_fastload_nsectors","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNEExtendedDOSHeader*","dos2_header","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNESectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNENameTable*","resname_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNENameTable*","nonresname_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNEModuleTable*","module_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEFileHeader.setDataPrototype("SgAsmNEEntryTable*","entry_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // NESectionTableEntry *st_entry;
  // NERelocTable *reloc_table;
     AsmNESection.setFunctionPrototype ( "HEADER_NE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmNESection.setDataPrototype("SgAsmNESectionTableEntry*","st_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESection.setDataPrototype("SgAsmNERelocTable*","reloc_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // unsigned    flags, sector;
  // addr_t      physical_size, virtual_size;
     AsmNESectionTable.setFunctionPrototype ( "HEADER_NE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNESectionTable.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("unsigned","sector","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("SgAsmExecutableFileFormat::addr_t","physical_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTable.setDataPrototype("SgAsmExecutableFileFormat::addr_t","virtual_size","= 0",
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
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
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
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // std::vector<NERelocEntry> entries;
     AsmNERelocTable.setFunctionPrototype ( "HEADER_NE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNERelocTable.setDataPrototype("SgAsmNERelocEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // unsigned e_res1[14];
  // addr_t e_lfanew;
     AsmNEExtendedDOSHeader.setFunctionPrototype ( "HEADER_NE_EXTENDED_DOS_HEADER", "../Grammar/BinaryInstruction.code");
     AsmNEExtendedDOSHeader.setDataPrototype("SgUnsignedList","e_res1","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNEExtendedDOSHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_lfanew","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // NEEntryFlags flags;         /* bit flags */
  // unsigned int3f;             /* always 0x3fxx */
  // unsigned section_idx;       /* zero indicates unused entry */
  // unsigned section_offset;    /* byte offset into section */
     AsmNEEntryPoint.setFunctionPrototype ( "HEADER_NE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
     AsmNEEntryPoint.setDataPrototype("SgAsmNEEntryPoint::NEEntryFlags","flags","= SgAsmNEEntryPoint::EF_RESERVED",
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
     AsmNERelocEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","src_offset","= 0",
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
     AsmNESectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","physical_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmNESectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","virtual_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // unsigned e_byte_order, e_word_order, e_format_level, e_cpu_type, e_os_type, e_module_version, e_flags;
  // unsigned e_eip_section, e_esp_section, e_last_page_size, e_page_offset_shift, e_fixup_sect_cksum, e_loader_sect_cksum;
  // unsigned e_secttab_nentries, e_rsrctab_nentries, e_fmtdirtab_nentries, e_import_modtab_nentries, e_preload_npages;
  // unsigned e_nonresnametab_size, e_nonresnametab_cksum, e_auto_ds_section, e_debug_info_size, e_num_instance_preload;
  // unsigned e_num_instance_demand, e_heap_size;
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

  // addr_t e_npages, e_eip, e_esp, e_page_size, e_fixup_sect_size, e_loader_sect_size, e_secttab_rfo, e_pagetab_rfo;
  // addr_t e_iterpages_offset, e_rsrctab_rfo, e_resnametab_rfo, e_entrytab_rfo, e_fmtdirtab_rfo, e_fixup_pagetab_rfo;
  // addr_t e_fixup_rectab_rfo, e_import_modtab_rfo, e_import_proctab_rfo, e_ppcksumtab_rfo, e_data_pages_offset;
  // addr_t e_nonresnametab_offset, e_debug_info_rfo;
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_npages","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_eip","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_esp","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_page_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_fixup_sect_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_loader_sect_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_secttab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_pagetab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_iterpages_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_rsrctab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_resnametab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_entrytab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_fmtdirtab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_fixup_pagetab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_fixup_rectab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_import_modtab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_import_proctab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_ppcksumtab_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_data_pages_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_nonresnametab_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_debug_info_rfo","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // ExtendedDOSHeader *dos2_header;
  // LESectionTable *section_table;
  // LEPageTable *page_table;
  // LENameTable *resname_table;
  // LENameTable *nonresname_table;
  // LEEntryTable *entry_table;
  // LERelocTable *reloc_table;
     AsmLEFileHeader.setDataPrototype("SgAsmLEExtendedDOSHeader*","dos2_header","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLESectionTable*","section_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLEPageTable*","page_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLENameTable*","resname_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLENameTable*","nonresname_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLEEntryTable*","entry_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEFileHeader.setDataPrototype("SgAsmLERelocTable*","reloc_table","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // LESectionTableEntry *st_entry;
     AsmLESection.setFunctionPrototype ( "HEADER_LE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmLESection.setDataPrototype("SgAsmLESectionTableEntry*","st_entry","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // std::vector<size_t> bundle_sizes;
  // std::vector<LEEntryPoint> entries;
     AsmLEEntryTable.setFunctionPrototype ( "HEADER_LE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLEEntryTable.setDataPrototype("SgSizeTList","bundle_sizes","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryTable.setDataPrototype("SgAsmLEEntryPointPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // std::vector<LERelocEntry> entries;
     AsmLERelocTable.setFunctionPrototype ( "HEADER_LE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmLERelocTable.setDataPrototype("SgAsmLERelocEntryPtrList","entries","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

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
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned","flags","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned","objnum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned","entry_type","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("unsigned","res1","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLEEntryPoint.setDataPrototype("SgAsmExecutableFileFormat::addr_t","entry_offset","= 0",
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
     AsmLESectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","mapped_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmLESectionTableEntry.setDataPrototype("SgAsmExecutableFileFormat::addr_t","base_addr","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // unsigned            e_last_page_size, e_total_pages, e_nrelocs, e_header_paragraphs, e_minalloc, e_maxalloc;
  // unsigned            e_ss, e_sp, e_cksum, e_ip, e_cs, e_overlay;
  // addr_t              e_relocs_offset;
  // unsigned char       e_res1[4];
  // ExecSection         *relocs;        /* Relocation info */
  // ExecSection         *rm_section;    /* Real mode code segment */
     AsmDOSFileHeader.setFunctionPrototype ( "HEADER_DOS_HEADER", "../Grammar/BinaryInstruction.code");
  /* Section in which this segment lives */
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_last_page_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_total_pages","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_nrelocs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_header_paragraphs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_minalloc","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_maxalloc","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_ss","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_sp","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_cksum","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_ip","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_cs","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("unsigned char","e_overlay","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","e_relocs_offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("SgUnsignedCharList","e_res1","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*","relocs","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*","rm_section","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // This data structure represents the ExecSection from file: ExecGeneric.h
  // ExecFile            *file;                          /* The file to which this section belongs */
  // ExecHeader          *header;                        /* Optional header associated with section */
  // addr_t              size;                           /* Size of section in bytes */
  // addr_t              offset;                         /* Starting offset of the section */
  // const unsigned char *data;                          /* Content of just this section; points into file's content */
  // SectionPurpose      purpose;                        /* General contents of the section */
  // bool                synthesized;                    /* Section was created by the format reader; not specified in file */
  // int                 id;                             /* Non-unique section ID (unique for ELF) or negative */
  // std::string         name;                           /* Optional, non-unique name of section */
  // bool                mapped;                         /* True if section should be mapped to program's address space */
  // addr_t              mapped_rva;                     /* Intended relative virtual address if `mapped' is true */
  // addr_t              mapped_size;                    /* Intended virtual size if 'mapped' is true */
  // bool                rperm;                          /* Mapped by loader into memory having read permission */
  // bool                wperm;                          /* Mapped by loader into memory having write permission */
  // bool                eperm;                          /* Mapped by loader into memory having execute permission */
  // RefMap              referenced;                     /* Begin/end offsets for areas referenced by extent() and extent_str() */
  // bool                congealed;                      /* Is "holes" up to date w.r.t. referenced? */
  // ExtentVector        holes;                          /* Unreferenced area (bigin/end offsets) */
     AsmGenericSection.setFunctionPrototype ( "HEADER_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");

  // Later we will want to turn this back on so that this IR node is consistant with the others (if appropriate).
  // AsmGenericSection.setAutomaticGenerationOfConstructor(false);
     AsmGenericSection.setAutomaticGenerationOfDestructor(false);

  /* The file to which this section belongs */
#if 0
     AsmGenericSection.setDataPrototype("SgAsmGenericFile*","file","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

  // DQ (8/15/2008): Put this back since the sections are in a list and the list is not in the header
  // (as I thought).  The list is in the SgAsmGenericFile which has a SgAsmFile as a parent.
  // DQ (8/14/2008): The get_header() function is now implemented in terms of the "get_parent()" function 
  // so that we can remove redundant representation of pointers to IR nodes. parent pointers are
  // a standard part of the design of the ROSE IR and this detail was not accounted for in the 
  // design of the executable format support.
  // AsmGenericSection.setDataPrototype("SgAsmGenericHeader*","header","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("SgAsmGenericHeader*","header","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  /* Size of section in bytes */
     AsmGenericSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Starting offset of the section */
     AsmGenericSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","offset","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Content of just this section; points into file's content */
  // AsmGenericSection.setDataPrototype("const unsigned char","data","= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // AsmGenericSection.setDataPrototype("unsigned char","data","= 0",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (12/8/2008): I can't make ROSETTA accept "unsigned char*"
     AsmGenericSection.setDataPrototype("char*","data","= NULL",
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
  /* Optional, non-unique name of section */
     AsmGenericSection.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* True if section should be mapped to program's address space */
     AsmGenericSection.setDataPrototype("bool","mapped","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Intended relative virtual address if `mapped' is true */
     AsmGenericSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","mapped_rva","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("SgAsmExecutableFileFormat::addr_t","mapped_size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool","rperm","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool","wperm","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool","eperm","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("SgAsmGenericSection::RefMap","referenced","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("bool","congealed","= false",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSection.setDataPrototype("SgAsmGenericSection::ExtentVector","holes","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // DQ (8/2/2008): This was removed from the design by Robb.
  /* All segments belonging within this section */
  // AsmGenericSection.setDataPrototype("SgAsmGenericSegmentPtrListPtr","segmentsList","= NULL",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // Need a separate IR node to hold the list of SgAsmGenericSection pointers.
     AsmGenericSectionList.setDataPrototype("SgAsmGenericSectionPtrList","sections","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericHeaderList.setDataPrototype("SgAsmGenericHeaderPtrList","headers","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmGenericDLLList.setFunctionPrototype ( "HEADER_GENERIC_DLL_LIST", "../Grammar/BinaryInstruction.code");
     AsmGenericDLLList.setDataPrototype("SgAsmGenericDLLPtrList","dlls","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

     AsmGenericSymbolList.setFunctionPrototype ( "HEADER_GENERIC_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmGenericSymbolList.setDataPrototype("SgAsmGenericSymbolPtrList","symbols","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // This data structure represents the ExecHeader from file: ExecGeneric.h
  // ExecFormat          exec_format;                    /* General info about the executable format */
  // std::vector<unsigned char> magic;                   /* Optional magic number in file byte order */
  // Architecture        target                         /* Machine for which this header and its sections, etc. was compiled */
  // addr_t              base_va;                        /* Base virtual address used by all "relative virtual addresses" (RVA) */
  // std::vector<addr_t> entry_rvas;                     /* Code entry points wrt base_va */
  // std::vector<ExecDLL*> dlls;                         /* List of dynamic libraries needed by this executable */
  // std::vector<ExecSymbol*> symbols;                   /* All symbols defined for this header */
     AsmGenericHeader.setFunctionPrototype ( "HEADER_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");

     AsmGenericHeader.setAutomaticGenerationOfDestructor(false);

  /* General info about the executable format */
     AsmGenericHeader.setDataPrototype("SgAsmGenericFormat*","exec_format","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
  /* Optional magic number in file byte order */
     AsmGenericHeader.setDataPrototype("SgCharList","magic","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Machine for which this header and its sections, etc. was compiled */
     AsmGenericHeader.setDataPrototype("SgAsmGenericArchitecture*","target","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  /* Base virtual address used by all "relative virtual addresses" (RVA) */
     AsmGenericHeader.setDataPrototype("SgAsmExecutableFileFormat::addr_t","base_va","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Code entry point wrt base_va */
     AsmGenericHeader.setDataPrototype("SgAddressList","entry_rvas","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericHeader.setDataPrototype("SgAsmGenericDLLList*","dlls","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmGenericHeader.setDataPrototype("SgAsmGenericSymbolList*","symbols","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);

  // DQ (8/16/2008): Added this here instead of in SgAsmGenericSection because it can be computed from the 
  // parent pointer in the SgAsmGenericSection where as it is reuired in the AsmGenericHeader IR node.
     AsmGenericHeader.setDataPrototype("SgAsmGenericFile*","file","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


  // This data structure represents the ExecFile from file: ExecGeneric.h
  // int                 fd;             // File descriptor opened for read-only (or negative)
  // struct stat64       sb;             // File attributes at time of file open (valid if fd>=0)
  // unsigned char       *data;          // Content of file mapped into memory   (or null on file error)
  // std::vector<ExecSection*> sections; // All known sections for this file
  // std::vector<ExecHeader*> headers;   // All format headers belonging to this file
     AsmGenericFile.setFunctionPrototype ( "HEADER_GENERIC_FILE", "../Grammar/BinaryInstruction.code");

  // Later we will want to turn this back on so that this IR node is consistant with the others (if appropriate).
  // AsmGenericFile.setAutomaticGenerationOfConstructor(false);
     AsmGenericFile.setAutomaticGenerationOfDestructor(false);

  /* File descriptor opened for read-only (or negative) */
     AsmGenericFile.setDataPrototype("int","fd","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // DQ (6/14/2008): This data member's type is not yet correctly implemented needs to reference stat or stat64.  
  // Need to discuss this with Robb.
  /* File attributes at time of file open (valid if fd>=0) */
     AsmGenericFile.setDataPrototype("SgAsmGenericFormat::fileDetails","sb","",
                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Content of file mapped into memory   (or null on file error) */
  // AsmGenericFile.setDataPrototype("SgCharList","data","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // AsmGenericFile.setDataPrototype("unsigned char*","data","",
  //                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("char*","data","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* All known sections for this file */
  // AsmGenericFile.setDataPrototype("SgAsmGenericSectionPtrList","sections","",
  //                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // AsmGenericFile.setDataPrototype("SgAsmGenericHeaderPtrList","headers","",
  //                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("SgAsmGenericSectionList*","sections","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericFile.setDataPrototype("SgAsmGenericHeaderList*","headers","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);


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
     AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecPurpose","purpose","= SgAsmGenericFormat::PURPOSE_UNSPECIFIED",
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

  // This data structure represents the ExecFile from file: ExecGeneric.h
     AsmGenericArchitecture.setFunctionPrototype ( "HEADER_GENERIC_ARCHITECTURE", "../Grammar/BinaryInstruction.code");
  /* Instruction set architecture */
     AsmGenericArchitecture.setDataPrototype("SgAsmGenericFormat::InsSetArchitecture","isa","= SgAsmGenericFormat::ISA_UNSPECIFIED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  /* Actual stored value if isa==ISA_OTHER */
     AsmGenericArchitecture.setDataPrototype("unsigned long","other","= 0",
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
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolDefState","def_state","= SgAsmGenericSymbol::SYM_UNDEFINED",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolBinding","binding","= SgAsmGenericSymbol::SYM_NO_BINDING",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolType","type","= SgAsmGenericSymbol::SYM_NO_TYPE",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmExecutableFileFormat::addr_t","value","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmExecutableFileFormat::addr_t","size","= 0",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("SgAsmGenericSection*","bound","= NULL",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmGenericSymbol.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



  // std::string         name;                           /* Name of library as stored in executable (usually a base name) */
  // std::vector<std::string> funcs;                     /* List of functions needed from the library */
     AsmGenericDLL.setFunctionPrototype ( "HEADER_GENERIC_DLL", "../Grammar/BinaryInstruction.code");

  // AsmGenericDLL.setAutomaticGenerationOfConstructor(false);
  // AsmGenericDLL.setAutomaticGenerationOfDestructor(false);

     AsmGenericDLL.setDataPrototype  ("std::string","name","= \"\"",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     AsmGenericDLL.setDataPrototype  ("SgStringList","funcs","",NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);






// *****************************************************
//           END OF BINARY FILE FORMAT IR NODES
// *****************************************************







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
  // AsmFunctionDeclaration.setDataPrototype("SgAsmBlockPtrList","body","",
  //                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("std::string","name_md5","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmStatementPtrList","statementList","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
     AsmFunctionDeclaration.setDataPrototype("SgAsmStatementPtrList","dest","",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);

  // These are used as data members in AsmDataStructureDeclaration
     AsmFieldDeclaration.setDataPrototype("std::string","name","= \"\"",
                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
  // Not clear if we want to store the offset explicitly
     AsmFieldDeclaration.setDataPrototype("unsigned long","offset","= 0",
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

     Asmx86RegisterReferenceExpression.setDataPrototype("X86RegisterClass","register_class","= x86_regclass_unknown",
                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
     Asmx86RegisterReferenceExpression.setDataPrototype("int","register_number","= 0",
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

  // Binary File Format
     AsmExecutableFileFormat.setFunctionSource ( "SOURCE_EXECUTABLE_FILE_FORMAT", "../Grammar/BinaryInstruction.code");

     AsmGenericHeader.setFunctionSource ( "SOURCE_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");
     AsmGenericSection.setFunctionSource ( "SOURCE_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmGenericFormat.setFunctionSource ( "SOURCE_GENERIC_FORMAT", "../Grammar/BinaryInstruction.code");
     AsmGenericArchitecture.setFunctionSource ( "SOURCE_GENERIC_ARCHITECTURE", "../Grammar/BinaryInstruction.code");
     AsmGenericSymbol.setFunctionSource ( "SOURCE_GENERIC_SYMBOL", "../Grammar/BinaryInstruction.code");
  // AsmGenericSectionList.setFunctionSource ( "SOURCE_GENERIC_SECTION_LIST", "../Grammar/BinaryInstruction.code");
     AsmGenericSymbolList.setFunctionSource ( "SOURCE_GENERIC_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmGenericFile.setFunctionSource ( "SOURCE_GENERIC_FILE", "../Grammar/BinaryInstruction.code");
     AsmGenericDLL.setFunctionSource ( "SOURCE_GENERIC_DLL", "../Grammar/BinaryInstruction.code");
     AsmGenericDLLList.setFunctionSource ( "SOURCE_GENERIC_DLL_LIST", "../Grammar/BinaryInstruction.code");

     AsmElfFileHeader.setFunctionSource ( "SOURCE_ELF_HEADER", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTable.setFunctionSource ( "SOURCE_ELF_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfSectionTableEntry.setFunctionSource ( "SOURCE_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSection.setFunctionSource ( "SOURCE_ELF_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicSection.setFunctionSource ( "SOURCE_ELF_DYNAMIC_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfDynamicEntry.setFunctionSource ( "SOURCE_ELF_DYNAMIC_ENTRY", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTable.setFunctionSource ( "SOURCE_ELF_SEGMENT_TABLE", "../Grammar/BinaryInstruction.code");
     AsmElfSegmentTableEntry.setFunctionSource ( "SOURCE_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
  // AsmElfSegmentEntry.setFunctionSource ( "SOURCE_ELF_SEGMENT_ENTRY", "../Grammar/BinaryInstruction.code");
  // AsmElfSegmentEntryList.setFunctionSource ( "SOURCE_ELF_SEGMENT_ENTRY_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolSection.setFunctionSource ( "SOURCE_ELF_SYMBOL_SECTION", "../Grammar/BinaryInstruction.code");
     AsmElfSymbolList.setFunctionSource ( "SOURCE_ELF_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmElfSymbol.setFunctionSource ( "SOURCE_ELF_SYMBOL", "../Grammar/BinaryInstruction.code");
     AsmPEDLL.setFunctionSource ( "SOURCE_PE_DLL", "../Grammar/BinaryInstruction.code");

     AsmPERVASizePair.setFunctionSource ( "SOURCE_PE_RVA_SIZE_PAIR", "../Grammar/BinaryInstruction.code");
     AsmPEFileHeader.setFunctionSource ( "SOURCE_PE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmPESection.setFunctionSource ( "SOURCE_PE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPESectionTable.setFunctionSource ( "SOURCE_PE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbolTable.setFunctionSource ( "SOURCE_PE_COFF_SYMBOL_TABLE", "../Grammar/BinaryInstruction.code");
     AsmPEImportHintName.setFunctionSource ( "SOURCE_PE_IMPORT_HINT_NAME", "../Grammar/BinaryInstruction.code");
     AsmPEImportDirectory.setFunctionSource ( "SOURCE_PE_IMPORT_DIRECTORY", "../Grammar/BinaryInstruction.code");
     AsmPEImportSection.setFunctionSource ( "SOURCE_PE_IMPORT_SECTION", "../Grammar/BinaryInstruction.code");
     AsmPEExtendedDOSHeader.setFunctionSource ( "SOURCE_PE_EXTENDED_DOS_HEADER", "../Grammar/BinaryInstruction.code");
     AsmPESectionTableEntry.setFunctionSource ( "SOURCE_PE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
  // AsmCoffSymbolList.setFunctionSource ( "SOURCE_PE_COFF_SYMBOL_LIST", "../Grammar/BinaryInstruction.code");
     AsmCoffSymbol.setFunctionSource ( "SOURCE_PE_COFF_SYMBOL", "../Grammar/BinaryInstruction.code");

     AsmNEFileHeader.setFunctionSource ( "SOURCE_NE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
     AsmNESection.setFunctionSource ( "SOURCE_NE_SECTION", "../Grammar/BinaryInstruction.code");
     AsmNESectionTable.setFunctionSource ( "SOURCE_NE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNENameTable.setFunctionSource ( "SOURCE_NE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEModuleTable.setFunctionSource ( "SOURCE_NE_MODULE_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEStringTable.setFunctionSource ( "SOURCE_NE_STRING_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEEntryTable.setFunctionSource ( "SOURCE_NE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNERelocTable.setFunctionSource ( "SOURCE_NE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
     AsmNEExtendedDOSHeader.setFunctionSource ( "SOURCE_NE_EXTENDED_DOS_HEADER", "../Grammar/BinaryInstruction.code");
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

     AsmFile.setFunctionSource                     ( "SOURCE_BINARY_FILE", "../Grammar/BinaryInstruction.code");
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


#if USE_OLD_BINARY_EXECUTABLE_IR_NODES
     AsmSectionHeaderList.setFunctionSource ( "SOURCE_BINARY_FILE_SECTION_HEADER_LIST", "../Grammar/BinaryInstruction.code");
     AsmProgramHeaderList.setFunctionSource ( "SOURCE_BINARY_FILE_PROGRAM_HEADER_LIST", "../Grammar/BinaryInstruction.code");
#endif

   }
