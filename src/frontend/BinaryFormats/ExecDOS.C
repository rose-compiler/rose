/* Copyright 2008 Lawrence Livermore National Security, LLC */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Hexdump.h>
#include <ROSE_NELMTS.h>

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MS-DOS Real Mode File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Construct a new DOS File Header with default values. The new section is placed at file offset zero and the size is
// initially one byte (calling reallocate() or parse() will extend it as necessary).
SgAsmDOSFileHeader::SgAsmDOSFileHeader(SgAsmGenericFile *f)
    : SgAsmGenericHeader(f) {
    initializeProperties();

    ASSERT_not_null(get_file());
    ASSERT_require(get_size() > 0);
    
    set_name(new SgAsmBasicString("DOS File Header"));
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Magic number */
    get_magic().clear();
    get_magic().push_back('M');
    get_magic().push_back('Z');

    /* Executable Format */
    ASSERT_not_null(get_executableFormat());
    get_executableFormat()->set_family(FAMILY_DOS);
    get_executableFormat()->set_purpose(PURPOSE_EXECUTABLE);
    get_executableFormat()->set_sex(Rose::BinaryAnalysis::ByteOrder::ORDER_LSB);
    get_executableFormat()->set_abi(ABI_MSDOS);
    get_executableFormat()->set_abiVersion(0);
    get_executableFormat()->set_wordSize(2);
    get_executableFormat()->set_version(0);
    get_executableFormat()->set_isCurrentVersion(true);

    set_isa(ISA_IA32_Family);
}

bool
SgAsmDOSFileHeader::isDos(SgAsmGenericFile *file)
{
    /* Turn off byte reference tracking for the duration of this function. We don't want our testing the file contents to
     * affect the list of bytes that we've already referenced or which we might reference later. */
    bool was_tracking = file->get_trackingReferences();
    file->set_trackingReferences(false);

    try {
        unsigned char magic[2];
        file->readContent(0, magic, sizeof magic);
        if ('M'!=magic[0] || 'Z'!=magic[1])
            throw 1;
    } catch (...) {
        file->set_trackingReferences(was_tracking);
        return false;
    }
    file->set_trackingReferences(was_tracking);
    return true;
}

SgAsmDOSFileHeader*
SgAsmDOSFileHeader::parse(bool define_rm_section)
{
    SgAsmGenericSection::parse();

    /* Read header from file */
    DOSFileHeader_disk disk;
    if (sizeof(disk)>get_size())
        extend(sizeof(disk)-get_size());
    readContentLocal(0, &disk, sizeof disk);

    /* Check magic number early. 
     * Some old compilers were little-endian ignorant and stored "ZM", but we will ignore this [DQ]. */
    if (disk.e_magic[0]!='M' || disk.e_magic[1]!='Z')
        throw FormatError("Bad DOS magic number");
    
    /* Decode file format */
    ROSE_ASSERT(Rose::BinaryAnalysis::ByteOrder::ORDER_LSB==get_executableFormat()->get_sex());
    p_e_last_page_size    = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_last_page_size);
    p_e_total_pages       = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_total_pages);
    p_e_nrelocs           = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_nrelocs);
    p_e_header_paragraphs = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_header_paragraphs);
    p_e_minalloc          = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_minalloc);
    p_e_maxalloc          = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_maxalloc);
    p_e_ss                = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_ss);
    p_e_sp                = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_sp);
    p_e_cksum             = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_cksum);
    p_e_ip                = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_ip);
    p_e_cs                = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_cs);
    p_e_relocs_offset     = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_relocs_offset);
    p_e_overlay           = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_overlay);
    p_e_res1              = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_res1);

    /* Magic number */
    p_magic.clear();
    p_magic.push_back(disk.e_magic[0]);
    p_magic.push_back(disk.e_magic[1]);

    /* The DOS File Header is followed by optional relocation entries */
    if (p_e_nrelocs>0) {
        SgAsmGenericSection *relocs = new SgAsmGenericSection(get_file(), this);
        relocs->set_offset(p_e_relocs_offset);
        relocs->set_size(p_e_nrelocs * sizeof(DOSRelocEntry_disk));
        relocs->parse();
        relocs->set_name(new SgAsmBasicString("DOS relocation table"));
        relocs->set_synthesized(true);
        relocs->set_purpose(SP_HEADER);
        set_relocs(relocs);
    }
    
    /* DOS real-mode text/data/etc. */
    if (define_rm_section)
        parseRealModeSection();

    /* Entry point */
    p_baseVa = 0;
    addEntryRva(Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_ip));

    return this;
}

