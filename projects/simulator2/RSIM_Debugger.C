#include <rose.h>
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Debugger.h"

#include "rose_strtoull.h"
#include "BaseSemantics2.h"
#include <Partitioner2/Utility.h>
#include <boost/algorithm/string/trim.hpp>
#include <readline/readline.h>
#include <readline/history.h>
#include <stringify.h>

using namespace rose;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

namespace RSIM_Debugger {

// The main debugger
class Debugger {
    std::istream &in_;                                  // for reading commands
    std::ostream &out_;                                 // for writing results
    bool detached_;                                     // don't ask for commands
    size_t singleStep_;                                 // stop when this hits zero
    AddressIntervalSet breakPointVas_;                  // instruction addresses to stop at
    std::set<X86InstructionKind> breakPointKinds_;      // instruction kinds to stop at
public:
    Debugger(std::istream &in, std::ostream &out): in_(in), out_(out), detached_(false), singleStep_(0) {}

    // Should debugger stop for this instruction callback?
    bool shouldStop(const RSIM_Callbacks::InsnCallback::Args &args) {
        if (detached_)
            return false;
        do {
            if (0 == singleStep_--)                     // must be first in this "do"
                break;
            if (breakPointVas_.exists(args.insn->get_address()))
                break;
            if (isSgAsmX86Instruction(args.insn) &&
                breakPointKinds_.find(isSgAsmX86Instruction(args.insn)->get_kind()) != breakPointKinds_.end())
                break;
            return false;
        } while (0);
        singleStep_ = -1;
        return true;
    }

    // Read-eval-print loop when stopped for an instruction
    bool repl(const RSIM_Callbacks::InsnCallback::Args &args) {
        out_ <<"stopped at " <<unparseInstructionWithAddress(args.insn) <<"\n";
        return repl(args.thread);
    }

    // Common read-eval-print loop
    bool repl(RSIM_Thread *thread) {
        while (true) {
            try {
                std::vector<std::string> cmd = readCommand();
                if (cmd.empty()) {
                    // prompt again
                } else if (cmd[0]=="b" || cmd[0]=="bp" || cmd[0]=="break" || cmd[0]=="breakpoint") {
                    cmd.erase(cmd.begin());
                    breakPointCommands(thread, cmd);
                } else if (cmd[0]=="c" || cmd[0]=="cont" || cmd[0]=="continue") {
                    return true;
                } else if (cmd[0]=="detach") {
                    detached_ = true;
                    return true;
                } else if (cmd[0]=="m" || cmd[0]=="mem" || cmd[0]=="memory") {
                    cmd.erase(cmd.begin());
                    memoryCommand(thread, cmd);
                } else if (cmd[0]=="q" || cmd[0]=="quit") {
                    exit(0);
                } else if (cmd[0]=="r" || cmd[0]=="reg" || cmd[0]=="register" || cmd[0]=="registers") {
                    cmd.erase(cmd.begin());
                    registerCommand(thread, cmd);
                } else if (cmd[0]=="s" || cmd[0]=="si" || cmd[0]=="step" || cmd[0]=="stepi") {
                    if (cmd.size() <= 1) {
                        singleStep_ = 0;
                    } else {
                        singleStep_ = parseInteger(cmd[1]);
                        if (singleStep_ > 0)
                            --singleStep_;              // i.e, number of instructions to skip the REPL
                    }
                    return true;
                } else if (boost::starts_with(cmd[0], "x/")) {
                    memoryCommand(thread, cmd);
                } else {
                    std::cerr <<"unknown command: \"" <<StringUtility::cEscape(cmd[0]) <<"\"\n";
                }
            } catch (const std::runtime_error &e) {
                std::cerr <<"error: " <<e.what() <<"\n";
            }
        }
    }

    // Split string into words, honoring quotes and backslashes
    std::vector<std::string> splitCommand(const char *s, bool stripComments=true) {
        std::vector<std::string> words;
        words.push_back("");

        bool wordHadQuote = false;
        char inQuote = 0;
        while (*s) {
            if ('\\'==*s) {
                words.back() += s[1];
                s += 2;
            } else if (inQuote == *s) {
                inQuote = 0;
                ++s;
            } else if ('"'==*s || '\''==*s) {
                inQuote = *s++;
                wordHadQuote = true;
            } else if (isspace(*s) && !inQuote) {
                words.push_back("");
                while (isspace(*++s)) /*void*/;
                wordHadQuote = false;
            } else if ('#'==*s && !inQuote) {
                break;
            } else {
                words.back() += *s++;
            }
        }
        if (inQuote)
            throw std::runtime_error("unterminated quote");
        if (words.back().empty() && !wordHadQuote)
            words.pop_back();
        return words;
    }

