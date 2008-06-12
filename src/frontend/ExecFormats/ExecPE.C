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
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_cpu_type",          e_cpu_type);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_nobjects",          e_nobjects);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_time",              e_time);
    fprintf(f, "%s%-*s = %"PRIu64"\n", p, w, "e_coff_symtab",       e_coff_symtab);
    fprintf(f, "%s%-*s = %u\n",        p, w, "e_coff_nsyms",        e_coff_nsyms);
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
        section->set_id(i+1); /*numbered starting at 1, not zero*/
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
// COFF Symbol Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
COFFSymbol::ctor(ExecSection *strtab, const COFFSymbol_disk *disk)
{
    if (disk->zero==0) {
        name_offset = le_to_host(disk->offset);
        if (name_offset<4) throw FormatError("name collides with size field");
        name = strtab->content_str(disk->offset);
    } else {
        char temp[9];
        memcpy(temp, disk->name, 8);
        temp[8] = '\0';
        name = temp;
    }
    value           = le_to_host(disk->value);
    section_num     = le_to_host(disk->section_num);
    type            = le_to_host(disk->type);
    storage_class   = le_to_host(disk->storage_class);
    num_aux_entries = le_to_host(disk->num_aux_entries);
}

/* Print some debugging info */
void
COFFSymbol::dump(FILE *f, const char *prefix, ExecFile *ef)
{
    char p[4096], ss[128], tt[128];
    const char *s=NULL, *t=NULL;
    ExecSection *section=NULL;
    sprintf(p, "%sCOFFSymbol.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %"PRIu64" \"%s\"\n", p, w, "name", name_offset, name.c_str());

    if (section_num<=0) {
        switch (section_num) {
          case 0:  s = "external, not assigned";    break;
          case -1: s = "absolute value";            break;
          case -2: s = "general debug, no section"; break;
          default: sprintf(ss, "%d", section_num); s = ss; break;
        }
        fprintf(f, "%s%-*s = %s\n", p, w, "section", s);
    } else if (NULL==ef) {
        fprintf(f, "%s%-*s = [%d] <section info not available here>\n", p, w, "section", section_num);
    } else if (NULL==(section=ef->lookup_section_id(section_num))) {
        fprintf(f, "%s%-*s = [%d] <not a valid section ID>\n", p, w, "section", section_num);
    } else {
        fprintf(f, "%s%-*s = [%d] \"%s\" @%"PRIu64", %"PRIu64" bytes\n", p, w, "section", 
                section_num, section->get_name().c_str(), section->get_offset(), section->get_size());
    }

    switch (type & 0xf0) {
      case 0x00: s = "none";     break;
      case 0x10: s = "pointer";  break;
      case 0x20: s = "function"; break;
      case 0x30: s = "array";    break;
      default:
        sprintf(ss, "%u", type>>8);
        s = ss;
        break;
    }
    switch (type & 0xf) {
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
        sprintf(tt, "%u", type & 0xf);
        t = tt;
        break;
    }
    fprintf(f, "%s%-*s = %s / %s\n",          p, w, "type", s, t);

    switch (storage_class) {
      case 0xff: s = "end of function"; t = "";                                  break;
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
      default:
        sprintf(ss, "%u", storage_class);
        s = ss;
        t = "";  
        break;
    }
    fprintf(f, "%s%-*s = %s\n",               p, w, "storage_class", s);
    fprintf(f, "%s%-*s = 0x%08x %s\n",        p, w, "value", value, t);

    fprintf(f, "%s%-*s = %u\n",               p, w, "num_aux_entries", num_aux_entries);
}

/* Constructor */
void
COFFSymtab::ctor(ExecFile *ef, PEFileHeader *fhdr)
{
    set_synthesized(false);
    set_name("COFF Symbols");
    set_purpose(SP_SYMTAB);

    /* The string table immediately follows the symbols. The first four bytes of the string table are the size of the
     * string table in little endian. */
    addr_t strtab_offset = get_offset() + fhdr->e_coff_nsyms * COFFSymbol_disk_size;
    ExecSection *strtab = new ExecSection(ef, strtab_offset, sizeof(uint32_t));
    strtab->set_synthesized(false);
    strtab->set_name("COFF Symbol Strtab");
    strtab->set_purpose(SP_HEADER);
    addr_t strtab_size = le_to_host(*(const uint32_t*)strtab->content(0, sizeof(uint32_t)));
    if (strtab_size<sizeof(uint32_t))
        throw FormatError("COFF symbol table string table size is less than four bytes");
    strtab->extend(strtab_size-sizeof(uint32_t));

    for (size_t i=0; i<fhdr->e_coff_nsyms; i++) {
        const COFFSymbol_disk *disk = (const COFFSymbol_disk*)content(i*COFFSymbol_disk_size, COFFSymbol_disk_size);
        fprintf(stderr, "ROBB: COFF symbol %zu found at section offset %zu:\n", i, i*COFFSymbol_disk_size);
        COFFSymbol *sym = new COFFSymbol(strtab, disk);
        sym->dump(stderr, "    ", ef);

        /* Parse aux symtab entries */
        if (sym->num_aux_entries>0) {
            if (sym->storage_class==2/*external*/ && sym->type==0x20/*function*/ && sym->section_num>0) {
                fprintf(stderr, "    ROBB: Function definition aux\n");
            } else if (0==sym->name.compare(".bf") || 0==sym->name.compare(".ef")) {
                fprintf(stderr, "    ROBB: Function begin/end aux\n");
            } else if (sym->storage_class==2/*external*/ && sym->section_num<=0 && sym->value==0) {
                fprintf(stderr, "    ROBB: Weak external aux\n");
            } else if (sym->storage_class==103/*file*/ && 0==sym->name.compare(".file")) {
                fprintf(stderr, "    ROBB: File aux\n");
                ROSE_ASSERT(sym->num_aux_entries==1);
                char fname[19];
                strcpy(fname, (const char*)content((i+1)*COFFSymbol_disk_size, COFFSymbol_disk_size));
                fname[18] = '\0';
                fprintf(stderr, "          name = \"%s\"\n", fname);
            } else if (sym->storage_class==3/*static*/ && NULL!=ef->lookup_section_name(sym->name)) {
                fprintf(stderr, "    ROBB: Section definition aux\n");
            } else if (sym->value==0 && sym->type==0x30/*static/null*/ && 1==sym->num_aux_entries) {
                fprintf(stderr, "    ROBB: COMDAT section aux\n");
            } else {
                fprintf(stderr, "    ROBB: unknown aux symbol (skipping)\n");
            }
        }
        
        i += sym->num_aux_entries; /*DEBUGGING: skip aux entries for now*/
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

    /* Construct the segments and their sections */
    new ObjectTable(pe_header);

    /* Parse the COFF symbol table */
    if (pe_header->e_coff_symtab && pe_header->e_coff_nsyms)
        new COFFSymtab(f, pe_header);

    /* Identify parts of the file that we haven't encountered during parsing */
    f->find_holes();
}
    
}; //namespace PE
}; //namespace Exec
