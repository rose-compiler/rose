/* Strings. Uniform treatment for strings stored in a binary file and strings generated on the fly. */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/AddressIntervalSet.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

const rose_addr_t
SgAsmGenericString::unallocated = ~(rose_addr_t)0;

std::string
SgAsmGenericString::get_string(bool /*escape*/) const
{
    ROSE_ASSERT(!"should have been pure virtual if ROSETTA supported that.");
    abort();

    // DQ (11/27/2009): MSVC requires a return stmt for any non-void return type of a function.
    return "error in SgAsmGenericString::get_string()";
}

void
SgAsmGenericString::set_string(const std::string&)
{
    ROSE_ASSERT(!"should have been pure virtual if ROSETTA supported that.");
    abort();
}

void
SgAsmGenericString::set_string(rose_addr_t /*offset*/)
{
    ROSE_ASSERT(!"should have been pure virtual if ROSETTA supported that.");
    abort();
}

void
SgAsmGenericString::dump(FILE*, const char */*prefix*/, ssize_t /*idx*/) const
{
    ROSE_ASSERT(!"should have been pure virtual if ROSETTA supported that.");
    abort();
}

SgAsmBasicString::SgAsmBasicString(const std::string &s) {
    initializeProperties();
    p_string = s;
}

SgAsmBasicString::SgAsmBasicString(const char *s) {
    initializeProperties();
    p_string = s;
}

/* Override ROSETTA because generated code doesn't match virtual signature in base class */
std::string
SgAsmBasicString::get_string(bool escape) const
{
    if (escape)
        return escapeString(p_string);
    return p_string;
}
void
SgAsmBasicString::set_string(const std::string &s)
{
    if (p_string!=s)
        set_isModified(true);
    p_string = s;
}
void
SgAsmBasicString::set_string(rose_addr_t offset)
{
    ASSERT_not_implemented("set_string(rose_addr_t offset=" + StringUtility::addrToString(offset) + ")");
}

void
SgAsmBasicString::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sBasicString[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sBasicString.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "value", get_string(true).c_str());
}
    
SgAsmStoredString::SgAsmStoredString(SgAsmGenericStrtab *strtab, rose_addr_t offset) {
    initializeProperties();
    ASSERT_not_null(strtab);
    p_storage = strtab->createStorage(offset, false);
}

SgAsmStoredString::SgAsmStoredString(SgAsmGenericStrtab *strtab, const std::string &s) {
    initializeProperties();
    ASSERT_not_null(strtab);
    p_storage = strtab->createStorage(0, false);
    set_string(s);
}

SgAsmStoredString::SgAsmStoredString(SgAsmStringStorage *storage) {
    initializeProperties();
    p_storage = storage;
}

std::string
SgAsmStoredString::get_string(bool escape) const 
{
    std::string retval = get_storage()->get_string();
    if (escape)
        retval = escapeString(retval);
    return retval;
}

rose_addr_t
SgAsmStoredString::get_offset() const
{
    if (NULL==get_storage())
        return unallocated;
    if (get_storage()->get_offset() == unallocated) {
        SgAsmGenericStrtab *strtab = get_storage()->get_strtab();
        ROSE_ASSERT(strtab!=NULL);
        strtab->reallocate(false);
        ROSE_ASSERT(get_storage()->get_offset() != unallocated);
    }
    return get_storage()->get_offset();
}

SgAsmGenericStrtab *
SgAsmStoredString::get_strtab() 
{
    return get_storage()->get_strtab();
}

void
SgAsmStoredString::set_string(const std::string &s)
{
    if (get_string()==s) return; /* no change in value */
    set_isModified(true);
    SgAsmStringStorage *storage = get_storage();
    ROSE_ASSERT(storage!=NULL); /* we don't even know which string table! */
    storage->get_strtab()->free(storage);
    storage->set_string(s);
}

void
SgAsmStoredString::set_string(rose_addr_t offset)
{
    set_isModified(true);
    SgAsmStringStorage *storage = get_storage();
    ROSE_ASSERT(storage!=NULL); /* we don't even know which string table! */
    storage->get_strtab()->rebind(storage, offset);
}

