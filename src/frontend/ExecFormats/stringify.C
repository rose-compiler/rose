

// DQ (8/20/2008): These are functions to stringify details which may be helpful.
// so they are saved so that we can decide if we want to move them into the
// binary executable file format support.  This is an older version of the 
// code and would have to be updated to use the new IR nodes.


#if 0
string
processorArchitectureName (SgAsmFile::pe_processor_architecture_enum processor_architecture_kind)
   {
  // This function supports the Portable Execution binary format (PE) used in Windows binaries.
  // PE format is a variation of the COFF format used by IBM and others.

     string s;

     switch (processor_architecture_kind)
        {
          case SgAsmFile::e_processor_architecture_none:    s = "No machine"; break;
          case SgAsmFile::e_processor_architecture_INTEL:   s = "INTEL";      break;
          case SgAsmFile::e_processor_architecture_MIPS:    s = "MIPS";       break;
          case SgAsmFile::e_processor_architecture_ALPHA:   s = "ALPHA";      break;
          case SgAsmFile::e_processor_architecture_PPC:     s = "PPC";        break;
          case SgAsmFile::e_processor_architecture_SHX:     s = "SHX";        break;
          case SgAsmFile::e_processor_architecture_ARM:     s = "ARM";        break;
          case SgAsmFile::e_processor_architecture_IA64:    s = "IA64";       break;
          case SgAsmFile::e_processor_architecture_ALPHA64: s = "ALPHA64";    break;
          case SgAsmFile::e_processor_architecture_MSIL:    s = "MSIL";       break;
          case SgAsmFile::e_processor_architecture_AMD64:   s = "AMD64";      break;
          case SgAsmFile::e_processor_architecture_UNKNOWN: s = "UNKNOWN";    break;

          default:
             {
               s = "error";

               printf ("Error: default reach for processor_architecture_type = %d \n",processor_architecture_kind);
             }
        }

     return s;
   }
#endif

#if 0
string
processorTypeName (SgAsmFile::pe_processor_type_enum processor_type_kind)
   {
  // This function supports the Portable Execution binary format (PE) used in Windows binaries.
  // PE format is a variation of the COFF format used by IBM and others.

     string s;

     switch (processor_type_kind)
        {
          case SgAsmFile::e_processor_type_none:          s = "No machine";    break;
          case SgAsmFile::e_processor_type_INTEL_386:     s = "INTEL_386";     break;
          case SgAsmFile::e_processor_type_INTEL_486:     s = "INTEL_486";     break;
          case SgAsmFile::e_processor_type_INTEL_PENTIUM: s = "INTEL_PENTIUM"; break;
          case SgAsmFile::e_processor_type_INTEL_860:     s = "INTEL_860";     break;
          case SgAsmFile::e_processor_type_INTEL_IA64:    s = "INTEL_IA64";    break;
          case SgAsmFile::e_processor_type_AMD_X8664:     s = "AMD_X8664";     break;
          case SgAsmFile::e_processor_type_MIPS_R2000:    s = "MIPS_R2000";    break;
          case SgAsmFile::e_processor_type_MIPS_R3000:    s = "MIPS_R3000";    break;
          case SgAsmFile::e_processor_type_MIPS_R4000:    s = "MIPS_R4000";    break;
          case SgAsmFile::e_processor_type_ALPHA_21064:   s = "ALPHA_21064";   break;
          case SgAsmFile::e_processor_type_PPC_601:       s = "PPC_601";       break;
          case SgAsmFile::e_processor_type_PPC_603:       s = "PPC_603";       break;
          case SgAsmFile::e_processor_type_PPC_604:       s = "PPC_604";       break;
          case SgAsmFile::e_processor_type_PPC_620:       s = "PPC_620";       break;
          case SgAsmFile::e_processor_type_HITACHI_SH3:   s = "HITACHI_SH3";   break;
          case SgAsmFile::e_processor_type_HITACHI_SH3E:  s = "HITACHI_SH3E";  break;
          case SgAsmFile::e_processor_type_HITACHI_SH4:   s = "HITACHI_SH4";   break;
          case SgAsmFile::e_processor_type_MOTOROLA_821:  s = "MOTOROLA_821";  break;
          case SgAsmFile::e_processor_type_SHx_SH3:       s = "SHx_SH3";       break;
          case SgAsmFile::e_processor_type_SHx_SH4:       s = "SHx_SH4";       break;
          case SgAsmFile::e_processor_type_STRONGARM:     s = "STRONGARM";     break;
          case SgAsmFile::e_processor_type_ARM720:        s = "ARM720";        break;
          case SgAsmFile::e_processor_type_ARM820:        s = "ARM820";        break;
          case SgAsmFile::e_processor_type_ARM920:        s = "ARM920";        break;
          case SgAsmFile::e_processor_type_ARM_7TDMI:     s = "ARM_7TDMI";     break;

          default:
             {
               s = "error";

               printf ("Error: default reach for processor_type_type = %d \n",processor_type_kind);
             }
        }

     return s;
   }
#endif

