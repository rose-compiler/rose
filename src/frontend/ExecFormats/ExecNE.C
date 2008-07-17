/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "ExecNE.h"

namespace Exec {
namespace NE {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extended DOS header for NE files
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
        e_res1[i]  = le_to_host(disk->e_res1[i]);
    e_nehdr_offset = le_to_host(disk->e_nehdr_offset);
}

/* Encode the extended header back into disk format */
void *
ExtendedDOSHeader::encode(ExtendedDOSHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_res1); i++)
        host_to_le(e_res1[i], &(disk->e_res1[i]));
    host_to_le(e_nehdr_offset,   &(disk->e_nehdr_offset));
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
    for (size_t i=0; i<NELMTS(e_res1); i++)
        fprintf(f, "%s%-*s = [%zd] %u\n", p, w, "e_res1", i, e_res1[i]);
    fprintf(f, "%s%-*s = %"PRIu64" byte offset\n",  p, w, "e_nehdr_offset",   e_nehdr_offset);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
void
NEFileHeader::ctor(ExecFile *f, addr_t offset)
{
    set_name("NE File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Decode file header */
    const NEFileHeader_disk *fh = (const NEFileHeader_disk*)content(0, sizeof(NEFileHeader_disk));
    e_linker_major         = le_to_host(fh->e_linker_major);
    e_linker_minor         = le_to_host(fh->e_linker_minor);
    e_entry_table_rfo      = le_to_host(fh->e_entry_table_rfo);
    e_entry_table_size     = le_to_host(fh->e_entry_table_size);
    e_checksum             = le_to_host(fh->e_checksum);
    e_flags                = le_to_host(fh->e_flags);
    e_autodata_sn          = le_to_host(fh->e_autodata_sn);
    e_bss_size             = le_to_host(fh->e_bss_size);
    e_stack_size           = le_to_host(fh->e_stack_size);
    e_csip                 = le_to_host(fh->e_csip);
    e_sssp                 = le_to_host(fh->e_sssp);
    e_nsections            = le_to_host(fh->e_nsections);
    e_nmodrefs             = le_to_host(fh->e_nmodrefs);
    e_nnonresnames         = le_to_host(fh->e_nnonresnames);
    e_sectab_rfo           = le_to_host(fh->e_sectab_rfo);
    e_rsrctab_rfo          = le_to_host(fh->e_rsrctab_rfo);
    e_resnametab_rfo       = le_to_host(fh->e_resnametab_rfo);
    e_modreftab_rfo        = le_to_host(fh->e_modreftab_rfo);
    e_importnametab_rfo    = le_to_host(fh->e_importnametab_rfo);
    e_nonresnametab_offset = le_to_host(fh->e_nonresnametab_offset);
    e_nmovable_entries     = le_to_host(fh->e_nmovable_entries);
    e_sector_align         = le_to_host(fh->e_sector_align);
    e_nresources           = le_to_host(fh->e_nresources);
    e_exetype              = le_to_host(fh->e_exetype);
    for (size_t i=0; i<NELMTS(fh->e_res1); i++)
        e_res1[i]          = le_to_host(fh->e_res1[i]);

    /* Magic number */
    for (size_t i=0; i<sizeof(fh->e_magic); ++i)
        magic.push_back(fh->e_magic[i]);

    /* File format */
    exec_format.family      = FAMILY_NE;
    exec_format.purpose     = e_flags & HF_LIBRARY ? PURPOSE_LIBRARY : PURPOSE_EXECUTABLE;
    exec_format.sex         = ORDER_LSB;
    exec_format.abi         = ABI_NT;
    exec_format.abi_version = 0;
    exec_format.word_size   = 2;
    ROSE_ASSERT(e_linker_major<=0xff && e_linker_minor<=0xff);
    exec_format.version     = (e_linker_major<<8) | e_linker_minor;
    exec_format.is_current_version = true; /*FIXME*/

    /* Target architecture */
    target.set_isa(ISA_IA32_386);

    /* Entry point */
//    entry_rva = e_entrypoint_rva; /*FIXME*/
}

/* Encode the NE header into disk format */
void *
NEFileHeader::encode(NEFileHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    host_to_le(e_linker_major,         &(disk->e_linker_major));
    host_to_le(e_linker_minor,         &(disk->e_linker_minor));
    host_to_le(e_entry_table_rfo,      &(disk->e_entry_table_rfo));
    host_to_le(e_entry_table_size,     &(disk->e_entry_table_size));
    host_to_le(e_checksum,             &(disk->e_checksum));
    host_to_le(e_flags,                &(disk->e_flags));
    host_to_le(e_autodata_sn,          &(disk->e_autodata_sn));
    host_to_le(e_bss_size,             &(disk->e_bss_size));
    host_to_le(e_stack_size,           &(disk->e_stack_size));
    host_to_le(e_csip,                 &(disk->e_csip));
    host_to_le(e_sssp,                 &(disk->e_sssp));
    host_to_le(e_nsections,            &(disk->e_nsections));
    host_to_le(e_nmodrefs,             &(disk->e_nmodrefs));
    host_to_le(e_nnonresnames,         &(disk->e_nnonresnames));
    host_to_le(e_sectab_rfo,           &(disk->e_sectab_rfo));
    host_to_le(e_rsrctab_rfo,          &(disk->e_rsrctab_rfo));
    host_to_le(e_resnametab_rfo,       &(disk->e_resnametab_rfo));
    host_to_le(e_modreftab_rfo,        &(disk->e_modreftab_rfo));
    host_to_le(e_importnametab_rfo,    &(disk->e_importnametab_rfo));
    host_to_le(e_nonresnametab_offset, &(disk->e_nonresnametab_offset));
    host_to_le(e_nmovable_entries,     &(disk->e_nmovable_entries));
    host_to_le(e_sector_align,         &(disk->e_sector_align));
    host_to_le(e_nresources,           &(disk->e_nresources));
    host_to_le(e_exetype,              &(disk->e_exetype));
    for (size_t i=0; i<NELMTS(disk->e_res1); i++)
        host_to_le(e_res1[i], &(disk->e_res1[i]));
    return disk;
}

/* Write the NE file header back to disk and all that it references */
void
NEFileHeader::unparse(FILE *f)
{
    NEFileHeader_disk fh;
    encode(&fh);
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
}
    
/* Print some debugging information */
void
NEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNEFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNEFileHeader.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_linker_major",         e_linker_major);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_linker_minor",         e_linker_minor);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_entry_table_rfo",      
                                                       e_entry_table_rfo, e_entry_table_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" bytes\n",           p, w, "e_entry_table_size",     e_entry_table_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_checksum",             e_checksum);
    fprintf(f, "%s%-*s = 0x%04x\n",                    p, w, "e_flags",                e_flags);
    fprintf(f, "%s%-*s = %u (1-origin)\n",             p, w, "e_autodata_sn",          e_autodata_sn);
    fprintf(f, "%s%-*s = %u bytes\n",                  p, w, "e_bss_size",             e_bss_size);
    fprintf(f, "%s%-*s = %u bytes\n",                  p, w, "e_stack_size",           e_stack_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_csip",                 e_csip);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_sssp",                 e_sssp);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nsections",            e_nsections);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nmodrefs",             e_nmodrefs);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nnonresnames",         e_nnonresnames);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_segtab_rfo",           
                                                       e_sectab_rfo, e_sectab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_rsrctab_rfo",
                                                       e_rsrctab_rfo, e_rsrctab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_resnametab_rfo",
                                                       e_resnametab_rfo, e_resnametab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_modreftab_rfo",
                                                       e_modreftab_rfo, e_modreftab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_importnametab_rfo",
                                                       e_importnametab_rfo, e_importnametab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" byte offset\n",     p, w, "e_nonresnametab_offset", e_nonresnametab_offset);
    fprintf(f, "%s%-*s = %u entries\n",                p, w, "e_nmovable_entries",     e_nmovable_entries);
    fprintf(f, "%s%-*s = %u (log2)\n",                 p, w, "e_sector_align",         e_sector_align);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nresources",           e_nresources);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_exetype",              e_exetype);
    for (size_t i=0; i<NELMTS(e_res1); i++)
        fprintf(f, "%s%-*s = [%zd] 0x%02x\n",           p, w, "e_res1", i, e_res1[i]);
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
// NE Section Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
NESectionTableEntry::ctor(const NESectionTableEntry_disk *disk)
{
    sector          = le_to_host(disk->sector);
    physical_size   = le_to_host(disk->physical_size);
    if (0==physical_size) physical_size = 64*1024;
    flags           = le_to_host(disk->flags);
    virtual_size    = le_to_host(disk->virtual_size);
    if (0==virtual_size) virtual_size = 64*1024;
}

