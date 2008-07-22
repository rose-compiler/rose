/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "ExecLE.h"

namespace Exec {
namespace LE {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
void
LEFileHeader::ctor(ExecFile *f, addr_t offset)
{

    /* Decode file header */
    const LEFileHeader_disk *fh = (const LEFileHeader_disk*)content(0, sizeof(LEFileHeader_disk));
    exec_format.family      = fh->e_magic[1]=='E' ? FAMILY_LE : FAMILY_LX;
    const char *section_name = FAMILY_LE==exec_format.family ? "LE File Header" : "LX File Header";
    set_name(section_name);
    set_synthesized(true);
    set_purpose(SP_HEADER);

    e_byte_order = le_to_host(fh->e_byte_order);
    e_word_order = le_to_host(fh->e_word_order);
    ROSE_ASSERT(e_byte_order==e_word_order);
    ByteOrder sex = 0==e_byte_order ? ORDER_LSB : ORDER_MSB;

    e_format_level           = disk_to_host(sex, fh->e_format_level);
    e_cpu_type               = disk_to_host(sex, fh->e_cpu_type);
    e_os_type                = disk_to_host(sex, fh->e_os_type);
    e_module_version         = disk_to_host(sex, fh->e_module_version);
    e_flags                  = disk_to_host(sex, fh->e_flags);
    e_npages                 = disk_to_host(sex, fh->e_npages);
    e_eip_section            = disk_to_host(sex, fh->e_eip_section);
    e_eip                    = disk_to_host(sex, fh->e_eip);
    e_esp_section            = disk_to_host(sex, fh->e_esp_section);
    e_esp                    = disk_to_host(sex, fh->e_esp);
    e_page_size              = disk_to_host(sex, fh->e_page_size);
    if (FAMILY_LE==exec_format.family) {
        e_last_page_size     = disk_to_host(sex, fh->e_lps_or_shift);
        e_page_offset_shift  = 0;
    } else {
        ROSE_ASSERT(FAMILY_LX==exec_format.family);
        e_last_page_size     = 0;
        e_page_offset_shift  = disk_to_host(sex, fh->e_lps_or_shift);
    }
    e_fixup_sect_size        = disk_to_host(sex, fh->e_fixup_sect_size);
    e_fixup_sect_cksum       = disk_to_host(sex, fh->e_fixup_sect_cksum);
    e_loader_sect_size       = disk_to_host(sex, fh->e_loader_sect_size);
    e_loader_sect_cksum      = disk_to_host(sex, fh->e_loader_sect_cksum);
    e_secttab_rfo            = disk_to_host(sex, fh->e_secttab_rfo);
    e_secttab_nentries       = disk_to_host(sex, fh->e_secttab_nentries);
    e_pagetab_rfo            = disk_to_host(sex, fh->e_pagetab_rfo);
    e_iterpages_offset       = disk_to_host(sex, fh->e_iterpages_offset);
    e_rsrctab_rfo            = disk_to_host(sex, fh->e_rsrctab_rfo);
    e_rsrctab_nentries       = disk_to_host(sex, fh->e_rsrctab_nentries);
    e_resnametab_rfo         = disk_to_host(sex, fh->e_resnametab_rfo);
    e_entrytab_rfo           = disk_to_host(sex, fh->e_entrytab_rfo);
    e_fmtdirtab_rfo          = disk_to_host(sex, fh->e_fmtdirtab_rfo);
    e_fmtdirtab_nentries     = disk_to_host(sex, fh->e_fmtdirtab_nentries);
    e_fixup_pagetab_rfo      = disk_to_host(sex, fh->e_fixup_pagetab_rfo);
    e_fixup_rectab_rfo       = disk_to_host(sex, fh->e_fixup_rectab_rfo);
    e_import_modtab_rfo      = disk_to_host(sex, fh->e_import_modtab_rfo);
    e_import_modtab_nentries = disk_to_host(sex, fh->e_import_modtab_nentries);
    e_import_proctab_rfo     = disk_to_host(sex, fh->e_import_proctab_rfo);
    e_ppcksumtab_rfo         = disk_to_host(sex, fh->e_ppcksumtab_rfo);
    e_data_pages_offset      = disk_to_host(sex, fh->e_data_pages_offset);
    e_preload_npages         = disk_to_host(sex, fh->e_preload_npages);
    e_nonresnametab_offset   = disk_to_host(sex, fh->e_nonresnametab_offset);
    e_nonresnametab_size     = disk_to_host(sex, fh->e_nonresnametab_size);
    e_nonresnametab_cksum    = disk_to_host(sex, fh->e_nonresnametab_cksum);
    e_auto_ds_section        = disk_to_host(sex, fh->e_auto_ds_section);
    e_debug_info_rfo         = disk_to_host(sex, fh->e_debug_info_rfo);
    e_debug_info_size        = disk_to_host(sex, fh->e_debug_info_size);
    e_num_instance_preload   = disk_to_host(sex, fh->e_num_instance_preload);
    e_num_instance_demand    = disk_to_host(sex, fh->e_num_instance_demand);
    e_heap_size              = disk_to_host(sex, fh->e_heap_size);

    /* Magic number */
    for (size_t i=0; i<sizeof(fh->e_magic); ++i)
        magic.push_back(fh->e_magic[i]);

    /* File format */
    //exec_format.family    = ???; /*set above*/
    exec_format.purpose     = HF_MODTYPE_PROG==(e_flags & HF_MODTYPE_MASK) ? PURPOSE_EXECUTABLE : PURPOSE_LIBRARY;
    exec_format.sex         = sex;
    switch (e_os_type) {
      case 0:  exec_format.abi = ABI_UNSPECIFIED; break;
      case 1:  exec_format.abi = ABI_OS2;         break;
      case 2:  exec_format.abi = ABI_NT;          break;
      case 3:  exec_format.abi = ABI_MSDOS;       break;
      case 4:  exec_format.abi = ABI_WIN386;      break;
      default: exec_format.abi = ABI_OTHER;       break;
    }
    exec_format.abi_version = 0;
    exec_format.word_size   = 4;
    exec_format.version     = e_format_level;
    exec_format.is_current_version = 0==e_format_level;

    /* Target architecture */
    switch (e_cpu_type) {
      case 0x01: target.set_isa(ISA_IA32_286);     break;
      case 0x02: target.set_isa(ISA_IA32_386);     break;
      case 0x03: target.set_isa(ISA_IA32_486);     break;
      case 0x04: target.set_isa(ISA_IA32_Pentium); break;
      case 0x20: target.set_isa(ISA_I860_860XR);   break; /*N10*/
      case 0x21: target.set_isa(ISA_I860_860XP);   break; /*N11*/
      case 0x40: target.set_isa(ISA_MIPS_MarkI);   break; /*R2000, R3000*/
      case 0x41: target.set_isa(ISA_MIPS_MarkII);  break; /*R6000*/
      case 0x42: target.set_isa(ISA_MIPS_MarkIII); break; /*R4000*/
      default: target.set_isa(ISA_OTHER, e_cpu_type); break;
    }

    /* Entry point */
//    entry_rva = ???; /*FIXME: see e_eip and e_eip_section; we must parse section table first */
}

/* Encode the LE header into disk format */
void *
LEFileHeader::encode(ByteOrder sex, LEFileHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    host_to_disk(sex, e_byte_order,             &(disk->e_byte_order));
    host_to_disk(sex, e_word_order,             &(disk->e_word_order));
    host_to_disk(sex, e_format_level,           &(disk->e_format_level));
    host_to_disk(sex, e_cpu_type,               &(disk->e_cpu_type));
    host_to_disk(sex, e_os_type,                &(disk->e_os_type));
    host_to_disk(sex, e_module_version,         &(disk->e_module_version));
    host_to_disk(sex, e_flags,                  &(disk->e_flags));
    host_to_disk(sex, e_npages,                 &(disk->e_npages));
    host_to_disk(sex, e_eip_section,            &(disk->e_eip_section));
    host_to_disk(sex, e_eip,                    &(disk->e_eip));
    host_to_disk(sex, e_esp_section,            &(disk->e_esp_section));
    host_to_disk(sex, e_esp,                    &(disk->e_esp));
    host_to_disk(sex, e_page_size,              &(disk->e_page_size));
    if (FAMILY_LE==exec_format.family) {
        host_to_disk(sex, e_last_page_size,     &(disk->e_lps_or_shift));
    } else {
        ROSE_ASSERT(FAMILY_LX==exec_format.family);
        host_to_disk(sex, e_page_offset_shift,  &(disk->e_lps_or_shift));
    }
    host_to_disk(sex, e_fixup_sect_size,        &(disk->e_fixup_sect_size));
    host_to_disk(sex, e_fixup_sect_cksum,       &(disk->e_fixup_sect_cksum));
    host_to_disk(sex, e_loader_sect_size,       &(disk->e_loader_sect_size));
    host_to_disk(sex, e_loader_sect_cksum,      &(disk->e_loader_sect_cksum));
    host_to_disk(sex, e_secttab_rfo,            &(disk->e_secttab_rfo));
    host_to_disk(sex, e_secttab_nentries,       &(disk->e_secttab_nentries));
    host_to_disk(sex, e_pagetab_rfo,            &(disk->e_pagetab_rfo));
    host_to_disk(sex, e_iterpages_offset,       &(disk->e_iterpages_offset));
    host_to_disk(sex, e_rsrctab_rfo,            &(disk->e_rsrctab_rfo));
    host_to_disk(sex, e_rsrctab_nentries,       &(disk->e_rsrctab_nentries));
    host_to_disk(sex, e_resnametab_rfo,         &(disk->e_resnametab_rfo));
    host_to_disk(sex, e_entrytab_rfo,           &(disk->e_entrytab_rfo));
    host_to_disk(sex, e_fmtdirtab_rfo,          &(disk->e_fmtdirtab_rfo));
    host_to_disk(sex, e_fmtdirtab_nentries,     &(disk->e_fmtdirtab_nentries));
    host_to_disk(sex, e_fixup_pagetab_rfo,      &(disk->e_fixup_pagetab_rfo));
    host_to_disk(sex, e_fixup_rectab_rfo,       &(disk->e_fixup_rectab_rfo));
    host_to_disk(sex, e_import_modtab_rfo,      &(disk->e_import_modtab_rfo));
    host_to_disk(sex, e_import_modtab_nentries, &(disk->e_import_modtab_nentries));
    host_to_disk(sex, e_import_proctab_rfo,     &(disk->e_import_proctab_rfo));
    host_to_disk(sex, e_ppcksumtab_rfo,         &(disk->e_ppcksumtab_rfo));
    host_to_disk(sex, e_data_pages_offset,      &(disk->e_data_pages_offset));
    host_to_disk(sex, e_preload_npages,         &(disk->e_preload_npages));
    host_to_disk(sex, e_nonresnametab_offset,   &(disk->e_nonresnametab_offset));
    host_to_disk(sex, e_nonresnametab_size,     &(disk->e_nonresnametab_size));
    host_to_disk(sex, e_nonresnametab_cksum,    &(disk->e_nonresnametab_cksum));
    host_to_disk(sex, e_auto_ds_section,        &(disk->e_auto_ds_section));
    host_to_disk(sex, e_debug_info_rfo,         &(disk->e_debug_info_rfo));
    host_to_disk(sex, e_debug_info_size,        &(disk->e_debug_info_size));
    host_to_disk(sex, e_num_instance_preload,   &(disk->e_num_instance_preload));
    host_to_disk(sex, e_num_instance_demand,    &(disk->e_num_instance_demand));
    host_to_disk(sex, e_heap_size,              &(disk->e_heap_size));
    return disk;
}

/* Write the LE file header back to disk and all that it references */
void
LEFileHeader::unparse(FILE *f)
{
    LEFileHeader_disk fh;
    encode(get_sex(), &fh);
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwrite = fwrite(&fh, sizeof fh, 1, f);
    ROSE_ASSERT(1==nwrite);

    /* The extended DOS header */
    if (dos2_header)
        dos2_header->unparse(f);

    /* The section table and all the non-synthesized sections */
    if (section_table)
        section_table->unparse(f);

    /* Sections defined in the file header */
    if (page_table)
        page_table->unparse(f);
    if (resname_table)
        resname_table->unparse(f);
    if (entry_table)
        entry_table->unparse(f);
}

/* Format name */
const char *
LEFileHeader::format_name()
{
    if (FAMILY_LE==exec_format.family) {
        return "LE";
    } else {
        ROSE_ASSERT(FAMILY_LX==exec_format.family);
        return "LX";
    }
}
    
/* Print some debugging information */
void
LEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s%sFileHeader[%zd].", prefix, format_name(), idx);
    } else {
        sprintf(p, "%s%sFileHeader.", prefix, format_name());
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_byte_order",             e_byte_order);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_word_order",             e_word_order);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_format_level",           e_format_level);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_cpu_type",               e_cpu_type);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_os_type",                e_os_type);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_module_version",         e_module_version);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_flags",                  e_flags);
    fprintf(f, "%s%-*s = %"PRIu64"\n",                 p, w, "e_npages",                 e_npages);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_eip_section",            e_eip_section);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n",             p, w, "e_eip",                    e_eip);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_esp_section",            e_esp_section);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n",             p, w, "e_esp",                    e_esp);
    fprintf(f, "%s%-*s = %"PRIu64"\n",                 p, w, "e_page_size",              e_page_size);
    if (FAMILY_LE==exec_format.family) {
        fprintf(f, "%s%-*s = %u\n",                    p, w, "e_last_page_size",         e_last_page_size);
    } else {
        ROSE_ASSERT(FAMILY_LX==exec_format.family);
        fprintf(f, "%s%-*s = %u\n",                    p, w, "e_page_offset_shift",      e_page_offset_shift);
    }
    fprintf(f, "%s%-*s = %"PRIu64"\n",                 p, w, "e_fixup_sect_size",        e_fixup_sect_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_fixup_sect_cksum",       e_fixup_sect_cksum);
    fprintf(f, "%s%-*s = %"PRIu64"\n",                 p, w, "e_loader_sect_size",       e_loader_sect_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_loader_sect_cksum",      e_loader_sect_cksum);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_secttab_rfo",
                                                       e_secttab_rfo, e_secttab_rfo+offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_secttab_nentries",       e_secttab_nentries);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_pagetab_rfo",
                                                       e_pagetab_rfo, e_pagetab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64"\n",                 p, w, "e_iterpages_offset",       e_iterpages_offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_rsrctab_rfo",
                                                       e_rsrctab_rfo, e_rsrctab_rfo+offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_rsrctab_nentries",       e_rsrctab_nentries);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_resnametab_rfo",
                                                       e_resnametab_rfo, e_resnametab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_entrytab_rfo",
                                                       e_entrytab_rfo, e_entrytab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_fmtdirtab_rfo",
                                                       e_fmtdirtab_rfo, e_fmtdirtab_rfo+offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_fmtdirtab_nentries",     e_fmtdirtab_nentries);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_fixup_pagetab_rfo",
                                                       e_fixup_pagetab_rfo, e_fixup_pagetab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_fixup_rectab_rfo",
                                                       e_fixup_rectab_rfo, e_fixup_rectab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_import_modtab_rfo",
                                                       e_import_modtab_rfo, e_import_modtab_rfo+offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_import_modtab_nentries", e_import_modtab_nentries);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_import_proctab_rfo",
                                                       e_import_proctab_rfo, e_import_proctab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_ppcksumtab_rfo",
                                                       e_ppcksumtab_rfo, e_ppcksumtab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64"\n",                 p, w, "e_data_pages_offset",      e_data_pages_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_preload_npages",         e_preload_npages);
    fprintf(f, "%s%-*s = %"PRIu64"\n",                 p, w, "e_nonresnametab_offset",   e_nonresnametab_offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nonresnametab_size",     e_nonresnametab_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_nonresnametab_cksum",    e_nonresnametab_cksum);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_auto_ds_section",        e_auto_ds_section);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_debug_info_rfo",
                                                       e_debug_info_rfo, e_debug_info_rfo+offset);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_debug_info_size",        e_debug_info_size);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_num_instance_preload",   e_num_instance_preload);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_num_instance_demand",    e_num_instance_demand);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_heap_size",              e_heap_size);

    if (dos2_header) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "dos2_header",
                dos2_header->get_id(), dos2_header->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "dos2_header");
    }
    if (section_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "section_table",
                section_table->get_id(), section_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "section_table");
    }
    if (page_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "page_table",
                page_table->get_id(), page_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "page_table");
    }
    if (resname_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "resname_table",
                resname_table->get_id(), resname_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "resname_table");
    }
    if (entry_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "entry_table",
                entry_table->get_id(), entry_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "entry_table");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Page Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
