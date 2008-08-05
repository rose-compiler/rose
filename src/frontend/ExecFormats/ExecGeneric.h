/* Copyright 2008 Lawrence Livermore National Security, LLC */
#ifndef Exec_ExecFormats_h
#define Exec_ExecFormats_h

#include <assert.h>
#include <map>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

// Added support to include ROSE IR nodes, to support translation
// #include "rose.h"
class SgAsmFile;

#define NELMTS(X)       (sizeof(X)/sizeof((X)[0]))      /* number of elements in a static-sized array */
#define DUMP_FIELD_WIDTH        64                      /* min columns to use for member names in dump() functions */

namespace Exec 
{

typedef uint64_t addr_t;                                /* address and size (file and memory) */
    

enum ByteOrder {
    ORDER_UNSPECIFIED,
    ORDER_LSB,                                          /* little-endian; least significant byte first */
    ORDER_MSB,                                          /* big-endian; most significant byte first */
};
    
enum ExecFamily {
    FAMILY_UNSPECIFIED,
    FAMILY_DOS,                                         /* Microsoft DOS format */
    FAMILY_ELF,                                         /* Unix Executable and Linking Format */
    FAMILY_LE,                                          /* Microsft Linear Executable format */
    FAMILY_LX,                                          /* OS/2 LX (Windows 9x VxD device drivers, extension of LE) */
    FAMILY_NE,                                          /* Microsoft New Executable Format */
    FAMILY_PE                                           /* Microsoft Portable Executable Format */
};

enum ExecABI {
    ABI_UNSPECIFIED,                                    /* Not specified in file and could not be inferred */
    ABI_OTHER,                                          /* Anything other than values below */
    ABI_86OPEN,                                         /* 86Open Common IA32 */
    ABI_AIX,                                            /* AIX */
    ABI_ARM,                                            /* ARM architecture */
    ABI_FREEBSD,                                        /* FreeBSD */
    ABI_HPUX,                                           /* HP/UX */
    ABI_IRIX,                                           /* IRIX */
    ABI_HURD,                                           /* GNU/Hurd */
    ABI_LINUX,                                          /* GNU/Linux */
    ABI_MODESTO,                                        /* Novell Modesto */
    ABI_MONTEREY,                                       /* Monterey project */
    ABI_MSDOS,                                          /* Microsoft DOS */
    ABI_NT,                                             /* Windows NT */
    ABI_NETBSD,                                         /* NetBSD */
    ABI_OS2,                                            /* OS/2 */
    ABI_SOLARIS,                                        /* Sun Solaris */
    ABI_SYSV,                                           /* SysV R4 */
    ABI_TRU64,                                          /* Compaq TRU64 UNIX */
    ABI_WIN386                                          /* Microsoft Windows */
};

/* Instruction sets organized by families */
enum InsSetArchitecture {
    ISA_UNSPECIFIED             = 0x0000,               /* File does not specify an architecture */
    ISA_OTHER                   = 0xffff,               /* Architecture is something other than below */
    ISA_FAMILY_MASK             = 0xff00,               /* Mask to get family part of ISA */

    ISA_IA32_Family             = 0x0100,               /* x86 IA-32 family of architectures; Intel, AMD, VIA, ... */
    ISA_IA32_286                = 0x0101,               /* 80286 */
    ISA_IA32_386                = 0x0102,               /* MMU with paging */
    ISA_IA32_486                = 0x0103,               /* risc-like pipelining, integrated FPU, on-chip cache */
    ISA_IA32_Pentium            = 0x0104,               /* superscalar, 64-bit databus, MMX */
    ISA_IA32_Cyrix6x86          = 0x0105,               /* register renaming, speculative execution */
    ISA_IA32_AMDK5              = 0x0106,               /* micro-op translation */
    ISA_IA32_PentiumPro         = 0x0107,               /* PAE, integrated L2 cache */
    ISA_IA32_PentiumII          = 0x0108,               /* L3-cache, 3D Now, SSE */
    ISA_IA32_Athlon             = 0x0109,               /* superscalar FPU, wide design */
    ISA_IA32_Pentium4           = 0x010a,               /* deeply pipelined, high frequency, SSE2, hyper-threading */
    ISA_IA32_PentiumM           = 0x010b,               /* low power */

    ISA_X8664_Family            = 0x0200,               /* x86-64 family of architectures: Intel, AMD, VIA, ... */
    ISA_X8664_Athlon64          = 0x0201,               /* on-die memory controller, 40-bit phys address space */
    ISA_X8664_Prescott          = 0x0202,               /* deeply pipelined, high frequency, SSE3 */
    ISA_X8664_IntelCore         = 0x0203,               /* low power, multi-core, lower clock frequency */
    ISA_X8664_AMDPhenom         = 0x0204,               /* quad core, 128-bit FPUs, SSE4a, native mem ctrl, on-die L3 cache */

    ISA_SPARC_Family            = 0x0300,               /* SPARC family of architectures; Sun Microsystems */
    ISA_SPARC_V7                = 0x0301,
    ISA_SPARC_V8                = 0x0302,
    ISA_SPARC_V8E               = 0x0303,
    ISA_SPARC_V9                = 0x0304,
    ISA_SPARC_V9JPS1            = 0x0305,
    ISA_SPARC_V9UA              = 0x0306,
    ISA_SPARC_V9JPS2            = 0x0307,

