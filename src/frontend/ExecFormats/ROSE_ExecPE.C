/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define ALIGN(ADDR,ALMNT)       ((((ADDR)+(ALMNT)-1)/(ALMNT))*(ALMNT))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Construct a new PE File Header with default values. */
void
SgAsmPEFileHeader::ctor()
{
    ROSE_ASSERT(get_file()!=NULL);
    ROSE_ASSERT(get_size()>0);

    set_name(new SgAsmBasicString("PE File Header"));
    set_synthesized(true);
    set_purpose(SP_HEADER);

    p_rvasize_pairs = new SgAsmPERVASizePairList;
    p_rvasize_pairs->set_parent(this);

    /* Magic number */
    p_magic.clear();
    p_magic.push_back('P');
    p_magic.push_back('E');
    p_magic.push_back('\0');
    p_magic.push_back('\0');

    /* Executable Format */
    ROSE_ASSERT(p_exec_format!=NULL);
    p_exec_format->set_family(FAMILY_PE);
    p_exec_format->set_purpose(PURPOSE_EXECUTABLE);
    p_exec_format->set_sex(ORDER_LSB);
    p_exec_format->set_word_size(4);
    p_exec_format->set_version(0);
    p_exec_format->set_is_current_version(true);
    p_exec_format->set_abi(ABI_NT);
    p_exec_format->set_abi_version(0);

    /* Default instruction architecture */
    p_e_cpu_type = 0x014c; /*i386*/
    p_isa = ISA_IA32_386;

    p_e_time = time(NULL);
}

/** Return true if the file looks like it might be a PE file according to the magic number.  The file must contain what
 *  appears to be a DOS File Header at address zero, and what appears to be a PE File Header at a file offset specified in
 *  part of the DOS File Header (actually, in the bytes that follow the DOS File Header). */
bool
SgAsmPEFileHeader::is_PE(SgAsmGenericFile *ef)
{
    /* Check DOS File Header magic number */
    SgAsmGenericSection *section = new SgAsmGenericSection(ef, NULL, 0, 0x40);
    section->grab_content();
    unsigned char dos_magic[2];
    section->content(0, 2, dos_magic);
    if ('M'!=dos_magic[0] || 'Z'!=dos_magic[1]) {
        delete section;
        return false;
    }
    
    /* Read offset of potential PE File Header */
    uint32_t lfanew_disk;
    section->content(0x3c, sizeof lfanew_disk, &lfanew_disk);
    addr_t pe_offset = le_to_host(lfanew_disk);
    delete section;
    
    /* Read the PE File Header magic number */
    section = new SgAsmGenericSection(ef, NULL, pe_offset, 4);
    section->grab_content();
    unsigned char pe_magic[4];
    section->content(0, 4, pe_magic);
    delete section;
    section = NULL;

    /* Check the PE magic number */
    return 'P'==pe_magic[0] && 'E'==pe_magic[1] && '\0'==pe_magic[2] && '\0'==pe_magic[3];
}

/** Initialize the header with information parsed from the file and construct and parse everything that's reachable from the
 *  header. The PE File Header should have been constructed such that SgAsmPEFileHeader::ctor() was called. */
SgAsmPEFileHeader*
SgAsmPEFileHeader::parse()
{
    SgAsmGenericHeader::parse();
    
    /* Read header */
    PEFileHeader_disk fh;
    if (sizeof(fh)>get_size())
        extend(sizeof(fh)-get_size());
    content(0, sizeof fh, &fh);

    /* Check magic number before getting too far */
    if (fh.e_magic[0]!='P' || fh.e_magic[1]!='E' || fh.e_magic[2]!='\0' || fh.e_magic[3]!='\0')
        throw FormatError("Bad PE magic number");

    /* Decode COFF file header */
    p_e_cpu_type           = le_to_host(fh.e_cpu_type);
    p_e_nsections          = le_to_host(fh.e_nsections);
    p_e_time               = le_to_host(fh.e_time);
    p_e_coff_symtab        = le_to_host(fh.e_coff_symtab);
    p_e_coff_nsyms         = le_to_host(fh.e_coff_nsyms);
    p_e_nt_hdr_size        = le_to_host(fh.e_nt_hdr_size);
    p_e_flags              = le_to_host(fh.e_flags);

    /* Read the "Optional Header" (optional in the sense that not all files have one, but required for an executable), the
     * size of which is stored in the e_nt_hdr_size of the main PE file header. According to 
     * http://www.phreedom.org/solar/code/tinype the Windows loader honors the e_nt_hdr_size even when set to smaller than the
     * smallest possible documented size of the optional header. Also it's possible for the optional header to extend beyond
     * the end of the file, in which case that part should be read as zero. */
    PE32OptHeader_disk oh32;
    memset(&oh32, 0, sizeof oh32);
    addr_t need32 = std::min(p_e_nt_hdr_size, (addr_t)(sizeof oh32));
    extend(need32);
    content(sizeof fh, sizeof oh32, &oh32);
    p_e_opt_magic = le_to_host(oh32.e_opt_magic);
    
    /* File format changes from ctor() */
    p_exec_format->set_purpose(p_e_flags & HF_PROGRAM ? PURPOSE_EXECUTABLE : PURPOSE_LIBRARY);
    p_exec_format->set_word_size(0x010b==p_e_opt_magic? 4 : 8);

    /* Decode the optional header. */
    addr_t entry_rva;
    if (4==p_exec_format->get_word_size()) {
        p_e_lmajor             = le_to_host(oh32.e_lmajor);
        p_e_lminor             = le_to_host(oh32.e_lminor);
        p_e_code_size          = le_to_host(oh32.e_code_size);
        p_e_data_size          = le_to_host(oh32.e_data_size);
        p_e_bss_size           = le_to_host(oh32.e_bss_size);
        entry_rva              = le_to_host(oh32.e_entrypoint_rva);
        p_e_code_rva           = le_to_host(oh32.e_code_rva);
        p_e_data_rva           = le_to_host(oh32.e_data_rva);
        p_base_va              = le_to_host(oh32.e_image_base);
        p_e_section_align      = le_to_host(oh32.e_section_align);
        p_e_file_align         = le_to_host(oh32.e_file_align);
        p_e_os_major           = le_to_host(oh32.e_os_major);
        p_e_os_minor           = le_to_host(oh32.e_os_minor);
        p_e_user_major         = le_to_host(oh32.e_user_major);
        p_e_user_minor         = le_to_host(oh32.e_user_minor);
        p_e_subsys_major       = le_to_host(oh32.e_subsys_major);
        p_e_subsys_minor       = le_to_host(oh32.e_subsys_minor);
        p_e_reserved9          = le_to_host(oh32.e_reserved9);
        p_e_image_size         = le_to_host(oh32.e_image_size);
        p_e_header_size        = le_to_host(oh32.e_header_size);
        p_e_file_checksum      = le_to_host(oh32.e_file_checksum);
        p_e_subsystem          = le_to_host(oh32.e_subsystem);
        p_e_dll_flags          = le_to_host(oh32.e_dll_flags);
        p_e_stack_reserve_size = le_to_host(oh32.e_stack_reserve_size);
        p_e_stack_commit_size  = le_to_host(oh32.e_stack_commit_size);
        p_e_heap_reserve_size  = le_to_host(oh32.e_heap_reserve_size);
        p_e_heap_commit_size   = le_to_host(oh32.e_heap_commit_size);
        p_e_loader_flags       = le_to_host(oh32.e_loader_flags);
        p_e_num_rvasize_pairs  = le_to_host(oh32.e_num_rvasize_pairs);
    } else if (8==p_exec_format->get_word_size()) {
        /* We guessed wrong. This is a 64-bit header, not 32-bit. */
        PE64OptHeader_disk oh64;
        memset(&oh64, 0, sizeof oh64);
        addr_t need64 = std::min(p_e_nt_hdr_size, (addr_t)(sizeof oh64));
        extend(need64 - need32);
        content(sizeof fh, sizeof oh64, &oh64);
        p_e_lmajor             = le_to_host(oh64.e_lmajor);
        p_e_lminor             = le_to_host(oh64.e_lminor);
        p_e_code_size          = le_to_host(oh64.e_code_size);
        p_e_data_size          = le_to_host(oh64.e_data_size);
        p_e_bss_size           = le_to_host(oh64.e_bss_size);
        entry_rva              = le_to_host(oh64.e_entrypoint_rva);
        p_e_code_rva           = le_to_host(oh64.e_code_rva);
     // p_e_data_rva           = le_to_host(oh.e_data_rva);             /* not in PE32+ */
        p_base_va              = le_to_host(oh64.e_image_base);
        p_e_section_align      = le_to_host(oh64.e_section_align);
        p_e_file_align         = le_to_host(oh64.e_file_align);
        p_e_os_major           = le_to_host(oh64.e_os_major);
        p_e_os_minor           = le_to_host(oh64.e_os_minor);
        p_e_user_major         = le_to_host(oh64.e_user_major);
        p_e_user_minor         = le_to_host(oh64.e_user_minor);
        p_e_subsys_major       = le_to_host(oh64.e_subsys_major);
        p_e_subsys_minor       = le_to_host(oh64.e_subsys_minor);
        p_e_reserved9          = le_to_host(oh64.e_reserved9);
        p_e_image_size         = le_to_host(oh64.e_image_size);
        p_e_header_size        = le_to_host(oh64.e_header_size);
        p_e_file_checksum      = le_to_host(oh64.e_file_checksum);
        p_e_subsystem          = le_to_host(oh64.e_subsystem);
        p_e_dll_flags          = le_to_host(oh64.e_dll_flags);
        p_e_stack_reserve_size = le_to_host(oh64.e_stack_reserve_size);
        p_e_stack_commit_size  = le_to_host(oh64.e_stack_commit_size);
        p_e_heap_reserve_size  = le_to_host(oh64.e_heap_reserve_size);
        p_e_heap_commit_size   = le_to_host(oh64.e_heap_commit_size);
        p_e_loader_flags       = le_to_host(oh64.e_loader_flags);
        p_e_num_rvasize_pairs  = le_to_host(oh64.e_num_rvasize_pairs);
    } else {
        throw FormatError("unrecognized Windows PE optional header magic number");
    }

    /* Magic number */
    p_magic.clear();
    for (size_t i = 0; i < sizeof(fh.e_magic); ++i)
        p_magic.push_back(fh.e_magic[i]);

    /* File format */
    ROSE_ASSERT(p_e_lmajor <= 0xffff && p_e_lminor <= 0xffff);
    p_exec_format->set_version((p_e_lmajor << 16) | p_e_lminor);
    p_exec_format->set_is_current_version(true); /*FIXME*/

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
        fprintf(stderr, "Warning: SgAsmPEFileHeader::ctor::p_e_cputype = 0x%x (%u)\n", p_e_cpu_type, p_e_cpu_type);
        set_isa(ISA_OTHER);
        break;
    }

    /* Entry point. We will eventually bind the entry point to a particular section (in SgAsmPEFileHeader::parse) so that if
     * sections are rearranged, extended, etc. the entry point will be updated automatically. */
    add_entry_rva(entry_rva);

    /* The PE File Header has a fixed-size component followed by some number of RVA/Size pairs. The add_rvasize_pairs() will
     * extend  the header and parse the RVA/Size pairs. */
    ROSE_ASSERT(get_e_num_rvasize_pairs() < 1000); /* just a sanity check before we allocate memory */
    add_rvasize_pairs();

    /* Construct the section table and its sections (non-synthesized sections). The specification says that the section table
     * comes after the optional (NT) header, which in turn comes after the fixed part of the PE header. The size of the
     * optional header is indicated in the fixed header. */
    addr_t secttab_offset = get_offset() + sizeof(PEFileHeader_disk) + get_e_nt_hdr_size();
    addr_t secttab_size = get_e_nsections() * sizeof(SgAsmPESectionTableEntry::PESectionTableEntry_disk);
    SgAsmPESectionTable *secttab = new SgAsmPESectionTable(this);
    secttab->set_offset(secttab_offset);
    secttab->set_size(secttab_size);
    secttab->parse();
    set_section_table(secttab);

    /* Parse the COFF symbol table and add symbols to the PE header */
    if (get_e_coff_symtab() && get_e_coff_nsyms()) {
        SgAsmCoffSymbolTable *symtab = (new SgAsmCoffSymbolTable(this))->parse();
        std::vector<SgAsmCoffSymbol*> & symbols = symtab->get_symbols()->get_symbols();
        for (size_t i = 0; i < symbols.size(); i++)
            add_symbol(symbols[i]);
        set_coff_symtab(symtab);
    }

    /* Associate RVAs with particular sections. */
    ROSE_ASSERT(get_entry_rvas().size()==1);
    get_entry_rvas()[0].bind(this);
    set_e_code_rva(get_e_code_rva().bind(this));
    set_e_data_rva(get_e_data_rva().bind(this));

    /* Turn header-specified tables (RVA/Size pairs) into generic sections */
    create_table_sections();
    return this;
}

