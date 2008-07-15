/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace Exec {
namespace PE {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extended DOS File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
ExtendedDOSHeader::ctor(ExecFile *f, addr_t offset)
{
    set_name("Extended DOS Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Decode */
    const ExtendedDOSHeader_disk *disk = (const ExtendedDOSHeader_disk*)content(0, sizeof(ExtendedDOSHeader_disk));
    for (size_t i=0; i<NELMTS(e_res1); i++)
        e_res1[i]       = le_to_host(disk->e_res1[i]);
    e_oemid             = le_to_host(disk->e_oemid);
    e_oeminfo           = le_to_host(disk->e_oeminfo);
    for (size_t i=0; i<NELMTS(e_res2); i++)
        e_res2[i]       = le_to_host(disk->e_res2[i]);
    e_lfanew            = le_to_host(disk->e_lfanew);
}

/* Encode the extended header back into disk format */
void *
ExtendedDOSHeader::encode(ExtendedDOSHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_res1); i++)
        host_to_le(e_res1[i], &(disk->e_res1[i]));
    host_to_le(e_oemid,    &(disk->e_oemid));
    host_to_le(e_oeminfo,  &(disk->e_oeminfo));
    for (size_t i=0; i<NELMTS(disk->e_res2); i++)
        host_to_le(e_res2[i], &(disk->e_res2[i]));
    host_to_le(e_lfanew,   &(disk->e_lfanew));
    return disk;
}

/* Write an extended header back to disk */
void
ExtendedDOSHeader::unparse(FILE *f)
{
    ExtendedDOSHeader_disk disk;
    encode(&disk);
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwrite = fwrite(&disk, sizeof disk, 1, f);
    ROSE_ASSERT(1==nwrite);
}
    
void
ExtendedDOSHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sExtendedDOSHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sExtendedDOSHeader.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res1[0]",   e_res1[0]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res1[1]",   e_res1[1]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res1[2]",   e_res1[2]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res1[3]",   e_res1[3]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_oemid",    e_oemid);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_oeminfo",  e_oeminfo);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[0]",  e_res2[0]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[1]",  e_res2[1]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[2]",  e_res2[2]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[3]",  e_res2[3]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[4]",  e_res2[4]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[5]",  e_res2[5]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[6]",  e_res2[6]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[7]",  e_res2[7]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[8]",  e_res2[8]);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_res2[9]",  e_res2[9]);
    fprintf(f, "%s%-*s = %"PRIu64" byte offset\n",  p, w, "e_lfanew",   e_lfanew);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
