#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "integerOps.h"
#include "stringify.h"
#include "Diagnostics.h"
#include <Sawyer/BitVector.h>

using namespace Rose;



SgAsmIntegerValueExpression::SgAsmIntegerValueExpression(uint64_t value, SgAsmType *type)
    : p_baseNode(NULL) {
    ASSERT_not_null(type);
    ASSERT_require2(0 == value || type->get_nBits() <= 8*sizeof(value), "ambiguous signed/unsigned interpretation");
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

std::string
SgAsmIntegerValueExpression::get_label(bool quiet/*=false*/) const
{
    SgNode *node = get_baseNode();
    if (node == NULL)
        return "";

    // Get the name of the base object if possible.
    std::string retval;
    std::string refkind;
    SgAsmGenericSymbol* symbol = isSgAsmGenericSymbol(node);
    if (isSgAsmFunction(node)) {
        retval = isSgAsmFunction(node)->get_name();
        refkind = "Func";
    } else if (symbol != NULL) {
        retval = symbol->get_name()->get_string();
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
        snprintf(buf, sizeof buf, "+%" PRId64, offset);
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
        snprintf(buf, sizeof buf, "%" PRId64, offset);
        retval += buf;
    }
    if (nbits!=0)
        retval += StringUtility::addrToString(offset, nbits);

    return retval;
}

size_t
SgAsmIntegerValueExpression::get_significantBits() const
{
    return p_bitVector.size();
}

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
        if (offset < -4294967296ll) {
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

uint64_t
SgAsmIntegerValueExpression::get_baseAddress() const
{
    return virtualAddress(get_baseNode());
}

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

int64_t
SgAsmIntegerValueExpression::get_signedValue() const
{
    ASSERT_require(get_significantBits() <= 8*sizeof(uint64_t));
    int64_t retval = get_baseAddress() + get_relativeValue();
    return retval;
}

void
SgAsmIntegerValueExpression::set_absoluteValue(uint64_t v)
{
    int64_t new_offset = v - get_baseAddress(); // mod 2^64
    set_relativeValue(new_offset);
}

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

void
SgAsmIntegerValueExpression::set_relativeValue(int64_t v, size_t nbits)
{
    ASSERT_require(nbits > 0);
    ASSERT_require(nbits <= 64);

    p_bitVector.resize(nbits).fromInteger(v);
}

#endif
