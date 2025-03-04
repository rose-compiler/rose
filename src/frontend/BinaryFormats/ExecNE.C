/* Copyright 2008 Lawrence Livermore National Security, LLC */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include <ROSE_NELMTS.h>

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

using namespace Rose::Diagnostics;

// Added to support RTI support in ROSE (not implemented)
std::ostream & operator<<(std::ostream&, const SgAsmNERelocEntry::iref_type&)
   {
     printf("Error: operator<< not implemented! \n");
     ROSE_ABORT();
   }
   
std::ostream & operator<<(std::ostream & os, const SgAsmNERelocEntry::iord_type&)
   {
     return os;
   }

std::ostream & operator<<(std::ostream & os, const SgAsmNERelocEntry::iname_type&)
   {
     return os;
   }

std::ostream & operator<<(std::ostream & os, const SgAsmNERelocEntry::osfixup_type&)
   {
     return os;
   }


SgAsmNERelocEntry::iref_type::iref_type()
   {
     sect_idx   = 0;
     res1       = 0;
     tgt_offset = 0;
   }
   
SgAsmNERelocEntry::iord_type::iord_type()
   {
     modref  = 0;
     ordinal = 0;
     addend  = 0;
   }
   
SgAsmNERelocEntry::iname_type::iname_type()
   {
     modref = 0;
     nm_off = 0;
     addend = 0;
   }
   
SgAsmNERelocEntry::osfixup_type::osfixup_type()
   {
     type = 0;
     res3 = 0;
   }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
SgAsmNEFileHeader::SgAsmNEFileHeader(SgAsmGenericFile *f, Rose::BinaryAnalysis::Address offset)
    : SgAsmGenericHeader(f) {
    initializeProperties();

    set_offset(offset);
    set_size(sizeof(NEFileHeader_disk));
    grabContent();

    set_name(new SgAsmBasicString("NE File Header"));
    set_synthesized(true);
    set_purpose(SP_HEADER);

 // DQ (8/16/2008): Added code to set SgAsmPEFileHeader as parent of input SgAsmGenericFile
    f->set_parent(this);

    NEFileHeader_disk fh;
    readContentLocal(0, &fh, sizeof fh);

    /* Check magic number early */
    if (fh.e_magic[0] != 'N' || fh.e_magic[1] != 'E')
        throw FormatError("Bad NE magic number");

    /* Decode file header */
    p_e_linker_major         = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_linker_major);
    p_e_linker_minor         = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_linker_minor);
    p_e_entrytab_rfo         = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_entrytab_rfo);
    p_e_entrytab_size        = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_entrytab_size);
    p_e_checksum             = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_checksum);
    p_e_flags1               = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_flags1);
    p_e_autodata_sn          = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_autodata_sn);
    p_e_bss_size             = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_bss_size);
    p_e_stack_size           = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_stack_size);
    p_e_csip                 = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_csip);
    p_e_sssp                 = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_sssp);
    p_e_nsections            = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_nsections);
    p_e_nmodrefs             = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_nmodrefs);
    p_e_nnonresnames         = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_nnonresnames);
    p_e_sectab_rfo           = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_sectab_rfo);
    p_e_rsrctab_rfo          = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_rsrctab_rfo);
    p_e_resnametab_rfo       = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_resnametab_rfo);
    p_e_modreftab_rfo        = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_modreftab_rfo);
    p_e_importnametab_rfo    = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_importnametab_rfo);
    p_e_nonresnametab_offset = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_nonresnametab_offset);
    p_e_nmovable_entries     = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_nmovable_entries);
    p_e_sector_align         = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_sector_align);
    p_e_nresources           = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_nresources);
    p_e_exetype              = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_exetype);
    p_e_flags2               = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_flags2);
    p_e_fastload_sector      = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_fastload_sector);
    p_e_fastload_nsectors    = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_fastload_nsectors);
    p_e_res1                 = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_res1);
    p_e_winvers              = Rose::BinaryAnalysis::ByteOrder::leToHost(fh.e_winvers);

    /* Magic number */
    for (size_t i = 0; i < sizeof(fh.e_magic); ++i)
        p_magic.push_back(fh.e_magic[i]);

    /* File format */
    get_executableFormat()->set_family(FAMILY_NE);
    get_executableFormat()->set_purpose(p_e_flags1 & HF1_LIBRARY ? PURPOSE_LIBRARY : PURPOSE_EXECUTABLE);
    get_executableFormat()->set_sex(Rose::BinaryAnalysis::ByteOrder::ORDER_LSB);
    get_executableFormat()->set_abi(ABI_NT);
    get_executableFormat()->set_abiVersion(0);
    get_executableFormat()->set_wordSize(2);
    ROSE_ASSERT(p_e_linker_major <= 0xff && p_e_linker_minor <= 0xff);
    get_executableFormat()->set_version((p_e_linker_major<<8) | p_e_linker_minor);
    get_executableFormat()->set_isCurrentVersion(true); /*FIXME*/

    /* Target architecture */
    switch (p_e_exetype) {
      case 0:
        set_isa(ISA_UNSPECIFIED);
        break;
      case 1:
        throw FormatError("use of reserved value for Windows NE header e_exetype");
      case 2:
        set_isa(ISA_IA32_386);
        break;
      case 3:
      case 4:
        throw FormatError("use of reserved value for Windows NE header e_exetype");
      default:
        set_isa(ISA_OTHER);
        break;
    }

    /* Entry point */
//    entry_rva = e_entrypoint_rva; /*FIXME*/
}

bool
SgAsmNEFileHeader::is_NE(SgAsmGenericFile *file)
{
    return isNe(file);
}

/* Return true if the file looks like it might be an NE file according to the magic number.  The file must contain what
 * appears to be a DOS File Header at address zero, and what appears to be an NE File Header at a file offset specified in
 * part of the DOS File Header (actually, in the bytes that follow the DOS File Header). */
bool
SgAsmNEFileHeader::isNe(SgAsmGenericFile *file)
{
    /* Turn off byte reference tracking for the duration of this function. We don't want our testing the file contents to
     * affect the list of bytes that we've already referenced or which we might reference later. */
    bool was_tracking = file->get_trackingReferences();
    file->set_trackingReferences(false);

    try {
        /* Check DOS File Header magic number at beginning of the file */
        unsigned char dos_magic[2];
        file->readContent(0, dos_magic, sizeof dos_magic);
        if ('M'!=dos_magic[0] || 'Z'!=dos_magic[1])
            throw 1;

        /* Read four-byte offset of potential PE File Header at offset 0x3c */
        uint32_t lfanew_disk;
        file->readContent(0x3c, &lfanew_disk, sizeof lfanew_disk);
        Rose::BinaryAnalysis::Address ne_offset = Rose::BinaryAnalysis::ByteOrder::leToHost(lfanew_disk);
        
        /* Look for the NE File Header magic number */
        unsigned char ne_magic[2];
        file->readContent(ne_offset, ne_magic, sizeof ne_magic);
        if ('N'!=ne_magic[0] || 'E'!=ne_magic[1])
            throw 1;
    } catch (...) {
        file->set_trackingReferences(was_tracking);
        return false;
    }
    
    file->set_trackingReferences(was_tracking);
    return true;
}