    ISA_M68K_Family             = 0x0400,               /* Motorala m68k family */
    ISA_M68K_68000              = 0x0401,               /* generation one: 16/32 internal; 8-, 16-, 32-bit interface */
    ISA_M68K_68EC000            = 0x0402,
    ISA_M68K_68HC000            = 0x0403,
    ISA_M68K_68008              = 0x0404,
    ISA_M68K_68010              = 0x0405,
    ISA_M68K_68012              = 0x0406,
    ISA_M68K_68020              = 0x0407,               /* generation two: fully 32-bit */
    ISA_M68K_68EC020            = 0x0408,
    ISA_M68K_68030              = 0x0409,
    ISA_M68K_68EC030            = 0x040a,
    ISA_M68K_68040              = 0x040b,               /* generation three: pipelined */
    ISA_M68K_68EC040            = 0x040c,
    ISA_M68K_68LC040            = 0x040d,
    ISA_M68K_68060              = 0x040e,               /* generation four: superscalar */
    ISA_M68K_ColdFire           = 0x040f,               /* other */
    ISA_M68K_DragonBall         = 0x0410,               /* other */

    ISA_M88K_Family             = 0x0500,               /* Motorola m88k family (not very popular) */
    ISA_M88K_88100              = 0x0501,               /* 32-bit, integrated FPU mated with 88200 MMU and cache controller */
    ISA_M88K_88110              = 0x0502,               /* single package of 88100+88200 */
    ISA_M88K_88110MP            = 0x0503,               /* on-chip comm for use in multi-processor systems */
    ISA_M88K_88120              = 0x0504,               /* superscalar (never actually released) */

    ISA_MIPS_Family             = 0x0600,               /* 32/64-bit RISC; MIPS Technologies, Inc. */
    ISA_MIPS_MarkI              = 0x0601,               /* R2000, R3000 */
    ISA_MIPS_MarkII             = 0x0602,               /* R6000 */
    ISA_MIPS_MarkIII            = 0x0603,               /* R4000 */
    ISA_MIPS_R2000              = 0x0604,               /* 32-bit, Big or little endian */
    ISA_MIPS_R3000              = 0x0605,               /* virtual identical: Pacempi's R3400, IDT's R3500, Toshiba R3900 */
    ISA_MIPS_R4000              = 0x0606,               /* 64-bit; others in the series had larger caches and bug fixes */
    ISA_MIPS_R4200              = 0x0607,               /* low-cost version of R4000 */
    ISA_MIPS_R4300              = 0x0608,               /* low-cost version of R4000 with 32-bit external bus */
    ISA_MIPS_R4600              = 0x0609,               /* "Orion" by Qauntum Effect Devices (QED); larger caches */
    ISA_MIPS_R4650              = 0x060a,               /* by QED */
    ISA_MIPS_R4700              = 0x060b,               /* "Orion" by QED */
    ISA_MIPS_R5000              = 0x060c,               /* by QED */
    ISA_MIPS_RM7000             = 0x060d,               /* by PMC-Sierra; 256kB L2 and optional L3 */
    ISA_MIPS_R8000              = 0x060e,               /* superscalar, fairly rare */
    ISA_MIPS_R10000             = 0x060f,               /* R8000 on a single chip; 32kB caches; out-of-order */
    ISA_MIPS_R12000             = 0x0610,               /* R10000 + higher clock rates */
    ISA_MIPS_R14000             = 0x0611,               /* R12000 + support for DDR SRAM; 200MHz front side bus */
    ISA_MIPS_R16000             = 0x0612,               /* R14000 + increased freq, more L1, smaller die */
    ISA_MIPS_R16000A            = 0x0613,
    ISA_MIPS_16                 = 0x0614,               /* Unknown. Windows PE architecture 0x266 "MIPS16" */
    ISA_MIPS_FPU                = 0x0615,               /* Unknown. Windows PE architecture 0x366 "MIPS with FPU" */
    ISA_MIPS_16FPU              = 0x0616,               /* Unknown. Windows PE architecture 0x466 "MIPS16 with FPU" */

    ISA_I860_Family             = 0x0700,               /* Intel i860 family; 1989-mid 90's; RISC VLIW */
    ISA_I860_860XR              = 0x0701,               /* (code named N10) 25-40MHz */
    ISA_I860_860XP              = 0x0702,               /* (code named N11) larger caches; 40-50MHz; same IS as XR */

    ISA_IA64_Family             = 0x0800,               /* Intel 64-bit architecture */
    ISA_IA64_Itanium            = 0x0801,               /* First generation */
    ISA_IA64_Itanium2           = 0x0802,               /* Second generation starting Nov 2007 */

