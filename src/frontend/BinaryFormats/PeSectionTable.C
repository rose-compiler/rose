/* Windows PE Section Tables (SgAsmPESectionTable and related classes) */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Loader.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


void
SgAsmPESectionTableEntry::ctor(const PESectionTableEntry_disk *disk)
{
    char name[9];
    strncpy(name, disk->name, 8);
    name[8] = '\0';
    this->set_name( name );

    /* Decode file format */
    p_virtual_size     = le_to_host(disk->virtual_size);
    p_rva              = le_to_host(disk->rva);
    p_physical_size    = le_to_host(disk->physical_size);
    p_physical_offset  = le_to_host(disk->physical_offset);
    p_coff_line_nums   = le_to_host(disk->coff_line_nums);
    p_n_relocs         = le_to_host(disk->n_relocs);
    p_n_coff_line_nums = le_to_host(disk->n_coff_line_nums);
    p_flags            = le_to_host(disk->flags);
}

/** Update this section table entry with newer information from the section */
void
SgAsmPESectionTableEntry::update_from_section(SgAsmPESection *section)
{
    p_virtual_size = section->get_mapped_size();
    p_rva = section->get_mapped_preferred_rva();
    p_physical_size = section->get_size();
    p_physical_offset = section->get_offset();
    p_name = section->get_name()->get_string();

#if 0 /*FIXME*/
    p_coff_line_nums = 0;
    p_n_relocs = 0;
    p_n_coff_line_nums = 0;
    p_flags = 0;
#endif
}

/* Encodes a section table entry back into disk format. */
void *
SgAsmPESectionTableEntry::encode(PESectionTableEntry_disk *disk) const
{
    /* The file can hold up to eight characters of the name. The name is NUL-padded, not necessarily NUL-terminated. */
    if (p_name.size()>8)
        fprintf(stderr, "warning: section name too long to store in PE file: \"%s\" (truncated)\n", p_name.c_str());
    memset(disk->name, 0, sizeof(disk->name));


#ifdef USE_ROSE
 // DQ (1/27/2010): std::min() does not appear to be handle different type of arguments for ROSE. Need to look into this later.
 // memcpy(disk->name, p_name.c_str(), std::min(sizeof(disk->name), (size_t)p_name.size()));
    memcpy(disk->name, p_name.c_str(), std::min( (size_t)(sizeof(disk->name)), (size_t)(p_name.size()) ));
#else
    memcpy(disk->name, p_name.c_str(), std::min(sizeof(disk->name), p_name.size()));
#endif


    host_to_le(p_virtual_size,     &(disk->virtual_size));
    host_to_le(p_rva,              &(disk->rva));
    host_to_le(p_physical_size,    &(disk->physical_size));
    host_to_le(p_physical_offset,  &(disk->physical_offset));
    host_to_le(p_coff_line_nums,   &(disk->coff_line_nums));
    host_to_le(p_n_relocs,         &(disk->n_relocs));
    host_to_le(p_n_coff_line_nums, &(disk->n_coff_line_nums));
    host_to_le(p_flags,            &(disk->flags));

    return disk;
}

/* Prints some debugging info */
void
SgAsmPESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESectionTableEntry.", prefix);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

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
void
SgAsmPESectionTable::ctor()
{
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    fhdr->set_section_table(this);

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
    ROSE_ASSERT(fhdr!=NULL);

    /* Parse section table and construct section objects, but do not parse the sections yet. */
    SgAsmGenericSectionPtrList pending;
    const size_t entsize = sizeof(SgAsmPESectionTableEntry::PESectionTableEntry_disk);
    for (size_t i=0; i<fhdr->get_e_nsections(); i++) {
        SgAsmPESectionTableEntry::PESectionTableEntry_disk disk;
        if (entsize!=read_content_local(i * entsize, &disk, entsize, false))
            fprintf(stderr, "SgAsmPESectionTable::parse: warning: section table entry %zu at file offset 0x%08"PRIx64
                    " extends beyond end of defined section table.\n",
                    i, get_offset()+i*entsize);
        SgAsmPESectionTableEntry *entry = new SgAsmPESectionTableEntry(&disk);

        SgAsmPESection *section = NULL;
        if (entry->get_name() == ".idata") {
            section = new SgAsmPEImportSection(fhdr);
        } else {
            section = new SgAsmPESection(fhdr);
        }
        section->init_from_section_table(entry, i+1);
        pending.push_back(section);
    }

    /* Build the memory mapping like the real loader would do. This is the same code used by
     * SgAsmExecutableFileFormat::parseBinaryFormat() except we're doing here early because we need it in the rest of the PE
     * parser. */
    ROSE_ASSERT(NULL==fhdr->get_loader_map());
    Loader *loader = Loader::find_loader(fhdr);
    ROSE_ASSERT(loader!=NULL);
    MemoryMap *loader_map = loader->map_all_sections(NULL, fhdr);
    fhdr->set_loader_map(loader_map);

    /* Parse each section after the loader map is created */
    for (size_t i=0; i<pending.size(); i++)
        pending[i]->parse();

    return this;
}

/** Attaches a previously unattached PE Section to the PE Section Table. This method complements
 *  SgAsmPESection::init_from_section_table. This method initializes the section table from the section while
 *  ini_from_section_table() initializes the section from the section table. */
void
SgAsmPESectionTable::add_section(SgAsmPESection *section)
{
    ROSE_ASSERT(section!=NULL);
    ROSE_ASSERT(section->get_file()==get_file());
    ROSE_ASSERT(section->get_header()==get_header());
    ROSE_ASSERT(section->get_section_entry()==NULL);            /* must not be in the section table yet */
    
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    
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
    entry->update_from_section(section);
    section->set_section_entry(entry);
}

/* Pre-unparsing updates */
bool
SgAsmPESectionTable::reallocate()
{
    bool reallocated = false;
    
    /* Resize based on section having largest ID */
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr != NULL);
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();
    int max_id = 0;
    for (size_t i=0; i<sections.size(); i++) {

        max_id = std::max(max_id, sections[i]->get_id());

	}
    
    size_t nsections = max_id; /*PE section IDs are 1-origin*/
    size_t need = nsections * sizeof(SgAsmPESectionTableEntry::PESectionTableEntry_disk);
    if (need < get_size()) {
        if (is_mapped()) {
            ROSE_ASSERT(get_mapped_size()==get_size());
            set_mapped_size(need);
        }
        set_size(need);
        reallocated = true;
    } else if (need > get_size()) {
        get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    return reallocated;
}

/* Writes the section table back to disk along with each of the sections. */
void
SgAsmPESectionTable::unparse(std::ostream &f) const
{
    SgAsmPEFileHeader *fhdr = dynamic_cast<SgAsmPEFileHeader*>(get_header());
    ROSE_ASSERT(fhdr != NULL);
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();

    for (size_t i = 0; i < sections.size(); i++) {
        if (sections[i]->get_id()>=0) {
            SgAsmPESection *section = dynamic_cast<SgAsmPESection*>(sections[i]);

            /* Write the table entry */
            ROSE_ASSERT(section->get_id() > 0); /*ID's are 1-origin in PE*/
            size_t slot = section->get_id() - 1;
            SgAsmPESectionTableEntry *shdr = section->get_section_entry();
            SgAsmPESectionTableEntry::PESectionTableEntry_disk disk;
            shdr->encode(&disk);
            write(f, slot*sizeof(disk), sizeof disk, &disk);

            /* Write the section */
            section->unparse(f);
        }
    }
}

/* Prints some debugging info */
void
SgAsmPESectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESectionTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    if (variantT() == V_SgAsmPESectionTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
