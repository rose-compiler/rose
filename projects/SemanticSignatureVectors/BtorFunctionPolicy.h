

struct BtorFunctionPolicy {
  BTRegisterInfo registerMap;
  BtorProblem problem;
  const RegisterDictionary *regdict;

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

  template<size_t Len>
  BtorWordType<Len> undefined() {
      return problem.build_var(Len);
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
  template <size_t From, size_t To>
  BtorWordType<To> unsignedExtend(const BtorWordType<From>& a) {
    return zeroExtendVar(a, To);
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
  void cpuid() {}
  BtorWordType<64> rdtsc();


  void startBlock(uint64_t addr);
  void finishBlock(uint64_t addr);
  void startInstruction(SgAsmX86Instruction* insn);
  void finishInstruction(SgAsmX86Instruction* insn);

  /** Reads from a named register. */
  template<size_t Len/*bits*/>
  BtorWordType<Len> readRegister(const char *regname) {
      return readRegister<Len>(findRegister(regname, Len));
  }

  /** Writes to a named register. */
  template<size_t Len/*bits*/>
  void writeRegister(const char *regname, const BtorWordType<Len> &value) {
      writeRegister<Len>(findRegister(regname, Len), value);
  }

  /** Generic register read. */
  template<size_t Len>
  BtorWordType<Len> readRegister(const RegisterDescriptor &reg) {
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
  void writeRegister(const RegisterDescriptor &reg, const BtorWordType<Len> &value) {
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

std::string btorTranslate(BtorFunctionPolicy& policy, const std::vector<SgAsmX86Instruction*>& instructions);

