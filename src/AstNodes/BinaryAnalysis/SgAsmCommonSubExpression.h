// FIXME[Robb P Matzke 2016-10-31]: no idea what this is
class SgAsmCommonSubExpression: public SgAsmExpression {
public:
    [[using Rosebud: rosetta, traverse]]
    SgAsmExpression* subexpression = nullptr;
};