/* Encode the NE header into disk format */
void *
SgAsmNEFileHeader::encode(SgAsmNEFileHeader::NEFileHeader_disk *disk) const
{
    for (size_t i = 0; i < ROSE_NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_linker_major,         &(disk->e_linker_major));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_linker_minor,         &(disk->e_linker_minor));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_entrytab_rfo,         &(disk->e_entrytab_rfo));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_entrytab_size,        &(disk->e_entrytab_size));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_checksum,             &(disk->e_checksum));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_flags1,               &(disk->e_flags1));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_autodata_sn,          &(disk->e_autodata_sn));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_bss_size,             &(disk->e_bss_size));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_stack_size,           &(disk->e_stack_size));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_csip,                 &(disk->e_csip));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_sssp,                 &(disk->e_sssp));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_nsections,            &(disk->e_nsections));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_nmodrefs,             &(disk->e_nmodrefs));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_nnonresnames,         &(disk->e_nnonresnames));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_sectab_rfo,           &(disk->e_sectab_rfo));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_rsrctab_rfo,          &(disk->e_rsrctab_rfo));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_resnametab_rfo,       &(disk->e_resnametab_rfo));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_modreftab_rfo,        &(disk->e_modreftab_rfo));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_importnametab_rfo,    &(disk->e_importnametab_rfo));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_nonresnametab_offset, &(disk->e_nonresnametab_offset));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_nmovable_entries,     &(disk->e_nmovable_entries));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_sector_align,         &(disk->e_sector_align));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_nresources,           &(disk->e_nresources));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_exetype,              &(disk->e_exetype));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_flags2,               &(disk->e_flags2));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_fastload_sector,      &(disk->e_fastload_sector));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_fastload_nsectors,    &(disk->e_fastload_nsectors));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_res1,                 &(disk->e_res1));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_winvers,              &(disk->e_winvers));

    return disk;
}

/* Write the NE file header back to disk and all that it references */
void
SgAsmNEFileHeader::unparse(std::ostream &f) const
{
    NEFileHeader_disk fh;
    encode(&fh);
    write(f, 0, sizeof fh, &fh);

    /* The extended DOS header */
    if (get_dos2Header())
        get_dos2Header()->unparse(f);

    /* The section table and all the non-synthesized sections */
    if (get_sectionTable())
        get_sectionTable()->unparse(f);

    /* Sections defined in the NE file header */
    if (get_residentNameTable())
        get_residentNameTable()->unparse(f);
    if (get_nonresidentNameTable())
        get_nonresidentNameTable()->unparse(f);
    if (get_moduleTable())
        get_moduleTable()->unparse(f);
    if (get_entryTable())
        get_entryTable()->unparse(f);
}
    
/* Print some debugging information */
void
SgAsmNEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNEFileHeader[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNEFileHeader.", prefix);
    }

    int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_linker_major",         p_e_linker_major);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_linker_minor",         p_e_linker_minor);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_entrytab_rfo",      
                                                       p_e_entrytab_rfo, p_e_entrytab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",         p, w, "e_entrytab_size",        p_e_entrytab_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_checksum",             p_e_checksum);
    fprintf(f, "%s%-*s = 0x%04x\n",                    p, w, "e_flags1",               p_e_flags1);
    fprintf(f, "%s%-*s = %u (1-origin)\n",             p, w, "e_autodata_sn",          p_e_autodata_sn);
    fprintf(f, "%s%-*s = %u bytes\n",                  p, w, "e_bss_size",             p_e_bss_size);
    fprintf(f, "%s%-*s = %u bytes\n",                  p, w, "e_stack_size",           p_e_stack_size);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_csip",                 p_e_csip);
    fprintf(f, "%s%-*s = 0x%08x\n",                    p, w, "e_sssp",                 p_e_sssp);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nsections",            p_e_nsections);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nmodrefs",             p_e_nmodrefs);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nnonresnames",         p_e_nnonresnames);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_sectab_rfo",           
                                                       p_e_sectab_rfo, p_e_sectab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_rsrctab_rfo",
                                                       p_e_rsrctab_rfo, p_e_rsrctab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_resnametab_rfo",
                                                       p_e_resnametab_rfo, p_e_resnametab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_modreftab_rfo",
                                                       p_e_modreftab_rfo, p_e_modreftab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " (%" PRIu64 " abs)\n", p, w, "e_importnametab_rfo",
                                                       p_e_importnametab_rfo, p_e_importnametab_rfo+p_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " byte offset\n",   p, w, "e_nonresnametab_offset", p_e_nonresnametab_offset);
    fprintf(f, "%s%-*s = %u entries\n",                p, w, "e_nmovable_entries",     p_e_nmovable_entries);
    fprintf(f, "%s%-*s = %u (log2)\n",                 p, w, "e_sector_align",         p_e_sector_align);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_nresources",           p_e_nresources);
    fprintf(f, "%s%-*s = %u\n",                        p, w, "e_exetype",              p_e_exetype);
    fprintf(f, "%s%-*s = 0x%02x\n",                    p, w, "e_flags2",               p_e_flags2);
    fprintf(f, "%s%-*s = sector %" PRIu64 "\n",        p, w, "e_fastload_sector",      p_e_fastload_sector); 
    fprintf(f, "%s%-*s = %" PRIu64 " sectors\n",       p, w, "e_fastload_nsectors",    p_e_fastload_nsectors); 
    fprintf(f, "%s%-*s = 0x%04x\n",                    p, w, "e_res1",                 p_e_res1);
    fprintf(f, "%s%-*s = 0x%04x\n",                    p, w, "e_winvers",              p_e_winvers);

    if (get_dos2Header()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "dos2_header",
                get_dos2Header()->get_id(), get_dos2Header()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "dos2_header");
    }
    if (get_sectionTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "section_table",
                get_sectionTable()->get_id(), get_sectionTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "section_table");
    }
    if (get_residentNameTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "resname_table",
                get_residentNameTable()->get_id(), get_residentNameTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "resname_table");
    }
    if (get_nonresidentNameTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "nonresname_table",
                get_nonresidentNameTable()->get_id(), get_nonresidentNameTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "nonresname_table");
    }
    if (get_moduleTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "module_table",
                get_moduleTable()->get_id(), get_moduleTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "module_table");
    }
    if (get_entryTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "entry_table",
                get_entryTable()->get_id(), get_entryTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "entry_table");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Section Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmNESectionTableEntry::SgAsmNESectionTableEntry(const SgAsmNESectionTableEntry::NESectionTableEntry_disk *disk) {
    initializeProperties();
    ASSERT_not_null(disk);
    p_sector          = Rose::BinaryAnalysis::ByteOrder::leToHost(disk->sector);
    p_physicalSize   = Rose::BinaryAnalysis::ByteOrder::leToHost(disk->physical_size);
    if (0==p_physicalSize && p_sector!=0) p_physicalSize = 64*1024;
    p_flags           = Rose::BinaryAnalysis::ByteOrder::leToHost(disk->flags);
    p_virtualSize    = Rose::BinaryAnalysis::ByteOrder::leToHost(disk->virtual_size);
    if (0==p_virtualSize) p_virtualSize = 64*1024;
}

