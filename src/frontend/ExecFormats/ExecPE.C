/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace Exec {
namespace PE {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MS-DOS Real Mode File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DOSFileHeader::ctor(ExecFile *f, addr_t offset)
{
    const DOSFileHeader_disk *disk = (const DOSFileHeader_disk*)content(0, sizeof(DOSFileHeader_disk));

    set_name("DOS File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Decode file format */
    e_cblp              = le_to_host(disk->e_cblp);
    e_cp                = le_to_host(disk->e_cp);
    e_crlc              = le_to_host(disk->e_crlc);
    e_cparhdr           = le_to_host(disk->e_cparhdr);
    e_minalloc          = le_to_host(disk->e_minalloc);
    e_maxalloc          = le_to_host(disk->e_maxalloc);
    e_ss                = le_to_host(disk->e_ss);
    e_sp                = le_to_host(disk->e_sp);
    e_csum              = le_to_host(disk->e_csum);
    e_ip                = le_to_host(disk->e_ip);
    e_cs                = le_to_host(disk->e_cs);
    e_lfarlc            = le_to_host(disk->e_lfarlc);
    e_ovno              = le_to_host(disk->e_ovno);
    for (size_t i=0; i<NELMTS(e_res1); i++)
        e_res1[i]       = le_to_host(disk->e_res1[i]);
    e_oemid             = le_to_host(disk->e_oemid);
    e_oeminfo           = le_to_host(disk->e_oeminfo);
    for (size_t i=0; i<NELMTS(e_res2); i++)
        e_res2[i]       = le_to_host(disk->e_res2[i]);
    e_lfanew            = le_to_host(disk->e_lfanew);

    /* Magic number */
    magic.push_back(disk->e_magic[0]);
    magic.push_back(disk->e_magic[1]);

    /* File format */
    exec_format.family      = FAMILY_DOS;
    exec_format.purpose     = PURPOSE_EXECUTABLE;
    exec_format.sex         = ORDER_LSB;
    exec_format.abi         = ABI_MSDOS;
    exec_format.abi_version = 0;
    exec_format.word_size   = 2;
    exec_format.version     = 0;
    exec_format.is_current_version = true;

    /* Target architecture */
    target.set_isa(ISA_IA32_386);

    /* Entry point */
    base_va = 0;
    entry_rva = le_to_host(disk->e_ip);
}

/* Encode the DOS file header into disk format */
void *
DOSFileHeader::encode(DOSFileHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    host_to_le(e_cblp,     disk->e_cblp);
    host_to_le(e_cp,       disk->e_cp);
    host_to_le(e_crlc,     disk->e_crlc);
    host_to_le(e_cparhdr,  disk->e_cparhdr);
    host_to_le(e_minalloc, disk->e_minalloc);
    host_to_le(e_maxalloc, disk->e_maxalloc);
    host_to_le(e_ss,       disk->e_ss);
    host_to_le(e_sp,       disk->e_sp);
    host_to_le(e_csum,     disk->e_csum);
    host_to_le(e_ip,       disk->e_ip);
    host_to_le(e_cs,       disk->e_cs);
    host_to_le(e_lfarlc,   disk->e_lfarlc);
    host_to_le(e_ovno,     disk->e_ovno);
    for (size_t i=0; i<NELMTS(disk->e_res1); i++)
        host_to_le(e_res1[i], disk->e_res1[i]);
    host_to_le(e_oemid,    disk->e_oemid);
    host_to_le(e_oeminfo,  disk->e_oeminfo);
    for (size_t i=0; i<NELMTS(disk->e_res2); i++)
        host_to_le(e_res2[i], disk->e_res2[i]);
    host_to_le(e_lfanew,   disk->e_lfanew);
    return disk;
}
    
/* Write the DOS file header back to disk */
void
DOSFileHeader::unparse(FILE *f)
{
    DOSFileHeader_disk disk;
    encode(&disk);

    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwrite = fwrite(&disk, sizeof disk, 1, f);
    ROSE_ASSERT(1==nwrite);

    if (rm_section)
        rm_section->unparse(f);
}
    
/* Print some debugging info */
void
DOSFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sDOSFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sDOSFileHeader.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "e_cblp",     e_cblp);
    fprintf(f, "%s%-*s = %u pages\n",        p, w, "e_cp",       e_cp);
    fprintf(f, "%s%-*s = %u relocations\n",  p, w, "e_crlc",     e_crlc);
    fprintf(f, "%s%-*s = %u paragraphs\n",   p, w, "e_cparhdr",  e_cparhdr);
    fprintf(f, "%s%-*s = %u paragraphs\n",   p, w, "e_minalloc", e_minalloc);
    fprintf(f, "%s%-*s = %u paragraphs\n",   p, w, "e_maxalloc", e_maxalloc);
    fprintf(f, "%s%-*s = 0x%08u\n",          p, w, "e_ss",       e_ss);
    fprintf(f, "%s%-*s = 0x%08u\n",          p, w, "e_sp",       e_sp);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_csum",     e_csum);
    fprintf(f, "%s%-*s = 0x%08u\n",          p, w, "e_ip",       e_ip);
    fprintf(f, "%s%-*s = 0x%08u\n",          p, w, "e_cs",       e_cs);
    fprintf(f, "%s%-*s = %u byte offset\n",  p, w, "e_lfarlc",   e_lfarlc);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_ovno",     e_ovno);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res[0]",   e_res1[0]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res[1]",   e_res1[1]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res[2]",   e_res1[2]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res[3]",   e_res1[3]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_oemid",    e_oemid);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_oeminfo",  e_oeminfo);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[0]",  e_res2[0]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[1]",  e_res2[1]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[2]",  e_res2[2]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[3]",  e_res2[3]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[4]",  e_res2[4]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[5]",  e_res2[5]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[6]",  e_res2[6]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[7]",  e_res2[7]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[8]",  e_res2[8]);
    fprintf(f, "%s%-*s = %u\n",              p, w, "e_res2[9]",  e_res2[9]);
    fprintf(f, "%s%-*s = %u byte offset\n",  p, w, "e_lfanew",   e_lfanew);
    if (rm_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "rm_section", rm_section->get_id(), rm_section->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "rm_section");
    }
}
    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
