/* Windows PE Section Tables (SgAsmPESectionTable and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/BinaryLoader.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/Diagnostics.h>

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

SgAsmPESectionTableEntry::SgAsmPESectionTableEntry(const SgAsmPESectionTableEntry::PESectionTableEntry_disk *disk) {
    initializeProperties();

    char name[9];
    strncpy(name, disk->name, 8);
    name[8] = '\0';
    this->set_name( name );

    /* Decode file format */
    ASSERT_not_null(disk);
    p_virtual_size     = ByteOrder::leToHost(disk->virtual_size);
    p_rva              = ByteOrder::leToHost(disk->rva);
    p_physical_size    = ByteOrder::leToHost(disk->physical_size);
    p_physical_offset  = ByteOrder::leToHost(disk->physical_offset);
    p_coff_line_nums   = ByteOrder::leToHost(disk->coff_line_nums);
    p_n_relocs         = ByteOrder::leToHost(disk->n_relocs);
    p_n_coff_line_nums = ByteOrder::leToHost(disk->n_coff_line_nums);
    p_flags            = ByteOrder::leToHost(disk->flags);
}

void
SgAsmPESectionTableEntry::update_from_section(SgAsmPESection *section)
{
    updateFromSection(section);
}

void
SgAsmPESectionTableEntry::updateFromSection(SgAsmPESection *section)
{
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(section);
    ROSE_ASSERT(fhdr!=nullptr);

    p_virtual_size = section->get_mappedSize();
    p_rva = section->get_mappedPreferredRva();
    p_physical_size = section->get_size();
    p_physical_offset = section->get_offset();
    p_name = section->get_name()->get_string();

    /* Mapping permissions */
    if (section->get_mappedReadPermission()) {
        p_flags |= SgAsmPESectionTableEntry::OF_READABLE;
    } else {
        p_flags &= ~SgAsmPESectionTableEntry::OF_READABLE;
    }
    if (section->get_mappedWritePermission()) {
        p_flags |= SgAsmPESectionTableEntry::OF_WRITABLE;
    } else {
        p_flags &= ~SgAsmPESectionTableEntry::OF_WRITABLE;
    }
    if (section->get_mappedExecutePermission()) {
        p_flags |= SgAsmPESectionTableEntry::OF_EXECUTABLE;
    } else {
        p_flags &= ~SgAsmPESectionTableEntry::OF_EXECUTABLE;
    }

    /* Mapping alignment */
    if (section->isMapped() && section->get_mappedAlignment()!=fhdr->get_e_section_align()) {
        switch (section->get_mappedAlignment()) {
            case 0:
            case 1:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_1;
                break;
            case 2:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_2;
                break;
            case 4:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_4;
                break;
            case 8:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_8;
                break;
            case 16:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_16;
                break;
            case 32:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_32;
                break;
            case 64:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_64;
                break;
            case 128:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_128;
                break;
            case 256:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_256;
                break;
            case 512:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_512;
                break;
            case 1024:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_1k;
                break;
            case 2048:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_2k;
                break;
            case 4096:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_4k;
                break;
            case 8192:
                p_flags &= ~SgAsmPESectionTableEntry::OF_ALIGN_MASK;
                p_flags |=  SgAsmPESectionTableEntry::OF_ALIGN_8k;
                break;
            default:
                break; /* leave as is */
        }
    }

#if 0 /*FIXME*/
    p_coff_line_nums = 0;
    p_n_relocs = 0;
    p_n_coff_line_nums = 0;
#endif
}

