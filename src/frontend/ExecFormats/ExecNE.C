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
    e_lfanew = le_to_host(disk->e_lfanew);
}

/* Encode the extended header back into disk format */
void *
ExtendedDOSHeader::encode(ExtendedDOSHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_res1); i++)
        host_to_le(e_res1[i], &(disk->e_res1[i]));
    host_to_le(e_lfanew, &(disk->e_lfanew));
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
    fprintf(f, "%s%-*s = %"PRIu64" byte offset\n",  p, w, "e_lfanew",   e_lfanew);
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
    e_entrytab_rfo         = le_to_host(fh->e_entrytab_rfo);
    e_entrytab_size        = le_to_host(fh->e_entrytab_size);
    e_checksum             = le_to_host(fh->e_checksum);
    e_flags1               = le_to_host(fh->e_flags1);
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
    e_flags2               = le_to_host(fh->e_flags2); 
    e_fastload_sector      = le_to_host(fh->e_fastload_sector);
    e_fastload_nsectors    = le_to_host(fh->e_fastload_nsectors);
    e_res1                 = le_to_host(fh->e_res1);
    e_winvers              = le_to_host(fh->e_winvers);

    /* Magic number */
    for (size_t i=0; i<sizeof(fh->e_magic); ++i)
        magic.push_back(fh->e_magic[i]);

    /* File format */
    exec_format.family      = FAMILY_NE;
    exec_format.purpose     = e_flags1 & HF1_LIBRARY ? PURPOSE_LIBRARY : PURPOSE_EXECUTABLE;
    exec_format.sex         = ORDER_LSB;
    exec_format.abi         = ABI_NT;
    exec_format.abi_version = 0;
    exec_format.word_size   = 2;
    ROSE_ASSERT(e_linker_major<=0xff && e_linker_minor<=0xff);
    exec_format.version     = (e_linker_major<<8) | e_linker_minor;
    exec_format.is_current_version = true; /*FIXME*/

    /* Target architecture */
    switch (e_exetype) {
      case 0:
        target.set_isa(ISA_UNSPECIFIED);
        break;
      case 1:
        throw FormatError("use of reserved value for Windows NE header e_exetype");
      case 2:
        target.set_isa(ISA_IA32_386);
        break;
      case 3:
      case 4:
        throw FormatError("use of reserved value for Windows NE header e_exetype");
      default:
        target.set_isa(ISA_OTHER, e_exetype);
        break;
    }

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
    host_to_le(e_entrytab_rfo,         &(disk->e_entrytab_rfo));
    host_to_le(e_entrytab_size,        &(disk->e_entrytab_size));
    host_to_le(e_checksum,             &(disk->e_checksum));
    host_to_le(e_flags1,               &(disk->e_flags1));
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
    host_to_le(e_flags2,               &(disk->e_flags2));
    host_to_le(e_fastload_sector,      &(disk->e_fastload_sector));
    host_to_le(e_fastload_nsectors,    &(disk->e_fastload_nsectors));
    host_to_le(e_res1,                 &(disk->e_res1));
    host_to_le(e_winvers,              &(disk->e_winvers));
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

    /* Sections defined in the NE file header */
    if (resname_table)
        resname_table->unparse(f);
    if (nonresname_table)
        nonresname_table->unparse(f);
    if (module_table)
        module_table->unparse(f);
    if (entry_table)
        entry_table->unparse(f);
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
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_entrytab_rfo",      
                                                       e_entrytab_rfo, e_entrytab_rfo+offset);
    fprintf(f, "%s%-*s = %"PRIu64" bytes\n",           p, w, "e_entrytab_size",        e_entrytab_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_checksum",             e_checksum);
    fprintf(f, "%s%-*s = 0x%04x\n",                    p, w, "e_flags1",               e_flags1);
    fprintf(f, "%s%-*s = %u (1-origin)\n",             p, w, "e_autodata_sn",          e_autodata_sn);
    fprintf(f, "%s%-*s = %u bytes\n",                  p, w, "e_bss_size",             e_bss_size);
    fprintf(f, "%s%-*s = %u bytes\n",                  p, w, "e_stack_size",           e_stack_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_csip",                 e_csip);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_sssp",                 e_sssp);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nsections",            e_nsections);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nmodrefs",             e_nmodrefs);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nnonresnames",         e_nnonresnames);
    fprintf(f, "%s%-*s = %"PRIu64" (%"PRIu64" abs)\n", p, w, "e_sectab_rfo",           
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
    fprintf(f, "%s%-*s = 0x%02x\n",                    p, w, "e_flags2",               e_flags2);
    fprintf(f, "%s%-*s = sector %"PRIu64"\n",          p, w, "e_fastload_sector",      e_fastload_sector); 
    fprintf(f, "%s%-*s = %"PRIu64" sectors\n",         p, w, "e_fastload_nsectors",    e_fastload_nsectors); 
    fprintf(f, "%s%-*s = 0x%04x\n",                    p, w, "e_res1",                 e_res1);
    fprintf(f, "%s%-*s = 0x%04x\n",                    p, w, "e_winvers",              e_winvers);

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
    if (resname_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "resname_table",
                resname_table->get_id(), resname_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "resname_table");
    }
    if (nonresname_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "nonresname_table",
                nonresname_table->get_id(), nonresname_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "nonresname_table");
    }
    if (module_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "module_table",
                module_table->get_id(), module_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "module_table");
    }
    if (entry_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "entry_table",
                entry_table->get_id(), entry_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "entry_table");
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
    if (0==physical_size && sector!=0) physical_size = 64*1024;
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

