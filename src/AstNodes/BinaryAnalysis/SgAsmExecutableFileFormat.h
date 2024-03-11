/** Base class for many binary analysis nodes. */
[[Rosebud::abstract]]
class SgAsmExecutableFileFormat: public SgAsmNode {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Exception for reading past the end of something.
     *
     *  This object is thrown when an attempt is made to read past the end of a file, section, header, segment, etc. */
    class ShortRead: public Rose::Exception {
    public:
        const SgAsmGenericSection *section;         /**< Section from which read occurred; null implies file-level write. */
        rose_addr_t offset;                         /**< Byte offset into section (or file). */
        rose_addr_t size;                           /**< Number of bytes of attempted read. */

        ShortRead(const class SgAsmGenericSection *section, size_t offset, size_t size)
            : Rose::Exception("short read"), section(section), offset(offset), size(size) {}
        ShortRead(const class SgAsmGenericSection *section, size_t offset, size_t size, const std::string &mesg)
            : Rose::Exception(mesg), section(section), offset(offset), size(size) {}
        ShortRead(const class SgAsmGenericSection *section, size_t offset, size_t size, const char *mesg)
            : Rose::Exception(mesg), section(section), offset(offset), size(size) {}
        ~ShortRead() throw () {}
    };

    /** Exception for writing past the end of something.
     *
     *  This object is thrown when an attempt is made to write past the end of a file, section, header, segment, etc. */
    class ShortWrite: public Rose::Exception {
    public:
        const SgAsmGenericSection *section;         /**< Section to which write occurred; null implies file-level write. */
        rose_addr_t          offset;                /**< Byte offset into section (or file). */
        rose_addr_t          size;                  /**< Number of bytes of attempted write. */

        ShortWrite(const class SgAsmGenericSection *section, size_t offset, size_t size)
            : Rose::Exception(""), section(section), offset(offset), size(size) {}
        ShortWrite(const class SgAsmGenericSection *section, size_t offset, size_t size, const std::string &mesg)
            : Rose::Exception(mesg), section(section), offset(offset), size(size) {}
        ShortWrite(const class SgAsmGenericSection *section, size_t offset, size_t size, const char *mesg)
            : Rose::Exception(mesg), section(section), offset(offset), size(size) {}
    };

    /** Exception for container syntax errors.
     *
     *  This object is thrown when the file contains an error that prevents ROSE from parsing it. */
    class FormatError: public Rose::Exception {
    public:
        FormatError(const std::string &mesg)
            : Rose::Exception(mesg) {}
        FormatError(const char *mesg)
            : Rose::Exception(mesg) {}
        ~FormatError() throw () {}
    };

    /** Information about the file in the filesystem. */
    typedef struct stat fileDetails;

    /** Architecture family. */
    enum ExecFamily {
        FAMILY_UNSPECIFIED,                         /**< Unspecified family. */
        FAMILY_DOS,                                 /**< Microsoft DOS format. */
        FAMILY_ELF,                                 /**< Unix Executable and Linking Format. */
        FAMILY_JVM,                                 /**< Java Virtual Machine (JVM) format. */
        FAMILY_LE,                                  /**< Microsft Linear Executable format. */
        FAMILY_LX,                                  /**< OS/2 LX (Windows 9x VxD device drivers, extension of LE). */
        FAMILY_NE,                                  /**< Microsoft New Executable Format. */
        FAMILY_PE                                   /**< Microsoft Portable Executable Format. */
    };

    /** Application binary interface. */
    enum ExecABI {
        ABI_UNSPECIFIED,                            /**< Not specified in file and could not be inferred */
        ABI_OTHER,                                  /**< Anything other than values below */
        ABI_86OPEN,                                 /**< 86Open Common IA32 */
        ABI_AIX,                                    /**< AIX */
        ABI_ARM,                                    /**< ARM architecture */
        ABI_FREEBSD,                                /**< FreeBSD */
        ABI_HPUX,                                   /**< HP/UX */
        ABI_IRIX,                                   /**< IRIX */
        ABI_HURD,                                   /**< GNU/Hurd */
        ABI_JVM,                                    /**< Java Virtual Machine (JVM). */
        ABI_LINUX,                                  /**< GNU/Linux */
        ABI_MODESTO,                                /**< Novell Modesto */
        ABI_MONTEREY,                               /**< Monterey project */
        ABI_MSDOS,                                  /**< Microsoft DOS */
        ABI_NT,                                     /**< Windows NT */
        ABI_NETBSD,                                 /**< NetBSD */
        ABI_OS2,                                    /**< OS/2 */
        ABI_SOLARIS,                                /**< Sun Solaris */
        ABI_SYSV,                                   /**< SysV R4 */
        ABI_TRU64,                                  /**< Compaq TRU64 UNIX */
        ABI_WIN386                                  /**< Microsoft Windows */
    };

