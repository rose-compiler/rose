#include "sage3basic.h"
#include "integerOps.h"
#include "stringify.h"
#include "Diagnostics.h"
#include <sawyer/BitVector.h>

using namespace rose;

/** @class SgAsmIntegerValueExpression
 *  Base class for integer values.
 *
 *  An integer value consists of an offset from an optional base node.  The base node must be some kind of object with a
 *  virtual address, such as an instruction, symbol, segment, etc.  If no base node is associated with the
 *  SgAsmIntegerValueExpression (the default situation), then a zero base address is used.
 *
 *  When a (new) base object is associated with an SgAsmIntegerValueExpression via the makeRelativeTo() method, the value of
 *  the expression does not change.  However, the value does change when the address of the associated base node changes.  For
 *  instance, one way to represent a function call to "main" is to have a CALL instruction whose operand is an
 *  SgAsmIntegerValueExpression that has a base which is either the entry instruction of "main" or the symbol for "main".  That
 *  way, if the address of "main" changes then the target address in the CALL instruction also changes.
 *
 *  The base class stores the bits that are interpretted as the signed offset. The offset is accessed with get_relativeValue()
 *  and set_relativeValue() methods. The class also defines get_absoluteValue() and set_aabsoluteValue() methods that operate
 *  on the absolute value (which isn't actually stored anywhere). */

/** @fn SgNode *get_baseNode() const
 *  Returns the base node associated with an integer. */

/** @fn void set_baseNode(SgNode*)
 *  Sets the base node associated with an integer.
 *
 *  The base node is changed without updating this object's relative value, thus this object's absolute value changes.  The
 *  value returned by get_absoluteValue() will probably differ from what it would have returned before calling
 *  set_baseNode().  If this is not the behavior that's needed, see the makeRelativeTo() method. */


SgAsmIntegerValueExpression::SgAsmIntegerValueExpression(uint64_t value, SgAsmType *type)
    : p_baseNode(NULL) {
    ASSERT_not_null(type);
    ASSERT_require2(type->get_nBits() <= 8*sizeof(value), "ambiguous signed/unsigned interpretation");
    set_type(type);
    p_bitVector.resize(type->get_nBits()).fromInteger(value);
}

SgAsmIntegerValueExpression::SgAsmIntegerValueExpression(const Sawyer::Container::BitVector &bv, SgAsmType *type)
    : p_baseNode(NULL) {
    ASSERT_not_null(type);
    ASSERT_require2(bv.size()==type->get_nBits(),
                    "value width (" + StringUtility::plural(bv.size(), "bits") + ") does not match type width (" +
                    StringUtility::plural(type->get_nBits(), "bits") + ")");
    set_type(type);
    p_bitVector = bv;
}

/** Returns the base address of an addressable IR node. */
uint64_t
SgAsmIntegerValueExpression::virtualAddress(SgNode *node)
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

    ASSERT_not_reachable("need addressable node type, got " + stringifyVariantT(node->variantT(), "V_"));
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
    SgNode *node = get_baseNode();
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
    int64_t offset = get_relativeValue();
    if (retval.empty()) {
        retval = "<" + refkind; // extra level of brackets to indicate that it's not a real name
        if (offset)
            retval += "@" + StringUtility::addrToString(virtualAddress(node), 32);
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
        retval += StringUtility::addrToString(offset, nbits);

    return retval;
}

/** Return the number of significant bits in the value. */
size_t
SgAsmIntegerValueExpression::get_significantBits() const
{
    return p_bitVector.size();
}

/** Makes the value of this integer relative to some other addressable node.  The absolute value of this expression is
 *  unchanged by this operation. The @p baseNode must be a type of IR node that has a virtual address, such as another
 *  instruction.  If @p baseNode is the null pointer, then the "relativeness" of this constant is removed (i.e., it will be
 *  relative to zero). */
