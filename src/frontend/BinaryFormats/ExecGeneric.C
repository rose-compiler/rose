/* Copyright 2008 Lawrence Livermore National Security, LLC */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "checkIsModifiedFlag.h"
#include <algorithm>
#include <fstream>

#ifndef _MSC_VER
// DQ (11/27/2009): This header file is not available using MSVS (Windows).
#include <sys/wait.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** Writes a new file from the IR node for a parsed executable file. Warning: This function might modify the AST by calling
 *  reallocate(), which makes sure all parts of the AST are consistent with respect to each other. */
void
SgAsmExecutableFileFormat::unparseBinaryFormat(const std::string &name, SgAsmGenericFile *ef)
{
    std::ofstream f(name.c_str(), std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
    ROSE_ASSERT(f.is_open());
    f.exceptions(std::ios::badbit | std::ios::failbit);
    unparseBinaryFormat(f, ef);
    f.close();
}

/** Unparses an executable file into the supplied output stream. Warning: This function might modify the AST by calling
 *  reallocate(), which makes sure all parts of the AST are consistent with respect to each other. */
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

    try {
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
                SgAsmGenericHeader *big_hdr = NULL;
                if (SgAsmPEFileHeader::is_PE(ef)) {
                    SgAsmDOSExtendedHeader *dos2_hdr = new SgAsmDOSExtendedHeader(dos_hdr);
                    dos2_hdr->parse();
                    SgAsmPEFileHeader *pe_hdr = new SgAsmPEFileHeader(ef);
                    pe_hdr->set_offset(dos2_hdr->get_e_lfanew());
                    pe_hdr->parse();
                    big_hdr = pe_hdr;
                } else if (SgAsmNEFileHeader::is_NE(ef)) {
                    SgAsmNEFileHeader::parse(dos_hdr);
                } else if (SgAsmLEFileHeader::is_LE(ef)) { /*or LX*/
                    SgAsmLEFileHeader::parse(dos_hdr);
                }

#if 0 /*This iterferes with disassembling the DOS interpretation*/
                /* Now go back and add the DOS Real-Mode section but rather than using the size specified in the DOS header,
                 * constrain it to not extend beyond the beginning of the PE, NE, LE, or LX file header. This makes detecting
                 * holes in the PE format much easier. */
                dos_hdr->parse_rm_section(big_hdr ? big_hdr->get_offset() : 0);
#else
                dos_hdr->parse_rm_section(0);
#endif
            } else {
                if (ef) delete ef->get_data_converter();
                delete ef;
                ef = NULL;
            }
        }
    } catch(...) {
        for (size_t ci=0; ci<converters.size(); ci++)
            delete converters[ci];
        if (ef) delete ef->get_data_converter();
        delete ef;
        ef = NULL;
        throw;
    }
    
    /* If no executable file could be parsed then try to use system tools to get the file type. On Unix-based systems, the
     * "file" command does a thorough job of trying to figure out the type of file. If we can't figure out the file name then
     * just throw some generic error. */
    if (!ef) {
        delete ef; ef=NULL;
        /* Use file(1) to try to figure out the file type to report in the exception */
        int child_stdout[2];
#ifdef _MSC_VER
#pragma message ("WARNING: Commented out use of functions from sys/wait.h")
                printf ("ERROR: Commented out use of functions from sys/wait.h \n");
                ROSE_ASSERT(false);
#else
                pipe(child_stdout);
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
            memset(buf, 0, sizeof buf);
            read(child_stdout[0], buf, sizeof buf);
            buf[sizeof(buf)-1] = '\0';
            if (char *nl = strchr(buf, '\n')) *nl = '\0'; /*keep only first line w/o LF*/
            waitpid(pid, NULL, 0);
            char mesg[64+sizeof buf];
            sprintf(mesg, "unrecognized file format: %s", buf);
            throw FormatError(mesg);
        } else {
            throw FormatError("unrecognized file format");
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

    return ef;
}
std::string SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::InsSetArchitecture isa)
{
  return isa_to_string(isa);
}

std::string SgAsmExecutableFileFormat::isa_family_to_string(SgAsmExecutableFileFormat::InsSetArchitecture isa)
{
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
  char buf[64];
  snprintf(buf,sizeof(buf),"unknown isa family (%zu)",size_t(isa & ISA_FAMILY_MASK)) ;
  return buf;
}