/* Encodes a section table entry back into disk format. */
void *
SgAsmNESectionTableEntry::encode(NESectionTableEntry_disk *disk) const
{
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_sector,          &(disk->sector));
    unsigned x_physical_size = p_physicalSize==64*1024 ? 0 : p_physicalSize;
    Rose::BinaryAnalysis::ByteOrder::hostToLe(x_physical_size, &(disk->physical_size));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_flags,           &(disk->flags));
    unsigned x_virtual_size = p_virtualSize==64*1024 ? 0 : p_virtualSize;
    Rose::BinaryAnalysis::ByteOrder::hostToLe(x_virtual_size,  &(disk->virtual_size));
    return disk;
}

/* Prints some debugging info */
void
SgAsmNESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmNEFileHeader *fhdr) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNESectionTableEntry[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNESectionTableEntry.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = %u",                      p, w, "sector",          p_sector);
    if (fhdr)
        fprintf(f, " (%" PRIu64 " byte offset)", (Rose::BinaryAnalysis::Address) p_sector << fhdr->get_e_sector_align());
    fputc('\n', f);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",     p, w, "physical_size",   p_physicalSize);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",     p, w, "virtual_size",    p_virtualSize);
    fprintf(f, "%s%-*s = 0x%08x",                  p, w, "flags",           p_flags);
    switch (p_flags & SF_TYPE_MASK) {
      case SF_CODE:  fputs(" code", f);  break;
      case SF_DATA:  fputs(" data", f);  break;
      case SF_ALLOC: fputs(" alloc", f); break;
      case SF_LOAD:  fputs(" load", f);  break;
      default: fprintf(f, " type=%u", p_flags & SF_TYPE_MASK); break;
    }
    if (p_flags & SF_MOVABLE)      fputs(" movable",     f);
    if (p_flags & SF_PURE)         fputs(" pure",        f);
    if (p_flags & SF_PRELOAD)      fputs(" preload",     f);
    if (p_flags & SF_NOT_WRITABLE) fputs(" const",       f);
    if (p_flags & SF_RELOCINFO)    fputs(" reloc",       f);
    if (p_flags & SF_DISCARDABLE)  fputs(" discardable", f);
    if (p_flags & SF_DISCARD)      fputs(" discard",     f);
    if (p_flags & SF_RESERVED)     fputs(" *",           f);
    fputc('\n', f);
}

SgAsmNESection::SgAsmNESection(SgAsmNEFileHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();
    set_sectionTableEntry(nullptr);
    set_relocationTable(nullptr);
}

/* Write section back to disk */
void
SgAsmNESection::unparse(std::ostream &f) const
{
    SgAsmGenericSection::unparse(f);
    if (get_relocationTable())
        get_relocationTable()->unparse(f);
}
    
/* Print some debugging info. */
void
SgAsmNESection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNESection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNESection.", prefix);
    }

        const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    SgAsmNEFileHeader *fhdr = dynamic_cast<SgAsmNEFileHeader*>(get_header());
    get_sectionTableEntry()->dump(f, p, -1, fhdr);
    if (get_relocationTable()) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "reloc_table",
                get_relocationTable()->get_id(), get_relocationTable()->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "reloc_table");
    }
}

/* Constructor */
SgAsmNESectionTable::SgAsmNESectionTable(SgAsmNEFileHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(fhdr->get_endOffset());
    set_size(fhdr->get_e_nsections() * sizeof(SgAsmNESectionTableEntry::NESectionTableEntry_disk));

    grabContent();

    set_synthesized(true);
    set_name(new SgAsmBasicString("NE Section Table"));
    set_purpose(SP_HEADER);

    const size_t entsize = sizeof(SgAsmNESectionTableEntry::NESectionTableEntry_disk);

    for (size_t i = 0; i < fhdr->get_e_nsections(); i++) {
        /* Parse the section table entry */
        SgAsmNESectionTableEntry::NESectionTableEntry_disk disk;
        readContentLocal(i*entsize, &disk, entsize);
        SgAsmNESectionTableEntry *entry = new SgAsmNESectionTableEntry(&disk);

        /* The section */
        Rose::BinaryAnalysis::Address section_offset = entry->get_sector() << fhdr->get_e_sector_align();
        SgAsmNESection *section = new SgAsmNESection(fhdr);
        section->set_offset(section_offset);
        section->set_size(0==section_offset ? 0 : entry->get_physicalSize());
        section->grabContent();
        section->set_synthesized(false);
        section->set_id(i+1); /*numbered starting at 1, not zero*/
        section->set_purpose(SP_PROGRAM);
        section->set_sectionTableEntry(entry);

        /* All NE sections are mapped. Their desired address is apparently based on their file offset. */
        Rose::BinaryAnalysis::Address mapped_rva = section_offset - fhdr->get_offset();
        section->set_mappedPreferredRva(mapped_rva);
        section->set_mappedActualVa(0); /*assigned by Loader*/
        section->set_mappedSize(entry->get_virtualSize());

        unsigned section_type = entry->get_flags() & SgAsmNESectionTableEntry::SF_TYPE_MASK;
        if (0 == section_offset) {
            section->set_name(new SgAsmBasicString(".bss"));
            section->set_mappedReadPermission(true);
            section->set_mappedWritePermission(entry->get_flags() & SgAsmNESectionTableEntry::SF_NOT_WRITABLE ? false : true);
            section->set_mappedExecutePermission(false);
        } else if (0 == section_type) {
            section->set_name(new SgAsmBasicString(".text"));
            section->set_mappedReadPermission(true);
            section->set_mappedWritePermission(entry->get_flags() & SgAsmNESectionTableEntry::SF_NOT_WRITABLE ? false : true);
            section->set_mappedExecutePermission(true);
        } else if (section_type & SgAsmNESectionTableEntry::SF_DATA) {
            section->set_name(new SgAsmBasicString(".data"));
            section->set_mappedReadPermission(true);
            section->set_mappedWritePermission(entry->get_flags() & (SgAsmNESectionTableEntry::SF_PRELOAD |
                                                            SgAsmNESectionTableEntry::SF_NOT_WRITABLE) ? false : true);
            section->set_mappedExecutePermission(false);
        }

        if (entry->get_flags() & SgAsmNESectionTableEntry::SF_RELOCINFO) {
            SgAsmNERelocTable *relocs = new SgAsmNERelocTable(fhdr, section);
            section->set_relocationTable(relocs);
        }
    }
}

/* Writes the section table back to disk along with each of the sections. */
void
SgAsmNESectionTable::unparse(std::ostream &f) const
{
    SgAsmNEFileHeader *fhdr = dynamic_cast<SgAsmNEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();

    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            SgAsmNESection *section = dynamic_cast<SgAsmNESection*>(sections[i]);

            /* Write the table entry */
            ROSE_ASSERT(section->get_id()>0); /*ID's are 1-origin in NE*/
            size_t slot = section->get_id()-1;
            SgAsmNESectionTableEntry *shdr = section->get_sectionTableEntry();
            SgAsmNESectionTableEntry::NESectionTableEntry_disk disk;
            shdr->encode(&disk);
            write(f, slot*sizeof(disk), sizeof disk, &disk);

            /* Write the section and it's optional relocation table */
            section->unparse(f);
            if (section->get_relocationTable())
                section->get_relocationTable()->unparse(f);
        }
    }
}