/* Write section back to disk */
void
NESection::unparse(FILE *f)
{
    ExecSection::unparse(f);
    if (reloc_table)
        reloc_table->unparse(f);
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
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);
    NEFileHeader *fhdr = dynamic_cast<NEFileHeader*>(get_header());
    st_entry->dump(f, p, -1, fhdr);
    if (reloc_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "reloc_table",
                reloc_table->get_id(), reloc_table->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "reloc_table");
    }
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
        NESection *section = new NESection(fhdr->get_file(), section_offset, 0==section_offset?0:entry->physical_size);
        section->set_synthesized(false);
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);
        section->set_header(fhdr);
        section->set_st_entry(entry);

        unsigned section_type = entry->flags & SF_TYPE_MASK;
        if (0==section_offset) {
            section->set_name(".bss");
            section->set_readable(true);
            section->set_writable(entry->flags & SF_NOT_WRITABLE ? false : true);
            section->set_executable(false);
        } else if (0==section_type) {
            section->set_name(".text");
            section->set_readable(true);
            section->set_writable(entry->flags & SF_NOT_WRITABLE ? false : true);
            section->set_executable(true);
        } else if (section_type & SF_DATA) {
            section->set_name(".data");
            section->set_readable(true);
            section->set_writable(entry->flags & (SF_PRELOAD|SF_NOT_WRITABLE) ? false : true);
            section->set_executable(false);
        }

        if (entry->flags & SF_RELOCINFO) {
            NERelocTable *relocs = new NERelocTable(fhdr, section->get_offset() + section->get_size());
            section->set_reloc_table(relocs);
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

            /* Write the section and it's optional relocation table */
            section->unparse(f);
            if (section->get_reloc_table())
                section->get_reloc_table()->unparse(f);
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
// NE Resident and Non-Resident Name Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor assumes ExecSection is zero bytes long so far */
void
NENameTable::ctor(NEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("NE Name Table");
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
NENameTable::unparse(FILE *f)
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
NENameTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNENameTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNENameTable.", prefix);
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
// NE Module Reference Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
NEModuleTable::ctor(NEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("NE Module Reference Table");
    set_purpose(SP_HEADER);
    set_header(fhdr);

    ROSE_ASSERT(NULL!=strtab);

    for (addr_t at=0; at<size; at+=2) {
        addr_t name_offset = le_to_host(*(const uint16_t*)content(at, 2));
        name_offsets.push_back(name_offset);
        names.push_back(strtab->get_string(name_offset));
    }

    /* Add libraries to file header */
    for (size_t i=0; i<names.size(); i++) {
        fhdr->add_dll(new ExecDLL(names[i]));
    }
}

/* Writes the section back to disk. */
void
NEModuleTable::unparse(FILE *f)
{
    strtab->unparse(f);

    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    for (size_t i=0; i<name_offsets.size(); i++) {
        uint16_t name_offset_le;
        host_to_le(name_offsets[i], &name_offset_le);
        ssize_t nwrite = fwrite(&name_offset_le, sizeof name_offset_le, 1, f);
        ROSE_ASSERT(1==nwrite);
    }
}
    
/* Prints some debugging info */
void
NEModuleTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNEModuleTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNEModuleTable.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);

    if (strtab) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "strtab", strtab->get_id(), strtab->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "strtab");
    }

    for (size_t i=0; i<names.size(); i++) {
        fprintf(f, "%s%-*s = [%zu] (offset %"PRIu64", %zu bytes) \"%s\"\n",
                p, w, "name", i, name_offsets[i], names[i].size(), names[i].c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE String Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor. We don't parse out the strings here because we want to keep track of what strings are actually referenced by
 * other parts of the file. We can get that information with the congeal() method. */
void
NEStringTable::ctor(NEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("NE String Table");
    set_purpose(SP_HEADER);
    set_header(fhdr);
}
    
/* Returns the string whose size indicator is at the specified offset within the table. There's nothing that prevents OFFSET
 * from pointing to some random location within the string table (but we will throw an exception if offset or the described
 * following string falls outside the string table). */
std::string
NEStringTable::get_string(addr_t offset)
{
    size_t length = content(offset, 1)[0];
    return std::string((const char*)content(offset+1, length), length);
}
    
/* Prints some debugging info */
void
NEStringTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNEStringTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNEStringTable.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);

    congeal();
    addr_t at=0;
    for (size_t i=0; at<get_size(); i++) {
        std::string s = get_string(at);
        char label[64];
        sprintf(label, "string-at-%"PRIu64, at);
        fprintf(f, "%s%-*s = [%zu] (offset %"PRIu64", %zu bytes) \"%s\"\n", p, w, "string", i, at, s.size(), s.c_str());
        at += 1 + s.size();
    }
    uncongeal();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Entry Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
NEEntryTable::ctor(NEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("NE Entry Table");
    set_purpose(SP_HEADER);
    set_header(fhdr);

    addr_t at=0;
    size_t bundle_nentries = content(at++, 1)[0];
    while (bundle_nentries>0) {
        bundle_sizes.push_back(bundle_nentries);
        unsigned segment_indicator = content(at++, 1)[0];
        if (0==segment_indicator) {
            /* Unused entries */
            for (size_t i=0; i<bundle_nentries; i++) {
                entries.push_back(NEEntryPoint());
            }
        } else if (0xff==segment_indicator) {
            /* Movable segment entries. */
            for (size_t i=0; i<bundle_nentries; i++, at+=6) {
                unsigned flags = content(at+0, 1)[0];
                unsigned int3f = le_to_host(*(const uint16_t*)content(at+1, 2));
                ROSE_ASSERT(int3f!=0); /*because we use zero to indicate a fixed entry in unparse()*/
                unsigned segno = content(at+3, 1)[0];
                unsigned segoffset = le_to_host(*(const uint16_t*)content(at+4, 2));
                entries.push_back(NEEntryPoint(flags, int3f, segno, segoffset));
            }
        } else {
            /* Fixed segment entries */
            for (size_t i=0; i<bundle_nentries; i++, at+=3) {
                unsigned flags = content(at+0, 1)[0];
                unsigned segoffset = le_to_host(*(const uint16_t*)content(at+1, 2));
                entries.push_back(NEEntryPoint(flags, 0, segment_indicator, segoffset));
            }
        }
        
        bundle_nentries = content(at++, 1)[0];
    }
}

/* Write section back to disk */
void
NEEntryTable::unparse(FILE *f)
{
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);

    for (size_t bi=0, ei=0; bi<bundle_sizes.size(); ei+=bundle_sizes[bi++]) {
        ROSE_ASSERT(bundle_sizes[bi]>0 && bundle_sizes[bi]<=0xff);
        unsigned char n = bundle_sizes[bi];
        fputc(n, f);

        ROSE_ASSERT(ei+bundle_sizes[bi]<=entries.size());
        if (0==entries[ei].segno) {
            /* Unused entries */
            fputc('\0', f);
        } else if (0==entries[ei].int3f) {
            /* Fixed entries */
            ROSE_ASSERT(entries[ei].segno<=0xff);
            unsigned char n = entries[ei].segno;
            fputc(n, f);
            for (size_t i=0; i<bundle_sizes[bi]; i++) {
                ROSE_ASSERT(entries[ei].segno==entries[ei+i].segno);
                ROSE_ASSERT(entries[ei+i].int3f==0);
                ROSE_ASSERT(entries[ei+i].flags<=0xff);
                n = entries[ei+i].flags;
                fputc(n, f);
                uint16_t eoff_le;
                host_to_le(entries[ei+i].segoffset, &eoff_le);
                fwrite(&eoff_le, sizeof eoff_le, 1, f);
            }
        } else {
            /* Movable entries */
            fputc(0xff, f);
            for (size_t i=0; i<bundle_sizes[bi]; i++) {
                ROSE_ASSERT(entries[ei+i].segno>0);
                ROSE_ASSERT(entries[ei+i].int3f!=0);
                ROSE_ASSERT(entries[ei+i].flags<=0xff);
                n = entries[ei+i].flags;
                fputc(n, f);
                uint16_t word;
                host_to_le(entries[ei+i].int3f, &word);
                fwrite(&word, sizeof word, 1, f);
                ROSE_ASSERT(entries[ei+i].segno<=0xff);
                n = entries[ei+i].segno;
                fputc(n, f);
                host_to_le(entries[ei+i].segoffset, &word);
                fwrite(&word, sizeof word, 1, f);
            }
        }
    }
    fputc('\0', f);
}