SgAsmPEFileHeader::~SgAsmPEFileHeader() 
{
    ROSE_ASSERT(p_rvasize_pairs->get_pairs().empty() == true);
    // Delete the pointers to the IR nodes containing the STL lists
    delete p_rvasize_pairs;
    p_rvasize_pairs = NULL;
}

/* Encode the PE header into disk format */
void *
SgAsmPEFileHeader::encode(PEFileHeader_disk *disk) const
{
    for (size_t i=0; i<NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    host_to_le(p_e_cpu_type,           &(disk->e_cpu_type));
    host_to_le(p_e_nsections,          &(disk->e_nsections));
    host_to_le(p_e_time,               &(disk->e_time));
    host_to_le(p_e_coff_symtab,        &(disk->e_coff_symtab));
    host_to_le(p_e_coff_nsyms,         &(disk->e_coff_nsyms));
    host_to_le(p_e_nt_hdr_size,        &(disk->e_nt_hdr_size));
    host_to_le(p_e_flags,              &(disk->e_flags));

    return disk;
}
void *
SgAsmPEFileHeader::encode(PE32OptHeader_disk *disk) const
{
    host_to_le(p_e_opt_magic,          &(disk->e_opt_magic));
    host_to_le(p_e_lmajor,             &(disk->e_lmajor));
    host_to_le(p_e_lminor,             &(disk->e_lminor));
    host_to_le(p_e_code_size,          &(disk->e_code_size));
    host_to_le(p_e_data_size,          &(disk->e_data_size));
    host_to_le(p_e_bss_size,           &(disk->e_bss_size));
    host_to_le(get_entry_rva(),        &(disk->e_entrypoint_rva));
    host_to_le(p_e_code_rva,           &(disk->e_code_rva));
    host_to_le(p_e_data_rva,           &(disk->e_data_rva));
    host_to_le(get_base_va(),          &(disk->e_image_base));
    host_to_le(p_e_section_align,      &(disk->e_section_align));
    host_to_le(p_e_file_align,         &(disk->e_file_align));
    host_to_le(p_e_os_major,           &(disk->e_os_major));
    host_to_le(p_e_os_minor,           &(disk->e_os_minor));
    host_to_le(p_e_user_major,         &(disk->e_user_major));
    host_to_le(p_e_user_minor,         &(disk->e_user_minor));
    host_to_le(p_e_subsys_major,       &(disk->e_subsys_major));
    host_to_le(p_e_subsys_minor,       &(disk->e_subsys_minor));
    host_to_le(p_e_reserved9,          &(disk->e_reserved9));
    host_to_le(p_e_image_size,         &(disk->e_image_size));
    host_to_le(p_e_header_size,        &(disk->e_header_size));
    host_to_le(p_e_file_checksum,      &(disk->e_file_checksum));
    host_to_le(p_e_subsystem,          &(disk->e_subsystem));
    host_to_le(p_e_dll_flags,          &(disk->e_dll_flags));
    host_to_le(p_e_stack_reserve_size, &(disk->e_stack_reserve_size));
    host_to_le(p_e_stack_commit_size,  &(disk->e_stack_commit_size));
    host_to_le(p_e_heap_reserve_size,  &(disk->e_heap_reserve_size));
    host_to_le(p_e_heap_commit_size,   &(disk->e_heap_commit_size));
    host_to_le(p_e_loader_flags,       &(disk->e_loader_flags));
    host_to_le(p_e_num_rvasize_pairs,  &(disk->e_num_rvasize_pairs));

    return disk;
}
void *
SgAsmPEFileHeader::encode(PE64OptHeader_disk *disk) const
{
    host_to_le(p_e_opt_magic,          &(disk->e_opt_magic));
    host_to_le(p_e_lmajor,             &(disk->e_lmajor));
    host_to_le(p_e_lminor,             &(disk->e_lminor));
    host_to_le(p_e_code_size,          &(disk->e_code_size));
    host_to_le(p_e_data_size,          &(disk->e_data_size));
    host_to_le(p_e_bss_size,           &(disk->e_bss_size));
    host_to_le(get_entry_rva(),        &(disk->e_entrypoint_rva));
    host_to_le(p_e_code_rva,           &(disk->e_code_rva));
 // host_to_le(p_e_data_rva,           &(disk->e_data_rva)); /* not present in PE32+ */
    host_to_le(get_base_va(),          &(disk->e_image_base));
    host_to_le(p_e_section_align,      &(disk->e_section_align));
    host_to_le(p_e_file_align,         &(disk->e_file_align));
    host_to_le(p_e_os_major,           &(disk->e_os_major));
    host_to_le(p_e_os_minor,           &(disk->e_os_minor));
    host_to_le(p_e_user_major,         &(disk->e_user_major));
    host_to_le(p_e_user_minor,         &(disk->e_user_minor));
    host_to_le(p_e_subsys_major,       &(disk->e_subsys_major));
    host_to_le(p_e_subsys_minor,       &(disk->e_subsys_minor));
    host_to_le(p_e_reserved9,          &(disk->e_reserved9));
    host_to_le(p_e_image_size,         &(disk->e_image_size));
    host_to_le(p_e_header_size,        &(disk->e_header_size));
    host_to_le(p_e_file_checksum,      &(disk->e_file_checksum));
    host_to_le(p_e_subsystem,          &(disk->e_subsystem));
    host_to_le(p_e_dll_flags,          &(disk->e_dll_flags));
    host_to_le(p_e_stack_reserve_size, &(disk->e_stack_reserve_size));
    host_to_le(p_e_stack_commit_size,  &(disk->e_stack_commit_size));
    host_to_le(p_e_heap_reserve_size,  &(disk->e_heap_reserve_size));
    host_to_le(p_e_heap_commit_size,   &(disk->e_heap_commit_size));
    host_to_le(p_e_loader_flags,       &(disk->e_loader_flags));
    host_to_le(p_e_num_rvasize_pairs,  &(disk->e_num_rvasize_pairs));

    return disk;
}
    
/* Adds the RVA/Size pairs to the end of the PE file header */
void
SgAsmPEFileHeader::add_rvasize_pairs()
{
    addr_t pairs_offset = get_size();
    addr_t pairs_size   = p_e_num_rvasize_pairs * sizeof(SgAsmPERVASizePair::RVASizePair_disk);
    SgAsmPERVASizePair::RVASizePair_disk pairs_disk;

    ROSE_ASSERT(p_rvasize_pairs != NULL);
    ROSE_ASSERT(p_rvasize_pairs->get_pairs().size()==0);
    p_rvasize_pairs->set_isModified(true);

    extend(pairs_size);
    for (size_t i = 0; i < p_e_num_rvasize_pairs; i++, pairs_offset += sizeof pairs_disk) {
        content(pairs_offset, sizeof pairs_disk, &pairs_disk);
        p_rvasize_pairs->get_pairs().push_back(new SgAsmPERVASizePair(&pairs_disk));
        p_rvasize_pairs->get_pairs().back()->set_parent(p_rvasize_pairs);
    }
}

/* Looks at the RVA/Size pairs in the PE header and creates an SgAsmGenericSection object for each one. We have to do this
 * near the end of parsing because RVA/Size pairs have only memory info and no file offsets -- we need to have parsed the
 * other sections so we can determine the file offset of each table. */
void
SgAsmPEFileHeader::create_table_sections()
{
    getpid();

    for (size_t i=0; i<p_rvasize_pairs->get_pairs().size(); i++) {
        SgAsmPERVASizePair *pair = p_rvasize_pairs->get_pairs()[i];
        if (0==pair->get_e_size())
            continue;

        /* Table names come from PE file specification and are hard coded by RVA/Size pair index */
        const char *tabname = NULL;
        switch (i) {
          case 0:  tabname = "Export Table";                 break;
          case 1:  tabname = "Import Table";                 break;
          case 2:  tabname = "Resource Table";               break;
          case 3:  tabname = "Exception Table";              break;
          case 4:  tabname = "Certificate Table";            break;
          case 5:  tabname = "Base relocation Table";        break;
          case 6:  tabname = "Debug";                        break;
          case 7:  tabname = "Architecture";                 break;
          case 8:  tabname = "Global Ptr";                   break;
          case 9:  tabname = "TLS Table";                    break;
          case 10: tabname = "Load Config Table";            break;
          case 11: tabname = "Bound Import";                 break;
          case 12: tabname = "Import Address Table";         break;
          case 13: tabname = "Delay Import Descriptor";      break;
          case 14: tabname = "CLR Runtime Header";           break;
          case 15: ROSE_ASSERT(!"reserved; should be zero"); break;
          default: ROSE_ASSERT(!"too many RVA/Size pairs");  break;
        }

        /* Find a section that contains the starting RVA of the table. We use that to find the file offset. */
        SgAsmGenericFile *ef = get_file();
        SgAsmGenericSection *contained_in = ef->get_section_by_va(get_base_va()+pair->get_e_rva());
        if (!contained_in) {
            fprintf(stderr, "SgAsmPEFileHeader::create_table_sections(): pair-%zu, rva=0x%08"PRIx64", size=%"PRIu64" bytes \"%s\""
                    ": unable to find a section containing the virtual address (skipping)\n",
                    i, pair->get_e_rva().get_rva(), pair->get_e_size(), tabname?tabname:"");
            continue;
        }
        addr_t file_offset = contained_in->get_rva_offset(pair->get_e_rva().get_rva());

        /* Create the new section */
        SgAsmGenericSection *tabsec = NULL;
        switch (i) {
          case 0:
            tabsec = new SgAsmPEExportSection(this);
            break;
          case 1:
            tabsec = new SgAsmPEImportSection(this);
            break;
          default:
            tabsec = new SgAsmGenericSection(ef, this, file_offset, pair->get_e_size());
            break;
        }
        if (tabname) tabsec->set_name(new SgAsmBasicString(tabname));
        tabsec->set_synthesized(true);
        tabsec->set_purpose(SP_HEADER);

        tabsec->set_offset(file_offset);
        tabsec->set_size(pair->get_e_size());
        tabsec->set_file_alignment(get_e_file_align());

        tabsec->set_mapped_alignment(get_e_section_align());
        tabsec->set_mapped_rva(pair->get_e_rva().get_rva());
        tabsec->set_mapped_size(pair->get_e_size());
        tabsec->set_mapped_rperm(true);
        tabsec->set_mapped_wperm(false);
        tabsec->set_mapped_xperm(false);
        tabsec->parse();
        pair->set_section(tabsec);
        pair->set_e_rva(pair->get_e_rva().set_section(tabsec));
    }
}

/* Change size of PE header based on word size */
bool
SgAsmPEFileHeader::reallocate()
{
    bool reallocated = SgAsmGenericHeader::reallocate();
    
    /* Resize if necessary */
    addr_t need = sizeof(PEFileHeader_disk);
    if (4==get_word_size()) {
        need += sizeof(PE32OptHeader_disk);
    } else if (8==get_word_size()) {
        need += sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    need += p_rvasize_pairs->get_pairs().size() * sizeof(SgAsmPERVASizePair::RVASizePair_disk);
    if (need<get_size()) {
        if (is_mapped()) {
            ROSE_ASSERT(get_mapped_size()==get_size());
            set_mapped_size(need);
        }
        set_size(need);
        reallocated = true;
    } else if (need>get_size()) {
        get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    /* Make sure header is consistent with sections. Reallocate() has already been called recursively for the sections.
     * Count the number of sections in the table and update the header's e_nsections member. */
    if (p_section_table) {
        ROSE_ASSERT(p_section_table->get_header()==this);
        SgAsmGenericSectionList *all = get_sections();
        p_e_nsections = 0;
        for (size_t i=0; i<all->get_sections().size(); i++) {
            SgAsmPESection *pesec = dynamic_cast<SgAsmPESection*>(all->get_sections()[i]);
            if (pesec && pesec->get_st_entry()!=NULL)
                p_e_nsections++;
        }

        addr_t header_size = ALIGN(p_section_table->get_offset() + p_section_table->get_size(),
                                   p_e_file_align>0 ? p_e_file_align : 1);
#if 1
        /* The PE Specification regarding e_header_size (known as "SizeOfHeader" on page 14 of "Microsoft Portable Executable
         * and Common Object File Format Specification: Revision 8.1 February 15, 2008" is not always followed. We recompute
         * it here as being the minimum RVA from all the sections defined in the PE Section Table, but not smaller
         * than the value according to the specification. This alternate value is kept if it's already in the parse tree,
         * otherwise we use the correct value. (RPM 2008-10-21) */
        addr_t min_offset;
        for (size_t i=0, nfound=0; i<all->get_sections().size(); i++) {
            SgAsmPESection *pesec = dynamic_cast<SgAsmPESection*>(all->get_sections()[i]);
            if (pesec && pesec->get_st_entry()!=NULL) {
                if (0==nfound++) {
                    min_offset = pesec->get_offset();
                } else {
                    min_offset = std::min(min_offset, pesec->get_offset());
                }
            }
        }
        addr_t header_size2 = std::max(header_size, min_offset);
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
            
    /* Update COFF symbol table related data members in the file header */
    if (get_coff_symtab()) {
        ROSE_ASSERT(get_coff_symtab()->get_header()==this);
        set_e_coff_symtab(get_coff_symtab()->get_offset());
        set_e_coff_nsyms(get_coff_symtab()->get_nslots());
    }

    /* Update some additional header fields */
    set_e_num_rvasize_pairs(get_rvasize_pairs()->get_pairs().size());
    set_e_opt_magic(4==get_word_size() ? 0x010b : 0x020b);
    set_e_lmajor((get_exec_format()->get_version() >> 16) & 0xffff);
    set_e_lminor(get_exec_format()->get_version() & 0xffff);

    /* Adjust the COFF Header's e_nt_hdr_size to accommodate the NT Optional Header in such a way that EXEs from tinype.com
     * don't change (i.e., don't increase e_nt_hdr_size if the bytes beyond it are zero anyway, and if they aren't then adjust
     * it as little as possible.  The RVA/Size pairs are considered to be part of the NT Optional Header. */
    size_t oh_size = p_rvasize_pairs->get_pairs().size() * sizeof(SgAsmPERVASizePair::RVASizePair_disk);
    size_t rvasize_offset; /*offset with respect to "oh" buffer allocated below*/
    if (4==get_word_size()) {
        oh_size += sizeof(PE32OptHeader_disk);
    } else if (8==get_word_size()) {
        oh_size += sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    unsigned char *oh = new unsigned char[oh_size];
    if (4==get_word_size()) {
        encode((PE32OptHeader_disk*)oh);
        rvasize_offset = sizeof(PE32OptHeader_disk);
    } else if (8==get_word_size()) {
        encode((PE64OptHeader_disk*)oh);
        rvasize_offset = sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    while (oh_size>p_e_nt_hdr_size) {
        if (0!=oh[oh_size-1]) break;
        --oh_size;
    }
    set_e_nt_hdr_size(oh_size);
    return reallocated;
}

/* Write the PE file header back to disk and all that it references */
void
SgAsmPEFileHeader::unparse(std::ostream &f) const
{
    /* Write unreferenced areas back to the file before anything else. */
    unparse_holes(f);
    
    /* Write the PE section table and, indirectly, the sections themselves. */
    if (p_section_table)
        p_section_table->unparse(f);

    /* Write sections that are pointed to by the file header */
    if (p_coff_symtab) {
        ROSE_ASSERT(p_e_coff_symtab == p_coff_symtab->get_offset());
        ROSE_ASSERT(p_e_coff_nsyms == p_coff_symtab->get_nslots());
        p_coff_symtab->unparse(f);
    }
    
    /* Write the sections from the header RVA/size pair table. */
    for (size_t i=0; i<p_rvasize_pairs->get_pairs().size(); i++) {
        SgAsmGenericSection *sizepair_section = p_rvasize_pairs->get_pairs()[i]->get_section();
        if (sizepair_section)
            sizepair_section->unparse(f);
    }

    /* Encode the "NT Optional Header" before the COFF Header since the latter depends on the former. Adjust the COFF Header's
     * e_nt_hdr_size to accommodate the NT Optional Header in such a way that EXEs from tinype.com don't change (i.e., don't
     * increase e_nt_hdr_size if the bytes beyond it are zero anyway, and if they aren't then adjust it as little as possible.
     * The RVA/Size pairs are considered to be part of the NT Optional Header. */
    size_t oh_size = p_rvasize_pairs->get_pairs().size() * sizeof(SgAsmPERVASizePair::RVASizePair_disk);
    size_t rvasize_offset; /*offset with respect to "oh" buffer allocated below*/
    if (4==get_word_size()) {
        oh_size += sizeof(PE32OptHeader_disk);
    } else if (8==get_word_size()) {
        oh_size += sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    unsigned char *oh = new unsigned char[oh_size];
    if (4==get_word_size()) {
        encode((PE32OptHeader_disk*)oh);
        rvasize_offset = sizeof(PE32OptHeader_disk);
    } else if (8==get_word_size()) {
        encode((PE64OptHeader_disk*)oh);
        rvasize_offset = sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }
    for (size_t i=0; i<p_rvasize_pairs->get_pairs().size(); i++, rvasize_offset+=sizeof(SgAsmPERVASizePair::RVASizePair_disk)) {
        SgAsmPERVASizePair::RVASizePair_disk *rvasize_disk = (SgAsmPERVASizePair::RVASizePair_disk*)(oh+rvasize_offset);
        p_rvasize_pairs->get_pairs()[i]->encode(rvasize_disk);
    }
    while (oh_size>p_e_nt_hdr_size) {
        if (0!=oh[oh_size-1]) break;
        --oh_size;
    }
    ROSE_ASSERT(p_e_nt_hdr_size==oh_size); /*set in reallocate()*/

    /* Write the fixed-length COFF Header */
    PEFileHeader_disk fh;
    encode(&fh);
    addr_t spos = write(f, 0, sizeof fh, &fh);

    /* Write the following "NT Optional Header" */
    spos = write(f, spos, oh_size, oh);
}
    
/* Print some debugging information */
void
SgAsmPEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEFileHeader.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    time_t t = p_e_time;
    char time_str[128];
    strftime(time_str, sizeof time_str, "%c", localtime(&t));

    SgAsmGenericHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = 0x%04x (%u)\n",               p, w, "e_cpu_type",          p_e_cpu_type, p_e_cpu_type);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nsections",         p_e_nsections);
    fprintf(f, "%s%-*s = %u (%s)\n",                   p, w, "e_time",              p_e_time, time_str);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, "e_coff_symtab",       p_e_coff_symtab, p_e_coff_symtab);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_coff_nsyms",        p_e_coff_nsyms);
    if (p_coff_symtab) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n",           p, w, "coff_symtab",
                p_coff_symtab->get_id(), p_coff_symtab->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",                  p, w, "coff_symtab");
    }
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, "e_nt_hdr_size",       p_e_nt_hdr_size, p_e_nt_hdr_size);
    fprintf(f, "%s%-*s = 0x%04x (%u)\n",               p, w, "e_flags",             p_e_flags, p_e_flags);
    fprintf(f, "%s%-*s = 0x%04x %s\n",                 p, w, "e_opt_magic",         p_e_opt_magic,
            0x10b == p_e_opt_magic ? "PE32" : (0x20b == p_e_opt_magic ? "PE32+" : "other"));
    fprintf(f, "%s%-*s = %u.%u\n",                     p, w, "linker_vers",         p_e_lmajor, p_e_lminor);
    fprintf(f, "%s%-*s = 0x%08x (%u) bytes\n",         p, w, "e_code_size",         p_e_code_size, p_e_code_size);
    fprintf(f, "%s%-*s = 0x%08x (%u) bytes\n",         p, w, "e_data_size",         p_e_data_size, p_e_data_size);
    fprintf(f, "%s%-*s = 0x%08x (%u) bytes\n",         p, w, "e_bss_size",          p_e_bss_size, p_e_bss_size);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "e_code_rva",          p_e_code_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "e_data_rva",          p_e_data_rva.to_string().c_str());
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
    for (unsigned i = 0; i < p_rvasize_pairs->get_pairs().size(); i++) {
        sprintf(p, "%sPEFileHeader.pair[%d].", prefix, i);
        w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
        fprintf(f, "%s%-*s = rva %s,\tsize 0x%08"PRIx64" (%"PRIu64")\n", p, w, "..",
                p_rvasize_pairs->get_pairs()[i]->get_e_rva().to_string().c_str(),
                p_rvasize_pairs->get_pairs()[i]->get_e_size(), p_rvasize_pairs->get_pairs()[i]->get_e_size());
    }
    if (p_section_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "section_table",
                p_section_table->get_id(), p_section_table->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "section_table");
    }

    if (variantT() == V_SgAsmPEFileHeader) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Section Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SgAsmPESectionTableEntry::ctor(const PESectionTableEntry_disk *disk)
{
    char name[9];
    strncpy(name, disk->name, 8);
    name[8] = '\0';
    this->set_name( name );

    /* Decode file format */
    p_virtual_size     = le_to_host(disk->virtual_size);
    p_rva              = le_to_host(disk->rva);
    p_physical_size    = le_to_host(disk->physical_size);
    p_physical_offset  = le_to_host(disk->physical_offset);
    p_coff_line_nums   = le_to_host(disk->coff_line_nums);
    p_n_relocs         = le_to_host(disk->n_relocs);
    p_n_coff_line_nums = le_to_host(disk->n_coff_line_nums);
    p_flags            = le_to_host(disk->flags);
}

/* Encodes a section table entry back into disk format. */
void *
SgAsmPESectionTableEntry::encode(PESectionTableEntry_disk *disk) const
{
    memset(disk->name, 0, sizeof(disk->name));

 // DQ: Not clear if this is the correct translation of the call to use std::string
 // memcpy(disk->name, p_name.c_str(), std::min(sizeof(name), name.size()));
 // memcpy(disk->name, p_name.c_str(), p_name.size(), p_name.size());
    memcpy(disk->name, p_name.c_str(), p_name.size());

 // printf ("Figure out the correct translation later! \n");
 // ROSE_ASSERT(false);

    host_to_le(p_virtual_size,     &(disk->virtual_size));
    host_to_le(p_rva,              &(disk->rva));
    host_to_le(p_physical_size,    &(disk->physical_size));
    host_to_le(p_physical_offset,  &(disk->physical_offset));
    host_to_le(p_coff_line_nums,   &(disk->coff_line_nums));
    host_to_le(p_n_relocs,         &(disk->n_relocs));
    host_to_le(p_n_coff_line_nums, &(disk->n_coff_line_nums));
    host_to_le(p_flags,            &(disk->flags));

    return disk;
}

/* Prints some debugging info */
void
SgAsmPESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",            p, w, "virtual_size",     p_virtual_size);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",              p, w, "rva",              p_rva);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",            p, w, "physical_size",    p_physical_size);
    fprintf(f, "%s%-*s = %" PRIu64 " file byte offset\n", p, w, "physical_offset",  p_physical_offset);
    fprintf(f, "%s%-*s = %u byte offset\n",               p, w, "coff_line_nums",   p_coff_line_nums);
    fprintf(f, "%s%-*s = %u\n",                           p, w, "n_relocs",         p_n_relocs);
    fprintf(f, "%s%-*s = %u\n",                           p, w, "n_coff_line_nums", p_n_coff_line_nums);
    fprintf(f, "%s%-*s = 0x%08x\n",                       p, w, "flags",            p_flags);
}