    /* See http://en.wikipedia.org/wiki/ARM_architecture */
    ISA_ARM_Family              = 0x0900,               /* Acorn RISC Machine, Advanced RISC Machines, ARM Limited */
    ISA_ARM_ARM1                = 0x0901,               /* ARM evaluation system */
    ISA_ARM_ARM2                = 0x0902,               /* ARM2, ARM250 cores */
    ISA_ARM_ARM3                = 0x0903,               /* ARM2a core */
    ISA_ARM_ARM6                = 0x0904,               /* ARM60, ARM600, ARM610 cores */
    ISA_ARM_ARM7                = 0x0905,               /* ARM{700,710,710a,7100,7500,7500FE} cores */
    ISA_ARM_ARM7TDMI            = 0x0906,               /* ARM{7TDMI,7TDMI-S,710T,720T,740T,7EJ-S} cores */
    ISA_ARM_StrongARM           = 0x0907,               /* SA-110, SA-1110 cores */
    ISA_ARM_ARM8                = 0x0908,               /* ARM810 core */
    ISA_ARM_ARM9TDMI            = 0x0909,               /* ARM{9TDMI,920T,922T,940T} cores */
    ISA_ARM_ARM9E               = 0x090a,               /* ARM{946E-S,966E-S,968E-S,926EJ-S,966HS} cores */
    ISA_ARM_ARM10E              = 0x090b,               /* ARM{1020E,1022E,1026EJ-S} cores */
    ISA_ARM_XScale              = 0x090c,               /* 80200, IOP310, IOP315, 80219, IOP321, IOP33x, IOP34x, PXA210,
                                                         * PXA250, PXA255, PXA26x, PXA27x, PXA800(E)F, Monahans, PXA900,
                                                         * IXC1100, IXP2400, IXP2800, IXP2850, IXP2325, IXP2350, IXP42x,
                                                         * IXP460, IXP465 cores */
    ISA_ARM_ARM11               = 0x090d,               /* ARMv{6,6T2,6KZ,6K} cores */
    ISA_ARM_Cortex              = 0x090e,               /* Cortex-{A8,A9,A9 MPCore,R4(F),M3,M1} cores */

    /* Others, not yet incorporated into this enum */
    ISA_OTHER_Family            = 0xf000,

    ISA_ATT_WE_32100            = 0xf001,               /* sometimes simply "M32" */
    ISA_IBM_System_370          = 0xf002,
    ISA_HPPA                    = 0xf003,
    ISA_Fujitsu_VPP500          = 0xf004,
    ISA_Sun_v8plus              = 0xf005,
    ISA_PowerPC                 = 0xf006,
    ISA_PowerPC_64bit           = 0xf007,
    ISA_IBM_S390                = 0xf008,
    ISA_NEC_V800_series         = 0xf009,
    ISA_Fujitsu_FR20            = 0xf00a,
    ISA_TRW_RH_32               = 0xf00b,
    ISA_Motorola_RCE            = 0xf00c,
    ISA_Digital_Alpha_fake      = 0xf00e,
    ISA_Hitachi_SH              = 0xf00f,
    ISA_Siemens_Tricore         = 0xf010,
    ISA_Argonaut_RISC_Core      = 0xf011,
    ISA_Hitachi_H8_300          = 0xf012,
    ISA_Hitachi_H8_300H         = 0xf013,
    ISA_Hitachi_H8S             = 0xf014,
    ISA_Hitachi_H8_500          = 0xf015,
    ISA_Stanford_MIPS_X         = 0xf016,
    ISA_Motorola_M68HC12        = 0xf017,
    ISA_Fujitsu_MMA_Multimedia_Accelerator=0xf018,
    ISA_Siemens_PCP             = 0xf019,
    ISA_Sony_nCPU_embeeded_RISC = 0xf01a,
    ISA_Denso_NDR1_microprocessor=0xf01b,
    ISA_Motorola_Start_Core_processor=0xf01c,
    ISA_Toyota_ME16_processor   = 0xf01d,
    ISA_STMicroelectronic_ST100_processor=0xf01e,
    ISA_Advanced_Logic_Corp_Tinyj_emb_family=0xf01f,
    ISA_AMD_x86_64_architecture = 0xf020,
    ISA_Sony_DSP_Processor      = 0xf021,
    ISA_Siemens_FX66_microcontroller=0xf022,
    ISA_STMicroelectronics_ST9_plus_8_16_microcontroller=0xf023,
    ISA_STMicroelectronics_ST7_8bit_microcontroller=0xf024,
    ISA_Motorola_MC68HC16_microcontroller=0xf025,
    ISA_Motorola_MC68HC11_microcontroller=0xf026,
    ISA_Motorola_MC68HC08_microcontroller=0xf027,
    ISA_Motorola_MC68HC05_microcontroller=0xf028,
    ISA_Silicon_Graphics_SVx    = 0xf029,
    ISA_STMicroelectronics_ST19_8bit_microcontroller=0xf02a,
    ISA_Digital_VAX             = 0xf02b,
    ISA_Axis_Communications_32bit_embedded_processor=0xf02c,
    ISA_Infineon_Technologies_32bit_embedded_processor=0xf02d,
    ISA_Element_14_64bit_DSP_Processor=0xf02e,
    ISA_LSI_Logic_16bit_DSP_Processor=0xf02f,
    ISA_Donald_Knuths_educational_64bit_processor=0xf030,
    ISA_Harvard_University_machine_independent_object_files=0xf031,
    ISA_SiTera_Prism            = 0xf032,
    ISA_Atmel_AVR_8bit_microcontroller=0xf033,
    ISA_Fujitsu_FR30            = 0xf034,
    ISA_Mitsubishi_D10V         = 0xf035,
    ISA_Mitsubishi_D30V         = 0xf036,
    ISA_NEC_v850                = 0xf037,
    ISA_Mitsubishi_M32R         = 0xf038,
    ISA_Matsushita_MN10300      = 0xf039,
    ISA_Matsushita_MN10200      = 0xf03a,
    ISA_picoJava                = 0xf03b,
    ISA_OpenRISC_32bit_embedded_processor=0xf03c,
    ISA_ARC_Cores_Tangent_A5    = 0xf03d,
    ISA_Tensilica_Xtensa_Architecture=0xf03e,
    ISA_Digital_Alpha		= 0xf03f,
    ISA_Matsushita_AM33         = 0xf040,
    ISA_EFI_ByteCode            = 0xf041
};

class Architecture {
  public:
    Architecture(): isa(ISA_UNSPECIFIED) {}
    Architecture(InsSetArchitecture isa) {set_isa(isa);}
    void set_isa(InsSetArchitecture isa) {this->isa=isa; other=0;}
    void set_isa(InsSetArchitecture isa, unsigned data) {this->isa = isa; this->other=data;}
    InsSetArchitecture get_isa() const {return isa;}
  private:
    InsSetArchitecture  isa;                            /* Instruction set architecture */
    unsigned            other;                          /* Actual stored value if isa==ISA_OTHER */
};

/* General purpose of a binary executable file */
enum ExecPurpose {
    PURPOSE_UNSPECIFIED,                                /* Purpose is not specified and could not be inferred */
    PURPOSE_OTHER,                                      /* A purpose other than any defined below */
    PURPOSE_EXECUTABLE,                                 /* Executable program */
    PURPOSE_LIBRARY,                                    /* Library (shared or relocatable) */
    PURPOSE_CORE_DUMP,                                  /* Post mortem image */
    PURPOSE_OS_SPECIFIC,                                /* Some operating system specific purpose */
    PURPOSE_PROC_SPECIFIC                               /* Some processor specific purpose */
};

/* Information about a particular executable file format. */
class ExecFormat {
  public:
    ExecFormat():
        family(FAMILY_UNSPECIFIED),
        purpose(PURPOSE_UNSPECIFIED),
        sex(ORDER_UNSPECIFIED),
        version(0),
        is_current_version(false),
        abi(ABI_UNSPECIFIED),
        abi_version(0),
        word_size(4)
        {}
    void dump(FILE*, const char *prefix, ssize_t idx);

