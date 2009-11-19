/* Classes describing basic features of a binary file that are in common to all file formats. (SgAsmGenericFormat class) */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Print some debugging info */
void
SgAsmGenericFormat::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096], sbuf[256];
    const char *s;
    if (idx>=0) {
        sprintf(p, "%sFormat[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sFormat.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    switch (get_family()) {
      case FAMILY_UNSPECIFIED:  s = "unspecified";                          break;
      case FAMILY_DOS:          s = "Microsoft DOS";                        break;
      case FAMILY_ELF:          s = "Executable and Linking Format (ELF)";  break;
      case FAMILY_LE:           s = "Microsoft Linear Executable (LE)";     break;
      case FAMILY_LX:           s = "OS/2 Extended Linear Executable (LX)"; break;
      case FAMILY_NE:           s = "Microsoft New Executable (NE)";        break;
      case FAMILY_PE:           s = "Microsoft Portable Executable (PE)";   break;
      default:
        sprintf(sbuf, "%u", get_family());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "family", s);

    switch (get_purpose()) {
      case PURPOSE_UNSPECIFIED:   s = "unspecified";                       break;
      case PURPOSE_OTHER:         s = "other";                             break;
      case PURPOSE_EXECUTABLE:    s = "executable program";                break;
      case PURPOSE_LIBRARY:       s = "library (shared or relocatable)";   break;
      case PURPOSE_CORE_DUMP:     s = "post mortem image (core dump)";     break;
      case PURPOSE_OS_SPECIFIC:   s = "operating system specific purpose"; break;
      case PURPOSE_PROC_SPECIFIC: s = "processor specific purpose";        break;
      default:
        sprintf(sbuf, "%u", get_purpose());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", s);

    switch (get_sex()) {
      case ORDER_UNSPECIFIED:     s = "unspecified";   break;
      case ORDER_LSB:             s = "little-endian"; break;
      case ORDER_MSB:             s = "big-endian";    break;
      default:
        sprintf(sbuf, "%u", get_sex());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "sex", s);

    fprintf(f, "%s%-*s = %u (%scurrent)\n", p, w, "version", get_version(), get_is_current_version() ? "" : "not-" );
    
    switch (get_abi()) {
      case ABI_UNSPECIFIED: s = "unspecified";        break;
      case ABI_86OPEN:      s = "86Open Common IA32"; break;
      case ABI_AIX:         s = "AIX";                break;
      case ABI_ARM:         s = "ARM architecture";   break;
      case ABI_FREEBSD:     s = "FreeBSD";            break;
      case ABI_HPUX:        s = "HP/UX";              break;
      case ABI_IRIX:        s = "IRIX";               break;
      case ABI_HURD:        s = "GNU/Hurd";           break;
      case ABI_LINUX:       s = "GNU/Linux";          break;
      case ABI_MODESTO:     s = "Novell Modesto";     break;
      case ABI_MONTEREY:    s = "Monterey project";   break;
      case ABI_MSDOS:       s = "Microsoft DOS";      break;
      case ABI_NT:          s = "Windows NT";         break;
      case ABI_NETBSD:      s = "NetBSD";             break;
      case ABI_SOLARIS:     s = "Sun Solaris";        break;
      case ABI_SYSV:        s = "SysV R4";            break;
      case ABI_TRU64:       s = "Compaq TRU64 UNIX";  break;
      default:
        sprintf(sbuf, "%u", get_abi());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n",  p, w, "ABI",      s);
    fprintf(f, "%s%-*s = %u\n",  p, w, "ABIvers",  get_abi_version());
    fprintf(f, "%s%-*s = %zu\n", p, w, "wordsize", get_word_size());
}
