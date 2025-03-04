#include <featureTests.h>
#ifdef ROSE_ENABLE_DEBUGGER_GDB
#include <Rose/BinaryAnalysis/Debugger/Gdb.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Debugger/Exception.h>
#include <Rose/BinaryAnalysis/Disassembler/M68k.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>
#include <rose_getline.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <list>
#include <string>
#include <thread>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gdb::Specimen
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Gdb::Specimen::Specimen() {}

Gdb::Specimen::Specimen(const boost::filesystem::path &exeName)
    : executable_(exeName) {}

Gdb::Specimen::Specimen(const boost::filesystem::path &exeName, const std::string &host, uint16_t port)
    : executable_(exeName) {
    remote_.host = host;
    remote_.port = port;
}

const boost::filesystem::path&
Gdb::Specimen::gdbName() const {
    return gdbName_;
}

void
Gdb::Specimen::gdbName(const boost::filesystem::path &p) {
    gdbName_ = p;
}

const boost::filesystem::path&
Gdb::Specimen::executable() const {
    return executable_;
}

void
Gdb::Specimen::executable(const boost::filesystem::path &p) {
    executable_ = p;
}

const Gdb::Specimen::Remote&
Gdb::Specimen::remote() const {
    return remote_;
}

void
Gdb::Specimen::remote(const Gdb::Specimen::Remote &r) {
    remote_ = r;
}