    // Read until we get a command, then parse it into words and return those words. Returns empty vector for EOF.
    std::vector<std::string> readCommand(const std::string &prompt = "RSIM> ") {
        while (1) {
            struct Resources {
                char *readline_buf;
                Resources(): readline_buf(NULL) {}
                ~Resources() {
                    if (readline_buf)
                        free(readline_buf);
                }
            } r;

            r.readline_buf = readline(prompt.c_str());
            if (!r.readline_buf)
                return std::vector<std::string>(1, "quit");
            char *s = r.readline_buf;
            while (isspace(*s)) ++s;
            if ('#'==*s || !*s)
                continue;

            add_history(r.readline_buf);
            return splitCommand(s);
        }
    }

    rose_addr_t parseInteger(std::string s) {
        boost::trim(s);
        return Sawyer::Container::BitVector::parse(s).toInteger();
    }

    AddressInterval parseAddressInterval(std::string s) {
        using namespace Sawyer::CommandLine;
        std::vector<std::string> args(1, "--interval");
        args.push_back(s);
        AddressInterval i;
        Parser().with(Switch("interval").argument("interval", Partitioner2::addressIntervalParser(i))).parse(args).apply();
        return i;
    }

    // Commands for checking registers against gdb's "i r" command.
    //   (nothing)                      -- checks GDB registers against RSIM registers
    //   fix                            -- checks GDB registers against RSIM registers and repairs differences
    // Cut and paste the "i r" output followed by a blank line.
    void registerCheckGdbCommand(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        char buf[4096];
        size_t nMismatches = 0;
        bool fix = false;
        if (cmd.size() >= 1) {
            if (cmd[0]=="fix") {
                fix = true;
            } else {
                throw std::runtime_error("unkonwn register check argument \"" + StringUtility::cEscape(cmd[0]) + "\"");
            }
        }
            
        while (in_.getline(buf, sizeof buf).good()) {
            std::vector<std::string> words = splitCommand(buf);
            if (words.empty())
                break;
            if (boost::starts_with(words[0], "(gdb)"))
                continue;
            if (words.size() < 2)
                throw std::runtime_error("every gdb \"i r\" output line should have at least two words");
            std::string regname = words[0];
            if (regname=="fctrl" || regname=="fstat" || regname=="ftag"  || regname=="fiseg" ||
                regname=="fioff" || regname=="foseg" || regname=="fooff" || regname=="fop"   ||
                regname=="mxcsr")
                continue;                               // don't compare some registers
            const RegisterDescriptor *reg = thread->get_process()->get_disassembler()->get_registers()->lookup(regname);
            if (!reg)
                throw std::runtime_error("unknown register \"" + StringUtility::cEscape(regname) + "\"");
            rose_addr_t gdbRegValue = parseInteger(words[1]);
            rose_addr_t simRegValue = thread->operators()->readRegister(*reg)->get_number();
            if (gdbRegValue != simRegValue) {
                out_ <<"*** MISMATCH for \"" <<StringUtility::cEscape(regname) <<"\""
                     <<" gdb=" <<StringUtility::toHex2(gdbRegValue, reg->get_nbits())
                     <<" sim=" <<StringUtility::toHex2(simRegValue, reg->get_nbits()) <<"\n";
                ++nMismatches;
                if (fix)
                    thread->operators()->writeRegister(*reg, thread->operators()->number_(reg->get_nbits(), gdbRegValue));
            }
        }
        if (0==nMismatches)
            out_ <<"registers matched\n";
    }

    // Commands that operate on registers
    //   (empty)                        -- show all registers
    //   check                          -- compare with gdb's "i r" output
    //   <name>                         -- show only one register
    //   <name> <value>                 -- set (and then show) one register
    void registerCommand(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        if (cmd.empty()) {
            out_ <<*thread->operators()->get_state()->get_register_state();
        } else if (cmd[0]=="check") {
            cmd.erase(cmd.begin());
            registerCheckGdbCommand(thread, cmd);
        } else {
            const RegisterDescriptor *reg = thread->get_process()->get_disassembler()->get_registers()->lookup(cmd[0]);
            if (!reg) {
                out_ <<"no such register \"" <<StringUtility::cEscape(cmd[0]) <<"\"\n";
                return;
            }

            if (cmd.size() > 1) {
                rose_addr_t n = parseInteger(cmd[1]);
                BaseSemantics::SValuePtr value = thread->operators()->number_(reg->get_nbits(), n);
                thread->operators()->writeRegister(*reg, value);
            }

            out_ <<*thread->operators()->readRegister(*reg) <<"\n";
        }
    }