/* Encodes a section table entry back into disk format. */
void *
NESectionTableEntry::encode(NESectionTableEntry_disk *disk)
{
    host_to_le(sector,          &(disk->sector));
    unsigned x_physical_size = physical_size==64*1024 ? 0 : physical_size;
    host_to_le(x_physical_size, &(disk->physical_size));
    host_to_le(flags,           &(disk->flags));
    unsigned x_virtual_size = virtual_size==64*1024 ? 0 : virtual_size;
    host_to_le(x_virtual_size,  &(disk->virtual_size));
    return disk;
}

/* Prints some debugging info */
void
NESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx, NEFileHeader *fhdr)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNESectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNESectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));


    fprintf(f, "%s%-*s = %u",                      p, w, "sector",          sector);
    if (fhdr)
        fprintf(f, " (%"PRIu64" byte offset)", (addr_t)sector << fhdr->e_sector_align);
    fputc('\n', f);
    fprintf(f, "%s%-*s = %"PRIu64" bytes\n",       p, w, "physical_size",   physical_size);
    fprintf(f, "%s%-*s = %"PRIu64" bytes\n",       p, w, "virtual_size",    virtual_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                p, w, "flags",           flags);
}

/* Print some debugging info. */
void
NESection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNESection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNESection.", prefix);
    }

    ExecSection::dump(f, p, -1);
    NEFileHeader *fhdr = dynamic_cast<NEFileHeader*>(get_header());
    st_entry->dump(f, p, -1, fhdr);
}

