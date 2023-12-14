// FIXME[Robb P Matzke 2016-10-31]: no idea what this is
class SgAsmControlFlagsExpression: public SgAsmExpression {
public:
    [[using Rosebud: rosetta]]
    unsigned long bitFlags = 0;

    // [Robb Matzke 2023-11-06] deprecated 2023-11
    unsigned long get_bit_flags() const ROSE_DEPRECATED("use get_bitFlags");
    void set_bit_flags(unsigned long) ROSE_DEPRECATED("use set_bitFlags");
};