void
SgAsmIntegerValueExpression::makeRelativeTo(SgNode *baseNode)
{
    ASSERT_require(get_significantBits() <= 8*sizeof(uint64_t));
    ASSERT_require(get_significantBits() > 0);
    int64_t curValue = get_absoluteValue();
    int64_t baseVa = virtualAddress(baseNode);
    int64_t offset = curValue - baseVa;
    set_baseNode(baseNode);

    // We don't want to change the size of the offset if we can help it.  But if we do need to widen it then use a power of two
    // bytes: 1, 2, 4, or 8
    size_t needWidth = 0;
    if (offset >= 0) {
        if ((uint64_t)offset > 0xffffffff) {
            needWidth = 64;
        } else if ((uint64_t)offset > 0xffff) {
            needWidth = 32;
        } else if ((uint64_t)offset > 0xff) {
            needWidth = 16;
        } else {
            needWidth = 8;
        }
    } else {
        if (offset < -4294967296) {
            needWidth = 64;
        } else if (offset < -65536) {
            needWidth = 32;
        } else if (offset < -256) {
            needWidth = 16;
        } else {
            needWidth = 8;
        }
    }

    size_t newWidth = std::max(get_significantBits(), needWidth);
    ASSERT_require(newWidth>0);
    uint64_t uoffset = (uint64_t)offset & IntegerOps::genMask<uint64_t>(newWidth);
    set_relativeValue(uoffset, newWidth);
}

/** Returns the base address.  The base address is the virtual address of the associated IR node, or zero if no IR node is
 *  associated with this integer value. */
uint64_t
SgAsmIntegerValueExpression::get_baseAddress() const
{
    return virtualAddress(get_baseNode());
}

/** Returns the current absolute value zero filled to 64 bits.  The absolute value is the 64-bit sum of the 64-bit address of
 *  the base node (or zero if no base node is associated with this object) and the 64-bit offset. However, this function
 *  returns only the specified number of low-order bits zero extended to the 64-bit return type.  If @p nbits is zero, then
 *  get_significantBits() is called. */
uint64_t
SgAsmIntegerValueExpression::get_absoluteValue(size_t nbits) const
{
    if (0==nbits)
        nbits = get_significantBits();
    ASSERT_require(nbits <= 8*sizeof(uint64_t));
    uint64_t retval = get_baseAddress() + get_relativeValue();
    uint64_t mask = IntegerOps::genMask<uint64_t>(nbits);
    return retval & mask; // clear high-order bits
}

/** Returns the current absolute value (base+offset) as a signed value. */
int64_t
SgAsmIntegerValueExpression::get_signedValue() const
{
    ASSERT_require(get_significantBits() <= 8*sizeof(uint64_t));
    int64_t retval = get_baseAddress() + get_relativeValue();
    return retval;
}

/** Set absolute value.  Changes the absolute value of this integer expression without changing the base node. */
void
SgAsmIntegerValueExpression::set_absoluteValue(uint64_t v)
{
    int64_t new_offset = v - get_baseAddress(); // mod 2^64
    set_relativeValue(new_offset);
}

/** Get relative value.  Interprets the bit vector as a signed value, sign extends it to 64-bits if necessary, and returns it. */
int64_t
SgAsmIntegerValueExpression::get_relativeValue() const
{
    size_t nbits = get_significantBits();
    ASSERT_require(nbits <= 8*sizeof(int64_t));
    ASSERT_require(nbits > 0);
    uint64_t uv = p_bitVector.toInteger();
    int64_t sv = IntegerOps::signExtend2(uv, nbits, 8*sizeof(int64_t));
    return sv;
}

/** Set relative value without changing the base value.  The relative value is interpretted as a signed value of the specified
 * width (defaulting to 64-bits). */
void
SgAsmIntegerValueExpression::set_relativeValue(int64_t v, size_t nbits)
{
    ASSERT_require(nbits > 0);
    ASSERT_require(nbits <= 64);

    p_bitVector.resize(nbits).fromInteger(v);
}


// FIXME[Robb P. Matzke 2014-07-22]: These are deprecated; use CamelCase versions instead
size_t SgAsmIntegerValueExpression::get_significant_bits() const { return get_significantBits(); }
void SgAsmIntegerValueExpression::make_relative_to(SgNode *b) { makeRelativeTo(b); }
uint64_t SgAsmIntegerValueExpression::get_base_address() const { return get_baseAddress(); }
uint64_t SgAsmIntegerValueExpression::get_absolute_value(size_t nbits) const { return get_absoluteValue(nbits); }
void SgAsmIntegerValueExpression::set_absolute_value(uint64_t v) { set_absoluteValue(v); }
int64_t SgAsmIntegerValueExpression::get_signed_value() const { return get_signedValue(); }
int64_t SgAsmIntegerValueExpression::get_relative_value() const { return get_relativeValue(); }
void SgAsmIntegerValueExpression::set_relative_value(int64_t v, size_t nbits) { set_relativeValue(v, nbits); }
