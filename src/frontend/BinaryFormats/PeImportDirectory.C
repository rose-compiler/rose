/* PE Import Directory. A PE Import Section is a list of PE Import Directories. */
#include "sage3basic.h"
#include "MemoryMap.h"

/** @class SgAsmPEImportDirectory
 *
 *  Portable Executable Import Directory.
 *
 *  Represents a single directory (usually corresponding to a single library) in the PE Import Section. See
 *  SgAsmPEImportSection for details. */




/** Constructor. The constructor makes @p section the parent of this new import directory, and adds this new import
 *  directory to the import section. */
void
SgAsmPEImportDirectory::ctor(SgAsmPEImportSection *section, const std::string &dll_name)
{
    ROSE_ASSERT(section!=NULL); /* needed for parsing */
    section->add_import_directory(this);
    p_time = time(NULL);

    p_dll_name = new SgAsmBasicString(dll_name);
    p_dll_name->set_parent(this);

    p_imports = new SgAsmPEImportItemList;
    p_imports->set_parent(this);
}

/** Number of bytes required for the table.  Returns the number of bytes required for the entire IAT or ILT (including the zero
 *  terminator) as it is currently defined in the Import Directory.  The returned size does not include space required to store
 *  any Hint/Name pairs, which are outside the ILT/IAT but pointed to by the ILT/IAT. */
size_t
SgAsmPEImportDirectory::iat_required_size() const
{
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(this);
    assert(fhdr!=NULL);
    return (p_imports->get_vector().size() + 1) * fhdr->get_word_size();
}

/** Find an import item in an import directory.  Returns the index of the specified import item in this directory, or -1 if the
 *  import item is not a child of this directory.  The hint index is checked first. */
int
SgAsmPEImportDirectory::find_import_item(const SgAsmPEImportItem *item, int hint) const
{
    const SgAsmPEImportItemPtrList &imports = get_imports()->get_vector();
    if (hint>=0 && (size_t)hint<imports.size() && imports[hint]==item)
        return hint;
    for (size_t i=0; i<imports.size(); ++i) {
        if (imports[i]==item)
            return i;
    }
    return -1;
}

/** Obtains the virtual address of the Hint/Name Table.  The Hint/Name Table is an implicit table--the PE file format
 *  specification talks about such a table, but it is not actually defined anywhere in the PE file.  Instead, various Import
 *  Lookup Table and Import Address Table entries might point to individual Hint/Name pairs, which collectively form an
 *  implicit Hint/Name Table.  There is no requirement that the Hint/Name pairs are contiguous in the address space, and indeed
 *  they often are not.  Therefore, the only way to describe the location of the Hint/Name Table is by a list of addresses.
 *
 *  This function will scan this Import Directory's import items, observe which items make references to Hint/Name pairs that
 *  have known addresses, and add those areas of virtual memory to the specified extent map.  This function returns the number
 *  of ILT entries that reference a Hint/Name pair. */
size_t
SgAsmPEImportDirectory::hintname_table_extent(ExtentMap &extent/*in,out*/) const
{
    size_t retval = 0;
    const SgAsmPEImportItemPtrList &imports = get_imports()->get_vector();
    for (SgAsmPEImportItemPtrList::const_iterator ii=imports.begin(); ii!=imports.end(); ++ii) {
        SgAsmPEImportItem *import = *ii;
        if (!import->get_by_ordinal() && import->get_hintname_rva().get_rva()!=0 && import->get_hintname_nalloc()>0) {
            size_t nbytes = std::min(import->get_hintname_nalloc(), import->hintname_required_size());
            extent.insert(Extent(import->get_hintname_rva().get_va(), nbytes));
            ++retval;
        }
    }
    return retval;
}

/** Parse an import directory. The import directory is parsed from the specified virtual address via the PE header's loader
 *  map. Return value is this directory entry on success, or the null pointer if the entry is all zero (which marks the end of
 *  the directory list). */
