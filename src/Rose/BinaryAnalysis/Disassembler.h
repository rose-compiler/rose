#ifndef ROSE_BinaryAnalysis_Disassembler_H
#define ROSE_BinaryAnalysis_Disassembler_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch64.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/Disassembler/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/Disassembler/Mips.h>
#include <Rose/BinaryAnalysis/Disassembler/Null.h>
#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/Disassembler/X86.h>

namespace Rose {
namespace BinaryAnalysis {

/** Instruction decoders.
 *
 *  A "disassembler" in ROSE is any class capable of decoding a sequence of bytes to construct a single @ref SgAsmInstruction
 *  AST. All decoders derive from the @ref Disassembler::Base decoder whose most important member function is @ref
 *  Disassembler::Base::disassembleOne "disassembleOne", which decodes one instruction from a byte sequence specified by a @ref
 *  MemoryMap and a virtual address (VA) within the map.
 *
 *  A new instruction set architecture (ISA) can be added to ROSE without modifying the ROSE source code. One does this by
 *  deriving a new decoder class from the @ref Disassembler::Base class and registering an instance of the class with @ref
 *  Disassembler::registerSubclass. The new class has a @ref Disassembler::Base::canDisassemble "canDisassemble" predicate that
 *  examines a file header (such as an ELF Header or PE Header) to see if the decoder is appropriate. Decoders can
 *  also be looked up by their names. Once the correct decoder is found, its @ref Disassembler::Base::clone "clone" method is
 *  called to create a copy that's used for the actual decoding. */
namespace Disassembler {
} // namespace

} // namespace
} // namespace

#endif
#endif
