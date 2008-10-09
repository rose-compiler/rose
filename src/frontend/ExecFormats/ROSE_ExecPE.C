/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// namespace Exec {
// namespace PE {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extended DOS File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SgAsmPEExtendedDOSHeader::ctor(SgAsmGenericFile *f, addr_t offset)
{
    set_name("Extended DOS Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Decode */
    ExtendedDOSHeader_disk disk;
    content(0, sizeof disk, &disk);

    for (size_t i=0; i<NELMTS(disk.e_res1); i++)
        p_e_res1.push_back(le_to_host(disk.e_res1[i]));

    p_e_oemid             = le_to_host(disk.e_oemid);
    p_e_oeminfo           = le_to_host(disk.e_oeminfo);

    for (size_t i=0; i< NELMTS(disk.e_res2); i++)
        p_e_res2.push_back(le_to_host(disk.e_res2[i]));

    p_e_lfanew            = le_to_host(disk.e_lfanew);
}

/* Encode the extended header back into disk format */
void *
SgAsmPEExtendedDOSHeader::encode(ExtendedDOSHeader_disk *disk)
{
    for (size_t i=0; i < NELMTS(disk->e_res1); i++)
        host_to_le(p_e_res1[i], &(disk->e_res1[i]));
    host_to_le(p_e_oemid,    &(disk->e_oemid));
    host_to_le(p_e_oeminfo,  &(disk->e_oeminfo));
    for (size_t i=0; i<NELMTS(disk->e_res2); i++)
        host_to_le(p_e_res2[i], &(disk->e_res2[i]));
    host_to_le(p_e_lfanew,   &(disk->e_lfanew));
    return disk;
}

/* Write an extended header back to disk */
void
SgAsmPEExtendedDOSHeader::unparse(FILE *f)
{
    ExtendedDOSHeader_disk disk;
    encode(&disk);
    write(f, 0, sizeof disk, &disk);
}
    
void
SgAsmPEExtendedDOSHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExtendedDOSHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExtendedDOSHeader.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res1[0]",   p_e_res1[0]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res1[1]",   p_e_res1[1]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res1[2]",   p_e_res1[2]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res1[3]",   p_e_res1[3]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_oemid",    p_e_oemid);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_oeminfo",  p_e_oeminfo);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[0]",  p_e_res2[0]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[1]",  p_e_res2[1]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[2]",  p_e_res2[2]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[3]",  p_e_res2[3]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[4]",  p_e_res2[4]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[5]",  p_e_res2[5]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[6]",  p_e_res2[6]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[7]",  p_e_res2[7]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[8]",  p_e_res2[8]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[9]",  p_e_res2[9]);
    fprintf(f, "%s%-*s = %"PRIu64" byte offset (0x%"PRIx64")\n",  p, w, "e_lfanew",   p_e_lfanew,p_e_lfanew);

    hexdump(f, get_offset(), std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
void
SgAsmPEFileHeader::ctor(SgAsmGenericFile *f, addr_t offset)
{

    set_name("PE File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

 // DQ (8/16/2008): Added code to set SgAsmPEFileHeader as parent of input SgAsmGenericFile
    f->set_parent(this);

    p_rvasize_pairs = new SgAsmPERVASizePairList;

    p_rvasize_pairs->set_parent(this);

    PEFileHeader_disk fh;
    content(0, sizeof fh, &fh);

    /* Check magic number before getting too far */
    if (fh.e_magic[0]!='P' || fh.e_magic[1]!='E' || fh.e_magic[2]!='\0' || fh.e_magic[3]!='\0')
        throw FormatError("Bad PE magic number");

    /* Decode file header */
    p_e_cpu_type           = le_to_host(fh.e_cpu_type);
    p_e_nsections          = le_to_host(fh.e_nsections);
    p_e_time               = le_to_host(fh.e_time);
    p_e_coff_symtab        = le_to_host(fh.e_coff_symtab);
    p_e_coff_nsyms         = le_to_host(fh.e_coff_nsyms);
    p_e_nt_hdr_size        = le_to_host(fh.e_nt_hdr_size);
    p_e_flags              = le_to_host(fh.e_flags);

    /* Read the optional header, the size of which is stored in the e_nt_hdr_size of the main PE file header. According to
     * http://www.phreedom.org/solar/code/tinype the Windows loader honors the e_nt_hdr_size even when set to smaller than the
     * smallest possible documented size of the optional header. Also it's possible for the optional header to extend beyond
     * the end of the file, in which case that part should be read as zero. */
    PE32OptHeader_disk oh32;
    memset(&oh32, 0, sizeof oh32);
    addr_t need32 = std::min(p_e_nt_hdr_size, (addr_t)(sizeof oh32));
    extend_up_to(need32);
    content(sizeof fh, sizeof oh32, &oh32);
    p_e_opt_magic = le_to_host(oh32.e_opt_magic);
    
    /* Decode the optional header. */
    if (0x010b == p_e_opt_magic) {                                           
        p_exec_format->set_word_size(4);
        p_e_lmajor             = le_to_host(oh32.e_lmajor);
        p_e_lminor             = le_to_host(oh32.e_lminor);
        p_e_code_size          = le_to_host(oh32.e_code_size);
        p_e_data_size          = le_to_host(oh32.e_data_size);
        p_e_bss_size           = le_to_host(oh32.e_bss_size);
        p_e_entrypoint_rva     = le_to_host(oh32.e_entrypoint_rva);
        p_e_code_rva           = le_to_host(oh32.e_code_rva);
        p_e_data_rva           = le_to_host(oh32.e_data_rva);
        p_e_image_base         = le_to_host(oh32.e_image_base);
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
    } else if (0x020b == p_e_opt_magic) {
        /* We guessed wrong so extend and read the 64-bit header. */
        p_exec_format->set_word_size(8);
        PE64OptHeader_disk oh64;
        memset(&oh64, 0, sizeof oh64);
        addr_t need64 = std::min(p_e_nt_hdr_size, (addr_t)(sizeof oh64));
        extend_up_to(need64 - need32);
        content(sizeof fh, sizeof oh64, &oh64);
        p_e_lmajor             = le_to_host(oh64.e_lmajor);
        p_e_lminor             = le_to_host(oh64.e_lminor);
        p_e_code_size          = le_to_host(oh64.e_code_size);
        p_e_data_size          = le_to_host(oh64.e_data_size);
        p_e_bss_size           = le_to_host(oh64.e_bss_size);
        p_e_entrypoint_rva     = le_to_host(oh64.e_entrypoint_rva);
        p_e_code_rva           = le_to_host(oh64.e_code_rva);
     // p_e_data_rva         = le_to_host(oh.e_data_rva); /* not in PE32+ */
        p_e_image_base         = le_to_host(oh64.e_image_base);
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
    for (size_t i = 0; i < sizeof(fh.e_magic); ++i)
        p_magic.push_back(fh.e_magic[i]);

    /* File format */
    p_exec_format->set_family(FAMILY_PE);
    p_exec_format->set_purpose( p_e_flags & HF_PROGRAM ? PURPOSE_EXECUTABLE : PURPOSE_LIBRARY );
    p_exec_format->set_sex( ORDER_LSB );
    p_exec_format->set_abi( ABI_NT );
    p_exec_format->set_abi_version(0);
 // p_exec_format.word_size   = ???; /*word size was set above*/
    ROSE_ASSERT(p_e_lmajor <= 0xffff && p_e_lminor <= 0xffff);
    p_exec_format->set_version( (p_e_lmajor << 16) | p_e_lminor );
    p_exec_format->set_is_current_version( true ); /*FIXME*/

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
        set_isa(ISA_OTHER);
        break;
    }

    /* Entry point */
    p_base_va = p_e_image_base;
    add_entry_rva(p_e_entrypoint_rva);
}

SgAsmPEFileHeader::~SgAsmPEFileHeader() 
{
 // printf ("In ~SgAsmPEFileHeader() \n");

    ROSE_ASSERT(p_rvasize_pairs->get_pairs().empty() == true);
    
 // Delete the pointers to the IR nodes containing the STL lists
    delete p_rvasize_pairs;
    p_rvasize_pairs = NULL;
}

/* Encode the PE header into disk format */
void *
SgAsmPEFileHeader::encode(PEFileHeader_disk *disk)
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
SgAsmPEFileHeader::encode(PE32OptHeader_disk *disk)
{
    host_to_le(p_e_opt_magic,          &(disk->e_opt_magic));
    host_to_le(p_e_lmajor,             &(disk->e_lmajor));
    host_to_le(p_e_lminor,             &(disk->e_lminor));
    host_to_le(p_e_code_size,          &(disk->e_code_size));
    host_to_le(p_e_data_size,          &(disk->e_data_size));
    host_to_le(p_e_bss_size,           &(disk->e_bss_size));
    host_to_le(p_e_entrypoint_rva,     &(disk->e_entrypoint_rva));
    host_to_le(p_e_code_rva,           &(disk->e_code_rva));
    host_to_le(p_e_data_rva,           &(disk->e_data_rva));
    host_to_le(p_e_image_base,         &(disk->e_image_base));
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
SgAsmPEFileHeader::encode(PE64OptHeader_disk *disk)
{
    host_to_le(p_e_opt_magic,          &(disk->e_opt_magic));
    host_to_le(p_e_lmajor,             &(disk->e_lmajor));
    host_to_le(p_e_lminor,             &(disk->e_lminor));
    host_to_le(p_e_code_size,          &(disk->e_code_size));
    host_to_le(p_e_data_size,          &(disk->e_data_size));
    host_to_le(p_e_bss_size,           &(disk->e_bss_size));
    host_to_le(p_e_entrypoint_rva,     &(disk->e_entrypoint_rva));
    host_to_le(p_e_code_rva,           &(disk->e_code_rva));
 // host_to_le(p_e_data_rva,           &(disk->e_data_rva)); /* not present in PE32+ */
    host_to_le(p_e_image_base,         &(disk->e_image_base));
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

    extend_up_to(pairs_size);
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
          case 0:  tabname = "Export table";                 break;
          case 1:  tabname = "Import table";                 break;
          case 2:  tabname = "Resource table";               break;
          case 3:  tabname = "Exception table";              break;
          case 4:  tabname = "Certificate table";            break;
          case 5:  tabname = "Base relocation table";        break;
          case 6:  tabname = "Debug";                        break;
          case 7:  tabname = "Architecture";                 break;
          case 8:  tabname = "Global ptr";                   break;
          case 9:  tabname = "TLS table";                    break;
          case 10: tabname = "Load config table";            break;
          case 11: tabname = "Bound import";                 break;
          case 12: tabname = "Import address table";         break;
          case 13: tabname = "Delay import descriptor";      break;
          case 14: tabname = "CLR runtime header";           break;
          case 15: ROSE_ASSERT(!"reserved; should be zero"); break;
          default: ROSE_ASSERT(!"too many RVA/Size pairs");  break;
        }

        /* Find a section that contains the starting RVA of the table. We use that to find the file offset. */
        SgAsmGenericFile *ef = get_file();
        SgAsmGenericSection *contained_in = ef->get_section_by_va(get_base_va()+pair->get_e_rva());
        if (!contained_in) {
            fprintf(stderr, "SgAsmPEFileHeader::create_table_sections(): pair-%zu, rva=0x%08"PRIx64", size=%"PRIu64" bytes \"%s\""
                    ": unable to find a section containing the virtual address (skipping)\n",
                    i, pair->get_e_rva(), pair->get_e_size(), tabname?tabname:"");
            continue;
        }
        addr_t file_offset = contained_in->get_rva_offset(pair->get_e_rva());
        

        SgAsmGenericSection *tabsec = new SgAsmGenericSection(ef, this, file_offset, pair->get_e_size());
        if (tabname) tabsec->set_name(tabname);
        tabsec->set_synthesized(true);
        tabsec->set_purpose(SP_HEADER);
        tabsec->set_mapped(pair->get_e_rva(), pair->get_e_size());
        tabsec->set_rperm(true);
        tabsec->set_wperm(false);
        tabsec->set_eperm(false);
        pair->set_section(tabsec);
    }
}

/* Write the PE file header back to disk and all that it references */
void
SgAsmPEFileHeader::unparse(FILE *f)
{
    /* The fixed length part of the header */
    PEFileHeader_disk fh;
    encode(&fh);

    /* The optional header */
    PE32OptHeader_disk oh32;
    PE64OptHeader_disk oh64;
    void *oh       = NULL;
    size_t oh_size = 0;

    if (4==get_word_size()) {
        oh = encode(&oh32);
        oh_size = sizeof oh32;
    } else if (8==get_word_size()) {
        oh = encode(&oh64);
        oh_size = sizeof oh64;
    } else {
        ROSE_ASSERT(!"unsupported word size");
    }
    
    /* Write file and optional header to disk */
    addr_t spos = write(f, 0, sizeof fh, &fh);
    spos = write(f, spos, oh_size, oh);

    /* The variable length RVA/size pair table and its sections */
    for (size_t i = 0; i < p_e_num_rvasize_pairs; i++) {
        SgAsmPERVASizePair::RVASizePair_disk rvasize_disk;
        p_rvasize_pairs->get_pairs()[i]->encode(&rvasize_disk);
        spos = write(f, spos, sizeof rvasize_disk, &rvasize_disk);
        SgAsmGenericSection *sizepair_section = p_rvasize_pairs->get_pairs()[i]->get_section();
        if (sizepair_section)
            sizepair_section->unparse(f);
    }

    /* The extended DOS header */
    if (p_dos2_header)
        p_dos2_header->unparse(f);

    /* The section table and all the non-synthesized sections */
    if (p_section_table)
        p_section_table->unparse(f);

    /* Sections that aren't in the section table */
    if (p_coff_symtab)
        p_coff_symtab->unparse(f);
}
    
/* Print some debugging information */
void
SgAsmPEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
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
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_cpu_type",          p_e_cpu_type);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_nsections",         p_e_nsections);
    fprintf(f, "%s%-*s = %u (%s)\n",       p, w, "e_time",              p_e_time, time_str);
    fprintf(f, "%s%-*s = %"PRIu64"\n",     p, w, "e_coff_symtab",       p_e_coff_symtab);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_coff_nsyms",        p_e_coff_nsyms);
    if (p_coff_symtab) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "coff_symtab", p_coff_symtab->get_id(), p_coff_symtab->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",      p, w, "coff_symtab");
    }
    fprintf(f, "%s%-*s = %"PRIu64"\n",     p, w, "e_nt_hdr_size",       p_e_nt_hdr_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_flags",             p_e_flags);
    fprintf(f, "%s%-*s = 0x%04x %s\n",     p, w, "e_opt_magic",         p_e_opt_magic,
            0x10b == p_e_opt_magic ? "PE32" : (0x20b == p_e_opt_magic ? "PE32+" : "other"));
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_lmajor",            p_e_lmajor);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_lminor",            p_e_lminor);
    fprintf(f, "%s%-*s = %u bytes\n",      p, w, "e_code_size",         p_e_code_size);
    fprintf(f, "%s%-*s = %u bytes\n",      p, w, "e_data_size",         p_e_data_size);
    fprintf(f, "%s%-*s = %u bytes\n",      p, w, "e_bss_size",          p_e_bss_size);
    fprintf(f, "%s%-*s = 0x%08x\n",        p, w, "e_entrypoint_rva",    p_e_entrypoint_rva);
    fprintf(f, "%s%-*s = 0x%08x\n",        p, w, "e_code_rva",          p_e_code_rva);
    fprintf(f, "%s%-*s = 0x%08x\n",        p, w, "e_data_rva",          p_e_data_rva);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "e_image_base",        p_e_image_base);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_section_align",     p_e_section_align);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_file_align",        p_e_file_align);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_os_major",          p_e_os_major);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_os_minor",          p_e_os_minor);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_user_major",        p_e_user_major);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_user_minor",        p_e_user_minor);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_subsys_major",      p_e_subsys_major);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_subsys_minor",      p_e_subsys_minor);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_reserved9",         p_e_reserved9);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_image_size",        p_e_image_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_header_size",       p_e_header_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_file_checksum",     p_e_file_checksum);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_subsystem",         p_e_subsystem);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_dll_flags",         p_e_dll_flags);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_stack_reserve_size",p_e_stack_reserve_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_stack_commit_size", p_e_stack_commit_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_heap_reserve_size", p_e_heap_reserve_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_heap_commit_size",  p_e_heap_commit_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_loader_flags",      p_e_loader_flags);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_num_rvasize_pairs", p_e_num_rvasize_pairs);
    for (unsigned i = 0; i < p_e_num_rvasize_pairs; i++) {
        sprintf(p, "%sPEFileHeader.pair[%d].", prefix, i);
        w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
        fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",  p, w, "e_rva",  p_rvasize_pairs->get_pairs()[i]->get_e_rva());
        fprintf(f, "%s%-*s = %" PRIu64 " bytes\n", p, w, "e_size", p_rvasize_pairs->get_pairs()[i]->get_e_size());
    }
    if (p_dos2_header) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "dos2_header", p_dos2_header->get_id(), p_dos2_header->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "dos2_header");
    }
    if (p_section_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "section_table",
                p_section_table->get_id(), p_section_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "section_table");
    }

    hexdump(f, get_offset(), std::string(p)+"data at ", p_data);
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
SgAsmPESectionTableEntry::encode(PESectionTableEntry_disk *disk)
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
SgAsmPESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
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
SgAsmPESection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    p_st_entry->dump(f, p, -1);

    hexdump(f, get_offset(), std::string(p)+"data at ", p_data);
}

