#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Debugger/Gdb.h>

#include <Rose/BinaryAnalysis/Debugger/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/StringUtility/Escape.h>
#include <rose_getline.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

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

const std::list<GdbResponse>&
Gdb::sendCommand(const std::string &cmd) {
    ASSERT_require(isAttached());
    resetResponses();

    // Read asynchronous responses that are already waiting
    while (true) {
        const auto list = readOptionalResponses();
        if (!list.empty()) {
            if (mlog[DEBUG]) {
                for (const GdbResponse &response: list)
                    mlog[DEBUG] <<response;
            }
        } else {
            break;
        }
    }

    // Send the command to GDB
    SAWYER_MESG_OR(mlog[TRACE], mlog[DEBUG]) <<"(gdb) -" <<cmd <<"\n";
    gdbInput_ <<"-" <<cmd <<"\n";
    gdbInput_.flush();

    // Read required response until we get a result record followed later by a end record.
    bool hasResult = false;
    while (!hasResult) {
        const auto list = readRequiredResponses();
        for (const GdbResponse &response: list) {
            if (response.result) {
                hasResult = true;
                break;
            }
        }
        if (mlog[DEBUG]) {
            for (const GdbResponse &response: list)
                mlog[DEBUG] <<response;
        }
    }

    // Read asynchronous responses until there aren't any more ready
    while (true) {
        const auto list = readOptionalResponses();
        if (!list.empty()) {
            if (mlog[DEBUG]) {
                for (const GdbResponse &response: list)
                    mlog[DEBUG] <<response;
            }
        } else {
            break;
        }
    }
    return responses_;
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

    if (mlog[TRACE]) {
        for (const GdbResponse &response: responses) {
            if (!response.console.empty())
                mlog[TRACE] <<response.console <<"\n";
        }
    }

    // Accumulate responses
    responses_.insert(responses_.end(), responses.begin(), responses.end());

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

const std::list<GdbResponse>&
Gdb::responses() const {
    return responses_;
}

void
Gdb::resetResponses() {
    responses_.clear();
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
    readRequiredResponses();

    // Attach to remote target
    sendCommand("target-select remote " + specimen.remote.host + ":" + boost::lexical_cast<std::string>(specimen.remote.port));

    // Get an appropriate disassembler for this architecture
    for (const GdbResponse &response: responses()) {
        if (GdbResponse::AsyncClass::STOPPED == response.exec.aclass) {
            if (const Yaml::Node &node = response.exec.results["frame"]["arch"]) {
                const std::string arch = node.as<std::string>();
                if ("m68k" == arch) {
                    disassembler_ = Disassembler::M68k::instance(m68k_freescale_cpu32);
                } else {
                    ASSERT_not_implemented("unrecognized architecture: " + arch);
                }
            }
        }
    }
    ASSERT_not_null(disassembler_);

    // Get the list of register names for this architecture.
    for (const GdbResponse &response: sendCommand("data-list-register-names")) {
        if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (Yaml::Node names = response.result.results["register-names"]) {
                ASSERT_require(names.isSequence());
                for (const auto &pair: names) {
                    const std::string name = pair.second.as<std::string>();
                    const RegisterDescriptor reg = registerDictionary()->find(name);
                    registers_.push_back(std::make_pair(name, reg));
                    if (!reg && !name.empty()) {
                        mlog[ERROR] <<"GDB register #" <<(registers_.size()-1) <<" \"" <<name <<"\""
                                    <<" is not present in dictionary \"" <<registerDictionary()->name() <<"\"\n";
                    }
                }
                break;
            }
        }
    }
    ASSERT_forbid(registers_.empty());

    resetResponses();
}

bool
Gdb::isAttached() {
    return gdbThread_.joinable();
}

void
Gdb::detach() {
    sendCommand("gdb-exit");
    registers_.clear();
    responses_.clear();
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

Sawyer::Optional<size_t>
Gdb::findRegisterIndex(RegisterDescriptor reg) const {
    ASSERT_forbid(registers_.empty());
    for (size_t i = 0; i < registers_.size(); ++i) {
        if (reg.majorNumber() == registers_[i].second.majorNumber() && reg.minorNumber() == registers_[i].second.minorNumber())
            return i;
    }
    return Sawyer::Nothing();
}

RegisterDescriptor
Gdb::findRegister(RegisterDescriptor reg) const {
    if (auto i = findRegisterIndex(reg)) {
        ASSERT_require(*i < registers_.size());
        return registers_[*i].second;
    } else {
        return RegisterDescriptor();
    }
}

Sawyer::Container::BitVector
Gdb::readRegister(ThreadId, RegisterDescriptor reg) {
    ASSERT_require(reg);
    resetResponses();

    // Find the GDB register of which `reg` is a part.
    Sawyer::Optional<size_t> idx = findRegisterIndex(reg);
    if (!idx)
        throw Exception("register " + boost::lexical_cast<std::string>(reg) + " not found in debugger");
    const RegisterDescriptor gdbReg  = registers_[*idx].second;

    // Send the data-list-register-values command and wait for its response. Then process the reponse to find
    // 'register-values=[{number="I", value="0xVALUE"}]' and parse the value. Extract from the value that portion to which
    // `reg` corresponds.
    for (const GdbResponse &response: sendCommand("data-list-register-values x " + boost::lexical_cast<std::string>(*idx))) {
        if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (const Yaml::Node sequence = response.result.results["register-values"]) {
                ASSERT_require(sequence.isSequence());
                for (const auto &elmt: sequence) {
                    ASSERT_require(elmt.second.isMap());
                    const size_t j = elmt.second["number"].as<size_t>();
                    ASSERT_require(j == *idx);
                    const std::string strval = elmt.second["value"].as<std::string>();
                    Sawyer::Container::BitVector whole(gdbReg.nBits());
                    ASSERT_require2(boost::starts_with(strval, "0x"), strval);
                    whole.fromHex(strval.substr(2));

                    if (reg == gdbReg) {
                        return whole;
                    } else {
                        Sawyer::Container::BitVector retval(reg.nBits());
                        const auto from = Sawyer::Container::BitVector::baseSize(reg.offset(), reg.nBits());
                        retval.copy(retval.hull(), whole, from);
                        return retval;
                    }
                }
            }
        }
    }

#if 1 // DEBUGGING [Robb Matzke 2022-10-19]
    std::cerr <<"ROBB: previous responses:\n";
    for (const GdbResponse &response: responses())
        std::cerr <<response;
    std::cerr <<"ROBB: next response:\n";
    for (const GdbResponse &response: readRequiredResponses())
        std::cerr <<response;
#endif
    throw Exception("no \"register-values\" response from debugger for \"data-list-register-values\"");
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

const std::vector<std::pair<std::string, RegisterDescriptor>>&
Gdb::registerNames() const {
    return registers_;
}

} // namespace
} // namespace
} // namespace

#endif
