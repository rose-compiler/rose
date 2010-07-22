
#include "rose.h"
#include "simulate.h"

#ifdef ROSE_ENABLE_SIMULATOR /*protects this whole file*/

#include "x86InstructionSemantics.h"
#include "integerOps.h"
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

extern void simulate_signal_check(LinuxMachineState& ms, uint32_t sourceAddr);
extern void simulate_sigreturn(LinuxMachineState& ms);
extern LinuxMachineState* targetForSignals;

struct EmulationPolicy {
    template <size_t Len>
    struct wordType {typedef Value<Len> type;};

    LinuxMachineState ms;
    IncrementalDisassembler id;

    EmulationPolicy(): ms(), id(ms.memory) {
        /* Build a disassembler for 32-bit x86 (see tests/roseTests/binaryTests/disassembleBuffer.C for an example) */
        SgAsmGenericFile *file = new SgAsmGenericFile();
        SgAsmElfFileHeader *elf = new SgAsmElfFileHeader(file); /*creates an i386 header by default*/
        id.init_disassembler(elf);
    }

#if 0 /*not actually ever used*/
    void setupExecutableContents(SgAsmInterpretation* interp) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        ROSE_ASSERT(1==headers.size()); /*original code did not support dynamic linking; we still don't [RPM 2010-06] */
        SgAsmGenericHeader* header = headers[0];
        ROSE_ASSERT (header);
        SgAsmElfFileHeader* elfHeader = isSgAsmElfFileHeader(header);
        ROSE_ASSERT (elfHeader);
        SgAsmGenericFile* genFile = isSgAsmGenericFile(header->get_parent());
        ROSE_ASSERT(genFile);

        /* Find an appropriate disassembler */
        id.init_disassembler(header);

        /* Set up mappings for all ELF Segments of this ELF Header. An ELF Segment is represented by an SgAsmGenericSection
         * with a non-null segment table entry. */
        SgAsmGenericSectionPtrList mapped_sections = header->get_mapped_sections();
        for (size_t i=0; i<mapped_sections.size(); ++i) {
            SgAsmElfSection *segment = isSgAsmElfSection(mapped_sections[i]);
            SgAsmElfSegmentTableEntry *ent = segment ? segment->get_segment_entry() : NULL;
            if (ent && ent->get_type()==SgAsmElfSegmentTableEntry::PT_LOAD) {
                bool willAllowRead = ent->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM;
                bool willAllowWrite = ent->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM;
                bool willAllowExec = ent->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM;
                uint32_t vaddr = (uint32_t)ent->get_vaddr();
                uint64_t offset = ent->get_offset();
                uint32_t filesz = (uint32_t)ent->get_filesz();
                uint32_t memsz = (uint32_t)ent->get_memsz();
                fprintf(stderr, "Loading at 0x%"PRIx32" with file size 0x%"PRIx32" and memory size 0x%"PRIx32"\n",
                        vaddr, filesz, memsz);
                SgFileContentList contentList = genFile->content(offset, filesz);
                for (uint32_t j = 0; j < memsz + PAGE_SIZE; j += PAGE_SIZE) {
                    ms.memory.mapZeroPageIfNeeded(j + vaddr);
                }
                ms.memory.writeMultiple(&contentList[0], filesz, vaddr);
                for (uint32_t j = filesz; j < memsz; j += PAGE_SIZE) {
                    ms.memory.mapZeroPageIfNeeded(j + vaddr);
                }
                for (uint32_t j = 0; j < filesz; j += PAGE_SIZE) {
                    ms.memory.findPage(j + vaddr).allow_read = willAllowRead;
                    ms.memory.findPage(j + vaddr).allow_write = willAllowWrite;
                    ms.memory.findPage(j + vaddr).allow_execute = willAllowExec;
                }
            }
        }
    }