void
PEFileHeader::ctor(ExecFile *f, addr_t offset)
{

    set_name("PE File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Decode file header */
    const PEFileHeader_disk *fh = (const PEFileHeader_disk*)content(0, sizeof(PEFileHeader_disk));
    e_cpu_type           = le_to_host(fh->e_cpu_type);
    e_nsections          = le_to_host(fh->e_nsections);
    e_time               = le_to_host(fh->e_time);
    e_coff_symtab        = le_to_host(fh->e_coff_symtab);
    e_coff_nsyms         = le_to_host(fh->e_coff_nsyms);
    e_nt_hdr_size        = le_to_host(fh->e_nt_hdr_size);
    e_flags              = le_to_host(fh->e_flags);

    /* Decode optional header. The version of header is based on a magic number */
    extend(2);
    e_opt_magic = le_to_host(*(const uint16_t*)content(sizeof(PEFileHeader_disk), 2));
    addr_t oh_offset = size;
    if (0x010b==e_opt_magic) {
        exec_format.word_size = 4;
        const PE32OptHeader_disk *oh;
        size_t oh_size = sizeof(*oh);
        extend(oh_size);
        oh = (const PE32OptHeader_disk*)content(oh_offset, oh_size);
        e_lmajor             = le_to_host(oh->e_lmajor);
        e_lminor             = le_to_host(oh->e_lminor);
        e_code_size          = le_to_host(oh->e_code_size);
        e_data_size          = le_to_host(oh->e_data_size);
        e_bss_size           = le_to_host(oh->e_bss_size);
        e_entrypoint_rva     = le_to_host(oh->e_entrypoint_rva);
        e_code_rva           = le_to_host(oh->e_code_rva);
        e_data_rva           = le_to_host(oh->e_data_rva);
        e_image_base         = le_to_host(oh->e_image_base);
        e_section_align      = le_to_host(oh->e_section_align);
        e_file_align         = le_to_host(oh->e_file_align);
        e_os_major           = le_to_host(oh->e_os_major);
        e_os_minor           = le_to_host(oh->e_os_minor);
        e_user_major         = le_to_host(oh->e_user_major);
        e_user_minor         = le_to_host(oh->e_user_minor);
        e_subsys_major       = le_to_host(oh->e_subsys_major);
        e_subsys_minor       = le_to_host(oh->e_subsys_minor);
        e_reserved9          = le_to_host(oh->e_reserved9);
        e_image_size         = le_to_host(oh->e_image_size);
        e_header_size        = le_to_host(oh->e_header_size);
        e_file_checksum      = le_to_host(oh->e_file_checksum);
        e_subsystem          = le_to_host(oh->e_subsystem);
        e_dll_flags          = le_to_host(oh->e_dll_flags);
        e_stack_reserve_size = le_to_host(oh->e_stack_reserve_size);
        e_stack_commit_size  = le_to_host(oh->e_stack_commit_size);
        e_heap_reserve_size  = le_to_host(oh->e_heap_reserve_size);
        e_heap_commit_size   = le_to_host(oh->e_heap_commit_size);
        e_loader_flags       = le_to_host(oh->e_loader_flags);
        e_num_rvasize_pairs  = le_to_host(oh->e_num_rvasize_pairs);
    } else if (0x020b==e_opt_magic) {
        exec_format.word_size = 8;
        const PE64OptHeader_disk *oh;
        size_t oh_size = sizeof(*oh);
        extend(oh_size);
        oh = (const PE64OptHeader_disk*)content(oh_offset, oh_size);
        e_lmajor             = le_to_host(oh->e_lmajor);
        e_lminor             = le_to_host(oh->e_lminor);
        e_code_size          = le_to_host(oh->e_code_size);
        e_data_size          = le_to_host(oh->e_data_size);
        e_bss_size           = le_to_host(oh->e_bss_size);
        e_entrypoint_rva     = le_to_host(oh->e_entrypoint_rva);
        e_code_rva           = le_to_host(oh->e_code_rva);
        //e_data_rva         = le_to_host(oh->e_data_rva); /* not in PE32+ */
        e_image_base         = le_to_host(oh->e_image_base);
        e_section_align      = le_to_host(oh->e_section_align);
        e_file_align         = le_to_host(oh->e_file_align);
        e_os_major           = le_to_host(oh->e_os_major);
        e_os_minor           = le_to_host(oh->e_os_minor);
        e_user_major         = le_to_host(oh->e_user_major);
        e_user_minor         = le_to_host(oh->e_user_minor);
        e_subsys_major       = le_to_host(oh->e_subsys_major);
        e_subsys_minor       = le_to_host(oh->e_subsys_minor);
        e_reserved9          = le_to_host(oh->e_reserved9);
        e_image_size         = le_to_host(oh->e_image_size);
        e_header_size        = le_to_host(oh->e_header_size);
        e_file_checksum      = le_to_host(oh->e_file_checksum);
        e_subsystem          = le_to_host(oh->e_subsystem);
        e_dll_flags          = le_to_host(oh->e_dll_flags);
        e_stack_reserve_size = le_to_host(oh->e_stack_reserve_size);
        e_stack_commit_size  = le_to_host(oh->e_stack_commit_size);
        e_heap_reserve_size  = le_to_host(oh->e_heap_reserve_size);
        e_heap_commit_size   = le_to_host(oh->e_heap_commit_size);
        e_loader_flags       = le_to_host(oh->e_loader_flags);
        e_num_rvasize_pairs  = le_to_host(oh->e_num_rvasize_pairs);
    } else {
        throw FormatError("unrecognized Windows PE optional header magic number");
    }

    /* Magic number */
    for (size_t i=0; i<sizeof(fh->e_magic); ++i)
        magic.push_back(fh->e_magic[i]);

    /* File format */
    exec_format.family      = FAMILY_PE;
    exec_format.purpose     = e_flags & HF_PROGRAM ? PURPOSE_EXECUTABLE : PURPOSE_LIBRARY;
    exec_format.sex         = ORDER_LSB;
    exec_format.abi         = ABI_NT;
    exec_format.abi_version = 0;
    //exec_format.word_size   = ???; /*word size was set above*/
    ROSE_ASSERT(e_lmajor<=0xffff && e_lminor<=0xffff);
    exec_format.version     = (e_lmajor<<16) | e_lminor;
    exec_format.is_current_version = true; /*FIXME*/

    /* Target architecture */
    switch (e_cpu_type) {
      case 0x0000:
        target.set_isa(ISA_UNSPECIFIED);
        break;
      case 0x014c:
        target.set_isa(ISA_IA32_386);
        break;
      case 0x014d:
        target.set_isa(ISA_IA32_486);
        break;
      case 0x014e:
        target.set_isa(ISA_IA32_Pentium);
        break;
      case 0x0162:
        target.set_isa(ISA_MIPS_MarkI); /* R2000, R3000 */
        break;
      case 0x0163:
        target.set_isa(ISA_MIPS_MarkII); /* R6000 */
        break;
      case 0x0166:
        target.set_isa(ISA_MIPS_MarkIII); /* R4000 */
        break;
      case 0x01a2: /*Hitachi SH3*/
      case 0x01a3: /*Hitachi SH3 with FPU*/
      case 0x01a6: /*Hitachi SH4*/
      case 0x01a8: /*Hitachi SH5*/
        target.set_isa(ISA_Hitachi_SH);
        break;
      case 0x01c0:
        target.set_isa(ISA_ARM);
        break;
      case 0x01d3:
        target.set_isa(ISA_Matsushita_AM33);
        break;
      case 0x01f0: /*w/o FPU*/
      case 0x01f1: /*with FPU*/
        target.set_isa(ISA_PowerPC);
        break;
      case 0x0200:
        target.set_isa(ISA_IA64_Family);
        break;
      case 0x0266:
        target.set_isa(ISA_MIPS_16);
        break;
      case 0x0366:
        target.set_isa(ISA_MIPS_FPU);
        break;
      case 0x0466:
        target.set_isa(ISA_MIPS_16FPU);
        break;
      case 0x0ebc:
        target.set_isa(ISA_EFI_ByteCode);
        break;
      case 0x8664:
        target.set_isa(ISA_X8664_Family);
        break;
      case 0x9041:
        target.set_isa(ISA_Mitsubishi_M32R);
        break;
        
      default:
        target.set_isa(ISA_OTHER, e_cpu_type);
        break;
    }

    /* Entry point */
    base_va = e_image_base;
    entry_rva = e_entrypoint_rva;
}

/* Encode the PE header into disk format */
void *
PEFileHeader::encode(PEFileHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    host_to_le(e_cpu_type,           &(disk->e_cpu_type));
    host_to_le(e_nsections,          &(disk->e_nsections));
    host_to_le(e_time,               &(disk->e_time));
    host_to_le(e_coff_symtab,        &(disk->e_coff_symtab));
    host_to_le(e_coff_nsyms,         &(disk->e_coff_nsyms));
    host_to_le(e_nt_hdr_size,        &(disk->e_nt_hdr_size));
    host_to_le(e_flags,              &(disk->e_flags));
    return disk;
}
void *
PEFileHeader::encode(PE32OptHeader_disk *disk)
{
    host_to_le(e_lmajor,             &(disk->e_lmajor));
    host_to_le(e_lminor,             &(disk->e_lminor));
    host_to_le(e_code_size,          &(disk->e_code_size));
    host_to_le(e_data_size,          &(disk->e_data_size));
    host_to_le(e_bss_size,           &(disk->e_bss_size));
    host_to_le(e_entrypoint_rva,     &(disk->e_entrypoint_rva));
    host_to_le(e_code_rva,           &(disk->e_code_rva));
    host_to_le(e_data_rva,           &(disk->e_data_rva));
    host_to_le(e_image_base,         &(disk->e_image_base));
    host_to_le(e_section_align,      &(disk->e_section_align));
    host_to_le(e_file_align,         &(disk->e_file_align));
    host_to_le(e_os_major,           &(disk->e_os_major));
    host_to_le(e_os_minor,           &(disk->e_os_minor));
    host_to_le(e_user_major,         &(disk->e_user_major));
    host_to_le(e_user_minor,         &(disk->e_user_minor));
    host_to_le(e_subsys_major,       &(disk->e_subsys_major));
    host_to_le(e_subsys_minor,       &(disk->e_subsys_minor));
    host_to_le(e_reserved9,          &(disk->e_reserved9));
    host_to_le(e_image_size,         &(disk->e_image_size));
    host_to_le(e_header_size,        &(disk->e_header_size));
    host_to_le(e_file_checksum,      &(disk->e_file_checksum));
    host_to_le(e_subsystem,          &(disk->e_subsystem));
    host_to_le(e_dll_flags,          &(disk->e_dll_flags));
    host_to_le(e_stack_reserve_size, &(disk->e_stack_reserve_size));
    host_to_le(e_stack_commit_size,  &(disk->e_stack_commit_size));
    host_to_le(e_heap_reserve_size,  &(disk->e_heap_reserve_size));
    host_to_le(e_heap_commit_size,   &(disk->e_heap_commit_size));
    host_to_le(e_loader_flags,       &(disk->e_loader_flags));
    host_to_le(e_num_rvasize_pairs,  &(disk->e_num_rvasize_pairs));
    return disk;
}
void *
PEFileHeader::encode(PE64OptHeader_disk *disk)
{
    host_to_le(e_lmajor,             &(disk->e_lmajor));
    host_to_le(e_lminor,             &(disk->e_lminor));
    host_to_le(e_code_size,          &(disk->e_code_size));
    host_to_le(e_data_size,          &(disk->e_data_size));
    host_to_le(e_bss_size,           &(disk->e_bss_size));
    host_to_le(e_entrypoint_rva,     &(disk->e_entrypoint_rva));
    host_to_le(e_code_rva,           &(disk->e_code_rva));
    //host_to_le(e_data_rva,           &(disk->e_data_rva)); /* not present in PE32+ */
    host_to_le(e_image_base,         &(disk->e_image_base));
    host_to_le(e_section_align,      &(disk->e_section_align));
    host_to_le(e_file_align,         &(disk->e_file_align));
    host_to_le(e_os_major,           &(disk->e_os_major));
    host_to_le(e_os_minor,           &(disk->e_os_minor));
    host_to_le(e_user_major,         &(disk->e_user_major));
    host_to_le(e_user_minor,         &(disk->e_user_minor));
    host_to_le(e_subsys_major,       &(disk->e_subsys_major));
    host_to_le(e_subsys_minor,       &(disk->e_subsys_minor));
    host_to_le(e_reserved9,          &(disk->e_reserved9));
    host_to_le(e_image_size,         &(disk->e_image_size));
    host_to_le(e_header_size,        &(disk->e_header_size));
    host_to_le(e_file_checksum,      &(disk->e_file_checksum));
    host_to_le(e_subsystem,          &(disk->e_subsystem));
    host_to_le(e_dll_flags,          &(disk->e_dll_flags));
    host_to_le(e_stack_reserve_size, &(disk->e_stack_reserve_size));
    host_to_le(e_stack_commit_size,  &(disk->e_stack_commit_size));
    host_to_le(e_heap_reserve_size,  &(disk->e_heap_reserve_size));
    host_to_le(e_heap_commit_size,   &(disk->e_heap_commit_size));
    host_to_le(e_loader_flags,       &(disk->e_loader_flags));
    host_to_le(e_num_rvasize_pairs,  &(disk->e_num_rvasize_pairs));
    return disk;
}
    
/* Adds the RVA/Size pairs to the end of the PE file header */
void
PEFileHeader::add_rvasize_pairs()
{
    const RVASizePair_disk *pairs_disk = (const RVASizePair_disk*)extend(e_num_rvasize_pairs * sizeof(RVASizePair_disk));
    for (size_t i=0; i<e_num_rvasize_pairs; i++) {
        rvasize_pairs.push_back(RVASizePair(pairs_disk+i));
    }
}

/* Write the PE file header back to disk and all that it references */
void
PEFileHeader::unparse(FILE *f)
{
    /* The fixed length part of the header */
    PEFileHeader_disk fh;
    encode(&fh);

    /* The optional header */
    PE32OptHeader_disk oh32;
    PE64OptHeader_disk oh64;
    void *oh=NULL;
    size_t oh_size=0;
    uint16_t oh_magic;
    host_to_le(e_opt_magic, &oh_magic);
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
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwrite = fwrite(&fh, sizeof fh, 1, f);
    ROSE_ASSERT(1==nwrite);
    nwrite = fwrite(&oh_magic, sizeof oh_magic, 1, f);
    ROSE_ASSERT(1==nwrite);
    nwrite = fwrite(oh, oh_size, 1, f);
    ROSE_ASSERT(1==nwrite);

    /* The variable length RVA/size pair table */
    for (size_t i=0; i<e_num_rvasize_pairs; i++) {
        RVASizePair_disk rvasize_disk;
        rvasize_pairs[i].encode(&rvasize_disk);
        nwrite = fwrite(&rvasize_disk, sizeof rvasize_disk, 1, f);
        ROSE_ASSERT(1==nwrite);
    }

    /* The extended DOS header */
    if (dos2_header)
        dos2_header->unparse(f);

    /* The section table and all the non-synthesized sections */
    if (section_table)
        section_table->unparse(f);

    /* Sections that aren't in the section table */
    if (coff_symtab)
        coff_symtab->unparse(f);
}
    
/* Print some debugging information */
void
PEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEFileHeader.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    time_t t = e_time;
    char time_str[128];
    strftime(time_str, sizeof time_str, "%c", localtime(&t));

    ExecHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_cpu_type",          e_cpu_type);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_nsections",         e_nsections);
    fprintf(f, "%s%-*s = %u (%s)\n",       p, w, "e_time",              e_time, time_str);
    fprintf(f, "%s%-*s = %"PRIu64"\n",     p, w, "e_coff_symtab",       e_coff_symtab);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_coff_nsyms",        e_coff_nsyms);
    if (coff_symtab) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "coff_symtab", coff_symtab->get_id(), coff_symtab->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",      p, w, "coff_symtab");
    }
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_nt_hdr_size",       e_nt_hdr_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_flags",             e_flags);
    fprintf(f, "%s%-*s = 0x%04x %s\n",     p, w, "e_opt_magic",         e_opt_magic,
            0x10b==e_opt_magic ? "PE32" : (0x20b==e_opt_magic ? "PE32+" : "other"));
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_lmajor",            e_lmajor);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_lminor",            e_lminor);
    fprintf(f, "%s%-*s = %u bytes\n",      p, w, "e_code_size",         e_code_size);
    fprintf(f, "%s%-*s = %u bytes\n",      p, w, "e_data_size",         e_data_size);
    fprintf(f, "%s%-*s = %u bytes\n",      p, w, "e_bss_size",          e_bss_size);
    fprintf(f, "%s%-*s = 0x%08x\n",        p, w, "e_entrypoint_rva",    e_entrypoint_rva);
    fprintf(f, "%s%-*s = 0x%08x\n",        p, w, "e_code_rva",          e_code_rva);
    fprintf(f, "%s%-*s = 0x%08x\n",        p, w, "e_data_rva",          e_data_rva);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "e_image_base",        e_image_base);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_section_align",     e_section_align);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_file_align",        e_file_align);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_os_major",          e_os_major);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_os_minor",          e_os_minor);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_user_major",        e_user_major);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_user_minor",        e_user_minor);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_subsys_major",      e_subsys_major);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_subsys_minor",      e_subsys_minor);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_reserved9",         e_reserved9);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_image_size",        e_image_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_header_size",       e_header_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_file_checksum",     e_file_checksum);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_subsystem",         e_subsystem);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_dll_flags",         e_dll_flags);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_stack_reserve_size",e_stack_reserve_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_stack_commit_size", e_stack_commit_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_heap_reserve_size", e_heap_reserve_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_heap_commit_size",  e_heap_commit_size);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_loader_flags",      e_loader_flags);
    fprintf(f, "%s%-*s = %u\n",            p, w, "e_num_rvasize_pairs", e_num_rvasize_pairs);
    for (unsigned i=0; i<e_num_rvasize_pairs; i++) {
        sprintf(p, "%sPEFileHeader.pair[%d].", prefix, i);
        w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));        
        fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",  p, w, "e_rva",  rvasize_pairs[i].e_rva);
        fprintf(f, "%s%-*s = %" PRIu64 " bytes\n", p, w, "e_size", rvasize_pairs[i].e_size);
    }
    if (dos2_header) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "dos2_header", dos2_header->get_id(), dos2_header->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "dos2_header");
    }
    if (section_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "section_table", section_table->get_id(), section_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "section_table");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Section Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
