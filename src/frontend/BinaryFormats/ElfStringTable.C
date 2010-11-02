/* ELF String Tables
 * 
 * String tables are represented with two main classes:
 *
 *    SgAsmElfStringSection is an SgAsmElfSection that contains a string table.  It points to an SgAsmElfStrtab.
 *
 *    SgAsmElfStrtab is a class representing the string table contained in an SgAsmElfStringSection. It inherits from
 *    SgAsmGenericStrtab.  An SgAsmElfStrtab points back to the SgAsmElfStringSection that contains it.
 */


// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
    
/** Non-parsing constructor. The ELF String Table is constructed to contain one NUL byte. */
void
SgAsmElfStringSection::ctor()
{
    get_name()->set_string("ELF String Table");
    if (get_size()==0)
        set_size(1);
    p_strtab = new SgAsmElfStrtab(this);
}

/** Parse the file content to initialize the string table */
SgAsmElfStringSection *
SgAsmElfStringSection::parse()
{
    SgAsmElfSection::parse();
    ROSE_ASSERT(p_strtab);
    p_strtab->get_freelist().clear(); /*because set_size() during construction added to the free list*/
    p_strtab->parse();
    return this;
}

/** Reallocate space for the string section if necessary. Note that reallocation is lazy here -- we don't shrink the section,
 *  we only enlarge it (if you want the section to shrink then call SgAsmGenericStrtab::reallocate(bool) with a true value
 *  rather than calling this function. SgAsmElfStringSection::reallocate is called in response to unparsing a file and gives
 *  the string table a chance to extend its container section if it needs to allocate more space for strings. */
bool
SgAsmElfStringSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();
    if (get_strtab()->reallocate(false))
        reallocated = true;

    /* Update parts of the section and segment tables not updated by superclass */
    SgAsmElfSectionTableEntry *secent = get_section_entry();
    if (secent)
        secent->set_sh_type(SgAsmElfSectionTableEntry::SHT_STRTAB);

    return reallocated;
}

/** Unparse an ElfStringSection by unparsing the ElfStrtab */
void
SgAsmElfStringSection::unparse(std::ostream &f) const
{
    get_strtab()->unparse(f);
    unparse_holes(f);
}

/** Augments superclass to make sure free list and such are adjusted properly. Any time the ELF String Section size is changed
 *  we adjust the free list in the ELF String Table contained in this section. */
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
            strtab->get_freelist().insert(orig_size, n);
        } else if (get_size() < orig_size) {
            /* Remove deleted address space from string table free list */
            rose_addr_t n = orig_size - get_size();
            strtab->get_freelist().erase(get_size(), n);
        }
    }
}

/** Print some debugging info */
void
SgAsmElfStringSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfStringSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfStringSection.", prefix);
    }
    
    SgAsmElfSection::dump(f, p, -1);

    ROSE_ASSERT(get_strtab()!=NULL);
    get_strtab()->dump(f, p, -1);

    if (variantT() == V_SgAsmElfStringSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}


/** Non-parsing constructor. The table is created to be at least one byte long and having a NUL character as the first byte. */
void
SgAsmElfStrtab::ctor()
{
    ROSE_ASSERT(get_container());
    if (get_container()->get_size()==0)
        get_container()->set_size(1);
    p_dont_free = create_storage(0, false);
}

/** Parses the string table. All that actually happens at this point is we look to see if the table begins with an empty
 *  string. */
SgAsmElfStrtab *
SgAsmElfStrtab::parse()
{
    SgAsmGenericStrtab::parse();
    ROSE_ASSERT(get_container());
    if (get_container()->get_size()>0) {
        unsigned char first_byte;
        get_container()->read_content_local(0, &first_byte, 1);
        if (first_byte=='\0') {
            if (p_dont_free) {
                ROSE_ASSERT(0==p_dont_free->get_offset());
            } else {
                p_dont_free = create_storage(0, false);
            }
        } else if (p_dont_free) {
            p_dont_free = NULL;
        }
    }
    return this;
}

/** Free StringStorage objects associated with this string table. It may not be safe to blow them away yet since other objects
 *  may still have SgAsmStoredStrings pointing to these storage objects. So instead, we will mark all this strtab's storage
 *  objects as no longer being associated with a string table. This allows the SgAsmStoredString objects to still function
 *  properly and their destructors (~SgAsmStoredString) will free their storage. */
SgAsmElfStrtab::~SgAsmElfStrtab()
{
    for (referenced_t::iterator i = p_storage_list.begin(); i != p_storage_list.end(); ++i) {
        SgAsmStringStorage *storage = *i;
        storage->set_strtab(NULL);
        storage->set_offset(SgAsmGenericString::unallocated);
    }
    p_storage_list.clear();
    p_dont_free = NULL; /*FIXME: can't delete for same reason as in SgAsmStoredString destructor. (RPM 2008-09-05) */
}

/** Creates the storage item for the string at the specified offset. If 'shared' is true then attempt to re-use a previous
 *  storage object, otherwise always create a new one. Each storage object is considered a separate string, therefore when two
 *  strings share the same storage object, changing one string changes the other. */
