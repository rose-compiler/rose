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
    fileFormat.family      = FAMILY_DOS;
    fileFormat.purpose     = PURPOSE_EXECUTABLE;
    fileFormat.sex         = ORDER_LSB;
    fileFormat.abi         = ABI_MSDOS;
    fileFormat.abi_version = 0;
    fileFormat.word_size   = 2;
    fileFormat.version     = 0;
    fileFormat.is_current_version = true;

    /* Target architecture */
    target.set_isa(ISA_IA32_386);

    /* Entry point */
    base_va = 0;
    entry_rva = le_to_host(disk->e_ip);
}

/* Print some debugging info */
void
DOSFileHeader::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sDOSFileHeader.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecHeader::dump(f, p);
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
    e_reserved1          = le_to_host(disk->e_reserved1);
    e_reserved2          = le_to_host(disk->e_reserved2);
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
    fileFormat.family      = FAMILY_PE;
    fileFormat.purpose     = e_flags & HF_PROGRAM ? PURPOSE_EXECUTABLE : PURPOSE_LIBRARY;
    fileFormat.sex         = ORDER_LSB;
    fileFormat.abi         = ABI_NT;
    fileFormat.abi_version = 0;
    fileFormat.word_size   = 4;
    ROSE_ASSERT(e_lmajor<=0xffff && e_lminor<=0xffff);
    fileFormat.version     = (e_lmajor<<16) | e_lminor;
    fileFormat.is_current_version = true; /*FIXME*/

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

/* Adds the RVA/Size pairs to the end of the PE file header */
void
PEFileHeader::add_rvasize_pairs()
{
    const RVASizePair_disk *pairs_disk = (const RVASizePair_disk*)extend(e_num_rvasize_pairs * sizeof(RVASizePair_disk));
    for (size_t i=0; i<e_num_rvasize_pairs; i++) {
        rvasize_pairs.push_back(RVASizePair(pairs_disk+i));
    }
}

/* Print some debugging information */
void
PEFileHeader::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sPEFileHeader.", prefix);
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecHeader::dump(f, p);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_cpu_type",          e_cpu_type);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_nobjects",          e_nobjects);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_time",              e_time);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved1",         e_reserved1);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved2",         e_reserved2);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_nt_hdr_size",       e_nt_hdr_size);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_flags",             e_flags);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved3",         e_reserved3);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_lmajor",            e_lmajor);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_lminor",            e_lminor);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved4",         e_reserved4);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved5",         e_reserved5);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved6",         e_reserved6);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_entrypoint_rva",    e_entrypoint_rva);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved7",         e_reserved7);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved8",         e_reserved8);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_image_base",        e_image_base);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_object_align",      e_object_align);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_file_align",        e_file_align);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_os_major",          e_os_major);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_os_minor",          e_os_minor);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_user_major",        e_user_major);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_user_minor",        e_user_minor);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_subsys_major",      e_subsys_major);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_subsys_minor",      e_subsys_minor);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved9",         e_reserved9);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_image_size",        e_image_size);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_header_size",       e_header_size);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_file_checksum",     e_file_checksum);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_subsystem",         e_subsystem);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_dll_flags",         e_dll_flags);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_stack_reserve_size",e_stack_reserve_size);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_stack_commit_size", e_stack_commit_size);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_heap_reserve_size", e_heap_reserve_size);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_heap_commit_size",  e_heap_commit_size);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_reserved10",        e_reserved10);
    fprintf(f, "%s%-*s = %u\n", p, w, "e_num_rvasize_pairs", e_num_rvasize_pairs);
    for (unsigned i=0; i<e_num_rvasize_pairs; i++) {
        sprintf(p, "%sPEFileHeader.pair[%d].", prefix, i);
        w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));        
        fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",  p, w, "e_rva",  rvasize_pairs[i].e_rva);
        fprintf(f, "%s%-*s = %" PRIu64 " bytes\n", p, w, "e_size", rvasize_pairs[i].e_size);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Object Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