/* Constructor */
void
SgAsmPESectionTable::ctor()
{
    set_synthesized(true);
    set_name("PE Section Table");
    set_purpose(SP_HEADER);

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
            section = new SgAsmPEImportSection(fhdr, entry->get_physical_offset(), entry->get_physical_size(), entry->get_rva());
        } else {
            section = new SgAsmPESection(fhdr, entry->get_physical_offset(), entry->get_physical_size());
        }
        section->set_synthesized(false);
        section->set_name(entry->get_name());
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);

     // DQ (8/18/2008): I think we need to set the parent explicit here, but I am not certain what to set it to be (using "this" is a default).
        section->set_parent(this);

        section->set_mapped(entry->get_rva(), entry->get_virtual_size());
        section->set_st_entry(entry);
        section->set_rperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_READABLE) ==
                           SgAsmPESectionTableEntry::OF_READABLE);
        section->set_wperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_WRITABLE) ==
                           SgAsmPESectionTableEntry::OF_WRITABLE);
        section->set_eperm((entry->get_flags() & SgAsmPESectionTableEntry::OF_EXECUTABLE) ==
                           SgAsmPESectionTableEntry::OF_EXECUTABLE);
        
        if (entry->get_flags() & (SgAsmPESectionTableEntry::OF_CODE|
                                  SgAsmPESectionTableEntry::OF_IDATA|
                                  SgAsmPESectionTableEntry::OF_UDATA))
            section->set_purpose(SP_PROGRAM);
    }
}

