/* Windows PE file header (SgAsmPEFileHeader and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Rose/Diagnostics.h>
#include <ROSE_NELMTS.h>

#include <boost/format.hpp>

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

/* The __attribute__ mechanism is only supported by GNU compilers */
#ifndef __GNUC__
#define  __attribute__(x)  /*NOTHING*/
#define  __attribute(x)    /*NOTHING*/
#endif

std::string
SgAsmPEFileHeader::rvasize_pair_name(PairPurpose idx, const char **short_name)
{
    return rvaSizePairName(idx, short_name);
}

std::string
SgAsmPEFileHeader::rvaSizePairName(PairPurpose idx, const char **short_name)
{
    const char *full="", *abbr="";
    switch (idx) {
        case PAIR_EXPORTS:              full="Export Table";            abbr="Exports";   break;
        case PAIR_IMPORTS:              full="Import Table";            abbr="Imports";   break;
        case PAIR_RESOURCES:            full="Resource Table";          abbr="Rsrc";      break;
        case PAIR_EXCEPTIONS:           full="Exception Table";         abbr="Excpns";    break;
        case PAIR_CERTIFICATES:         full="Certificate Table";       abbr="Certs";     break;
        case PAIR_BASERELOCS:           full="Base Relocation Table";   abbr="BaseReloc"; break;
        case PAIR_DEBUG:                full="Debug";                   abbr="Debug";     break;
        case PAIR_ARCHITECTURE:         full="Architecture";            abbr="Arch";      break;
        case PAIR_GLOBALPTR:            full="Global Ptr";              abbr="GlobPtr";   break;
        case PAIR_TLS:                  full="TLS Table";               abbr="TLS";       break;
        case PAIR_LOADCONFIG:           full="Load Config Table";       abbr="LCT";       break;
        case PAIR_BOUNDIMPORT:          full="Bound Import Table";      abbr="BIT";       break;
        case PAIR_IAT:                  full="Import Address Table";    abbr="IAT";       break;
        case PAIR_DELAYIMPORT:          full="Delay Import Descriptor"; abbr="DID";       break;
        case PAIR_CLRRUNTIME:           full="CLR Runtime Header";      abbr="CLRHdr";    break;
        case PAIR_RESERVED15:           full="Reserved Pair 15";        abbr="Pair15";    break;
        // default:  NOT PRESENT (it would prevent compiler warnings for newly added enum members)
    }

    if (short_name)
        *short_name = abbr;
    return full;
}

/* Construct a new PE File Header with default values. */
SgAsmPEFileHeader::SgAsmPEFileHeader(SgAsmGenericFile *f)
    : SgAsmGenericHeader(f) {
    initializeProperties();

    ROSE_ASSERT(get_file()!=NULL);
    ROSE_ASSERT(get_size()>0);

    set_name(new SgAsmBasicString("PE File Header"));
    set_synthesized(true);
    set_purpose(SP_HEADER);

    p_rvaSizePairs = new SgAsmPERVASizePairList(this);

    /* Magic number */
    p_magic.clear();
    p_magic.push_back('P');
    p_magic.push_back('E');
    p_magic.push_back('\0');
    p_magic.push_back('\0');

    /* Executable Format */
    ROSE_ASSERT(get_executableFormat()!=NULL);
    get_executableFormat()->set_family(FAMILY_PE);
    get_executableFormat()->set_purpose(PURPOSE_EXECUTABLE);
    get_executableFormat()->set_sex(ByteOrder::ORDER_LSB);
    get_executableFormat()->set_wordSize(4);
    get_executableFormat()->set_version(0);
    get_executableFormat()->set_isCurrentVersion(true);
    get_executableFormat()->set_abi(ABI_NT);
    get_executableFormat()->set_abiVersion(0);

    /* Default instruction architecture */
    p_e_cpu_type = 0x014c; /*i386*/
    p_isa = ISA_IA32_386;

    p_e_time = time(NULL);
    p_e_nt_hdr_size = sizeof(PE32OptHeader_disk);
}

bool
SgAsmPEFileHeader::is_PE(SgAsmGenericFile *file)
{
    return isPe(file);
}

bool
SgAsmPEFileHeader::isPe(SgAsmGenericFile *file)
{
    /* Turn off byte reference tracking for the duration of this function. We don't want our testing the file contents to
     * affect the list of bytes that we've already referenced or which we might reference later. */
    bool was_tracking = file->get_trackingReferences();
    file->set_trackingReferences(false);

    try {
        /* Check DOS File Header magic number at beginning of the file */
        unsigned char dos_magic[2];
        file->readContent(0, dos_magic, sizeof dos_magic);
        if ('M'!=dos_magic[0] || 'Z'!=dos_magic[1])
            throw 1;

        /* Read four-byte offset of potential PE File Header at offset 0x3c */
        uint32_t lfanew_disk;
        file->readContent(0x3c, &lfanew_disk, sizeof lfanew_disk);
        Address pe_offset = ByteOrder::leToHost(lfanew_disk);
        
        /* Look for the PE File Header magic number */
        unsigned char pe_magic[4];
        file->readContent(pe_offset, pe_magic, sizeof pe_magic);
        if ('P'!=pe_magic[0] || 'E'!=pe_magic[1] || '\0'!=pe_magic[2] || '\0'!=pe_magic[3])
            throw 1;
    } catch (...) {
        file->set_trackingReferences(was_tracking);
        return false;
    }
    
    file->set_trackingReferences(was_tracking);
    return true;
}

/* Initialize the header with information parsed from the file and construct and parse everything that's reachable from the
 * header. The PE File Header should have been constructed such that SgAsmPEFileHeader::ctor() was called. */
