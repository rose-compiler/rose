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
#include <ROSE_UNUSED.h>

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

void
Gdb::Specimen::print(std::ostream &out) const {
    out <<executable_;
    if (!remote_.host.empty())
        out <<" at tcp:" <<remote_.host <<":" <<remote_.port;
}

std::string
Gdb::Specimen::toString() const {
    return boost::lexical_cast<std::string>(*this);
}

std::ostream&
operator<<(std::ostream &out, const Gdb::Specimen &specimen) {
    specimen.print(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegInfo
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Gdb::RegInfo::RegInfo(const RegisterDescriptor reg)
    : reg_(reg) {}

Gdb::RegInfo::RegInfo(const size_t nBits)
    : nBits_(nBits) {}

RegisterDescriptor
Gdb::RegInfo::reg() const {
    return reg_;
}

size_t
Gdb::RegInfo::nBits() const {
    return reg_ ? reg_.nBits() : nBits_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gdb
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Gdb::Gdb()
    : gdbOutputPipe_(ioctx_) {}

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

bool
Gdb::checkWrites() const {
    return checkWrites_;
}

void
Gdb::checkWrites(bool b) {
    checkWrites_ = b;
}

// Reads lines of GDB output asynchronously and adds them to a queue of lines in a thread-safe manner.
class OutputHandler {
public:
    boost::asio::io_context &ioctx_;
    boost::process::async_pipe &gdbOutputPipe_;
    boost::asio::streambuf &gdbOutputBuffer_;
    Gdb::Fifo<std::string> &lines_;

public:
    explicit OutputHandler(boost::asio::io_context &ioctx, boost::process::async_pipe &gdbOutputPipe,
                           boost::asio::streambuf &gdbOutputBuffer, Gdb::Fifo<std::string> &lines)
        : ioctx_(ioctx), gdbOutputPipe_(gdbOutputPipe), gdbOutputBuffer_(gdbOutputBuffer), lines_(lines) {}

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
    outputHandler.ioctx_.run();
    gdb.wait();
    exitCodePromise.set_value(gdb.exit_code());
}

void
Gdb::attach(const Specimen &specimen) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream warn(mlog[WARN]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"command: attach(specimen=" <<specimen <<")\n";
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
    OutputHandler outputHandler(ioctx_, gdbOutputPipe_, gdbOutputBuffer_, gdbOutput_);
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
                } else if ("powerpc:common" == arch) {
                    disassembler_ = Architecture::findByName("ppc32-be").orThrow()->newInstructionDecoder();
                } else if ("i386" == arch) {
                    disassembler_ = Architecture::findByName("intel-pentium4").orThrow()->newInstructionDecoder();
                } else {
                    ASSERT_not_implemented("unrecognized architecture: " + arch);
                }
            }
        }
    }
    ASSERT_not_null(disassembler_);

    // Get the list of register names for this architecture.
    //
    //  The response JSON is an array of names, but some of those names might be empty. Empty names seem to be a result of the
    //  remote GDB server having a full list of names but providing only a subset based on the architecture. Our `registers_` list
    //  will include the empty names so that our register numbers (the vector index) are the same as GDBs register index.
    std::map<std::string, size_t> seen;                 // track registers we've seen so we can report duplicates
    for (const GdbResponse &response: sendCommand("-data-list-register-names")) {
        if (GdbResponse::ResultClass::ERROR == response.result.rclass) {
            SAWYER_MESG(error) <<response.result.results["msg"].as<std::string>() <<"\n";
        } else if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (Yaml::Node names = response.result.results["register-names"]) {
                ASSERT_require(names.isSequence());
                for (const auto &pair: names) {
                    const std::string name = pair.second.as<std::string>();
                    if (name.empty()) {
                        // GDB gave us an empty name, so push an empty placeholder.
                        registers_.push_back(std::make_pair("", RegInfo(0)));

                    } else if (const RegisterDescriptor reg = registerDictionary()->find(name)) {
                        // GDB gave us a register name that we know.
                        registers_.push_back(std::make_pair(name, RegInfo(reg)));

                    } else {
                        // GDB gave us a name that we don't know. At a minimum, we need to know its size, so we need to ask GDB for
                        // it. The response to this query will be something like "value=\"4\"".
                        for (const GdbResponse &response: sendCommand("-data-evaluate-expression \"sizeof($" + name + ")\"")) {
                            if (GdbResponse::ResultClass::ERROR == response.result.rclass) {
                                SAWYER_MESG(error) <<response.result.results["msg"].as<std::string>() <<"\n";
                            } else if (GdbResponse::ResultClass::DONE == response.result.rclass) {
                                const size_t nbits = 8 * response.result.results["value"].as<size_t>();
                                registers_.push_back(std::make_pair(name, RegInfo(nbits)));
                            }
                        }
                    }

                    // Check for duplicates. This could happen for many different reasons given GDB and/or QEMUs history and the way
                    // they organize their registers.
                    ASSERT_forbid(registers_.empty());
                    if (!registers_.back().first.empty()) {
                        const size_t idx = registers_.size() - 1;
                        const auto inserted = seen.insert(std::make_pair(name, idx));
                        if (!inserted.second) {
                            SAWYER_MESG(warn) <<"register \"" <<name <<"\" at index " <<idx
                                              <<" was previously seen at index " <<inserted.first->second <<"\n";
                        }
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
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"command: detach()\n";

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
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"command: terminate()\n";

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
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"command: setBreakPoint(" <<StringUtility::addrToString(where) <<")\n";

    if (!isAttached())
        throw Exception("not attached to subordinate process");

    if (where.isSingleton() && gdbHandlesBreakPoints()) {
        // We're adding a break point for a single address, and GDB is currently handling all the break points (if
        // any). Therefore, make GDB handle this break point too.
        if (gdbBreakPoints_.find(where.least()) == gdbBreakPoints_.end()) {
            for (const auto &response: sendCommand("-break-insert *" + boost::lexical_cast<std::string>(where.least()))) {
                if (GdbResponse::ResultClass::ERROR == response.result.rclass) {
                    SAWYER_MESG(error) <<response.result.results["msg"].as<std::string>() <<"\n";
                } else if (GdbResponse::ResultClass::DONE == response.result.rclass) {
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
Gdb::setBreakPoints(const AddressIntervalSet &where) {
    for (const AddressInterval &interval: where.intervals())
        setBreakPoint(interval);
}

AddressIntervalSet
Gdb::breakPoints() {
    return breakPoints_;
}

void
Gdb::clearBreakPoint(const AddressInterval &where) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"command: clearBreakPoint(" <<StringUtility::addrToString(where) <<")\n";

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
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"command: clearBreakPoints()\n";

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
Gdb::singleStep(const ThreadId tid) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"command: singleStep(thread=" <<tid <<")\n";

    if (!isAttached())
        throw Exception("not attached to subordinate process");
    sendCommand("-exec-step-instruction");
}

void
Gdb::runToBreakPoint(const ThreadId tid) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"command: runToBreakPoint(thread=" <<tid <<")\n";

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
Gdb::findRegisterIndex(const RegisterDescriptor reg) const {
    ASSERT_forbid(registers_.empty());
    for (size_t i = 0; i < registers_.size(); ++i) {
        if (reg.majorNumber() == registers_[i].second.reg().majorNumber() &&
            reg.minorNumber() == registers_[i].second.reg().minorNumber())
            return i;
    }
    return Sawyer::Nothing();
}

RegisterDescriptor
Gdb::findRegister(const RegisterDescriptor reg) const {
    if (auto i = findRegisterIndex(reg)) {
        ASSERT_require(*i < registers_.size());
        return registers_[*i].second.reg();
    } else {
        return RegisterDescriptor();
    }
}

std::vector<RegisterDescriptor>
Gdb::availableRegisters() {
    std::vector<RegisterDescriptor> retval;
    retval.reserve(registers_.size());
    for (const auto &r: registers_) {
        if (r.second.reg())
            retval.push_back(r.second.reg());
    }
    return retval;
}

Sawyer::Container::BitVector
Gdb::readAllRegisters(const ThreadId tid) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"command: readAllRegisters(thread=" <<tid <<")\n";

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

#ifndef NDEBUG
    std::map<std::string /*name*/, std::pair<size_t /*index*/, std::string /*value*/>> seen;
#endif

    // Ask GDB for all the register values
    Sawyer::Container::BitVector retval(totalBits);
    for (const GdbResponse &response: sendCommand("-data-list-register-values x")) {
        if (GdbResponse::ResultClass::ERROR == response.result.rclass) {
            SAWYER_MESG(error) <<response.result.results["msg"].as<std::string>() <<"\n";
        } else if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (const Yaml::Node sequence = response.result.results["register-values"]) {
                ASSERT_require(sequence.isSequence());
                for (const auto &elmt: sequence) {
                    ASSERT_require(elmt.second.isMap());
                    const size_t idx = elmt.second["number"].as<size_t>();
                    ASSERT_require(idx < registers_.size());
                    const std::string &regName = registers_[idx].first;
                    ROSE_UNUSED(regName);
                    const size_t nBits = registers_[idx].second.nBits();
                    ASSERT_require2(nBits > 0, "idx = " + boost::lexical_cast<std::string>(idx) + ", name = \"" + regName + "\"");

                    const std::string hexstr = parseDataListRegisterValuesValue(elmt.second["value"].as<std::string>());

#ifndef NDEBUG
                    // Have we already seen this register with a different value?
                    {
                        const auto inserted = seen.insert(std::make_pair(regName, std::make_pair(idx, hexstr)));
                        if (!inserted.second && hexstr != inserted.first->second.second) {
                            SAWYER_MESG(error) <<"register \"" <<regName <<" at index " <<idx <<" with value " <<hexstr
                                               <<" was already seen at index " <<inserted.first->second.first
                                               <<" with value " <<inserted.first->second.second <<"\n";
                        }
                    }
#endif

                    // Insert the value into the bit vector we're returning.
                    const auto range = Sawyer::Container::BitVector::BitRange::baseSize(offsets[idx], nBits);
                    ASSERT_require(boost::starts_with(hexstr, "0x"));
                    retval.fromHex(range, hexstr.substr(2));
                }
            }
        }
    }
    return retval;
}

std::string
Gdb::parseDataListRegisterValuesValue(const std::string &valstr) {
    std::regex vectorRe("[^_a-zA-Z0-9]uint[0-9]+ = (0x[0-9a-f]+)");
    std::smatch found;
    if (boost::starts_with(valstr, "0x")) {
        return valstr;
    } else if (std::regex_search(valstr, found, vectorRe)) {
        return found.str(1);
    } else {
        ASSERT_not_implemented("unrecognized output from -data-list-register-values: \"" + valstr + "\"");
    }
}

Sawyer::Container::BitVector
Gdb::readRegisterIndexed(const ThreadId tid, const size_t idx) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"command: readRegister(thread=" <<tid <<", index=" <<idx <<")\n";
    ASSERT_require(idx < registers_.size());
    ASSERT_forbid(registers_[idx].first.empty());       // GDB never gave us a name for this register

    if (!isAttached())
        throw Exception("not attached to subordinate process");
    resetResponses();

    for (const GdbResponse &response: sendCommand("-data-list-register-values x " + boost::lexical_cast<std::string>(idx))) {
        if (GdbResponse::ResultClass::ERROR == response.result.rclass) {
            SAWYER_MESG(error) <<response.result.results["msg"].as<std::string>() <<"\n";
        } else if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (const Yaml::Node sequence = response.result.results["register-values"]) {
                ASSERT_require(sequence.isSequence());
                for (const auto &elmt: sequence) {
                    ASSERT_require(elmt.second.isMap());
                    ASSERT_require(elmt.second["number"].as<size_t>() == idx);

                    // Hexadecimal string value for the register
                    const std::string hexstr = parseDataListRegisterValuesValue(elmt.second["value"].as<std::string>());

                    // Insert the value into the bit vector we're returning.
                    Sawyer::Container::BitVector retval(registers_[idx].second.nBits());
                    retval.fromHex(hexstr.substr(2));
                    return retval;
                }
            }
        }
    }
    throw Exception("no \"register-values\" response from debugger for \"data-list-register-values\"");
}

Sawyer::Container::BitVector
Gdb::readRegister(const ThreadId tid, const RegisterDescriptor reg) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"command: readRegister(thread=" <<tid <<", reg=" <<reg <<")\n";

    ASSERT_require(reg);
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    resetResponses();

    // Find the GDB register of which `reg` is a part, then read it using the GDB register index.
    Sawyer::Optional<size_t> idx = findRegisterIndex(reg);
    if (!idx) {
        std::string s = "register ";
        if (const auto name = registerDictionary()->name(reg))
            s += *name + " ";
        s += reg.toString() + " not found in debugger";
        if (!registers_.empty()) {
            s += "\n  available registers are:";
            for (const std::pair<std::string, RegInfo> &r: registers_)
                s += (boost::format("\n    %-10s %s") % r.first % r.second.reg().toString()).str();
        }
        throw Exception(s);
    }
    return readRegisterIndexed(tid, *idx);
}

void
Gdb::writeAllRegisters(const ThreadId tid, const Sawyer::Container::BitVector &allValues) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"command: writeAllRegisters(thread=" <<tid <<")\n";

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
    for (size_t register_idx = 0; register_idx < registers_.size(); ++register_idx) {
        if (const size_t nBits = registers_[register_idx].second.nBits()) {
            const std::string &regName = registers_[register_idx].first;
            ASSERT_forbid(regName.empty());
            const auto range = Sawyer::Container::BitVector::BitRange::baseSize(offsets[register_idx], nBits);
            const std::string valueString = "0x" + allValues.toHex(range);

            if (nBits <= 64 || nBits == 80) {
#if 1 // [Robb Matzke 2025-07-24]: this seems to work and is faster than the three commands below.
                sendCommand("-gdb-set $" + regName + "=" + valueString);
#else // [Robb Matzke 2025-07-25]: this also works, and I'm saving it here in case there's a problem with the one above.
                sendCommand("-var-create temp_reg * $" + regName);
                sendCommand("-var-assign temp_reg " + valueString);
                sendCommand("-var-delete temp_reg");
#endif
            } else if (nBits % 32 == 0) {
                // Vector register that can be treated as an array of 32-bit integers.
                const size_t nWords = nBits / 32;
                for (size_t word_idx = 0; word_idx < nWords; ++word_idx) {
                    const auto range = Sawyer::Container::BitVector::BitRange::baseSize(offsets[register_idx] + word_idx*32, 32);
                    const std::string valueString = "0x" + allValues.toHex(range);
                    const auto cmd = boost::format("-gdb-set $%1%.v%2%_int32[%3%]=%4%") % regName % nWords % word_idx % valueString;
                    sendCommand(cmd.str());
                }
            } else {
                ASSERT_not_implemented("nBits = " + boost::lexical_cast<std::string>(nBits));
            }

            // Check that the register was written successfully.
            if (checkWrites_) {
                const std::string check = "0x" + readRegisterIndexed(tid, register_idx).toHex();
                ASSERT_always_require2(valueString == check,
                                       "write to register \"" + regName + "\""
                                       " at index " + boost::lexical_cast<std::string>(register_idx) +
                                       " failed; wrote " + valueString + " but read back " + check);
            }
        } else {
            // This was a register slot for which GDB didn't originally give a name when we first attached.
            ASSERT_require(registers_[register_idx].first.empty());
        }
    }
}

void
Gdb::writeRegister(const ThreadId tid, const RegisterDescriptor reg, const Sawyer::Container::BitVector &value) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"command: writeRegister(thread=" <<tid <<", reg=" <<reg <<", value=0x" <<value.toHex()<<")\n";

    ASSERT_require(reg);
    if (!isAttached())
        throw Exception("not attached to subordinate process");
    resetResponses();

    // Find the GDB register of which `reg` is a part.
    Sawyer::Optional<size_t> idx = findRegisterIndex(reg);
    if (!idx)
        throw Exception("register " + boost::lexical_cast<std::string>(reg) + " not found in debugger");
    const std::string &regName = registers_[*idx].first;
    const RegisterDescriptor gdbReg = registers_[*idx].second.reg();
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

#if 1 // [Robb Matzke 2025-07-25]: this seems to work and is faster than the three commands below.
    sendCommand("-gdb-set $" + regName + "=0x" + gdbValue.toHex());
#else // [Robb Matzke 2025-07-25]: this also works, and I'm saving it here in case there's a problem with the one above.
    sendCommand("-var-create temp_reg * $" + regName);
    sendCommand("-var-assign temp_reg 0x" + gdbValue.toHex());
    sendCommand("-var-delete temp_reg");
#endif

    if (checkWrites_) {
        // Check that the register was written successfully.
        const std::string check = readRegisterIndexed(tid, *idx).toHex();
        ASSERT_always_require2(gdbValue.toHex() == check,
                               "write to register \"" + regName + "\" at index " + boost::lexical_cast<std::string>(*idx) +
                               " failed; wrote 0x" + gdbValue.toHex() + " but read back 0x" + check);
    }
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
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"command: readMemory(addr=" <<StringUtility::addrToString(va) <<", nbytes=" <<nBytes <<")\n";

    if (!isAttached())
        throw Exception("not attached to subordinate process");
    sendCommand("-data-read-memory-bytes " + boost::lexical_cast<std::string>(va) + " " + boost::lexical_cast<std::string>(nBytes));
    for (const GdbResponse &response: responses()) {
        if (GdbResponse::ResultClass::ERROR == response.result.rclass) {
            SAWYER_MESG(error) <<response.result.results["msg"].as<std::string>() <<"\n";
        } else if (GdbResponse::ResultClass::DONE == response.result.rclass) {
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
Gdb::writeMemory(const Address va, const size_t nBytes, const uint8_t *bytes) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream error(mlog[ERROR]);
    SAWYER_MESG(debug) <<"command: writeMemory(addr=" <<StringUtility::addrToString(va) <<", nbytes=" <<nBytes <<")\n";

    if (!isAttached())
        throw Exception("not attached to subordinate process");
    std::string s = "-data-write-memory-bytes " + StringUtility::intToHex(va) + " \"";
    s.reserve(s.size() + 2*nBytes + 1);
    for (size_t i = 0; i < nBytes; ++i)
        s += (boost::format("%02x") % (unsigned)bytes[i]).str();
    s += "\"";
    sendCommand(s);
    for (const GdbResponse &response: responses()) {
        if (GdbResponse::ResultClass::ERROR == response.result.rclass) {
            SAWYER_MESG(error) <<response.result.results["msg"].as<std::string>() <<"\n";
        } else if (GdbResponse::ResultClass::DONE == response.result.rclass) {
            if (checkWrites_) {
                const std::vector<uint8_t> readBytes = readMemory(va, nBytes, ByteOrder::EL).toBytes();
                ASSERT_always_require2(readBytes.size() == nBytes,
                                       "wrote " + StringUtility::plural(nBytes, "bytes") +
                                       " but read only " + StringUtility::plural(readBytes.size(), "bytes"));
                for (size_t i = 0; i < nBytes; ++i) {
                    ASSERT_always_require2(bytes[i] == readBytes[i],
                                           (boost::format("wrote 0x%02x at %s, but read back 0x%02x")
                                            % bytes[i] % (va + i) % readBytes[i]).str());
                }
            }
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

const std::vector<std::pair<std::string, Gdb::RegInfo>>&
Gdb::registerNames() const {
    return registers_;
}

} // namespace
} // namespace
} // namespace

#endif