/* Writes the section table back to disk along with each of the sections. */
void
SgAsmPESectionTable::unparse(FILE *f)
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
SgAsmPESectionTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESectionTable.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);

    hexdump(f, get_offset(), std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Import Directory (".idata" section)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmPEImportDirectory::ctor(const PEImportDirectory_disk *disk)
{
    p_hintnames_rva   = le_to_host(disk->hintnames_rva);
    p_time            = le_to_host(disk->time);
    p_forwarder_chain = le_to_host(disk->forwarder_chain);
    p_dll_name_rva    = le_to_host(disk->dll_name_rva);
    p_bindings_rva    = le_to_host(disk->bindings_rva);
}

/* Encode a directory entry back into disk format */
void *
SgAsmPEImportDirectory::encode(PEImportDirectory_disk *disk)
{
    host_to_le(p_hintnames_rva,   &(disk->hintnames_rva));
    host_to_le(p_time,            &(disk->time));
    host_to_le(p_forwarder_chain, &(disk->forwarder_chain));
    host_to_le(p_dll_name_rva,    &(disk->dll_name_rva));
    host_to_le(p_bindings_rva,    &(disk->bindings_rva));
    return disk;
}

/* Print debugging info */
void
SgAsmPEImportDirectory::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportDirectory[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportDirectory.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "hintnames_rva",   p_hintnames_rva);
    fprintf(f, "%s%-*s = %lu %s",          p, w, "time",            (unsigned long)p_time, ctime(&p_time));
    fprintf(f, "%s%-*s = %u\n",            p, w, "forwarder_chain", p_forwarder_chain);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "dll_name_rva",    p_dll_name_rva);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "bindings_rva",    p_bindings_rva);
}

