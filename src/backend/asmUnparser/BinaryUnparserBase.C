#include <sage3basic.h>
#include <BinaryUnparserBase.h>
#include <Diagnostics.h>
#include <Partitioner2/Partitioner.h>

#include <boost/algorithm/string/trim.hpp>
#include <sstream>

namespace P2 = rose::BinaryAnalysis::Partitioner2;
namespace S2 = rose::BinaryAnalysis::InstructionSemantics2;

namespace rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functionality
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnparserBase::init() {
    registerNames_ = RegisterNames(partitioner_.instructionProvider().registerDictionary());
    cg_ = partitioner_.functionCallGraph(false /*compress parallel edges*/);
}

// class method
std::string
UnparserBase::leftJustify(const std::string &s, size_t width) {
    if (s.size() > width)
        return s;
    return s + std::string(width-s.size(), ' ');
}

// class method
std::string
UnparserBase::juxtaposeColumns(const std::vector<std::string> &parts, const std::vector<size_t> &minWidths,
                               const std::string &columnSeparator) {
    std::string retval;

    // Split each part into separate lines
    size_t nlines = 0;
    std::vector<std::vector<std::string> > partLines(parts.size());
    for (size_t i=0; i<parts.size(); ++i) {
        partLines[i] = StringUtility::split('\n', parts[i]);
        nlines = std::max(nlines, partLines[i].size());
    }

    // Emit one line at a time across all columns
    const std::string empty;
    for (size_t i=0; i<nlines; ++i) {
        if (i != 0)
            retval += "\n";
        std::ostringstream ss;
        for (size_t j=0; j<partLines.size(); ++j) {
            const std::string &s = i < partLines[j].size() ? partLines[j][i] : empty;
            if (j > 0)
                ss <<columnSeparator;
            ss <<leftJustify(s, minWidths[j]);
        }
        retval += boost::trim_right_copy(ss.str());
    }
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Top-level
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnparserBase::unparse(std::ostream &out, SgAsmInstruction *insn) const {
    State state;
    emitInstruction(out, insn, state);
}

void
UnparserBase::unparse(std::ostream &out, const P2::BasicBlock::Ptr &bb) const {
    State state;
    emitBasicBlock(out, bb, state);
}

void
UnparserBase::unparse(std::ostream &out, const P2::DataBlock::Ptr &db) const {
    State state;
    emitDataBlock(out, db, state);
}

void
UnparserBase::unparse(std::ostream &out, const P2::Function::Ptr &f) const {
    State state;
    emitFunction(out, f, state);
}

std::string
UnparserBase::operator()(SgAsmInstruction *insn) const {
    std::ostringstream ss;
    unparse(ss, insn);
    return ss.str();
}

std::string
UnparserBase::operator()(const Partitioner2::BasicBlock::Ptr &bb) const {
    std::ostringstream ss;
    unparse(ss, bb);
    return ss.str();
}

std::string
UnparserBase::operator()(const Partitioner2::DataBlock::Ptr &db) const {
    std::ostringstream ss;
    unparse(ss, db);
    return ss.str();
}

std::string
UnparserBase::operator()(const Partitioner2::Function::Ptr &f) const {
    std::ostringstream ss;
    unparse(ss, f);
    return ss.str();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnparserBase::emitFunction(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    FunctionGuard push(state, function);
    emitFunctionPrologue(out, function, state);
    emitFunctionBody(out, function, state);
    emitFunctionEpilogue(out, function, state);
}

void
UnparserBase::emitFunctionPrologue(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    out <<std::string(120, ';') <<"\n"
        <<";;; " <<function->printableName() <<"\n";
    emitFunctionComment(out, function, state);
    if (settings().function.showingReasons)
        emitFunctionReasons(out, function, state);
    if (settings().function.cg.showing) {
        emitFunctionCallers(out, function, state);
        emitFunctionCallees(out, function, state);
    }
    if (settings().function.stackDelta.showing)
        emitFunctionStackDelta(out, function, state);
    if (settings().function.callconv.showing)
        emitFunctionCallingConvention(out, function, state);
    if (settings().function.mayReturn.showing)
        emitFunctionMayReturn(out, function, state);
    if (settings().function.noop.showing)
        emitFunctionNoopAnalysis(out, function, state);
}

void
UnparserBase::emitFunctionBody(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    rose_addr_t nextBlockVa = 0;
    BOOST_FOREACH (rose_addr_t bbVa, function->basicBlockAddresses()) {
        out <<"\n";
        if (P2::BasicBlock::Ptr bb = partitioner_.basicBlockExists(bbVa)) {
            if (bbVa != *function->basicBlockAddresses().begin()) {
                if (bbVa > nextBlockVa) {
                    out <<";;; skip forward " <<StringUtility::plural(bbVa - nextBlockVa, "bytes") <<"\n";
                } else if (bbVa < nextBlockVa) {
                    out <<";;; skip backward " <<StringUtility::plural(nextBlockVa - bbVa, "bytes") <<"\n";
                }
            }
            emitBasicBlock(out, bb, state);
            nextBlockVa = bb->fallthroughVa();
        }
    }
    BOOST_FOREACH (P2::DataBlock::Ptr db, function->dataBlocks()) {
        out <<"\n";
        emitDataBlock(out, db, state);
    }
}

void
UnparserBase::emitFunctionEpilogue(std::ostream &out, const P2::Function::Ptr&, State&) const {
    out <<"\n\n";
}

static void
addFunctionReason(std::vector<std::string> &strings /*in,out*/, unsigned &flags /*in,out*/,
                  unsigned bit, const std::string &why) {
    if (0 != (flags & bit)) {
        strings.push_back(why);
        flags &= ~bit;
    }
}

void
UnparserBase::emitFunctionReasons(std::ostream &out, const P2::Function::Ptr &function, State&) const {
    unsigned flags = function->reasons();
    std::vector<std::string> strings;
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_ENTRY_POINT,  "program entry point");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_IMPORT,       "import");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_THUNK,        "thunk");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_EXPORT,       "export");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_CALL_TARGET,  "function call target");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_CALL_INSN,    "possible function call target");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_EXCEPTION_HANDLER, "exception handler");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_EH_FRAME,     "referenced by ELF .eh_frame");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_SYMBOL,       "referenced by symbol table");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_PATTERN,      "pattern recognition");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_GRAPH,        "CFG traversal");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_PADDING,      "padding");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_DISCONT,      "discontiguous");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_INSNHEAD,     "possibly unreached (head)");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_LEFTOVERS,    "provisional");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_INTRABLOCK,   "possibly unreached (intra)");
    addFunctionReason(strings, flags, SgAsmFunction::FUNC_USERDEF,      "user defined");
    if (flags != 0) {
        char buf[64];
        sprintf(buf, "0x%08x", flags);
        strings.push_back("other (" + std::string(buf));
    }
    if (!strings.empty())
        out <<";;; reasons for function: " <<boost::join(strings, ", ") <<"\n";
}