/* Prints some debugging info */
void
SgAsmNESectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNESectionTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNESectionTable.", prefix);
    }
    SgAsmGenericSection::dump(f, p, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Resident and Non-Resident Name Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor assumes SgAsmGenericSection is zero bytes long so far */
SgAsmNENameTable::SgAsmNENameTable(SgAsmNEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(offset);
    set_size(0);
    grabContent();

    set_synthesized(true);
    set_name(new SgAsmBasicString("NE Name Table"));
    set_purpose(SP_HEADER);

    /* Resident exported procedure names, until we hit a zero length name. The first name
     * is for the library itself and the corresponding ordinal has no meaning. */
    Rose::BinaryAnalysis::Address at = 0;
    while (1) {
        extend(1);
        unsigned char byte;
        readContentLocal(at++, &byte, 1);
        size_t length = byte;
        if (0==length) break;

        extend(length);
        char *buf = new char[length];
        readContentLocal(at, buf, length);
        p_names.push_back(std::string(buf, length));
        delete[] buf;
        at += length;

        extend(2);
        uint16_t u16_disk;
        readContentLocal(at, &u16_disk, 2);
        p_ordinals.push_back(Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk));
        at += 2;
    }
}

/* Writes the section back to disk. */
void
SgAsmNENameTable::unparse(std::ostream &f) const
{
    Rose::BinaryAnalysis::Address spos=0; /*section offset*/
    ROSE_ASSERT(p_names.size() == p_ordinals.size());

    for (size_t i = 0; i < p_names.size(); i++) {
        /* Name length */
        ROSE_ASSERT(p_names[i].size() <= 0xff);
        unsigned char len = p_names[i].size();
        spos = write(f, spos, len);

        /* Name */
        spos = write(f, spos, p_names[i]);

        /* Ordinal */
        ROSE_ASSERT(p_ordinals[i]<=0xffff);
        uint16_t ordinal_le;
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_ordinals[i], &ordinal_le);
        spos = write(f, spos, sizeof ordinal_le, &ordinal_le);
    }
    
    /* Zero-terminated */
    write(f, spos, '\0');
}

/* Prints some debugging info */
void
SgAsmNENameTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNENameTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNENameTable.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    ROSE_ASSERT(p_names.size() == p_ordinals.size());
    for (size_t i = 0; i < p_names.size(); i++) {
        fprintf(f, "%s%-*s = [%zd] \"%s\"\n", p, w, "names",    i, escapeString(p_names[i]).c_str());
        fprintf(f, "%s%-*s = [%zd] %u\n",     p, w, "ordinals", i, p_ordinals[i]);
    }
}

std::vector<std::string>
SgAsmNENameTable::get_names_by_ordinal(unsigned ordinal)
{
    return get_namesByOrdinal(ordinal);
}

/* Returns all names associated with a particular ordinal */
std::vector<std::string>
SgAsmNENameTable::get_namesByOrdinal(unsigned ordinal)
{
    std::vector<std::string> retval;
    for (size_t i = 0; i < p_ordinals.size(); i++) {
        if (p_ordinals[i] == ordinal) {
            retval.push_back(p_names[i]);
        }
    }
    return retval;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Module Reference Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
SgAsmNEModuleTable::SgAsmNEModuleTable(SgAsmNEFileHeader *fhdr, SgAsmNEStringTable *strtab, Rose::BinaryAnalysis::Address offset,
                                       Rose::BinaryAnalysis::Address size)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();
    p_strtab = strtab;

    set_offset(offset);
    set_size(size);
    grabContent();

    set_synthesized(true);
    set_name(new SgAsmBasicString("NE Module Reference Table"));
    set_purpose(SP_HEADER);

    ASSERT_not_null(p_strtab);

    for (Rose::BinaryAnalysis::Address at = 0; at < get_size(); at += 2) {
        uint16_t u16_disk;
        readContentLocal(at, &u16_disk, 2);
        Rose::BinaryAnalysis::Address name_offset = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
        p_nameOffsets.push_back(name_offset);
        p_names.push_back(p_strtab->get_string(name_offset));
    }

    /* Add libraries to file header */
    for (size_t i = 0; i < p_names.size(); i++) {
        fhdr->addDll(new SgAsmGenericDLL(new SgAsmBasicString(p_names[i])));
    }
}

/* Writes the section back to disk. */
void
SgAsmNEModuleTable::unparse(std::ostream &f) const
{
    Rose::BinaryAnalysis::Address spos = 0; /*section offset*/
    p_strtab->unparse(f);

    for (size_t i = 0; i < p_nameOffsets.size(); i++) {
        uint16_t name_offset_le;
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_nameOffsets[i], &name_offset_le);
        spos = write(f, spos, sizeof name_offset_le, &name_offset_le);
    }
}
    
/* Prints some debugging info */
void
SgAsmNEModuleTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNEModuleTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNEModuleTable.", prefix);
    }

        const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);

    if (p_strtab) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "strtab",
                p_strtab->get_id(), p_strtab->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "strtab");
    }

    for (size_t i = 0; i < p_names.size(); i++) {
        fprintf(f, "%s%-*s = [%" PRIuPTR "] (offset %" PRIu64 ", %" PRIuPTR " bytes) \"%s\"\n",
                p, w, "name", i, p_nameOffsets[i], p_names[i].size(), escapeString(p_names[i]).c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE String Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor. We don't parse out the strings here because we want to keep track of what strings are actually referenced by
 * other parts of the file. We can get that information with the congeal() method. */
SgAsmNEStringTable::SgAsmNEStringTable(SgAsmNEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset,
                                       Rose::BinaryAnalysis::Address size)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(offset);
    set_size(size);
    grabContent();

    set_synthesized(true);
    set_name(new SgAsmBasicString("NE String Table"));
    set_purpose(SP_HEADER);
}
    
/* Returns the string whose size indicator is at the specified offset within the table. There's nothing that prevents OFFSET
 * from pointing to some random location within the string table (but we will throw an exception if offset or the described
 * following string falls outside the string table). */
std::string
SgAsmNEStringTable::get_string(Rose::BinaryAnalysis::Address offset)
{
    unsigned char byte;
    readContentLocal(offset, &byte, 1);
    size_t length = byte;

    char *buf = new char[length];
    readContentLocal(offset+1, buf, length);
    std::string retval(buf, length);
    delete[] buf;
    return retval;
}

/* Prints some debugging info */
void
SgAsmNEStringTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNEStringTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNEStringTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

#if 0 /*Can't parse strings because it would affect the list of referenced bytes*/
    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));
    bool was_congealed = get_congealed();
    congeal();
    Rose::BinaryAnalysis::Address at=0;
    for (size_t i=0; at<get_size(); i++) {
        std::string s = get_string(at);
        char label[64];
        snprintf(label, sizeof(label), "string-at-%" PRIu64, at);
        fprintf(f, "%s%-*s = [%" PRIuPTR "] (offset %" PRIu64 ", %" PRIuPTR " bytes) \"%s\"\n",
                p, w, "string", i, at, s.size(), s.c_str());
        at += 1 + s.size();
    }
    if (!was_congealed)
      uncongeal();
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Entry Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmNEEntryPoint::SgAsmNEEntryPoint(SgAsmNEEntryPoint::NEEntryFlags flags, unsigned int3f, unsigned s_idx, unsigned s_off) {
    initializeProperties();
    set_flags(flags);
    set_int3f(int3f);
    set_sectionIndex(s_idx);
    set_sectionOffset(s_off);
}