PESectionTableEntry::ctor(const PESectionTableEntry_disk *disk)
{
    char name[9];
    strncpy(name, disk->name, 8);
    name[8] = '\0';
    this->name = name;

    /* Decode file format */
    virtual_size     = le_to_host(disk->virtual_size);
    rva              = le_to_host(disk->rva);
    physical_size    = le_to_host(disk->physical_size);
    physical_offset  = le_to_host(disk->physical_offset);
    coff_line_nums   = le_to_host(disk->coff_line_nums);
    n_relocs         = le_to_host(disk->n_relocs);
    n_coff_line_nums = le_to_host(disk->n_coff_line_nums);
    flags            = le_to_host(disk->flags);
}

/* Encodes a section table entry back into disk format. */
void *
PESectionTableEntry::encode(PESectionTableEntry_disk *disk)
{
    memset(disk->name, 0, sizeof(disk->name));
    memcpy(disk->name, name.c_str(), std::min(sizeof(name), name.size()));

    host_to_le(virtual_size,     &(disk->virtual_size));
    host_to_le(rva,              &(disk->rva));
    host_to_le(physical_size,    &(disk->physical_size));
    host_to_le(physical_offset,  &(disk->physical_offset));
    host_to_le(coff_line_nums,   &(disk->coff_line_nums));
    host_to_le(n_relocs,         &(disk->n_relocs));
    host_to_le(n_coff_line_nums, &(disk->n_coff_line_nums));
    host_to_le(flags,            &(disk->flags));
    return disk;
}

