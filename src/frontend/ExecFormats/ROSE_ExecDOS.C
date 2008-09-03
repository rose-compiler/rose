/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// namespace Exec {
// namespace DOS {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MS-DOS Real Mode File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SgAsmDOSFileHeader::ctor(SgAsmGenericFile *f, addr_t offset)
{
    const DOSFileHeader_disk *disk = (const DOSFileHeader_disk*)content(0, sizeof(DOSFileHeader_disk));

    set_name("DOS File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

 // DQ (8/16/2008): Added code to set SgAsmPEFileHeader as parent of input SgAsmGenericFile
    f->set_parent(this);

 // DQ: Some old compilers were little-endian ignorant and stored "ZM", but we will ignore this.
 /* Check magic number early */
    if (disk->e_magic[0]!='M' || disk->e_magic[1]!='Z')
        throw FormatError("Bad DOS magic number");

    /* Decode file format */
    p_e_last_page_size    = le_to_host(disk->e_last_page_size);
    p_e_total_pages       = le_to_host(disk->e_total_pages);
    p_e_nrelocs           = le_to_host(disk->e_nrelocs);
    p_e_header_paragraphs = le_to_host(disk->e_header_paragraphs);
    p_e_minalloc          = le_to_host(disk->e_minalloc);
    p_e_maxalloc          = le_to_host(disk->e_maxalloc);
    p_e_ss                = le_to_host(disk->e_ss);
    p_e_sp                = le_to_host(disk->e_sp);
    p_e_cksum             = le_to_host(disk->e_cksum);
    p_e_ip                = le_to_host(disk->e_ip);
    p_e_cs                = le_to_host(disk->e_cs);
    p_e_relocs_offset     = le_to_host(disk->e_relocs_offset);
    p_e_overlay           = le_to_host(disk->e_overlay);
    for (size_t i=0; i<NELMTS(disk->e_res1); i++)
        p_e_res1.push_back(le_to_host(disk->e_res1[i]));

    /* Magic number */
    p_magic.push_back(disk->e_magic[0]);
    p_magic.push_back(disk->e_magic[1]);

    /* File format */
    p_exec_format->set_family(FAMILY_DOS);
    p_exec_format->set_purpose(PURPOSE_EXECUTABLE);
    p_exec_format->set_sex(ORDER_LSB);
    p_exec_format->set_abi(ABI_MSDOS);
    p_exec_format->set_abi_version(0);
    p_exec_format->set_word_size(2);
    p_exec_format->set_version(0);
    p_exec_format->set_is_current_version(true);

    /* Target architecture */
    set_isa(ISA_IA32_Family);

    /* Entry point */
    p_base_va = 0;
    add_entry_rva(le_to_host(disk->e_ip));
}

/* Encode the DOS file header into disk format */
void *
SgAsmDOSFileHeader::encode(DOSFileHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    host_to_le(p_e_last_page_size,     &(disk->e_last_page_size));
    host_to_le(p_e_total_pages,        &(disk->e_total_pages));
    host_to_le(p_e_nrelocs,            &(disk->e_nrelocs));
    host_to_le(p_e_header_paragraphs,  &(disk->e_header_paragraphs));
    host_to_le(p_e_minalloc,           &(disk->e_minalloc));
    host_to_le(p_e_maxalloc,           &(disk->e_maxalloc));
    host_to_le(p_e_ss,                 &(disk->e_ss));
    host_to_le(p_e_sp,                 &(disk->e_sp));
    host_to_le(p_e_cksum,              &(disk->e_cksum));
    host_to_le(p_e_ip,                 &(disk->e_ip));
    host_to_le(p_e_cs,                 &(disk->e_cs));
    host_to_le(p_e_relocs_offset,      &(disk->e_relocs_offset));
    host_to_le(p_e_overlay,            &(disk->e_overlay));
    for (size_t i=0; i<NELMTS(disk->e_res1); i++)
        host_to_le(p_e_res1[i],        &(disk->e_res1[i]));
    return disk;
}

/* Write the DOS file header back to disk */
void
SgAsmDOSFileHeader::unparse(FILE *f)
{
    DOSFileHeader_disk disk;
    encode(&disk);
    write(f, 0, sizeof(disk), &disk);

    if (p_relocs)
        p_relocs->unparse(f);

    if (p_rm_section)
        p_rm_section->unparse(f);
}

/* Adds the real-mode section to the DOS file header. If max_offset is non-zero then use that as the maximum offset of the
 * real-mode section. If the DOS header indicates a zero sized section then return NULL. If the section exists or is zero size due
 * to the max_offset then return the section. */
SgAsmGenericSection *
SgAsmDOSFileHeader::add_rm_section(addr_t max_offset)
{
    ROSE_ASSERT( NULL == p_rm_section );
    
    addr_t rm_offset = p_e_header_paragraphs * 16;
    addr_t rm_end = p_e_total_pages * 512 - (p_e_total_pages>0 ? 512 - p_e_last_page_size % 512 : 0);
    ROSE_ASSERT(rm_end >= rm_offset);
    addr_t rm_size = rm_end - rm_offset;
    if (rm_size == 0)
        return NULL;

    if (max_offset>0) {
        if (max_offset < rm_offset) {
            rm_size = 0;
        } else if (rm_offset + rm_size > max_offset) {
            rm_size = max_offset - rm_offset;
        }
    }

    try {
        p_rm_section = new SgAsmGenericSection(get_file(), this, rm_offset, rm_size);
    } catch (ShortRead &p_ex) {
        /* If the offset or size is out of bounds for the file then assume that the real-mode section does not exist. This
         * can indicate that the DOS header is being used for something other than a DOS header. See
         * http://www.phreedom.org/solar/code/tinype/ for some examples of overlapping the DOS header with the PE header. */
        return NULL;
    }
    
    p_rm_section->set_name("DOS real-mode text/data");
    p_rm_section->set_synthesized(true);
    p_rm_section->set_purpose(SP_PROGRAM);
    p_rm_section->set_mapped(0, rm_size);
    p_rm_section->set_rperm(true);
    p_rm_section->set_wperm(true);
    p_rm_section->set_eperm(true);
    return p_rm_section;
}
    
/* Print some debugging info */
void
SgAsmDOSFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sDOSFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sDOSFileHeader.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u bytes\n",              p, w, "e_last_page_size",     p_e_last_page_size);
    fprintf(f, "%s%-*s = %u 512-byte pages\n",     p, w, "e_total_pages",        p_e_total_pages);
    fprintf(f, "%s%-*s = %u relocations\n",        p, w, "e_nrelocs",            p_e_nrelocs);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_header_paragraphs",  p_e_header_paragraphs);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_minalloc",           p_e_minalloc);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_maxalloc",           p_e_maxalloc);
    fprintf(f, "%s%-*s = 0x%08u\n",                p, w, "e_ss",                 p_e_ss);
    fprintf(f, "%s%-*s = 0x%08u\n",                p, w, "e_sp",                 p_e_sp);
    fprintf(f, "%s%-*s = %u (zero implies not used)\n",p, w, "e_cksum",          p_e_cksum);
    fprintf(f, "%s%-*s = 0x%08u\n",                p, w, "e_ip",                 p_e_ip);
    fprintf(f, "%s%-*s = 0x%08u\n",                p, w, "e_cs",                 p_e_cs);
    fprintf(f, "%s%-*s = byte %"PRIu64"\n",        p, w, "e_relocs_offset",      p_e_relocs_offset);
    fprintf(f, "%s%-*s = %u\n",                    p, w, "e_overlay",            p_e_overlay);
    for (size_t i=0; i < p_e_res1.size(); i++) {
        fprintf(f, "%s%-*s = [%zd] %u\n",          p, w, "e_res1",               i, p_e_res1[i]);
    }
    if (p_relocs) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "relocs", p_relocs->get_id(), p_relocs->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",        p, w, "relocs");
    }
    if (p_rm_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "rm_section", p_rm_section->get_id(), p_rm_section->get_name().c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",        p, w, "rm_section");
    }

    fprintf (f, "%sSaved raw data (size = %zu) \n",prefix,p_data.size());
    hexdump(f, (addr_t) 0x0, "    ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
/* Returns true if a cursory look at the file indicates that it could be a DOS executable file. */
bool
SgAsmDOSFileHeader::is_DOS(SgAsmGenericFile *f)
{
    SgAsmDOSFileHeader *fhdr    = NULL;
    bool           retval  = false;

    try {
        fhdr = new SgAsmDOSFileHeader(f, 0);
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }

    delete fhdr;
    return retval;
}

/* Parses the structure of a DOS file and adds the information to the ExecFile. */
SgAsmDOSFileHeader *
SgAsmDOSFileHeader::parse(SgAsmGenericFile *ef, bool define_rm_section)
{
    ROSE_ASSERT(ef);
    
    SgAsmDOSFileHeader *fhdr = new SgAsmDOSFileHeader(ef, 0);

    /* The DOS file header is followed by optional relocation entries */
    if (fhdr->p_e_nrelocs > 0) {
        SgAsmGenericSection *relocs = new SgAsmGenericSection(ef, fhdr, fhdr->p_e_relocs_offset,
                                                              fhdr->p_e_nrelocs * sizeof(DOSRelocEntry_disk));
        relocs->set_name("DOS relocation table");
        relocs->set_synthesized(true);
        relocs->set_purpose(SP_HEADER);
        fhdr->set_relocs(relocs);
    }

    /* DOS real-mode text/data/etc. */
    if (define_rm_section == true)
        fhdr->add_rm_section();
    
    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();

    return fhdr;
}

// }; //namespace DOS
// }; //namespace Exec
