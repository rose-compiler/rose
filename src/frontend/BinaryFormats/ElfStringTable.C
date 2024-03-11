/* ELF String Tables
 * 
 * String tables are represented with two main classes:
 *
 *    SgAsmElfStringSection is an SgAsmElfSection that contains a string table.  It points to an SgAsmElfStrtab.
 *
 *    SgAsmElfStrtab is a class representing the string table contained in an SgAsmElfStringSection. It inherits from
 *    SgAsmGenericStrtab.  An SgAsmElfStrtab points back to the SgAsmElfStringSection that contains it.
 */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/Diagnostics.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics;
    
// Non-parsing constructor. The ELF String Table is constructed to contain one NUL byte.
SgAsmElfStringSection::SgAsmElfStringSection(SgAsmElfFileHeader *fhdr)
    : SgAsmElfSection(fhdr) {
    initializeProperties();

    get_name()->set_string("ELF String Table");
    if (get_size() == 0)
        set_size(1);
    p_strtab = new SgAsmElfStrtab(this);
}

// Parse the file content to initialize the string table
SgAsmElfStringSection *
SgAsmElfStringSection::parse()
{
    SgAsmElfSection::parse();
    ROSE_ASSERT(p_strtab);
    p_strtab->get_freeList().clear(); /*because set_size() during construction added to the free list*/
    p_strtab->parse();
    return this;
}

bool
SgAsmElfStringSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();
    if (get_strtab()->reallocate(false))
        reallocated = true;

    /* Update parts of the section and segment tables not updated by superclass */
    SgAsmElfSectionTableEntry *secent = get_sectionEntry();
    if (secent)
        secent->set_sh_type(SgAsmElfSectionTableEntry::SHT_STRTAB);

    return reallocated;
}

void
SgAsmElfStringSection::unparse(std::ostream &f) const
{
    get_strtab()->unparse(f);
    unparseHoles(f);
}

void
SgAsmElfStringSection::set_size(rose_addr_t newsize)
{
    rose_addr_t orig_size = get_size();
    SgAsmElfSection::set_size(newsize);
    SgAsmGenericStrtab *strtab = get_strtab();

    if (strtab) {
        if (get_size() > orig_size) {
            /* Add new address space to string table free list */
            rose_addr_t n = get_size() - orig_size;
            strtab->get_freeList().insert(AddressInterval::baseSize(orig_size, n));
        } else if (get_size() < orig_size) {
            /* Remove deleted address space from string table free list */
            rose_addr_t n = orig_size - get_size();
            strtab->get_freeList().erase(AddressInterval::baseSize(get_size(), n));
        }
    }
}

