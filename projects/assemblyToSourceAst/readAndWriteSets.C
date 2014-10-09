#include "rose.h"
#include "x86InstructionSemantics.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <boost/array.hpp>

using namespace std;
using boost::array;

struct ReadAndWriteSet {
  set<X86GeneralPurposeRegister> readGprs;
  set<X86Flag> readFlags;
  bool readMemory;
  set<X86GeneralPurposeRegister> writtenGprs;
  set<X86Flag> writtenFlags;
  bool writtenMemory;
  ReadAndWriteSet(): readMemory(false), writtenMemory(false) {}
};

ostream& operator<<(ostream& o, const ReadAndWriteSet& rws) {
  o << "reads {";
  bool first = true;
  for (set<X86GeneralPurposeRegister>::const_iterator j = rws.readGprs.begin(); j != rws.readGprs.end(); ++j) {
    if (!first) o << ", ";
    first = false;
    o << gprToString(*j);
  }
  for (set<X86Flag>::const_iterator j = rws.readFlags.begin(); j != rws.readFlags.end(); ++j) {
    if (!first) o << ", ";
    first = false;
    o << flagToString(*j);
  }
  if (rws.readMemory) {
    if (!first) o << ", ";
    first = false;
    o << "memory";
  }
  o << "} writes {";
  first = true;
  for (set<X86GeneralPurposeRegister>::const_iterator j = rws.writtenGprs.begin(); j != rws.writtenGprs.end(); ++j) {
    if (!first) o << ", ";
    first = false;
    o << gprToString(*j);
  }
  for (set<X86Flag>::const_iterator j = rws.writtenFlags.begin(); j != rws.writtenFlags.end(); ++j) {
    if (!first) o << ", ";
    first = false;
    o << flagToString(*j);
  }
  if (rws.writtenMemory) {
    if (!first) o << ", ";
    first = false;
    o << "memory";
  }
  o << "}";
  return o;
}

template <size_t Len> struct W {};

// This is used as a Policy
struct ReadAndWriteSetPolicy {
  map<rose_addr_t, ReadAndWriteSet> readAndWriteSets;
  SgAsmInstruction* currentInstruction;
  rose_addr_t currentAddr;
  const RegisterDictionary *regdict;

  ReadAndWriteSetPolicy(): regdict(NULL) {}

  /** Returns the register dictionary. */
  const RegisterDictionary *get_register_dictionary() const {
      return regdict ? regdict : RegisterDictionary::dictionary_pentium4();
  }

  /** Sets the register dictionary. */
  void set_register_dictionary(const RegisterDictionary *regdict) {
      this->regdict = regdict;
  }

  /** Finds a register by name. */
  const RegisterDescriptor& findRegister(const std::string &regname, size_t nbits=0) {
      const RegisterDescriptor *reg = get_register_dictionary()->lookup(regname);
      if (!reg) {
          std::ostringstream ss;
          ss <<"Invalid register: \"" <<regname <<"\"";
          abort();
      }
      if (nbits>0 && reg->get_nbits()!=nbits) {
          std::ostringstream ss;
          ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
             <<reg->get_nbits() <<" " <<(1==reg->get_nbits()?"byte":"bytes");
          abort();
      }
      return *reg;
  }

  W<32> readGPR(X86GeneralPurposeRegister r) {
    readAndWriteSets[currentAddr].readGprs.insert(r);
    return W<32>();
  }

  void writeGPR(X86GeneralPurposeRegister r, const W<32>& value) {
    readAndWriteSets[currentAddr].writtenGprs.insert(r);
  }

  W<16> readSegreg(X86SegmentRegister r) {
    return W<16>();
  }

  void writeSegreg(X86SegmentRegister r, const W<16>& value) {
  }

  W<32> readIP() {
    return W<32>();
  }

  void writeIP(const W<32>& newIp) {
  }

  W<1> readFlag(X86Flag f) {
    readAndWriteSets[currentAddr].readFlags.insert(f);
    return W<1>();
  }

