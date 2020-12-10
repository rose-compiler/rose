/* ELF Note Sections (SgAsmElfNoteSection and related classes) */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

void
SgAsmElfNoteEntry::ctor(SgAsmElfNoteSection *section)
{
    ROSE_ASSERT(section->get_entries()!=NULL);
    section->get_entries()->get_entries().push_back(this);
    ROSE_ASSERT(section->get_entries()->get_entries().size()>0);
    set_parent(section->get_entries());

    p_name = new SgAsmBasicString("");
    p_name->set_parent(this);
}

SgAsmGenericString *
SgAsmElfNoteEntry::get_name() const
{
    return p_name;
}

void
SgAsmElfNoteEntry::set_name(SgAsmGenericString *name)
{
    if (name!=p_name) {
        if (p_name) {
            p_name->set_parent(NULL);
            SageInterface::deleteAST(p_name);
        }
        p_name = name;
        if (p_name)
            p_name->set_parent(this);
        set_isModified(true);
    }
}

void
SgAsmElfNoteEntry::set_payload(const void *_buf, size_t nbytes)
{
    const unsigned char *buf = (const unsigned char*)_buf;
    p_payload.clear();
    p_payload.reserve(nbytes);
    for (size_t i=0; i<nbytes; i++)
        p_payload.push_back(buf[i]);
}

rose_addr_t
SgAsmElfNoteEntry::parse(rose_addr_t at)
{
    /* Find the section holding this note */
    SgAsmElfNoteSection *notes = SageInterface::getEnclosingNode<SgAsmElfNoteSection>(this);
    ROSE_ASSERT(notes!=NULL);
    ROSE_ASSERT(at < notes->get_size());
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(notes->get_header());
    ROSE_ASSERT(fhdr!=NULL);
    
    /* Length of note entry name, including NUL termination */
    uint32_t u32;
    notes->read_content_local(at, &u32, 4);
    size_t name_size = disk_to_host(fhdr->get_sex(), u32);
    at += 4;

    /* Length of note entry description (i.e., the payload) */
    notes->read_content_local(at, &u32, 4);
    size_t payload_size = disk_to_host(fhdr->get_sex(), u32);
    at += 4;

    /* Type of note */
    notes->read_content_local(at, &u32, 4);
    unsigned type = disk_to_host(fhdr->get_sex(), u32);
    at += 4;

    /* NUL-terminated name */
    std::string note_name = notes->read_content_local_str(at);
    if (note_name.size() + 1 > name_size && name_size > 0)
        note_name = note_name.substr(0, name_size-1);
    at += name_size;
    at = (at+3) & ~0x3; /* payload is aligned on a four-byte offset */

    /* Set properties */
    get_name()->set_string(note_name);
    set_type(type);
    p_payload = notes->read_content_local_ucl(at, payload_size);

    return at + payload_size;
}

rose_addr_t
SgAsmElfNoteEntry::unparse(std::ostream &f, rose_addr_t at)
{
    /* Find the section holding this note */
    SgAsmElfNoteSection *notes = SageInterface::getEnclosingNode<SgAsmElfNoteSection>(this);
    ROSE_ASSERT(notes!=NULL);
    ROSE_ASSERT(at < notes->get_size());
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(notes->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Name size, including NUL termination */
    uint32_t u32;
    host_to_disk(fhdr->get_sex(), p_name->get_string().size()+1, &u32);
    notes->write(f, at, 4, &u32);
    at += 4;

    /* Payload size */
    host_to_disk(fhdr->get_sex(), p_payload.size(), &u32);
    notes->write(f, at, 4, &u32);
    at += 4;
    
    /* Type */
    host_to_disk(fhdr->get_sex(), p_type, &u32);
    notes->write(f, at, 4, &u32);
    at += 4;
    
    /* Name with NUL termination and padded to a multiple of four bytes */
    std::string name = p_name->get_string();
    while ((name.size()+1) % 4)
        name += '\0';
    notes->write(f, at, name.size()+1, name.c_str());
    at += name.size()+1;
    
    /* Payload */
    notes->write(f, at, p_payload);
    at += p_payload.size();
    
    return at;
}

rose_addr_t
SgAsmElfNoteEntry::calculate_size() const {
    rose_addr_t need = 12;                           /*namesize, payloadsize, type*/
    need += p_name->get_string().size() + 1;    /*name plus NUL terminator*/
    need = (need+3) & ~0x3;                     /*pad to align payload on a four-byte offset */
    need += p_payload.size();
    return need;
}

void
SgAsmElfNoteEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfNoteEntry[%zd]", prefix, idx);
    } else {
        sprintf(p, "%sElfNoteEntry", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s \"%s\"\n", p, w, "name", p_name->get_string(true).c_str());
    fprintf(f, "%s%-*s %u\n", p, w, "type", p_type);
    if (p_payload.size()>0) {
        fprintf(f, "%s%-*s = %" PRIuPTR " bytes\n", p, w, "extra", p_payload.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_payload);
    }
}

void
SgAsmElfNoteSection::ctor()
{
    p_entries = new SgAsmElfNoteEntryList;
    p_entries->set_parent(this);
}

/* Parse existing notes */
SgAsmElfNoteSection *
SgAsmElfNoteSection::parse()
{
    SgAsmElfSection::parse();

    rose_addr_t at=0;
    while (at < get_size()) {
        SgAsmElfNoteEntry *note = new SgAsmElfNoteEntry(this);
        at = note->parse(at);
    }
    return this;
}

bool
SgAsmElfNoteSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();
    
    /* How much space is needed by the notes? */
    rose_addr_t need = 0;
    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfNoteEntry *ent = p_entries->get_entries()[i];
        need += ent->calculate_size();
    }

    /* Adjust the section/segment size */
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

void
SgAsmElfNoteSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);

    rose_addr_t at = 0;
    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfNoteEntry *ent = p_entries->get_entries()[i];
        at = ent->unparse(f, at);
    }
}


void
SgAsmElfNoteSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfNoteSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfNoteSection.", prefix);
    }

    SgAsmElfSection::dump(f, p, -1);

    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfNoteEntry *ent = p_entries->get_entries()[i];
        ent->dump(f, p, i);
    }
}

#endif