void
SgAsmElfStringSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sElfStringSection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sElfStringSection.", prefix);
    }
    
    SgAsmElfSection::dump(f, p, -1);

    ROSE_ASSERT(get_strtab()!=NULL);
    get_strtab()->dump(f, p, -1);

    if (variantT() == V_SgAsmElfStringSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

SgAsmElfStrtab::SgAsmElfStrtab(class SgAsmElfSection *containing_section)
    : SgAsmGenericStrtab(containing_section) {
    initializeProperties();
    ASSERT_not_null(get_container());
    if (get_container()->get_size() == 0)
        get_container()->set_size(1);
    set_dontFree(createStorage(0, false));
}

SgAsmElfStrtab *
SgAsmElfStrtab::parse()
{
    SgAsmGenericStrtab::parse();
    ROSE_ASSERT(get_container());
    if (get_container()->get_size()>0) {
        unsigned char first_byte;
        get_container()->readContentLocal(0, &first_byte, 1);
        if (first_byte=='\0') {
            if (get_dontFree()) {
                ROSE_ASSERT(0==get_dontFree()->get_offset());
            } else {
                p_dontFree = createStorage(0, false);
            }
        } else if (get_dontFree()) {
            p_dontFree = NULL;
        }
    }
    return this;
}

void
SgAsmElfStrtab::destructorHelper() {
    for (referenced_t::iterator i = p_storageList.begin(); i != p_storageList.end(); ++i) {
        SgAsmStringStorage *storage = *i;
        storage->set_strtab(NULL);
        storage->set_offset(SgAsmGenericString::unallocated);
    }
    p_storageList.clear();
    p_dontFree = NULL; /*FIXME: can't delete for same reason as in SgAsmStoredString destructor. (RPM 2008-09-05) */
}

SgAsmStringStorage *
SgAsmElfStrtab::create_storage(rose_addr_t offset, bool shared) {
    return createStorage(offset, shared);
}

SgAsmStringStorage *
SgAsmElfStrtab::createStorage(rose_addr_t offset, bool shared)
{
    ROSE_ASSERT(offset!=SgAsmGenericString::unallocated);

    /* Has this string already been created? If so, return previous storage object. However, never share the empty_string at
     * offset zero created when this string table was constructed because the ELF spec says it needs to stay there whether
     * referenced or not. */
    if (shared) {
        for (referenced_t::iterator i=p_storageList.begin(); i!=p_storageList.end(); i++) {
            if ((*i)->get_offset()==offset && (*i) != get_dontFree())
                return *i;
        }
    }

    /* Create a new storage object at this offset. */
    SgAsmStringStorage *storage = NULL;
    if (0==offset && 0==get_container()->get_data().size()) {
        ROSE_ASSERT(get_container()->get_size()>=1);
        storage = new SgAsmStringStorage(this, "", 0);
    } else {
        std::string s = get_container()->readContentLocalString(offset);
        storage = new SgAsmStringStorage(this, s, offset);
    }

    /* It's a bad idea to free (e.g., modify) strings before we've identified all the strings in the table. Consider
     * the case where offset 1 is "domain" and offset 3 is "main" (i.e., they overlap). If we modify "main" before knowing
     * about "domain" then we'll end up freeing the last part of "domain" (and possibly replacing it with something else)!
     *
     * The only time we can guarantee this is OK is when the new storage points to the same file location as "dontFree"
     * since the latter is guaranteed to never be freed or shared. This exception is used when creating a new, unallocated
     * string (see SgAsmStoredString(SgAsmGenericStrtab,const std::string&)). */
    if (get_numberFreed()>0 && (!get_dontFree() || offset!=get_dontFree()->get_offset())) {
        mlog[WARN] <<"SgAsmElfStrtab::createStorage(" <<StringUtility::addrToString(offset) <<"): "
                   <<StringUtility::plural(p_numberFreed, "other strings")
                   <<" (of " <<p_storageList.size() <<" created)"
                   <<" in [" <<get_container()->get_id() <<"] \"" <<get_container()->get_name()->get_string(true) <<"\" "
                   <<(1==get_numberFreed()?"has":"have") <<" been modified and/or reallocated\n";
        ROSE_ASSERT(0==get_numberFreed());
    }
    
    p_storageList.push_back(storage);
    set_isModified(true);
    return storage;
}

void
SgAsmElfStrtab::rebind(SgAsmStringStorage *storage, rose_addr_t offset)
{
    ROSE_ASSERT(get_dontFree() && storage!=get_dontFree() && storage->get_offset()==get_dontFree()->get_offset());
    std::string s = get_container()->readContentLocalString(offset, false /*relax*/);
    storage->set_offset(offset);
    storage->set_string(s);
}

rose_addr_t
SgAsmElfStrtab::get_storage_size(const SgAsmStringStorage *storage) {
    return get_storageSize(storage);
}

rose_addr_t
SgAsmElfStrtab::get_storageSize(const SgAsmStringStorage *storage) {
    return storage->get_string().size() + 1;
}

void
SgAsmElfStrtab::allocate_overlap(SgAsmStringStorage *storage)
{
    return allocateOverlap(storage);
}

void
SgAsmElfStrtab::allocateOverlap(SgAsmStringStorage *storage)
{
    ROSE_ASSERT(storage->get_offset()==SgAsmGenericString::unallocated);
    size_t need = storage->get_string().size();
    for (size_t i=0; i<p_storageList.size(); i++) {
        SgAsmStringStorage *existing = p_storageList[i];
        if (existing->get_offset()!=SgAsmGenericString::unallocated) {
            size_t have = existing->get_string().size();
            if (need<=have && 0==existing->get_string().compare(have-need, need, storage->get_string())) {
                /* An existing string ends with the new string. */
                storage->set_offset(existing->get_offset() + (have-need));
                return;
            } else if (need>have && existing->get_offset()>=need-have &&
                       0==storage->get_string().compare(need-have, have, existing->get_string())) {
                /* New string ends with an existing string. Check for, and allocate, free space. */
                rose_addr_t offset = existing->get_offset() - (need-have); /* positive diffs checked above */
                AddressInterval allocationRequest = AddressInterval::baseSize(offset, need-have);
                if (get_freeList().contains(allocationRequest)) {
                    get_freeList().erase(allocationRequest);
                    storage->set_offset(offset);
                    return;
                }
            }
        }
    }
}

void
SgAsmElfStrtab::unparse(std::ostream &f) const
{
    SgAsmGenericSection *container = get_container();
    
    /* Write strings with NUL termination. Shared strings will be written more than once, but that's OK. */
    for (size_t i=0; i<p_storageList.size(); i++) {
        SgAsmStringStorage *storage = p_storageList[i];
        if (storage->get_offset() == SgAsmGenericString::unallocated) {
            mlog[WARN] <<"during unparsing, this string is unallocated in the string table and will be discarded: "
                       <<"\"" <<StringUtility::cEscape(storage->get_string()) <<"\"\n";
        } else {
            rose_addr_t at = container->write(f, storage->get_offset(), storage->get_string());
            container->write(f, at, '\0');
        }
    }
    
    /* Fill free areas with zero */
    BOOST_FOREACH (const AddressInterval &interval, get_freeList().intervals())
        container->write(f, interval.least(), std::string(interval.size(), '\0'));
}

#endif
