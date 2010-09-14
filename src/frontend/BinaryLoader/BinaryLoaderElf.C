#include "sage3basic.h"
#include "BinaryLoaderElf.h"

#include <fstream>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* This binary loader can handle all ELF files. */
bool
BinaryLoaderElf::can_load(SgAsmGenericHeader *hdr) const
{
    return isSgAsmElfFileHeader(hdr)!=NULL;
}

/* Sorter for sections returned by get_remap_sections() */
static bool
section_cmp(SgAsmGenericSection *_a, SgAsmGenericSection *_b)
{
    SgAsmElfSection *a = isSgAsmElfSection(_a);
    SgAsmElfSection *b = isSgAsmElfSection(_b);
    ROSE_ASSERT(a!=NULL && b!=NULL);
    SgAsmElfSegmentTableEntry *a_seg = a->get_segment_entry();
    SgAsmElfSegmentTableEntry *b_seg = b->get_segment_entry();
    if (a_seg && b_seg) {
#if 0
        /* Sort ELF Segments by position in ELF Segment Table */
        return a_seg->get_index() < b_seg->get_index();
#else
        /* Sort ELF Segments by preferred virtual address */
        return a->get_mapped_preferred_rva() < b->get_mapped_preferred_rva();
#endif
    } else if (a_seg) {
        /* ELF Segments come before ELF Sections */
        return true;
    } else {
        /* Sort ELF Sections by position in ELF Section Table */
        return a->get_id() < b->get_id();
    }
}

SgAsmGenericSectionPtrList
BinaryLoaderElf::get_remap_sections(SgAsmGenericHeader *header)
{
    SgAsmGenericSectionPtrList retval = header->get_mapped_sections();
    std::sort(retval.begin(), retval.end(), section_cmp);
    return retval;
}

/* For any given file header, start mapping at a particular location in the address space. */
rose_addr_t
BinaryLoaderElf::rebase(MemoryMap *map, SgAsmGenericHeader *header, const SgAsmGenericSectionPtrList &sections)
{

    /* Find the minimum address desired by the sections to be mapped */
    rose_addr_t min_preferred_rva = (uint64_t)(-1);
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si)
        min_preferred_rva = std::min(min_preferred_rva, (*si)->get_mapped_preferred_rva());
    rose_addr_t min_preferred_va = header->get_base_va() + min_preferred_rva;

    /* Minimum address at which to map */
    rose_addr_t map_base_va = ALIGN_UP(map->find_last_free(), 4096);

    /* If the minimum preferred virtual address is less than the floor of the mapping area then we should add the difference
     * to the base virtual address, effectively moving the preferred mapping of all the sections up by that difference. */
    return min_preferred_va<map_base_va ? map_base_va-min_preferred_va : header->get_base_va();
}

BinaryLoader::MappingContribution
BinaryLoaderElf::align_values(SgAsmGenericSection *_section, MemoryMap *map,
                              rose_addr_t *malign_lo_p, rose_addr_t *malign_hi_p,
                              rose_addr_t *va_p, rose_addr_t *mem_size_p,
                              rose_addr_t *offset_p, rose_addr_t *file_size_p,
                              rose_addr_t *va_offset_p, bool *anon_lo_p, bool *anon_hi_p,
                              ConflictResolution *resolve_p)
{
    SgAsmElfSection *section = isSgAsmElfSection(_section);

    /* ELF Segments are aligned using the superclass, but when the section has a low- or high-padding area we'll use file
     * contents for the low area and zeros for the high area. Due to our rebase() method, there should be no conflicts between
     * this header's sections and sections previously mapped from other headers.  Therefore, any conflicts are within a single
     * header and are resolved by over-mapping. */
    if (section->get_segment_entry()) {


        MappingContribution retval = BinaryLoader::align_values(section, map, malign_lo_p, malign_hi_p, va_p,
                                                                mem_size_p, offset_p, file_size_p, va_offset_p,
                                                                anon_lo_p, anon_hi_p, resolve_p);
        *anon_lo_p = false;
        *anon_hi_p = true;
        *resolve_p = RESOLVE_OVERMAP;
        return retval;
    }

    /* ELF Sections are often used to refine areas of a segment.  A real loader ignores ELF Sections, but the refinement can
     * be useful for analyses.  Therefore, we'll allow ELF Sections to contribute to the mapping.  Alignment of ELF Sections
     * is temporarily set to none (because they live at arbitrary offsets in ELF Segments). If the section lives in a segment
     * that has been mapped to an address other than its preferred address, then we must make sure the ELF Section is also
     * similarly moved along with the ELF Segment. */
    SgAsmElfSection *part_of = NULL; /*what segment is this section a part of (if any)? */
    SgAsmGenericSectionPtrList sections = section->get_header()->get_mapped_sections();
    for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); ++si) {
        SgAsmElfSection *segment = isSgAsmElfSection(*si);
        if (segment && segment->get_segment_entry() &&
            section->get_mapped_preferred_rva() >= segment->get_mapped_preferred_rva() &&
            section->get_mapped_preferred_rva() < segment->get_mapped_preferred_rva() + segment->get_mapped_size()) {
            part_of = segment;
            break;
        }
    }
    rose_addr_t diff = part_of ? part_of->get_mapped_actual_va() - part_of->get_mapped_preferred_va() : 0;
    *malign_hi_p = *malign_lo_p = 1; /*no alignment constraint*/
    *va_p = section->get_header()->get_base_va() + section->get_mapped_preferred_rva() + diff;
    *mem_size_p = section->get_mapped_size();
    *offset_p = section->get_offset();
    *file_size_p = section->get_size();
    *va_offset_p = 0;
    *anon_lo_p = *anon_hi_p = true;
    *resolve_p = RESOLVE_OVERMAP;       /*erase (part of) the previous ELF Segment's memory */
    return CONTRIBUTE_ADD;
}

/* class method */
void
BinaryLoaderElf::get_dynamic_vars(SgAsmGenericHeader *hdr, std::string &rpath/*out*/, std::string &runpath/*out*/)
{
    rpath = runpath = "";

    SgAsmElfDynamicSection *dynamic = isSgAsmElfDynamicSection(hdr->get_section_by_name(".dynamic"));
    if (dynamic) {
        SgAsmElfDynamicEntryPtrList& entries = dynamic->get_entries()->get_entries();
        for (size_t i=0; i<entries.size(); ++i) {
            if (SgAsmElfDynamicEntry::DT_RPATH == entries[i]->get_d_tag()) {
                rpath = entries[i]->get_d_val().to_string();
            } else if (SgAsmElfDynamicEntry::DT_RUNPATH == entries[i]->get_d_tag()) {
                runpath = entries[i]->get_d_val().to_string();
            }
        }
    }
}

