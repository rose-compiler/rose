/* PE Import Directory. A PE Import Section is a list of PE Import Directories. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "MemoryMap.h"
#include "Diagnostics.h"

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

/* Constructor. The constructor makes @p section the parent of this new import directory, and adds this new import
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

size_t
SgAsmPEImportDirectory::iat_required_size() const
{
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(this);
    assert(fhdr!=NULL);
    return (p_imports->get_vector().size() + 1) * fhdr->get_word_size();
}

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

size_t
SgAsmPEImportDirectory::hintname_table_extent(AddressIntervalSet &extent/*in,out*/) const
{
    size_t retval = 0;
    const SgAsmPEImportItemPtrList &imports = get_imports()->get_vector();
    for (SgAsmPEImportItemPtrList::const_iterator ii=imports.begin(); ii!=imports.end(); ++ii) {
        SgAsmPEImportItem *import = *ii;
        if (!import->get_by_ordinal() && import->get_hintname_rva().get_rva()!=0 && import->get_hintname_nalloc()>0) {
            size_t nbytes = std::min(import->get_hintname_nalloc(), import->hintname_required_size());
            extent.insert(AddressInterval::baseSize(import->get_hintname_rva().get_va(), nbytes));
            ++retval;
        }
    }
    return retval;
}

SgAsmPEImportDirectory *
SgAsmPEImportDirectory::parse(rose_addr_t idir_va, bool isLastEntry)
{
    SgAsmPEFileHeader *fhdr = SageInterface::getEnclosingNode<SgAsmPEFileHeader>(this);
    ROSE_ASSERT(fhdr!=NULL);

    /* Read the import directory from disk via loader map. */
    PEImportDirectory_disk disk, zero;
    memset(&zero, 0, sizeof zero);
    size_t nread = fhdr->get_loader_map()->readQuick(&disk, idir_va, sizeof disk);
    if (nread!=sizeof disk) {
        if (SgAsmPEImportSection::show_import_mesg()) {
            mlog[WARN] <<"SgAsmPEImportDirectory::parse: short read (" <<StringUtility::plural(nread, "bytes") <<")"
                       <<" of directory at va " <<StringUtility::addrToString(idir_va)
                       <<" (needed " <<StringUtility::plural(sizeof disk, "bytes") <<")\n";
        }
        memset(&disk, 0, sizeof disk);
    }

    /* An all-zero entry marks the end of the list. In this case return null. */
    if (!memcmp(&disk, &zero, sizeof zero))
        return NULL;
#if 0 // [Robb Matzke 2017-10-16]: this mechanism to find end-of-list is not documented by Microsoft and might be wrong.
    if (isLastEntry) {
        mlog[WARN] <<"SgAsmPEImportDirectory::parse: import directory at va " <<StringUtility::addrToString(idir_va)
                   <<" is last in section but has a non-zero value (pretending it's zero)\n";
        return NULL;
    }
#endif

    p_ilt_rva         = ByteOrder::le_to_host(disk.ilt_rva);
    p_time            = ByteOrder::le_to_host(disk.time);
    p_forwarder_chain = ByteOrder::le_to_host(disk.forwarder_chain);
    p_dll_name_rva    = ByteOrder::le_to_host(disk.dll_name_rva);
    p_iat_rva         = ByteOrder::le_to_host(disk.iat_rva);

    /* Bind RVAs to best sections */
    p_ilt_rva.bind(fhdr);
    p_dll_name_rva.bind(fhdr);
    p_iat_rva.bind(fhdr);

    /* Library name.  The PE format specification does not require the name to be contained in the import section, but we issue
     * a warning because ROSE may have problems allocating new space for the name if it's changed. */
    ROSE_ASSERT(p_dll_name!=NULL);
    std::string dll_name;
    if (0==p_dll_name_rva.get_section()) {
        if (SgAsmPEImportSection::show_import_mesg()) {
            mlog[WARN] <<"SgAsmPEImportDirectory::parse: import directory at va " <<StringUtility::addrToString(idir_va)
                       <<" has bad DLL name rva " <<p_dll_name_rva <<"\n";
        }
    } else {
        try {
            dll_name = p_dll_name_rva.get_section()->read_content_str(fhdr->get_loader_map(), p_dll_name_rva.get_va());
        } catch (const MemoryMap::NotMapped &e) {
            if (SgAsmPEImportSection::show_import_mesg()) {
                mlog[WARN] <<"SgAsmPEImportDirectory::parse: short read of dll name at rva "
                           << p_dll_name_rva <<" for import directory at va " <<StringUtility::addrToString(idir_va) <<"\n";
                if (e.map) {
                    mlog[WARN] <<"    Memory map in effect at time of error is:\n";
                    e.map->dump(mlog[WARN], "        ");
                }
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

/* Parse an Import Lookup Table or Import Address Table.  The table begins at the specified address and consists of 32- or 64-
 * bit vectors which are (1) an ordinal number with high order bit set, (2) a relative virtual address of a hint/name pair, or
 * (3) a virtual address filled in by the dynamic linker when the shared object is bound.  The last case is assumed if @p
 * @p assume_bound is set.
 *
 * Normally, the ILT and IAT of an executable file have identical structure and content and the IAT is changed only after the
 * shared objects are dynamically linked.  However, we might encounter cases where we discover that the IAT has values that
 * are different than the ILT even when @p assume_bound is false.  When this happens, we emit a warning message and treat the
 * conflicting IAT entry as a bound address. */
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
            if (SgAsmPEImportSection::show_import_mesg()) {
                mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                           <<" at va " <<StringUtility::addrToString(entry_va)
                           <<": table entry is outside mapped memory\n";
                if (e.map) {
                    mlog[WARN] <<"    Memory map in effect at time of error:\n";
                    e.map->dump(mlog[WARN], "        ");
                }
            }
        }
        entry_word = ByteOrder::le_to_host(entry_word);

        /* ILT/IAT are to be terminated by a zero word.  However, the length of the IAT is required to be the same as the
         * length of the ILT. Handle three cases here:
         *      (1) Termination of the ILT by a zero entry; stop now
         *      (2) Premature termination of the IAT; continue processing
         *      (3) Missing (or late) termination of the IAT; stop now */
        if (0==entry_word) {
            if (idx<imports.size()) {
                if (SgAsmPEImportSection::show_import_mesg()) {
                    mlog[ERROR] <<"SgAsmPEImportDirectory: IAT entry #" <<idx
                                <<" at va " <<StringUtility::addrToString(entry_va)
                                <<": IAT zero termination occurs before end of corresponding ILT;"
                                <<(assume_bound?"":"assuming this is a bound null address and ")
                                <<" continuing to read IAT entries.\n";
                }
                assert(idx<imports.size());
                imports[idx]->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
                continue;
            } else {
                break;
            }
        } else if (processing_iat && idx>=imports.size()) {
            if (SgAsmPEImportSection::show_import_mesg()) {
                mlog[WARN] <<"SgAsmPEImportDirectory: IAT entry #" <<idx
                           <<" at va " <<StringUtility::addrToString(entry_va)
                           <<": IAT extends beyond end of corresponding ILT (it is not zero terminated)"
                           <<"; forcibly terminating here\n";
            }
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
                if (SgAsmPEImportSection::show_import_mesg()) {
                    mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                               <<" at va " <<StringUtility::addrToString(entry_va)
                               <<": bound address " <<StringUtility::addrToString(entry_word)
                               <<" conflicts with bound address already discovered "
                               <<import_item->get_bound_rva().get_rva() <<" (using new value)\n";
                }
            }
            import_item->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
        } else if (0!=(entry_word & by_ordinal_bit)) {
            /* The entry is an ordinal number. */
            if (entry_existed && import_item->get_ordinal()!=0 && import_item->get_ordinal()!=(entry_word & 0xffff)) {
                if (SgAsmPEImportSection::show_import_mesg()) {
                    mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                               <<" at va " <<StringUtility::addrToString(entry_va)
                               <<": ordinal " <<(entry_word & 0xffff) <<" conflicts with ordinal already discovered "
                               <<import_item->get_ordinal()
                               <<" (assuming this is a bound address)\n";
                }
                import_item->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
            } else {
                import_item->set_by_ordinal(true);
                import_item->set_ordinal(entry_word & 0xffff);
                if (0!=(entry_word & ~(by_ordinal_bit | 0xffff))) {
                    if (SgAsmPEImportSection::show_import_mesg()) {
                        mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                                   <<" at va " <<StringUtility::addrToString(entry_va)
                                   <<": contains extra bits in violation of PE specification"
                                   <<" (extra bits removed)\n";
                    }
                }
            }
        } else {
            /* The entry word points to a Hint/Name pair: a 2-byte, little endian hint, followed by a NUL-terminated string,
             * followed by an optional extra byte to make the total length a multiple of two. */
            if (entry_existed && import_item->get_by_ordinal()) {
                if (SgAsmPEImportSection::show_import_mesg()) {
                    mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                               <<" at va " <<StringUtility::addrToString(entry_va)
                               <<": entry type \"hint/name\" conflicts with entry type already discovered"
                               <<" \"ordinal\" (assuming this is a bound address)\n";
                }
                import_item->set_bound_rva(rose_rva_t(entry_word).bind(fhdr));
            } else if (entry_existed && import_item->get_hintname_rva().get_rva()>0 &&
                       import_item->get_hintname_rva().get_rva()!=entry_word) {
                if (SgAsmPEImportSection::show_import_mesg()) {
                    mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                               <<" at va " <<StringUtility::addrToString(entry_va)
                               <<": hint/name rva " <<entry_word
                               <<" conflicts with hint/name rva already discovered "
                               <<import_item->get_hintname_rva().get_rva()
                               <<" (assuming this is a bound address)\n";
                }
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
                    if (SgAsmPEImportSection::show_import_mesg()) {
                        mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                                   <<" at va " <<StringUtility::addrToString(entry_va)
                                   <<": points to unmapped Hint/Name pair at va "
                                   <<StringUtility::addrToString(entry_word_va) <<"\n";
                        if (e.map) {
                            mlog[WARN] <<"    Memory map in effect at time of error:\n";
                            e.map->dump(mlog[WARN], "        ");
                        }
                    }
                    continue; // to next ILT/IAT entry
                }
                hint = ByteOrder::le_to_host(hint);
                import_item->set_hint(hint);

                std::string s;
                try {
                    s = isec->read_content_str(fhdr->get_loader_map(), entry_word_va+2);
                } catch (const MemoryMap::NotMapped &e) {
                    import_item->get_name()->set_string("");
                    if (SgAsmPEImportSection::show_import_mesg()) {
                        mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                                   <<" at va " <<StringUtility::addrToString(entry_va)
                                   <<": points to an unmapped Hint/Name pair at va "
                                   <<StringUtility::addrToString(entry_word_va) <<"\n";
                        if (e.map) {
                            mlog[WARN] <<"    Memory map in effect at time of error:\n";
                            e.map->dump(mlog[WARN], "        ");
                        }
                    }
                    continue; // to next ILT/IAT entry
                }
                import_item->get_name()->set_string(s);

                if ((s.size()+1)  % 2) {
                    uint8_t byte = 0;
                    try {
                        isec->read_content(fhdr->get_loader_map(), entry_word_va+2+s.size()+1, &byte, 1);
                    } catch (const MemoryMap::NotMapped &e) {
                        if (SgAsmPEImportSection::show_import_mesg()) {
                            mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                                       <<" at va " <<StringUtility::addrToString(entry_va)
                                       <<": points to an unmapped Hint/Name pair at va "
                                       <<StringUtility::addrToString(entry_word_va)
                                       <<" (when reading pad byte)\n";
                            if (e.map) {
                                mlog[WARN] <<"    Memory map in effect at time of error:\n";
                                e.map->dump(mlog[WARN], "        ");
                            }
                        }
                        continue; // to next ILT/IAT entry
                    }
                    if (byte) {
                        if (SgAsmPEImportSection::show_import_mesg()) {
                            mlog[WARN] <<"SgAsmPEImportDirctory: ILT/IAT entry #" <<idx
                                       <<" at va " <<StringUtility::addrToString(entry_va)
                                       <<": has a non-zero padding byte: "
                                       <<StringUtility::toHex2(byte, 8) <<"\n";
                        }
                    }
                }
                import_item->set_hintname_nalloc(import_item->hintname_required_size());
            }
        }
    }
}

