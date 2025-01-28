static const char *purpose = "emulate a VxWorks telnet server";
static const char
*description =
    "Emulates a VxWorks terminal over telnet in order to test ROSE features that interact with such a connection.";

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/Alignment.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BitOps.h>
#include <Rose/CommandLine.h>
#include <Rose/Initialize.h>
#include <Rose/StringUtility/NumberToString.h>

#include <Sawyer/Parse.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <regex>
#include <string>

#include <ext/stdio_filebuf.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

struct Settings {
    uint16_t port = 8080;
    std::string prompt = "-> ";
};

struct Context {
    Settings settings;
    P2::Engine::Ptr engine;
    MemoryMap::Ptr memory;
    Address nextDumpAddress = 0;
};

static const size_t BUFFER_SIZE = 1024;
static const size_t NBYTES_PER_LINE = 16;
static const size_t NLINES_PER_BATCH = 4096 / NBYTES_PER_LINE;
static const size_t BITS_PER_WORD = 32;
static const std::string numberRe = "(?:0x[0-9a-fA-F]+|0b[01]+|[1-9][0-9]*|0)";
static Sawyer::Message::Facility mlog;

static Sawyer::CommandLine::Parser
createSwitchParser(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup tool("Tool-specific switches");
    tool.name("tool");

    tool.insert(Switch("port", 'p')
                .argument("number", nonNegativeIntegerParser(settings.port))
                .doc("TCP/IP port number at which to listen for connections. The default is " +
                     boost::lexical_cast<std::string>(settings.port) + "."));

    tool.insert(Switch("prompt")
                .argument("string", anyParser(settings.prompt))
                .doc("The terminal command prompt. The default is \"" + StringUtility::cEscape(settings.prompt) + "\"."));

    Parser parser = Rose::CommandLine::createEmptyParser(purpose, description);
    parser.errorStream(mlog[FATAL]);
    parser.doc("Synopsis", "@prop{programName} [@v{switches}] [@v{specimens}]");
    parser.with(tool);
    parser.with(Rose::CommandLine::genericSwitches());
    return parser;
}

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Sawyer::CommandLine::Parser &parser) {
    return parser.parse(argc, argv).apply().unreachedArgs();
}

static void
greeting(std::ostream &client) {
    client << "Instantiating /ram0 as rawFs, device = 0x1\n"
              "Instantiating /ram0 as rawFs,  device = 0x1\n"
              "Formatting /ram0 for HRFS v1.2\n"
              "Formatting...OK.\n"
              "Target Name: vxTarget \n"
              "Instantiating /tmp as rawFs, device = 0x10001\n"
              "Instantiating /tmp as rawFs,  device = 0x10001\n"
              "Formatting /tmp for HRFS v1.2\n"
              "Formatting...OK.\n"
              " \n"
              " _________            _________\n"
              " \\........\\          /......../\n"
              "  \\........\\        /......../\n"
              "   \\........\\      /......../\n"
              "    \\........\\    /......../\n"
              "     \\........\\   \\......./\n"
              "      \\........\\   \\...../              VxWorks SMP 64-bit\n"
              "       \\........\\   \\.../\n"
              "        \\........\\   \\./     Release version: 24.03\n"
              "         \\........\\   -      Build date: Mar 27 2024 13:35:56\n"
              "          \\........\\\n"
              "           \\......./         Copyright Wind River Systems, Inc.\n"
              "            \\...../   -                 1984-2024\n"
              "             \\.../   /.\\\n"
              "              \\./   /...\\\n"
              "               -   -------\\n"
              "\n"
              "                   Board: x86_64 Processor (ACPI_BOOT_OP) SMP/SMT\n"
              "               CPU Count: 1\n"
              "          OS Memory Size: ~446MB\n"
              "        ED&R Policy Mode: Deployed\n"
              "     Debug Agent: Started (always)\n"
              "         Stop Mode Agent: Not started\n"
              "\n"
              "Instantiating /ram as rawFs,  device = 0x20001\n"
              "Formatting /ram for DOSFS\n"
              "Instantiating /ram as rawFs, device = 0x20001\n"
              "Formatting...Retrieved old volume params with %38 confidence:\n"
              "Volume Parameters: FAT type: FAT32, sectors per cluster 0\n"
              "  0 FAT copies, 0 clusters, 0 sectors per FAT\n"
              "  Sectors reserved 0, hidden 0, FAT sectors 0\n"
              "  Root dir entries 0, sysId (null)  , serial number 1a0000\n"
              "  Label:"           " ...\n"
              "Disk with 64 sectors of 512 bytes will be formatted with:\n"
              "Volume Parameters: FAT type: FAT12, sectors per cluster 1\n"
              "  2 FAT copies, 54 clusters, 1 sectors per FAT\n"
              "  Sectors reserved 1, hidden 0, FAT sectors 2\n"
              "  Root dir entries 112, sysId VXDOS12 , serial number 1a0000\n"
              "  Label:"           " ...\n"
              "OK.\n"
              "\n"
              " Adding 24522 symbols for standalone.\n"
              "\n";
}

static void crash(const std::string &mesg) [[noreturn]] {
    std::cerr <<"simulated server crash: " <<mesg <<"\n";
    abort();
}

