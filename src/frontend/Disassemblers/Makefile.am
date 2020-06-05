include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# This will cause the dynamic library to be built (as libroseDisassemblers.so)
noinst_LTLIBRARIES = libroseDisassemblers.la

libroseDisassemblers_la_DEPENDENCIES =

libroseDisassemblers_la_SOURCES =												\
	SgAsmBlock.C SgAsmFunction.C												\
	SgAsmInstruction.C SgAsmArm64Instruction.C SgAsmPowerpcInstruction.C SgAsmX86Instruction.C SgAsmMipsInstruction.C	\
	SgAsmM68kInstruction.C SgAsmExecutableFileFormat.C x86InstructionProperties.C						\
	SgAsmInterpretation.C SgAsmIntegerValueExpression.C SgAsmFloatValueExpression.C SgAsmExpression.C SgAsmType.C		\
	Registers.C RegisterDescriptor.C											\
        Disassembler.C DisassemblerArm.C DisassemblerMips.C DisassemblerM68k.C DisassemblerPowerpc.C DisassemblerX86.C		\
	Assembler.C AssemblerX86.C AssemblerX86Init.C RegisterParts.C								\
	AssemblerX86Init1.C AssemblerX86Init2.C AssemblerX86Init3.C AssemblerX86Init4.C AssemblerX86Init5.C			\
	AssemblerX86Init6.C AssemblerX86Init7.C AssemblerX86Init8.C AssemblerX86Init9.C BinaryInstructionCache.C

pkginclude_HEADERS =													\
	Registers.h RegisterDescriptor.h BitPattern.h									\
	Disassembler.h DisassemblerArm.h DisassemblerMips.h DisassemblerM68k.h DisassemblerPowerpc.h DisassemblerX86.h	\
	Assembler.h AssemblerX86.h AssemblerX86Init.h									\
	InstructionEnumsX86.h InstructionEnumsMips.h InstructionEnumsM68k.h x86InstructionProperties.h			\
	InstructionEnumsArm64.h InstructionEnumsPowerpc.h RegisterParts.h BinaryInstructionCache.h

EXTRA_DIST = CMakeLists.txt dummyDisassembler.C