/* Print some debugging info. */
void
SgAsmPESection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESection.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);
    if (p_st_entry)
        p_st_entry->dump(f, p, -1);

    if (variantT() == V_SgAsmPESection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

/* Constructor */
void
SgAsmPESectionTable::ctor()
{
    set_synthesized(true);
    set_name(new SgAsmBasicString("PE Section Table"));
    set_purpose(SP_HEADER);
}

/* Parser */
SgAsmPESectionTable*
SgAsmPESectionTable::parse()
{
    SgAsmGenericSection::parse();

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    const size_t entsize = sizeof(SgAsmPESectionTableEntry::PESectionTableEntry_disk);
    for (size_t i=0; i<fhdr->get_e_nsections(); i++) {
        /* Parse the section table entry */
        SgAsmPESectionTableEntry::PESectionTableEntry_disk disk;
        content(i * entsize, entsize, &disk);
        SgAsmPESectionTableEntry *entry = new SgAsmPESectionTableEntry(&disk);

        /* The section */
        SgAsmPESection *section = NULL;
        if (entry->get_name() == ".idata") {
            section = new SgAsmPEImportSection(fhdr);
        } else {
            section = new SgAsmPESection(fhdr);
        }
        section->set_synthesized(false);
        section->set_name(new SgAsmBasicString(entry->get_name()));
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);

        section->set_offset(entry->get_physical_offset());
        section->set_size(entry->get_physical_size());
        section->set_file_alignment(fhdr->get_e_file_align());

        section->set_mapped_rva(entry->get_rva());
        section->set_mapped_size(entry->get_virtual_size());
        section->set_mapped_alignment(fhdr->get_e_section_align());
        section->set_st_entry(entry);
        section->set_mapped_rperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_READABLE)
                                  == SgAsmPESectionTableEntry::OF_READABLE);
        section->set_mapped_wperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_WRITABLE)
                                  == SgAsmPESectionTableEntry::OF_WRITABLE);
        section->set_mapped_xperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_EXECUTABLE)
                                  == SgAsmPESectionTableEntry::OF_EXECUTABLE);
        
        if (entry->get_flags() & (SgAsmPESectionTableEntry::OF_CODE|
                                  SgAsmPESectionTableEntry::OF_IDATA|
                                  SgAsmPESectionTableEntry::OF_UDATA)) {
            section->set_purpose(SP_PROGRAM);
        }
        section->parse();
    }
    return this;
}