SgAsmStringStorage *
SgAsmElfStrtab::create_storage(rose_addr_t offset, bool shared)
{
    ROSE_ASSERT(offset!=SgAsmGenericString::unallocated);

    /* Has this string already been created? If so, return previous storage object. However, never share the empty_string at
     * offset zero created when this string table was constructed because the ELF spec says it needs to stay there whether
     * referenced or not. */
    if (shared) {
        for (referenced_t::iterator i=p_storage_list.begin(); i!=p_storage_list.end(); i++) {
            if ((*i)->get_offset()==offset && (*i) != p_dont_free)
                return *i;
        }
    }

    /* Create a new storage object at this offset. */
    SgAsmStringStorage *storage = NULL;
    if (0==offset && 0==get_container()->get_data().size()) {
        ROSE_ASSERT(get_container()->get_size()>=1);
        storage = new SgAsmStringStorage(this, "", 0);
    } else {
        std::string s = get_container()->read_content_local_str(offset);
        storage = new SgAsmStringStorage(this, s, offset);
    }

    /* It's a bad idea to free (e.g., modify) strings before we've identified all the strings in the table. Consider
     * the case where offset 1 is "domain" and offset 3 is "main" (i.e., they overlap). If we modify "main" before knowing
     * about "domain" then we'll end up freeing the last part of "domain" (and possibly replacing it with something else)!
     *
     * The only time we can guarantee this is OK is when the new storage points to the same file location as "dont_free"
     * since the latter is guaranteed to never be freed or shared. This exception is used when creating a new, unallocated
     * string (see SgAsmStoredString(SgAsmGenericStrtab,const std::string&)). */
    if (p_num_freed>0 && (!p_dont_free || offset!=p_dont_free->get_offset())) {
        fprintf(stderr,
                "SgAsmElfStrtab::create_storage(%"PRIu64"): %zu other string%s (of %zu created) in [%d] \"%s\""
                " %s been modified and/or reallocated!\n",
                offset, p_num_freed, 1==p_num_freed?"":"s", p_storage_list.size(),
                get_container()->get_id(), get_container()->get_name()->c_str(),
                1==p_num_freed?"has":"have");
        ROSE_ASSERT(0==p_num_freed);
    }
    
    p_storage_list.push_back(storage);
    set_isModified(true);
    return storage;
}

/** Similar to create_storage() but uses a storage object that's already been allocated. */
void
SgAsmElfStrtab::rebind(SgAsmStringStorage *storage, rose_addr_t offset)
{
    ROSE_ASSERT(p_dont_free && storage!=p_dont_free && storage->get_offset()==p_dont_free->get_offset());
    std::string s = get_container()->read_content_local_str(offset);
    storage->set_offset(offset);
    storage->set_string(s);
}

/** Returns the number of bytes required to store the string in the string table. This is the length of the string plus
 *  one for the NUL terminator. */
rose_addr_t
SgAsmElfStrtab::get_storage_size(const SgAsmStringStorage *storage) {
    return storage->get_string().size() + 1;
}

/** Tries to find a suitable offset for a string such that it overlaps with some other string already allocated. If the new
 *  string is the same as the end of some other string (new="main", existing="domain") then we just use an offset into that
 *  string since the space is already allocated for the existing string. If the new string ends with an existing string
 *  (new="domain", existing="main") and there's enough free space before the existing string (two bytes in this case) then
 *  we allocate some of that free space and use a suitable offset. In any case, upon return storage->get_offset() will return
 *  the allocated offset if successful, or SgAsmGenericString::unallocated if we couldn't find an overlap. */
void
SgAsmElfStrtab::allocate_overlap(SgAsmStringStorage *storage)
{
    ROSE_ASSERT(storage->get_offset()==SgAsmGenericString::unallocated);
    size_t need = storage->get_string().size();
    for (size_t i=0; i<p_storage_list.size(); i++) {
        SgAsmStringStorage *existing = p_storage_list[i];
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
                if (get_freelist().subtract_from(offset, need-have).size()==0) {
                    get_freelist().allocate_at(offset, need-have);
                    storage->set_offset(offset);
                    return;
                }
            }
        }
    }
}

/** Write string table back to disk. Free space is zeroed out; holes are left as they are. */
void
SgAsmElfStrtab::unparse(std::ostream &f) const
{
    SgAsmGenericSection *container = get_container();
    
    /* Write strings with NUL termination. Shared strings will be written more than once, but that's OK. */
    for (size_t i=0; i<p_storage_list.size(); i++) {
        SgAsmStringStorage *storage = p_storage_list[i];
        ROSE_ASSERT(storage->get_offset()!=SgAsmGenericString::unallocated);
        rose_addr_t at = container->write(f, storage->get_offset(), storage->get_string());
        container->write(f, at, '\0');
    }
    
    /* Fill free areas with zero */
    for (ExtentMap::const_iterator i=get_freelist().begin(); i!=get_freelist().end(); ++i) {
        container->write(f, i->first, std::string(i->second, '\0'));
    }
}
