/* PE Import Lookup Table (also used for the Import Address Table) */
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Constructor. rva is the address of the table and should be bound to a section (which may not necessarily be isec). This
 * C object represents one of two PE objects depending on the value of is_iat.
 *    true  => PE Import Address Table
 *    false => PE Import Lookup Table */
void
SgAsmPEImportLookupTable::ctor(SgAsmPEImportSection *isec, rose_rva_t rva, size_t idir_idx, bool is_iat)
{
    ROSE_ASSERT(p_entries==NULL);
    p_entries = new SgAsmPEImportILTEntryList();
    p_entries->set_parent(this);
    p_is_iat = is_iat;
    const char *tname = is_iat ? "Import Address Table" : "Import Lookup Table";

    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(isec->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Read the Import Lookup (or Address) Table, an array of 32 or 64 bit values, the last of which is zero */
    if (rva.get_section()!=isec) {
        rose_addr_t start_rva = isec->get_mapped_actual_va() - isec->get_base_va();
        SgAsmPEImportSection::import_mesg("SgAsmPEImportSection::ctor: warning: %s RVA is outside PE Import Table\n"
                                          "        Import Directory Entry #%zu\n"
                                          "        %s RVA is %s\n"
                                          "        PE Import Table mapped from 0x%08"PRIx64" to 0x%08"PRIx64"\n", 
                                          tname, idir_idx, tname, rva.to_string().c_str(),
                                          start_rva, start_rva+isec->get_mapped_size());
    }

    for (size_t i=0; 1; i++) {
        uint64_t ilt_entry_word=0;
        unsigned char buf[8];
        ROSE_ASSERT(fhdr->get_word_size() <= sizeof buf);
        try {
            isec->read_content(fhdr->get_loader_map(), rva.get_rva(), buf, fhdr->get_word_size());
        } catch (const MemoryMap::NotMapped &e) {
            if (SgAsmPEImportSection::import_mesg("SgAsmPEImportSection::ctor: error: in PE Import Directory entry %zu: "
                                                  "%s entry %zu starting at RVA 0x%08"PRIx64
                                                  " contains unmapped virtual address 0x%08"PRIx64"\n",
                                                  idir_idx, tname, i, rva.get_rva(), e.va) &&
                e.map) {
                    fprintf(stderr, "Memory map in effect at time of error:\n");
                    e.map->dump(stderr, "    ");
            }
        }

        if (4==fhdr->get_word_size()) {
            ilt_entry_word = le_to_host(*(uint32_t*)buf);
        } else if (8==fhdr->get_word_size()) {
            ilt_entry_word = le_to_host(*(uint64_t*)buf);
        } else {
            throw FormatError("unsupported PE word size");
        }

        rva.set_rva(rva.get_rva()+fhdr->get_word_size()); /*advance to next entry of table*/
        if (0==ilt_entry_word)
            break;

        SgAsmPEImportILTEntry *ilt_entry = new SgAsmPEImportILTEntry(isec, ilt_entry_word);
        add_ilt_entry(ilt_entry);

        if (SgAsmPEImportILTEntry::ILT_HNT_ENTRY_RVA==ilt_entry->get_entry_type()) {
            SgAsmPEImportHNTEntry *hnt_entry = new SgAsmPEImportHNTEntry(isec, ilt_entry->get_hnt_entry_rva());
            ilt_entry->set_hnt_entry(hnt_entry);
            hnt_entry->set_parent(ilt_entry);
        }
    }
}

/* Adds another Import Lookup Table Entry or Import Address Table Entry to the Import Lookup Table */
void
SgAsmPEImportLookupTable::add_ilt_entry(SgAsmPEImportILTEntry *ilt_entry)
{
    ROSE_ASSERT(p_entries!=NULL);
    ROSE_ASSERT(ilt_entry);
    p_entries->set_isModified(true);
    p_entries->get_vector().push_back(ilt_entry);
    ROSE_ASSERT(p_entries->get_vector().size()>0);
    ilt_entry->set_parent(this);
}

void
SgAsmPEImportLookupTable::unparse(std::ostream &f, const SgAsmPEFileHeader *fhdr, rose_rva_t rva) const
{
    if (rva!=0) {
        //const char *tname = p_is_iat ? "Import Address Table" : "Import Lookup Table";
        for (size_t i=0; i<p_entries->get_vector().size(); i++) {
            SgAsmPEImportILTEntry *ilt_entry = p_entries->get_vector()[i];
            try {
                ilt_entry->unparse(f, fhdr, rva, i);
            } catch (const ShortWrite&) {
                SgAsmPEImportSection::import_mesg("SgAsmPEImportLookupTable::unparse: "
                                             "error: ILT entry #%zu skipped (short write)\n", i);
            }
        }

        /* Zero terminated */
        uint64_t zero = 0;
        ROSE_ASSERT(fhdr->get_word_size()<=sizeof zero);
        rose_addr_t spos = rva.get_rel() + p_entries->get_vector().size() * fhdr->get_word_size();
        rva.get_section()->write(f, spos, fhdr->get_word_size(), &zero);
    }
}

/* Print some debugging info for an Import Lookup Table or Import Address Table */
void
SgAsmPEImportLookupTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    prefix = "    ...";
    const char *tabbr = p_is_iat ? "IAT" : "ILT";
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s%s[%zd].", prefix, tabbr, idx);
    } else {
        sprintf(p, "%s%s.", prefix, tabbr);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %zu\n", p, w, "nentries", p_entries->get_vector().size());
    for (size_t i=0; i<p_entries->get_vector().size(); i++) {
        SgAsmPEImportILTEntry *ilt_entry = p_entries->get_vector()[i];
        ilt_entry->dump(f, p, i);
    }
}