/* Writes the section table back to disk along with each of the sections. */
void
SgAsmPESectionTable::unparse(std::ostream &f) const
{
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr != NULL);
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();

    for (size_t i = 0; i < sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            SgAsmPESection *section = dynamic_cast<SgAsmPESection*>(sections[i]);

            /* Write the table entry */
            ROSE_ASSERT(section->get_id() > 0); /*ID's are 1-origin in PE*/
            size_t slot = section->get_id() - 1;
            SgAsmPESectionTableEntry *shdr = section->get_st_entry();
            SgAsmPESectionTableEntry::PESectionTableEntry_disk disk;
            shdr->encode(&disk);
            write(f, slot*sizeof(disk), sizeof disk, &disk);

            /* Write the section */
            section->unparse(f);
        }
    }
}

/* Prints some debugging info */
void
SgAsmPESectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESectionTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    if (variantT() == V_SgAsmPESectionTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Import Directory (".idata" section)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmPEImportDirectory::ctor(SgAsmPEImportSection *section, size_t idx)
{
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(section->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    set_parent(section);

    size_t entry_size = sizeof(PEImportDirectory_disk);
    PEImportDirectory_disk disk, zero;
    memset(&zero, 0, sizeof zero);
    section->content(idx*entry_size, entry_size, &disk);

    if (0==memcmp(&disk, &zero, sizeof zero)) {
        p_idx = -1;
    } else {
        p_idx = idx;
    }
    
    p_ilt_rva         = le_to_host(disk.ilt_rva);
    p_time            = le_to_host(disk.time);
    p_forwarder_chain = le_to_host(disk.forwarder_chain);
    p_dll_name_rva    = le_to_host(disk.dll_name_rva);
    p_iat_rva         = le_to_host(disk.iat_rva);

    /* Bind RVAs to best sections */
    p_ilt_rva.bind(fhdr);
    p_dll_name_rva.bind(fhdr);
    p_iat_rva.bind(fhdr);

    if (p_dll_name_rva.get_section()) {
        p_dll_name = new SgAsmBasicString(p_dll_name_rva.get_section()->content_str(p_dll_name_rva.get_rel()));
    } else {
        p_dll_name = NULL;
    }
}

/* Encode a directory entry back into disk format */
void *
SgAsmPEImportDirectory::encode(PEImportDirectory_disk *disk) const
{
    host_to_le(p_ilt_rva.get_rva(),      &(disk->ilt_rva));
    host_to_le(p_time,                   &(disk->time));
    host_to_le(p_forwarder_chain,        &(disk->forwarder_chain));
    host_to_le(p_dll_name_rva.get_rva(), &(disk->dll_name_rva));
    host_to_le(p_iat_rva.get_rva(),      &(disk->iat_rva));
    return disk;
}

void
SgAsmPEImportDirectory::unparse(std::ostream &f, const SgAsmPEImportSection *section) const
{
    ROSE_ASSERT(get_idx()>=0);

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(section->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    if (p_dll_name_rva>0) {
        addr_t spos = p_dll_name_rva.get_section()->write(f, p_dll_name_rva.get_rel(), p_dll_name->get_string());
        p_dll_name_rva.get_section()->write(f, spos, '\0');
    }
    if (p_ilt)
        p_ilt->unparse(f, fhdr, p_ilt_rva);
    if (p_iat)
        p_iat->unparse(f, fhdr, p_iat_rva);
    
    PEImportDirectory_disk disk;
    encode(&disk);
    section->write(f, get_idx()*sizeof disk, sizeof disk, &disk);
}


/* Print debugging info */
void
SgAsmPEImportDirectory::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportDirectory[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportDirectory.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %s",          p, w, "dll_name_rva", p_dll_name_rva.to_string().c_str());
    if (p_dll_name)
        fprintf(f, " \"%s\"", p_dll_name->c_str());
    fputc('\n', f);

    fprintf(f, "%s%-*s = %s\n",          p, w, "ilt_rva", p_ilt_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",          p, w, "iat_rva", p_iat_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %lu %s",        p, w, "time", (unsigned long)p_time, ctime(&p_time));
    fprintf(f, "%s%-*s = 0x%08x (%u)\n", p, w, "forwarder_chain", p_forwarder_chain, p_forwarder_chain);
    if (p_ilt)
        p_ilt->dump(f, p, -1);
    if (p_iat)
        p_iat->dump(f, p, -1);
}

/* Construct an Import Lookup Table Entry or a Lookup Address Table Entry */
void
SgAsmPEImportILTEntry::ctor(SgAsmPEImportSection *isec, uint64_t ilt_word)
{
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(isec->get_header());
    ROSE_ASSERT(fhdr!=NULL);
    p_hnt_entry = NULL;

    /* Initialize */
    p_ordinal = 0;
    p_hnt_entry_rva = 0;
    p_hnt_entry = NULL;
    p_extra_bits = 0;
    p_bound_rva = 0;

    /* Masks for different word sizes */
    uint64_t ordmask;                                           /* if bit is set then ILT Entry is an Ordinal */
    uint64_t hnrvamask = 0x7fffffff;                            /* Hint/Name RVA mask (both word sizes use 31 bits) */
    if (4==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<31;
    } else if (8==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<63;
    } else {
        throw FormatError("unsupported PE word size");
    }
    
    if (ilt_word & ordmask) {
        /* Ordinal */
        p_entry_type = ILT_ORDINAL;
        p_ordinal = ilt_word & 0xffff;
        p_extra_bits = ilt_word & ~(ordmask|0xffff);
    } else if (0!=(ilt_word & ~hnrvamask) || NULL==fhdr->get_best_section_by_va((ilt_word&hnrvamask) + fhdr->get_base_va())) {
        /* Bound address */
        p_entry_type = ILT_BOUND_RVA;
        p_bound_rva = ilt_word;
        p_bound_rva.bind(fhdr);
    } else {
        /* Hint/Name Pair RVA */
        p_entry_type = ILT_HNT_ENTRY_RVA;
        p_hnt_entry_rva = ilt_word & hnrvamask;
        p_hnt_entry_rva.bind(fhdr);
    }
}

/* Encode the PE Import Lookup Table or PE Import Address Table object into a word. */
uint64_t
SgAsmPEImportILTEntry::encode(const SgAsmPEFileHeader *fhdr) const
{
    uint64_t w = 0;

    /* Masks for different word sizes */
    uint64_t ordmask;                                           /* if bit is set then ILT Entry is an Ordinal */
    if (4==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<31;
    } else if (8==fhdr->get_word_size()) {
        ordmask = (uint64_t)1<<63;
    } else {
        throw FormatError("unsupported PE word size");
    }
    
    switch (p_entry_type) {
      case ILT_ORDINAL:
        w |= ordmask | p_extra_bits | p_ordinal;
        break;
      case ILT_BOUND_RVA:
        w |= p_bound_rva.get_rva();
        break;
      case ILT_HNT_ENTRY_RVA:
        w |= p_hnt_entry_rva.get_rva();
        break;
    }
    return w;
}

void
SgAsmPEImportILTEntry::unparse(std::ostream &f, const SgAsmPEFileHeader *fhdr, rva_t rva, size_t idx) const
{
    ROSE_ASSERT(rva.get_section()!=NULL);
    uint64_t ilt_entry_word = encode(fhdr);
    if (4==fhdr->get_word_size()) {
        uint32_t ilt_entry_disk;
        host_to_le(ilt_entry_word, &ilt_entry_disk);
        rva.get_section()->write(f, rva.get_rel()+idx*4, 4, &ilt_entry_disk);
    } else if (8==fhdr->get_word_size()) {
        uint64_t ilt_entry_disk;
        host_to_le(ilt_entry_word, &ilt_entry_disk);
        rva.get_section()->write(f, rva.get_rel()+idx*8, 8, &ilt_entry_disk);
    } else {
        throw FormatError("unsupported PE word size");
    }

    if (p_hnt_entry)
        p_hnt_entry->unparse(f, p_hnt_entry_rva);
}

/* Print debugging info for an Import Lookup Table Entry or an Import Address Table Entry */
void
SgAsmPEImportILTEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sentry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sentry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    switch (p_entry_type) {
      case ILT_ORDINAL:
        fprintf(f, "%s%-*s = 0x%04x (%u)\n", p, w, "ordinal", p_ordinal, p_ordinal);
        break;
      case ILT_HNT_ENTRY_RVA:
        fprintf(f, "%s%-*s = %s\n", p, w, "hnt_entry_rva", p_hnt_entry_rva.to_string().c_str());
        break;
      case ILT_BOUND_RVA:
        fprintf(f, "%s%-*s = %s\n", p, w, "bound_rva", p_bound_rva.to_string().c_str());
        break;
      default:
        ROSE_ASSERT(!"PE Import Lookup Table entry type is not valid");
    }
    if (p_extra_bits)
        fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "extra_bits", p_extra_bits);
    if (p_hnt_entry)
        p_hnt_entry->dump(f, p, -1);
}

/* Constructor. rva is the address of the table and should be bound to a section (which may not necessarily be isec). This
 * C object represents one of two PE objects depending on the value of is_iat.
 *    true  => PE Import Address Table
 *    false => PE Import Lookup Table */
void
SgAsmPEImportLookupTable::ctor(SgAsmPEImportSection *isec, rva_t rva, size_t idir_idx, bool is_iat)
{
    ROSE_ASSERT(p_entries==NULL);
    p_entries = new SgAsmPEImportILTEntryList();
    p_entries->set_parent(this);
    p_is_iat = is_iat;
    const char *tname = is_iat ? "Import Address Table" : "Import Lookup Table";

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(isec->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Read the Import Lookup (or Address) Table, an array of 32 or 64 bit values, the last of which is zero */
    if (!rva.get_section()) {
        fprintf(stderr, "SgAsmPEImportSection::ctor: error: in PE Import Directory entry %zu "
                "%s RVA (0x%08"PRIx64") is not in the mapped address space.\n",
                idir_idx, tname, rva.get_rva());
        return;
    }
    
    if (rva.get_section()!=isec) {
        fprintf(stderr, "SgAsmPEImportSection::ctor: warning: %s RVA is outside PE Import Table\n", tname);
        fprintf(stderr, "        Import Directory Entry #%zu\n", idir_idx);
        fprintf(stderr, "        %s RVA is %s\n", tname, rva.to_string().c_str());
        fprintf(stderr, "        PE Import Table mapped from 0x%08"PRIx64" to 0x%08"PRIx64"\n", 
                isec->get_mapped_rva(), isec->get_mapped_rva()+isec->get_mapped_size());
    }

    for (size_t i=0; 1; i++) {
        uint64_t ilt_entry_word=0;
        if (4==fhdr->get_word_size()) {
            const uint32_t *ilt_entry_disk;
            ilt_entry_disk = (const uint32_t*)rva.get_section()->content(rva.get_rel(), sizeof ilt_entry_disk);
            ilt_entry_word = le_to_host(*ilt_entry_disk);
        } else if (8==fhdr->get_word_size()) {
            const uint64_t *ilt_entry_disk;
            ilt_entry_disk = (const uint64_t*)rva.get_section()->content(rva.get_rel(), sizeof ilt_entry_disk);
            ilt_entry_word = le_to_host(*ilt_entry_disk);
        } else {
            throw FormatError("unsupported PE word size");
        }
        rva.set_rva(rva.get_rva()+fhdr->get_word_size()); /*advance to next entry of table*/
        if (0==ilt_entry_word)
            break;

        SgAsmPEImportILTEntry *ilt_entry = new SgAsmPEImportILTEntry(isec, ilt_entry_word);
        add_ilt_entry(ilt_entry);

        if (SgAsmPEImportILTEntry::ILT_HNT_ENTRY_RVA==ilt_entry->get_entry_type()) {
            SgAsmPEImportHNTEntry *hnt_entry = new SgAsmPEImportHNTEntry(ilt_entry->get_hnt_entry_rva());
            ilt_entry->set_hnt_entry(hnt_entry);
            hnt_entry->set_parent(ilt_entry);
        }
    }
}

/* Adds another Import Lookup Table Entry or Import Address Table Entry to the Import Lookup Table */
void
SgAsmPEImportLookupTable::add_ilt_entry(SgAsmPEImportILTEntry *ilt_entry)
{
    ROSE_ASSERT(p_entries!=NULL);
    ROSE_ASSERT(ilt_entry);
    p_entries->set_isModified(true);
    p_entries->get_vector().push_back(ilt_entry);
    ROSE_ASSERT(p_entries->get_vector().size()>0);
    ilt_entry->set_parent(this);
}

void
SgAsmPEImportLookupTable::unparse(std::ostream &f, const SgAsmPEFileHeader *fhdr, rva_t rva) const
{
    if (rva!=0) {
        //const char *tname = p_is_iat ? "Import Address Table" : "Import Lookup Table";
        for (size_t i=0; i<p_entries->get_vector().size(); i++) {
            SgAsmPEImportILTEntry *ilt_entry = p_entries->get_vector()[i];
            ilt_entry->unparse(f, fhdr, rva, i);
        }

        /* Zero terminated */
        uint64_t zero = 0;
        ROSE_ASSERT(fhdr->get_word_size()<=sizeof zero);
        addr_t spos = rva.get_rel() + p_entries->get_vector().size() * fhdr->get_word_size();
        rva.get_section()->write(f, spos, fhdr->get_word_size(), &zero);
    }
}

/* Print some debugging info for an Import Lookup Table or Import Address Table */
void
SgAsmPEImportLookupTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    prefix = "    ...";
    const char *tabbr = p_is_iat ? "IAT" : "ILT";
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s%s[%zd].", prefix, tabbr, idx);
    } else {
        sprintf(p, "%s%s.", prefix, tabbr);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %zu\n", p, w, "nentries", p_entries->get_vector().size());
    for (size_t i=0; i<p_entries->get_vector().size(); i++) {
        SgAsmPEImportILTEntry *ilt_entry = p_entries->get_vector()[i];
        ilt_entry->dump(f, p, i);
    }
}

/* Constructor */
void
SgAsmPEImportHNTEntry::ctor(rva_t rva)
{
    ROSE_ASSERT(rva.get_rva() % 2 == 0);
    ROSE_ASSERT(rva.get_section());

    const uint16_t *hint_disk = (const uint16_t*)rva.get_section()->content(rva.get_rel(), 2);
    p_hint = le_to_host(*hint_disk);
    
    std::string s = rva.get_section()->content_str(rva.get_rel()+2);
    p_name = new SgAsmBasicString(s);
    
    if (s.size()+1 % 2) {
        p_padding = *(rva.get_section()->content(rva.get_rel()+2+s.size()+1, 1));
    } else {
        p_padding = 0;
    }
}

void
SgAsmPEImportHNTEntry::unparse(std::ostream &f, rva_t rva) const
{
    uint16_t hint_disk;
    host_to_le(p_hint, &hint_disk);
    addr_t spos = rva.get_rel();
    spos = rva.get_section()->write(f, spos, 2, &hint_disk);
    spos = rva.get_section()->write(f, spos, p_name->get_string());
    spos = rva.get_section()->write(f, spos, '\0');
    if ((p_name->get_string().size()+1) % 2)
        rva.get_section()->write(f, spos, p_padding);
}

/* Print debugging info */
void
SgAsmPEImportHNTEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sHNTEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sHNTEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u\t\"%s\"", p, w, "hint/name", p_hint, p_name->c_str());
    if ((p_name->get_string().size()+1)%2)
        fprintf(f, " + '\\%03o'", p_padding);
    fputc('\n', f);
}