  void writeFlag(X86Flag f, const W<1>& value) {
    readAndWriteSets[currentAddr].writtenFlags.insert(f);
  }

  template <size_t Len>
  W<Len> number(uint64_t n) {
    return W<Len>();
  }

  template <size_t Len1, size_t Len2>
  W<Len1 + Len2> concat(const W<Len1>& a, const W<Len2>& b) {
    return W<Len1 + Len2>();
  }

  template <size_t From, size_t To, size_t Len>
  W<To - From> extract(const W<Len>& a) {
    return W<To - From>();
  }

  W<1> true_() {return W<1>();}
  W<1> false_() {return W<1>();}
  W<1> undefined_() {return W<1>();}

  template <size_t Len>
  W<Len> invert(const W<Len>& a) {
    return W<Len>();
  }

  template <size_t Len>
  W<Len> negate(const W<Len>& a) {
    return W<Len>();
  }

  template <size_t Len>
  W<Len> and_(const W<Len>& a, const W<Len>& b) {
    return W<Len>();
  }

  template <size_t Len>
  W<Len> or_(const W<Len>& a, const W<Len>& b) {
    return W<Len>();
  }

  template <size_t Len>
  W<Len> xor_(const W<Len>& a, const W<Len>& b) {
    return W<Len>();
  }

  template <size_t From, size_t To>
  W<To> signExtend(const W<From>& a) {
    return W<To>();
  }

  template <size_t From, size_t To>
  W<To> unsignedExtend(const W<From>& a) {
      return W<To>();
  }
    
  template <size_t Len>
  W<Len> ite(const W<1>& sel, const W<Len>& ifTrue, const W<Len>& ifFalse) {
    return W<Len>();
  }

  template <size_t Len>
  W<1> equalToZero(const W<Len>& a) {
    return W<1>();
  }

  template <size_t Len>
  W<Len> add(const W<Len>& a, const W<Len>& b) { // Simple case
    return W<Len>();
  }

  template <size_t Len>
  W<Len> addWithCarries(const W<Len>& a, const W<Len>& b, const W<1>& carryIn, W<Len>& carries) { // Full case
    return W<Len>();
  }

  template <size_t Len, size_t SCLen>
  W<Len> rotateLeft(const W<Len>& a, const W<SCLen>& cnt) {
    return W<Len>();
  }

  template <size_t Len, size_t SCLen>
  W<Len> rotateRight(const W<Len>& a, const W<SCLen>& cnt) {
    return W<Len>();
  }

  template <size_t Len, size_t SCLen>
  W<Len> shiftLeft(const W<Len>& a, const W<SCLen>& cnt) {
    return W<Len>();
  }

  template <size_t Len, size_t SCLen>
  W<Len> shiftRight(const W<Len>& a, const W<SCLen>& cnt) {
    return W<Len>();
  }

  template <size_t Len, size_t SCLen>
  W<Len> shiftRightArithmetic(const W<Len>& a, const W<SCLen>& cnt) {
    return W<Len>();
  }

  template <size_t Len1, size_t Len2>
  W<Len1 + Len2> signedMultiply(const W<Len1>& a, const W<Len2>& b) {
    return W<Len1 + Len2>();
  }

  template <size_t Len1, size_t Len2>
  W<Len1 + Len2> unsignedMultiply(const W<Len1>& a, const W<Len2>& b) {
    return W<Len1 + Len2>();
  }

  template <size_t Len1, size_t Len2>
  W<Len1> signedDivide(const W<Len1>& a, const W<Len2>& b) {
    return W<Len1>();
  }

  template <size_t Len1, size_t Len2>
  W<Len2> signedModulo(const W<Len1>& a, const W<Len2>& b) {
    return W<Len2>();
  }

  template <size_t Len1, size_t Len2>
  W<Len1> unsignedDivide(const W<Len1>& a, const W<Len2>& b) {
    return W<Len1>();
  }

  template <size_t Len1, size_t Len2>
  W<Len2> unsignedModulo(const W<Len1>& a, const W<Len2>& b) {
    return W<Len2>();
  }

