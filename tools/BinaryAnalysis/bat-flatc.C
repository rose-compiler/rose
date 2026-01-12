static const char* purpose = "convert between ROSE rba files and FlatBuffer partitioner encodings";
static const char* description =
  "This tool interoperates between the ROSE rba format and raw FlatBuffer partitioner files. "
  "It can produce an RBA file from a FlatBuffer schema and vice versa. "
  "The conversion direction is determined by the input file extension (.fbs for FlatBuffer, .rba for RBA).\n\n"
  "For FlatBuffer to RBA conversion (.fbs input), when the serialization format is set to FlatBuffer, "
  "the tool will directly inline the input FlatBuffer binary into a framed RBA file. With other formats, "
  "it will rebuild the partitioner from the FlatBuffer data and save it using the specified format.\n\n"
  "For RBA to FlatBuffer conversion (.rba input), the specified format is used to load the partitioner "
  "from the input file before converting it to FlatBuffer format.\n\n";

#include <batSupport.h>
#include <rose.h>

#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>

#include <Rose/BinaryAnalysis/SerialIo.h>
#include <Rose/BinaryAnalysis/Serialization/FlatBuffer.h>
#include <Rose/BinaryAnalysis/Serialization/SerialFrame.h>

#include <Sawyer/Stopwatch.h>

#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace Rose;
using namespace Sawyer::Message::Common;
using namespace Rose::BinaryAnalysis;
namespace FB = Rose::BinaryAnalysis::Serialization::FlatBuffers;

namespace {

Sawyer::Message::Facility mlog;

struct Settings {
    // Tool-specific command-line settings
    boost::filesystem::path inputFileName; // positional
    // Output command-line settings
    boost::filesystem::path outputFileName;
    // Desired ROSE rba format
    Serialization::Format stateFormat;

    Settings() : outputFileName("-"), stateFormat(Serialization::BINARY) {}
};

// Build a command line parser without running it
Sawyer::CommandLine::Parser
createSwitchParser(Settings& settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup output("Output switches");
    output.insert(
      Switch("output", 'o')
        .argument("filename", anyParser(settings.outputFileName))
        .doc(
          "Send tool output to the specified file instead of standard output. This tool will refuse to write binary "
          "data to standard "
          "output if it appears to be the terminal; if you really need that, pipe this tool's output through "
          "@man{cat}{1}.  Specifying the output name \"-\" (a single hyphen) is the explicit way of saying that "
          "output should be sent to standard output."
        )
    );

    output.insert(
      Bat::stateFileFormatSwitch(settings.stateFormat)
        .doc(
          "Specifies the serialization format for RBA files. For FlatBuffer to RBA conversion, when set to "
          "flatbuffer, the tool directly inlines the input FlatBuffer binary. With other formats, it rebuilds "
          "the partitioner and saves it in the specified format. For RBA to FlatBuffer conversion, this format "
          "is used to load the partitioner from the input file."
        )
    );

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] @v{flatbuffer.fbs|specimen.rba}");
    parser.errorStream(mlog[FATAL]);
    parser.with(output);
    parser.with(Rose::CommandLine::genericSwitches());

    return parser;
}