#endif

    void setupArgs(const vector<string>& args) {
        /* Set up an initial stack */
        vector<uint32_t> pointers;
        unsigned int sp = 0xC0000000U;

        for (size_t i = 0; i < (1 << 26); i += PAGE_SIZE) {
            ms.memory.mapZeroPageIfNeeded(sp - i - PAGE_SIZE);
        }

        pointers.push_back(args.size());

        for (unsigned int i = 0; i < args.size(); ++i) {
            sp -= args[i].size() + 1;
            ms.memory.writeString(args[i].c_str(), sp);
            pointers.push_back(sp);
        }
        pointers.push_back(0);

        for (unsigned int i = 0; ; ++i) {
            if (!environ[i]) break;
            sp -= strlen(environ[i]) + 1;
            ms.memory.writeString(environ[i], sp);
            pointers.push_back(sp);
        }
        pointers.push_back(0);

        pointers.push_back(0);
        pointers.push_back(0);

        sp &= ~3U;

        sp -= 4 * pointers.size();
        ms.memory.writeMultiple((uint8_t*)&pointers[0], 4 * pointers.size(), sp);

        ms.gprs[x86_gpr_sp] = sp;
    }

    template <size_t Len>
    Value<Len> number(uint64_t n) {
        return n;
    }

    template <size_t From, size_t To, size_t Len>
    Value<To - From> extract(Value<Len> a) {
        return a.val() >> From;
        /* Other bits will automatically be masked off */
    }     

    template <size_t Len1, size_t Len2>
    Value<Len1 + Len2> concat(Value<Len1> a, Value<Len2> b) {
        /* Concats a on LSB side of b */
        return a.val() | (b.val() << Len1);
    }

    Value<1> true_() {return 1;}
    Value<1> false_() {return 0;}
    Value<1> undefined_() {return 1;} /* used e.g., by AND to set value of AF */

    template <size_t Len>
    Value<Len> and_(Value<Len> a, Value<Len> b) {
        return a.val() & b.val();
    }

    template <size_t Len>
    Value<Len> or_(Value<Len> a, Value<Len> b) {
        return a.val() | b.val();
    }

    template <size_t Len>
    Value<Len> xor_(Value<Len> a, Value<Len> b) {
        return a.val() ^ b.val();
    }

    template <size_t Len>
    Value<Len> invert(Value<Len> a) {
        return ~a.val();
    }

    template <size_t Len>
    Value<Len> negate(Value<Len> a) {
        return -a.val();
    }

    template <size_t Len>
    Value<Len> ite(Value<1> sel, Value<Len> a, Value<Len> b) {
        return sel.val() ? a.val() : b.val();
    }

    template <size_t Len>
    Value<1> equalToZero(Value<Len> a) {
        return a.val() == 0;
    }

    template <size_t Len, size_t SCLen>
    Value<Len> generateMask(Value<SCLen> a) {
        if (a.val() >= Len) return ~0ULL;
        return shl1(a.val()) - 1ULL;
    }

    template <size_t Len>
    Value<Len> add(Value<Len> a, Value<Len> b) {
        return a.val() + b.val();
    }

    template <size_t Len>
    Value<Len> addWithCarries(Value<Len> a, Value<Len> b, Value<1> carryIn, Value<Len>& carries) {
        Value<Len + 1> e = a.val() + b.val() + carryIn.val();
        carries = (a.val() ^ b.val() ^ e.val()) >> 1;
        return e.val();
    }

    template <size_t Len, size_t SALen>
    Value<Len> shiftLeft(Value<Len> a, Value<SALen> sa) {
        return IntegerOps::shiftLeft<Len>(a.val(), sa.val());
    }

    template <size_t Len, size_t SALen>
    Value<Len> shiftRight(Value<Len> a, Value<SALen> sa) {
        return IntegerOps::shiftRightLogical<Len>(a.val(), sa.val());
    }

    template <size_t Len, size_t SALen>
    Value<Len> shiftRightArithmetic(Value<Len> a, Value<SALen> sa) {
        return IntegerOps::shiftRightArithmetic<Len>(a.val(), sa.val());
    }

    template <size_t Len1, size_t Len2>
    Value<Len1> rotateLeft(Value<Len1> a, Value<Len2> b) {
        return (a.val() << b.val()) | (a.val() >> (Len1 - b.val()));
    }

    template <size_t Len1, size_t Len2>
    Value<Len1> rotateRight(Value<Len1> a, Value<Len2> b) {
        return IntegerOps::rotateRight<Len1>(a.val(), b.val());
    }

    template <size_t Len1, size_t Len2>
    Value<Len1 + Len2> unsignedMultiply(Value<Len1> a, Value<Len2> b) {
        return a.val() * b.val();
    }

    template <size_t Len1, size_t Len2>
    Value<Len1 + Len2> signedMultiply(Value<Len1> a, Value<Len2> b) {
        return signExtend<Len1, 64>(a).val() * signExtend<Len2, 64>(b).val();
    }

    template <size_t Len1, size_t Len2>
    Value<Len1> unsignedDivide(Value<Len1> a, Value<Len2> b) {
        return a.val() / b.val();
    }

    template <size_t Len1, size_t Len2>
    Value<Len2> unsignedModulo(Value<Len1> a, Value<Len2> b) {
        return a.val() % b.val();
    }

    template <size_t Len1, size_t Len2>
    Value<Len1> signedDivide(Value<Len1> a, Value<Len2> b) {
        return signExtend<Len1, 64>(a).val() / signExtend<Len2, 64>(b).val();
    }

    template <size_t Len1, size_t Len2>
    Value<Len2> signedModulo(Value<Len1> a, Value<Len2> b) {
        return signExtend<Len1, 64>(a).val() % signExtend<Len2, 64>(b).val();
    }

    template <size_t From, size_t To>
    Value<To> signExtend(Value<From> a) {
        return IntegerOps::signExtend<From, To, uint64_t>(a.val());
    }

    template <size_t Len>
    Value<Len> leastSignificantSetBit(Value<Len> a) {
        for (size_t i=0; i<Len; ++i) {
            if (a.val() & ((uint64_t)1<<i))
                return i;
        }
        return 0;
    }

    template <size_t Len>
    Value<Len> mostSignificantSetBit(Value<Len> a) {
        for (size_t i=Len; i>0; --i) {
            if (a.val() & ((uint64_t)1<<(i-1)))
                return i-1;
        }
        return 0;
    }

    Value<1> readFlag(X86Flag flag) {
#ifdef DEBUG
        fprintf(stderr, "flag[%s] -> %u\n", flagToString(flag), ms.flags[flag]);
#endif
        return ms.flags[flag];
    }

    void writeFlag(X86Flag flag, Value<1> value) {
#ifdef DEBUG
        fprintf(stderr, "flag[%s] <- %"PRIu64"\n", flagToString(flag), value.val());
#endif
        ms.flags[flag] = value.val();
    }

    Value<32> readGPR(X86GeneralPurposeRegister num) {
#ifdef DEBUG
        fprintf(stderr, "gpr[e%s] -> %"PRIX32"\n", gprToString(num), ms.gprs[num]);
#endif
        return ms.gprs[num];
    }

    void writeGPR(X86GeneralPurposeRegister num, Value<32> val) {
#ifdef DEBUG
        fprintf(stderr, "gpr[e%s] <- %"PRIX64"\n", gprToString(num), val.val());
#endif
        ms.gprs[num] = val.val();
    }

    Value<16> readSegreg(X86SegmentRegister num) {
#ifdef DEBUG
        fprintf(stderr, "segreg[%s] -> %"PRIX16"\n", segregToString(num), ms.segregs[num]);
#endif
        return ms.segregs[num];
    }

    void writeSegreg(X86SegmentRegister num, Value<16> val) {
#ifdef DEBUG
        fprintf(stderr, "segreg[e%s] <- %"PRIX16"\n", segregToString(num), val.val());
#endif
        ms.writeSegreg(num, val.val()); /* Set up shadow registers too */
    }

    Value<32> readIP() {
        return ms.ip;
    }

    void writeIP(Value<32> val) {
#ifdef DEBUG
        fprintf(stderr, "ip = %"PRIX64"\n", val.val());
#endif
        ms.ip = val.val();
    }

    template <size_t Len>
    Value<Len> readMemory(X86SegmentRegister segreg, Value<32> address, Value<1> cond) {
        if (!cond.val()) return 0;
#ifdef DEBUG
        fprintf(stderr, "readMemory<%u>(%"PRIX64") -> %"PRIX64"\n", Len, address.val(), ms.readMemory<Len>(segreg, address.val()));
#endif
        return ms.readMemory<Len>(segreg, address.val());
    }

    template <size_t Len>
    void writeMemory(X86SegmentRegister segreg, Value<32> address, Value<Len> data, Value<1> cond) {
        if (!cond.val()) return;
#ifdef DEBUG
        fprintf(stderr, "writeMemory<%u>(%"PRIX64", %"PRIX64")\n", Len, address.val(), data.val());
#endif
        ms.writeMemory<Len>(segreg, address.val(), data.val());
    }

    void hlt() {
        fprintf(stderr, "hlt\n");
        abort();
    }

    Value<64> rdtsc() {
#ifdef DEBUG
        fprintf(stderr, "rdtsc\n");
#endif
        return Value<64>(0);
    }

    void interrupt(uint8_t num) {
#ifdef DEBUG
        fprintf(stderr, "int 0x%02X [%u]\n", (unsigned int)num, (unsigned int)ms.gprs[x86_gpr_ax]);
#endif
        if (num != 0x80) {
            fprintf(stderr, "Bad interrupt\n");
            abort();
        }
        linuxSyscall(ms);
    }

    Value<32> filterCallTarget(Value<32> a) {
        return a;
    }

    Value<32> filterReturnTarget(Value<32> a) {
        return a;
    }

    Value<32> filterIndirectJumpTarget(Value<32> a) {
        return a;
    }

    void startBlock(uint64_t addr) {
    }

    void finishBlock(uint64_t addr) {
    }

    void startInstruction(SgAsmInstruction* insn) {
        if (ms.signalQueue.anySignalsWaiting()) {
            simulate_signal_check(ms, insn->get_address());
        }
    }

    void finishInstruction(SgAsmInstruction* insn) {}
};

int main(int argc, char** argv) {
    typedef X86InstructionSemantics<EmulationPolicy, Value> Semantics;

    ROSE_ASSERT (argc >= 2);
    EmulationPolicy policy;
    targetForSignals = &policy.ms;
    setup(policy.ms, argc - 1, argv + 1);
    Semantics t(policy);
    size_t ninsns = 0;

    while (true) {
        if (policy.ms.ip == 0x00536967) { /* "\0Sig" in big-endian notation */
            simulate_sigreturn(policy.ms);
            continue;
        }
        try {
#if 0
            fprintf(stderr, "\033[K\n[%07zu] %s\033[K\r\033[1A",
                    ninsns++, unparseInstructionWithAddress(policy.id[policy.ms.ip]).c_str()),
#else
            fprintf(stderr, "[%07zu] %s\n",
                    ninsns++, unparseInstructionWithAddress(policy.id[policy.ms.ip]).c_str()),
#endif
            t.processInstruction(policy.id[policy.ms.ip]);
        } catch (const Semantics::Exception &e) {
            std::cerr <<e <<"\n\n";
            abort();
        }
    }
    return 0;
}

#else

int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 1;
}

#endif /*ROSE_ENABLE_SIMULATOR*/
