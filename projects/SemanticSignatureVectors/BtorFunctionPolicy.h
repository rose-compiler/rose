

struct BtorFunctionPolicy {
  BTRegisterInfo registerMap;
  BtorProblem problem;

  void makeRegMap(BTRegisterInfo& rm, const std::string& prefix);


  BtorFunctionPolicy();

  BtorWordType<32> readGPR(X86GeneralPurposeRegister r);
  void writeGPR(X86GeneralPurposeRegister r, const BtorWordType<32>& value);
  BtorWordType<16> readSegreg(X86SegmentRegister sr);
  void writeSegreg(X86SegmentRegister sr, BtorWordType<16> val);
  BtorWordType<32> readIP();
  void writeIP(const BtorWordType<32>& newIp);
  BtorWordType<1> readFlag(X86Flag f);
  void writeFlag(X86Flag f, const BtorWordType<1>& value);

  template <size_t Len>
  BtorWordType<Len> number(uint64_t n) {
    return problem.build_constant(Len, (n & (IntegerOps::GenMask<uint64_t, Len>::value)));
  }

  Comp concat(const Comp& a, const Comp& b);

  template <size_t Len1, size_t Len2>
  BtorWordType<Len1 + Len2> concat(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    return problem.build_op_concat(b, a); // Our concat puts a on the LSB side of b, while BTOR does the opposite
  }

  template <size_t From, size_t To>
  BtorWordType<To - From> extract(const Comp& a) {
    BOOST_STATIC_ASSERT(From < To);
    return extractVar(a, From, To);
  }

  Comp extractVar(const Comp& a, size_t from, size_t to);

  BtorWordType<1> true_() {return ones(1);}
  BtorWordType<1> false_() {return zero(1);}
  BtorWordType<1> undefined_() {return problem.build_var(1);}

  template <size_t Len>
  BtorWordType<Len> invert(const BtorWordType<Len>& a) {
    return a.invert();
    // return problem.build_op_not(a);
  }

  Comp invert(const Comp& a) {
    return a.invert();
    // return problem.build_op_not(a);
  }

  template <size_t Len>
  BtorWordType<Len> negate(const BtorWordType<Len>& a) {
    return problem.build_op_neg(a);
  }

  template <size_t Len>
  BtorWordType<Len> and_(const BtorWordType<Len>& a, const BtorWordType<Len>& b) {
    return problem.build_op_and(a, b);
  }

  template <size_t Len>
  BtorWordType<Len> or_(const BtorWordType<Len>& a, const BtorWordType<Len>& b) {
    return problem.build_op_or(a, b);
  }

  template <size_t Len>
  BtorWordType<Len> xor_(const BtorWordType<Len>& a, const BtorWordType<Len>& b) {
    return problem.build_op_xor(a, b);
  }

  template <size_t From, size_t To>
  BtorWordType<To> signExtend(const BtorWordType<From>& a) {
    return signExtendVar(a, To);
  }

  Comp signExtendVar(const Comp& a, size_t to);
  Comp zeroExtendVar(const Comp& a, size_t to);

  template <size_t Len>
  BtorWordType<Len> ite(const BtorWordType<1>& sel, const BtorWordType<Len>& ifTrue, const BtorWordType<Len>& ifFalse) {
    return ite((Comp)sel, (Comp)ifTrue, (Comp)ifFalse);
  }

  Comp ite(const Comp& sel, const Comp& ifTrue, const Comp& ifFalse) {
    return problem.build_op_cond(sel, ifTrue, ifFalse);
  }

  template <size_t Len>
  BtorWordType<1> equalToZero(const BtorWordType<Len>& a) {
    return invert(problem.build_op_redor(a));
  }

  template <size_t Len>
  BtorWordType<Len> zero() {
    return zero(Len);
  }

  Comp zero(uint width) {
    assert (width != 0);
    return problem.build_op_zero(width);
  }

  template <size_t Len>
  BtorWordType<Len> ones() {
    return ones(Len);
  }

  Comp ones(uint width) {
    assert (width != 0);
    return problem.build_op_ones(width);
  }

  template <size_t Len>
  BtorWordType<Len> add(const BtorWordType<Len>& a, const BtorWordType<Len>& b) { // Simple case
    return problem.build_op_add(a, b);
  }

  template <size_t Len>
  BtorWordType<Len> addWithCarries(const BtorWordType<Len>& a, const BtorWordType<Len>& b, const BtorWordType<1>& carryIn, BtorWordType<Len>& carries) { // Full case
    Comp carriesInt;
    Comp sum = addWithCarries((Comp)a, (Comp)b, (Comp)carryIn, carriesInt);
    carries = carriesInt;
    return sum;
  }

  Comp addWithCarries(const Comp& a, const Comp& b, const Comp& carryIn, Comp& carries);

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> rotateLeft(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_rol(a, extractVar(cnt, 0, (size_t) ceil(log2(a.bitWidth()))));
  }

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> rotateRight(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_ror(a, extractVar(cnt, 0, (size_t) ceil(log2(a.bitWidth()))));
  }

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> shiftLeft(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_sll(a, extractVar(cnt, 0, (size_t) ceil(log2(a.bitWidth()))));
  }

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> shiftRight(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_srl(a, extractVar(cnt, 0, (size_t) ceil(log2(a.bitWidth()))));
  }

