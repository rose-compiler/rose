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

typedef SymbolicExpr::TreeNode TreeNode ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::InternalNode InternalNode ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::LeafNode LeafNode ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");

typedef SymbolicExpr::TreeNodePtr TreeNodePtr ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::InternalNodePtr InternalNodePtr ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::LeafNodePtr LeafNodePtr ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");

typedef SymbolicExpr::TreeNodes TreeNodes ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::RenameMap RenameMap ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::Formatter Formatter ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");

using SymbolicExpr::VisitAction;
using SymbolicExpr::CONTINUE;
using SymbolicExpr::TRUNCATE;
using SymbolicExpr::TERMINATE;

using SymbolicExpr::MAX_NNODES;
typedef SymbolicExpr::Visitor Visitor ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");

typedef SymbolicExpr::Simplifier Simplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::AddSimplifier AddSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::AndSimplifier AndSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::OrSimplifier OrSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::XorSimplifier XorSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::SmulSimplifier SmulSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::UmulSimplifier UmulSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::ConcatSimplifier ConcatSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::ExtractSimplifier ExtractSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::AsrSimplifier AsrSim ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::InvertSimplifier InvertSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::NegateSimplifier NegateSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::IteSimplifier IteSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::NoopSimplifier NoopSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::RolSimplifier RolSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::RorSimplifier RorSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::UextendSimplifier UextendSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::SextendSimplifier SextendSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::EqSimplifier EqSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::SgeSimplifier SgeSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::SgtSimplifier SgtSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::SleSimplifier SleSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::SltSimplifier SltSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::UgeSimplifier UgeSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::UgtSimplifier UgtSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::UleSimplifier UleSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::UltSimplifier UltSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::ZeropSimplifier ZeropSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::SdivSimplifier SdivSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::SmodSimplifier SmodSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::UdivSimplifier UdivSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::UmodSimplifier UmodSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::ShiftSimplifier ShiftSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::ShlSimplifier ShlSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::ShrSimplifier ShrSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::LssbSimplifier LssbSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");
typedef SymbolicExpr::MssbSimplifier MssbSimplifier ROSE_DEPRECATED("use rose::BinaryAnalysis::SymbolicExpr instead");

} // namespace
} // namespace
} // namespace

#endif
