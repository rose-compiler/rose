/* Windows PE file header (SgAsmPEFileHeader and related classes) */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


void
SgAsmPERVASizePair::ctor(const RVASizePair_disk *disk) {
    p_e_rva  = le_to_host(disk->e_rva);
    p_e_size = le_to_host(disk->e_size);
}

void*
SgAsmPERVASizePair::encode(RVASizePair_disk *disk) const {
    host_to_le(p_e_rva,  &(disk->e_rva));
    host_to_le(p_e_size, &(disk->e_size));
    return disk;
}

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
    p_e_nt_hdr_size = sizeof(PE32OptHeader_disk);
}

/** Return true if the file looks like it might be a PE file according to the magic number.  The file must contain what
 *  appears to be a DOS File Header at address zero, and what appears to be a PE File Header at a file offset specified in
 *  part of the DOS File Header (actually, in the bytes that follow the DOS File Header). */
bool
SgAsmPEFileHeader::is_PE(SgAsmGenericFile *file)
{
    /* Turn off byte reference tracking for the duration of this function. We don't want our testing the file contents to
     * affect the list of bytes that we've already referenced or which we might reference later. */
    bool was_tracking = file->get_tracking_references();
    file->set_tracking_references(false);

    try {
        /* Check DOS File Header magic number at beginning of the file */
        unsigned char dos_magic[2];
        file->read_content(0, dos_magic, sizeof dos_magic);
        if ('M'!=dos_magic[0] || 'Z'!=dos_magic[1])
            throw 1;

        /* Read four-byte offset of potential PE File Header at offset 0x3c */
        uint32_t lfanew_disk;
        file->read_content(0x3c, &lfanew_disk, sizeof lfanew_disk);
        addr_t pe_offset = le_to_host(lfanew_disk);
        
        /* Look for the PE File Header magic number */
        unsigned char pe_magic[4];
        file->read_content(pe_offset, pe_magic, sizeof pe_magic);
        if ('P'!=pe_magic[0] || 'E'!=pe_magic[1] || '\0'!=pe_magic[2] || '\0'!=pe_magic[3])
            throw 1;
    } catch (...) {
        file->set_tracking_references(was_tracking);
        return false;
    }
    
    file->set_tracking_references(was_tracking);
    return true;
}

/** Initialize the header with information parsed from the file and construct and parse everything that's reachable from the
 *  header. The PE File Header should have been constructed such that SgAsmPEFileHeader::ctor() was called. */
SgAsmPEFileHeader*
SgAsmPEFileHeader::parse()
{
    SgAsmGenericHeader::parse();
    
    /* Read header, zero padding if the file isn't large enough */
    PEFileHeader_disk fh;
    if (sizeof(fh)>get_size())
        extend(sizeof(fh)-get_size());
    if (sizeof(fh)!=read_content_local(0, &fh, sizeof fh, false))
        fprintf(stderr, "SgAsmPEFileHeader::parse: warning: short read of PE header at byte 0x%08"PRIx64"\n", get_offset());

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
	addr_t need32 = sizeof(PEFileHeader_disk) + std::min(p_e_nt_hdr_size, (addr_t)(sizeof oh32));
    if (need32>get_size())
        extend(need32-get_size());
    if (sizeof(oh32)!=read_content_local(sizeof fh, &oh32, sizeof oh32, false))
        fprintf(stderr, "SgAsmPEFileHeader::parse: warning: short read of PE Optional Header at byte 0x%08"PRIx64"\n", 
                get_offset() + sizeof(fh));
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
        addr_t need64 = sizeof(PEFileHeader_disk) + std::min(p_e_nt_hdr_size, (addr_t)(sizeof oh64));
        if (need64>get_size())
            extend(need64-get_size());
        if (sizeof(oh64)!=read_content_local(sizeof fh, &oh64, sizeof oh64))
            fprintf(stderr, "SgAsmPEFileHeader::parse: warning: short read of PE Optional Header at byte 0x%08"PRIx64"\n", 
                    get_offset() + sizeof(fh));
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
        fprintf(stderr, "SgAsmPEFileHeader::parse: warning: unrecognized e_cputype = 0x%x (%u)\n", p_e_cpu_type, p_e_cpu_type);
        set_isa(ISA_OTHER);
        break;
    }

    /* Entry point. We will eventually bind the entry point to a particular section (in SgAsmPEFileHeader::parse) so that if
     * sections are rearranged, extended, etc. the entry point will be updated automatically. */
    add_entry_rva(entry_rva);

    /* The PE File Header has a fixed-size component followed by some number of RVA/Size pairs. The add_rvasize_pairs() will
     * extend  the header and parse the RVA/Size pairs. */
    if (get_e_num_rvasize_pairs() > 1000) {
        fprintf(stderr, "warning: PE File Header contains an unreasonable number of Rva/Size pairs. Limiting to 1000.\n");
        set_e_num_rvasize_pairs(1000);
    }
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

    /* Parse the COFF symbol table */
    if (get_e_coff_symtab() && get_e_coff_nsyms()) {
        SgAsmCoffSymbolTable *symtab = new SgAsmCoffSymbolTable(this);
        symtab->set_offset(get_e_coff_symtab());
        symtab->parse();
        set_coff_symtab(symtab);
    }

    /* Associate RVAs with particular sections so that if a section's mapping is changed the RVA gets adjusted automatically. */
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
        if (sizeof(pairs_disk)!=read_content_local(pairs_offset, &pairs_disk, sizeof pairs_disk, false))
            fprintf(stderr, "SgAsmPEFileHeader::add_rvasize_pairs: warning: RVA/Size pair %zu at file offset 0x%08"PRIx64
                    " extends beyond the end of file (assuming 0/0)\n", i, get_offset()+pairs_offset);
        p_rvasize_pairs->get_pairs().push_back(new SgAsmPERVASizePair(&pairs_disk));
        p_rvasize_pairs->get_pairs().back()->set_parent(p_rvasize_pairs);
    }
}