void
UnparserBase::emitFunctionCallers(std::ostream &out, const P2::Function::Ptr &function, State&) const {
    P2::FunctionCallGraph::Graph::ConstVertexIterator vertex = cg().findFunction(function);
    if (cg().graph().isValidVertex(vertex)) {
        BOOST_FOREACH (const P2::FunctionCallGraph::Graph::Edge &edge, vertex->inEdges()) {
            out <<";;; ";
            switch (edge.value().type()) {
                case P2::E_FUNCTION_CALL:
                    out <<"called from ";
                    break;
                case P2::E_FUNCTION_XFER:
                    out <<"transfered from ";
                    break;
                default:
                    out <<"predecessor ";
                    break;
            }
            out <<edge.source()->value()->printableName();
            if (edge.value().count() > 1)
                out <<" " <<edge.value().count() <<" times";
            out <<"\n";
        }
    }
}

void
UnparserBase::emitFunctionCallees(std::ostream &out, const P2::Function::Ptr &function, State&) const {
    P2::FunctionCallGraph::Graph::ConstVertexIterator vertex = cg().findFunction(function);
    if (cg().graph().isValidVertex(vertex)) {
        BOOST_FOREACH (const P2::FunctionCallGraph::Graph::Edge &edge, vertex->outEdges()) {
            out <<";;; ";
            switch (edge.value().type()) {
                case P2::E_FUNCTION_CALL:
                    out <<"calls ";
                    break;
                case P2::E_FUNCTION_XFER:
                    out <<"transfers to ";
                    break;
                default:
                    out <<"successor ";
                    break;
            }
            out <<edge.target()->value()->printableName();
            if (edge.value().count() > 1)
                out <<" " <<edge.value().count() <<" times";
            out <<"\n";
        }
    }
}

