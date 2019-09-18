#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "Diagnostics.h"

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics;

Printer::~Printer() {
    out_ <<onDestruction_;
}

std::string
Printer::flags_to_str(const Translate *tlist, uint32_t value) {
    std::string s;
    int nelmts=0;

    /* Print the non-zero flags until we don't find any more. */
    while (1) {
        bool found = 0;
        for (const Translate *t=tlist; t->str; t++) {
            if (0!=t->mask && 0!=t->val && (value & t->mask)==t->val) {
                if (nelmts++) s += "|";
                s += t->str;
                value &= ~(t->val);
                found = true;
            }
        }
        if (!found) break;
    }

    /* Look for a flag(s) corresponding to a zero value if we haven't printed anything yet */
    if (!nelmts && !value) {
        for (const Translate *t=tlist; t->str; t++) {
            if (0!=t->mask && (value & t->mask)==t->val) {
                if (nelmts++) s += "|";
                s += t->str;
                value &= ~(t->val);
            }
        }
    }

    /* Print stuff with TF_FMT elements (t->mask is zero; t->val is the mask; t->str is the format) */
    for (const Translate *t=tlist; t->str; t++) {
        if (0==t->mask && 0!=t->val) {
            if (nelmts++) s += "|";
            char buf[256];
            snprintf(buf, sizeof buf, t->str, value & t->val);
            s += buf;
            value &= ~(t->val);
        }
    }

    /* Generically print what's left */
    if (nelmts==0 || value!=0) {
        if (nelmts++) s += "|";
        s += StringUtility::unsignedToHex(value);
    }
    return s;
}

