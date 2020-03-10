/* SgAsmBlock member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file) because
 * then they won't get indexed/formatted/etc. by C-aware tools. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "stringify.h"

using namespace Rose;

rose_addr_t
SgAsmBlock::get_fallthrough_va()
{
    ROSE_ASSERT(!get_statementList().empty());
    SgAsmInstruction *last = isSgAsmInstruction(get_statementList().back());
    ROSE_ASSERT(last!=NULL);
    return last->get_address() + last->get_size();
}

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

std::string
SgAsmBlock::reason_key(const std::string &prefix)
{
    return (prefix + "L = left over blocks    N = NOP/zero padding     F = fragment\n" +
            prefix + "J = jump table          E = Function entry\n" +
            prefix + "H = CFG head            U = user-def reason      M = miscellaneous\n" +
            prefix + "1 = first CFG traversal 2 = second CFG traversal 3 = third CFG traversal\n");
}

std::string
SgAsmBlock::reason_str(bool do_pad) const
{
    return reason_str(do_pad, get_reason());
}

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

bool
SgAsmBlock::is_function_call(rose_addr_t &target_va, rose_addr_t &return_va) 
{
    static const rose_addr_t INVALID_ADDR = (rose_addr_t)(-1);
    target_va = return_va = INVALID_ADDR;;
    if (!is_basic_block())
        return false;
    std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(this);
    assert(!insns.empty()); // basic blocks must have instructions

    // Check that all the successors point to functions entry addresses (other functions or this block's function).  There
    // might be one edge that points to the fall-through address of this block, and that's ok.
    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(this);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(func);
    std::set<rose_addr_t> callee_vas;
    if (interp) {
        const InstructionMap &imap = interp->get_instruction_map();
        const SgAsmIntegerValuePtrList &successors = get_successors();
        for (SgAsmIntegerValuePtrList::const_iterator si=successors.begin(); si!=successors.end(); ++si) {
            rose_addr_t successor_va = (*si)->get_absoluteValue();
            if (SgAsmInstruction *target_insn = imap.get_value_or(successor_va, NULL)) {
                SgAsmFunction *target_func = SageInterface::getEnclosingNode<SgAsmFunction>(target_insn);
                if (successor_va==target_func->get_entry_va()) {
                    callee_vas.insert(successor_va); // branches to a function entry point
                } else if (return_va!=INVALID_ADDR) {
                    target_va = return_va = INVALID_ADDR;
                    return false; // multiple function-local CFG edges that are not this function's entry point
                } else {
                    return_va = successor_va; // possible return address
                }
            }
        }
    }

    // Now for the architecture-dependent determination.  This will not update target_va or return_va if they cannot be
    // determined or are ambiguous; so we must reset them to INVALID_ADDR if we're about to return false.
    bool retval = insns.front()->isFunctionCallSlow(insns, &target_va, &return_va);
    if (!retval) {
        target_va = return_va = INVALID_ADDR;
    } else if (INVALID_ADDR==target_va && 1==callee_vas.size()) {
        target_va = *callee_vas.begin();
    }
    return retval;
}

void
SgAsmBlock::append_statement( SgAsmStatement* statement )
   {
     p_statementList.push_back(statement);
   }

void
SgAsmBlock::remove_children(  )
   {
     p_statementList.clear();
   }

void
SgAsmBlock::remove_statement( SgAsmStatement* statement )
   {
     SgAsmStatementPtrList::iterator l = p_statementList.begin();
     for (;l!=p_statementList.end();l++) {
        SgAsmStatement* st = *l;
        if (st==statement) {
             break;
        }
     }  
        if (l!=p_statementList.end())
            p_statementList.erase(l);
   }

SgAsmFunction *
SgAsmBlock::get_enclosing_function() const {
        return SageInterface::getEnclosingNode<SgAsmFunction>(this);
}

#endif