/* Constructor */
void
SgAsmPEImportHintName::ctor(SgAsmGenericSection *section, addr_t offset)
{
    PEImportHintName_disk disk;
    section->content(offset, sizeof disk, &disk);
    p_hint = le_to_host(disk.hint);
    p_name = section->content_str(offset+sizeof disk);
    p_padding = (p_name.size()+1) % 2 ? *(section->content(offset + sizeof(disk)+ p_name.size() + 1, 1)) : '\0';
}

/* Writes the hint/name back to disk at the specified offset */
void
SgAsmPEImportHintName::unparse(FILE *f, SgAsmGenericSection *section, addr_t spos)
{
    /* The hint */
    uint16_t hint_le;

 // DQ (8/16/2008): Assertion on p_hint in host_to_le
    assert(0==(p_hint & ~0xffff));

    host_to_le(p_hint, &hint_le);
    spos = section->write(f, spos, sizeof hint_le, &hint_le);

    /* NUL-terminated name */
    spos = section->write(f, spos, p_name);
    spos = section->write(f, spos, '\0');
    
    /* Padding to make an even size */
    if ((p_name.size() + 1) % 2)
        section->write(f, spos, p_padding);
}

/* Print debugging info */
void
SgAsmPEImportHintName::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sImporNameHint[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sImporNameHint.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u\n",     p, w, "hint",    p_hint);
    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name",    p_name.c_str());
    fprintf(f, "%s%-*s = 0x%02x\n", p, w, "padding", p_padding);
}

