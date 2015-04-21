#include "rose.h"
#include "Diagnostics.h"

using namespace rose::Diagnostics;

#include "x86print.h"

#undef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

std::string
flags_to_str(const Translate *tlist, uint32_t value)
{
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
        s += hex_to_str(value);
    }
    return s;
}

void
print_flags(Sawyer::Message::Stream &m, const Translate *tlist, uint32_t value)
{
    m <<flags_to_str(tlist, value);
}

void
print_enum(Sawyer::Message::Stream &m, const Translate *tlist, uint32_t value)
{
    for (const Translate *t=tlist; t->str; t++) {
        if (value==t->val) {
            m <<t->str;
            return;
        }
    }
    mfprintf(m)("%"PRId32, value);
}

void
print_signed(Sawyer::Message::Stream &m, uint32_t value)
{
    mfprintf(m)("%"PRId32, value);
}

void
print_pointer(Sawyer::Message::Stream &m, uint32_t value)
{
    if (0==value) {
        m <<"NULL";
    } else {
        mfprintf(m)("0x%08"PRIx32, value);
    }
}

void
print_struct(Sawyer::Message::Stream &m, uint32_t value, ArgInfo::StructPrinter printer, const uint8_t *buf, size_t need,
             size_t have)
{
    if (0==value) {
        m <<"NULL";
    } else if (have<need) {
        mfprintf(m)("0x%08"PRIx32" {<short read>}", value);
    } else {
        mfprintf(m)("0x%08"PRIx32" {", value);
        (printer)(m, buf, have);
        m <<"}";
    }
}

std::string
hex_to_str(uint32_t value)
{
    char buf[64];
    if (value<10) {
        snprintf(buf, sizeof buf, "%"PRIu32, value);
    } else {
        snprintf(buf, sizeof buf, "0x%08"PRIx32, value);
    }
    return buf;
}

void
print_hex(Sawyer::Message::Stream &m, uint32_t value)
{
    m <<hex_to_str(value);
}

void
print_string(Sawyer::Message::Stream &m, const std::string &value, bool str_fault, bool str_trunc)
{
    m <<"\"" <<StringUtility::cEscape(value) <<"\"";
    if (str_fault || str_trunc)
        m <<"...";
    if (str_fault)
        m <<"[EFAULT]";
}

void
print_buffer(Sawyer::Message::Stream &m, uint32_t va, const uint8_t *buf, size_t actual_sz, size_t print_sz)
{
    size_t nchars = 0;
    mfprintf(m)("0x%08"PRIx32" [", va);
    for (size_t i=0; i<actual_sz && nchars<print_sz; i++) {
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
                    mfprintf(m)("%c", buf[i]);
                    nchars++;
                } else {
                    mfprintf(m)("\\%03o", (unsigned)buf[i]);
                    nchars+=3;
                }
                break;
        }
    }
    m <<"]";
    if (print_sz<actual_sz)
        m <<"...";
}

void
print_time(Sawyer::Message::Stream &m, uint32_t value)
{
    time_t ts = value;
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[256];
    strftime(buf, sizeof buf, "%c", &tm);
    mfprintf(m)("%"PRIu32" (%s)", value, buf);
}

/* letters are documented in RSIM_Thread::syscall_enter() */
void
print_single(Sawyer::Message::Stream &m, char fmt, const ArgInfo *info)
{
    switch (fmt) {
        case '-':
            m <<"<unused>";
            break;
        case 'b':
            print_buffer(m, info->val, info->struct_buf, info->struct_nread, info->struct_size);
            break;
        case 'd':
            print_signed(m, info->val);
            break;
        case 'e':
            print_enum(m, info->xlate, info->val);
            break;
        case 'f':
            print_flags(m, info->xlate, info->val);
            break;
        case 'p':
            print_pointer(m, info->val);
            break;
        case 'P':
            print_struct(m, info->val, info->struct_printer, info->struct_buf, info->struct_size, info->struct_nread);
            break;
        case 's':
            print_string(m, info->str, info->str_fault, info->str_trunc);
            break;
        case 't':
            print_time(m, info->val);
            break;
        case 'x':
            print_hex(m, info->val);
            break;
        default: abort();
    }
}

