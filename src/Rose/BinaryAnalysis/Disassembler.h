#ifndef ROSE_BinaryAnalysis_Disassembler_H
#define ROSE_BinaryAnalysis_Disassembler_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler/Aarch32.h>
#include <Rose/BinaryAnalysis/Disassembler/Aarch64.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/BasicTypes.h>
#include <Rose/BinaryAnalysis/Disassembler/Cil.h>
#include <Rose/BinaryAnalysis/Disassembler/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/Jvm.h>
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
 *  MemoryMap and a virtual address (VA) within the map. */
namespace Disassembler {
} // namespace

} // namespace
} // namespace

#endif
#endif
