/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "ExecNE.h"

namespace Exec {
namespace NE {


// Added to support RTI support in ROSE (not implemented)
std::ostream & operator<< ( std::ostream & os, const NERelocEntry::iref_type & x )
   {
     printf ("Error: operator<< not implemented! \n");
     ROSE_ASSERT(false);

     return os;
   }
   
std::ostream & operator<< ( std::ostream & os, const NERelocEntry::iord_type & x )
   {
     return os;
   }

std::ostream & operator<< ( std::ostream & os, const NERelocEntry::iname_type & x )
   {
     return os;
   }

std::ostream & operator<< ( std::ostream & os, const NERelocEntry::osfixup_type & x )
   {
     return os;
   }


NERelocEntry::iref_type::iref_type()
   {
     sect_idx   = 0;
     res1       = 0;
     tgt_offset = 0;
   }
   
NERelocEntry::iord_type::iord_type()
   {
     modref  = 0;
     ordinal = 0;
     addend  = 0;
   }
   
NERelocEntry::iname_type::iname_type()
   {
     modref = 0;
     nm_off = 0;
     addend = 0;
   }
   
NERelocEntry::osfixup_type::osfixup_type()
   {
     type = 0;
     res3 = 0;
   }


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
    write(f, 0, sizeof disk, &disk);
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

    const NEFileHeader_disk *fh = (const NEFileHeader_disk*)content(0, sizeof(NEFileHeader_disk));

    /* Check magic number early */
    if (fh->e_magic[0]!='N' || fh->e_magic[1]!='E')
        throw FormatError("Bad NE magic number");

    /* Decode file header */
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
    write(f, 0, sizeof fh, &fh);

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
    fprintf(f, "%s%-*s = 0x%08x",                  p, w, "flags",           flags);
    switch (flags & SF_TYPE_MASK) {
      case SF_CODE:  fputs(" code", f);  break;
      case SF_DATA:  fputs(" data", f);  break;
      case SF_ALLOC: fputs(" alloc", f); break;
      case SF_LOAD:  fputs(" load", f);  break;
      default: fprintf(f, " type=%u", flags & SF_TYPE_MASK); break;
    }
    if (flags & SF_MOVABLE)      fputs(" movable",     f);
    if (flags & SF_PURE)         fputs(" pure",        f);
    if (flags & SF_PRELOAD)      fputs(" preload",     f);
    if (flags & SF_NOT_WRITABLE) fputs(" const",       f);
    if (flags & SF_RELOCINFO)    fputs(" reloc",       f);
    if (flags & SF_DISCARDABLE)  fputs(" discardable", f);
    if (flags & SF_DISCARD)      fputs(" discard",     f);
    if (flags & SF_RESERVED)     fputs(" *",           f);
    fputc('\n', f);
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

        /* All NE sections are mapped. There desired address is apparently based on their file offset. */
        addr_t mapped_rva = section_offset - fhdr->get_offset();
        section->set_mapped(mapped_rva, entry->virtual_size);

