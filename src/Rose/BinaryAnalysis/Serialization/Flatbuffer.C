#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/Serialization/FlatbufferStorage.h>

#include <unistd.h>

namespace Rose {
namespace BinaryAnalysis {

class FlatbufferSerializer final: public SerialIo::Serializer {
  public:
    std::vector<char> savePartitioner(
      const Partitioner2::PartitionerConstPtr& partitioner, Serialization::ProgressCallback progress
    ) override {

        Serialization::FlatbufferPartitionerSaver saver(partitioner);
        saver.save();

        auto buf = saver.buffer();
        ASSERT_require(buf.first != nullptr);
        const size_t nBytes = buf.second;

        // Report initial progress
        if (progress)
            progress(0, nBytes, "serializing-flatbuffers");

        // Copy the buffer to a vector
        std::vector<char> payload(buf.first, buf.first + nBytes);

        // Report completion
        if (progress)
            progress(nBytes, nBytes, "serializing-flatbuffers");

        return payload;
    }
};

class FlatbufferDeserializer final: public SerialIo::Deserializer {
  public:
    Partitioner2::PartitionerPtr
    loadPartitioner(const std::vector<char>& data, Serialization::ProgressCallback progress) override {

        // Report initial progress
        if (progress)
            progress(0, data.size(), "deserializing-flatbuffer");

        // Create and verify the loader
        auto loader = Serialization::FlatbufferPartitionerLoader::fromBytes(std::move(data));
        if (!loader.verify())
            throw Serialization::Exception("invalid FlatBuffer partitioner data");

        // Report completion
        if (progress)
            progress(data.size(), data.size(), "deserializing-flatbuffer");

        return loader.load();
    }
};

// Static initialization to register the FlatBuffers backends
namespace {
struct RegisterFlatBufferSerialization {
    RegisterFlatBufferSerialization() {
        SerialIo::registerSerialization({
          Serialization::FLATBUFFERS,                                  /* format */
          []() { return std::make_shared<FlatbufferDeserializer>(); }, /* deserializer factory */
          []() { return std::make_shared<FlatbufferSerializer>(); },   /* serializer factory */
        });
    }
} registerFlatBufferSerialization;
} // namespace

} // namespace BinaryAnalysis
} // namespace Rose

#endif