/** Constructor for PE import data. Constructs an SgAsmPEImportSection that represents either a PE ".idata" section as defined
 *  by the PE Section Table, or a PE Import Table as described by the RVA/Size pairs at the end of the NT Optional Header. The
 *  ".idata" section and PE Import Table both have the same format, which is generally:
 * 
 * @code
 * +-------------------------------+  Starts at address zero of the .idata
 * | Import Directory Table:       |  section or PE Import Table. Each Directory
 * |   Import Directory Entry #0   |  represents one library and all  its
 * |   Import Directory Entry #1   |  associated symbols.
 * |   ...                         |
 * |   Zero-filled Directory Entry |
 * +-------------------------------+
 *
 * +-------------------------------+  One table per dynamic library, starting
 * | Import Lookup Table (ILT)     |  at arbitrary RVA specified in the
 * |   ILT Entry #0                |  Directory Table.
 * |   ILT Entry #1                |
 * |   Zero-filled ILTEntry        |
 * +-------------------------------+
 *
 * +-------------------------------+  There is no starting RVA for this table.
 * | Hint-Name Table               |  Rather, ILT Entries each contain an RVA
 * |                               |  to an entry in the Hint-Name Table.
 * +-------------------------------+
 * @endcode
 */
void
SgAsmPEImportSection::ctor()
{
    set_synthesized(true);
    set_name(new SgAsmBasicString("PE Section Table"));
    set_purpose(SP_HEADER);

    p_import_directories = new SgAsmPEImportDirectoryList();
    p_import_directories->set_parent(this);
}

SgAsmPEImportSection*
SgAsmPEImportSection::parse()
{
    SgAsmPESection::parse();

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    for (size_t i = 0; 1; i++) {
        /* Read idata directory entries. The list is terminated with a zero-filled entry whose idx will be negative */
        SgAsmPEImportDirectory *idir = new SgAsmPEImportDirectory(this, i);
        if (idir->get_idx()<0) {
            delete idir;
            break;
        }

        /* Library name warnings and errors */
        rva_t rva = idir->get_dll_name_rva();
        if (!rva.get_section()) {
            fprintf(stderr, "SgAsmPEImportSection::ctor: error: in PE Import Directory entry %zu "
                    "Name RVA (0x%08"PRIx64") is not in the mapped address space.\n",
                    i, rva.get_rva());
        } else if (rva.get_section()!=this) {
            fprintf(stderr, "SgAsmPEImportSection::ctor: warning: Name RVA is outside PE Import Table\n");
            fprintf(stderr, "        Import Directory Entry #%zu\n", i);
            fprintf(stderr, "        Name RVA is %s\n", rva.to_string().c_str());
            fprintf(stderr, "        PE Import Table mapped from 0x%08"PRIx64" to 0x%08"PRIx64"\n", 
                    get_mapped_rva(), get_mapped_rva()+get_mapped_size());
        }

        /* Import Lookup Table */
        SgAsmPEImportLookupTable *ilt = new SgAsmPEImportLookupTable(this, idir->get_ilt_rva(), i, false);
        idir->set_ilt(ilt);
        ilt->set_parent(idir);

        /* Import Address Table (same class as the Import Lookup Table) */
        SgAsmPEImportLookupTable *iat = new SgAsmPEImportLookupTable(this, idir->get_iat_rva(), i, true);
        idir->set_iat(iat);
        iat->set_parent(idir);

        add_import_directory(idir);

        /* Create the GenericDLL for this library */
        SgAsmGenericDLL *dll = new SgAsmGenericDLL(idir->get_dll_name());
        for (size_t j=0; j<ilt->get_entries()->get_vector().size(); j++) {
            SgAsmPEImportILTEntry *e = ilt->get_entries()->get_vector()[j];
            SgAsmPEImportHNTEntry *hn = e->get_hnt_entry();
            if (hn!=NULL)
                dll->add_symbol(hn->get_name()->c_str());
        }
        fhdr->add_dll(dll);
    }
    return this;
}

void
SgAsmPEImportSection::add_import_directory(SgAsmPEImportDirectory *d)
{
    ROSE_ASSERT(p_import_directories!=NULL);
    p_import_directories->set_isModified(true);
    p_import_directories->get_vector().push_back(d);
    d->set_parent(this);
}