    // Commands to change memory protection
    //   <protection> [<interval>]      -- set protection bits for [part of] memory; bits 'r', 'w', 'x', and/or '-'
    void memoryProtectionCommands(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        if (cmd.empty())
            throw std::runtime_error("protection letters 'r', 'w', and/or 'x' expected");
        unsigned prot = 0;
        BOOST_FOREACH (char ch, cmd[1]) {
            switch (ch) {
                case 'r': prot |= MemoryMap::READABLE; break;
                case 'w': prot |= MemoryMap::WRITABLE; break;
                case 'x': prot |= MemoryMap::EXECUTABLE; break;
                case '-': break;
                default:
                    throw std::runtime_error("protection letters 'r', 'w', and/or 'x' expected");
            }
        }
        if (cmd.size()==1) {
            thread->get_process()->get_memory().any().changeAccess(prot, ~prot);
        } else {
            AddressInterval where = parseAddressInterval(cmd[2]);
            thread->get_process()->get_memory().atOrAfter(where.least()).atOrBefore(where.greatest())
                .changeAccess(prot, ~prot);
        }
    }

    // Commands to dump memory to a file
    //   dump <file> <interval>         -- dump specified memory to raw binary file
    void memoryDumpCommands(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        struct Resources {
            FILE *f;
            Resources(): f(NULL) {}
            ~Resources() {
                if (f)
                    fclose(f);
            }
        } r;

        if (cmd.empty())
            throw std::runtime_error("\"dump\" expects <filename> <interval>");
        r.f = fopen(cmd[0].c_str(), "wb");
        if (r.f==NULL)
            throw std::runtime_error(std::string(strerror(errno)) + " \"" + StringUtility::cEscape(cmd[0]) + "\"");
        AddressInterval interval = parseAddressInterval(cmd[1]);
        uint8_t buffer[8192];
        while (!interval.isEmpty()) {
            size_t nRead = thread->get_process()->get_memory().at(interval.least())
                           .limit(std::min(interval.size(), rose_addr_t(sizeof buffer)))
                           .read(buffer).size();
            if (0==nRead)
                throw std::runtime_error("short read");
            size_t nWrite = fwrite(buffer, 1, nRead, r.f);
            if (nWrite != nRead)
                throw std::runtime_error("short write");
            if (interval.least() + nRead - 1 == interval.greatest())
                break;
            interval = AddressInterval::hull(interval.least()+nRead, interval.greatest());
        }
    }

    // Commands for mapping new memory
    //   <map_resource>                 -- see MemoryMap::insertFile for the syntax.
    void memoryMapCommands(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        if (cmd.empty())
            throw std::runtime_error("expected <map_resource>");
        std::string resource = cmd[0];
        if (boost::starts_with(resource, "map:")) {
            resource = resource.substr(3);
        } else if (!boost::starts_with(resource, ":")) {
            resource = ":" + resource;
        }
        thread->get_process()->get_memory().insertFile(resource);
    }

    // Commands for producing memory hexdumps
    //   <interval>                     -- produce hex dump for specified interval
    void memoryHexdumpCommands(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        if (cmd.empty())
            throw std::runtime_error("expected <interval>");
        AddressInterval interval = parseAddressInterval(cmd[0]);
        uint8_t buffer[8192];
        HexdumpFormat fmt;
        while (!interval.isEmpty()) {
            size_t nRead = thread->get_process()->get_memory().at(interval.least())
                           .limit(std::min(interval.size(), rose_addr_t(sizeof buffer)))
                           .read(buffer).size();
            if (0==nRead)
                throw std::runtime_error("short read");
            if (interval.least() & 0xf)
                out_ <<"warning: not 16-byte aligned\n";
            SgAsmExecutableFileFormat::hexdump(out_, interval.least(), buffer, nRead, fmt);
            out_ <<"\n";
            if (interval.least() + nRead - 1 == interval.greatest())
                break;
            interval = AddressInterval::hull(interval.least()+nRead, interval.greatest());
        }
    }