void
PEFileHeader::ctor(ExecFile *f, addr_t offset)
{
    const PEFileHeader_disk *disk = (const PEFileHeader_disk*)content(0, sizeof(PEFileHeader_disk));

    set_name("PE File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Decode file format */
    e_cpu_type           = le_to_host(disk->e_cpu_type);
    e_nobjects           = le_to_host(disk->e_nobjects);
    e_time               = le_to_host(disk->e_time);
    e_coff_symtab        = le_to_host(disk->e_coff_symtab);
    e_coff_nsyms         = le_to_host(disk->e_coff_nsyms);
    e_nt_hdr_size        = le_to_host(disk->e_nt_hdr_size);
    e_flags              = le_to_host(disk->e_flags);
    e_reserved3          = le_to_host(disk->e_reserved3);
    e_lmajor             = le_to_host(disk->e_lmajor);
    e_lminor             = le_to_host(disk->e_lminor);
    e_reserved4          = le_to_host(disk->e_reserved4);
    e_reserved5          = le_to_host(disk->e_reserved5);
    e_reserved6          = le_to_host(disk->e_reserved6);
    e_entrypoint_rva     = le_to_host(disk->e_entrypoint_rva);
    e_reserved7          = le_to_host(disk->e_reserved7);
    e_reserved8          = le_to_host(disk->e_reserved8);
    e_image_base         = le_to_host(disk->e_image_base);
    e_object_align       = le_to_host(disk->e_object_align);
    e_file_align         = le_to_host(disk->e_file_align);
    e_os_major           = le_to_host(disk->e_os_major);
    e_os_minor           = le_to_host(disk->e_os_minor);
    e_user_major         = le_to_host(disk->e_user_major);
    e_user_minor         = le_to_host(disk->e_user_minor);
    e_subsys_major       = le_to_host(disk->e_subsys_major);
    e_subsys_minor       = le_to_host(disk->e_subsys_minor);
    e_reserved9          = le_to_host(disk->e_reserved9);
    e_image_size         = le_to_host(disk->e_image_size);
    e_header_size        = le_to_host(disk->e_header_size);
    e_file_checksum      = le_to_host(disk->e_file_checksum);
    e_subsystem          = le_to_host(disk->e_subsystem);
    e_dll_flags          = le_to_host(disk->e_dll_flags);
    e_stack_reserve_size = le_to_host(disk->e_stack_reserve_size);
    e_stack_commit_size  = le_to_host(disk->e_stack_commit_size);
    e_heap_reserve_size  = le_to_host(disk->e_heap_reserve_size);
    e_heap_commit_size   = le_to_host(disk->e_heap_commit_size);
    e_reserved10         = le_to_host(disk->e_reserved10);
    e_num_rvasize_pairs  = le_to_host(disk->e_num_rvasize_pairs);

    /* Magic number */
    for (size_t i=0; i<sizeof(disk->e_magic); ++i)
        magic.push_back(disk->e_magic[i]);

    /* File format */
    exec_format.family      = FAMILY_PE;
    exec_format.purpose     = e_flags & HF_PROGRAM ? PURPOSE_EXECUTABLE : PURPOSE_LIBRARY;
    exec_format.sex         = ORDER_LSB;
    exec_format.abi         = ABI_NT;
    exec_format.abi_version = 0;
    exec_format.word_size   = 4;
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
    host_to_le(e_cpu_type,           disk->e_cpu_type);
    host_to_le(e_nobjects,           disk->e_nobjects);
    host_to_le(e_time,               disk->e_time);
    host_to_le(e_coff_symtab,        disk->e_coff_symtab);
    host_to_le(e_coff_nsyms,         disk->e_coff_nsyms);
    host_to_le(e_nt_hdr_size,        disk->e_nt_hdr_size);
    host_to_le(e_flags,              disk->e_flags);
    host_to_le(e_reserved3,          disk->e_reserved3);
    host_to_le(e_lmajor,             disk->e_lmajor);
    host_to_le(e_lminor,             disk->e_lminor);
    host_to_le(e_reserved4,          disk->e_reserved4);
    host_to_le(e_reserved5,          disk->e_reserved5);
    host_to_le(e_reserved6,          disk->e_reserved6);
    host_to_le(e_entrypoint_rva,     disk->e_entrypoint_rva);
    host_to_le(e_reserved7,          disk->e_reserved7);
    host_to_le(e_reserved8,          disk->e_reserved8);
    host_to_le(e_image_base,         disk->e_image_base);
    host_to_le(e_object_align,       disk->e_object_align);
    host_to_le(e_file_align,         disk->e_file_align);
    host_to_le(e_os_major,           disk->e_os_major);
    host_to_le(e_os_minor,           disk->e_os_minor);
    host_to_le(e_user_major,         disk->e_user_major);
    host_to_le(e_user_minor,         disk->e_user_minor);
    host_to_le(e_subsys_major,       disk->e_subsys_major);
    host_to_le(e_subsys_minor,       disk->e_subsys_minor);
    host_to_le(e_reserved9,          disk->e_reserved9);
    host_to_le(e_image_size,         disk->e_image_size);
    host_to_le(e_header_size,        disk->e_header_size);
    host_to_le(e_file_checksum,      disk->e_file_checksum);
    host_to_le(e_subsystem,          disk->e_subsystem);
    host_to_le(e_dll_flags,          disk->e_dll_flags);
    host_to_le(e_stack_reserve_size, disk->e_stack_reserve_size);
    host_to_le(e_stack_commit_size,  disk->e_stack_commit_size);
    host_to_le(e_heap_reserve_size,  disk->e_heap_reserve_size);
    host_to_le(e_heap_commit_size,   disk->e_heap_commit_size);
    host_to_le(e_reserved10,         disk->e_reserved10);
    host_to_le(e_num_rvasize_pairs,  disk->e_num_rvasize_pairs);
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
    PEFileHeader_disk disk;
    encode(&disk);
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwrite = fwrite(&disk, sizeof disk, 1, f);
    ROSE_ASSERT(1==nwrite);

    /* The variable length RVA/size pair table */
    for (size_t i=0; i<e_num_rvasize_pairs; i++) {
        RVASizePair_disk rvasize_disk;
        rvasize_pairs[i].encode(&rvasize_disk);
        nwrite = fwrite(&rvasize_disk, sizeof rvasize_disk, 1, f);
        ROSE_ASSERT(1==nwrite);
    }

    /* The object table and all the sections/segments */
    if (object_table)
        object_table->unparse(f);
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

    ExecHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_cpu_type",          e_cpu_type);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_nobjects",          e_nobjects);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_time",              e_time);
    fprintf(f, "%s%-*s = %"PRIu64"\n", p, w, "e_coff_symtab",       e_coff_symtab);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_coff_nsyms",        e_coff_nsyms);
    if (coff_symtab) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "coff_symtab", coff_symtab->get_id(), coff_symtab->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "coff_symtab");
    }
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_nt_hdr_size",       e_nt_hdr_size);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_flags",             e_flags);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_reserved3",         e_reserved3);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_lmajor",            e_lmajor);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_lminor",            e_lminor);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_reserved4",         e_reserved4);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_reserved5",         e_reserved5);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_reserved6",         e_reserved6);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_entrypoint_rva",    e_entrypoint_rva);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_reserved7",         e_reserved7);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_reserved8",         e_reserved8);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_image_base",        e_image_base);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_object_align",      e_object_align);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_file_align",        e_file_align);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_os_major",          e_os_major);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_os_minor",          e_os_minor);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_user_major",        e_user_major);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_user_minor",        e_user_minor);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_subsys_major",      e_subsys_major);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_subsys_minor",      e_subsys_minor);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_reserved9",         e_reserved9);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_image_size",        e_image_size);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_header_size",       e_header_size);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_file_checksum",     e_file_checksum);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_subsystem",         e_subsystem);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_dll_flags",         e_dll_flags);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_stack_reserve_size",e_stack_reserve_size);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_stack_commit_size", e_stack_commit_size);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_heap_reserve_size", e_heap_reserve_size);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_heap_commit_size",  e_heap_commit_size);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_reserved10",        e_reserved10);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_num_rvasize_pairs", e_num_rvasize_pairs);
    for (unsigned i=0; i<e_num_rvasize_pairs; i++) {
        sprintf(p, "%sPEFileHeader.pair[%d].", prefix, i);
        w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));        
        fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",  p, w, "e_rva",  rvasize_pairs[i].e_rva);
        fprintf(f, "%s%-*s = %" PRIu64 " bytes\n", p, w, "e_size", rvasize_pairs[i].e_size);
    }
    if (object_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "object_table", object_table->get_id(), object_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "object_table");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Segments and segment table (a.k.a., "object table")
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
PEObjectTableEntry::ctor(const PEObjectTableEntry_disk *disk)
{
    char name[9];
    strncpy(name, disk->name, 8);
    name[8] = '\0';
    this->name = name;

    /* Decode file format */
    virtual_size    = le_to_host(disk->virtual_size);
    rva             = le_to_host(disk->rva);
    physical_size   = le_to_host(disk->physical_size);
    physical_offset = le_to_host(disk->physical_offset);
    reserved[0]     = le_to_host(disk->reserved[0]);
    reserved[1]     = le_to_host(disk->reserved[1]);
    reserved[2]     = le_to_host(disk->reserved[2]);
    flags           = le_to_host(disk->flags);
}