/* Encodes a section table entry back into disk format. */
void *
SgAsmPESectionTableEntry::encode(PESectionTableEntry_disk *disk) const
{
    /* The file can hold up to eight characters of the name. The name is NUL-padded, not necessarily NUL-terminated. */
    if (p_name.size()>8) {
        mlog[WARN] <<"section name too long to store in PE file: \"" <<escapeString(p_name) <<"\" (truncated)\n";
    }
    memset(disk->name, 0, sizeof(disk->name));

#ifdef USE_ROSE
    // DQ (1/27/2010): std::min() does not appear to be handle different type of arguments for ROSE. Need to look into this later.
    // memcpy(disk->name, p_name.c_str(), std::min(sizeof(disk->name), (size_t)p_name.size()));
    memcpy(disk->name, p_name.c_str(), std::min( (size_t)(sizeof(disk->name)), (size_t)(p_name.size()) ));
#else
    memcpy(disk->name, p_name.c_str(), std::min(sizeof(disk->name), p_name.size()));
#endif

    ByteOrder::hostToLe(p_virtual_size,     &(disk->virtual_size));
    ByteOrder::hostToLe(p_rva,              &(disk->rva));
    ByteOrder::hostToLe(p_physical_size,    &(disk->physical_size));
    ByteOrder::hostToLe(p_physical_offset,  &(disk->physical_offset));
    ByteOrder::hostToLe(p_coff_line_nums,   &(disk->coff_line_nums));
    ByteOrder::hostToLe(p_n_relocs,         &(disk->n_relocs));
    ByteOrder::hostToLe(p_n_coff_line_nums, &(disk->n_coff_line_nums));
    ByteOrder::hostToLe(p_flags,            &(disk->flags));

    return disk;
}

/* Prints some debugging info */
void
SgAsmPESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPESectionTableEntry[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPESectionTableEntry.", prefix);
    }

    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",            p, w, "virtual_size",     p_virtual_size);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",              p, w, "rva",              p_rva);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",            p, w, "physical_size",    p_physical_size);
    fprintf(f, "%s%-*s = %" PRIu64 " file byte offset\n", p, w, "physical_offset",  p_physical_offset);
    fprintf(f, "%s%-*s = %u byte offset\n",               p, w, "coff_line_nums",   p_coff_line_nums);
    fprintf(f, "%s%-*s = %u\n",                           p, w, "n_relocs",         p_n_relocs);
    fprintf(f, "%s%-*s = %u\n",                           p, w, "n_coff_line_nums", p_n_coff_line_nums);
    fprintf(f, "%s%-*s = 0x%08x\n",                       p, w, "flags",            p_flags);
}

/* Constructor */
SgAsmPESectionTable::SgAsmPESectionTable(SgAsmPEFileHeader *fhdr)
    : SgAsmGenericSection(fhdr->get_file(), fhdr) {
    initializeProperties();

    ROSE_ASSERT(fhdr!=nullptr);
    fhdr->set_sectionTable(this);

    set_synthesized(true);
    set_name(new SgAsmBasicString("PE Section Table"));
    set_purpose(SP_HEADER);
}

/* Parser */
SgAsmPESectionTable*
SgAsmPESectionTable::parse()
{
    SgAsmGenericSection::parse();

    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=nullptr);

    /* Parse section table and construct section objects, but do not parse the sections yet. */
    SgAsmGenericSectionPtrList pending;
    const size_t entsize = sizeof(SgAsmPESectionTableEntry::PESectionTableEntry_disk);
    for (size_t i=0; i<fhdr->get_e_nsections(); i++) {
        SgAsmPESectionTableEntry::PESectionTableEntry_disk disk;
        if (entsize!=readContentLocal(i * entsize, &disk, entsize, false))
            mlog[WARN] <<"SgAsmPESectionTable::parse: section table entry " <<i
                       <<" at file offset " <<StringUtility::addrToString(get_offset()+i*entsize)
                       <<" extends beyond end of defined section table.\n";
        SgAsmPESectionTableEntry *entry = new SgAsmPESectionTableEntry(&disk);

        SgAsmPESection *section = nullptr;
        if (entry->get_name() == ".idata") {
            // If the PAIR_IMPORTS rva/size pair has a non-zero pointer, then avoid creating an import table from this ".idata"
            // section. Sometimes the rva/size pair will point to a different region in memory than ".idata", in which case the
            // rva/size pair should be honored instead.
            SgAsmPERVASizePair *pair = fhdr->get_rvaSizePairs()->get_pairs()[SgAsmPEFileHeader::PAIR_IMPORTS];
            if (pair->get_e_rva().va().orElse(pair->get_e_rva().rva()) == 0) {
                section = new SgAsmPEImportSection(fhdr); // treat .idata as an import table
            } else {
                section = new SgAsmPESection(fhdr); // construct the import table from the rva/size pair instead
            }
        } else {
            section = new SgAsmPESection(fhdr);
        }
        section->initFromSectionTable(entry, i+1);
        pending.push_back(section);
    }

    /* Build the memory mapping like the real loader would do. This is the same code used by
     * SgAsmExecutableFileFormat::parseBinaryFormat() except we're doing it here early because we need it in the rest of the
     * PE parser. */
    ROSE_ASSERT(nullptr==fhdr->get_loaderMap());
    BinaryLoader::Ptr loader = BinaryLoader::lookup(fhdr); /*no need to clone; we're not changing any settings*/
    ASSERT_not_null(loader);
    MemoryMap::Ptr loader_map = MemoryMap::instance();
    loader->remap(loader_map, fhdr);
    fhdr->set_loaderMap(loader_map);

    /* Parse each section after the loader map is created */
    for (size_t i=0; i<pending.size(); i++)
        pending[i]->parse();

    return this;
}

