// FIXME[Robb P Matzke 2017-02-13]: what is this?
class SgAsmSynthesizedFieldDeclaration: public SgAsmSynthesizedDeclaration {
public:
    /** Property: Name. */
    [[using Rosebud: rosetta]]
    std::string name;

    // FIXME[Robb P Matzke 2017-02-13]: Is this bytes, bits, or what?
    // Not clear if we want to store the offset explicitly
    /** Property: Offset. */
    [[using Rosebud: rosetta]]
    uint64_t offset = 0;
};