ObjectTableEntry::ctor(const ObjectTableEntry_disk *disk)
{
    char name[9];
    strncpy(name, disk->name, 8);
    name[9] = '\0';
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

/* Prints some debugging info */
void
ObjectTableEntry::dump(FILE *f, const char *prefix)
{
    const char *p = prefix; /* Don't append to prefix since caller (ObjectTable::dump) is adding array indices. */
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
    
/* Constructor */
void
ObjectTable::ctor(PEFileHeader *fhdr)
{
    set_synthesized(true);
    set_name("PE Object Table");
    set_purpose(SP_HEADER);
    
    const size_t entsize = sizeof(ObjectTableEntry_disk);
    for (size_t i=0; i<fhdr->e_nobjects; i++) {
        /* Parse the object table entry */
        const ObjectTableEntry_disk *disk = (const ObjectTableEntry_disk*)content(i*entsize, entsize);
        ObjectTableEntry *entry = new ObjectTableEntry(disk);
        entries.push_back(entry);
        
        /* Use the entry to define a segment in a synthesized section */
        ExecSection *section = new ExecSection(fhdr->get_file(), entry->physical_offset, entry->physical_size);
        section->set_synthesized(true);
        section->set_name(entry->name);
        section->set_id(i);
        section->set_purpose(SP_PROGRAM);

        ExecSegment *segment = NULL;
        if (0==entry->name.compare(".idata")) {
            segment = new ImportSegment(section, 0, entry->physical_size, entry->rva, entry->virtual_size);
        } else {
            segment = new ExecSegment(section, 0, entry->physical_size, entry->rva, entry->virtual_size);
        }

        segment->set_name(entry->name);
        if (entry->flags & (OF_CODE|OF_IDATA|OF_UDATA))
            section->set_purpose(SP_PROGRAM);
        if (entry->flags & OF_EXECUTABLE)
            segment->set_executable(true);
        if (entry->flags & OF_WRITABLE)
            segment->set_writable(true);
    }
}

/* Prints some debugging info */
void
ObjectTable::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sObjectTable.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p);
    fprintf(f, "%s%-*s = %zu entries\n", p, w, "size", entries.size());
    for (size_t i=0; i<entries.size(); i++) {
        sprintf(p, "%sObjectTable.entries[%zu].", prefix, i);
        entries[i]->dump(f, p);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Import Directory (".idata" segment)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
ImportDirectory::ctor(const ImportDirectory_disk *disk)
{
    hintnames_rva   = le_to_host(disk->hintnames_rva);
    time            = le_to_host(disk->time);
    forwarder_chain = le_to_host(disk->forwarder_chain);
    dll_name_rva    = le_to_host(disk->dll_name_rva);
    bindings_rva    = le_to_host(disk->bindings_rva);
}

/* Print debugging info */
void
ImportDirectory::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sImportDirectory.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "hintnames_rva",   hintnames_rva);
    fprintf(f, "%s%-*s = %lu %s",          p, w, "time",            (unsigned long)time, ctime(&time));
    fprintf(f, "%s%-*s = %u\n",            p, w, "forwarder_chain", forwarder_chain);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "dll_name_rva",    dll_name_rva);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n", p, w, "bindings_rva",    bindings_rva);
}

