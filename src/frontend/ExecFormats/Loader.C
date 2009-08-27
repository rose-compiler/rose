#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Main documentation is in the class declaration. */

std::vector<Loader*> Loader::p_registered;

/* Class method */
void
Loader::initclass() 
{
    static bool initialized=false;
    if (!initialized) {
        initialized = true;
        register_subclass(new Loader);
        register_subclass(new LoaderELF);
        register_subclass(new LoaderPE);
    }
}

/* Class method to register a loader. */
void
Loader::register_subclass(Loader *loader)
{
    ROSE_ASSERT(loader!=NULL);
    initclass();
    p_registered.push_back(loader);
}

/* Class method to find a loader that can handle a particular file. */
Loader *
Loader::find_loader(SgAsmGenericHeader *header)
{
    for (std::vector<Loader*>::reverse_iterator i=p_registered.rbegin(); i!=p_registered.rend(); i++) {
        if ((*i)->can_handle(header))
            return *i;
    }
    return NULL;
}

/* Returns a map containing the specified sections (if they are mapped). */
MemoryMap *
Loader::map_all_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            return section->is_mapped() ? CONTRIBUTE_ADD : CONTRIBUTE_NONE;
        }
    } s1;
    return create_map(sections, &s1, allow_overmap);
}

/* Returns a map of code-containing sections. A code-containing section is any section that is mapped with execute permission or
 * any section explicitly marked as containing code. Any section that is not code-containing is subtracted from the mapping. */
MemoryMap *
Loader::map_code_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (section->is_mapped() && (section->get_contains_code() || section->get_mapped_xperm())) {
                return CONTRIBUTE_ADD;
            } else if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(sections, &s1, allow_overmap);
}

/* Returns a map of executable sections. Any mapped section that isn't executable is subtracted from the mapping. */
MemoryMap *
Loader::map_executable_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else if (section->get_mapped_xperm()) {
                return CONTRIBUTE_ADD;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(sections, &s1, allow_overmap);
}

/* Returns a map of executable sections. Any section that is not executable is removed from the mapping. */
MemoryMap *
Loader::map_writable_sections(const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else if (section->get_mapped_wperm()) {
                return CONTRIBUTE_ADD;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(sections, &s1, allow_overmap);
}

/* Align section addresses and sizes */
void
Loader::align_values(SgAsmGenericSection *section,
                     rose_addr_t *va/*out*/, rose_addr_t *size/*out*/, rose_addr_t *offset/*out*/)
{
    ROSE_ASSERT(section->is_mapped());
    *va = section->get_header()->get_base_va() + section->get_mapped_rva();
    if (section->get_mapped_rva()==0 && section->get_rose_mapped_rva()>0)
        *va += section->get_rose_mapped_rva();
    *size = section->get_size();
    *offset = section->get_offset();
    
    rose_addr_t ma = section->get_mapped_alignment();
    if (ma>0) {
        rose_addr_t n = ALIGN_DN(*va, section->get_mapped_alignment());
        *size += *va - n;
        *va = n;
    }
    
    rose_addr_t fa = section->get_file_alignment();
    if (fa>0) {
        *offset = ALIGN_DN(*offset, fa);
    }
}

/* Returns sections in mapping order. */
SgAsmGenericSectionPtrList
Loader::order_sections(const SgAsmGenericSectionPtrList &sections)
{
    SgAsmGenericSectionPtrList retval;
    for (size_t i=0; i<sections.size(); i++) {
        if (!sections[i]->get_synthesized()) {
            retval.push_back(sections[i]);
        }
    }
    return sections;
}

/* Uses a selector to determine what sections should be mapped. */
MemoryMap *
Loader::create_map(const SgAsmGenericSectionPtrList &unordered_sections, Selector *selector, bool allow_overmap)
{
    MemoryMap *map = new MemoryMap();
    SgAsmGenericSectionPtrList sections = order_sections(unordered_sections);
    if (p_debug)
        fprintf(p_debug, "Loader: creating memory map...\n");
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmGenericSection *section = sections[i];
        Contribution contrib = selector->contributes(section);
        if (CONTRIBUTE_NONE==contrib)
            continue;
        rose_addr_t va=0, offset=0, size=0;
        align_values(section, &va, &size, &offset);

        if (p_debug) {
            fprintf(p_debug, "  %smapping section [%d] \"%s\"\n",
                    CONTRIBUTE_SUB==contrib ? "un" : "",
                    section->get_id(), section->get_name()->c_str());
            fprintf(p_debug, "    Section specified: RVA 0x%08"PRIx64", offset 0x%08"PRIx64", size 0x%"PRIx64"\n", 
                    section->get_mapped_rva(), section->get_offset(), section->get_size());
            fprintf(p_debug, "    Aligned values:     VA 0x%08"PRIx64", offset 0x%08"PRIx64", size 0x%"PRIx64"\n", 
                    va, offset, size);
        }
        
        if (allow_overmap || CONTRIBUTE_SUB==contrib)
            map->erase(MemoryMap::MapElement(va, size, offset));
        if (CONTRIBUTE_ADD==contrib)
            map->insert(MemoryMap::MapElement(va, size, offset));
    }
    if (p_debug) {
        fprintf(p_debug, "Loader: created map:\n");
        map->dump(p_debug, "    ");
    }

    return map;
}