// Convert FlatBuffer file to RBA file
void fbToRBA(const boost::filesystem::path& inputFileName, const boost::filesystem::path& outputFileName, Serialization::Format stateFormat) {
    mlog[INFO] << "Converting FlatBuffer to RBA: " << inputFileName << " -> " << outputFileName << "\n";
    
    // Read the FlatBuffer file
    std::ifstream inFile(inputFileName.string().c_str(), std::ios::binary);
    if (!inFile) {
        throw Exception("cannot open input file: " + inputFileName.string());
    }
    
    std::vector<char> fbData;
    inFile.seekg(0, std::ios::end);
    size_t size = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    fbData.resize(size);
    inFile.read(fbData.data(), size);
    inFile.close();
    
    if (stateFormat == Serialization::FLATBUFFERS) {
        // Just wrap the FlatBuffer data in a framed RBA file
        mlog[INFO] << "Using direct FlatBuffer inlining\n";
        
        // Create a frame record with the FlatBuffer data
        Serialization::FrameRecord frameRecord(Serialization::PARTITIONER, Serialization::FLATBUFFERS);
        frameRecord.payload(fbData);
        
        // Write to output file using SerialFrame
        std::shared_ptr<Serialization::SerialFrame> frame = std::make_shared<Serialization::SerialFrame>();
        frame->openForWrite(outputFileName, nullptr);
        frame->writeFileHeader();
        frame->writeFrameRecord(frameRecord);
        frame->close();
    } else {
        // Use the SerialIO interface to rebuild the partitioner and save it
        mlog[INFO] << "Rebuilding partitioner\n";
        
        // Create and verify the loader
        auto loader = FB::Deserializer::fromBytes(fbData);
        if (!loader.verify()) {
            throw Exception("invalid FlatBuffer partitioner data in " + inputFileName.string());
        }
        
        // Load the partitioner
        auto partitioner = loader.load();
        if (!partitioner) {
            throw Exception("failed to load partitioner from FlatBuffer data");
        }
        
        // Save the partitioner using SerialIo
        SerialOutput::Ptr saver = SerialOutput::instance();
        saver->format(stateFormat);
        saver->open(outputFileName);
        saver->savePartitioner(partitioner);
        saver->close();
    }
    
    mlog[INFO] << "Conversion complete\n";
}

// Convert RBA file to FlatBuffer file
void rbaToFB(const boost::filesystem::path& inputFileName, const boost::filesystem::path& outputFileName, Serialization::Format stateFormat) {
    mlog[INFO] << "Converting RBA to FlatBuffer: " << inputFileName << " -> " << outputFileName << "\n";
    
    // Load the partitioner from the RBA file
    Partitioner2::PartitionerPtr partitioner;
    try {
        partitioner = Partitioner2::Partitioner::instanceFromRbaFile(inputFileName.string(), stateFormat);
        if (!partitioner) {
            throw Exception("failed to load partitioner from " + inputFileName.string());
        }
    } catch (const std::exception& e) {
        throw Exception("cannot load partitioner from " + inputFileName.string() + ": " + e.what());
    }
    
    // Serialize the partitioner to FlatBuffer
    FB::Serializer saver(partitioner);
    saver.save();
    
    // Write to output file
    try {
        saver.write(outputFileName);
    } catch (const std::exception& e) {
        throw Exception("failed to write FlatBuffer output to " + outputFileName.string() + ": " + e.what());
    }
    
    mlog[INFO] << "Conversion complete\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace

int
main(int argc, char* argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("Loading FlatBuffers file");

    Bat::checkRoseVersionNumber(MINIMUM_ROSE_LIBRARY_VERSION, mlog[FATAL]);
    Bat::registerSelfTests();

    // Create and parse settings
    Settings settings;
    auto     parser = createSwitchParser(settings);

    std::vector<std::string> input = parser.parse(argc, argv).apply().unreachedArgs();

    if (input.empty()) {
        mlog[FATAL] << "no input file specified; see --help\n";
        exit(1);
    }

    settings.inputFileName = input[0];
    Bat::checkRbaOutput(settings.outputFileName, mlog);

    try {
        // Determine conversion direction based on file extension
        std::string ext = settings.inputFileName.extension().string();
        
        if (boost::iequals(ext, ".fbs")) {
            // FlatBuffer to RBA conversion
            fbToRBA(settings.inputFileName, settings.outputFileName, settings.stateFormat);
        } else if (boost::iequals(ext, ".rba")) {
            // RBA to FlatBuffer conversion
            rbaToFB(settings.inputFileName, settings.outputFileName, settings.stateFormat);
        } else {
            mlog[FATAL] << "unsupported file extension: " << ext << "; expected .fbs or .rba\n";
            exit(1);
        }
    } catch (const std::exception& e) {
        mlog[FATAL] << "conversion failed: " << e.what() << "\n";
        exit(1);
    }

    return 0;
}