    /** Instruction sets organized by families */
    enum InsSetArchitecture {
        ISA_UNSPECIFIED             = 0x0000,       /**< File does not specify an architecture */
        ISA_OTHER                   = 0xffff,       /**< Architecture is something other than below */
        ISA_FAMILY_MASK             = 0xff00,       /**< Mask to get family part of ISA */

        ISA_IA32_Family             = 0x0100,       /**< x86 IA-32 family of architectures; Intel, AMD, VIA, ... */
        ISA_IA32_286                = 0x0101,       /**< 80286 */
        ISA_IA32_386                = 0x0102,       /**< MMU with paging */
        ISA_IA32_486                = 0x0103,       /**< risc-like pipelining, integrated FPU, on-chip cache */
        ISA_IA32_Pentium            = 0x0104,       /**< superscalar, 64-bit databus, MMX */
        ISA_IA32_Cyrix6x86          = 0x0105,       /**< register renaming, speculative execution */
        ISA_IA32_AMDK5              = 0x0106,       /**< micro-op translation */
        ISA_IA32_PentiumPro         = 0x0107,       /**< PAE, integrated L2 cache */
        ISA_IA32_PentiumII          = 0x0108,       /**< L3-cache, 3D Now, SSE */
        ISA_IA32_Athlon             = 0x0109,       /**< superscalar FPU, wide design */
        ISA_IA32_Pentium4           = 0x010a,       /**< deeply pipelined, high frequency, SSE2, hyper-threading */
        ISA_IA32_PentiumM           = 0x010b,       /**< low power */

        ISA_X8664_Family            = 0x0200,       /**< x86-64 family of architectures: Intel, AMD, VIA, ... */
        ISA_X8664_Athlon64          = 0x0201,       /**< on-die memory controller, 40-bit phys address space */
        ISA_X8664_Prescott          = 0x0202,       /**< deeply pipelined, high frequency, SSE3 */
        ISA_X8664_IntelCore         = 0x0203,       /**< low power, multi-core, lower clock frequency */
        ISA_X8664_AMDPhenom         = 0x0204,       /**< quad core, 128-bit FPUs, SSE4a, native mem ctrl, on-die L3 cache */

        ISA_SPARC_Family            = 0x0300,       /**< SPARC family of architectures; Sun Microsystems */
        ISA_SPARC_V7                = 0x0301,
        ISA_SPARC_V8                = 0x0302,
        ISA_SPARC_V8E               = 0x0303,
        ISA_SPARC_V9                = 0x0304,
        ISA_SPARC_V9JPS1            = 0x0305,
        ISA_SPARC_V9UA              = 0x0306,
        ISA_SPARC_V9JPS2            = 0x0307,

        ISA_M68K_Family             = 0x0400,       /**< Motorala m68k family */
        ISA_M68K_68000              = 0x0401,       /**< generation one: 16/32 internal; 8-, 16-, 32-bit interface */
        ISA_M68K_68EC000            = 0x0402,
        ISA_M68K_68HC000            = 0x0403,
        ISA_M68K_68008              = 0x0404,
        ISA_M68K_68010              = 0x0405,
        ISA_M68K_68012              = 0x0406,
        ISA_M68K_68020              = 0x0407,       /**< generation two: fully 32-bit */
        ISA_M68K_68EC020            = 0x0408,
        ISA_M68K_68030              = 0x0409,
        ISA_M68K_68EC030            = 0x040a,
        ISA_M68K_68040              = 0x040b,       /**< generation three: pipelined */
        ISA_M68K_68EC040            = 0x040c,
        ISA_M68K_68LC040            = 0x040d,
        ISA_M68K_68060              = 0x040e,       /**< generation four: superscalar */
        ISA_M68K_ColdFire           = 0x040f,       /**< other */
        ISA_M68K_DragonBall         = 0x0410,       /**< other */

        ISA_M88K_Family             = 0x0500,       /**< Motorola m88k family (not very popular) */
        ISA_M88K_88100              = 0x0501,       /**< 32-bit, integrated FPU mated with 88200 MMU and cache controller */
        ISA_M88K_88110              = 0x0502,       /**< single package of 88100+88200 */
        ISA_M88K_88110MP            = 0x0503,       /**< on-chip comm for use in multi-processor systems */
        ISA_M88K_88120              = 0x0504,       /**< superscalar (never actually released) */