/* Write the import section back to disk */
void
SgAsmPEImportSection::unparse(std::ostream &f) const
{
    unparse_holes(f);

    /* Import Directory Entries and all they point to (even in other sections) */
    for (size_t i=0; i<get_import_directories()->get_vector().size(); i++) {
        get_import_directories()->get_vector()[i]->unparse(f, this);
    }
    
    /* Zero terminated */
    SgAsmPEImportDirectory::PEImportDirectory_disk zero;
    memset(&zero, 0, sizeof zero);
    write(f, get_import_directories()->get_vector().size()*sizeof(zero), sizeof zero, &zero);
}

/* Print debugging info */
void
SgAsmPEImportSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    SgAsmPESection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu\n", p, w, "ndirectories", p_import_directories->get_vector().size());
    for (size_t i=0; i<p_import_directories->get_vector().size(); i++)
        p_import_directories->get_vector()[i]->dump(f, p, i);

    if (variantT() == V_SgAsmPEImportSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Export Section.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmPEExportDirectory::ctor(SgAsmPEExportSection *section)
{
    set_parent(section);

    size_t entry_size = sizeof(PEExportDirectory_disk);
    const PEExportDirectory_disk *disk = (const PEExportDirectory_disk*)section->content(0, entry_size);
    
    p_res1         = le_to_host(disk->res1);
    p_timestamp    = le_to_host(disk->timestamp);
    p_vmajor       = le_to_host(disk->vmajor);
    p_vminor       = le_to_host(disk->vminor);
    p_name_rva     = le_to_host(disk->name_rva);       p_name_rva.set_section(section);
    p_ord_base     = le_to_host(disk->ord_base);
    p_expaddr_n    = le_to_host(disk->expaddr_n);
    p_nameptr_n    = le_to_host(disk->nameptr_n);
    p_expaddr_rva  = le_to_host(disk->expaddr_rva);    p_expaddr_rva.set_section(section);
    p_nameptr_rva  = le_to_host(disk->nameptr_rva);    p_nameptr_rva.set_section(section);
    p_ordinals_rva = le_to_host(disk->ordinals_rva);   p_ordinals_rva.set_section(section);

    std::string name = p_name_rva.get_section()->content_str(p_name_rva.get_rel());
    p_name = new SgAsmBasicString(name);
}

/* Print debugging info */
void
SgAsmPEExportDirectory::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEExportDirectory[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEExportDirectory.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = \"%s\"\n",                    p, w, "name", p_name->c_str());
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",               p, w, "res1", p_res1, p_res1);
    fprintf(f, "%s%-*s = %lu %s",                      p, w, "timestamp", (unsigned long)p_timestamp, ctime(&p_timestamp));
    fprintf(f, "%s%-*s = %u\n",                        p, w, "vmajor", p_vmajor);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "vminor", p_vminor);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "name_rva", p_name_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %u\n",                        p, w, "ord_base", p_ord_base);
    fprintf(f, "%s%-*s = %zu\n",                       p, w, "expaddr_n", p_expaddr_n);
    fprintf(f, "%s%-*s = %zu\n",                       p, w, "nameptr_n", p_nameptr_n);
    fprintf(f, "%s%-*s = %s\n",                        p, w, "expaddr_rva", p_expaddr_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "nameptr_rva", p_nameptr_rva.to_string().c_str());
    fprintf(f, "%s%-*s = %s\n",                        p, w, "ordinals_rva", p_ordinals_rva.to_string().c_str());
}

/* Constructor */
void
SgAsmPEExportEntry::ctor(SgAsmGenericString *fname, unsigned ordinal, rva_t expaddr, SgAsmGenericString *forwarder)
{
    set_name(fname);
    set_ordinal(ordinal);
    set_export_rva(expaddr);
    set_forwarder(forwarder);
}

/* Print debugging info */
void
SgAsmPEExportEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEExportEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEExportEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = [ord %u] rva=%s \"%s\"", p, w, "info", p_ordinal, p_export_rva.to_string().c_str(), p_name->c_str());
    if (p_forwarder)
        fprintf(f, " -> \"%s\"", p_forwarder->c_str());
    fputc('\n', f);
}

/* Override ROSETTA to set parent */
void
SgAsmPEExportEntry::set_name(SgAsmGenericString *fname)
{
    if (p_name!=fname)
        set_isModified(true);
    p_name = fname;
    if (p_name) p_name->set_parent(this);
}
void
SgAsmPEExportEntry::set_forwarder(SgAsmGenericString *forwarder)
{
    if (p_forwarder!=forwarder)
        set_isModified(true);
    p_forwarder = forwarder;
    if (p_forwarder) p_forwarder->set_parent(this);
}
    
/* Constructor */
void
SgAsmPEExportSection::ctor()
{
    ROSE_ASSERT(p_exports  == NULL);
    p_exports = new SgAsmPEExportEntryList();
    p_exports->set_parent(this);
}

SgAsmPEExportSection*
SgAsmPEExportSection::parse()
{
    SgAsmPESection::parse();

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    p_export_dir = new SgAsmPEExportDirectory(this);

    for (size_t i=0; i<p_export_dir->get_nameptr_n(); i++) {
        /* Function name */
        const ExportNamePtr_disk *nameptr_disk;
        addr_t nameptr_offset = p_export_dir->get_nameptr_rva().get_rel(this) + i*sizeof(*nameptr_disk);
        nameptr_disk = (const ExportNamePtr_disk*)content(nameptr_offset, sizeof(*nameptr_disk));
        rva_t nameptr = le_to_host(*nameptr_disk);
        SgAsmGenericString *fname = new SgAsmBasicString(content_str(nameptr.get_rel(this)));

        /* Ordinal (sort of an index into the Export Address Table contained in this same section) */
        const ExportOrdinal_disk *ordinal_disk;
        addr_t ordinal_offset = p_export_dir->get_ordinals_rva().get_rel(this) + i*sizeof(*ordinal_disk);
        ordinal_disk = (const ExportOrdinal_disk*)content(ordinal_offset, sizeof(*ordinal_disk));
        unsigned ordinal = le_to_host(*ordinal_disk);

        /* Export address. Convert the symbol's Ordinal into an index into the Export Address Table. The spec says to subtract
         * the ord_base from the Ordinal to get the index, but testing has shown this to be off by one (e.g., Windows-XP file
         * /WINDOWS/system32/msacm32.dll's Export Table's first symbol has the name "XRegThunkEntry" with an Ordinal of zero
         * and the ord_base is one. The index according to spec would be -1 rather than the correct value of zero.) */
        rva_t expaddr;
        if (ordinal >= (p_export_dir->get_ord_base()-1)) {
            unsigned expaddr_idx = ordinal - (p_export_dir->get_ord_base()-1);
            ROSE_ASSERT(expaddr_idx < p_export_dir->get_expaddr_n());
            const ExportAddress_disk *expaddr_disk;
            addr_t expaddr_offset = p_export_dir->get_expaddr_rva().get_rel(this) + expaddr_idx*sizeof(*expaddr_disk);
            expaddr_disk = (const ExportAddress_disk*)content(expaddr_offset, sizeof(*expaddr_disk));
            expaddr = le_to_host(*expaddr_disk);
            expaddr.bind(fhdr);
        } else {
            expaddr = 0xffffffff; /*Ordinal out of range!*/
        }

        /* If export address is within this section then it points to a NUL-terminated forwarder name. */
        SgAsmGenericString *forwarder = NULL;
        if (expaddr.get_rva()>=get_mapped_rva() && expaddr.get_rva()<get_mapped_rva()+get_mapped_size()) {
            forwarder = new SgAsmBasicString(content_str(expaddr.get_rel(this)));
        }

        SgAsmPEExportEntry *entry = new SgAsmPEExportEntry(fname, ordinal, expaddr, forwarder);
        add_entry(entry);
    }
    return this;
}

void
SgAsmPEExportSection::add_entry(SgAsmPEExportEntry *entry)
{
    ROSE_ASSERT(p_exports!=NULL);
    p_exports->set_isModified(true);
    p_exports->get_exports().push_back(entry);
}

