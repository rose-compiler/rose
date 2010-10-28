/* PE Import Hint Name Table Entry */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Constructor */
void
SgAsmPEImportHNTEntry::ctor(SgAsmPEImportILTEntry *ilt_entry, std::string name)
{
    ROSE_ASSERT(get_name()==NULL);
    set_name(new SgAsmBasicString(name));
    ilt_entry->point_to_hnt(this);
}

/** Parses an HNT entry at the specified relative virtual address. */
SgAsmPEImportHNTEntry *
SgAsmPEImportHNTEntry::parse(rose_rva_t rva)
{
    SgAsmPEImportSection *isec = SageInterface::getEnclosingNode<SgAsmPEImportSection>(this);
    ROSE_ASSERT(isec!=NULL);
    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(isec->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Hint */
    uint16_t hint_disk = 0;
    try {
        isec->read_content(fhdr->get_loader_map(), rva.get_rva(), &hint_disk, sizeof hint_disk);
    } catch (const MemoryMap::NotMapped &e) {
        if (SgAsmPEImportSection::import_mesg("SgAsmPEImportHNTEntry::ctor: warning: hint at RVA 0x%08"PRIx64
                                              " contains unmapped virtual address 0x%08"PRIx64"\n", rva.get_rva(), e.va) &&
            e.map) {
            fprintf(stderr, "Memory map in effect at time of error:\n");
            e.map->dump(stderr, "    ");
        }
    }
    p_hint = le_to_host(hint_disk);
    
    /* Name */
    std::string s;
    try {
        s = isec->read_content_str(fhdr->get_loader_map(), rva.get_rva()+2);
    } catch (const MemoryMap::NotMapped &e) {
        if (SgAsmPEImportSection::import_mesg("SgAsmPEImportHNTEntry::ctor: warning: string at RVA 0x%08"PRIx64
                                              " contains unmapped virtual address 0x%08"PRIx64"\n", rva.get_rva()+2, e.va) &&
            e.map) {
            fprintf(stderr, "Memory map in effect at time of error:\n");
            e.map->dump(stderr, "    ");
        }
    }
    p_name->set_string(s);

    /* Padding */
    p_padding = 0;
    if (s.size()+1 % 2) {
        try {
            unsigned char byte;
            isec->read_content(fhdr->get_loader_map(), rva.get_rva()+2+s.size()+1, &byte, 1);
            p_padding = byte;
        } catch (const MemoryMap::NotMapped &e) {
            if (SgAsmPEImportSection::import_mesg("SgAsmPEImportHNTEntry::ctor: warning: padding at virtual address 0x%08"PRIx64
                                                  " is not mapped\n", e.va) &&
                e.map) {
                fprintf(stderr, "Memory map in effect at time of error:\n");
                e.map->dump(stderr, "    ");
            }
        }
    }
    return this;
}

void
SgAsmPEImportHNTEntry::unparse(std::ostream &f, rose_rva_t rva) const
{
    uint16_t hint_disk;
    host_to_le(p_hint, &hint_disk);
    rose_addr_t spos = rva.get_rel();
    spos = rva.get_section()->write(f, spos, 2, &hint_disk);
    spos = rva.get_section()->write(f, spos, p_name->get_string());
    spos = rva.get_section()->write(f, spos, '\0');
    if ((p_name->get_string().size()+1) % 2)
        rva.get_section()->write(f, spos, p_padding);
}

/* Print debugging info */
void
SgAsmPEImportHNTEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sHNTEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sHNTEntry.", prefix);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %u\t\"%s\"", p, w, "hint/name", p_hint, p_name->c_str());
    if ((p_name->get_string().size()+1)%2)
        fprintf(f, " + '\\%03o'", p_padding);
    fputc('\n', f);
}