/* Unparse a PE Import Lookup Table or Import Address Table.  The @p table_start address is the location where the ILT or IAT
 * begins and should be bound to the section to which the table is being written.  The table is filled with ordinals and/or
 * hint/name pairs unless @p assume_bound is set, in which case the table is filled with the bound addresses.  The PE spec
 * requires that the IAT in the executable file has the exact same structure and content as the ILT, so this method is
 * normally called wtih @p assume_bound set to false.
 *
 * The @p tablesize argument is the number of bytes allocated for the table, and may be less than the number of bytes required
 * to write the entire table.  This can happen, for instance, when the AST was modified by adding entries to the ILT/IAT but
 * the size (and probably location) of the table could not be changed.  This is typical of IATs since code in the .text
 * section often references IAT entries via indirect jumps/calls and it is infeasible to move the IAT to a new location.  If
 * unparsing is unable to write all table entries due to the @p tablesize limit, an error message is printed. */
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
        if (SgAsmPEImportSection::show_import_mesg()) {
            mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT: table is not associated with a section: "
                       <<table_start.to_string() <<"\n";
        }
        return;
    }

    /* Must we limit how many entries are written? Don't forget the zero terminator. */
    size_t nelmts = std::min(tablesize/entry_size, imports.size()+1);
    if (nelmts<imports.size()+1) {
        if (SgAsmPEImportSection::show_import_mesg()) {
            mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT: table at " <<table_start.to_string()
                       <<" is truncated from " <<StringUtility::plural(imports.size()+1, "entries", "entry")
                       <<" to " <<StringUtility::plural(nelmts, "entries", "entry")
                       <<" (inc. zero terminator) due to allocation constraints.\n";
        }
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
                if (SgAsmPEImportSection::show_import_mesg()) {
                    mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                               <<" at rva " <<entry_rva.get_rva()
                                <<": ordinal is out of bounds: " <<imports[idx]->get_ordinal() <<" (truncated to 16 bits)\n";
                }
            }
            entry_word |= by_ordinal_bit | (imports[idx]->get_ordinal() & 0xffff);
        } else if (0==hn_rva.get_rva() || NULL==hn_rva.get_section()) {
            if (SgAsmPEImportSection::show_import_mesg()) {
                mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                           <<" at rva " <<entry_rva.get_rva()
                           <<": non-ordinal entry has invalid hint/name pair address " <<hn_rva
                           <<" or is not associated with any section.\n";
            }
            entry_word = hn_rva.get_rva(); // best we can do
        } else {
            size_t bufsz = std::min(imports[idx]->get_hintname_nalloc(), imports[idx]->hintname_required_size());
            if (bufsz < imports[idx]->hintname_required_size()) {
                if (SgAsmPEImportSection::show_import_mesg()) {
                    mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                               <<" at rva " <<entry_rva.get_rva()
                               <<": insufficient space (" <<StringUtility::plural(bufsz, "bytes") <<")"
                               <<" allocated for hint/name pair at rva " <<hn_rva
                               <<" (need " <<StringUtility::plural(imports[idx]->hintname_required_size(), "bytes") <<")\n";
                }
            }
            if (bufsz>=2) {
                std::vector<uint8_t> buf(bufsz, 0);
                unsigned hint = imports[idx]->get_hint();
                std::string name = imports[idx]->get_name()->get_string();
                if (0!=(hint & ~0xffff)) {
                    if (SgAsmPEImportSection::show_import_mesg()) {
                        mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                                   <<" at rva " <<entry_rva.get_rva()
                                   <<": hint is out of bounds: " <<hint <<" (truncated to 16 bits)\n";
                    }
                }
                ByteOrder::host_to_le(hint, &hint); // nudge, nudge. Know what I mean?
                memcpy(&buf[0], &hint, 2);
                memcpy(&buf[2], name.c_str(), std::min(name.size()+1, bufsz-2));
                if (bufsz>2)
                    buf[bufsz-1] = '\0';
                hn_rva.get_section()->write(f, hn_rva.get_rel(), bufsz, &buf[0]);
                if (0!=(hn_rva.get_rva() & by_ordinal_bit)) {
                    if (SgAsmPEImportSection::show_import_mesg()) {
                        mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                                   <<" at rva " <<entry_rva.get_rva()
                                   <<": hint/name pair rva " <<hn_rva.get_rva()
                                   <<" has by_ordinal bit set\n";
                    }
                }
            }
            entry_word = hn_rva.get_rva() & ~by_ordinal_bit;
        }

        /* Write the IAT/ILT entry */
        if (0==entry_word) {
            if (SgAsmPEImportSection::show_import_mesg()) {
                mlog[WARN] <<"SgAsmPEImportDirectory: ILT/IAT entry #" <<idx
                           <<" at rva " <<entry_rva.get_rva()
                           <<": zero table entry will cause table to be truncated when read\n";
            }
        }
        uint64_t disk = 0; // we might write only the first few bytes
        ByteOrder::host_to_le(entry_word, &disk);
        entry_rva.get_section()->write(f, entry_rva.get_rel(), entry_size, &disk);
    }
}

