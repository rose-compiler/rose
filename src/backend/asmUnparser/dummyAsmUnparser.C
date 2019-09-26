// dummy function definitions to use when binary support is disabled.  ROSETTA still generates the binary IR node types, so
// we have to still define some of the functions they use.
#include "sage3basic.h"
#include "AsmUnparser_compat.h"

// Dummy declaration for the Rose::BinaryAnalysis namespace
namespace Rose {
namespace BinaryAnalysis {

size_t
AsmUnparser::unparse(std::ostream &output, SgNode *ast)
{
  return 0;
}

bool
AsmUnparser::unparse_one_node(std::ostream &output, SgNode *node)
{
  return false;
}

bool
AsmUnparser::unparse_insn(bool enabled, std::ostream &output, SgAsmInstruction *insn, size_t position_in_block)
{
  return false;
}

bool
AsmUnparser::unparse_basicblock(bool enabled, std::ostream &output, SgAsmBlock *block)
{
  return false;
}

bool
AsmUnparser::unparse_staticdata(bool enabled, std::ostream &output, SgAsmStaticData *data, size_t position_in_block)
{
  return false;
}

bool
AsmUnparser::unparse_datablock(bool enabled, std::ostream &output, SgAsmBlock *block)
{
  return false;
}

bool
AsmUnparser::unparse_function(bool enabled, std::ostream &output, SgAsmFunction *func)
{
  return false;
}

bool
AsmUnparser::unparse_interpretation(bool enabled, std::ostream &output, SgAsmInterpretation *interp)
{
  return false;
}

std::string
AsmUnparser::line_prefix() const
{
    char buf[256];
    return buf;
}

bool
AsmUnparser::InsnBlockSeparation::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnSkipBackBegin::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnFuncEntry::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnAddress::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnRawBytes::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnBlockEntry::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnStackDelta::operator()(bool enabled, const InsnArgs &args) {
    return enabled;
}

bool
AsmUnparser::InsnBody::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnNoEffect::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnComment::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnLineTermination::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InsnSkipBackEnd::operator()(bool enabled, const InsnArgs &args)
{
    return enabled;
}

/******************************************************************************************************************************
 *                                      Basic block callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::BasicBlockNoopUpdater::operator()(bool enabled, const BasicBlockArgs &args)
{
    return enabled;
}

bool
AsmUnparser::BasicBlockNoopWarning::operator()(bool enabled, const BasicBlockArgs &args)
{
    return enabled;
}

bool
AsmUnparser::BasicBlockReasons::operator()(bool enabled, const BasicBlockArgs &args)
{
    return enabled;
}

bool
AsmUnparser::BasicBlockBody::operator()(bool enabled, const BasicBlockArgs &args)
{
    return enabled;
}

bool
AsmUnparser::BasicBlockOutgoingStackDelta::operator()(bool enabled, const BasicBlockArgs &args)
{
    return enabled;
}

bool
AsmUnparser::BasicBlockSuccessors::operator()(bool enabled, const BasicBlockArgs &args)
{
    return enabled;
}

bool
AsmUnparser::BasicBlockLineTermination::operator()(bool enabled, const BasicBlockArgs &args)
{
    return enabled;
}

bool
AsmUnparser::BasicBlockCleanup::operator()(bool enabled, const BasicBlockArgs &args)
{
    return enabled;
}

/******************************************************************************************************************************
 *                                      Static data callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::StaticDataBlockSeparation::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

bool
AsmUnparser::StaticDataSkipBackBegin::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

bool
AsmUnparser::StaticDataBlockEntry::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

bool
AsmUnparser::StaticDataRawBytes::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

bool
AsmUnparser::StaticDataDetails::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

bool
AsmUnparser::StaticDataComment::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

bool
AsmUnparser::StaticDataLineTermination::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

void
AsmUnparser::StaticDataDisassembler::reset()
{
}

void
AsmUnparser::StaticDataDisassembler::init(Disassembler *d, AsmUnparser *u)
{
}

bool
AsmUnparser::StaticDataDisassembler::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

bool
AsmUnparser::StaticDataSkipBackEnd::operator()(bool enabled, const StaticDataArgs &args)
{
    return enabled;
}

/******************************************************************************************************************************
 *                                      Data block callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::DataBlockBody::operator()(bool enabled, const DataBlockArgs &args)
{
    return enabled;
}

bool
AsmUnparser::DataBlockLineTermination::operator()(bool enabled, const DataBlockArgs &args)
{
    return enabled;
}

/******************************************************************************************************************************
 *                                      Function callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::FunctionEntryAddress::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionSeparator::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionReasons::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionName::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionLineTermination::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionComment::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionPredecessors::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionSuccessors::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionAttributes::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

bool
AsmUnparser::FunctionBody::operator()(bool enabled, const FunctionArgs &args)
{
    return enabled;
}

/******************************************************************************************************************************
 *                                      Interpretation callbacks
 ******************************************************************************************************************************/

bool
AsmUnparser::InterpName::operator()(bool enabled, const InterpretationArgs &args)
{
    return enabled;
}

bool
AsmUnparser::InterpBody::operator()(bool enabled, const InterpretationArgs &args)
{
    return enabled;
}
void AsmUnparser::initDiagnostics() {}

void AsmUnparser::init()
{}

bool
AsmUnparser::BasicBlockPredecessors::operator()(bool enabled, const BasicBlockArgs &args) {
    return false;
}

bool
AsmUnparser::is_unparsable_node(SgNode *node)
{return false;}

const RegisterDictionary *
AsmUnparser::get_registers() const
{
    return user_registers ? user_registers : interp_registers;
}

SgNode *
AsmUnparser::find_unparsable_node(SgNode *ast)
{
    SgNode *root = NULL;
    return root;
}

std::vector<SgNode*>
AsmUnparser::find_unparsable_nodes(SgNode *ast) {
    return std::vector<SgNode*>();
}

} // namespace
} // namespace

std::string unparseExpression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*,
                              const Rose::BinaryAnalysis::RegisterDictionary*) {
    abort();
}

