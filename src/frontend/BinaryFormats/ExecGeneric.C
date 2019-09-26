/* Copyright 2008 Lawrence Livermore National Security, LLC */
#include "sage3basic.h"
#include "checkIsModifiedFlag.h"
#include <algorithm>
#include <fstream>
#include <stringify.h>

#ifndef _MSC_VER
// DQ (11/27/2009): This header file is not available using MSVS (Windows).
#include <sys/wait.h>
#endif

using namespace Rose;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void
SgAsmExecutableFileFormat::unparseBinaryFormat(const std::string &name, SgAsmGenericFile *ef)
{
    std::ofstream f(name.c_str(), std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
    ROSE_ASSERT(f.is_open());
    f.exceptions(std::ios::badbit | std::ios::failbit);
    unparseBinaryFormat(f, ef);
    f.close();
}

void
SgAsmExecutableFileFormat::unparseBinaryFormat(std::ostream &f, SgAsmGenericFile *ef)
{
    ROSE_ASSERT(ef);

    if (checkIsModifiedFlag(ef))
        ef->reallocate();

    ef->unparse(f);
}

SgAsmGenericFile *
SgAsmExecutableFileFormat::parseBinaryFormat(const char *name)
{
    SgAsmGenericFile *ef=NULL;
    std::vector<DataConverter*> converters;
    converters.push_back(NULL); /*no conversion*/
    converters.push_back(new Rot13);

    struct Cleanup {
        std::vector<DataConverter*> &converters;
        SgAsmGenericFile *&ef;
        bool canceled;
        Cleanup(std::vector<DataConverter*> &converters, SgAsmGenericFile *&ef)
            : converters(converters), ef(ef), canceled(false) {}
        ~Cleanup() {
            if (!canceled) {
                BOOST_FOREACH (DataConverter *converter, converters)
                    delete converter;
                if (ef)
                    delete ef->get_data_converter();
                SageInterface::deleteAST(ef);
            }
        }
    } cleanup(converters, ef);

    for (size_t ci=0; !ef && ci<converters.size(); ci++) {
        ef = new SgAsmGenericFile();
        ef->set_data_converter(converters[ci]);
        converters[ci] = NULL;
        ef->parse(name);

        if (SgAsmElfFileHeader::is_ELF(ef)) {
            (new SgAsmElfFileHeader(ef))->parse();
        } else if (SgAsmDOSFileHeader::is_DOS(ef)) {
            SgAsmDOSFileHeader *dos_hdr = new SgAsmDOSFileHeader(ef);
            dos_hdr->parse(false); /*delay parsing the DOS Real Mode Section*/

            /* DOS Files can be overloaded to also be PE, NE, LE, or LX. Such files have an Extended DOS Header
             * immediately after the DOS File Header (various forms of Extended DOS Header exist). The Extended DOS Header
             * contains a file offset to a PE, NE, LE, or LX File Header, the first bytes of which are a magic number. The
             * is_* methods check for this magic number. */
            if (SgAsmPEFileHeader::is_PE(ef)) {
                SgAsmDOSExtendedHeader *dos2_hdr = new SgAsmDOSExtendedHeader(dos_hdr);
                dos2_hdr->parse();
                SgAsmPEFileHeader *pe_hdr = new SgAsmPEFileHeader(ef);
                pe_hdr->set_offset(dos2_hdr->get_e_lfanew());
                pe_hdr->parse();
            } else if (SgAsmNEFileHeader::is_NE(ef)) {
                SgAsmNEFileHeader::parse(dos_hdr);
            } else if (SgAsmLEFileHeader::is_LE(ef)) { /*or LX*/
                SgAsmLEFileHeader::parse(dos_hdr);
            }
            dos_hdr->parse_rm_section(0);
        } else {
            if (ef) delete ef->get_data_converter();
            SageInterface::deleteAST(ef);      /* ~SgAsmGenericFile() closes ef->p_fd if it was opened. */
            ef = NULL;
        }
    }

    /* If no executable file could be parsed then try to use system tools to get the file type. On Unix-based systems, the
     * "file" command does a thorough job of trying to figure out the type of file. If we can't figure out the file name then
     * just throw some generic error. */
    if (!ef) {
        /* Use file(1) to try to figure out the file type to report in the exception */
        int child_stdout[2];
#ifdef _MSC_VER
#pragma message ("WARNING: Commented out use of functions from sys/wait.h")
                printf ("ERROR: Commented out use of functions from sys/wait.h \n");
                ROSE_ASSERT(false);
#else
        if (pipe(child_stdout) == -1)
            throw FormatError("unrecognized file format for \"" + StringUtility::cEscape(name) + "\"");

        pid_t pid = fork();
        if (0==pid) {
            close(0);
            dup2(child_stdout[1], 1);
            close(child_stdout[0]);
            close(child_stdout[1]);
            execlp("file", "file", "-b", name, NULL);
            exit(1);
        } else if (pid>0) {
            char buf[4096];
            ssize_t nread = read(child_stdout[0], buf, sizeof buf);
            if (nread >= 0) {
                memset(buf+nread, 0, sizeof(buf)-nread);
            } else {
                memset(buf, 0, sizeof(buf));
            }
            buf[sizeof(buf)-1] = '\0';
            if (char *nl = strchr(buf, '\n')) *nl = '\0'; /*keep only first line w/o LF*/
            waitpid(pid, NULL, 0);
            char mesg[64+sizeof buf];
            sprintf(mesg, "unrecognized file format for \"%s\": %s", StringUtility::cEscape(name).c_str(), buf);
            throw FormatError(mesg);
        } else {
            throw FormatError("unrecognized file format for \"" + StringUtility::cEscape(name) + "\"");
        }
#endif
    }

    ef->set_tracking_references(false); /*all done parsing*/

    /* Is the file large enough to hold all sections?  If any section extends past the EOF then set truncate_zeros, which will
     * cause the unparser to not write zero bytes to the end of the file. */
    ef->set_truncate_zeros(ef->get_current_size()>ef->get_orig_size());

    /* If any section is the target of a function symbol then mark that section as containing code even if that section is not
     * memory mapped with execute permission. */

#ifndef USE_ROSE
 // DQ (1/27/2010): This is a problem for ROSE compiling this file.
    struct: public AstSimpleProcessing {
        void visit(SgNode *node) {
            SgAsmGenericSymbol *symbol = isSgAsmGenericSymbol(node);
            if (symbol && symbol->get_type()==SgAsmGenericSymbol::SYM_FUNC) {
                SgAsmGenericSection *section = symbol->get_bound();
                if (section)
                    section->set_contains_code(true);
            }
        }
    } t1;
    t1.traverse(ef, preorder);
#endif

    cleanup.canceled = true;
    return ef;
}
std::string SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::InsSetArchitecture isa)
{
  return isa_to_string(isa);
}