void
BinaryLoaderElf::add_lib_defaults(SgAsmGenericHeader *hdr/*=NULL*/)
{
    /* The LD_PRELOAD environment variable may contain space-separated library names */
    const char* ld_preload_env = getenv("LD_PRELOAD");
    if (ld_preload_env) {
        std::string s = ld_preload_env;
        boost::regex re;
        re.assign("\\s+");
        boost::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            add_preload(*iter);
    }

    /* Add the DT_RPATH directories to the search path (only if no DT_RUNPATH). Use of DT_RPATH is deprecated. */
    std::string rpath, runpath;
    get_dynamic_vars(hdr, rpath, runpath);
    if (!rpath.empty() && runpath.empty()) {
        boost::regex re;
        re.assign("[:;]");
        boost::sregex_token_iterator iter(rpath.begin(), rpath.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            add_directory(*iter);
    }
    
    /* Add the paths from the LD_LIBRARY_PATH environment variable */
    const char *ld_library_path_env = getenv("LD_LIBRARY_PATH");
    if (ld_library_path_env) {
        std::string s = ld_library_path_env;
        boost::regex re;
        re.assign("[:;]");
        boost::sregex_token_iterator iter(s.begin(), s.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            add_directory(*iter);
    }
    
    /* Add paths from the .dynamic DT_RUNPATH variable */
    if (!runpath.empty()) {
        boost::regex re;
        re.assign("[:;]");
        boost::sregex_token_iterator iter(runpath.begin(), runpath.end(), re, -1);
        boost::sregex_token_iterator iterEnd;
        for (; iter!=iterEnd; ++iter)
            add_directory(*iter);
    }
    
    /* Add architecture-specific libraries */
    if (hdr) {
        switch (hdr->get_isa() & SgAsmGenericHeader::ISA_FAMILY_MASK) {
            case SgAsmGenericHeader::ISA_X8664_Family:
                add_directory("/lib64");
                add_directory("/usr/lib64");
                break;
            case SgAsmGenericHeader::ISA_IA32_Family:
                add_directory("/lib32");
                add_directory("/usr/lib32");
                break;
            default:
                /*none*/
                break;
        }
    }

    /* Add system library locations */
    add_directory("/lib");
    add_directory("/usr/lib");
}

/* Reference Elf TIS Portal Formats Specification, Version 1.1 */
void
BinaryLoaderElf::fixup(SgAsmInterpretation *interp)
{
    typedef SymbolMap::BaseMap BaseMap;
    typedef std::vector<std::pair<SgAsmGenericSection*, BaseMap*> > SymbolMapList;


    // build map for each symbol table map<std::string,SgElfSymbol>
    // compute global symbol resolution
    //   in the DT_SYMBOLIC case, start w/ local map(s) before proceeding
    //
    // build map from SgAsmGenericSection to its symbolmap map<SgAsmGenericSection*,SymbolMap>
    // biuld list of

    /* Build a mapping (symbolMaps) from each ".dynsym" section (one per file header) to a SymbolMap::BaseMap. The BaseMap
     * holds the symbols defined (i.e., not local, hidden, or only referenced) by the section. */
    SgAsmGenericHeaderPtrList& headers = interp->get_headers()->get_headers();
    SymbolMapList symbolMaps; /*maps each .dynsym section to a SymbolMap::BaseMap object. */
    for (size_t h=0; h < headers.size(); ++h) {
        SgAsmGenericHeader* header = headers[h];
        if (get_debug())
            fprintf(get_debug(), "BinaryLoaderElf: building symbol map for %s...\n", header->get_file()->get_name().c_str());
        SymverResolver versionResolver(header);

        /* There must be zero or one ELF Section named ".dynsym" under this header.  If the section does not exist then the
         * header is not dynamically linked and neither provides nor requires dynamic symbol resolution.
         * 
         * Manual 1-10 under SHT_SYMTAB and SHT_DYNSYM: ...SHT_DYNSYM section holds a minimal set of dynamic linking symbols.
         * FIXME: Technically, the dynsym may be omitted, and we should truly use the .dynamic section's DT_SYMTAB entry(ies) */
        SgAsmElfSymbolSection *dynsym = isSgAsmElfSymbolSection(header->get_section_by_name(".dynsym"));
        if (!dynsym) continue;
        ROSE_ASSERT(dynsym->get_section_entry());
        ROSE_ASSERT(SgAsmElfSectionTableEntry::SHT_DYNSYM == dynsym->get_section_entry()->get_sh_type());

        /* Add each symbol definition to the BaseMap and then add the BaseMap to the symbolMaps */
        BaseMap* symbolMap = new BaseMap;
        for (size_t symbol_idx=0; symbol_idx<dynsym->get_symbols()->get_symbols().size(); symbol_idx++) {
            if (0==symbol_idx) continue; /*this must be undefined, so we can skip it*/

            SgAsmElfSymbol* symbol = dynsym->get_symbols()->get_symbols()[symbol_idx];
            if (get_debug()) fprintf(get_debug(), "  symbol [%zu] \"%s\" ", symbol_idx, symbol->get_name()->c_str());

            VersionedSymbol symver = versionResolver.get_versioned_symbol(symbol);
            if (symver.is_local()) {
                /* Symbols in the dynsym should never be local (if this is isn't the case, we can ignore them). Symbol
                 * versioning also may make symbols "local". */
                if (get_debug()) fprintf(get_debug(), " local (ignoring)\n");
            } else if (symver.is_hidden()) {
                /* Symbol versioning can result in 'hidden' symbols that should be ignored. */
                if (get_debug()) fprintf(get_debug(), " hidden (ignoring)\n");
            } else if (symver.is_reference()) {
                /* Symbol versioning lets us determine which symbols are 'references' i.e.  that are only used to help
                 * relocating */     
                if (get_debug()) fprintf(get_debug(), " reference (ignoring)\n");
            } else {
                std::string symName = symver.get_name();
                ROSE_ASSERT(symName==symbol->get_name()->get_string());
                SymbolMapEntry &symbolEntry = (*symbolMap)[symName];
                symbolEntry.addVersion(symver);
                if (get_debug()) fputs(" added.\n", get_debug());
            }
        }
        symbolMaps.push_back(std::make_pair(dynsym, symbolMap));
    }

    /* Construct a masterSymbolMap by merging all the definitions from the symbolMaps we created above. */
    if (get_debug()) fprintf(get_debug(), "BinaryLoaderElf: resolving dynamic symbols...\n");
    SymbolMap masterSymbolMap;
    BaseMap& masterSymbolMapBase=masterSymbolMap.get_base_map();
    for (SymbolMapList::const_iterator smi=symbolMaps.begin(); smi!=symbolMaps.end(); ++smi) {
        const BaseMap *symbolMap = smi->second;
        for(BaseMap::const_iterator newSymbolIter=symbolMap->begin(); newSymbolIter!=symbolMap->end(); ++newSymbolIter) {
            const SymbolMapEntry &newEntry = newSymbolIter->second;
            const std::string symName = newSymbolIter->first;
            BaseMap::iterator oldSymbolIter=masterSymbolMapBase.find(symName);

            if (oldSymbolIter == masterSymbolMapBase.end()) {
                /* no symbol yet, set it (this is the most common case) */
                masterSymbolMapBase[symName] = newSymbolIter->second;
            } else {
                /* We have to go through version by version to 'merge' a complete map */
                oldSymbolIter->second.merge(newEntry);
            }
        }
    }
    if (get_debug()) {
        for (BaseMap::const_iterator bmi=masterSymbolMapBase.begin(); bmi!=masterSymbolMapBase.end(); ++bmi) {
            const std::string name = bmi->first;
            const SymbolMapEntry &entry = bmi->second;
            fprintf(get_debug(), "  %s\n", entry.get_vsymbol().dump_versioned_name().c_str());
        }
    }

    /* Perform relocations on all mapped sections of all the headers of this interpretation */
    if (get_debug()) fprintf(get_debug(), "BinaryLoaderElf: performing relocation fixups...\n");
    SgAsmElfSectionPtrList mappedSections;
    for (size_t h=0; h < headers.size(); ++h) {
        SgAsmElfFileHeader* elfHeader = isSgAsmElfFileHeader(headers[h]);
        ROSE_ASSERT(elfHeader != NULL);
        SgAsmGenericSectionPtrList sections = elfHeader->get_sectab_sections();
        for (size_t i=0; i<sections.size(); ++i) {
            if (sections[i]->is_mapped()) {
                ROSE_ASSERT(NULL != isSgAsmElfSection(sections[i]));
                mappedSections.push_back(isSgAsmElfSection(sections[i]));
            }
        }
    }
    //std::sort(mappedSections.begin(),mappedSections.end(),extentSorterActualVA);
    for (size_t h=0; h<headers.size(); ++h) {
        SgAsmElfFileHeader* elfHeader = isSgAsmElfFileHeader(headers[h]);
        ROSE_ASSERT(NULL != elfHeader);
        performRelocations(elfHeader, mappedSections, masterSymbolMap);
    }
}
    
/*========================================================================================================================
 * VersionedSymbol methods
 *======================================================================================================================== */

bool
BinaryLoaderElf::VersionedSymbol::is_local() const
{
    if (p_version_entry && p_version_entry->get_value() == 0) {
        return true;
    } else if (p_symbol->SgAsmElfSymbol::STB_LOCAL == p_symbol->get_elf_binding()) {
        return true;
    }
    return false;
}

bool
BinaryLoaderElf::VersionedSymbol::is_hidden() const
{
    if (p_version_entry && p_version_entry->get_value() & VERSYM_HIDDEN)
        return true;
    return false;
}

bool
BinaryLoaderElf::VersionedSymbol::is_reference() const
{
    return NULL != p_version_need;
}

bool
BinaryLoaderElf::VersionedSymbol::is_base_definition() const
{
    if (NULL == p_version_entry)
         return true; /* unversioned entries are always considered "base" */
    if (NULL == p_version_def)
         return false; /* if it's not a definition, then it's clearly not a base definition */
    if (p_version_def->get_flags() & VER_FLG_BASE)
        return true;
    return false;
}

std::string
BinaryLoaderElf::VersionedSymbol::get_version() const
{
    if (p_version_def)
        return p_version_def->get_entries()->get_entries().front()->get_name()->c_str();
    if (p_version_need)
        return p_version_need->get_name()->c_str();
    return std::string();
}

std::string
BinaryLoaderElf::VersionedSymbol::dump_versioned_name() const
{
    std::string name = get_name();
    if (is_hidden())
        name += "[HIDDEN]";
    if (p_version_def) {
        if (p_version_def->get_flags() & VER_FLG_BASE)
            name += "[BASE]";

        SgAsmElfSymverDefinedAuxPtrList& entries=p_version_def->get_entries()->get_entries();
        name += entries.front()->get_name()->c_str();
        if (entries.size() > 1) {
            name += ";";
            for (size_t i=1;i < entries.size(); ++i) {
                name += " ";
                name += entries[i]->get_name()->c_str();
            }
        }
    }
    if (p_version_need) {
        if (p_version_need->get_flags() & VER_FLG_WEAK)
            name += "[WEAK]";
        name += p_version_need->get_name()->c_str();
    }
    return name;
}

/*========================================================================================================================
 * SymbolMapEntry methods
 *======================================================================================================================== */



BinaryLoaderElf::VersionedSymbol
BinaryLoaderElf::SymbolMapEntry::get_vsymbol(const VersionedSymbol &version) const
{
    if (NULL == version.get_version_need())
        return get_base_version();

    std::string neededVersion = version.get_version_need()->get_name()->c_str();
    for (size_t i=0; i<p_versions.size(); ++i) {
        SgAsmElfSymverDefinedEntry* def = p_versions[i].get_version_def();
        if (NULL == def)
            continue;
        if (neededVersion == def->get_entries()->get_entries().front()->get_name()->c_str())
            return p_versions[i];
    }
    ROSE_ASSERT(false);/* TODO, handle cases where input uses versioning, but definition does not */
}

void
BinaryLoaderElf::SymbolMapEntry::addVersion(const VersionedSymbol& vsymbol)
{
    if (vsymbol.is_base_definition()) {
        /* There can be only one "base" version. */
        ROSE_ASSERT(p_versions.empty() || false == get_vsymbol().is_base_definition());
        p_versions.push_back(p_versions.front()); /* swap the front to the back */
        p_versions[0] = vsymbol;
    } else {
        p_versions.push_back(vsymbol);
    }
}

void
BinaryLoaderElf::SymbolMapEntry::merge(const SymbolMapEntry& newEntry)
{
    VersionedSymbol oldSymbol = p_versions.front();
    VersionedSymbol newSymbol = newEntry.p_versions.front();
    int start=0;

    /* Merge base definitions. */
    if (oldSymbol.is_base_definition() && newSymbol.is_base_definition()) {
        if (oldSymbol.get_symbol()->get_elf_binding() == SgAsmElfSymbol::STB_WEAK &&
            newSymbol.get_symbol()->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL) {
            /* the new symbol becomes the new base */
            p_versions[0] = newSymbol;
            start=1;
            if (!oldSymbol.get_version().empty() && oldSymbol.get_version() != newSymbol.get_version()) {
                /* The old symbol was overridden, but it still has a unique version - so we need to keep it. */
                p_versions.push_back(oldSymbol);
            }
        }
    }

    /* Merge remaining versions. */
    for (size_t i=start; i<newEntry.p_versions.size(); ++i) {
        newSymbol = newEntry.p_versions[i];
        std::string newVersion=newSymbol.get_version();
        if (newVersion.empty()) {
            /* If the version has no version then it should have been the base entry, in which case it would have been handled
             * above.  Since it isn't, this is some 'weaker' non-versioned entry, and it can be dropped. */
            continue;
        }
        size_t found=false;
        for (size_t j=0; j<p_versions.size(); ++j) {
            oldSymbol = p_versions[j];
            if (oldSymbol.get_version() == newSymbol.get_version() &&                     /* matching version string */
                oldSymbol.get_symbol()->get_elf_binding() == SgAsmElfSymbol::STB_WEAK &&  /* old is weak */
                newSymbol.get_symbol()->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL) {/* new is strong */
                p_versions[j] = newSymbol; /* override the old symbol */
                found=true;
                break;
            }
        }
        if (!found) {
            /* This version doesn't exist in this entry, so append it to the list. */
            p_versions.push_back(newSymbol);
        }
    }
}

/*========================================================================================================================
 * SymbolMap methods
 *======================================================================================================================== */

const BinaryLoaderElf::SymbolMapEntry *
BinaryLoaderElf::SymbolMap::find(std::string name) const
{
    BaseMap::const_iterator iter = p_base_map.find(name);
    if (p_base_map.end() ==iter)
        return NULL;
    return &(iter->second);
}

const BinaryLoaderElf::SymbolMapEntry *
BinaryLoaderElf::SymbolMap::find(std::string name, std::string version) const
{
    if (version.empty()) {
        return find(name);
    } else {
        return find(name + "("  + version + ")");
    }
}

/*========================================================================================================================
 * SymverResolver methods
 *======================================================================================================================== */

void
BinaryLoaderElf::SymverResolver::ctor(SgAsmGenericHeader* header)
{
    /* Locate the .dynsym, .gnu.version, .gnu.version_d, and/or .gnu_version_r sections. We could have done this with
     * header->get_section_by_name(), but this is possibly more reliable. */
    SgAsmElfSymbolSection* dynsym=NULL;
    SgAsmElfSymverSection* symver=NULL;
    SgAsmElfSymverDefinedSection* symver_def=NULL;
    SgAsmElfSymverNeededSection* symver_need=NULL;
    SgAsmGenericSectionPtrList& sections = header->get_sections()->get_sections();
    for (size_t sec=0; sec < sections.size(); ++sec) {
        SgAsmGenericSection* section = sections[sec];
        if (isSgAsmElfSymbolSection(section) && isSgAsmElfSymbolSection(section)->get_is_dynamic()) {
            dynsym = isSgAsmElfSymbolSection(section);
        } else if(isSgAsmElfSymverSection(section)) {
            symver = isSgAsmElfSymverSection(section);
        } else if(isSgAsmElfSymverDefinedSection(section)) {
            symver_def = isSgAsmElfSymverDefinedSection(section);
        } else if(isSgAsmElfSymverNeededSection(section)) {
            symver_need=isSgAsmElfSymverNeededSection(section);
        }
    }

    /* Build maps */
    if (symver_def)
        makeSymbolVersionDefMap(symver_def);    /* for p_symbolVersionDefMap */
    if (symver_need)
        makeSymbolVersionNeedMap(symver_need);  /* for p_symbolVersionNeedMap */
    makeVersionedSymbolMap(dynsym, symver);     /* for p_versionedSymbolMap */
}

BinaryLoaderElf::VersionedSymbol
BinaryLoaderElf::SymverResolver::get_versioned_symbol(SgAsmElfSymbol* symbol) const
{
    /* If we don't actually have versioned symbols, return the identity version. */
    if (p_versionedSymbolMap.empty())
        return VersionedSymbol(symbol);

    /* We should have every symbol that might get looked up in here */
    VersionedSymbolMap::const_iterator iter = p_versionedSymbolMap.find(symbol);
    ROSE_ASSERT(p_versionedSymbolMap.end() != iter);
    return *(iter->second);
}


void
BinaryLoaderElf::SymverResolver::makeSymbolVersionDefMap(SgAsmElfSymverDefinedSection* section)
{
    ROSE_ASSERT(NULL != section);
    SgAsmElfSymverDefinedEntryPtrList& defs = section->get_entries()->get_entries();
    for (size_t def=0; def < defs.size(); ++def) {
        SgAsmElfSymverDefinedEntry* defEntry =  defs[def];
        p_symbolVersionDefMap[defEntry->get_index()]=defEntry;
    }
}

void
BinaryLoaderElf::SymverResolver::makeSymbolVersionNeedMap(SgAsmElfSymverNeededSection* section)
{
    ROSE_ASSERT(NULL != section);
    SgAsmElfSymverNeededEntryPtrList& needs = section->get_entries()->get_entries();
    for (size_t need=0; need < needs.size(); ++need) {
        SgAsmElfSymverNeededEntry* needEntry = needs[need];
        SgAsmElfSymverNeededAuxPtrList& auxes = needEntry->get_entries()->get_entries();
        for (size_t aux=0; aux < auxes.size(); ++aux) {
            SgAsmElfSymverNeededAux* auxEntry = auxes[aux];
            p_symbolVersionNeedMap[auxEntry->get_other()]=auxEntry;
        }
    }
}

void
BinaryLoaderElf::SymverResolver::makeVersionedSymbolMap(SgAsmElfSymbolSection* dynsym, SgAsmElfSymverSection* symver/*=NULL*/)
{
    ROSE_ASSERT(dynsym && dynsym->get_is_dynamic());
    SgAsmElfSymbolPtrList& symbols = dynsym->get_symbols()->get_symbols();

    /* symverSection may be NULL, but if it isn't, it must have the same number of entries as dynsym */
    ROSE_ASSERT(NULL == symver || symbols.size() == symver->get_entries()->get_entries().size());

    for (size_t symbol_idx=0; symbol_idx<symbols.size(); symbol_idx++) {
        SgAsmElfSymbol *symbol = symbols[symbol_idx];

        ROSE_ASSERT(p_versionedSymbolMap.end() == p_versionedSymbolMap.find(symbol));
        VersionedSymbol* versionedSymbol = new VersionedSymbol;
        p_versionedSymbolMap.insert(std::make_pair(symbol,versionedSymbol));

        versionedSymbol->set_symbol(symbol,dynsym);
        if (symver) {
            SgAsmElfSymverEntry *symverEntry = symver->get_entries()->get_entries()[symbol_idx];
            uint16_t value=symverEntry->get_value();

            /*  VERSYM_HIDDEN = 0x8000 - if set, we should actually completely ignore the symbol
             *  unless this version is specifically requested (which is the normal case anwyay?)
             *  at any rate - we need to strip bit-15
             * This appears to be poorly documented by the spec  [MCB] */
            value &= ~0x8000;

            if (0 == value || 1 == value) {
                /* 0 and 1 are special (local and global respectively) they DO NOT correspond to entries on symver_def
                 * Also, (no documentation for this) if bit 15 is set, this symbol should be ignored? [MCB] */
                versionedSymbol->set_version_entry(symverEntry);    
            } else {
                SymbolVersionDefinitionMap::const_iterator defIter = p_symbolVersionDefMap.find(value);
                SymbolVersionNeededMap::const_iterator needIter = p_symbolVersionNeedMap.find(value);
                /* We must have a match from defs or needs, not both, not neither. */
                ROSE_ASSERT((p_symbolVersionDefMap.end() == defIter) != (p_symbolVersionNeedMap.end() == needIter));
                versionedSymbol->set_version_entry(symverEntry);
                if (p_symbolVersionDefMap.end() != defIter) {
                    versionedSymbol->set_version_def(defIter->second);
                } else if (p_symbolVersionNeedMap.end() != needIter) {
                    versionedSymbol->set_version_need(needIter->second);
                }
            }
        }
    }
}





/*************************************************************************************************************************
 * Low-level ELF stuff and lots of commented out code begins here and continues to the end of the file.  I've moved this stuff
 * from the BinaryLoader_ElfSupport name space to class methods in BinaryLoaderElf, but have not had a chance to review these
 * functions yet. [RPM 2010-09-14]
 *************************************************************************************************************************/



/**
    Relocation of Jump Table Entry (e.g. jump slot).
    A call to a library looks like the following :
      .text: [EXECUTABLE READ ONLY]
        ...
A       call 0x000002080 <file relative address in plt>
F       ...

      .plt [EXECUTABLE READ ONLY]
        ...
B 0x2080 jmp *03030 <file relative address in .got.plt>
C 0x2086 push 0x4 <This is the 4th function in the plt>
D 0x208b jmp *0x00002018 <basically a call to dynamic linker>

      .got.plt [NOT EXECUTABLE WRITABLE]
        ...
Z 0x3030 : 0x2086
        


LIBRARY :
   .text:
     ...
E 0xA018: FOO <do function stuff here>
  ...
  ret


In the above example, in "normal" operation, we would see:
The FIRST time we call the "foo"
A
B <jumps to *Z, which starts pointing at C>
C
D <change Z to the value of E: 0xA018, call Foo directly>
E <execute Foo>
F

The SECOND time, we will have called the dynamic linker, and C and D are no inert
A
B <jumps to *Z which is NOW just E>
E <execute Foo>
F

In this relocation, we need to mimic the dynamic linker, and overwrite the
.got entry (Z) with the address of the function (E i.e. Foo)
Thus, we're performing
*Z = E

*/
SgAsmGenericSection *
BinaryLoaderElf::find_mapped_section(SgAsmGenericHeader* header, rose_addr_t va)
{
    SgAsmGenericSectionPtrList sections = header->get_sections_by_va(va, false);
    SgAsmGenericSection* section=NULL;
    for (size_t sec=0; sec < sections.size(); ++sec) {
        SgAsmElfSection* elfSection = isSgAsmElfSection(sections[sec]);
        if (NULL == elfSection)
            continue;
        ROSE_ASSERT(true == elfSection->is_mapped());// this should be guaranteed by get_sections_by_rva

        if (NULL == elfSection->get_section_entry())
            continue;
        if ((elfSection->get_section_entry()->get_sh_flags() & SgAsmElfSectionTableEntry::SHF_TLS)
            && (elfSection->get_section_entry()->get_sh_type() == SgAsmElfSectionTableEntry::SHT_NOBITS)) {
            //TODO handle .tbss correctly
            continue;
        }

        // there should be only one - could also just assume its the case, and break
        ROSE_ASSERT(NULL == section);
        section = elfSection;
    }
    return section;
}

void
BinaryLoaderElf::relocate_X86_JMP_SLOT(SgAsmElfRelocEntry* reloc,
                                       SgAsmElfRelocSection* parentSection,
                                       const SymbolMap &masterSymbolMap,
                                       const SymverResolver &resolver,
                                       const size_t addrSize)
{
    ROSE_ASSERT(NULL != reloc);
    ROSE_ASSERT(NULL != parentSection); // could fix to just use getEnclosingNode

    SgAsmElfSymbolSection* linkedSymbolSection = isSgAsmElfSymbolSection(parentSection->get_linked_section());
    ROSE_ASSERT(NULL != linkedSymbolSection);

    SgAsmElfSymbol* relocSymbol = linkedSymbolSection->get_symbols()->get_symbols()[reloc->get_sym()];
    ROSE_ASSERT(NULL != relocSymbol);

    VersionedSymbol relocVSymbol = resolver.get_versioned_symbol(relocSymbol);
    if (get_verbose() > 0)
        printf("relocVSymbol: %s\n", relocVSymbol.dump_versioned_name().c_str());

    std::string symbolName=relocVSymbol.get_name();
    const SymbolMapEntry *symbolEntry = masterSymbolMap.find(symbolName);

    if (NULL == symbolEntry) { // TODO check for weak relocsymbol
        printf("Could not find symbol '%s'\n", relocSymbol->get_name()->c_str());
        return;
    }

    VersionedSymbol sourceVSymbol = symbolEntry->get_vsymbol(relocVSymbol);
    SgAsmElfSymbol* sourceSymbol=sourceVSymbol.get_symbol();
    ROSE_ASSERT(NULL != sourceSymbol);

    ROSE_ASSERT(0 != sourceSymbol->get_st_shndx());// test an assumption

    SgAsmGenericHeader* symbolHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(sourceVSymbol.get_parent());


    // The symbol contains a virtual address in a section in the same file as the symbol
    //  however, since that section may have been moved, we need to adjust the resulted symbol value (an address)
    //  to do that, we have to find the relavant section via va (i.e. preferred address)
    SgAsmGenericSection* sourceSection=find_mapped_section(symbolHeader,sourceSymbol->get_value());

    // investiage SHN_COMMON

    SgAsmGenericHeader* relocHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc->get_parent());
    SgAsmGenericSection* targetSection=find_mapped_section(relocHeader,reloc->get_r_offset());
    ROSE_ASSERT(NULL != targetSection);

    // reloc->get_r_offset is the va assuming the library was not moved
    //  so we have to adjust it by the same amount the library WAS moved
    const rose_addr_t reloc_va = reloc->get_r_offset() +
                                 (targetSection->get_mapped_actual_va() - targetSection->get_mapped_preferred_va());

    // offset from start of section
    rose_addr_t relocSectionOffset = reloc_va - targetSection->get_mapped_actual_va();

    const rose_addr_t symbol_va = sourceSymbol->get_value() +
                                  (sourceSection->get_mapped_actual_va() - sourceSection->get_mapped_preferred_va());

    SgFileContentList contents = targetSection->get_data();

    if (addrSize == 4) {
        ROSE_ASSERT(symbol_va <= UINT_MAX);
        uint32_t reloc_value_32 = symbol_va;
        memcpy(&(contents.at(relocSectionOffset)),&reloc_value_32,addrSize);
    } else if(addrSize == 8) {
        // TODO Throw some asserts around this
        memcpy(&(contents.at(relocSectionOffset)),&symbol_va,addrSize);
    }

    if (get_verbose() > 0) {
        const char* targetSectionName = targetSection->get_name()->c_str();
        const char* sourceSectionName = "";
        if (sourceSection)
            sourceSectionName = sourceSection->get_name()->c_str();

        printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"[%s]\n",reloc_va,targetSectionName,symbol_va,sourceSectionName);
    }
}

