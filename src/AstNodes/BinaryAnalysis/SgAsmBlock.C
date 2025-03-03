#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmBlock.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionMap.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <stringify.h>

#include <SgAsmFunction.h>
#include <SgAsmInstruction.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmInterpretation.h>
#include <Cxx_GrammarDowncast.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

Address
SgAsmBlock::get_fallthroughVa()
{
    ROSE_ASSERT(!get_statementList().empty());
    SgAsmInstruction *last = isSgAsmInstruction(get_statementList().back());
    ROSE_ASSERT(last!=NULL);
    return last->get_address() + last->get_size();
}

bool
SgAsmBlock::hasInstructions() const
{
    const SgAsmStatementPtrList &stmts = get_statementList();
    for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
        if (isSgAsmInstruction(*si))
            return true;
    }
    return false;
}

bool
SgAsmBlock::isBasicBlock() const {
    return hasInstructions();
}

std::string
SgAsmBlock::reasonKey(const std::string &prefix)
{
    return (prefix + "L = left over blocks    N = NOP/zero padding     F = fragment\n" +
            prefix + "J = jump table          E = Function entry\n" +
            prefix + "H = CFG head            U = user-def reason      M = miscellaneous\n" +
            prefix + "1 = first CFG traversal 2 = second CFG traversal 3 = third CFG traversal\n");
}

std::string
SgAsmBlock::reasonString(bool do_pad) const
{
    return reasonString(do_pad, get_reason());
}

std::string
SgAsmBlock::reasonString(bool do_pad, unsigned r)
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
SgAsmBlock::isFunctionCall(Address &target_va, Address &return_va)
{
    static const Address INVALID_ADDR = (Address)(-1);
    target_va = return_va = INVALID_ADDR;;
    if (!isBasicBlock())
        return false;
    std::vector<SgAsmInstruction*> insns = AST::Traversal::findDescendantsTyped<SgAsmInstruction>(this);
    assert(!insns.empty()); // basic blocks must have instructions

    // Check that all the successors point to functions entry addresses (other functions or this block's function).  There
    // might be one edge that points to the fall-through address of this block, and that's ok.
    SgAsmFunction *func = AST::Traversal::findParentTyped<SgAsmFunction>(this);
    SgAsmInterpretation *interp = AST::Traversal::findParentTyped<SgAsmInterpretation>(func);
    std::set<Address> callee_vas;
    if (interp) {
        const InstructionMap &imap = interp->get_instructionMap();
        const SgAsmIntegerValuePtrList &successors = get_successors();
        for (SgAsmIntegerValuePtrList::const_iterator si=successors.begin(); si!=successors.end(); ++si) {
            Address successor_va = (*si)->get_absoluteValue();
            if (SgAsmInstruction *target_insn = imap.get_value_or(successor_va, NULL)) {
                SgAsmFunction *target_func = AST::Traversal::findParentTyped<SgAsmFunction>(target_insn);
                if (successor_va==target_func->get_entryVa()) {
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
    bool retval = insns.front()->architecture()->isFunctionCallSlow(insns, &target_va, &return_va);
    if (!retval) {
        target_va = return_va = INVALID_ADDR;
    } else if (INVALID_ADDR==target_va && 1==callee_vas.size()) {
        target_va = *callee_vas.begin();
    }
    return retval;
}

void
SgAsmBlock::appendStatement( SgAsmStatement* statement )
   {
     p_statementList.push_back(statement);
   }

void
SgAsmBlock::removeChildren(  )
   {
     p_statementList.clear();
   }

void
SgAsmBlock::removeStatement( SgAsmStatement* statement )
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
SgAsmBlock::get_enclosingFunction() const {
    return AST::Traversal::findParentTyped<SgAsmFunction>(const_cast<SgAsmBlock*>(this));
}

bool
SgAsmBlock::get_successors_complete() const {
    return get_successorsComplete();
}

void
SgAsmBlock::set_successors_complete(bool x) {
    set_successorsComplete(x);
}

SgAsmBlock*
SgAsmBlock::get_immediate_dominator() const {
    return get_immediateDominator();
}

void
SgAsmBlock::set_immediate_dominator(SgAsmBlock *x) {
    set_immediateDominator(x);
}

size_t
SgAsmBlock::get_cached_vertex() const {
    return get_cachedVertex();
}

void
SgAsmBlock::set_cached_vertex(size_t x) {
    set_cachedVertex(x);
}

double
SgAsmBlock::get_code_likelihood() const {
    return get_codeLikelihood();
}

void
SgAsmBlock::set_code_likelihood(double x) {
    set_codeLikelihood(x);
}

void
SgAsmBlock::append_statement(SgAsmStatement *x) {
    appendStatement(x);
}

void
SgAsmBlock::remove_statement(SgAsmStatement *x) {
    return removeStatement(x);
}

void
SgAsmBlock::remove_children() {
    removeChildren();
}

Address
SgAsmBlock::get_fallthrough_va() {
    return get_fallthroughVa();
}

SgAsmFunction*
SgAsmBlock::get_enclosing_function() const {
    return get_enclosingFunction();
}

bool
SgAsmBlock::has_instructions() const {
    return hasInstructions();
}

bool
SgAsmBlock::is_basic_block() const {
    return isBasicBlock();
}

bool
SgAsmBlock::is_function_call(Address &target_va/*out*/, Address &return_va/*out*/) {
    return isFunctionCall(target_va, return_va);
}

std::string
SgAsmBlock::reason_key(const std::string &prefix) {
    return reasonKey(prefix);
}

std::string
SgAsmBlock::reason_str(bool pad) const {
    return reasonString(pad);
}

std::string
SgAsmBlock::reason_str(bool pad, unsigned reason) {
    return reasonString(pad, reason);
}

#endif