/* Constructor */
void
SgAsmPEDLL::ctor(const std::string&) 
{
    p_hintnames = new SgAsmPEImportHintNameList;
    p_hintnames->set_parent(this);
}

/* Print debugging info */
void
SgAsmPEDLL::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEDLL[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEDLL.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    SgAsmGenericDLL::dump(f, p, -1);
    if (p_idir)
        p_idir->dump(f, p, -1);
    for (size_t i=0; i < p_hintname_rvas.size(); i++) {
        fprintf(f, "%s%-*s = [%zu] 0x%08"PRIx64"\n", p, w, "hintname_rva", i, p_hintname_rvas[i]);
        fprintf(f, "%s%-*s = [%zu] 0x%08"PRIx64"\n", p, w, "binding", i, p_bindings[i]);
    }
}

/* Constructor */
void
SgAsmPEImportSection::ctor(addr_t offset, addr_t size, addr_t mapped_rva)
{
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    size_t entry_size = sizeof(SgAsmPEImportDirectory::PEImportDirectory_disk);
    SgAsmPEImportDirectory::PEImportDirectory_disk zero;
    memset(&zero, 0, sizeof zero);

    p_dlls = new SgAsmPEDLLList();
    p_dlls->set_parent(this);

    /* Read idata directory entries--one per DLL*/
    for (size_t i = 0; 1; i++) {
        /* End of list is marked by an entry of all zero. */
        SgAsmPEImportDirectory::PEImportDirectory_disk idir_disk;
        content(i*entry_size, entry_size, &idir_disk);
        if (!memcmp(&zero, &idir_disk, sizeof zero)) break;
        SgAsmPEImportDirectory *idir = new SgAsmPEImportDirectory(&idir_disk);

        /* The library's name is indicated by RVA. We need a section offset instead. */
        ROSE_ASSERT(idir->get_dll_name_rva() >= mapped_rva);
        addr_t dll_name_offset = idir->get_dll_name_rva() - mapped_rva;
        std::string dll_name = content_str(dll_name_offset);

        /* Create the DLL objects */
        SgAsmPEDLL *dll = new SgAsmPEDLL(dll_name);
        dll->set_idir(idir);

        idir->set_parent(dll);

        /* The idir->hintname_rvas is an (optional) RVA for a NULL-terminated array whose members are either:
         *    1. an RVA of a hint/name pair (if the high-order bit of the array member is clear)
         *    2. an ordinal if the high-order bit is set */
        if (idir->get_hintnames_rva() != 0) {
            if (idir->get_hintnames_rva() < mapped_rva)
                throw FormatError("hint/name RVA is before beginning of \".idata\" object");

         // DQ (8/11/2008): Should this really be a "for loop"; test is "1;"
            for (addr_t hintname_rvas_offset = idir->get_hintnames_rva() - mapped_rva; /*section offset of RVA/ordinal array */
                 1; 
                 hintname_rvas_offset += fhdr->get_word_size()) {
                addr_t hintname_rva = 0; /*RVA of the hint/name pair*/
                bool import_by_ordinal=false; /*was high-order bit of array element set?*/
                if (4==fhdr->get_word_size()) {
                    uint32_t word;
                    content(hintname_rvas_offset, sizeof word, &word);
                    hintname_rva = le_to_host(word);
                    import_by_ordinal = (hintname_rva & 0x80000000) != 0;
                } else if (8==fhdr->get_word_size()) {
                    uint64_t word;
                    content(hintname_rvas_offset, sizeof word, &word);
                    hintname_rva = le_to_host(word);
                    import_by_ordinal = (hintname_rva & 0x8000000000000000ull) != 0;
                } else {
                    ROSE_ASSERT(!"unsupported word size");
                }

                /* Array is NULL terminated */
                if (0==hintname_rva)
                    break;

                /* Add arrary value and hint/name pair to the DLL object */
                dll->add_hintname_rva(hintname_rva);
                if (!import_by_ordinal) {
                    addr_t hintname_offset = (hintname_rva & 0x7fffffff) - mapped_rva;
                    SgAsmPEImportHintName *hintname = new SgAsmPEImportHintName(this, hintname_offset);
                    dll->add_function(hintname->get_name());
                    dll->add_hintname(hintname);

                 // Make the parent the list IR node
                    hintname->set_parent(dll->get_hintnames());
                }
            }
        }
        
        /* The idir->bindings_rva is a NULL-terminated array of RVAs */
        if (idir->get_bindings_rva() != 0) {
            if (idir->get_bindings_rva() < mapped_rva)
                throw FormatError("bindings RVA is before beginning of \".idata\" object");

            for (addr_t bindings_offset  = idir->get_bindings_rva()  - mapped_rva; /*section offset of RVA array for bindings*/
                 1; 
                 bindings_offset += fhdr->get_word_size()) {
                addr_t binding=0;
                if (4==fhdr->get_word_size()) {
                    uint32_t word;
                    content(bindings_offset, sizeof word, &word);
                    binding = le_to_host(word);
                } else if (8==fhdr->get_word_size()) {
                    uint64_t word;
                    content(bindings_offset, sizeof word, &word);
                    binding = le_to_host(word);
                } else {
                    ROSE_ASSERT(!"unsupported word size");
                }

                /* Array is NULL terminated; otherwise add binding to DLL object */
                if (0==binding)
                    break;
                dll->add_binding(binding);
            }
        }

        /* Add dll to both this section and to the file header */
        add_dll(dll);
        fhdr->add_dll(dll);
    }
}

