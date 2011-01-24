#include "x86print.h"

#define __STDC_FORMAT_MACROS
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int
print_flags(FILE *f, const Translate *tlist, uint32_t value)
{
    int retval=0, nelmts=0;

    /* Print the non-zero flags until we don't find any more. */
    while (1) {
        bool found = 0;
        for (const Translate *t=tlist; t->str; t++) {
            if (0!=t->mask && 0!=t->val && (value & t->mask)==t->val) {
                retval += fprintf(f, "%s%s", nelmts++?"|":"", t->str);
                value &= ~(t->val);
                found = true;
            }
        }
        if (!found) break;
    }

    /* Look for a flag(s) corresponding to a zero value if we haven't printed anything yet */
    if (!nelmts && !value) {
        for (const Translate *t=tlist; t->str; t++) {
            if ((value & t->mask)==t->val) {
                retval += fprintf(f, "%s%s", nelmts++?"|":"", t->str);
                value &= ~(t->val);
            }
        }
    }

    /* Generically print what's left */
    if (nelmts==0 || value!=0) {
        if (nelmts++) retval += fprintf(f, "|");
        retval += print_hex(f, value);
    }

    return retval;
}

int
print_enum(FILE *f, const Translate *tlist, uint32_t value)
{
    for (const Translate *t=tlist; t->str; t++) {
        if (value==t->val)
            return fprintf(f, "%s", t->str);
    }
    return fprintf(f, "%"PRId32, value);
}

int
print_signed(FILE *f, uint32_t value)
{
    return fprintf(f, "%"PRId32, value);
}

int
print_pointer(FILE *f, uint32_t value)
{
    if (0==value)
        return fprintf(f, "NULL");
    return fprintf(f, "0x%08"PRIx32, value);
}

int
print_struct(FILE *f, uint32_t value, ArgInfo::StructPrinter printer, const uint8_t *buf, size_t need, size_t have)
{
    if (0==value)
        return fprintf(f, "NULL");
    if (have<need)
        return fprintf(f, "0x%08"PRIx32" {<short read>}", value);
    return (fprintf(f, "0x%08"PRIx32" {", value) +
            (printer)(f, buf, have) +
            fprintf(f, "}"));
}

int
print_hex(FILE *f, uint32_t value)
{
    if (value<10)
        return fprintf(f, "%"PRIu32, value);
    return fprintf(f, "0x%08"PRIx32, value);
}

int
print_string(FILE *f, const std::string &value, bool str_fault, bool str_trunc)
{
    int retval=fprintf(f, "\"");
    for (size_t i=0; i<value.size(); i++) {
        switch (value[i]) {
            case '"': retval += fprintf(f, "\\\""); break;
            case '\a': retval += fprintf(f, "\\a"); break;
            case '\b': retval += fprintf(f, "\\b"); break;
            case '\f': retval += fprintf(f, "\\f"); break;
            case '\n': retval += fprintf(f, "\\n"); break;
            case '\r': retval += fprintf(f, "\\r"); break;
            case '\t': retval += fprintf(f, "\\t"); break;
            case '\v': retval += fprintf(f, "\\v"); break;
            default:
                if (isprint(value[i])) {
                    retval += fprintf(f, "%c", value[i]);
                } else {
                    retval += fprintf(f, "\\%03o", (unsigned)value[i]);
                }
                break;
        }
    }
    retval += fprintf(f, "\"");
    if (str_fault || str_trunc)
        retval += fprintf(f, "...");
    if (str_fault)
        retval += fprintf(f, "[EFAULT]");
    return retval;
}

int
print_time(FILE *f, uint32_t value)
{
    time_t ts = value;
    struct tm tm;
    localtime_r(&ts, &tm);
    char buf[256];
    strftime(buf, sizeof buf, "%c", &tm);
    return fprintf(f, "%s", buf);
}

int
print_single(FILE *f, char fmt, const ArgInfo *info)
{
    int retval=0;
    switch (fmt) {
        case '-':
            retval += fprintf(f, "<unused>");
            break;
        case 'd':
            retval += print_signed(f, info->val);
            break;
        case 'e':
            retval += print_enum(f, info->xlate, info->val);
            break;
        case 'f':
            retval += print_flags(f, info->xlate, info->val);
            break;
        case 'p':
            retval += print_pointer(f, info->val);
            break;
        case 'P':
            retval += print_struct(f, info->val, info->struct_printer, info->struct_buf, info->struct_size, info->struct_nread);
            break;
        case 's':
            retval += print_string(f, info->str, info->str_fault, info->str_trunc);
            break;
        case 't':
            retval += print_time(f, info->val);
            break;
        case 'x':
            retval += print_hex(f, info->val);
            break;
        default: abort();
    }
    return retval;
}

int
print_enter(FILE *f, const char *name, const char *format, const ArgInfo *info)
{
    int retval = fprintf(f, "%s(", name);
    for (size_t i=0; format && format[i]; i++) {
        if (i>0)
            retval += fprintf(f, ", ");
        retval += print_single(f, format[i], info+i);
    }
    retval += fprintf(f, ")");
    int width = std::max(0, 42-retval);
    retval += fprintf(f, "%*s", width, " = ");
    return retval;
}

int
print_leave(FILE *f, char fmt, const ArgInfo *info)
{
    int retval = 0;
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
        retval += fprintf(f, "%"PRId32" ", info->val);
        retval += print_enum(f, t, en);
        retval += fprintf(f, " (%s)\n", strerror(en));
    } else {
        retval += print_single(f, fmt, info);
        retval += fprintf(f, "\n");
    }
    return retval;
}