void
UnparserBase::emitFunctionComment(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    std::string s = boost::trim_copy(function->comment());
    if (!s.empty()) {
        out <<";;;\n";
        emitCommentBlock(out, s, state, ";;; ");
        out <<";;;\n";
    }
}

void
UnparserBase::emitFunctionStackDelta(std::ostream &out, const P2::Function::Ptr &function, State&) const {
    if (settings().function.stackDelta.concrete) {
        int64_t delta = function->stackDeltaConcrete();
        if (delta != SgAsmInstruction::INVALID_STACK_DELTA)
            out <<";;; function stack delta is " <<StringUtility::toHex2(delta, 64) <<"\n";
    } else if (S2::BaseSemantics::SValuePtr delta = function->stackDelta()) {
        out <<";;; function stack delta is " <<*delta <<"\n";
    }
}

void
UnparserBase::emitFunctionCallingConvention(std::ostream &out, const P2::Function::Ptr &function, State &state) const {
    const CallingConvention::Analysis &analyzer = function->callingConventionAnalysis();
    if (analyzer.hasResults()) {
        if (analyzer.didConverge()) {
            std::ostringstream ss;
            ss <<analyzer;
            out <<";;; calling convention:\n";
            emitCommentBlock(out, ss.str(), state, ";;;   ");
        } else {
            out <<";;; calling convention analysis did not converge\n";
        }
    }
}

void
UnparserBase::emitFunctionNoopAnalysis(std::ostream &out, const P2::Function::Ptr &function, State&) const {
    if (function->isNoop().getOptional().orElse(false))
        out <<";;; this function is a no-op\n";
}

void
UnparserBase::emitFunctionMayReturn(std::ostream &out, const P2::Function::Ptr &function, State&) const {
    if (!partitioner().functionOptionalMayReturn(function).orElse(true))
        out <<";;; this function does not return to its caller\n";
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Basic Blocks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnparserBase::emitBasicBlock(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    BasicBlockGuard push(state, bb);
    emitBasicBlockPrologue(out, bb, state);
    emitBasicBlockBody(out, bb, state);
    emitBasicBlockEpilogue(out, bb, state);
}

void
UnparserBase::emitBasicBlockPrologue(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    emitBasicBlockComment(out, bb, state);
    if (settings().bblock.cfg.showingSharing)
        emitBasicBlockSharing(out, bb, state);
    if (settings().bblock.cfg.showingPredecessors)
        emitBasicBlockPredecessors(out, bb, state);
}

void
UnparserBase::emitBasicBlockBody(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    ASSERT_not_null(bb);
    if (0 == bb->nInstructions()) {
        out <<"no instructions";
    } else {
        BOOST_FOREACH (SgAsmInstruction *insn, bb->instructions()) {
            emitInstruction(out, insn, state);
            out <<"\n";
        }
    }
}

void
UnparserBase::emitBasicBlockEpilogue(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    BOOST_FOREACH (P2::DataBlock::Ptr db, bb->dataBlocks())
        emitDataBlock(out, db, state);
    if (settings().bblock.cfg.showingSuccessors)
        emitBasicBlockSuccessors(out, bb, state);
}

void
UnparserBase::emitBasicBlockComment(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    std::string s = boost::trim_copy(bb->comment());
    if (!s.empty())
        emitCommentBlock(out, s, state, "\t;; ");
}

void
UnparserBase::emitBasicBlockSharing(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    ASSERT_not_null(bb);
    std::vector<P2::Function::Ptr> functions = partitioner().functionsOwningBasicBlock(bb);
    std::vector<P2::Function::Ptr>::iterator current = std::find(functions.begin(), functions.end(), state.currentFunction);
    if (current != functions.end())
        functions.erase(current);
    BOOST_FOREACH (P2::Function::Ptr function, functions)
        out <<"\t;; block owned by " <<function->printableName() <<"\n";
}

void
UnparserBase::emitBasicBlockPredecessors(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    ASSERT_not_null(bb);
    P2::ControlFlowGraph::ConstVertexIterator vertex = partitioner().findPlaceholder(bb->address());
    ASSERT_require(partitioner().cfg().isValidVertex(vertex));

    Sawyer::Container::Map<rose_addr_t, std::string> preds;
    BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, vertex->inEdges()) {
        P2::ControlFlowGraph::ConstVertexIterator pred = edge.source();
        switch (pred->value().type()) {
            case P2::V_BASIC_BLOCK:
                if (pred->value().bblock()->nInstructions() > 1) {
                    rose_addr_t insnVa = pred->value().bblock()->instructions().back()->get_address();
                    std::string s = "instruction " + StringUtility::addrToString(insnVa) + " from " + bb->printableName();
                    preds.insert(insnVa, s);
                } else {
                    std::ostringstream ss;
                    emitAddress(ss, pred->value().address(), state);
                    preds.insert(pred->value().address(), ss.str());
                }
                break;
            case P2::V_USER_DEFINED: {
                std::ostringstream ss;
                emitAddress(ss, pred->value().address(), state);
                preds.insert(pred->value().address(), ss.str());
                break;
            }
            case P2::V_NONEXISTING:
            case P2::V_UNDISCOVERED:
            case P2::V_INDETERMINATE:
                ASSERT_not_reachable("vertex type should have no successors");
            default:
                ASSERT_not_implemented("cfg vertex type");
        }
    }

    BOOST_FOREACH (const std::string &s, preds.values())
        out <<"\t;; predecessor: " <<s <<"\n";
}

