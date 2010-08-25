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
print_hex(FILE *f, uint32_t value)
{
    if (value<10)
        return fprintf(f, "%"PRIu32, value);
    return fprintf(f, "0x%08"PRIx32, value);
}

int
print_string(FILE *f, const std::string &value)
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
        case 'd':
            retval += print_signed(f, info->val);
            break;
        case 'e':
            retval += print_enum(f, info->xlate, info->val);
            break;
        case 'f': {
            retval += print_flags(f, info->xlate, info->val);
            break;
        case 'p':
            retval += print_pointer(f, info->val);
            break;
        case 's':
            retval += print_string(f, info->str);
            break;
        case 't':
            retval += print_time(f, info->val);
            break;
        case 'x':
            retval += print_hex(f, info->val);
            break;
        }
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
    int en = (int32_t)info->val < 0 ? -(int32_t)info->val : 0;
    if (en!=0) {
        /*FIXME: are we sure that these host numbers are valid for the target machine also? I hope so, because we use
         *       these symbolic names throughout the syscall handling code in x86sim.C [RPM 2010-08-07] */
        static const Translate t[] = { TF(EPERM), TF(ENOENT), TF(ESRCH), TF(EINTR), TF(EIO), TF(ENXIO), TF(E2BIG), TF(ENOEXEC),
                                       TF(EBADF), TF(ECHILD), TF(EAGAIN), TF(ENOMEM), TF(EACCES), TF(EFAULT), TF(ENOTBLK),
                                       TF(EBUSY), TF(EEXIST), TF(EXDEV), TF(ENODEV), TF(ENOTDIR), TF(EISDIR), TF(EINVAL),
                                       TF(ENFILE), TF(EMFILE), TF(ENOTTY), TF(ETXTBSY), TF(EFBIG), TF(ENOSPC), TF(ESPIPE),
                                       TF(EROFS), TF(EMLINK), TF(EPIPE), TF(EDOM), TF(ERANGE), TF(EDEADLK), TF(ENAMETOOLONG),
                                       TF(ENOLCK), TF(ENOSYS), TF(ENOTEMPTY), TF(ELOOP), TF(ENOMSG), TF(EIDRM),
#ifdef ECHRNG
                                       TF(ECHRNG),
#endif
#ifdef EL2NSYNC
                                       TF(EL2NSYNC),
#endif
#ifdef EL3HLT
                                       TF(EL3HLT),
#endif
#ifdef EL3RST
                                       TF(EL3RST),
#endif
#ifdef ELNRNG
                                       TF(ELNRNG),
#endif
#ifdef EUNATCH
                                       TF(EUNATCH),
#endif
#ifdef ENOCSI
                                       TF(ENOCSI),
#endif
#ifdef EL2HLT
                                       TF(EL2HLT),
#endif
#ifdef EBADE
                                       TF(EBADE),
#endif
#ifdef EBADR
                                       TF(EBADR),
#endif
#ifdef EXFULL
                                       TF(EXFULL),
#endif
#ifdef ENOANO
                                       TF(ENOANO),
#endif
#ifdef EBADRQC
                                       TF(EBADRQC),
#endif
#ifdef EBADSLT
                                       TF(EBADSLT),
#endif
#ifdef EBFONT
                                       TF(EBFONT),
#endif
#ifdef ENOSTR
                                       TF(ENOSTR),
#endif
#ifdef ENODATA
                                       TF(ENODATA),
#endif
#ifdef ETIME
                                       TF(ETIME),
#endif
#ifdef ENOSR
                                       TF(ENOSR),
#endif
#ifdef ENONET
                                       TF(ENONET),
#endif
#ifdef ENOPKG
                                       TF(ENOPKG),
#endif
#ifdef EREMOTE
                                       TF(EREMOTE),
#endif

#ifdef ENOLINK
                                       TF(ENOLINK),
#endif
#ifdef EADV
                                       TF(EADV),
#endif
#ifdef ESRMNT
                                       TF(ESRMNT),
#endif
#ifdef ECOMM
                                       TF(ECOMM),
#endif
#ifdef EPROTO
                                       TF(EPROTO),
#endif
#ifdef EMULTIHOP
                                       TF(EMULTIHOP),
#endif
#ifdef EDOTDOT
                                       TF(EDOTDOT),
#endif
#ifdef EBADMSG
                                       TF(EBADMSG),
#endif
#ifdef EOVERFLOW
                                       TF(EOVERFLOW),
#endif
#ifdef ENOTUNIQ
                                       TF(ENOTUNIQ),
#endif
#ifdef EBADFD
                                       TF(EBADFD),
#endif
#ifdef EREMCHG
                                       TF(EREMCHG),
#endif
#ifdef ELIBACC
                                       TF(ELIBACC),
#endif
#ifdef ELIBBAD
                                       TF(ELIBBAD),
#endif
#ifdef ELIBSCN
                                       TF(ELIBSCN),
#endif
#ifdef ELIBMAX
                                       TF(ELIBMAX),
#endif
#ifdef ELIBEXEC
                                       TF(ELIBEXEC),
#endif
#ifdef EILSEQ
                                       TF(EILSEQ),
#endif
#ifdef ERESTART
                                       TF(ERESTART),
#endif
#ifdef ESTRPIPE
                                       TF(ESTRPIPE),
#endif
#ifdef EUSERS
                                       TF(EUSERS),
#endif
#ifdef ENOTSOCK
                                       TF(ENOTSOCK),
#endif
#ifdef EDESTADDRREQ
                                       TF(EDESTADDRREQ),
#endif
#ifdef EMSGSIZE
                                       TF(EMSGSIZE),
#endif
#ifdef EPROTOTYPE
                                       TF(EPROTOTYPE),
#endif
#ifdef ENOPROTOOPT
                                       TF(ENOPROTOOPT),
#endif
#ifdef EPROTONOSUPPORT
                                       TF(EPROTONOSUPPORT),
#endif
#ifdef ESOCKTNOSUPPORT
                                       TF(ESOCKTNOSUPPORT),
#endif
#ifdef EOPNOTSUPP
                                       TF(EOPNOTSUPP),
#endif
#ifdef EPFNOSUPPORT
                                       TF(EPFNOSUPPORT),
#endif
#ifdef EAFNOSUPPORT
                                       TF(EAFNOSUPPORT),
#endif
#ifdef EADDRINUSE
                                       TF(EADDRINUSE),
#endif
#ifdef EADDRNOTAVAIL
                                       TF(EADDRNOTAVAIL),
#endif
#ifdef ENETDOWN
                                       TF(ENETDOWN),
#endif
#ifdef ENETUNREACH
                                       TF(ENETUNREACH),
#endif
#ifdef ENETRESET
                                       TF(ENETRESET),
#endif
#ifdef ECONNABORTED
                                       TF(ECONNABORTED),
#endif
#ifdef ECONNRESET
                                       TF(ECONNRESET),
#endif
#ifdef ENOBUFS
                                       TF(ENOBUFS),
#endif
#ifdef EISCONN
                                       TF(EISCONN),
#endif
#ifdef ENOTCONN
                                       TF(ENOTCONN),
#endif
#ifdef ESHUTDOWN
                                       TF(ESHUTDOWN),
#endif
#ifdef ETOOMANYREFS
                                       TF(ETOOMANYREFS),
#endif
#ifdef ETIMEDOUT
                                       TF(ETIMEDOUT),
#endif
#ifdef ECONNREFUSED
                                       TF(ECONNREFUSED),
#endif
#ifdef EHOSTDOWN
                                       TF(EHOSTDOWN),
#endif
#ifdef EHOSTUNREACH
                                       TF(EHOSTUNREACH),
#endif
#ifdef EALREADY
                                       TF(EALREADY),
#endif
#ifdef EINPROGRESS
                                       TF(EINPROGRESS),
#endif
#ifdef ESTALE
                                       TF(ESTALE),
#endif
#ifdef EUCLEAN
                                       TF(EUCLEAN),
#endif
#ifdef ENOTNAM
                                       TF(ENOTNAM),
#endif
#ifdef ENAVAIL
                                       TF(ENAVAIL),
#endif
#ifdef EISNAM
                                       TF(EISNAM),
#endif
#ifdef EREMOTEIO
                                       TF(EREMOTEIO),
#endif
#ifdef EDQUOT
                                       TF(EDQUOT),
#endif
#ifdef ENOMEDIUM
                                       TF(ENOMEDIUM),
#endif
#ifdef EMEDIUMTYPE
                                       TF(EMEDIUMTYPE),
#endif
#ifdef ECANCELED
                                       TF(ECANCELED),
#endif
#ifdef ENOKEY
                                       TF(ENOKEY),
#endif
#ifdef EKEYEXPIRED
                                       TF(EKEYEXPIRED),
#endif
#ifdef EKEYREVOKED
                                       TF(EKEYREVOKED),
#endif
#ifdef EKEYREJECTED
                                       TF(EKEYREJECTED),
#endif
#ifdef EOWNERDEAD
                                       TF(EOWNERDEAD),
#endif
#ifdef ENOTRECOVERABLE
                                       TF(ENOTRECOVERABLE),
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
