/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "ExecLX.h"

namespace Exec {
namespace LX {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LX File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
void
LXFileHeader::ctor(ExecFile *f, addr_t offset)
{
    set_name("LX File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Decode file header */
    const LXFileHeader_disk *fh = (const LXFileHeader_disk*)content(0, sizeof(LXFileHeader_disk));
    e_byte_order = le_to_host(fh->e_byte_order);
    e_word_order = le_to_host(fh->e_word_order);
    ROSE_ASSERT(0==e_byte_order || 1==e_byte_order);
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
    e_page_offset_shift      = disk_to_host(sex, fh->e_page_offset_shift);
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
    exec_format.family      = FAMILY_LX;
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
      case 1: target.set_isa(ISA_IA32_286); break;
      case 2: target.set_isa(ISA_IA32_386); break;
      case 3: target.set_isa(ISA_IA32_486); break;
      default: target.set_isa(ISA_OTHER, e_cpu_type); break;
    }

    /* Entry point */
//    entry_rva = ???; /*FIXME: see e_eip and e_eip_section; we must parse section table first */
}

/* Encode the LX header into disk format */
void *
LXFileHeader::encode(ByteOrder sex, LXFileHeader_disk *disk)
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
    host_to_disk(sex, e_page_offset_shift,      &(disk->e_page_offset_shift));
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

/* Write the LX file header back to disk and all that it references */
void
LXFileHeader::unparse(FILE *f)
{
    LXFileHeader_disk fh;
    encode(get_sex(), &fh);
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwrite = fwrite(&fh, sizeof fh, 1, f);
    ROSE_ASSERT(1==nwrite);

    /* The extended DOS header */
    if (dos2_header)
        dos2_header->unparse(f);

    /* The section table and all the non-synthesized sections */

    /* Sections defined in the LX file header */
}
    
/* Print some debugging information */
void
LXFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sLXFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sLXFileHeader.", prefix);
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
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_page_offset_shift",      e_page_offset_shift);
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
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns true if a cursory look at the file indicates that it could be an LX file. */
bool
is_LX(ExecFile *f)
{
    DOS::DOSFileHeader  *dos_hdr = NULL;
    ExtendedDOSHeader   *dos2_hdr = NULL;
    LXFileHeader        *lx_hdr  = NULL;
    bool                retval  = false;

    try {
        dos_hdr = new DOS::DOSFileHeader(f, 0);
        if (dos_hdr->get_magic().size()<2 || dos_hdr->get_magic()[0]!='M' || dos_hdr->get_magic()[1]!='Z') goto done;
        if (dos_hdr->e_relocs_offset!=0x40) goto done;

        dos2_hdr = new ExtendedDOSHeader(f, dos_hdr->get_size());
        lx_hdr = new LXFileHeader(f, dos2_hdr->e_lfanew);
        if (lx_hdr->get_magic().size()!=2 || lx_hdr->get_magic()[0]!=0x4c /*L*/ || lx_hdr->get_magic()[1]!=0x58 /*X*/)
            goto done;
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }
done:
    delete dos_hdr;
    delete dos2_hdr;
    delete lx_hdr;
    return retval;
}

/* Parses the structure of an LX file and adds the information to the ExecFile. */
LXFileHeader *
parse(ExecFile *ef)
{
    ROSE_ASSERT(ef);

    /* All LX files are also DOS files, so parse the DOS part first */
    DOS::DOSFileHeader *dos_header = DOS::parse(ef);
    ROSE_ASSERT(dos_header->e_relocs_offset==0x40);
    ef->unfill_holes(); /*they probably contain NE information*/

    /* LX files extend the DOS header with some additional info */
    ExtendedDOSHeader *dos2_header = new ExtendedDOSHeader(ef, dos_header->get_size());
    
    /* The LX header */
    LXFileHeader *lx_header = new LXFileHeader(ef, dos2_header->e_lfanew);

    /* The extended part of the DOS header is owned by the LX header */
    dos2_header->set_header(lx_header);
    lx_header->set_dos2_header(dos2_header);

    /* Sections defined by the NE file header */
    /*FIXME*/

    /* Construct the section table and its sections (non-synthesized sections) */
    /*FIXME*/
    
    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();

    return lx_header;
}

}; //namespace LX
}; //namespace Exec
