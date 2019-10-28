#include <sage3basic.h>
#include <BaseSemanticsMerger.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

Merger::Merger()
    : memoryAddressesMayAlias_(true), memoryMergeDebugging_(false) {}

Merger::~Merger() {}

Merger::Ptr
Merger::instance() {
    return Ptr(new Merger);
}

} // namespace
} // namespace
} // namespace
} // namespace