std::string SgAsmExecutableFileFormat::isa_to_string(SgAsmExecutableFileFormat::InsSetArchitecture isa)
{

  switch(isa){
    case ISA_UNSPECIFIED:         return "ISA_UNSPECIFIED"; /* File does not specify an architecture */
    case ISA_OTHER:               return "ISA_OTHER";       /* Architecture is something other than below */
    case ISA_FAMILY_MASK:         return "ISA_FAMILY_MASK";
    case ISA_IA32_286:            return "IA32_286";        /* 80286 */
    case ISA_IA32_386:            return "IA32_386";        /* MMU with paging */
    case ISA_IA32_486:            return "IA32_486";        /* risc-like pipelining, integrated FPU, on-chip cache */
    case ISA_IA32_Pentium:        return "IA32_Pentium";    /* superscalar, 64-bit databus, MMX */
    case ISA_IA32_Cyrix6x86:      return "IA32_Cyrix6x86";  /* register renaming, speculative execution */
    case ISA_IA32_AMDK5:          return "IA32_AMDK5";      /* micro-op translation */
    case ISA_IA32_PentiumPro:     return "IA32_PentiumPro"; /* PAE, integrated L2 cache */
    case ISA_IA32_PentiumII:      return "IA32_PentiumII";  /* L3-cache, 3D Now, SSE */
    case ISA_IA32_Athlon:         return "IA32_Athlon";     /* superscalar FPU, wide design */
    case ISA_IA32_Pentium4:       return "IA32_Pentium4";   /* deeply pipelined, high frequency, SSE2, hyper-threading */
    case ISA_IA32_PentiumM:       return "IA32_PentiumM";   /* low power */

    case ISA_X8664_Athlon64:      return "X8664_Athlon64";  /* on-die memory controller, 40-bit phys address space */
    case ISA_X8664_Prescott:      return "X8664_Prescott";  /* deeply pipelined, high frequency, SSE3 */
    case ISA_X8664_IntelCore:     return "X8664_IntelCore"; /* low power, multi-core, lower clock frequency */
    case ISA_X8664_AMDPhenom:     return "X8664_AMDPhenom"; /* quad core, 128-bit FPUs, SSE4a, native mem ctrl, on-die L3 cache */

    case ISA_SPARC_V7:            return "SPARC_V7"; 
    case ISA_SPARC_V8:            return "SPARC_V8"; 
    case ISA_SPARC_V8E:           return "SPARC_V8E"; 
    case ISA_SPARC_V9:            return "SPARC_V9"; 
    case ISA_SPARC_V9JPS1:        return "SPARC_V9JPS1"; 
    case ISA_SPARC_V9UA:          return "SPARC_V9UA"; 
    case ISA_SPARC_V9JPS2:        return "SPARC_V9JPS2"; 

    case ISA_M68K_68000:          return "M68K_68000";      /* generation one: 16/32 internal; 8-, 16-, 32-bit interface */
    case ISA_M68K_68EC000:        return "M68K_68EC000"; 
    case ISA_M68K_68HC000:        return "M68K_68HC000"; 
    case ISA_M68K_68008:          return "M68K_68008"; 
    case ISA_M68K_68010:          return "M68K_68010"; 
    case ISA_M68K_68012:          return "M68K_68012"; 
    case ISA_M68K_68020:          return "M68K_68020";      /* generation two: fully 32-bit */
    case ISA_M68K_68EC020:        return "M68K_68EC020"; 
    case ISA_M68K_68030:          return "M68K_68030"; 
    case ISA_M68K_68EC030:        return "M68K_68EC030"; 
    case ISA_M68K_68040:          return "M68K_68040";      /* generation three: pipelined */
    case ISA_M68K_68EC040:        return "M68K_68EC040"; 
    case ISA_M68K_68LC040:        return "M68K_68LC040"; 
    case ISA_M68K_68060:          return "M68K_68060";      /* generation four: superscalar */
    case ISA_M68K_ColdFire:       return "M68K_ColdFire";   /* other */
    case ISA_M68K_DragonBall:     return "M68K_DragonBall"; /* other */

    case ISA_M88K_88100:          return "M88K_88100";      /* 32-bit, integrated FPU mated with 88200 MMU and cache controller */
    case ISA_M88K_88110:          return "M88K_88110";      /* single package of 88100+88200 */
    case ISA_M88K_88110MP:        return "M88K_88110MP";    /* on-chip comm for use in multi-processor systems */
    case ISA_M88K_88120:          return "M88K_88120";      /* superscalar (never actually released) */


    case ISA_MIPS_MarkI:          return "MIPS_MarkI";      /* R2000, R3000 */
    case ISA_MIPS_MarkII:         return "MIPS_MarkII";     /* R6000 */
    case ISA_MIPS_MarkIII:        return "MIPS_MarkIII";    /* R4000 */
    case ISA_MIPS_R2000:          return "MIPS_R2000";      /* 32-bit, Big or little endian */
    case ISA_MIPS_R3000:          return "MIPS_R3000";      /* virtual identical: Pacempi's R3400, IDT's R3500, Toshiba R3900 */
    case ISA_MIPS_R4000:          return "MIPS_R4000";      /* 64-bit; others in the series had larger caches and bug fixes */
    case ISA_MIPS_R4200:          return "MIPS_R4200";      /* low-cost version of R4000 */
    case ISA_MIPS_R4300:          return "MIPS_R4300";      /* low-cost version of R4000 with 32-bit external bus */
    case ISA_MIPS_R4600:          return "MIPS_R4600";      /* "Orion" by Qauntum Effect Devices (QED); larger caches */
    case ISA_MIPS_R4650:          return "MIPS_R4650";      /* by QED */
    case ISA_MIPS_R4700:          return "MIPS_R4700";      /* "Orion" by QED */
    case ISA_MIPS_R5000:          return "MIPS_R5000";      /* by QED */
    case ISA_MIPS_RM7000:         return "MIPS_RM7000";     /* by PMC-Sierra; 256kB L2 and optional L3 */
    case ISA_MIPS_R8000:          return "MIPS_R8000";      /* superscalar, fairly rare */
    case ISA_MIPS_R10000:         return "MIPS_R10000";     /* R8000 on a single chip; 32kB caches; out-of-order */
    case ISA_MIPS_R12000:         return "MIPS_R12000";     /* R10000 + higher clock rates */
    case ISA_MIPS_R14000:         return "MIPS_R14000";     /* R12000 + support for DDR SRAM; 200MHz front side bus */
    case ISA_MIPS_R16000:         return "MIPS_R16000";     /* R14000 + increased freq, more L1, smaller die */
    case ISA_MIPS_R16000A:        return "MIPS_R16000A"; 
    case ISA_MIPS_16:             return "MIPS_16";         /* Unknown. Windows PE architecture 0x266 "MIPS16" */
    case ISA_MIPS_FPU:            return "MIPS_FPU";        /* Unknown. Windows PE architecture 0x366 "MIPS with FPU" */
    case ISA_MIPS_16FPU:          return "MIPS_16FPU";      /* Unknown. Windows PE architecture 0x466 "MIPS16 with FPU" */

    case ISA_I860_860XR:          return "I860_860XR";      /* (code named N10) 25-40MHz */
    case ISA_I860_860XP:          return "I860_860XP";      /* (code named N11) larger caches; 40-50MHz; same IS as XR */

    case ISA_IA64_Itanium:        return "IA64_Itanium";    /* First generation */
    case ISA_IA64_Itanium2:       return "IA64_Itanium2";   /* Second generation starting Nov 2007 */

      /* See http://en.wikipedia.org/wiki/ARM_architecture */
    case ISA_ARM_ARM1:            return "ARM1";            /* ARM evaluation system */
    case ISA_ARM_ARM2:            return "ARM2";            /* ARM2, ARM250 cores */
    case ISA_ARM_ARM3:            return "ARM3";            /* ARM2a core */
    case ISA_ARM_ARM6:            return "ARM6";            /* ARM60, ARM600, ARM610 cores */
    case ISA_ARM_ARM7:            return "ARM7";            /* ARM{700,710,710a,7100,7500,7500FE} cores */
    case ISA_ARM_ARM7TDMI:        return "ARM7TDMI";        /* ARM{7TDMI,7TDMI-S,710T,720T,740T,7EJ-S} cores */
    case ISA_ARM_StrongARM:       return "StrongARM";       /* SA-110, SA-1110 cores */
    case ISA_ARM_ARM8:            return "ARM8";            /* ARM810 core */
    case ISA_ARM_ARM9TDMI:        return "ARM9TDMI";        /* ARM{9TDMI,920T,922T,940T} cores */
    case ISA_ARM_ARM9E:           return "ARM9E";           /* ARM{946E-S,966E-S,968E-S,926EJ-S,966HS} cores */
    case ISA_ARM_ARM10E:          return "ARM10E";          /* ARM{1020E,1022E,1026EJ-S} cores */
    case ISA_ARM_XScale:          return "ARM_XScale";      /* 80200, IOP310, IOP315, 80219, IOP321, IOP33x, IOP34x, PXA210,
                                                             * PXA250, PXA255, PXA26x, PXA27x, PXA800(E)F, Monahans, PXA900,
							     * IXC1100, IXP2400, IXP2800, IXP2850, IXP2325, IXP2350, IXP42x,
							     * IXP460, IXP465 cores */
    case ISA_ARM_ARM11:           return "ARM11";            /* ARMv{6,6T2,6KZ,6K} cores */
    case ISA_ARM_Cortex:          return "ARM_Cortex";       /* Cortex-{A8,A9,A9 MPCore,R4(F),M3,M1} cores */
          
      /* Others, not yet incorporated into this enum */
    case ISA_ATT_WE_32100:        return "ATT_WE_32100";     /* sometimes simply "M32" */
    case ISA_IBM_System_370:      return "IBM_System_370"; 
    case ISA_HPPA:                return "HPPA"; 
    case ISA_Fujitsu_VPP500:      return "Fujitsu_VPP500"; 
    case ISA_Sun_v8plus:          return "Sun_v8plus"; 
    case ISA_PowerPC:             return "PowerPC"; 
    case ISA_PowerPC_64bit:       return "PowerPC_64bit"; 
    case ISA_IBM_S390:            return "IBM_S390"; 
    case ISA_NEC_V800_series:     return "NEC_V800_series"; 
    case ISA_Fujitsu_FR20:        return "Fujitsu_FR20"; 
    case ISA_TRW_RH_32:           return "TRW_RH_32"; 
    case ISA_Motorola_RCE:        return "Motorola_RCE"; 
    case ISA_Digital_Alpha_fake:  return "Digital_Alpha_fake"; 
    case ISA_Hitachi_SH:          return "Hitachi_SH"; 
    case ISA_Siemens_Tricore:     return "Siemens_Tricore"; 
    case ISA_Argonaut_RISC_Core:  return "Argonaut_RISC_Core"; 
    case ISA_Hitachi_H8_300:      return "Hitachi_H8_300"; 
    case ISA_Hitachi_H8_300H:     return "Hitachi_H8_300H"; 
    case ISA_Hitachi_H8S:         return "Hitachi_H8S"; 
    case ISA_Hitachi_H8_500:      return "Hitachi_H8_500"; 
    case ISA_Stanford_MIPS_X:     return "Stanford_MIPS_X"; 
    case ISA_Motorola_M68HC12:    return "Motorola_M68HC12"; 
    case ISA_Fujitsu_MMA_Multimedia_Accelerator: return "Fujitsu_MMA_Multimedia_Accelerator"; 
    case ISA_Siemens_PCP:         return "Siemens_PCP"; 
    case ISA_Sony_nCPU_embeeded_RISC:   return "Sony_nCPU_embeeded_RISC"; 
    case ISA_Denso_NDR1_microprocessor: return "Denso_NDR1_microprocessor"; 
    case ISA_Motorola_Start_Core_processor: return "Motorola_Start_Core_processor"; 
    case ISA_Toyota_ME16_processor:     return "Toyota_ME16_processor"; 
    case ISA_STMicroelectronic_ST100_processor: return "STMicroelectronic_ST100_processor"; 
    case ISA_Advanced_Logic_Corp_Tinyj_emb_family: return "Advanced_Logic_Corp_Tinyj_emb_family"; 
    case ISA_AMD_x86_64_architecture:   return "AMD_x86_64_architecture"; 
    case ISA_Sony_DSP_Processor:        return "Sony_DSP_Processor"; 
    case ISA_Siemens_FX66_microcontroller: return "Siemens_FX66_microcontroller"; 
    case ISA_STMicroelectronics_ST9_plus_8_16_microcontroller: return "STMicroelectronics_ST9_plus_8_16_microcontroller"; 
    case ISA_STMicroelectronics_ST7_8bit_microcontroller: return "STMicroelectronics_ST7_8bit_microcontroller"; 
    case ISA_Motorola_MC68HC16_microcontroller: return "Motorola_MC68HC16_microcontroller"; 
    case ISA_Motorola_MC68HC11_microcontroller: return "Motorola_MC68HC11_microcontroller"; 
    case ISA_Motorola_MC68HC08_microcontroller: return "Motorola_MC68HC08_microcontroller"; 
    case ISA_Motorola_MC68HC05_microcontroller: return "Motorola_MC68HC05_microcontroller"; 
    case ISA_Silicon_Graphics_SVx:      return "Silicon_Graphics_SVx"; 
    case ISA_STMicroelectronics_ST19_8bit_microcontroller: return "STMicroelectronics_ST19_8bit_microcontroller"; 
    case ISA_Digital_VAX:               return "Digital_VAX"; 
    case ISA_Axis_Communications_32bit_embedded_processor: return "Axis_Communications_32bit_embedded_processor"; 
    case ISA_Infineon_Technologies_32bit_embedded_processor: return "Infineon_Technologies_32bit_embedded_processor"; 
    case ISA_Element_14_64bit_DSP_Processor: return "Element_14_64bit_DSP_Processor"; 
    case ISA_LSI_Logic_16bit_DSP_Processor: return "LSI_Logic_16bit_DSP_Processor"; 
    case ISA_Donald_Knuths_educational_64bit_processor: return "Donald_Knuths_educational_64bit_processor"; 
    case ISA_Harvard_University_machine_independent_object_files: return "Harvard_University_machine_independent_object_files"; 
    case ISA_SiTera_Prism:             return "SiTera_Prism"; 
    case ISA_Atmel_AVR_8bit_microcontroller: return "Atmel_AVR_8bit_microcontroller"; 
    case ISA_Fujitsu_FR30:             return "Fujitsu_FR30"; 
    case ISA_Mitsubishi_D10V:          return "Mitsubishi_D10V"; 
    case ISA_Mitsubishi_D30V:          return "Mitsubishi_D30V"; 
    case ISA_NEC_v850:                 return "NEC_v850"; 
    case ISA_Mitsubishi_M32R:          return "Mitsubishi_M32R"; 
    case ISA_Matsushita_MN10300:       return "Matsushita_MN10300"; 
    case ISA_Matsushita_MN10200:       return "Matsushita_MN10200"; 
    case ISA_picoJava:                 return "picoJava"; 
    case ISA_OpenRISC_32bit_embedded_processor: return "OpenRISC_32bit_embedded_processor"; 
    case ISA_ARC_Cores_Tangent_A5:     return "ARC_Cores_Tangent_A5"; 
    case ISA_Tensilica_Xtensa_Architecture: return "Tensilica_Xtensa_Architecture"; 
    case ISA_Digital_Alpha:            return "Digital_Alpha"; 
    case ISA_Matsushita_AM33:          return "Matsushita_AM33"; 
    case ISA_EFI_ByteCode:             return "EFI_ByteCode"; 

    case ISA_IA32_Family:
    case ISA_X8664_Family: 
    case ISA_SPARC_Family: 
    case ISA_M68K_Family:  
    case ISA_M88K_Family:  
    case ISA_MIPS_Family:  
    case ISA_I860_Family:  
    case ISA_IA64_Family:  
    case ISA_ARM_Family:   
    case ISA_OTHER_Family: 
      return isa_family_to_string(isa);
  };
  char buf[64];
  snprintf(buf,sizeof(buf),"unknown isa (%zu)",size_t(isa)) ;
  return buf;
}

