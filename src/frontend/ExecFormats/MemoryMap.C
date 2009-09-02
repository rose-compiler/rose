#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* See header file for full documentation */

rose_addr_t
MemoryMap::MapElement::get_va_offset(rose_addr_t va) const
{
    if (va<get_va() || va>=get_va()+get_size())
        throw NotMapped(NULL, va);
    return get_offset() + (va - get_va());
}

bool
MemoryMap::consistent(const MapElement &a, const MapElement &b)
{
    if (a.is_anonymous() && b.is_anonymous()) {
        return true;
    } else if (a.is_anonymous() || b.is_anonymous()) {
        return false;
    } else {
        return a.va-b.va == a.offset-b.offset;
    }
}

void
MemoryMap::insertMappedSections(SgAsmGenericHeader *header)
{
    SgAsmGenericSectionPtrList sections = header->get_mapped_sections();
    for (SgAsmGenericSectionPtrList::iterator i=sections.begin(); i!=sections.end(); ++i)
        insert(*i);
    if (header->is_mapped())
        insert(header);
}

void
MemoryMap::insert(SgAsmGenericSection *section)
{
    SgAsmGenericHeader *header = section->get_header();
    rose_addr_t base_va = header->get_base_va();
    if (!section->is_mapped() || 0==section->get_size())
        return;
    rose_addr_t va = base_va + section->get_mapped_rva();

#if 0 /*DEBUGGING*/
    fprintf(stderr, "MemoryMap::insert(section [%d] \"%s\" at rva 0x%08"PRIx64"-0x%08"PRIx64")\n",
            section->get_id(), section->get_name()->c_str(), va, va+section->get_mapped_size());
#endif

    if (section->get_mapped_rva()==0 && section->get_rose_mapped_rva()>0)
        va = base_va + section->get_rose_mapped_rva();
    MapElement add(va, section->get_size(), section->get_offset());

    insert(add);
}

void
MemoryMap::insert(MapElement add)
{
    if (add.size==0)
        return;

    /* Remove existing elements that are contiguous with or overlap with the new element, extending the new element to cover
     * the removed element. We also check the consistency of the mapping: one virtual address cannot be mapped to two or more
     * file locations. */
    std::vector<MapElement>::iterator i=elements.begin();
    while (i!=elements.end()) {
        MapElement &old = *i;
        if (old.va+old.size < add.va || old.va > add.va+add.size) {
            /* Existing element is left or right of new one and not contiguous with it. */
            ++i;
        } else if (add.va >= old.va && add.va+add.size <= old.va+old.size) {
            /* New element is contained within (or congruent to) an existing element. */
            if (!consistent(add, old))
                throw Inconsistent(this, add, old); /*note, "this" might have already been modified*/
            return;
        } else if (old.va >= add.va && old.va+old.size <= add.va+add.size) {
            /* Existing element is contained within the new element. */
            if (!consistent(old, add))
                throw Inconsistent(this, add, old); /*note, "this" might have already been modified*/
            elements.erase(i);
        } else if (add.va+add.size == old.va) {
            /* New element is left contiguous with existing element. */
            if (consistent(old, add)) {
                add.size += old.size;
                elements.erase(i);
            } else {
                ++i;
            }
        } else if (old.va+old.size == add.va) {
            /* New element is right contiguous with existing element. */
            if (consistent(add, old)) {
                add.size += old.size;
                add.va = old.va;
                add.offset = old.offset;
                elements.erase(i);
            } else {
                ++i;
            }
        } else if (add.va < old.va) {
            /* New element overlaps left part of existing element. */
            if (!consistent(old, add))
                throw Inconsistent(this, add, old); /*note, "this" might have already been modified*/
            add.size += old.va - add.va;
            add.va = old.va;
            add.offset = old.offset;
            elements.erase(i);
        } else {
            /* New element overlaps right part of existing element. */
            if (!consistent(add, old))
                throw Inconsistent(this, add, old); /*note, "this" might have already been modified*/
            add.size = add.va+add.size - old.va;
            add.va = old.va;
            add.offset = old.offset;
            elements.erase(i);
        }
    }

    /* Insert the new element */
    elements.push_back(add);
    sorted = false;
}