/* Print some debugging info */
void
NEEntryTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNEEntryTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNEEntryTable.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu bundles\n", p, w, "nbundles", bundle_sizes.size());
    for (size_t bi=0, ei=0; bi<bundle_sizes.size(); ei+=bundle_sizes[bi++]) {
        fprintf(f, "%s%-*s = [%zu] %zu entries\n", p, w, "bundle_size", bi, bundle_sizes[bi]);
        for (size_t i=0; i<bundle_sizes[bi]; i++) {
            if (0==entries[ei+i].segno) {
                fprintf(f, "%s%-*s = [%zu] %s\n", p, w, "entry_type", ei+i, "unused");
                ROSE_ASSERT(0==entries[ei+i].flags);
                ROSE_ASSERT(0==entries[ei+i].int3f);
                ROSE_ASSERT(0==entries[ei+i].segoffset);
            } else if (0==entries[ei+i].int3f) {
                fprintf(f, "%s%-*s = [%zu] %s\n",     p, w, "entry_type",   ei+i, "fixed");
                fprintf(f, "%s%-*s = [%zu] 0x%02x\n", p, w, "entry_flags",  ei+i, entries[ei+i].flags);
                fprintf(f, "%s%-*s = [%zu] %d\n",     p, w, "entry_segno",  ei+i, entries[ei+i].segno);
                fprintf(f, "%s%-*s = [%zu] 0x%04x\n", p, w, "entry_offset", ei+i, entries[ei+i].segoffset);
            } else {
                fprintf(f, "%s%-*s = [%zu] %s\n",     p, w, "entry_type",   ei+i, "movable");
                fprintf(f, "%s%-*s = [%zu] 0x%02x\n", p, w, "entry_flags",  ei+i, entries[ei+i].flags);
                fprintf(f, "%s%-*s = [%zu] 0x%04x\n", p, w, "entry_int3f",  ei+i, entries[ei+i].int3f);
                fprintf(f, "%s%-*s = [%zu] %d\n",     p, w, "entry_segno",  ei+i, entries[ei+i].segno);
                fprintf(f, "%s%-*s = [%zu] 0x%04x\n", p, w, "entry_offset", ei+i, entries[ei+i].segoffset);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Relocation Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
NERelocEntry::ctor(ExecSection *relocs, addr_t at)
{
    /* The source type:
     *   0x0 => low byte at the specified offset
     *   0x2 => 16-bit selector
     *   0x3 => 32-bit pointer
     *   0x5 => 16-bit offset
     *   0xb => 48-bit pointer
     *   0xd => 32-bit offset
     * Only the low-order 4 bits are used; the high-order bits are reserved. */
    unsigned n = relocs->content(at+0, 1)[0];
    src_type = n & 0x0f;
    res1 = n & ~0x0f;

    /* The target type:
     *   0x0 => internal reference
     *   0x1 => imported ordinal
     *   0x2 => imported name
     *   0x3 => OS fixup
     * Only the low-order 3 bits are used. The fourth bit is the "additive" flag; remaining bits are reserved. */
    n = relocs->content(at+1, 1)[0];
    tgt_type = n & 0x03;
    additive = n & 0x04 ? true : false;
    res2 = n & ~0x07;
    
    /* src_offset is the byte offset into the source section that needs to be patched. If this is an additive relocation then
     * the source will be patched by adding the target value to the value stored at the source. Otherwise the target value is
     * written to the source and the old contents of the source contains the next source offset, until we get 0xffff. */
    src_offset = le_to_host(*(const uint16_t*)relocs->content(at+2, 2));

    switch (tgt_type) {
      case 0x00:
        /* Internal reference */
        iref.segno = relocs->content(at+4, 1)[0];
        iref.res3  = relocs->content(at+5, 1)[0];
        iref.tgt_offset = le_to_host(*(const uint16_t*)relocs->content(at+6, 2));
        break;
      case 0x01:
        /* Imported ordinal */
        iord.modref  = le_to_host(*(const uint16_t*)relocs->content(at+4, 2));
        iord.ordinal = le_to_host(*(const uint16_t*)relocs->content(at+6, 2));
        break;
      case 0x02:
        /* Imported name */
        iname.modref = le_to_host(*(const uint16_t*)relocs->content(at+4, 2));
        iname.nm_off = le_to_host(*(const uint16_t*)relocs->content(at+6, 2));
        break;
      case 0x03:
        /* Operating system fixup */
        osfixup.type = le_to_host(*(const uint16_t*)relocs->content(at+4, 2));
        osfixup.res3 = le_to_host(*(const uint16_t*)relocs->content(at+6, 2));
        break;
    }
}

/* Write entry back to disk at current file offset */
void
NERelocEntry::unparse(FILE *f)
{
    unsigned char byte;
    byte = (res1 & ~0x0f) | (src_type & 0x0f);
    fputc(byte, f);
    byte = (res2 & ~0x07) | (additive?0x04:0x00) | (tgt_type & 0x03);
    fputc(byte, f);
    
    uint16_t word;
    host_to_le(src_offset, &word);
    fwrite(&word, sizeof word, 1, f);
    
    switch (tgt_type) {
      case 0x00:
        host_to_le(iref.segno, &byte);
        fputc(byte, f);
        host_to_le(iref.res3, &byte);
        fputc(byte, f);
        host_to_le(iref.tgt_offset, &word);
        fwrite(&word, sizeof word, 1, f);
        break;
      case 0x01:
        host_to_le(iord.modref, &word);
        fwrite(&word, sizeof word, 1, f);
        host_to_le(iord.ordinal, &word);
        fwrite(&word, sizeof word, 1, f);
        break;
      case 0x02:
        host_to_le(iname.modref, &word);
        fwrite(&word, sizeof word, 1, f);
        host_to_le(iname.nm_off, &word);
        fwrite(&word, sizeof word, 1, f);
        break;
      case 0x03:
        host_to_le(osfixup.type, &word);
        fwrite(&word, sizeof word, 1, f);
        host_to_le(osfixup.res3, &word);
        fwrite(&word, sizeof word, 1, f);
        break;
      default:
        ROSE_ASSERT(!"unknown relocation target type");
    }
}
    
/* Print some debugging info */
void
NERelocEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sRelocEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sRelocEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    const char *s;
    char sbuf[128];
    switch (src_type) {
      case 0x00: s = "low byte";        break;
      case 0x02: s = "16-bit selector"; break;
      case 0x03: s = "32-bit pointer";  break;
      case 0x05: s = "16-bit offset";   break;
      case 0x0b: s = "48-bit pointer";  break;
      case 0x0d: s = "32-bit offset";   break;
      default:
        sprintf(sbuf, "%u", src_type);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n",     p, w, "src_type", s);
    fprintf(f, "%s%-*s = 0x%04u\n", p, w, "res1",     res1);
    
    switch (tgt_type) {
      case 0x00: s = "internal reference"; break;
      case 0x01: s = "imported ordinal";   break;
      case 0x02: s = "imported name";      break;
      case 0x03: s = "OS fixup";           break;
      default:
        sprintf(sbuf, "%u", tgt_type);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n",            p, w, "tgt_type",   s);
    fprintf(f, "%s%-*s = %s\n",            p, w, "additive",   additive?"true":"false");
    fprintf(f, "%s%-*s = 0x%04x\n",        p, w, "res2",       res2);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "src_offset", src_offset);

    switch (tgt_type) {
      case 0x00:
        fprintf(f, "%s%-*s = %u\n",            p, w, "segno",      iref.segno);
        fprintf(f, "%s%-*s = 0x%02x\n",        p, w, "res3",       iref.res3);
        fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "tgt_offset", iref.tgt_offset);
        break;
      case 0x01:
        fprintf(f, "%s%-*s = %u\n",            p, w, "modref",     iord.modref);
        fprintf(f, "%s%-*s = %u\n",            p, w, "ordinal",    iord.ordinal);
        break;
      case 0x02:
        fprintf(f, "%s%-*s = %u\n",            p, w, "modref",     iname.modref);
        fprintf(f, "%s%-*s = %u\n",            p, w, "nm_off",     iname.nm_off);
        break;
      case 0x03:
        fprintf(f, "%s%-*s = %u\n",            p, w, "type",       osfixup.type);
        fprintf(f, "%s%-*s = 0x%04x\n",        p, w, "res3",       osfixup.res3);
        break;
      default:
        ROSE_ASSERT(!"unknown relocation target type");
    }
}
    
/* Constructor. We don't know how large the relocation table is until we're parsing it (specifically, after we've read the
 * number of entries stored in the first two bytes), therefore the section should have an initial size of zero and we extend
 * it as we parse it. */
void
NERelocTable::ctor(NEFileHeader *fhdr)
{
    char name[64];
    sprintf(name, "NE Relocation Table %"PRIu64, offset);
    set_synthesized(true);
    set_name(name);
    set_purpose(SP_HEADER);
    set_header(fhdr);

    ROSE_ASSERT(0==size);
    addr_t at=0;

    extend(2);
    size_t nrelocs = le_to_host(*(const uint16_t*)content(at, 2));
    at += 2;
    
    for (size_t i=0; i<nrelocs; i++, at+=8) {
        extend(8);
        entries.push_back(NERelocEntry(this, at));
    }
}

/* Write relocation table back to disk */
void
NERelocTable::unparse(FILE *f)
{
    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    
    uint16_t size_le;
    host_to_le(entries.size(), &size_le);
    fwrite(&size_le, sizeof size_le, 1, f);
    
    for (size_t i=0; i<entries.size(); i++) {
        entries[i].unparse(f);
    }
}
    
/* Print some debugging info */
void
NERelocTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNERelocTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNERelocTable.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu entries\n", p, w, "size", entries.size());
    for (size_t i=0; i<entries.size(); i++) {
        entries[i].dump(f, p, i);
    }
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
        ne_hdr = new NEFileHeader(f, dos2_hdr->e_lfanew);
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
    DOS::DOSFileHeader *dos_header = DOS::parse(ef, false);
    ROSE_ASSERT(dos_header->e_relocs_offset==0x40);
    ef->unfill_holes(); /*they probably contain NE information*/

    /* NE files extend the DOS header with some additional info */
    ExtendedDOSHeader *dos2_header = new ExtendedDOSHeader(ef, dos_header->get_size());
    
    /* The NE header */
    NEFileHeader *ne_header = new NEFileHeader(ef, dos2_header->e_lfanew);

    /* The extended part of the DOS header is owned by the NE header */
    dos2_header->set_header(ne_header);
    ne_header->set_dos2_header(dos2_header);

    /* Now go back and add the DOS Real-Mode section but rather than using the size specified in the DOS header, constrain it
     * to not extend beyond the beginning of the NE file header. This makes detecting holes in the NE format much easier. */
    dos_header->add_rm_section(ne_header->get_offset());

    /* Sections defined by the NE file header */
    if (ne_header->e_resnametab_rfo>0) {
        addr_t resnames_offset = ne_header->get_offset() + ne_header->e_resnametab_rfo;
        NENameTable *resnames = new NENameTable(ne_header, resnames_offset);
        resnames->set_name("NE Resident Name Table");
        ne_header->set_resname_table(resnames);
    }
    if (ne_header->e_modreftab_rfo>0 &&
        ne_header->e_importnametab_rfo > ne_header->e_modreftab_rfo) {
        /* Imported Name Table must be read before the Module Reference Table since the latter references the former. However,
         * the Imported Name Table comes immediately after the Module Reference Table and before the Entry Table in the file. */
        ROSE_ASSERT(ne_header->e_importnametab_rfo>0);
        ROSE_ASSERT(ne_header->e_entrytab_rfo > ne_header->e_importnametab_rfo);
        addr_t strtab_offset = ne_header->get_offset() + ne_header->e_importnametab_rfo;
        addr_t strtab_size   = ne_header->e_entrytab_rfo - ne_header->e_importnametab_rfo;
        NEStringTable *strtab = new NEStringTable(ne_header, strtab_offset, strtab_size);

        /* Module reference table */
        addr_t modref_offset = ne_header->get_offset() + ne_header->e_modreftab_rfo;
        addr_t modref_size   = ne_header->e_importnametab_rfo - ne_header->e_modreftab_rfo;
        NEModuleTable *modtab = new NEModuleTable(ne_header, modref_offset, modref_size, strtab);
        ne_header->set_module_table(modtab);
    }
    if (ne_header->e_entrytab_rfo>0 && ne_header->e_entrytab_size>0) {
        addr_t enttab_offset = ne_header->get_offset() + ne_header->e_entrytab_rfo;
        addr_t enttab_size = ne_header->e_entrytab_size;
        NEEntryTable *enttab = new NEEntryTable(ne_header, enttab_offset, enttab_size);
        ne_header->set_entry_table(enttab);
    }
    if (ne_header->e_nonresnametab_offset>0) {
        NENameTable *nonres = new NENameTable(ne_header, ne_header->e_nonresnametab_offset);
        nonres->set_name("NE Non-Resident Name Table");
        ne_header->set_nonresname_table(nonres);
    }

    /* Construct the section table and its sections (non-synthesized sections) */
    ne_header->set_section_table(new NESectionTable(ne_header));
    
    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();

    return ne_header;
}

}; //namespace NE
}; //namespace Exec
