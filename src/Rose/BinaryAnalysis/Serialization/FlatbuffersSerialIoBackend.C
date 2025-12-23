#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_FLATBUFFERS

#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/Serialization/FlatbufferStorage.h>

#include <unistd.h>

namespace Rose {
namespace BinaryAnalysis {

class FlatbuffersSerialOutputBackend final: public SerialIo::OutputBackend {
  public:
    std::vector<uint8_t> savePartitioner(
        const Partitioner2::PartitionerConstPtr &partitioner, 
        Serialization::ProgressCallback progress) override {
        
        Serialization::FlatbufferPartitionerSaver saver(partitioner);
        saver.save();

        auto buf = saver.buffer();
        ASSERT_require(buf.first != nullptr);
        const size_t nBytes = buf.second;

        // Report initial progress
        if (progress)
            progress(0, nBytes, "serializing-flatbuffers");

        // Copy the buffer to a vector
        std::vector<uint8_t> payload(buf.first, buf.first + nBytes);
        
        // Report completion
        if (progress)
            progress(nBytes, nBytes, "serializing-flatbuffers");
        
        return payload;
    }
};

class FlatbuffersSerialInputBackend final: public SerialIo::InputBackend {
  public:
    Partitioner2::PartitionerPtr loadPartitioner(
        const uint8_t* data, 
        size_t size,
        Serialization::ProgressCallback progress) override {
        
        // Report initial progress
        if (progress)
            progress(0, size, "deserializing-flatbuffers");
            
        // Create a copy of the data for the loader
        std::vector<uint8_t> bytes(data, data + size);
        
        // Create and verify the loader
        auto loader = Serialization::FlatbufferPartitionerLoader::fromBytes(std::move(bytes));
        if (!loader.verify())
            throw SerialIo::Exception("invalid FlatBuffers partitioner data");
            
        // Report completion
        if (progress)
            progress(size, size, "deserializing-flatbuffers");
            
        return loader.load();
    }
};

// Static initialization to register the FlatBuffers backends
namespace {
struct RegisterFlatBuffersBackends {
    RegisterFlatBuffersBackends() {
        SerialIo::registerBackend(
          {Serialization::FLATBUFFERS, []() { return std::make_unique<FlatbuffersSerialOutputBackend>(); },
           []() { return std::make_unique<FlatbuffersSerialInputBackend>(); }}
        );
    }
} registerFlatBuffersBackends;
} // namespace

// Legacy C-style factory functions - deprecated, kept for backward compatibility
extern "C" SerialIo::OutputBackend *
RoseBinaryAnalysis_makeFlatbuffersSerialOutputBackend() {
    return new FlatbuffersSerialOutputBackend;
}

extern "C" SerialIo::InputBackend *
RoseBinaryAnalysis_makeFlatbuffersSerialInputBackend() {
    return new FlatbuffersSerialInputBackend;
}

} // namespace BinaryAnalysis
} // namespace Rose

#endif
#endif
