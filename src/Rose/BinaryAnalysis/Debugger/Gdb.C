#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Debugger/Gdb.h>

#include <Rose/BinaryAnalysis/Debugger/Exception.h>
#include <Rose/StringUtility/Escape.h>
#include <rose_getline.h>

#include <boost/algorithm/string/trim.hpp>

#include <list>
#include <string>
#include <thread>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

Gdb::Gdb()
    : gdbOutputPipe_(ios_) {}

Gdb::~Gdb() {
    if (isAttached())
        detach();
}

Gdb::Ptr
Gdb::instance() {
    return Ptr(new Gdb);
}

Gdb::Ptr
Gdb::instance(const Specimen &specimen) {
    auto gdb = instance();
    gdb->attach(specimen);
    return gdb;
}

// Reads lines of GDB output asynchronously and adds them to a queue of lines in a thread-safe manner.
class OutputHandler {
public:
    boost::asio::io_service &ios_;
    boost::process::async_pipe &gdbOutputPipe_;
    boost::asio::streambuf &gdbOutputBuffer_;
    Gdb::Fifo<std::string> &lines_;

public:
    explicit OutputHandler(boost::asio::io_service &ios, boost::process::async_pipe &gdbOutputPipe,
                           boost::asio::streambuf &gdbOutputBuffer, Gdb::Fifo<std::string> &lines)
        : ios_(ios), gdbOutputPipe_(gdbOutputPipe), gdbOutputBuffer_(gdbOutputBuffer), lines_(lines) {}

    void operator()(const boost::system::error_code &ec, size_t size) {
        if (!ec) {
            std::istream in(&gdbOutputBuffer_);
            std::string line = boost::trim_copy(rose_getline(in));
            //mlog[DEBUG] <<"from GDB command standard output: " <<line <<"\n";
            lines_.append(line);
            boost::asio::async_read_until(gdbOutputPipe_, gdbOutputBuffer_, "\n", *this);
        } else {
            //mlog[DEBUG] <<"GDB standard output closed\n";
            lines_.close();
        }
    }

};

void
Gdb::sendCommand(const std::string &cmd) {
    ASSERT_require(isAttached());

    // Read asynchronous responses that are already waiting
    while (true) {
        auto responses = readOptionalResponses();
        if (!responses.empty()) {
            for (const GdbResponse &response: responses)
                SAWYER_MESG(mlog[DEBUG]) <<response;
        } else {
            break;
        }
    }

    // Send the command to GDB
    SAWYER_MESG(mlog[DEBUG]) <<"GDB command: -" <<cmd <<"\n";
    gdbInput_ <<"-" <<cmd <<"\n";
    gdbInput_.flush();

    // Read required response
    auto responses = readRequiredResponses();
    for (const GdbResponse &response: responses)
        SAWYER_MESG(mlog[DEBUG]) <<response;

    // Read asynchronous responses until there aren't any more ready
    while (true) {
        auto responses = readOptionalResponses();
        if (!responses.empty()) {
            for (const GdbResponse &response: responses)
                SAWYER_MESG(mlog[DEBUG]) <<response;
        } else {
            break;
        }
    }
}

std::list<GdbResponse>
Gdb::readResponseSet(bool required) {
    std::list<GdbResponse> responses;

    // Get the first line of the response, which might be optional
    Sawyer::Optional<std::string> line = required ? gdbOutput_.blockingNext() : gdbOutput_.nonblockingNext();
    if (!line)
        return responses;
    responses.push_back(GdbResponse::parse(*line));
    if (responses.back().atEnd)
        return responses;

    // Since we had a first line of a response set, then keep reading until we get to the last line.
    while ((line = gdbOutput_.blockingNext())) {
        responses.push_back(GdbResponse::parse(*line));
        if (responses.back().atEnd)
            break;
    }
    return responses;
}

std::list<GdbResponse>
Gdb::readRequiredResponses() {
    return readResponseSet(true);
}

std::list<GdbResponse>
Gdb::readOptionalResponses() {
    return readResponseSet(false);
}

// Starts the GDB process and reads/writes
static void
gdbIoThread(const std::vector<std::string> &argv, OutputHandler &outputHandler, boost::process::opstream &gdbInput) {
    boost::process::child gdb(argv,
                              boost::process::std_out > outputHandler.gdbOutputPipe_,
                              boost::process::std_in < gdbInput);

    boost::asio::async_read_until(outputHandler.gdbOutputPipe_, outputHandler.gdbOutputBuffer_, "\n", outputHandler);
    outputHandler.ios_.run();
    gdb.wait();
    int result = gdb.exit_code();
}

void
Gdb::attach(const Specimen &specimen) {
    if (isAttached())
        throw Exception("already attached");

    std::vector<std::string> argv;
    argv.push_back(boost::process::search_path(specimen.gdbName).string());
    argv.push_back("-n");                               // skip initialization files
    argv.push_back("-q");                               // do not print introductory and copyright messages
    argv.push_back("--interpreter=mi");
    if (!specimen.executable.empty())
        argv.push_back(specimen.executable.string());

    OutputHandler outputHandler(ios_, gdbOutputPipe_, gdbOutputBuffer_, gdbOutput_);
    gdbThread_ = std::thread(gdbIoThread, argv, std::ref(outputHandler), std::ref(gdbInput_));
    auto responses = readRequiredResponses();
    for (const GdbResponse &response: responses)
        SAWYER_MESG(mlog[DEBUG]) <<response;

    sendCommand("target-select remote " + specimen.remote.host + ":" + boost::lexical_cast<std::string>(specimen.remote.port));
}

bool
Gdb::isAttached() {
    return gdbThread_.joinable();
}

void
Gdb::detach() {
    sendCommand("gdb-exit");
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

void
Gdb::terminate() {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

std::vector<ThreadId>
Gdb::threadIds() {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

void
Gdb::executionAddress(ThreadId, rose_addr_t) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

rose_addr_t
Gdb::executionAddress(ThreadId) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

void
Gdb::setBreakPoint(const AddressInterval&) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

void
Gdb::clearBreakPoint(const AddressInterval&) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

void
Gdb::clearBreakPoints() {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

void
Gdb::singleStep(ThreadId) {
    sendCommand("exec-next-instruction");
}

void
Gdb::runToBreakPoint(ThreadId) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

Sawyer::Container::BitVector
Gdb::readRegister(ThreadId, RegisterDescriptor) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

void
Gdb::writeRegister(ThreadId, RegisterDescriptor, const Sawyer::Container::BitVector&) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

void
Gdb::writeRegister(ThreadId, RegisterDescriptor, uint64_t value) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

size_t
Gdb::readMemory(rose_addr_t va, size_t nBytes, uint8_t *buffer) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

std::vector<uint8_t>
Gdb::readMemory(rose_addr_t va, size_t nBytes) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

Sawyer::Container::BitVector
Gdb::readMemory(rose_addr_t va, size_t nBytes, ByteOrder::Endianness order) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

size_t
Gdb::writeMemory(rose_addr_t va, size_t nBytes, const uint8_t *bytes) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

std::string
Gdb::readCString(rose_addr_t va, size_t maxBytes) {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

bool
Gdb::isTerminated() {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

std::string
Gdb::howTerminated() {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

RegisterDictionaryPtr
Gdb::registerDictionary() const {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

Disassembler::BasePtr
Gdb::disassembler() const {
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

} // namespace
} // namespace
} // namespace

#endif