/* Write the import section back to disk */
void
SgAsmPEImportSection::unparse(FILE *f)
{
    SgAsmGenericHeader *fhdr = get_header();

 // This is the same as accessing p_dlls and used to be redundant with "dlls" before being use with RISE IR nodes.
 // const std::vector<SgAsmPEDLL*> & dlls = get_dlls();
    const std::vector<SgAsmPEDLL*> & dlls = get_dlls()->get_dlls();

    for (size_t dllno = 0; dllno < dlls.size(); dllno++) {
        SgAsmPEDLL *dll = dlls[dllno];
        SgAsmPEImportDirectory *idir = dll->get_idir();
        
        /* Directory entry */
        SgAsmPEImportDirectory::PEImportDirectory_disk idir_disk;
        idir->encode(&idir_disk);
        write(f, dllno*sizeof(idir_disk), sizeof idir_disk, &idir_disk);

        /* Library name */
        ROSE_ASSERT(idir->get_dll_name_rva() >= p_mapped_rva);
        addr_t dll_name_offset = idir->get_dll_name_rva() - p_mapped_rva;
        ROSE_ASSERT(dll_name_offset + dll->get_name().size() + 1 < get_size());
        addr_t spos = write(f, dll_name_offset, dll->get_name());
        write(f, spos, '\0');

        /* Write the hint/name pairs and the array entries that point to them. */
        if (idir->get_hintnames_rva() != 0) {
            ROSE_ASSERT(idir->get_hintnames_rva() >= p_mapped_rva);
            addr_t hintname_rvas_spos = idir->get_hintnames_rva() - p_mapped_rva; /*section offset*/
            const std::vector<addr_t> & hintname_rvas = dll->get_hintname_rvas();
            const std::vector<SgAsmPEImportHintName*> & hintnames = dll->get_hintnames()->get_hintnames();
            for (size_t i = 0; i <= hintname_rvas.size(); i++) {
                /* Hint/name RVA */
                addr_t hintname_rva = i < hintname_rvas.size() ? hintname_rvas[i] : 0; /*zero terminated*/
                bool import_by_ordinal = false;
                if (4 == fhdr->get_word_size()) {
                    uint32_t rva_le;
                    host_to_le(hintname_rva, &rva_le);
                    write(f, hintname_rvas_spos + i*fhdr->get_word_size(), sizeof rva_le, &rva_le);
                    import_by_ordinal = (hintname_rva & 0x80000000) != 0;
                } else if (8==fhdr->get_word_size()) {
                    uint64_t rva_le;
                    host_to_le(hintname_rva, &rva_le);
                    write(f, hintname_rvas_spos + i*fhdr->get_word_size(), sizeof rva_le, &rva_le);
                    import_by_ordinal = (hintname_rva & 0x8000000000000000ull) != 0;
                } else {
                    ROSE_ASSERT(!"unsupported word size");
                }
            
                /* Hint/name pair */
                if (i<hintname_rvas.size() && !import_by_ordinal) {
                    addr_t hintname_spos = (hintname_rvas[i] & 0x7fffffff) - p_mapped_rva; /*section offset*/
                    hintnames[i]->unparse(f, this, hintname_spos);
                }
            }
        }
        
        /* Write the bindings array */
        if (idir->get_bindings_rva() != 0) {
            ROSE_ASSERT(idir->get_bindings_rva() >= p_mapped_rva);
            const addr_t bindings_spos = idir->get_bindings_rva() - p_mapped_rva; /*section offset*/
            const std::vector<addr_t> & bindings = dll->get_bindings();
            for (size_t i=0; i<=bindings.size(); i++) {
                addr_t binding = i<bindings.size() ? bindings[i] : 0; /*zero terminated*/
                if (4==fhdr->get_word_size()) {
                    uint32_t binding_le;
                    host_to_le(binding, &binding_le);
                    write(f, bindings_spos + i*fhdr->get_word_size(), sizeof binding_le, &binding_le);
                } else if (8==fhdr->get_word_size()) {
                    uint64_t binding_le;
                    host_to_le(binding, &binding_le);
                    write(f, bindings_spos + i*fhdr->get_word_size(), sizeof binding_le, &binding_le);
                } else {
                    ROSE_ASSERT(!"unsupported word size");
                }
            }
        }
    }

    /* DLL list is zero terminated */
    {
        SgAsmPEImportDirectory::PEImportDirectory_disk zero;
        memset(&zero, 0, sizeof zero);
        write(f, dlls.size()*sizeof zero, sizeof zero, &zero);
    }

    unparse_holes(f);
}