SgAsmPEImportDirectory *
SgAsmPEImportDirectory::parse(rose_addr_t idir_va)
{
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(this);
    ROSE_ASSERT(fhdr!=NULL);

    /* Read the import directory from disk via loader map. */
    PEImportDirectory_disk disk, zero;
    memset(&zero, 0, sizeof zero);
    size_t nread = fhdr->get_loader_map()->read(&disk, idir_va, sizeof disk);
    if (nread!=sizeof disk) {
        SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory::parse: short read (%zu byte%s) of directory "
                                          "at va 0x%08"PRIx64" (needed %zu bytes)\n",
                                          nread, 1==nread?"":"s", idir_va, sizeof disk);
        memset(&disk, 0, sizeof disk);
    }

    /* An all-zero entry marks the end of the list. In this case return null. */
    if (!memcmp(&disk, &zero, sizeof zero))
        return NULL;

    p_ilt_rva         = le_to_host(disk.ilt_rva);
    p_time            = le_to_host(disk.time);
    p_forwarder_chain = le_to_host(disk.forwarder_chain);
    p_dll_name_rva    = le_to_host(disk.dll_name_rva);
    p_iat_rva         = le_to_host(disk.iat_rva);

    /* Bind RVAs to best sections */
    p_ilt_rva.bind(fhdr);
    p_dll_name_rva.bind(fhdr);
    p_iat_rva.bind(fhdr);

    /* Library name.  The PE format specification does not require the name to be contained in the import section, but we issue
     * a warning because ROSE may have problems allocating new space for the name if it's changed. */
    ROSE_ASSERT(p_dll_name!=NULL);
    std::string dll_name;
    if (0==p_dll_name_rva.get_section()) {
        SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory::parse: import directory at va 0x%08"PRIx64
                                          " has bad DLL name rva %s\n", idir_va, p_dll_name_rva.to_string().c_str());
    } else {
        try {
            dll_name = p_dll_name_rva.get_section()->read_content_str(fhdr->get_loader_map(), p_dll_name_rva.get_va());
        } catch (const MemoryMap::NotMapped &e) {
            if (SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory::parse: short read of dll name at rva %s for "
                                                  " import directory at va 0x%08"PRIx64"\n",
                                                  p_dll_name_rva.to_string().c_str(), idir_va) &&
                e.map) {
                fprintf(stderr, "    Memory map in effect at time of error is:\n");
                e.map->dump(stderr, "        ");
            }
        }
    }
    p_dll_name->set_string(dll_name);
    p_dll_name_nalloc = dll_name.size() + 1;

    parse_ilt_iat(p_ilt_rva, false);
    p_ilt_nalloc = 0==p_ilt_rva.get_rva() ? 0 : iat_required_size();

    parse_ilt_iat(p_iat_rva, false); // IAT initially is a copy of the ILT (i.e., objects are not bound to addresses yet)
    p_iat_nalloc = 0==p_iat_rva.get_rva() ? 0 : iat_required_size();

    return this;
}

/** Parse an Import Lookup Table or Import Address Table.  The table begins at the specified address and consists of 32- or 64-
 *  bit vectors which are (1) an ordinal number with high order bit set, (2) a relative virtual address of a hint/name pair, or
 *  (3) a virtual address filled in by the dynamic linker when the shared object is bound.  The last case is assumed if @p
 *  @p assume_bound is set.
 *
 *  Normally, the ILT and IAT of an executable file have identical structure and content and the IAT is changed only after the
 *  shared objects are dynamically linked.  However, we might encounter cases where we discover that the IAT has values that
 *  are different than the ILT even when @p assume_bound is false.  When this happens, we emit a warning message and treat the
 *  conflicting IAT entry as a bound address. */
