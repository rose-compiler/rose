/* String Tables (SgAsmPEStringSection and SgAsmCoffStrtab and related classes) */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Table Sections
//
//    SgAsmPEStringTable is derived from SgAsmPESection, which is derived in turn from SgAsmGenericSection. A PE String Table
//    Section points to a COFF String Table (SgAsmCoffStrtab) that is contained in the section.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmPEStringSection::ctor()
{
    get_name()->set_string("PE String Table");
    p_strtab = new SgAsmCoffStrtab(this);
}

SgAsmPEStringSection*
SgAsmPEStringSection::parse()
{
    SgAsmPESection::parse();
    ROSE_ASSERT(p_strtab);
    p_strtab->parse();
    return this;
}

/* Reallocate space for the string table if necessary. Note that reallocation is lazy here -- we don't shrink the section,
 * we only enlarge it (if you want the section to shrink then call SgAsmGenericStrtab::reallocate(bool) with a true value
 * rather than calling this function. SgAsmPEStringSection::reallocate is called in response to unparsing a file and gives
 * the string table a chance to extend its container section if it needs to allocate more space for strings. */
bool
SgAsmPEStringSection::reallocate()
{
    return get_strtab()->reallocate(false);
}

/* Unparse an ElfStringSection by unparsing the ElfStrtab */
void
SgAsmPEStringSection::unparse(std::ostream &f) const
{
    get_strtab()->unparse(f);
    unparse_holes(f);
}

/* Augments superclass to make sure free list and such are adjusted properly */
void
SgAsmPEStringSection::set_size(rose_addr_t newsize)
{
    rose_addr_t orig_size = get_size();
    SgAsmPESection::set_size(newsize);
    SgAsmGenericStrtab *strtab = get_strtab();

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

/* Print some debugging info */
void
SgAsmPEStringSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPEStringSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPEStringSection.", prefix);
    }
    
    SgAsmPESection::dump(f, p, -1);

    ROSE_ASSERT(get_strtab()!=NULL);
    get_strtab()->dump(f, p, -1);

    if (variantT() == V_SgAsmPEStringSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Tables
//
//    An SgAsmCoffStrtab is a COFF String Table, inheriting from SgAsmGenericStrtab. String tables point to the
//    SgAsmGenericSection that contains them.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Free StringStorage objects associated with this string table. It may not be safe to blow them away yet since other objects
 * may still have SgAsmStoredStrings pointing to these storage objects. So instead, we will mark all this strtab's storage
 * objects as no longer being associated with a string table. This allows the SgAsmStoredString objects to still function
 * properly and their destructors (~SgAsmStoredString) will free their storage. */
SgAsmCoffStrtab::~SgAsmCoffStrtab()
{
    for (referenced_t::iterator i = p_storage_list.begin(); i != p_storage_list.end(); ++i) {
        SgAsmStringStorage *storage = *i;
        storage->set_strtab(NULL);
        storage->set_offset(SgAsmGenericString::unallocated);
    }
    p_storage_list.clear();
    p_dont_free = NULL; /*FIXME: can't delete for same reason as in SgAsmStoredString destructor. (RPM 2008-09-05) */
}

/* Creates the storage item for the string at the specified offset. If "shared" is true then attempt to re-use a previous storage
 * object, otherwise create a new one. Each storage object is considered to be a separate string, therefore when two strings
 * share the same storage object, changing one string changes the other. */
SgAsmStringStorage *
SgAsmCoffStrtab::create_storage(rose_addr_t offset, bool shared)
{
    ROSE_ASSERT(offset!=SgAsmGenericString::unallocated);
    SgAsmGenericSection *container = get_container();

    /* Has the string already been created? */
    if (shared) {
        for (referenced_t::iterator i=p_storage_list.begin(); i!=p_storage_list.end(); ++i) {
            if ((*i)->get_offset()==offset && (*i)!=p_dont_free)
                return *i;
        }
    }

    /* Read string length byte */
    unsigned char byte;
    container->read_content_local(offset, &byte, 1);
    unsigned len = byte;

    /* Make sure new storage isn't inside some other string. (We don't support nested strings in COFF where the length byte of
     * the nested string is one of the characters of the outer string.) */
    for (referenced_t::iterator i=p_storage_list.begin(); i!=p_storage_list.end(); ++i) {
        ROSE_ASSERT((*i)->get_offset()==SgAsmGenericString::unallocated ||
                    offset + 1 + len <= (*i)->get_offset() ||
                    offset >= 1 + (*i)->get_string().size());
    }

    /* Create storage object */
    char *buf = new char[len];
    container->read_content_local(offset+1, buf, len);
    SgAsmStringStorage *storage = new SgAsmStringStorage(this, std::string(buf, len), offset);
    delete[] buf;

    /* It's a bad idea to free (e.g., modify) strings before we've identified all the strings in the table. Consider
     * the case where two strings have the same value and point to the same offset (i.e., they share storage). If we modify one
     * before we know about the other then (at best) we modify the other one also.
     * 
     * The only time we can guarantee this is OK is when the new storage points to the same file location as "dont_free"
     * since the latter is guaranteed to never be freed or shared. This exception is used when creating a new, unallocated
     * string (see SgAsmStoredString(SgAsmGenericStrtab,const std::string&)). */
    if (p_num_freed>0 && (!p_dont_free || offset!=p_dont_free->get_offset())) {
        fprintf(stderr,
                "SgAsmCoffStrtab::create_storage(%"PRIu64"): %zu other string%s (of %zu created) in [%d] \"%s\""
                " %s been modified and/or reallocated!\n",
                offset, p_num_freed, 1==p_num_freed?"":"s", p_storage_list.size(),
                container->get_id(), container->get_name()->c_str(),
                1==p_num_freed?"has":"have");
        ROSE_ASSERT(0==p_num_freed);
    }

    set_isModified(true);
    p_storage_list.push_back(storage);
    return storage;
}

/* Returns the number of bytes required to store the string in the string table. This is one (the length byte) plus the
 * length of the string. */
rose_addr_t
SgAsmCoffStrtab::get_storage_size(const SgAsmStringStorage *storage) {
    return 1 + storage->get_string().size();
}

/* Write string table back to disk. Free space is zeroed out; holes are left as they are. */
void
SgAsmCoffStrtab::unparse(std::ostream &f) const
{
    SgAsmGenericSection *container = get_container();

    /* Write length coded strings. Shared strings will be written more than once, but that's OK. */
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