    ExecFamily          family;                         /* General format: ELF, PE, etc. */
    ExecPurpose         purpose;                        /* executable, library, etc. */
    ByteOrder           sex;
    unsigned            version;                        /* actual file format version number stored in file */
    bool                is_current_version;             /* is 'version' considered to be the current, supported version */
    ExecABI             abi;                            /* application binary interface */
    unsigned            abi_version;                    /* version of the ABI targeted by this file */
    size_t              word_size;                      /* in bytes (e.g., Elf32 is 4; Elf64 is 8) */
};

/* Reason for section's existence. This is a union of all the section purposes from the various supported file formats.
 * However, at this time we're only interested in a few kinds of sections and therefore most will fall into the SP_OTHER
 * category. */
enum SectionPurpose {
    SP_UNSPECIFIED,                                     /* File format did not specify a reason and none could be determined */
    SP_PROGRAM,                                         /* Program-supplied data, code, etc. */
    SP_HEADER,                                          /* Section contains a header for the executable file format */
    SP_SYMTAB,                                          /* Symbol table */
    SP_OTHER                                            /* File-specified purpose other than any given in this enum */
};                                                      

/* Thrown when an attempt is made to read past the end of a file, section, header, segment, etc. */
class ShortRead {
  public:
    ShortRead(class ExecSection *section, size_t offset, size_t size)
        : section(section), offset(offset), size(size) {}
    ShortRead(class ExecSection *section, size_t offset, size_t size, const std::string &mesg)
        : section(section), offset(offset), size(size), mesg(mesg) {}
    ShortRead(class ExecSection *section, size_t offset, size_t size, const char *mesg)
        : section(section), offset(offset), size(size), mesg(mesg) {}
    ExecSection         *section;                       /* Section from which read occurred; null implies file-level write */
    addr_t              offset;                         /* Byte offset into section (or file) */
    addr_t              size;                           /* Number of bytes of attempted read */
    std::string         mesg;                           /* Optional message */
};

/* Thrown when an attempt is made to write past the end of a file, section, header, segment, etc. */
class ShortWrite {
  public:
    ShortWrite(class ExecSection *section, size_t offset, size_t size)
        : section(section), offset(offset), size(size) {}
    ShortWrite(class ExecSection *section, size_t offset, size_t size, const std::string &mesg)
        : section(section), offset(offset), size(size), mesg(mesg) {}
    ShortWrite(class ExecSection *section, size_t offset, size_t size, const char *mesg)
        : section(section), offset(offset), size(size), mesg(mesg) {}
    ExecSection         *section;                       /* Section to which write occurred; null implies file-level write */
    addr_t              offset;                         /* Byte offset into section (or file) */
    addr_t              size;                           /* Number of bytes of attempted write */
    std::string         mesg;                           /* Optional message */
};


/* Thrown when the file contains an error that prevents us from parsing it. */
class FormatError {
public:
    FormatError(const std::string &mesg) {this->mesg=mesg;}
    FormatError(const char *mesg) {this->mesg=mesg;}
    std::string mesg;
};

        
/////////////////////////////////////////////// SageIII Nodes ///////////////////////////////////////////////////////////////

class ExecFile;                                         /* An executable file */
class ExecSection;                                      /* One section of an executable file */
class ExecHeader;                                       /* Executable file format header (e.g., Elf, PE, DOS, etc. file header */
class ExecDLL;                                          /* One required dynamically-linked library and its functions */
class ExecSymbol;

/* Represents an entire binary executable file. The file is mmap'd so that file data structures can be easily associated with
 * memory, so that the file contents is available to the child nodes of an ExecFile, and so that the data can be shared among
 * children (such as overlapping ExecSection objects). Note that there are no I/O methods for ExecFile; see ExecSection. */
class ExecFile {
  public:
    ExecFile(std::string file_name)
        : fd(-1), data(NULL)
        {ctor(file_name);}
    virtual ~ExecFile();                                /* Destructor deletes children and munmaps and closes file */
    void unparse(const char *filename);                 /* Unparse the file -- mirror image of parsing */
    void dump(FILE*);                                   /* Print some debugging info */