void
SgAsmDOSFileHeader::updateFromRealModeSection()
{
    /* Find the DOS Extended Header */
    SgAsmDOSFileHeader *dos1 = this;
    SgAsmDOSExtendedHeader *dos2 = NULL;
    const SgAsmGenericSectionPtrList &sections = dos1->get_sections()->get_sections();
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); !dos2 && si!=sections.end(); si++)
        dos2 = isSgAsmDOSExtendedHeader(*si);

    /* Update DOS File Header with info about the real-mode text+data section. */
    size_t header_size = dos1->get_size() + (dos2 ? dos2->get_size() : 0);
    size_t total_size =  header_size + (p_rm_section ? p_rm_section->get_size() : 0);
    p_e_header_paragraphs = (header_size + 15) / 16;            /* rounded up to next paragraph */
    p_e_total_pages = (total_size + 511) / 512;                 /* rounded up to next page */
    p_e_last_page_size = total_size % 512;
}

/* Encode the DOS file header into disk format */
void *
SgAsmDOSFileHeader::encode(DOSFileHeader_disk *disk) const
{
    for (size_t i=0; i<ROSE_NELMTS(disk->e_magic); i++)
        disk->e_magic[i] = get_magic()[i];
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_last_page_size,     &(disk->e_last_page_size));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_total_pages,        &(disk->e_total_pages));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_nrelocs,            &(disk->e_nrelocs));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_header_paragraphs,  &(disk->e_header_paragraphs));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_minalloc,           &(disk->e_minalloc));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_maxalloc,           &(disk->e_maxalloc));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_ss,                 &(disk->e_ss));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_sp,                 &(disk->e_sp));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_cksum,              &(disk->e_cksum));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_ip,                 &(disk->e_ip));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_cs,                 &(disk->e_cs));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_relocs_offset,      &(disk->e_relocs_offset));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_overlay,            &(disk->e_overlay));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_res1,               &(disk->e_res1));
    return disk;
}

bool
SgAsmDOSFileHeader::reallocate()
{
    bool reallocated = SgAsmGenericHeader::reallocate();

    Rose::BinaryAnalysis::Address need = sizeof(DOSFileHeader_disk);
    if (need < get_size()) {
        if (isMapped()) {
            ROSE_ASSERT(get_mappedSize()==get_size());
            set_mappedSize(need);
        }
        set_size(need);
        reallocated = true;
    } else if (need > get_size()) {
        get_file()->shiftExtend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    if (p_relocs)
        p_e_relocs_offset = p_relocs->get_offset();
    updateFromRealModeSection();

    return reallocated;
}

/* Write the DOS file header back to disk */
void
SgAsmDOSFileHeader::unparse(std::ostream &f) const
{
    /* Unparse each section reachable from the DOS File Header (e.g., the Extended DOS Header) */
    SgAsmDOSExtendedHeader *dos2 = NULL;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        if (!dos2)
            dos2 = isSgAsmDOSExtendedHeader(*i);
        (*i)->unparse(f);
    }

    /* Some sanity checks:
     *  1. DOS File Header must be at the beginning of the file.
     *  2. DOS Extended Header, if present, must immediately follow DOS File Header
     *  3. DOS Real-Mode Text/Data section must immediately follow headers */
    ROSE_ASSERT(0==get_offset());
    ROSE_ASSERT(!dos2 || dos2->get_offset()==get_size());
//  ROSE_ASSERT(get_size()+(dos2?dos2->get_size():0) < (size_t)p_e_header_paragraphs*16);
//  ROSE_ASSERT(!p_rm_section || p_rm_section->get_offset()==(size_t)p_e_header_paragraphs*16);

    /* Unparse the header itself */
    DOSFileHeader_disk disk;
    encode(&disk);
    write(f, 0, sizeof(disk), &disk);
}