/* Print some debugging info */
void
SgAsmStoredString::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sStoredString[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sStoredString.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = 0x%08lx\n", p, w, "storage", (unsigned long)get_storage());
    if (get_storage())
        get_storage()->dump(f, p, -1);
}

SgAsmStringStorage::SgAsmStringStorage(SgAsmGenericStrtab *strtab, const std::string &string, rose_addr_t offset) {
    initializeProperties();
    p_strtab = strtab;
    p_string = string;
    p_offset = offset;
}

/* Print some debugging info */
void
SgAsmStringStorage::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sStringStorage[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sStringStorage.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s =", p, w, "sec,offset,val");
    SgAsmGenericStrtab *strtab = get_strtab();
    if (strtab) {
        fprintf(f, " section [%d] \"%s\"",
                strtab->get_container()->get_id(),
                strtab->get_container()->get_name()->get_string(true).c_str());
    } else {
        fputs(" no section", f);
    }
    if (!strtab || get_offset()==SgAsmGenericString::unallocated) {
        fputs(", not allocated", f);
    } else {
        fprintf(f, ", offset 0x%08" PRIx64 " (%" PRIu64 ")", get_offset(), get_offset());
    }
    fprintf(f, ", \"%s\"\n", escapeString(get_string()).c_str());
}

SgAsmGenericStrtab::SgAsmGenericStrtab(SgAsmGenericSection *container) {
    initializeProperties();
    set_container(container);
}

SgAsmStoredString *
SgAsmGenericStrtab::create_string(rose_addr_t offset, bool shared)
{
    return createString(offset, shared);
}

SgAsmStoredString *
SgAsmGenericStrtab::createString(rose_addr_t offset, bool shared)
{
    SgAsmStringStorage *storage = createStorage(offset, shared);
    return new SgAsmStoredString(storage);
}

void
SgAsmGenericStrtab::free(SgAsmStringStorage *storage)
{
    ROSE_ASSERT(storage!=NULL);
    ROSE_ASSERT(storage!=get_dontFree());
    rose_addr_t old_offset = storage->get_offset();
    if (old_offset!=SgAsmGenericString::unallocated) {
        set_isModified(true);
        storage->set_offset(SgAsmGenericString::unallocated);
        free(old_offset, storage->get_string().size()+1);
    }
}

void
SgAsmGenericStrtab::free(rose_addr_t offset, rose_addr_t size)
{
    if (offset==SgAsmGenericString::unallocated || 0==size)
        return;

    ROSE_ASSERT(offset+size <= get_container()->get_size());
    set_isModified(true);
    
    /* Make sure area is not already in free list.  The freelist.insert() handles this gracefully, but if we're freeing
     * something that's already in the list then we have a logic error somewhere. */
    ROSE_ASSERT(!get_freeList().overlaps(AddressInterval::baseSize(offset, size)));

    /* Preserve anything that's still referenced. The caller should have assigned SgAsmStoredString::unalloced to the "offset"
     * member of the string storage to indicate that it's memory in the string table is no longer in use. */
    AddressIntervalSet toFree;
    toFree.insert(AddressInterval::baseSize(offset, size));
    for (size_t i=0; i<p_storageList.size(); i++) {
        SgAsmStringStorage *storage = p_storageList[i];
        if (storage->get_offset()!=SgAsmGenericString::unallocated)
            toFree.erase(AddressInterval::baseSize(storage->get_offset(), get_storageSize(storage)));
    }

    /* Add un-referenced extents to free list. */
    get_freeList().insertMultiple(toFree);
}

void
SgAsmGenericStrtab::free_all_strings(bool blow_away_holes)
{
    freeAllStrings(blow_away_holes);
}

