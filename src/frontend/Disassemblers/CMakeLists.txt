########### install files ###############

install(FILES  	    Assembler.h AssemblerX86.h AssemblerX86Init.h
                    Disassembler.h BinaryDebugger.h
               	    DisassemblerArm.h DisassemblerM68k.h DisassemblerMips.h DisassemblerPowerpc.h DisassemblerX86.h
	       	    InstructionEnumsM68k.h InstructionEnumsMips.h InstructionEnumsX86.h
	       	    Partitioner.h Registers.h
	DESTINATION include)