/* Print some debugging info */
void
SgAsmNEEntryPoint::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNEEntryPoint[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNEEntryPoint.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    if (0 == get_sectionIndex()) {
        fprintf(f, "%s%-*s = %s\n", p, w, "type", "unused");
        ROSE_ASSERT(SgAsmNEEntryPoint::EF_ZERO == p_flags);
        ROSE_ASSERT(0 == p_int3f);
        ROSE_ASSERT(0 == get_sectionOffset());
    } else {
        fprintf(f, "%s%-*s = %s\n",         p, w, "type",           0 == p_int3f ? "fixed" : "movable");
        fprintf(f, "%s%-*s = 0x%02x",       p, w, "flags",          p_flags);
        if (p_flags & EF_EXPORTED) fputs(" exported", f);
        if (p_flags & EF_GLOBAL)   fputs(" global",   f);
        if (p_flags & EF_RESERVED) fputs(" *",        f);
        fputc('\n', f);
        if (p_int3f)
            fprintf(f, "%s%-*s = 0x%04x\n", p, w, "int3f",          p_int3f);
        fprintf(f, "%s%-*s = %d\n",         p, w, "section_idx",    get_sectionIndex());
        fprintf(f, "%s%-*s = 0x%04x\n",     p, w, "section_offset", get_sectionOffset());
    }
}

/* Constructor */
SgAsmNEEntryTable::SgAsmNEEntryTable(SgAsmNEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset,
                                     Rose::BinaryAnalysis::Address size)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    set_offset(offset);
    set_size(size);
    grabContent();

    set_synthesized(true);
    set_name(new SgAsmBasicString("NE Entry Table"));
    set_purpose(SP_HEADER);

    unsigned char byte;
    uint16_t u16_disk;

    Rose::BinaryAnalysis::Address at = 0;
    readContentLocal(at++, &byte, 1);
    size_t bundle_nentries = byte;
    while (bundle_nentries > 0) {
        p_bundle_sizes.push_back(bundle_nentries);
        readContentLocal(at++, &byte, 1);
        unsigned segment_indicator = byte;
        if (0 == segment_indicator) {
            /* Unused entries */
            for (size_t i = 0; i < bundle_nentries; i++) {
                p_entries.push_back(new SgAsmNEEntryPoint());
            }
        } else if (0xff == segment_indicator) {
            /* Movable segment entries. */
            for (size_t i = 0; i < bundle_nentries; i++, at+=6) {
                readContentLocal(at, &byte, 1);
                SgAsmNEEntryPoint::NEEntryFlags flags = (SgAsmNEEntryPoint::NEEntryFlags)byte;
                readContentLocal(at+1, &u16_disk, 2);
                unsigned int3f = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
                ROSE_ASSERT(int3f!=0); /*because we use zero to indicate a fixed entry in unparse()*/
                readContentLocal(at+3, &byte, 1);
                unsigned segno = byte;
                readContentLocal(at+4, &u16_disk, 2);
                unsigned segoffset = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
                p_entries.push_back(new SgAsmNEEntryPoint(flags, int3f, segno, segoffset));
            }
        } else {
            /* Fixed segment entries */
            for (size_t i = 0; i < bundle_nentries; i++, at+=3) {
                readContentLocal(at, &byte, 1);
                SgAsmNEEntryPoint::NEEntryFlags flags = (SgAsmNEEntryPoint::NEEntryFlags)byte;
                readContentLocal(at+1, &u16_disk, 2);
                unsigned segoffset = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
                p_entries.push_back(new SgAsmNEEntryPoint(flags, 0, segment_indicator, segoffset));
            }
        }
        
        readContentLocal(at++, &byte, 1);
        bundle_nentries = byte;
    }
}

/* Populates the entryRvas vector of the NE header based on the contents of this Entry Table. The Section (Object) Table must
 * have already been parsed and nonsynthesized sections constructed. */
void
SgAsmNEEntryTable::populate_entries()
{
    SgAsmGenericHeader *fhdr = get_header();
    for (size_t i=0; i < p_entries.size(); i++) {
        const SgAsmNEEntryPoint & entry = *(p_entries[i]);
        SgAsmGenericSection *section = NULL;
        if (0 == entry.get_sectionIndex()) {
            /* Unused entry */
        } else if (NULL == (section = get_file()->get_sectionById(entry.get_sectionIndex()))) {
            mlog[WARN] <<"ignoring bad entry section_idx\n"; // FIXME[Robb P. Matzke 2015-07-07]
            entry.dump(stderr, "      ", i);
        } else {
            ROSE_ASSERT(section->isMapped());
            Rose::BinaryAnalysis::Address entry_rva = section->get_mappedPreferredRva() + entry.get_sectionOffset();
            fhdr->addEntryRva(entry_rva);
#if 0 /*DEBUGGING*/
            /* Entry points often have names. Here's how to get them. */
            SgAsmNEFileHeader *ne_header = dynamic_cast<SgAsmNEFileHeader*>(fhdr);
            SgAsmNENameTable *nametab = ne_header->get_nonresname_table();
            std::vector<std::string> names = nametab->get_names_by_ordinal(i+1);
            fprintf(stderr, "ROBB: entry[%" PRIuPTR "] (ordinal %" PRIuPTR ")\n", i, i+1);
            for (size_t j = 0; j < p_names.size(); j++) {
                fprintf(stderr, "ROBB:     name=\"%s\"\n", p_names[j].c_str());
            }
#endif
        }
    }
}

/* Write section back to disk */
void
SgAsmNEEntryTable::unparse(std::ostream &f) const
{
    Rose::BinaryAnalysis::Address spos=0; /*section offset*/

    for (size_t bi=0, ei=0; bi < p_bundle_sizes.size(); ei += p_bundle_sizes[bi++]) {
        ROSE_ASSERT(p_bundle_sizes[bi] > 0 && p_bundle_sizes[bi] <= 0xff);
        unsigned char n = p_bundle_sizes[bi];
        spos = write(f, spos, n);

        ROSE_ASSERT(ei + p_bundle_sizes[bi] <= p_entries.size());
        if (0 == p_entries[ei]->get_sectionIndex()) {
            /* Unused entries */
            spos = write(f, spos, '\0');
        } else if (0 == p_entries[ei]->get_int3f()) {
            /* Fixed entries */
            ROSE_ASSERT(p_entries[ei]->get_sectionIndex() <= 0xff);
            unsigned char n = p_entries[ei]->get_sectionIndex();
            spos = write(f, spos, n);
            for (size_t i = 0; i < p_bundle_sizes[bi]; i++) {
                ROSE_ASSERT(p_entries[ei]->get_sectionIndex() == p_entries[ei+i]->get_sectionIndex());
                ROSE_ASSERT(p_entries[ei+i]->get_int3f() == 0);
                ROSE_ASSERT(p_entries[ei+i]->get_flags() <= 0xff);
                n = p_entries[ei+i]->get_flags();
                spos = write(f, spos, n);
                uint16_t eoff_le;
                Rose::BinaryAnalysis::ByteOrder::hostToLe(p_entries[ei+i]->get_sectionOffset(), &eoff_le);
                spos = write(f, spos, sizeof eoff_le, &eoff_le);
            }
        } else {
            /* Movable entries */
            spos = write(f, spos, '\377');
            for (size_t i = 0; i < p_bundle_sizes[bi]; i++) {
                ROSE_ASSERT(p_entries[ei+i]->get_sectionIndex() > 0);
                ROSE_ASSERT(p_entries[ei+i]->get_int3f() != 0);
                ROSE_ASSERT(p_entries[ei+i]->get_flags() <= 0xff);
                n = p_entries[ei+i]->get_flags();
                spos = write(f, spos, n);
                uint16_t word;
                Rose::BinaryAnalysis::ByteOrder::hostToLe(p_entries[ei+i]->get_int3f(), &word);
                spos = write(f, spos, sizeof word, &word);
                ROSE_ASSERT(p_entries[ei+i]->get_sectionIndex() <= 0xff);
                n = p_entries[ei+i]->get_sectionIndex();
                spos = write(f, spos, n);
                Rose::BinaryAnalysis::ByteOrder::hostToLe(p_entries[ei+i]->get_sectionOffset(), &word);
                spos = write(f, spos, sizeof word, &word);
            }
        }
    }
    write(f, spos, '\0');
}