/* Encodes an object table entry back into disk format. */
void *
PEObjectTableEntry::encode(PEObjectTableEntry_disk *disk)
{
    memset(disk->name, 0, sizeof(disk->name));
    memcpy(disk->name, name.c_str(), std::min(sizeof(name), name.size()));

    host_to_le(virtual_size,    disk->virtual_size);
    host_to_le(rva,             disk->rva);
    host_to_le(physical_size,   disk->physical_size);
    host_to_le(physical_offset, disk->physical_offset);
    for (size_t i=0; i<NELMTS(reserved); i++)
        host_to_le(reserved[i], disk->reserved[i]);
    host_to_le(flags, disk->flags);
    return disk;
}

/* Prints some debugging info */
void
PEObjectTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEObjectTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEObjectTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",            p, w, "virtual_size",    virtual_size);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",              p, w, "rva",             rva);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",            p, w, "physical_size",   physical_size);
    fprintf(f, "%s%-*s = %" PRIu64 " file byte offset\n", p, w, "physical_offset", physical_offset);
    fprintf(f, "%s%-*s = %u\n",                  p, w, "reserved[0]",     reserved[0]);
    fprintf(f, "%s%-*s = %u\n",                  p, w, "reserved[1]",     reserved[1]);
    fprintf(f, "%s%-*s = %u\n",                  p, w, "reserved[2]",     reserved[2]);
    fprintf(f, "%s%-*s = 0x%08x\n",              p, w, "flags",           flags);
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
PEObjectTable::ctor(PEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("PE Object Table");
    set_purpose(SP_HEADER);
    set_header(fhdr);
    
    const size_t entsize = sizeof(PEObjectTableEntry_disk);
    for (size_t i=0; i<fhdr->e_nobjects; i++) {
        /* Parse the object table entry */
        const PEObjectTableEntry_disk *disk = (const PEObjectTableEntry_disk*)content(i*entsize, entsize);
        PEObjectTableEntry *entry = new PEObjectTableEntry(disk);

        /* The section to hold the segment */
        PESection *section=NULL;
        if (0==entry->name.compare(".idata")) {
            section = new PEImportSection(fhdr, entry->physical_offset, entry->physical_size, entry->rva);
        } else {
            section = new PESection(fhdr->get_file(), entry->physical_offset, entry->physical_size);
        }
        section->set_synthesized(true);
        section->set_name(entry->name);
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);
        section->set_header(fhdr);
        section->set_mapped_rva(entry->rva);
        section->set_st_entry(entry);

        /* Define the segment */
        ExecSegment *segment = new ExecSegment(section, 0, entry->physical_size, entry->rva, entry->virtual_size);
        segment->set_name(entry->name);

        if (entry->flags & (OF_CODE|OF_IDATA|OF_UDATA))
            section->set_purpose(SP_PROGRAM);
        if (entry->flags & OF_EXECUTABLE)
            segment->set_executable(true);
        if (entry->flags & OF_WRITABLE)
            segment->set_writable(true);
    }
}