uint64_t
Printer::nextArg() {
    if (argNum_ < 0)
        return thread_->syscall_arg(argNum_++);
    if (args32_)
        return args32_[argNum_++];
    if (args64_)
        return args64_[argNum_++];
    return thread_->syscall_arg(argNum_++);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::unused() {
    ++argNum_;
    out_ <<(++nPrinted_ > 1 ? ", " : "") <<"<unused>";
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::str(const std::string &s) {
    out_ <<s;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::arg(int n) {
    argNum_ = n;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::b(int64_t nbytes) {
    rose_addr_t va = nextArg();
    return b(va, nbytes);
}

Printer&
Printer::b(rose_addr_t va, int64_t nbytes) {
    uint8_t buf[1024];
    size_t nread = 0;
    if (nbytes > 0) {
        size_t need = std::min(size_t(nbytes), sizeof buf);
        nread = thread_->get_process()->mem_read(buf, va, need);
    }
    return b(va, buf, nread, 1024);
}

Printer&
Printer::b(rose_addr_t va, const uint8_t *buf, size_t actualSize, size_t printSize) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        print_buffer(out_, va, buf, actualSize, printSize);
    }
    return *this;
}

void
Printer::print_buffer(Sawyer::Message::Stream &m, rose_addr_t va, const uint8_t *buf, size_t actualSize, size_t printSize) {
    if (m) {
        size_t nchars = 0;
        m <<StringUtility::addrToString(va) <<" \"";
        for (size_t i=0; i<actualSize && nchars<printSize; i++) {
            switch (buf[i]) {
                case '"' : m <<"\\\""; nchars+=2; break;
                case '\a': m <<"\\a";  nchars+=2; break;
                case '\b': m <<"\\b";  nchars+=2; break;
                case '\f': m <<"\\f";  nchars+=2; break;
                case '\n': m <<"\\n";  nchars+=2; break;
                case '\r': m <<"\\r";  nchars+=2; break;
                case '\t': m <<"\\t";  nchars+=2; break;
                case '\v': m <<"\\v";  nchars+=2; break;
                default:
                    if (isprint(buf[i])) {
                        m <<(char)buf[i];
                        nchars++;
                    } else {
                        mfprintf(m)("\\%03o", (unsigned)buf[i]);
                        nchars += 4;
                    }
                    break;
            }
        }
        m <<"\"";
        if (printSize < actualSize)
            m <<"...";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::d() {
    uint64_t uv = nextArg();
    int64_t sv = IntegerOps::signExtend2(uv, thread_->get_process()->wordSize(), 64);
    return d(sv);
}

Printer&
Printer::d(int64_t value) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        print_decimal(out_, value);
    }
    return *this;
}

void
Printer::print_decimal(Sawyer::Message::Stream &m, int64_t value) {
    SAWYER_MESG(m) <<value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::e(const Translate *tlist) {
    uint64_t value = nextArg();
    return e(value, tlist);
}

Printer&
Printer::e(uint64_t value, const Translate *tlist) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        print_enum(out_, tlist, value);
    }
    return *this;
}

void
Printer::print_enum(Sawyer::Message::Stream &m, const Translate *tlist, uint64_t value) {
    if (m) {
        for (const Translate *t=tlist; t->str; t++) {
            if (value == t->val) {
                m <<t->str;
                return;
            }
        }
        m <<value;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::f(const Translate *tlist) {
    uint64_t value = nextArg();
    return f(value, tlist);
}

Printer&
Printer::f(uint64_t value, const Translate *tlist) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        print_flags(out_, tlist, value);
    }
    return *this;
}

void
Printer::print_flags(Sawyer::Message::Stream &m, const Translate *tlist, uint64_t value) {
    SAWYER_MESG(m) <<flags_to_str(tlist, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::p() {
    rose_addr_t value = nextArg();
    return p(value);
}
Printer&
Printer::p(rose_addr_t value) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        print_pointer(out_, value);
    }
    return *this;
}

void
Printer::print_pointer(Sawyer::Message::Stream &m, rose_addr_t va) {
    if (0==va) {
        SAWYER_MESG(m) <<"NULL";
    } else {
        SAWYER_MESG(m) <<StringUtility::addrToString(va);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::P(size_t nBytes, StructPrinter printer) {
    rose_addr_t va = nextArg();
    return P(va, nBytes, printer);
}

Printer&
Printer::P(rose_addr_t va, size_t nBytes, StructPrinter printer) {
    uint8_t *buffer = new uint8_t[nBytes];
    size_t nRead = thread_->get_process()->mem_read(buffer, va, nBytes);
    P(va, buffer, nBytes, nRead, printer);
    delete[] buffer;
    return *this;
}

Printer&
Printer::P(rose_addr_t va, const uint8_t *buffer, size_t need, size_t have, StructPrinter printer) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        print_struct(out_, va, buffer, need, have, printer);
    }
    return *this;
}

void
Printer::print_struct(Sawyer::Message::Stream &m, rose_addr_t va, const uint8_t *buf, size_t need, size_t have,
                      StructPrinter printer) {
    if (m) {
        if (0 == va) {
            m <<"NULL";
        } else if (have < need) {
            m <<StringUtility::addrToString(va) <<" {<short read>}";
        } else {
            m <<StringUtility::addrToString(va) <<" {";
            (printer)(m, buf, have);
            m <<"}";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::s() {
    rose_addr_t va = nextArg();
    return s(va);
}

Printer&
Printer::s(rose_addr_t va) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        static const size_t limit=1024;
        bool error = false;
        std::string str = thread_->get_process()->read_string(va, limit, &error);
        print_string(out_, va, str, str.size()==limit, error);
    }
    return *this;
}

void
Printer::print_string(Sawyer::Message::Stream &m, rose_addr_t va, const std::string &str, bool truncated, bool error) {
    if (m) {
        m <<StringUtility::addrToString(va) <<" ";
        print_string(m, str, truncated, error);
    }
}

void
Printer::print_string(Sawyer::Message::Stream &m, const std::string &str, bool truncated, bool error) {
    if (m) {
        m <<"\"" <<StringUtility::cEscape(str) <<"\"";
        if (truncated)
            m <<"...";
        if (error)
            m <<"[EFAULT]";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::t() {
    uint64_t value = nextArg();
    return t(value);
}

Printer&
Printer::t(uint64_t value) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        print_time(out_, value);
    }
    return *this;
}

void
Printer::print_time(Sawyer::Message::Stream &m, uint64_t value) {
    if (m) {
        time_t ts = value;
        struct tm tm;
        localtime_r(&ts, &tm);
        char buf[256];
        strftime(buf, sizeof buf, "%c", &tm);
        m <<value <<" " <<buf;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::x() {
    uint64_t value = nextArg();
    return x(value);
}

Printer&
Printer::x(uint64_t value) {
    if (!hadRetError_) {
        out_ <<(++nPrinted_ > 1 ? ", " : "");
        print_hex(out_, value);
    }
    return *this;
}

void
Printer::print_hex(Sawyer::Message::Stream &m, uint64_t value) {
    SAWYER_MESG(m) <<StringUtility::toHex(value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::ret() {
    size_t nbits = thread_->get_process()->get_simulator()->syscallReturnRegister().nBits();
    uint64_t unsignedRetval = nextArg();
    int64_t signedRetval = IntegerOps::signExtend2(unsignedRetval, nbits, 64);
    return ret(signedRetval);
}

Printer&
Printer::ret(int64_t retval) {
    hadRetError_ = print_ret(out_, retval);
    ++nPrinted_;
    return *this;
}

bool
Printer::print_ret(Sawyer::Message::Stream &m, int64_t retval) {
    if (m) {
        if (retval < 0 && retval >= -256) {
            int en = -retval;
            print_enum(m, error_numbers, en);
            return true;
        } else {
            m <<retval;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Printer&
Printer::eret() {
    RegisterDescriptor reg = thread_->get_process()->get_simulator()->syscallReturnRegister();
    uint64_t unsignedRetval = thread_->operators()->readRegister(reg)->get_number();
    int64_t signedRetval = IntegerOps::signExtend2(unsignedRetval, reg.nBits(), 64);
    return eret(signedRetval);
}

Printer&
Printer::eret(int64_t retval) {
    hadRetError_ = print_eret(out_, retval);
    return *this;
}

bool
Printer::print_eret(Sawyer::Message::Stream &m, int64_t retval) {
    if (m) {
        if (retval < 0 && retval >= -256) {
            int en = -retval;
            print_enum(m, error_numbers, en);
            m <<" ";
            return true;
        }
    }
    return false;
}


#endif