SgAsmPEFileHeader*
SgAsmPEFileHeader::parse()
{
    SgAsmGenericHeader::parse();
    
    /* Read header, zero padding if the file isn't large enough */
    PEFileHeader_disk fh;
    if (sizeof(fh)>get_size())
        extend(sizeof(fh)-get_size());
    if (sizeof(fh)!=readContentLocal(0, &fh, sizeof fh, false))
        mlog[WARN] <<"SgAsmPEFileHeader::parse: short read of PE header at byte "
                   <<StringUtility::addrToString(get_offset()) <<"\n";

    /* Check magic number before getting too far */
    if (fh.e_magic[0]!='P' || fh.e_magic[1]!='E' || fh.e_magic[2]!='\0' || fh.e_magic[3]!='\0')
        throw FormatError("Bad PE magic number");

    /* Decode COFF file header */
    p_e_cpu_type           = ByteOrder::leToHost(fh.e_cpu_type);
    p_e_nsections          = ByteOrder::leToHost(fh.e_nsections);
    p_e_time               = ByteOrder::leToHost(fh.e_time);
    p_e_coff_symtab        = ByteOrder::leToHost(fh.e_coff_symtab);
    p_e_coff_nsyms         = ByteOrder::leToHost(fh.e_coff_nsyms);
    p_e_nt_hdr_size        = ByteOrder::leToHost(fh.e_nt_hdr_size);
    p_e_flags              = ByteOrder::leToHost(fh.e_flags);

    /* Read the "Optional Header" (optional in the sense that not all files have one, but required for an executable), the
     * size of which is stored in the e_nt_hdr_size of the main PE file header. According to 
     * http://www.phreedom.org/solar/code/tinype the Windows loader honors the e_nt_hdr_size even when set to smaller than the
     * smallest possible documented size of the optional header. Also it's possible for the optional header to extend beyond
     * the end of the file, in which case that part should be read as zero. */
    PE32OptHeader_disk oh32;
    Address need32 = sizeof(PEFileHeader_disk) + std::min(p_e_nt_hdr_size, (Address)(sizeof oh32));
    if (need32>get_size())
        extend(need32-get_size());
    if (sizeof(oh32)!=readContentLocal(sizeof fh, &oh32, sizeof oh32, false))
        mlog[WARN] <<"SgAsmPEFileHeader::parse: short read of PE Optional Header at byte "
                   <<StringUtility::addrToString(get_offset() + sizeof(fh)) <<"\n";
    p_e_opt_magic = ByteOrder::leToHost(oh32.e_opt_magic);
    
    /* File format changes from ctor() */
    get_executableFormat()->set_purpose(p_e_flags & HF_PROGRAM ? PURPOSE_EXECUTABLE : PURPOSE_LIBRARY);
    get_executableFormat()->set_wordSize(0x010b==p_e_opt_magic? 4 : 8);

    /* Decode the optional header. */
    Address entry_rva;
    if (4==get_executableFormat()->get_wordSize()) {
        p_e_lmajor             = ByteOrder::leToHost(oh32.e_lmajor);
        p_e_lminor             = ByteOrder::leToHost(oh32.e_lminor);
        p_e_code_size          = ByteOrder::leToHost(oh32.e_code_size);
        p_e_data_size          = ByteOrder::leToHost(oh32.e_data_size);
        p_e_bss_size           = ByteOrder::leToHost(oh32.e_bss_size);
        entry_rva              = ByteOrder::leToHost(oh32.e_entrypoint_rva);
        p_e_code_rva           = ByteOrder::leToHost(oh32.e_code_rva);
        p_e_data_rva           = ByteOrder::leToHost(oh32.e_data_rva);
        p_baseVa              = ByteOrder::leToHost(oh32.e_image_base);
        p_e_section_align      = ByteOrder::leToHost(oh32.e_section_align);
        p_e_file_align         = ByteOrder::leToHost(oh32.e_file_align);
        p_e_os_major           = ByteOrder::leToHost(oh32.e_os_major);
        p_e_os_minor           = ByteOrder::leToHost(oh32.e_os_minor);
        p_e_user_major         = ByteOrder::leToHost(oh32.e_user_major);
        p_e_user_minor         = ByteOrder::leToHost(oh32.e_user_minor);
        p_e_subsys_major       = ByteOrder::leToHost(oh32.e_subsys_major);
        p_e_subsys_minor       = ByteOrder::leToHost(oh32.e_subsys_minor);
        p_e_reserved9          = ByteOrder::leToHost(oh32.e_reserved9);
        p_e_image_size         = ByteOrder::leToHost(oh32.e_image_size);
        p_e_header_size        = ByteOrder::leToHost(oh32.e_header_size);
        p_e_file_checksum      = ByteOrder::leToHost(oh32.e_file_checksum);
        p_e_subsystem          = ByteOrder::leToHost(oh32.e_subsystem);
        p_e_dll_flags          = ByteOrder::leToHost(oh32.e_dll_flags);
        p_e_stack_reserve_size = ByteOrder::leToHost(oh32.e_stack_reserve_size);
        p_e_stack_commit_size  = ByteOrder::leToHost(oh32.e_stack_commit_size);
        p_e_heap_reserve_size  = ByteOrder::leToHost(oh32.e_heap_reserve_size);
        p_e_heap_commit_size   = ByteOrder::leToHost(oh32.e_heap_commit_size);
        p_e_loader_flags       = ByteOrder::leToHost(oh32.e_loader_flags);
        p_e_num_rvasize_pairs  = ByteOrder::leToHost(oh32.e_num_rvasize_pairs);
    } else if (8==get_executableFormat()->get_wordSize()) {
        /* We guessed wrong. This is a 64-bit header, not 32-bit. */
        PE64OptHeader_disk oh64;
        Address need64 = sizeof(PEFileHeader_disk) + std::min(p_e_nt_hdr_size, (Address)(sizeof oh64));
        if (need64>get_size())
            extend(need64-get_size());
        if (sizeof(oh64)!=readContentLocal(sizeof fh, &oh64, sizeof oh64))
            mlog[WARN] <<"SgAsmPEFileHeader::parse: short read of PE Optional Header at byte "
                       <<StringUtility::addrToString(get_offset() + sizeof(fh)) <<"\n";
        p_e_lmajor             = ByteOrder::leToHost(oh64.e_lmajor);
        p_e_lminor             = ByteOrder::leToHost(oh64.e_lminor);
        p_e_code_size          = ByteOrder::leToHost(oh64.e_code_size);
        p_e_data_size          = ByteOrder::leToHost(oh64.e_data_size);
        p_e_bss_size           = ByteOrder::leToHost(oh64.e_bss_size);
        entry_rva              = ByteOrder::leToHost(oh64.e_entrypoint_rva);
        p_e_code_rva           = ByteOrder::leToHost(oh64.e_code_rva);
     // p_e_data_rva           = ByteOrder::leToHost(oh.e_data_rva);             /* not in PE32+ */
        p_baseVa              = ByteOrder::leToHost(oh64.e_image_base);
        p_e_section_align      = ByteOrder::leToHost(oh64.e_section_align);
        p_e_file_align         = ByteOrder::leToHost(oh64.e_file_align);
        p_e_os_major           = ByteOrder::leToHost(oh64.e_os_major);
        p_e_os_minor           = ByteOrder::leToHost(oh64.e_os_minor);
        p_e_user_major         = ByteOrder::leToHost(oh64.e_user_major);
        p_e_user_minor         = ByteOrder::leToHost(oh64.e_user_minor);
        p_e_subsys_major       = ByteOrder::leToHost(oh64.e_subsys_major);
        p_e_subsys_minor       = ByteOrder::leToHost(oh64.e_subsys_minor);
        p_e_reserved9          = ByteOrder::leToHost(oh64.e_reserved9);
        p_e_image_size         = ByteOrder::leToHost(oh64.e_image_size);
        p_e_header_size        = ByteOrder::leToHost(oh64.e_header_size);
        p_e_file_checksum      = ByteOrder::leToHost(oh64.e_file_checksum);
        p_e_subsystem          = ByteOrder::leToHost(oh64.e_subsystem);
        p_e_dll_flags          = ByteOrder::leToHost(oh64.e_dll_flags);
        p_e_stack_reserve_size = ByteOrder::leToHost(oh64.e_stack_reserve_size);
        p_e_stack_commit_size  = ByteOrder::leToHost(oh64.e_stack_commit_size);
        p_e_heap_reserve_size  = ByteOrder::leToHost(oh64.e_heap_reserve_size);
        p_e_heap_commit_size   = ByteOrder::leToHost(oh64.e_heap_commit_size);
        p_e_loader_flags       = ByteOrder::leToHost(oh64.e_loader_flags);
        p_e_num_rvasize_pairs  = ByteOrder::leToHost(oh64.e_num_rvasize_pairs);
    } else {
        throw FormatError("unrecognized Windows PE optional header magic number");
    }

    /* Magic number */
    p_magic.clear();
    for (size_t i = 0; i < sizeof(fh.e_magic); ++i)
        p_magic.push_back(fh.e_magic[i]);

    /* File format */
    ROSE_ASSERT(p_e_lmajor <= 0xffff && p_e_lminor <= 0xffff);
    get_executableFormat()->set_version((p_e_lmajor << 16) | p_e_lminor);
    get_executableFormat()->set_isCurrentVersion(true); /*FIXME*/

    /* Target architecture */
    switch (p_e_cpu_type) {
      case 0x0000:
        set_isa(ISA_UNSPECIFIED);
        break;
      case 0x014c:
        set_isa(ISA_IA32_386);
        break;
      case 0x014d:
        set_isa(ISA_IA32_486);
        break;
      case 0x014e:
        set_isa(ISA_IA32_Pentium);
        break;
      case 0x0162:
        set_isa(ISA_MIPS_MarkI); /* R2000, R3000 */
        break;
      case 0x0163:
        set_isa(ISA_MIPS_MarkII); /* R6000 */
        break;
      case 0x0166:
        set_isa(ISA_MIPS_MarkIII); /* R4000 */
        break;
      case 0x01a2: /*Hitachi SH3*/
      case 0x01a3: /*Hitachi SH3 with FPU*/
      case 0x01a6: /*Hitachi SH4*/
      case 0x01a8: /*Hitachi SH5*/
        set_isa(ISA_Hitachi_SH);
        break;
      case 0x01c0:
        set_isa(ISA_ARM_Family);
        break;
      case 0x01d3:
        set_isa(ISA_Matsushita_AM33);
        break;
      case 0x01f0: /*w/o FPU*/
      case 0x01f1: /*with FPU*/
        set_isa(ISA_PowerPC);
        break;
      case 0x0200:
        set_isa(ISA_IA64_Family);
        break;
      case 0x0266:
        set_isa(ISA_MIPS_16);
        break;
      case 0x0366:
        set_isa(ISA_MIPS_FPU);
        break;
      case 0x0466:
        set_isa(ISA_MIPS_16FPU);
        break;
      case 0x0ebc:
        set_isa(ISA_EFI_ByteCode);
        break;
      case 0x8664:
        set_isa(ISA_X8664_Family);
        break;
      case 0x9041:
        set_isa(ISA_Mitsubishi_M32R);
        break;
      default:
        mlog[WARN] <<"SgAsmPEFileHeader::parse: warning: unrecognized e_cputype = "
                   <<StringUtility::toHex2(p_e_cpu_type, 16) <<"\n";
        set_isa(ISA_OTHER);
        break;
    }

    /* The NT loader normally maps this file header at the header's base virtual address. */
    set_mappedPreferredRva(0);
    set_mappedActualVa(0); /* will be assigned by BinaryLoader */
    set_mappedSize(p_e_header_size);
    set_mappedAlignment(0);
    set_mappedReadPermission(true);
    set_mappedWritePermission(false);
    set_mappedExecutePermission(false);

    /* Entry point. We will eventually bind the entry point to a particular section (in SgAsmPEFileHeader::parse) so that if
     * sections are rearranged, extended, etc. the entry point will be updated automatically. */
    addEntryRva(entry_rva);

    /* The PE File Header has a fixed-size component followed by some number of RVA/Size pairs. The addRvaSizePairs() will
     * extend  the header and parse the RVA/Size pairs. */
    if (get_e_num_rvasize_pairs() > 1000) {
        mlog[WARN] <<"PE File Header contains an unreasonable number of Rva/Size pairs. Limiting to 1000.\n";
        set_e_num_rvasize_pairs(1000);
    }
    addRvaSizePairs();

    /* Construct the section table and its sections (non-synthesized sections). The specification says that the section table
     * comes after the optional (NT) header, which in turn comes after the fixed part of the PE header. The size of the
     * optional header is indicated in the fixed header. */
    Address secttab_offset = get_offset() + sizeof(PEFileHeader_disk) + get_e_nt_hdr_size();
    Address secttab_size = get_e_nsections() * sizeof(SgAsmPESectionTableEntry::PESectionTableEntry_disk);
    SgAsmPESectionTable *secttab = new SgAsmPESectionTable(this);
    secttab->set_offset(secttab_offset);
    secttab->set_size(secttab_size);
    secttab->parse();
    set_sectionTable(secttab);

    /* Parse the COFF symbol table */
    if (get_e_coff_symtab() && get_e_coff_nsyms()) {
        SgAsmCoffSymbolTable *symtab = new SgAsmCoffSymbolTable(this);
        symtab->set_offset(get_e_coff_symtab());
        symtab->parse();
        set_coffSymbolTable(symtab);
    }

    /* Associate RVAs with particular sections so that if a section's mapping is changed the RVA gets adjusted automatically. */
    ROSE_ASSERT(get_entryRvas().size()==1);
    get_entryRvas()[0].bindBestSection(this);
    get_e_code_rva().bindBestSection(this);
    get_e_data_rva().bindBestSection(this);

    /* Turn header-specified tables (RVA/Size pairs) into generic sections */
    createTableSections();
    return this;
}

