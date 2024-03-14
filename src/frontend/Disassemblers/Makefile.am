include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la

libroseDisassemblers_la_DEPENDENCIES =

libroseDisassemblers_la_SOURCES = SgAsmBlock.C SgAsmFunction.C					\
	SgAsmInstruction.C SgAsmAarch32Instruction.C SgAsmPEExportEntry.C			\
	SgAsmAarch64Instruction.C SgAsmPowerpcInstruction.C SgAsmUserInstruction.C		\
	SgAsmX86Instruction.C SgAsmCilInstruction.C SgAsmJvmInstruction.C			\
	SgAsmMipsInstruction.C SgAsmM68kInstruction.C SgAsmNullInstruction.C			\
	SgAsmExecutableFileFormat.C x86InstructionProperties.C					\
	SgAsmInterpretation.C SgAsmIntegerValueExpression.C SgAsmControlFlagsExpression.C	\
	SgAsmFloatValueExpression.C SgAsmExpression.C SgAsmType.C				\
	SgAsmStackExpression.C SgAsmStaticData.C SgAsmValueExpression.C				\
	Assembler.C AssemblerX86.C AssemblerX86Init.C SgAsmOperandList.C			\
	AssemblerX86Init1.C AssemblerX86Init2.C AssemblerX86Init3.C				\
	AssemblerX86Init4.C AssemblerX86Init5.C AssemblerX86Init6.C				\
	AssemblerX86Init7.C AssemblerX86Init8.C AssemblerX86Init9.C SgAsmGenericHeader.C

pkginclude_HEADERS = BitPattern.h		\
	Assembler.h AssemblerX86.h		\
	AssemblerX86Init.h			\
	x86InstructionProperties.h

EXTRA_DIST = CMakeLists.txt dummyDisassembler.C
