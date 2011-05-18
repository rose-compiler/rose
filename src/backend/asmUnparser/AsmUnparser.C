#include "sage3basic.h"
#include "AsmUnparser.h"
#include "AsmUnparser_compat.h" /*FIXME: needed until no longer dependent upon unparseInstruction()*/

/** Returns a vector of booleans indicating whether an instruction is part of a no-op sequence.  The sequences returned by
 *  SgAsmInstruction::find_noop_subsequences() can overlap, but we cannot assume that removing overlapping sequences will
 *  result in a meaningful basic block.  For instance, consider the following block:
 *
 *  \code
 *      1: mov eax, 1
 *      2: mov eax, 2
 *      3: mov eax, 1
 *      4: mov eax, 2
 *  \endcode
 *
 *  The subsequences <2,3> and <3,4> are both no-ops when considered independently.  However, we cannot remove all four
 *  instructions because the sequence <1,2,3,4> is not a no-op.
 *
 *  Therefore, this function takes the list returned by find_noop_subsequences and greedily selects the longest non-overlapping
 *  sequences, and returns a vector indexed by instruction position and containing a boolean to indicate whether that
 *  instruction is part of a selected no-op sequence.  Note that this algorithm does not necessarily maximize the number of
 *  no-op instructions. */
static std::vector<bool>
build_noop_index(const std::vector <std::pair <size_t, size_t> > &noops)
{
    /* Sort subsequences into buckets by length */
    size_t retval_size = 0;
    std::map<size_t/*size*/, std::vector <size_t/*offset*/> > sorted;
    for (std::vector<std::pair<size_t, size_t> >::const_iterator ni=noops.begin(); ni!=noops.end(); ++ni) {
        sorted[(*ni).second].push_back((*ni).first);
        retval_size = std::max(retval_size, (*ni).first + (*ni).second);
    }
    
    /* Allocate a return value */
    if (0==retval_size) {
        std::vector<bool> empty;
        return empty;
    }
    std::vector<bool> retval(retval_size, false);

    /* Process in order from largest to smallest */
    for (std::map<size_t, std::vector<size_t> >::reverse_iterator szi=sorted.rbegin(); szi!=sorted.rend(); ++szi) {
        size_t sz = (*szi).first;
        for (std::vector<size_t>::const_iterator idxi=(*szi).second.begin(); idxi<(*szi).second.end(); ++idxi) {
            size_t idx = *idxi;
            
            /* Are any instructions in this range already marked as no-ops?  If so, then skip this one. */
            bool overlaps = false;
            for (size_t i=0; i<sz && !overlaps; ++i)
                overlaps = retval[idx+i];
            if (overlaps)
                continue;
            
            /* Mark these instructions as no-ops */
            for (size_t i=0; i<sz; ++i)
                retval[idx+i] = true;
        }
    }
    
    return retval;
}

void
AsmUnparser::init()
{
    insn_callbacks.pre
        //.append(&insnAddress)                 /* Using insnRawBytes instead, which also prints addresses. */
        .append(&insnRawBytes);
    insn_callbacks.unparse
        .append(&insnBody);
    insn_callbacks.post
        .append(&insnNoEffect)
        .append(&insnComment)
        .append(&insnLineTermination);

    //basicblock_callbacks.pre
        //.append(&basicBlockNoopUpdater)       /* Disabled by default for speed. */
        //.append(&basicBlockNoopWarning);      /* No-op if basicBlockNoopUpdater isn't used. */
    basicblock_callbacks.unparse
        .append(&basicBlockBody);
    basicblock_callbacks.post
        .append(&basicBlockSuccessors)
        .append(&basicBlockLineTermination)
        .append(&basicBlockCleanup);

    function_callbacks.pre
        .append(&functionEntryAddress)
        .append(&functionSeparator)
        .append(&functionReasons)
        .append(&functionName)
        .append(&functionLineTermination);
    function_callbacks.unparse
        .append(&functionBody);

    interp_callbacks.pre
        .append(&interpName);
    interp_callbacks.unparse
        .append(&interpBody);
}


void
AsmUnparser::add_function_labels(SgNode *node)
{
    struct T1: public SgSimpleProcessing {
        AsmUnparser *unparser;
        T1(AsmUnparser *unparser)
            : unparser(unparser) {}
        void visit(SgNode *node) {
            SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
            if (func)
                unparser->labels[func->get_entry_va()] = func->get_name();
        }
    } traversal(this);
    traversal.traverse(node, preorder);
};

bool
AsmUnparser::is_unparsable_node(SgNode *node)
{
    if (isSgAsmFunctionDeclaration(node) || isSgAsmInstruction(node) || isSgAsmInterpretation(node))
        return true;

    SgAsmBlock *block = isSgAsmBlock(node);
    if (block!=NULL) {
        const SgAsmStatementPtrList &stmts = block->get_statementList();
        if (!stmts.empty() && isSgAsmInstruction(stmts.front()))
            return true;
    }

    return false;
}

