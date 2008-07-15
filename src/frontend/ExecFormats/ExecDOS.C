/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace Exec {
namespace DOS {

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
    e_last_page_size    = le_to_host(disk->e_last_page_size);
    e_total_pages       = le_to_host(disk->e_total_pages);
    e_nrelocs           = le_to_host(disk->e_nrelocs);
    e_header_paragraphs = le_to_host(disk->e_header_paragraphs);
    e_minalloc          = le_to_host(disk->e_minalloc);
    e_maxalloc          = le_to_host(disk->e_maxalloc);
    e_ss                = le_to_host(disk->e_ss);
    e_sp                = le_to_host(disk->e_sp);
    e_cksum             = le_to_host(disk->e_cksum);
    e_ip                = le_to_host(disk->e_ip);
    e_cs                = le_to_host(disk->e_cs);
    e_relocs_offset     = le_to_host(disk->e_relocs_offset);
    e_overlay           = le_to_host(disk->e_overlay);
    for (size_t i=0; i<NELMTS(e_res1); i++)
        e_res1[i]       = le_to_host(disk->e_res1[i]);

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
    host_to_le(e_last_page_size,     &(disk->e_last_page_size));
    host_to_le(e_total_pages,        &(disk->e_total_pages));
    host_to_le(e_nrelocs,            &(disk->e_nrelocs));
    host_to_le(e_header_paragraphs,  &(disk->e_header_paragraphs));
    host_to_le(e_minalloc,           &(disk->e_minalloc));
    host_to_le(e_maxalloc,           &(disk->e_maxalloc));
    host_to_le(e_ss,                 &(disk->e_ss));
    host_to_le(e_sp,                 &(disk->e_sp));
    host_to_le(e_cksum,              &(disk->e_cksum));
    host_to_le(e_ip,                 &(disk->e_ip));
    host_to_le(e_cs,                 &(disk->e_cs));
    host_to_le(e_relocs_offset,      &(disk->e_relocs_offset));
    host_to_le(e_overlay,            &(disk->e_overlay));
    for (size_t i=0; i<NELMTS(e_res1); i++)
        host_to_le(e_res1[i],        &(disk->e_res1[i]));
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

    if (relocs)
        relocs->unparse(f);

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
    fprintf(f, "%s%-*s = %u bytes\n",              p, w, "e_last_page_size",     e_last_page_size);
    fprintf(f, "%s%-*s = %u 512-byte pages\n",     p, w, "e_total_pages",        e_total_pages);
    fprintf(f, "%s%-*s = %u relocations\n",        p, w, "e_nrelocs",            e_nrelocs);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_header_paragraphs",  e_header_paragraphs);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_minalloc",           e_minalloc);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_maxalloc",           e_maxalloc);
    fprintf(f, "%s%-*s = 0x%08u\n",                p, w, "e_ss",                 e_ss);
    fprintf(f, "%s%-*s = 0x%08u\n",                p, w, "e_sp",                 e_sp);
    fprintf(f, "%s%-*s = %u\n",                    p, w, "e_cksum",              e_cksum);
    fprintf(f, "%s%-*s = 0x%08u\n",                p, w, "e_ip",                 e_ip);
    fprintf(f, "%s%-*s = 0x%08u\n",                p, w, "e_cs",                 e_cs);
    fprintf(f, "%s%-*s = byte %"PRIu64"\n",        p, w, "e_relocs_offset",      e_relocs_offset);
    fprintf(f, "%s%-*s = %u\n",                    p, w, "e_overlay",            e_overlay);
    for (size_t i=0; i<NELMTS(e_res1); i++) {
        fprintf(f, "%s%-*s = [%zd] %u\n",          p, w, "e_res1",               i, e_res1[i]);
    }
    if (relocs) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "relocs", relocs->get_id(), relocs->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",        p, w, "relocs");
    }
    if (rm_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "rm_section", rm_section->get_id(), rm_section->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",        p, w, "rm_section");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
/* Returns true if a cursory look at the file indicates that it could be a DOS executable file. */
bool
is_DOS(ExecFile *f)
{
    DOSFileHeader *fhdr    = NULL;
    bool           retval  = false;

    try {
        fhdr = new DOSFileHeader(f, 0);
        if (fhdr->get_magic().size()<2 || fhdr->get_magic()[0]!='M' || fhdr->get_magic()[1]!='Z') goto done;
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }
done:
    delete fhdr;
    return retval;
}

/* Parses the structure of a DOS file and adds the information to the ExecFile. */
DOSFileHeader *
parse(ExecFile *ef)
{
    ROSE_ASSERT(ef);
    
    DOSFileHeader *fhdr = new DOSFileHeader(ef, 0);

    /* The DOS file header is followed by optional relocation entries */
    if (fhdr->e_nrelocs > 0) {
        ExecSection *relocs = new ExecSection(ef, fhdr->e_relocs_offset, fhdr->e_nrelocs*sizeof(DOSRelocEntry_disk));
        relocs->set_name("DOS relocation table");
        relocs->set_synthesized(true);
        relocs->set_purpose(SP_HEADER);
        relocs->set_header(fhdr);
        fhdr->set_relocs(relocs);
    }

    /* DOS real-mode text/data/etc. */
    addr_t rm_offset = fhdr->e_header_paragraphs * 16;
    addr_t rm_end = fhdr->e_total_pages * 512 - (512 - fhdr->e_last_page_size % 512);
    ROSE_ASSERT(rm_end > rm_offset);
    addr_t rm_size = rm_end - rm_offset;
    ExecSection *rm_section = new ExecSection(ef, rm_offset, rm_size);
    rm_section->set_name("DOS real-mode text/data");
    rm_section->set_synthesized(true);
    rm_section->set_purpose(SP_PROGRAM);
    rm_section->set_header(fhdr);
    rm_section->set_executable(true);
    fhdr->set_rm_section(rm_section);

    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();

    return fhdr;
}

}; //namespace DOS
}; //namespace Exec