void
SgAsmPESectionTable::add_section(SgAsmPESection *section)
{
    addSection(section);
}

void
SgAsmPESectionTable::addSection(SgAsmPESection *section)
{
    ROSE_ASSERT(section!=nullptr);
    ROSE_ASSERT(section->get_file()==get_file());
    ROSE_ASSERT(section->get_header()==get_header());
    ROSE_ASSERT(section->get_section_entry()==nullptr);         /* must not be in the section table yet */
    
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=nullptr);
    
    /* Assign an ID if there isn't one yet. */
    if (section->get_id()<0) {
        SgAsmGenericSectionList *seclist = fhdr->get_sections();;
        int max_id=0; /*assume zero is used so we start at one*/
        for (size_t i=0; i<seclist->get_sections().size(); i++) {
            SgAsmGenericSection *s = seclist->get_sections()[i];

                        max_id = std::max(max_id, s->get_id());

                }
        section->set_id(max_id+1);
    }
    
    /* Create a new section table entry. */
    SgAsmPESectionTableEntry *entry = new SgAsmPESectionTableEntry;
    entry->updateFromSection(section);
    section->set_section_entry(entry);
}

/* Pre-unparsing updates */
bool
SgAsmPESectionTable::reallocate()
{
    bool reallocated = false;
    
    /* Resize based on section having largest ID */
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr != nullptr);
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();
    int max_id = 0;
    for (size_t i=0; i<sections.size(); i++) {

        max_id = std::max(max_id, sections[i]->get_id());

        }
    
    size_t nsections = max_id; /*PE section IDs are 1-origin*/
    size_t need = nsections * sizeof(SgAsmPESectionTableEntry::PESectionTableEntry_disk);
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

    return reallocated;
}

/* Writes the section table back to disk. */
void
SgAsmPESectionTable::unparse(std::ostream &f) const
{
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr != nullptr);
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();

    for (size_t i = 0; i < sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            SgAsmPESection *section = isSgAsmPESection(sections[i]);
            ROSE_ASSERT(section!=nullptr);

            /* Write the table entry */
            ROSE_ASSERT(section->get_id() > 0); /*ID's are 1-origin in PE*/
            size_t slot = section->get_id() - 1;
            SgAsmPESectionTableEntry *shdr = section->get_section_entry();
            SgAsmPESectionTableEntry::PESectionTableEntry_disk disk;
            shdr->encode(&disk);
            write(f, slot*sizeof(disk), sizeof disk, &disk);
        }
    }
}

/* Prints some debugging info */
void
SgAsmPESectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPESectionTable[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPESectionTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    if (variantT() == V_SgAsmPESectionTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

#endif
