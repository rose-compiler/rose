add_library(roseBinaryFormats OBJECT
  ### Utilities ###
  ByteOrder.C
  DataConversion.C
  ExecGeneric.C
  ExtentMap.C
  Hexdump.C
  MemoryMap.C
  Rva.C
  SRecord.C
  BinaryVxcoreParser.C

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

  ### Other Formats (partial implementations and not so well tested) ###
  ExecDOS.C
  ExecNE.C
  ExecLE.C)

add_dependencies(roseBinaryFormats rosetta_generated)

install(
  FILES  DataConversion.h IntelPinSupport.h MemoryMap.h ByteOrder.h
         SRecord.h WorkLists.h SgSharedVector.h StatSerializer.h BinaryVxcoreParser.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