void
UnparserBase::emitBasicBlockSuccessors(std::ostream &out, const P2::BasicBlock::Ptr &bb, State &state) const {
    const P2::BasicBlock::Successors &succs = partitioner().basicBlockSuccessors(bb);
    std::vector<std::string> strings;

    // Real successors
    BOOST_FOREACH (const P2::BasicBlock::Successor &succ, succs) {
        std::string s;
        switch (succ.type()) {
            case P2::E_CALL_RETURN:     s = "call return to ";  break;
            case P2::E_FUNCTION_CALL:   s = "call to ";         break;
            case P2::E_FUNCTION_XFER:   s = "xfer to ";         break;
            case P2::E_FUNCTION_RETURN: s = "return to ";       break;
            case P2::E_NORMAL:                                  break;
            case P2::E_USER_DEFINED:    s = "user-defined to "; break;
            default: ASSERT_not_implemented("basic block successor type");
        }
        ASSERT_not_null(succ.expr());
        SymbolicExpr::Ptr expr = succ.expr()->get_expression();
        if (expr->isNumber() && expr->nBits() <= 64) {
            std::ostringstream ss;
            emitAddress(ss, expr->toInt(), state);
            s += ss.str();
        } else if (expr->isLeafNode()) {
            s += "unknown";
        } else {
            std::ostringstream ss;
            ss <<*expr;
            s += ss.str();
        }

        if (std::find(strings.begin(), strings.end(), s) == strings.end())
            strings.push_back(s);
    }

    // Ghost successors due to opaque predicates
    if (bb->ghostSuccessors().isCached()) {
        BOOST_FOREACH (rose_addr_t va, bb->ghostSuccessors().get()) {
            std::ostringstream ss;
            emitAddress(ss, va, state);
            if (std::find(strings.begin(), strings.end(), ss.str()) == strings.end() &&
                std::find(strings.begin(), strings.end(), ss.str() + " (ghost)") == strings.end())
                strings.push_back(ss.str() + " (ghost)");
        }
    }

    // Output
    if (strings.empty()) {
        out <<"\t;; successor: none\n";
    } else {
        BOOST_FOREACH (const std::string &s, strings)
            out <<"\t;; successor: " <<s <<"\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Data Blocks
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnparserBase::emitDataBlock(std::ostream &out, const P2::DataBlock::Ptr &db, State &state) const {
    emitDataBlockPrologue(out, db, state);
    emitDataBlockBody(out, db, state);
    emitDataBlockEpilogue(out, db, state);
}

void
UnparserBase::emitDataBlockPrologue(std::ostream&, const P2::DataBlock::Ptr&, State&) const {}

void
UnparserBase::emitDataBlockBody(std::ostream &out, const P2::DataBlock::Ptr&, State&) const {
    out <<"data blocks not implemented yet";
}

void
UnparserBase::emitDataBlockEpilogue(std::ostream&, const P2::DataBlock::Ptr&, State&) const {}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Instructions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnparserBase::emitInstruction(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    emitInstructionPrologue(out, insn, state);
    emitInstructionBody(out, insn, state);
    emitInstructionEpilogue(out, insn, state);
}

void
UnparserBase::emitInstructionBody(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    std::vector<std::string> parts;
    std::vector<size_t> fieldWidths;

    // Address
    if (settings().insn.address.showing) {
        if (insn) {
            std::ostringstream ss;
            emitInstructionAddress(ss, insn, state);
            parts.push_back(ss.str());
        } else {
            parts.push_back("");
        }
        fieldWidths.push_back(settings().insn.address.fieldWidth);
    }

    // Raw bytes
    if (settings().insn.bytes.showing) {
        if (insn) {
            std::ostringstream ss;
            emitInstructionBytes(ss, insn, state);
            parts.push_back(ss.str());
        } else {
            parts.push_back("");
        }
        fieldWidths.push_back(settings().insn.bytes.fieldWidth);
    }

    // Stack delta
    if (settings().insn.stackDelta.showing) {
        if (insn) {
            std::ostringstream ss;
            emitInstructionStackDelta(ss, insn, state);
            parts.push_back(ss.str());
        } else {
            parts.push_back("");
        }
        fieldWidths.push_back(settings().insn.stackDelta.fieldWidth);
    }

    // Mnemonic
    if (insn) {
        std::ostringstream ss;
        emitInstructionMnemonic(ss, insn, state);
        parts.push_back(ss.str());
    } else {
        parts.push_back("none");
    }
    fieldWidths.push_back(settings().insn.mnemonic.fieldWidth);
        

    // Operands
    if (insn) {
        std::ostringstream ss;
        emitInstructionOperands(ss, insn, state);
        parts.push_back(ss.str());
    } else {
        parts.push_back("");
    }
    fieldWidths.push_back(settings().insn.operands.fieldWidth);

    // Comment
    if (settings().insn.comment.showing) {
        std::ostringstream ss;
        emitInstructionComment(ss, insn, state);
        parts.push_back(ss.str());
        fieldWidths.push_back(settings().insn.comment.fieldWidth);
    }

    out <<juxtaposeColumns(parts, fieldWidths);
}

void
UnparserBase::emitInstructionAddress(std::ostream &out, SgAsmInstruction *insn, State&) const {
    ASSERT_not_null(insn);
    out <<StringUtility::addrToString(insn->get_address());
}

void
UnparserBase::emitInstructionBytes(std::ostream &out, SgAsmInstruction *insn, State&) const {
    ASSERT_not_null(insn);
    const SgUnsignedCharList &bytes = insn->get_raw_bytes();
    for (size_t i = 0; i < bytes.size(); ++i) {
        if (0 == i) {
            // void
        } else if (0 == i % 8) {
            out <<"\n";
        } else {
            out <<" ";
        }
        Diagnostics::mfprintf(out)("%02x", bytes[i]);
    }
}

void
UnparserBase::emitInstructionStackDelta(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    // Although SgAsmInstruction has stackDeltaIn and stackDelta out data members, they're not initialized or used when the
    // instruction only exists inside a Partitioner2 object -- they're only initialized when a complete AST is created from the
    // Partitioner2. Instead, we need to look at the enclosing function's stack delta analysis.  Since basic blocks can be
    // shared among functions, this method prints the stack deltas from the perspective of the function we're emitting.
    ASSERT_not_null(insn);
    if (P2::Function::Ptr function = state.currentFunction) {
        const StackDelta::Analysis &sdAnalysis = function->stackDeltaAnalysis();
        if (sdAnalysis.hasResults()) {
            int64_t delta = sdAnalysis.toInt(sdAnalysis.instructionInputStackDeltaWrtFunction(insn));
            if (SgAsmInstruction::INVALID_STACK_DELTA == delta) {
                out <<" ??";
            } else if (delta == 0) {
                out <<" 00";
            } else if (delta > 0) {
                Diagnostics::mfprintf(out)("+%02x", (unsigned)delta);
            } else {
                Diagnostics::mfprintf(out)("-%02x", (unsigned)(-delta));
            }
            return;
        }
    }
    out <<"??";
}

void
UnparserBase::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn, State&) const {
    ASSERT_not_null(insn);
    out <<insn->get_mnemonic();
}

void
UnparserBase::emitInstructionOperands(std::ostream &out, SgAsmInstruction *insn, State &state) const {
    ASSERT_not_null(insn);
    const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
    for (size_t i=0; i<operands.size(); ++i) {
        if (i > 0)
            out <<settings().insn.operands.separator;
        emitOperand(out, operands[i], state);
    }
}

void
UnparserBase::emitInstructionComment(std::ostream &out, SgAsmInstruction *insn, State&) const {
    ASSERT_not_null(insn);
    std::string comment = insn->get_comment();
    boost::trim(comment);
    if (!comment.empty())
        out <<"; " <<StringUtility::cEscape(comment);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Operand expressions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
UnparserBase::emitOperand(std::ostream &out, SgAsmExpression *expr, State &state) const {
    emitOperandPrologue(out, expr, state);
    emitOperandBody(out, expr, state);
    emitOperandEpilogue(out, expr, state);
}

void
UnparserBase::emitAddress(std::ostream &out, rose_addr_t va, State&) const {
    if (P2::Function::Ptr f = partitioner().functionExists(va)) {
        out <<f->printableName();
    } else if (P2::BasicBlock::Ptr bb = partitioner().basicBlockExists(va)) {
        out <<bb->printableName();
    } else {
        out <<StringUtility::addrToString(va);
    }
}

void
UnparserBase::emitAddress(std::ostream &out, const Sawyer::Container::BitVector &bv, State &state) const {
    if (bv.size() > 64) {
        out <<"0x" <<bv.toHex();
    } else {
        emitAddress(out, bv.toInteger(), state);
    }
}

std::vector<std::string>
UnparserBase::emitInteger(std::ostream &out, const Sawyer::Container::BitVector &bv, State &state, bool isSigned) const {
    std::vector<std::string> comments;

    if (bv.isEqualToZero()) {
        out <<"0";
    } else if (bv.size() <= 64 && bv.toInteger() < 16) {
        out <<bv.toInteger();
    } else if (bv.size() == 32 && bv.toInteger() >= 0xffff && bv.toInteger() < 0xffff0000) {
        emitAddress(out, bv, state);
    } else if (bv.size() == 64 && bv.toInteger() >= 0xffff && bv.toInteger() < 0xffffffffffff0000ull) {
        emitAddress(out, bv, state);
    } else if (bv.size() > 64) {
        out <<"0x" <<bv.toHex();                        // too wide for decimal representation
    } else {
        out <<"0x" <<bv.toHex();
        uint64_t ui = bv.toInteger();
        comments.push_back(boost::lexical_cast<std::string>(ui));
        if (isSigned) {
            int64_t si = (int64_t)IntegerOps::signExtend2(ui, bv.size(), 64);
            if (si < 0)
                comments.push_back(boost::lexical_cast<std::string>(si));
        }
    }

    return comments;
}

std::vector<std::string>
UnparserBase::emitSignedInteger(std::ostream &out, const Sawyer::Container::BitVector &bv, State &state) const {
    return emitInteger(out, bv, state, true /*signed*/);
}

std::vector<std::string>
UnparserBase::emitUnsignedInteger(std::ostream &out, const Sawyer::Container::BitVector &bv, State &state) const {
    return emitInteger(out, bv, state, false /*unsigned*/);
}

void
UnparserBase::emitRegister(std::ostream &out, const RegisterDescriptor &reg, State&) const {
    out <<registerNames()(reg);
}

void
UnparserBase::emitCommentBlock(std::ostream &out, const std::string &comment, State&, const std::string &prefix) const {
    std::vector<std::string> lines = StringUtility::split('\n', comment);
    BOOST_FOREACH (const std::string &line, lines)
        out <<prefix <<line <<"\n";
}

void
UnparserBase::emitTypeName(std::ostream &out, SgAsmType *type, State&) const {
    ASSERT_not_null(type);

    if (SgAsmIntegerType *it = isSgAsmIntegerType(type)) {
        switch (it->get_nBits()) {
            case 8: out <<"int8_t"; return;
            case 16: out <<"int16_t"; return;
            case 32: out <<"int32_t"; return;
            case 64: out <<"int64_t"; return;
        }
    }
    ASSERT_not_implemented(type->toString());
}


} // namespace
} // namespace
} // namespace
