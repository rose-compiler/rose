add_library(roseBinaryFormats OBJECT
  ### Utilities ###
  DataConversion.C
  ExecGeneric.C
  ExtentMap.C
  Hexdump.C
  BinaryVxcoreParser.C

  ### BinaryComposite classes ###
  BinaryComposite.C
  JvmComposite.C

  ### Generic Base Classes ###
  GenericDynamicLinking.C
  GenericFile.C
  GenericFormat.C
  GenericHeader.C
  GenericSection.C
  GenericString.C

  ### Executable Linking Format (ELF) ###
  ElfDynamicLinking.C
  ElfErrorFrame.C
  ElfFileHeader.C
  ElfNote.C
  ElfRelocation.C
  ElfSection.C
  ElfSectionTable.C
  ElfSegmentTable.C
  ElfStringTable.C
  ElfSymbolTable.C
  ElfSymbolVersion.C

  ### Java Virtual Machine Format (JVM) ###
  JvmNode.C
  JvmAttribute.C
  JvmConstantPool.C
  JvmField.C
  JvmFileHeader.C
  JvmMethod.C

  ### Microsoft Windows PE Format */
  PeExport.C
  PeFileHeader.C
  PeImportDirectory.C
  PeImportItem.C
  PeImportSection.C
  PeRvaSizePair.C
  PeSection.C
  PeSectionTable.C
  PeStringTable.C
  PeSymbolTable.C

  ### CIL class implementations
  CilImpl.C
  CliHeader.C

  ### Other Formats (partial implementations and not so well tested) ###
  ExecDOS.C
  ExecNE.C
  ExecLE.C
  Dwarf.C)

add_dependencies(roseBinaryFormats rosetta_generated)

install(
  FILES  DataConversion.h IntelPinSupport.h
         WorkLists.h SgSharedVector.h StatSerializer.h BinaryVxcoreParser.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
