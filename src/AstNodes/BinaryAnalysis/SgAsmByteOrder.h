/** Byte order specification. */
class SgAsmByteOrder: public SgAsmExpression {

    /** Property: Byte order. */
    [[using Rosebud: ctor_arg]]
    ByteOrder::Endianness byteOrder;
};