    /* File contents */
    addr_t get_size() {return sb.st_size;}              /* Size of file in bytes as of time it was opened */
    const unsigned char *content() {return data;}       /* File contents */

    /* Functions for sections (plurals return vectors; singular return one or NULL) */
    void add_section(ExecSection*);                     /* Add new section to the file; called implicitly by section constructor */
    std::vector<ExecSection*>& get_sections() {return sections;}/* all sections (including file headers) */
    ExecSection *get_section_by_id(int id);             /* Returns first section having specified ID */
    ExecSection *get_section_by_name(std::string, char sep='\0');/* Find section within file by name */
    std::vector<ExecSection*> get_sections_by_offset(addr_t offset, addr_t size);
    std::vector<ExecSection*> get_sections_by_rva(addr_t rva); /*Return sections mapped to specified relative virtual address */
    ExecSection *get_section_by_va(addr_t va);          /* Return single section mapped to specified virtual address */
    std::vector<ExecSection*> get_sections_by_va(addr_t va); /*Return all sections mapped to specified virtual address */
    addr_t get_next_section_offset(addr_t offset);      /* Find file offset for next section */
    void fill_holes();                                  /* Find holes in file and create sections to fill them */
    void unfill_holes();                                /* Undoes what fill_holes() did, returning hole sections to unused pool */

    /* Functions for file headers (a kind of section) */
    void add_header(ExecHeader*);                       /* Add a new file header to the list of headers for this file */
    std::vector<ExecHeader*>& get_headers() {return headers;}/* all file header sections */
    ExecHeader *get_header(ExecFamily);                 /* Returns the specified header (exception if more than one) */
    const char *format_name();                          /* Return a string describing the file format */

  private:
    void ctor(std::string file_name);
    int                 fd;                             /* File descriptor opened for read-only (or negative) */
    struct stat64       sb;                             /* File attributes at time of file open (valid if fd>=0) */
    unsigned char       *data;                          /* Content of file mapped into memory   (or null on file error) */
    std::vector<ExecSection*> sections;                 /* All known sections for this file */
    std::vector<ExecHeader*> headers;                   /* All format headers belonging to this file */
};

/* An executable file is loosely partitioned into sections (ExecSection) that are normally non-overlapping and cover the
 * entire file.  Some file formats explicitly define sections (e.g., ELF and PE), but sections are also used internally as a
 * bookkeeping mechanism by synthesizing sections to describe file headers, etc.
 *
 * ExecSection serves as a base class for many other file-level data structures.
 *
 * Every section belongs to exactly one file and and has a pointer to its ExecFile The ExecFile has a list of all of its
 * sections. An ExecSection can also appear in other lists which it may not know about (i.e., be careful when destroying it). */
class ExecSection {
  public:
    typedef std::multimap<addr_t,addr_t> RefMap;        /* Used for keeping track of referenced areas */
    typedef std::pair<addr_t,addr_t> ExtentPair;        /* Begin/end offsets for part of a section */
    typedef std::vector<ExtentPair> ExtentVector;       /* Vector of extents; usually non-overlapping, sorted by starting offset */

    ExecSection(ExecFile *f, addr_t offset, addr_t size)
        : file(NULL), header(NULL), size(0), offset(0), data(0), purpose(SP_UNSPECIFIED), synthesized(false),
        id(-1), mapped(false), mapped_rva(0), rperm(true), wperm(true), eperm(false),
        congealed(false)
        {ctor(f, offset, size);}
    virtual ~ExecSection();
    virtual void        dump(FILE*, const char *prefix, ssize_t idx);
    virtual void        unparse(FILE*);
    void                unparse(FILE*, const ExtentVector&);
    ExecHeader          *is_file_header();              /* true if section represents a top level file header */

    /* Functions for section extent within the file */
    ExecFile            *get_file() {return file;}      /* read-only */
    addr_t              get_size() {return size;}       /* read-only */
    addr_t              get_offset() {return offset;}   /* read-only */
    addr_t              end_offset() {return offset+size;} /* file offset for end of section */
    void                extend(addr_t nbytes);         /* make section larger by extending the end */
    void                extend_up_to(addr_t nbytes);      /* like extend() but more relaxed at EOF */
    addr_t              write(FILE *f, addr_t offset, size_t bufsize, const void *buf);
    addr_t              write(FILE *f, addr_t offset, const std::string &str) {
        return write(f, offset, str.size(), str.c_str());}
    addr_t              write(FILE *f, addr_t offset, char c) {
        return write(f, offset, 1, &c);}
        
