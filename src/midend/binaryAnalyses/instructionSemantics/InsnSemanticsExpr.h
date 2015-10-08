#ifndef Rose_InsnSemanticsExpr_H
#define Rose_InsnSemanticsExpr_H

#include <BinarySymbolicExpr.h>

namespace rose {
namespace BinaryAnalysis {

// [Robb P. Matzke 2015-10-08]: deprecated: use rose::BinaryAnalysis::SymbolicExpr instead.
namespace InsnSemanticsExpr {

using SymbolicExpr::Operator;
using SymbolicExpr::OP_ADD;
using SymbolicExpr::OP_AND;
using SymbolicExpr::OP_ASR;
using SymbolicExpr::OP_BV_AND;
using SymbolicExpr::OP_BV_OR;
using SymbolicExpr::OP_BV_XOR;
using SymbolicExpr::OP_CONCAT;
using SymbolicExpr::OP_EQ;
using SymbolicExpr::OP_EXTRACT;
using SymbolicExpr::OP_INVERT;
using SymbolicExpr::OP_ITE;
using SymbolicExpr::OP_LSSB;
using SymbolicExpr::OP_MSSB;
using SymbolicExpr::OP_NE;
using SymbolicExpr::OP_NEGATE;
using SymbolicExpr::OP_NOOP;
using SymbolicExpr::OP_OR;
using SymbolicExpr::OP_READ;
using SymbolicExpr::OP_ROL;
using SymbolicExpr::OP_ROR;
using SymbolicExpr::OP_SDIV;
using SymbolicExpr::OP_SEXTEND;
using SymbolicExpr::OP_SGE;
using SymbolicExpr::OP_SGT;
using SymbolicExpr::OP_SHL0;
using SymbolicExpr::OP_SHL1;
using SymbolicExpr::OP_SHR0;
using SymbolicExpr::OP_SHR1;
using SymbolicExpr::OP_SLE;
using SymbolicExpr::OP_SLT;
using SymbolicExpr::OP_SMOD;
using SymbolicExpr::OP_SMUL;
using SymbolicExpr::OP_UDIV;
using SymbolicExpr::OP_UEXTEND;
using SymbolicExpr::OP_UGE;
using SymbolicExpr::OP_UGT;
using SymbolicExpr::OP_ULE;
using SymbolicExpr::OP_ULT;
using SymbolicExpr::OP_UMOD;
using SymbolicExpr::OP_UMUL;
using SymbolicExpr::OP_WRITE;
using SymbolicExpr::OP_ZEROP;

typedef SymbolicExpr::TreeNode TreeNode;
typedef SymbolicExpr::InternalNode InternalNode;
typedef SymbolicExpr::LeafNode LeafNode;

typedef SymbolicExpr::TreeNodePtr TreeNodePtr;
typedef SymbolicExpr::InternalNodePtr InternalNodePtr;
typedef SymbolicExpr::LeafNodePtr LeafNodePtr;

typedef SymbolicExpr::TreeNodes TreeNodes;
typedef SymbolicExpr::RenameMap RenameMap;
typedef SymbolicExpr::Formatter Formatter;

using SymbolicExpr::VisitAction;
using SymbolicExpr::CONTINUE;
using SymbolicExpr::TRUNCATE;
using SymbolicExpr::TERMINATE;

using SymbolicExpr::MAX_NNODES;
typedef SymbolicExpr::Visitor Visitor;

typedef SymbolicExpr::Simplifier Simplifier;
typedef SymbolicExpr::AddSimplifier AddSimplifier;
typedef SymbolicExpr::AndSimplifier AndSimplifier;
typedef SymbolicExpr::OrSimplifier OrSimplifier;
typedef SymbolicExpr::XorSimplifier XorSimplifier;
typedef SymbolicExpr::SmulSimplifier SmulSimplifier;
typedef SymbolicExpr::UmulSimplifier UmulSimplifier;
typedef SymbolicExpr::ConcatSimplifier ConcatSimplifier;
typedef SymbolicExpr::ExtractSimplifier ExtractSimplifier;
typedef SymbolicExpr::AsrSimplifier AsrSim;
typedef SymbolicExpr::InvertSimplifier InvertSimplifier;
typedef SymbolicExpr::NegateSimplifier NegateSimplifier;
typedef SymbolicExpr::IteSimplifier IteSimplifier;
typedef SymbolicExpr::NoopSimplifier NoopSimplifier;
typedef SymbolicExpr::RolSimplifier RolSimplifier;
typedef SymbolicExpr::RorSimplifier RorSimplifier;
typedef SymbolicExpr::UextendSimplifier UextendSimplifier;
typedef SymbolicExpr::SextendSimplifier SextendSimplifier;
typedef SymbolicExpr::EqSimplifier EqSimplifier;
typedef SymbolicExpr::SgeSimplifier SgeSimplifier;
typedef SymbolicExpr::SgtSimplifier SgtSimplifier;
typedef SymbolicExpr::SleSimplifier SleSimplifier;
typedef SymbolicExpr::SltSimplifier SltSimplifier;
typedef SymbolicExpr::UgeSimplifier UgeSimplifier;
typedef SymbolicExpr::UgtSimplifier UgtSimplifier;
typedef SymbolicExpr::UleSimplifier UleSimplifier;
typedef SymbolicExpr::UltSimplifier UltSimplifier;
typedef SymbolicExpr::ZeropSimplifier ZeropSimplifier;
typedef SymbolicExpr::SdivSimplifier SdivSimplifier;
typedef SymbolicExpr::SmodSimplifier SmodSimplifier;
typedef SymbolicExpr::UdivSimplifier UdivSimplifier;
typedef SymbolicExpr::UmodSimplifier UmodSimplifier;
typedef SymbolicExpr::ShiftSimplifier ShiftSimplifier;
typedef SymbolicExpr::ShlSimplifier ShlSimplifier;
typedef SymbolicExpr::ShrSimplifier ShrSimplifier;
typedef SymbolicExpr::LssbSimplifier LssbSimplifier;
typedef SymbolicExpr::MssbSimplifier MssbSimplifier;

} // namespace
} // namespace
} // namespace

#endif
