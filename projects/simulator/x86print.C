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
                                       TF(ENOLCK), TF(ENOSYS), TF(ENOTEMPTY), TF(ELOOP), TF(ENOMSG), TF(EIDRM), TF(ECHRNG),
                                       TF(EL2NSYNC), TF(EL3HLT), TF(EL3RST), TF(ELNRNG), TF(EUNATCH), TF(ENOCSI), TF(EL2HLT),
                                       TF(EBADE), TF(EBADR), TF(EXFULL), TF(ENOANO), TF(EBADRQC), TF(EBADSLT), TF(EBFONT),
                                       TF(ENOSTR), TF(ENODATA), TF(ETIME), TF(ENOSR), TF(ENONET), TF(ENOPKG), TF(EREMOTE),
                                       TF(ENOLINK), TF(EADV), TF(ESRMNT), TF(ECOMM), TF(EPROTO), TF(EMULTIHOP), TF(EDOTDOT),
                                       TF(EBADMSG), TF(EOVERFLOW), TF(ENOTUNIQ), TF(EBADFD), TF(EREMCHG), TF(ELIBACC),
                                       TF(ELIBBAD), TF(ELIBSCN), TF(ELIBMAX), TF(ELIBEXEC), TF(EILSEQ), TF(ERESTART),
                                       TF(ESTRPIPE), TF(EUSERS), TF(ENOTSOCK), TF(EDESTADDRREQ), TF(EMSGSIZE), TF(EPROTOTYPE),
                                       TF(ENOPROTOOPT), TF(EPROTONOSUPPORT), TF(ESOCKTNOSUPPORT), TF(EOPNOTSUPP), TF(EPFNOSUPPORT),
                                       TF(EAFNOSUPPORT), TF(EADDRINUSE), TF(EADDRNOTAVAIL), TF(ENETDOWN), TF(ENETUNREACH),
                                       TF(ENETRESET), TF(ECONNABORTED), TF(ECONNRESET), TF(ENOBUFS), TF(EISCONN), TF(ENOTCONN),
                                       TF(ESHUTDOWN), TF(ETOOMANYREFS), TF(ETIMEDOUT), TF(ECONNREFUSED), TF(EHOSTDOWN),
                                       TF(EHOSTUNREACH), TF(EALREADY), TF(EINPROGRESS), TF(ESTALE), TF(EUCLEAN), TF(ENOTNAM),
                                       TF(ENAVAIL), TF(EISNAM), TF(EREMOTEIO), TF(EDQUOT), TF(ENOMEDIUM), TF(EMEDIUMTYPE),
                                       TF(ECANCELED), TF(ENOKEY), TF(EKEYEXPIRED), TF(EKEYREVOKED), TF(EKEYREJECTED),
                                       TF(EOWNERDEAD), TF(ENOTRECOVERABLE), T_END };
        retval += fprintf(f, "%"PRId32" ", info->val);
        retval += print_enum(f, t, en);
        retval += fprintf(f, " (%s)\n", strerror(en));
    } else {
        retval += print_single(f, fmt, info);
        retval += fprintf(f, "\n");
    }
    return retval;
}
