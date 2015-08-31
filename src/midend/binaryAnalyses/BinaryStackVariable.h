#ifndef ROSE_BinaryAnalysis_StackVariable_H
#define ROSE_BinaryAnalysis_StackVariable_H

#include <BaseSemantics2.h>
#include <MemoryCellList.h>
#include <Sawyer/Interval.h>

namespace rose {
namespace BinaryAnalysis {

/** Information about the location of a stack variable. */
struct StackVariableLocation {
    int64_t offset;                                     /**< Signed offset from initial stack pointer. This is the low address. */
    size_t nBytes;                                      /**< Size of variable in bytes. */
    InstructionSemantics2::BaseSemantics::SValuePtr address; /**< Complete address, not just the stack offset. */
    StackVariableLocation()
        : offset(0), nBytes(0) {}
    StackVariableLocation(int64_t offset, size_t nBytes, const InstructionSemantics2::BaseSemantics::SValuePtr &address)
        : offset(offset), nBytes(nBytes), address(address) {}
    typedef Sawyer::Container::Interval<int64_t> Interval;
    Interval interval() const { return Interval::baseSize(offset, nBytes); }
};

/** Meta information for a stack variable. This is information that's not in @ref StackVariableLocation nor the value. */
struct StackVariableMeta {
    InstructionSemantics2::BaseSemantics::MemoryCellList::AddressSet writers; /**< Instructions that wrote to a location. */
    InstructionSemantics2::BaseSemantics::InputOutputPropertySet ioProperties; /**< Properties of a location. */
    StackVariableMeta() {}
    StackVariableMeta(const InstructionSemantics2::BaseSemantics::MemoryCellList::AddressSet &writers, 
                      const InstructionSemantics2::BaseSemantics::InputOutputPropertySet &ioProperties)
        : writers(writers), ioProperties(ioProperties) {}
    bool operator==(const StackVariableMeta &other) const {
        return writers == other.writers && ioProperties == other.ioProperties;
    }
};

/** A multi-byte variable that appears on the stack. */
struct StackVariable {
    StackVariableLocation location;                     /**< Location of the stack variable. */
    StackVariableMeta meta;                             /**< Meta-data for the stack variable. */
    StackVariable() {}
    StackVariable(const StackVariableLocation &location, const StackVariableMeta &meta)
        : location(location), meta(meta) {}
};

/** Multiple stack variables. */
typedef std::vector<StackVariable> StackVariables;

} // namespace
} // namespace

#endif
