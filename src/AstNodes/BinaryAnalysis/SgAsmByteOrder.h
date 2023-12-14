#include <Rose/BinaryAnalysis/ByteOrder.h>

/** Byte order specification. */
class SgAsmByteOrder: public SgAsmExpression {
public:
    /** Property: Byte order. */
    [[using Rosebud: ctor_arg]]
    Rose::BinaryAnalysis::ByteOrder::Endianness byteOrder;
};
