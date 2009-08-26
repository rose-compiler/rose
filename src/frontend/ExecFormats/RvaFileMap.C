#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* See header file for full documentation */

rose_addr_t
RvaFileMap::MapElement::get_rva_offset(rose_addr_t rva) const
{
    if (rva<get_rva() || rva>=get_rva()+get_size())
        throw NotMapped(NULL, rva);
    return get_offset() + (rva - get_rva());
}

bool
RvaFileMap::consistent(const MapElement &a, const MapElement &b) 
{
    return a.rva-b.rva == a.offset-b.offset;
}

void
RvaFileMap::insertMappedSections(SgAsmGenericHeader *header) 
{
    SgAsmGenericSectionPtrList sections = header->get_mapped_sections();
    for (SgAsmGenericSectionPtrList::iterator i=sections.begin(); i!=sections.end(); ++i)
        insert(*i);
    if (header->is_mapped())
        insert(header);
}

void
RvaFileMap::insert(SgAsmGenericSection *section)
{
    SgAsmGenericHeader *header = section->get_header();
    if (header) {
        ROSE_ASSERT(0==base_va || base_va==header->get_base_va());
        base_va = header->get_base_va();
    }

    if (!section->is_mapped() || 0==section->get_size())
        return;

#if 0 /*DEBUGGING*/
    fprintf(stderr, "RvaFileMap::insert(section [%d] \"%s\" at rva 0x%08"PRIx64"-0x%08"PRIx64")\n", 
            section->get_id(), section->get_name()->c_str(), section->get_mapped_rva(),
            section->get_mapped_rva()+section->get_mapped_size());
#endif

    rose_addr_t rva = section->get_mapped_rva();
    if (!rva && section->get_rose_mapped_rva())
        rva = section->get_rose_mapped_rva();
    MapElement add(rva, section->get_size(), section->get_offset());

    insert(add);
}

void
RvaFileMap::insert(MapElement add)
{
    /* Remove existing elements that are contiguous with or overlap with the new element, extending the new element to cover
     * the removed element. We also check the consistency of the mapping: one virtual address cannot be mapped to two or more
     * file locations. */
    std::vector<MapElement>::iterator i=elements.begin();
    while (i!=elements.end()) {
        MapElement &old = *i;
        if (old.rva+old.size < add.rva || old.rva > add.rva+add.size) {
            /* Existing element is left or right of new one and not contiguous with it. */
            ++i;
        } else if (add.rva >= old.rva && add.rva+add.size <= old.rva+old.size) {
            /* New element is contained within (or congruent to) an existing element. */
            if (!consistent(add, old))
                throw Inconsistent(this, add, old); /*note, "this" might have already been modified*/
            return;
        } else if (old.rva >= add.rva && old.rva+old.size <= add.rva+add.size) {
            /* Existing element is contained within the new element. */
            if (!consistent(old, add))
                throw Inconsistent(this, add, old); /*note, "this" might have already been modified*/
            elements.erase(i);
        } else if (add.rva+add.size == old.rva) {
            /* New element is left contiguous with existing element. */
            if (consistent(old, add)) {
                add.size += old.size;
                elements.erase(i);
            } else {
                ++i;
            }
        } else if (old.rva+old.size == add.rva) {
            /* New element is right contiguous with existing element. */
            if (consistent(add, old)) {
                add.size += old.size;
                add.rva = old.rva;
                add.offset = old.offset;
                elements.erase(i);
            } else {
                ++i;
            }
        } else if (add.rva < old.rva) {
            /* New element overlaps left part of existing element. */
            if (!consistent(old, add))
                throw Inconsistent(this, add, old); /*note, "this" might have already been modified*/
            add.size += old.rva - add.rva;
            add.rva = old.rva;
            add.offset = old.offset;
            elements.erase(i);
        } else {
            /* New element overlaps right part of existing element. */
            if (!consistent(add, old))
                throw Inconsistent(this, add, old); /*note, "this" might have already been modified*/
            add.size = add.rva+add.size - old.rva;
            add.rva = old.rva;
            add.offset = old.offset;
            elements.erase(i);
        }
    }
    
    /* Insert the new element */
    elements.push_back(add);
    sorted = false;
}