LEPageTableEntry::ctor(ByteOrder sex, const LEPageTableEntry_disk *disk)
{
    unsigned pageno_lo = disk_to_host(sex, disk->pageno_lo);
    unsigned pageno_hi = disk_to_host(sex, disk->pageno_hi);
    pageno      = (pageno_hi << 8) | pageno_lo;
    flags       = disk_to_host(sex, disk->flags);
}

/* Encode page table entry to disk format */
void *
LEPageTableEntry::encode(ByteOrder sex, LEPageTableEntry_disk *disk)
{
    host_to_disk(sex, (pageno & 0xff),    &(disk->pageno_lo));
    host_to_disk(sex, (pageno>>8)&0xffff, &(disk->pageno_hi));
    host_to_disk(sex, flags,              &(disk->flags));
    return disk;
}

/* Print some debugging information */
void
LEPageTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPageTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPageTableEntry.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = 0x%04x\n", p, w, "flags",          flags);
    fprintf(f, "%s%-*s = %u\n",     p, w, "pageno", pageno);
}

/* Constructor */
void
LEPageTable::ctor(LEFileHeader *fhdr)
{
    char section_name[64];
    sprintf(section_name, "%s Page Table", fhdr->format_name());

    set_synthesized(true);
    set_name(section_name);
    set_purpose(SP_HEADER);
    set_header(fhdr);

    const addr_t entry_size = sizeof(LEPageTableEntry_disk);
    for (addr_t entry_offset=0; entry_offset+entry_size<=size; entry_offset+=entry_size) {
        const LEPageTableEntry_disk *disk = (const LEPageTableEntry_disk*)content(entry_offset, entry_size);
        entries.push_back(new LEPageTableEntry(fhdr->get_sex(), disk));
    }
}