/* Constructor */
void
NESectionTable::ctor(NEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("NE Section Table");
    set_purpose(SP_HEADER);
    set_header(fhdr);
    
    const size_t entsize = sizeof(NESectionTableEntry_disk);
    for (size_t i=0; i<fhdr->e_nsections; i++) {
        /* Parse the section table entry */
        const NESectionTableEntry_disk *disk = (const NESectionTableEntry_disk*)content(i*entsize, entsize);
        NESectionTableEntry *entry = new NESectionTableEntry(disk);

        /* The section */
        addr_t section_offset = entry->sector << fhdr->e_sector_align;
        NESection *section = new NESection(fhdr->get_file(), section_offset, section_offset==0?0:entry->physical_size);
        section->set_synthesized(false);
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);
        section->set_header(fhdr);
        section->set_st_entry(entry);

        unsigned section_type = entry->flags & SF_TYPE_MASK;
        if (0==section_offset) {
            section->set_name(".bss");
            section->set_readable(true);
            section->set_writable(true);
            section->set_executable(false);
        } else if (0==section_type) {
            section->set_name(".text");
            section->set_readable(true);
            section->set_writable(false);
            section->set_executable(true);
        } else if (section_type & SF_DATA) {
            section->set_name(".data");
            section->set_readable(true);
            section->set_writable((entry->flags & SF_PRELOAD)==SF_PRELOAD ? false : true);
            section->set_executable(false);
        }
    }
}

/* Writes the section table back to disk along with each of the sections. */
void
NESectionTable::unparse(FILE *f)
{
    ExecFile *ef = get_file();
    NEFileHeader *fhdr = dynamic_cast<NEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    std::vector<ExecSection*> sections = ef->get_sections();

    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            NESection *section = dynamic_cast<NESection*>(sections[i]);

            /* Write the table entry */
            ROSE_ASSERT(section->get_id()>0); /*ID's are 1-origin in NE*/
            size_t slot = section->get_id()-1;
            NESectionTableEntry *shdr = section->get_st_entry();
            NESectionTableEntry_disk disk;
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
NESectionTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNESectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNESectionTable.", prefix);
    }
    ExecSection::dump(f, p, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns true if a cursory look at the file indicates that it could be a NE file. */
bool
is_NE(ExecFile *f)
{
    DOS::DOSFileHeader  *dos_hdr = NULL;
    ExtendedDOSHeader   *dos2_hdr = NULL;
    NEFileHeader        *ne_hdr  = NULL;
    bool                retval  = false;

    try {
        dos_hdr = new DOS::DOSFileHeader(f, 0);
        if (dos_hdr->get_magic().size()<2 || dos_hdr->get_magic()[0]!='M' || dos_hdr->get_magic()[1]!='Z') goto done;
        if (dos_hdr->e_relocs_offset!=0x40) goto done;

        dos2_hdr = new ExtendedDOSHeader(f, dos_hdr->get_size());
        ne_hdr = new NEFileHeader(f, dos2_hdr->e_nehdr_offset);
        if (ne_hdr->get_magic().size()!=2 || ne_hdr->get_magic()[0]!=0x4e /*N*/ || ne_hdr->get_magic()[1]!=0x45 /*E*/)
            goto done;
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }
done:
    delete dos_hdr;
    delete dos2_hdr;
    delete ne_hdr;
    return retval;
}

/* Parses the structure of an NE file and adds the information to the ExecFile. */
NEFileHeader *
parse(ExecFile *ef)
{
    ROSE_ASSERT(ef);

    /* All NE files are also DOS files, so parse the DOS part first */
    DOS::DOSFileHeader *dos_header = DOS::parse(ef);
    ROSE_ASSERT(dos_header->e_relocs_offset==0x40);
    ef->unfill_holes(); /*they probably contain NE information*/

    /* NE files extend the DOS header with some additional info */
    ExtendedDOSHeader *dos2_header = new ExtendedDOSHeader(ef, dos_header->get_size());
    
    /* The NE header */
    NEFileHeader *ne_header = new NEFileHeader(ef, dos2_header->e_nehdr_offset);

    /* The extended part of the DOS header is owned by the NE header */
    dos2_header->set_header(ne_header);
    ne_header->set_dos2_header(dos2_header);

    /* Construct the section table and its sections (non-synthesized sections) */
    ne_header->set_section_table(new NESectionTable(ne_header));

    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();

    return ne_header;
}

}; //namespace NE
}; //namespace Exec
