#ifndef ROSE_BinaryAnalysis_Concolic_Settings_H
#define ROSE_BinaryAnalysis_Concolic_Settings_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING

#include <Rose/BinaryAnalysis/Concolic/BasicTypes.h>
#include <Rose/BinaryAnalysis/Concolic/Emulation.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

/** Settings to control various aspects of an executor. */
class ConcolicExecutorSettings {
public:
    Partitioner2::EngineSettings partitionerEngine;
    Partitioner2::LoaderSettings loader;
    Partitioner2::DisassemblerSettings disassembler;
    Partitioner2::PartitionerSettings partitioner;
    Emulation::RiscOperators::Settings emulationSettings;

    bool traceSemantics;                            /** Whether to debug semantic steps by using a semantic tracer. */
    AddressIntervalSet showingStates;               /** Instructions after which to show the semantic state. */

    ~ConcolicExecutorSettings();
    ConcolicExecutorSettings();
};

} // namespace
} // namespace
} // namespace

#endif
#endif
