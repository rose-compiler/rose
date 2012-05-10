/* SgAsmBlock member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file) because
 * then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "stringify.h"

/** Fall-through virtual address.  A block's fall-through address is the virtual address that follows the last byte of the
 *  block's last instruction.  The block must have instructions (e.g., it cannot be a strict data block). */
rose_addr_t
SgAsmBlock::get_fallthrough_va()
{
    ROSE_ASSERT(!get_statementList().empty());
    SgAsmInstruction *last = isSgAsmInstruction(get_statementList().back());
    ROSE_ASSERT(last!=NULL);
    return last->get_address() + last->get_size();
}

/** Determins if a block contains instructions.  Returns true if the block has instructions, false otherwise. We look only at
 *  the immediate descendants of this block.  See also, SageInterface::querySubTree() in order to get the list of all
 *  instructions or to consider all descendants. */
bool
SgAsmBlock::has_instructions() const
{
    const SgAsmStatementPtrList &stmts = get_statementList();
    for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
        if (isSgAsmInstruction(*si))
            return true;
    }
    return false;
}

/** Returns a multi-line string describing the letters used for basic block reasons.  The letters are returned by the padding
 *  version of reason_str(). */
std::string
SgAsmBlock::reason_key(const std::string &prefix)
{
    return (prefix + "L = left over blocks    N = NOP/zero padding     F = fragment\n" +
            prefix + "J = jump table          E = Function entry\n" +
            prefix + "H = CFG head            U = user-def reason      M = miscellaneous\n" +
            prefix + "1 = first CFG traversal 2 = second CFG traversal 3 = third CFG traversal\n");
}

/** Returns reason string for this block. */
std::string
SgAsmBlock::reason_str(bool do_pad) const
{
    return reason_str(do_pad, get_reason());
}

/** Class method that converts a reason bit vector to a human-friendly string. The second argument is the bit vector of
 *  SgAsmBlock::Reason bits.  Some of the positions in the padded return value are used for more than one bit.  For instance,
 *  the first character can be "L" for leftovers, "N" for padding, "E" for entry point, or "-" for none of the above. */
std::string
SgAsmBlock::reason_str(bool do_pad, unsigned r)
{
    using namespace StringUtility; // for add_to_reason_string()
    std::string result;

    if (r & BLK_LEFTOVERS) {
        add_to_reason_string(result, true, do_pad, "L", "leftovers");
    } else if (r & BLK_PADDING) {
        add_to_reason_string(result, true, do_pad, "N", "padding");
    } else if (r & BLK_FRAGMENT) {
        add_to_reason_string(result, true, do_pad, "F", "fragment");
    } else if (r & BLK_JUMPTABLE) {
        add_to_reason_string(result, true, do_pad, "J", "jumptable");
    } else {
        add_to_reason_string(result, (r & BLK_ENTRY_POINT),  do_pad, "E", "entry point");
    }

    if (r & BLK_CFGHEAD) {
        add_to_reason_string(result, true, do_pad, "H", "CFG head");
    } else if (r & BLK_GRAPH1) {
        add_to_reason_string(result, true, do_pad, "1", "graph-1");
    } else if (r & BLK_GRAPH2) {
        add_to_reason_string(result, true, do_pad, "2", "graph-2");
    } else {
        add_to_reason_string(result, (r & BLK_GRAPH3), do_pad, "3", "graph-3");
    }

    if (r & BLK_USERDEF) {
        add_to_reason_string(result, true, do_pad, "U", "user defined");
    } else {
        char abbr[32], full[64];
        int width = snprintf(abbr, sizeof abbr, "%u", BLK_MISCMASK);
        snprintf(abbr, sizeof abbr, "M%0*u", width, (r & BLK_MISCMASK));
        abbr[sizeof(abbr)-1] = '\0';
        if (!do_pad) {
            std::string miscname = stringifySgAsmBlockReason((r & BLK_MISCMASK), "BLK_");
            if (miscname.empty() || miscname[0]=='(') {
                snprintf(full, sizeof full, "misc-%u", (r & BLK_MISCMASK));
            } else {
                for (size_t i=0; i<miscname.size(); ++i)
                    miscname[i] = tolower(miscname[i]);
                strncpy(full, miscname.c_str(), sizeof full);
            }
            full[sizeof(full)-1] = '\0';
        } else {
            full[0] = '\0';
        }
        add_to_reason_string(result, (r & BLK_MISCMASK), do_pad, abbr, full);
    }
    return result;
}
