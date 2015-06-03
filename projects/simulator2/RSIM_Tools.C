#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "rose_strtoull.h"
#include "BaseSemantics2.h"
#include <boost/algorithm/string/trim.hpp>
#include <readline/readline.h>
#include <readline/history.h>

using namespace rose::BinaryAnalysis::InstructionSemantics2;

namespace RSIM_Tools {

std::vector<std::string>
InteractiveDebugger::readCommand(const Args &args) {
    while (1) {
        struct Resources {
            char *readline_buf;
            Resources(): readline_buf(NULL) {}
            ~Resources() {
                if (readline_buf)
                    free(readline_buf);
            }
        } r;

        r.readline_buf = readline("RSIM> ");
        if (!r.readline_buf)
            return std::vector<std::string>(1, "quit");

        std::vector<std::string> words;
        char *s = r.readline_buf;
        while (isspace(*s)) ++s;
        if ('#'==*s || !*s)
            continue;

        add_history(r.readline_buf);
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
            } else {
                words.back() += *s++;
            }
        }
        if (inQuote) {
            out_ <<"unterminated quote\n";
            continue;
        }
        if (words.back().empty() && !wordHadQuote)
            words.pop_back();
        return words;
    }
}

rose_addr_t
InteractiveDebugger::parseInteger(std::string s) {
    boost::trim(s);
    return Sawyer::Container::BitVector::parse(s).toInteger();
}

void
InteractiveDebugger::registerCommand(const Args &args, std::vector<std::string> &cmd) {
    if (cmd.empty()) {
        // r -- show register state
        out_ <<*args.thread->operators()->get_state()->get_register_state();
    } else {
        const RegisterDescriptor *reg = args.thread->get_process()->get_disassembler()->get_registers()->lookup(cmd[0]);
        if (!reg) {
            out_ <<"no such register \"" <<StringUtility::cEscape(cmd[0]) <<"\"\n";
            return;
        }

        // r <name> <value> -- set register <name> to <value>
        if (cmd.size() > 1) {
            rose_addr_t n = parseInteger(cmd[1]);
            BaseSemantics::SValuePtr value = args.thread->operators()->number_(reg->get_nbits(), n);
            args.thread->operators()->writeRegister(*reg, value);
        }

        // r <name> -- show register value (also show when setting)
        out_ <<*args.thread->operators()->readRegister(*reg) <<"\n";
    }
}

bool
InteractiveDebugger::operator()(bool enabled, const Args &args) {
    bool stopSingleStep = singleStep_-- == 0;
    if (enabled && !detached_ && (stopSingleStep || breakPoints_.exists(args.insn->get_address()))) {
        singleStep_ = false;
        while (true) {
            try {
                std::vector<std::string> cmd = readCommand(args);
                if (cmd.empty()) {
                    // prompt again
                } else if (std::string("c")==cmd[0] || std::string("continue")==cmd[0]) {
                    return enabled;
                } else if (std::string("detach") == cmd[0]) {
                    detached_ = true;
                    return true;
                } else if (std::string("q")==cmd[0] || std::string("quit")==cmd[0]) {
                    exit(0);
                } else if (std::string("r")==cmd[0] || std::string("reg")==cmd[0]) {
                    cmd.erase(cmd.begin());
                    registerCommand(args, cmd);
                } else if (std::string("s")==cmd[0] || std::string("step")==cmd[0] ||
                           std::string("si")==cmd[0] || std::string("stepi")==cmd[0]) {
                    singleStep_ = true;
                    return enabled;
                } else {
                    std::cerr <<"unknown command\n";
                }
            } catch (const std::runtime_error &e) {
                std::cerr <<"error: " <<e.what() <<"\n";
            }
        }
    }
    return enabled;
};

} // namespace

#endif
