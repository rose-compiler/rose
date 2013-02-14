/* This source file contains anything related to SgAsm*Expression nodes. */
#include "sage3basic.h"
#include "stringify.h"          /* used in error messages */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Documentation for ROSETTA-generated things.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** @class SgAsmValueExpression
 *  Assembly operands that contain numeric values.
 *
 *  Assembly instruction (SgAsmInstruction) operands are represented by SgAsmExpression nodes in the AST. If the expression has
 *  a numeric value then an SgAsmValueExpression is used, which is a subclass of SgAsmExpression.
 * 
 *  Values of various types (integers and floating-point values of various sizes) are represented by subclasses of
 *  SgAsmValueExpression.
 */

/** @var unsigned short SgAsmValueExpression::p_bit_offset
 *  The bit position of the value in the instruction encoding.
 *  
 *  If a numeric constant comes directly from an instruction encoding, then the p_bit_size attribute will be non-zero and the
 *  p_bit_offset will contain the index for the first bit of the constant.
 *  
 *  Bits are numbered so that bits zero through seven are in the first byte, bits eight through 15 are in the second byte,
 *  etc. Within a byte, bits are numbered so that lower indexes are less significant bits. In other words, given bit index N
 *  and an array of bytes, the particular bit value is:
 *  
 * @code
 *  SgUnsignedCharList bytes;
 *  unsigned short index;
 *  bool bit = (bytes[index/8] & (1<<(index%8))) ? true : false;
 * @endcode
 */

/** @var unsigned short SgAsmValueExpression::p_bit_size
 *  The size of the value in the instruction encoding.
 *  
 *  If a numeric constant comes directly from an instruction encoding, then the p_bit_size attribute will be non-zero and
 *  indicate the number of bits used to represent the constant in the encoding.
 *  
 *  @sa SgAsmValueExpression::p_bit_offset
 */

/** @fn unsigned short SgAsmValueExpression::get_bit_offset() const
 *  Accessor for SgAsmValueExpression::p_bit_offset */

