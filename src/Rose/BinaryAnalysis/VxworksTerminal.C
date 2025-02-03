#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/VxworksTerminal.h>

#include <Rose/BinaryAnalysis/Alignment.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>

#include <Sawyer/Optional.h>
#include <Sawyer/Parse.h>
#include <Sawyer/ProgressBar.h>
#include <Sawyer/Result.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <regex>
#include <string>
#include <thread>

#ifndef BOOST_WINDOWS
# include <arpa/inet.h>                                 // for htons() etc.
# include <fcntl.h>                                     // for fcntl()
# include <netdb.h>                                     // for getaddrinfo()
# include <netinet/in.h>                                // for struct sockaddr_in
# include <sys/socket.h>                                // for socket()
#endif

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VxworksTerminal::Settings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
VxworksTerminal::Settings::print(std::ostream &out) const {
    out <<"VxWorks settings:"
        <<"  address: " <<StringUtility::addrToString(where) <<"\n"
        <<"  perm:    "
        <<((permissions & MemoryMap::READABLE) != 0 ? "r" : "")
        <<((permissions & MemoryMap::WRITABLE) != 0 ? "w" : "")
        <<((permissions & MemoryMap::EXECUTABLE) != 0 ? "x" : "") <<"\n"
        <<"  prompt:  \"" <<StringUtility::cEscape(prompt) <<"\"\n"
        <<"  delay:   " <<delaySeconds <<" seconds\n"
        <<"  timeout: " <<timeoutSeconds <<" seconds\n"
        <<"  swath:   " <<StringUtility::plural(bytesPerRequest, "bytes") <<"\n"
        <<"  host:    \"" <<StringUtility::cEscape(host) <<"\"\n"
        <<"  port:    " <<port <<"\n";
}