#if 0
string
machineArchitectureName (SgAsmFile::elf_machine_architecture_enum machine_architecture_kind)
   {
     string s;

     switch (machine_architecture_kind)
        {
          case SgAsmFile::e_machine_architecture_none:                     s = "No machine";               break;
          case SgAsmFile::e_machine_architecture_ATT_WE_32100:             s = "AT&T WE 32100";            break;
          case SgAsmFile::e_machine_architecture_Sun_Sparc:                s = "SUN SPARC";                break;
          case SgAsmFile::e_machine_architecture_Intel_80386:              s = "Intel 80386";              break;
          case SgAsmFile::e_machine_architecture_Motorola_m68k_family:     s = "Motorola m68k family";     break;
          case SgAsmFile::e_machine_architecture_Motorola_m88k_family:     s = "Motorola m88k family";     break;
          case SgAsmFile::e_machine_architecture_Intel_80860:              s = "Intel 80860";              break;
          case SgAsmFile::e_machine_architecture_MIPS_R3000_big_endian:    s = "MIPS R3000 big-endian";    break;
          case SgAsmFile::e_machine_architecture_IBM_System_370:           s = "IBM System/370";           break;
          case SgAsmFile::e_machine_architecture_MIPS_R3000_little_endian: s = "MIPS R3000 little-endian"; break;
          case SgAsmFile::e_machine_architecture_HPPA:               s = "HPPA";                 break;
          case SgAsmFile::e_machine_architecture_Fujitsu_VPP500:     s = "Fujitsu VPP500";       break;
          case SgAsmFile::e_machine_architecture_Sun_v8plus:         s = "Sun's v8plus";         break;
          case SgAsmFile::e_machine_architecture_Intel_80960:        s = "Intel 80960";          break;
          case SgAsmFile::e_machine_architecture_PowerPC:            s = "PowerPC";              break;
          case SgAsmFile::e_machine_architecture_PowerPC_64bit:      s = "PowerPC 64-bit";       break;
          case SgAsmFile::e_machine_architecture_IBM_S390:           s = "IBM S390";             break;
          case SgAsmFile::e_machine_architecture_NEC_V800_series:    s = "NEC V800 series";      break;
          case SgAsmFile::e_machine_architecture_Fujitsu_FR20:       s = "Fujitsu FR20";         break;
          case SgAsmFile::e_machine_architecture_TRW_RH_32:          s = "TRW RH-32";            break;
          case SgAsmFile::e_machine_architecture_Motorola_RCE:       s = "Motorola RCE";         break;
          case SgAsmFile::e_machine_architecture_ARM:                s = "ARM";                  break;
          case SgAsmFile::e_machine_architecture_Digital_Alpha_fake: s = "Digital Alpha (fake)"; break;
          case SgAsmFile::e_machine_architecture_Hitachi_SH:         s = "Hitachi SH";           break;
          case SgAsmFile::e_machine_architecture_SPARC_v9_64bit:     s = "SPARC v9 64-bit";      break;
          case SgAsmFile::e_machine_architecture_Siemens_Tricore:    s = "Siemens Tricore";      break;
          case SgAsmFile::e_machine_architecture_Argonaut_RISC_Core: s = "Argonaut RISC Core";   break;
          case SgAsmFile::e_machine_architecture_Hitachi_H8_300:     s = "Hitachi H8/300";       break;
          case SgAsmFile::e_machine_architecture_Hitachi_H8_300H:    s = "Hitachi H8/300H";      break;
          case SgAsmFile::e_machine_architecture_Hitachi_H8S:        s = "Hitachi H8S";          break;
          case SgAsmFile::e_machine_architecture_Hitachi_H8_500:     s = "Hitachi H8/500";       break;
          case SgAsmFile::e_machine_architecture_Intel_Merced:       s = "Intel Merced";         break;
          case SgAsmFile::e_machine_architecture_Stanford_MIPS_X:    s = "Stanford MIPS-X";      break;
          case SgAsmFile::e_machine_architecture_Motorola_Coldfire:  s = "Motorola Coldfire";    break;
          case SgAsmFile::e_machine_architecture_Motorola_M68HC12:   s = "Motorola M68HC12";     break;
          case SgAsmFile::e_machine_architecture_Fujitsu_MMA_Multimedia_Accelerator:   s = "Fujitsu MMA Multimedia Accelerator"; break;
          case SgAsmFile::e_machine_architecture_Siemens_PCP:                          s = "Siemens PCP";                        break;
          case SgAsmFile::e_machine_architecture_Sony_nCPU_embeeded_RISC:              s = "Sony nCPU embeeded RISC";            break;
          case SgAsmFile::e_machine_architecture_Denso_NDR1_microprocessor:            s = "Denso NDR1 microprocessor";          break;
          case SgAsmFile::e_machine_architecture_Motorola_Start_Core_processor:        s = "Motorola Start*Core processor";      break;
          case SgAsmFile::e_machine_architecture_Toyota_ME16_processor:                s = "Toyota ME16 processor";              break;
          case SgAsmFile::e_machine_architecture_STMicroelectronic_ST100_processor:    s = "STMicroelectronic ST100 processor";  break;
          case SgAsmFile::e_machine_architecture_Advanced_Logic_Corp_Tinyj_emb_family: s = "Advanced Logic Corp. Tinyj emb.fam"; break;
          case SgAsmFile::e_machine_architecture_AMD_x86_64_architecture:              s = "AMD x86-64 architecture";            break;
          case SgAsmFile::e_machine_architecture_Sony_DSP_Processor:                   s = "Sony DSP Processor";                 break;
          case SgAsmFile::e_machine_architecture_Siemens_FX66_microcontroller:         s = "Siemens FX66 microcontroller";       break;
          case SgAsmFile::e_machine_architecture_STMicroelectronics_ST9_plus_8_16_microcontroller: s = "STMicroelectronics ST9+ 8/16 mc";   break;
          case SgAsmFile::e_machine_architecture_STMicroelectronics_ST7_8bit_microcontroller:      s = "STmicroelectronics ST7 8 bit mc";   break;
          case SgAsmFile::e_machine_architecture_Motorola_MC68HC16_microcontroller:              s = "Motorola MC68HC16 microcontroller"; break;
          case SgAsmFile::e_machine_architecture_Motorola_MC68HC11_microcontroller:              s = "Motorola MC68HC11 microcontroller"; break;
          case SgAsmFile::e_machine_architecture_Motorola_MC68HC08_microcontroller:              s = "Motorola MC68HC08 microcontroller"; break;
          case SgAsmFile::e_machine_architecture_Motorola_MC68HC05_microcontroller:              s = "Motorola MC68HC05 microcontroller"; break;
          case SgAsmFile::e_machine_architecture_Silicon_Graphics_SVx:                           s = "Silicon Graphics SVx";              break;
          case SgAsmFile::e_machine_architecture_STMicroelectronics_ST19_8bit_microcontroller:   s = "STMicroelectronics ST19 8 bit mc";  break;
          case SgAsmFile::e_machine_architecture_Digital_VAX:                                    s = "Digital VAX";                       break;
          case SgAsmFile::e_machine_architecture_Axis_Communications_32bit_embedded_processor:   s = "Axis Communications 32-bit embedded processor";   break;
          case SgAsmFile::e_machine_architecture_Infineon_Technologies_32bit_embedded_processor: s = "Infineon Technologies 32-bit embedded processor"; break;
          case SgAsmFile::e_machine_architecture_Element_14_64bit_DSP_Processor:                 s = "Element 14 64-bit DSP Processor";                 break;
          case SgAsmFile::e_machine_architecture_LSI_Logic_16bit_DSP_Processor:                  s = "LSI Logic 16-bit DSP Processor";                  break;
          case SgAsmFile::e_machine_architecture_Donald_Knuths_educational_64bit_processor:      s = "Donald Knuth's educational 64-bit processor";     break;
          case SgAsmFile::e_machine_architecture_Harvard_University_machine_independent_object_files: s = "Harvard University machine-independent object files"; break;
          case SgAsmFile::e_machine_architecture_SiTera_Prism:                      s = "SiTera Prism";                       break;
          case SgAsmFile::e_machine_architecture_Atmel_AVR_8bit_microcontroller:    s = "Atmel AVR 8-bit microcontroller";    break;
          case SgAsmFile::e_machine_architecture_Fujitsu_FR30:                      s = "Fujitsu FR30";                       break;
          case SgAsmFile::e_machine_architecture_Mitsubishi_D10V:                   s = "Mitsubishi D10V";                    break;
          case SgAsmFile::e_machine_architecture_Mitsubishi_D30V:                   s = "Mitsubishi D30V";                    break;
          case SgAsmFile::e_machine_architecture_NEC_v850:                          s = "NEC v850";                           break;
          case SgAsmFile::e_machine_architecture_Mitsubishi_M32R:                   s = "Mitsubishi M32R";                    break;
          case SgAsmFile::e_machine_architecture_Matsushita_MN10300:                s = "Matsushita MN10300";                 break;
          case SgAsmFile::e_machine_architecture_Matsushita_MN10200:                s = "Matsushita MN10200";                 break;
          case SgAsmFile::e_machine_architecture_picoJava:                          s = "picoJava";                           break;
          case SgAsmFile::e_machine_architecture_OpenRISC_32bit_embedded_processor: s = "OpenRISC 32-bit embedded processor"; break;
          case SgAsmFile::e_machine_architecture_ARC_Cores_Tangent_A5:              s = "ARC Cores Tangent-A5";               break;
          case SgAsmFile::e_machine_architecture_Tensilica_Xtensa_Architecture:     s = "Tensilica Xtensa Architecture";      break;
          case SgAsmFile::e_machine_architecture_Digital_Alpha:                     s = "Digital Alpha";                      break;

          default:
             {
               s = "error";

               printf ("Error: default reach for machine_architecture_type = %d \n",machine_architecture_kind);
             }
        }

     return s;
   }
#endif