    // Commands for examining memory data in small units. This command is similar to gdb's "x/" command.
    //   x/[<repeat>][<format>][<size>] <address>
    // If <repeat> is missing then 1 is assumed. Value must be decimal.
    // If <format> is missing then ROSE's native numeric format is assumed. Otherwise it should be:
    //   o(octal), h(hexadecimal), d(signed decimal), u(unsigned decimal), t(binary), f(floating point), a(address)
    //   c(ASCII character), s(NUL-terminated ASCII string).
    // If <size> is missing then the virtual CPU's natural word size is assumed. Otherwise it should be:
    //   b(byte), h(little-endian two bytes), w(little-endian four bytes), g(little-endian eight bytes)
    void memoryExamineCommands(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        ASSERT_require(!cmd.empty());
        ASSERT_require(boost::starts_with(cmd[0], "x/"));

        const char *s = cmd[0].c_str() + 2;
        char *rest;

        // Parse optional repeat count
        size_t n = strtoul(s, &rest, 10);
        n = std::max(size_t(1), n);
        s = rest;

        // Parse optional format letter
        char fmt = '\0';
        if (*s && strchr("oxdutfaics", *s)) // octal, hex, decimal, unsigned, binary, float, addr, insn, char, string
            fmt = *s++;

        // Parse optional size letter
        size_t nBytes = 0;
        switch (*s) {
            case 'b': nBytes = 1; ++s; break;
            case 'h': nBytes = 2; ++s; break;
            case 'w': nBytes = 4; ++s; break;
            case 'g': nBytes = 8; ++s; break;
            default: nBytes = thread->get_process()->wordSize() / 8; break;
        }
        if ('f'==fmt && nBytes != sizeof(float) && nBytes != sizeof(double))
            throw std::runtime_error("invalid size for floating point format");
        if (*s)
            throw std::runtime_error("extra letters in memory format string: \""+StringUtility::cEscape(cmd[0])+"\"");

        // Parse address
        if (cmd.size() < 2)
            throw std::runtime_error("address expected");
        rose_addr_t va = parseInteger(cmd[1]);

        // Display results
        for (size_t i=0; i<n; ++i) {

            // Read value and convert bytes to little endian if necessary
            uint8_t bytes[16];
            memset(bytes, 0xaa, sizeof bytes);          // debugging
            size_t nRead = 0;
            uint64_t value = 0;
            if (fmt!='i' && fmt!='s') {
                nRead = thread->get_process()->get_memory().at(va).limit(nBytes).read(bytes).size();
                if (nRead != nBytes)
                    throw std::runtime_error("short read");
                for (size_t j=0; j<nBytes; ++j)
                    value |= uint64_t(bytes[j]) << (j*8);
            }

            out_ <<StringUtility::addrToString(va) <<": ";
            switch (fmt) {
                case '\0':
                case 'a':
                case 'x':
                    out_ << StringUtility::toHex2(value, 8*nBytes) <<"\n";
                    va += nBytes;
                    break;
                case 'o': {
                    char buffer[64];
                    sprintf(buffer, "0%"PRIo64, value);
                    out_ <<buffer <<"\n";
                    va += nBytes;
                    break;
                }
                case 'd': {
                    char buffer[64];
                    int64_t sv = IntegerOps::signExtend2(value, 8*nBytes, 8*sizeof(value));
                    sprintf(buffer, "%"PRId64, sv);
                    out_ <<buffer <<"\n";
                    va += nBytes;
                    break;
                }
                case 'u': {
                    char buffer[64];
                    sprintf(buffer, "%"PRIu64, value);
                    out_ <<buffer <<"\n";
                    va += nBytes;
                    break;
                }
                case 't': {
                    Sawyer::Container::BitVector bv(8*nBytes);
                    bv.fromInteger(value);
                    out_ <<"0b" <<bv.toBinary() <<"\n";
                    va += nBytes;
                    break;
                }
                case 'f':
                    if (nBytes == sizeof(float)) {
                        out_ <<*(float*)&value <<"\n";
                    } else {
                        ASSERT_require(nBytes == sizeof(double));
                        out_ <<*(double*)&value <<"\n";
                    }
                    va += nBytes;
                    break;
                case 'i':
                    if (SgAsmInstruction *insn = thread->get_process()->get_instruction(va)) {
                        out_ <<unparseInstruction(insn) <<"\n";
                        va += insn->get_size();
                    } else {
                        out_ <<"<no instruction>\n";
                        va += 1;
                    }
                    break;
                case 'c':
                    out_ <<"\"" <<StringUtility::cEscape(std::string(bytes, bytes+nBytes)) <<"\"\n";
                    va += nBytes;
                    break;
                case 's': {
                    bool error = false;
                    std::string s = thread->get_process()->read_string(va, nBytes, &error);
                    out_ <<"\"" <<StringUtility::cEscape(s) <<"\"" <<(error?" (error)":"") <<"\n";
                    va += s.size() + (error?0:1);
                    break;
                }
                default:
                    ASSERT_not_reachable("invalid format");
            }
        }
    }