  template <size_t Len>
  W<Len> leastSignificantSetBit(const W<Len>& in) {
    return W<Len>();
  }

  template <size_t Len>
  W<Len> mostSignificantSetBit(const W<Len>& in) {
    return W<Len>();
  }

  template <size_t Len> // In bits
  W<Len> readMemory(X86SegmentRegister segreg, const W<32>& addr, W<1> cond) {
    readAndWriteSets[currentAddr].readMemory = true;
    return W<Len>();
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const W<32>& addr, const W<Len>& data, W<1> cond) {
    readAndWriteSets[currentAddr].writtenMemory = true;
  }
  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const W<32>& addr, const W<Len>& data, const W<32> repeat, W<1> cond) {
      writeMemory(segreg, addr, data, cond);
  }

  W<32> filterIndirectJumpTarget(const W<32>& addr) {return addr;}
  W<32> filterCallTarget(const W<32>& addr) {return addr;}
  W<32> filterReturnTarget(const W<32>& addr) {return addr;}

  void hlt() {} // FIXME
  void interrupt(uint8_t num) {readAndWriteSets[currentAddr].readMemory |= false;}
  void sysenter() {readAndWriteSets[currentAddr].readMemory |= false;}
  void cpuid() {} // FIXME
  W<64> rdtsc() {return W<64>();} // FIXME

  void startBlock(uint64_t addr) {
  }

  void finishBlock(uint64_t addr) {
  }

  void startInstruction(SgAsmInstruction* insn) {
    currentInstruction = insn;
    currentAddr = insn->get_address();
  }

  void finishInstruction(SgAsmInstruction*) {}

  /** Reads from a named register. */
  template<size_t Len/*bits*/>
  W<Len> readRegister(const char *regname) {
      return readRegister<Len>(findRegister(regname, Len));
  }

  /** Writes to a named register. */
  template<size_t Len/*bits*/>
  void writeRegister(const char *regname, const W<Len> &value) {
      writeRegister<Len>(findRegister(regname, Len), value);
  }

  /** Generic register read. */
  template<size_t Len>
  W<Len> readRegister(const RegisterDescriptor &reg) {
      switch (Len) {
          case 1:
              // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this granularity.
              assert(reg.get_major()==x86_regclass_flags);
              assert(reg.get_nbits()==1);
              return unsignedExtend<1, Len>(readFlag((X86Flag)reg.get_offset()));

          case 8:
              // Only general-purpose registers can be accessed at a byte granularity, and we can access only the low-order
              // byte or the next higher byte.  For instance, "al" and "ah" registers.
              assert(reg.get_major()==x86_regclass_gpr);
              assert(reg.get_nbits()==8);
              switch (reg.get_offset()) {
                  case 0:
                      return extract<0, Len>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
                  case 8:
                      return extract<8, 8+Len>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
                  default:
                      assert(false);
                      abort();
              }

          case 16:
              assert(reg.get_nbits()==16);
              assert(reg.get_offset()==0);
              switch (reg.get_major()) {
                  case x86_regclass_segment:
                      return unsignedExtend<16, Len>(readSegreg((X86SegmentRegister)reg.get_minor()));
                  case x86_regclass_gpr:
                      return extract<0, Len>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
                  case x86_regclass_flags:
                      return unsignedExtend<16, Len>(concat(readFlag((X86Flag)0),
                                                     concat(readFlag((X86Flag)1),
                                                     concat(readFlag((X86Flag)2),
                                                     concat(readFlag((X86Flag)3),
                                                     concat(readFlag((X86Flag)4),
                                                     concat(readFlag((X86Flag)5),
                                                     concat(readFlag((X86Flag)6),
                                                     concat(readFlag((X86Flag)7),
                                                     concat(readFlag((X86Flag)8),
                                                     concat(readFlag((X86Flag)9),
                                                     concat(readFlag((X86Flag)10),
                                                     concat(readFlag((X86Flag)11),
                                                     concat(readFlag((X86Flag)12),
                                                     concat(readFlag((X86Flag)13),
                                                     concat(readFlag((X86Flag)14),
                                                            readFlag((X86Flag)15)))))))))))))))));
                  default:
                      assert(false);
                      abort();
              }

          case 32:
              assert(reg.get_offset()==0);
              switch (reg.get_major()) {
                  case x86_regclass_gpr:
                      return unsignedExtend<32, Len>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
                  case x86_regclass_ip:
                      return unsignedExtend<32, Len>(readIP());
                  case x86_regclass_segment:
                      return unsignedExtend<16, Len>(readSegreg((X86SegmentRegister)reg.get_minor()));
                  case x86_regclass_flags: {
                      return unsignedExtend<32, Len>(concat(readRegister<16>("flags"), // no-op sign extension
                                                     concat(readFlag((X86Flag)16),
                                                     concat(readFlag((X86Flag)17),
                                                     concat(readFlag((X86Flag)18),
                                                     concat(readFlag((X86Flag)19),
                                                     concat(readFlag((X86Flag)20),
                                                     concat(readFlag((X86Flag)21),
                                                     concat(readFlag((X86Flag)22),
                                                     concat(readFlag((X86Flag)23),
                                                     concat(readFlag((X86Flag)24),
                                                     concat(readFlag((X86Flag)25),
                                                     concat(readFlag((X86Flag)26),
                                                     concat(readFlag((X86Flag)27),
                                                     concat(readFlag((X86Flag)28),
                                                     concat(readFlag((X86Flag)29),
                                                     concat(readFlag((X86Flag)30),
                                                            readFlag((X86Flag)31))))))))))))))))));
                  }
                  default:
                      assert(false);
                      abort();
              }

          default:
              assert(false);
              abort();
      }
  }

  /** Generic register write. */
  template<size_t Len>
  void writeRegister(const RegisterDescriptor &reg, const W<Len> &value) {
      switch (Len) {
          case 1:
              assert(reg.get_major()==x86_regclass_flags);
              assert(reg.get_nbits()==1);
              writeFlag((X86Flag)reg.get_offset(), unsignedExtend<Len, 1>(value));
              break;

          case 8:
              // Only general purpose registers can be accessed at byte granularity, and only for offsets 0 and 8.
              assert(reg.get_major()==x86_regclass_gpr);
              assert(reg.get_nbits()==8);
              switch (reg.get_offset()) {
                  case 0:
                      writeGPR((X86GeneralPurposeRegister)reg.get_minor(),
                               concat(signExtend<Len, 8>(value),
                                      extract<8, 32>(readGPR((X86GeneralPurposeRegister)reg.get_minor()))));
                      break;
                  case 8:
                      writeGPR((X86GeneralPurposeRegister)reg.get_minor(),
                               concat(extract<0, 8>(readGPR((X86GeneralPurposeRegister)reg.get_minor())),
                                      concat(unsignedExtend<Len, 8>(value),
                                             extract<16, 32>(readGPR((X86GeneralPurposeRegister)reg.get_minor())))));
                      break;
                  default:
                      assert(false);
                      abort();
              }
              break;

          case 16:
              assert(reg.get_nbits()==16);
              assert(reg.get_offset()==0);
              switch (reg.get_major()) {
                  case x86_regclass_segment:
                      writeSegreg((X86SegmentRegister)reg.get_minor(), unsignedExtend<Len, 16>(value));
                      break;
                  case x86_regclass_gpr:
                      writeGPR((X86GeneralPurposeRegister)reg.get_minor(),
                               concat(unsignedExtend<Len, 16>(value),
                                      extract<16, 32>(readGPR((X86GeneralPurposeRegister)reg.get_minor()))));
                      break;
                  case x86_regclass_flags:
                      writeFlag((X86Flag)0,  extract<0,  1 >(value));
                      writeFlag((X86Flag)1,  extract<1,  2 >(value));
                      writeFlag((X86Flag)2,  extract<2,  3 >(value));
                      writeFlag((X86Flag)3,  extract<3,  4 >(value));
                      writeFlag((X86Flag)4,  extract<4,  5 >(value));
                      writeFlag((X86Flag)5,  extract<5,  6 >(value));
                      writeFlag((X86Flag)6,  extract<6,  7 >(value));
                      writeFlag((X86Flag)7,  extract<7,  8 >(value));
                      writeFlag((X86Flag)8,  extract<8,  9 >(value));
                      writeFlag((X86Flag)9,  extract<9,  10>(value));
                      writeFlag((X86Flag)10, extract<10, 11>(value));
                      writeFlag((X86Flag)11, extract<11, 12>(value));
                      writeFlag((X86Flag)12, extract<12, 13>(value));
                      writeFlag((X86Flag)13, extract<13, 14>(value));
                      writeFlag((X86Flag)14, extract<14, 15>(value));
                      writeFlag((X86Flag)15, extract<15, 16>(value));
                      break;
                  default:
                      assert(false);
                      abort();
              }
              break;

          case 32:
              assert(reg.get_offset()==0);
              switch (reg.get_major()) {
                  case x86_regclass_gpr:
                      writeGPR((X86GeneralPurposeRegister)reg.get_minor(), signExtend<Len, 32>(value));
                      break;
                  case x86_regclass_ip:
                      writeIP(unsignedExtend<Len, 32>(value));
                      break;
                  case x86_regclass_flags:
                      assert(reg.get_nbits()==32);
                      writeRegister<16>("flags", unsignedExtend<Len, 16>(value));
                      writeFlag((X86Flag)16, extract<16, 17>(value));
                      writeFlag((X86Flag)17, extract<17, 18>(value));
                      writeFlag((X86Flag)18, extract<18, 19>(value));
                      writeFlag((X86Flag)19, extract<19, 20>(value));
                      writeFlag((X86Flag)20, extract<20, 21>(value));
                      writeFlag((X86Flag)21, extract<21, 22>(value));
                      writeFlag((X86Flag)22, extract<22, 23>(value));
                      writeFlag((X86Flag)23, extract<23, 24>(value));
                      writeFlag((X86Flag)24, extract<24, 25>(value));
                      writeFlag((X86Flag)25, extract<25, 26>(value));
                      writeFlag((X86Flag)26, extract<26, 27>(value));
                      writeFlag((X86Flag)27, extract<27, 28>(value));
                      writeFlag((X86Flag)28, extract<28, 29>(value));
                      writeFlag((X86Flag)29, extract<29, 30>(value));
                      writeFlag((X86Flag)30, extract<30, 31>(value));
                      writeFlag((X86Flag)31, extract<31, 32>(value));
                      break;
                  default:
                      assert(false);
                      abort();
              }
              break;

          default:
              assert(false);
              abort();
      }
  }
};

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  ReadAndWriteSetPolicy policy;
  X86InstructionSemantics<ReadAndWriteSetPolicy, W> t(policy);
  vector<SgNode*> insns = NodeQuery::querySubTree(proj, V_SgAsmX86Instruction);
  for (size_t i = 0; i < insns.size(); ++i) {
    SgAsmX86Instruction* b = isSgAsmX86Instruction(insns[i]);
    ROSE_ASSERT (b);
    try {
        t.processInstruction(b);
    } catch (const X86InstructionSemantics<ReadAndWriteSetPolicy, W>::Exception &e) {
        fprintf(stderr, "%s: %s\n", e.mesg.c_str(), unparseInstructionWithAddress(e.insn).c_str());
    }

  }
  VirtualBinCFG::AuxiliaryInformation info(proj);
  for (map<rose_addr_t, ReadAndWriteSet>::const_iterator i = policy.readAndWriteSets.begin(); i != policy.readAndWriteSets.end(); ++i) {
    cout << '"' << unparseInstructionWithAddress(info.getInstructionAtAddress(i->first)) << '"' << ' ' << i->second << endl;
  }
  return 0;
}