/* Looks at the RVA/Size pairs in the PE header and creates an SgAsmGenericSection object for each one.  This must be done
 * after we build the mapping from virtual addresses to file offsets. */
void
SgAsmPEFileHeader::create_table_sections()
{
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
          case 15: tabname = "Reserved";                     break; /*PE documentation says "must be zero", but often is not*/
          default: ROSE_ASSERT(!"too many RVA/Size pairs");  break;
        }

        /* Find the starting offset in the file.
         * FIXME: We have a potential problem here in that ROSE sections are always contiguous in the file but a section created
         *        from an RVA/Size pair is not necessarily contiguous in the file.  Normally such sections are in fact
         *        contiguous and we'll just ignore this for now.  In any case, as long as these sections only ever read their
         *        data via the same MemoryMap that we use here, everything should be fine. [RPM 2009-08-17] */
        MemoryMap *map = get_loader_map();
        ROSE_ASSERT(map!=NULL);
        const MemoryMap::MapElement *elmt = map->find(get_base_va() + pair->get_e_rva());
        if (!elmt) {
            fprintf(stderr, "SgAsmPEFileHeader::create_table_sections: warning: pair-%zu, rva=0x%08"PRIx64", size=%"PRIu64
                    " bytes \"%s\": unable to find a mapping for the virtual address (skipping)\n",
                    i, pair->get_e_rva().get_rva(), pair->get_e_size(), tabname?tabname:"");
            continue;
        }
        addr_t file_offset = elmt->is_anonymous() ? 0 : elmt->get_va_offset(get_base_va() + pair->get_e_rva());

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
                tabsec = new SgAsmGenericSection(get_file(), this);
                break;
        }
        if (tabname) tabsec->set_name(new SgAsmBasicString(tabname));
        tabsec->set_synthesized(true);
        tabsec->set_purpose(SP_HEADER);

        tabsec->set_offset(file_offset);
        tabsec->set_size(pair->get_e_size());
        tabsec->set_file_alignment(1);

        tabsec->set_mapped_alignment(1);
        tabsec->set_mapped_preferred_rva(pair->get_e_rva().get_rva());
        tabsec->set_mapped_actual_va(pair->get_e_rva().get_rva()+get_base_va()); /*FIXME: not sure this is correct. [RPM 2009-09-11]*/
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
            if (pesec && pesec->get_section_entry()!=NULL)
                p_e_nsections++;
        }

        addr_t header_size = ALIGN_UP(p_section_table->get_offset() + p_section_table->get_size(),
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
            if (pesec && pesec->get_section_entry()!=NULL) {
                if (0==nfound++) {
                    min_offset = pesec->get_offset();
                } else {
                    min_offset = std::min(min_offset, pesec->get_offset() );
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

    /* The size of the optional header. If there's a section table then we use its offset to calculate the optional header
     * size in order to be compatible with the PE loader. Otherwise use the actual optional header size. */
    if (p_section_table) {
        ROSE_ASSERT(p_section_table->get_offset() >= get_offset() + sizeof(PEFileHeader_disk));
        p_e_nt_hdr_size = p_section_table->get_offset() - (get_offset() + sizeof(PEFileHeader_disk));
    } else if (4==get_word_size()) {
        p_e_nt_hdr_size = sizeof(PE32OptHeader_disk);
    } else if (8==get_word_size()) {
        p_e_nt_hdr_size = sizeof(PE64OptHeader_disk);
    } else {
        throw FormatError("invalid PE word size");
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
        char p2[256];
        sprintf(p2, "%s.pair[%d].", p, i);
        w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p2));
        fprintf(f, "%s%-*s = rva %s,\tsize 0x%08"PRIx64" (%"PRIu64")\n", p2, w, "..",
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

    /* Show the simulated loader memory map */
    const MemoryMap *map = get_loader_map();
    if (map) {
        map->dump(f, (std::string(p)+"loader_map: ").c_str());
    } else {
        fprintf(f, "%s%-*s = not defined\n", p, w, "loader_map");
    }
}
