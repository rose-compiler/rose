#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmFunction.h>

#include <Rose/AST/Traversal.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <SgAsmBlock.h>
#include <SgAsmInstruction.h>
#include <SgAsmStaticData.h>
#include <Cxx_GrammarDowncast.h>

#include "stringify.h"
#include <ROSE_UNUSED.h>

#ifdef ROSE_HAVE_LIBGCRYPT
#include <gcrypt.h>
#include <boost/scope_exit.hpp>
#endif

using namespace Rose;
using namespace Rose::BinaryAnalysis;

std::string
SgAsmFunction::reason_key(const std::string &prefix)
{
    return reasonKey(prefix);
}

std::string
SgAsmFunction::reasonKey(const std::string &prefix)
{
    return (prefix + "E = entry address         H = CFG head             C = function call(*)\n" +
            prefix + "X = exception frame       T = thunk, t = target    I = imported/dyn-linked\n" +
            prefix + "O = exported              S = function symbol      P = instruction pattern\n" +
            prefix + "G = CFG graph analysis    U = user-def detection   N = NOP/zero padding\n" +
            prefix + "D = discontiguous blocks  V = intra-function block L = leftover blocks\n" +
            prefix + "Mxxx are miscellaneous reasons (at most one misc reason per function):\n" +
            prefix + "      M001 = code between function padding bytes\n" +
            prefix + "Note: \"c\" means this is the target of a call-like instruction or instruction\n" +
            prefix + "      sequence but the call is not present in the global control flow graph, while\n" +
            prefix + "      \"C\" means the call is in the CFG.\n");
}

std::string
SgAsmFunction::reason_str(bool do_pad) const
{
    return reasonString(do_pad);
}

std::string
SgAsmFunction::reasonString(bool do_pad) const
{
    return reasonString(do_pad, get_reason());
}

std::string
SgAsmFunction::reason_str(bool do_pad, unsigned r)
{
    return reasonString(do_pad, r);
}

std::string
SgAsmFunction::reasonString(bool do_pad, unsigned r)
{
    using namespace StringUtility; // for add_to_reason_string()
    std::string result;

    /* entry point and instruction heads are mutually exclusive, so we use the same column for both when padding. */
    if (r & FUNC_ENTRY_POINT) {
        add_to_reason_string(result, true, do_pad, "E", "entry point");
    } else {
        add_to_reason_string(result, (r & FUNC_INSNHEAD), do_pad, "H", "insn head");
    }

    /* Function call:
     *   "C" means the function was detected because we saw a call-like instructon (such as x86 CALL or FARCALL) or instruction
     *       sequence (such as pushing the return value and then branching) in code that was determined to be reachable by
     *       analyzing the control flow graph.
     *
     *   "c" means this function is the target of some call-like instruction (such as x86 CALL or FARCALL) but could not
     *       determine whether the instruction is actually executed.
     */
    if (r & FUNC_CALL_TARGET) {
        add_to_reason_string(result, true, do_pad, "C", "function call");
    } else {
        add_to_reason_string(result, (r & FUNC_CALL_INSN), do_pad, "c", "call instruction");
    }

    if (r & FUNC_EH_FRAME) {
        add_to_reason_string(result, true,                 do_pad, "X", "exception frame");
    } else {
        add_to_reason_string(result, (r & FUNC_THUNK),     do_pad, "T", "thunk");
    }
    add_to_reason_string(result, (r & FUNC_IMPORT),        do_pad, "I", "import");
    add_to_reason_string(result, (r & FUNC_EXPORT),        do_pad, "E", "export");
    add_to_reason_string(result, (r & FUNC_SYMBOL),        do_pad, "S", "symbol");
    add_to_reason_string(result, (r & FUNC_PATTERN),       do_pad, "P", "pattern");
    add_to_reason_string(result, (r & FUNC_GRAPH),         do_pad, "G", "graph");
    add_to_reason_string(result, (r & FUNC_USERDEF),       do_pad, "U", "user defined");
    add_to_reason_string(result, (r & FUNC_PADDING),       do_pad, "N", "padding");
    add_to_reason_string(result, (r & FUNC_DISCONT),       do_pad, "D", "discontiguous");
    add_to_reason_string(result, (r & FUNC_LEFTOVERS),     do_pad, "L", "leftovers");
    add_to_reason_string(result, (r & FUNC_INTRABLOCK),    do_pad, "V", "intrablock");
    add_to_reason_string(result, (r & FUNC_THUNK_TARGET),  do_pad, "t", "thunk target");

    /* The miscellaneous marker is special. It's a single letter like the others, but is followed by a fixed width
     * integer indicating the (user-defined) algorithm that added the function. */
    {
        char abbr[32], full[64];
        int width = snprintf(abbr, sizeof abbr, "%u", FUNC_MISCMASK);
        snprintf(abbr, sizeof abbr, "M%0*u", width, (r & FUNC_MISCMASK));
        abbr[sizeof(abbr)-1] = '\0';
        if (!do_pad) {
            std::string miscname = stringifySgAsmFunctionFunctionReason((r & FUNC_MISCMASK), "FUNC_");
            if (miscname.empty() || miscname[0]=='(') {
                snprintf(full, sizeof full, "misc-%u", (r & FUNC_MISCMASK));
            } else {
                for (size_t i=0; i<miscname.size(); ++i)
                    miscname[i] = tolower(miscname[i]);
                strncpy(full, miscname.c_str(), sizeof full);
            }
            full[sizeof(full)-1] = '\0';
        } else {
            full[0] = '\0';
        }
        add_to_reason_string(result, (r & FUNC_MISCMASK), do_pad, abbr, full);
    }

    return result;
}