    /* Functions for accessing content */
    const unsigned char *content(addr_t offset, addr_t size);/*returns ptr to SIZE bytes starting at OFFSET */
    void                content(addr_t offset, addr_t size, void *buf); /* copies content into BUF */
    const char          *content_str(addr_t offset);    /* returns ptr to NUL-terminated string starting at OFFSET */
    const ExtentVector& congeal();                      /* congeal referenced areas into holes */
    const RefMap&       uncongeal();                    /* uncongeal holes back into references */

    /* Functions related to mapping of sections into executable memory */
    bool                is_mapped() {return mapped;}
    void                set_mapped(addr_t rva, addr_t size) {mapped=true; mapped_rva=rva; mapped_size=size;}
    void                clear_mapped() {mapped=false; mapped_rva=mapped_size=0;}
    addr_t              get_mapped_rva() {return mapped ? mapped_rva : 0;}
    addr_t              get_mapped_size() {return mapped ? mapped_size : 0;}
    bool                get_eperm() {return eperm;}
    void                set_eperm(bool b) {eperm=b;}
    bool                get_wperm() {return wperm;}
    void                set_wperm(bool b) {wperm=b;}
    bool                get_rperm() {return rperm;}
    void                set_rperm(bool b) {rperm=b;}
    addr_t              get_va_offset(addr_t va);       /* Return file offset for specified virtual address */

    /* Accessors for private members */
    ExecHeader          *get_header() {return header;}
    void                set_header(ExecHeader *hdr) {header=hdr;}
    int                 get_id() {return id;}
    void                set_id(int i) {id=i;}
    const std::string&  get_name() {return name;}
    void                set_name(const char *s) {name=s;}
    void                set_name(const unsigned char *s) {name=(const char*)s;}
    void                set_name(std::string &s) {name=s;}
    SectionPurpose      get_purpose() {return purpose;}
    void                set_purpose(SectionPurpose p) {purpose=p;}
    void                set_synthesized(bool b) {synthesized=b;}
    bool                get_synthesized() {return synthesized;}

  protected:
    void                ctor(ExecFile*, addr_t offset, addr_t size);
    void                unparse_holes(FILE*);

    ExecFile            *file;                          /* The file to which this section belongs */
    ExecHeader          *header;                        /* Optional header associated with section */
    addr_t              size;                           /* Size of section in bytes */
    addr_t              offset;                         /* Starting offset of the section */
    const unsigned char *data;                          /* Content of just this section; points into file's content */
    SectionPurpose      purpose;                        /* General contents of the section */
    bool                synthesized;                    /* Section was created by the format reader; not specified in file */
    int                 id;                             /* Non-unique section ID (unique for ELF) or negative */
    std::string         name;                           /* Optional, non-unique name of section */
    bool                mapped;                         /* True if section should be mapped to program's address space */
    addr_t              mapped_rva;                     /* Intended relative virtual address if `mapped' is true */
    addr_t              mapped_size;                    /* Intended virtual size if 'mapped' is true */
    bool                rperm;                          /* Mapped by loader into memory having read permission */
    bool                wperm;                          /* Mapped by loader into memory having write permission */
    bool                eperm;                          /* Mapped by loader into memory having execute permission */
    RefMap              referenced;                     /* Begin/end offsets for areas referenced by extent() and extent_str() */
    bool                congealed;                      /* Is "holes" up to date w.r.t. referenced? */
    ExtentVector        holes;                          /* Unreferenced area (bigin/end offsets) */
};

/* An ExecHeader represents any kind of top-level file header. File headers generally define other file data structures such
 * as sections and segments. A single file may have multiple top-level headers (e.g., a Windows PE file has a PE header and an
 * MS-DOS header, each of which describes, in essence, a complete executable sub-file.
 *
 * Being an ExecSection, file headers are added to the list of sections maintained by the executable file. Their constructor
 * also adds them to the file's list of headers. */
class ExecHeader : public ExecSection {
  public:
    ExecHeader(ExecFile *ef, addr_t offset, addr_t size)
        : ExecSection(ef, offset, size),
        base_va(0)
        {ctor(ef, offset, size);}
    virtual ~ExecHeader();
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    virtual const char *format_name() = 0;
    
    /* Functions for dynamically linked libraries (DLLs) */
    void add_dll(ExecDLL *dll);                         /* Add new DLL to list of DLLs for this file */
    std::vector<ExecDLL*>& get_dlls() {return dlls;}    /* all necessary dynamically loaded libraries */

    /* Functions for symbols */
    void add_symbol(ExecSymbol*);                       /* add a new symbol to the symbol table. Duplicates are allowed. */
    std::vector<ExecSymbol*>& get_symbols() {return symbols;}

    /* Accessors for protected/private members */
    ExecFormat& get_exec_format() {return exec_format;}
    std::vector<unsigned char>& get_magic() {return magic;}
    const Architecture& get_target() const {return target;}
    addr_t get_base_va() const {return base_va;}
    addr_t get_entry_rva() const {return entry_rvas[0];}
    const std::vector<addr_t>& get_entry_rvas() const {return entry_rvas;}
    void add_entry_rva(addr_t rva) {entry_rvas.push_back(rva);}