void
SgAsmPEImportDirectory::parse_ilt_iat(const rose_rva_t &table_start, bool assume_bound)
{
    SgAsmPEImportSection *isec = SageInterface::getEnclosingNode<SgAsmPEImportSection>(this);
    assert(isec!=NULL);
    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(isec->get_header());
    assert(fhdr!=NULL);
    assert(fhdr->get_word_size() <= sizeof(uint64_t));
    assert(get_imports()!=NULL);
    SgAsmPEImportItemPtrList &imports = get_imports()->get_vector();
    bool processing_iat = !imports.empty(); // we always process the ILT first (but it might be empty)
    
    if (0==table_start.get_rva())
        return;                 // no ILT/IAT present


    rose_addr_t entry_va=table_start.get_va(), entry_size=fhdr->get_word_size();
    uint64_t by_ordinal_bit = 1ull << (8*entry_size-1);

    for (size_t idx=0; 1; ++idx, entry_va+=entry_size) {
        /* Read the 32- or 64-bit ILT/IAT entry from proces mapped memory. */
        uint64_t entry_word = 0;
        try {
            isec->read_content(fhdr->get_loader_map(), entry_va, &entry_word, entry_size);
        } catch (const MemoryMap::NotMapped &e) {
            if (SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at VA 0x%08"PRIx64
                                                  ": table entry is outside mapped memory\n", idx, entry_va) && e.map) {
                fprintf(stderr, "    Memory map in effect at time of error:\n");
                e.map->dump(stderr, "        ");
            }
        }
        entry_word = le_to_host(entry_word);

        /* ILT/IAT are to be terminated by a zero word.  However, the length of the IAT is required to be the same as the
         * length of the ILT. Handle three cases here:
         *      (1) Termination of the ILT by a zero entry; stop now
         *      (2) Premature termination of the IAT; continue processing
         *      (3) Missing (or late) termination of the IAT; stop now */
        if (0==entry_word) {
            if (idx<imports.size()) {
                SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: IAT entry #%zu at va 0x%08"PRIx64
                                                  ": IAT zero termination occurs before end of corresponding ILT;"
                                                  " %scontinuing to read IAT entries.\n", idx, entry_va,
                                                  (assume_bound?"":"assuming this is a bound null address and "));
                assert(idx<imports.size());
                imports[idx]->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
                continue;
            } else {
                break;
            }
        } else if (processing_iat && idx>=imports.size()) {
            SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: IAT entry #%zu at va 0x%08"PRIx64
                                              ": IAT extends beyond end of corresponding ILT (it is not zero terminated)"
                                              "; forcibly terminating here\n", idx, entry_va);
            break;
        }

        /* Create the new table entry if necessary. */
        bool entry_existed = true;
        if (idx>=imports.size()) {
            assert(idx==imports.size());
            new SgAsmPEImportItem(get_imports()); // adds new item to imports list
            entry_existed = false;
        }
        SgAsmPEImportItem *import_item = imports[idx];

        if (assume_bound) {
            /* The entry word is a virtual address. */
            if (entry_existed && import_item->get_bound_rva().get_rva()>0 &&
                import_item->get_bound_rva().get_rva()!=entry_word) {
                SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                  ": bound address 0x%08"PRIx64" conflicts with bound address already"
                                                  " discovered 0x%08"PRIx64" (using new value)\n", idx, entry_va, entry_word,
                                                  import_item->get_bound_rva().get_rva());
            }
            import_item->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
        } else if (0!=(entry_word & by_ordinal_bit)) {
            /* The entry is an ordinal number. */
            if (entry_existed && import_item->get_ordinal()!=0 && import_item->get_ordinal()!=(entry_word & 0xffff)) {
                SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                  ": ordinal %"PRIu64" conflicts with ordinal already discovered %u"
                                                  " (assuming this is a bound address)\n", idx, entry_va, (entry_word&0xffff),
                                                  import_item->get_ordinal());
                import_item->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
            } else {
                import_item->set_by_ordinal(true);
                import_item->set_ordinal(entry_word & 0xffff);
                if (0!=(entry_word & ~(by_ordinal_bit | 0xffff))) {
                    SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                      ": contains extra bits in violation of PE specification"
                                                      " (extra bits removed)\n", idx, entry_va);
                }
            }
        } else {
            /* The entry word points to a Hint/Name pair: a 2-byte, little endian hint, followed by a NUL-terminated string,
             * followed by an optional extra byte to make the total length a multiple of two. */
            if (entry_existed && import_item->get_by_ordinal()) {
                SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                  ": entry type \"hint/name\" conflicts with entry type already discovered"
                                                  " \"ordinal\" (assuming this is a bound address)\n", idx, entry_va);
                import_item->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
            } else if (entry_existed && import_item->get_hintname_rva().get_rva()>0 &&
                       import_item->get_hintname_rva().get_rva()!=entry_word) {
                SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                  ": hint/name rva 0x%08"PRIx64" conflicts with hint/name rva already"
                                                  " discovered 0x%08"PRIx64" (assuming this is a bound address)\n",
                                                  idx, entry_va, entry_word, import_item->get_hintname_rva().get_rva());
                import_item->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
            } else {
                import_item->set_by_ordinal(false);
                import_item->set_hintname_rva(rose_rva_t(entry_word).bind(fhdr));
                import_item->set_hintname_nalloc(0); // for now, will adjust after we read it
                rose_addr_t entry_word_va = entry_word + fhdr->get_base_va();
                uint16_t hint;
                try {
                    isec->read_content(fhdr->get_loader_map(), entry_word_va, &hint, sizeof hint);
                } catch (const MemoryMap::NotMapped &e) {
                    import_item->set_hint(0);
                    import_item->get_name()->set_string("");
                    if (SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                          ": points to unmapped Hint/Name pair at va 0x%08"PRIx64
                                                          " (when reading hint)\n", idx, entry_va, entry_word_va) &&
                        e.map) {
                        fprintf(stderr, "    Memory map in effect at time of error:\n");
                        e.map->dump(stderr, "        ");
                    }
                    continue; // to next ILT/IAT entry
                }
                hint = le_to_host(hint);
                import_item->set_hint(hint);

                std::string s;
                try {
                    s = isec->read_content_str(fhdr->get_loader_map(), entry_word_va+2);
                } catch (const MemoryMap::NotMapped &e) {
                    import_item->get_name()->set_string("");
                    if (SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                          ": points to an unmapped Hint/Name pair at va 0x%08"PRIx64
                                                          " (when reading name)\n", idx, entry_va, entry_word_va) &&
                        e.map) {
                        fprintf(stderr, "    Memory map in effect at time of error:\n");
                        e.map->dump(stderr, "        ");
                    }
                    continue; // to next ILT/IAT entry
                }
                import_item->get_name()->set_string(s);

                if ((s.size()+1)  % 2) {
                    uint8_t byte = 0;
                    try {
                        isec->read_content(fhdr->get_loader_map(), entry_word_va+2+s.size()+1, &byte, 1);
                    } catch (const MemoryMap::NotMapped &e) {
                        if (SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                              ": points to an unmapped Hint/Name pair at va 0x%08"PRIx64
                                                              " (when reading pad byte)\n", idx, entry_va, entry_word_va) &&
                            e.map) {
                            fprintf(stderr, "    Memory map in effect at time of error:\n");
                            e.map->dump(stderr, "        ");
                        }
                        continue; // to next ILT/IAT entry
                    }
                    if (byte) {
                        SgAsmPEImportSection::import_mesg("SgAsmPEImportDirctory: ILT/IAT entry #%zu at va 0x%08"PRIx64
                                                          ": has a non-zero padding byte: 0x%02x\n", idx, entry_va, byte);
                    }
                }
                import_item->set_hintname_nalloc(import_item->hintname_required_size());
            }
        }
    }
}

