/* SgAsmFunction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "stringify.h"

#include "rosePublicConfig.h"
#ifdef ROSE_HAVE_LIBGCRYPT
#include <gcrypt.h>
#endif

using namespace Rose;

std::string
SgAsmFunction::reason_key(const std::string &prefix)
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
    return reason_str(do_pad, get_reason());
}

std::string
SgAsmFunction::reason_str(bool do_pad, unsigned r)
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
SgAsmFunction::get_extent(AddressIntervalSet *extents, rose_addr_t *lo_addr, rose_addr_t *hi_addr, NodeSelector *selector)
{
    struct T1: public AstSimpleProcessing {
        AddressIntervalSet *extents;
        rose_addr_t *lo_addr, *hi_addr;
        NodeSelector *selector;
        size_t nnodes;
        T1(AddressIntervalSet *extents, rose_addr_t *lo_addr, rose_addr_t *hi_addr, NodeSelector *selector)
            : extents(extents), lo_addr(lo_addr), hi_addr(hi_addr), selector(selector), nnodes(0) {
            if (lo_addr)
                *lo_addr = 0;
            if (hi_addr)
                *hi_addr = 0;
        }
        void visit(SgNode *node) {
            if (selector && !(*selector)(node))
                return;
            SgAsmInstruction *insn = isSgAsmInstruction(node);
            SgAsmStaticData *data = isSgAsmStaticData(node);
            rose_addr_t lo, hi;
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
        }
    } t1(extents, lo_addr, hi_addr, selector);
    t1.traverse(this, preorder);
    return t1.nnodes;
}

bool
SgAsmFunction::get_sha1(uint8_t digest[20], NodeSelector *selector)
{
#ifdef ROSE_HAVE_LIBGCRYPT
    struct T1: public AstSimpleProcessing {
        NodeSelector *selector;
        gcry_md_hd_t md; // message digest
        T1(NodeSelector *selector): selector(selector) {
            gcry_error_t error __attribute__((unused)) = gcry_md_open(&md, GCRY_MD_SHA1, 0);
            assert(GPG_ERR_NO_ERROR==error);
        }
        ~T1() {
            gcry_md_close(md);
        }
        void visit(SgNode *node) {
            if (selector && !(*selector)(node))
                return;
            SgAsmInstruction *insn = isSgAsmInstruction(node);
            SgAsmStaticData *data = isSgAsmStaticData(node);
            if (insn) {
                SgUnsignedCharList buf = insn->get_raw_bytes();
                gcry_md_write(md, &buf[0], buf.size());
            } else if (data) {
                SgUnsignedCharList buf = data->get_raw_bytes();
                gcry_md_write(md, &buf[0], buf.size());
            }
        }
        void read(uint8_t digest[20]) {
            assert(gcry_md_get_algo_dlen(GCRY_MD_SHA1)==20);
            gcry_md_final(md);
            unsigned char *d = gcry_md_read(md, GCRY_MD_SHA1);
            assert(d!=NULL);
            memcpy(digest, d, 20);
        }
    } t1(selector);
    t1.traverse(this, preorder);
    t1.read(digest);
    return true;
#else
    memset(digest, 0, 20);
    return false;
#endif
}

SgAsmBlock *
SgAsmFunction::get_entry_block() const {
    for (SgAsmStatementPtrList::const_iterator si=p_statementList.begin(); si!=p_statementList.end(); ++si) {
        SgAsmBlock *bb = isSgAsmBlock(*si);
        if (bb && bb->get_address()==p_entry_va)
            return bb;
    }
    return NULL;
}

void
SgAsmFunction::append_statement( SgAsmStatement* statement )
   {
     p_statementList.push_back(statement);
   }

void
SgAsmFunction::remove_statement( SgAsmStatement* statement )
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