/* Print debugging info */
void
SgAsmPEImportSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    SgAsmPESection::dump(f, p, -1);

    hexdump(f, get_offset(), std::string(p)+"data at ", p_data);
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
        set_name(strtab->content_str(p_st_name_offset));
    } else {
        char temp[9];
        memcpy(temp, disk.st_name, 8);
        temp[8] = '\0';
        set_name(temp);
        p_st_name_offset = 0;
    }

    p_st_name            = get_name();
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
                set_name(strtab->content_str(fname_offset));
                if (debug)
                    fprintf(stderr, "COFF aux file: offset=%"PRIu64", name=\"%s\"\n", fname_offset, get_name().c_str());
            } else {
                /* Aux data contains a NUL-padded name; the NULs (if any) are not part of the name. */
                ROSE_ASSERT(p_st_num_aux_entries == 1);
                char fname[COFFSymbol_disk_size+1];
                memcpy(fname, &(p_aux_data[0]), COFFSymbol_disk_size);
                fname[COFFSymbol_disk_size] = '\0';
                set_name(fname);
                if (debug)
                    fprintf(stderr, "COFF aux file: inline-name=\"%s\"\n", get_name().c_str());
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
SgAsmCoffSymbol::encode(COFFSymbol_disk *disk)
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
SgAsmCoffSymbol::dump(FILE *f, const char *prefix, ssize_t idx)
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
    set_name("COFF Symbols");
    set_purpose(SP_SYMTAB);

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    p_symbols = new SgAsmCoffSymbolList;
    p_symbols->set_parent(this);

    /* The string table immediately follows the symbols. The first four bytes of the string table are the size of the
     * string table in little endian. */
    addr_t strtab_offset = get_offset() + fhdr->get_e_coff_nsyms() * SgAsmCoffSymbol::COFFSymbol_disk_size;
    p_strtab = new SgAsmGenericSection(fhdr->get_file(), fhdr, strtab_offset, sizeof(uint32_t));
    p_strtab->set_synthesized(true);
    p_strtab->set_name("COFF Symbol Strtab");
    p_strtab->set_purpose(SP_HEADER);

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
}