/** Unparse a PE Import Lookup Table or Import Address Table.  The @p table_start address is the location where the ILT or IAT
 *  begins and should be bound to the section to which the table is being written.  The table is filled with ordinals and/or
 *  hint/name pairs unless @p assume_bound is set, in which case the table is filled with the bound addresses.  The PE spec
 *  requires that the IAT in the executable file has the exact same structure and content as the ILT, so this method is
 *  normally called wtih @p assume_bound set to false.
 *
 *  The @p tablesize argument is the number of bytes allocated for the table, and may be less than the number of bytes required
 *  to write the entire table.  This can happen, for instance, when the AST was modified by adding entries to the ILT/IAT but
 *  the size (and probably location) of the table could not be changed.  This is typical of IATs since code in the .text
 *  section often references IAT entries via indirect jumps/calls and it is infeasible to move the IAT to a new location.  If
 *  unparsing is unable to write all table entries due to the @p tablesize limit, an error message is printed. */
void
SgAsmPEImportDirectory::unparse_ilt_iat(std::ostream &f, const rose_rva_t &table_start, bool assume_bound,
                                        size_t tablesize) const
{
    SgAsmPEImportSection *isec = SageInterface::getEnclosingNode<SgAsmPEImportSection>(this);
    assert(isec!=NULL);
    SgAsmPEFileHeader *fhdr = isSgAsmPEFileHeader(isec->get_header());
    assert(fhdr!=NULL);
    assert(fhdr->get_word_size() <= sizeof(uint64_t));
    const SgAsmPEImportItemPtrList &imports = get_imports()->get_vector();
    size_t entry_size = fhdr->get_word_size();
    uint64_t by_ordinal_bit = 1ull << (8*entry_size-1);

    if (0==table_start.get_rva() || imports.empty())
        return;
    if (!table_start.get_section()) {
        SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT: table is not associated with a section: %s\n",
                                          table_start.to_string().c_str());
        return;
    }

    /* Must we limit how many entries are written? Don't forget the zero terminator. */
    size_t nelmts = std::min(tablesize/entry_size, imports.size()+1);
    if (nelmts<imports.size()+1) {
        SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT: table at 0x%08"PRIx64" is truncated from %zu to"
                                          " %zu entries (inc. zero terminator) due to allocation constraints.\n",
                                          table_start.to_string().c_str(), imports.size()+1, nelmts);
    }

    rose_rva_t entry_rva = table_start;
    for (size_t idx=0; idx<nelmts/*including zero terminator*/; ++idx, entry_rva.increment(entry_size)) {
        uint64_t entry_word = 0;

        /* Write the zero terminator? */
        if (idx+1==nelmts) {
            uint64_t zero = 0;
            entry_rva.get_section()->write(f, entry_rva.get_rel(), entry_size, &zero);
            break;
        }

        rose_rva_t hn_rva = imports[idx]->get_hintname_rva();

        /* Build the IAT/ILT entry */
        if (assume_bound) {
            entry_word = imports[idx]->get_bound_rva().get_rva();
        } else if (imports[idx]->get_by_ordinal()) {
            if (0!=(imports[idx]->get_ordinal() & ~0xffff)) {
                SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at rva 0x%08"PRIx64
                                                  ": ordinal is out of bounds: %u (truncated to 16 bits)\n",
                                                  idx, entry_rva.get_rva(), imports[idx]->get_ordinal());
            }
            entry_word |= by_ordinal_bit | (imports[idx]->get_ordinal() & 0xffff);
        } else if (0==hn_rva.get_rva() || NULL==hn_rva.get_section()) {
            SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at rva 0x%08"PRIx64
                                              ": non-ordinal entry has invalid hint/name pair address %s or is"
                                              " not associated with any section.\n",
                                              idx, entry_rva.get_rva(), hn_rva.to_string().c_str());
            entry_word = hn_rva.get_rva(); // best we can do
        } else {
            size_t bufsz = std::min(imports[idx]->get_hintname_nalloc(), imports[idx]->hintname_required_size());
            if (bufsz < imports[idx]->hintname_required_size()) {
                SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at rva 0x%08"PRIx64
                                                  ": insufficient space (%zu byte%s) allocated for hint/name pair at"
                                                  " rva %s (need %zu bytes)\n",
                                                  idx, entry_rva.get_rva(), bufsz, 1==bufsz?"":"s",
                                                  hn_rva.to_string().c_str(), imports[idx]->hintname_required_size());
            }
            if (bufsz>=2) {
                uint8_t *buf = new uint8_t[bufsz];
                unsigned hint = imports[idx]->get_hint();
                std::string name = imports[idx]->get_name()->get_string();
                if (0!=(hint & ~0xffff)) {
                    SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at rva 0x%08"PRIx64
                                                      ": hint is out of bounds: %u (truncated to 16 bits)\n",
                                                      idx, entry_rva.get_rva(), hint);
                }
                host_to_le(hint, &hint); // nudge, nudge. Know what I mean?
                memcpy(buf, &hint, 2);
                memcpy(buf+2, name.c_str(), std::min(name.size()+1, bufsz-2));
                if (bufsz>2)
                    buf[bufsz-1] = '\0';
                hn_rva.get_section()->write(f, hn_rva.get_rel(), bufsz, buf);
                if (0!=(hn_rva.get_rva() & by_ordinal_bit)) {
                    SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at rva 0x%08"PRIx64
                                                      ": hint/name pair rva 0x%08"PRIx64" has by_ordinal bit set\n",
                                                      idx, entry_rva.get_rva(), hn_rva.get_rva());
                }
                delete[] buf;
            }
            entry_word = hn_rva.get_rva() & ~by_ordinal_bit;
        }

        /* Write the IAT/ILT entry */
        if (0==entry_word) {
            SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: ILT/IAT entry #%zu at rva 0x%08"PRIx64
                                              ": zero table entry will cause table to be truncated when read\n",
                                              idx, entry_rva.get_rva());
        }
        uint64_t disk = 0; // we might write only the first few bytes
        host_to_le(entry_word, &disk);
        entry_rva.get_section()->write(f, entry_rva.get_rel(), entry_size, &disk);
    }
}
    