/* Prints some debugging info */
void
PESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",            p, w, "virtual_size",     virtual_size);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",              p, w, "rva",              rva);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",            p, w, "physical_size",    physical_size);
    fprintf(f, "%s%-*s = %" PRIu64 " file byte offset\n", p, w, "physical_offset",  physical_offset);
    fprintf(f, "%s%-*s = %u byte offset\n",               p, w, "coff_line_nums",   coff_line_nums);
    fprintf(f, "%s%-*s = %u\n",                           p, w, "n_relocs",         n_relocs);
    fprintf(f, "%s%-*s = %u\n",                           p, w, "n_coff_line_nums", n_coff_line_nums);
    fprintf(f, "%s%-*s = 0x%08x\n",                       p, w, "flags",            flags);
}

/* Print some debugging info. */
void
PESection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESection.", prefix);
    }

    ExecSection::dump(f, p, -1);
    st_entry->dump(f, p, -1);
}

/* Constructor */
void
PESectionTable::ctor(PEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("PE Section Table");
    set_purpose(SP_HEADER);
    set_header(fhdr);
    
    const size_t entsize = sizeof(PESectionTableEntry_disk);
    for (size_t i=0; i<fhdr->e_nsections; i++) {
        /* Parse the section table entry */
        const PESectionTableEntry_disk *disk = (const PESectionTableEntry_disk*)content(i*entsize, entsize);
        PESectionTableEntry *entry = new PESectionTableEntry(disk);

        /* The section */
        PESection *section=NULL;
        if (0==entry->name.compare(".idata")) {
            section = new PEImportSection(fhdr, entry->physical_offset, entry->physical_size, entry->rva);
        } else {
            section = new PESection(fhdr->get_file(), entry->physical_offset, entry->physical_size);
        }
        section->set_synthesized(false);
        section->set_name(entry->name);
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);
        section->set_header(fhdr);
        section->set_mapped_rva(entry->rva);
        section->set_st_entry(entry);
        section->set_readable((entry->flags & OF_READABLE)==OF_READABLE);
        section->set_writable((entry->flags & OF_WRITABLE)==OF_WRITABLE);
        section->set_executable((entry->flags & OF_EXECUTABLE)==OF_EXECUTABLE);
        
        if (entry->flags & (OF_CODE|OF_IDATA|OF_UDATA))
            section->set_purpose(SP_PROGRAM);
    }
}

/* Writes the section table back to disk along with each of the sections. */
void
PESectionTable::unparse(FILE *f)
{
    ExecFile *ef = get_file();
    PEFileHeader *fhdr = dynamic_cast<PEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    std::vector<ExecSection*> sections = ef->get_sections();

    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            PESection *section = dynamic_cast<PESection*>(sections[i]);

            /* Write the table entry */
            ROSE_ASSERT(section->get_id()>0); /*ID's are 1-origin in PE*/
            size_t slot = section->get_id()-1;
            PESectionTableEntry *shdr = section->get_st_entry();
            PESectionTableEntry_disk disk;
            shdr->encode(&disk);
            addr_t entry_offset = offset + slot * sizeof disk;
            int status = fseek(f, entry_offset, SEEK_SET);
            ROSE_ASSERT(status>=0);
            size_t nwrite = fwrite(&disk, sizeof disk, 1, f);
            ROSE_ASSERT(1==nwrite);

            /* Write the section */
            section->unparse(f);
        }
    }
}

/* Prints some debugging info */
void
PESectionTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESectionTable.", prefix);
    }
    ExecSection::dump(f, p, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Import Directory (".idata" section)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
PEImportDirectory::ctor(const PEImportDirectory_disk *disk)
{
    hintnames_rva   = le_to_host(disk->hintnames_rva);
    time            = le_to_host(disk->time);
    forwarder_chain = le_to_host(disk->forwarder_chain);
    dll_name_rva    = le_to_host(disk->dll_name_rva);
    bindings_rva    = le_to_host(disk->bindings_rva);
}

/* Encode a directory entry back into disk format */
void *
PEImportDirectory::encode(PEImportDirectory_disk *disk)
{
    host_to_le(hintnames_rva,   &(disk->hintnames_rva));
    host_to_le(time,            &(disk->time));
    host_to_le(forwarder_chain, &(disk->forwarder_chain));
    host_to_le(dll_name_rva,    &(disk->dll_name_rva));
    host_to_le(bindings_rva,    &(disk->bindings_rva));
    return disk;
}

/* Print debugging info */
void
PEImportDirectory::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportDirectory[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportDirectory.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "hintnames_rva",   hintnames_rva);
    fprintf(f, "%s%-*s = %lu %s",          p, w, "time",            (unsigned long)time, ctime(&time));
    fprintf(f, "%s%-*s = %u\n",            p, w, "forwarder_chain", forwarder_chain);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "dll_name_rva",    dll_name_rva);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "bindings_rva",    bindings_rva);
}

/* Constructor */
void
PEImportHintName::ctor(ExecSection *section, addr_t offset)
{
    const PEImportHintName_disk *disk = (const PEImportHintName_disk*)section->content(offset, sizeof(*disk));
    hint = le_to_host(disk->hint);
    name = section->content_str(offset+sizeof(*disk));
    padding = (name.size()+1) % 2 ? *(section->content(offset+sizeof(*disk)+name.size()+1, 1)) : '\0';
}

/* Writes the hint/name back to disk at the specified offset */
void
PEImportHintName::unparse(FILE *f, addr_t offset)
{
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);

    /* The hint */
    uint16_t hint_le;
    host_to_le(hint, &hint_le);
    size_t nwrite = fwrite(&hint_le, sizeof hint_le, 1, f);
    ROSE_ASSERT(1==nwrite);

    /* NUL-terminated name */
    fputs(name.c_str(), f);
    fputc('\0', f);
    
    /* Padding to make an even size */
    if ((name.size()+1) % 2)
        fputc(padding, f);
}

/* Print debugging info */
void
PEImportHintName::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sImporNameHint[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sImporNameHint.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u\n",     p, w, "hint",    hint);
    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name",    name.c_str());
    fprintf(f, "%s%-*s = 0x%02x\n", p, w, "padding", padding);
}

/* Print debugging info */
void
PEDLL::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEDLL[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEDLL.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    ExecDLL::dump(f, p, -1);
    if (idir)
        idir->dump(f, p, -1);
    for (size_t i=0; i<hintname_rvas.size(); i++) {
        fprintf(f, "%s%-*s = [%zu] 0x%08"PRIx64"\n", p, w, "hintname_rva", i, hintname_rvas[i]);
        fprintf(f, "%s%-*s = [%zu] 0x%08"PRIx64"\n", p, w, "binding", i, bindings[i]);
    }
}