/* Print some debugging info */
void
SgAsmNEEntryTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNEEntryTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNEEntryTable.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %" PRIuPTR " bundles\n", p, w, "nbundles", p_bundle_sizes.size());
    for (size_t i = 0; i < p_bundle_sizes.size(); i++) {
        fprintf(f, "%s%-*s = [%" PRIuPTR "] %" PRIuPTR " entries\n", p, w, "bundle_size", i, p_bundle_sizes[i]);
    }
    for (size_t i = 0; i < p_entries.size(); i++) {
        p_entries[i]->dump(f, p, i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Relocation Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor. */
SgAsmNERelocEntry::SgAsmNERelocEntry(SgAsmGenericSection *relocs, Rose::BinaryAnalysis::Address at,
                                     Rose::BinaryAnalysis::Address *rec_size) {
    initializeProperties();

    unsigned char byte;
    uint16_t u16_disk;
    uint32_t u32_disk;

    Rose::BinaryAnalysis::Address orig_at = at;
    ROSE_ASSERT(at == relocs->get_size()); /*the section is extended as we parse*/

    /* Only the low nibble is used for source type; the high nibble is modifier bits */
    relocs->extend(1);
    relocs->readContentLocal(at++, &byte, 1);
    unsigned n = byte;
    p_src_type = (SgAsmNERelocEntry::NERelocSrcType)(n & 0x0f);
    p_modifier = (SgAsmNERelocEntry::NERelocModifiers)(n>>8);

    /* The target type (3 bits), additive flag (1 bit), and target flags */
    relocs->extend(1);
    relocs->readContentLocal(at++, &byte, 1);
    n = byte;
    p_tgt_type = (SgAsmNERelocEntry::NERelocTgtType)(n & 0x03);
    p_flags = (SgAsmNERelocEntry::NERelocFlags)(n>>2);
    
    /* src_offset is the byte offset into the source section that needs to be patched. If this is an additive relocation then
     * the source will be patched by adding the target value to the value stored at the source. Otherwise the target value is
     * written to the source and the old contents of the source contains the next source offset, until we get 0xffff. */
    relocs->extend(2);
    relocs->readContentLocal(at, &u16_disk, 2);
    p_src_offset = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
    at += 2;

    switch (p_tgt_type) {
      case RF_TGTTYPE_IREF:
        /* Internal reference */
        relocs->extend(4);
        relocs->readContentLocal(at++, &byte, 1);
        p_iref.sect_idx = byte;
        relocs->readContentLocal(at++, &byte, 1);
        p_iref.res1  = byte;
        relocs->readContentLocal(at, &u16_disk, 2);
        p_iref.tgt_offset = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
        at += 2;
        break;
      case RF_TGTTYPE_IORD:
        /* Imported ordinal */
        relocs->extend(4);
        relocs->readContentLocal(at, &u16_disk, 2);
        p_iord.modref  = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
        relocs->readContentLocal(at+2, &u16_disk, 2);
        p_iord.ordinal = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
        at += 4;
        if (p_flags & RF_2EXTRA) {
            if (p_flags & RF_32ADD) {
                relocs->extend(4);
                relocs->readContentLocal(at+8, &u32_disk, 4);
                p_iord.addend = Rose::BinaryAnalysis::ByteOrder::leToHost(u32_disk);
                at += 4;
            } else {
                relocs->extend(2);
                relocs->readContentLocal(at+8, &u16_disk, 2);
                p_iord.addend = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
                at += 2;
            }
        } else {
            p_iord.addend = 0;
        }
        break;
      case RF_TGTTYPE_INAME:
        /* Imported name */
        relocs->extend(4);
        relocs->readContentLocal(at, &u16_disk, 2);
        p_iname.modref = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
        relocs->readContentLocal(at+2, &u16_disk, 2);
        p_iname.nm_off = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
        at += 4;
        if (p_flags & RF_2EXTRA) {
            if (p_flags & RF_32ADD) {
                relocs->extend(4);
                relocs->readContentLocal(at+8, &u32_disk, 4);
                p_iname.addend = Rose::BinaryAnalysis::ByteOrder::leToHost(u32_disk);
                at += 4;
            } else {
                relocs->extend(2);
                relocs->readContentLocal(at+8, &u16_disk, 2);
                p_iname.addend = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
                at += 2;
            }
        } else {
            p_iname.addend = 0;
        }
        break;
      case RF_TGTTYPE_OSFIXUP:
        /* Operating system fixup */
        relocs->extend(4);
        relocs->readContentLocal(at, &u16_disk, 2);
        p_osfixup.type = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
        relocs->readContentLocal(at+2, &u16_disk, 2);
        p_osfixup.res3 = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
        at += 4;
        break;
    }

    if (rec_size)
        *rec_size = at - orig_at;
}

/* Write entry back to disk at the specified section and section offset, returning new offset */
Rose::BinaryAnalysis::Address
SgAsmNERelocEntry::unparse(std::ostream &f, const SgAsmGenericSection *section, Rose::BinaryAnalysis::Address spos) const
{
    unsigned char byte;
    byte = (p_modifier << 8) | (p_src_type & 0x0f);
    spos = section->write(f, spos, byte);
    byte = (p_flags << 2) | (p_tgt_type & 0x03);
    spos = section->write(f, spos, byte);
    
    uint16_t word;
    uint32_t dword;
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_src_offset, &word);
    spos = section->write(f, spos, sizeof word, &word);
    
    switch (p_tgt_type) {
      case RF_TGTTYPE_IREF:
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iref.sect_idx, &byte);
        spos = section->write(f, spos, byte);
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iref.res1, &byte);
        spos = section->write(f, spos, byte);
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iref.tgt_offset, &word);
        spos = section->write(f, spos, sizeof word, &word);
        break;
      case RF_TGTTYPE_IORD:
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iord.modref, &word);
        spos = section->write(f, spos, sizeof word, &word);
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iord.ordinal, &word);
        spos = section->write(f, spos, sizeof word, &word);
        if (p_flags & RF_2EXTRA) {
            if (p_flags & RF_32ADD) {
                Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iord.addend, &dword);
                spos = section->write(f, spos, sizeof dword, &dword);
            } else {
                Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iord.addend, &word);
                spos = section->write(f, spos, sizeof word, &word);
            }
        } else {
            ROSE_ASSERT(p_iord.addend==0);
        }
        break;
      case RF_TGTTYPE_INAME:
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iname.modref, &word);
        spos = section->write(f, spos, sizeof word, &word);
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iname.nm_off, &word);
        spos = section->write(f, spos, sizeof word, &word);
        if (p_flags & RF_2EXTRA) {
            if (p_flags & RF_32ADD) {
                Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iname.addend, &dword);
                spos = section->write(f, spos, sizeof dword, &dword);
            } else {
                Rose::BinaryAnalysis::ByteOrder::hostToLe(p_iname.addend, &word);
                spos = section->write(f, spos, sizeof word, &word);
            }
        } else {
            ROSE_ASSERT(p_iname.addend==0);
        }
        break;
      case RF_TGTTYPE_OSFIXUP:
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_osfixup.type, &word);
        spos = section->write(f, spos, sizeof word, &word);
        Rose::BinaryAnalysis::ByteOrder::hostToLe(p_osfixup.res3, &word);
        spos = section->write(f, spos, sizeof word, &word);
        break;
      default:
        ROSE_ASSERT(!"unknown relocation target type");
    }
    return spos;
}
    