/** Encode an import directory entry back into disk format */
void *
SgAsmPEImportDirectory::encode(PEImportDirectory_disk *disk) const
{
    host_to_le(p_ilt_rva.get_rva(),      &(disk->ilt_rva));
    host_to_le(p_time,                   &(disk->time));
    host_to_le(p_forwarder_chain,        &(disk->forwarder_chain));
    host_to_le(p_dll_name_rva.get_rva(), &(disk->dll_name_rva));
    host_to_le(p_iat_rva.get_rva(),      &(disk->iat_rva));
    return disk;
}

/** Allocates space for this import directory's name, import lookup table, and import address table.  The items are allocated
 *  beginning at the specified relative virtual address. Items are reallocated if they are not allocated or if they are
 *  allocated in the same section to which start_rva points (the import section).   They are not reallocated if they already
 *  exist in some other section. The return value is the number of bytes allocated in the import section.  Upon return, this
 *  directory's address data members are initialized with possibly new values. */
size_t
SgAsmPEImportDirectory::reallocate(rose_rva_t start_rva)
{
    rose_rva_t end_rva = start_rva;
    const SgAsmPEImportItemPtrList &imports = get_imports()->get_vector();

    /* Allocate space for the name if it hasn't been allocated already; reallocate space if its allocated in the import
     * section.  Allocate space even if the name is the empty string. */
    if (0==p_dll_name_rva.get_rva() || p_dll_name_rva.get_section()==end_rva.get_section()) {
        p_dll_name_nalloc = get_dll_name()->get_string().size() + 1;
        p_dll_name_rva = end_rva;
        end_rva.increment(p_dll_name_nalloc);
    }

    /* Allocate space for the import lookup table if it hasn't been allocated yet. The table is terminated with a zero
     * entry (as is the IAT according to the spec even though the IAT size is implied by the ILT size). */
    if (0==p_ilt_rva.get_rva() || p_ilt_rva.get_section()==end_rva.get_section()) {
        p_ilt_nalloc = iat_required_size(); // ILT and IAT are always the same size
        p_ilt_rva = end_rva;
        end_rva.increment(p_ilt_nalloc);
    }

    /* Allocate space for the import address table if it hasn't been allocated yet.   Note, the import address table should
     * usually be allocated explicitly because it is referenced by the program's instructions. */
    if (0==p_iat_rva.get_rva() || p_iat_rva.get_section()==end_rva.get_section()) {
        p_iat_nalloc = iat_required_size();
        p_iat_rva = end_rva;
        end_rva.increment(p_iat_nalloc);
    }

    /* Allocate space for the Hint/Name pairs that haven't been allocated yet.  The ILT and IAT will both point to the same
     * hint/name pair when the IAT is a copy of the ILT.  When the IAT is unparsed as bound addresses, then space for the
     * hint/name pair is still needed because the ILT still points to it.  We allocate space even for hint/name pairs that are
     * zero and the empty string.  We don't reallocate hint/name pairs that are stored in a different file section (so be
     * careful that you don't increase the length of the name). */
    for (size_t i=0; i<imports.size(); ++i) {
        if (!imports[i]->get_by_ordinal() &&
            (0==imports[i]->get_hintname_rva() || imports[i]->get_hintname_rva().get_section()==end_rva.get_section())) {
            size_t sz = imports[i]->hintname_required_size();
            imports[i]->set_hintname_nalloc(sz);
            imports[i]->set_hintname_rva(end_rva);
            end_rva.increment(sz);
        }
    }

    return end_rva.get_rva() - start_rva.get_rva();
}