/** @fn unsigned short SgAsmValueExpression::get_bit_size() const
 * Accessor for SgAsmValueExpression::p_bit_size */


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** @class SgAsmIntegerValueExpression
 *  Base class for integer values.
 *
 *  An integer value consists of an offset from an optional base node.  The base node must be some kind of object with a
 *  virtual address, such as an instruction, symbol, segment, etc.  If no base node is associated with the
 *  SgAsmIntegerValueExpression (the default situation), then a zero base address is used.
 *
 *  When a (new) base object is associated with an SgAsmIntegerValueExpression via the make_relative_to() method, the value of
 *  the expression does not change.  However, the value does change when the address of the associated base node changes.  For
 *  instance, one way to represent a function call to "main" is to have a CALL instruction whose operand is an
 *  SgAsmIntegerValueExpression that has a base which is either the entry instruction of "main" or the symbol for "main".  That
 *  way, if the address of "main" changes then the target address in the CALL instruction also changes.
 *
 *  This base class stores the offset as a 64-bit, unsigned integer that is accessed with the get_relative_value() and
 *  set_relative_value() methods.  The class also defines get_absolute_value() and set_absolute_value() methods that operate on
 *  the absolute value (which isn't actually stored anywhere).  Subclasses will typically define their get_value() and
 *  set_value() methods in terms of get_absolute_value() and set_absolute() using a narrower integer type. */

/** @fn SgNode *get_base_node() const
 *  Returns the base node associated with an integer. */

/** @fn void set_base_node(SgNode*)
 *  Sets the base node associated with an integer.
 *
 *  The base node is changed without updating this object's relative value, thus this object's absolute value changes.  The
 *  value returned by subclass get_value() methods will probably differ from what it would have returned before calling
 *  set_base_node().  If this is not the behavior that's needed, see the make_relative_to() method. */

/** @fn uint64_t get_relative_value() const
 *  Returns the value relative to the base node address. If there is no associated base node, then get_relative_value() returns
 *  the same value as get_absolute_value(). */

/** @fn void set_relative_value(uint64_t)
 *  Sets the value relative to the base node address.  Changing the relative value also changes the absolute value by the same
 *  amount. */






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      SgAsmIntegerValueExpression
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** Returns the base address of an addressable IR node. */
uint64_t
SgAsmIntegerValueExpression::virtual_address(SgNode *node)
{
    if (!node)
        return 0;
    if (isSgAsmFunction(node))
        return isSgAsmFunction(node)->get_entry_va();
    if (isSgAsmStatement(node)) // instructions, block, function, staticdata, ...
        return isSgAsmStatement(node)->get_address();
    if (isSgAsmGenericSymbol(node))
        return isSgAsmGenericSymbol(node)->get_value();
    if (isSgAsmPEImportItem(node))
        return isSgAsmPEImportItem(node)->get_bound_rva().get_va();
    if (isSgAsmPERVASizePair(node))
        return isSgAsmPERVASizePair(node)->get_e_rva().get_va();
    if (isSgAsmGenericSection(node)) {
        SgAsmGenericSection *section = isSgAsmGenericSection(node);
        if (section->is_mapped())
            return section->get_mapped_actual_va();
        return 0;
    }

    std::cerr <<"SgAsmIntegerValueExpression::virtual_address: non-addressable node type: "
              <<stringifyVariantT(node->variantT(), "V_") <<std::endl;
    assert(!"need addressable node type"); // to get location info in error messsage
    abort(); // if asserts are disabled
}

/** Returns a label for the value.  The label consists of the base object name (if available) or address, followed by a plus
 *  sign or minus sign, followed by the offset from that object.  The empty string is returned if this integer value expression
 *  has no base object (i.e., it's absolute).
 *
 *  If the base object has no name and the integer value points directly at the object (offset=0) then one of two things
 *  happen: if @p quiet is true, the empty string is returned, otherwise the label is the name of the node type enclosed in an
 *  extra set of angle brackets.  This is useful to indicate that a value is relative rather than absolute.  For instance, the
 *  instruction listing "call 0x004126bb" is ambiguous as to whether 0x004126bb points to a known, unnamed function, a non-entry
 *  instruction within a function, or some memory location we didn't disassemble.  But when labeled with @p quiet being false,
 *  the output will be:
 *
 *  <ul>
 *    <li>call 0x004126bb<main>; points to a function with a name</li>
 *    <li>call 0x004126bb<<Func>>; points to a function without a name</li>
 *    <li>call 0x004126bb<<Insn>>; points to an instruction that's not a function entry point</li>
 *    <li>call 0x004126bb; points to something that's not been disassembled</li>
 *  </ul>
 */
std::string
SgAsmIntegerValueExpression::get_label(bool quiet/*=false*/) const
{
    SgNode *node = get_base_node();
    if (!node)
        return "";

    // Get the name of the base object if possible.
    std::string retval;
    std::string refkind;
    if (isSgAsmFunction(node)) {
        retval = isSgAsmFunction(node)->get_name();
        refkind = "Func";
    } else if (isSgAsmGenericSymbol(node)) {
        retval = isSgAsmGenericSymbol(node)->get_name()->get_string();
        refkind = "Sym";
    } else if (isSgAsmPEImportItem(node)) {
        retval = isSgAsmPEImportItem(node)->get_name()->get_string();
        refkind = "Import";
    } else if (isSgAsmGenericSection(node)) {
        retval = isSgAsmGenericSection(node)->get_short_name();
        refkind = "Section";
    } else if (isSgAsmInstruction(node)) {
        refkind = "Insn";
    } else if (isSgAsmStaticData(node)) {
        SgAsmBlock *blk = SageInterface::getEnclosingNode<SgAsmBlock>(node);
        if (blk && 0!=(blk->get_reason() & SgAsmBlock::BLK_JUMPTABLE)) {
            refkind = "JumpTable";
        } else {
            refkind = "StaticData";
        }
    } else if (isSgAsmBlock(node)) {
        refkind = "BBlock";
    } else if (isSgAsmPERVASizePair(node)) {
        refkind = "Rva/Size";
    } else {
        refkind = "Reference";
    }

    // If it has no name, then use something fairly generic.  That way we can at least indicate that the value is relative.
    int64_t offset = (int64_t)get_relative_value();
    if (retval.empty()) {
        retval = "<" + refkind; // extra level of brackets to indicate that it's not a real name
        if (offset)
            retval += "@" + StringUtility::addrToString(virtual_address(node), 32, false/*unsigned*/);
        retval += ">";
    }

    // Append the offset, but consider it to be signed.  Disregard the number of significant bits in the absolute value and use
    // a smaller bit width if possible.  But don't use the minimum bit width since this makes it hard to tell how many bits
    // there are at a glance (use only 8, 16, 32, or 64).
    size_t nbits = 0;
    if (offset > 0xffffffffll) {
        nbits = 64;
        retval += "+";
    } else if (offset > 0xffffll) {
        nbits = 32;
        retval += "+";
    } else if (offset > 0xffll) {
        nbits = 16;
        retval += "+";
    } else if (offset > 9) {
        nbits = 8;
        retval += "+";
    } else if (offset > 0) {
        char buf[64];
        snprintf(buf, sizeof buf, "+%"PRId64, offset);
        retval += buf;
    } else if (offset==0) {
        /*void*/
    } else if (-offset > 0xffffffffll) {
        nbits = 64;
        offset = -offset;
        retval += "-";
    } else if (-offset > 0xffffll) {
        nbits = 32;
        offset = -offset;
        retval += "-";
    } else if (-offset > 0xffll) {
        nbits = 16;
        offset = -offset;
        retval += "-";
    } else if (-offset > 9) {
        nbits = 8;
        offset = -offset;
        retval += "-";
    } else {
        char buf[64];
        snprintf(buf, sizeof buf, "%"PRId64, offset);
        retval += buf;
    }
    if (nbits!=0)
        retval += StringUtility::addrToString(offset, nbits, false/*unsigned*/);

    return retval;
}

/** Set the number of significant bits to return by default for get_absolute_value(). */
void
SgAsmIntegerValueExpression::set_significant_bits(size_t nbits)
{
    assert(nbits>0 && nbits<=64); // usual values are 8, 16, 32, and 64
    p_significant_bits = nbits;
}

/** Return the number of significant bits in the value. */
size_t
SgAsmIntegerValueExpression::get_significant_bits() const
{
    return p_significant_bits;
}

/** Makes the value of this integer relative to some other addressable node.  The absolute value of this expression is
 *  unchanged by this operation. The @p base_node must be a type of IR node that has a virtual address, such as another
 *  instruction.  If @p base_node is the null pointer, then the "relativeness" of this constant is removed (i.e., it will be
 *  relative to zero). */
void
SgAsmIntegerValueExpression::make_relative_to(SgNode *base_node)
{
    uint64_t cur_value = get_absolute_value();
    uint64_t base_va = virtual_address(base_node);
    set_base_node(base_node);
    set_relative_value(cur_value - base_va);    // mod 2^64
}

/** Returns the base address.  The base address is the virtual address of the associated IR node, or zero if no IR node is
 *  associated with this integer value. */
uint64_t
SgAsmIntegerValueExpression::get_base_address() const
{
    return virtual_address(get_base_node());
}

/** Returns the current absolute value.  The absolute value is the sum of the address of the base node (or zero if no base node
 *  is associated with this object) and the offset.  Subclasses typically redefine this method to return a narrower integer
 *  type.  All underlying arithmetic is modulo 2^64; this function returns only the specified number of low-order bits.  If @p
 *  nbits is zero, then  get_significant_bits() is called. */
uint64_t
SgAsmIntegerValueExpression::get_absolute_value(size_t nbits) const
{
    if (0==nbits)
        nbits = get_significant_bits();
    uint64_t retval = get_base_address() + get_relative_value();
    // note: (uint64_t)1 << 64 is undefined behavior, so do it the hard way
    uint64_t mask = (nbits >= 64 ? (uint64_t)0 : (uint64_t)1<<(uint64_t)nbits) - 1;
    return retval & mask; // clear high-order bits
}

/** Returns the current absolute value.  The returned value is signed, unlike get_absolute_value(). */
int64_t
SgAsmIntegerValueExpression::get_absolute_signed_value() const
{
    return get_base_address() + get_relative_value();
}

/** Set absolute value.  Changes the absolute value of this integer expression without changing the base node.   Subclasses
 *  typically redefine this method to take a narrower integer argument, sign extend that argument, and then call the base
 *  class' method. */
void
SgAsmIntegerValueExpression::set_absolute_value(uint64_t v)
{
    uint64_t new_offset = v - get_base_address(); // mod 2^64
    set_relative_value(new_offset);
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Accessors for subclasses
//
// These are the get_value() and set_value() accessors for subclasses.  They're backward compatible with the old interface that
// stored the absolute value in the subclass objects with data types particular to that subclass.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Returns absolute value as an 8-bit unsigned integer.  See SgAsmIntegerValueExpression::get_absolute_value() */
uint8_t
SgAsmByteValueExpression::get_value() const
{
    return SgAsmIntegerValueExpression::get_absolute_value();
}

/** Sets absolute value.  See SgAsmIntegerValueExpression::set_absolute_value(). */
void
SgAsmByteValueExpression::set_value(uint8_t n_)
{
    set_significant_bits(8);
    uint64_t n = (int64_t)(int8_t)n_; // sign extend
    SgAsmIntegerValueExpression::set_absolute_value(n);
}

/** Returns absolute value as a 16-bit unsigned integer. See SgAsmIntegerValueExpression::get_absolute_value() */
uint16_t
SgAsmWordValueExpression::get_value() const
{
    return SgAsmIntegerValueExpression::get_absolute_value();
}

/** Sets absolute value.  See SgAsmIntegerValueExpression::set_absolute_value(). */
void
SgAsmWordValueExpression::set_value(uint16_t n_)
{
    set_significant_bits(16);
    uint64_t n = (int64_t)(int16_t)n_; // sign extend
    SgAsmIntegerValueExpression::set_absolute_value(n);
}

/** Returns absolute value as a 32-bit unsigned integer. See SgAsmIntegerValueExpression::get_absolute_value() */
uint32_t
SgAsmDoubleWordValueExpression::get_value() const
{
    return SgAsmIntegerValueExpression::get_absolute_value();
}

/** Sets absolute value.  See SgAsmIntegerValueExpression::set_absolute_value(). */
void
SgAsmDoubleWordValueExpression::set_value(uint32_t n_)
{
    set_significant_bits(32);
    uint64_t n = (int64_t)(int32_t)n_; // sign extend
    SgAsmIntegerValueExpression::set_absolute_value(n);
}

/** Returns absolute value as a 64-bit unsigned integer. See SgAsmIntegerValueExpression::get_absolute_value() */
uint64_t
SgAsmQuadWordValueExpression::get_value() const
{
    return SgAsmIntegerValueExpression::get_absolute_value();
}

/** Sets absolute value.  See SgAsmIntegerValueExpression::set_absolute_value(). */
void
SgAsmQuadWordValueExpression::set_value(uint64_t n_)
{
    set_significant_bits(64);
    SgAsmIntegerValueExpression::set_absolute_value(n_);
}

