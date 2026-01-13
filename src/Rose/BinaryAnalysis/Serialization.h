#ifndef ROSE_BinaryAnalysis_Serialization_H
#define ROSE_BinaryAnalysis_Serialization_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/SerialIo.h>

#include <Rose/BinaryAnalysis/Serialization/SerialFrame.h>

#ifdef ROSE_ENABLE_FLATBUFFERS
#include <Rose/BinaryAnalysis/Serialization/FlatBufferSchema.h>
#include <Rose/BinaryAnalysis/Serialization/FlatBuffers.h>
#endif

namespace Rose {
namespace BinaryAnalysis {

namespace Serialization {}

} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif