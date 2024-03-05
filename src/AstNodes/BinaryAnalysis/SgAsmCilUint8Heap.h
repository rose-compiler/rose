/** Base class for CIL branch of binary analysis IR nodes. */
class SgAsmCilUint8Heap : public SgAsmCilDataStream {
    [[using Rosebud: rosetta, mutators(), large]]
    std::vector<uint8_t> Stream;

public:
    void parse(const std::vector<uint8_t>& buf, size_t startOfMetaData) override;
    void unparse(std::vector<uint8_t>& buf, size_t startOfMetaData) const override;
    void dump(std::ostream& os) const override;
};