/* Writes the object table back to disk along with each of the objects. */
void
PEObjectTable::unparse(FILE *f)
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
            PEObjectTableEntry *shdr = section->get_st_entry();
            PEObjectTableEntry_disk disk;
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
PEObjectTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEObjectTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEObjectTable.", prefix);
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
    padding = name.size() % 2 ? *(section->content(offset+sizeof(*disk)+name.size()+1, 1)) : '\0';
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
        dirs.push_back(idir);

        /* The library's name is indicated by RVA. We need a section offset instead. */
        ROSE_ASSERT(idir->dll_name_rva >= mapped_rva);
        addr_t dll_name_offset = idir->dll_name_rva - mapped_rva;
        std::string dll_name = content_str(dll_name_offset);
        ExecDLL *dll = new ExecDLL(dll_name);


        /* The thunks is an array of RVAs that point to hint/name pairs for the entities imported from the DLL that we're
         * currently processing. Each hint is a two-byte little-endian value. Each name is NUL-terminated. The array address
         * and hint/name pair addresses are all specified with RVAs, but we need to convert them to section offsets in order
         * to read them since we've not mapped sections to their preferred base addresses. */
        if (idir->hintnames_rva < mapped_rva)
            throw FormatError("hint/name RVA is before beginning of \".idata\" segment");
        if (idir->bindings_rva < mapped_rva)
            throw FormatError("bindings RVA is before beginning of \".idata\" segment");
        addr_t hintnames_offset = idir->hintnames_rva - mapped_rva;
        addr_t bindings_offset  = idir->bindings_rva  - mapped_rva;
        while (1) {
            addr_t hint_rva = le_to_host(*(const uint32_t*)content(hintnames_offset, sizeof(uint32_t)));
            hintnames_offset += sizeof(uint32_t);
            if (0==hint_rva) break; /*list of RVAs is null terminated */
            addr_t hint_offset = hint_rva - mapped_rva;
            PEImportHintName *hintname = new PEImportHintName(this, hint_offset);
            dll->add_function(hintname->get_name());

            /* FIXME: do something with binding */
            addr_t binding = le_to_host(*(const uint32_t*)content(bindings_offset, sizeof(uint32_t)));
            bindings_offset += sizeof(uint32_t);
        }

        fhdr->add_dll(dll);
    }
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
    for (size_t i=0; i<dirs.size(); i++)
        dirs[i]->dump(f, p, i);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COFF Symbol Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor reads symbol table entries beginning at entry 'i'. We can't pass an array of COFFSymbolEntry_disk structs
 * because the disk size is 18 bytes, which is not properly aligned according to the C standard. Therefore we pass the actual
 * section and table index. The symbol occupies the specified table slot and st_num_aux_entries additional slots. */