void *
SgAsmPEImportDirectory::encode(PEImportDirectory_disk *disk) const
{
    ByteOrder::host_to_le(p_ilt_rva.get_rva(),      &(disk->ilt_rva));
    ByteOrder::host_to_le(p_time,                   &(disk->time));
    ByteOrder::host_to_le(p_forwarder_chain,        &(disk->forwarder_chain));
    ByteOrder::host_to_le(p_dll_name_rva.get_rva(), &(disk->dll_name_rva));
    ByteOrder::host_to_le(p_iat_rva.get_rva(),      &(disk->iat_rva));
    return disk;
}

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
        if (SgAsmPEImportSection::show_import_mesg()) {
            mlog[WARN] <<"SgAsmPEImportDirectory: DLL name address " <<p_dll_name_rva
                       <<" is invalid or not bound to any file section\n";
        }
    } else if (p_dll_name->get_string().size()+1 > p_dll_name_nalloc) {
        if (SgAsmPEImportSection::show_import_mesg()) {
            mlog[WARN] <<"SgAsmPEImportDirectory: insufficient space allocated ("
                       <<StringUtility::plural(p_dll_name_nalloc, "bytes") <<")"
                       <<" for DLL name (need " <<StringUtility::plural(p_dll_name->get_string().size()+1, "bytes") <<")\n";
        }
    } else if (p_dll_name_nalloc > 0) {
        std::vector<uint8_t> buf(p_dll_name_nalloc, 0);
        memcpy(&buf[0], p_dll_name->get_string().c_str(), p_dll_name->get_string().size());
        p_dll_name_rva.get_section()->write(f, p_dll_name_rva.get_rel(), p_dll_name_nalloc, &buf[0]);
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

    fprintf(f, "%s%-*s = %s for %" PRIuPTR " bytes", p, w, "dll_name_rva", p_dll_name_rva.to_string().c_str(), p_dll_name_nalloc);
    if (p_dll_name)
        fprintf(f, " \"%s\"", p_dll_name->get_string(true).c_str());
    fputc('\n', f);

    fprintf(f, "%s%-*s = %lu %s",        p, w, "time", (unsigned long)p_time, ctime(&p_time));
    fprintf(f, "%s%-*s = 0x%08x (%u)\n", p, w, "forwarder_chain", p_forwarder_chain, p_forwarder_chain);
    fprintf(f, "%s%-*s = %s for %" PRIuPTR " bytes\n", p, w, "ilt_rva", p_ilt_rva.to_string().c_str(), p_ilt_nalloc);
    fprintf(f, "%s%-*s = %s for %" PRIuPTR " bytes\n", p, w, "iat_rva", p_iat_rva.to_string().c_str(), p_iat_nalloc);
    fprintf(f, "%s%-*s = %" PRIuPTR "\n",         p, w, "nentries", p_imports->get_vector().size());

    for (size_t i=0; i<p_imports->get_vector().size(); ++i) {
        SgAsmPEImportItem *import = p_imports->get_vector()[i];
        import->dump(f, p, i);
    }
}

#endif