/* Encode the PE header into disk format */
void *
SgAsmPEFileHeader::encode(PEFileHeader_disk *disk) const
{
    for (size_t i=0; i<ROSE_NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    ByteOrder::hostToLe(p_e_cpu_type,           &(disk->e_cpu_type));
    ByteOrder::hostToLe(p_e_nsections,          &(disk->e_nsections));
    ByteOrder::hostToLe(p_e_time,               &(disk->e_time));
    ByteOrder::hostToLe(p_e_coff_symtab,        &(disk->e_coff_symtab));
    ByteOrder::hostToLe(p_e_coff_nsyms,         &(disk->e_coff_nsyms));
    ByteOrder::hostToLe(p_e_nt_hdr_size,        &(disk->e_nt_hdr_size));
    ByteOrder::hostToLe(p_e_flags,              &(disk->e_flags));

    return disk;
}
void *
SgAsmPEFileHeader::encode(PE32OptHeader_disk *disk) const
{
    ByteOrder::hostToLe(p_e_opt_magic,          &(disk->e_opt_magic));
    ByteOrder::hostToLe(p_e_lmajor,             &(disk->e_lmajor));
    ByteOrder::hostToLe(p_e_lminor,             &(disk->e_lminor));
    ByteOrder::hostToLe(p_e_code_size,          &(disk->e_code_size));
    ByteOrder::hostToLe(p_e_data_size,          &(disk->e_data_size));
    ByteOrder::hostToLe(p_e_bss_size,           &(disk->e_bss_size));
    ByteOrder::hostToLe(get_entryRva(),         &(disk->e_entrypoint_rva));
    ByteOrder::hostToLe(p_e_code_rva,           &(disk->e_code_rva));
    ByteOrder::hostToLe(p_e_data_rva,           &(disk->e_data_rva));
    ByteOrder::hostToLe(get_baseVa(),           &(disk->e_image_base));
    ByteOrder::hostToLe(p_e_section_align,      &(disk->e_section_align));
    ByteOrder::hostToLe(p_e_file_align,         &(disk->e_file_align));
    ByteOrder::hostToLe(p_e_os_major,           &(disk->e_os_major));
    ByteOrder::hostToLe(p_e_os_minor,           &(disk->e_os_minor));
    ByteOrder::hostToLe(p_e_user_major,         &(disk->e_user_major));
    ByteOrder::hostToLe(p_e_user_minor,         &(disk->e_user_minor));
    ByteOrder::hostToLe(p_e_subsys_major,       &(disk->e_subsys_major));
    ByteOrder::hostToLe(p_e_subsys_minor,       &(disk->e_subsys_minor));
    ByteOrder::hostToLe(p_e_reserved9,          &(disk->e_reserved9));
    ByteOrder::hostToLe(p_e_image_size,         &(disk->e_image_size));
    ByteOrder::hostToLe(p_e_header_size,        &(disk->e_header_size));
    ByteOrder::hostToLe(p_e_file_checksum,      &(disk->e_file_checksum));
    ByteOrder::hostToLe(p_e_subsystem,          &(disk->e_subsystem));
    ByteOrder::hostToLe(p_e_dll_flags,          &(disk->e_dll_flags));
    ByteOrder::hostToLe(p_e_stack_reserve_size, &(disk->e_stack_reserve_size));
    ByteOrder::hostToLe(p_e_stack_commit_size,  &(disk->e_stack_commit_size));
    ByteOrder::hostToLe(p_e_heap_reserve_size,  &(disk->e_heap_reserve_size));
    ByteOrder::hostToLe(p_e_heap_commit_size,   &(disk->e_heap_commit_size));
    ByteOrder::hostToLe(p_e_loader_flags,       &(disk->e_loader_flags));
    ByteOrder::hostToLe(p_e_num_rvasize_pairs,  &(disk->e_num_rvasize_pairs));

    return disk;
}
void *
SgAsmPEFileHeader::encode(PE64OptHeader_disk *disk) const
{
    ByteOrder::hostToLe(p_e_opt_magic,          &(disk->e_opt_magic));
    ByteOrder::hostToLe(p_e_lmajor,             &(disk->e_lmajor));
    ByteOrder::hostToLe(p_e_lminor,             &(disk->e_lminor));
    ByteOrder::hostToLe(p_e_code_size,          &(disk->e_code_size));
    ByteOrder::hostToLe(p_e_data_size,          &(disk->e_data_size));
    ByteOrder::hostToLe(p_e_bss_size,           &(disk->e_bss_size));
    ByteOrder::hostToLe(get_entryRva(),         &(disk->e_entrypoint_rva));
    ByteOrder::hostToLe(p_e_code_rva,           &(disk->e_code_rva));
 // ByteOrder::hostToLe(p_e_data_rva,           &(disk->e_data_rva)); /* not present in PE32+ */
    ByteOrder::hostToLe(get_baseVa(),           &(disk->e_image_base));
    ByteOrder::hostToLe(p_e_section_align,      &(disk->e_section_align));
    ByteOrder::hostToLe(p_e_file_align,         &(disk->e_file_align));
    ByteOrder::hostToLe(p_e_os_major,           &(disk->e_os_major));
    ByteOrder::hostToLe(p_e_os_minor,           &(disk->e_os_minor));
    ByteOrder::hostToLe(p_e_user_major,         &(disk->e_user_major));
    ByteOrder::hostToLe(p_e_user_minor,         &(disk->e_user_minor));
    ByteOrder::hostToLe(p_e_subsys_major,       &(disk->e_subsys_major));
    ByteOrder::hostToLe(p_e_subsys_minor,       &(disk->e_subsys_minor));
    ByteOrder::hostToLe(p_e_reserved9,          &(disk->e_reserved9));
    ByteOrder::hostToLe(p_e_image_size,         &(disk->e_image_size));
    ByteOrder::hostToLe(p_e_header_size,        &(disk->e_header_size));
    ByteOrder::hostToLe(p_e_file_checksum,      &(disk->e_file_checksum));
    ByteOrder::hostToLe(p_e_subsystem,          &(disk->e_subsystem));
    ByteOrder::hostToLe(p_e_dll_flags,          &(disk->e_dll_flags));
    ByteOrder::hostToLe(p_e_stack_reserve_size, &(disk->e_stack_reserve_size));
    ByteOrder::hostToLe(p_e_stack_commit_size,  &(disk->e_stack_commit_size));
    ByteOrder::hostToLe(p_e_heap_reserve_size,  &(disk->e_heap_reserve_size));
    ByteOrder::hostToLe(p_e_heap_commit_size,   &(disk->e_heap_commit_size));
    ByteOrder::hostToLe(p_e_loader_flags,       &(disk->e_loader_flags));
    ByteOrder::hostToLe(p_e_num_rvasize_pairs,  &(disk->e_num_rvasize_pairs));

    return disk;
}

void
SgAsmPEFileHeader::set_rvasize_pair(PairPurpose idx, SgAsmPESection *section)
{
    set_rvaSizePair(idx, section);
}

void
SgAsmPEFileHeader::set_rvaSizePair(PairPurpose idx, SgAsmPESection *section)
{
    ROSE_ASSERT(get_rvaSizePairs()!=NULL);
    ROSE_ASSERT(section->get_parent()!=NULL);
    ROSE_ASSERT(isSgAsmPEFileHeader(section->get_header())!=NULL);

    switch (idx) {
        case PAIR_EXPORTS:
        case PAIR_IMPORTS:
        case PAIR_RESOURCES:
        case PAIR_EXCEPTIONS:
        case PAIR_CERTIFICATES:
        case PAIR_BASERELOCS:
        case PAIR_DEBUG:
        case PAIR_ARCHITECTURE:
        case PAIR_GLOBALPTR:
        case PAIR_TLS:
        case PAIR_LOADCONFIG:
        case PAIR_BOUNDIMPORT:
        case PAIR_IAT:
        case PAIR_DELAYIMPORT:
        case PAIR_CLRRUNTIME:
        case PAIR_RESERVED15:
            break;
        default:
            mlog[WARN] <<"SgAsmPEFileHeader::set_rvasize_pair: index " <<idx <<" exceeds specification limit\n";
    }

    /* Extend array of rva/size pairs if necessary */
    if ((size_t)idx>=get_rvaSizePairs()->get_pairs().size()) {
        get_rvaSizePairs()->get_pairs().resize(idx+1, NULL);
        for (size_t i=0; i<=(size_t)idx; i++) {
            if (NULL==get_rvaSizePairs()->get_pairs()[i]) {
                SgAsmPERVASizePair *pair = new SgAsmPERVASizePair(get_rvaSizePairs(), 0, 0);
                get_rvaSizePairs()->get_pairs()[i] = pair;
            }
        }
    }

    SgAsmPERVASizePair *pair = get_rvaSizePairs()->get_pairs()[idx];
    ROSE_ASSERT(pair!=NULL);
    pair->set_e_rva(RelativeVirtualAddress(section->get_mappedPreferredRva(), section));
    pair->set_e_size(section->get_mappedSize());
    pair->set_section(section);

    /* If the section has no name then give it one based on the RVA/Size index. This is mostly for convenience and debugging
     * since the name is never stored in the file. */
    if (section->get_name()->get_string().empty()) {
        const char *short_name;
        section->get_name()->set_string(rvaSizePairName(idx, &short_name));
        section->set_shortName(short_name);
    }
}

void
SgAsmPEFileHeader::update_rvasize_pairs()
{
    updateRvaSizePairs();
}

void
SgAsmPEFileHeader::updateRvaSizePairs()
{
    for (size_t i=0; i<get_rvaSizePairs()->get_pairs().size(); i++) {
        SgAsmPERVASizePair *pair = get_rvaSizePairs()->get_pairs()[i];
        SgAsmGenericSection *section = pair->get_section();
        if (section) {
            pair->set_e_rva(RelativeVirtualAddress(section->get_mappedPreferredRva(), section));
            pair->set_e_size(section->get_mappedSize());
        }
    }
}

void
SgAsmPEFileHeader::add_rvasize_pairs()
{
    return addRvaSizePairs();
}

/* Adds the RVA/Size pairs to the end of the PE file header */
void
SgAsmPEFileHeader::addRvaSizePairs()
{
    Address pairs_offset = get_size();
    Address pairs_size   = p_e_num_rvasize_pairs * sizeof(SgAsmPERVASizePair::RVASizePair_disk);
    SgAsmPERVASizePair::RVASizePair_disk pairs_disk;

    ROSE_ASSERT(get_rvaSizePairs() != NULL);
    ROSE_ASSERT(get_rvaSizePairs()->get_pairs().size()==0);
    get_rvaSizePairs()->set_isModified(true);

    extend(pairs_size);
    for (size_t i = 0; i < p_e_num_rvasize_pairs; i++, pairs_offset += sizeof pairs_disk) {
        if (sizeof(pairs_disk)!=readContentLocal(pairs_offset, &pairs_disk, sizeof pairs_disk, false))
            mlog[WARN] <<"SgAsmPEFileHeader::addRvaSizePairs: RVA/Size pair " <<i
                       <<" at file offset " <<StringUtility::addrToString(get_offset()+pairs_offset)
                       <<" extends beyond the end of file (assuming 0/0)\n";
        get_rvaSizePairs()->get_pairs().push_back(new SgAsmPERVASizePair(get_rvaSizePairs(), &pairs_disk));
    }
}

void
SgAsmPEFileHeader::create_table_sections()
{
    createTableSections();
}

/* Looks at the RVA/Size pairs in the PE header and creates an SgAsmGenericSection object for each one.  This must be done
 * after we build the mapping from virtual addresses to file offsets. */
void
SgAsmPEFileHeader::createTableSections()
{

    /* First, only create the sections. */
    for (size_t i=0; i<get_rvaSizePairs()->get_pairs().size(); i++) {
        SgAsmPERVASizePair *pair = get_rvaSizePairs()->get_pairs()[i];

        if (0==pair->get_e_size())
            continue;

        /* Table names come from PE file specification and are hard coded by RVA/Size pair index */
        const char *tabname_short;
        std::string tabname = rvaSizePairName((PairPurpose)i, &tabname_short);

        /* Find the starting offset in the file.
         * FIXME: We have a potential problem here in that ROSE sections are always contiguous in the file but a section created
         *        from an RVA/Size pair is not necessarily contiguous in the file.  Normally such sections are in fact
         *        contiguous and we'll just ignore this for now.  In any case, as long as these sections only ever read their
         *        data via the same MemoryMap that we use here, everything should be fine. [RPM 2009-08-17] */
        Address pair_va = get_baseVa() + pair->get_e_rva();
        MemoryMap::Ptr map = get_loaderMap();
        ROSE_ASSERT(map!=NULL);
        if (!map->baseSize(pair_va, pair->get_e_size()).exists(Sawyer::Container::MATCH_WHOLE)) {
            mlog[WARN] <<"SgAsmPEFileHeader::createTableSections: pair-" <<i
                       <<", rva=" <<StringUtility::addrToString(pair->get_e_rva().rva())
                       <<", size=" <<StringUtility::plural(pair->get_e_size(), "bytes")
                       <<" \"" <<StringUtility::cEscape(tabname) <<"\":"
                       <<" unable to find a mapping for the virtual address (skipping)\n";
            continue;
        }
        const MemoryMap::Node &me = *map->at(pair_va).findNode();
        Address file_offset = me.value().offset() + pair_va - me.key().least();

        /* Create the new section */
        SgAsmGenericSection *tabsec = NULL;
        switch (i) {
            case 0: {
                /* Sometimes export sections are represented by a ".edata" section, and sometimes they're represented by an
                 * RVA/Size pair, sometimes both point to the same part of the file, and sometimes the RVA/Size pair points to
                 * a different part of the file. We don't want the exports duplicated in the AST, so we only create this table
                 * as exports if we haven't already seen some other export section. */
                SgAsmGenericSectionPtrList &sections = get_sections()->get_sections();
                bool seen_exports = false;
                for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); !seen_exports && si!=sections.end(); ++si)
                    seen_exports = isSgAsmPEExportSection(*si);
                if (seen_exports) {
                    tabsec = new SgAsmGenericSection(get_file(), this);
                } else {
                    tabsec = new SgAsmPEExportSection(this);
                }
                break;
            }
            case 1: {
                /* Sometimes import sections are represented by a ".idata" section, and sometimes they're represented by an
                 * RVA/Size pair, and sometimes both point to the same part of the file.  We don't want the imports duplicated
                 * in the AST, so we only create this table as imports if we haven't already seen some other import section. */
                SgAsmGenericSectionPtrList &sections = get_sections()->get_sections();
                bool seen_imports = false;
                for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); !seen_imports && si!=sections.end(); ++si)
                    seen_imports = isSgAsmPEImportSection(*si);
                if (seen_imports) {
                    tabsec = new SgAsmGenericSection(get_file(), this);
                } else {
                    tabsec = new SgAsmPEImportSection(this);
                }
                break;
            }
            case 14: {
                /* 14==IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR
                 * CLR metadata is stored in this section. It is used to indicate that the object file contains
                 * managed code. The format of the metadata is not documented, but can be handed to the CLR
                 * interfaces for handling metadata. https://docs.microsoft.com/en-us/windows/win32/debug/pe-format
                 */
                tabsec = new SgAsmCliHeader(this);
                break;
            }
            default: {
                tabsec = new SgAsmGenericSection(get_file(), this);
                break;
            }
        }
        tabsec->set_name(new SgAsmBasicString(tabname));
        tabsec->set_shortName(tabname_short);
        tabsec->set_synthesized(true);
        tabsec->set_purpose(SP_HEADER);

        tabsec->set_offset(file_offset);
        tabsec->set_size(pair->get_e_size());
        tabsec->set_fileAlignment(1);

        tabsec->set_mappedAlignment(1);
        tabsec->set_mappedPreferredRva(pair->get_e_rva().rva());
        tabsec->set_mappedActualVa(pair->get_e_rva().rva()+get_baseVa()); /*FIXME: not sure this is correct. [RPM 2009-09-11]*/
        tabsec->set_mappedSize(pair->get_e_size());
        tabsec->set_mappedReadPermission(true);
        tabsec->set_mappedWritePermission(false);
        tabsec->set_mappedExecutePermission(false);
        pair->set_section(tabsec);
        pair->get_e_rva().bindSection(tabsec);
    }

    /* Now parse the sections */
    for (size_t i=0; i<get_rvaSizePairs()->get_pairs().size(); i++) {
        SgAsmPERVASizePair *pair = get_rvaSizePairs()->get_pairs()[i];
        SgAsmGenericSection *tabsec = pair->get_section();
        if (tabsec)
            tabsec->parse();
    }
}

