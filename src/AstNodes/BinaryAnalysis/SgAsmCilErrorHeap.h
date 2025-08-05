/** Base class for CIL branch of binary analysis IR nodes. */
class SgAsmCilErrorHeap : public SgAsmCilDataStream {
public:
    void parse(const std::vector<uint8_t>& buf, size_t startOfMetaData) override;
    void unparse(std::vector<uint8_t>& buf, size_t startOfMetaData) const override;
    void dump(std::ostream& os) const override;
};
