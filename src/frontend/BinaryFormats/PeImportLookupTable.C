/* PE Import Lookup Table (also used for the Import Address Table) */
#include "sage3basic.h"
#include "stringify.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Constructs a new import lookup table or import address table and adds it as a child of the import directory. */
void
SgAsmPEImportLookupTable::ctor(SgAsmPEImportDirectory *idir)
{
    ROSE_ASSERT(idir!=NULL);
    set_parent(idir);

    ROSE_ASSERT(p_entries==NULL);
    p_entries = new SgAsmPEImportILTEntryList();
    p_entries->set_parent(this);

    switch (get_table_kind()) {
        case ILT_LOOKUP_TABLE:
            ROSE_ASSERT(idir->get_ilt()==NULL);
            idir->set_ilt(this);
            break;
        case ILT_ADDRESS_TABLE:
            ROSE_ASSERT(idir->get_iat()==NULL);
            idir->set_iat(this);
            break;
    }
}

/** Parses a PE Import Lookup Table from the file. The @p rva is the address of the table and should be bound to a section
 * (which may not necessarily be isec). The @p idir_idx argument is only for error messages and should indicate the index of
 * this table's SgAsmPEImportDirectory object within the SgAsmPEImportSection. */
SgAsmPEImportLookupTable *
SgAsmPEImportLookupTable::parse(rose_rva_t rva, size_t idir_idx)
{
    std::string tname = stringifySgAsmPEImportLookupTableTableKind(get_table_kind());
    SgAsmPEImportSection *isec = SageInterface::getEnclosingNode<SgAsmPEImportSection>(this);
    ROSE_ASSERT(isec!=NULL);
    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(isec->get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Read the Import Lookup (or Address) Table, an array of 32 or 64 bit values, the last of which is zero */
    if (rva.get_section()!=isec) {
        rose_addr_t start_rva = isec->get_mapped_actual_va() - isec->get_base_va();
        SgAsmPEImportSection::import_mesg("SgAsmPEImportSection::ctor: warning: %s rva is outside PE Import Table\n"
                                          "        Import Directory Entry #%zu\n"
                                          "        %s rva is %s\n"
                                          "        PE Import Table mapped from 0x%08"PRIx64" to 0x%08"PRIx64"\n", 
                                          tname.c_str(), idir_idx, tname.c_str(), rva.to_string().c_str(),
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
                                                  idir_idx, tname.c_str(), i, rva.get_rva(), e.va) &&
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

        SgAsmPEImportILTEntry *ilt_entry = new SgAsmPEImportILTEntry(this);
        ilt_entry->parse(ilt_entry_word);

        if (SgAsmPEImportILTEntry::ILT_HNT_ENTRY_RVA==ilt_entry->get_entry_type()) {
            SgAsmPEImportHNTEntry *hnt_entry = new SgAsmPEImportHNTEntry(ilt_entry);
            hnt_entry->parse(ilt_entry->get_hnt_entry_rva());
        }
    }
    return this;
}

/** Constructs a name/hint Import Lookup Table Entry and adds it to the Import Lookup Table.  Returns the index of the new
 *  entry. */
size_t
SgAsmPEImportLookupTable::add_entry(const std::string &name, size_t hint) 
{
    ROSE_ASSERT(get_entries()!=NULL);
    SgAsmPEImportILTEntry *ilt_entry = new SgAsmPEImportILTEntry(this);
    new SgAsmPEImportHNTEntry(ilt_entry, name, hint); /* added to ilt_entry by side effect */
    return add_entry(ilt_entry); /* only to get the index used by the ILTEntry c'tor */
}
    
/** Adds another Import Lookup Table Entry or Import Address Table Entry to the Import Lookup Table. Returns the index of the
 *  new entry. If the entry is already present then it will not be added again. */
size_t
SgAsmPEImportLookupTable::add_entry(SgAsmPEImportILTEntry *ilt_entry)
{
    ROSE_ASSERT(ilt_entry);
    ROSE_ASSERT(get_entries()!=NULL);
    SgAsmPEImportILTEntryPtrList &entries = get_entries()->get_vector();

    for (size_t i=0; i<entries.size(); i++) {
        if (entries[i]==ilt_entry)
            return i;
    }
    
    get_entries()->set_isModified(true);
    entries.push_back(ilt_entry);
    ilt_entry->set_parent(this);
    return entries.size()-1;
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
    switch (get_table_kind()) {
        case ILT_LOOKUP_TABLE:
            prefix = "    ...ILT";
            break;
        case ILT_ADDRESS_TABLE:
            prefix = "    ...IAT";
            break;
        //default: (omitted for compiler warnings)
    }

    char p[4096];
    if (idx>=0) {
        sprintf(p, "%s[%zd].", prefix, idx);
    } else {
        sprintf(p, "%s.", prefix);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %zu\n", p, w, "nentries", p_entries->get_vector().size());
    for (size_t i=0; i<p_entries->get_vector().size(); i++) {
        SgAsmPEImportILTEntry *ilt_entry = p_entries->get_vector()[i];
        ilt_entry->dump(f, p, i);
    }
}
