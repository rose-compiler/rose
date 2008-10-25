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

struct ReadAndWriteSetPolicy {
  map<SgAsmInstruction*, ReadAndWriteSet> readAndWriteSets;
  SgAsmInstruction* currentInstruction;

  W<32> readGPR(X86GeneralPurposeRegister r) {
    readAndWriteSets[currentInstruction].readGprs.insert(r);
    return W<32>();
  }

  void writeGPR(X86GeneralPurposeRegister r, const W<32>& value) {
    readAndWriteSets[currentInstruction].writtenGprs.insert(r);
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
    readAndWriteSets[currentInstruction].readFlags.insert(f);
    return W<1>();
  }

  void writeFlag(X86Flag f, const W<1>& value) {
    readAndWriteSets[currentInstruction].writtenFlags.insert(f);
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
    readAndWriteSets[currentInstruction].readMemory = true;
    return W<Len>();
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const W<32>& addr, const W<Len>& data, W<1> cond) {
    readAndWriteSets[currentInstruction].writtenMemory = true;
  }

  W<32> filterIndirectJumpTarget(const W<32>& addr) {return addr;}
  W<32> filterCallTarget(const W<32>& addr) {return addr;}
  W<32> filterReturnTarget(const W<32>& addr) {return addr;}

  void hlt() {} // FIXME
  void interrupt(uint8_t num) {} // FIXME
  W<64> rdtsc() {return W<64>();} // FIXME

  void startBlock(uint64_t addr) {
  }

  void finishBlock(uint64_t addr) {
  }

  void startInstruction(SgAsmInstruction* insn) {
    currentInstruction = insn;
  }

  void finishInstruction(SgAsmInstruction*) {}

};

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  ReadAndWriteSetPolicy policy;
  X86InstructionSemantics<ReadAndWriteSetPolicy, W> t(policy);
  vector<SgNode*> blocks = NodeQuery::querySubTree(proj, V_SgAsmBlock);
  for (size_t i = 0; i < blocks.size(); ++i) {
    SgAsmBlock* b = isSgAsmBlock(blocks[i]);
    ROSE_ASSERT (b);
    t.processBlock(b);
  }
  for (map<SgAsmInstruction*, ReadAndWriteSet>::const_iterator i = policy.readAndWriteSets.begin(); i != policy.readAndWriteSets.end(); ++i) {
    cout << '"' << unparseInstruction(i->first) << '"' << ' ' << i->second << endl;
  }
  return 0;
}
