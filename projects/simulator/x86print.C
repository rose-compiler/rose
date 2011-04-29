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

void
print_leave(RTS_Message *m, char fmt, const ArgInfo *info)
{
    int en = (int32_t)info->val<0 && (int32_t)info->val>-256 ? -(int32_t)info->val : 0;
    if (en!=0) {
        /*FIXME: are we sure that these host numbers are valid for the target machine also? I hope so, because we use
         *       these symbolic names throughout the syscall handling code in x86sim.C [RPM 2010-08-07] */
        static const Translate t[] = { TE(EPERM), TE(ENOENT), TE(ESRCH), TE(EINTR), TE(EIO), TE(ENXIO), TE(E2BIG), TE(ENOEXEC),
                                       TE(EBADF), TE(ECHILD), TE(EAGAIN), TE(ENOMEM), TE(EACCES), TE(EFAULT), TE(ENOTBLK),
                                       TE(EBUSY), TE(EEXIST), TE(EXDEV), TE(ENODEV), TE(ENOTDIR), TE(EISDIR), TE(EINVAL),
                                       TE(ENFILE), TE(EMFILE), TE(ENOTTY), TE(ETXTBSY), TE(EFBIG), TE(ENOSPC), TE(ESPIPE),
                                       TE(EROFS), TE(EMLINK), TE(EPIPE), TE(EDOM), TE(ERANGE), TE(EDEADLK), TE(ENAMETOOLONG),
                                       TE(ENOLCK), TE(ENOSYS), TE(ENOTEMPTY), TE(ELOOP), TE(ENOMSG), TE(EIDRM),
#ifdef EAGAIN
                                       TE(EAGAIN),
#endif
#ifdef EWOULDBLOCK
                                       TE(EWOULDBLOCK),
#endif
#ifdef ECHRNG
                                       TE(ECHRNG),
#endif
#ifdef EL2NSYNC
                                       TE(EL2NSYNC),
#endif
#ifdef EL3HLT
                                       TE(EL3HLT),
#endif
#ifdef EL3RST
                                       TE(EL3RST),
#endif
#ifdef ELNRNG
                                       TE(ELNRNG),
#endif
#ifdef EUNATCH
                                       TE(EUNATCH),
#endif
#ifdef ENOCSI
                                       TE(ENOCSI),
#endif
#ifdef EL2HLT
                                       TE(EL2HLT),
#endif
#ifdef EBADE
                                       TE(EBADE),
#endif
#ifdef EBADR
                                       TE(EBADR),
#endif
#ifdef EXFULL
                                       TE(EXFULL),
#endif
#ifdef ENOANO
                                       TE(ENOANO),
#endif
#ifdef EBADRQC
                                       TE(EBADRQC),
#endif
#ifdef EBADSLT
                                       TE(EBADSLT),
#endif
#ifdef EBFONT
                                       TE(EBFONT),
#endif
#ifdef ENOSTR
                                       TE(ENOSTR),
#endif
#ifdef ENODATA
                                       TE(ENODATA),
#endif
#ifdef ETIME
                                       TE(ETIME),
#endif
#ifdef ENOSR
                                       TE(ENOSR),
#endif
#ifdef ENONET
                                       TE(ENONET),
#endif
#ifdef ENOPKG
                                       TE(ENOPKG),
#endif
#ifdef EREMOTE
                                       TE(EREMOTE),
#endif

#ifdef ENOLINK
                                       TE(ENOLINK),
#endif
#ifdef EADV
                                       TE(EADV),
#endif
#ifdef ESRMNT
                                       TE(ESRMNT),
#endif
#ifdef ECOMM
                                       TE(ECOMM),
#endif
#ifdef EPROTO
                                       TE(EPROTO),
#endif
#ifdef EMULTIHOP
                                       TE(EMULTIHOP),
#endif
#ifdef EDOTDOT
                                       TE(EDOTDOT),
#endif
#ifdef EBADMSG
                                       TE(EBADMSG),
#endif
#ifdef EOVERFLOW
                                       TE(EOVERFLOW),
#endif
#ifdef ENOTUNIQ
                                       TE(ENOTUNIQ),
#endif
#ifdef EBADFD
                                       TE(EBADFD),
#endif
#ifdef EREMCHG
                                       TE(EREMCHG),
#endif
#ifdef ELIBACC
                                       TE(ELIBACC),
#endif
#ifdef ELIBBAD
                                       TE(ELIBBAD),
#endif
#ifdef ELIBSCN
                                       TE(ELIBSCN),
#endif
#ifdef ELIBMAX
                                       TE(ELIBMAX),
#endif
#ifdef ELIBEXEC
                                       TE(ELIBEXEC),
#endif
#ifdef EILSEQ
                                       TE(EILSEQ),
#endif
#ifdef ERESTART
                                       TE(ERESTART),
#endif
#ifdef ESTRPIPE
                                       TE(ESTRPIPE),
#endif
#ifdef EUSERS
                                       TE(EUSERS),
#endif
#ifdef ENOTSOCK
                                       TE(ENOTSOCK),
#endif
#ifdef EDESTADDRREQ
                                       TE(EDESTADDRREQ),
#endif
#ifdef EMSGSIZE
                                       TE(EMSGSIZE),
#endif
#ifdef EPROTOTYPE
                                       TE(EPROTOTYPE),
#endif
#ifdef ENOPROTOOPT
                                       TE(ENOPROTOOPT),
#endif
#ifdef EPROTONOSUPPORT
                                       TE(EPROTONOSUPPORT),
#endif
#ifdef ESOCKTNOSUPPORT
                                       TE(ESOCKTNOSUPPORT),
#endif
#ifdef EOPNOTSUPP
                                       TE(EOPNOTSUPP),
#endif
#ifdef EPFNOSUPPORT
                                       TE(EPFNOSUPPORT),
#endif
#ifdef EAFNOSUPPORT
                                       TE(EAFNOSUPPORT),
#endif
#ifdef EADDRINUSE
                                       TE(EADDRINUSE),
#endif
#ifdef EADDRNOTAVAIL
                                       TE(EADDRNOTAVAIL),
#endif
#ifdef ENETDOWN
                                       TE(ENETDOWN),
#endif
#ifdef ENETUNREACH
                                       TE(ENETUNREACH),
#endif
#ifdef ENETRESET
                                       TE(ENETRESET),
#endif
#ifdef ECONNABORTED
                                       TE(ECONNABORTED),
#endif
#ifdef ECONNRESET
                                       TE(ECONNRESET),
#endif
#ifdef ENOBUFS
                                       TE(ENOBUFS),
#endif
#ifdef EISCONN
                                       TE(EISCONN),
#endif
#ifdef ENOTCONN
                                       TE(ENOTCONN),
#endif
#ifdef ESHUTDOWN
                                       TE(ESHUTDOWN),
#endif
#ifdef ETOOMANYREFS
                                       TE(ETOOMANYREFS),
#endif
#ifdef ETIMEDOUT
                                       TE(ETIMEDOUT),
#endif
#ifdef ECONNREFUSED
                                       TE(ECONNREFUSED),
#endif
#ifdef EHOSTDOWN
                                       TE(EHOSTDOWN),
#endif
#ifdef EHOSTUNREACH
                                       TE(EHOSTUNREACH),
#endif
#ifdef EALREADY
                                       TE(EALREADY),
#endif
#ifdef EINPROGRESS
                                       TE(EINPROGRESS),
#endif
#ifdef ESTALE
                                       TE(ESTALE),
#endif
#ifdef EUCLEAN
                                       TE(EUCLEAN),
#endif
#ifdef ENOTNAM
                                       TE(ENOTNAM),
#endif
#ifdef ENAVAIL
                                       TE(ENAVAIL),
#endif
#ifdef EISNAM
                                       TE(EISNAM),
#endif
#ifdef EREMOTEIO
                                       TE(EREMOTEIO),
#endif
#ifdef EDQUOT
                                       TE(EDQUOT),
#endif
#ifdef ENOMEDIUM
                                       TE(ENOMEDIUM),
#endif
#ifdef EMEDIUMTYPE
                                       TE(EMEDIUMTYPE),
#endif
#ifdef ECANCELED
                                       TE(ECANCELED),
#endif
#ifdef ENOKEY
                                       TE(ENOKEY),
#endif
#ifdef EKEYEXPIRED
                                       TE(EKEYEXPIRED),
#endif
#ifdef EKEYREVOKED
                                       TE(EKEYREVOKED),
#endif
#ifdef EKEYREJECTED
                                       TE(EKEYREJECTED),
#endif
#ifdef EOWNERDEAD
                                       TE(EOWNERDEAD),
#endif
#ifdef ENOTRECOVERABLE
                                       TE(ENOTRECOVERABLE),
#endif
#ifdef ENOTSUP
                                       TE(ENOTSUP),
#endif
#ifdef EPROCLIM
                                       TE(EPROCLIM),
#endif
#ifdef EBADRPC
                                       TE(EBADRPC),
#endif
#ifdef ERPCMISMATCH
                                       TE(ERPCMISMATCH),
#endif
#ifdef EPROGUNAVAIL
                                       TE(EPROGUNAVAIL),
#endif
#ifdef EPROGMISMATCH
                                       TE(EPROGMISMATCH),
#endif
#ifdef EPROCUNAVAIL
                                       TE(EPROCUNAVAIL),
#endif
#ifdef EFTYPE
                                       TE(EFTYPE),
#endif
#ifdef EAUTH
                                       TE(EAUTH),
#endif
#ifdef ENEEDAUTH
                                       TE(ENEEDAUTH),
#endif
#ifdef EPWROFF
                                       TE(EPWROFF),
#endif
#ifdef EDEVERR
                                       TE(EDEVERR),
#endif
#ifdef EBADEXEC
                                       TE(EBADEXEC),
#endif
#ifdef EBADARCH
                                       TE(EBADARCH),
#endif
#ifdef ESHLIBVERS
                                       TE(ESHLIBVERS),
#endif
#ifdef EBADMACHO
                                       TE(EBADMACHO),
#endif
#ifdef ENOATTR
                                       TE(ENOATTR), 
#endif
#ifdef ENOPOLICY
                                       TE(ENOPOLICY),
#endif
                                       T_END };
        m->more("%"PRId32" ", info->val);
        print_enum(m, t, en);
        m->more(" (%s)\n", strerror(en));
    } else {
        print_single(m, fmt, info);
        m->more("\n");
    }
}