static bool
handleDump(Context &ctx, std::ostream &client, const std::string &command) {
    std::regex re("\\s*d"                               // command
                  "(?:\\s+(" + numberRe + ")"           // starting address
                  "(?:,\\s*(" + numberRe + ")"          // count
                  "(?:,\\s*([124])"                     // item size in bytes
                  ")?)?)?"                              // 0, 1, 2, or 3 arguments
                  "\\s*");

    std::smatch found;
    const Address addrMask = BitOps::lowMask<Address>(BITS_PER_WORD);
    if (std::regex_match(command, found, re)) {
        const Address startAddr = found.str(1).empty() ? ctx.nextDumpAddress : *Sawyer::parse<Address>(found.str(1)) & addrMask;
        const size_t nValues = found.str(2).empty() ? 1024 : *Sawyer::parse<size_t>(found.str(2)) & addrMask;
        const size_t bytesPerValue = found.str(3).empty() ? 1 : *Sawyer::parse<size_t>(found.str(3));

        std::cerr <<"dump addr=" <<StringUtility::addrToString(startAddr)
                  <<", nValues=" <<nValues <<", bytesPerValue=" <<bytesPerValue <<"\n";
        if (nValues == 0)
            return true;

        ASSERT_always_require2(bytesPerValue == 1, "not implemented yet");
        Alignment alignment(16, 32);
        const Address lineAddr = alignment.alignDown(startAddr);
        std::string ascii;                              // ASCII characters per line of data

        // Handle padding for first line of output that might not be aligned.
        client <<(boost::format("0x%08x: ") % lineAddr);
        for (Address addr = alignment.alignDown(startAddr); addr != startAddr; addr = (addr + 1) & addrMask) {
            client <<"   ";
            ascii += ' ';
        }

        // Dump the data
        const Address endAddr = (startAddr + nValues) & addrMask;
        for (Address addr = startAddr; addr != endAddr; addr = (addr + 1) & addrMask) {
            if (addr != startAddr && addr == alignment.alignDown(addr)) {
                client <<"   *" <<ascii <<"*" <<std::endl;
                ascii = "";
                client <<(boost::format("0x%08x: ") % addr);
            }
            uint8_t byte = 0;
            if (ctx.memory->at(addr).limit(1).read(&byte).size() != 1)
                crash("read from unmapped memory address " + StringUtility::addrToString(addr));
            client <<(boost::format(" %02x") % (unsigned)byte);
            if (isprint(byte) && !isspace(byte)) {
                ascii += (char)byte;
            } else {
                ascii += '.';
            }
        }

        // padding at end of last line
        for (Address addr = endAddr; addr != alignment.alignUp(endAddr).orElse(0); addr = (addr + 1) & addrMask) {
            ascii += ' ';
            client <<"   ";
        }
        client <<"   *" <<ascii <<"*" <<std::endl;

        ctx.nextDumpAddress = endAddr;
        return true;
    } else {
        return false;
    }
}

static void
handleCommand(Context &ctx, std::ostream &client, const std::string &command) {
    handleDump(ctx, client, command);
}

static int
createListenerSocket(const Settings &settings) {
    const int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set server address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(settings.port);

    // Allow the address to be reused immediately if this server crashes.
    int optval = 1;
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == -1) {
        perror("setsockopt failed");
        close(serverFd);
        exit(1);
    }

    // Bind the socket
    if (bind(serverFd, (struct sockaddr*)&serverAddr, sizeof serverAddr) == -1) {
        perror("bind failed");
        close(serverFd);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(serverFd, 5) == -1) {
        perror("listen failed");
        close(serverFd);
        exit(1);
    }

    printf("server started on port %u...\n", settings.port);
    return serverFd;
}

static int
acceptConnection(const int serverFd) {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof clientAddr;
    const int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd == -1) {
        perror("accept failed");
        return -1;
    }
    printf("Connection from %s\n", inet_ntoa(clientAddr.sin_addr));
    return clientFd;
}

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&mlog, "tool");
    mlog.comment("fake VxWorks telnet server");

    Context ctx;
    Sawyer::CommandLine::Parser switchParser = createSwitchParser(ctx.settings);
    ctx.engine = P2::Engine::forge(argc, argv, switchParser /*in,out*/);
    const std::vector<std::string> specimen = parseCommandLine(argc, argv, switchParser);
    ctx.memory = notnull(ctx.engine->loadSpecimens(specimen));
    std::cerr <<*ctx.memory;

    const int serverFd = createListenerSocket(ctx.settings);
    while (true) {
        const int clientFd = acceptConnection(serverFd);
        __gnu_cxx::stdio_filebuf<char> filebuf(clientFd, std::ios::out);
        std::ostream client(&filebuf);

        greeting(client);

        std::string input;
        while (1) {
            client <<ctx.settings.prompt;
            client.flush();

            // Read input from client -- the command
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, sizeof(buffer));
            const ssize_t n = read(clientFd, buffer, sizeof(buffer) - 1);
            if (-1 == n) {
                perror("read error");
                break;
            } else if (n <= 0) {
                printf("client disconnected\n");
                break;
            } else {
                input += std::string(buffer, buffer + n);
                std::vector<std::string> lines;
                boost::split(lines, input, boost::is_any_of("\n"));
                input = lines.back();
                lines.pop_back();
                for (const std::string &line: lines)
                    handleCommand(ctx, client, line);
            }
        }

        // Close the client connection
        client.flush();
        close(clientFd);
    }

    // Close the server socket (although it will never reach here in this case)
    close(serverFd);
    return 0;
}