    // Commands that operate on memory
    //   (empty)                        -- show memory map
    //   del[ete] ...                   -- commands to delete memory areas
    //   dump ...                       -- commands to dump memory areas into files
    //   hex[dump] ...                  -- show memory hexdump
    //   prot[ection] ...               -- commands to change memory area protection bits
    //   map ...                        -- insert (or change) memory by loading (part of) a file
    //   x/...                          -- examine memory, similar to the GDB "x/" command
    void memoryCommand(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        if (cmd.empty()) {
            thread->get_process()->get_memory().dump(out_);
        } else if (cmd[0]=="del" || cmd[0]=="delete") {
            if (cmd.size()==1) {
                thread->get_process()->get_memory().clear();
            } else {
                AddressInterval where = parseAddressInterval(cmd[1]);
                thread->get_process()->get_memory().erase(where);
            }
        } else if (cmd[0]=="dump") {
            cmd.erase(cmd.begin());
            memoryDumpCommands(thread, cmd);
        } else if (cmd[0]=="hex" || cmd[0]=="hexdump") {
            cmd.erase(cmd.begin());
            memoryHexdumpCommands(thread, cmd);
        } else if (cmd[0]=="map") {
            cmd.erase(cmd.begin());
            memoryMapCommands(thread, cmd);
        } else if (cmd[0]=="p" || cmd[0]=="prot" || cmd[0]=="protect" || cmd[0]=="protection") {
            cmd.erase(cmd.begin());
            memoryProtectionCommands(thread, cmd);
        } else if (boost::starts_with(cmd[0], "x/")) {
            memoryExamineCommands(thread, cmd);
        } else {
            throw std::runtime_error("unknown memory command");
        }
    }

    // Breakpoint commands
    //   (empty)                        -- show all breakpoints
    //   <interval>                     -- set breakpoints at specified instruction addresses
    //   insn <insn_kind>               -- set breakpoint for certain kind of insn (e.g., "insn rdtsc")
    void breakPointCommands(RSIM_Thread *thread, std::vector<std::string> &cmd) {
        if (cmd.empty()) {
            if (!breakPointVas_.isEmpty()) {
                out_ <<"instruction addresses:\n";
                BOOST_FOREACH (const AddressInterval &interval, breakPointVas_.intervals())
                    out_ <<"  " <<interval <<"\n";
            }
            if (!breakPointKinds_.empty()) {
                out_ <<"instruction types:\n";
                BOOST_FOREACH (X86InstructionKind kind, breakPointKinds_)
                    out_ <<"  " <<stringifyX86InstructionKind(kind, "x86_") <<"\n";
            }
        } else if (cmd[0]=="insn" || cmd[0]=="instruction") {
            if (cmd.size() < 2)
                throw std::runtime_error("expected instruction <kind>");
            for (int i=0; i<x86_last_instruction; ++i) {
                if (stringifyX86InstructionKind(i, "x86_") == cmd[1]) {
                    breakPointKinds_.insert(X86InstructionKind(i));
                    return;
                }
            }
            throw std::runtime_error("unknown instruction mnemonic \"" + StringUtility::cEscape(cmd[1]) + "\"");
        } else {
            AddressInterval interval = parseAddressInterval(cmd[0]);
            breakPointVas_.insert(interval);
        }
    }
};


// Invoke debugger per instruction
class PerInstruction: public RSIM_Callbacks::InsnCallback {
    Debugger *debugger_;
public:
    PerInstruction(Debugger *debugger): debugger_(debugger) {}
    virtual PerInstruction *clone() { return this; }
    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && debugger_->shouldStop(args))
            return debugger_->repl(args);
        return enabled;
    }
};

// Attach interactive debugger to the simulator
void
attach(RSIM_Simulator &simulator, std::istream &in, std::ostream &out) {
    if (simulator.get_process())
        throw std::runtime_error("debugger must be attached before process is loaded"); // FIXME[Robb P. Matzke 2015-06-05]
    Debugger *debugger = new Debugger(in, out);
    simulator.install_callback(new PerInstruction(debugger));
}

} // namespace

#endif