/* Change size of PE header based on word size */
bool
SgAsmPEFileHeader::reallocate()
{
    struct Resources {
        unsigned char *oh;
        Resources(): oh(NULL) {}
        ~Resources() { delete[] oh; }
    } r;

    bool reallocated = SgAsmGenericHeader::reallocate();
    
    /* Resize if necessary */
    Address need = sizeof(PEFileHeader_disk);
    if (4==get_wordSize()) {
        need += sizeof(PE32OptHeader_disk);
    } else if (8==get_wordSize()) {
        need += sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    need += get_rvaSizePairs()->get_pairs().size() * sizeof(SgAsmPERVASizePair::RVASizePair_disk);
    if (need<get_size()) {
        if (isMapped()) {
            ROSE_ASSERT(get_mappedSize()==get_size());
            set_mappedSize(need);
        }
        set_size(need);
        reallocated = true;
    } else if (need>get_size()) {
        get_file()->shiftExtend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    /* Make sure the RVA/Size pairs at the end of the header are consistent with the sections to which they point. Reallocate()
     * has already been called recursively for the sections. */
    updateRvaSizePairs();

    /* Make sure header is consistent with sections. Reallocate() has already been called recursively for the sections.
     * Count the number of sections in the table and update the header's e_nsections member. */
    if (get_sectionTable()) {
        ROSE_ASSERT(get_sectionTable()->get_header()==this);
        SgAsmGenericSectionList *all = get_sections();
        p_e_nsections = 0;
        for (size_t i=0; i<all->get_sections().size(); i++) {
            SgAsmPESection *pesec = dynamic_cast<SgAsmPESection*>(all->get_sections()[i]);
            if (pesec && pesec->get_section_entry()!=NULL)
                p_e_nsections++;
        }

        Address header_size = alignUp(get_sectionTable()->get_offset() + get_sectionTable()->get_size(),
                                      (Address)(p_e_file_align>0 ? p_e_file_align : 1));
#if 1
        /* The PE Specification regarding e_header_size (known as "SizeOfHeader" on page 14 of "Microsoft Portable Executable
         * and Common Object File Format Specification: Revision 8.1 February 15, 2008" is not always followed. We recompute
         * it here as being the minimum RVA from all the sections defined in the PE Section Table, but not smaller
         * than the value according to the specification. This alternate value is kept if it's already in the parse tree,
         * otherwise we use the correct value. (RPM 2008-10-21) */
        Address min_offset = 0;
        for (size_t i=0, nfound=0; i<all->get_sections().size(); i++) {
            SgAsmPESection *pesec = dynamic_cast<SgAsmPESection*>(all->get_sections()[i]);
            if (pesec && pesec->get_section_entry()!=NULL) {
                if (0==nfound++) {
                    min_offset = pesec->get_offset();
                } else {
                    min_offset = std::min(min_offset, pesec->get_offset() );
                }
            }
        }

        Address header_size2 = std::max(header_size, min_offset);
        if (p_e_header_size==header_size2)
            header_size = header_size2;

        /* If the original header size was zero then don't change that--leave it at zero. Some tiny executables have a zero
         * value here and as a result, since this is near the end of the NT Optional Header, they can truncate the file and
         * the loader will fill the optional header with zeros when reading. (RPM 2008-11-11) */
        if (p_e_header_size==0)
            header_size = 0;
#endif
        p_e_header_size = header_size;
    }

    /* The size of the optional header. If there's a section table then we use its offset to calculate the optional header
     * size in order to be compatible with the PE loader. Otherwise use the actual optional header size. */
    if (get_sectionTable()) {
        ROSE_ASSERT(get_sectionTable()->get_offset() >= get_offset() + sizeof(PEFileHeader_disk));
        p_e_nt_hdr_size = get_sectionTable()->get_offset() - (get_offset() + sizeof(PEFileHeader_disk));
    } else if (4==get_wordSize()) {
        p_e_nt_hdr_size = sizeof(PE32OptHeader_disk);
    } else if (8==get_wordSize()) {
        p_e_nt_hdr_size = sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("invalid PE word size");
    }
            
    /* Update COFF symbol table related data members in the file header */
    if (get_coffSymbolTable()) {
        ROSE_ASSERT(get_coffSymbolTable()->get_header()==this);
        set_e_coff_symtab(get_coffSymbolTable()->get_offset());
        set_e_coff_nsyms(get_coffSymbolTable()->get_nslots());
    }

    /* Update some additional header fields */
    set_e_num_rvasize_pairs(get_rvaSizePairs()->get_pairs().size());
    set_e_opt_magic(4==get_wordSize() ? 0x010b : 0x020b);
    set_e_lmajor((get_executableFormat()->get_version() >> 16) & 0xffff);
    set_e_lminor(get_executableFormat()->get_version() & 0xffff);

    /* Adjust the COFF Header's e_nt_hdr_size to accommodate the NT Optional Header in such a way that EXEs from tinype.com
     * don't change (i.e., don't increase e_nt_hdr_size if the bytes beyond it are zero anyway, and if they aren't then adjust
     * it as little as possible.  The RVA/Size pairs are considered to be part of the NT Optional Header. */
    size_t oh_size = get_rvaSizePairs()->get_pairs().size() * sizeof(SgAsmPERVASizePair::RVASizePair_disk);
    if (4==get_wordSize()) {
        oh_size += sizeof(PE32OptHeader_disk);
    } else if (8==get_wordSize()) {
        oh_size += sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    r.oh = new unsigned char[oh_size];
    memset(r.oh, 0, oh_size);
    if (4==get_wordSize()) {
        encode((PE32OptHeader_disk*)r.oh);
    } else if (8==get_wordSize()) {
        encode((PE64OptHeader_disk*)r.oh);
    } else {
        throw FormatError("unsupported PE word size");
    }
    while (oh_size>p_e_nt_hdr_size) {
        if (0!=r.oh[oh_size-1]) break;
        --oh_size;
    }
    set_e_nt_hdr_size(oh_size);
    return reallocated;
}

/* Write the PE file header back to disk and all that it references */
void
SgAsmPEFileHeader::unparse(std::ostream &f) const
{
    struct Resources {
        unsigned char *oh;
        Resources(): oh(NULL) {}
        ~Resources() { delete[] oh; }
    } r;

    /* Write unreferenced areas back to the file before anything else. */
    unparseHoles(f);
    
    /* Write sections in the order of specialization, from least specialized to most specialized. This gives more specialized
     * sections a chance to overwrite the less specialized sections. */
    const SgAsmGenericSectionPtrList &sections = get_sections()->get_sections();
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
        if (V_SgAsmGenericSection==(*si)->variantT())
            (*si)->unparse(f);
    }
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
        if (V_SgAsmPESection==(*si)->variantT())
            (*si)->unparse(f);
    }
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
        if (V_SgAsmGenericSection!=(*si)->variantT() && V_SgAsmPESection!=(*si)->variantT())
            (*si)->unparse(f);
    }

    /* Encode the "NT Optional Header" before the COFF Header since the latter depends on the former. Adjust the COFF Header's
     * e_nt_hdr_size to accommodate the NT Optional Header in such a way that EXEs from tinype.com don't change (i.e., don't
     * increase e_nt_hdr_size if the bytes beyond it are zero anyway, and if they aren't then adjust it as little as possible.
     * The RVA/Size pairs are considered to be part of the NT Optional Header. */
    size_t oh_size = get_rvaSizePairs()->get_pairs().size() * sizeof(SgAsmPERVASizePair::RVASizePair_disk);
    size_t rvasize_offset; /*offset with respect to "oh" buffer allocated below*/
    if (4==get_wordSize()) {
        oh_size += sizeof(PE32OptHeader_disk);
    } else if (8==get_wordSize()) {
        oh_size += sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    r.oh = new unsigned char[oh_size];
    if (4==get_wordSize()) {
        encode((PE32OptHeader_disk*)r.oh);
        rvasize_offset = sizeof(PE32OptHeader_disk);
    } else if (8==get_wordSize()) {
        encode((PE64OptHeader_disk*)r.oh);
        rvasize_offset = sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    for (size_t i=0; i<get_rvaSizePairs()->get_pairs().size(); i++, rvasize_offset+=sizeof(SgAsmPERVASizePair::RVASizePair_disk)) {
        SgAsmPERVASizePair::RVASizePair_disk *rvasize_disk = (SgAsmPERVASizePair::RVASizePair_disk*)(r.oh+rvasize_offset);
        get_rvaSizePairs()->get_pairs()[i]->encode(rvasize_disk);
    }
    while (oh_size>p_e_nt_hdr_size) {
        if (0!=r.oh[oh_size-1]) break;
        --oh_size;
    }
    ROSE_ASSERT(p_e_nt_hdr_size==oh_size); /*set in reallocate()*/

    /* Write the fixed-length COFF Header */
    PEFileHeader_disk fh;
    encode(&fh);
    Address spos = write(f, 0, sizeof fh, &fh);

    /* Write the following "NT Optional Header" */
    spos = write(f, spos, oh_size, r.oh);
}
    
/* Print some debugging information */
void
SgAsmPEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPEFileHeader[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPEFileHeader.", prefix);
    }

    int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));
    time_t t = p_e_time;
    char time_str[128];
    struct tm *tm = localtime(&t);
    if (tm) {
        strftime(time_str, sizeof time_str, "%c", tm);
    } else {
        strcpy(time_str, "INVALID");
    }

    SgAsmGenericHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = 0x%04x (%u)\n",               p, w, "e_cpu_type",          p_e_cpu_type, p_e_cpu_type);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nsections",         p_e_nsections);
    fprintf(f, "%s%-*s = %u (%s)\n",                   p, w, "e_time",              p_e_time, time_str);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, w, "e_coff_symtab",   p_e_coff_symtab, p_e_coff_symtab);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_coff_nsyms",        p_e_coff_nsyms);
    if (get_coffSymbolTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n",           p, w, "coff_symtab",
                get_coffSymbolTable()->get_id(), get_coffSymbolTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",                  p, w, "coff_symtab");
    }
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, w, "e_nt_hdr_size",   p_e_nt_hdr_size, p_e_nt_hdr_size);
    fprintf(f, "%s%-*s = 0x%04x (%u)\n",               p, w, "e_flags",             p_e_flags, p_e_flags);
    fprintf(f, "%s%-*s = 0x%04x %s\n",                 p, w, "e_opt_magic",         p_e_opt_magic,
            0x10b == p_e_opt_magic ? "PE32" : (0x20b == p_e_opt_magic ? "PE32+" : "other"));
    fprintf(f, "%s%-*s = %u.%u\n",                     p, w, "linker_vers",         p_e_lmajor, p_e_lminor);
    fprintf(f, "%s%-*s = 0x%08x (%u) bytes\n",         p, w, "e_code_size",         p_e_code_size, p_e_code_size);
    fprintf(f, "%s%-*s = 0x%08x (%u) bytes\n",         p, w, "e_data_size",         p_e_data_size, p_e_data_size);
    fprintf(f, "%s%-*s = 0x%08x (%u) bytes\n",         p, w, "e_bss_size",          p_e_bss_size, p_e_bss_size);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "e_code_rva",          p_e_code_rva.toString().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "e_data_rva",          p_e_data_rva.toString().c_str());
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_section_align",     p_e_section_align, p_e_section_align);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_file_align",        p_e_file_align, p_e_file_align);
    fprintf(f, "%s%-*s = %u.%u\n",                     p, w, "os_vers",             p_e_os_major, p_e_os_minor);
    fprintf(f, "%s%-*s = %u.%u\n",                     p, w, "user_vers",           p_e_user_major, p_e_user_minor);
    fprintf(f, "%s%-*s = %u.%u\n",                     p, w, "subsys_vers",         p_e_subsys_major, p_e_subsys_minor);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_reserved9",         p_e_reserved9);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_image_size",        p_e_image_size, p_e_image_size);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_header_size",       p_e_header_size, p_e_header_size);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_file_checksum",     p_e_file_checksum, p_e_file_checksum);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_subsystem",         p_e_subsystem);
    fprintf(f, "%s%-*s = 0x%04x (%u)\n",               p, w, "e_dll_flags",         p_e_dll_flags, p_e_dll_flags);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_stack_reserve_size",
            p_e_stack_reserve_size, p_e_stack_reserve_size);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_stack_commit_size", p_e_stack_commit_size, p_e_stack_commit_size);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_heap_reserve_size", p_e_heap_reserve_size, p_e_heap_reserve_size);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_heap_commit_size",  p_e_heap_commit_size, p_e_heap_commit_size);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "e_loader_flags",      p_e_loader_flags, p_e_loader_flags);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_num_rvasize_pairs", p_e_num_rvasize_pairs);
    for (unsigned i = 0; i < get_rvaSizePairs()->get_pairs().size(); i++) {
        std::string p2 = (boost::format("%s.pair[%d].") %p %i).str();
        w = std::max(size_t{1}, DUMP_FIELD_WIDTH - p2.size());
        fprintf(f, "%s%-*s = rva %s,\tsize 0x%08" PRIx64 " (%" PRIu64 ")\n", p2.c_str(), w, "..",
                get_rvaSizePairs()->get_pairs()[i]->get_e_rva().toString().c_str(),
                get_rvaSizePairs()->get_pairs()[i]->get_e_size(), get_rvaSizePairs()->get_pairs()[i]->get_e_size());
    }
    if (get_sectionTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "section_table",
                get_sectionTable()->get_id(), get_sectionTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "section_table");
    }

    if (variantT() == V_SgAsmPEFileHeader) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);

    /* Show the simulated loader memory map */
    const MemoryMap::Ptr map = get_loaderMap();
    if (map) {
        map->dump(f, (std::string(p)+"loader_map: ").c_str());
    } else {
        fprintf(f, "%s%-*s = not defined\n", p, w, "loader_map");
    }
}