void COFFSymbol::ctor(PEFileHeader *fhdr, ExecSection *symtab, ExecSection *strtab, size_t idx)
{
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

    st_section_num     = le_to_host(disk->st_section_num);
    st_type            = le_to_host(disk->st_type);
    st_storage_class   = le_to_host(disk->st_storage_class);
    st_num_aux_entries = le_to_host(disk->st_num_aux_entries);

    /* Read additional aux entries. We keep this as 'char' to avoid alignment problems. */
    if (st_num_aux_entries>0) {
        const unsigned char *aux_data = symtab->content((idx+1)*COFFSymbol_disk_size, st_num_aux_entries*COFFSymbol_disk_size);
        if (st_storage_class==2/*external*/ && st_type==0x20/*function*/ && st_section_num>0) {
            fprintf(stderr, "    ROBB: Function definition aux\n");
        } else if (0==get_name().compare(".bf") || 0==get_name().compare(".ef")) {
            fprintf(stderr, "    ROBB: Function begin/end aux\n");
        } else if (st_storage_class==2/*external*/ && st_section_num<=0 && get_value()==0) {
            fprintf(stderr, "    ROBB: Weak external aux\n");
        } else if (st_storage_class==103/*file*/ && 0==get_name().compare(".file")) {
            ROSE_ASSERT(st_num_aux_entries==1);
            char fname[COFFSymbol_disk_size+1];
            strcpy(fname, (const char*)aux_data);
            fname[COFFSymbol_disk_size] = '\0';
            set_name(fname); /*replace ".file" with the real name*/
        } else if (st_storage_class==3/*static*/ && NULL!=fhdr->get_file()->get_section_by_name(get_name())) {
            fprintf(stderr, "    ROBB: Section definition aux\n");
        } else if (get_value()==0 && st_type==0x30/*static/null*/ && 1==st_num_aux_entries) {
            fprintf(stderr, "    ROBB: COMDAT section aux\n");
        } else {
            fprintf(stderr, "    ROBB: unknown aux symbol (skipping)\n");
        }
    }

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
      case 0x30: type = SYM_DATA;    break;     /*array*/
    }
    
    value = le_to_host(disk->st_value);
    def_state = SYM_DEFINED;
}