/* Constructor */
void
ImportSegment::ctor(ExecSection *section, addr_t offset, addr_t size, addr_t rva, addr_t mapped_size)
{
    size_t entry_size = sizeof(ImportDirectory_disk);
    ImportDirectory_disk zero;
    memset(&zero, 0, sizeof zero);

    /* Read idata directory entries--one per DLL*/
    for (size_t i=0; 1; i++) {
        /* End of list is marked by an entry of all zero. */
        const ImportDirectory_disk *idir_disk = (const ImportDirectory_disk*)section->content(i*entry_size, entry_size);
        if (!memcmp(&zero, idir_disk, sizeof zero)) break;
        ImportDirectory *idir = new ImportDirectory(idir_disk);
        dirs.push_back(idir);

        /* The library's name is indicated by RVA. We need a section offset instead. */
        ROSE_ASSERT(idir->dll_name_rva >= get_mapped_rva());
        addr_t dll_name_offset = get_offset() + idir->dll_name_rva - get_mapped_rva();
        std::string dll_name = section->content_str(dll_name_offset);
        fprintf(stderr, "ROBB: dll = \"%s\"\n", dll_name.c_str());

        /* The thunks is an array of RVAs that point to hint/name pairs for the entities imported from the DLL that we're
         * currently processing. Each hint is a two-byte little-endian value. Each name is NUL-terminated. The array address
         * and hint/name pair addresses are all specified with RVAs, but we need to convert them to section offsets in order
         * to read them since we've not mapped sections to their preferred base addresses. */
        if (idir->hintnames_rva < get_mapped_rva())
            throw FormatError("hint/name RVA is before beginning of \".idata\" segment");
        if (idir->bindings_rva < get_mapped_rva())
            throw FormatError("bindings RVA is before beginning of \".idata\" segment");
        addr_t hintnames_offset = get_offset() + idir->hintnames_rva - get_mapped_rva();
        addr_t bindings_offset  = get_offset() + idir->bindings_rva  - get_mapped_rva();
        while (1) {
            addr_t hint_rva = le_to_host(*(const uint32_t*)section->content(hintnames_offset, sizeof(uint32_t)));
            hintnames_offset += sizeof(uint32_t);
            if (0==hint_rva) break; /*list of RVAs is null terminated */
            addr_t hint_offset = get_offset() + hint_rva - get_mapped_rva();
            unsigned hint = le_to_host(*(const uint16_t*)section->content(hint_offset, sizeof(uint16_t)));
            const char *name = section->content_str(hint_offset+sizeof(uint16_t));
            addr_t binding = le_to_host(*(const uint32_t*)section->content(bindings_offset, sizeof(uint32_t)));
            /*FIXME: what to do with "hint", "name", and "binding"? */
            fprintf(stderr, "ROBB: DLL hint=%u name=\"%s\" binding=0x%08"PRIx64"\n", hint, name, binding);
        }
    }
}

/* Print debugging info */
void
ImportSegment::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sImportSegment.", prefix);
    
    ExecSegment::dump(f, p);
    for (size_t i=0; i<dirs.size(); i++)
        dirs[i]->dump(f, p);
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
        if (dos_hdr->magic.size()<2 || dos_hdr->magic[0]!='M' || dos_hdr->magic[1]!='Z') goto done;
        pe_hdr = new PEFileHeader(f, dos_hdr->e_lfanew);
        if (pe_hdr->magic.size()<4 ||
            pe_hdr->magic[0]!=0x50 || pe_hdr->magic[1]!=0x45 || pe_hdr->magic[2]!=0x00 || pe_hdr->magic[3]!=0x00) goto done;
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }
done:
    delete dos_hdr;
    delete pe_hdr;
    return retval;
}
        
/* Parses the structure of a PE file and adds the information to the ExecFile. */
void
parse(ExecFile *f)
{
    ROSE_ASSERT(f);
    
    /* The MS-DOS real-mode "MZ" file header, which is always the first 64 bytes of the file */
    ROSE_ASSERT(sizeof(DOSFileHeader_disk)==64);
    DOSFileHeader *dos_header = new DOSFileHeader(f, 0);

    /* The MS-DOS real-mode stub program sits between the DOS file header and the PE file header */
    /* FIXME: this should be an executable segment. What is the entry address? */
    size_t dos_stub_offset = dos_header->end_offset();
    ROSE_ASSERT(dos_header->e_lfanew > dos_stub_offset);
    size_t dos_stub_size = dos_header->e_lfanew - dos_stub_offset;
    if (dos_stub_size>0) {
        ExecSection *dos_stub = new ExecSection(f, dos_stub_offset, dos_stub_size);
        dos_stub->set_name("DOS real-mode stub");
        dos_stub->set_synthesized(true);
        dos_stub->set_purpose(SP_PROGRAM);
    }
    
    /* The PE header has a fixed-size component followed by some number of RVA/Size pairs */
    PEFileHeader *pe_header = new PEFileHeader(f, dos_header->e_lfanew);
    ROSE_ASSERT(pe_header->e_num_rvasize_pairs < 1000); /* just a sanity check before we allocate memory */
    pe_header->add_rvasize_pairs();

    new ObjectTable(pe_header); /* c'tor adds the new segment to the file */
    f->find_holes();            /* Identify parts of the file that we haven't encountered during parsing */
}
    
}; //namespace PE
}; //namespace Exec