/* Write symbol table back to disk */
void
SgAsmCoffSymbolTable::unparse(FILE *f)
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
SgAsmCoffSymbolTable::dump(FILE *f, const char *prefix, ssize_t idx)
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

    hexdump(f, get_offset(), std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns true if a cursory look at the file indicates that it could be a PE file. */
bool
SgAsmPEFileHeader::is_PE(SgAsmGenericFile *f)
{
    SgAsmDOSFileHeader       *dos_hdr  = NULL;
    SgAsmPEExtendedDOSHeader *dos2_hdr = NULL;
    SgAsmPEFileHeader        *pe_hdr   = NULL;

    bool retval  = false;

    try {
        dos_hdr  = new SgAsmDOSFileHeader(f, 0);
        dos2_hdr = new SgAsmPEExtendedDOSHeader(f, dos_hdr->get_size());
        pe_hdr   = new SgAsmPEFileHeader(f, dos2_hdr->get_e_lfanew());
        retval   = true;
    } catch (...) {
        /* cleanup is below */
    }

    delete dos_hdr;
    delete dos2_hdr;
    delete pe_hdr;
    return retval;
}


/* Parses the structure of a PE file and adds the information to the ExecFile. */
SgAsmPEFileHeader *
SgAsmPEFileHeader::parse(SgAsmGenericFile *ef)
{
    ROSE_ASSERT(ef);

    /* All PE files are also DOS files, so parse the DOS part first */
    SgAsmDOSFileHeader *dos_header = SgAsmDOSFileHeader::parse(ef, false);

    /* PE files extend the DOS header with some additional info */
    SgAsmPEExtendedDOSHeader *dos2_header = new SgAsmPEExtendedDOSHeader(ef, dos_header->get_size());
    
    /* The PE header has a fixed-size component followed by some number of RVA/Size pairs */
    SgAsmPEFileHeader *pe_header = new SgAsmPEFileHeader(ef, dos2_header->get_e_lfanew());
    ROSE_ASSERT(pe_header->get_e_num_rvasize_pairs() < 1000); /* just a sanity check before we allocate memory */
    pe_header->add_rvasize_pairs();

    /* The extended part of the DOS header is owned by the PE header */
    dos2_header->set_header(pe_header);
    pe_header->add_section(dos2_header);
    pe_header->set_dos2_header(dos2_header);

    /* Now go back and add the DOS Real-Mode section but rather than using the size specified in the DOS header, constrain it
     * to not extend beyond the beginning of the PE file header. This makes detecting holes in the PE format much easier. */
    dos_header->add_rm_section(pe_header->get_offset());

    /* Construct the section table and its sections (non-synthesized sections). The specification says that the section table
     * comes after the optional (NT) header, which in turn comes after the fixed part of the PE header. The size of the
     * optional header is indicated in the fixed header. */
    addr_t secttab_offset = pe_header->get_offset() + sizeof(PEFileHeader_disk) + pe_header->get_e_nt_hdr_size();
    addr_t secttab_size = pe_header->get_e_nsections() * sizeof(SgAsmPESectionTableEntry::PESectionTableEntry_disk);
    SgAsmPESectionTable *secttab = new SgAsmPESectionTable(pe_header, secttab_offset, secttab_size);
    pe_header->set_section_table(secttab);

    /* Parse the COFF symbol table and add symbols to the PE header */
    if (pe_header->get_e_coff_symtab() && pe_header->get_e_coff_nsyms()) {
        SgAsmCoffSymbolTable *symtab = new SgAsmCoffSymbolTable(pe_header);
        std::vector<SgAsmCoffSymbol*> & symbols = symtab->get_symbols()->get_symbols();
        for (size_t i = 0; i < symbols.size(); i++)
            pe_header->add_symbol(symbols[i]);
        pe_header->set_coff_symtab(symtab);
    }

    /* Turn header-specified tables (RVA/Size pairs) into generic sections */
    pe_header->create_table_sections();
    
    return pe_header;
}

// }; //namespace PE
// }; //namespace Exec