SgNode *
AsmUnparser::find_unparsable_node(SgNode *ast)
{
    SgNode *root = NULL;
    try {
        struct T1: public SgSimpleProcessing {
            AsmUnparser *unparser;
            T1(AsmUnparser *unparser): unparser(unparser) {}
            void visit(SgNode *node) {
                if (unparser->is_unparsable_node(node))
                    throw node;
            }
        };
        T1(this).traverse(ast, preorder);
    } catch (SgNode *node) {
        root = node;
    }
    return root;
}

std::vector<SgNode*>
AsmUnparser::find_unparsable_nodes(SgNode *ast)
{
    struct T1: public AstPrePostProcessing {
        AsmUnparser *unparser;
        SgNode *ignore;
        std::vector<SgNode*> found;
        T1(AsmUnparser *unparser): unparser(unparser), ignore(NULL) {}
        void preOrderVisit(SgNode *node) {
            if (!ignore && unparser->is_unparsable_node(node)) {
                found.push_back(node);
                ignore = node;
            }
        }
        void postOrderVisit(SgNode *node) {
            if (ignore==node)
                ignore = NULL;
        }
    } t1(this);
    t1.traverse(ast);
    return t1.found;
}

/******************************************************************************************************************************
 *                                      Main unparsing functions
 ******************************************************************************************************************************/

size_t
AsmUnparser::unparse(std::ostream &output, SgNode *ast)
{
    std::vector<SgNode*> unparsable = find_unparsable_nodes(ast);
    for (std::vector<SgNode*>::iterator ui=unparsable.begin(); ui!=unparsable.end(); ++ui) {
        SgNode *node = *ui;

        SgAsmInstruction *insn = isSgAsmInstruction(node);
        if (insn) {
            unparse_insn(true, output, insn, (size_t)(-1));
            continue;
        }

        SgAsmBlock *block = isSgAsmBlock(node);
        if (block) {
            unparse_basicblock(true, output, block);
            continue;
        }

        SgAsmFunctionDeclaration *func = isSgAsmFunctionDeclaration(node);
        if (func) {
            unparse_function(true, output, func);
            continue;
        }

        SgAsmInterpretation *interp = isSgAsmInterpretation(node);
        if (interp) {
            unparse_interpretation(true, output, interp);
            continue;
        }
    }
    return unparsable.size();
}

bool
AsmUnparser::unparse_insn(bool enabled, std::ostream &output, SgAsmInstruction *insn, size_t position_in_block)
{
    UnparserCallback::InsnArgs args(this, output, insn, position_in_block);
    enabled = insn_callbacks.pre    .apply(enabled, args);
    enabled = insn_callbacks.unparse.apply(enabled, args);
    enabled = insn_callbacks.post   .apply(enabled, args);
    return enabled;
}

bool
AsmUnparser::unparse_basicblock(bool enabled, std::ostream &output, SgAsmBlock *block)
{
    std::vector<SgAsmInstruction*> insns;
    const SgAsmStatementPtrList &stmts = block->get_statementList();
    for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
        if (isSgAsmInstruction(*si))
            insns.push_back(isSgAsmInstruction(*si));
    }

    UnparserCallback::BasicBlockArgs args(this, output, block, insns);
    enabled = basicblock_callbacks.pre    .apply(enabled, args);
    enabled = basicblock_callbacks.unparse.apply(enabled, args);
    enabled = basicblock_callbacks.post   .apply(enabled, args);
    return enabled;
}

bool
AsmUnparser::unparse_function(bool enabled, std::ostream &output, SgAsmFunctionDeclaration *func)
{
    UnparserCallback::FunctionArgs args(this, output, func);
    enabled = function_callbacks.pre    .apply(enabled, args);
    enabled = function_callbacks.unparse.apply(enabled, args);
    enabled = function_callbacks.post   .apply(enabled, args);
    return enabled;
}

bool
AsmUnparser::unparse_interpretation(bool enabled, std::ostream &output, SgAsmInterpretation *interp)
{
    UnparserCallback::InterpretationArgs args(this, output, interp);
    enabled = interp_callbacks.pre    .apply(enabled, args);
    enabled = interp_callbacks.unparse.apply(enabled, args);
    enabled = interp_callbacks.post   .apply(enabled, args);
    return enabled;
}



/******************************************************************************************************************************
 *                                      Instruction callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::InsnAddress::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled)
        args.output <<StringUtility::addrToString(args.insn->get_address()) <<":";
    return enabled;
}

bool
AsmUnparser::InsnRawBytes::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled)
        SgAsmExecutableFileFormat::hexdump(args.output, args.insn->get_address(), &(args.insn->get_raw_bytes()[0]),
                                           args.insn->get_raw_bytes().size(), fmt);
    return enabled;
}

bool
AsmUnparser::InsnBody::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled)
        args.output <<"   " <<unparseInstruction(args.insn, &args.unparser->labels);
    return enabled;
}

bool
AsmUnparser::InsnNoEffect::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled &&
        args.position_in_block<args.unparser->insn_is_noop.size() &&
        args.unparser->insn_is_noop[args.position_in_block])
        args.output <<" !EFFECT";
    return enabled;
}

bool
AsmUnparser::InsnComment::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled && !args.insn->get_comment().empty())
        args.output <<" /* " <<args.insn->get_comment() <<"*/";
    return enabled;
}