std::string
SgAsmExecutableFileFormat::isa_family_to_string(SgAsmExecutableFileFormat::InsSetArchitecture isa) {
    switch(isa & ISA_FAMILY_MASK){
        case ISA_IA32_Family:  return "Intel x86";     /* x86 IA-32 family of architectures; Intel, AMD, VIA, ... */
        case ISA_X8664_Family: return "x86-64";        /* x86-64 family of architectures: Intel, AMD, VIA, ... */
        case ISA_SPARC_Family: return "SPARC";         /* SPARC family of architectures; Sun Microsystems */
        case ISA_M68K_Family:  return "Motorola m68k"; /* Motorala m68k family */
        case ISA_M88K_Family:  return "Motorola m88k"; /* Motorola m88k family (not very popular) */
        case ISA_MIPS_Family:  return "MIPS";          /* 32/64-bit RISC; MIPS Technologies, Inc. */
        case ISA_I860_Family:  return "Intel i860";    /* Intel i860 family; 1989-mid 90's; RISC VLIW */
        case ISA_IA64_Family:  return "Itanium";       /* Intel 64-bit architecture */
        case ISA_ARM_Family:   return "ARM";           /* Acorn RISC Machine, Advanced RISC Machines, ARM Limited */
        case ISA_OTHER_Family:
            switch(isa){
                case ISA_PowerPC:
                case ISA_PowerPC_64bit: return "PowerPC";
                default:
                    return "Other Family";
            };
        default:
            break;
    };

    return "unknown isa family (" + StringUtility::unsignedToHex(isa & ISA_FAMILY_MASK) + ")";
}


std::string
SgAsmExecutableFileFormat::isa_to_string(SgAsmExecutableFileFormat::InsSetArchitecture isa) {
    if (const char *s = stringify::SgAsmExecutableFileFormat::InsSetArchitecture(isa))
        return s;
    return "unknown isa (" + StringUtility::unsignedToHex(isa) + ")";
}

std::string
SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::ExecFamily family) {
    switch(family){
        case FAMILY_UNSPECIFIED:  return "unspecified";
        case FAMILY_DOS:          return "Microsoft DOS";
        case FAMILY_ELF:          return "Executable and Linking Format (ELF)";
        case FAMILY_LE:           return "Microsoft Linear Executable (LE)";
        case FAMILY_LX:           return "OS/2 Extended Linear Executable (LX)";
        case FAMILY_NE:           return "Microsoft New Executable (NE)";
        case FAMILY_PE:           return "Microsoft Portable Executable (PE)";
    };

    return "unknown exec family (" + StringUtility::unsignedToHex(family) + ")";
}

std::string
SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::ExecABI abi) {
    switch(abi){
        case ABI_UNSPECIFIED: return "unspecified";
        case ABI_OTHER:       return "other";
        case ABI_86OPEN:      return "86Open Common IA32";
        case ABI_AIX:         return "AIX";
        case ABI_ARM:         return "ARM architecture";
        case ABI_FREEBSD:     return "FreeBSD";
        case ABI_HPUX:        return "HP/UX";
        case ABI_IRIX:        return "IRIX";
        case ABI_HURD:        return "GNU/Hurd";
        case ABI_LINUX:       return "GNU/Linux";
        case ABI_MODESTO:     return "Novell Modesto";
        case ABI_MONTEREY:    return "Monterey project";
        case ABI_MSDOS:       return "Microsoft DOS";
        case ABI_NT:          return "Windows NT";
        case ABI_NETBSD:      return "NetBSD";
        case ABI_OS2:         return "OS/2";
        case ABI_SOLARIS:     return "Sun Solaris";
        case ABI_SYSV:        return "SysV R4";
        case ABI_TRU64:       return "Compaq TRU64 UNIX";
        case ABI_WIN386:      return "Microsoft Windows";
    };

    return "unknown abi (" + StringUtility::unsignedToHex(abi) + ")";
}

std::string
SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::ExecPurpose purpose) {
    switch(purpose){
        case PURPOSE_UNSPECIFIED:   return "unspecified";
        case PURPOSE_OTHER:         return "other";
        case PURPOSE_EXECUTABLE:    return "executable program";
        case PURPOSE_LIBRARY:       return "library (shared or relocatable)";
        case PURPOSE_CORE_DUMP:     return "post mortem image (core dump)";
        case PURPOSE_OS_SPECIFIC:   return "operating system specific purpose";
        case PURPOSE_PROC_SPECIFIC: return "processor specific purpose";
    };

    return "unknown exec purpose (" + StringUtility::unsignedToHex(purpose) + ")";
}