void
SgAsmPEImportDirectory::unparse(std::ostream &f, const SgAsmPEImportSection *section, size_t idx) const
{
    /* The DLL name */
    if (0==p_dll_name_rva.get_rva() || NULL==p_dll_name_rva.get_section()) {
        SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: DLL name address %s is invalid or not bound to"
                                          " any file section\n", p_dll_name_rva.to_string().c_str());
    } else if (p_dll_name->get_string().size()+1 > p_dll_name_nalloc) {
        SgAsmPEImportSection::import_mesg("SgAsmPEImportDirectory: insufficient space allocated (%zu byte%s) for DLL name"
                                          " (need %zu byte%s)\n", p_dll_name_nalloc, 1==p_dll_name_nalloc?"":"s",
                                          p_dll_name->get_string().size()+1, 1==p_dll_name->get_string().size()?"":"s");
    } else {
        uint8_t *buf = new uint8_t[p_dll_name_nalloc];
        memcpy(buf, p_dll_name->get_string().c_str(), p_dll_name->get_string().size());
        memset(buf+p_dll_name->get_string().size(), 0, p_dll_name_nalloc - p_dll_name->get_string().size());
        p_dll_name_rva.get_section()->write(f, p_dll_name_rva.get_rel(), p_dll_name_nalloc, buf);
        delete[] buf;
    }

    /* Import Lookup Table and Import Address Table (and indirectly, Hint/Name entries). According to the PE secification: "The
     * structure and content of the import address table are identical to those of the import lookup table, until the file is
     * bound." */
    if (p_ilt_rva>0)
        unparse_ilt_iat(f, p_ilt_rva, false, p_ilt_nalloc);
    if (p_iat_rva>0)
        unparse_ilt_iat(f, p_iat_rva, false, p_iat_nalloc);
    
    PEImportDirectory_disk disk;
    encode(&disk);
    section->write(f, idx*sizeof disk, sizeof disk, &disk);
}