bool
AsmUnparser::InsnLineTermination::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled)
        args.output <<std::endl;
    return enabled;
}

/******************************************************************************************************************************
 *                                      Basic block callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::BasicBlockNoopUpdater::operator()(bool enabled, const BasicBlockArgs &args)
{
    args.unparser->insn_is_noop.clear();
    if (enabled) {
        typedef std::vector<std::pair<size_t, size_t> > NoopSequences; /* array of index,size pairs */
        NoopSequences noops = args.insns.front()->find_noop_subsequences(args.insns, true, true);
        if (!noops.empty()) {
            args.unparser->insn_is_noop = build_noop_index(noops);
            if (debug) {
                args.output <<"No-effect sequences by (index, length):\n";
                for (NoopSequences::iterator ni=noops.begin(); ni!=noops.end(); ++ni)
                    args.output <<"    (" <<(*ni).first <<"," <<(*ni).second <<")\n";
            }
        }
    }
    return enabled;
}

bool
AsmUnparser::BasicBlockNoopWarning::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled && !args.unparser->insn_is_noop.empty()) {
        size_t nnoops = 0;
        for (size_t i=0; i<args.unparser->insn_is_noop.size(); i++) {
            if (args.unparser->insn_is_noop[i])
                nnoops++;
        }
        if (nnoops>0) {
            args.output <<StringUtility::addrToString(args.block->get_address()) <<": omitting "
                        <<nnoops <<" instruction" <<(1==nnoops?"":"s") <<" as no-op sequences from this block.\n";
        }
    }
    return enabled;
}

bool
AsmUnparser::BasicBlockBody::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled) {
        for (size_t i=0; i<args.insns.size(); i++)
            args.unparser->unparse_insn(enabled, args.output, args.insns[i], i);
    }
    return enabled;
}

bool
AsmUnparser::BasicBlockSuccessors::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled) {
        args.output <<"            (successors:";
        const SgAsmTargetPtrList &successors = args.block->get_successors();
        for (SgAsmTargetPtrList::const_iterator si=successors.begin(); si!=successors.end(); ++si)
            args.output <<" " <<StringUtility::addrToString((*si)->get_address());
        if (!args.block->get_successors_complete())
            args.output <<"...";
        args.output <<")\n";
    }
    return enabled;
}

bool
AsmUnparser::BasicBlockLineTermination::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled)
        args.output <<std::endl;
    return enabled;
}

bool
AsmUnparser::BasicBlockCleanup::operator()(bool enabled, const BasicBlockArgs &args)
{
    args.unparser->insn_is_noop.clear();
    return enabled;
}

/******************************************************************************************************************************
 *                                      Function callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::FunctionEntryAddress::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled)
        args.output <<StringUtility::addrToString(args.func->get_entry_va()) <<":";
    return enabled;
}

bool
AsmUnparser::FunctionSeparator::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled)
        args.output <<" ============================ Function";
    return enabled;
}

bool
AsmUnparser::FunctionReasons::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled)
        args.output <<" (" <<args.func->reason_str(false) <<")";
    return enabled;
}

bool
AsmUnparser::FunctionName::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled) {
        if (args.func->get_name().empty()) {
            args.output <<" unknown name";
        } else {
            args.output <<" <" <<args.func->get_name() <<">";
        }
    }
    return enabled;
}

bool
AsmUnparser::FunctionLineTermination::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled)
        args.output <<std::endl;
    return enabled;
}

bool
AsmUnparser::FunctionBody::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled) {
        const SgAsmStatementPtrList stmts = args.func->get_statementList();
        for (size_t i=0; i<stmts.size(); i++)
            args.unparser->unparse(args.output, stmts[i]);
    }
    return enabled;
}

/******************************************************************************************************************************
 *                                      Interpretation callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::InterpName::operator()(bool enabled, const InterpretationArgs &args)
{
    if (enabled) {
        const SgAsmGenericHeaderPtrList &headers = args.interp->get_headers()->get_headers();
        if (1==headers.size()) {
            args.output <<"/* Interpretation " <<headers[0]->format_name() <<" */\n";
        } else {
            args.output <<"/* Interpretation including:\n";
            for (size_t i=0; i<headers.size(); i++)
                args.output <<" *    " <<headers[i]->format_name() <<" from " <<headers[i]->get_file()->get_name() <<"\n";
            args.output <<" */\n";
        }
    }
    return enabled;
}

bool
AsmUnparser::InterpBody::operator()(bool enabled, const InterpretationArgs &args)
{
    if (enabled) {
        SgAsmBlock *global = args.interp->get_global_block();
        if (global) {
            const SgAsmStatementPtrList stmts = global->get_statementList();
            for (size_t i=0; i<stmts.size(); ++i)
                args.unparser->unparse(args.output, stmts[i]);
        }
    }
    return enabled;
}