void
RvaFileMap::erase(SgAsmGenericSection *section)
{
    SgAsmGenericHeader *header = section->get_header();
    if (header) {
        ROSE_ASSERT(0==base_va || base_va==header->get_base_va());
        base_va = header->get_base_va();
    }

    if (!section->is_mapped() || 0==section->get_size())
        return;

#if 0 /*DEBUGGING*/
    fprintf(stderr, "RvaFileMap::erase(section [%d] \"%s\" at rva 0x%08"PRIx64"-0x%08"PRIx64")\n", 
            section->get_id(), section->get_name()->c_str(), section->get_mapped_rva(),
            section->get_mapped_rva()+section->get_mapped_size());
#endif

    rose_addr_t rva = section->get_mapped_rva();
    if (!rva && section->get_rose_mapped_rva())
        rva = section->get_rose_mapped_rva();
    MapElement me(rva, section->get_size(), section->get_offset());

    erase(me);
}

void
RvaFileMap::erase(MapElement me)
{
    /* Remove existing elements that overlap with the erasure area, reducing their size to the part that doesn't overlap, and
     * then add the non-overlapping parts back at the end. */
    std::vector<MapElement>::iterator i=elements.begin();
    std::vector<MapElement> saved;
    while (i!=elements.end()) {
        MapElement &old = *i;
        if (me.rva+me.size <= old.rva || old.rva+old.size <= me.rva) {
            /* Non overlapping */
            ++i;
            continue;
        }
        
        if (me.rva > old.rva) {
            /* Erasure begins right of existing element. */
            saved.push_back(MapElement(old.rva, me.rva-old.rva, old.offset));
        }
        if (me.rva+me.size < old.rva+old.size) {
            /* Erasure ends left of existing element. */
            saved.push_back(MapElement(me.rva+me.size, old.rva+old.size-(me.rva+me.size), old.offset+me.rva+me.size-old.rva));
        }
        elements.erase(i);
    }
    
    /* Now add saved elements back in. */
    for (i=saved.begin(); i!=saved.end(); ++i)
        insert(*i);
}
                
const RvaFileMap::MapElement *
RvaFileMap::findRVA(rose_addr_t rva) const
{
    if (!sorted) {
        sort(elements.begin(), elements.end());
        sorted = true;
    }
    
    size_t lo=0, hi=elements.size();
    while (lo<hi) {
        size_t mid=(lo+hi)/2;
        const MapElement &elmt = elements[mid];
        if (rva < elmt.rva) {
            hi = mid;
        } else if (rva >= elmt.rva+elmt.size) {
            lo = mid+1;
        } else {
            return &elmt;
        }
    }
    return NULL;
}

const std::vector<RvaFileMap::MapElement> &
RvaFileMap::get_elements() const {
    if (!sorted) {
        sort(elements.begin(), elements.end());
        sorted = true;
    }
    return elements;
}

size_t
RvaFileMap::readRVA(unsigned char *dst_buf, const unsigned char *src_buf, rose_addr_t start_rva, size_t desired) const
{
    size_t ncopied = 0;
    while (ncopied < desired) {
        const RvaFileMap::MapElement *m = findRVA(start_rva);
        if (!m)
            break;
        ROSE_ASSERT(start_rva >= m->get_rva());
        size_t m_offset = start_rva - m->get_rva();
        ROSE_ASSERT(m_offset < m->get_size());
        size_t n = std::min(desired-ncopied, m->get_size()-m_offset);
        memcpy(dst_buf+ncopied, src_buf+m->get_offset()+m_offset, n);
        ncopied += n;
    }

    memset(dst_buf+ncopied, 0, desired-ncopied);
    return ncopied;
}
    

size_t
RvaFileMap::readVA(unsigned char *dst_buf, const unsigned char *src_buf, rose_addr_t start_va, size_t desired) const
{
    ROSE_ASSERT(dst_buf!=NULL);
    ROSE_ASSERT(start_va >= get_base_va());
    rose_addr_t start_rva = start_va - get_base_va();
    return readRVA(dst_buf, src_buf, start_rva, desired);
}

void
RvaFileMap::dump(FILE *f, const char *prefix) const
{
    if (!sorted) {
        sort(elements.begin(), elements.end());
        sorted = true;
    }
    for (size_t i=0; i<elements.size(); i++) {
        fprintf(f, "%srva 0x%08"PRIx64" + 0x%08zu = 0x%08"PRIx64" at offset 0x%08"PRIx64"\n",
                prefix, elements[i].get_rva(), elements[i].get_size(), elements[i].get_rva()+elements[i].get_size(),
                elements[i].get_offset());
    }
}