void
Gdb::Specimen::remote(const std::string &host, uint16_t port) {
    remote(Gdb::Specimen::Remote{host, port});
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gdb
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    void operator()(const boost::system::error_code &ec, size_t /*size*/) {
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
    SAWYER_MESG_OR(mlog[TRACE], mlog[DEBUG]) <<"(gdb) " <<cmd <<"\n";
    gdbInput_ <<cmd <<"\n";
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
gdbIoThread(const std::vector<std::string> &argv, OutputHandler &outputHandler, boost::process::opstream &gdbInput,
            std::promise<int> &&exitCodePromise) {
    boost::process::child gdb(argv,
                              boost::process::std_out > outputHandler.gdbOutputPipe_,
                              boost::process::std_in < gdbInput);

    boost::asio::async_read_until(outputHandler.gdbOutputPipe_, outputHandler.gdbOutputBuffer_, "\n", outputHandler);
    outputHandler.ios_.run();
    gdb.wait();
    exitCodePromise.set_value(gdb.exit_code());
}

void
Gdb::attach(const Specimen &specimen) {
    if (isAttached())
        throw Exception("already attached");

    std::vector<std::string> argv;
    argv.push_back(boost::process::search_path(specimen.gdbName()).string());
    if (argv.back().empty())
        throw Exception("cannot find GDB executable \"" + StringUtility::cEscape(specimen.gdbName().string()) + "\"");
    argv.push_back("-n");                               // skip initialization files
    argv.push_back("-q");                               // do not print introductory and copyright messages
    argv.push_back("--interpreter=mi");
    if (!specimen.executable().empty())
        argv.push_back(specimen.executable().string());

    std::promise<int> exitCodePromise;
    exitCodeFuture_ = exitCodePromise.get_future();
    exitCode_ = Sawyer::Nothing();
    OutputHandler outputHandler(ios_, gdbOutputPipe_, gdbOutputBuffer_, gdbOutput_);
    gdbThread_ = std::thread(gdbIoThread, argv, std::ref(outputHandler), std::ref(gdbInput_), std::move(exitCodePromise));
    readRequiredResponses();

    // Attach to remote target
    sendCommand("-target-select remote " + specimen.remote().host + ":" + boost::lexical_cast<std::string>(specimen.remote().port));

    // Get an appropriate disassembler for this architecture
    for (const GdbResponse &response: responses()) {
        if (GdbResponse::AsyncClass::STOPPED == response.exec.aclass) {
            if (const Yaml::Node &node = response.exec.results["frame"]["arch"]) {
                const std::string arch = node.as<std::string>();
                if ("m68k" == arch) {
                    disassembler_ = Architecture::findByName("nxp-coldfire").orThrow()->newInstructionDecoder();
                } else if ("m68k:isa-a:nodiv" == arch) {
                    disassembler_ = Architecture::findByName("nxp-coldfire").orThrow()->newInstructionDecoder();
                } else {
                    ASSERT_not_implemented("unrecognized architecture: " + arch);
                }
            }
        }
    }
    ASSERT_not_null(disassembler_);

    // Get the list of register names for this architecture.
    for (const GdbResponse &response: sendCommand("-data-list-register-names")) {
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
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    sendCommand("-gdb-exit");
    gdbThread_.join();

    gdbOutput_.clear();
    registers_.clear();
    responses_.clear();
    gdbBreakPoints_.clear();
    breakPoints_.clear();
}

void
Gdb::terminate() {
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    sendCommand("kill");
    detach();
}

std::vector<ThreadId>
Gdb::threadIds() {
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    ASSERT_not_implemented("[Robb Matzke 2022-10-17]");
}

bool
Gdb::gdbHandlesBreakPoints() const {
    return breakPoints_.isEmpty() || !gdbBreakPoints_.empty();
}

void
Gdb::setBreakPoint(const AddressInterval &where) {
    if (!isAttached())
        throw Exception("not attached to subordinate process");

    if (where.isSingleton() && gdbHandlesBreakPoints()) {
        // We're adding a break point for a single address, and GDB is currently handling all the break points (if
        // any). Therefore, make GDB handle this break point too.
        if (gdbBreakPoints_.find(where.least()) == gdbBreakPoints_.end()) {
            for (const auto &response: sendCommand("-break-insert *" + boost::lexical_cast<std::string>(where.least()))) {
                if (GdbResponse::ResultClass::DONE == response.result.rclass) {
                    if (Yaml::Node id = response.result.results["bkpt"]["number"]) {
                        gdbBreakPoints_.insert(std::make_pair(where.least(), id.as<unsigned>()));
                        break;
                    }
                }
            }
        }
    } else {
        // We're adding multiple addresses or we're adding a single address but we already have multi-address breakpoints not
        // handled by GDB. Therefore, handle the break points in ROSE.
        for (const auto &gdbBreakPoint: gdbBreakPoints_)
            sendCommand("-break-delete " + boost::lexical_cast<std::string>(gdbBreakPoint.second));
        gdbBreakPoints_.clear();
    }
    breakPoints_.insert(where);
}

void
Gdb::clearBreakPoint(const AddressInterval &where) {
    if (!isAttached())
        throw Exception("not attached to subordinate process");

    // Remove break points known to GDB
    for (auto iter = gdbBreakPoints_.begin(); iter != gdbBreakPoints_.end(); /*void*/) {
        if (where.contains(iter->first)) {
            sendCommand("-break-delete " + boost::lexical_cast<std::string>(iter->second));
            iter = gdbBreakPoints_.erase(iter);
        } else {
            ++iter;
        }
    }

    // Remove break points from ROSE
    breakPoints_.erase(where);
}

void
Gdb::clearBreakPoints() {
    if (!isAttached())
        throw Exception("not attached to subordinate process");

    // Remove break points known to GDB
    for (const auto &gdbBreakPoint: gdbBreakPoints_)
        sendCommand("-break-delete " + boost::lexical_cast<std::string>(gdbBreakPoint.second));
    gdbBreakPoints_.clear();

    // Remove break points from ROSE
    breakPoints_.clear();
}

void
Gdb::singleStep(ThreadId) {
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    sendCommand("-exec-step-instruction");
}

void
Gdb::runToBreakPoint(ThreadId tid) {
    if (!isAttached())
        throw Exception("not attached to subordinate process");

    if (gdbHandlesBreakPoints()) {
        // Continue, and block until the subordinate stops again
        sendCommand("-exec-continue");
        std::list<GdbResponse> latest = responses();
        while (true) {
            for (const GdbResponse &response: latest) {
                if (GdbResponse::AsyncClass::STOPPED == response.exec.aclass)
                    return;
            }
            latest = readRequiredResponses();
        }
    } else {
        // Single step until the subordinate hits one of our break points
        while (!isTerminated()) {
            singleStep(tid);
            if (breakPoints_.contains(executionAddress(tid)))
                return;
        }
    }
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

std::vector<RegisterDescriptor>
Gdb::availableRegisters() {
    std::vector<RegisterDescriptor> retval;
    retval.reserve(registers_.size());
    for (const auto &r: registers_)
        retval.push_back(r.second);
    return retval;
}

Sawyer::Container::BitVector
Gdb::readAllRegisters(ThreadId) {
    resetResponses();

    // Find the offset for each register, and the total number of bits for all registers.
    // offsets[i] is the bit offset in retval for the start of register i.
    std::vector<size_t> offsets;
    size_t totalBits = 0;
    offsets.reserve(registers_.size());
    for (size_t i = 0; i < registers_.size(); ++i) {
        offsets.push_back(i > 0 ? offsets.back() + registers_[i-1].second.nBits() : 0);
        totalBits += registers_[i].second.nBits();
    }

    // Ask GDB for all the register values
    Sawyer::Container::BitVector retval(totalBits);
    for (const GdbResponse &response: sendCommand("-data-list-register-values x")) {
        if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (const Yaml::Node sequence = response.result.results["register-values"]) {
                ASSERT_require(sequence.isSequence());
                for (const auto &elmt: sequence) {
                    ASSERT_require(elmt.second.isMap());
                    const size_t idx = elmt.second["number"].as<size_t>();
                    ASSERT_require(idx < registers_.size());
                    const RegisterDescriptor reg = registers_[idx].second;
                    const std::string strval = elmt.second["value"].as<std::string>();
                    ASSERT_require2(boost::starts_with(strval, "0x"), strval);
                    const auto range = Sawyer::Container::BitVector::BitRange::baseSize(offsets[idx], reg.nBits());
                    retval.fromHex(range, strval.substr(2));
                }
            }
        }
    }
    return retval;
}

Sawyer::Container::BitVector
Gdb::readRegister(ThreadId, RegisterDescriptor reg) {
    ASSERT_require(reg);
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    resetResponses();

    // Find the GDB register of which `reg` is a part.
    Sawyer::Optional<size_t> idx = findRegisterIndex(reg);
    if (!idx) {
        std::string s = "register ";
        if (const auto name = registerDictionary()->name(reg))
            s += *name + " ";
        s += reg.toString() + " not found in debugger";
        if (!registers_.empty()) {
            s += "\n  available registers are:";
            for (const std::pair<std::string, RegisterDescriptor> &r: registers_)
                s += (boost::format("\n    %-10s %s") % r.first % r.second.toString()).str();
        }
        throw Exception(s);
    }
    const RegisterDescriptor gdbReg = registers_[*idx].second;

    // Send the data-list-register-values command and wait for its response. Then process the reponse to find
    // 'register-values=[{number="I", value="0xVALUE"}]' and parse the value. Extract from the value that portion to which
    // `reg` corresponds.
    for (const GdbResponse &response: sendCommand("-data-list-register-values x " + boost::lexical_cast<std::string>(*idx))) {
        if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (const Yaml::Node sequence = response.result.results["register-values"]) {
                ASSERT_require(sequence.isSequence());
                for (const auto &elmt: sequence) {
                    ASSERT_require(elmt.second.isMap());
                    ASSERT_require(elmt.second["number"].as<size_t>() == *idx);
                    const std::string strval = elmt.second["value"].as<std::string>();
                    Sawyer::Container::BitVector whole(gdbReg.nBits());
                    ASSERT_require2(boost::starts_with(strval, "0x"), strval);
                    whole.fromHex(strval.substr(2));

                    if (reg == gdbReg) {
                        return whole;
                    } else {
                        Sawyer::Container::BitVector retval(reg.nBits());
                        retval.copy(retval.hull(), whole, reg.bits());
                        return retval;
                    }
                }
            }
        }
    }

    throw Exception("no \"register-values\" response from debugger for \"data-list-register-values\"");
}

void
Gdb::writeAllRegisters(ThreadId, const Sawyer::Container::BitVector &allValues) {
    // Calculate the starting bit offset for each register, and the total number of bits needed.
    // offsets[i] is the bit offset in retval for the start of register i
    std::vector<size_t> offsets;
    size_t totalBits = 0;
    offsets.reserve(registers_.size());
    for (size_t i = 0; i < registers_.size(); ++i) {
        offsets.push_back(i > 0 ? offsets.back() + registers_[i-1].second.nBits() : 0);
        totalBits += registers_[i].second.nBits();
    }
    ASSERT_always_require(allValues.size() >= totalBits);

    // GDB MI doesn't have a command to set multiple registers at the same time, so we have to do them one at a time.
    for (size_t i = 0; i < registers_.size(); ++i) {
        const auto range = Sawyer::Container::BitVector::BitRange::baseSize(offsets[i], registers_[i].second.nBits());
        const auto valueString = "0x" + allValues.toHex(range);
        sendCommand("-var-create temp_reg * $" + registers_[i].first);
        sendCommand("-var-assign temp_reg " + valueString);
        sendCommand("-var-delete temp_reg");
    }
}

void
Gdb::writeRegister(ThreadId tid, RegisterDescriptor reg, const Sawyer::Container::BitVector &value) {
    ASSERT_require(reg);
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    resetResponses();

    // Find the GDB register of which `reg` is a part.
    Sawyer::Optional<size_t> idx = findRegisterIndex(reg);
    if (!idx)
        throw Exception("register " + boost::lexical_cast<std::string>(reg) + " not found in debugger");
    const RegisterDescriptor gdbReg = registers_[*idx].second;
    Sawyer::Container::BitVector gdbValue;

    // Obtain the value to be written to the GDB register. If we're writing to only part of the register, then we need
    // to read it first.
    if (reg.bits() != gdbReg.bits()) {
        ASSERT_require(gdbReg.bits().contains(reg.bits()));
        gdbValue = readRegister(tid, gdbReg);
        const auto to = Sawyer::Container::BitVector::BitRange::baseSize(reg.offset() - gdbReg.offset(), reg.nBits());
        const auto from = value.hull();
        gdbValue.copy(to, value, from);
    } else {
        gdbValue = value;
    }

    // Write the value.  There is no command to write directly to a register (not that I can find), so we need to
    // create a variable first, then write to the variable, and finally delete the variable.
    sendCommand("-var-create temp_reg * $" + registers_[*idx].first);
    sendCommand("-var-assign temp_reg 0x" + gdbValue.toHex());
    sendCommand("-var-delete temp_reg");
}

void
Gdb::writeRegister(ThreadId tid, RegisterDescriptor reg, uint64_t value) {
    using namespace Sawyer::Container;
    BitVector bits(reg.nBits());
    bits.fromInteger(value);
    writeRegister(tid, reg, bits);
}

size_t
Gdb::readMemory(Address va, size_t nBytes, uint8_t *buffer) {
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    sendCommand("-data-read-memory-bytes " + boost::lexical_cast<std::string>(va) + " " + boost::lexical_cast<std::string>(nBytes));
    for (const GdbResponse &response: responses()) {
        if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (const Yaml::Node memoryList = response.result.results["memory"]) {
                ASSERT_require(memoryList.isSequence());
                for (const auto &elmt: memoryList) {
                    ASSERT_require(elmt.second.isMap());

                    // When reading address 0xffffffff, GDB's "end" marker will overflow to zero.  It would have been better if they
                    // had used a "least" and "greatest" address (akin to C++ library "front" and "back" iterators or Sawyer's
                    // "Interval" type), but they didn't. Therefore we need to handle this situation as a special case.
                    const Address begin = elmt.second["begin"].as<Address>() + elmt.second["offset"].as<Address>();
                    const Address end = elmt.second["end"].as<Address>();
                    ASSERT_require2(end > begin || (begin > 0 && end == 0),
                                    "va=" + StringUtility::addrToString(va) +
                                    ", nBytes=" + StringUtility::addrToString(nBytes) +
                                    ", begin=" + StringUtility::addrToString(begin) +
                                    ", end=" + StringUtility::addrToString(end));
                    const size_t nBytes = end > 0 ? end - begin : 0xffffffff - begin + 1;

                    // Convert the hexadecimal string to bytes
                    const std::string content = elmt.second["contents"].as<std::string>();
                    ASSERT_require(content.size() == 2 * nBytes);
                    for (size_t i = 0; i < nBytes; ++i) {
                        buffer[i] = 16 * StringUtility::hexadecimalToInt(content[2*i]) +
                                    StringUtility::hexadecimalToInt(content[2*i+1]);
                    }
                    return nBytes;
                }
            }
        }
    }
    return 0;
}

std::vector<uint8_t>
Gdb::readMemory(Address va, size_t nBytes) {
    std::vector<uint8_t> retval(nBytes);
    nBytes = readMemory(va, nBytes, retval.data());
    retval.resize(nBytes);
    return retval;
}

Sawyer::Container::BitVector
Gdb::readMemory(Address va, size_t nBytes, ByteOrder::Endianness sex) {
    std::vector<uint8_t> bytes = readMemory(va, nBytes);
    if (bytes.size() < nBytes)
        throw Exception("short read at " + StringUtility::addrToString(va));

    Sawyer::Container::BitVector retval(8 * nBytes);
    for (size_t i = 0; i < nBytes; ++i) {
        Sawyer::Container::BitVector::BitRange where;
        switch (sex) {
            case ByteOrder::ORDER_LSB:
                where = Sawyer::Container::BitVector::BitRange::baseSize(8*i, 8);
                break;
            case ByteOrder::ORDER_MSB:
                where = Sawyer::Container::BitVector::BitRange::baseSize(8*(nBytes - (i+1)), 8);
                break;
            default:
                ASSERT_not_reachable("invalid byte order");
        }
        retval.fromInteger(where, bytes[i]);
    }
    return retval;
}

size_t
Gdb::writeMemory(Address va, size_t nBytes, const uint8_t *bytes) {
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    std::string s = "-data-write-memory-bytes " + StringUtility::intToHex(va) + " \"";
    s.reserve(s.size() + 2*nBytes + 1);
    for (size_t i = 0; i < nBytes; ++i)
        s += (boost::format("%02x") % (unsigned)bytes[i]).str();
    s += "\"";
    sendCommand(s);
    for (const GdbResponse &response: responses()) {
        if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            return nBytes;
        }
    }
    throw Exception("write failed at " + StringUtility::addrToString(va) + " for " + StringUtility::plural(nBytes, "bytes"));
}

bool
Gdb::isTerminated() {
    return !gdbThread_.joinable() && (exitCodeFuture_.valid() || exitCode_);
}

std::string
Gdb::howTerminated() {
    if (exitCodeFuture_.valid())
        exitCode_ = exitCodeFuture_.get();
    if (exitCode_) {
        return "exited with status " + boost::lexical_cast<std::string>(*exitCode_);
    } else {
        return "";
    }
}

const std::vector<std::pair<std::string, RegisterDescriptor>>&
Gdb::registerNames() const {
    return registers_;
}

} // namespace
} // namespace
} // namespace

#endif
