/** An expression class for Jovial bit values.
 *
 *  A Jovial bit formula (expression) consists of bit operands and a bit operator.
 *  The bit operators are NOT, AND, OR, XOR, and EQV.
 */
class SgJovialBitVal: public SgValueExp {

    /** Property: A copy (as coded) of the bit value string. */
    [[using Rosebud: rosetta, ctor_arg]]
    std::string valueString;

    // FIXME[Robb Matzke 2023-04-21]: These pragmas are compatibility of last resort. Please remove when possible because in my
    // experience most of this stuff doesn't need to be generated--it can be written directly in C++ using virtual functions,
    // template meta programming, and introspection rather than text substitution.
#pragma rosetta setFunctionSource("SOURCE_EMPTY_POST_CONSTRUCTION_INITIALIZATION", "../Grammar/Expression.code")
#pragma rosetta setFunctionSource("SOURCE_GET_TYPE_GENERIC","../Grammar/Expression.code");
#pragma rosetta editSubstitute("GENERIC_TYPE", "SgJovialBitType");
};
