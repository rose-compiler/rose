/* The ExtentMap class. This class is similar std::map<rose_addr_t,rose_addr_t> where the two addresses are the starting
 * offset and size.  The main difference is that if two adjacent extents are added to the map they will be condensed into a
 * single extent.  This class is used to keep track of what parts of a binary file have been parsed, and is also used to
 * manage string table free lists, among other things. */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Class method comparing two extents. The return value is one of the following letters, depending on how extent A is related
 *  to extent B:
 *     C (congruent):  A and B are congruent
 *     L (left):       A is left of B
 *     R (right):      A is right of B
 *     O (outer):      A contains B, but A and B are not congruent
 *     I (inner):      A is contained by B, but A and B are not congruent
 *     B (beginning):  A overlaps with the beginning of B but does not contain B
 *     E (ending):     A overlaps with the end of B but does not contain B */
char
ExtentMap::category(const ExtentPair &a, const ExtentPair &b)
{
    if (a.first==b.first && a.second==b.second)
        return 'C';
    if (a.first+a.second <= b.first)
        return 'L';
    if (a.first >= b.first+b.second)
        return 'R';
    if (a.first <= b.first && a.first+a.second >= b.first+b.second)
        return 'O';
    if (a.first >= b.first && a.first+a.second <= b.first+b.second)
        return 'I';
    if (a.first <= b.first) /*already know a.first+a.second > b.first*/
        return 'B';
    return 'E';
}

/** Return an extent map which contains all extents in (offset,size) that are not in "this" extent map. */
ExtentMap
ExtentMap::subtract_from(rose_addr_t offset, rose_addr_t size) const
{
    ExtentMap result;
    for (ExtentMap::const_iterator i=begin(); i!=end() && offset+size>(*i).first; ++i) {
        ROSE_ASSERT((*i).second > 0);
        if (0==size) {
            /* We've added everything to the result */
            return result;
        } else if ((*i).first >= offset+size) {
            /* Subtrahend extent is to the right of offset,size */
            break;
        } else if ((*i).first+(*i).second <= offset) {
            /* Subtrahend extent is to the left of offset,size */
        } else if ((*i).first+(*i).second >= offset+size) {
            if ((*i).first <= offset) {
                /* Subtrahend extent contains all of offset,size */
                return result;
            } else {
                /* Subtrahend extent starts inside offset,size and ends to the right of offset,size. Add left part of offset,
                 * size. */
                size = (*i).first - offset;
                break;
            }
        } else if ((*i).first > offset) {
            /* Subtrahend extent is inside offset,size. Add left part, save right part for another pass through the loop. */
            result.super::insert(value_type(offset, (*i).first - offset));
            size -= (*i).first + (*i).second - offset;
            offset = (*i).first + (*i).second;
        } else {
            /* Subtrahend extent starts left of offset,size and ends inside offset,size. Discard the left part of offset,size. */
            size -= (*i).first + (*i).second - offset;
            offset = (*i).first + (*i).second;
        }
    }
    if (size>0)
        result.super::insert(value_type(offset,size));
    return result;
}

/** Adds the specified extent to the map of extents. Coalesce adjacent entries in the map. */
void
ExtentMap::insert(rose_addr_t offset, rose_addr_t size, rose_addr_t align_lo/*=1*/, rose_addr_t align_hi/*=1*/)
{

    /* Adjust starting and ending offsets to alignment constraints. */
    if (align_lo>1) {
        rose_addr_t new_offset = ALIGN_DN(offset, align_lo);
        if (new_offset != offset) {
            size += offset - new_offset;
            offset = new_offset;
        }
    }
    if (align_hi>1) {
        rose_addr_t new_hi = ALIGN_UP(offset+size, align_hi);
        if (new_hi != offset+size)
            size = new_hi - offset;
    }

    /* Insert into map */
    if (0==size) return;
    ExtentMap to_add = subtract_from(offset, size);
    for (iterator i=to_add.begin(); i!=to_add.end(); ++i) {
        iterator right = find((*i).first+(*i).second);
        if (right!=end()) {
            (*i).second += (*right).second;
            super::erase(right);
        }

        iterator inserted = super::insert(begin(), *i);
        if (inserted!=begin()) {
            iterator left = inserted;
            --left;
            if ((*left).first+(*left).second == (*i).first) {
                (*left).second += (*i).second;
                super::erase(inserted);
            }
        }
    }
}

/** Inserts contents of one extent map into another */
void
ExtentMap::insert(const ExtentMap &map)
{
    for (ExtentMap::const_iterator i=map.begin(); i!=map.end(); ++i) {
        insert(*i);
    }
}

/** Returns all extents that (partially) overlap with the specified area */
ExtentMap
ExtentMap::overlap_with(rose_addr_t offset, rose_addr_t size) const
{
    ExtentMap result;
    for (const_iterator i=begin(); i!=end(); ++i) {
        if ((*i).first <= offset+size && (*i).first+(*i).second > offset)
            result.super::insert(value_type((*i).first, (*i).second));
    }
    return result;
}