/* Print some debugging info */
void
COFFSymbol::dump(FILE *f, const char *prefix, ssize_t idx, ExecFile *ef)
{
    char p[4096], ss[128], tt[128];
    const char *s=NULL, *t=NULL;
    ExecSection *section=NULL;
    if (idx>=0) {
        sprintf(p, "%sCOFFSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sCOFFSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));


    ExecSymbol::dump(f, p, -1);
    
    if (st_section_num<=0) {
        switch (st_section_num) {
          case 0:  s = "external, not assigned";    break;
          case -1: s = "absolute value";            break;
          case -2: s = "general debug, no section"; break;
          default: sprintf(ss, "%d", st_section_num); s = ss; break;
        }
        fprintf(f, "%s%-*s = %s\n", p, w, "section", s);
    } else if (NULL==ef) {
        fprintf(f, "%s%-*s = [%d] <section info not available here>\n", p, w, "section", st_section_num);
    } else if (NULL==(section=ef->get_section_by_id(st_section_num))) {
        fprintf(f, "%s%-*s = [%d] <not a valid section ID>\n", p, w, "section", st_section_num);
    } else {
        fprintf(f, "%s%-*s = [%d] \"%s\" @%"PRIu64", %"PRIu64" bytes\n", p, w, "section", 
                st_section_num, section->get_name().c_str(), section->get_offset(), section->get_size());
    }

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
      case 100:  s = "block(bb,eb)";    t = "relocatable address";               break;
      case 101:  s = "function";        t = "nlines or size";                    break;
      case 102:  s = "struct end";      t = "";                                  break;
      case 103:  s = "file";            t = "";                                  break;
      case 104:  s = "section";         t = "";                                  break;
      case 105:  s = "weak extern";     t = "";                                  break;
      case 107:  s = "CLR token";       t = "";                                  break;
      case 0xff: s = "end of function"; t = "";                                  break;
      default:
        sprintf(ss, "%u", st_storage_class);
        s = ss;
        t = "";  
        break;
    }
    fprintf(f, "%s%-*s = %s\n",               p, w, "storage_class", s);

    fprintf(f, "%s%-*s = %u\n",               p, w, "num_aux_entries", st_num_aux_entries);
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
    ExecSection *strtab = new ExecSection(ef, strtab_offset, sizeof(uint32_t));
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
    DOSFileHeader *dos_hdr = NULL;
    PEFileHeader  *pe_hdr  = NULL;
    bool           retval  = false;

    try {
        dos_hdr = new DOSFileHeader(f, 0);
        if (dos_hdr->get_magic().size()<2 || dos_hdr->get_magic()[0]!='M' || dos_hdr->get_magic()[1]!='Z') goto done;
        pe_hdr = new PEFileHeader(f, dos_hdr->e_lfanew);
        if (pe_hdr->get_magic().size()<4 ||
            pe_hdr->get_magic()[0]!=0x50 || pe_hdr->get_magic()[1]!=0x45 ||
            pe_hdr->get_magic()[2]!=0x00 || pe_hdr->get_magic()[3]!=0x00) goto done;
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }
done:
    delete dos_hdr;
    delete pe_hdr;
    return retval;
}

#if 0
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
#endif

/* Parses the structure of a PE file and adds the information to the ExecFile. */
void
parseBinaryFormat(ExecFile *f, SgAsmFile* asmFile)
   {
     ROSE_ASSERT(f != NULL);
     ROSE_ASSERT(asmFile != NULL);

  /* The MS-DOS real-mode "MZ" file header, which is always the first 64 bytes of the file */
     ROSE_ASSERT(sizeof(DOSFileHeader_disk)==64);
     DOSFileHeader *dos_header = new DOSFileHeader(f, 0);

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
     new PEObjectTable(pe_header);

  /* Parse the COFF symbol table */
     if (pe_header->e_coff_symtab && pe_header->e_coff_nsyms)
          new COFFSymtab(f, pe_header);

  /* Identify parts of the file that we haven't encountered during parsing */
     f->fill_holes();
   }

/* Parses the structure of a PE file and adds the information to the ExecFile. */
PEFileHeader *
parse(ExecFile *ef)
{
    ROSE_ASSERT(ef);
    
    /* The MS-DOS real-mode "MZ" file header, which is always the first 64 bytes of the file */
    ROSE_ASSERT(sizeof(DOSFileHeader_disk)==64);
    DOSFileHeader *dos_header = new DOSFileHeader(ef, 0);

    /* The MS-DOS real-mode stub program sits between the DOS file header and the PE file header */
    /* FIXME: this should be an executable segment. What is the entry address? */
    size_t dos_stub_offset = dos_header->end_offset();
    ROSE_ASSERT(dos_header->e_lfanew > dos_stub_offset);
    size_t dos_stub_size = dos_header->e_lfanew - dos_stub_offset;
    if (dos_stub_size>0) {
        ExecSection *dos_stub = new ExecSection(ef, dos_stub_offset, dos_stub_size);
        dos_stub->set_name("DOS real-mode stub");
        dos_stub->set_synthesized(true);
        dos_stub->set_purpose(SP_PROGRAM);
        dos_stub->set_header(dos_header);
        dos_header->set_rm_section(dos_stub);
    }
    
    /* The PE header has a fixed-size component followed by some number of RVA/Size pairs */
    PEFileHeader *pe_header = new PEFileHeader(ef, dos_header->e_lfanew);
    ROSE_ASSERT(pe_header->e_num_rvasize_pairs < 1000); /* just a sanity check before we allocate memory */
    pe_header->add_rvasize_pairs();

    /* Construct the segments and their sections */
    pe_header->set_object_table(new PEObjectTable(pe_header));

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