/* Print debugging info */
void
SgAsmPEExportSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEExportSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEExportSection.", prefix);
    }
    
    SgAsmPESection::dump(f, p, -1);

    if (p_export_dir)
        p_export_dir->dump(f, p, -1);
    for (size_t i=0; i<p_exports->get_exports().size(); i++)
        p_exports->get_exports()[i]->dump(f, p, i);

    if (variantT() == V_SgAsmPEExportSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Table Sections
//
//    SgAsmPEStringTable is derived from SgAsmPESection, which is derived in turn from SgAsmGenericSection. A PE String Table
//    Section points to a COFF String Table (SgAsmCoffStrtab) that is contained in the section.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmPEStringSection::ctor()
{
    get_name()->set_string("PE String Table");
    p_strtab = new SgAsmCoffStrtab(this);
}

SgAsmPEStringSection*
SgAsmPEStringSection::parse()
{
    SgAsmPESection::parse();
    ROSE_ASSERT(p_strtab);
    p_strtab->parse();
    return this;
}

/* Reallocate space for the string table if necessary. Note that reallocation is lazy here -- we don't shrink the section,
 * we only enlarge it (if you want the section to shrink then call SgAsmGenericStrtab::reallocate(bool) with a true value
 * rather than calling this function. SgAsmPEStringSection::reallocate is called in response to unparsing a file and gives
 * the string table a chance to extend its container section if it needs to allocate more space for strings. */
bool
SgAsmPEStringSection::reallocate()
{
    return get_strtab()->reallocate(false);
}

/* Unparse an ElfStringSection by unparsing the ElfStrtab */
void
SgAsmPEStringSection::unparse(std::ostream &f) const
{
    get_strtab()->unparse(f);
    unparse_holes(f);
}

/* Augments superclass to make sure free list and such are adjusted properly */
void
SgAsmPEStringSection::set_size(addr_t newsize)
{
    addr_t orig_size = get_size();
    SgAsmPESection::set_size(newsize);
    SgAsmGenericStrtab *strtab = get_strtab();

    if (get_size() > orig_size) {
        /* Add new address space to string table free list */
        addr_t n = get_size() - orig_size;
        strtab->get_freelist().insert(orig_size, n);
    } else if (get_size() < orig_size) {
        /* Remove deleted address space from string table free list */
        addr_t n = orig_size - get_size();
        strtab->get_freelist().erase(get_size(), n);
    }
}

/* Print some debugging info */
void
SgAsmPEStringSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEStringSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEStringSection.", prefix);
    }
    
    SgAsmPESection::dump(f, p, -1);

    ROSE_ASSERT(get_strtab()!=NULL);
    get_strtab()->dump(f, p, -1);

    if (variantT() == V_SgAsmPEStringSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Tables
//
//    An SgAsmCoffStrtab is a COFF String Table, inheriting from SgAsmGenericStrtab. String tables point to the
//    SgAsmGenericSection that contains them.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Free StringStorage objects associated with this string table. It may not be safe to blow them away yet since other objects
 * may still have SgAsmStoredStrings pointing to these storage objects. So instead, we will mark all this strtab's storage
 * objects as no longer being associated with a string table. This allows the SgAsmStoredString objects to still function
 * properly and their destructors (~SgAsmStoredString) will free their storage. */
SgAsmCoffStrtab::~SgAsmCoffStrtab()
{
    for (referenced_t::iterator i = p_storage_list.begin(); i != p_storage_list.end(); ++i) {
        SgAsmStringStorage *storage = *i;
        storage->set_strtab(NULL);
        storage->set_offset(SgAsmGenericString::unallocated);
    }
    p_storage_list.clear();
    p_dont_free = NULL; /*FIXME: can't delete for same reason as in SgAsmStoredString destructor. (RPM 2008-09-05) */
}

/* Creates the storage item for the string at the specified offset. If "shared" is true then attempt to re-use a previous storage
 * object, otherwise create a new one. Each storage object is considered to be a separate string, therefore when two strings
 * share the same storage object, changing one string changes the other. */
SgAsmStringStorage *
SgAsmCoffStrtab::create_storage(addr_t offset, bool shared)
{
    ROSE_ASSERT(offset!=SgAsmGenericString::unallocated);
    SgAsmGenericSection *container = get_container();

    /* Has the string already been created? */
    if (shared) {
        for (referenced_t::iterator i=p_storage_list.begin(); i!=p_storage_list.end(); ++i) {
            if ((*i)->get_offset()==offset && (*i)!=p_dont_free)
                return *i;
        }
    }
  
    /* Read string length byte */
    unsigned len = container->content(offset, 1)[0];

    /* Make sure new storage isn't inside some other string. (We don't support nested strings in COFF where the length byte of
     * the nested string is one of the characters of the outer string.) */
    for (referenced_t::iterator i=p_storage_list.begin(); i!=p_storage_list.end(); ++i) {
        ROSE_ASSERT((*i)->get_offset()==SgAsmGenericString::unallocated ||
                    offset + 1 + len <= (*i)->get_offset() ||
                    offset >= 1 + (*i)->get_string().size());
    }

    /* Create storage object */
    const char *s = (const char*)container->content(offset+1, len);
    SgAsmStringStorage *storage = new SgAsmStringStorage(this, std::string(s, len), offset);

    /* It's a bad idea to free (e.g., modify) strings before we've identified all the strings in the table. Consider
     * the case where two strings have the same value and point to the same offset (i.e., they share storage). If we modify one
     * before we know about the other then (at best) we modify the other one also.
     * 
     * The only time we can guarantee this is OK is when the new storage points to the same file location as "dont_free"
     * since the latter is guaranteed to never be freed or shared. This exception is used when creating a new, unallocated
     * string (see SgAsmStoredString(SgAsmGenericStrtab,const std::string&)). */
    if (p_num_freed>0 && (!p_dont_free || offset!=p_dont_free->get_offset())) {
        fprintf(stderr,
                "SgAsmCoffStrtab::create_storage(%"PRIu64"): %zu other string%s (of %zu created) in [%d] \"%s\""
                " %s been modified and/or reallocated!\n",
                offset, p_num_freed, 1==p_num_freed?"":"s", p_storage_list.size(),
                container->get_id(), container->get_name()->c_str(),
                1==p_num_freed?"has":"have");
        ROSE_ASSERT(0==p_num_freed);
    }

    set_isModified(true);
    p_storage_list.push_back(storage);
    return storage;
}

/* Returns the number of bytes required to store the string in the string table. This is one (the length byte) plus the
 * length of the string. */
rose_addr_t
SgAsmCoffStrtab::get_storage_size(const SgAsmStringStorage *storage) {
    return 1 + storage->get_string().size();
}

/* Write string table back to disk. Free space is zeroed out; holes are left as they are. */
void
SgAsmCoffStrtab::unparse(std::ostream &f) const
{
    SgAsmGenericSection *container = get_container();

    /* Write length coded strings. Shared strings will be written more than once, but that's OK. */
    for (size_t i=0; i<p_storage_list.size(); i++) {
        SgAsmStringStorage *storage = p_storage_list[i];
        ROSE_ASSERT(storage->get_offset()!=SgAsmGenericString::unallocated);
        addr_t at = container->write(f, storage->get_offset(), storage->get_string());
        container->write(f, at, '\0');
    }
    
    /* Fill free areas with zero */
    for (ExtentMap::const_iterator i=get_freelist().begin(); i!=get_freelist().end(); ++i) {
        container->write(f, i->first, std::string(i->second, '\0'));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COFF Symbol Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor reads symbol table entries beginning at entry 'i'. We can't pass an array of COFFSymbolEntry_disk structs
 * because the disk size is 18 bytes, which is not properly aligned according to the C standard. Therefore we pass the actual
 * section and table index. The symbol occupies the specified table slot and st_num_aux_entries additional slots. */
void
SgAsmCoffSymbol::ctor(SgAsmPEFileHeader *fhdr, SgAsmGenericSection *symtab, SgAsmGenericSection *strtab, size_t idx)
{
    static const bool debug = false;
    COFFSymbol_disk disk;
    symtab->content(idx * COFFSymbol_disk_size, COFFSymbol_disk_size, &disk);
    if (disk.st_zero == 0) {
        p_st_name_offset = le_to_host(disk.st_offset);
        if (p_st_name_offset < 4) throw FormatError("name collides with size field");
        set_name(new SgAsmBasicString(strtab->content_str(p_st_name_offset)));
    } else {
        char temp[9];
        memcpy(temp, disk.st_name, 8);
        temp[8] = '\0';
        set_name(new SgAsmBasicString(temp));
        p_st_name_offset = 0;
    }

    p_st_name            = get_name()->get_string();
    p_st_section_num     = le_to_host(disk.st_section_num);
    p_st_type            = le_to_host(disk.st_type);
    p_st_storage_class   = le_to_host(disk.st_storage_class);
    p_st_num_aux_entries = le_to_host(disk.st_num_aux_entries);

    /* Bind to section number. We can do this now because we've already parsed the PE Section Table */
    ROSE_ASSERT(fhdr->get_section_table()!=NULL);
    if (p_st_section_num > 0) {
        p_bound = fhdr->get_file()->get_section_by_id(p_st_section_num);
        ROSE_ASSERT(p_bound != NULL);
    }
    
    /* Make initial guesses for storage class, type, and definition state. We'll adjust them after reading aux entries. */
    p_value = le_to_host(disk.st_value);
    p_def_state = SYM_DEFINED;
    switch (p_st_storage_class) {
      case 0:    p_binding = SYM_NO_BINDING; break; /*none*/
      case 1:    p_binding = SYM_LOCAL;      break; /*stack*/
      case 2:    p_binding = SYM_GLOBAL;     break; /*extern*/
      case 3:    p_binding = SYM_GLOBAL;     break; /*static*/
      case 4:    p_binding = SYM_LOCAL;      break; /*register*/
      case 5:    p_binding = SYM_GLOBAL;     break; /*extern def*/
      case 6:    p_binding = SYM_LOCAL;      break; /*label*/
      case 7:    p_binding = SYM_LOCAL;      break; /*label(undef)*/
      case 8:    p_binding = SYM_LOCAL;      break; /*struct member*/
      case 9:    p_binding = SYM_LOCAL;      break; /*formal arg*/
      case 10:   p_binding = SYM_LOCAL;      break; /*struct tag*/
      case 11:   p_binding = SYM_LOCAL;      break; /*union member*/
      case 12:   p_binding = SYM_GLOBAL;     break; /*union tag*/
      case 13:   p_binding = SYM_GLOBAL;     break; /*typedef*/
      case 14:   p_binding = SYM_GLOBAL;     break; /*static(undef)*/
      case 15:   p_binding = SYM_GLOBAL;     break; /*enum tag*/
      case 16:   p_binding = SYM_LOCAL;      break; /*enum member*/
      case 17:   p_binding = SYM_GLOBAL;     break; /*register param*/
      case 18:   p_binding = SYM_LOCAL;      break; /*bit field*/
      case 100:  p_binding = SYM_GLOBAL;     break; /*block(bb or eb)*/
      case 101:  p_binding = SYM_GLOBAL;     break; /*function*/
      case 102:  p_binding = SYM_LOCAL;      break; /*struct end*/
      case 103:  p_binding = SYM_GLOBAL;     break; /*file*/
      case 104:  p_binding = SYM_GLOBAL;     break; /*section*/
      case 105:  p_binding = SYM_WEAK;       break; /*weak extern*/
      case 107:  p_binding = SYM_LOCAL;      break; /*CLR token*/
      case 0xff: p_binding = SYM_GLOBAL;     break; /*end of function*/
    }
    switch (p_st_type & 0xf0) {
      case 0x00: p_type = SYM_NO_TYPE; break;     /*none*/
      case 0x10: p_type = SYM_DATA;    break;     /*ptr*/
      case 0x20: p_type = SYM_FUNC;    break;     /*function*/
      case 0x30: p_type = SYM_ARRAY;   break;     /*array*/
    }
    
    /* Read additional aux entries. We keep this as 'char' to avoid alignment problems. */
    if (p_st_num_aux_entries > 0) {
        p_aux_data = symtab->content_ucl((idx+1)*COFFSymbol_disk_size, p_st_num_aux_entries * COFFSymbol_disk_size);

        if (get_type() == SYM_FUNC && p_st_section_num > 0) {
            /* Function */
            unsigned bf_idx      = le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned size        = le_to_host(*(uint32_t*)&(p_aux_data[4]));
            unsigned lnum_ptr    = le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned next_fn_idx = le_to_host(*(uint32_t*)&(p_aux_data[12]));
            unsigned res1        = le_to_host(*(uint16_t*)&(p_aux_data[16]));
            set_size(size);
            if (debug) {
                fprintf(stderr, "COFF aux func %s: bf_idx=%u, size=%u, lnum_ptr=%u, next_fn_idx=%u, res1=%u\n", 
                        p_st_name.c_str(), bf_idx, size, lnum_ptr, next_fn_idx, res1);
            }
            
        } else if (p_st_storage_class == 101 /*function*/ && (0 == p_st_name.compare(".bf") || 0 == p_st_name.compare(".ef"))) {
            /* Beginning/End of function */
            unsigned res1        = le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned lnum        = le_to_host(*(uint16_t*)&(p_aux_data[4])); /*line num within source file*/
            unsigned res2        = le_to_host(*(uint16_t*)&(p_aux_data[6]));
            unsigned res3        = le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned next_bf     = le_to_host(*(uint32_t*)&(p_aux_data[12])); /*only for .bf; reserved in .ef*/
            unsigned res4        = le_to_host(*(uint16_t*)&(p_aux_data[16]));
            if (debug) {
                fprintf(stderr, "COFF aux %s: res1=%u, lnum=%u, res2=%u, res3=%u, next_bf=%u, res4=%u\n", 
                        p_st_name.c_str(), res1, lnum, res2, res3, next_bf, res4);
            }
            
        } else if (p_st_storage_class == 2/*external*/ && p_st_section_num == 0/*undef*/ && get_value()==0) {
            /* Weak External */
            unsigned sym2_idx    = le_to_host(*(uint32_t*)&(p_aux_data[0]));
            unsigned flags       = le_to_host(*(uint32_t*)&(p_aux_data[4]));
            unsigned res1        = le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned res2        = le_to_host(*(uint32_t*)&(p_aux_data[12]));
            unsigned res3        = le_to_host(*(uint16_t*)&(p_aux_data[16]));
            if (debug) {
                fprintf(stderr, "COFF aux weak %s: sym2_idx=%u, flags=%u, res1=%u, res2=%u, res3=%u\n",
                        p_st_name.c_str(), sym2_idx, flags, res1, res2, res3);
            }
            
        } else if (p_st_storage_class == 103/*file*/ && 0 == p_st_name.compare(".file")) {
            /* This symbol is a file. The file name is stored in the aux data as either the name itself or an offset
             * into the string table. Replace the fake ".file" with the real file name. */
            const COFFSymbol_disk *d = (const COFFSymbol_disk*) &(p_aux_data[0]);
            if (0 == d->st_zero) {
                addr_t fname_offset = le_to_host(d->st_offset);
                if (fname_offset < 4) throw FormatError("name collides with size field");
                set_name(new SgAsmBasicString(strtab->content_str(fname_offset)));
                if (debug)
                    fprintf(stderr, "COFF aux file: offset=%"PRIu64", name=\"%s\"\n", fname_offset, get_name()->c_str());
            } else {
                /* Aux data contains a NUL-padded name; the NULs (if any) are not part of the name. */
                ROSE_ASSERT(p_st_num_aux_entries == 1);
                char fname[COFFSymbol_disk_size+1];
                memcpy(fname, &(p_aux_data[0]), COFFSymbol_disk_size);
                fname[COFFSymbol_disk_size] = '\0';
                set_name(new SgAsmBasicString(fname));
                if (debug)
                    fprintf(stderr, "COFF aux file: inline-name=\"%s\"\n", get_name()->c_str());
            }
            set_type(SYM_FILE);

        } else if (p_st_storage_class == 3/*static*/ && NULL != fhdr->get_file()->get_section_by_name(p_st_name, '$')) {
            /* Section */
            unsigned size         = le_to_host(*(uint32_t*)&(p_aux_data[0])); /*same as section header SizeOfRawData */
            unsigned nrel         = le_to_host(*(uint16_t*)&(p_aux_data[4])); /*number of relocations*/
            unsigned nln_ents     = le_to_host(*(uint16_t*)&(p_aux_data[6])); /*number of line number entries */
            unsigned cksum        = le_to_host(*(uint32_t*)&(p_aux_data[8]));
            unsigned sect_id      = le_to_host(*(uint16_t*)&(p_aux_data[12])); /*1-base index into section table*/
            unsigned comdat       = p_aux_data[14]; /*comdat selection number if section is a COMDAT section*/
            unsigned res1         = p_aux_data[15];
            unsigned res2         = le_to_host(*(uint16_t*)&(p_aux_data[16]));
            set_size(size);
            set_type(SYM_SECTION);
            if (debug) {
                fprintf(stderr, 
                        "COFF aux section: size=%u, nrel=%u, nln_ents=%u, cksum=%u, sect_id=%u, comdat=%u, res1=%u, res2=%u\n", 
                        size, nrel, nln_ents, cksum, sect_id, comdat, res1, res2);
            }
            
        } else if (p_st_storage_class==3/*static*/ && (p_st_type & 0xf)==0/*null*/ &&
                   get_value()==0 && NULL!=fhdr->get_file()->get_section_by_name(p_st_name)) {
            /* COMDAT section */
            /*FIXME: not implemented yet*/
            fprintf(stderr, "COFF aux comdat %s: (FIXME) not implemented yet\n", p_st_name.c_str());
            hexdump(stderr, (addr_t) symtab->get_offset()+(idx+1)*COFFSymbol_disk_size, "    ", p_aux_data);

        } else {
            fprintf(stderr, "COFF aux unknown %s: (FIXME) st_storage_class=%u, st_type=0x%02x, st_section_num=%d\n", 
                    p_st_name.c_str(), p_st_storage_class, p_st_type, p_st_section_num);
            hexdump(stderr, symtab->get_offset()+(idx+1)*COFFSymbol_disk_size, "    ", p_aux_data);
        }
    }

}

/* Encode a symbol back into disk format */
void *
SgAsmCoffSymbol::encode(COFFSymbol_disk *disk) const
{
    if (0 == p_st_name_offset) {
        /* Name is stored in entry */
        memset(disk->st_name, 0, sizeof(disk->st_name));
        ROSE_ASSERT(p_st_name.size() <= sizeof(disk->st_name));
        memcpy(disk->st_name, p_st_name.c_str(), p_st_name.size());
    } else {
        /* Name is an offset into the string table */
        disk->st_zero = 0;
        host_to_le(p_st_name_offset, &(disk->st_offset));
    }
    
 // host_to_le(get_value(),          &(disk->st_value));
    host_to_le(p_value,              &(disk->st_value));
    host_to_le(p_st_section_num,     &(disk->st_section_num));
    host_to_le(p_st_type,            &(disk->st_type));
    host_to_le(p_st_storage_class,   &(disk->st_storage_class));
    host_to_le(p_st_num_aux_entries, &(disk->st_num_aux_entries));
    return disk;
}

/* Print some debugging info */
void
SgAsmCoffSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096], ss[128], tt[128];
    const char *s=NULL, *t=NULL;
    if (idx>=0) {
        sprintf(p, "%sCOFFSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sCOFFSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));


    SgAsmGenericSymbol::dump(f, p, -1);

    switch (p_st_section_num) {
      case 0:  s = "external, not assigned";    break;
      case -1: s = "absolute value";            break;
      case -2: s = "general debug, no section"; break;
      default: sprintf(ss, "%d", p_st_section_num); s = ss; break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "st_section_num", s);

    switch (p_st_type & 0xf0) {
      case 0x00: s = "none";     break;
      case 0x10: s = "pointer";  break;
      case 0x20: s = "function"; break;
      case 0x30: s = "array";    break;
      default:
        sprintf(ss, "%u", p_st_type >> 8);
        s = ss;
        break;
    }
    switch (p_st_type & 0xf) {
      case 0x00: t = "none";            break;
      case 0x01: t = "void";            break;
      case 0x02: t = "char";            break;
      case 0x03: t = "short";           break;
      case 0x04: t = "int";             break;
      case 0x05: t = "long";            break;
      case 0x06: t = "float";           break;
      case 0x07: t = "double";          break;
      case 0x08: t = "struct";          break;
      case 0x09: t = "union";           break;
      case 0x0a: t = "enum";            break;
      case 0x0b: t = "enum member";     break;
      case 0x0c: t = "byte";            break;
      case 0x0d: t = "2-byte word";     break;
      case 0x0e: t = "unsigned int";    break;
      case 0x0f: t = "4-byte unsigned"; break;
      default:
        sprintf(tt, "%u", p_st_type & 0xf);
        t = tt;
        break;
    }
    fprintf(f, "%s%-*s = %s / %s\n",          p, w, "st_type", s, t);

    switch (p_st_storage_class) {
      case 0:    s = "none";            t = "";                                  break;
      case 1:    s = "auto variable";   t = "stack frame offset";                break;
      case 2:    s = "external";        t = "size or section offset";            break;
      case 3:    s = "static";          t = "offset in section or section name"; break;
      case 4:    s = "register";        t = "register number";                   break;
      case 5:    s = "extern_def";      t = "";                                  break;
      case 6:    s = "label";           t = "offset in section";                 break;
      case 7:    s = "label(undef)";    t = "";                                  break;
      case 8:    s = "struct member";   t = "member number";                     break;
      case 9:    s = "formal arg";      t = "argument number";                   break;
      case 10:   s = "struct tag";      t = "tag name";                          break;
      case 11:   s = "union member";    t = "member number";                     break;
      case 12:   s = "union tag";       t = "tag name";                          break;
      case 13:   s = "typedef";         t = "";                                  break;
      case 14:   s = "static(undef)";   t = "";                                  break;
      case 15:   s = "enum tag";        t = "";                                  break;
      case 16:   s = "enum member";     t = "member number";                     break;
      case 17:   s = "register param";  t = "";                                  break;
      case 18:   s = "bit field";       t = "bit number";                        break;
      case 19:   s = "auto arg";        t = "";                                  break;
      case 20:   s = "dummy entry (EOB)"; t="";                                  break;
      case 100:  s = "block(bb,eb)";    t = "relocatable address";               break;
      case 101:  s = "function";        t = "nlines or size";                    break;
      case 102:  s = "struct end";      t = "";                                  break;
      case 103:  s = "file";            t = "";                                  break;
      case 104:  s = "section/line#";   t = "";                                  break;
      case 105:  s = "weak extern";     t = "";                                  break;
      case 106:  s = "ext in dmert pub lib";t="";                                break;
      case 107:  s = "CLR token";       t = "";                                  break;
      case 0xff: s = "end of function"; t = "";                                  break;
      default:
        sprintf(ss, "%u", p_st_storage_class);
        s = ss;
        t = "";  
        break;
    }
    fprintf(f, "%s%-*s = %s\n",               p, w, "st_storage_class", s);
    fprintf(f, "%s%-*s = \"%s\"\n",           p, w, "st_name", p_st_name.c_str());
    fprintf(f, "%s%-*s = %u\n",               p, w, "st_num_aux_entries", p_st_num_aux_entries);
    fprintf(f, "%s%-*s = %zu bytes\n",        p, w, "aux_data", p_aux_data.size());
    hexdump(f, 0, std::string(p)+"aux_data at ", p_aux_data);
}

/* Constructor */
void
SgAsmCoffSymbolTable::ctor()
{
    set_synthesized(true);
    set_name(new SgAsmBasicString("COFF Symbols"));
    set_purpose(SP_SYMTAB);

    p_symbols = new SgAsmCoffSymbolList;
    p_symbols->set_parent(this);
}

SgAsmCoffSymbolTable*
SgAsmCoffSymbolTable::parse()
{
    /* Set the section size according to the number of entries indicated in the header. */
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    set_offset(fhdr->get_e_coff_symtab());
    set_size(fhdr->get_e_coff_nsyms()*SgAsmCoffSymbol::COFFSymbol_disk_size);

    SgAsmGenericSection::parse();

    /* The string table immediately follows the symbols. The first four bytes of the string table are the size of the
     * string table in little endian. */
    addr_t strtab_offset = get_offset() + fhdr->get_e_coff_nsyms() * SgAsmCoffSymbol::COFFSymbol_disk_size;
    p_strtab = new SgAsmGenericSection(fhdr->get_file(), fhdr, strtab_offset, sizeof(uint32_t));
    p_strtab->set_synthesized(true);
    p_strtab->set_name(new SgAsmBasicString("COFF Symbol Strtab"));
    p_strtab->set_purpose(SP_HEADER);
    p_strtab->parse();

    uint32_t word;
    p_strtab->content(0, sizeof word, &word);
    addr_t strtab_size = le_to_host(word);
    if (strtab_size < sizeof(uint32_t))
        throw FormatError("COFF symbol table string table size is less than four bytes");
    p_strtab->extend(strtab_size - sizeof(uint32_t));

    for (size_t i = 0; i < fhdr->get_e_coff_nsyms(); i++) {
        SgAsmCoffSymbol *symbol = new SgAsmCoffSymbol(fhdr, this, p_strtab, i);
        i += symbol->get_st_num_aux_entries();
        p_symbols->get_symbols().push_back(symbol);
    }
    return this;
}

/** Returns the number of COFF Symbol Table slots occupied by the symbol table. The number of slots can be larger than the
 *  number of symbols since some symbols might have auxiliary entries. */
size_t
SgAsmCoffSymbolTable::get_nslots() const
{
    size_t nsyms = p_symbols->get_symbols().size();
    size_t nslots = nsyms;
    for (size_t i=0; i<nsyms; i++) {
        SgAsmCoffSymbol *symbol = p_symbols->get_symbols()[i];
        nslots += symbol->get_st_num_aux_entries();
    }
    return nslots;
}

/* Write symbol table back to disk */
void
SgAsmCoffSymbolTable::unparse(std::ostream &f) const
{
    addr_t spos = 0; /*section offset*/
    
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmCoffSymbol *symbol = p_symbols->get_symbols()[i];
        SgAsmCoffSymbol::COFFSymbol_disk disk;
        symbol->encode(&disk);
        spos = write(f, spos, SgAsmCoffSymbol::COFFSymbol_disk_size, &disk);
        spos = write(f, (addr_t) spos, symbol->get_aux_data());
    }
    if (get_strtab())
        get_strtab()->unparse(f);
}

/* Print some debugging info */
void
SgAsmCoffSymbolTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sCOFFSymtab[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sCOFFSymtab.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu symbols\n", p, w, "size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        p_symbols->get_symbols()[i]->dump(f, p, i);
    }

    if (variantT() == V_SgAsmCoffSymbolTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