void
BinaryLoaderElf::relocate_X86_64_RELATIVE(SgAsmElfRelocEntry* reloc,
                                          SgAsmElfRelocSection* parentSection,
                                          const SymbolMap &masterSymbolMap,
                                          const SymverResolver &resolver,
                                          const size_t addrSize)
{
    ROSE_ASSERT(NULL != reloc);
    ROSE_ASSERT(NULL != parentSection); // could fix to just use getEnclosingNode

    //SgAsmElfSection* linkedSection = parentSection->get_linked_section();
    //ROSE_ASSERT(NULL != linkedSymbolSection);

    ROSE_ASSERT(0 == reloc->get_sym());// _RELATIVE relocs must set their symbol index to zero

    SgAsmGenericHeader* relocHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc->get_parent());
    SgAsmGenericSection* targetSection=find_mapped_section(relocHeader,reloc->get_r_offset());
    ROSE_ASSERT(NULL != targetSection);

    rose_addr_t targetSectionShift=(targetSection->get_mapped_actual_va() - targetSection->get_mapped_preferred_va());
    if (targetSectionShift == 0) {
        // _RELATIVE locations are used to shift address so they are still pointing at the same object
        //  of the section was loaded at the 'preferred' address, nothing needs to be done
        return;
    }

    // reloc->get_r_offset is the va assuming the library was not moved
    //  so we have to adjust it by the same amount the library WAS moved
    const rose_addr_t reloc_va = reloc->get_r_offset() + targetSectionShift;

    // offset from start of section
    rose_addr_t relocSectionOffset = reloc_va - targetSection->get_mapped_actual_va();

    SgFileContentList contents = targetSection->get_data();

    // The new value is simply the address (plus the addend - if any)
    const rose_addr_t reloc_value = reloc_va + reloc->get_r_addend();

    if (addrSize == 4) {
        ROSE_ASSERT(reloc_value <= (uint64_t)(UINT_MAX));
        uint32_t reloc_value_32 = reloc_value;
        memcpy(&(contents.at(relocSectionOffset)),&reloc_value_32,addrSize);
    } else if (addrSize == 8) {
        // TODO Throw some asserts around this
        memcpy(&(contents.at(relocSectionOffset)),&reloc_va,addrSize);
    }

    if (get_verbose() > 0) {
        const char* targetSectionName = targetSection->get_name()->c_str();
        printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"\n",reloc_va,targetSectionName,reloc_value);
    }
}