SgAsmPERVASizePairList*
SgAsmPEFileHeader::get_rvasize_pairs() const {
    return get_rvaSizePairs();
}

void
SgAsmPEFileHeader::set_rvasize_pairs(SgAsmPERVASizePairList *x) {
    set_rvaSizePairs(x);
}

SgAsmPESectionTable*
SgAsmPEFileHeader::get_section_table() const {
    return get_sectionTable();
}

void
SgAsmPEFileHeader::set_section_table(SgAsmPESectionTable *x) {
    set_sectionTable(x);
}

SgAsmCoffSymbolTable*
SgAsmPEFileHeader::get_coff_symtab() const {
    return get_coffSymbolTable();
}

void
SgAsmPEFileHeader::set_coff_symtab(SgAsmCoffSymbolTable *x) {
    set_coffSymbolTable(x);
}

const char*
SgAsmPEFileHeader::format_name() const {
    return formatName();
}

const char*
SgAsmPEFileHeader::formatName() const {
    return "PE";
}

MemoryMap::Ptr
SgAsmPEFileHeader::get_loader_map() const {
    return get_loaderMap();
}

MemoryMap::Ptr
SgAsmPEFileHeader::get_loaderMap() const {
    return p_loader_map;
}

void
SgAsmPEFileHeader::set_loader_map(const MemoryMap::Ptr &x) {
    set_loaderMap(x);
}

void
SgAsmPEFileHeader::set_loaderMap(const MemoryMap::Ptr &x) {
    p_loader_map = x;
}

#endif