    /* Convenience functions */
    ByteOrder get_sex() const {return exec_format.sex;}
    size_t get_word_size() const {return exec_format.word_size;}

  protected:
    ExecFormat          exec_format;                    /* General info about the executable format */
    std::vector<unsigned char> magic;                   /* Optional magic number in file byte order */
    Architecture        target;                         /* Machine for which this header and its sections, etc. was compiled */
    addr_t              base_va;                        /* Base virtual address used by all "relative virtual addresses" (RVA) */
    std::vector<addr_t> entry_rvas;                     /* Code entry points wrt base_va */
    std::vector<ExecDLL*> dlls;                         /* List of dynamic libraries needed by this executable */
    std::vector<ExecSymbol*> symbols;                   /* All symbols defined for this header */

  private:
    void ctor(ExecFile *f, addr_t offset, addr_t size);
};

/* Represents information about a dynamic library that must be linked at runtime. */
/* FIXME: ELF doesn't associate function names with the dll yet */
class ExecDLL {
  public:
    ExecDLL(std::string name) : name(name) {}
    virtual ~ExecDLL() {}
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    void add_function(std::string fname) {funcs.push_back(fname);} /* Add a needed function to the import list for this DLL */

    /* Accessors for protected/private data members */
    const std::string& get_name() const {return name;}
    void set_name(const std::string &s) {name=s;}

  private:
    std::string         name;                           /* Name of library as stored in executable (usually a base name) */
    std::vector<std::string> funcs;                     /* List of functions needed from the library */
};

enum SymbolDefState {
    SYM_UNDEFINED,                                      /* Symbol has not been defined yet */
    SYM_TENTATIVE,                                      /* Does not have size/value yet (uninitialized C or Fortran common blks) */
    SYM_DEFINED                                         /* Created and assigned storage */
};

enum SymbolType {
    SYM_NO_TYPE,                                        /* No type or type is unknown */
    SYM_DATA,                                           /* Normal variable definitions */
    SYM_FUNC,                                           /* Function */
    SYM_SECTION,                                        /* Section of a file */
    SYM_FILE,                                           /* Name of a file */
    SYM_ARRAY,                                          /* Array of some other type */
    SYM_TLS,                                            /* Thread-local storage */
    SYM_REGISTER                                        /* CPU register value (see Sparc) */
};

enum SymbolBinding {
    SYM_NO_BINDING,                                     /* Unknown binding */
    SYM_LOCAL,
    SYM_GLOBAL,
    SYM_WEAK
};

/* A symbol from a symbol table */
class ExecSymbol {
  public:
    ExecSymbol()
        : def_state(SYM_UNDEFINED), binding(SYM_NO_BINDING), type(SYM_NO_TYPE), value(0), size(0), bound(NULL)
        {}
    virtual ~ExecSymbol() {}
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data members */
    const std::string& get_name() {return name;}
    void set_name(std::string &s) {name=s;}
    void set_name(const std::string &s) {name=s;}
    void set_name(const char *s) {name=s;}
    addr_t get_value() {return value;}
    void set_value(addr_t v) {value=v;}
    addr_t get_size() {return size;}
    void set_size(addr_t sz) {size=sz;}
    SymbolType get_type() {return type;}
    void set_type(SymbolType t) {type=t;}
    ExecSection *get_bound() {return bound;}
    void set_bound(ExecSection *s) {bound=s;}

  protected:
    SymbolDefState      def_state;                      /* Undefined, created but not allocated, created and allocated, etc. */
    SymbolBinding       binding;                        /* local, global, etc. */
    SymbolType          type;                           /* file, section, variable, function, etc. */
    addr_t              value;                          /* symbol value or address if defined */
    addr_t              size;                           /* size of symbol if defined */
    ExecSection         *bound;                         /* section when defined locally */