std::string SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::ByteOrder sex)
{
  switch (sex) {
    case ORDER_UNSPECIFIED:     return "unspecified";   
    case ORDER_LSB:             return "little-endian"; 
    case ORDER_MSB:             return "big-endian";    
  };
  char buf[64];
  snprintf(buf,sizeof(buf),"unknown byte order (%zu)",size_t(sex)) ;
  return buf;
}

std::string SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::ExecFamily family)
{
  switch(family){
    case FAMILY_UNSPECIFIED:  return "unspecified";                          
    case FAMILY_DOS:          return "Microsoft DOS";                        
    case FAMILY_ELF:          return "Executable and Linking Format (ELF)";  
    case FAMILY_LE:           return "Microsoft Linear Executable (LE)";     
    case FAMILY_LX:           return "OS/2 Extended Linear Executable (LX)"; 
    case FAMILY_NE:           return "Microsoft New Executable (NE)";        
    case FAMILY_PE:           return "Microsoft Portable Executable (PE)";   
  };
  char buf[128];
  snprintf(buf,sizeof(buf),"unknown exec family (%zu)",size_t(family)) ;
  return buf;
}

std::string SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::ExecABI abi)
{
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
  char buf[64];
  snprintf(buf,sizeof(buf),"unknown abi (%zu)",size_t(abi)) ;
  return buf;
}

std::string SgAsmExecutableFileFormat::to_string(SgAsmExecutableFileFormat::ExecPurpose purpose)
{
  switch(purpose){
    case PURPOSE_UNSPECIFIED:   return "unspecified";                       
    case PURPOSE_OTHER:         return "other";                             
    case PURPOSE_EXECUTABLE:    return "executable program";                
    case PURPOSE_LIBRARY:       return "library (shared or relocatable)";   
    case PURPOSE_CORE_DUMP:     return "post mortem image (core dump)";     
    case PURPOSE_OS_SPECIFIC:   return "operating system specific purpose"; 
    case PURPOSE_PROC_SPECIFIC: return "processor specific purpose";        
  };
  char buf[64];
  snprintf(buf,sizeof(buf),"unknown exec purpose (%zu)",size_t(purpose)) ;
  return buf;
}