/** Print debugging info */
void
SgAsmPEImportDirectory::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEImportDirectory[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEImportDirectory.", prefix);
    }

    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %s for %zu bytes", p, w, "dll_name_rva", p_dll_name_rva.to_string().c_str(), p_dll_name_nalloc);
    if (p_dll_name)
        fprintf(f, " \"%s\"", p_dll_name->get_string(true).c_str());
    fputc('\n', f);

    fprintf(f, "%s%-*s = %lu %s",        p, w, "time", (unsigned long)p_time, ctime(&p_time));
    fprintf(f, "%s%-*s = 0x%08x (%u)\n", p, w, "forwarder_chain", p_forwarder_chain, p_forwarder_chain);
    fprintf(f, "%s%-*s = %s for %zu bytes\n", p, w, "ilt_rva", p_ilt_rva.to_string().c_str(), p_ilt_nalloc);
    fprintf(f, "%s%-*s = %s for %zu bytes\n", p, w, "iat_rva", p_iat_rva.to_string().c_str(), p_iat_nalloc);
    fprintf(f, "%s%-*s = %zu\n",         p, w, "nentries", p_imports->get_vector().size());

    for (size_t i=0; i<p_imports->get_vector().size(); ++i) {
        SgAsmPEImportItem *import = p_imports->get_vector()[i];
        import->dump(f, p, i);
    }
}