  private:
    std::string         name;                           /* Symbol name may be the empty string */
};

void hexdump(FILE *f, addr_t base_addr, const char *prefix, const unsigned char *data, size_t n);
    
// DQ (6/15/2008): Picking a better name, using "parse" is a compiler project is difficult to trace.
// ExecFile *parse(const char *name);
ExecFile *parse(const char *name);

// DQ (6/15/2008): I have restored the previous function and written another one for use in ROSE.
// ExecFile *parseBinaryFormat(const std::string & name, SgAsmFile* asmFile);
void parseBinaryFormat(const std::string & name, SgAsmFile* asmFile);

/* Return byte order of caller */
inline ByteOrder host_order() {
    static const int i = 1;
    return *(unsigned char*)&i ? ORDER_LSB : ORDER_MSB;
}

/* Swap (reverse) bytes taking care of sign extension */
inline uint8_t swap_bytes(uint8_t n) {
    return n;
}
inline uint16_t swap_bytes(uint16_t n) {
    return ((n>>8) & 0xff) | ((n<<8) & 0xff00);
}
inline uint32_t swap_bytes(uint32_t n) {
    return ((n>>24) & 0xff) | ((n>>8) & 0xff00) | ((n<<8) & 0xff0000) | ((n<<24) & 0xff000000u);
}
inline uint64_t swap_bytes(uint64_t n) {
    return (((n>>56) & (0xffull<<0 )) | ((n>>40) & (0xffull<<8 )) | ((n>>24) & (0xffull<<16)) | ((n>>8 ) & (0xffull<<24)) |
            ((n<<8 ) & (0xffull<<32)) | ((n<<24) & (0xffull<<40)) | ((n<<40) & (0xffull<<48)) | ((n<<56) & (0xffull<<56)));
}
inline int8_t swap_bytes(int8_t n) {
    return swap_bytes((uint8_t)n);
}
inline int16_t swap_bytes(int16_t n) {
    return swap_bytes((uint16_t)n);
}
inline int32_t swap_bytes(int32_t n) {
    return swap_bytes((uint32_t)n);
}
inline int64_t swap_bytes(int64_t n) {
    return swap_bytes((uint64_t)n);
}


/* Little-endian byte order conversions */
inline uint8_t le_to_host(uint8_t n) {
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}
inline uint16_t le_to_host(uint16_t n) {
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}
inline uint32_t le_to_host(uint32_t n) {
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}
inline uint64_t le_to_host(uint64_t n) {
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}
inline int8_t le_to_host(int8_t n) {
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}
inline int16_t le_to_host(int16_t n) {
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}
inline int32_t le_to_host(int32_t n) {
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}
inline int64_t le_to_host(int64_t n) {
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

inline void host_to_le(unsigned h, uint8_t *n) {
    assert(0==(h & ~0xff));
    uint8_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_le(unsigned h, uint16_t *n) {
    assert(0==(h & ~0xffff));
    uint16_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_le(unsigned h, uint32_t *n) {
    assert(0==(h & ~0xfffffffful));
    uint32_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_le(addr_t h, uint64_t *n) {
    assert(0==(h & ~0xffffffffffffffffull));
    uint64_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_le(int h, int8_t *n) {
    assert((unsigned)h<=0x8f || ((unsigned)h|0xff)==(unsigned)-1);
    int8_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_le(int h, int16_t *n) {
    assert((unsigned)h<=0x8fff || ((unsigned)h|0xffff)==(unsigned)-1);
    int16_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_le(int h, int32_t *n) {
    assert((unsigned)h<=0x8fffffffu || ((unsigned)h|0xffffffffu)==(unsigned)-1);
    int32_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_le(int64_t h, int64_t *n) {
    *n = ORDER_LSB==host_order() ? h : swap_bytes(h);
}

/* Big-endian byte order conversions */
inline uint8_t be_to_host(uint8_t n) {
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}
inline uint16_t be_to_host(uint16_t n) {
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}
inline uint32_t be_to_host(uint32_t n) {
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}
inline uint64_t be_to_host(uint64_t n) {
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}
inline int8_t be_to_host(int8_t n) {
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}
inline int16_t be_to_host(int16_t n) {
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}
inline int32_t be_to_host(int32_t n) {
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}
inline int64_t be_to_host(int64_t n) {
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

inline void host_to_be(unsigned h, uint8_t *n) {
    assert(0==(h & ~0xff));
    uint8_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_be(unsigned h, uint16_t *n) {
    assert(0==(h & ~0xffff));
    uint16_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_be(unsigned h, uint32_t *n) {
    assert(0==(h & ~0xfffffffful));
    uint32_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_be(addr_t h, uint64_t *n) {
    assert(0==(h & ~0xffffffffffffffffull));
    uint64_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_be(int h, int8_t *n) {
    assert((unsigned)h<0x8f || ((unsigned)h|0xff)==(unsigned)-1);
    int8_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_be(int h, int16_t *n) {
    assert((unsigned)h<0x8fff || ((unsigned)h|0xffff)==(unsigned)-1);
    int16_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_be(int h, int32_t *n) {
    assert((unsigned)h<0x8ffffffful || ((unsigned)h|0xfffffffful)==(unsigned)-1);
    int32_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}
inline void host_to_be(int64_t h, int64_t *n) {
    *n = ORDER_MSB==host_order() ? h : swap_bytes(h);
}

/* Caller-specified byte order conversions */
inline uint8_t disk_to_host(ByteOrder sex, uint8_t n) {
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}
inline uint16_t disk_to_host(ByteOrder sex, uint16_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}
inline uint32_t disk_to_host(ByteOrder sex, uint32_t n) {
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}
inline uint64_t disk_to_host(ByteOrder sex, uint64_t n) {
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}
inline int8_t disk_to_host(ByteOrder sex, int8_t n) {
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}
inline int16_t disk_to_host(ByteOrder sex, int16_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}
inline int32_t disk_to_host(ByteOrder sex, int32_t n) {
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}
inline int64_t disk_to_host(ByteOrder sex, int64_t n) {
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

//inline void host_to_disk(ByteOrder, unsigned char h, unsigned char *np) {
//    *np = h;
//}
inline void host_to_disk(ByteOrder sex, unsigned h, uint8_t *np) {
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}
inline void host_to_disk(ByteOrder sex, unsigned h, uint16_t *np) {
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}
inline void host_to_disk(ByteOrder sex, unsigned h, uint32_t *np) {
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}
inline void host_to_disk(ByteOrder sex, addr_t h, uint64_t *np) {
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}
inline void host_to_disk(ByteOrder sex, int h, int8_t *np) {
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}
inline void host_to_disk(ByteOrder sex, int h, int16_t *np) {
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}
inline void host_to_disk(ByteOrder sex, int h, int32_t *np) {
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}
inline void host_to_disk(ByteOrder sex, int64_t h, int64_t *np) {
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}


};

#endif /* !Exec_ExecFormats_h */