/* Returns info about a particular page. Indices are 1-origin */
LEPageTableEntry *
LEPageTable::get_page(size_t idx)
{
    ROSE_ASSERT(idx>0);
    ROSE_ASSERT(idx<=entries.size());
    return entries[idx-1];
}

/* Write page table back to disk */
void
LEPageTable::unparse(FILE *f)
{
    fseek(f, offset, SEEK_SET);
    for (size_t i=0; i<entries.size(); i++) {
        LEPageTableEntry_disk disk;
        entries[i]->encode(get_header()->get_sex(), &disk);
        fwrite(&disk, sizeof disk, 1, f);
    }
}

/* Print some debugging information */
void
LEPageTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s%sPageTable[%zd].", prefix, get_header()->format_name(), idx);
    } else {
        sprintf(p, "%s%sPageTable.", prefix, get_header()->format_name());
    }

    ExecSection::dump(f, p, -1);
    for (size_t i=0; i<entries.size(); i++) {
        entries[i]->dump(f, p, i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Section Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
LESectionTableEntry::ctor(ByteOrder sex, const LESectionTableEntry_disk *disk)
{
    mapped_size      = disk_to_host(sex, disk->mapped_size);
    base_addr        = disk_to_host(sex, disk->base_addr);
    flags            = disk_to_host(sex, disk->flags);
    pagemap_index    = disk_to_host(sex, disk->pagemap_index);
    pagemap_nentries = disk_to_host(sex, disk->pagemap_nentries);
    res1             = disk_to_host(sex, disk->res1);
}

/* Encodes a section table entry back into disk format. */
void *
LESectionTableEntry::encode(ByteOrder sex, LESectionTableEntry_disk *disk)
{
    host_to_disk(sex, mapped_size,      &(disk->mapped_size));
    host_to_disk(sex, base_addr,        &(disk->base_addr));
    host_to_disk(sex, flags,            &(disk->flags));
    host_to_disk(sex, pagemap_index,    &(disk->pagemap_index));
    host_to_disk(sex, pagemap_nentries, &(disk->pagemap_nentries));
    host_to_disk(sex, res1,             &(disk->res1));
    return disk;
}

/* Prints some debugging info */
void
LESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sLESectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sLESectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %"PRIu64" bytes\n", p, w, "mapped_size",      mapped_size);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n",   p, w, "base_addr",        base_addr);
    fprintf(f, "%s%-*s = 0x%08x\n",          p, w, "flags",            flags);
    fprintf(f, "%s%-*s = %u\n",              p, w, "pagemap_index",    pagemap_index);
    fprintf(f, "%s%-*s = %u entries\n",      p, w, "pagemap_nentries", pagemap_nentries);
    fprintf(f, "%s%-*s = 0x%08x\n",          p, w, "res1",             res1);
}
    
