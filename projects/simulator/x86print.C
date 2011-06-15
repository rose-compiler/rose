#include "rose.h"

#include "x86print.h"

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
print_flags(RTS_Message *m, const Translate *tlist, uint32_t value)
{
    m->more("%s", flags_to_str(tlist, value).c_str());
}

void
print_enum(RTS_Message *m, const Translate *tlist, uint32_t value)
{
    for (const Translate *t=tlist; t->str; t++) {
        if (value==t->val) {
            m->more("%s", t->str);
            return;
        }
    }
    m->more("%"PRId32, value);
}

void
print_signed(RTS_Message *m, uint32_t value)
{
    m->more("%"PRId32, value);
}

void
print_pointer(RTS_Message *m, uint32_t value)
{
    if (0==value) {
        m->more("NULL");
    } else {
        m->more("0x%08"PRIx32, value);
    }
}

void
print_struct(RTS_Message *m, uint32_t value, ArgInfo::StructPrinter printer, const uint8_t *buf, size_t need, size_t have)
{
    if (0==value) {
        m->more("NULL");
    } else if (have<need) {
        m->more("0x%08"PRIx32" {<short read>}", value);
    } else {
        m->more("0x%08"PRIx32" {", value);
        (printer)(m, buf, have);
        m->more("}");
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
print_hex(RTS_Message *m, uint32_t value)
{
    m->more("%s", hex_to_str(value).c_str());
}

void
print_string(RTS_Message *m, const std::string &value, bool str_fault, bool str_trunc)
{
    m->more("\"");
    for (size_t i=0; i<value.size(); i++) {
        switch (value[i]) {
            case '"' : m->more("\\\""); break;
            case '\a': m->more("\\a"); break;
            case '\b': m->more("\\b"); break;
            case '\f': m->more("\\f"); break;
            case '\n': m->more("\\n"); break;
            case '\r': m->more("\\r"); break;
            case '\t': m->more("\\t"); break;
            case '\v': m->more("\\v"); break;
            default:
                if (isprint(value[i])) {
                    m->more("%c", value[i]);
                } else {
                    m->more("\\%03o", (unsigned)value[i]);
                }
                break;
        }
    }
    m->more("\"");
    if (str_fault || str_trunc)
        m->more("...");
    if (str_fault)
        m->more("[EFAULT]");
}

void
print_buffer(RTS_Message *m, uint32_t va, const uint8_t *buf, size_t actual_sz, size_t print_sz)
{
    size_t nchars = 0;
    m->more("0x%08"PRIx32" [", va);
    for (size_t i=0; i<actual_sz && nchars<print_sz; i++) {
        switch (buf[i]) {
            case '"' : m->more("\\\""); nchars+=2; break;
            case '\a': m->more("\\a");  nchars+=2; break;
            case '\b': m->more("\\b");  nchars+=2; break;
            case '\f': m->more("\\f");  nchars+=2; break;
            case '\n': m->more("\\n");  nchars+=2; break;
            case '\r': m->more("\\r");  nchars+=2; break;
            case '\t': m->more("\\t");  nchars+=2; break;
            case '\v': m->more("\\v");  nchars+=2; break;
            default:
                if (isprint(buf[i])) {
                    m->more("%c", buf[i]);
                    nchars++;
                } else {
                    m->more("\\%03o", (unsigned)buf[i]);
                    nchars+=3;
                }
                break;
        }
    }
    m->more("]");
    if (print_sz<actual_sz)
        m->more("...");
}

void
print_time(RTS_Message *m, uint32_t value)
{
    time_t ts = value;
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[256];
    strftime(buf, sizeof buf, "%c", &tm);
    m->more("%s", buf);
}

/* letters are documented in RSIM_Thread::syscall_enter() */
void
print_single(RTS_Message *m, char fmt, const ArgInfo *info)
{
    switch (fmt) {
        case '-':
            m->more("<unused>");
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