void
BinaryLoaderElf::relocate_X86_64_64(SgAsmElfRelocEntry* reloc,
                                    SgAsmElfRelocSection* parentSection,
                                    const SymbolMap &masterSymbolMap,
                                    const SymverResolver &resolver,
                                    const size_t addrSize)
{
    ROSE_ASSERT(NULL != reloc);
    ROSE_ASSERT(NULL != parentSection); // could fix to just use getEnclosingNode

    SgAsmElfSymbolSection* linkedSymbolSection = isSgAsmElfSymbolSection(parentSection->get_linked_section());
    ROSE_ASSERT(NULL != linkedSymbolSection);

    SgAsmElfSymbol* relocSymbol = linkedSymbolSection->get_symbols()->get_symbols()[reloc->get_sym()];
    ROSE_ASSERT(NULL != relocSymbol);

    VersionedSymbol relocVSymbol = resolver.get_versioned_symbol(relocSymbol);
    if (get_verbose() > 0)
        printf("relocVSymbol: %s\n", relocVSymbol.dump_versioned_name().c_str());

    std::string symbolName=relocVSymbol.get_name();
    const SymbolMapEntry *symbolEntry = masterSymbolMap.find(symbolName);

    if (NULL == symbolEntry) { // TODO check for weak relocsymbol
        printf("Could not find symbol '%s'\n", relocSymbol->get_name()->c_str());
        return;
    }

    VersionedSymbol sourceVSymbol = symbolEntry->get_vsymbol(relocVSymbol);
    SgAsmElfSymbol* sourceSymbol=sourceVSymbol.get_symbol();
    ROSE_ASSERT(NULL != sourceSymbol);
    ROSE_ASSERT(0 != sourceSymbol->get_st_shndx());// test an assumption

    SgAsmGenericHeader* symbolHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(sourceVSymbol.get_parent());


    // The symbol contains a virtual address in a section in the same file as the symbol
    //  however, since that section may have been moved, we need to adjust the resulted symbol value (an address)
    //  to do that, we have to find the relavant section via va (i.e. preferred address)
    SgAsmGenericSection* sourceSection=find_mapped_section(symbolHeader,sourceSymbol->get_value());

    // investiage SHN_COMMON


    SgAsmGenericHeader* relocHeader = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc->get_parent());
    SgAsmGenericSection* targetSection=find_mapped_section(relocHeader,reloc->get_r_offset());
    ROSE_ASSERT(NULL != targetSection);

    // reloc->get_r_offset is the va assuming the library was not moved
    //  so we have to adjust it by the same amount the library WAS moved
    const rose_addr_t reloc_va = reloc->get_r_offset() +
                                 (targetSection->get_mapped_actual_va() - targetSection->get_mapped_preferred_va());

    // offset from start of section
    rose_addr_t relocSectionOffset = reloc_va - targetSection->get_mapped_actual_va();

    const rose_addr_t symbol_va = sourceSymbol->get_value() +
                                  (sourceSection->get_mapped_actual_va() - sourceSection->get_mapped_preferred_va());

    SgFileContentList contents = targetSection->get_data();
    rose_addr_t reloc_value = symbol_va + reloc->get_r_addend();

    if (addrSize == 4) {
        ROSE_ASSERT(reloc_value <= (uint64_t)(UINT_MAX));
        uint32_t reloc_value_32 = reloc_value;
        memcpy(&(contents.at(relocSectionOffset)),&reloc_value_32,addrSize);
    } else if (addrSize == 8) {
        // TODO Throw some asserts around this
        memcpy(&(contents.at(relocSectionOffset)),&symbol_va,addrSize);
    }

    if (get_verbose() > 0) {
        const char* targetSectionName = targetSection->get_name()->c_str();
        const char* sourceSectionName = "";
        if (sourceSection)
            sourceSectionName = sourceSection->get_name()->c_str();
        printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"[%s]\n",reloc_va,targetSectionName,reloc_value,sourceSectionName);
    }
}

