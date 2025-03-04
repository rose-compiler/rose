/* String Tables (SgAsmPEStringSection and SgAsmCoffStrtab and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/BinaryAnalysis/Hexdump.h>
#include <Rose/Diagnostics.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::Diagnostics;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Table Sections
//
//    SgAsmPEStringTable is derived from SgAsmPESection, which is derived in turn from SgAsmGenericSection. A PE String Table
//    Section points to a COFF String Table (SgAsmCoffStrtab) that is contained in the section.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
SgAsmPEStringSection::SgAsmPEStringSection(SgAsmPEFileHeader *fhdr)
    : SgAsmPESection(fhdr), p_strtab(NULL) {
    initializeProperties();

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
    unparseHoles(f);
}

/* Augments superclass to make sure free list and such are adjusted properly */
void
SgAsmPEStringSection::set_size(Address newsize)
{
    Address orig_size = get_size();
    SgAsmPESection::set_size(newsize);
    SgAsmGenericStrtab *strtab = get_strtab();

    if (get_size() > orig_size) {
        /* Add new address space to string table free list */
        Address n = get_size() - orig_size;
        strtab->get_freeList().insert(AddressInterval::baseSize(orig_size, n));
    } else if (get_size() < orig_size) {
        /* Remove deleted address space from string table free list */
        Address n = orig_size - get_size();
        strtab->get_freeList().erase(AddressInterval::baseSize(get_size(), n));
    }
}

/* Print some debugging info */
void
SgAsmPEStringSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sPEStringSection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sPEStringSection.", prefix);
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

SgAsmCoffStrtab::SgAsmCoffStrtab(class SgAsmPESection *containing_section)
    : SgAsmGenericStrtab(containing_section) {
    initializeProperties();
}

/* Free StringStorage objects associated with this string table. It may not be safe to blow them away yet since other objects
 * may still have SgAsmStoredStrings pointing to these storage objects. So instead, we will mark all this strtab's storage
 * objects as no longer being associated with a string table. This allows the SgAsmStoredString objects to still function
 * properly and their destructors (~SgAsmStoredString) will free their storage. */
void
SgAsmCoffStrtab::destructorHelper() {
    for (referenced_t::iterator i = p_storageList.begin(); i != p_storageList.end(); ++i) {
        SgAsmStringStorage *storage = *i;
        storage->set_strtab(NULL);
        storage->set_offset(SgAsmGenericString::unallocated);
    }
    p_storageList.clear();
    p_dontFree = NULL; /*FIXME: can't delete for same reason as in SgAsmStoredString destructor. (RPM 2008-09-05) */
}

/* Creates the storage item for the string at the specified offset. If "shared" is true then attempt to re-use a previous storage
 * object, otherwise create a new one. Each storage object is considered to be a separate string, therefore when two strings
 * share the same storage object, changing one string changes the other. */
SgAsmStringStorage *
SgAsmCoffStrtab::createStorage(Address offset, bool shared)
{
    ROSE_ASSERT(offset!=SgAsmGenericString::unallocated);
    SgAsmGenericSection *container = get_container();

    /* Has the string already been created? */
    if (shared) {
        for (referenced_t::iterator i=p_storageList.begin(); i!=p_storageList.end(); ++i) {
            if ((*i)->get_offset()==offset && (*i)!=get_dontFree())
                return *i;
        }
    }

    /* Read string length byte */
    unsigned char byte;
    container->readContentLocal(offset, &byte, 1);
    unsigned len = byte;

    /* Make sure new storage isn't inside some other string. (We don't support nested strings in COFF where the length byte of
     * the nested string is one of the characters of the outer string.) */
    for (referenced_t::iterator i=p_storageList.begin(); i!=p_storageList.end(); ++i) {
        ROSE_ASSERT((*i)->get_offset()==SgAsmGenericString::unallocated ||
                    offset + 1 + len <= (*i)->get_offset() ||
                    offset >= 1 + (*i)->get_string().size());
    }

    /* Create storage object */
    char *buf = new char[len];
    container->readContentLocal(offset+1, buf, len);
    SgAsmStringStorage *storage = new SgAsmStringStorage(this, std::string(buf, len), offset);
    delete[] buf;

    /* It's a bad idea to free (e.g., modify) strings before we've identified all the strings in the table. Consider
     * the case where two strings have the same value and point to the same offset (i.e., they share storage). If we modify one
     * before we know about the other then (at best) we modify the other one also.
     * 
     * The only time we can guarantee this is OK is when the new storage points to the same file location as "dont_free"
     * since the latter is guaranteed to never be freed or shared. This exception is used when creating a new, unallocated
     * string (see SgAsmStoredString(SgAsmGenericStrtab,const std::string&)). */
    if (get_numberFreed()>0 && (!get_dontFree() || offset!=get_dontFree()->get_offset())) {
        mlog[WARN] <<"SgAsmCoffStrtab::create_storage(" <<offset <<"): "
                   <<StringUtility::plural(p_numberFreed, "other strings")
                   <<" (of " <<p_storageList.size() <<" created) in [" <<container->get_id() <<"]"
                   <<" \"" <<container->get_name()->get_string(true) <<"\""
                   <<(1==get_numberFreed()?"has":"have") <<" been modified and/or reallocated\n";
        ROSE_ASSERT(0==get_numberFreed());
    }

    set_isModified(true);
    p_storageList.push_back(storage);
    return storage;
}

/* Returns the number of bytes required to store the string in the string table. This is one (the length byte) plus the
 * length of the string. */
Address
SgAsmCoffStrtab::get_storageSize(const SgAsmStringStorage *storage) {
    return 1 + storage->get_string().size();
}

/* Write string table back to disk. Free space is zeroed out; holes are left as they are. */
void
SgAsmCoffStrtab::unparse(std::ostream &f) const
{
    SgAsmGenericSection *container = get_container();

    /* Write length coded strings. Shared strings will be written more than once, but that's OK. */
    for (size_t i=0; i<p_storageList.size(); i++) {
        SgAsmStringStorage *storage = p_storageList[i];
        ROSE_ASSERT(storage->get_offset()!=SgAsmGenericString::unallocated);
        Address at = container->write(f, storage->get_offset(), storage->get_string());
        container->write(f, at, '\0');
    }
    
    /* Fill free areas with zero */
    BOOST_FOREACH (const AddressInterval &interval, get_freeList().intervals())
        container->write(f, interval.least(), std::string(interval.size(), '\0'));
}

SgAsmStringStorage*
SgAsmCoffStrtab::create_storage(Address x, bool y) {
    return createStorage(x, y);
}

Address
SgAsmCoffStrtab::get_storage_size(const SgAsmStringStorage *x) {
    return get_storageSize(x);
}

#endif