std::ostream&
operator<<(std::ostream &out, const VxworksTerminal::Settings &x) {
    x.print(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VxworksTerminal
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sawyer::Message::Facility VxworksTerminal::mlog;

VxworksTerminal::~VxworksTerminal() {
    if (server_ != -1)
        close(server_);
}

VxworksTerminal::VxworksTerminal() {}

VxworksTerminal::VxworksTerminal(const Settings &settings)
    : settings_(settings) {}

VxworksTerminal::Ptr
VxworksTerminal::instance() {
    return Ptr(new VxworksTerminal);
}

VxworksTerminal::Ptr
VxworksTerminal::instance(const Settings &settings) {
    return Ptr(new VxworksTerminal(settings));
}

void
VxworksTerminal::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("Rose::BinaryAnalysis::VxworksTerminal", Diagnostics::destination);
        mlog.comment("interacting with VxWorks terminals");
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

const VxworksTerminal::Settings&
VxworksTerminal::settings() const {
    return settings_;
}

VxworksTerminal::Settings&
VxworksTerminal::settings() {
    return settings_;
}

std::string
VxworksTerminal::locatorStringDocumentation(const Settings &dflt) {
    return ("Beginning with the first colon, a VxWorks terminal resource string has the form "
            "\":@v{memory_properties}:@v{connection_properties}:@v{host}\" where both sets of properties may be empty strings "
            "but the three colons and the @v{host} are always required.\n\n"
            
            "The @v{memory_properties} have the form \"@v{address}+@v{size}[=@v{access}]\" where the @v{access} is optional "
            "(indicated by the square brackets which should not be present in the actual resource string). The @v{address} is the "
            "starting address for reading or writing memory and @v{size} is the number of bytes to read or write. The addresses "
            "apply to both the remote VxWorks device and the local ROSE analysis address space. The @v{access} field "
            "is the accessibility represented by zero or more of the characters \"r\" (readable), \"w\" (writable), and/or \"x\" "
            "(executable). When writing to a ROSE analysis address space, if the \"=\" is present but no accessibility letters "
            "are specified, then the local ROSE memory will have no access bits set, but if the \"=\" is also absent then the "
            "memory is set to be readable, writable, and executable.\n\n"

            "The @v{connection_properties} are a comma-separated list of @v{name}=@v{value} properties. If the @v{value} contains "
            "commas, colons, single or double quotes, or backslashes then the value should be surrounded by single or double "
            "quotes. The following properties are understood:"

            "@named{prompt}{Specifies the VxWorks terminal prompt string. If the value starts and ends with a slash then it is "
            "interpreted as a regular expression, otherwise the value uses the usual C-style literal string escape mechanism. "
            "Terminal prompts are expected to be single-line strings with no line termination. If the terminal emits a space "
            "character after the prompt, then be sure to include it in the prompt @v{value}." +
            [&dflt]() {
                if (!dflt.prompt.empty()) {
                    if (dflt.prompt.size() >= 2 && boost::starts_with(dflt.prompt, "/") && boost::ends_with(dflt.prompt, "/")) {
                        return " The default is \"" + dflt.prompt + "\".";
                    } else {
                        return " The default is \"" + StringUtility::cEscape(dflt.prompt) + "\".";
                    }
                } else {
                    return std::string();
                }
            }() +
            "}"

            "@named{delay}{Specifies the amount of time to delay between when a prompt is detected and when a command is sent "
            "to the remote terminal. The value must be an integer followed by either \"ms\" for milliseconds or \"s\" (or nothing) "
            "for seconds. Sending commands too soon after receiving a prompt can crash VxWorks. The default is " +
            [&dflt]() {
                const unsigned ms = ::round(dflt.delaySeconds * 1000);
                if (ms == 0) {
                    return std::string("0.");
                } else if (ms % 1000 == 0) {
                    return (boost::format("%us.") % (ms / 1000)).str();
                } else {
                    return (boost::format("%ums.") % ms).str();
                }
            }() +
            "}"

            "@named{timeout}{Specifies the amount of time to wait before assuming that a terminal prompt was missed. Sometimes "
            "a VxWorks terminal will asynchronously emit status information that interferes with the detection of the prompt, "
            "and the connection will deadlock since ROSE is waiting for a prompt before sending the next command, and the "
            "VxWorks terminal is waiting for a command before sending another prompt. If no output is received from the VxWorks "
            "terminal within the timeout period, then ROSE assumes that a prompt was missed and sends the next command. The "
            "timeout value must be an integer followed by either \"ms\" for milliseconds or \"s\" (or nothing) for seconds. "
            "The default is " +
            [&dflt]() {
                const unsigned ms = ::round(dflt.timeoutSeconds * 1000);
                if (ms == 0) {
                    return std::string("0.");
                } else if (ms % 1000 == 0) {
                    return (boost::format("%us.") % (ms / 1000)).str();
                } else {
                    return (boost::format("%ums.") % ms).str();
                }
            }() +
            "}"

            "@named{swath}{Specifies the maximum number of bytes of data that should be transferred per VxWorks terminal "
            "command. Transferring too many bytes at a time is likely to overflow internal VxWorks buffers and cause a crash. "
            "The default is " + StringUtility::plural(dflt.bytesPerRequest, "bytes") + " per command.}"

            "@named{port}{Specifies the TCP/IP port to which this tool should connect. The default is " +
            [&dflt]() {
                if (23 == dflt.port) {
                    return std::string("23 (the normal telnet port).");
                } else {
                    return boost::lexical_cast<std::string>(dflt.port) + ".";
                }
            }() +
            "}"

            "The @v{host} is the remote host to which this command should connect. It can be specified as a dotted quad "
            "IPv4 address or a hostname.");
}

Sawyer::Result<VxworksTerminal::Settings, MemoryMap::LocatorError>
VxworksTerminal::parseLocatorString(const std::string &locatorString, const Settings &dflt) {
    auto locatorError = [&locatorString](const std::string &message, const size_t locatorOffset) {
        return Sawyer::makeError(MemoryMap::LocatorError(message, locatorString, locatorOffset));
    };

    // The memory part between the first two colons
    if (!boost::starts_with(locatorString, ":"))
        return locatorError("not a valid VxWorks locator string (missing colon)", 0);
    const size_t secondColon = locatorString.find(':', 1);
    if (secondColon == std::string::npos)
        return locatorError("not a valid VxWorks locator string (missing second colon)", locatorString.size());
    const auto addrSizePerm = MemoryMap::parseAddrSizePerm(locatorString.substr(1, secondColon - 1));
    if (!addrSizePerm)
        return locatorError(addrSizePerm.unwrapError().first, 1 + addrSizePerm.unwrapError().second);
    if (dflt.where && !addrSizePerm->addr)
        return locatorError("memory address required", 1);
    if (dflt.where && !addrSizePerm->size)
        return locatorError("size in bytes required", 1 + addrSizePerm->sizeAt);

    // The connection part between the second and third colons
    std::vector<std::tuple<std::string /*name*/, std::string /*value*/, size_t /*index*/>> nameValuePairs;
    std::string remaining;
    size_t remainingAt;
    std::tie(nameValuePairs, remaining, remainingAt) = MemoryMap::parseNameValuePairs(locatorString.substr(secondColon + 1));
    Sawyer::Optional<std::string> prompt;
    Sawyer::Optional<double> delaySeconds;
    Sawyer::Optional<double> timeoutSeconds;
    Sawyer::Optional<size_t> swath;
    Sawyer::Optional<uint16_t> port;
    std::regex durationRe("([0-9]+)(s|ms)?");
    for (const auto &tuple: nameValuePairs) {
        const std::string &name = std::get<0>(tuple);
        const std::string &value = std::get<1>(tuple);
        const size_t nameAt = secondColon + 1 + std::get<2>(tuple);
        const size_t valueAt = nameAt + name.size() + 1;

        if (name == "prompt") {
            if (prompt) {
                return locatorError("duplicate \"prompt\" setting", nameAt);
            } else if (value.empty()) {
                return locatorError("\"prompt\" value cannot be empty", valueAt);
            } else if (value.find('\n') != std::string::npos) {
                return locatorError("\"prompt\" value cannot contain line-feed characters", valueAt);
            } else {
                prompt = value;
            }

        } else if (name == "delay") {
            std::smatch found;
            if (delaySeconds) {
                return locatorError("duplicate \"delay\" setting", nameAt);
            } else if (!std::regex_match(value, found, durationRe)) {
                return locatorError("syntax error in duration", valueAt);
            } else if (const auto parsed = Sawyer::parse<unsigned>(found.str(1))) {
                delaySeconds = (double)*parsed * (found.str(2) == "ms" ? 0.001 : 1.0);
            } else {
                return locatorError("delay " + parsed.unwrapError(), valueAt);
            }

        } else if (name == "timeout") {
            std::smatch found;
            if (timeoutSeconds) {
                return locatorError("duplicate \"timeout\" setting", nameAt);
            } else if (!std::regex_match(value, found, durationRe)) {
                return locatorError("syntax error in duration", valueAt);
            } else if (const auto parsed = Sawyer::parse<unsigned>(found.str(1))) {
                timeoutSeconds = (double)*parsed * (found.str(2) == "ms" ? 0.001 : 1.0);
            } else {
                return locatorError("timeout " + parsed.unwrapError(), valueAt);
            }

        } else if (name == "swath") {
            if (swath) {
                return locatorError("duplicate \"swath\" setting", nameAt);
            } else if (const auto parsed = Sawyer::parse<size_t>(value)) {
                swath = *parsed;
            } else {
                return locatorError("swath " + parsed.unwrapError(), valueAt);
            }

        } else if (name == "port") {
            if (port) {
                return locatorError("duplicate \"port\" setting", nameAt);
            } else if (const auto parsed = Sawyer::parse<uint16_t>(value)) {
                port = *parsed;
            } else {
                return locatorError("port " + parsed.unwrapError(), valueAt);
            }

        } else {
            return locatorError("unrecognized setting \"" + name + "\"", nameAt);
        }
    }
    if (boost::starts_with(remaining, ","))
        return locatorError("invalid name=value pair", secondColon + 1 + remainingAt + 1);

    // After the third colon is a host name or IP address
    std::regex hostIpRe("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\.){3}"
                         "(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
    std::regex hostNameRe("(([a-zA-Z0-9]|[a-zA-Z0-9][-a-zA-Z0-9]*[a-zA-Z0-9])\\.)*"
                           "([A-Za-z0-9]|[A-Za-z0-9][-A-Za-z0-9]*[A-Za-z0-9])");
    const size_t thirdColon = secondColon + 1 + remainingAt; // third colon expected here
    if (!boost::starts_with(remaining, ":")) {
        if (remaining.empty()) {
            return locatorError("not a valid VxWorks locator string (missing third colon)", thirdColon);
        } else {
            return locatorError("invalid name=value pair", thirdColon);
        }
    }
    const std::string host = locatorString.substr(thirdColon + 1);
    if (host.empty())
        return locatorError("not a valid VxWorks locator string: no host name or address", locatorString.size());
    if (std::regex_match(host, hostIpRe)) {
        // okay
    } else if (std::regex_match(host, hostNameRe)) {
        if (host.size() > 255)
            return locatorError("invalid RFC 1123 host name: total length must be less than 256 octets", thirdColon + 1);
        std::vector<std::string> labels;
        size_t labelAt = thirdColon + 1;
        boost::split(labels, host, boost::is_any_of("."));
        for (const std::string &label: labels) {
            if (label.empty() || label.size() > 63)
                return locatorError("invalid RFC 1123 host name: label must be between 1 and 63 octets", labelAt);
            labelAt += label.size() + 1;
        }
    } else {
        return locatorError("invalid RFC 1123 host name or IPv4 address", thirdColon + 1);
    }

    // Save the results
    Settings retval = dflt;
    if (addrSizePerm->addr) {
        retval.where =
            AddressInterval::baseSize(*addrSizePerm->addr, addrSizePerm->size.orElse(dflt.where ? dflt.where.size() : 1));
    } else if (addrSizePerm->size) {
        retval.where = AddressInterval::baseSize(dflt.where ? dflt.where.least() : 0, *addrSizePerm->size);
    }
    if (addrSizePerm->perm)
        retval.permissions = *addrSizePerm->perm;
    if (prompt)
        retval.prompt = *prompt;
    if (delaySeconds)
        retval.delaySeconds = *delaySeconds;
    if (timeoutSeconds)
        retval.timeoutSeconds = *timeoutSeconds;
    if (swath)
        retval.bytesPerRequest = *swath;
    if (port)
        retval.port = *port;
    retval.host = host;

    SAWYER_MESG(mlog[DEBUG]) <<"parsed VxWorks locator string \"" + StringUtility::cEscape(locatorString) + "\"\n"
                             <<retval;
    return Sawyer::makeOk(retval);
}

Sawyer::Optional<std::string>
VxworksTerminal::open() {
#ifdef BOOST_WINDOWS
    return "VxWorks connection is not supported on Windows";
#else

    // C resources that must be cleaned up on return or exceptions
    struct Resources {
        int server = -1;                                // file descriptor connected to the telnet (or other) server
        struct addrinfo *serverAddrs = nullptr;

        ~Resources() {
            if (server != -1)
                close(server);
            if (serverAddrs)
                freeaddrinfo(serverAddrs);
        }
    } r;

    if (isOpen())
        return std::string("already open");

    // Look up the server IP address
    {
        struct addrinfo hints;
        memset(&hints, 0, sizeof hints);
        hints.ai_flags = AI_ADDRCONFIG;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        const int ecode = getaddrinfo(settings_.host.c_str(), (boost::format("%d") % settings_.port).str().c_str(), &hints,
                                      &r.serverAddrs);
        if (ecode == EAI_SYSTEM) {
            return std::string("VxWorks connection getaddrinfo failed: ") + strerror(errno);
        } else if (ecode != 0) {
            return std::string("VxWorks connection getaddrinfo failed: ") + gai_strerror(ecode);
        }
        ASSERT_not_null(r.serverAddrs);
    }

    // Connect to the server
    for (struct addrinfo *addr = r.serverAddrs; addr; addr = addr->ai_next) {
        r.server = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == r.server) {
            if (!addr->ai_next) {
                return std::string("VxWorks connection socket failed: ") + strerror(errno);
            } else {
                continue;
            }
        }

        if (connect(r.server, addr->ai_addr, addr->ai_addrlen) == -1) {
            if (!addr->ai_next) {
                return std::string("VxWorks connection connect failed: ") + strerror(errno);
            } else {
                continue;
            }
        }
    }

    // Make the socket non-blocking
    {
        const int flags = fcntl(r.server, F_GETFL, 0);
        if (-1 == flags)
            return std::string("VxWorks connection fntl(F_GETFL) failed: ") + strerror(errno);
        if (-1 == fcntl(r.server, F_SETFL, flags | O_NONBLOCK))
            return std::string("VxWorks connection fntl(F_SETFL) failed: ") + strerror(errno);
    }

    std::swap(server_, r.server);
    SAWYER_MESG(mlog[DEBUG]) <<"connected to server, fd=" <<server_ <<"\n";
    return Sawyer::Nothing();
#endif
}

bool
VxworksTerminal::isOpen() const {
    return server_ != -1;
}

// Handles terminal interaction for the "download" functions.
class Download: public VxworksTerminal::DataTransfer {
    VxworksTerminal &terminal;
    AddressInterval remaining;                          // addresses remaining to be read
    Alignment lineAlignment;                            // alignment of data lines
    Sawyer::Message::Stream debug, warn;
    std::regex dataMatchRe;                             // regex to match a line of data
    Sawyer::ProgressBar<size_t> progress;
    
public:
    std::vector<uint8_t> data;                          // data tha twas read

public:
    Download(VxworksTerminal &terminal, const AddressInterval &where)
        : terminal(terminal), remaining(where), lineAlignment(16, 32), debug(terminal.mlog[DEBUG]), warn(terminal.mlog[WARN]),
          dataMatchRe(".*?"                             // prompt
                      "((?:0x)?[0-9a-fA-F]{8}): "       // line address
                      "( *)"                            // 3 spaces per skipped byte
                      "((?: [0-9a-fA-F]{2})+)"          // numeric data
                      "\\s*\\*.{16}\\*"),               // ASCII data
          progress(where.size(), terminal.mlog[MARCH], "transfer")
        {
            if (remaining)
                data.reserve(remaining.size());
            progress.suffix(" bytes");
        }

    std::string handleInput(const std::string &line) {
        using StringUtility::addrToString;
        using StringUtility::plural;

        std::smatch found;
        if (remaining && std::regex_match(line, found, dataMatchRe)) {
            Sawyer::Message::Stream debug(VxworksTerminal::mlog[DEBUG]);
            Sawyer::Message::Stream warn(VxworksTerminal::mlog[WARN]);

            SAWYER_MESG(debug) <<"found data line: \"" <<line <<"\"\n";
            const Address lineAddr = *Sawyer::parse<Address>(found.str(1));
            const Address startAddr = lineAddr + found.str(2).size() / 3;
            SAWYER_MESG(debug) <<"  remaining=" <<addrToString(remaining)
                               <<", lineAddr=" <<addrToString(lineAddr)
                               <<", startAddr=" <<addrToString(startAddr)
                               <<", nspaces=" <<found.str(2).size() <<"\n";

            if (startAddr != remaining.least()) {
                SAWYER_MESG(warn) <<"expected data at " <<addrToString(remaining.least())
                                  <<" but got data at " <<addrToString(startAddr) <<" (ignoring line)\n";
                return "";
            }
            const size_t maxBytesExpected = std::min(16 - (startAddr - lineAddr), remaining.size());
            ASSERT_require2(maxBytesExpected > 1 && maxBytesExpected <= 16, maxBytesExpected);

            // Parse the data
            std::vector<std::string> parts;
            boost::regex spacesRe(" +");
            boost::split_regex(parts, boost::trim_copy(found.str(3)), spacesRe);
            if (parts.size() > maxBytesExpected) {
                SAWYER_MESG(warn) <<"expected up to " <<plural(maxBytesExpected, "bytes") <<" but got " <<parts.size() <<"\n";
                return "";
            }
            for (const std::string &part: parts)
                data.push_back(*Sawyer::parse<uint8_t>("0x" + part));
            progress += parts.size();

            // Update addresses remaining
            if (remaining.size() == parts.size()) {
                remaining = AddressInterval();
            } else {
                remaining = AddressInterval::hull(startAddr + parts.size(), remaining.greatest());
            }
        }
        return "";
    }

    std::string handlePrompt(const std::string&) {
        if (remaining) {
            const size_t nBytes = std::min(terminal.settings().bytesPerRequest, (size_t)remaining.size());
            return (boost::format("d 0x%x, %u, 1\n") % remaining.least() % nBytes).str();
        } else {
            state = State::TERMINATE;
            return "";
        }
    }
};

Sawyer::PartialResult<std::vector<uint8_t>, std::string>
VxworksTerminal::download() {
    return download(settings_.where);
}

Sawyer::PartialResult<std::vector<uint8_t>, std::string>
VxworksTerminal::download(const AddressInterval &where) {
    Download xfer(*this, where);

    if (!isOpen())
        return {xfer.data, "connection is not open"};
    if (where.isEmpty())
        return Sawyer::makeOk(xfer.data);

    if (const auto completed = transferData(xfer)) {
        if (xfer.data.size() == where.size()) {
            return Sawyer::makeOk(xfer.data);
        } else {
            return {xfer.data, "short read"};
        }
    } else {
        return {xfer.data, completed.unwrapError()};
    }
}

Sawyer::PartialResult<AddressInterval, std::string>
VxworksTerminal::download(const MemoryMap::Ptr &map, const std::string &name) {
    return download(map, settings_.where, name);
}

Sawyer::PartialResult<AddressInterval, std::string>
VxworksTerminal::download(const MemoryMap::Ptr &map, const AddressInterval &where, const std::string &name) {
    ASSERT_not_null(map);
    if (const auto data = download(where)) {
        ASSERT_require(data->size() == where.size());
        map->insert(where, MemoryMap::Segment::anonymousInstance(where.size(), settings_.permissions, name));
        const size_t nCopied = map->at(where).write(*data).size();
        ASSERT_always_require(nCopied == where.size());
        return Sawyer::makeOk(where);
    } else if (data->empty()) {
        return {AddressInterval(), data.unwrapError()};
    } else {
        return {AddressInterval::baseSize(where.least(), data->size()), data.unwrapError()};
    }
}

bool
VxworksTerminal::matchPrompt(const std::string &input) {
    if (settings_.prompt.size() >= 2 && boost::starts_with(settings_.prompt, "/") && boost::ends_with(settings_.prompt, "/")) {
        std::regex promptRe(settings_.prompt.substr(1, settings_.prompt.size()-2));
        return std::regex_match(input, promptRe);
    } else {
        return input == settings_.prompt;
    }
}

Sawyer::Result<bool, std::string>
VxworksTerminal::transferData(DataTransfer &xfer) {
#ifdef BOOST_WINDOWS
    return Sawyer::makeError("not implemented on Windows");
#else
    if (!isOpen())
        return Sawyer::makeError("connection is not open");

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    std::string input;                                  // any previous partial line from the server
    std::string output;                                 // output waitito ng be sent to the server

    while (xfer.state != DataTransfer::State::TERMINATE) {
        if (xfer.state == DataTransfer::State::DRAIN && output.empty())
            break;

        fd_set rdSet, wrSet, exSet;
        FD_ZERO(&rdSet);
        FD_SET(server_, &rdSet);
        FD_ZERO(&wrSet);
        if (!output.empty())
            FD_SET(server_, &wrSet);
        FD_ZERO(&exSet);
        //FD_SET(server_, &exSet);

        struct timeval timeout;
        double seconds = NAN;
        timeout.tv_usec = modf(settings_.timeoutSeconds, &seconds) * 1000000.0;
        timeout.tv_sec = seconds;
        const int nset = select(server_ + 1, &rdSet, &wrSet, &exSet, &timeout);
        if (-1 == nset)
            return Sawyer::makeError(std::string("VxWorks connection select failed: ") + strerror(errno));

        if (0 == nset) {
            SAWYER_MESG(debug) <<"timeout\n";
            const std::string result = xfer.handlePrompt("");
            SAWYER_MESG(debug) <<"prompt response: \"" <<StringUtility::cEscape(result) <<"\"\n";
            output += result;
            continue;
        }

        if (FD_ISSET(server_, &rdSet)) {
            char buffer[4096];
            const ssize_t nRead = read(server_, buffer, sizeof buffer);
            if (-1 == nRead) {
                return Sawyer::makeError(std::string("VxWorks connection read failed: ") + strerror(errno));
            } else if (0 == nRead) {
                break;
            } else if (nRead > 0) {
                input += std::string(buffer, buffer + nRead);
                std::vector<std::string> lines;
                boost::split(lines, input, boost::is_any_of("\n"));
                ASSERT_forbid(lines.empty());
                input = lines.back();                   // empty if input ended with line termination
                lines.pop_back();
                for (const std::string &line: lines) {
                    SAWYER_MESG(debug) <<"input: " <<line <<"\n";
                    const std::string result = xfer.handleInput(line);
                    if (!result.empty()) {
                        SAWYER_MESG(debug) <<"input response: \"" <<StringUtility::cEscape(result) <<"\"\n";
                        output += result;
                    }
                }
                if (matchPrompt(input)) {
                    SAWYER_MESG(debug) <<"prompt found: \"" <<StringUtility::cEscape(input) <<"\"\n";
                    SAWYER_MESG(debug) <<"  delaying " <<settings_.delaySeconds <<" seconds...";
                    std::this_thread::sleep_for(std::chrono::duration<double>(settings_.delaySeconds));
                    SAWYER_MESG(debug) <<"\n";
                    const std::string result = xfer.handlePrompt(input);
                    SAWYER_MESG(debug) <<"prompt response: \"" <<StringUtility::cEscape(result) <<"\"\n";
                    output += result;
                }
            }
        }

        if (FD_ISSET(server_, &wrSet)) {
            const ssize_t nWrite = write(server_, output.c_str(), output.size());
            if (-1 == nWrite) {
                return Sawyer::makeError(std::string("VxWorks connection write failed: ") + strerror(errno));
            } else if (0 == nWrite) {
                break;
            } else if (nWrite > 0) {
                SAWYER_MESG(debug) <<"output: \"" + StringUtility::cEscape(output.substr(0, nWrite)) + "\"\n";
                output = output.substr(nWrite);
            }
        }

        if (FD_ISSET(server_, &exSet)) {
            ASSERT_not_implemented("exception");
        }
    }

    switch (xfer.state) {
        case DataTransfer::State::NORMAL:
            return Sawyer::makeError("lost connection");
        case DataTransfer::State::DRAIN:
            if (!output.empty())
                return Sawyer::makeError("lost connection");
            break;
        case DataTransfer::State::TERMINATE:
            break;
    }
    return Sawyer::makeOk(true);
#endif
}

} // namespace
} // namespace

#endif