void
SgAsmGenericStrtab::freeAllStrings(bool blow_away_holes)
{
    SgAsmGenericSection *container = get_container();
    SgAsmGenericFile *file = container->get_file();
    bool is_tracking = file->get_trackingReferences();
    set_isModified(true);

    /* Mark all storage objects as being unallocated. Never free the dont_free storage (if any). */
    for (size_t i=0; i<p_storageList.size(); i++) {
        if (p_storageList[i]->get_offset()!=SgAsmGenericString::unallocated && p_storageList[i]!=get_dontFree()) {
            p_numberFreed++;
            p_storageList[i]->set_offset(SgAsmGenericString::unallocated);
        }
    }

    /* Mark holes as referenced */
    if (blow_away_holes) {
        file->set_trackingReferences(true);
        file->markReferencedExtent(container->get_offset(), container->get_size());
        file->set_trackingReferences(is_tracking);
    }

    /* The free list is everything that's been referenced in the container section. */
    get_freeList() = container->get_referencedExtents();

    /* Remove the empty string from the free list */
    if (get_dontFree())
        get_freeList().erase(AddressInterval::baseSize(get_dontFree()->get_offset(), get_dontFree()->get_string().size()+1));
}

bool
SgAsmGenericStrtab::reallocate(bool shrink)
{
    bool reallocated = false;
    SgAsmGenericSection *container = get_container();
    rose_addr_t extend_size = 0;                                     /* amount by which to extend string table */

    /* Get list of strings that need to be allocated and sort by descending size. */
    std::vector<size_t> map;
    for (size_t i=0; i<p_storageList.size(); i++) {
        SgAsmStringStorage *storage = p_storageList[i];
        if (storage->get_offset()==SgAsmGenericString::unallocated) {
            map.push_back(i);
        }
    }
    for (size_t i=1; i<map.size(); i++) {
        for (size_t j=0; j<i; j++) {
            if (p_storageList[map[j]]->get_string().size() < p_storageList[map[i]]->get_string().size()) {
                size_t x = map[i];
                map[i] = map[j];
                map[j] = x;
            }
        }
    }

    /* Allocate from largest to smallest so we have the best chance of finding overlaps */
    for (size_t i=0; i<map.size(); i++) {
        SgAsmStringStorage *storage = p_storageList[map[i]];
        ROSE_ASSERT(storage->get_offset()==SgAsmGenericString::unallocated);

        /* We point empty strings at the dont_free storage if possible. */
        if (storage->get_string()=="" && get_dontFree()) {
            ROSE_ASSERT(get_dontFree()->get_string()=="");
            storage->set_offset(0);
        }

        /* If there's already a string with the same value then they can share space in the string table. They're still
         * considered two separate strings, so changing one doesn't affect the other. */
        if (storage->get_offset()==SgAsmGenericString::unallocated) {
            for (size_t j=0; j<p_storageList.size(); j++) {
                SgAsmStringStorage *previous = p_storageList[j];
                if (previous->get_offset()!=SgAsmGenericString::unallocated && previous->get_string()==storage->get_string()) {
                    storage->set_offset(previous->get_offset());
                    break;
                }
            }
        }

        /* Some string tables may be able to overlap strings. For instance, ELF can overlap "domain" and "main" since it
         * encodes strings with NUL termination. */
        if (storage->get_offset()==SgAsmGenericString::unallocated)
            allocateOverlap(storage);
        
        /* If we couldn't share another string then try to allocate from free space (avoiding holes) */
        if (storage->get_offset()==SgAsmGenericString::unallocated) {
            try {
                size_t need = storage->get_string().size() + 1;
                AddressIntervalSet::ConstIntervalIterator iter = get_freeList().bestFit(need,
                                                                                        get_freeList().intervals().begin());
                if (iter==get_freeList().intervals().end())
                    throw std::bad_alloc();
                ASSERT_require(iter->size() >= need);
                AddressInterval allocated = AddressInterval::baseSize(iter->least(), need);
                get_freeList().erase(allocated);
                rose_addr_t new_offset = allocated.least();
                storage->set_offset(new_offset);
            } catch(std::bad_alloc &x) {
                /* nothing large enough on the free list */
            }
        }

        /* If no free space area large enough then prepare to extend the section. */
        if (storage->get_offset()==SgAsmGenericString::unallocated) {
            extend_size += storage->get_string().size() + 1;
        }
    }

    /* If we were unable to allocate everything and there's still free space then it may be possible to reallocate all
     * strings in order to repack the table and avoid internal fragmentation. */
    //FIXME (RPM 2008-09-25)

    /* Change string table size as necessary. */
    if (extend_size>0) {
        /* The string table isn't large enough, so make it larger by extending the section that contains the table. The
         * containing section's "set_size" method should add the new space to the string table's free list. If our recursion
         * level is more than two calls deep then something went horribly wrong! */
        reallocated = true;
        container->get_file()->shiftExtend(container, 0, extend_size);
        reallocate(false);
    } else if (shrink && get_freeList().size()>0) {
        /* See if we can release any address space and shrink the containing section. The containing section's "set_size"
         * method will adjust the free list by removing some bytes from it. */
        AddressIntervalSet::ConstIntervalIterator iter = get_freeList().intervals().end();
        ASSERT_forbid(iter == get_freeList().intervals().begin());
        --iter;
        AddressInterval hi = *iter;
        if (hi.least() + hi.size() == container->get_size())
            container->set_size(hi.least());
    }

    if (reallocated)
        set_isModified(true);
    return reallocated;
}