/** Removes the specified extent from the map of extents. */
void
ExtentMap::erase(rose_addr_t offset, rose_addr_t size)
{
    ExtentMap candidates = overlap_with(offset, size);
    for (iterator i=candidates.begin(); i!=candidates.end(); ++i) {
        iterator candidate = find((*i).first);
        ROSE_ASSERT(candidate!=end());
        if (offset <= (*candidate).first) {
            if (offset+size >= (*i).first + (*i).second) {
                /* Erase entire candidate */
                super::erase(candidate);
            } else {
                /* Erase left part of candidate */
                super::erase(candidate);
                super::insert(value_type(offset+size, (*i).first+(*i).second - (offset+size)));
            }
        } else if (offset+size >= (*i).first + (*i).second) {
            /* Erase right part of candidate */
            (*candidate).second = offset - (*i).first;
        } else {
            /* Erase middle of candidate */
            (*candidate).second = offset - (*i).first;
            super::insert(value_type(offset+size, (*i).first+(*i).second - (offset+size)));
        }
    }
}

/** Removes the specified extents from this extent. */
void
ExtentMap::erase(const ExtentMap& subtrahend)
{
    for (const_iterator i=subtrahend.begin(); i!=subtrahend.end(); ++i)
        erase((*i).first, (*i).second);
}

/** Return the extent that's the closest match in size without removing it from the map. If two extents tie for the best fit then
 *  return the one with the lower offset. Returns map.end() on failure. */
ExtentMap::iterator
ExtentMap::best_fit(rose_addr_t size)
{
    iterator best = end();
    for (iterator i=begin(); i!=end(); ++i) {
        if ((*i).second==size)
            return i;
        if ((*i).second > size && (best==end() || (*i).second < (*best).second))
            best = i;
    }
    return best;
}

/** Return the extent with the highest offset. */
ExtentMap::iterator
ExtentMap::highest_offset()
{
    ExtentMap::iterator ret = end();
    if (size()>0) --ret;
    return ret;
}

/** Allocate an extent of the specified size (best fit first) from the extent map, removing the returned extent from the map. */
ExtentPair
ExtentMap::allocate_best_fit(rose_addr_t size)
{
    iterator bfi = best_fit(size);
    if (bfi==end())
        throw std::bad_alloc();
    ExtentPair saved = *bfi;
    super::erase(bfi);
    if (saved.second>size)
        super::insert(value_type(saved.first+size, saved.second-size));
    return ExtentPair(saved.first, size);
}

/** Allocate an extent of the specified size (first fit) from the extent map, removing the returned extent from the map. */
ExtentPair
ExtentMap::allocate_first_fit(rose_addr_t size)
{
    for (iterator i=begin(); i!=end(); ++i) {
        if ((*i).second >= size) {
            ExtentPair saved = *i;
            super::erase(i);
            if (saved.second > size)
                super::insert(value_type(saved.first+size, saved.second-size));
            return ExtentPair(saved.first, size);
        }
    }
    throw std::bad_alloc();
}

/** Allocate the specified extent, which must be in the free list. */
void
ExtentMap::allocate_at(const ExtentPair &request)
{
    ROSE_ASSERT(subtract_from(request).size()==0); /*entire request should be on free list*/
    erase(request);
}

/** Number of bytes represented by extent. */
size_t
ExtentMap::size() const
{
    size_t retval=0;
    for (const_iterator i=begin(); i!=end(); ++i)
        retval += (*i).second;
    return retval;
}

/** Precipitates individual extents into larger extents by combining individual extents that are separated by an amount less
 *  than or equal to some specified @p reagent value.  Individual elements that would have been adjacent have already
 *  been combined by the other modifying methods (insert, erase, etc). */
void
ExtentMap::precipitate(rose_addr_t reagent)
{
    ExtentMap result;
    for (iterator i=begin(); i!=end(); /*void*/) {
        ExtentPair left = *i++;
        for (/*void*/; i!=end() && left.first+left.second+reagent >= i->first; i++)
            left.second = (i->first + i->second) - left.first;
        result.insert(left);
    }
    *this = result;
}

/** Find the extent pair that contains the specified address. Throw std::bad_alloc() if the address is not found. */
ExtentPair
ExtentMap::find_address(rose_addr_t addr) const
{
    const_iterator i = upper_bound(addr);
    if (i==begin())
        throw std::bad_alloc();
    --i;
    if (i->first+i->second <= addr)
        throw std::bad_alloc();
    return *i;
}

/** Print info about an extent map */
void
ExtentMap::dump_extents(FILE *f, const char *prefix, const char *label, bool pad) const
{
    char p[4096];
    size_t idx=0;
    for (const_iterator i=begin(); i!=end(); ++i, ++idx) {
        if (!label) label = "Extent";
        sprintf(p, "%s%s[%zd]", prefix, label, idx);
        int w = pad ? std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p)) : 1;
        fprintf(f, "%s%-*s = offset 0x%08"PRIx64" (%"PRIu64"),"
                " for 0x%08"PRIx64" (%"PRIu64") byte%s,"
                " ending at 0x%08"PRIx64" (%"PRIu64")\n",
                p, w, "", (*i).first, (*i).first,
                (*i).second, (*i).second, 1==(*i).second?"":"s", 
                (*i).first+(*i).second, (*i).first+(*i).second);
    }
}