size_t
SgAsmFunction::get_extent(AddressIntervalSet *extents, Address *lo_addr, Address *hi_addr, NodeSelector *selector) {
    if (lo_addr)
        *lo_addr = 0;
    if (hi_addr)
        *hi_addr = 0;
    size_t nnodes = 0;

    AST::Traversal::forwardPre<SgNode>(this, [extents, lo_addr, hi_addr, selector, &nnodes](SgNode *node) {
        if (selector && !(*selector)(node))
            return;
        SgAsmInstruction *insn = isSgAsmInstruction(node);
        SgAsmStaticData *data = isSgAsmStaticData(node);
        Address lo, hi;
        if (insn) {
            lo = insn->get_address();
            hi = lo + insn->get_size();
        } else if (data) {
            lo = data->get_address();
            hi = lo + data->get_size();
        } else {
            return;
        }

        if (0==nnodes++) {
            if (lo_addr)
                *lo_addr = lo;
            if (hi_addr)
                *hi_addr = hi;
        } else {
            if (lo_addr)
                *lo_addr = std::min(*lo_addr, lo);
            if (hi_addr)
                *hi_addr = std::max(*hi_addr, hi);
        }
        if (extents && hi>lo)
            extents->insert(AddressInterval::baseSize(lo, hi-lo));
    });

    return nnodes;
}

bool
SgAsmFunction::get_sha1(uint8_t digest[20], NodeSelector *selector)
{
#ifdef ROSE_HAVE_LIBGCRYPT
    gcry_md_hd_t md; // message digest
    gcry_error_t error = gcry_md_open(&md, GCRY_MD_SHA1, 0);
    ASSERT_always_require(GPG_ERR_NO_ERROR == error);
    BOOST_SCOPE_EXIT(md) {
        gcry_md_close(md);
    } BOOST_SCOPE_EXIT_END;

    AST::Traversal::forwardPre<SgNode>(this, [selector, &md](SgNode *node) {
        if (selector && !(*selector)(node)) {
            return;
        } else if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
            SgUnsignedCharList buf = insn->get_rawBytes();
            gcry_md_write(md, &buf[0], buf.size());
        } else if (SgAsmStaticData *data = isSgAsmStaticData(node)) {
            SgUnsignedCharList buf = data->get_rawBytes();
            gcry_md_write(md, &buf[0], buf.size());
        }
    });

    ASSERT_require(gcry_md_get_algo_dlen(GCRY_MD_SHA1) == 20);
    gcry_md_final(md);
    unsigned char *d = gcry_md_read(md, GCRY_MD_SHA1);
    ASSERT_not_null(d);
    memcpy(digest, d, 20);
    return true;
#else
    ROSE_UNUSED(selector);
    memset(digest, 0, 20);
    return false;
#endif
}

SgAsmBlock *
SgAsmFunction::get_entry_block() const {
    return get_entryBlock();
}

SgAsmBlock *
SgAsmFunction::get_entryBlock() const {
    for (SgAsmStatementPtrList::const_iterator si=p_statementList.begin(); si!=p_statementList.end(); ++si) {
        SgAsmBlock *bb = isSgAsmBlock(*si);
        if (bb && bb->get_address()==get_entryVa())
            return bb;
    }
    return NULL;
}

void
SgAsmFunction::append_statement( SgAsmStatement* statement ) {
    appendStatement(statement);
}

void
SgAsmFunction::appendStatement( SgAsmStatement* statement )
   {
     p_statementList.push_back(statement);
   }

void
SgAsmFunction::remove_statement( SgAsmStatement* statement ) {
    removeStatement(statement);
}

void
SgAsmFunction::removeStatement( SgAsmStatement* statement )
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

SgAsmFunction::function_kind_enum
SgAsmFunction::get_function_kind() const {
    return get_functionKind();
}

void
SgAsmFunction::set_function_kind(SgAsmFunction::function_kind_enum x) {
    set_functionKind(x);
}

SgAsmFunction::MayReturn
SgAsmFunction::get_may_return() const {
    return get_mayReturn();
}

void
SgAsmFunction::set_may_return(MayReturn x) {
    set_mayReturn(x);
}

const std::string&
SgAsmFunction::get_name_md5() const {
    return get_nameMd5();
}

void
SgAsmFunction::set_name_md5(const std::string &x) {
    set_nameMd5(x);
}

Address
SgAsmFunction::get_entry_va() const {
    return get_entryVa();
}

void
SgAsmFunction::set_entry_va(Address x) {
    set_entryVa(x);
}

SgSymbolTable*
SgAsmFunction::get_symbol_table() const {
    return get_symbolTable();
}

void
SgAsmFunction::set_symbol_table(SgSymbolTable *x) {
    return set_symbolTable(x);
}

size_t
SgAsmFunction::get_cached_vertex() const {
    return get_cachedVertex();
}

void
SgAsmFunction::set_cached_vertex(size_t x) {
    set_cachedVertex(x);
}

#endif