/* Print some debugging info */
void
SgAsmNERelocEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sRelocEntry[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sRelocEntry.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    const char *s;
    switch (p_src_type) {
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
    fprintf(f, "%s%-*s = %u (%s)\n", p, w, "src_type", p_src_type, s);

    switch (p_modifier) {
      case RF_MODIFIER_SINGLE:  s = "single";          break;
      case RF_MODIFIER_MULTI:   s = "multiple";        break;
      default:                  s = "unknown";         break;
    }
    fprintf(f, "%s%-*s = 0x%04u (%s)\n", p, w, "modifier", p_modifier, s);
    
    switch (p_tgt_type) {
      case RF_TGTTYPE_IREF:    s = "internal reference"; break;
      case RF_TGTTYPE_IORD:    s = "imported ordinal";   break;
      case RF_TGTTYPE_INAME:   s = "imported name";      break;
      case RF_TGTTYPE_OSFIXUP: s = "OS fixup";           break;
      default:                 s = "unknown";            break;
    }
    fprintf(f, "%s%-*s = %u (%s)\n",       p, w, "tgt_type",   p_tgt_type, s);

    fprintf(f, "%s%-*s = 0x%04x",          p, w, "flags", p_flags);
    if (p_flags & RF_ADDITIVE)  fputs(" additive",  f);
    if (p_flags & RF_2EXTRA)    fputs(" 2-extra",   f);
    if (p_flags & RF_32ADD)     fputs(" 32-add",    f);
    if (p_flags & RF_16SECTION) fputs(" 16-sect",   f);
    if (p_flags & RF_8ORDINAL)  fputs(" 8-ordinal", f);
    fputc('\n', f);

    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "src_offset", p_src_offset);

    switch (p_tgt_type) {
      case RF_TGTTYPE_IREF:
        fprintf(f, "%s%-*s = %u\n",            p, w, "sect_idx",   p_iref.sect_idx);
        fprintf(f, "%s%-*s = 0x%02x\n",        p, w, "res3",       p_iref.res1);
        fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, "tgt_offset", p_iref.tgt_offset);
        break;
      case RF_TGTTYPE_IORD:
        fprintf(f, "%s%-*s = %u\n",            p, w, "modref",     p_iord.modref);
        fprintf(f, "%s%-*s = %u\n",            p, w, "ordinal",    p_iord.ordinal);
        fprintf(f, "%s%-*s = %" PRIu64 "\n",   p, w, "addend",     p_iord.addend);
        break;
      case RF_TGTTYPE_INAME:
        fprintf(f, "%s%-*s = %u\n",            p, w, "modref",     p_iname.modref);
        fprintf(f, "%s%-*s = %u\n",            p, w, "nm_off",     p_iname.nm_off);
        fprintf(f, "%s%-*s = %" PRIu64 "\n",   p, w, "addend",     p_iname.addend);
        break;
      case RF_TGTTYPE_OSFIXUP:
        fprintf(f, "%s%-*s = %u\n",            p, w, "type",       p_osfixup.type);
        fprintf(f, "%s%-*s = 0x%04x\n",        p, w, "res3",       p_osfixup.res3);
        break;
      default:
        ROSE_ASSERT(!"unknown relocation target type");
    }
}
    
/* Constructor. We don't know how large the relocation table is until we're parsing it (specifically, after we've read the
 * number of entries stored in the first two bytes), therefore the section should have an initial size of zero and we extend
 * it as we parse it. */
SgAsmNERelocTable::SgAsmNERelocTable(SgAsmNEFileHeader *fhdr, SgAsmNESection *section)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    ROSE_ASSERT(section!=NULL);
    set_offset(section->get_offset() + section->get_size()); /*reloc section begins immediately after section payload*/
    set_size(0);
    grabContent();

    char name[64];
    snprintf(name, sizeof(name), "NE Relocation Table %" PRIu64, p_offset);
    set_synthesized(true);
    set_name(new SgAsmBasicString(name));
    set_purpose(SP_HEADER);

    ROSE_ASSERT(0 == get_size());

    Rose::BinaryAnalysis::Address at = 0, reloc_size = 0;

    extend(2);
    uint16_t u16_disk;
    readContentLocal(at, &u16_disk, 2);
    size_t nrelocs = Rose::BinaryAnalysis::ByteOrder::leToHost(u16_disk);
    at += 2;
    
    for (size_t i = 0; i < nrelocs; i++, at += reloc_size) {
        p_entries.push_back(new SgAsmNERelocEntry(this, at, &reloc_size));
    }
}

/* Write relocation table back to disk */
void
SgAsmNERelocTable::unparse(std::ostream &f) const
{
    Rose::BinaryAnalysis::Address spos=0; /*section offset*/
    uint16_t size_le;
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_entries.size(), &size_le);
    spos = write(f, spos, sizeof size_le, &size_le);
    
    for (size_t i = 0; i < p_entries.size(); i++) {
        spos = p_entries[i]->unparse(f, this, spos);
    }
}
    