SgAsmGenericSection *
SgAsmDOSFileHeader::parseRealModeSection(Rose::BinaryAnalysis::Address max_offset)
{
    ROSE_ASSERT(NULL == p_rm_section);
    
    Rose::BinaryAnalysis::Address rm_offset = p_e_header_paragraphs * 16;
    Rose::BinaryAnalysis::Address rm_end = p_e_total_pages * 512;
    if (p_e_total_pages>0)
        rm_end -= 512 - (p_e_last_page_size%512);

    Rose::BinaryAnalysis::Address rm_size = rm_end>rm_offset ? rm_end-rm_offset : 0;
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
        p_rm_section = new SgAsmGenericSection(get_file(), this);
        p_rm_section->set_offset(rm_offset);
        p_rm_section->set_size(rm_size);
        p_rm_section->parse();
    } catch (ShortRead &p_ex) {
        /* If the offset or size is out of bounds for the file then assume that the real-mode section does not exist. This
         * can indicate that the DOS header is being used for something other than a DOS header. See
         * http://www.phreedom.org/solar/code/tinype/ for some examples of overlapping the DOS header with the PE header. */
        return NULL;
    }
    
    p_rm_section->set_name(new SgAsmBasicString("DOS real-mode text/data"));
    p_rm_section->set_synthesized(true);
    p_rm_section->set_purpose(SP_PROGRAM);
    p_rm_section->set_mappedPreferredRva(0);
    p_rm_section->set_mappedActualVa(0); /*will be assigned by Loader*/
    p_rm_section->set_mappedSize(rm_size);
    p_rm_section->set_mappedReadPermission(true);
    p_rm_section->set_mappedWritePermission(true);
    p_rm_section->set_mappedExecutePermission(true);
    return p_rm_section;
}

/* Print some debugging info */
void
SgAsmDOSFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sDOSFileHeader[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sDOSFileHeader.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u bytes\n",              p, w, "e_last_page_size",     p_e_last_page_size);
    fprintf(f, "%s%-*s = %u 512-byte pages\n",     p, w, "e_total_pages",        p_e_total_pages);
    fprintf(f, "%s%-*s = %u relocations\n",        p, w, "e_nrelocs",            p_e_nrelocs);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_header_paragraphs",  p_e_header_paragraphs);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_minalloc",           p_e_minalloc);
    fprintf(f, "%s%-*s = %u 16-byte paragraphs\n", p, w, "e_maxalloc",           p_e_maxalloc);
    fprintf(f, "%s%-*s = 0x%08u (%u)\n",           p, w, "e_ss",                 p_e_ss, p_e_ss);
    fprintf(f, "%s%-*s = 0x%08u (%u)\n",           p, w, "e_sp",                 p_e_sp, p_e_sp);
    fprintf(f, "%s%-*s = %u (zero implies not used)\n",p, w, "e_cksum",          p_e_cksum);
    fprintf(f, "%s%-*s = 0x%08u (%u)\n",           p, w, "e_ip",                 p_e_ip, p_e_ip);
    fprintf(f, "%s%-*s = 0x%08u (%u)\n",           p, w, "e_cs",                 p_e_cs, p_e_cs);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " (%" PRIu64 ")\n", p, w, "e_relocs_offset",  p_e_relocs_offset, p_e_relocs_offset);
    fprintf(f, "%s%-*s = %u\n",                    p, w, "e_overlay",            p_e_overlay);
    fprintf(f, "%s%-*s = 0x%08u (%u)\n",           p, w, "e_res1",               p_e_res1, p_e_res1);
    if (p_relocs) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "relocs",
                p_relocs->get_id(), p_relocs->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",        p, w, "relocs");
    }
    if (p_rm_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "rm_section",
                p_rm_section->get_id(), p_rm_section->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",        p, w, "rm_section");
    }

    Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extended DOS File Header
