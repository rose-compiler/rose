#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION

#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/Partitioner2.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <sstream>

namespace Rose {
namespace BinaryAnalysis {

// Class for Boost-based serialization backend
class BoostSerialOutputBackend final: public SerialIo::OutputBackend {
  private:
    Serialization::Format format_;

  public:
    explicit BoostSerialOutputBackend(Serialization::Format format) : format_(format) {}

    std::vector<uint8_t> savePartitioner(
      const Partitioner2::PartitionerConstPtr& partitioner, Serialization::ProgressCallback progress
    ) override {

        // Report initial progress
        if (progress)
            progress(0, 0, "serializing-boost");

        // Use the raw pointer for serialization
        const Partitioner2::Partitioner* raw = partitioner.getRawPointer();

        // Serialize to an in-memory stream
        std::ostringstream   oss;
        std::vector<uint8_t> result;

        try {
            switch (format_) {
            case Serialization::BINARY: {
                boost::archive::binary_oarchive ar(oss);
                ar & raw;
                break;
            }
            case Serialization::TEXT: {
                boost::archive::text_oarchive ar(oss);
                ar & raw;
                break;
            }
            case Serialization::XML: {
                boost::archive::xml_oarchive ar(oss);
                ar&                          BOOST_SERIALIZATION_NVP(raw);
                break;
            }
            default:
                throw Serialization::Exception("unsupported format in Boost serialization backend");
            }

            // Convert to bytes
            std::string serialized = oss.str();
            result.assign(serialized.begin(), serialized.end());

            // Report completion
            if (progress)
                progress(result.size(), result.size(), "serializing-boost");

        } catch (const std::exception& e) {
            throw Serialization::Exception(std::string("Boost serialization failed: ") + e.what());
        }

        return result;
    }
};

// Class for Boost-based deserialization backend
class BoostSerialInputBackend final: public SerialIo::InputBackend {
  private:
    Serialization::Format format_;

  public:
    explicit BoostSerialInputBackend(Serialization::Format format) : format_(format) {}

    Partitioner2::PartitionerPtr
    loadPartitioner(const uint8_t* data, size_t size, Serialization::ProgressCallback progress) override {

        // Report initial progress
        if (progress)
            progress(0, size, "deserializing-boost");

        // Create a string from the binary data
        std::string        serialized(reinterpret_cast<const char*>(data), size);
        std::istringstream iss(serialized);

        Partitioner2::Partitioner* raw = nullptr;

        try {
            switch (format_) {
            case Serialization::BINARY: {
                boost::archive::binary_iarchive ar(iss);
                ar & raw;
                break;
            }
            case Serialization::TEXT: {
                boost::archive::text_iarchive ar(iss);
                ar & raw;
                break;
            }
            case Serialization::XML: {
                boost::archive::xml_iarchive ar(iss);
                ar&                          BOOST_SERIALIZATION_NVP(raw);
                break;
            }
            default:
                throw Serialization::Exception("unsupported format in Boost serialization backend");
            }

            // Report completion
            if (progress)
                progress(size, size, "deserializing-boost");

        } catch (const std::exception& e) {
            throw Serialization::Exception(std::string("Boost deserialization failed: ") + e.what());
        }

        return Partitioner2::PartitionerPtr(raw);
    }
};

// Static initialization to register the Boost backends
namespace {
struct RegisterBoostBackends {
    RegisterBoostBackends() {
        SerialIo::registerBackend(
          {Serialization::BINARY, []() { return std::make_unique<BoostSerialOutputBackend>(Serialization::BINARY); },
           []() { return std::make_unique<BoostSerialInputBackend>(Serialization::BINARY); }}
        );

        SerialIo::registerBackend(
          {Serialization::TEXT, []() { return std::make_unique<BoostSerialOutputBackend>(Serialization::TEXT); },
           []() { return std::make_unique<BoostSerialInputBackend>(Serialization::TEXT); }}
        );

        SerialIo::registerBackend(
          {Serialization::XML, []() { return std::make_unique<BoostSerialOutputBackend>(Serialization::XML); },
           []() { return std::make_unique<BoostSerialInputBackend>(Serialization::XML); }}
        );
    }
} registerBoostBackends;
} // namespace

} // namespace BinaryAnalysis
} // namespace Rose

#endif // ROSE_ENABLE_BOOST_SERIALIZATION
#endif // ROSE_ENABLE_BINARY_ANALYSIS