        unsigned section_type = entry->flags & SF_TYPE_MASK;
        if (0==section_offset) {
            section->set_name(".bss");
            section->set_rperm(true);
            section->set_wperm(entry->flags & SF_NOT_WRITABLE ? false : true);
            section->set_eperm(false);
        } else if (0==section_type) {
            section->set_name(".text");
            section->set_rperm(true);
            section->set_wperm(entry->flags & SF_NOT_WRITABLE ? false : true);
            section->set_eperm(true);
        } else if (section_type & SF_DATA) {
            section->set_name(".data");
            section->set_rperm(true);
            section->set_wperm(entry->flags & (SF_PRELOAD|SF_NOT_WRITABLE) ? false : true);
            section->set_eperm(false);
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
            write(f, slot*sizeof(disk), sizeof disk, &disk);

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
    addr_t spos=0; /*section offset*/
    ROSE_ASSERT(names.size()==ordinals.size());

    for (size_t i=0; i<names.size(); i++) {
        /* Name length */
        ROSE_ASSERT(names[i].size()<=0xff);
        unsigned char len = names[i].size();
        spos = write(f, spos, len);

        /* Name */
        spos = write(f, spos, names[i]);

        /* Ordinal */
        ROSE_ASSERT(ordinals[i]<=0xffff);
        uint16_t ordinal_le;
        host_to_le(ordinals[i], &ordinal_le);
        spos = write(f, spos, sizeof ordinal_le, &ordinal_le);
    }
    
    /* Zero-terminated */
    write(f, spos, '\0');
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

/* Returns all names associated with a particular ordinal */
std::vector<std::string>
NENameTable::get_names_by_ordinal(unsigned ordinal)
{
    std::vector<std::string> retval;
    for (size_t i=0; i<ordinals.size(); i++) {
        if (ordinals[i]==ordinal) {
            retval.push_back(names[i]);
        }
    }
    return retval;
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
    addr_t spos=0; /*section offset*/
    strtab->unparse(f);

    for (size_t i=0; i<name_offsets.size(); i++) {
        uint16_t name_offset_le;
        host_to_le(name_offsets[i], &name_offset_le);
        spos = write(f, spos, sizeof name_offset_le, &name_offset_le);
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

/* Print some debugging info */
void
NEEntryPoint::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sNEEntryPoint[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sNEEntryPoint.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    if (0==section_idx) {
        fprintf(f, "%s%-*s = %s\n", p, w, "type", "unused");
        ROSE_ASSERT(0==flags);
        ROSE_ASSERT(0==int3f);
        ROSE_ASSERT(0==section_offset);
    } else {
        fprintf(f, "%s%-*s = %s\n",         p, w, "type",           0==int3f ? "fixed" : "movable");
        fprintf(f, "%s%-*s = 0x%02x",       p, w, "flags",          flags);
        if (flags & EF_EXPORTED) fputs(" exported", f);
        if (flags & EF_GLOBAL)   fputs(" global",   f);
        if (flags & EF_RESERVED) fputs(" *",        f);
        fputc('\n', f);
        if (int3f)
            fprintf(f, "%s%-*s = 0x%04x\n", p, w, "int3f",          int3f);
        fprintf(f, "%s%-*s = %d\n",         p, w, "section_idx",    section_idx);
        fprintf(f, "%s%-*s = 0x%04x\n",     p, w, "section_offset", section_offset);
    }
}

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
                NEEntryFlags flags = (NEEntryFlags)content(at+0, 1)[0];
                unsigned int3f = le_to_host(*(const uint16_t*)content(at+1, 2));
                ROSE_ASSERT(int3f!=0); /*because we use zero to indicate a fixed entry in unparse()*/
                unsigned segno = content(at+3, 1)[0];
                unsigned segoffset = le_to_host(*(const uint16_t*)content(at+4, 2));
                entries.push_back(NEEntryPoint(flags, int3f, segno, segoffset));
            }
        } else {
            /* Fixed segment entries */
            for (size_t i=0; i<bundle_nentries; i++, at+=3) {
                NEEntryFlags flags = (NEEntryFlags)content(at+0, 1)[0];
                unsigned segoffset = le_to_host(*(const uint16_t*)content(at+1, 2));
                entries.push_back(NEEntryPoint(flags, 0, segment_indicator, segoffset));
            }
        }
        
        bundle_nentries = content(at++, 1)[0];
    }
}

/* Populates the entry_rvas vector of the NE header based on the contents of this Entry Table. The Section (Object) Table must
 * have already been parsed and nonsynthesized sections constructed. */
void
NEEntryTable::populate_entries()
{
    ExecHeader *fhdr = get_header();
    for (size_t i=0; i<entries.size(); i++) {
        const NEEntryPoint& entry = entries[i];
        ExecSection *section;
        if (0==entry.section_idx) {
            /* Unused entry */
        } else if (NULL==(section = get_file()->get_section_by_id(entry.section_idx))) {
            fprintf(stderr, "Ignoring bad entry section_idx (FIXME)\n");
            entry.dump(stderr, "      ", i);
        } else {
            ROSE_ASSERT(section->is_mapped());
            addr_t entry_rva = section->get_mapped_rva() + entry.section_offset;
            fhdr->add_entry_rva(entry_rva);
#if 0 /*DEBUGGING*/
            /* Entry points often have names. Here's how to get them. */
            NEFileHeader *ne_header = dynamic_cast<NEFileHeader*>(fhdr);
            NENameTable *nametab = ne_header->get_nonresname_table();
            std::vector<std::string> names = nametab->get_names_by_ordinal(i+1);
            fprintf(stderr, "ROBB: entry[%zu] (ordinal %zu)\n", i, i+1);
            for (size_t j=0; j<names.size(); j++) {
                fprintf(stderr, "ROBB:     name=\"%s\"\n", names[j].c_str());
            }
#endif
        }
    }
}

/* Write section back to disk */
void
NEEntryTable::unparse(FILE *f)
{
    addr_t spos=0; /*section offset*/

    for (size_t bi=0, ei=0; bi<bundle_sizes.size(); ei+=bundle_sizes[bi++]) {
        ROSE_ASSERT(bundle_sizes[bi]>0 && bundle_sizes[bi]<=0xff);
        unsigned char n = bundle_sizes[bi];
        spos = write(f, spos, n);

        ROSE_ASSERT(ei+bundle_sizes[bi]<=entries.size());
        if (0==entries[ei].section_idx) {
            /* Unused entries */
            spos = write(f, spos, '\0');
        } else if (0==entries[ei].int3f) {
            /* Fixed entries */
            ROSE_ASSERT(entries[ei].section_idx<=0xff);
            unsigned char n = entries[ei].section_idx;
            spos = write(f, spos, n);
            for (size_t i=0; i<bundle_sizes[bi]; i++) {
                ROSE_ASSERT(entries[ei].section_idx==entries[ei+i].section_idx);
                ROSE_ASSERT(entries[ei+i].int3f==0);
                ROSE_ASSERT(entries[ei+i].flags<=0xff);
                n = entries[ei+i].flags;
                spos = write(f, spos, n);
                uint16_t eoff_le;
                host_to_le(entries[ei+i].section_offset, &eoff_le);
                spos = write(f, spos, sizeof eoff_le, &eoff_le);
            }
        } else {
            /* Movable entries */
            spos = write(f, spos, '\377');
            for (size_t i=0; i<bundle_sizes[bi]; i++) {
                ROSE_ASSERT(entries[ei+i].section_idx>0);
                ROSE_ASSERT(entries[ei+i].int3f!=0);
                ROSE_ASSERT(entries[ei+i].flags<=0xff);
                n = entries[ei+i].flags;
                spos = write(f, spos, n);
                uint16_t word;
                host_to_le(entries[ei+i].int3f, &word);
                spos = write(f, spos, sizeof word, &word);
                ROSE_ASSERT(entries[ei+i].section_idx<=0xff);
                n = entries[ei+i].section_idx;
                spos = write(f, spos, n);
                host_to_le(entries[ei+i].section_offset, &word);
                spos = write(f, spos, sizeof word, &word);
            }
        }
    }
    write(f, spos, '\0');
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
    for (size_t i=0; i<bundle_sizes.size(); i++) {
        fprintf(f, "%s%-*s = [%zu] %zu entries\n", p, w, "bundle_size", i, bundle_sizes[i]);
    }
    for (size_t i=0; i<entries.size(); i++) {
        entries[i].dump(f, p, i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Relocation Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor. */
void
NERelocEntry::ctor(ExecSection *relocs, addr_t at, addr_t *rec_size/*out*/)
{
    addr_t orig_at = at;
    ROSE_ASSERT(at==relocs->get_size()); /*the section is extended as we parse*/

    /* Only the low nibble is used for source type; the high nibble is modifier bits */
    relocs->extend(1);
    unsigned n = relocs->content(at++, 1)[0];                              
    src_type = (NERelocSrcType)(n & 0x0f);
    modifier = (NERelocModifiers)(n>>8);

    /* The target type (3 bits), additive flag (1 bit), and target flags */
    relocs->extend(1);
    n = relocs->content(at++, 1)[0];
    tgt_type = (NERelocTgtType)(n & 0x03);
    flags = (NERelocFlags)(n>>2);
    
    /* src_offset is the byte offset into the source section that needs to be patched. If this is an additive relocation then
     * the source will be patched by adding the target value to the value stored at the source. Otherwise the target value is
     * written to the source and the old contents of the source contains the next source offset, until we get 0xffff. */
    relocs->extend(2);
    src_offset = le_to_host(*(const uint16_t*)relocs->content(at, 2));
    at += 2;

    switch (tgt_type) {
      case RF_TGTTYPE_IREF:
        /* Internal reference */
        relocs->extend(4);
        iref.sect_idx = relocs->content(at++, 1)[0];
        iref.res1  = relocs->content(at++, 1)[0];
        iref.tgt_offset = le_to_host(*(const uint16_t*)relocs->content(at, 2));
        at += 2;
        break;
      case RF_TGTTYPE_IORD:
        /* Imported ordinal */
        relocs->extend(4);
        iord.modref  = le_to_host(*(const uint16_t*)relocs->content(at+0, 2));
        iord.ordinal = le_to_host(*(const uint16_t*)relocs->content(at+2, 2));
        at += 4;
        if (flags & RF_2EXTRA) {
            if (flags & RF_32ADD) {
                relocs->extend(4);
                iord.addend = le_to_host(*(const uint32_t*)relocs->content(at+8, 4));
                at += 4;
            } else {
                relocs->extend(2);
                iord.addend = le_to_host(*(const uint16_t*)relocs->content(at+8, 2));
                at += 2;
            }
        } else {
            iord.addend = 0;
        }
        break;
      case RF_TGTTYPE_INAME:
        /* Imported name */
        relocs->extend(4);
        iname.modref = le_to_host(*(const uint16_t*)relocs->content(at+0, 2));
        iname.nm_off = le_to_host(*(const uint16_t*)relocs->content(at+2, 2));
        at += 4;
        if (flags & RF_2EXTRA) {
            if (flags & RF_32ADD) {
                relocs->extend(4);
                iname.addend = le_to_host(*(const uint16_t*)relocs->content(at+8, 4));
                at += 4;
            } else {
                relocs->extend(2);
                iname.addend = le_to_host(*(const uint16_t*)relocs->content(at+8, 2));
                at += 2;
            }
        } else {
            iname.addend = 0;
        }
        break;
      case RF_TGTTYPE_OSFIXUP:
        /* Operating system fixup */
        relocs->extend(4);
        osfixup.type = le_to_host(*(const uint16_t*)relocs->content(at+0, 2));
        osfixup.res3 = le_to_host(*(const uint16_t*)relocs->content(at+2, 2));
        at += 4;
        break;
    }

    if (rec_size)
        *rec_size = at - orig_at;
}

/* Write entry back to disk at the specified section and section offset, returning new offset */
addr_t
NERelocEntry::unparse(FILE *f, ExecSection *section, addr_t spos)
{
    unsigned char byte;
    byte = (modifier << 8) | (src_type & 0x0f);
    spos = section->write(f, spos, byte);
    byte = (flags << 2) | (tgt_type & 0x03);
    spos = section->write(f, spos, byte);
    
    uint16_t word;
    uint32_t dword;
    host_to_le(src_offset, &word);
    spos = section->write(f, spos, sizeof word, &word);
    
    switch (tgt_type) {
      case RF_TGTTYPE_IREF:
        host_to_le(iref.sect_idx, &byte);
        spos = section->write(f, spos, byte);
        host_to_le(iref.res1, &byte);
        spos = section->write(f, spos, byte);
        host_to_le(iref.tgt_offset, &word);
        spos = section->write(f, spos, sizeof word, &word);
        break;
      case RF_TGTTYPE_IORD:
        host_to_le(iord.modref, &word);
        spos = section->write(f, spos, sizeof word, &word);
        host_to_le(iord.ordinal, &word);
        spos = section->write(f, spos, sizeof word, &word);
        if (flags & RF_2EXTRA) {
            if (flags & RF_32ADD) {
                host_to_le(iord.addend, &dword);
                spos = section->write(f, spos, sizeof dword, &dword);
            } else {
                host_to_le(iord.addend, &word);
                spos = section->write(f, spos, sizeof word, &word);
            }
        } else {
            ROSE_ASSERT(iord.addend==0);
        }
        break;
      case RF_TGTTYPE_INAME:
        host_to_le(iname.modref, &word);
        spos = section->write(f, spos, sizeof word, &word);
        host_to_le(iname.nm_off, &word);
        spos = section->write(f, spos, sizeof word, &word);
        if (flags & RF_2EXTRA) {
            if (flags & RF_32ADD) {
                host_to_le(iname.addend, &dword);
                spos = section->write(f, spos, sizeof dword, &dword);
            } else {
                host_to_le(iname.addend, &word);
                spos = section->write(f, spos, sizeof word, &word);
            }
        } else {
            ROSE_ASSERT(iname.addend==0);
        }
        break;
      case RF_TGTTYPE_OSFIXUP:
        host_to_le(osfixup.type, &word);
        spos = section->write(f, spos, sizeof word, &word);
        host_to_le(osfixup.res3, &word);
        spos = section->write(f, spos, sizeof word, &word);
        break;
      default:
        ROSE_ASSERT(!"unknown relocation target type");
    }
    return spos;
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
    switch (src_type) {
      case RF_SRCTYPE_8OFF:     s = "byte offset";     break;
      case RF_SRCTYPE_WORDSEG:  s = "16-bit selector"; break;
      case RF_SRCTYPE_16PTR:    s = "16-bit pointer";  break;
      case RF_SRCTYPE_16OFF:    s = "16-bit offset";   break;
      case RF_SRCTYPE_32PTR:    s = "32-bit pointer";  break;
      case RF_SRCTYPE_32OFF:    s = "32-bit offset";   break;
      case RF_SRCTYPE_NEARCALL: s = "near call/jump";  break;
      case RF_SRCTYPE_48PTR:    s = "48-bit pointer";  break;
      case RF_SRCTYPE_32OFF_b:  s = "32-bit offset";   break;
      default:                  s = "unknown";         break;
    }
    fprintf(f, "%s%-*s = %u (%s)\n", p, w, "src_type", src_type, s);

    switch (modifier) {
      case RF_MODIFIER_SINGLE:  s = "single";          break;
      case RF_MODIFIER_MULTI:   s = "multiple";        break;
      default:                  s = "unknown";         break;
    }
    fprintf(f, "%s%-*s = 0x%04u (%s)\n", p, w, "modifier", modifier, s);
    
    switch (tgt_type) {
      case RF_TGTTYPE_IREF:    s = "internal reference"; break;
      case RF_TGTTYPE_IORD:    s = "imported ordinal";   break;
      case RF_TGTTYPE_INAME:   s = "imported name";      break;
      case RF_TGTTYPE_OSFIXUP: s = "OS fixup";           break;
      default:                 s = "unknown";            break;
    }
    fprintf(f, "%s%-*s = %u (%s)\n",       p, w, "tgt_type",   tgt_type, s);

    fprintf(f, "%s%-*s = 0x%04x",          p, w, "flags", flags);
    if (flags & RF_ADDITIVE)  fputs(" additive",  f);
    if (flags & RF_2EXTRA)    fputs(" 2-extra",   f);
    if (flags & RF_32ADD)     fputs(" 32-add",    f);
    if (flags & RF_16SECTION) fputs(" 16-sect",   f);
    if (flags & RF_8ORDINAL)  fputs(" 8-ordinal", f);
    fputc('\n', f);

    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "src_offset", src_offset);

    switch (tgt_type) {
      case RF_TGTTYPE_IREF:
        fprintf(f, "%s%-*s = %u\n",            p, w, "sect_idx",   iref.sect_idx);
        fprintf(f, "%s%-*s = 0x%02x\n",        p, w, "res3",       iref.res1);
        fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "tgt_offset", iref.tgt_offset);
        break;
      case RF_TGTTYPE_IORD:
        fprintf(f, "%s%-*s = %u\n",            p, w, "modref",     iord.modref);
        fprintf(f, "%s%-*s = %u\n",            p, w, "ordinal",    iord.ordinal);
        fprintf(f, "%s%-*s = %"PRIu64"\n",     p, w, "addend",     iord.addend);
        break;
      case RF_TGTTYPE_INAME:
        fprintf(f, "%s%-*s = %u\n",            p, w, "modref",     iname.modref);
        fprintf(f, "%s%-*s = %u\n",            p, w, "nm_off",     iname.nm_off);
        fprintf(f, "%s%-*s = %"PRIu64"\n",     p, w, "addend",     iname.addend);
        break;
      case RF_TGTTYPE_OSFIXUP:
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
    addr_t at=0, reloc_size;

    extend(2);
    size_t nrelocs = le_to_host(*(const uint16_t*)content(at, 2));
    at += 2;
    
    for (size_t i=0; i<nrelocs; i++, at+=reloc_size) {
        entries.push_back(NERelocEntry(this, at, &reloc_size));
    }
}

/* Write relocation table back to disk */
void
NERelocTable::unparse(FILE *f)
{
    addr_t spos=0; /*section offset*/
    uint16_t size_le;
    host_to_le(entries.size(), &size_le);
    spos = write(f, spos, sizeof size_le, &size_le);
    
    for (size_t i=0; i<entries.size(); i++) {
        spos = entries[i].unparse(f, this, spos);
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
        dos2_hdr = new ExtendedDOSHeader(f, dos_hdr->get_size());
        ne_hdr = new NEFileHeader(f, dos2_hdr->e_lfanew);
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }

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

    /* NE files have multiple entry points that are defined in the Entry Table */
    if (NEEntryTable *enttab = ne_header->get_entry_table())
        enttab->populate_entries();
    
    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();

    return ne_header;
}

}; //namespace NE
}; //namespace Exec