        ISA_MIPS_Family             = 0x0600,       /**< 32/64-bit RISC; MIPS Technologies, Inc. */
        ISA_MIPS_MarkI              = 0x0601,       /**< R2000, R3000 */
        ISA_MIPS_MarkII             = 0x0602,       /**< R6000 */
        ISA_MIPS_MarkIII            = 0x0603,       /**< R4000 */
        ISA_MIPS_R2000              = 0x0604,       /**< 32-bit, Big or little endian */
        ISA_MIPS_R3000              = 0x0605,       /**< virtual identical: Pacempi's R3400, IDT's R3500, Toshiba R3900 */
        ISA_MIPS_R4000              = 0x0606,       /**< 64-bit; others in the series had larger caches and bug fixes */
        ISA_MIPS_R4200              = 0x0607,       /**< low-cost version of R4000 */
        ISA_MIPS_R4300              = 0x0608,       /**< low-cost version of R4000 with 32-bit external bus */
        ISA_MIPS_R4600              = 0x0609,       /**< "Orion" by Qauntum Effect Devices (QED); larger caches */
        ISA_MIPS_R4650              = 0x060a,       /**< by QED */
        ISA_MIPS_R4700              = 0x060b,       /**< "Orion" by QED */
        ISA_MIPS_R5000              = 0x060c,       /**< by QED */
        ISA_MIPS_RM7000             = 0x060d,       /**< by PMC-Sierra; 256kB L2 and optional L3 */
        ISA_MIPS_R8000              = 0x060e,       /**< superscalar, fairly rare */
        ISA_MIPS_R10000             = 0x060f,       /**< R8000 on a single chip; 32kB caches; out-of-order */
        ISA_MIPS_R12000             = 0x0610,       /**< R10000 + higher clock rates */
        ISA_MIPS_R14000             = 0x0611,       /**< R12000 + support for DDR SRAM; 200MHz front side bus */
        ISA_MIPS_R16000             = 0x0612,       /**< R14000 + increased freq, more L1, smaller die */
        ISA_MIPS_R16000A            = 0x0613,
        ISA_MIPS_16                 = 0x0614,       /**< Unknown. Windows PE architecture 0x266 "MIPS16" */
        ISA_MIPS_FPU                = 0x0615,       /**< Unknown. Windows PE architecture 0x366 "MIPS with FPU" */
        ISA_MIPS_16FPU              = 0x0616,       /**< Unknown. Windows PE architecture 0x466 "MIPS16 with FPU" */

        ISA_I860_Family             = 0x0700,       /**< Intel i860 family; 1989-mid 90's; RISC VLIW */
        ISA_I860_860XR              = 0x0701,       /**< (code named N10) 25-40MHz */
        ISA_I860_860XP              = 0x0702,       /**< (code named N11) larger caches; 40-50MHz; same IS as XR */

        ISA_IA64_Family             = 0x0800,       /**< Intel 64-bit architecture */
        ISA_IA64_Itanium            = 0x0801,       /**< First generation */
        ISA_IA64_Itanium2           = 0x0802,       /**< Second generation starting Nov 2007 */

        // See http://en.wikipedia.org/wiki/ARM_architecture
        ISA_ARM_Family              = 0x0900,       /**< Acorn RISC Machine, Advanced RISC Machines, ARM Limited */
        ISA_ARM_ARM1                = 0x0901,       /**< ARM evaluation system */
        ISA_ARM_ARM2                = 0x0902,       /**< ARM2, ARM250 cores */
        ISA_ARM_ARM3                = 0x0903,       /**< ARM2a core */
        ISA_ARM_ARM6                = 0x0904,       /**< ARM60, ARM600, ARM610 cores */
        ISA_ARM_ARM7                = 0x0905,       /**< ARM{700,710,710a,7100,7500,7500FE} cores */
        ISA_ARM_ARM7TDMI            = 0x0906,       /**< ARM{7TDMI,7TDMI-S,710T,720T,740T,7EJ-S} cores */
        ISA_ARM_StrongARM           = 0x0907,       /**< SA-110, SA-1110 cores */
        ISA_ARM_ARM8                = 0x0908,       /**< ARM810 core */
        ISA_ARM_ARM9TDMI            = 0x0909,       /**< ARM{9TDMI,920T,922T,940T} cores */
        ISA_ARM_ARM9E               = 0x090a,       /**< ARM{946E-S,966E-S,968E-S,926EJ-S,966HS} cores */
        ISA_ARM_ARM10E              = 0x090b,       /**< ARM{1020E,1022E,1026EJ-S} cores */
        ISA_ARM_XScale              = 0x090c,       /**< 80200, IOP310, IOP315, 80219, IOP321, IOP33x, IOP34x, PXA210,
                                                     *   PXA250, PXA255, PXA26x, PXA27x, PXA800(E)F, Monahans, PXA900,
                                                     *   IXC1100, IXP2400, IXP2800, IXP2850, IXP2325, IXP2350, IXP42x,
                                                     *   IXP460, IXP465 cores */
        ISA_ARM_ARM11               = 0x090d,       /**< ARMv{6,6T2,6KZ,6K} cores */
        ISA_ARM_Cortex              = 0x090e,       /**< Cortex-{A8,A9,A9 MPCore,R4(F),M3,M1} cores */
        ISA_ARM_A64                 = 0x090f,       /**< ARM AArch64 A64 instruction set. */

