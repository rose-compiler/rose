#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "AsmUnparser.h"

#include "AsmUnparser_compat.h" /*FIXME: needed until no longer dependent upon unparseInstruction()*/
#include "Disassembler.h"

namespace Rose {
namespace BinaryAnalysis {

using namespace Diagnostics;

Sawyer::Message::Facility AsmUnparser::mlog;

/* Returns a vector of booleans indicating whether an instruction is part of a no-op sequence.  The sequences returned by
 * SgAsmInstruction::findNoopSubsequences() can overlap, but we cannot assume that removing overlapping sequences will
 * result in a meaningful basic block.  For instance, consider the following block:
 *
 *      1: mov eax, 1
 *      2: mov eax, 2
 *      3: mov eax, 1
 *      4: mov eax, 2
 *
 * The subsequences <2,3> and <3,4> are both no-ops when considered independently.  However, we cannot remove all four
 * instructions because the sequence <1,2,3,4> is not a no-op.
 *
 * Therefore, this function takes the list returned by findNoopSubsequences and greedily selects the longest non-overlapping
 * sequences, and returns a vector indexed by instruction position and containing a boolean to indicate whether that
 * instruction is part of a selected no-op sequence.  Note that this algorithm does not necessarily maximize the number of
 * no-op instructions. */
static std::vector<bool>
build_noop_index(const std::vector <std::pair <size_t /*offset*/, size_t /*size*/> > &noops) {
    typedef std::vector<std::pair<size_t /*offset*/, size_t /*size*/> > Unsorted;
    typedef std::map<size_t/*size*/, std::vector <size_t/*offset*/> > Sorted;

    /* Sort subsequences into buckets by length */
    size_t retval_size = 0;
    Sorted sorted;
    for (Unsorted::const_iterator ni = noops.begin(); ni != noops.end(); ++ni) {
        sorted[ni->second /*size*/].push_back(ni->first /*offset*/);
        retval_size = std::max(retval_size, ni->first /*offset*/ + ni->second /*size*/);
    }
    
    /* Allocate a return value */
    if (0==retval_size) {
        std::vector<bool> empty;
        return empty;
    }
    std::vector<bool> retval(retval_size, false);

    /* Process in order from largest to smallest */
    for (Sorted::reverse_iterator szi = sorted.rbegin(); szi != sorted.rend(); ++szi) {
        size_t sz = szi->first;
        BOOST_FOREACH (size_t idx, szi->second) {
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

// class method
void AsmUnparser::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::AsmUnparser");
        mlog.comment("generating assembly listings (vers 1)");
    }
}

void
AsmUnparser::init()
{
    organization = ORGANIZED_BY_AST;

    insn_callbacks.pre
        .append(&insnBlockSeparation)           /* used only for ORGANIZED_BY_ADDRESS */
        .append(&insnSkipBackBegin)             /* used only for ORGANIZED_BY_ADDRESS */
        .append(&insnFuncEntry)                 /* used only for ORGANIZED_BY_ADDRESS */
        //.append(&insnAddress)                 /* Using insnRawBytes instead, which also prints addresses. */
        .append(&insnRawBytes)
        .append(&insnBlockEntry)                /* used only for ORGANIZED_BY_ADDRESS */
        .append(&insnStackDelta);
    insn_callbacks.unparse
        .append(&insnBody);
    insn_callbacks.post
        .append(&insnNoEffect)
        .append(&insnComment)
        .append(&insnLineTermination)
        .append(&insnSkipBackEnd);              /* used only for ORGANIZED_BY_ADDRESS */

    basicblock_callbacks.pre
        //.append(&basicBlockNoopUpdater)       /* Disabled by default for speed. */
        //.append(&basicBlockNoopWarning)       /* No-op if basicBlockNoopUpdater isn't used. */
        .append(&basicBlockReasons)
        .append(&basicBlockPredecessors);
    basicblock_callbacks.unparse
        .append(&basicBlockBody);               /* used only for ORGANIZED_BY_AST */
    basicblock_callbacks.post
        .append(&basicBlockOutgoingStackDelta)
        .append(&basicBlockSuccessors)
        .append(&basicBlockLineTermination)
        .append(&basicBlockCleanup);

    staticdata_callbacks.pre
        .append(&staticDataBlockSeparation)     /* used only for ORGANIZED_BY_ADDRESS */
        .append(&staticDataSkipBackBegin)       /* used only for ORGANIZED_BY_ADDRESS */
        .append(&staticDataRawBytes)
        .append(&staticDataBlockEntry);         /* used only for ORGANIZED_BY_ADDRESS */
    staticdata_callbacks.unparse
        .append(&staticDataDetails)
        .append(&staticDataComment);
    staticdata_callbacks.post
        .append(&staticDataLineTermination)
        .append(&staticDataDisassembler)
        .append(&staticDataSkipBackEnd);        /* used only for ORGANIZED_BY_ADDRESS */

    datablock_callbacks.unparse
        .append(&dataBlockBody)                 /* used only for ORGANIZED_BY_AST */
        .append(&dataBlockLineTermination);

    function_callbacks.pre
        .append(&functionEntryAddress)
        .append(&functionSeparator)
        .append(&functionReasons)
        .append(&functionName)
        .append(&functionLineTermination)
        .append(&functionComment)
        .append(&functionPredecessors)
        .append(&functionSuccessors)
        .append(&functionAttributes)
        .append(&functionLineTermination);
    function_callbacks.unparse
        .append(&functionBody);                 /* used only for ORGANIZED_BY_AST */

    interp_callbacks.pre
        .append(&interpName);
    interp_callbacks.unparse
        .append(&interpBody);
}

const RegisterDictionary *
AsmUnparser::get_registers() const
{
    return user_registers ? user_registers : interp_registers;
}

void
AsmUnparser::add_function_labels(SgNode *node)
{
    struct T1: public SgSimpleProcessing {
        AsmUnparser *unparser;
        T1(AsmUnparser *unparser)
            : unparser(unparser) {}
        void visit(SgNode *node) {
            SgAsmFunction *func = isSgAsmFunction(node);
            if (func)
                unparser->labels[func->get_entry_va()] = func->get_name();
        }
    } traversal(this);
    traversal.traverse(node, preorder);
};

void
AsmUnparser::add_control_flow_graph(const BinaryAnalysis::ControlFlow::Graph &cfg)
{
    // Control flow graph
    this->cfg = cfg;
    {
        cfg_blockmap.clear();
        boost::graph_traits<CFG>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=vertices(cfg); vi!=vi_end; ++vi) {
            SgAsmBlock *blk = get(boost::vertex_name, cfg, *vi);
            if (blk)
                cfg_blockmap[blk] = *vi;
        }
    }

    // Function call graph
    BinaryAnalysis::FunctionCall().build_cg_from_cfg(cfg, cg);
    {
        cg_functionmap.clear();
        boost::graph_traits<CG>::vertex_iterator vi, vi_end;
        for (boost::tie(vi, vi_end)=vertices(cg); vi!=vi_end; ++vi) {
            SgAsmFunction *func = get(boost::vertex_name, cg, *vi);
            if (func)
                cg_functionmap[func] = *vi;
        }
    }
}
    

bool
AsmUnparser::is_unparsable_node(SgNode *node)
{
    if (isSgAsmFunction(node) || isSgAsmInstruction(node) || isSgAsmStaticData(node) || isSgAsmInterpretation(node))
        return true;

    SgAsmBlock *block = isSgAsmBlock(node);
    if (block!=NULL) {
        const SgAsmStatementPtrList &stmts = block->get_statementList();
        if (!stmts.empty() &&
            (isSgAsmInstruction(stmts.front()) || isSgAsmStaticData(stmts.front())))
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

void
AsmUnparser::start_of_object(rose_addr_t va, std::ostream &output)
{
    if (skipback.active && ORGANIZED_BY_ADDRESS==organization) {
        if (skipback.triggered) {
            if (va > skipback.va) {
                rose_addr_t nskipped = va - skipback.va;
                output <<"Skipping " <<nskipped <<" byte" <<(1==nskipped?"":"s") <<"\n";
            } else if (va < skipback.va) {
                rose_addr_t nback = skipback.va - va;
                output <<"Backward " <<nback <<" byte" <<(1==nback?"":"s") <<"\n";
            }
        }
        skipback.triggered = true;
    }
}

void
AsmUnparser::end_of_object(rose_addr_t va)
{
    if (skipback.active && ORGANIZED_BY_ADDRESS==organization) {
        skipback.triggered = true;
        skipback.va = va;
    }
}

std::string
AsmUnparser::line_prefix() const
{
    char buf[256];
    int nprint = snprintf(buf, sizeof buf, lineprefix.format.c_str(), lineprefix.address);
    if (nprint<0 || (size_t)nprint >= sizeof buf)
        strcpy(buf, "<OVERFLOW> ");
    return buf;
}

std::string
AsmUnparser::invalid_register(SgAsmInstruction *insn, RegisterDescriptor reg, const RegisterDictionary *regdict)
{
    using namespace StringUtility;
    Stream warn(mlog[WARN]);

    std::string regstr = numberToString(reg.majorNumber()) + "." + numberToString(reg.minorNumber()) + "." +
                         numberToString(reg.offset()) + "." + numberToString(reg.nBits());
    if (insn) {
        warn <<"invalid register reference " <<regstr <<" at va " <<addrToString(insn->get_address()) <<"\n";
    } else {
        warn <<"invalid register reference " <<regstr <<"\n";
    }

    static bool wasDescribed = false;
    if (!wasDescribed) {
        warn <<"  This warning is caused by instructions using registers that don't have names in the\n"
             <<"  register dictionary.  The register dictionary used during unparsing comes from either\n"
             <<"  the explicitly specified dictionary (see AsmUnparser::set_registers()) or the dictionary\n"
             <<"  associated with the SgAsmInterpretation being unparsed.  The interpretation normally\n"
             <<"  chooses a dictionary based on the architecture specified in the file header. For example,\n"
             <<"  this warning may be caused by a file whose header specifies i386 but the instructions in\n"
             <<"  the file are for the amd64 architecture.  The assembly listing will indicate unnamed\n"
             <<"  registers with the notation \"BAD_REGISTER(a.b.c.d)\" where \"a\" and \"b\" are the major\n"
             <<"  and minor numbers for the register, \"c\" is the bit offset within the underlying machine\n"
             <<"  register, and \"d\" is the number of significant bits.\n";
        if (regdict!=NULL)
            warn <<"  Dictionary in use at time of warning: " <<regdict->get_architecture_name() <<"\n";
        wasDescribed = true;
    }
    return "BAD_REGISTER(" + regstr + ")";
}



/******************************************************************************************************************************
 *                                      Main unparsing functions
 ******************************************************************************************************************************/

std::string
AsmUnparser::to_string(SgNode *ast)
{
    std::ostringstream ss;
    unparse(ss, ast);
    return ss.str();
}

size_t
AsmUnparser::unparse(std::ostream &output, SgNode *ast)
{
    size_t retval = 0;

    switch (get_organization()) {
        case ORGANIZED_BY_AST: {
            std::vector<SgNode*> unparsable = find_unparsable_nodes(ast);
            for (std::vector<SgNode*>::iterator ui=unparsable.begin(); ui!=unparsable.end(); ++ui)
                unparse_one_node(output, *ui);
            retval = unparsable.size();
            break;
        }


        case ORGANIZED_BY_ADDRESS: {
            typedef std::multimap<rose_addr_t, SgAsmStatement*> StatementMap;
            struct GatherStatements: public AstSimpleProcessing {
                StatementMap *stmts;
                GatherStatements(StatementMap *stmts): stmts(stmts) {}
                void visit(SgNode *node) {
                    if (isSgAsmInstruction(node) || isSgAsmStaticData(node)) {
                        SgAsmStatement *stmt = isSgAsmStatement(node);
                        assert(stmt!=NULL);
                        stmts->insert(std::make_pair(stmt->get_address(), stmt));
                    }
                }
            };
            StatementMap stmts;
            GatherStatements(&stmts).traverse(ast, preorder);
            for (StatementMap::iterator si=stmts.begin(); si!=stmts.end(); ++si)
                unparse_one_node(output, si->second);
            retval = stmts.size();
            break;
        }

        // no default since it would prevent compiler warnings
    }
    return retval;
}

bool
AsmUnparser::unparse_one_node(std::ostream &output, SgNode *node)
{
    SgAsmInstruction *insn = isSgAsmInstruction(node);
    if (insn)
        return unparse_insn(true, output, insn, INVALID_INDEX);

    SgAsmStaticData *data = isSgAsmStaticData(node);
    if (data)
        return unparse_staticdata(true, output, data, INVALID_INDEX);

    SgAsmBlock *block = isSgAsmBlock(node);
    if (block) {
        const SgAsmStatementPtrList &stmts = block->get_statementList();
        assert(!stmts.empty());
        if (isSgAsmInstruction(stmts.front())) {
            return unparse_basicblock(true, output, block);
        } else if (isSgAsmStaticData(stmts.front())) {
            return unparse_datablock(true, output, block);
        }
    }

    SgAsmFunction *func = isSgAsmFunction(node);
    if (func)
        return unparse_function(true, output, func);

    SgAsmInterpretation *interp = isSgAsmInterpretation(node);
    if (interp)
        return unparse_interpretation(true, output, interp);

    return false;
}

bool
AsmUnparser::unparse_insn(bool enabled, std::ostream &output, SgAsmInstruction *insn, size_t position_in_block)
{
    set_prefix_address(insn->get_address());
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

    set_prefix_address(block->get_address());
    UnparserCallback::BasicBlockArgs args(this, output, block, insns);
    enabled = basicblock_callbacks.pre    .apply(enabled, args);
    enabled = basicblock_callbacks.unparse.apply(enabled, args);
    enabled = basicblock_callbacks.post   .apply(enabled, args);
    return enabled;
}

bool
AsmUnparser::unparse_staticdata(bool enabled, std::ostream &output, SgAsmStaticData *data, size_t position_in_block)
{
    set_prefix_address(data->get_address());
    UnparserCallback::StaticDataArgs args(this, output, data, position_in_block);
    enabled = staticdata_callbacks.pre    .apply(enabled, args);
    enabled = staticdata_callbacks.unparse.apply(enabled, args);
    enabled = staticdata_callbacks.post   .apply(enabled, args);
    return enabled;
}

bool
AsmUnparser::unparse_datablock(bool enabled, std::ostream &output, SgAsmBlock *block)
{
    std::vector<SgAsmStaticData*> datalist;
    const SgAsmStatementPtrList &stmts = block->get_statementList();
    for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
        if (isSgAsmStaticData(*si))
            datalist.push_back(isSgAsmStaticData(*si));
    }

    set_prefix_address(block->get_address());
    UnparserCallback::DataBlockArgs args(this, output, block, datalist);
    enabled = datablock_callbacks.pre    .apply(enabled, args);
    enabled = datablock_callbacks.unparse.apply(enabled, args);
    enabled = datablock_callbacks.post   .apply(enabled, args);
    return enabled;
}

bool
AsmUnparser::unparse_function(bool enabled, std::ostream &output, SgAsmFunction *func)
{
    set_prefix_address(func->get_entry_va());
    UnparserCallback::FunctionArgs args(this, output, func);
    enabled = function_callbacks.pre    .apply(enabled, args);
    enabled = function_callbacks.unparse.apply(enabled, args);
    enabled = function_callbacks.post   .apply(enabled, args);
    return enabled;
}

bool
AsmUnparser::unparse_interpretation(bool enabled, std::ostream &output, SgAsmInterpretation *interp)
{
    const RegisterDictionary *old_interp_registers = interp_registers;
    interp_registers = interp->get_registers();
    try {
        const SgAsmGenericHeaderPtrList &hdrs = interp->get_headers()->get_headers();
        if (!hdrs.empty()) {
            set_prefix_address(hdrs[0]->get_entry_rva() + hdrs[0]->get_base_va());
        } else {
            set_prefix_address(0);
        }

        UnparserCallback::InterpretationArgs args(this, output, interp);
        enabled = interp_callbacks.pre    .apply(enabled, args);
        enabled = interp_callbacks.unparse.apply(enabled, args);
        enabled = interp_callbacks.post   .apply(enabled, args);
    } catch (...) {
        interp_registers = old_interp_registers;
        throw;
    }
    interp_registers = old_interp_registers;
    return enabled;
}



/******************************************************************************************************************************
 *                                      Instruction callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::InsnBlockSeparation::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled && ORGANIZED_BY_ADDRESS==args.unparser->get_organization()) {
        SgAsmBlock *block = isSgAsmBlock(args.insn->get_parent()); // look only to immediate parent
        if (block!=prev_block) {
            prev_block = block;
            args.output <<std::endl; // blank line, no line prefix
        }
    }
    return enabled;
}

bool
AsmUnparser::InsnSkipBackBegin::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled)
        args.unparser->start_of_object(args.insn->get_address(), args.output);
    return enabled;
}

bool
AsmUnparser::InsnFuncEntry::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled && ORGANIZED_BY_ADDRESS==args.unparser->get_organization()) {
        SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(args.insn);
        ASSERT_not_null(func);
        if (func->get_entry_va()==args.insn->get_address())
            args.unparser->unparse_function(true, args.output, func);
    }
    return enabled;
}

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
    if (enabled) {
        HexdumpFormat tmp_fmt = fmt;
        tmp_fmt.addr_fmt = args.unparser->get_prefix_format();
        SgAsmExecutableFileFormat::hexdump(args.output, args.insn->get_address(), &(args.insn->get_raw_bytes()[0]),
                                           args.insn->get_size(), tmp_fmt);
    }
    return enabled;
}

bool
AsmUnparser::InsnBlockEntry::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled && ORGANIZED_BY_ADDRESS==args.unparser->get_organization()) {
        SgAsmBlock *block = isSgAsmBlock(args.insn->get_parent()); // look only to immediate parent
        bool is_first_insn = block && args.insn==block->get_statementList().front();

        static size_t width = 0;
        if (0==width)
            width = block->reason_str(true, 0).size();

        if (show_function) {
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(block);
            char buf[32];
            if (func && is_first_insn) {
                snprintf(buf, sizeof buf, "F%08" PRIx64, func->get_entry_va());
            } else {
                sprintf(buf, "%*s", 9, "");
            }
            args.output <<" " <<buf;
        }

        if (show_reasons) {
            if (block && is_first_insn) {
                args.output <<" " <<block->reason_str(true) <<" ";
            } else {
                args.output <<std::setw(width+2) <<" ";
            }
        }
    }
    return enabled;
}

bool
AsmUnparser::InsnStackDelta::operator()(bool enabled, const InsnArgs &args) {
    static const int deltaWidth = 2;                    // min column width for delta digits
    if (enabled) {
        int64_t delta = args.insn->get_stackDeltaIn();
        if (delta != SgAsmInstruction::INVALID_STACK_DELTA) {
            mfprintf(args.output)("<sp%+-*" PRId64 ">", deltaWidth+1, delta);
        } else {
            args.output <<std::string(deltaWidth+5, ' ');
        }
    }
    return enabled;
}

bool
AsmUnparser::InsnBody::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled)
        args.output <<"   " <<unparseInstruction(args.insn, &args.unparser->labels, args.unparser->get_registers());
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

bool
AsmUnparser::InsnSkipBackEnd::operator()(bool enabled, const InsnArgs &args)
{
    if (enabled)
        args.unparser->end_of_object(args.insn->get_address() + args.insn->get_size());
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
        NoopSequences noops = args.insns.front()->findNoopSubsequences(args.insns, true, true);
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
AsmUnparser::BasicBlockPredecessors::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled) {
        CFG_BlockMap::const_iterator bmi=args.unparser->cfg_blockmap.find(args.block);
        if (bmi!=args.unparser->cfg_blockmap.end()) {
            CFG_Vertex vertex = bmi->second;
            size_t npreds = 0;
            SgAsmFunction *my_func = args.block->get_enclosing_function();
            boost::graph_traits<CFG>::in_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=in_edges(vertex, args.unparser->cfg); ei!=ei_end; ++ei) {
                SgAsmBlock *pred_blk = get(boost::vertex_name, args.unparser->cfg, source(*ei, args.unparser->cfg));
                SgAsmFunction *pred_func = pred_blk ? pred_blk->get_enclosing_function() : NULL;
                if (pred_blk) {
                    if (0==npreds++) {
                        args.output <<args.unparser->line_prefix() <<"Predecessor blocks: ";
                    } else {
                        args.output <<", ";
                    }
                    args.output <<StringUtility::addrToString(pred_blk->get_address());
                    if (pred_func && my_func && pred_func!=my_func) {
                        if (pred_func->get_name().empty()) {
                            args.output <<"<<Func>>";
                        } else {
                            args.output <<"<" <<pred_func->get_name() <<">";
                        }
                    }
                }
            }
            if (npreds>0)
                args.output <<"\n";
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
            args.output <<args.unparser->line_prefix() <<"omitting "
                        <<nnoops <<" instruction" <<(1==nnoops?"":"s") <<" as no-op sequences from this block.\n";
        }
    }
    return enabled;
}

bool
AsmUnparser::BasicBlockReasons::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled) {
        args.output <<args.unparser->line_prefix() <<"Basic block reasons: " <<args.block->reason_str(false);
        if (args.block->get_code_likelihood()<1.0 && args.block->get_code_likelihood()>=0.0)
            args.output <<"; " <<floor(100.0*args.block->get_code_likelihood()+0.5) <<"% code likelihood";
        args.output <<"\n";
    }
    return enabled;
}

bool
AsmUnparser::BasicBlockBody::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled && ORGANIZED_BY_AST==args.unparser->get_organization()) {
        for (size_t i=0; i<args.insns.size(); i++)
            args.unparser->unparse_insn(enabled, args.output, args.insns[i], i);
    }
    return enabled;
}

bool
AsmUnparser::BasicBlockOutgoingStackDelta::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled) {
        int64_t n = args.block->get_stackDeltaOut();
        if (n != SgAsmInstruction::INVALID_STACK_DELTA) {
            args.output <<args.unparser->line_prefix();
            mfprintf(args.output)("Outgoing stack delta: %+" PRId64 "\n", n);
        }
    }
    return enabled;
}

static bool
increasing_block_address(SgAsmBlock *a, SgAsmBlock *b) {
    if (NULL == b)
        return false;
    if (NULL == a)
        return true;
    return a->get_address() < b->get_address();
}

static bool
increasing_integer_expr(SgAsmIntegerValueExpression *a, SgAsmIntegerValueExpression *b) {
    if (NULL == b)
        return false;
    if (NULL == a)
        return true;
    return a->get_absoluteValue() < b->get_absoluteValue();
}

bool
AsmUnparser::BasicBlockSuccessors::operator()(bool enabled, const BasicBlockArgs &args)
{
    if (enabled) {
        size_t nsucs = 0;
        args.output <<args.unparser->line_prefix() <<"Successor blocks: ";
        SgAsmFunction *my_func = args.block->get_enclosing_function();

        CFG_BlockMap::const_iterator bmi = args.unparser->cfg_blockmap.find(args.block);
        if (bmi!=args.unparser->cfg_blockmap.end()) {
            // Use the unparser's CFG if it contains info for this block.
            CFG_Vertex vertex = bmi->second;
            std::vector<SgAsmBlock*> suc_blks;

            boost::graph_traits<CFG>::out_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=out_edges(vertex, args.unparser->cfg); ei!=ei_end; ++ei) {
                SgAsmBlock *suc_blk = get(boost::vertex_name, args.unparser->cfg, target(*ei, args.unparser->cfg));
                suc_blks.push_back(suc_blk);
            }
            std::sort(suc_blks.begin(), suc_blks.end(), increasing_block_address);

            BOOST_FOREACH (SgAsmBlock *suc_blk, suc_blks) {
                SgAsmFunction *suc_func = suc_blk->get_enclosing_function();
                if (suc_blk) {
                    args.output <<(0==nsucs++?"":", ") <<StringUtility::addrToString(suc_blk->get_address());
                    if (suc_func && my_func && suc_func!=my_func) {
                        if (suc_blk->get_address() != suc_func->get_entry_va())
                            args.output <<" in ";
                        if (suc_func->get_name().empty()) {
                            args.output <<"<<Func>>";
                        } else {
                            args.output <<"<" <<suc_func->get_name() <<">";
                        }
                    }
                }
            }
        } else {
            // Use the successors cached in the AST. We print them as absolute virtual addresses rather than using
            // SgAsmIntegerValueExpression::get_label() because the value would probably have already been printed using
            // get_label() in the previous disassembled instruction.
            SgAsmIntegerValuePtrList successors = args.block->get_successors();
            std::sort(successors.begin(), successors.end(), increasing_integer_expr);
            for (SgAsmIntegerValuePtrList::const_iterator si=successors.begin(); si!=successors.end(); ++si) {
                args.output <<(0==nsucs++?"":", ") <<StringUtility::addrToString((*si)->get_absoluteValue());
                SgAsmBlock *suc_blk = isSgAsmBlock((*si)->get_baseNode());
                SgAsmFunction *suc_func = suc_blk ? suc_blk->get_enclosing_function() : isSgAsmFunction((*si)->get_baseNode());
                if (suc_func && my_func && suc_func!=my_func) {
                    if (suc_func->get_entry_va() != (*si)->get_absoluteValue())
                        args.output <<" in ";
                    if (suc_func->get_name().empty()) {
                        args.output <<"<<Func>>";
                    } else {
                        args.output <<"<" <<suc_func->get_name() <<">";
                    }
                }
            }
        }

        // The control flow graph doesn't store whether successor information is complete or not.  We have no choice but to get
        // that tidbit from the AST.
        if (!args.block->get_successors_complete())
            args.output <<(0==nsucs++?"":", ") <<"unknown";
        args.output <<(0==nsucs?"none\n":"\n");
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
 *                                      Static data callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::StaticDataBlockSeparation::operator()(bool enabled, const StaticDataArgs &args)
{
    if (enabled && ORGANIZED_BY_ADDRESS==args.unparser->get_organization()) {
        SgAsmBlock *block = isSgAsmBlock(args.data->get_parent()); // look only to immediate parent
        if (block!=prev_block) {
            prev_block = block;
            args.output <<std::endl;
        }
    }
    return enabled;
}

bool
AsmUnparser::StaticDataSkipBackBegin::operator()(bool enabled, const StaticDataArgs &args)
{
    if (enabled)
        args.unparser->start_of_object(args.data->get_address(), args.output);
    return enabled;
}

bool
AsmUnparser::StaticDataBlockEntry::operator()(bool enabled, const StaticDataArgs &args)
{
    if (enabled && ORGANIZED_BY_ADDRESS==args.unparser->get_organization()) {
        SgAsmBlock *block = isSgAsmBlock(args.data->get_parent()); // look only to immediate parent
        bool is_first_data = block && args.data==block->get_statementList().front();

        static size_t width = 0;
        if (0==width)
            width = block->reason_str(true, 0).size();

        if (show_function) {
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(block);
            char buf[32];
            if (func && is_first_data) {
                snprintf(buf, sizeof buf, "F%08" PRIx64, func->get_entry_va());
            } else {
                sprintf(buf, "%*s", 9, "");
            }
            args.output <<" " <<buf;
        }

        if (show_reasons) {
            if (block && is_first_data) {
                args.output <<" " <<block->reason_str(true) <<" ";
            } else {
                args.output <<std::setw(width+2) <<" ";
            }
        }
    }
    return enabled;
}

bool
AsmUnparser::StaticDataRawBytes::operator()(bool enabled, const StaticDataArgs &args)
{
    rose_addr_t start_address = 0;
    char prefix[64], *addr_fmt=NULL;
    HexdumpFormat tmp_fmt = fmt;

    if (enabled) {
        if (show_address) {
            addr_fmt = strdup(args.unparser->get_prefix_format().c_str());
            if (show_offset) {
                start_address = 0;
                sprintf(prefix, "0x%08" PRIx64 "+", args.data->get_address());
                tmp_fmt.prefix = prefix;
            } else {
                start_address = args.data->get_address();
                tmp_fmt.prefix = "";
            }
        } else {
            tmp_fmt.prefix = "";
            addr_fmt = strdup("");
        }

        tmp_fmt.addr_fmt = addr_fmt ? addr_fmt : "";
        if (!tmp_fmt.multiline)
            args.output <<tmp_fmt.prefix;

        SgAsmExecutableFileFormat::hexdump(args.output, start_address, &(args.data->get_raw_bytes()[0]),
                                           args.data->get_size(), tmp_fmt);
        if (addr_fmt != NULL)
            free(addr_fmt);
    }
    return enabled;
}

bool
AsmUnparser::StaticDataDetails::operator()(bool enabled, const StaticDataArgs &args)
{
    if (enabled) {
        SgAsmBlock *dblock = isSgAsmBlock(args.data->get_parent()); // look only to parent for data block
        size_t nbytes = args.data->get_size();

        if (dblock && 0!=(dblock->get_reason() & SgAsmBlock::BLK_JUMPTABLE)) {
            args.output <<" " <<nbytes <<"-byte jump table beginning at "
                        <<StringUtility::addrToString(args.data->get_address());
        } else if (dblock && 0!=(dblock->get_reason() & SgAsmBlock::BLK_PADDING)) {
            args.output <<" " <<nbytes <<"-byte padding beginning at "
                        <<StringUtility::addrToString(args.data->get_address());
        } else {
            args.output <<" " <<nbytes <<" byte" <<(1==nbytes?"":"s") <<" untyped data beginning at "
                        <<StringUtility::addrToString(args.data->get_address());
        }
    }
    return enabled;
}

bool
AsmUnparser::StaticDataComment::operator()(bool enabled, const StaticDataArgs &args)
{
    if (enabled && !args.data->get_comment().empty())
        args.output <<" /* " <<args.data->get_comment() <<" */";
    return enabled;
}

bool
AsmUnparser::StaticDataLineTermination::operator()(bool enabled, const StaticDataArgs &args)
{
    if (enabled)
        args.output <<std::endl;
    return enabled;
}

void
AsmUnparser::StaticDataDisassembler::reset()
{
    if (unparser_allocated_here)
        delete unparser;
    unparser_allocated_here = false;
    disassembler = NULL;
    unparser = NULL;
}

void
AsmUnparser::StaticDataDisassembler::init(Disassembler *d, AsmUnparser *u)
{
    reset();
    disassembler = d;

    if (u) {
        unparser = u;
        unparser_allocated_here = false;
    } else {
        unparser = new AsmUnparser;
        unparser->insn_callbacks.pre.erase(&unparser->insnStackDelta); // no stack deltas for data
        unparser->insn_callbacks.unparse.prepend(&data_note);
        unparser_allocated_here = true;
    }
}

bool
AsmUnparser::StaticDataDisassembler::operator()(bool enabled, const StaticDataArgs &args)
{
    SgAsmBlock *block = SageInterface::getEnclosingNode<SgAsmBlock>(args.data);
    if (enabled && block && disassembler && unparser &&
        0==(block->get_reason() & SgAsmBlock::BLK_PADDING) &&
        0==(block->get_reason() & SgAsmBlock::BLK_JUMPTABLE)) {
        SgUnsignedCharList data = args.data->get_raw_bytes();
        MemoryMap::Ptr map = MemoryMap::instance();
        map->insert(AddressInterval::baseSize(args.data->get_address(), data.size()),
                    MemoryMap::Segment::staticInstance(&data[0], data.size(), MemoryMap::READABLE|MemoryMap::EXECUTABLE,
                                                       "static data block"));
        unparser->set_prefix_format(args.unparser->get_prefix_format());
        rose_addr_t offset=0, nskipped=0;
        while (offset < data.size()) {
            rose_addr_t insn_va = args.data->get_address() + offset;
            SgAsmInstruction *insn = NULL;
            try {
                insn = disassembler->disassembleOne(map, insn_va, NULL);
                unparser->unparse(args.output, insn);
                offset += insn->get_size();
                SageInterface::deleteAST(insn);
            } catch (...) {
                offset += 1;
                ++nskipped;
            }
        }
    }
    return enabled;
}

bool
AsmUnparser::StaticDataSkipBackEnd::operator()(bool enabled, const StaticDataArgs &args)
{
    if (enabled)
        args.unparser->end_of_object(args.data->get_address() + args.data->get_size());
    return enabled;
}

/******************************************************************************************************************************
 *                                      Data block callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::DataBlockBody::operator()(bool enabled, const DataBlockArgs &args)
{
    if (enabled && ORGANIZED_BY_AST==args.unparser->get_organization()) {
        for (size_t i=0; i<args.datalist.size(); i++)
            args.unparser->unparse_staticdata(enabled, args.output, args.datalist[i], i);
    }
    return enabled;
}

bool
AsmUnparser::DataBlockLineTermination::operator()(bool enabled, const DataBlockArgs &args)
{
    if (enabled)
        args.output <<std::endl;
    return enabled;
}

/******************************************************************************************************************************
 *                                      Function callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::FunctionEntryAddress::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled)
        args.output <<args.unparser->line_prefix();
    return enabled;
}

bool
AsmUnparser::FunctionSeparator::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled)
        args.output <<"================================== Function";
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
AsmUnparser::FunctionComment::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled) {
        std::string s = args.func->get_comment();
        if (!s.empty()) {
            args.output <<s;
            if (0==s.compare(s.size()-1, 1, "\n"))
                args.output <<std::endl;
        }
    }
    return enabled;
}

bool
AsmUnparser::FunctionPredecessors::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled) {
        CG_FunctionMap::const_iterator fmi = args.unparser->cg_functionmap.find(args.func);
        if (fmi!=args.unparser->cg_functionmap.end()) {
            CG_Vertex vertex = fmi->second;
            size_t npreds = 0;
            boost::graph_traits<CG>::in_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=in_edges(vertex, args.unparser->cg); ei!=ei_end; ++ei) {
                SgAsmFunction *pred = get(boost::vertex_name, args.unparser->cg, source(*ei, args.unparser->cg));
                if (pred) {
                    ++npreds;
                    args.output <<args.unparser->line_prefix()
                                <<"Called by " <<StringUtility::addrToString(pred->get_entry_va());
                    std::string fname = pred->get_name();
                    if (!fname.empty())
                        args.output <<"<" <<fname <<">";
                    args.output <<"\n";
                }
            }
            if (0==npreds)
                args.output <<args.unparser->line_prefix() <<"No known callers.\n";
        }
    }
    return enabled;
}

bool
AsmUnparser::FunctionSuccessors::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled) {
        CG_FunctionMap::const_iterator fmi = args.unparser->cg_functionmap.find(args.func);
        if (fmi!=args.unparser->cg_functionmap.end()) {
            CG_Vertex vertex = fmi->second;
            size_t nsuccs = 0;
            boost::graph_traits<CG>::out_edge_iterator ei, ei_end;
            for (boost::tie(ei, ei_end)=out_edges(vertex, args.unparser->cg); ei!=ei_end; ++ei) {
                SgAsmFunction *succ = get(boost::vertex_name, args.unparser->cg, target(*ei, args.unparser->cg));
                if (succ) {
                    ++nsuccs;
                    args.output <<args.unparser->line_prefix()
                                <<"This function calls " <<StringUtility::addrToString(succ->get_entry_va());
                    std::string fname = succ->get_name();
                    if (!fname.empty())
                        args.output <<"<" <<fname <<">";
                    args.output <<"\n";
                }
            }
        }
    }
    return enabled;
}

bool
AsmUnparser::FunctionAttributes::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled) {
        switch (args.func->get_may_return()) {
            case SgAsmFunction::RET_ALWAYS:
            case SgAsmFunction::RET_SOMETIMES:
            case SgAsmFunction::RET_UNKNOWN:
                // the usual cases, don't say anything, assume function might return
                break;
            case SgAsmFunction::RET_NEVER: {
                args.output <<args.unparser->line_prefix() <<"Function does not return to caller.\n";
                break;
            }
        }

        if (args.func->get_stackDelta() != SgAsmInstruction::INVALID_STACK_DELTA) {
            args.output <<args.unparser->line_prefix();
            mfprintf(args.output)("Function stack delta: %+" PRId64 "\n", args.func->get_stackDelta());
        }
    }
    return enabled;
}

bool
AsmUnparser::FunctionBody::operator()(bool enabled, const FunctionArgs &args)
{
    if (enabled && ORGANIZED_BY_AST==args.unparser->get_organization()) {
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
    if (enabled && ORGANIZED_BY_AST==args.unparser->get_organization()) {
        SgAsmBlock *global = args.interp->get_global_block();
        if (global) {
            const SgAsmStatementPtrList stmts = global->get_statementList();
            for (size_t i=0; i<stmts.size(); ++i)
                args.unparser->unparse(args.output, stmts[i]);
        }
    }
    return enabled;
}

} // namespace
} // namespace

#endif