/* Print some debugging info. */
void
LESection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    LEFileHeader *fhdr = dynamic_cast<LEFileHeader*>(get_header());

    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s%sSection[%zd].", prefix, fhdr->format_name(), idx);
    } else {
        sprintf(p, "%s%sSection.", prefix, fhdr->format_name());
    }

    ExecSection::dump(f, p, -1);
    st_entry->dump(f, p, -1);
}

/* Constructor */
void
LESectionTable::ctor(LEFileHeader *fhdr)
{
    set_synthesized(true);
    char section_name[64];
    sprintf(section_name, "%s Section Table", fhdr->format_name());
    set_name(section_name);
    set_purpose(SP_HEADER);
    set_header(fhdr);

    LEPageTable *pages = fhdr->get_page_table();
    
    const size_t entsize = sizeof(LESectionTableEntry_disk);
    for (size_t i=0; i<fhdr->e_secttab_nentries; i++) {
        /* Parse the section table entry */
        const LESectionTableEntry_disk *disk = (const LESectionTableEntry_disk*)content(i*entsize, entsize);
        LESectionTableEntry *entry = new LESectionTableEntry(fhdr->get_sex(), disk);

        /* The section */
        addr_t section_offset, section_size; /*offset and size of section within file */
        LEPageTableEntry *page = pages->get_page(entry->pagemap_index);
#if 1 /* FIXME: The section_offset and section_size are not calculated correctly! */
        if (1) {
            section_offset = section_size = 0;
        } else
#endif
        if (FAMILY_LE==fhdr->get_exec_format().family) {
            section_offset = page->get_pageno() * fhdr->e_page_size;
            section_size = entry->pagemap_nentries * fhdr->e_page_size;
        } else {
            ROSE_ASSERT(FAMILY_LX==fhdr->get_exec_format().family);
            section_offset = page->get_pageno() << fhdr->e_page_offset_shift;
            section_size = entry->pagemap_nentries * (1<<fhdr->e_page_offset_shift);
        }

        LESection *section = new LESection(fhdr->get_file(), section_offset, section_size);
        section->set_synthesized(false);
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);
        section->set_header(fhdr);
        section->set_st_entry(entry);

        /* Section permissions */
        section->set_readable((entry->flags & SF_READABLE)==SF_READABLE);
        section->set_writable((entry->flags & SF_WRITABLE)==SF_WRITABLE);
        section->set_executable((entry->flags & SF_EXECUTABLE)==SF_EXECUTABLE);

        unsigned section_type = entry->flags & SF_TYPE_MASK;
        if (SF_TYPE_ZERO==section_type) {
            section->set_name(".bss");
        } else if (entry->flags & SF_EXECUTABLE) {
            section->set_name(".text");
        }
    }
}

