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
  /*tps 11/23/2010 : temp fix for stringifySgAsmPEImportLookupTableTableKind not defined under windows */
#ifdef _MSC_VER
  std::string tname = "";
  ROSE_ASSERT(false);
#else
    std::string tname = stringifySgAsmPEImportLookupTableTableKind(get_table_kind());
#endif
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

/** Adds an Import Lookup Table Entry (or Import Address Table Entry) to the Import Lookup Table (or Import Address Table). If
 *  an index is specified then the table is extended if necessary and all entries intervening between the old end-of-table
 *  and the newly added entry are initialized to null pointers. If an index was specified and the element already exists then
 *  it will be moved. If the destination already points to an entry then that entry will be unlinked from the AST.
 *
 *  If an index is not specified and the entry already exists in the table then its position becomes this method's return value
 *  and no other action is performed.  Otherwise, an unspecified index is the same as choosing an index which is one past the
 *  end of the table, thus extending the table by exactly one element.
 *
 *  An entry can be removed from the table by specifying a null @p ilt_entry along with an index.
 *
 *  The return value is the index where the entry was inserted (or already exists if @p idx is unspecified).
 *  previous location. */
size_t
SgAsmPEImportLookupTable::add_entry(SgAsmPEImportILTEntry *ilt_entry, size_t idx/*=-1*/)
{
    ROSE_ASSERT(get_entries()!=NULL);
    SgAsmPEImportILTEntryPtrList &entries = get_entries()->get_vector();
    
    /* If this entry is already present elsewhere then remove it. However, if the destination index is unspecified then return
     * the index instead. */
    if (ilt_entry!=NULL && ilt_entry->get_parent()==this) {
        for (size_t i=0; i<entries.size(); i++) {
            if (i!=idx && entries[i]==ilt_entry) {
                if (idx==(size_t)(-1))
                    return i;
                set_isModified(true);
                entries[i] = NULL;
            }
        }
    }

    /* Extend the table if necessary */
    if (idx==(size_t)-1)
        idx = entries.size();
    if (idx >= entries.size())
        entries.resize(idx+1, NULL);

    /* If there's already something else stored at the destination then unlink it from the AST. */
    if (entries[idx] && entries[idx]!=ilt_entry) {
        set_isModified(true);
        entries[idx]->set_parent(NULL);
        entries[idx] = NULL;
    }

    /* Insert new entry */
    if (entries[idx]==NULL) {
        set_isModified(true);
        entries[idx] = ilt_entry;
        ilt_entry->set_parent(this);
    }

    return idx;
}

/** Constructs an Import Lookup Table Entry (or Import Address Table Entry) that points to a name/hint, and adds it to the
 *  Import Lookup Table. Returns the table index where the entry was added. */
size_t
SgAsmPEImportLookupTable::add_name(const std::string &name, size_t hint, size_t idx/*=-1*/)
{
    SgAsmPEImportILTEntry *ilt_entry = new SgAsmPEImportILTEntry(NULL); /* do not add to table yet */
    new SgAsmPEImportHNTEntry(ilt_entry, name, hint); /* added to ilt_entry by side effect */
    return add_entry(ilt_entry, idx);
}

/** Constructs an Import Lookup Table Entry (or Import Address Table Entry) which contains the address of the symbol. */
size_t
SgAsmPEImportLookupTable::add_address(rose_rva_t rva, size_t idx/*=-1*/)
{
    SgAsmPEImportILTEntry *ilt_entry = new SgAsmPEImportILTEntry(NULL); /* do not add to table yet */
    ilt_entry->set_entry_type(SgAsmPEImportILTEntry::ILT_BOUND_RVA);
    ilt_entry->set_bound_rva(rva);
    return add_entry(ilt_entry, idx);
}

/** Allocates space for this import lookup table.  Space is allocated beginning at the specified @p start_rva. Space is
 *  (re)allocated for Hint/Name table entries if the entry is not allocated or if its rose_rva_t points to the same section as
 *  start_rva points to.  Returns the number of bytes allocated. */
size_t
SgAsmPEImportLookupTable::reallocate(rose_rva_t start_rva)
{
    rose_rva_t end_rva = start_rva;
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(this);
    ROSE_ASSERT(fhdr);
    
    SgAsmPEImportILTEntryPtrList &entries = get_entries()->get_vector();
    end_rva.increment(fhdr->get_word_size() + (entries.size() + 1)); /* zero terminated */

    /* Some ILT entries might point to Hint/Name pairs in the Hint/Name Table. The Hint/Name Table will be allocated
     * immediately after the ILT entries, if necessary. */
    for (size_t i=0; i<entries.size(); i++) {
        SgAsmPEImportILTEntry *ilt_entry = entries[i];
        if (SgAsmPEImportILTEntry::ILT_HNT_ENTRY_RVA==ilt_entry->get_entry_type()) {
            SgAsmPEImportHNTEntry *hnt_entry = ilt_entry->get_hnt_entry();
            if (0==ilt_entry->get_hnt_entry_rva().get_rva() ||
                ilt_entry->get_hnt_entry_rva().get_section()==end_rva.get_section()) {
                ilt_entry->set_hnt_entry_rva(end_rva);
                end_rva.increment(hnt_entry->encoded_size());
            }
        }
    }

    return end_rva.get_rva() - start_rva.get_rva();
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
