/* Generic code to be inserted into policy classes that are similar to PartialSymbolicSemantics in how they organized the
 * registers in their State classes.   This header should not be protected with #ifdef because we might need to include it more
 * than once, each time into a different class.
 *
 * Most policies define a type named "ValueType", but some call it something else.  It is safe to #define ValueType to some
 * other type name.
 *
 * The policy state is assumed to be in the Policy::cur_state data member, which has arrays "gpr", "segreg" and "flag" whose
 * size are indicated by State class constants "n_gprs", "n_segregs", and "n_flags".
 *
 * Most policies store the "EIP" register in the cur_state along with the other registers.   However, at least one policy
 * stores it in the Policy object rather than the state.  Therefore, EIP_LOCATION should be a CPP symbol defining the location,
 * and defaults to "cur_state.ip".
 *
 * Some of the signExtend() and unsignedExtend() calls are only executed in the no-op case and are present only for their static
 * type. */

#ifndef EIP_LOCATION
#define EIP_LOCATION cur_state.registers.ip
#endif

/** Finds a register by name. */
const RegisterDescriptor& findRegister(const std::string &regname, size_t nbits=0) {
    const RegisterDescriptor *reg = get_register_dictionary()->lookup(regname);
    if (!reg) {
        std::ostringstream ss;
        ss <<"Invalid register: \"" <<regname <<"\"";
        throw Exception(ss.str());
    }
    if (nbits>0 && reg->get_nbits()!=nbits) {
        std::ostringstream ss;
        ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
           <<reg->get_nbits() <<" " <<(1==reg->get_nbits()?"byte":"bytes");
        throw Exception(ss.str());
    }
    return *reg;
}

/** Reads from a named register. */
template<size_t Len/*bits*/>
ValueType<Len> readRegister(const char *regname) {
    return readRegister<Len>(findRegister(regname, Len));
}

/** Writes to a named register. */
template<size_t Len/*bits*/>
void writeRegister(const char *regname, const ValueType<Len> &value) {
    writeRegister<Len>(findRegister(regname, Len), value);
}