void
BinaryLoaderElf::performRelocation(SgAsmElfRelocEntry* reloc,
                                   SgAsmElfRelocSection* parentSection,
                                   const SgAsmElfSectionPtrList& extentSortedSections,
                                   const SymverResolver &resolver,
                                   const SymbolMap& masterSymbolMap)
{
    ROSE_ASSERT(NULL != reloc);
    ROSE_ASSERT(NULL != parentSection); // could fix to just use getEnclosingNode

    SgAsmGenericHeader* header = parentSection->get_header();
    ROSE_ASSERT(NULL != header);// need the header to determine ISA

    SgAsmGenericHeader::InsSetArchitecture isa = header->get_isa();
    
    {
        SgAsmElfSymbolSection* linkedSymbolSection = isSgAsmElfSymbolSection(parentSection->get_linked_section());
        ROSE_ASSERT(NULL != linkedSymbolSection);

        SgAsmElfSymbol* relocSymbol = linkedSymbolSection->get_symbols()->get_symbols()[reloc->get_sym()];
        ROSE_ASSERT(NULL != relocSymbol);

#if 0 /* may be null according to SgAsmElfRelocSection::ctor() [RPM 2010-09-13] */
        SgAsmElfSection* targetSection=parentSection->get_target_section();
        ROSE_ASSERT(NULL != targetSection);
#endif

        std::string symbolName=relocSymbol->get_name()->c_str();

        std::string relocStr;
        if (header)
            relocStr = reloc->to_string(reloc->get_type(),isa);
        printf("%16s '%25s' 0x%016lx",relocStr.c_str(),symbolName.c_str(),reloc->get_r_offset());
    }

    // TODO support other than x86
    switch (isa & SgAsmGenericHeader::ISA_FAMILY_MASK) {
        case SgAsmGenericHeader::ISA_IA32_Family:
            switch (reloc->get_type()) {
                //case SgAsmElfRelocEntry::R_386_JMP_SLOT:relocate_X86_JMP_SLOT(reloc,parentSection,masterSymbolMap,resolver,extentSortedSections,4);break;
                //case SgAsmElfRelocEntry::R_386_GLOB_DAT:relocate_X86_GLOB_DAT(reloc,symbol,symbolMap,extentSortedSections,4);break;
                //case SgAsmElfRelocEntry::R_386_32:      relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;
                //case SgAsmElfRelocEntry::R_386_RELATIVE:relocate_386_RELATIVE(reloc,symbol,symbolMap,extentSortedSections);break;
                case SgAsmElfRelocEntry::R_386_TLS_TPOFF:
                case SgAsmElfRelocEntry::R_386_TLS_IE:
                case SgAsmElfRelocEntry::R_386_TLS_GOTIE:
                case SgAsmElfRelocEntry::R_386_TLS_LE:
                case SgAsmElfRelocEntry::R_386_TLS_GD:
                case SgAsmElfRelocEntry::R_386_TLS_LDM:
                case SgAsmElfRelocEntry::R_386_TLS_GD_32:
                case SgAsmElfRelocEntry::R_386_TLS_GD_PUSH:
                case SgAsmElfRelocEntry::R_386_TLS_GD_CALL:
                case SgAsmElfRelocEntry::R_386_TLS_GD_POP:
                case SgAsmElfRelocEntry::R_386_TLS_LDM_32:
                case SgAsmElfRelocEntry::R_386_TLS_LDM_PUSH:
                case SgAsmElfRelocEntry::R_386_TLS_LDM_CALL:
                case SgAsmElfRelocEntry::R_386_TLS_LDM_POP:
                case SgAsmElfRelocEntry::R_386_TLS_LDO_32:
                case SgAsmElfRelocEntry::R_386_TLS_IE_32:
                case SgAsmElfRelocEntry::R_386_TLS_LE_32:
                case SgAsmElfRelocEntry::R_386_TLS_DTPMOD32:
                case SgAsmElfRelocEntry::R_386_TLS_DTPOFF32:
                case SgAsmElfRelocEntry::R_386_TLS_TPOFF32:
                    printf(" Thread Local Storage not supported");
                    break;
                default:
                    printf(" <unresolved>");
                    break;
            };
            break;

        case SgAsmGenericHeader::ISA_X8664_Family:
            switch (reloc->get_type()) {
                case SgAsmElfRelocEntry::R_X86_64_JUMP_SLOT:
                    relocate_X86_JMP_SLOT(reloc, parentSection, masterSymbolMap, resolver, 8);
                    break;
                case SgAsmElfRelocEntry::R_X86_64_GLOB_DAT:
                    relocate_X86_JMP_SLOT(reloc, parentSection, masterSymbolMap, resolver, 8);
                    break;
                //case SgAsmElfRelocEntry::R_X86_64_32:      relocate_X86_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;// TODO should zero extend symbol value
                //case SgAsmElfRelocEntry::R_X86_64_32S:     relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;// TODO should sign extend symbol value
                case SgAsmElfRelocEntry::R_X86_64_64:
                    relocate_X86_64_64(reloc, parentSection, masterSymbolMap, resolver, 8);
                    break;
                case SgAsmElfRelocEntry::R_X86_64_RELATIVE:
                    relocate_X86_64_RELATIVE(reloc, parentSection, masterSymbolMap, resolver, 8);
                    break;
                default:
                    printf(" <unresolved>");
                    break;
            };
            break;

        default:
            printf(" <unresolved>");
    }
    printf("\n");
}

void
BinaryLoaderElf::performRelocations(SgAsmElfFileHeader* elfHeader,
                                    const SgAsmElfSectionPtrList& extentSortedSections,
                                    const SymbolMap& masterSymbolMap)
{
    SymverResolver resolver(elfHeader);
    SgAsmGenericSectionPtrList sections = elfHeader->get_sectab_sections();
    for (size_t sec=0; sec < sections.size(); ++sec) {
        SgAsmElfRelocSection* relocSection = isSgAsmElfRelocSection(sections[sec]);
        if (NULL == relocSection)
            continue;

        SgAsmElfRelocEntryPtrList &relocs = relocSection->get_entries()->get_entries();
        for (size_t r=0; r <  relocs.size(); ++r) {
            SgAsmElfRelocEntry* reloc = relocs[r];
            performRelocation(reloc, relocSection, extentSortedSections,resolver,masterSymbolMap);
        }
    }
}

// #if 0
// VersionedSymbol makeVersionedSymbol(SgAsmElfSymbol* symbol)
// {
//   VersionedSymbol vsymbol(symbol);
//   SgAsmElfSymverSection* symver=NULL;
//   SgAsmElfSymverDefinedSection* symver_def=NULL;
//   SgAsmElfSymverNeededSection* symver_need=NULL;
//
//   SgAsmElfFileHeader* header = SageInterface::getEnclosingNode<SgAsmElfFileHeader>(symbol);
//   if(header == NULL){
//     // somehow free floating - just return it
//     return vsymbol;
//   }
//   SgAsmElfSymbolSection* dynsym = SageInterface::getEnclosingNode<SgAsmElfSymbolSection>(symbol);
//   ROSE_ASSERT(NULL != dynsym);
//   if(false == dynsym->get_is_dynamic()){
//     // only the dynamic symbol table contains versioned symbols
//     return vsymbol;
//   }
//
//   SgAsmGenericSectionPtrList& sections = header->get_sections()->get_sections();
//   for(size_t sec=0; sec < sections.size(); ++sec){
//     SgAsmGenericSection* section = sections[sec];
//     if(isSgAsmElfSymverSection(section)){
//       symver = isSgAsmElfSymverSection(section);
//     }
//     else if(isSgAsmElfSymverDefinedSection(section)){
//       symver_def = isSgAsmElfSymverDefinedSection(section);
//     }
//     else if(isSgAsmElfSymverDefinedSection(section)){
//       symver_need = isSgAsmElfSymverNeededSection(section);
//     }
//   }
//   if(NULL == symver){
//     // symbol versioning not present here, return unversioned
//     return vsymbol;
//   }
//
// }
// #endif

    /*
    // 1. Get section map (name -> list<section*>)
    getSectionNameMap(binaryFile,allSectionsMap,loadableSectionsMap);

    // 2. Create Symbol map from relevant sections (.dynsym)
    SymbolMap symbolMap;
    getSymbolMap(allSectionsMap,symbolMap);

    // 3. Create Extent sorted list of loadable sections
    SgAsmElfSectionPtrList extentSortedSections;
    sortSections(loadableSectionsMap,extentSortedSections);

    // 4. Collect Relocation Entries.
    RelocationEntryList relocations;
    getRelocationEntries(binaryFile, relocations);

    // 5.  for each relocation entry, perform relocation
    processRelocations(relocations,symbolMap,extentSortedSections);
    */