        ISA_JVM                     = 0x0a00,       /**< Java Virtual Machine (JVM) instruction set. */

        // Others, not yet incorporated into this enum
        ISA_OTHER_Family            = 0xf000,

        ISA_ATT_WE_32100            = 0xf001,       /**< Sometimes simply "M32" */
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
        ISA_Digital_Alpha         = 0xf03f,
        ISA_Matsushita_AM33         = 0xf040,
        ISA_EFI_ByteCode            = 0xf041
    };

    /** General purpose of a binary executable file. */
    enum ExecPurpose {
        PURPOSE_UNSPECIFIED,                        /**< Purpose is not specified and could not be inferred */
        PURPOSE_OTHER,                              /**< A purpose other than any defined below */
        PURPOSE_EXECUTABLE,                         /**< Executable program */
        PURPOSE_LIBRARY,                            /**< Library (shared or relocatable) */
        PURPOSE_CORE_DUMP,                          /**< Post mortem image */
        PURPOSE_OS_SPECIFIC,                        /**< Some operating system specific purpose */
        PURPOSE_PROC_SPECIFIC                       /**< Some processor specific purpose */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Factory method that parses a binary file. */
    static SgAsmGenericFile *parseBinaryFormat(const char *name);

    /** Dump debugging information into a named text file.
     *
     *  Writes a new file from the IR node for a parsed executable file. Warning: This function might modify the AST by calling @ref
     *  SgAsmGenericSection::reallocate, which makes sure all parts of the AST are consistent with respect to each other. */
    static void unparseBinaryFormat(const std::string &name, SgAsmGenericFile*);

    /** Dump debugging information to specified stream.
     *
     *  Unparses an executable file into the supplied output stream. Warning: This function might modify the AST by calling @ref
     *  SgAsmGenericSection::reallocate, which makes sure all parts of the AST are consistent with respect to each other. */
    static void unparseBinaryFormat(std::ostream&, SgAsmGenericFile*);

    /** Diagnostic stream. */
    static Sawyer::Message::Facility mlog;

    /** Initialize diagnostic streams.
     *
     *  This is called automatically by @ref Rose::initialize. */
    static void initDiagnostics();

    // These convert enums to strings. It is better to use the automatic enum stringification instead. They have names like
    // Rose::stringifySgAsmExecutableFileFormatInsnSetArchitecture, etc. */
    static std::string isaFamilyToString(SgAsmExecutableFileFormat::InsSetArchitecture);
    static std::string isaToString(SgAsmExecutableFileFormat::InsSetArchitecture);
    static std::string toString(SgAsmExecutableFileFormat::InsSetArchitecture);
    static std::string toString(SgAsmExecutableFileFormat::ExecFamily);
    static std::string toString(SgAsmExecutableFileFormat::ExecABI);
    static std::string toString(SgAsmExecutableFileFormat::ExecPurpose);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    static std::string isa_family_to_string(SgAsmExecutableFileFormat::InsSetArchitecture) ROSE_DEPRECATED("use isaFamilyToString");
    static std::string isa_to_string(SgAsmExecutableFileFormat::InsSetArchitecture) ROSE_DEPRECATED("use isaToString");
    static std::string to_string(SgAsmExecutableFileFormat::InsSetArchitecture) ROSE_DEPRECATED("use toString");
    static std::string to_string(SgAsmExecutableFileFormat::ExecFamily) ROSE_DEPRECATED("use toString");
    static std::string to_string(SgAsmExecutableFileFormat::ExecABI) ROSE_DEPRECATED("use toString");
    static std::string to_string(SgAsmExecutableFileFormat::ExecPurpose) ROSE_DEPRECATED("use toString");
};
