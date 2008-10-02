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

struct W {};

struct ReadAndWriteSetPolicy {
  template <size_t Len>
  struct wordType {typedef W type;};

  map<SgAsmInstruction*, ReadAndWriteSet> readAndWriteSets;
  SgAsmInstruction* currentInstruction;

  W readGPR(X86GeneralPurposeRegister r) {
    readAndWriteSets[currentInstruction].readGprs.insert(r);
    return W();
  }

  void writeGPR(X86GeneralPurposeRegister r, const W& value) {
    readAndWriteSets[currentInstruction].writtenGprs.insert(r);
  }

  W readSegreg(X86SegmentRegister r) {
    return W();
  }

  void writeSegreg(X86SegmentRegister r, const W& value) {
  }

  W readIP() {
    return W();
  }

  void writeIP(const W& newIp) {
  }

  W readFlag(X86Flag f) {
    readAndWriteSets[currentInstruction].readFlags.insert(f);
    return W();
  }

  void writeFlag(X86Flag f, const W& value) {
    readAndWriteSets[currentInstruction].writtenFlags.insert(f);
  }

  template <size_t Len>
  W number(uint64_t n) {
    return W();
  }

  W concat(const W& a, const W& b) {
    return W();
  }

  template <size_t From, size_t To>
  W extract(const W& a) {
    return W();
  }

  W false_() {return W();}
  W true_() {return W();}

  W invert(const W& a) {
    return W();
  }

  W and_(const W& a, const W& b) {
    return W();
  }

  W or_(const W& a, const W& b) {
    return W();
  }

  W xor_(const W& a, const W& b) {
    return W();
  }

  W parity(const W& a) {
    return W();
  }

  template <size_t From, size_t To>
  W signExtend(const W& a) {
    return W();
  }

  W ite(const W& sel, const W& ifTrue, const W& ifFalse) {
    return W();
  }

  W equalToZero(const W& a) {
    return W();
  }

  template <size_t Len>
  W generateMask(W) {
    return W();
  }

  W add(const W& a, const W& b) { // Simple case
    return W();
  }

  W addWithCarries(const W& a, const W& b, const W& carryIn, W& carries) { // Full case
    return W();
  }

  W rotateLeft(const W& a, const W& cnt) {
    return W();
  }

  W signedMultiply(const W& a, const W& b) {
    return W();
  }

  W unsignedMultiply(const W& a, const W& b) {
    return W();
  }

  W signedDivide(const W& a, const W& b) {
    return W();
  }

  W signedModulo(const W& a, const W& b) {
    return W();
  }

  W unsignedDivide(const W& a, const W& b) {
    return W();
  }

  W unsignedModulo(const W& a, const W& b) {
    return W();
  }

  W leastSignificantSetBit(const W& in) {
    return W();
  }

  W mostSignificantSetBit(const W& in) {
    return W();
  }

  template <size_t Len> // In bits
  W readMemory(X86SegmentRegister segreg, const W& addr, W cond) {
    readAndWriteSets[currentInstruction].readMemory = true;
    return W();
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const W& addr, const W& data, W cond) {
    readAndWriteSets[currentInstruction].writtenMemory = true;
  }

  void hlt() {} // FIXME
  void interrupt(uint8_t num) {} // FIXME
  W rdtsc() {return W();} // FIXME

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
  X86InstructionSemantics<ReadAndWriteSetPolicy> t(policy);
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
