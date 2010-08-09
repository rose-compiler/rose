// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "Loader.h"
/* See header file for full documentation */

rose_addr_t
MemoryMap::MapElement::get_va_offset(rose_addr_t va) const
{
    if (va<get_va() || va>=get_va()+get_size())
        throw NotMapped(NULL, va);
    return get_offset() + (va - get_va());
}

bool
MemoryMap::MapElement::consistent(const MapElement &other) const
{
    if (is_read_only()!=other.is_read_only()) {
        return false;
    } else if (get_mapperms()!=other.get_mapperms()) {
        return false;
    } else if (is_anonymous() && other.is_anonymous()) {
        return true;
    } else if (is_anonymous() || other.is_anonymous()) {
        return false;
    } else {
        return va - other.va == offset - other.offset;
    }
}

void
MemoryMap::MapElement::merge_anonymous(const MapElement &other, size_t oldsize)
{
    /* This element must have already been merged with the other, except for the base ptr */
    ROSE_ASSERT(is_anonymous());
    ROSE_ASSERT(other.is_anonymous());
    ROSE_ASSERT(va<=other.va);
    ROSE_ASSERT(offset<=other.offset);
    ROSE_ASSERT(size>=other.size);

    if (NULL==other.base)
        return;
    
    uint8_t *newbase = new uint8_t[offset+size];
    memset(newbase, 0, offset+size);
    if (base)
        memcpy(newbase+offset, (uint8_t*)base+offset, oldsize);
    memcpy(newbase+offset+(other.va-va), (uint8_t*)other.base+other.offset, other.size);

    if (1==*anonymous)
        delete[] (uint8_t*)base;
    base = newbase;
    *anonymous = 1;
}

void
MemoryMap::MapElement::merge_names(const MapElement &other)
{
    if (name.empty()) {
        set_name(other.get_name());
    } else if (other.name.empty()) {
        /*void*/
    } else {
        set_name(get_name()+"+"+other.get_name());
    }
}

MemoryMap::MapElement &
MemoryMap::MapElement::set_name(const std::string &s)
{
    static const size_t limit = 35;
    name = s;
    if (name.size()>limit)
        name = name.substr(0, limit-3) + "...";
    return *this;
}

bool
MemoryMap::MapElement::merge(const MapElement &other)
{
    size_t oldsize = size;
    if (va+size < other.va || va > other.va+other.size) {
        /* Other element is left or right of this one and not contiguous with it. */
        return false;
    } else if (other.va >= va && other.va+other.size <= va+size) {
        /* Other element is contained within (or congruent to) this element. */
        if (!consistent(other))
            throw Inconsistent(NULL, *this, other);
    } else if (va >= other.va && va+size <= other.va+other.size) {
        /* Other element encloses this element. */
        if (!consistent(other))
            throw Inconsistent(NULL, *this, other);
        offset = other.offset;
        va = other.va;
        base = other.base;
        size = other.size;
        merge_names(other);
    } else if (other.va + other.size == va) {
        /* Other element is left contiguous with this element. */
        if (!consistent(other))
            return false; /*no exception since they don't overlap*/
        size += other.size;
        va = other.va;
        base = other.base;
        offset = other.offset;
        merge_names(other);
    } else if (va + size == other.va) {
        /* Other element is right contiguous with this element. */
        if (!consistent(other))
            return false; /*no exception since they don't overlap*/
        size += other.size;
        merge_names(other);
    } else if (other.va < va) {
        /* Other element overlaps left part of this element. */
        if (!consistent(other))
            throw Inconsistent(NULL, *this, other);
        size += va - other.va;
        va = other.va;
        base = other.base;
        offset = other.offset;
        merge_names(other);
    } else {
        /* Other element overlaps right part of this element. */
        if (!consistent(other))
            throw Inconsistent(NULL, *this, other);
        size = (other.va + other.size) - va;
        merge_names(other);
    }

    /* Adjust backing store for anonymous elements. This is necessary because two anonymous elements are consistent if they
     * are adjacent or overlap, even if their base addresses are different or their offsets are inconsistent. By merging
     * anonymous elements we can reduce the number of memory allocations that are necessary. */
    if (is_anonymous())
        merge_anonymous(other, oldsize);

    return true;
}

void
MemoryMap::insert(MapElement add)
{
    if (add.size==0)
        return;

    try {
        /* Remove existing elements that are contiguous with or overlap with the new element, extending the new element to cover
         * the removed element. We also check the consistency of the mapping and throw an exception if the new element overlaps
         * inconsistently with an existing element. */
        std::vector<MapElement>::iterator i=elements.begin();
        while (i!=elements.end()) {
            MapElement &old = *i;
            if (add.merge(old)) {
                elements.erase(i);
            } else {
                ++i;
            }
        }

        /* Insert the new element */
        assert(NULL==find(add.va));
        elements.push_back(add);
        sorted = false;
    } catch (Exception &e) {
        e.map = this;
        throw e;
    }
}
    