/* Print some debugging info */
void
SgAsmNERelocTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sNERelocTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sNERelocTable.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %" PRIuPTR " entries\n", p, w, "size", p_entries.size());
    for (size_t i = 0; i < p_entries.size(); i++) {
        p_entries[i]->dump(f, p, i);
    }
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Parses the structure of an NE file and adds the information to the SgAsmGenericFile. */
SgAsmNEFileHeader *
SgAsmNEFileHeader::parse(SgAsmDOSFileHeader *dos_header)
{
    ROSE_ASSERT(dos_header);
    SgAsmGenericFile *ef = dos_header->get_file();
    ROSE_ASSERT(ef);

    /* NE files extend the DOS header with some additional info */
    SgAsmDOSExtendedHeader *dos2_header = new SgAsmDOSExtendedHeader(dos_header);
    dos2_header->set_offset(dos_header->get_size());
    dos2_header->parse();
    
    /* The NE header */
    SgAsmNEFileHeader *ne_header = new SgAsmNEFileHeader(ef, dos2_header->get_e_lfanew());
    ne_header->set_dos2Header(dos2_header);

    /* Sections defined by the NE file header */
    if (ne_header->get_e_resnametab_rfo() > 0) {
        Rose::BinaryAnalysis::Address resnames_offset = ne_header->get_offset() + ne_header->get_e_resnametab_rfo();
        SgAsmNENameTable *resnames = new SgAsmNENameTable(ne_header, resnames_offset);
        resnames->set_name(new SgAsmBasicString("NE Resident Name Table"));
        ne_header->set_residentNameTable(resnames);
    }
    if (ne_header->get_e_modreftab_rfo() > 0 && ne_header->get_e_importnametab_rfo() > ne_header->get_e_modreftab_rfo()) {
        /* Imported Name Table must be read before the Module Reference Table since the latter references the former. However,
         * the Imported Name Table comes immediately after the Module Reference Table and before the Entry Table in the file. */
        ROSE_ASSERT(ne_header->get_e_importnametab_rfo() > 0);
        ROSE_ASSERT(ne_header->get_e_entrytab_rfo() > ne_header->get_e_importnametab_rfo());
        Rose::BinaryAnalysis::Address strtab_offset = ne_header->get_offset() + ne_header->get_e_importnametab_rfo();
        Rose::BinaryAnalysis::Address strtab_size   = ne_header->get_e_entrytab_rfo() - ne_header->get_e_importnametab_rfo();
        SgAsmNEStringTable *strtab = new SgAsmNEStringTable(ne_header, strtab_offset, strtab_size);

        /* Module reference table */
        Rose::BinaryAnalysis::Address modref_offset = ne_header->get_offset() + ne_header->get_e_modreftab_rfo();
        Rose::BinaryAnalysis::Address modref_size   = ne_header->get_e_importnametab_rfo() - ne_header->get_e_modreftab_rfo();
        SgAsmNEModuleTable *modtab = new SgAsmNEModuleTable(ne_header, strtab, modref_offset, modref_size);
        ne_header->set_moduleTable(modtab);
    }
    if (ne_header->get_e_entrytab_rfo() > 0 && ne_header->get_e_entrytab_size() > 0) {
        Rose::BinaryAnalysis::Address enttab_offset = ne_header->get_offset() + ne_header->get_e_entrytab_rfo();
        Rose::BinaryAnalysis::Address enttab_size = ne_header->get_e_entrytab_size();
        SgAsmNEEntryTable *enttab = new SgAsmNEEntryTable(ne_header, enttab_offset, enttab_size);
        ne_header->set_entryTable(enttab);
    }
    if (ne_header->get_e_nonresnametab_offset() > 0) {
        SgAsmNENameTable *nonres = new SgAsmNENameTable(ne_header, ne_header->get_e_nonresnametab_offset());
        nonres->set_name(new SgAsmBasicString("NE Non-Resident Name Table"));
        ne_header->set_nonresidentNameTable(nonres);
    }

    /* Construct the section table and its sections (non-synthesized sections) */
    ne_header->set_sectionTable(new SgAsmNESectionTable(ne_header));

    // DQ (11/8/2008): Note that "enttab" appears twice as a variable name in this function (in different nested scopes)
    /* NE files have multiple entry points that are defined in the Entry Table */
    if (SgAsmNEEntryTable *enttab = ne_header->get_entryTable())
        enttab->populate_entries();
    
    return ne_header;
}

unsigned
SgAsmNEEntryPoint::get_section_idx() const {
    return get_sectionIndex();
}

void
SgAsmNEEntryPoint::set_section_idx(unsigned x) {
    set_sectionIndex(x);
}

unsigned
SgAsmNEEntryPoint::get_section_offset() const {
    return get_sectionOffset();
}

void
SgAsmNEEntryPoint::set_section_offset(unsigned x) {
    set_sectionOffset(x);
}

SgAsmDOSExtendedHeader*
SgAsmNEFileHeader::get_dos2_header() const {
    return get_dos2Header();
}

void
SgAsmNEFileHeader::set_dos2_header(SgAsmDOSExtendedHeader *x) {
    set_dos2Header(x);
}

SgAsmNESectionTable*
SgAsmNEFileHeader::get_section_table() const {
    return get_sectionTable();
}

void
SgAsmNEFileHeader::set_section_table(SgAsmNESectionTable *x) {
    set_sectionTable(x);
}

SgAsmNENameTable*
SgAsmNEFileHeader::get_resname_table() const {
    return get_residentNameTable();
}

void
SgAsmNEFileHeader::set_resname_table(SgAsmNENameTable *x) {
    set_residentNameTable(x);
}

SgAsmNENameTable*
SgAsmNEFileHeader::get_nonresname_table() const {
    return get_nonresidentNameTable();
}

void
SgAsmNEFileHeader::set_nonresname_table(SgAsmNENameTable *x) {
    set_nonresidentNameTable(x);
}

SgAsmNEModuleTable*
SgAsmNEFileHeader::get_module_table() const {
    return get_moduleTable();
}

void
SgAsmNEFileHeader::set_module_table(SgAsmNEModuleTable *x) {
    set_moduleTable(x);
}

SgAsmNEEntryTable*
SgAsmNEFileHeader::get_entry_table() const {
    return get_entryTable();
}

void
SgAsmNEFileHeader::set_entry_table(SgAsmNEEntryTable *x) {
    set_entryTable(x);
}

const char*
SgAsmNEFileHeader::format_name() const {
    return formatName();
}

const char*
SgAsmNEFileHeader::formatName() const {
    return "NE";
}

const SgAddressList&
SgAsmNEModuleTable::get_name_offsets() const {
    return get_nameOffsets();
}

void
SgAsmNEModuleTable::set_name_offsets(const SgAddressList &x) {
    set_nameOffsets(x);
}

SgAsmNESectionTableEntry*
SgAsmNESection::get_st_entry() const {
    return get_sectionTableEntry();
}

void
SgAsmNESection::set_st_entry(SgAsmNESectionTableEntry *x) {
    set_sectionTableEntry(x);
}

SgAsmNERelocTable*
SgAsmNESection::get_reloc_table() const {
    return get_relocationTable();
}

void
SgAsmNESection::set_reloc_table(SgAsmNERelocTable *x) {
    set_relocationTable(x);
}

Rose::BinaryAnalysis::Address
SgAsmNESectionTableEntry::get_physical_size() const {
    return get_physicalSize();
}

void
SgAsmNESectionTableEntry::set_physical_size(Rose::BinaryAnalysis::Address x) {
    set_physicalSize(x);
}

Rose::BinaryAnalysis::Address
SgAsmNESectionTableEntry::get_virtual_size() const {
    return get_virtualSize();
}

void
SgAsmNESectionTableEntry::set_virtual_size(Rose::BinaryAnalysis::Address x) {
    set_virtualSize(x);
}

Rose::BinaryAnalysis::Address
SgAsmNESectionTable::get_physical_size() const {
    return get_physicalSize();
}

void
SgAsmNESectionTable::set_physical_size(Rose::BinaryAnalysis::Address x) {
    set_physicalSize(x);
}

Rose::BinaryAnalysis::Address
SgAsmNESectionTable::get_virtual_size() const {
    return get_virtualSize();
}

void
SgAsmNESectionTable::set_virtual_size(Rose::BinaryAnalysis::Address x) {
    set_virtualSize(x);
}

#endif