void
MemoryMap::erase(SgAsmGenericSection *section)
{
    SgAsmGenericHeader *header = section->get_header();
    rose_addr_t base_va = header->get_base_va();

    if (!section->is_mapped() || 0==section->get_size())
        return;
    rose_addr_t va = base_va + section->get_mapped_rva();

#if 0 /*DEBUGGING*/
    fprintf(stderr, "MemoryMap::erase(section [%d] \"%s\" at va 0x%08"PRIx64"-0x%08"PRIx64")\n",
            section->get_id(), section->get_name()->c_str(), va, va+section->get_mapped_size());
#endif

    if (section->get_mapped_rva()==0 && section->get_rose_mapped_rva()>0)
        va = base_va + section->get_rose_mapped_rva();
    MapElement me(va, section->get_size(), section->get_offset());

    erase(me);
}

void
MemoryMap::erase(MapElement me)
{
    if (me.size==0)
        return;

    /* Remove existing elements that overlap with the erasure area, reducing their size to the part that doesn't overlap, and
     * then add the non-overlapping parts back at the end. */
    std::vector<MapElement>::iterator i=elements.begin();
    std::vector<MapElement> saved;
    while (i!=elements.end()) {
        MapElement &old = *i;
        if (me.va+me.size <= old.va || old.va+old.size <= me.va) {
            /* Non overlapping */
            ++i;
            continue;
        }

        if (me.va > old.va) {
            /* Erasure begins right of existing element. */
            MapElement tosave = old;
            tosave.size = me.va-old.va;
            saved.push_back(tosave);
        }
        if (me.va+me.size < old.va+old.size) {
            /* Erasure ends left of existing element. */
            MapElement tosave = old;
            tosave.va = me.va+me.size;
            tosave.size = (old.va+old.size) - (me.va+me.size);
            if (!tosave.anonymous)
                tosave.offset += (me.va+me.size) - old.va;
            saved.push_back(tosave);
        }
        elements.erase(i);
    }

    /* Now add saved elements back in. */
    for (i=saved.begin(); i!=saved.end(); ++i)
        insert(*i);
}

const MemoryMap::MapElement *
MemoryMap::find(rose_addr_t va) const
{
    if (!sorted) {
        sort(elements.begin(), elements.end());
        sorted = true;
    }

    size_t lo=0, hi=elements.size();
    while (lo<hi) {
        size_t mid=(lo+hi)/2;
        const MapElement &elmt = elements[mid];
        if (va < elmt.va) {
            hi = mid;
        } else if (va >= elmt.va+elmt.size) {
            lo = mid+1;
        } else {
            return &elmt;
        }
    }
    return NULL;
}

const std::vector<MemoryMap::MapElement> &
MemoryMap::get_elements() const {
    if (!sorted) {
        sort(elements.begin(), elements.end());
        sorted = true;
    }
    return elements;
}

size_t
MemoryMap::read(unsigned char *dst_buf, const unsigned char *src_buf, rose_addr_t start_va, size_t desired) const
{
    size_t ncopied = 0;
    while (ncopied < desired) {
        const MemoryMap::MapElement *m = find(start_va);
        if (!m)
            break;
        ROSE_ASSERT(start_va >= m->get_va());
        size_t m_offset = start_va - m->get_va();
        ROSE_ASSERT(m_offset < m->get_size());
        size_t n = std::min(desired-ncopied, m->get_size()-m_offset);
        if (m->is_anonymous()) {
            memset(dst_buf+ncopied, 0, n);
        } else {
            memcpy(dst_buf+ncopied, src_buf+m->get_offset()+m_offset, n);
        }
        ncopied += n;
    }

    memset(dst_buf+ncopied, 0, desired-ncopied);
    return ncopied;
}

void
MemoryMap::dump(FILE *f, const char *prefix) const
{
    if (!sorted) {
        sort(elements.begin(), elements.end());
        sorted = true;
    }
    for (size_t i=0; i<elements.size(); i++) {
        fprintf(f, "%sva 0x%08"PRIx64" + 0x%08zu = 0x%08"PRIx64,
                prefix, elements[i].get_va(), elements[i].get_size(), elements[i].get_va()+elements[i].get_size());
        if (elements[i].is_anonymous()) {
            fprintf(f, " anonymous\n");
        } else {
            fprintf(f, " at offset 0x%08"PRIx64"\n", elements[i].get_offset());
        }
    }
}