/* Writes the section table back to disk along with each of the sections. */
void
LESectionTable::unparse(FILE *f)
{
    ExecFile *ef = get_file();
    LEFileHeader *fhdr = dynamic_cast<LEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    std::vector<ExecSection*> sections = ef->get_sections();

    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            LESection *section = dynamic_cast<LESection*>(sections[i]);

            /* Write the table entry */
            ROSE_ASSERT(section->get_id()>0); /*ID's are 1-origin in LE*/
            size_t slot = section->get_id()-1;
            LESectionTableEntry *shdr = section->get_st_entry();
            LESectionTableEntry_disk disk;
            shdr->encode(get_header()->get_sex(), &disk);
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
LESectionTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s%sSectionTable[%zd].", prefix, get_header()->format_name(), idx);
    } else {
        sprintf(p, "%s%sSectionTable.", prefix, get_header()->format_name());
    }
    ExecSection::dump(f, p, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Resident and Non-Resident Name Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor assumes ExecSection is zero bytes long so far */
void
LENameTable::ctor(LEFileHeader *fhdr)
{
    set_synthesized(true);
    char section_name[64];
    sprintf(section_name, "%s Name Table", fhdr->format_name());
    set_name(section_name);
    set_purpose(SP_HEADER);
    set_header(fhdr);
    
    /* Resident exported procedure names, until we hit a zero length name. The first name
     * is for the library itself and the corresponding ordinal has no meaning. */
    addr_t at = 0;
    while (1) {
        extend(1);
        size_t length = content(at++, 1)[0];
        if (0==length) break;

        extend(length);
        names.push_back(std::string((const char*)content(at, length), length));
        at += length;

        extend(2);
        ordinals.push_back(le_to_host(*(const uint16_t*)content(at, 2)));
        at += 2;
    }
}

/* Writes the section back to disk. */
void
LENameTable::unparse(FILE *f)
{
    ROSE_ASSERT(names.size()==ordinals.size());

    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    for (size_t i=0; i<names.size(); i++) {
        /* Name length */
        ROSE_ASSERT(names[i].size()<=0xff);
        unsigned char len = names[i].size();
        fputc(len, f);

        /* Name */
        fputs(names[i].c_str(), f);

        /* Ordinal */
        ROSE_ASSERT(ordinals[i]<=0xffff);
        uint16_t ordinal_le;
        host_to_le(ordinals[i], &ordinal_le);
        fwrite(&ordinal_le, sizeof ordinal_le, 1, f);
    }
    
    /* Zero-terminated */
    fputc('\0', f);
}

/* Prints some debugging info */
void
LENameTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sLENameTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sLENameTable.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);
    ROSE_ASSERT(names.size()==ordinals.size());
    for (size_t i=0; i<names.size(); i++) {
        fprintf(f, "%s%-*s = [%zd] \"%s\"\n", p, w, "names",    i, names[i].c_str());
        fprintf(f, "%s%-*s = [%zd] %u\n",     p, w, "ordinals", i, ordinals[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Entry Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
LEEntryPoint::ctor(ByteOrder sex, const LEEntryPoint_disk *disk)
{
    flags        = disk_to_host(sex, disk->flags);
    objnum       = disk_to_host(sex, disk->objnum);
    entry_type   = disk_to_host(sex, disk->entry_type);
    entry_offset = disk_to_host(sex, disk->entry_offset);
    res1         = disk_to_host(sex, disk->res1);
}

/* Print some debugging info */
void
LEEntryPoint::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sEntryPoint[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sEntryPoint.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = 0x%02x",    p, w, "flags",        flags);
    if (flags & 0x01)
        fprintf(f, " 32-bit");
    if (flags & 0x01) {
        fprintf(f, " non-empty\n");
        fprintf(f, "%s%-*s = %u\n", p, w, "objnum",       objnum);
        fprintf(f, "%s%-*s = 0x%02x", p, w, "entry_type",   entry_type);
        if (entry_type & 0x01) fputs(" exported", f);
        if (entry_type & 0x02) fputs(" shared-data", f);
        fprintf(f, " stack-params=%u\n", (entry_type >> 3) & 0x1f);
        fprintf(f, "%s%-*s = %"PRIu64"\n", p, w, "entry_offset", entry_offset);
        fprintf(f, "%s%-*s = 0x%04x\n",    p, w, "res1",         res1);
    } else {
        fprintf(f, " empty\n");
    }
}

/* Constructor. We don't know the size of the LE Entry table until after reading the first byte. Therefore the ExecSection is
 * created with an initial size of zero. */
void
LEEntryTable::ctor(LEFileHeader *fhdr)
{
    set_synthesized(true);
    char section_name[64];
    sprintf(section_name, "%s Entry Table", fhdr->format_name());
    set_name(section_name);
    set_purpose(SP_HEADER);
    set_header(fhdr);
    
    ROSE_ASSERT(0==size);
    
    addr_t at=0;
    extend(1);
    size_t nentries = content(at++, 1)[0];
    for (size_t i=0; i<nentries; i++) {
        extend(1);
        uint8_t flags = content(at, 1)[0];
        if (flags & 0x01) {
            extend(sizeof(LEEntryPoint_disk)-1);
            const LEEntryPoint_disk *disk = (const LEEntryPoint_disk*)content(at, sizeof(LEEntryPoint_disk));
            entries.push_back(LEEntryPoint(fhdr->get_sex(), disk));
        } else {
            entries.push_back(LEEntryPoint(fhdr->get_sex(), flags));
        }
    }
}

/* Write entry table back to file */
void
LEEntryTable::unparse(FILE *f)
{
    fprintf(stderr, "Exec::LE::LEEntryTable::unparse() FIXME\n");
}

/* Print some debugging info */
void
LEEntryTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s%sEntryTable[%zd].", prefix, get_header()->format_name(), idx);
    } else {
        sprintf(p, "%s%sEntryTable.", prefix, get_header()->format_name());
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu entry points\n", p, w, "size", entries.size());
    for (size_t i=0; i<entries.size(); i++) {
        entries[i].dump(f, p, i);
    }
}

    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns true if a cursory look at the file indicates that it could be an LE or LX file. */
bool
is_LE(ExecFile *f)
{
    DOS::DOSFileHeader  *dos_hdr = NULL;
    ExtendedDOSHeader   *dos2_hdr = NULL;
    LEFileHeader        *le_hdr  = NULL;
    bool                retval  = false;

    try {
        dos_hdr = new DOS::DOSFileHeader(f, 0);
        if (dos_hdr->get_magic().size()<2 || dos_hdr->get_magic()[0]!='M' || dos_hdr->get_magic()[1]!='Z') goto done;
        if (dos_hdr->e_relocs_offset!=0x40) goto done;

        dos2_hdr = new ExtendedDOSHeader(f, dos_hdr->get_size());
        le_hdr = new LEFileHeader(f, dos2_hdr->e_lfanew);
        if (le_hdr->get_magic().size()!=2 || le_hdr->get_magic()[0]!=0x4c /*L*/ ||
            (le_hdr->get_magic()[1]!=0x45 /*E*/ && le_hdr->get_magic()[1]!=0x58 /*X*/))
            goto done;
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }
done:
    delete dos_hdr;
    delete dos2_hdr;
    delete le_hdr;
    return retval;
}

/* Parses the structure of an LE/LX file and adds the information to the ExecFile. */
LEFileHeader *
parse(ExecFile *ef)
{
    ROSE_ASSERT(ef);

    /* All LE files are also DOS files, so parse the DOS part first */
    DOS::DOSFileHeader *dos_header = DOS::parse(ef, false);
    ROSE_ASSERT(dos_header->e_relocs_offset==0x40);
    ef->unfill_holes(); /*they probably contain NE information*/

    /* LE files extend the DOS header with some additional info */
    ExtendedDOSHeader *dos2_header = new ExtendedDOSHeader(ef, dos_header->get_size());
    
    /* The LE header */
    LEFileHeader *le_header = new LEFileHeader(ef, dos2_header->e_lfanew);

    /* The extended part of the DOS header is owned by the LE header */
    dos2_header->set_header(le_header);
    le_header->set_dos2_header(dos2_header);

    /* Now go back and add the DOS Real-Mode section but rather than using the size specified in the DOS header, constrain it
     * to not extend beyond the beginning of the LE file header. This makes detecting holes in the LE format much easier. */
    dos_header->add_rm_section(le_header->get_offset());

    /* Page Table */
    if (le_header->e_pagetab_rfo > 0 && le_header->e_npages > 0) {
        addr_t table_offset = le_header->get_offset() + le_header->e_pagetab_rfo;
        addr_t table_size = le_header->e_npages * sizeof(LEPageTableEntry_disk);
        LEPageTable *table = new LEPageTable(le_header, table_offset, table_size);
        le_header->set_page_table(table);
    }

    /* Section (Object) Table */
    if (le_header->e_secttab_rfo > 0 && le_header->e_secttab_nentries > 0) {
        addr_t table_offset = le_header->get_offset() + le_header->e_secttab_rfo;
        addr_t table_size = le_header->e_secttab_nentries * sizeof(LESectionTableEntry_disk);
        LESectionTable *table = new LESectionTable(le_header, table_offset, table_size);
        le_header->set_section_table(table);
    }
    
    /* Resource Table */
    if (le_header->e_rsrctab_rfo > 0 && le_header->e_rsrctab_nentries > 0) {
        /*FIXME*/
    }

    /* Resident Names Table */
    if (le_header->e_resnametab_rfo > 0) {
        addr_t table_offset = le_header->get_offset() + le_header->e_resnametab_rfo;
        LENameTable *table = new LENameTable(le_header, table_offset);
        char section_name[64];
        sprintf(section_name, "%s Resident Name Table", le_header->format_name());
        table->set_name(section_name);
        le_header->set_resname_table(table);
    }

    /* Entry Table */
    if (le_header->e_entrytab_rfo > 0) {
        addr_t table_offset = le_header->get_offset() + le_header->e_entrytab_rfo;
        LEEntryTable *table = new LEEntryTable(le_header, table_offset);
        le_header->set_entry_table(table);
    }

//    /*
//     * The table locations are indicated in the header but sizes are not stored. Any table whose offset is zero or whose
//     * size, calculated from the location of the following table, is zero is not present. */
//    addr_t end_rfo = le_header->get_size() + le_header->e_loader_sect_size;
//    if (le_header->e_ppcksumtab_rfo > 0 && le_header->e_ppcksumtab_rfo < end_rfo) {
//        /* Per-Page Checksum */
//        addr_t table_offset = le_header->get_offset() + le_header->e_ppcksumtab_rfo;
//        addr_t table_size   = end_rfo - le_header->e_ppcksumtab_rfo;
//        ExecSection *table = new ExecSection(ef, table_offset, table_size);
//        table->set_synthesized(true);
//        char section_name[64];
//        sprintf(section_name, "%s Per-Page Checksum Table", le_header->format_name());
//        table->set_name(section_name);
//        table->set_purpose(SP_HEADER);
//        table->set_header(le_header);
//        end_rfo = le_header->e_ppcksumtab_rfo;
//    }
//    if (0) {
//        /* FIXME: "Resident Directives Data" goes here! */
//    }       
//    if (le_header->e_fmtdirtab_rfo > 0 && le_header->e_fmtdirtab_rfo < end_rfo) {
//        /* Module Format Directives Table */
//        addr_t table_offset = le_header->get_offset() + le_header->e_fmtdirtab_rfo;
//        addr_t table_size   = end_rfo - le_header->e_fmtdirtab_rfo;
//        ExecSection *table = new ExecSection(ef, table_offset, table_size);
//        table->set_synthesized(true);
//        char section_name[64];
//        sprintf(section_name, "%s Module Format Directives Table", le_header->format_name());
//        table->set_name(section_name);
//        table->set_purpose(SP_HEADER);
//        table->set_header(le_header);
//        end_rfo = le_header->e_fmtdirtab_rfo;
//    }
    
    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();

    return le_header;
}

}; //namespace LE
}; //namespace Exec