  template <size_t Len, size_t SCLen>
  BtorWordType<Len> shiftRightArithmetic(const BtorWordType<Len>& a, const BtorWordType<SCLen>& cnt) {
    return problem.build_op_sra(a, extractVar(cnt, 0, (size_t) ceil(log2(a.bitWidth()))));
  }

  // Expanding multiplies
  template <size_t Len1, size_t Len2>
  BtorWordType<Len1 + Len2> signedMultiply(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    uint len = a.bitWidth() + b.bitWidth();
    Comp aFull = signExtendVar(a, len);
    Comp bFull = signExtendVar(b, len);
    return problem.build_op_mul(aFull, bFull);
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len1 + Len2> unsignedMultiply(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    uint len = a.bitWidth() + b.bitWidth();
    Comp aFull = zeroExtendVar(a, len);
    Comp bFull = zeroExtendVar(b, len);
    return problem.build_op_mul(aFull, bFull);
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len1> signedDivide(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = signExtendVar(b, a.bitWidth());
    return problem.build_op_sdiv(a, bFull);
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len2> signedModulo(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = signExtendVar(b, a.bitWidth());
    return extractVar(problem.build_op_smod(a, bFull), 0, b.bitWidth());
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len1> unsignedDivide(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = zeroExtendVar(b, a.bitWidth());
    return problem.build_op_udiv(a, bFull);
  }

  template <size_t Len1, size_t Len2>
  BtorWordType<Len2> unsignedModulo(const BtorWordType<Len1>& a, const BtorWordType<Len2>& b) {
    assert (a.bitWidth() >= b.bitWidth());
    Comp bFull = zeroExtendVar(b, a.bitWidth());
    return extractVar(problem.build_op_urem(a, bFull), 0, b.bitWidth());
  }

  template <size_t Len>
  BtorWordType<Len> leastSignificantSetBit(const BtorWordType<Len>& in) {
    Comp result = number<Len>(0); // Not found
    for (size_t i = Len; i > 0; --i) { // Scan from MSB to LSB as later checks override earlier ones
      result = ite(extractVar(in, i - 1, i), number<Len>(i - 1), result);
    }
    return result;
  }

  template <size_t Len>
  BtorWordType<Len> mostSignificantSetBit(const BtorWordType<Len>& in) {
    Comp result = number<Len>(0); // Not found
    for (size_t i = 0; i < Len; ++i) { // Scan from LSB to MSB as later checks override earlier ones
      result = ite(extractVar(in, i, i + 1), number<Len>(i), result);
    }
    return result;
  }

  template <size_t Len> // In bits
  BtorWordType<Len> readMemory(X86SegmentRegister segreg, const BtorWordType<32>& addr, BtorWordType<1> cond) {
    BtorWordType<Len> result = zero(Len);
    for (size_t i = 0; i < Len / 8; ++i) {
      Comp thisByte = readMemoryByte(segreg, add(addr, number<32>(i)), cond);
      if (Len == 8) {
        result = thisByte;
      } else if (i == 0) {
        result = problem.build_op_or(result, concat(thisByte, zero(Len - 8)));
      } else if (i == Len / 8 - 1) {
        result = problem.build_op_or(result, concat(zero(Len - 8), thisByte));
      } else {
        result = problem.build_op_or(result, concat(concat(zero(i * 8), thisByte), zero(Len - ((i + 1) * 8))));
      }
    }
    return result;
  }

  Comp readMemoryByte(X86SegmentRegister segreg, const BtorWordType<32>& addr, BtorWordType<1> cond) {
    return problem.build_op_read(registerMap.memory, addr);
  }

  void writeMemoryByte(X86SegmentRegister segreg, const BtorWordType<32>& addr, const Comp& data, BtorWordType<1> cond) {
    assert (addr.bitWidth() == 32);
    assert (data.bitWidth() == 8);
    registerMap.memory = problem.build_op_acond(cond, problem.build_op_write(registerMap.memory, addr, data), registerMap.memory);
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const BtorWordType<32>& addr, const BtorWordType<Len>& data, BtorWordType<1> cond) {
      BOOST_STATIC_ASSERT (Len % 8 == 0);
      for (size_t i = 0; i < Len / 8; ++i) {
          writeMemoryByte(segreg, (i == 0 ? (Comp)addr : problem.build_op_add(addr, problem.build_constant(32, i))),
                          extractVar(data, i * 8, i * 8 + 8), cond);
      }
  }

  template <size_t Len>
  void writeMemory(X86SegmentRegister segreg, const BtorWordType<32>& addr, const BtorWordType<Len>& data,
                   BtorWordType<32> repeat, BtorWordType<1> cond) {
      writeMemory(segreg, addr, data, cond);
  }

  BtorWordType<32> filterIndirectJumpTarget(const BtorWordType<32>& addr) {return addr;}
  BtorWordType<32> filterCallTarget(const BtorWordType<32>& addr) {return addr;}
  BtorWordType<32> filterReturnTarget(const BtorWordType<32>& addr) {return addr;}

  void hlt();
  void interrupt(uint8_t num);
  void sysenter();
  BtorWordType<64> rdtsc();


  void startBlock(uint64_t addr);
  void finishBlock(uint64_t addr);
  void startInstruction(SgAsmx86Instruction* insn);
  void finishInstruction(SgAsmx86Instruction* insn);
};

std::string btorTranslate(BtorFunctionPolicy& policy, const std::vector<SgAsmx86Instruction*>& instructions);

