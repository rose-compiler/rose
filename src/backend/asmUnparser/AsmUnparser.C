#include "sage3basic.h"
#include "AsmUnparser.h"
#include "AsmUnparser_compat.h" /*FIXME: needed until no longer dependent upon unparseInstruction()*/

void
AsmUnparser::unparse(std::ostream &o, SgAsmInstruction *insn)
{
    pre(o, insn);

    if (insn_show_bytes) {
        HexdumpFormat fmt;
        fmt.width = 6;                  /* bytes per line of output */
        fmt.pad_chars = true;           /* character output should be padded */

        SgAsmExecutableFileFormat::hexdump(o, insn->get_address(), &(insn->get_raw_bytes()[0]), insn->get_raw_bytes().size(), fmt);
    } else {
        o <<StringUtility::addrToString(insn->get_address()) <<":";
    }
    o <<"   " <<unparseInstruction(insn);
    if (!insn->has_effect())
        o <<"; no-effect";
    if (!insn->get_comment().empty())
        o << "/* " <<insn->get_comment() << "*/";
    if (insn_linefeed)
        o <<"\n";

    post(o, insn);
}

void
AsmUnparser::unparse(std::ostream &o, SgAsmBlock *blk)
{
    std::string blk_addrstr = StringUtility::addrToString(blk->get_address());

    /* The instructions in this block (do not traverse deeper) */
    const SgAsmStatementPtrList &stmts = blk->get_statementList();
    std::vector<SgAsmInstruction*> insns;
    insns.reserve(stmts.size());
    for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
        if (isSgAsmInstruction(*si))
            insns.push_back(isSgAsmInstruction(*si));
    }

    if (!stmts.empty() && insns.empty()) {
        /* This must be a "global block" containing function declarations. */
        for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
            if (isSgAsmFunctionDeclaration(*si))
                unparse(o, isSgAsmFunctionDeclaration(*si));
        }

    } else {
        pre(o, blk); /*only for SgAsmBlock nodes that are basic blocks*/

        /* Remove no-op sequences from the listing. */
        std::vector<bool> is_noop(insns.size(), false);
        if (blk_detect_noop_seq || blk_remove_noop_seq || blk_show_noop_seq) {
            typedef std::vector<std::pair<size_t, size_t> > NoopSequences; /* array of index,size pairs */
            NoopSequences noops = insns.front()->find_noop_subsequences(insns, true);
            if (!noops.empty()) {
                if (blk_show_noop_seq) o <<"Noops:";
                for (NoopSequences::iterator ni=noops.begin(); ni!=noops.end(); ++ni) {
                    if (blk_show_noop_seq) o <<" (" <<(*ni).first <<"," <<(*ni).second <<")";
                    for (size_t i=0; i<(*ni).second; ++i)
                        is_noop[(*ni).first+i] = true;
                }
                if (blk_show_noop_seq) o <<"\n";
                if (blk_remove_noop_seq && blk_show_noop_warning) {
                    size_t nerased = 0;
                    for (size_t i=0; i<insns.size(); ++i)
                        if (is_noop[i]) ++nerased;
                    if (nerased)
                        o <<StringUtility::addrToString(blk->get_address()) <<": omitting "
                          <<nerased <<" no-op instruction" <<(1==nerased?"":"s") <<" from this block...\n";
                }
            }
        }

        /* The instructions */
        for (size_t i=0; i<insns.size(); i++) {
            insn_is_noop_seq = is_noop[i];
            if (!blk_remove_noop_seq || !insn_is_noop_seq)
                unparse(o, insns[i]);
        }

        if (blk_show_successors) {
            /* Show cached block successors. These are the successors that were probably cached by the instruction
             * partitioner, which does fairly extensive analysis -- definitely more than just looking at the last
             * instruction of the block! */
            o <<"            (successors:";
            const SgAddressList &sucs = blk->get_cached_successors();
            for (SgAddressList::const_iterator si=sucs.begin(); si!=sucs.end(); ++si)
                o <<" " <<StringUtility::addrToString(*si);
            if (!blk->get_complete_successors())
                o <<"...";
            o <<")\n";
        }
        post(o, blk);   /* only for SgAsmBlock nodes that are basic blocks */
    }
}

void
AsmUnparser::unparse(std::ostream &o, SgAsmFunctionDeclaration *func)
{
    pre(o, func);

    if (func_show_title) {
        o <<StringUtility::addrToString(func->get_entry_va())
          <<": ============================ Function (" <<func->reason_str(false) <<")";
        if (func->get_name().size()>0) {
            o <<" <" <<func->get_name() <<">";
        } else {
            o <<" unknown name";
        }
        o <<"\n";
    }

    const SgAsmStatementPtrList stmts = func->get_statementList();
    ROSE_ASSERT(!stmts.empty());
    for (size_t i=0; i<stmts.size(); ++i) {
        SgAsmBlock *blk = isSgAsmBlock(stmts[i]);
        ROSE_ASSERT(blk!=NULL);
        unparse(o, blk);
        if (i+1<stmts.size()) o <<"\n";
    }

    post(o, func);
}

void
AsmUnparser::unparse(std::ostream &o, SgAsmInterpretation *interp)
{
    pre(o, interp);

    if (interp_show_title) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        if (1==headers.size()) {
            o <<"/* Interpretation " <<headers[0]->format_name() <<" */\n";
        } else {
            o <<"/* Interpretation including:\n";
            for (size_t i=0; i<headers.size(); i++)
                o <<" *    " <<headers[i]->format_name() <<" from " <<headers[i]->get_file()->get_name() <<"\n";
            o <<" */\n";
        }
    }

    SgAsmBlock *global = interp->get_global_block();
    if (global) {
        const SgAsmStatementPtrList stmts = global->get_statementList();
        for (size_t i=0; i<stmts.size(); ++i) {
            SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(stmts[i]);
            ROSE_ASSERT(func!=NULL);
            unparse(o, func);
            if (i+1<stmts.size()) o <<"\n";
        }
    }

    post(o, interp);
}