const AddressIntervalSet&
SgAsmGenericStrtab::get_freelist() const
{
    return get_freeList();
}

AddressIntervalSet&
SgAsmGenericStrtab::get_freelist()
{
    return get_freeList();
}

void
SgAsmGenericStrtab::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    SgAsmGenericSection *container = get_container();

    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sStrtab[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sStrtab.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    if (container) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "container",
                container->get_id(), container->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = <null>\n", p, w, "container");
    }

    fprintf(f, "%s%-*s =", p, w, "dont_free");
    for (size_t i=0; i<p_storageList.size(); ++i) {
        if (p_storageList[i] == get_dontFree())
            fprintf(f, " p_storageList[%" PRIuPTR "]", i);
    }
    fputc('\n', f);
    
    fprintf(f, "%s%-*s = %" PRIuPTR " strings\n", p, w, "referenced", p_storageList.size());
    for (size_t i=0; i<p_storageList.size(); i++) {
        p_storageList[i]->dump(f, p, i);
    }

    fprintf(f, "%s%-*s = %" PRIu64 " free regions\n", p, w, "freelist", get_freeList().size());
    BOOST_FOREACH (const AddressInterval &interval, get_freeList().intervals()) {
        fprintf(f, "%s%-*s = offset 0x%08" PRIx64 " (%" PRIu64 "),"
                " for 0x%08" PRIx64 " (%" PRIu64 ") byte%s,"
                " ending at 0x%08" PRIx64 " (%" PRIu64 ")\n",
                p, w, "",
                interval.least(), interval.least(),
                interval.size(), interval.size(), 1==interval.size()?"":"s",
                interval.greatest()+1, interval.greatest()+1);
    }
}

const SgAsmGenericStrtab::referenced_t&
SgAsmGenericStrtab::get_storage_list() const {
    return get_storageList();
}

void
SgAsmGenericStrtab::set_storage_list(const referenced_t &x) {
    set_storageList(x);
}

SgAsmStringStorage*
SgAsmGenericStrtab::get_dont_free() const {
    return get_dontFree();
}

void
SgAsmGenericStrtab::set_dont_free(SgAsmStringStorage *x) {
    set_dontFree(x);
}

size_t
SgAsmGenericStrtab::get_num_freed() const {
    return get_numberFreed();
}

void
SgAsmGenericStrtab::set_num_freed(size_t x) {
    set_numberFreed(x);
}

void
SgAsmGenericStrtab::allocate_overlap(SgAsmStringStorage *x) {
    allocateOverlap(x);
}

void
SgAsmGenericStrtab::allocateOverlap(SgAsmStringStorage *) {}

SgAsmStringStorage*
SgAsmGenericStrtab::create_storage(rose_addr_t x, bool y) {
    return createStorage(x, y);
}

SgAsmStringStorage*
SgAsmGenericStrtab::createStorage(rose_addr_t, bool) {
    ASSERT_not_implemented("subclass must implement");
}

rose_addr_t
SgAsmGenericStrtab::get_storage_size(const SgAsmStringStorage *x) {
    return get_storageSize(x);
}

rose_addr_t
SgAsmGenericStrtab::get_storageSize(const SgAsmStringStorage*) {
    ASSERT_not_implemented("subclass must implement");
}

void
SgAsmGenericStrtab::rebind(SgAsmStringStorage*, rose_addr_t) {
    ASSERT_not_implemented("subclass must implement");
}

#endif