// This is normally tacked onto the end of a DOS File Header when the executable is PE, NE, LE, or LX. We treat it as a
// section belonging to the DOS File Header. The PE, NE, LE and LX File Header IR nodes usually also point to this section.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SgAsmDOSExtendedHeader::SgAsmDOSExtendedHeader(SgAsmDOSFileHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();
    set_name(new SgAsmBasicString("DOS Extended Header"));
    set_offset(get_header()->get_size());
    set_synthesized(true);
    set_purpose(SP_HEADER);
    set_size(sizeof(DOSExtendedHeader_disk));
}

SgAsmDOSExtendedHeader*
SgAsmDOSExtendedHeader::parse()
{
    SgAsmGenericSection::parse();
    
    /* Read header from file */
    DOSExtendedHeader_disk disk;
    readContentLocal(0, &disk, sizeof disk);

    /* Decode file format */
    ROSE_ASSERT(get_header()!=NULL); /*should be the DOS File Header*/
    ROSE_ASSERT(Rose::BinaryAnalysis::ByteOrder::ORDER_LSB==get_header()->get_sex());
    p_e_res1              = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_res1);
    p_e_oemid             = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_oemid);
    p_e_oeminfo           = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_oeminfo);
    p_e_res2              = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_res2);
    p_e_res3              = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_res3);
    p_e_res4              = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_res4);
    p_e_res5              = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_res5);
    p_e_res6              = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_res6);
    p_e_lfanew            = Rose::BinaryAnalysis::ByteOrder::leToHost(disk.e_lfanew);

    return this;
}

/* Encode the extended header back into disk format */
void *
SgAsmDOSExtendedHeader::encode(DOSExtendedHeader_disk *disk) const
{
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_res1,     &(disk->e_res1));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_oemid,    &(disk->e_oemid));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_oeminfo,  &(disk->e_oeminfo));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_res2,     &(disk->e_res2));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_res3,     &(disk->e_res3));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_res4,     &(disk->e_res4));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_res5,     &(disk->e_res5));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_res6,     &(disk->e_res6));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_lfanew,   &(disk->e_lfanew));
    return disk;
}

/* Write an extended header back to disk */
void
SgAsmDOSExtendedHeader::unparse(std::ostream &f) const
{
    DOSExtendedHeader_disk disk;
    encode(&disk);
    write(f, 0, sizeof disk, &disk);
}
    
void
SgAsmDOSExtendedHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sDOSExtendedHeader[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sDOSExtendedHeader.", prefix);
    }

    const int w = std::max(size_t{1}, Rose::DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",            p, w, "e_res1",     p_e_res1, p_e_res1);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_oemid",    p_e_oemid);
    fprintf(f, "%s%-*s = %u\n",                     p, w, "e_oeminfo",  p_e_oeminfo);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",            p, w, "e_res2",     p_e_res2, p_e_res2);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",            p, w, "e_res3",     p_e_res3, p_e_res3);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",            p, w, "e_res4",     p_e_res4, p_e_res4);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",            p, w, "e_res5",     p_e_res5, p_e_res5);
    fprintf(f, "%s%-*s = 0x%08x (%u)\n",            p, w, "e_res6",     p_e_res6, p_e_res6);
    fprintf(f, "%s%-*s = %" PRIu64 " byte offset (0x%" PRIx64 ")\n",  p, w, "e_lfanew",   p_e_lfanew,p_e_lfanew);

    if (variantT() == V_SgAsmDOSExtendedHeader) //unless a base class
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}

const char*
SgAsmDOSFileHeader::format_name() const {
    return formatName();
}

const char*
SgAsmDOSFileHeader::formatName() const {
    return "DOS";
}

SgAsmGenericSection*
SgAsmDOSFileHeader::parse_rm_section(Rose::BinaryAnalysis::Address x) {
    return parseRealModeSection(x);
}

void
SgAsmDOSFileHeader::update_from_rm_section() {
    updateFromRealModeSection();
}

bool
SgAsmDOSFileHeader::is_DOS(SgAsmGenericFile *x) {
    return isDos(x);
}

#endif