void
MemoryMap::erase(MapElement me)
{
    if (me.size==0)
        return;

    try {
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
                tosave.offset += (me.va+me.size) - old.va;
                saved.push_back(tosave);
            }
            elements.erase(i);
        }

        /* Now add saved elements back in. */
        for (i=saved.begin(); i!=saved.end(); ++i)
            insert(*i);
    } catch(Exception &e) {
        e.map = this;
        throw e;
    }
}

const MemoryMap::MapElement *
MemoryMap::find(rose_addr_t va) const
{
    if (!sorted) {
	std::sort(elements.begin(), elements.end());
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

rose_addr_t
MemoryMap::find_free(rose_addr_t start_va, size_t size, rose_addr_t alignment) const
{
    if (!sorted) {
	std::sort(elements.begin(), elements.end());
        sorted = true;
    }

    start_va = ALIGN_UP(start_va, alignment);
    for (size_t i=0; i<elements.size(); i++) {
        const MapElement &me = elements[i];
        if (me.va + me.size <= start_va)
            continue;
        if (me.va > start_va &&  me.va - start_va >= size)
            break;
        rose_addr_t x = start_va;
        start_va = ALIGN_UP(me.va + me.size, alignment);
        if (start_va<x)
            throw NoFreeSpace(this, size);
    }

    if (start_va+size < start_va)
        throw NoFreeSpace(this, size);

    return start_va;
}

const std::vector<MemoryMap::MapElement> &
MemoryMap::get_elements() const {
    if (!sorted) {
	std::sort(elements.begin(), elements.end());
        sorted = true;
    }
    return elements;
}

void
MemoryMap::prune(bool(*predicate)(const MapElement&))
{
    std::vector<MapElement> keep;
    for (size_t i=0; i<elements.size(); i++) {
        if (!predicate(elements[i]))
            keep.push_back(elements[i]);
    }
    elements = keep;
}

size_t
MemoryMap::read(void *dst_buf, rose_addr_t start_va, size_t desired) const
{
    size_t ncopied = 0;
    while (ncopied < desired) {
        const MemoryMap::MapElement *m = find(start_va);
        if (!m || 0==(m->get_mapperms() & MM_PROT_READ))
            break;
        ROSE_ASSERT(start_va >= m->get_va());
        size_t m_offset = start_va - m->get_va();
        ROSE_ASSERT(m_offset < m->get_size());
        size_t n = std::min(desired-ncopied, m->get_size()-m_offset);
        if (m->is_anonymous() && NULL==m->get_base()) {
            memset((uint8_t*)dst_buf+ncopied, 0, n);
        } else {
            memcpy((uint8_t*)dst_buf+ncopied, (uint8_t*)m->get_base()+m->get_offset()+m_offset, n);
        }
        ncopied += n;
    }

    memset((uint8_t*)dst_buf+ncopied, 0, desired-ncopied);
    return ncopied;
}

size_t
MemoryMap::write(const void *src_buf, rose_addr_t start_va, size_t nbytes) const
{
    size_t ncopied = 0;
    while (ncopied < nbytes) {
        const MemoryMap::MapElement *m = find(start_va);
        if (!m || m->is_read_only() || 0==(m->get_mapperms() & MM_PROT_READ))
            break;
        ROSE_ASSERT(start_va >= m->get_va());
        size_t m_offset = start_va - m->get_va();
        ROSE_ASSERT(m_offset < m->get_size());
        size_t n = std::min(nbytes-ncopied, m->get_size()-m_offset);
        if (m->is_anonymous() && NULL==m->get_base()) {
            ROSE_ASSERT(*m->anonymous==0);
            *(m->anonymous) = 1;
            m->base = new uint8_t[m->get_size()];
            memset(m->base, 0, m->get_size());
        }
        memcpy((uint8_t*)m->get_base()+m->get_offset()+m_offset, (uint8_t*)src_buf+ncopied, n);
        ncopied += n;
    }
    return ncopied;
}

void
MemoryMap::mprotect(const MapElement &region)
{
    /* Check whether the region refers to addresses not in the memory map. */
    ExtentMap e;
    e.insert(ExtentPair(region.get_va(), region.get_size()));
    e.erase(va_extents());
    if (!e.empty())
        throw NotMapped(this, e.begin()->first);

    std::vector<MapElement> created;
    std::vector<MapElement>::iterator i=elements.begin();
    while (i!=elements.end()) {
        MapElement &other = *i;
        if (other.get_va() >= region.get_va()) {
            if (other.get_va()+other.get_size() <= region.get_va()+region.get_size()) {
                /* other is fully contained in (or congruent to) region; change other's permissions */
                other.set_mapperms(region.get_mapperms());
                i++;
            } else if (other.get_va() < region.get_va()+region.get_size()) {
                /* left part of other is contained in region; split other into two parts */
                size_t left_sz = region.get_va() + region.get_size() - other.get_va();
                ROSE_ASSERT(left_sz>0);
                MapElement left = other;
                left.set_size(left_sz);
                left.set_mapperms(region.get_mapperms());
                created.push_back(left);

                size_t right_sz = other.get_size() - left_sz;
                MapElement right = other;
                ROSE_ASSERT(right_sz>0);
                right.set_va(other.get_va() + left_sz);
                right.set_offset(right.get_offset() + left_sz);
                right.set_size(right_sz);
                created.push_back(right);

                elements.erase(i);
            } else {
                /* other is right of region; skip it */
                i++;
            }
        } else if (other.get_va()+other.get_size() <= region.get_va()) {
            /* other is left of desired region; skip it */
            i++;
        } else if (other.get_va()+other.get_size() <= region.get_va() + region.get_size()) {
            /* right part of other is contained in region; split other into two parts */
            size_t left_sz = region.get_va() - other.get_va();
            ROSE_ASSERT(left_sz>0);
            MapElement left = other;
            left.set_size(left_sz);
            created.push_back(left);

            size_t right_sz = other.get_size() - left_sz;
            MapElement right = other;
            right.set_va(other.get_va() + left_sz);
            right.set_offset(right.get_offset() + left_sz);
            right.set_size(right_sz);
            right.set_mapperms(region.get_mapperms());
            created.push_back(right);

            elements.erase(i);
        } else {
            /* other contains entire region and extends left and right; split into three parts */
            size_t left_sz = region.get_va() - other.get_va();
            ROSE_ASSERT(left_sz>0);
            MapElement left = other;
            left.set_size(left_sz);
            created.push_back(left);
            
            size_t mid_sz = region.get_size();
            ROSE_ASSERT(mid_sz>0);
            MapElement mid = other;
            mid.set_va(region.get_va());
            mid.set_offset(mid.get_offset() + left_sz);
            mid.set_size(region.get_size());
            mid.set_mapperms(region.get_mapperms());
            created.push_back(mid);
            
            size_t right_sz = other.get_size() - (left_sz + mid_sz);
            ROSE_ASSERT(right_sz>0);
            MapElement right = other;
            right.set_va(region.get_va()+region.get_size());
            right.set_offset(mid.get_offset() + mid_sz);
            right.set_size(right_sz);
            created.push_back(right);
            
            elements.erase(i);
        }
    }

    elements.insert(elements.end(), created.begin(), created.end());
    sorted = false;
}

ExtentMap
MemoryMap::va_extents() const
{
    ExtentMap retval;
    for (size_t i=0; i<elements.size(); i++) {
        const MapElement& me = elements[i];
        retval.insert(me.get_va(), me.get_size());
    }
    return retval;
}

rose_addr_t
MemoryMap::highest_va() const
{
    if (elements.empty())
        throw NotMapped(this, 0);
    
    if (!sorted) {
	std::sort(elements.begin(), elements.end());
        sorted = true;
    }

    MapElement &me = elements.back();
    ROSE_ASSERT(me.get_size()>0);
    return me.get_va() + me.get_size() - 1;
}

void
MemoryMap::dump(FILE *f, const char *prefix) const
{
    if (!sorted) {
	std::sort(elements.begin(), elements.end());
        sorted = true;
    }

    if (0==elements.size())
        fprintf(f, "%sempty\n", prefix);

    std::map<void*,std::string> bases;
    for (size_t i=0; i<elements.size(); i++) {
        const MapElement &me = elements[i];
        std::string basename;
        std::map<void*,std::string>::iterator found = bases.find(me.get_base());

        /* Convert the base address to a unique name like "aaa", "aab", "aac", etc. This makes it easier to compare outputs
         * from different runs since the base addresses are likely to be different between runs but the names aren't. */   
        if (me.is_anonymous()) {
            basename = "anonymous";
        } else if (NULL==me.get_base()) {
            basename = "base null";
        } else if (found==bases.end()) {
            size_t j = bases.size();
            ROSE_ASSERT(j<26*26*26);
            basename = "base ";
            basename += 'a'+(j/(26*26))%26;
            basename += 'a'+(j/26)%26;
            basename += 'a'+(j%26);
            bases.insert(std::make_pair(me.get_base(), basename));
        } else {
            basename = found->second;
        }


        fprintf(f, "%sva 0x%08"PRIx64" + 0x%08zx = 0x%08"PRIx64" %c%c%c at %-9s + 0x%08"PRIx64,
                prefix, me.get_va(), me.get_size(), me.get_va()+me.get_size(),
                0==(me.get_mapperms()&MM_PROT_READ) ?'-':'r',
                0==(me.get_mapperms()&MM_PROT_WRITE)?'-':'w',
                0==(me.get_mapperms()&MM_PROT_EXEC) ?'-':'x',
                basename.c_str(), elements[i].get_offset());

        if (!me.name.empty())
            fprintf(f, " %s", me.name.c_str());
        
        fputc('\n', f);
    }
}