/** Generic register read. */
template<size_t Len>
ValueType<Len> readRegister(const RegisterDescriptor &reg) {
    switch (Len) {
        case 1:
            // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this granularity.
            if (reg.get_major()!=x86_regclass_flags)
                throw Exception("bit access only valid for FLAGS/EFLAGS register");
            if (reg.get_minor()!=0 || reg.get_offset()>=cur_state.registers.n_flags)
                throw Exception("register not implemented in semantic policy");
            if (reg.get_nbits()!=1)
                throw Exception("semantic policy supports only single-bit flags");
            return unsignedExtend<1, Len>(cur_state.registers.flag[reg.get_offset()]);

        case 8:
            // Only general-purpose registers can be accessed at a byte granularity, and we can access only the low-order
            // byte or the next higher byte.  For instance, "al" and "ah" registers.
            if (reg.get_major()!=x86_regclass_gpr)
                throw Exception("byte access only valid for general purpose registers");
            if (reg.get_minor()>=cur_state.registers.n_gprs)
                throw Exception("register not implemented in semantic policy");
            ASSERT_require(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
            switch (reg.get_offset()) {
                case 0:
                    return extract<0, Len>(cur_state.registers.gpr[reg.get_minor()]);
                case 8:
                    return extract<8, 8+Len>(cur_state.registers.gpr[reg.get_minor()]);
                default:
                    throw Exception("invalid one-byte access offset");
            }

        case 16:
            if (reg.get_nbits()!=16)
                throw Exception("invalid 2-byte register");
            if (reg.get_offset()!=0)
                throw Exception("policy does not support non-zero offsets for word granularity register access");
            switch (reg.get_major()) {
                case x86_regclass_segment:
                    if (reg.get_minor()>=cur_state.registers.n_segregs)
                        throw Exception("register not implemented in semantic policy");
                    return unsignedExtend<16, Len>(cur_state.registers.segreg[reg.get_minor()]);
                case x86_regclass_gpr:
                    if (reg.get_minor()>=cur_state.registers.n_gprs)
                        throw Exception("register not implemented in semantic policy");
                    return extract<0, Len>(cur_state.registers.gpr[reg.get_minor()]);
                case x86_regclass_flags:
                    if (reg.get_minor()!=0 || cur_state.registers.n_flags<16)
                        throw Exception("register not implemented in semantic policy");
                    return unsignedExtend<16, Len>(concat<1, 15>(cur_state.registers.flag[0],
                                                   concat<1, 14>(cur_state.registers.flag[1],
                                                   concat<1, 13>(cur_state.registers.flag[2],
                                                   concat<1, 12>(cur_state.registers.flag[3],
                                                   concat<1, 11>(cur_state.registers.flag[4],
                                                   concat<1, 10>(cur_state.registers.flag[5],
                                                   concat<1, 9>(cur_state.registers.flag[6],
                                                   concat<1, 8>(cur_state.registers.flag[7],
                                                   concat<1, 7>(cur_state.registers.flag[8],
                                                   concat<1, 6>(cur_state.registers.flag[9],
                                                   concat<1, 5>(cur_state.registers.flag[10],
                                                   concat<1, 4>(cur_state.registers.flag[11],
                                                   concat<1, 3>(cur_state.registers.flag[12],
                                                   concat<1, 2>(cur_state.registers.flag[13],
                                                   concat<1, 1>(cur_state.registers.flag[14],
                                                          cur_state.registers.flag[15]))))))))))))))));
                default:
                    throw Exception("word access not valid for this register type");
            }

        case 32:
            if (reg.get_offset()!=0)
                throw Exception("policy does not support non-zero offsets for double word granularity register access");
            switch (reg.get_major()) {
                case x86_regclass_gpr:
                    if (reg.get_minor()>=cur_state.registers.n_gprs)
                        throw Exception("register not implemented in semantic policy");
                    return unsignedExtend<32, Len>(cur_state.registers.gpr[reg.get_minor()]);
                case x86_regclass_ip:
                    if (reg.get_minor()!=0)
                        throw Exception("register not implemented in semantic policy");
                    return unsignedExtend<32, Len>(EIP_LOCATION);
                case x86_regclass_segment:
                    if (reg.get_minor()>=cur_state.registers.n_segregs || reg.get_nbits()!=16)
                        throw Exception("register not implemented in semantic policy");
                    return unsignedExtend<16, Len>(cur_state.registers.segreg[reg.get_minor()]);
                case x86_regclass_flags: {
                    if (reg.get_minor()!=0 || cur_state.registers.n_flags<32)
                        throw Exception("register not implemented in semantic policy");
                    if (reg.get_nbits()!=32)
                        throw Exception("register is not 32 bits");
                    return unsignedExtend<32, Len>(concat(readRegister<16>("flags"), // no-op sign extension
                                                   concat(cur_state.registers.flag[16],
                                                   concat(cur_state.registers.flag[17],
                                                   concat(cur_state.registers.flag[18],
                                                   concat(cur_state.registers.flag[19],
                                                   concat(cur_state.registers.flag[20],
                                                   concat(cur_state.registers.flag[21],
                                                   concat(cur_state.registers.flag[22],
                                                   concat(cur_state.registers.flag[23],
                                                   concat(cur_state.registers.flag[24],
                                                   concat(cur_state.registers.flag[25],
                                                   concat(cur_state.registers.flag[26],
                                                   concat(cur_state.registers.flag[27],
                                                   concat(cur_state.registers.flag[28],
                                                   concat(cur_state.registers.flag[29],
                                                   concat(cur_state.registers.flag[30],
                                                          cur_state.registers.flag[31])))))))))))))))));
                }
                default:
                    throw Exception("double word access not valid for this register type");
            }

        default:
            throw Exception("invalid register access width");
    }
}

/** Generic register write. */
template<size_t Len>
void writeRegister(const RegisterDescriptor &reg, const ValueType<Len> &value) {
    switch (Len) {
        case 1:
            // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this granularity.
            if (reg.get_major()!=x86_regclass_flags)
                throw Exception("bit access only valid for FLAGS/EFLAGS register");
            if (reg.get_minor()!=0 || reg.get_offset()>=cur_state.registers.n_flags)
                throw Exception("register not implemented in semantic policy");
            if (reg.get_nbits()!=1)
                throw Exception("semantic policy supports only single-bit flags");
            cur_state.registers.flag[reg.get_offset()] = unsignedExtend<Len, 1>(value);
            break;

        case 8:
            // Only general purpose registers can be accessed at byte granularity, and only for offsets 0 and 8.
            if (reg.get_major()!=x86_regclass_gpr)
                throw Exception("byte access only valid for general purpose registers.");
            if (reg.get_minor()>=cur_state.registers.n_gprs)
                throw Exception("register not implemented in semantic policy");
            ASSERT_require(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
            switch (reg.get_offset()) {
                case 0:
                    cur_state.registers.gpr[reg.get_minor()] =
                        concat(signExtend<Len, 8>(value), extract<8, 32>(cur_state.registers.gpr[reg.get_minor()])); // no-op extend
                    break;
                case 8:
                    cur_state.registers.gpr[reg.get_minor()] =
                        concat(extract<0, 8>(cur_state.registers.gpr[reg.get_minor()]),
                               concat(unsignedExtend<Len, 8>(value),
                                      extract<16, 32>(cur_state.registers.gpr[reg.get_minor()])));
                    break;
                default:
                    throw Exception("invalid byte access offset");
            }
            break;

        case 16:
            if (reg.get_nbits()!=16)
                throw Exception("invalid 2-byte register");
            if (reg.get_offset()!=0)
                throw Exception("policy does not support non-zero offsets for word granularity register access");
            switch (reg.get_major()) {
                case x86_regclass_segment:
                    if (reg.get_minor()>=cur_state.registers.n_segregs)
                        throw Exception("register not implemented in semantic policy");
                    cur_state.registers.segreg[reg.get_minor()] = unsignedExtend<Len, 16>(value);
                    break;
                case x86_regclass_gpr:
                    if (reg.get_minor()>=cur_state.registers.n_gprs)
                        throw Exception("register not implemented in semantic policy");
                    cur_state.registers.gpr[reg.get_minor()] =
                        concat(unsignedExtend<Len, 16>(value),
                               extract<16, 32>(cur_state.registers.gpr[reg.get_minor()]));
                    break;
                case x86_regclass_flags:
                    if (reg.get_minor()!=0 || cur_state.registers.n_flags<16)
                        throw Exception("register not implemented in semantic policy");
                    cur_state.registers.flag[0]  = extract<0,  1 >(value);
                    cur_state.registers.flag[1]  = extract<1,  2 >(value);
                    cur_state.registers.flag[2]  = extract<2,  3 >(value);
                    cur_state.registers.flag[3]  = extract<3,  4 >(value);
                    cur_state.registers.flag[4]  = extract<4,  5 >(value);
                    cur_state.registers.flag[5]  = extract<5,  6 >(value);
                    cur_state.registers.flag[6]  = extract<6,  7 >(value);
                    cur_state.registers.flag[7]  = extract<7,  8 >(value);
                    cur_state.registers.flag[8]  = extract<8,  9 >(value);
                    cur_state.registers.flag[9]  = extract<9,  10>(value);
                    cur_state.registers.flag[10] = extract<10, 11>(value);
                    cur_state.registers.flag[11] = extract<11, 12>(value);
                    cur_state.registers.flag[12] = extract<12, 13>(value);
                    cur_state.registers.flag[13] = extract<13, 14>(value);
                    cur_state.registers.flag[14] = extract<14, 15>(value);
                    cur_state.registers.flag[15] = extract<15, 16>(value);
                    break;
                default:
                    throw Exception("word access not valid for this register type");
            }
            break;

        case 32:
            if (reg.get_offset()!=0)
                throw Exception("policy does not support non-zero offsets for double word granularity register access");
            switch (reg.get_major()) {
                case x86_regclass_gpr:
                    if (reg.get_minor()>=cur_state.registers.n_gprs)
                        throw Exception("register not implemented in semantic policy");
                    cur_state.registers.gpr[reg.get_minor()] = signExtend<Len, 32>(value);
                    break;
                case x86_regclass_ip:
                    if (reg.get_minor()!=0)
                        throw Exception("register not implemented in semantic policy");
                    EIP_LOCATION = unsignedExtend<Len, 32>(value);
                    break;
                case x86_regclass_flags:
                    if (reg.get_minor()!=0 || cur_state.registers.n_flags<32)
                        throw Exception("register not implemented in semantic policy");
                    if (reg.get_nbits()!=32)
                        throw Exception("register is not 32 bits");
                    writeRegister<16>("flags", unsignedExtend<Len, 16>(value));
                    cur_state.registers.flag[16] = extract<16, 17>(value);
                    cur_state.registers.flag[17] = extract<17, 18>(value);
                    cur_state.registers.flag[18] = extract<18, 19>(value);
                    cur_state.registers.flag[19] = extract<19, 20>(value);
                    cur_state.registers.flag[20] = extract<20, 21>(value);
                    cur_state.registers.flag[21] = extract<21, 22>(value);
                    cur_state.registers.flag[22] = extract<22, 23>(value);
                    cur_state.registers.flag[23] = extract<23, 24>(value);
                    cur_state.registers.flag[24] = extract<24, 25>(value);
                    cur_state.registers.flag[25] = extract<25, 26>(value);
                    cur_state.registers.flag[26] = extract<26, 27>(value);
                    cur_state.registers.flag[27] = extract<27, 28>(value);
                    cur_state.registers.flag[28] = extract<28, 29>(value);
                    cur_state.registers.flag[29] = extract<29, 30>(value);
                    cur_state.registers.flag[30] = extract<30, 31>(value);
                    cur_state.registers.flag[31] = extract<31, 32>(value);
                    break;
                default:
                    throw Exception("double word access not valid for this register type");
            }
            break;

        default:
            throw Exception("invalid register access width");
    }
}

#undef EIP_LOCATION