/* This is a big section of commented out code. I'm prefixing all lines with "//" to make it more obvious. [RPM 2010-08-31] */
// #if 0
//
// typedef std::map<string,SgAsmElfSectionPtrList> SectionNameMap;
//
// class ElfSectionCollector : public AstSimpleProcessing
// {
// public:
//   SectionNameMap mAllSections;
//   SectionNameMap mLoadableSections;
//
//   virtual void visit(SgNode* node)
//   {
//     SgAsmElfSection* elfSection = isSgAsmElfSection(node);
//     if(elfSection == NULL)
//       return; // not an elf section
//
//     // TODO we shouldn't really be using 'name' as a designator, rather, we should use sectionEntry flags
//     string name = elfSection->get_name()->c_str();
//     SgAsmElfSectionPtrList& allEntryList = mAllSections[name];
//     if(!allEntryList.empty() && elfSection == allEntryList.back()){
//       // we're getting duplicates in visit
//       return;
//     }
//
//     allEntryList.push_back(elfSection);
//
//     bool isLoadable=true;
//     const SgAsmElfSectionTableEntry* sectionEntry = elfSection->get_section_entry();
//     if(sectionEntry) {
//       const uint64_t flags = sectionEntry->get_sh_flags();
//       if(!(flags & SgAsmElfSectionTableEntry::SHF_ALLOC )){
//      // Elf lets us completely skip loading a section if its not SHF_ALLOC
//      isLoadable=false;
//       }
//       if((flags & SgAsmElfSectionTableEntry::SHF_TLS) &&
//          sectionEntry->get_sh_type() == SgAsmElfSectionTableEntry::SHT_NOBITS){ // .tbss
//         //TODO support Thread Local Storage.  In particular, .tbss gives us
//      //  issues because it occupies zero disk and zero LOGICAL space
//      isLoadable=false;
//       }
//     }
//     if(isLoadable){
//       mLoadableSections[name].push_back(elfSection);
//     }
//   }
// };
//
// void getSectionNameMap(SgBinaryComposite* binaryFile,
//                        SectionNameMap& allSections,
//                     SectionNameMap& loadedSections)
// {
//   ElfSectionCollector visitor;
//   visitor.traverse(binaryFile,preorder);
//   allSections = visitor.mAllSections;
//   loadedSections = visitor.mLoadableSections;
// }
//
//
// struct ElfSymbolMapEntry
// {
//   ElfSymbolMapEntry() : symbol(NULL){};
// public:
//   rose_addr_t get_va() const
//   { return symbol->get_value() + section->get_base_va();}
//
//   SgAsmElfSymbol* symbol;
//   SgAsmGenericSection* section;
// };
//
// /// map from symbol name to a symbol entry
// typedef Rose_STL_Container<ElfSymbolMapEntry> ElfSymbolMapEntryList;
// typedef std::map<string, ElfSymbolMapEntryList> SymbolMap;
//
//
// class SymbolCollector : public AstSimpleProcessing
// {
// public:
//   SymbolMap _symbolMap;
//   virtual void visit(SgNode* node)
//   {
//     if (isSgAsmElfSymbol(node)){
//       SgAsmElfSymbol* symbol = isSgAsmElfSymbol(node);
//       SgAsmGenericHeader* header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(symbol);
//       string symbolName = symbol->get_name()->c_str();
//       if(symbolName.empty()){
//      return;
//       }
//       ElfSymbolMapEntryList& entryList = _symbolMap[symbolName];
//       if(header == NULL){
//      return;
//       }
//       SgAsmGenericSectionPtrList sections = header->get_sections_by_va(symbol->get_value());
//
//       ROSE_ASSERT(!sections.empty());
//       SgAsmGenericSectionPtrList::iterator iter = sections.begin();
//       for(; iter != sections.end(); ++iter){
//      SgAsmGenericSection* section = *iter;
//      SgAsmElfSection* elfSection = isSgAsmElfSection(section);
//      
//      // TODO document this
//      if((!elfSection) ||
//            (elfSection->get_section_entry() &&
//             !(elfSection->get_section_entry()->get_sh_flags() & SgAsmElfSectionTableEntry::SHF_ALLOC)))
//      {
//        continue;
//      }
//      
//      ElfSymbolMapEntry addr;
//      addr.section = *iter;
//      addr.symbol = symbol;
//      entryList.push_back(addr);
//       }
//     }
//   }
// };
//
//
// void getSymbolMap( const SectionNameMap &allSectionsMap,SymbolMap &symbolMap)
// {
//
//   SectionNameMap::const_iterator findIter=allSectionsMap.find(".dynsym");
//   ROSE_ASSERT(findIter != allSectionsMap.end());// must have at least one dynamic symbol table
//
//   const SgAsmElfSectionPtrList& dynsymSections = findIter->second;
//   SymbolCollector symbolCollector;
//   {
//     SgAsmElfSectionPtrList::const_iterator sectionIter=dynsymSections.begin();
//     for(; sectionIter != dynsymSections.end(); ++sectionIter){
//       symbolCollector.traverse(*sectionIter,preorder);
//     }
//   }
//   symbolMap = symbolCollector._symbolMap;
// }
//
// bool extentSorterActualVA(const SgAsmGenericSection* lhs,const SgAsmGenericSection* rhs)
// {
//   return // sorted by start of extent, then by length
//     (std::make_pair(lhs->get_mapped_actual_va(),
//                  lhs->get_mapped_size()))
//     <
//     (std::make_pair(rhs->get_mapped_actual_va(),
//                  rhs->get_mapped_size()));
// }
//
// bool extentSorterPreferedRVA(const SgAsmGenericSection* lhs,const SgAsmGenericSection* rhs)
// {
//   return // sorted by start of extent, then by length
//     (std::make_pair(lhs->get_mapped_actual_rva(),
//                  lhs->get_mapped_size()))
//     <
//     (std::make_pair(rhs->get_mapped_actual_rva(),
//                  rhs->get_mapped_size()));
// }
//
// void sortSections(const SectionNameMap& sectionMap,SgAsmElfSectionPtrList& sortedSections)
// {
//   SectionNameMap::const_iterator sectionIter=sectionMap.begin();
//
//   for( ;sectionIter!=sectionMap.end();++sectionIter){
//     sortedSections.insert(sortedSections.end(),sectionIter->second.begin(),sectionIter->second.end());
//   }
//   std::sort(sortedSections.begin(),sortedSections.end(),extentSorterActualVA);
//   sortedSections.erase(std::unique(sortedSections.begin(), sortedSections.end()), sortedSections.end());
// }
// struct RelocationEntry
// {
//   SgAsmElfRelocEntry* reloc;
//   SgAsmElfSymbol* symbol;
// };
//
// typedef Rose_STL_Container<RelocationEntry> RelocationEntryList;
// typedef Rose_STL_Container<SgAsmElfRelocSection*> SgAsmElfRelocSectionPtrList;
// void getRelocationEntries(SgBinaryComposite *binaryFile,RelocationEntryList& relocs)
// {
// #if 0
//
//   SgAsmElfRelocSectionPtrList relocSections =
//     SageInterface::querySubTree<SgAsmElfRelocSection>(binaryFile,V_SgAsmElfRelocSection);
//
//   SgAsmElfRelocSectionPtrList::const_iterator sectionIter = relocSections.begin();
//   for(;sectionIter != relocSections.end();++sectionIter){
//     SgAsmElfRelocSection* section = *sectionIter;
//
//     SgAsmElfSymbolSection *symtab=isSgAsmElfSymbolSection(section->get_linked_section());
//     ROSE_ASSERT(symtab != NULL);
//
//     SgAsmElfRelocEntryPtrList& entries = section->get_entries()->get_entries();
//     SgAsmElfRelocEntryPtrList::iterator iter = entries.begin();
//     for(; iter != entries.end(); ++iter)
//     {
//       SgAsmElfRelocEntry* reloc = *iter;
//       RelocationEntry entry;
//       entry.reloc = reloc;
//       entry.symbol = symtab->get_symbols()->get_symbols()[reloc->get_sym()];
//       relocs.push_back(entry);
//     }        
//   }
// #endif
// }
//
// string sectionString(SgAsmGenericSection* section)
// {
//   string str;
//   SgAsmGenericFile* file = SageInterface::getEnclosingNode<SgAsmGenericFile>(section);
//   if(file)
//     str += file->get_name();
//   else
//     str += "<none>";
//
//   str += "[";
//   str += section->get_name()->c_str();
//   str += "]";
//   return str;
// }
//
// // TODO use sh_type instead of section name
// ElfSymbolMapEntry
// findSection(const ElfSymbolMapEntryList& symbolEntry,
//          const string& sectionName)
// {
//   ElfSymbolMapEntryList::const_iterator iter = symbolEntry.begin();
//   for(; iter != symbolEntry.end(); ++iter){
//     if(iter->section->get_name()->c_str() == sectionName)
//       return *iter;
//   }
//   return ElfSymbolMapEntry();
// }
//
// SgAsmElfSectionPtrList sectionsOfAddress(rose_addr_t va_addr,
//                                          const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SgAsmElfSectionPtrList returnSections;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(va_addr >= section->get_mapped_actual_va() &&
//        va_addr < (section->get_mapped_actual_va() + section->get_mapped_size()))
//     {
//       returnSections.push_back(section);
//     }
//   }
//   return returnSections;
// }
//
// SgAsmElfSection* chooseSectionAtAddress(rose_addr_t va_addr,
//                                      const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   // we always prefer sections to segments (both of which are SgAsmElfSection's
//   SgAsmElfSection* segment=NULL;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(va_addr >= section->get_mapped_actual_va() &&
//        va_addr < (section->get_mapped_actual_va() + section->get_mapped_size()))
//     {
//       if(section->get_section_entry())
//      return section;
//       else
//      segment = section;
//     }
//   }
//   return segment;
// }                                    
//
// // TODO use sh_type instead of sectionName
// SgAsmElfSection*
// findElfSection(const SgAsmElfSectionPtrList& sections,
//                const string& sectionName)
// {
//   for(size_t i=0; i < sections.size(); ++i){
//     if(sections[i]->get_name()->c_str() == sectionName)
//       return sections[i];
//   }
//   return NULL;
// }
//
// ElfSymbolMapEntryList removeSegments(const ElfSymbolMapEntryList &symbolEntry)
// {
//   ElfSymbolMapEntryList returnList;
//   ElfSymbolMapEntryList::const_iterator iter = symbolEntry.begin();
//   for(; iter != symbolEntry.end(); ++iter){
//     SgAsmElfSection* section = isSgAsmElfSection(iter->section);
//     if(section && section->get_section_entry() != NULL)
//     {
//       // have to be careful here, some section DO have a segment_entry (like .dynamic)
//       returnList.push_back(*iter);
//     }
//   }
//   return returnList;
// }
//
// SgAsmElfSectionPtrList removeSegments(const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SgAsmElfSectionPtrList returnSections;
//   for(size_t i=0; i < extentSortedSections.size(); ++i)
//   {
//     SgAsmElfSection* section = extentSortedSections[i];
//     if(section->get_section_entry() != NULL)
//     {
//       // have to be careful here, some section DO have a segment_entry (like .dynamic)
//       returnSections.push_back(section);
//     }
//   }
//   return returnSections;
// }
//
//
// string stringOfSectionsOfAddress(rose_addr_t va_addr,
//                                    const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   std::vector<SgAsmElfSection*> sections = sectionsOfAddress(va_addr,extentSortedSections);
//   SgAsmElfSection* bestSection = NULL;
//
//   for(size_t i=0; i < sections.size(); ++i)
//   {
//     SgAsmElfSection* section = sections[i];
//     if(bestSection == NULL || bestSection->get_segment_entry())
//       bestSection = section;
//   }
//   if(bestSection == NULL)
//   {
//     return "<unknown>";
//   }
//   else
//   {
//     return sectionString(bestSection);
//   }
// }
//
// void printSymbolMapEntry(const ElfSymbolMapEntryList& entries,
//                          const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   printf("'%s':\n", entries.front().symbol->get_name()->c_str());
//
//   for(size_t i=0; i < entries.size(); ++i){
//     const ElfSymbolMapEntry& entry = entries[i];
//     rose_addr_t symbol_va = entry.get_va();
//     printf(" %01d 0x%016x %s\n", i, symbol_va, sectionString(entry.section).c_str());
//     printf("   %s\n",stringOfSectionsOfAddress(symbol_va,extentSortedSections).c_str());
//   }
// }
//
// void
// printSymbolMap(const SymbolMap& symbolMap,
//                const SgAsmElfSectionPtrList &extentSortedSections)
// {
//   SymbolMap::const_iterator iter = symbolMap.begin();
//   SymbolMap::const_iterator iterEnd = symbolMap.end();
//   for(; iter != iterEnd; ++iterEnd){
//     const ElfSymbolMapEntryList &entry = iter->second;
//     printSymbolMapEntry(entry,extentSortedSections);
//   }
// }
//
// ElfSymbolMapEntry chooseSymbol(const ElfSymbolMapEntryList& entries,
//                                SgAsmElfSectionTableEntry::SectionType sh_type=SgAsmElfSectionTableEntry::SHT_NULL)
// {
//   ROSE_ASSERT(!entries.empty());
//   ElfSymbolMapEntry best = ElfSymbolMapEntry();
//   bool bestGlobalBinding = false;
//   bool bestIsSection = false;
//   // indicates that section type is an important filter
//   bool checkSectionType = (sh_type != SgAsmElfSectionTableEntry::SHT_NULL);
//   for(size_t i=0; i < entries.size(); ++i){
//     SgAsmElfSection* section = isSgAsmElfSection(entries[i].section);
//     SgAsmElfSectionTableEntry* sectionEntry = NULL;
//     if(section)
//       sectionEntry = section->get_section_entry();
//
//     if(checkSectionType &&
//        (!sectionEntry || sectionEntry->get_sh_type() != sh_type)){
//       // we're asked to check the section type, but it is not a section
//       //  or its the wrong type, so we just drop it completely.
//       continue;
//     }
//
//     bool globalBinding = entries[i].symbol->get_elf_binding() == SgAsmElfSymbol::STB_GLOBAL;
//     bool isSection = (sectionEntry != NULL);// sections must have a section entry
//
//     if(bestGlobalBinding == globalBinding &&
//        (bestIsSection || !isSection)){
//       // if the global binding is the same
//       //  and we are not a section (and best IS), we lose
//       continue;
//     }
//     else if(bestGlobalBinding && !globalBinding){
//       // the current best is globally bound, and we are not, we lose
//       continue;
//     }
//     else{
//       // we are globally bound and/or we are a section
//       bestGlobalBinding = globalBinding;
//       bestIsSection = isSection;
//       best = entries[i];
//     }
//   }
//   return best;
// }
//                      
//
// rose_addr_t symbolValue(const SgAsmElfSymbol* relocSymbol,
//                      const SymbolMap &symbolMap,
//                      const SgAsmElfSectionPtrList& extentSortedSections,
//                      SgAsmGenericSection* &symbolSection)
// {
//   ROSE_ASSERT(relocSymbol);
//   string symbolName = relocSymbol->get_name()->c_str();
//   SymbolMap::const_iterator findIter = symbolMap.find(symbolName);
//   if(findIter == symbolMap.end() || findIter->second.empty()){
//     if(relocSymbol->get_elf_binding() == SgAsmElfSymbol::STB_WEAK){
//       return 0;// Its okay to have an undefined symbol if the RELOCATION's symbol is weak
//     }
//     else{
//       printf("Unresolved relocation : Symbol not found: %s\n ",symbolName.c_str());
//       return 0;
//     }
//   }
//   else{
//     const ElfSymbolMapEntryList &entryList = findIter->second;
//     ElfSymbolMapEntry sourceSymbolEntry = chooseSymbol(entryList/*,SgAsmElfSectionTableEntry::SHT_PROGBITS.text*/);
//     SgAsmElfSymbol* sourceSymbol = isSgAsmElfSymbol(sourceSymbolEntry.symbol);
//
//     rose_addr_t symbol_va = sourceSymbol->get_value() + SageInterface::getEnclosingNode<SgAsmElfSection>(sourceSymbol)->get_base_va();
//
//     symbolSection = sourceSymbolEntry.section;
//     ROSE_ASSERT(symbolSection != NULL);
//     return symbol_va;
//   }
//
// }
//                      
//
// void
// relocate_X86_Symbol(const SgAsmElfRelocEntry* reloc,
//                  const SgAsmElfSymbol* symbol,
//                  const SymbolMap &symbolMap,
//                  const SgAsmElfSectionPtrList& extentSortedSections,
//                  const rose_addr_t addrSize)
// {
//   ROSE_ASSERT(reloc != NULL);
//   ROSE_ASSERT(symbol != NULL);
//   SgAsmGenericSection *sourceSection = NULL;
//   rose_addr_t symbol_va = symbolValue(symbol,symbolMap,extentSortedSections,sourceSection);
//
//   const SgAsmElfSection* relocSection=SageInterface::getEnclosingNode<SgAsmElfSection>(reloc);
//   ROSE_ASSERT(relocSection != NULL);
//
//   const rose_addr_t reloc_va = reloc->get_r_offset() + relocSection->get_base_va();
//
//   SgAsmElfSection* targetSection = chooseSectionAtAddress(reloc_va,extentSortedSections);
//   ROSE_ASSERT(targetSection);
//
//   // the relocation should come from the same SgAsmFile as the target, we use base_va as a proxy for this
//   ROSE_ASSERT(relocSection->get_base_va() == targetSection->get_base_va());
//
//   /// offset from start of section
//   rose_addr_t relocSectionOffset = reloc_va - targetSection ->get_mapped_actual_va();
//
//   SgFileContentList contents = targetSection->get_data();
//   const rose_addr_t reloc_value = symbol_va;
//
//   if(addrSize == 4){
//     ROSE_ASSERT(symbol_va <= UINT_MAX);
//     uint32_t reloc_value_32 = symbol_va;
//     memcpy(&(contents.at(relocSectionOffset)),&reloc_value_32,addrSize);
//   }
//   else if(addrSize == 8){
//     // TODO Throw some asserts around this
//     memcpy(&(contents.at(relocSectionOffset)),&symbol_va,addrSize);
//   }
//
//   if(get_verbose() > 0){
//     const char* targetSectionName = targetSection->get_name()->c_str();
//     const char* sourceSectionName = "";
//     if(sourceSection)
//       sourceSectionName = sourceSection->get_name()->c_str();
//
//     printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"[%s]\n",reloc_va,targetSectionName,symbol_va,sourceSectionName);
//   }
// }
//
//
// /**
//     Relocation of Jump Table Entry (e.g. jump slot).
//     A call to a library looks like the following :
//       .text: [EXECUTABLE READ ONLY]
//         ...
// A       call 0x000002080 <file relative address in plt>
// F       ...
//
//       .plt [EXECUTABLE READ ONLY]
//         ...
// B 0x2080 jmp *03030 <file relative address in .got.plt>
// C 0x2086 push 0x4 <This is the 4th function in the plt>
// D 0x208b jmp *0x00002018 <basically a call to dynamic linker>
//
//       .got.plt [NOT EXECUTABLE WRITABLE]
//         ...
// Z 0x3030 : 0x2086
//      
//
//
// LIBRARY :
//    .text:
//      ...
// E 0xA018: FOO <do function stuff here>
//   ...
//   ret
//
//
// In the above example, in "normal" operation, we would see:
// The FIRST time we call the "foo"
// A
// B <jumps to *Z, which starts pointing at C>
// C
// D <change Z to the value of E: 0xA018, call Foo directly>
// E <execute Foo>
// F
//
// The SECOND time, we will have called the dynamic linker, and C and D are no inert
// A
// B <jumps to *Z which is NOW just E>
// E <execute Foo>
// F
//
// In this relocation, we need to mimic the dynamic linker, and overwrite the
// .got entry (Z) with the address of the function (E i.e. Foo)
// Thus, we're performing
// *Z = E
//
// */
// void
// relocate_X86_JMP_SLOT(const SgAsmElfRelocEntry* reloc,
//                    const SgAsmElfSymbol* symbol,
//                    const SymbolMap &symbolMap,
//                    const SgAsmElfSectionPtrList& extentSortedSections,
//                    const size_t addrSize)
// {
//   relocate_X86_Symbol(reloc,symbol,symbolMap,extentSortedSections,addrSize);
// }
//
// /**
//    Pointers to data.  Generally:
//    *reloc_va (.got) = symbol->address (.data);
//  */
// void
// relocate_X86_GLOB_DAT(const SgAsmElfRelocEntry* reloc,
//                    const SgAsmElfSymbol* symbol,
//                    const SymbolMap &symbolMap,
//                    const SgAsmElfSectionPtrList& extentSortedSections,
//                    const size_t addrSize)
// {
//   relocate_X86_Symbol(reloc,symbol,symbolMap,extentSortedSections,addrSize);
// }
//
// void
// relocate_386_32(const SgAsmElfRelocEntry* reloc,
//              const SgAsmElfSymbol* symbol,
//              const SymbolMap &symbolMap,
//              const SgAsmElfSectionPtrList& extentSortedSections,
//              const size_t addrSize)
// {
//   ROSE_ASSERT(reloc != NULL);
//   ROSE_ASSERT(symbol != NULL);
//   SgAsmGenericSection *sourceSection = NULL;
//   rose_addr_t symbol_va = symbolValue(symbol,symbolMap,extentSortedSections,sourceSection);
//
//   const SgAsmElfSection* relocSection=SageInterface::getEnclosingNode<SgAsmElfSection>(reloc);
//   ROSE_ASSERT(relocSection != NULL);
//
//   const rose_addr_t reloc_va = reloc->get_r_offset() + relocSection->get_base_va();
//
//   SgAsmElfSection* targetSection = chooseSectionAtAddress(reloc_va,extentSortedSections);
//   ROSE_ASSERT(targetSection != NULL);
//
//   // the relocation should come from the same SgAsmFile as the target, we use base_va as a proxy for this
//   ROSE_ASSERT(relocSection->get_base_va() == targetSection->get_base_va());
//
//   /// offset from start of section
//   rose_addr_t relocSectionOffset = reloc_va - targetSection ->get_mapped_actual_va();
//
//   SgFileContentList contents = targetSection->get_data();
//   rose_addr_t reloc_value = symbol_va + reloc->get_r_addend();
//   if(addrSize == 4){
//     ROSE_ASSERT(symbol_va <= UINT_MAX);
//     uint32_t reloc_value_32 = reloc_value;
//     memcpy(&(contents.at(relocSectionOffset)),&reloc_value,addrSize);
//   }
//   else if(addrSize == 8){
//     // TODO Throw some asserts around this
//     memcpy(&(contents.at(relocSectionOffset)),&symbol_va,addrSize);
//   }
//
//   if(get_verbose() > 0){
//     const char* targetSectionName = targetSection->get_name()->c_str();
//     const char* sourceSectionName = "";
//     if(sourceSection)
//       sourceSectionName = sourceSection->get_name()->c_str();
//
//     printf("*0x%08"PRIx64"[%s]=0x%08"PRIx64"[%s]\n",reloc_va,targetSectionName,symbol_va,sourceSectionName);
//   }
// }
//
// void
// relocate_386_RELATIVE(const SgAsmElfRelocEntry* reloc,
//                    const SgAsmElfSymbol* symbol,//may be null
//                    const SymbolMap &symbolMap,
//                    const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   const SgAsmElfSection* relocSection=SageInterface::getEnclosingNode<SgAsmElfSection>(reloc);
//   ROSE_ASSERT(relocSection != NULL);
//
//   const rose_addr_t reloc_addr_va = reloc->get_r_offset() + relocSection->get_base_va();
//
//   SgAsmElfSectionPtrList targetSections = removeSegments(sectionsOfAddress(reloc_addr_va,extentSortedSections));
//   ROSE_ASSERT(targetSections.size() == 1);
//
//   SgAsmElfSection* targetSection = targetSections.front();
//   ROSE_ASSERT(relocSection->get_base_va() == targetSection->get_base_va());
//
//   // The "addend" is actually a pointer to unrelocated data.
//   //   So, we find its relocated address, and set *reloc_va = reloc_data
//   const rose_addr_t reloc_data_va = reloc->get_r_addend() + relocSection->get_base_va();
//   std::vector<SgAsmElfSection*> dataSections = removeSegments(sectionsOfAddress(reloc_data_va,extentSortedSections));
//   if(  dataSections.size() != 1)
//   {
//     dataSections = sectionsOfAddress(reloc_data_va,extentSortedSections);
//     dataSections = removeSegments(dataSections);
//   }
//   ROSE_ASSERT(dataSections.size() == 1);
//   SgAsmElfSection* dataSection = dataSections.front();
//
//   ROSE_ASSERT(relocSection->get_base_va() == dataSection->get_base_va());
//
//   rose_addr_t relocSectionAddrOffset = reloc_addr_va - targetSection->get_mapped_actual_va();
//   SgFileContentList targetMemBase = targetSection->get_data();
//
//   unsigned char* targetMem = &(targetMemBase.at(relocSectionAddrOffset));// size is 8?
//
//   rose_addr_t relocSectionDataOffset = reloc_data_va - dataSection->get_mapped_actual_va();
//   const SgFileContentList dataMemBase = dataSection->get_data();
//   const unsigned char* dataMem = &(dataMemBase.at(relocSectionDataOffset));// size is 8?
//
//   /*
//   std::cout << " R_386_RELATIVE: ";
//   std::cout << "*"
//          << std::hex << reloc_addr_va << "[" << targetSection->get_name()->c_str() << "] = "
//          << std::hex << reloc_data_va << "[" << dataSection->get_name()->c_str() << "]" ;
//   */
//   //std::cout << std::endl;
//
// }
//
// void processOneRelocation(RelocationEntry relocation,
//                        const SymbolMap& symbolMap,
//                        const SgAsmElfSectionPtrList& extentSortedSections)
// {
//     SgAsmElfRelocEntry* reloc = relocation.reloc;
//     SgAsmElfSymbol* symbol = relocation.symbol;
//     SgAsmGenericSection* section = SageInterface::getEnclosingNode<SgAsmGenericSection>(reloc);
//     ROSE_ASSERT(reloc != NULL);
//     ROSE_ASSERT(symbol != NULL);
//     ROSE_ASSERT(section != NULL);
//
//     SgAsmGenericHeader* header = SageInterface::getEnclosingNode<SgAsmGenericHeader>(reloc);
//     ROSE_ASSERT(header != NULL);// need the header to determine ISA
//     SgAsmGenericHeader::InsSetArchitecture isa = header->get_isa();
//
//     const rose_addr_t reloc_va = reloc->get_r_offset() + section->get_base_va();
//
//     string symbolName = symbol->get_name()->c_str();// symbolName may be empty
//     if(get_verbose())
//     {
//       std::string relocStr;
//       if(header)
//      relocStr = reloc->to_string(reloc->get_type(),isa);
//       printf("%16s '%25s' 0x%016x",relocStr.c_str(),symbolName.c_str(),reloc_va);
//     }
//
//     // TODO support other than x86
//     switch(isa & SgAsmGenericHeader::ISA_FAMILY_MASK){
//       case SgAsmGenericHeader::ISA_IA32_Family:
//      switch(reloc->get_type()){
//        case SgAsmElfRelocEntry::R_386_JMP_SLOT:relocate_X86_JMP_SLOT(reloc,symbol,symbolMap,extentSortedSections,4);break;
//        case SgAsmElfRelocEntry::R_386_GLOB_DAT:relocate_X86_GLOB_DAT(reloc,symbol,symbolMap,extentSortedSections,4);break;
//        case SgAsmElfRelocEntry::R_386_32:      relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;
//        case SgAsmElfRelocEntry::R_386_RELATIVE:relocate_386_RELATIVE(reloc,symbol,symbolMap,extentSortedSections);break;
//        case SgAsmElfRelocEntry::R_386_TLS_TPOFF:
//        case SgAsmElfRelocEntry::R_386_TLS_IE:
//        case SgAsmElfRelocEntry::R_386_TLS_GOTIE:
//        case SgAsmElfRelocEntry::R_386_TLS_LE:
//        case SgAsmElfRelocEntry::R_386_TLS_GD:
//        case SgAsmElfRelocEntry::R_386_TLS_LDM:
//        case SgAsmElfRelocEntry::R_386_TLS_GD_32:
//        case SgAsmElfRelocEntry::R_386_TLS_GD_PUSH:
//        case SgAsmElfRelocEntry::R_386_TLS_GD_CALL:
//        case SgAsmElfRelocEntry::R_386_TLS_GD_POP:
//        case SgAsmElfRelocEntry::R_386_TLS_LDM_32:
//        case SgAsmElfRelocEntry::R_386_TLS_LDM_PUSH:
//        case SgAsmElfRelocEntry::R_386_TLS_LDM_CALL:
//        case SgAsmElfRelocEntry::R_386_TLS_LDM_POP:
//        case SgAsmElfRelocEntry::R_386_TLS_LDO_32:
//        case SgAsmElfRelocEntry::R_386_TLS_IE_32:
//        case SgAsmElfRelocEntry::R_386_TLS_LE_32:
//        case SgAsmElfRelocEntry::R_386_TLS_DTPMOD32:
//        case SgAsmElfRelocEntry::R_386_TLS_DTPOFF32:
//        case SgAsmElfRelocEntry::R_386_TLS_TPOFF32:
//          printf("%s%s\n", "<unresolved>: Thread Local Storage not supported",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str()); break;
//        default:
//          printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());
//      };
//      break;
//       case SgAsmGenericHeader::ISA_X8664_Family:
//      switch(reloc->get_type()){
//        case SgAsmElfRelocEntry::R_X86_64_JUMP_SLOT:relocate_X86_JMP_SLOT(reloc,symbol,symbolMap,extentSortedSections,8);break;
//        case SgAsmElfRelocEntry::R_X86_64_GLOB_DAT:relocate_X86_GLOB_DAT(reloc,symbol,symbolMap,extentSortedSections,8);break;
//        case SgAsmElfRelocEntry::R_X86_64_32:      relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;// TODO should zero extend symbol value
//        case SgAsmElfRelocEntry::R_X86_64_32S:     relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,4);break;// TODO should sign extend symbol value
//        case SgAsmElfRelocEntry::R_X86_64_64:      relocate_386_32      (reloc,symbol,symbolMap,extentSortedSections,8);break;
//        case SgAsmElfRelocEntry::R_X86_64_RELATIVE:relocate_386_RELATIVE(reloc,symbol,symbolMap,extentSortedSections);break;
//        default:
//          printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());      
//      };
//      break;
//       default:
//      printf("%s%s\n","<unresolved>:",SgAsmElfRelocEntry::to_string(reloc->get_type(),isa).c_str());
//     };
// }
//
// void processRelocations(const RelocationEntryList& relocations,
//                      const SymbolMap& symbolMap,
//                      const SgAsmElfSectionPtrList& extentSortedSections)
// {
//   RelocationEntryList::const_iterator relocationIter = relocations.begin();
//   for(; relocationIter != relocations.end(); ++relocationIter){
//     processOneRelocation(*relocationIter,symbolMap,extentSortedSections);
//   }
// }
//
// #endif