/* Constructor */
void
PEImportSection::ctor(PEFileHeader *fhdr, addr_t offset, addr_t size, addr_t mapped_rva)
{
    size_t entry_size = sizeof(PEImportDirectory_disk);
    PEImportDirectory_disk zero;
    memset(&zero, 0, sizeof zero);

    /* Read idata directory entries--one per DLL*/
    for (size_t i=0; 1; i++) {
        /* End of list is marked by an entry of all zero. */
        const PEImportDirectory_disk *idir_disk = (const PEImportDirectory_disk*)content(i*entry_size, entry_size);
        if (!memcmp(&zero, idir_disk, sizeof zero)) break;
        PEImportDirectory *idir = new PEImportDirectory(idir_disk);

        /* The library's name is indicated by RVA. We need a section offset instead. */
        ROSE_ASSERT(idir->dll_name_rva >= mapped_rva);
        addr_t dll_name_offset = idir->dll_name_rva - mapped_rva;
        std::string dll_name = content_str(dll_name_offset);

        /* Create the DLL objects */
        PEDLL *dll = new PEDLL(dll_name);
        dll->set_idir(idir);

        /* The idir->hintname_rvas is an (optional) RVA for a NULL-terminated array whose members are either:
         *    1. an RVA of a hint/name pair (if the high-order bit of the array member is clear)
         *    2. an ordinal if the high-order bit is set */
        if (idir->hintnames_rva!=0) {
            if (idir->hintnames_rva < mapped_rva)
                throw FormatError("hint/name RVA is before beginning of \".idata\" object");
            for (addr_t hintname_rvas_offset = idir->hintnames_rva - mapped_rva; /*section offset of RVA/ordinal array */
                 1; 
                 hintname_rvas_offset += fhdr->get_word_size()) {
                addr_t hintname_rva = 0; /*RVA of the hint/name pair*/
                bool import_by_ordinal=false; /*was high-order bit of array element set?*/
                if (4==fhdr->get_word_size()) {
                    hintname_rva = le_to_host(*(const uint32_t*)content(hintname_rvas_offset, sizeof(uint32_t)));
                    import_by_ordinal = (hintname_rva & 0x80000000) != 0;
                } else if (8==fhdr->get_word_size()) {
                    hintname_rva = le_to_host(*(const uint64_t*)content(hintname_rvas_offset, sizeof(uint64_t)));
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
                    PEImportHintName *hintname = new PEImportHintName(this, hintname_offset);
                    dll->add_function(hintname->get_name());
                    dll->add_hintname(hintname);
                }
            }
        }
        
        /* The idir->bindings_rva is a NULL-terminated array of RVAs */
        if (idir->bindings_rva!=0) {
            if (idir->bindings_rva < mapped_rva)
                throw FormatError("bindings RVA is before beginning of \".idata\" object");
            for (addr_t bindings_offset  = idir->bindings_rva  - mapped_rva; /*section offset of RVA array for bindings*/
                 1; 
                 bindings_offset += fhdr->get_word_size()) {
                addr_t binding=0;
                if (4==fhdr->get_word_size()) {
                    binding = le_to_host(*(const uint32_t*)content(bindings_offset, sizeof(uint32_t)));
                } else if (8==fhdr->get_word_size()) {
                    binding = le_to_host(*(const uint64_t*)content(bindings_offset, sizeof(uint64_t)));
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
PEImportSection::unparse(FILE *f)
{
    ExecHeader *fhdr = get_header();
    const std::vector<PEDLL*> &dlls = get_dlls();
    for (size_t dllno=0; dllno<dlls.size(); dllno++) {
        PEDLL *dll = dlls[dllno];
        PEImportDirectory *idir = dll->get_idir();
        
        /* Directory entry */
        PEImportDirectory_disk idir_disk;
        idir->encode(&idir_disk);
        int status = fseek(f, offset+(dllno*sizeof idir_disk), SEEK_SET);
        ROSE_ASSERT(status>=0);
        size_t nwrite = fwrite(&idir_disk, sizeof idir_disk, 1, f);
        ROSE_ASSERT(1==nwrite);

        /* Library name */
        ROSE_ASSERT(idir->dll_name_rva >= mapped_rva);
        addr_t dll_name_offset = idir->dll_name_rva - mapped_rva;
        ROSE_ASSERT(dll_name_offset + dll->get_name().size() + 1 < size);
        status = fseek(f, offset+dll_name_offset, SEEK_SET);
        ROSE_ASSERT(status>=0);
        fputs(dll->get_name().c_str(), f);
        fputc('\0', f);

        /* Write the hint/name pairs and the array entries that point to them. */
        if (idir->hintnames_rva!=0) {
            ROSE_ASSERT(idir->hintnames_rva >= mapped_rva);
            addr_t hintname_rvas_offset = offset + idir->hintnames_rva - mapped_rva; /*file offset*/
            const std::vector<addr_t> &hintname_rvas = dll->get_hintname_rvas();
            const std::vector<PEImportHintName*> &hintnames = dll->get_hintnames();
            for (size_t i=0; i<=hintname_rvas.size(); i++) {
                /* Hint/name RVA */
                addr_t hintname_rva = i<hintname_rvas.size() ? hintname_rvas[i] : 0; /*zero terminated*/
                bool import_by_ordinal=false;
                status = fseek(f, hintname_rvas_offset + i*fhdr->get_word_size(), SEEK_SET);
                ROSE_ASSERT(status>=0);
                if (4==fhdr->get_word_size()) {
                    uint32_t rva_le;
                    host_to_le(hintname_rva, &rva_le);
                    nwrite = fwrite(&rva_le, sizeof rva_le, 1, f);
                    import_by_ordinal = (hintname_rva & 0x80000000) != 0;
                } else if (8==fhdr->get_word_size()) {
                    uint64_t rva_le;
                    host_to_le(hintname_rva, &rva_le);
                    nwrite = fwrite(&rva_le, sizeof rva_le, 1, f);
                    import_by_ordinal = (hintname_rva & 0x8000000000000000ull) != 0;
                } else {
                    ROSE_ASSERT(!"unsupported word size");
                }
                ROSE_ASSERT(1==nwrite);
            
                /* Hint/name pair */
                if (i<hintname_rvas.size() && !import_by_ordinal) {
                    addr_t hintname_offset = offset + (hintname_rvas[i] & 0x7fffffff) - mapped_rva; /*file offset*/
                    hintnames[i]->unparse(f, hintname_offset);
                }
            }
        }
        
        /* Write the bindings array */
        if (idir->bindings_rva!=0) {
            ROSE_ASSERT(idir->bindings_rva >= mapped_rva);
            const addr_t bindings_offset = offset + idir->bindings_rva - mapped_rva; /*file offset*/
            const std::vector<addr_t> &bindings = dll->get_bindings();
            for (size_t i=0; i<=bindings.size(); i++) {
                addr_t binding = i<bindings.size() ? bindings[i] : 0; /*zero terminated*/
                status = fseek(f, bindings_offset + i*fhdr->get_word_size(), SEEK_SET);
                ROSE_ASSERT(status>=0);
                if (4==fhdr->get_word_size()) {
                    uint32_t binding_le;
                    host_to_le(binding, &binding_le);
                    nwrite = fwrite(&binding_le, sizeof binding_le, 1, f);
                } else if (8==fhdr->get_word_size()) {
                    uint64_t binding_le;
                    host_to_le(binding, &binding_le);
                    nwrite = fwrite(&binding_le, sizeof binding_le, 1, f);
                } else {
                    ROSE_ASSERT(!"unsupported word size");
                }
                ROSE_ASSERT(1==nwrite);
            }
        }
    }

    /* DLL list is zero terminated */
    {
        PEImportDirectory_disk zero;
        memset(&zero, 0, sizeof zero);
        int status = fseek(f, offset + dlls.size()*sizeof zero, SEEK_SET);
        ROSE_ASSERT(status>=0);
        size_t nwrite = fwrite(&zero, sizeof zero, 1, f);
        ROSE_ASSERT(1==nwrite);
    }

    unparse_holes(f);
}

/* Print debugging info */
void
PEImportSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportSection.", prefix);
    }
    
    PESection::dump(f, p, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COFF Symbol Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor reads symbol table entries beginning at entry 'i'. We can't pass an array of COFFSymbolEntry_disk structs
 * because the disk size is 18 bytes, which is not properly aligned according to the C standard. Therefore we pass the actual
 * section and table index. The symbol occupies the specified table slot and st_num_aux_entries additional slots. */
void COFFSymbol::ctor(PEFileHeader *fhdr, ExecSection *symtab, ExecSection *strtab, size_t idx)
{
    static const bool debug=false;
    const COFFSymbol_disk *disk = (const COFFSymbol_disk*)symtab->content(idx*COFFSymbol_disk_size, COFFSymbol_disk_size);
    if (disk->st_zero==0) {
        st_name_offset = le_to_host(disk->st_offset);
        if (st_name_offset<4) throw FormatError("name collides with size field");
        set_name(strtab->content_str(st_name_offset));
    } else {
        char temp[9];
        memcpy(temp, disk->st_name, 8);
        temp[8] = '\0';
        set_name(temp);
        st_name_offset = 0;
    }

    st_name            = get_name();
    st_section_num     = le_to_host(disk->st_section_num);
    st_type            = le_to_host(disk->st_type);
    st_storage_class   = le_to_host(disk->st_storage_class);
    st_num_aux_entries = le_to_host(disk->st_num_aux_entries);

    /* Bind to section number. We can do this now because we've already parsed the PE Section Table */
    ROSE_ASSERT(fhdr->get_section_table()!=NULL);
    if (st_section_num>0) {
        bound = fhdr->get_file()->get_section_by_id(st_section_num);
        ROSE_ASSERT(bound!=NULL);
    }
    
    /* Make initial guesses for storage class, type, and definition state. We'll adjust them after reading aux entries. */
    value = le_to_host(disk->st_value);
    def_state = SYM_DEFINED;
    switch (st_storage_class) {
      case 0:    binding = SYM_NO_BINDING; break; /*none*/
      case 1:    binding = SYM_LOCAL;      break; /*stack*/
      case 2:    binding = SYM_GLOBAL;     break; /*extern*/
      case 3:    binding = SYM_GLOBAL;     break; /*static*/
      case 4:    binding = SYM_LOCAL;      break; /*register*/
      case 5:    binding = SYM_GLOBAL;     break; /*extern def*/
      case 6:    binding = SYM_LOCAL;      break; /*label*/
      case 7:    binding = SYM_LOCAL;      break; /*label(undef)*/
      case 8:    binding = SYM_LOCAL;      break; /*struct member*/
      case 9:    binding = SYM_LOCAL;      break; /*formal arg*/
      case 10:   binding = SYM_LOCAL;      break; /*struct tag*/
      case 11:   binding = SYM_LOCAL;      break; /*union member*/
      case 12:   binding = SYM_GLOBAL;     break; /*union tag*/
      case 13:   binding = SYM_GLOBAL;     break; /*typedef*/
      case 14:   binding = SYM_GLOBAL;     break; /*static(undef)*/
      case 15:   binding = SYM_GLOBAL;     break; /*enum tag*/
      case 16:   binding = SYM_LOCAL;      break; /*enum member*/
      case 17:   binding = SYM_GLOBAL;     break; /*register param*/
      case 18:   binding = SYM_LOCAL;      break; /*bit field*/
      case 100:  binding = SYM_GLOBAL;     break; /*block(bb or eb)*/
      case 101:  binding = SYM_GLOBAL;     break; /*function*/
      case 102:  binding = SYM_LOCAL;      break; /*struct end*/
      case 103:  binding = SYM_GLOBAL;     break; /*file*/
      case 104:  binding = SYM_GLOBAL;     break; /*section*/
      case 105:  binding = SYM_WEAK;       break; /*weak extern*/
      case 107:  binding = SYM_LOCAL;      break; /*CLR token*/
      case 0xff: binding = SYM_GLOBAL;     break; /*end of function*/
    }
    switch (st_type & 0xf0) {
      case 0x00: type = SYM_NO_TYPE; break;     /*none*/
      case 0x10: type = SYM_DATA;    break;     /*ptr*/
      case 0x20: type = SYM_FUNC;    break;     /*function*/
      case 0x30: type = SYM_ARRAY;   break;     /*array*/
    }
    
    /* Read additional aux entries. We keep this as 'char' to avoid alignment problems. */
    if (st_num_aux_entries>0) {
        aux_size = st_num_aux_entries * COFFSymbol_disk_size;
        aux_data = symtab->content((idx+1)*COFFSymbol_disk_size, aux_size);

        if (get_type()==SYM_FUNC && st_section_num>0) {
            /* Function */
            unsigned bf_idx      = le_to_host(*(const uint32_t*)(aux_data+0));
            unsigned size        = le_to_host(*(const uint32_t*)(aux_data+4));
            unsigned lnum_ptr    = le_to_host(*(const uint32_t*)(aux_data+8));
            unsigned next_fn_idx = le_to_host(*(const uint32_t*)(aux_data+12));
            unsigned res1        = le_to_host(*(const uint16_t*)(aux_data+16));
            set_size(size);
            if (debug) {
                fprintf(stderr, "COFF aux func %s: bf_idx=%u, size=%u, lnum_ptr=%u, next_fn_idx=%u, res1=%u\n", 
                        st_name.c_str(), bf_idx, size, lnum_ptr, next_fn_idx, res1);
            }
            
        } else if (st_storage_class==101/*function*/ && (0==st_name.compare(".bf") || 0==st_name.compare(".ef"))) {
            /* Beginning/End of function */
            unsigned res1        = le_to_host(*(const uint32_t*)(aux_data+0));
            unsigned lnum        = le_to_host(*(const uint16_t*)(aux_data+4)); /*line num within source file*/
            unsigned res2        = le_to_host(*(const uint16_t*)(aux_data+6));
            unsigned res3        = le_to_host(*(const uint32_t*)(aux_data+8));
            unsigned next_bf     = le_to_host(*(const uint32_t*)(aux_data+12)); /*only for .bf; reserved in .ef*/
            unsigned res4        = le_to_host(*(const uint16_t*)(aux_data+16));
            if (debug) {
                fprintf(stderr, "COFF aux %s: res1=%u, lnum=%u, res2=%u, res3=%u, next_bf=%u, res4=%u\n", 
                        st_name.c_str(), res1, lnum, res2, res3, next_bf, res4);
            }
            
        } else if (st_storage_class==2/*external*/ && st_section_num==0/*undef*/ && get_value()==0) {
            /* Weak External */
            unsigned sym2_idx    = le_to_host(*(const uint32_t*)(aux_data+0));
            unsigned flags       = le_to_host(*(const uint32_t*)(aux_data+4));
            unsigned res1        = le_to_host(*(const uint32_t*)(aux_data+8));
            unsigned res2        = le_to_host(*(const uint32_t*)(aux_data+12));
            unsigned res3        = le_to_host(*(const uint16_t*)(aux_data+16));
            if (debug) {
                fprintf(stderr, "COFF aux weak %s: sym2_idx=%u, flags=%u, res1=%u, res2=%u, res3=%u\n", 
                        st_name.c_str(), sym2_idx, flags, res1, res2, res3);
            }
            
        } else if (st_storage_class==103/*file*/ && 0==st_name.compare(".file")) {
            /* This symbol is a file. The file name is stored in the aux data as either the name itself or an offset
             * into the string table. Replace the fake ".file" with the real file name. */
            const COFFSymbol_disk *d = (const COFFSymbol_disk*)aux_data;
            if (0==d->st_zero) {
                addr_t fname_offset = le_to_host(d->st_offset);
                if (fname_offset<4) throw FormatError("name collides with size field");
                set_name(strtab->content_str(fname_offset));
                if (debug)
                    fprintf(stderr, "COFF aux file: offset=%"PRIu64", name=\"%s\"\n", fname_offset, get_name().c_str());
            } else {
                ROSE_ASSERT(st_num_aux_entries==1);
                char fname[COFFSymbol_disk_size+1];
                strcpy(fname, (const char*)aux_data);
                fname[COFFSymbol_disk_size] = '\0';
                set_name(fname);
                if (debug)
                    fprintf(stderr, "COFF aux file: inline-name=\"%s\"\n", get_name().c_str());
            }
            set_type(SYM_FILE);

        } else if (st_storage_class==3/*static*/ && NULL!=fhdr->get_file()->get_section_by_name(st_name, '$')) {
            /* Section */
            unsigned size         = le_to_host(*(const uint32_t*)(aux_data+0)); /*same as section header SizeOfRawData */
            unsigned nrel         = le_to_host(*(const uint16_t*)(aux_data+4)); /*number of relocations*/
            unsigned nln_ents     = le_to_host(*(const uint16_t*)(aux_data+6)); /*number of line number entries */
            unsigned cksum        = le_to_host(*(const uint32_t*)(aux_data+8));
            unsigned sect_id      = le_to_host(*(const uint16_t*)(aux_data+12)); /*1-base index into section table*/
            unsigned comdat       = aux_data[14]; /*comdat selection number if section is a COMDAT section*/
            unsigned res1         = aux_data[15];
            unsigned res2         = le_to_host(*(const uint16_t*)(aux_data+16));
            set_size(size);
            set_type(SYM_SECTION);
            if (debug) {
                fprintf(stderr, 
                        "COFF aux section: size=%u, nrel=%u, nln_ents=%u, cksum=%u, sect_id=%u, comdat=%u, res1=%u, res2=%u\n", 
                        size, nrel, nln_ents, cksum, sect_id, comdat, res1, res2);
            }
            
        } else if (st_storage_class==3/*static*/ && (st_type & 0xf)==0/*null*/ &&
                   get_value()==0 && NULL!=fhdr->get_file()->get_section_by_name(st_name)) {
            /* COMDAT section */
            /*FIXME: not implemented yet*/
            fprintf(stderr, "COFF aux comdat %s: (FIXME) not implemented yet\n", st_name.c_str());
            hexdump(stderr, symtab->get_offset()+(idx+1)*COFFSymbol_disk_size, "    ", aux_data, aux_size);

        } else {
            fprintf(stderr, "COFF aux unknown %s: (FIXME) st_storage_class=%u, st_type=0x%02x, st_section_num=%d\n", 
                    st_name.c_str(), st_storage_class, st_type, st_section_num);
            hexdump(stderr, symtab->get_offset()+(idx+1)*COFFSymbol_disk_size, "    ", aux_data, aux_size);
        }
    }

}

/* Encode a symbol back into disk format */
void *
COFFSymbol::encode(COFFSymbol_disk *disk)
{
    if (0==st_name_offset) {
        /* Name is stored in entry */
        memset(disk->st_name, 0, sizeof(disk->st_name));
        ROSE_ASSERT(st_name.size()<=sizeof(disk->st_name));
        memcpy(disk->st_name, st_name.c_str(), st_name.size());
    } else {
        /* Name is an offset into the string table */
        disk->st_zero = 0;
        host_to_le(st_name_offset, &(disk->st_offset));
    }
    
    host_to_le(get_value(),        &(disk->st_value));
    host_to_le(st_section_num,     &(disk->st_section_num));
    host_to_le(st_type,            &(disk->st_type));
    host_to_le(st_storage_class,   &(disk->st_storage_class));
    host_to_le(st_num_aux_entries, &(disk->st_num_aux_entries));
    return disk;
}

/* Print some debugging info */
void
COFFSymbol::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096], ss[128], tt[128];
    const char *s=NULL, *t=NULL;
    if (idx>=0) {
        sprintf(p, "%sCOFFSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sCOFFSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));


    ExecSymbol::dump(f, p, -1);

    switch (st_section_num) {
      case 0:  s = "external, not assigned";    break;
      case -1: s = "absolute value";            break;
      case -2: s = "general debug, no section"; break;
      default: sprintf(ss, "%d", st_section_num); s = ss; break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "st_section_num", s);

    switch (st_type & 0xf0) {
      case 0x00: s = "none";     break;
      case 0x10: s = "pointer";  break;
      case 0x20: s = "function"; break;
      case 0x30: s = "array";    break;
      default:
        sprintf(ss, "%u", st_type>>8);
        s = ss;
        break;
    }
    switch (st_type & 0xf) {
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
        sprintf(tt, "%u", st_type & 0xf);
        t = tt;
        break;
    }
    fprintf(f, "%s%-*s = %s / %s\n",          p, w, "st_type", s, t);

    switch (st_storage_class) {
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
        sprintf(ss, "%u", st_storage_class);
        s = ss;
        t = "";  
        break;
    }
    fprintf(f, "%s%-*s = %s\n",               p, w, "st_storage_class", s);
    fprintf(f, "%s%-*s = \"%s\"\n",           p, w, "st_name", st_name.c_str());
    fprintf(f, "%s%-*s = %u\n",               p, w, "st_num_aux_entries", st_num_aux_entries);
    fprintf(f, "%s%-*s = %zu bytes\n",        p, w, "aux_size", aux_size);
    hexdump(f, 0, "        ", aux_data, aux_size);
}

/* Constructor */
void
COFFSymtab::ctor(ExecFile *ef, PEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("COFF Symbols");
    set_purpose(SP_SYMTAB);
    set_header(fhdr);

    /* The string table immediately follows the symbols. The first four bytes of the string table are the size of the
     * string table in little endian. */
    addr_t strtab_offset = get_offset() + fhdr->e_coff_nsyms * COFFSymbol_disk_size;
    strtab = new ExecSection(ef, strtab_offset, sizeof(uint32_t));
    strtab->set_synthesized(true);
    strtab->set_name("COFF Symbol Strtab");
    strtab->set_purpose(SP_HEADER);
    strtab->set_header(fhdr);
    addr_t strtab_size = le_to_host(*(const uint32_t*)strtab->content(0, sizeof(uint32_t)));
    if (strtab_size<sizeof(uint32_t))
        throw FormatError("COFF symbol table string table size is less than four bytes");
    strtab->extend(strtab_size-sizeof(uint32_t));

    for (size_t i=0; i<fhdr->e_coff_nsyms; i++) {
        COFFSymbol *symbol = new COFFSymbol(fhdr, this, strtab, i);
        i += symbol->st_num_aux_entries;
        symbols.push_back(symbol);
    }
}

/* Write symbol table back to disk */
void
COFFSymtab::unparse(FILE *f)
{
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    
    for (size_t i=0; i<symbols.size(); i++) {
        COFFSymbol *symbol = symbols[i];
        COFFSymbol_disk disk;
        symbol->encode(&disk);
        size_t nwrite = fwrite(&disk, COFFSymbol_disk_size, 1, f);
        ROSE_ASSERT(1==nwrite);
        
        if (symbol->get_aux_size()>0) {
            nwrite = fwrite(symbol->get_aux_data(), symbol->get_aux_size(), 1, f);
            ROSE_ASSERT(1==nwrite);
        }
    }
    if (get_strtab())
        get_strtab()->unparse(f);
}

/* Print some debugging info */
void
COFFSymtab::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sCOFFSymtab[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sCOFFSymtab.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    ExecSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu symbols\n", p, w, "size", symbols.size());
    for (size_t i=0; i<symbols.size(); i++) {
        symbols[i]->dump(f, p, i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns true if a cursory look at the file indicates that it could be a PE file. */
bool
is_PE(ExecFile *f)
{
    DOS::DOSFileHeader  *dos_hdr = NULL;
    ExtendedDOSHeader   *dos2_hdr = NULL;
    PEFileHeader        *pe_hdr  = NULL;
    bool                retval  = false;

    try {
        dos_hdr = new DOS::DOSFileHeader(f, 0);
        if (dos_hdr->get_magic().size()<2 || dos_hdr->get_magic()[0]!='M' || dos_hdr->get_magic()[1]!='Z') goto done;
        if (dos_hdr->e_relocs_offset!=0x40) goto done;

        dos2_hdr = new ExtendedDOSHeader(f, dos_hdr->get_size());
        pe_hdr = new PEFileHeader(f, dos2_hdr->e_lfanew);
        if (pe_hdr->get_magic().size()<4 ||
            pe_hdr->get_magic()[0]!=0x50 || pe_hdr->get_magic()[1]!=0x45 ||
            pe_hdr->get_magic()[2]!=0x00 || pe_hdr->get_magic()[3]!=0x00) goto done;
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }
done:
    delete dos_hdr;
    delete dos2_hdr;
    delete pe_hdr;
    return retval;
}

#if 1
// DQ (6/16/2008): This is an alternative form of the parseBinaryFormat suggested by Robb.
// It is equivalent, but does not reflect that I expect future changes later (once I am done
// with getting the ELF work into use in ROSE).
/* Parses the structure of a PE file and adds the information to the ExecFile and asmFile */
void
parseBinaryFormat(ExecFile *ef, SgAsmFile* asmFile)
   {
     ROSE_ASSERT(ef != NULL);
     ROSE_ASSERT(asmFile != NULL);
     parse(ef);
   }
#else
// RPM (2008-06-09) This cut-n-pasted version of Exec::PE::parse() is now out-of-date.
/* Parses the structure of a PE file and adds the information to the ExecFile. */
void
parseBinaryFormat(ExecFile *f, SgAsmFile* asmFile)
   {
     ROSE_ASSERT(f != NULL);
     ROSE_ASSERT(asmFile != NULL);

  /* The MS-DOS real-mode "MZ" file header, which is always the first 64 bytes of the file */
     ROSE_ASSERT(sizeof(DOS::DOSFileHeader_disk)==64);
     DOS::DOSFileHeader *dos_header = new DOS::DOSFileHeader(f, 0);

  /* The MS-DOS real-mode stub program sits between the DOS file header and the PE file header */
  /* FIXME: this should be an executable segment. What is the entry address? */
     size_t dos_stub_offset = dos_header->end_offset();
     ROSE_ASSERT(dos_header->e_lfanew > dos_stub_offset);
     size_t dos_stub_size = dos_header->e_lfanew - dos_stub_offset;
     if (dos_stub_size>0)
        {
          ExecSection *dos_stub = new ExecSection(f, dos_stub_offset, dos_stub_size);
          dos_stub->set_name("DOS real-mode stub");
          dos_stub->set_synthesized(true);
          dos_stub->set_purpose(SP_PROGRAM);
        }
    
  /* The PE header has a fixed-size component followed by some number of RVA/Size pairs */
     PEFileHeader *pe_header = new PEFileHeader(f, dos_header->e_lfanew);
     ROSE_ASSERT(pe_header->e_num_rvasize_pairs < 1000); /* just a sanity check before we allocate memory */
     pe_header->add_rvasize_pairs();

  /* Construct the segments and their sections */
     new PESectionTable(pe_header);

  /* Parse the COFF symbol table */
     if (pe_header->e_coff_symtab && pe_header->e_coff_nsyms)
          new COFFSymtab(f, pe_header);

  /* Identify parts of the file that we haven't encountered during parsing */
     f->fill_holes();
   }
#endif

/* Parses the structure of a PE file and adds the information to the ExecFile. */
PEFileHeader *
parse(ExecFile *ef)
{
    ROSE_ASSERT(ef);

    /* All PE files are also DOS files, so parse the DOS part first */
    DOS::DOSFileHeader *dos_header = DOS::parse(ef);
    ROSE_ASSERT(dos_header->e_relocs_offset==0x40);
    ef->unfill_holes(); /*they probably contain PE information*/

    /* PE files extend the DOS header with some additional info */
    ExtendedDOSHeader *dos2_header = new ExtendedDOSHeader(ef, dos_header->get_size());
    
    /* The PE header has a fixed-size component followed by some number of RVA/Size pairs */
    PEFileHeader *pe_header = new PEFileHeader(ef, dos2_header->e_lfanew);
    ROSE_ASSERT(pe_header->e_num_rvasize_pairs < 1000); /* just a sanity check before we allocate memory */
    pe_header->add_rvasize_pairs();

    /* The extended part of the DOS header is owned by the PE header */
    dos2_header->set_header(pe_header);
    pe_header->set_dos2_header(dos2_header);

    /* Construct the section table and its sections (non-synthesized sections) */
    pe_header->set_section_table(new PESectionTable(pe_header));

    /* Parse the COFF symbol table and add symbols to the PE header */
    if (pe_header->e_coff_symtab && pe_header->e_coff_nsyms) {
        COFFSymtab *symtab = new COFFSymtab(ef, pe_header);
        std::vector<COFFSymbol*> &symbols = symtab->get_symbols();
        for (size_t i=0; i<symbols.size(); i++)
            pe_header->add_symbol(symbols[i]);
        pe_header->set_coff_symtab(symtab);
    }

    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();

    return pe_header;
}

}; //namespace PE
}; //namespace Exec
