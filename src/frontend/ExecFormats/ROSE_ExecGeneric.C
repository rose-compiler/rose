/* Copyright 2008 Lawrence Livermore National Security, LLC */

// DQ (4/21/2009): This is now set uniformally in sage3.h (included by rose.h).
// #define _FILE_OFFSET_BITS 64
#include "rose.h"

// DQ (4/21/2009): This is now included uniformally in sage3.h (included by rose.h).
// #include <sys/stat.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <algorithm>
#include <boost/math/common_factor.hpp>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Relative Virtual Addresses (RVA)
// An RVA is always relative to the base virtual address (base_va) defined in an executable file header.
// A rose_rva_t is optionally tied to an SgAsmGenericSection so that if the mapped address of the section is modified then
// the RVA stored in the rose_rva_t object is also adjusted.  The section-relative offset is always treated as an unsigned
// quantity, but negative offsets can be accommodated via integer overflow.
//
// Be careful about adjusting the RVA (the address or section) using ROSETTA's accessors.
//     symbol.p_address.set_section(section);          // this works
//     symbol.get_address().set_section(section);      // using ROSETTA accessor modifies a temporary copy of the RVA
// But if ROSETTA returns a vector then we can modify the RVA:
//     symbol.p_addresses[0].set_section(section);     // this works
//     symbol.get_addresses()[0].set_section(section); // so does this.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


rose_addr_t
rose_rva_t::get_rva() const 
{
    rose_addr_t rva = addr;
    if (section) rva += section->get_mapped_rva();
    return rva;
}

rose_rva_t&
rose_rva_t::set_rva(rose_addr_t rva)
{
    addr = rva;
    if (section)
        addr -= section->get_mapped_rva();
    return *this;
}

SgAsmGenericSection *
rose_rva_t::get_section() const
{
    return section;
}

rose_rva_t&
rose_rva_t::set_section(SgAsmGenericSection *new_section)
{
    if (section) {
        addr += section->get_mapped_rva();
        section = NULL;
    }
    if (new_section) {
        addr -= new_section->get_mapped_rva();
    }
    section = new_section;
    return *this;
}

/* Set the section to the section that best (most specifically) describes the virtual address */
rose_rva_t&
rose_rva_t::bind(SgAsmGenericHeader *fhdr)
{
    rose_addr_t va = get_rva() + fhdr->get_base_va();
    SgAsmGenericSection *secbind = fhdr->get_best_section_by_va(va);
    return set_section(secbind);
}

/* Return address relative to currently bound section */
rose_addr_t
rose_rva_t::get_rel() const
{
    return addr;
}

/* Return address relative to specified section */
rose_addr_t
rose_rva_t::get_rel(SgAsmGenericSection *s)
{
    ROSE_ASSERT(s!=NULL);
    ROSE_ASSERT(s->is_mapped());
    ROSE_ASSERT(get_rva() >= s->get_mapped_rva());
    return get_rva() - s->get_mapped_rva();
}

/* Convert to a string representation */
std::string
rose_rva_t::to_string() const
{
    char s[1024];
    sprintf(s, "0x%08"PRIx64" (%"PRIu64")", get_rva(), get_rva());
    std::string ss = s;

    if (get_section()) {
        sprintf(s, " + 0x%08"PRIx64" (%"PRIu64")", get_rel(), get_rel());
        ss += " <" + get_section()->get_name()->get_string() + s + ">";
    }
    return ss;
}

std::ostream &
operator<<(std::ostream &os, const rose_rva_t &rva)
{
    os << rva.to_string();
    return os;
}

/* Arithmetic */
rose_addr_t operator+(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() + a2.get_rva();}
rose_addr_t operator-(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() - a2.get_rva();}

/* Comparisons */
bool operator< (const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() <  a2.get_rva();}
bool operator<=(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() <= a2.get_rva();}
bool operator> (const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() >  a2.get_rva();}
bool operator>=(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() >= a2.get_rva();}
bool operator==(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() == a2.get_rva();}
bool operator!=(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() != a2.get_rva();}

    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Strings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Like get_string() except it returns a C NUL-terminated string */
const char *
SgAsmGenericString::c_str() const 
{
    return get_string().c_str();
}

std::string
SgAsmGenericString::get_string() const
{
    ROSE_ASSERT(!"should have been pure virtual if ROSETTA supported that.");
    abort();
}

void
SgAsmGenericString::set_string(const std::string &s)
{
    ROSE_ASSERT(!"should have been pure virtual if ROSETTA supported that.");
    abort();
}

void
SgAsmGenericString::set_string(addr_t offset)
{
    ROSE_ASSERT(!"should have been pure virtual if ROSETTA supported that.");
    abort();
}

void
SgAsmGenericString::dump(FILE*, const char *prefix, ssize_t idx) const
{
    ROSE_ASSERT(!"should have been pure virtual if ROSETTA supported that.");
    abort();
}

/* Constructor */
void
SgAsmBasicString::ctor()
{
#if 0
    fprintf(stderr, "SgAsmBasicString::ctor this=0x%08lx\n", (unsigned long)this);
    if (this==(void*)0x685998)
        abort(); /*DEBUGGING (rpm 2008-10-10)*/
#endif
}

/* Override ROSETTA because generated code doesn't match virtual signature in base class */
std::string
SgAsmBasicString::get_string() const
{
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
SgAsmBasicString::set_string(addr_t offset)
{
    fprintf(stderr, "SgAsmBasicString::set_string(addr_t offset=%"PRIu64"): not supported\n", offset);
    abort();
}

/* Print some debugging info */
void
SgAsmBasicString::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sBasicString[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sBasicString.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "value", get_string().c_str());
}
    
/* Stored String constructors/destructor */
void
SgAsmStoredString::ctor(SgAsmGenericStrtab *strtab, rose_addr_t offset, bool shared)
{
    p_storage = strtab->create_storage(offset, shared);
}
void
SgAsmStoredString::ctor(SgAsmGenericStrtab *strtab, const std::string &s)
{
    p_storage = strtab->create_storage(0, false);
    set_string(s);
}
void
SgAsmStoredString::ctor(SgAsmStringStorage *storage)
{
    p_storage = storage;
}
#if 0
// DQ (9/9/2008): Use the destructor built automatically by ROSETTA.
SgAsmStoredString::~SgAsmStoredString()
{
#if 0 /* FIXME: Strings may share storage, so we can't free it. (RPM 2008-09-03) */
    /* Free storage if it isn't associated with a string table. */
    if (p_storage && NULL==p_storage->strtab)
        delete p_storage;
#endif
    p_storage = NULL;
}
#endif

/* Returns the std::string associated with the SgAsmStoredString. */
std::string
SgAsmStoredString::get_string() const 
{
    return get_storage()->get_string();
}

/* Returns the offset into the string table where the string is allocated. If the string is not allocated then this call
 * triggers a reallocation. */
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

/* Returns the string table that holds this string, even if the string value isn't currently allocated in the table. */
SgAsmGenericStrtab *
SgAsmStoredString::get_strtab() 
{
    return get_storage()->get_strtab();
}

/* Give the string a new value */
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

/* Give the string a new value by specifying the offset of a string already existing in the string table. */
void
SgAsmStoredString::set_string(addr_t offset)
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
        sprintf(p, "%sStoredString[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sStoredString.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = 0x%08lx\n", p, w, "storage", (unsigned long)get_storage());
    if (get_storage())
        get_storage()->dump(f, p, -1);
}

/* Print some debugging info */
void
SgAsmStringStorage::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sStringStorage[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sStringStorage.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s =", p, w, "sec,offset,val");
    SgAsmGenericStrtab *strtab = get_strtab();
    if (strtab) {
        fprintf(f, " section [%d] \"%s\"", strtab->get_container()->get_id(), strtab->get_container()->get_name()->c_str());
    } else {
        fputs(" no section", f);
    }
    if (!strtab || get_offset()==SgAsmGenericString::unallocated) {
        fputs(", not allocated", f);
    } else {
        fprintf(f, ", offset 0x%08"PRIx64" (%"PRIu64")", get_offset(), get_offset());
    }
    fprintf(f, ", \"%s\"\n", get_string().c_str());
}

/* Constructs an SgAsmStoredString from an offset into this string table. */
SgAsmStoredString *
SgAsmGenericStrtab::create_string(addr_t offset, bool shared)
{
    SgAsmStringStorage *storage = create_storage(offset, shared);
    return new SgAsmStoredString(storage);
}

/* Free area of this string table that corresponds to the string currently stored. Use this in preference to the offset/size
 * version of free() when possible. */
void
SgAsmGenericStrtab::free(SgAsmStringStorage *storage)
{
    ROSE_ASSERT(storage!=NULL);
    ROSE_ASSERT(storage!=p_dont_free);
    addr_t old_offset = storage->get_offset();
    if (old_offset!=SgAsmGenericString::unallocated) {
        set_isModified(true);
        storage->set_offset(SgAsmGenericString::unallocated);
        free(old_offset, storage->get_string().size()+1);
    }
}

/* Add a range of bytes to the free list after subtracting areas that are referenced by other strings. For instance, an ELF
 * string table can have "main" and "domain" sharing storage. If we free the "domain" string then only "do" should be added
 * to the free list. */
void
SgAsmGenericStrtab::free(addr_t offset, addr_t size)
{
    if (offset==SgAsmGenericString::unallocated || 0==size)
        return;

    ROSE_ASSERT(offset+size <= get_container()->get_size());
    set_isModified(true);
    
    /* Make sure area is not already in free list.  The freelist.insert() handles this gracefully, but if we're freeing
     * something that's already in the list then we have a logic error somewhere. */
    ROSE_ASSERT(get_freelist().overlap_with(offset, size).size()==0);

    /* Preserve anything that's still referenced. The caller should have assigned SgAsmStoredString::unalloced to the "offset"
     * member of the string storage to indicate that it's memory in the string table is no longer in use. */
    ExtentMap s_extents;
    for (size_t i=0; i<p_storage_list.size(); i++) {
        SgAsmStringStorage *storage = p_storage_list[i];
        if (storage->get_offset()!=SgAsmGenericString::unallocated)
            s_extents.insert(storage->get_offset(), get_storage_size(storage));
    }
    ExtentMap to_free = s_extents.subtract_from(offset, size);

    /* Add un-refrened extents to free list. */
    get_freelist().insert(to_free);
}

/* Free all strings so they will be reallocated later. This is more efficient than calling free() for each storage object. If
 * blow_way_holes is true then any areas that are unreferenced in the string table will be marked as referenced and added to
 * the free list. */
void
SgAsmGenericStrtab::free_all_strings(bool blow_away_holes)
{
    SgAsmGenericSection *container = get_container();
    SgAsmGenericFile *file = container->get_file();
    bool is_tracking = file->get_tracking_references();
    set_isModified(true);

    /* Mark all storage objects as being unallocated. Never free the dont_free storage (if any). */
    for (size_t i=0; i<p_storage_list.size(); i++) {
        if (p_storage_list[i]->get_offset()!=SgAsmGenericString::unallocated && p_storage_list[i]!=p_dont_free) {
            p_num_freed++;
            p_storage_list[i]->set_offset(SgAsmGenericString::unallocated);
        }
    }

    /* Mark holes as referenced */
    if (blow_away_holes) {
        file->set_tracking_references(true);
        container->content(0, container->get_size());
        file->set_tracking_references(is_tracking);
    }

    /* The free list is everything that's been referenced in the container section. */
    get_freelist() = container->get_referenced_extents();

    /* Remove the empty string from the free list */
    if (p_dont_free)
	get_freelist().erase(p_dont_free->get_offset(), p_dont_free->get_string().size()+1);
}

/* Allocates storage for strings that have been modified but not allocated. We first try to fit unallocated strings into free
 * space. Any that are left will cause the string table to be extended. Returns true if the reallocation would potentially
 * affect some other section. If "shrink" is true then release address space that's no longer needed at the end of the table. */
bool
SgAsmGenericStrtab::reallocate(bool shrink)
{
    bool reallocated = false;
    SgAsmGenericSection *container = get_container();
    addr_t extend_size = 0;                                     /* amount by which to extend string table */

    /* Get list of strings that need to be allocated and sort by descending size. */
    std::vector<size_t> map;
    for (size_t i=0; i<p_storage_list.size(); i++) {
        SgAsmStringStorage *storage = p_storage_list[i];
        if (storage->get_offset()==SgAsmGenericString::unallocated) {
            map.push_back(i);
        }
    }
    for (size_t i=1; i<map.size(); i++) {
        for (size_t j=0; j<i; j++) {
            if (p_storage_list[map[j]]->get_string().size() < p_storage_list[map[i]]->get_string().size()) {
                size_t x = map[i];
                map[i] = map[j];
                map[j] = x;
            }
        }
    }

    /* Allocate from largest to smallest so we have the best chance of finding overlaps */
    for (size_t i=0; i<map.size(); i++) {
        SgAsmStringStorage *storage = p_storage_list[map[i]];
        ROSE_ASSERT(storage->get_offset()==SgAsmGenericString::unallocated);

        /* We point empty strings at the dont_free storage if possible. */
        if (storage->get_string()=="" && p_dont_free) {
            ROSE_ASSERT(p_dont_free->get_string()=="");
            storage->set_offset(0);
        }

        /* If there's already a string with the same value then they can share space in the string table. They're still
         * considered two separate strings, so changing one doesn't affect the other. */
        if (storage->get_offset()==SgAsmGenericString::unallocated) {
            for (size_t j=0; j<p_storage_list.size(); j++) {
                SgAsmStringStorage *previous = p_storage_list[j];
                if (previous->get_offset()!=SgAsmGenericString::unallocated && previous->get_string()==storage->get_string()) {
                    storage->set_offset(previous->get_offset());
                    break;
                }
            }
        }

        /* Some string tables may be able to overlap strings. For instance, ELF can overlap "domain" and "main" since it
         * encodes strings with NUL termination. */
        if (storage->get_offset()==SgAsmGenericString::unallocated)
            allocate_overlap(storage);
        
        /* If we couldn't share another string then try to allocate from free space (avoiding holes) */
        if (storage->get_offset()==SgAsmGenericString::unallocated) {
            ExtentPair e(0, 0);
            try {
                e = get_freelist().allocate_best_fit(storage->get_string().size()+1);
                addr_t new_offset = e.first;
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
        fprintf(stderr, "SgAsmElfStrtab::reallocate(): need to extend [%d] \"%s\" by %zu byte%s\n", 
                container->get_id(), container->get_name()->c_str(), extend_size, 1==extend_size?"":"s");
        static bool recursive=false;
        ROSE_ASSERT(!recursive);
        recursive = reallocated = true;
        try {
            container->get_file()->shift_extend(container, 0, extend_size);
            reallocate(false);
            recursive = false;
        } catch (...) {
            recursive = false;
            throw;
        }
    } else if (shrink && get_freelist().size()>0) {
        /* See if we can release any address space and shrink the containing section. The containing section's "set_size"
         * method will adjust the free list by removing some bytes from it. */
        ExtentPair hi = *(get_freelist().highest_offset());
        if (hi.first + hi.second == container->get_size())
            container->set_size(hi.first);
    }

    if (reallocated)
        set_isModified(true);
    return reallocated;
}

/* Returns a reference to the free list. Don't use ROSETTA-generated version because callers need to be able to modify the
 * free list. */
const ExtentMap&
SgAsmGenericStrtab::get_freelist() const
{
    return p_freelist;
}
ExtentMap&
SgAsmGenericStrtab::get_freelist()
{
    return p_freelist;
}

/* Print some debugging info */
void
SgAsmGenericStrtab::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    SgAsmGenericSection *container = get_container();

    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sStrtab[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sStrtab.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    if (container) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "container", container->get_id(), container->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = <null>\n", p, w, "container");
    }

    fprintf(f, "%s%-*s =", p, w, "dont_free");
    for (size_t i=0; i<p_storage_list.size(); ++i) {
        if (p_storage_list[i] == p_dont_free)
            fprintf(f, " p_storage_list[%zu]", i);
    }
    fputc('\n', f);
    
    fprintf(f, "%s%-*s = %zu strings\n", p, w, "referenced", p_storage_list.size());
    for (size_t i=0; i<p_storage_list.size(); i++) {
        p_storage_list[i]->dump(f, p, i);
    }

    fprintf(f, "%s%-*s = %zu free regions\n", p, w, "freelist", get_freelist().size());
    get_freelist().dump_extents(f, p, "freelist");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmGenericFormat
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Print some debugging info */
void
SgAsmGenericFormat::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096], sbuf[256];
    const char *s;
    if (idx>=0) {
        sprintf(p, "%sFormat[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sFormat.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    switch (get_family()) {
      case FAMILY_UNSPECIFIED:  s = "unspecified";                          break;
      case FAMILY_DOS:          s = "Microsoft DOS";                        break;
      case FAMILY_ELF:          s = "Executable and Linking Format (ELF)";  break;
      case FAMILY_LE:           s = "Microsoft Linear Executable (LE)";     break;
      case FAMILY_LX:           s = "OS/2 Extended Linear Executable (LX)"; break;
      case FAMILY_NE:           s = "Microsoft New Executable (NE)";        break;
      case FAMILY_PE:           s = "Microsoft Portable Executable (PE)";   break;
      default:
        sprintf(sbuf, "%u", get_family());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "family", s);

    switch (get_purpose()) {
      case PURPOSE_UNSPECIFIED:   s = "unspecified";                       break;
      case PURPOSE_OTHER:         s = "other";                             break;
      case PURPOSE_EXECUTABLE:    s = "executable program";                break;
      case PURPOSE_LIBRARY:       s = "library (shared or relocatable)";   break;
      case PURPOSE_CORE_DUMP:     s = "post mortem image (core dump)";     break;
      case PURPOSE_OS_SPECIFIC:   s = "operating system specific purpose"; break;
      case PURPOSE_PROC_SPECIFIC: s = "processor specific purpose";        break;
      default:
        sprintf(sbuf, "%u", get_purpose());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", s);

    switch (get_sex()) {
      case ORDER_UNSPECIFIED:     s = "unspecified";   break;
      case ORDER_LSB:             s = "little-endian"; break;
      case ORDER_MSB:             s = "big-endian";    break;
      default:
        sprintf(sbuf, "%u", get_sex());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "sex", s);

    fprintf(f, "%s%-*s = %u (%scurrent)\n", p, w, "version", get_version(), get_is_current_version() ? "" : "not-" );
    
    switch (get_abi()) {
      case ABI_UNSPECIFIED: s = "unspecified";        break;
      case ABI_86OPEN:      s = "86Open Common IA32"; break;
      case ABI_AIX:         s = "AIX";                break;
      case ABI_ARM:         s = "ARM architecture";   break;
      case ABI_FREEBSD:     s = "FreeBSD";            break;
      case ABI_HPUX:        s = "HP/UX";              break;
      case ABI_IRIX:        s = "IRIX";               break;
      case ABI_HURD:        s = "GNU/Hurd";           break;
      case ABI_LINUX:       s = "GNU/Linux";          break;
      case ABI_MODESTO:     s = "Novell Modesto";     break;
      case ABI_MONTEREY:    s = "Monterey project";   break;
      case ABI_MSDOS:       s = "Microsoft DOS";      break;
      case ABI_NT:          s = "Windows NT";         break;
      case ABI_NETBSD:      s = "NetBSD";             break;
      case ABI_SOLARIS:     s = "Sun Solaris";        break;
      case ABI_SYSV:        s = "SysV R4";            break;
      case ABI_TRU64:       s = "Compaq TRU64 UNIX";  break;
      default:
        sprintf(sbuf, "%u", get_abi());
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n",  p, w, "ABI",      s);
    fprintf(f, "%s%-*s = %u\n",  p, w, "ABIvers",  get_abi_version());
    fprintf(f, "%s%-*s = %zu\n", p, w, "wordsize", get_word_size());
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmGenericFile
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Non-parsing constructor. If you're creating an executable from scratch then call this function and you're done. But if
 *  you're parsing an existing file then call parse() in order to map the file's contents into memory for parsing. */
void
SgAsmGenericFile::ctor()
{
    ROSE_ASSERT(this != NULL);

    ROSE_ASSERT(p_fd == -1);
    ROSE_ASSERT(p_holes == NULL);
    ROSE_ASSERT(p_truncate_zeros == false);

    ROSE_ASSERT(p_headers == NULL);
    p_headers  = new SgAsmGenericHeaderList();
    ROSE_ASSERT(p_headers != NULL);
    p_headers->set_parent(this);

    ROSE_ASSERT(p_holes == NULL);
    p_holes  = new SgAsmGenericSectionList();
    ROSE_ASSERT(p_holes != NULL);
    p_holes->set_parent(this);
}

/* Loads file contents into memory (actually, just mmaps the file) */
SgAsmGenericFile *
SgAsmGenericFile::parse(std::string fileName)
{
    ROSE_ASSERT(p_fd < 0); /*can call parse() only once per object*/

    p_fd = open(fileName.c_str(), O_RDONLY);
    if (p_fd<0 || fstat(p_fd, &p_sb)<0) {
        std::string mesg = "Could not open binary file";
        throw FormatError(mesg + ": " + strerror(errno));
    }

    /* Map the file into memory so we don't have to read it explicitly */
    unsigned char *mapped = (unsigned char*)mmap(NULL, p_sb.st_size, PROT_READ, MAP_PRIVATE, p_fd, 0);
    if (!mapped) {
        std::string mesg = "Could not mmap binary file";
        throw FormatError(mesg + ": " + strerror(errno));
    }

    /* Make file contents available through an STL vector without actually reading the file */
    p_data = SgFileContentList(mapped, p_sb.st_size);
    return this;
}

/* Destructs by closing and unmapping the file and destroying all sections, headers, etc. */
SgAsmGenericFile::~SgAsmGenericFile() 
{
    /* Delete child headers before this */
    while (p_headers->get_headers().size()) {
        SgAsmGenericHeader *header = p_headers->get_headers().back();
        p_headers->get_headers().pop_back();
        delete header;
    }
    ROSE_ASSERT(p_headers->get_headers().empty()   == true);
    
    /* Unmap and close */
    unsigned char *mapped = p_data.pool();
    if (mapped && p_data.size()>0)
        munmap(mapped, p_data.size());
    p_data.clear();

    if ( p_fd >= 0 )
        close(p_fd);

 // Delete the pointers to the IR nodes containing the STL lists
    delete p_headers;
    p_headers = NULL;
    delete p_holes;
    p_holes = NULL;
}

/* Returns original size of file, based on file system */
rose_addr_t
SgAsmGenericFile::get_orig_size() const
{
    return p_data.size();
}

/* Returns current size of file based on section with highest ending address. */
rose_addr_t
SgAsmGenericFile::get_current_size() const
{
    addr_t retval=0;
    SgAsmGenericSectionPtrList sections = get_sections();
    for (SgAsmGenericSectionPtrList::iterator i=sections.begin(); i!=sections.end(); ++i) {
        retval = std::max(retval, (*i)->get_end_offset());
    }
    return retval;
}

/** Marks part of a file as having been referenced if we are tracking references. */
void
SgAsmGenericFile::mark_referenced_extent(addr_t offset, addr_t size)
{
    if (get_tracking_references()) {
        p_referenced_extents.insert(offset, size);
        delete p_unreferenced_cache;
        p_unreferenced_cache = NULL;
    }
}

/** Returns the parts of the file that have never been referenced. */
const ExtentMap &
SgAsmGenericFile::get_unreferenced_extents() const
{
    if (!p_unreferenced_cache) {
        p_unreferenced_cache = new ExtentMap();
        *p_unreferenced_cache = p_referenced_extents.subtract_from(0, get_current_size());
    }
    return *p_unreferenced_cache;
}

/** Reads data from a file. Reads up to @p size bytes of data from the file beginning at the specified byte offset (measured
 *  from the beginning of the file), placing the result in dst_buf, and returning the number of bytes read. If the number of
 *  bytes read is less than @p size then one of two things happen: if @p strict is true (the default) then an
 *  SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise @p dst_buf is zero padded so that exactly @p size
 *  bytes are always initialized. */
size_t
SgAsmGenericFile::read_content(addr_t offset, unsigned char *dst_buf, addr_t size, bool strict)
{
    size_t retval;
    if (offset+size <= p_data.size()) {
        retval = size;
    } else if (offset > p_data.size()) {
        if (strict)
            throw ShortRead(NULL, offset, size);
        retval = 0;
    } else {
        if (strict)
            throw ShortRead(NULL, p_data.size(), offset+size - (p_data.size()+offset));
        retval = p_data.size() - offset;
    }
    if (retval>0)
        memcpy(dst_buf, &(p_data[offset]), retval);
    if (get_tracking_references())
        mark_referenced_extent(offset, retval);
    memset(dst_buf+retval, 0, size-retval);
    return retval;
}

/** Reads data from a file. Reads up to @p size bytes of data starting at the specified relative virtual address. The @p map
 *  specifies how virtual addresses are mapped to file offsets. If @p map is the null pointer then the map already associated
 *  with this file is used (the map provided by the loader memory mapping emulation). As bytes are read, if we encounter a
 *  virtual address that is not mapped we stop reading and do one of two things: if @p strict is set then an RvaFileMap::NotMapped exception is thrown; otherwise the rest of the @p dst_buf is zero filled and the number of bytes read (not filled) is returned. */
size_t
SgAsmGenericFile::read_content(const RvaFileMap *map, addr_t rva, unsigned char *dst_buf, addr_t size, bool strict)
{
    if (!map)
        map = get_loader_map();
    ROSE_ASSERT(map!=NULL);

    /* Note: This is the same algorithm as used by RvaFileMap::readRVA() except we do it here so that we have an opportunity
     *       to track the file byte references. */
    size_t ncopied = 0;
    while (ncopied < size) {
        const RvaFileMap::MapElement *m = map->findRVA(rva);
        if (!m) break;                                                  /*we reached a non-mapped virtual address*/
        ROSE_ASSERT(rva >= m->get_rva());                               /*or else map->findRVA() malfunctioned*/
        size_t m_offset = rva - m->get_rva();                           /*offset relative to start of map element*/
        ROSE_ASSERT(m_offset < m->get_size());                          /*or else map->findRVA() malfunctioned*/
        size_t nread = std::min(size-ncopied, m->get_size()-m_offset);  /*bytes to read in this pass*/
        size_t file_offset = m->get_offset() + m_offset;
        ROSE_ASSERT(file_offset<get_data().size());
        ROSE_ASSERT(file_offset+nread<=get_data().size());
        memcpy(dst_buf+ncopied, &(get_data()[file_offset]), nread);
        ncopied += nread;
        if (get_tracking_references())
            mark_referenced_extent(file_offset, nread);
    }

    if (ncopied<size) {
        if (strict)
            throw RvaFileMap::NotMapped(map, rva);
        memset(dst_buf+ncopied, 0, size-ncopied);                       /*zero pad result if necessary*/
    }
    return ncopied;
}

/** Reads a string from a file. Returns the string stored at the specified relative virtual address. The
 *  returned string contains the bytes beginning at the starting RVA and continuing until we reach a NUL byte or an address
 *  which is not mapped. If we reach an address which is not mapped then one of two things happen: if @p strict is set then an
 *  RvaFileMap::NotMapped exception is thrown; otherwise the string is simply terminated. The returned string does not include
 *  the NUL byte. */
std::string
SgAsmGenericFile::read_content_str(const RvaFileMap *map, addr_t rva, bool strict)
{
    static char *buf=NULL;
    static size_t nalloc=0;
    size_t nused=0;

    /* Note: reading one byte at a time might not be the most efficient way to do this, but it does cause the referenced bytes
     *       to be tracked very precisely. */ 
    while (1) {
        if (nused >= nalloc) {
            nalloc = std::max((size_t)32, 2*nalloc);
            buf = (char*)realloc(buf, nalloc);
            ROSE_ASSERT(buf!=NULL);
        }

        unsigned char byte;
        read_content(map, rva+nused, &byte, 1, strict); /*might throw RvaSizeMap::NotMapped or return a NUL*/
        if (!byte)
            return std::string(buf, nused);
        buf[nused++] = byte;
    }
}

/** Reads a string from a file. Returns the NUL-terminated string stored at the specified relative virtual address. The
 *  returned string contains the bytes beginning at the specified starting file offset and continuing until we reach a NUL
 *  byte or an invalid file offset. If we reach an invalid file offset one of two things happen: if @p strict is set (the
 *  default) then an SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise the string is simply terminated. The
 *  returned string does not include the NUL byte. */
std::string
SgAsmGenericFile::read_content_str(addr_t offset, bool strict)
{
    static char *buf=NULL;
    static size_t nalloc=0;
    size_t nused=0;

    /* Note: reading one byte at a time might not be the most efficient way to do this, but it does cause the referenced bytes
     *       to be tracked very precisely. */ 
    while (1) {
        if (nused >= nalloc) {
            nalloc = std::max((size_t)32, 2*nalloc);
            buf = (char*)realloc(buf, nalloc);
            ROSE_ASSERT(buf!=NULL);
        }

        unsigned char byte;
        read_content(offset+nused, &byte, 1, strict); /*might throw ShortRead or return a NUL*/
        if (!byte)
            return std::string(buf, nused);
        buf[nused++] = byte;
    }
}

/* DEPRECATED: use read_content() instead */
/** Returns a vector that points to part of the file content without actually ever referencing the file content until the
 *  vector elements are referenced. If @p relax is true and the desired extent falls entirely or partially outside the range
 *  of data known to the file then return an SgFileContentList with as much data as possible rather than throwing an
 *  exception. */
SgFileContentList
SgAsmGenericFile::content(addr_t offset, addr_t size, bool relax)
{
    if (offset+size <= p_data.size()) {
        return SgFileContentList(p_data, offset, size);
    } else if (!relax) {
        throw ShortRead(NULL, offset, size);
    } else if (offset > p_data.size()) {
        return SgFileContentList(p_data, 0, 0);
    } else {
        size = p_data.size() - offset;
        return SgFileContentList(p_data, offset, size);
    }
}

/* Adds a new header to the file. This is called implicitly by the header constructor */
void
SgAsmGenericFile::add_header(SgAsmGenericHeader *header) 
{
    ROSE_ASSERT(p_headers!=NULL);
    p_headers->set_isModified(true);

#ifndef NDEBUG
    /* New header must not already be present. */
    for (size_t i=0; i< p_headers->get_headers().size(); i++) {
        ROSE_ASSERT(p_headers->get_headers()[i] != header);
    }
#endif
    header->set_parent(p_headers);
    p_headers->get_headers().push_back(header);
}

/* Removes a header from the header list in a file */
void
SgAsmGenericFile::remove_header(SgAsmGenericHeader *hdr)
{
    if (hdr!=NULL) {
        ROSE_ASSERT(p_headers  != NULL);
        SgAsmGenericHeaderPtrList::iterator i = find(p_headers->get_headers().begin(), p_headers->get_headers().end(), hdr);
        if (i != p_headers->get_headers().end()) {
            p_headers->get_headers().erase(i);
            p_headers->set_isModified(true);
        }
    }
}

/* Adds a new hole to the file. This is called implicitly by the hole constructor */
void
SgAsmGenericFile::add_hole(SgAsmGenericSection *hole)
{
    ROSE_ASSERT(p_holes!=NULL);
    p_holes->set_isModified(true);

#ifndef NDEBUG
    /* New hole must not already be present. */
    for (size_t i=0; i< p_holes->get_sections().size(); i++) {
        ROSE_ASSERT(p_holes->get_sections()[i] != hole);
    }
#endif
    hole->set_parent(p_holes);
    p_holes->get_sections().push_back(hole);
}

/* Removes a hole from the list of holes in a file */
void
SgAsmGenericFile::remove_hole(SgAsmGenericSection *hole)
{
    if (hole!=NULL) {
        ROSE_ASSERT(p_holes!=NULL);
        SgAsmGenericSectionPtrList::iterator i = find(p_holes->get_sections().begin(), p_holes->get_sections().end(), hole);
        if (i != p_holes->get_sections().end()) {
            p_holes->get_sections().erase(i);
            p_holes->set_isModified(true);
        }
    }
}

/* Returns list of all sections in the file (including headers, holes, etc). */
SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections(bool include_holes) const
{
    SgAsmGenericSectionPtrList retval;

    /* Start with headers and holes */
    retval.insert(retval.end(), p_headers->get_headers().begin(), p_headers->get_headers().end());
    if (include_holes)
        retval.insert(retval.end(), p_holes->get_sections().begin(), p_holes->get_sections().end());

    /* Add sections pointed to by headers. */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections()->get_sections();
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns list of all sections in the file that are memory mapped, including headers and holes. */
SgAsmGenericSectionPtrList
SgAsmGenericFile::get_mapped_sections() const
{
    SgAsmGenericSectionPtrList retval;
    SgAsmGenericSectionPtrList all = get_sections(true);
    for (size_t i=0; i<all.size(); i++) {
        if (all[i]->is_mapped())
            retval.push_back(all[i]);
    }
    return retval;
}

/* Returns sections having specified ID across all headers, including headers and holes. */
SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_id(int id) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        if ((*i)->get_id()==id)
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        if ((*i)->get_id()==id)
            retval.push_back(*i);
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_id(id);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns the pointer to section with the specified ID across all headers only if there's exactly one match. Headers and
 * holes are included in the results. */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_id(int id, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_id(id);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns all sections having specified name across all headers, including headers and holes. */
SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_name(std::string name, char sep/*or NUL*/) const
{
    SgAsmGenericSectionPtrList retval;

    /* Truncate name */
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }

    /* Holes */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        std::string secname = (*i)->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        std::string secname = (*i)->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(*i);

        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_name(name, sep);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns pointer to the section with the specified name, or NULL if there isn't exactly one match. Any characters in the name
 * after the first occurrence of SEP are ignored (default is NUL). For instance, if sep=='$' then the following names are all
 * equivalent: .idata, .idata$, and .idata$1 */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_name(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_name(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns all sections that contain all of the specified portion of the file across all headers, including headers and holes. */
SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_offset(addr_t offset, addr_t size) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        if (offset >= (*i)->get_offset() &&
            offset < (*i)->get_offset()+(*i)->get_size() &&
            offset-(*i)->get_offset() + size <= (*i)->get_size())
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        if (offset >= (*i)->get_offset() &&
            offset < (*i)->get_offset()+(*i)->get_size() &&
            offset-(*i)->get_offset() + size <= (*i)->get_size())
            retval.push_back(*i);
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_offset(offset, size);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns single section that contains all of the specified portion of the file across all headers, including headers and
 * holes. */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_offset(addr_t offset, addr_t size, size_t *nfound) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_offset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns all sections that are mapped to include the specified relative virtual address across all headers, including
 * headers and holes. */
SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_rva(addr_t rva) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes (probably not mapped anyway) */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        if ((*i)->is_mapped() &&
            rva >= (*i)->get_mapped_rva() && rva < (*i)->get_mapped_rva() + (*i)->get_mapped_size())
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        if ((*i)->is_mapped() &&
            rva >= (*i)->get_mapped_rva() && rva < (*i)->get_mapped_rva() + (*i)->get_mapped_size())
            retval.push_back(*i);
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_rva(rva);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns single section that is mapped to include the specified relative virtual file address across all headers, including
 * headers and holes. */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_rva(addr_t rva, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_rva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns all sections that are mapped to include the specified virtual address across all headers, including headers and
 * holes. */
SgAsmGenericSectionPtrList
SgAsmGenericFile::get_sections_by_va(addr_t va) const
{
    SgAsmGenericSectionPtrList retval;

    /* Holes (probably not mapped anyway) */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
        addr_t rva = va; /* Holes don't belong to any header and therefore have a zero base_va */
        if ((*i)->is_mapped() &&
            rva >= (*i)->get_mapped_rva() && rva < (*i)->get_mapped_rva() + (*i)->get_mapped_size())
            retval.push_back(*i);
    }

    /* Headers and their sections */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
        /* Headers probably aren't mapped, but just in case... */
        addr_t rva = va; /* Headers don't belong to any header and therefore have a zero base_va */
        if ((*i)->is_mapped() &&
            rva >= (*i)->get_mapped_rva() && rva < (*i)->get_mapped_rva() + (*i)->get_mapped_size())
            retval.push_back(*i);

        /* Header sections */
        const SgAsmGenericSectionPtrList &recurse = (*i)->get_sections_by_va(va);
        retval.insert(retval.end(), recurse.begin(), recurse.end());
    }
    return retval;
}

/* Returns single section that is mapped to include the specified virtual address across all headers. See also
 * get_best_section_by_va(). */
SgAsmGenericSection *
SgAsmGenericFile::get_section_by_va(addr_t va, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_va(va);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Similar to get_section_by_va() except when more than one section contains the specified virtual address we choose the
 * "best" one. All candidates must map the virtual address to the same file address or else we fail (return null and number of
 * candidates). See code below for definition of "best". */
SgAsmGenericSection *
SgAsmGenericFile::get_best_section_by_va(addr_t va, size_t *nfound/*optional*/) const
{
    const SgAsmGenericSectionPtrList &candidates = get_sections_by_va(va);
    if (nfound)
        *nfound = candidates.size();
    return best_section_by_va(candidates, va);
}

/* Definition for "best" as used by
 * SgAsmGenericFile::get_best_section_by_va() and
 * SgAsmGenericHeader::get_best_section_by_va() */
SgAsmGenericSection *
SgAsmGenericFile::best_section_by_va(const SgAsmGenericSectionPtrList &sections, addr_t va)
{
    if (0==sections.size())
        return NULL;
    if (1==sections.size()) 
        return sections[0];
    SgAsmGenericSection *best = sections[0];
    addr_t fo0 = sections[0]->get_va_offset(va);
    for (size_t i=1; i<sections.size(); i++) {
        if (fo0 != sections[i]->get_va_offset(va))
            return NULL; /* all sections sections must map the VA to the same file offset */
        if (best->get_mapped_size() > sections[i]->get_mapped_size()) {
            best = sections[i]; /*prefer sections with a smaller mapped size*/
        } else if (best->get_name()->get_string().size()==0 && sections[i]->get_name()->get_string().size()>0) {
            best = sections[i]; /*prefer sections having a name*/
        } else {
            /*prefer section defined earlier*/
        }
    }
    return best;
}

/* Appears to be the same as SgAsmGenericFile::get_best_section_by_va() except it excludes sections named "ELF Segment Table".
 * Perhaps it should be rewritten in terms of the other. (RPM 2008-09-02) */
SgAsmGenericSection *
SgAsmGenericFile::get_best_possible_section_by_va(addr_t va)
{
  // This function is implemented for use in:
  //      "DisassemblerCommon::AsmFileWithData::getSectionOfAddress(uint64_t addr)"
  // It supports a more restrictive selection of valid sections to associate with 
  // a given address so that we can avoid disassembly of sections that are not code.

     const std::vector<SgAsmGenericSection*> &possible = get_sections_by_va(va);

     if (0 == possible.size())
        {
          return NULL;
        }
       else
        {
          if (1 == possible.size())
             {
            // printf ("Only one alternative: va = %p possible[0] id = %d name = %s (return %s) \n",
            //      (void*)va,possible[0]->get_id(),possible[0]->get_name().c_str(),(possible[0]->get_id() < 0) ? "NULL" : "it");
            // return possible[0];
               if (possible[0]->get_id() < 0)
                    return NULL;
                 else
                    return possible[0];
             }
        }

#if 0
     printf ("Select from %zu alternatives \n",possible.size());
     for (size_t i = 0; i < possible.size(); i++)
        {
          printf ("   va = %p possible[%zu] id = %d name = %s \n",(void*)va,i,possible[i]->get_id(),possible[i]->get_name().c_str());
        }
#endif

  /* Choose the "best" section to return. */
     SgAsmGenericSection *best = possible[0];
     addr_t fo0 = possible[0]->get_va_offset(va);
     for (size_t i = 1; i < possible.size(); i++)
        {
          if (fo0 != possible[i]->get_va_offset(va))
            return NULL; /* all possible sections must map the VA to the same file offset */

          if (best->get_id() < 0 && possible[i]->get_id() > 0)
             {
               best = possible[i]; /*prefer sections defined in a section or object table*/
             }
            else
               if (best->get_mapped_size() > possible[i]->get_mapped_size())
                  {
                    best = possible[i]; /*prefer sections with a smaller mapped size*/
                  }
                 else
                     if (best->get_name()->get_string().size()==0 && possible[i]->get_name()->get_string().size()>0)
                       {
                         best = possible[i]; /*prefer sections having a name*/
                       }
                      else
                       {
                      /* prefer section defined earlier*/

                       }
        }

     ROSE_ASSERT(best != NULL);

  // Add a few things that we just don't want to disassemble
     if (best->get_name()->get_string() == "ELF Segment Table")
          return NULL;

  // printf ("   best: va = %p id = %d name = %s \n",(void*)va,best->get_id(),best->get_name().c_str());

     return best;
}

/* Given a file address, return the file offset of the following section(s). If there is no following section then return an
 * address of -1 (when signed) */
rose_addr_t
SgAsmGenericFile::get_next_section_offset(addr_t offset)
{
    addr_t found = ~(addr_t)0;
    const SgAsmGenericSectionPtrList &sections = get_sections();
    for (SgAsmGenericSectionPtrList::const_iterator i=sections.begin(); i!=sections.end(); ++i) {
        if ((*i)->get_offset() >= offset && (*i)->get_offset() < found)
            found = (*i)->get_offset();
    }
    return found;
}

/* Shifts (to a higher offset) and/or enlarges the specified section, S, taking all other sections into account.
 *
 * The neighborhood(S) is S itself and the set of all sections that overlap or are adjacent to the neighborhood of S,
 * recursively.
 *
 * The address space can be partitioned into three categories:
 *     - Section: part of an address space that is referenced by an SgAsmGenericSection other than a "hole" section.
 *     - Hole:    part of an address space that is referenced only by a "hole" section.
 *     - Unref:   part of an address space that is not used by any section, including any "hole" section.
 * 
 * The last two categories define parts of the address space that can be optionally elastic--they expand or contract
 * to take up slack or provide space for neighboring sections. This is controlled by the "elasticity" argument.
 *
 * Note that when elasticity is ELASTIC_HOLE we simply ignore the "hole" sections, effectively removing their addresses from
 * the range of addresses under consideration. This avoids complications that arise when a "hole" overlaps with a real section
 * (due to someone changing offsets in an incompatible manner), but causes the hole offset and size to remain fixed.
 * (FIXME RPM 2008-10-20)
 *
 * When section S is shifted by 'Sa' bytes and/or enlarged by 'Sn' bytes, other sections are affected as follows:
 *     Cat L:  Not affected
 *     Cat R:  Shifted by Sa+Sn if they are in neighborhood(S). Otherwise the amount of shifting depends on the size of the
 *             hole right of neighborhood(S).
 *     Cat C:  Shifted Sa and enlarged Sn.
 *     Cat O:  If starting address are the same: Shifted Sa
 *             If starting address not equal:    Englarged Sa+Sn
 *     Cat I:  Shifted Sa, not enlarged
 *     Cat B:  Not shifted, but enlarged Sn
 *     Cat E:  Shifted Sa and enlarged Sn
 *
 * Generally speaking, the "space" argument should be SgAsmGenericFile::ADDRSP_ALL in order to adjust both file and memory
 * offsets and sizes in a consistent manner.
 *
 * To change the address and/or size of S without regard to other sections in the same file, use set_offset() and set_size()
 * (for file address space) or set_mapped_rva() and set_mapped_size() (for memory address space).
 */
void
SgAsmGenericFile::shift_extend(SgAsmGenericSection *s, addr_t sa, addr_t sn, AddressSpace space, Elasticity elasticity)
{
    ROSE_ASSERT(s!=NULL);
    ROSE_ASSERT(s->get_file()==this);
    ROSE_ASSERT(space & (ADDRSP_FILE|ADDRSP_MEMORY) != 0);

    const bool debug = false;
    static size_t ncalls=0;
    char p[256];

    if (debug) {
        const char *space_s="unknown";
        if (space & ADDRSP_FILE) {
            space_s = "file";
        } else if (space & ADDRSP_MEMORY) {
            space_s = "memory";
        }
        sprintf(p, "SgAsmGenericFile::shift_extend[%zu]: ", ncalls++);
        fprintf(stderr, "%s    -- START --\n", p);
        fprintf(stderr, "%s    S = [%d] \"%s\"\n", p, s->get_id(), s->get_name()->c_str());
        fprintf(stderr, "%s    %s Sa=0x%08"PRIx64" (%"PRIu64"), Sn=0x%08"PRIx64" (%"PRIu64")\n", p, space_s, sa, sa, sn, sn);
        fprintf(stderr, "%s    elasticity = %s\n", p, (ELASTIC_NONE==elasticity ? "none" :
                                                       ELASTIC_UNREF==elasticity ? "unref" :
                                                       ELASTIC_HOLE==elasticity ? "unref+holes" :
                                                       "unknown"));
    }

    /* No-op case */
    if (0==sa && 0==sn) {
        if (debug) {
            fprintf(stderr, "%s    No change necessary.\n", p);
            fprintf(stderr, "%s    -- END --\n", p);
        }
        return;
    }
    
    bool filespace = (space & ADDRSP_FILE)!=0;
    bool memspace = (space & ADDRSP_MEMORY)!=0;
    addr_t align=1, aligned_sa, aligned_sasn;
    SgAsmGenericSectionPtrList neighbors, villagers;
    ExtentMap amap; /* address mappings for all extents */
    ExtentPair sp;

    /* Get a list of all sections that may need to be adjusted. */
    SgAsmGenericSectionPtrList all;
    switch (elasticity) {
      case ELASTIC_NONE:
      case ELASTIC_UNREF:
        all = filespace ? get_sections() : get_mapped_sections();
        break;
      case ELASTIC_HOLE:
        all = filespace ? get_sections(false) : get_mapped_sections();
        break;
    }
    if (debug) {
        fprintf(stderr, "%s    Following sections are in 'all' set:\n", p);
        for (size_t i=0; i<all.size(); i++) {
            ExtentPair ep;
            if (filespace) {
                ep = all[i]->get_file_extent();
            } else {
                ROSE_ASSERT(all[i]->is_mapped());
                ep = all[i]->get_mapped_extent();
            }
            fprintf(stderr, "%s        0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64" [%d] \"%s\"\n",
                    p, ep.first, ep.second, ep.first+ep.second, all[i]->get_id(), all[i]->get_name()->c_str());
        }
    }

    for (size_t pass=0; pass<2; pass++) {
        if (debug) fprintf(stderr, "%s    -- PASS %zu --\n", p, pass);

        /* S offset and size in file or memory address space */
        if (filespace) {
            sp = s->get_file_extent();
        } else if (!memspace || !s->is_mapped()) {
            return; /*nothing to do*/
        } else {
            sp = s->get_mapped_extent();
        }
    
        /* Build address map */
        for (size_t i=0; i<all.size(); i++) {
            if (filespace) {
                amap.insert(all[i]->get_file_extent());
            } else {
                ROSE_ASSERT(all[i]->is_mapped());
                amap.insert(all[i]->get_mapped_extent());
            }
        }
        if (debug) {
            fprintf(stderr, "%s    Address map:\n", p);
            amap.dump_extents(stderr, (std::string(p)+"        ").c_str(), "amap");
            fprintf(stderr, "%s    Extent of S:\n", p);
            fprintf(stderr, "%s        start=0x%08"PRIx64" size=0x%08"PRIx64" end=0x%08"PRIx64"\n",
                    p, sp.first, sp.second, sp.first+sp.second);
        }
        
        /* Neighborhood (nhs) of S is a single extent. However, if S is zero size then nhs will be empty. */
        ExtentMap nhs_map = amap.overlap_with(sp);
        if (debug) {
            fprintf(stderr, "%s    Neighborhood of S:\n", p);
            nhs_map.dump_extents(stderr, (std::string(p)+"        ").c_str(), "nhs_map");
        }
        ExtentPair nhs;
        if (nhs_map.size()>0) {
            nhs = *(nhs_map.begin());
        } else {
            nhs = sp;
        }

        /* What sections are in the neighborhood (including S), and right of the neighborhood? */
        neighbors.clear(); /*sections in neighborhood*/
        neighbors.push_back(s);
        villagers.clear(); /*sections right of neighborhood*/
        if (debug)
            fprintf(stderr, "%s    Ignoring left (L) sections:\n", p);
        for (size_t i=0; i<all.size(); i++) {
            SgAsmGenericSection *a = all[i];
            if (a==s) continue; /*already pushed onto neighbors*/
            ExtentPair ap;
            if (filespace) {
                ap = a->get_file_extent();
            } else if (!a->is_mapped()) {
                continue;
            } else {
                ap = a->get_mapped_extent();
            }
            switch (ExtentMap::category(ap, nhs)) {
              case 'L':
                if (debug)
                    fprintf(stderr, "%s        L 0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64" [%d] \"%s\"\n", 
                            p, ap.first, ap.second, ap.first+ap.second, a->get_id(), a->get_name()->c_str());
                break;
              case 'R':
                if (ap.first==nhs.first+nhs.second && 0==ap.second) {
                    /* Empty sections immediately right of the neighborhood of S should actually be considered part of the
                     * neighborhood rather than right of it. */
                    neighbors.push_back(a);
                } else if (elasticity!=ELASTIC_NONE) {
                    /* If holes are elastic then treat things right of the hole as being part of the right village; otherwise
                     * add those sections to the neighborhood of S even though they fall outside 'nhs' (it's OK because this
                     * partitioning of sections is the only thing we use 'nhs' for anyway. */
                    villagers.push_back(a);
                } else if ('L'==ExtentMap::category(ap, sp)) {
                    /*ignore sections left of S*/
                } else {
                    neighbors.push_back(a);
                }
                break;
              default:
                if ('L'!=ExtentMap::category(ap, sp)) /*ignore sections left of S*/
                    neighbors.push_back(a);
                break;
            }
        }
        if (debug) {
            fprintf(stderr, "%s    Neighbors:\n", p);
            for (size_t i=0; i<neighbors.size(); i++) {
                SgAsmGenericSection *a = neighbors[i];
                ExtentPair ap = filespace ? a->get_file_extent() : a->get_mapped_extent();
                addr_t align = filespace ? a->get_file_alignment() : a->get_mapped_alignment();
                char cat = ExtentMap::category(ap, sp);
                fprintf(stderr, "%s        %c %c0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64,
                        p, cat, 0==ap.first % (align?align:1) ? ' ' : '!', ap.first, ap.second, ap.first+ap.second);
                if (strchr("RICE", cat)) {
                    fprintf(stderr, " align=0x%08"PRIx64, align);
                } else {
                    fputs("                 ", stderr);
                }
                fprintf(stderr, " [%2d] \"%s\"\n", a->get_id(), a->get_name()->c_str());
            }
            if (villagers.size()>0) fprintf(stderr, "%s    Villagers:\n", p);
            for (size_t i=0; i<villagers.size(); i++) {
                SgAsmGenericSection *a = villagers[i];
                ExtentPair ap = filespace ? a->get_file_extent() : a->get_mapped_extent();
                addr_t align = filespace ? a->get_file_alignment() : a->get_mapped_alignment();
                fprintf(stderr, "%s        %c %c0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64,
                        p, ExtentMap::category(ap, sp), /*cat should always be R*/
                        0==ap.first % (align?align:1) ? ' ' : '!', ap.first, ap.second, ap.first+ap.second);
                fputs("                 ", stderr);
                fprintf(stderr, " [%2d] \"%s\"\n", a->get_id(), a->get_name()->c_str());
            }
        }
        
        /* Adjust Sa to satisfy all alignment constraints in neighborhood(S) for sections that will move (cats R, I, C, and E). */
        align = 1;
        for (size_t i=0; i<neighbors.size(); i++) {
            SgAsmGenericSection *a = neighbors[i];
            ExtentPair ap = filespace ? a->get_file_extent() : a->get_mapped_extent();
            if (strchr("RICE", ExtentMap::category(ap, sp))) {
                addr_t x = filespace ? a->get_file_alignment() : a->get_mapped_alignment();
                align = boost::math::lcm(align, x?x:1);
            }
        }
        aligned_sa = (sa/align + (sa%align?1:0))*align;
        aligned_sasn = ((sa+sn)/align + ((sa+sn)%align?1:0))*align;
        if (debug) {
            fprintf(stderr, "%s    Alignment LCM = 0x%08"PRIx64" (%"PRIu64")\n", p, align, align);
            fprintf(stderr, "%s    Aligned Sa    = 0x%08"PRIx64" (%"PRIu64")\n", p, aligned_sa, aligned_sa);
            fprintf(stderr, "%s    Aligned Sa+Sn = 0x%08"PRIx64" (%"PRIu64")\n", p, aligned_sasn, aligned_sasn);
        }
        
        /* Are there any sections to the right of neighborhood(S)? If so, find the one with the lowest start address and use
         * that to define the size of the hole right of neighborhood(S). */
        if (0==villagers.size()) break;
        SgAsmGenericSection *after_hole = NULL;
        ExtentPair hp(0, 0);
        for (size_t i=0; i<villagers.size(); i++) {
            SgAsmGenericSection *a = villagers[i];
            ExtentPair ap = filespace ? a->get_file_extent() : a->get_mapped_extent();
            if (!after_hole || ap.first<hp.first) {
                after_hole = a;
                hp = ap;
            }
        }
        ROSE_ASSERT(after_hole);
        ROSE_ASSERT(hp.first > nhs.first+nhs.second);
        addr_t hole_size = hp.first - (nhs.first+nhs.second);
        if (debug) {
            fprintf(stderr, "%s    hole size = 0x%08"PRIx64" (%"PRIu64"); need 0x%08"PRIx64" (%"PRIu64"); %s\n",
                    p, hole_size, hole_size, aligned_sasn, aligned_sasn,
                    hole_size>=aligned_sasn ? "large enough" : "not large enough");
        }
        if (hole_size >= aligned_sasn) break;
        addr_t need_more = aligned_sasn - hole_size;

        /* Hole is not large enough. We need to recursively move things that are right of our neighborhood, then recompute the
         * all-sections address map and neighborhood(S). */
        ROSE_ASSERT(0==pass); /*logic problem since the recursive call should have enlarged the hole enough*/
        if (debug) {
            fprintf(stderr, "%s    Calling recursively to increase hole size by 0x%08"PRIx64" (%"PRIu64") bytes\n",
                    p, need_more, need_more);
        }
        shift_extend(after_hole, need_more, 0, space, elasticity);
        if (debug) fprintf(stderr, "%s    Returned from recursive call\n", p);
    }

    /* Consider sections that are in the same neighborhood as S */
    if (debug) fprintf(stderr, "%s    -- ADJUSTING --\n", p);
    bool resized_mem = false;
    for (size_t i=0; i<neighbors.size(); i++) {
        SgAsmGenericSection *a = neighbors[i];
        ExtentPair ap = filespace ? a->get_file_extent() : a->get_mapped_extent();
        switch (ExtentMap::category(ap, sp)) {
          case 'L':
            break;
          case 'R':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sasn);
            } else {
                a->set_mapped_rva(a->get_mapped_rva()+aligned_sasn);
            }
            break;
          case 'C': /*including S itself*/
          case 'E':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sa);
                a->set_size(a->get_size()+sn);
                if (memspace && !resized_mem && a->is_mapped()) {
                    shift_extend(a, 0, sn, ADDRSP_MEMORY, elasticity);
                    resized_mem = true;
                }
            } else {
                a->set_mapped_rva(a->get_mapped_rva()+aligned_sa);
                a->set_mapped_size(a->get_mapped_size()+sn);
            }
            break;
          case 'O':
            if (ap.first==sp.first) {
                if (filespace) {
                    a->set_offset(a->get_offset()+aligned_sa);
                    a->set_size(a->get_size()+sn);
                } else {
                    a->set_mapped_rva(a->get_mapped_rva()+aligned_sa);
                    a->set_mapped_size(a->get_mapped_size()+sn);
                }
            } else {
                if (filespace) {
                    a->set_size(a->get_size()+aligned_sasn);
                    if (memspace && !resized_mem && a->is_mapped()) {
                        shift_extend(a, 0, aligned_sasn, ADDRSP_MEMORY, elasticity);
                        resized_mem = true;
                    }
                } else {
                    a->set_mapped_size(a->get_mapped_size()+aligned_sasn);
                }
            }
            break;
          case 'I':
            if (filespace) {
                a->set_offset(a->get_offset()+aligned_sa);
            } else {
                a->set_mapped_rva(a->get_mapped_rva()+aligned_sa);
            }
            break;
          case 'B':
            if (filespace) {
                a->set_size(a->get_size()+sn);
                if (memspace && !resized_mem && a->is_mapped()) {
                    shift_extend(a, 0, sn, ADDRSP_MEMORY, elasticity);
                    resized_mem = true;
                }
            } else {
                a->set_mapped_size(a->get_size()+sn);
            }
            break;
          default:
            ROSE_ASSERT(!"invalid extent category");
            break;
        }
        if (debug) {
            addr_t x = filespace ? a->get_file_alignment() : a->get_mapped_alignment();
            fprintf(stderr, "%s   %c0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64,
                    p, 0==ap.first%(x?x:1)?' ':'!', ap.first, ap.second, ap.first+ap.second);
            ExtentPair newap = filespace ? a->get_file_extent() : a->get_mapped_extent();
            fprintf(stderr, " -> %c0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64,
                    0==newap.first%(x?x:1)?' ':'!', newap.first, newap.second, newap.first+newap.second);
            fprintf(stderr, " [%2d] \"%s\"\n", a->get_id(), a->get_name()->c_str());
        }
    }
    if (debug) fprintf(stderr, "%s    -- END --\n", p);
}

/* Print basic info about the sections of a file */
void
SgAsmGenericFile::dump(FILE *f) const
{
    SgAsmGenericSectionPtrList sections = get_sections();
    if (sections.size()==0) {
        fprintf(f, "No sections defined for file.\n");
        return;
    }
    
    /* Sort sections by offset (lowest to highest), then size (largest to smallest but zero-sized entries first) */
    for (size_t i = 1; i < sections.size(); i++) {
        for (size_t j=0; j<i; j++) {
            if (sections[j]->get_offset() == sections[i]->get_offset()) {
                addr_t size_i = sections[i]->get_size();
                if (0==size_i) size_i = ~(addr_t)0;
                addr_t size_j = sections[j]->get_size();
                if (0==size_j) size_j = ~(addr_t)0;
                if (size_j < size_i) {
                    SgAsmGenericSection *x = sections[j];
                    sections[j] = sections[i];
                    sections[i] = x;
                }
            } else if (sections[j]->get_offset() > sections[i]->get_offset()) {
                SgAsmGenericSection *x = sections[j];
                sections[j] = sections[i];
                sections[i] = x;
            }
        }
    }
    
    /* Print results */
    fprintf(f, "File sections:\n");
    fprintf(f, "  Flg File-Addr  File-Size  File-End    Base-VA    Start-RVA  Virt-Size  End-RVA    Perm  ID Name\n");
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---------- ---- --- -----------------\n");
    addr_t high_water = 0;
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmGenericSection *section = sections[i];
        
        /* Does section overlap with any other (before or after)? */
        char overlap[4] = "   "; /* status characters: overlap prior, overlap subsequent, hole */
        for (size_t j=0; overlap[0]==' ' && j<i; j++) {
            if (sections[j]->get_offset()+sections[j]->get_size() > section->get_offset()) {
                overlap[0] = '<';
            }
        }
        for (size_t j=i+1; overlap[1]==' ' && j<sections.size(); j++) {
            if (section->get_offset()+section->get_size() > sections[j]->get_offset()) {
                overlap[1] = '>';
            }
        }

        /* Is there a hole before section[i]? */
        if (high_water < section->get_offset()) {
            overlap[2] = 'H'; /* truly unaccounted region of the file */
        } else if (i>0 && sections[i-1]->get_offset()+sections[i-1]->get_size() < section->get_offset()) {
            overlap[2] = 'h'; /* unaccounted only if overlaps are not allowed */
        }
        high_water = std::max(high_water, section->get_offset() + section->get_size());
        fprintf(f, "  %3s", overlap);

        /* File addresses */
        fprintf(f, "%c0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64,
                section->get_file_alignment()==0 || section->get_offset()%section->get_file_alignment()==0?' ':'!',
                section->get_offset(), section->get_size(), section->get_offset()+section->get_size());

        /* Mapped addresses */
        if (section->is_mapped()) {
            fprintf(f, " %c0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64" 0x%08"PRIx64,
                    section->get_mapped_alignment()==0 || section->get_mapped_rva()%section->get_mapped_alignment()==0?' ':'!',
                    section->get_base_va(), section->get_mapped_rva(), section->get_mapped_size(),
                    section->get_mapped_rva()+section->get_mapped_size());
        } else {
            fprintf(f, " %*s", 4*11, "");
        }

        /* Permissions */
        if (section->is_mapped()) {
            fprintf(f, " %c%c%c ",
                    section->get_mapped_rperm()?'r':'-',
                    section->get_mapped_wperm()?'w':'-', 
                    section->get_mapped_xperm()?'x':'-');
        } else {
            fputs("     ", f);
        }

        /* Section ID, name */
        if (section->get_id()>=0) {
            fprintf(f, " %3d", section->get_id());
        } else {
            fputs("    ", f);
        }
        fprintf(f, " %s\n", section->get_name()->c_str());
    }

    char overlap[4] = "   ";
    if (high_water < get_current_size()) {
        overlap[2] = 'H';
    } else if (sections.back()->get_offset() + sections.back()->get_size() < get_current_size()) {
        overlap[2] = 'h';
    }
    fprintf(f, "  %3s 0x%08"PRIx64"%*s EOF", overlap, get_current_size(), 76, "");
    if (get_current_size()!=p_data.size())
        fprintf(f, " (original EOF was 0x%08zx)", p_data.size());
    if (get_truncate_zeros())
        fputs(" [ztrunc]", f);
    fputc('\n', f);
    fprintf(f, "  --- ---------- ---------- ----------  ---------- ---------- ---------- ---------- ---- --- -----------------\n");

    /* Show the simulated loader memory map */
    const RvaFileMap *map = get_loader_map();
    if (map) {
        fprintf(f, "Simulated loader memory map:\n");
        map->dump(f, "    ");
    }

    /* Show what part of the file has not been referenced */
    ExtentMap holes = get_unreferenced_extents();
    if (holes.size()>0) {
        fprintf(f, "These parts of the file have not been referenced during parsing:\n");
        holes.dump_extents(f, "    ", "", false);
    }
}

/** Synthesizes "hole" sections to describe the parts of the file that are not yet referenced by other sections. Note that holes
 *  are used to represent parts of the original file data, before sections were modified by walking the AST (at this time it is
 *  not possible to create a hole outside the original file content). */
void
SgAsmGenericFile::fill_holes()
{
    /* Get the list of file extents referenced by all file sections */
    ExtentMap refs;
    SgAsmGenericSectionPtrList sections = get_sections();
    for (SgAsmGenericSectionPtrList::iterator i=sections.begin(); i!=sections.end(); ++i) {
        refs.insert((*i)->get_offset(), (*i)->get_size());
    }

    /* The hole extents are everything other than the sections */
    ExtentMap holes = refs.subtract_from(0, p_data.size());

    /* Create the sections representing the holes */
    for (ExtentMap::iterator i=holes.begin(); i!=holes.end(); ++i) {
      	SgAsmGenericSection *hole = new SgAsmGenericSection(this, NULL);
        hole->set_offset((*i).first);
        hole->set_size((*i).second);
        hole->parse();
      	hole->set_synthesized(true);
      	hole->set_name(new SgAsmBasicString("hole"));
      	hole->set_purpose(SgAsmGenericSection::SP_UNSPECIFIED);
      	add_hole(hole);
    }
}

/* Deletes "hole" sections */
void
SgAsmGenericFile::unfill_holes()
{
    set_isModified(true);

    SgAsmGenericSectionPtrList to_delete = get_holes()->get_sections();
    for (size_t i=0; i<to_delete.size(); i++) {
        SgAsmGenericSection *hole = to_delete[i];
        delete hole;
    }
    
    /* Destructor for holes should have removed links to those holes. */
    ROSE_ASSERT(get_holes()->get_sections().size()==0);
}

/* Call this before unparsing to make sure everything is consistent. */
void
SgAsmGenericFile::reallocate()
{
    bool reallocated;
    do {
        reallocated = false;

        /* holes */
        for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i) {
            if ((*i)->reallocate())
                reallocated = true;
        }

        /* file headers (and indirectly, all that they reference) */
        for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i) {
            if ((*i)->reallocate())
                reallocated = true;
        }
    } while (reallocated);
}

/* Mirror image of parsing an executable file. The result (unless the AST has been modified) should be identical to the
 * original file. */
void
SgAsmGenericFile::unparse(std::ostream &f) const
{
#if 0
    /* This is only for debugging -- fill the file with something other than zero so we have a better chance of making sure
     * that all data is written back to the file, including things that are zero. */
    addr_t remaining = get_current_size();
    unsigned char buf[4096];
    memset(buf, 0xaa, sizeof buf);
    while (remaining>=sizeof buf) {
        f.write((const char*)buf, sizeof buf);
        ROSE_ASSERT(f);
        remaining -= sizeof buf;
    }
    f.write((const char*)buf, remaining);
    ROSE_ASSERT(f);
#endif

    /* Write unreferenced sections (i.e., "holes") back to disk */
    for (SgAsmGenericSectionPtrList::iterator i=p_holes->get_sections().begin(); i!=p_holes->get_sections().end(); ++i)
        (*i)->unparse(f);
    
    /* Write file headers (and indirectly, all that they reference) */
    for (SgAsmGenericHeaderPtrList::iterator i=p_headers->get_headers().begin(); i!=p_headers->get_headers().end(); ++i)
        (*i)->unparse(f);
}

/* Extend the output file by writing the last byte if it hasn't been written yet. */
void
SgAsmGenericFile::extend_to_eof(std::ostream &f)
{
    f.seekp(0, std::ios::end);
    if (f.tellp()<(off_t)get_current_size()) {
        f.seekp(get_current_size()-1);
        const char zero = '\0';
        f.write(&zero, 1);
    }
}


/* Return a string describing the file format. We use the last header so that files like PE, NE, LE, LX, etc. which also have
 * a DOS header report the format of the second (PE, etc.) header rather than the DOS header. */
const char *
SgAsmGenericFile::format_name() const
{
    return p_headers->get_headers().back()->format_name();
}

/* Returns the header for the specified format. */
SgAsmGenericHeader *
SgAsmGenericFile::get_header(SgAsmGenericFormat::ExecFamily efam)
{
    SgAsmGenericHeader *retval = NULL;
    for (size_t i = 0; i < p_headers->get_headers().size(); i++) {
        if (p_headers->get_headers()[i]->get_exec_format()->get_family() == efam) {
            ROSE_ASSERT(NULL == retval);
            retval = p_headers->get_headers()[i];
        }
    }

    return retval;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GenericSection
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Section constructors set the optional section header relationship--a bidirectional link between this new section and its
 *  optional, single header.  This new section points to its header and the header contains a list that points to this new
 *  section.  The section-to-header part of the link is deleted by the default destructor by virtue of being a simple pointer,
 *  but we also need to delete the other half of the link in the destructors. */
void
SgAsmGenericSection::ctor(SgAsmGenericFile *ef, SgAsmGenericHeader *hdr)
{
    ROSE_ASSERT(ef != NULL);

    ROSE_ASSERT(p_name==NULL);
    p_name = new SgAsmBasicString("");

    /* Add this section to the header's section list */
    if (hdr)
        hdr->add_section(this);
}

/* Destructor must remove section/header link */
SgAsmGenericSection::~SgAsmGenericSection()
{
    SgAsmGenericFile* ef = get_file();
    SgAsmGenericHeader *hdr = get_header();

    /* See constructor comment. This deletes both halves of the header/section link. */
    if (hdr) {
        hdr->remove_section(this);
        set_header(NULL);
    }
    
    /* FIXME: holes should probably be their own class, which would make the file/hole bidirectional linking more like the
     *        header/section bidirectional links (RPM 2008-09-02) */
    ef->remove_hole(this);

    /* Delete children */
    /*not a child*/;     p_file   = NULL;
    delete p_name;       p_name   = NULL;

    /* If the section has allocated its own local pool for the p_data member (rather than pointing into the SgAsmGenericFile)
     * then free that now. */
    if (local_data_pool!=NULL) {
        free(local_data_pool);
        local_data_pool = NULL;
    }
}

/** Saves a reference to the original file data for a section based on the sections current offset and size. Once we do this,
 *  changing the offset or size of the file will not affect the original data. The original data can be extended, however, by
 *  calling SgAsmGenericSection::extend(), which is typically done during parsing. */
void
SgAsmGenericSection::grab_content()
{
    SgAsmGenericFile *ef = get_file();
    ROSE_ASSERT(ef);

    if (get_offset()<=ef->get_orig_size()) {
        if (get_offset()+get_size()<=ef->get_orig_size()) {
            p_data = ef->content(get_offset(), get_size());
        } else {
            p_data = ef->content(get_offset(), ef->get_orig_size()-get_offset());
        }
    }
}

/** Sections typically point into the memory mapped, read-only file stored in the SgAsmGenericFile parent initialized by
 *  calling grab_content() (or indirectly by calling parse()).  This is also the same data which is, by default, written back
 *  out to the new file during unparse().  Programs modify section content by either overriding the unparse() method or by
 *  modifying the p_data values. But in order to modify p_data we have to make sure that it's pointing to a read/write memory
 *  pool. This function replaces the read-only memory pool with a new one containing @p nbytes bytes of zeros. */
unsigned char *
SgAsmGenericSection::local_content(size_t nbytes)
{
    if (local_data_pool!=NULL)
        free(local_data_pool);
    local_data_pool = (unsigned char*)calloc(nbytes, 1);
    p_data = SgSharedVector<unsigned char>(local_data_pool, nbytes);
    return &(p_data[0]);
}

/* Accessors for section name. Setting the section name makes the SgAsmGenericString node a child of the section. */
SgAsmGenericString *
SgAsmGenericSection::get_name() const 
{
    return p_name;
}
void
SgAsmGenericSection::set_name(SgAsmGenericString *s)
{
    if (s)
        s->set_parent(this);
    if (p_name!=s)
        set_isModified(true);
    p_name = s;
}

/* Returns the current file size of the section in bytes. The original size of the section (available when parse() is called
 * for the function, but possibly updated while parsing) is available through the size of the original data: p_data.size() */
rose_addr_t
SgAsmGenericSection::get_size() const
{
    return p_size;
}

/* Adjust the current size of a section. This is virtual because some sections may need to do something special. This function
 * should not adjust the size of other sections, or the mapping of any section (see SgAsmGenericFile::resize() for that). */
void
SgAsmGenericSection::set_size(addr_t size)
{
    if (p_size!=size)
        set_isModified(true);
    p_size = size;
}

/* Returns current file offset of section in bytes. */
rose_addr_t
SgAsmGenericSection::get_offset() const
{
    return p_offset;
}

/* Adjust the current offset of a section. This is virtual because some sections may need to do something special. This
 * function should not adjust the offset of other sections, or the mapping of any section. */
void
SgAsmGenericSection::set_offset(addr_t offset)
{
    if (p_offset!=offset)
        set_isModified(true);
    p_offset = offset;
}

/* Returns starting byte offset in the file */
rose_addr_t
SgAsmGenericSection::get_end_offset()
{
    return get_offset() + get_size();
}

/* Returns the file extent for the section */
ExtentPair
SgAsmGenericSection::get_file_extent() const 
{
    return ExtentPair(get_offset(), get_size());
}

/* Returns whether section desires to be mapped to memory */
bool
SgAsmGenericSection::is_mapped() const
{
    return (get_mapped_rva()!=0 || get_mapped_size()!=0 ||
            get_mapped_rperm() || get_mapped_wperm() || get_mapped_xperm());
}

/* Causes section to not be mapped to memory. */
void
SgAsmGenericSection::clear_mapped()
{
    set_mapped_size(0);
    set_mapped_rva(0);
    set_mapped_rperm(false);
    set_mapped_wperm(false);
    set_mapped_xperm(false);
}

/* Returns mapped size of section. */
rose_addr_t
SgAsmGenericSection::get_mapped_size() const
{
    ROSE_ASSERT(this != NULL);
    return p_mapped_size;
}

/* Resizes a mapped section without consideration of other sections that might be mapped. See also
 * SgAsmGenericFile::mapped_resize(). */
void
SgAsmGenericSection::set_mapped_size(addr_t size)
{
    ROSE_ASSERT(this != NULL);
    if (p_mapped_size!=size)
        set_isModified(true);
    p_mapped_size = size;
}

/* Returns relative virtual address w.r.t., base address of header */
rose_addr_t
SgAsmGenericSection::get_mapped_rva() const
{
    ROSE_ASSERT(this != NULL);
    return p_mapped_rva;
}

/* Moves a mapped section without consideration of other sections that might be mapped. */
void
SgAsmGenericSection::set_mapped_rva(addr_t a)
{
    ROSE_ASSERT(this != NULL);
    if (p_mapped_rva!=a)
        set_isModified(true);
    p_mapped_rva = a;
}

/* Returns (non-relative) virtual address if mapped, zero otherwise. */
rose_addr_t
SgAsmGenericSection::get_mapped_va()
{
    ROSE_ASSERT(this != NULL);
    if (is_mapped())
        return get_base_va() + get_mapped_rva();
    return 0;
}

/* Returns base virtual address for a section, or zero if the section is not associated with a header. */
rose_addr_t
SgAsmGenericSection::get_base_va() const
{
    ROSE_ASSERT(this != NULL);
    SgAsmGenericHeader *hdr = get_header();
    return hdr ? hdr->get_base_va() : 0;
}

/* Returns the memory extent for a mapped section. If the section is not mapped then offset and size will be zero */
ExtentPair
SgAsmGenericSection::get_mapped_extent() const
{
    ROSE_ASSERT(this != NULL);
    return ExtentPair(get_mapped_rva(), get_mapped_size());
}

/** Reads data from a file. Reads up to @p bytes of data beginning at byte @p start_offset from the beginning of the file,
 *  placing the results in @p dst_buf and returning the number of bytes read. The return value could be smaller than @p size
 *  if the end-of-file is reached. If the return value is smaller than @p size then one of two things happen: if @p strict is
 *  set (the default) then an SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise the @p dst_buf will be
 *  padded with zero bytes so that exactly @p size bytes of @p dst_buf are always initialized. */
size_t
SgAsmGenericSection::read_content(addr_t start_offset, unsigned char *dst_buf, addr_t size, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    return file->read_content(start_offset, dst_buf, size, strict);
}

/** Reads data from a file. Reads up to @p bytes of data beginning at byte @p start_rva in the mapped address space and
 *  placing the results in @p dst_buf and returning the number of bytes read. The return value could be smaller than @p size
 *  if the reading encounters virtual addresses that are not mapped.  When an unmapped virtual address is encountered the
 *  reading stops (even if subsequent virtual addresses are defined) and one of two things happen: if @p strict is set (the
 *  default) then an RvaFileMap::NotMapped exception is thrown, otherwise the @p dst_buf is padded with zeros so that all @p
 *  size bytes are initialized. The @p map is used to map virtual addresses to file offsets; if @p map is NULL then the map
 *  defined in the underlying file is used. */
size_t
SgAsmGenericSection::read_content(const RvaFileMap *map, addr_t start_rva, unsigned char *dst_buf, addr_t size, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    return file->read_content(map, start_rva, dst_buf, size, strict);
}

/** Reads data from a file. This behaves the same as read_content() except the @p start_offset is relative to the beginning of
 *  this section.   Reading past the end of the section is not allowed and treated as a short read, and one of two things
 *  happen: if @p strict is set (the default) then an SgAsmExecutableFileFormat::ShortRead exception is thrown, otherwise the
 *  result is zero padded so as to contain exactly @p size bytes. */
size_t
SgAsmGenericSection::read_content_local(addr_t start_offset, unsigned char *dst_buf, addr_t size, bool strict)
{
    size_t retval;
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    if (start_offset > get_size()) {
        if (strict)
            throw ShortRead(this, start_offset, size);
        retval = 0;
    } else if (start_offset+size > get_size()) {
        if (strict)
            throw ShortRead(this, get_size(), start_offset+size-get_size());
        retval = get_size() - start_offset;
    } else {
        retval = size;
    }

    file->read_content(get_offset()+start_offset, dst_buf, retval, true);
    memset(dst_buf+retval, 0, size-retval);
    return retval;
}

/** Reads content of a section and returns it as a container.  The returned container will always have exactly @p size byte.
 *  If @p size bytes are not available in this section at the specified offset then the container will be zero padded. */
SgUnsignedCharList
SgAsmGenericSection::read_content_ucl(addr_t rel_offset, addr_t size)
{
    SgUnsignedCharList retval;
    unsigned char *buf = new unsigned char[size];
    read_content_local(rel_offset, buf, size, false); /*zero pads; never throws*/
    for (size_t i=0; i<size; i++)
        retval.push_back(buf[i]);
    delete[] buf;
    return retval;
}

/** Reads a string from the file. The string begins at the specified virtual address and continues until the first NUL byte or
 *  until we reach an address that is not mapped. However, if @p strict is set (the default) and we reach an unmapped address
 *  then an RvaFileMap::NotMapped exception is thrown. The @p map defines the mapping from virtual addresses to file offsets;
 *  if @p map is NULL then the map defined in the underlying file is used. */
std::string
SgAsmGenericSection::read_content_str(const RvaFileMap *map, addr_t start_rva, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    return file->read_content_str(map, start_rva, strict);
}

/** Reads a string from the file. The string begins at the specified absolute file offset and continues until the first NUL
 *  byte or end of file is reached. However, if @p strict is set (the default) and we reach the end-of-file then an
 *  SgAsmExecutableFileFormat::ShortRead exception is thrown. */
std::string
SgAsmGenericSection::read_content_str(addr_t abs_offset, bool strict)
{
    SgAsmGenericFile *file = get_file();
    ROSE_ASSERT(file!=NULL);
    return file->read_content_str(abs_offset, strict);
}

/** Reads a string from the file. The string begins at the specified file offset relative to the start of this section and
 *  continues until the first NUL byte or the end of section is reached. However, if @p strict is set (the default) and we
 *  reach the end-of-section then an SgAsmExecutableFileFormat::ShortRead exception is thrown. */
std::string
SgAsmGenericSection::read_content_local_str(addr_t rel_offset, bool strict)
{
    static char *buf=NULL;
    static size_t nalloc=0;
    size_t nused=0;
    addr_t base = get_offset();

    while (1) {
        if (nused >= nalloc) {
            nalloc = std::max((size_t)32, 2*nalloc);
            buf = (char*)realloc(buf, nalloc);
            ROSE_ASSERT(buf!=NULL);
        }

        unsigned char byte;
        if (rel_offset+nused>get_size()) {
            if (strict)
                throw ShortRead(this, rel_offset+nused, 1);
            byte = '\0';
        } else {
            read_content(base+rel_offset+nused, &byte, 1, strict); /*might throw ShortRead or return a NUL*/
        }
        if (!byte)
            return std::string(buf, nused);
        buf[nused++] = byte;
    }
}
    

/* Returns ptr to content at specified offset after ensuring that the required amount of data is available. One can think of
 * this function as being similar to fseek()+fread() in that it returns contents of part of a file as bytes. The main
 * difference is that instead of the caller supplying the buffer, the callee uses its own buffer (part of the buffer that was
 * returned by the OS from the mmap of the binary file).  The content() functions also keep track of what parts of the section
 * have been returned so that it's easy to find the parts that are apparently unused. */
/* DEPRECATED: use read_content() instead */
const unsigned char *
SgAsmGenericSection::content(addr_t offset, addr_t size)
{
    if (offset > p_data.size() || offset+size > p_data.size())
        throw ShortRead(this, offset, size);
    get_file()->mark_referenced_extent(get_offset()+offset, size);
    return &(p_data[offset]);
}

/* Copies the specified part of the section into a buffer. This is more like fread() than the two-argument version in that the
 * caller must supply the buffer (the two-arg version returns a ptr to the mmap'd memory). Any part of the selected area that
 * is outside the domain of the section will be filled with zero (in contrast to the two-argument version that throws an
 * exception). */
/* DEPRECATED: use read_content() instead */
void
SgAsmGenericSection::content(addr_t offset, addr_t size, void *buf)
{
    if (offset >= p_data.size()) {
        memset(buf, 0, size);
    } else if (offset+size > p_data.size()) {
        addr_t nbytes = p_data.size() - offset;
        memcpy(buf, &(p_data[offset]), nbytes);
        memset((char*)buf+nbytes, 0, size-nbytes);
    } else {
        memcpy(buf, &(p_data[offset]), size);
    }
    get_file()->mark_referenced_extent(get_offset()+offset, size);
}

/** Returns ptr to a NUL-terminated string. The string is allowed to extend past the end of the section if @p relax is true. */
/* DEPRECATED: use read_content_str() instead */
std::string
SgAsmGenericSection::content_str(addr_t offset, bool relax)
{
    if (offset>=p_data.size())
        return "";

    const char *ret = (const char*)&(p_data[offset]);
    size_t nchars=0;

    while (offset+nchars < p_data.size() && ret[nchars]) nchars++;
    nchars++; /*NUL*/

    if (!relax && offset+nchars>p_data.size())
        throw ShortRead(this, offset, nchars);
    get_file()->mark_referenced_extent(get_offset()+offset, nchars);

    return std::string(ret, nchars-1);
}

/* Like the low-level content(addr_t,addr_t) but returns an object rather than a ptr directly into the file content. This is
 * the recommended way to obtain file content for IR nodes that need to point to that content. The other function is more of a
 * low-level, efficient file read operation. This function is capable of reading past the end of the original data. */
/* DEPRECATED: use read_content_ucl() instead */
const SgUnsignedCharList
SgAsmGenericSection::content_ucl(addr_t offset, addr_t size)
{
    SgUnsignedCharList returnValue;

    addr_t have = size;
    if (offset>=p_data.size()) {
        have = 0;
    } else if (offset+size<=p_data.size()) {
        have = size;
    } else {
        have = p_data.size()-offset;
    }
    
    if (have>0) {
        const unsigned char *data = content(offset, have);
        for (addr_t i=0; i<have; i++)
            returnValue.push_back(data[i]);
    }
    returnValue.resize(size, '\0');
    return returnValue;
}

/** Extract an unsigned LEB128 value and adjust the offset according to how many bytes it occupied. */
/* DEPRECATED: use read_content_uleb128() instead */
uint64_t
SgAsmGenericSection::content_uleb128(rose_addr_t *atp)
{
    int shift=0;
    uint64_t retval=0;
    while (1) {
        unsigned char byte;
        content(*atp, 1, &byte);
        *atp += 1;
        ROSE_ASSERT(shift<64);
        retval |= (byte & 0x7f) << shift;
        shift += 7;
        if (0==(byte & 0x80))
            break;
    }
    return retval;
}

/** Extract a signed LEB128 value and adjust the offset according to how many bytes it occupied. */
/* DEPRECATED: use read_content_sleb128() instead */
int64_t
SgAsmGenericSection::content_sleb128(rose_addr_t *atp)
{
    int shift=0;
    int64_t retval=0;
    while (1) {
        unsigned char byte;
        content(*atp, 1, &byte);
        *atp += 1;
        ROSE_ASSERT(shift<64);
        retval |= (byte & 0x7f) << shift;
        shift += 7;
        if (0==(byte & 0x80))
            break;
    }
    retval = (retval << (64-shift)) >> (64-shift); /*sign extend*/
    return retval;
}


/** Write data to a file section.
 *
 *   @param f       Output steam to which to write
 *   @param offset  Byte offset relative to start of this section
 *   @param bufsize Size of @p buf in bytes
 *   @param buf     Buffer of bytes to be written
 *
 *  @returns Returns the section-relative byte offset for the first byte beyond what would have been written if all bytes
 *  of the buffer were written.
 *
 *  The buffer is allowed to extend past the end of the section as long as the part that extends beyond is all zeros. The
 *  zeros will not be written to the output file.  Furthermore, any trailing zeros that extend beyond the end of the file will
 *  not be written (end-of-file is determined by SgAsmGenericFile::get_orig_size()) */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, addr_t offset, size_t bufsize, const void *buf) const
{
    size_t nwrite, nzero;

    ROSE_ASSERT(this != NULL);

    /* Don't write past end of section */
    if (offset>=get_size()) {
        nwrite = 0;
        nzero  = bufsize;
    } else if (offset+bufsize<=get_size()) {
        nwrite = bufsize;
        nzero = 0;
    } else {
        nwrite = get_size() - offset;
        nzero = bufsize - nwrite;
    }

    /* Don't write past end of current EOF if we can help it. */
    f.seekp(0, std::ios::end);
    addr_t filesize = f.tellp();

#if 0
 // DQ (2/3/2009): Added to help debug problem that I was unable to figure out (handling *.o files).
    if (filesize == 0)
       {
         printf ("In SgAsmGenericSection::write(): filesize = %zu offset = %zu bufsize = %zu \n",filesize,offset,bufsize);
      // return offset+bufsize;
       }
#endif

    while (nwrite>0 && 0==((const char*)buf)[nwrite-1] && get_offset()+offset+nwrite>filesize)
        --nwrite;

    /* Write bytes to file. This is a good place to set a break point if you're trying to figure out what section is writing
     * to a particular file address. For instance, if byte 0x7c is incorrect in the unparsed file you would set a conditional
     * breakpoint for o<=0x7c && o+nwrite>0x7c */
    ROSE_ASSERT(f);
    off_t o = get_offset() + offset;
    f.seekp(o);
    ROSE_ASSERT(f);
    f.write((const char*)buf, nwrite);
    ROSE_ASSERT(f);

    /* Check that truncated data is all zero and fail if it isn't */
    for (size_t i=nwrite; i<bufsize; i++) {
        if (((const char*)buf)[i]) {
            char mesg[1024];
            sprintf(mesg, "non-zero value truncated: buf[0x%zx]=0x%02x", i, ((const unsigned char*)buf)[i]);
#if 1
            fprintf(stderr, "ROBB: SgAsmGenericSection::write(): %s\n", mesg);
            fprintf(stderr, "in [%d] \"%s\"\n", get_id(), get_name()->c_str());
            fprintf(stderr, "section is at file offset 0x%08"PRIx64" (%"PRIu64"), size 0x%"PRIx64" (%"PRIu64") bytes\n", 
                    get_offset(), get_offset(), get_size(), get_size());
            fprintf(stderr, "      ");
            HexdumpFormat hf;
            hf.prefix = "      ";
            hexdump(stderr, get_offset()+offset, (const unsigned char*)buf, bufsize, hf);
            fprintf(stderr, "\n");
            abort(); /*DEBUGGING*/
#endif
            throw SgAsmGenericFile::ShortWrite(this, offset, bufsize, mesg);
        }
    }

    return offset+bufsize;
}

/* See related method above */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, addr_t offset, const SgFileContentList &buf) const
{
    if (0==buf.size())
        return 0;
    return write(f, offset, buf.size(), &(buf[0]));
}

/* See related method above */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, addr_t offset, const SgUnsignedCharList &buf) const
{
    if (0==buf.size())
        return 0;
    return write(f, offset, buf.size(), (void*)&(buf[0]));
}

/* See related method above. */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, addr_t offset, const std::string &str) const
{
    return write(f, offset, str.size(), &(str[0]));
}

/* See related method above. */
rose_addr_t
SgAsmGenericSection::write(std::ostream &f, addr_t offset, char c) const
{
    return write(f, offset, 1, &c);
}

/** Encode an unsigned value as LEB128 and return the next offset. */
rose_addr_t
SgAsmGenericSection::write_uleb128(unsigned char *buf, addr_t offset, uint64_t val) const
{
    if (val==0) {
        buf[offset++] = 0;
    } else {
        while (val) {
            unsigned char byte = val & 0x7f;
            val >>= 7;
            if (val!=0)
                byte |= 0x80;
            buf[offset++] = byte;
        }
    }
    return offset;
}

/** Encode a signed value as LEB128 and return the next offset. */
rose_addr_t
SgAsmGenericSection::write_sleb128(unsigned char *buf, addr_t offset, int64_t val) const
{
    if (val==0) {
        buf[offset++] = 0;
    } else if (val==-1) {
        buf[offset++] = 0x7f;
    } else {
        while (val!=0 && val!=-1) {
            unsigned char byte = (uint64_t)val & 0x7f;
            val >>= 7; /*sign extending*/
            if (val!=0 && val!=-1)
                byte |= 0x80;
            buf[offset++] = byte;
        }
    }
    return offset;
}

/** Returns a list of parts of a single section that have been referenced.  The offsets are relative to the start of the
 *  section. */
ExtentMap
SgAsmGenericSection::get_referenced_extents() const
{
    ExtentMap retval;
    ExtentPair s(get_offset(), get_size());
    const ExtentMap &file_extents = get_file()->get_referenced_extents();
    for (ExtentMap::const_iterator i=file_extents.begin(); i!=file_extents.end(); i++) {
        switch (ExtentMap::category(*i, s)) {
            case 'C': /*congruent*/
            case 'I': /*extent is inside section*/
                retval.insert(i->first-get_offset(), i->second);
                break;
            case 'L': /*extent is left of section*/
            case 'R': /*extent is right of section*/
                break;
            case 'O': /*extent contains all of section*/
                retval.insert(0, get_size());
                break;
            case 'B': /*extent overlaps with beginning of section*/
                retval.insert(0, i->first+i->second - get_offset());
                break;
            case 'E': /*extent overlaps with end of section*/
                retval.insert(i->first-get_offset(), get_offset()+get_size() - i->first);
                break;
            default:
                ROSE_ASSERT(!"invalid extent overlap category");
        }
    }
    return retval;
}

ExtentMap
SgAsmGenericSection::get_unreferenced_extents() const
{
    return get_referenced_extents().subtract_from(0, get_size()); /*complement*/
}

/** Extend a section by some number of bytes during the construction and/or parsing phase. This is function is considered to
 *  be part of the parsing and construction of a section--it changes the part of the file that's considered the "original
 *  size" of the section. To adjust the size of a section after the executable file is parsed, see SgAsmGenericFile::resize().
 *  Sections are allowed to extend beyond the end of the file and the original data (p_data) is extended only up to the end
 *  of the file. */
void
SgAsmGenericSection::extend(addr_t size)
{
    ROSE_ASSERT(get_file() != NULL);
    ROSE_ASSERT(get_file()->get_tracking_references()); /*can only be called during the parsing phase*/
    addr_t new_size = get_size() + size;

    /* Ending file address for section using new size, limited by total file size */
    addr_t new_end = std::min(get_file()->get_orig_size(), get_offset()+new_size);
    if (get_offset()<=new_end) {
        p_data.resize(new_end-get_offset());
    } else {
        ROSE_ASSERT(0==p_data.size());
    }

    if (p_size!=new_size)
        set_isModified(true);
    p_size = new_size;
}

/* True (the SgAsmGenericHeader pointer) if this section is also a top-level file header, false (NULL) otherwise. */
SgAsmGenericHeader *
SgAsmGenericSection::is_file_header()
{
    try {
        SgAsmGenericHeader *retval = dynamic_cast<SgAsmGenericHeader*>(this);
        return retval;
    } catch(...) {
        return NULL;
    }
}

/* Write a section back to the file. This is the generic version that simply writes the content. Subclasses should override
 * this. */
void
SgAsmGenericSection::unparse(std::ostream &f) const
{
#if 0
    /* FIXME: for now we print the names of all sections we dump using this method. Eventually most of these sections will
     *        have subclasses that override this method. */
    fprintf(stderr, "SgAsmGenericSection::unparse(FILE*) for section [%d] \"%s\"\n", id, name.c_str());
#endif

    write(f, 0, p_data);
}

/* Write just the specified regions back to the file */
void
SgAsmGenericSection::unparse(std::ostream &f, const ExtentMap &map) const
{
    for (ExtentMap::const_iterator i=map.begin(); i!=map.end(); ++i) {
        ROSE_ASSERT((*i).first+(*i).second <= get_size());
        const unsigned char *extent_data;
        size_t nwrite;
        if ((*i).first >= p_data.size()) {
            extent_data = NULL;
            nwrite = 0;
        } else if ((*i).first + (*i).second > p_data.size()) {
            extent_data = &p_data[(*i).first];
            nwrite = p_data.size() - (*i).first;
        } else {
            extent_data = &p_data[(*i).first];
            nwrite = (*i).second;
        }
        if (extent_data)
            write(f, (*i).first, (*i).second, extent_data);
    }
}

/* Write holes (unreferenced areas) back to the file */
void
SgAsmGenericSection::unparse_holes(std::ostream &f) const
{
#if 0 /*DEBUGGING*/
    ExtentMap holes = get_unreferenced_extents();
    fprintf(stderr, "Section \"%s\", 0x%"PRIx64" bytes\n", get_name()->c_str(), get_size());
    holes.dump_extents(stderr, "  ", "");
#endif
//    unparse(f, get_unreferenced_extents());
}

/* Returns the file offset associated with the relative virtual address of a mapped section. */
rose_addr_t
SgAsmGenericSection::get_rva_offset(addr_t rva)
{
    return get_va_offset(rva + get_base_va());
}

/* Returns the file offset associated with the virtual address of a mapped section. */
rose_addr_t
SgAsmGenericSection::get_va_offset(addr_t va)
{
    ROSE_ASSERT(is_mapped());
    ROSE_ASSERT(va >= get_base_va());
    addr_t rva = va - get_base_va();
    ROSE_ASSERT(rva >= get_mapped_rva());
    return get_offset() + (rva - get_mapped_rva());
}

/* Class method that prints info about offsets into known sections */
void
SgAsmGenericSection::dump_containing_sections(FILE *f, const std::string &prefix, rose_rva_t rva,
                                              const SgAsmGenericSectionPtrList &slist)
{
    for (size_t i=0; i<slist.size(); i++) {
        SgAsmGenericSection *s = slist[i];
        if (s->is_mapped() && rva>=s->get_mapped_rva() && rva<s->get_mapped_rva()+s->get_mapped_size()) {
            addr_t offset = rva - s->get_mapped_rva();
            fprintf(f, "%-*s   is 0x%08"PRIx64" (%"PRIu64") bytes into section [%d] \"%s\"\n",
                    DUMP_FIELD_WIDTH, prefix.c_str(), offset, offset, s->get_id(), s->get_name()->c_str());
        }
    }
}

/* Print some debugging info */
void
SgAsmGenericSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096], sbuf[256];
    const char *s;
    if (idx>=0) {
        sprintf(p, "%sSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSection.", prefix);
    }
    
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n",                      p, w, "name",        p_name->c_str());
    fprintf(f, "%s%-*s = %d\n",                          p, w, "id",          p_id);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes into file\n", p, w, "offset", p_offset, p_offset);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "size", get_size(), get_size());
    if (0==get_file_alignment()) {
        fprintf(f, "%s%-*s = not specified\n", p, w, "file_alignment");
    } else {
        fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") %s\n", p, w, "file_alignment", 
                get_file_alignment(), get_file_alignment(),
                0==get_offset()%get_file_alignment()?"satisfied":"NOT SATISFIED");
    }
    fprintf(f, "%s%-*s = %s\n",                          p, w, "synthesized", p_synthesized?"yes":"no");
    if (p_header) {
        fprintf(f, "%s%-*s = \"%s\"\n",                  p, w, "header",      p_header->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = not associated\n",          p, w, "header");
    }
    
    switch (p_purpose) {
      case SP_UNSPECIFIED: s = "not specified"; break;
      case SP_PROGRAM:     s = "program-supplied data/code/etc"; break;
      case SP_HEADER:      s = "executable format header";       break;
      case SP_SYMTAB:      s = "symbol table";                   break;
      case SP_OTHER:       s = "other";                          break;
      default:
        sprintf(sbuf, "%u", p_purpose);
        s = sbuf;
        break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", s);

    if (is_mapped()) {
        fprintf(f, "%s%-*s = rva=0x%08"PRIx64", size=%"PRIu64" bytes\n", p, w, "mapped",  p_mapped_rva, p_mapped_size);
        if (0==get_mapped_alignment()) {
            fprintf(f, "%s%-*s = not specified\n", p, w, "mapped_alignment");
        } else {
            fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") %s\n", p, w, "mapped_alignment", 
                    get_mapped_alignment(), get_mapped_alignment(),
                    0==get_mapped_rva()%get_mapped_alignment()?"satisfied":"NOT SATISFIED");
        }
        fprintf(f, "%s%-*s = %c%c%c\n", p, w, "permissions",
                get_mapped_rperm()?'r':'-', get_mapped_wperm()?'w':'-', get_mapped_xperm()?'x':'-');
    } else {
        fprintf(f, "%s%-*s = <not mapped>\n",    p, w, "mapped");
    }

    // DQ (2/4/2009): Added variable to support specification of where code is since in object files
    // sections are marked as non-executable even when they contain code and the disassembler
    // looks only at if sections (maybe segments) are marked executable (this will change to 
    // include sections explicit marked as code using this variable).
    fprintf(f, "%s%-*s = %s\n", p, w, "contains_code", get_contains_code()?"true":"false");
    fprintf(f, "%s%-*s = %"PRIx64" (%"PRIu64") \n", p, w, "rose_mapped_rva", p_rose_mapped_rva,p_rose_mapped_rva);

    // DQ (8/31/2008): Output the contents if this not derived from (there is likely a 
    // better implementation if the hexdump function was a virtual member function).
    if (variantT() == V_SgAsmGenericSection) {
        hexdump(f, 0, std::string(p)+"data at ", p_data);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmGenericSection class methods for manipulating extents and extent maps (e.g., referenced lists, free lists, etc).
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Class method comparing two extents. The return value is one of the following letters, depending on how extent A is related
 * to extent B:
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

/* Return an extent map which contains all extents in (offset,size) that are not in "this" extent map. */
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

/* Adds the specified extent to the map of extents. Coalesce adjacent entries in the map. */
void
ExtentMap::insert(rose_addr_t offset, rose_addr_t size)
{
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

/* Inserts contents of one extent map into another */
void
ExtentMap::insert(const ExtentMap &map)
{
    for (ExtentMap::const_iterator i=map.begin(); i!=map.end(); ++i) {
        insert(*i);
    }
}

/* Returns all extents that (partially) overlap with the specified area */
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

/* Removes the specified extent from the map of extents. */
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

/* Return the extent that's the closest match in size without removing it from the map. If two extents tie for the best fit then
 * return the one with the lower offset. Returns map.end() on failure. */
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

/* Return the extent with the highest offset. */
ExtentMap::iterator
ExtentMap::highest_offset()
{
    ExtentMap::iterator ret = end();
    if (size()>0) --ret;
    return ret;
}

/* Allocate an extent of the specified size (best fit first) from the extent map, removing the returned extent from the map. */
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

/* Allocate an extent of the specified size (first fit) from the extent map, removing the returned extent from the map. */
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

/* Allocate the specified extent, which must be in the free list. */
void
ExtentMap::allocate_at(const ExtentPair &request)
{
    ROSE_ASSERT(subtract_from(request).size()==0); /*entire request should be on free list*/
    erase(request);
}

/* Print info about an extent map */
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmGenericHeader
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor.
 * Headers (SgAsmGenericHeader and derived classes) set the file/header relationship--a bidirectional link between this new
 * header and the single file that contains this new header. This new header points to its file and the file contains a list
 * that points to this new header. The header-to-file half of the link is deleted by the default destructor by virtue of being
 * a simple pointer, but we also need to delete the other half of the link in the destructors. */
void
SgAsmGenericHeader::ctor()
{
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* The bidirectional link between file and header */
    get_file()->add_header(this);

    /* Create child IR nodes and set their parent (initialized to null in real constructor) */
    ROSE_ASSERT(p_symbols == NULL);
    p_symbols = new SgAsmGenericSymbolList;
    p_symbols->set_parent(this);

    ROSE_ASSERT(p_dlls == NULL);
    p_dlls    = new SgAsmGenericDLLList;
    p_dlls->set_parent(this);

    ROSE_ASSERT(p_exec_format == NULL);
    p_exec_format = new SgAsmGenericFormat;
    p_exec_format->set_parent(this);

    ROSE_ASSERT(p_sections == NULL);
    p_sections = new SgAsmGenericSectionList;
    p_sections->set_parent(this);
}

/* Destructor must remove header/file link. */
SgAsmGenericHeader::~SgAsmGenericHeader() 
{
    /* Delete child sections before this */
    SgAsmGenericSectionPtrList to_delete = get_sections()->get_sections();
    for (size_t i=0; i<to_delete.size(); i++) {
        SgAsmGenericSection *section = to_delete[i];
        delete section;
    }

    /* Deletion of section children should have emptied the list of header-to-section links */
    ROSE_ASSERT(p_sections->get_sections().empty() == true);

    /* Destroy the header/file bidirectional link. See comment in constructor. */
    ROSE_ASSERT(get_file()!=NULL);
    get_file()->remove_header(this);
    //set_file(NULL);   -- the file pointer was moved into the superclass in order to be easily available to all sections

    /* Delete children */
    delete p_symbols;     p_symbols     = NULL;
    delete p_dlls;        p_dlls        = NULL;
    delete p_exec_format; p_exec_format = NULL;
    delete p_sections;    p_sections    = NULL;
}

/* Allow all sections to reallocate themselves */
bool
SgAsmGenericHeader::reallocate()
{
    bool reallocated = false;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        if ((*i)->reallocate())
            reallocated = true;
    }
    return reallocated;
}
    
/* Unparse headers and all they point to */
void
SgAsmGenericHeader::unparse(std::ostream &f) const
{
    SgAsmGenericSection::unparse(f);

    /* Unparse each section */
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i)
        (*i)->unparse(f);
}

/** Returns the RVA (relative to the header's base virtual address) of the first entry point. If there are no entry points
 *  defined then return a zero RVA. */
rose_addr_t
SgAsmGenericHeader::get_entry_rva() const
{
    if (p_entry_rvas.size()==0)
        return rose_addr_t();
    return p_entry_rvas[0].get_rva();
}

/* Adds a new section to the header. This is called implicitly by the section constructor. */
void
SgAsmGenericHeader::add_section(SgAsmGenericSection *section)
{
    ROSE_ASSERT(section != NULL);
    ROSE_ASSERT(p_sections != NULL);
    p_sections->set_isModified(true);

#ifndef NDEBUG
    /* New section must not already be present. */
    for (size_t i = 0; i < p_sections->get_sections().size(); i++) {
        ROSE_ASSERT(p_sections->get_sections()[i] != section);
    }
#endif
    section->set_header(this);
    section->set_parent(p_sections);
    p_sections->get_sections().push_back(section);
}

/* Removes a secton from the header's section list. */
void
SgAsmGenericHeader::remove_section(SgAsmGenericSection *section)
{
    if (section!=NULL) {
        ROSE_ASSERT(p_sections != NULL);
        SgAsmGenericSectionPtrList::iterator i = find(p_sections->get_sections().begin(),
                                                        p_sections->get_sections().end(),
                                                        section);
        if (i != p_sections->get_sections().end()) {
            p_sections->get_sections().erase(i);
            p_sections->set_isModified(true);
        }
    }
}

/* Add a new DLL to the header DLL list */
void
SgAsmGenericHeader::add_dll(SgAsmGenericDLL *dll)
{
    ROSE_ASSERT(p_dlls != NULL);
    p_dlls->set_isModified(true);

#ifndef NDEBUG
 // for (size_t i = 0; i < p_dlls.size(); i++) {
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++) {
        ROSE_ASSERT(p_dlls->get_dlls()[i] != dll); /*duplicate*/
    }
#endif
    p_dlls->get_dlls().push_back(dll);

    dll->set_parent(p_dlls);
}

/* Add a new symbol to the symbol table. The SgAsmGenericHeader has a list of symbol pointers. These pointers point to symbols
 * that are defined in various sections throughout the executable. It's not absolutely necessary to store them here since the
 * sections where they're defined also point to them--they're here only for convenience.
 * 
 * FIXME: If symbols are stored in one central location we should probably use something other than an
 *        unsorted list. (RPM 2008-08-19) */
void
SgAsmGenericHeader::add_symbol(SgAsmGenericSymbol *symbol)
{
    ROSE_ASSERT(p_symbols);
    p_symbols->set_isModified(true);

#if 0 /*turned off because too slow!!! (RPM 2008-08-19)*/
#ifndef NDEBUG
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        ROSE_ASSERT(p_symbols->get_symbols()[i] != symbol); /*duplicate*/
    }
#endif
#endif
    p_symbols->get_symbols().push_back(symbol);

    /* FIXME: symbols have two parents: the header's p_symbols list and the list in the section where the symbol was defined.
     *        We probably want to keep them only with the section that defines them. For example, SgAsmElfSymbolSection.
     *        (RPM 2008-08-19) */
    p_symbols->get_symbols().back()->set_parent(p_symbols);
}

/* Returns the list of sections that are memory mapped */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_mapped_sections() const
{
    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        if ((*i)->is_mapped()) {
            retval.push_back(*i);
        }
    }
    return retval;
}
    
/* Returns sections in this header that have the specified ID. */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_id(int id) const
{
    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        if ((*i)->get_id() == id) {
            retval.push_back(*i);
        }
    }
    return retval;
}

/* Returns single section in this header that has the specified ID. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_id(int id, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_id(id);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns sections in this header that have the specified name. If 'SEP' is a non-null string then ignore any part of name at
 * and after SEP. */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_name(std::string name, char sep/*or NUL*/) const
{
    if (sep) {
        size_t pos = name.find(sep);
        if (pos!=name.npos)
            name.erase(pos);
    }

    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        std::string secname = (*i)->get_name()->get_string();
        if (sep) {
            size_t pos = secname.find(sep);
            if (pos!=secname.npos)
                secname.erase(pos);
        }
        if (0==secname.compare(name))
            retval.push_back(*i);
    }
    return retval;
}

/* Returns single section in this header that has the specified name. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_name(const std::string &name, char sep/*or NUL*/, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_name(name, sep);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns sectons in this header that contain all of the specified portion of the file. */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_offset(addr_t offset, addr_t size) const
{
    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i=p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        SgAsmGenericSection *section = *i;
        if (offset >= section->get_offset() &&
            offset < section->get_offset()+section->get_size() &&
            offset-section->get_offset() + size <= section->get_size())
            retval.push_back(section);
    }
    return retval;
}

/* Returns single section in this header that contains all of the specified portion of the file. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_offset(addr_t offset, addr_t size, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_offset(offset, size);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns sections in this header that are mapped to include the specified relative virtual address. */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_rva(addr_t rva) const
{
    SgAsmGenericSectionPtrList retval;
    for (SgAsmGenericSectionPtrList::iterator i = p_sections->get_sections().begin(); i!=p_sections->get_sections().end(); ++i) {
        SgAsmGenericSection *section = *i;
        if (section->is_mapped() &&
            rva >= section->get_mapped_rva() && rva < section->get_mapped_rva() + section->get_mapped_size()) {
            retval.push_back(section);
        }
    }
    return retval;
}

/* Returns single section in this header that is mapped to include the specified relative virtual address. */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_rva(addr_t rva, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_rva(rva);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Returns sections in this header that are mapped to include the specified virtual address */
SgAsmGenericSectionPtrList
SgAsmGenericHeader::get_sections_by_va(addr_t va) const
{
    if (va < get_base_va())
        return SgAsmGenericSectionPtrList();
    addr_t rva = va - get_base_va();
    return get_sections_by_rva(rva);
}

/* Returns single section in this header that is mapped to include the specified virtual address. See also
 * get_best_section_by_va(). */
SgAsmGenericSection *
SgAsmGenericHeader::get_section_by_va(addr_t va, size_t *nfound/*optional*/) const
{
    SgAsmGenericSectionPtrList possible = get_sections_by_va(va);
    if (nfound) *nfound = possible.size();
    return possible.size()==1 ? possible[0] : NULL;
}

/* Like SgAsmGenericFile::get_best_section_by_va() except considers only sections defined in this header. */
SgAsmGenericSection *
SgAsmGenericHeader::get_best_section_by_va(addr_t va, size_t *nfound) const
{
    const SgAsmGenericSectionPtrList &candidates = get_sections_by_va(va);
    if (nfound) *nfound = candidates.size();
    return SgAsmGenericFile::best_section_by_va(candidates, va);
}

/* Print some debugging info */
void
SgAsmGenericHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sHeader.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);

    ROSE_ASSERT(p_exec_format != NULL);
    p_exec_format->dump(f, p, -1);

    fprintf(f, "%s%-*s = 0x%x\n", p, w, "ins_arch", p_isa);

    fprintf(f, "%s%-*s = \"", p, w, "magic");
    for (size_t i = 0; i < p_magic.size(); i++) {
        switch (p_magic[i]) {
          case '\\': fputs("\\\\", f); break;
          case '\n': fputs("\\n", f); break;
          case '\r': fputs("\\r", f); break;
          case '\t': fputs("\\t", f); break;
          default:
            if (isprint(p_magic[i])) {
                fputc(p_magic[i], f);
            } else {
                fprintf(f, "\\%03o", (unsigned)p_magic[i]);
            }
            break;
        }
    }
    fputs("\"\n", f);
    
    /* Base virtual address and entry addresses */
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, "base_va", get_base_va(), get_base_va());
    fprintf(f, "%s%-*s = %zu entry points\n", p, w, "entry_rva.size", p_entry_rvas.size());
    for (size_t i = 0; i < p_entry_rvas.size(); i++) {
        char label[64];
        sprintf(label, "entry_rva[%zu]", i);
        addr_t entry_rva = p_entry_rvas[i].get_rva();
        fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, label, entry_rva, entry_rva);
        SgAsmGenericSectionPtrList sections = get_file()->get_sections();
        dump_containing_sections(f, std::string(p)+label, entry_rva, sections);
    }

    fprintf(f, "%s%-*s = %zu sections\n", p, w, "section", p_sections->get_sections().size());
    for (size_t i=0; i<p_sections->get_sections().size(); i++) {
        SgAsmGenericSection *section = p_sections->get_sections()[i];
        char label[1024];
        sprintf(label, "section[%zu]", i);
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, label, section->get_id(), section->get_name()->c_str());
    }
    
    fprintf(f, "%s%-*s = %zu entries\n", p, w, "DLL.size", p_dlls->get_dlls().size());
    for (size_t i = 0; i < p_dlls->get_dlls().size(); i++)
        p_dlls->get_dlls()[i]->dump(f, p, i);

    fprintf(f, "%s%-*s = %zu entries\n", p, w, "Symbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++)
        p_symbols->get_symbols()[i]->dump(f, p, i);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamically linked libraries
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Accessors for "name" like ROSETTA's except setting name reparents the SgAsmGenericString. */
SgAsmGenericString *
SgAsmGenericDLL::get_name() const 
{
    return p_name;
}
void
SgAsmGenericDLL::set_name(SgAsmGenericString *s)
{
    if (s)
        s->set_parent(this);
    if (p_name!=s)
        set_isModified(true);
    p_name = s;
}

/* Print some debugging info */
void
SgAsmGenericDLL::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sDLL[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sDLL.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "lib_name", p_name->c_str());
    for (size_t i = 0; i < p_symbols.size(); i++)
        fprintf(f, "%s%-*s = [%zd] \"%s\"\n", p, w, "symbol_name", i, p_symbols[i].c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbols and symbol tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
SgAsmGenericSymbol::ctor()
{
    p_name = new SgAsmBasicString("");
}

/* Like ROSETTA-generated accessors, but also sets parent */
SgAsmGenericString *
SgAsmGenericSymbol::get_name() const
{
    return p_name;
}
void
SgAsmGenericSymbol::set_name(SgAsmGenericString *s)
{
    if (s)
        s->set_parent(this);
    if (p_name!=s)
        set_isModified(true);
    p_name = s;
}

std::string
SgAsmGenericSymbol::stringifyDefState() const
{
    std::string s;
    switch (p_def_state) {
      case SYM_UNDEFINED: 
        s = "SYM_UNDEFINED";
        break;
      case SYM_TENTATIVE: 
        s = "SYM_TENTATIVE";
        break;
      case SYM_DEFINED:
        s = "SYM_DEFINED";
        break;
      default:
        printf ("Error: (out of range) p_def_state = %d \n",p_def_state);
        ROSE_ASSERT(false);
    }
    return s;
}

std::string
SgAsmGenericSymbol::stringifyType() const
{
    std::string s;
    switch (p_type) {
      case SYM_NO_TYPE: 
        s = "SYM_NO_TYPE";
        break;
      case SYM_DATA: 
        s = "SYM_DATA";
        break;
      case SYM_FUNC:
        s = "SYM_FUNC";
        break;
      case SYM_SECTION:
        s = "SYM_SECTION";
        break;
      case SYM_FILE:
        s = "SYM_FILE";
        break;
      case SYM_ARRAY:
        s = "SYM_ARRAY";
        break;
      case SYM_TLS:
        s = "SYM_TLS";
        break;
      case SYM_REGISTER:
        s = "SYM_REGISTER";
        break;
      case SYM_COMMON:
        s = "SYM_COMMON";
        break;
      default:
        printf ("Error: (out of range) p_def_state = %d \n",p_def_state);
        ROSE_ASSERT(false);
    }
    return s;
}

std::string
SgAsmGenericSymbol::stringifyBinding() const
{
    std::string s;
    switch (p_binding) {
      case SYM_NO_BINDING: 
        s = "SYM_NO_BINDING";
        break;
      case SYM_LOCAL: 
        s = "SYM_LOCAL";
        break;
      case SYM_GLOBAL:
        s = "SYM_GLOBAL";
        break;
      case SYM_WEAK:
        s = "SYM_WEAK";
        break;
      default:
        printf ("Error: (out of range) p_def_state = %d \n",p_def_state);
        ROSE_ASSERT(false);
    }
    return s;
}

/* Print some debugging info */
void
SgAsmGenericSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = \"%s\"\n", p, w, "name", p_name->c_str());

    const char *s_def_state = NULL;
    switch (p_def_state) {
      case SYM_UNDEFINED: s_def_state = "undefined"; break;
      case SYM_TENTATIVE: s_def_state = "tentative"; break;
      case SYM_DEFINED:   s_def_state = "defined";   break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "def_state", s_def_state);

    const char *s_bind = NULL;
    switch (p_binding) {
      case SYM_NO_BINDING: s_bind = "no-binding"; break;
      case SYM_LOCAL:      s_bind = "local";      break;
      case SYM_GLOBAL:     s_bind = "global";     break;
      case SYM_WEAK:       s_bind = "weak";       break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "binding", s_bind);
    
    const char *s_type = NULL;
    switch (p_type) {
      case SYM_NO_TYPE:  s_type = "no-type";  break;
      case SYM_DATA:     s_type = "data";     break;
      case SYM_FUNC:     s_type = "function"; break;
      case SYM_SECTION:  s_type = "section";  break;
      case SYM_FILE:     s_type = "file";     break;
      case SYM_TLS:      s_type = "thread";   break;
      case SYM_REGISTER: s_type = "register"; break;
      case SYM_ARRAY:    s_type = "array";    break;
      case SYM_COMMON:   s_type = "common";   break;
    }
    fprintf(f, "%s%-*s = %s\n", p, w, "type", s_type);
    if (p_bound) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "bound", p_bound->get_id(), p_bound->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = none\n", p, w, "bound");
    }
    
    fprintf(f, "%s%-*s = 0x%08"PRIx64, p, w, "value", p_value);
    if (p_value > 9) {
        fprintf(f, " (unsigned)%"PRIu64, p_value);
        if ((int64_t)p_value < 0) fprintf(f, " (signed)%"PRId64, (int64_t)p_value);
    }
    fputc('\n', f);

    fprintf(f, "%s%-*s = %"PRIu64" bytes\n", p, w, "size", p_size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


SgAsmExecutableFileFormat::ByteOrder
SgAsmExecutableFileFormat::host_order()
{
    static const int i = 1;
    return *(unsigned char*)&i ? ORDER_LSB : ORDER_MSB;
}

/* Swap (reverse) bytes taking care of sign extension */
uint8_t
SgAsmExecutableFileFormat::swap_bytes(uint8_t n)
{
    return n;
}

uint16_t
SgAsmExecutableFileFormat::swap_bytes(uint16_t n)
{
    return ((n>>8) & 0xff) | ((n<<8) & 0xff00);
}

uint32_t
SgAsmExecutableFileFormat::swap_bytes(uint32_t n)
{
    return ((n>>24) & 0xff) | ((n>>8) & 0xff00) | ((n<<8) & 0xff0000) | ((n<<24) & 0xff000000u);
}

uint64_t
SgAsmExecutableFileFormat::swap_bytes(uint64_t n)
{
    return (((n>>56) & (0xffull<<0 )) | ((n>>40) & (0xffull<<8 )) | ((n>>24) & (0xffull<<16)) | ((n>>8 ) & (0xffull<<24)) |
            ((n<<8 ) & (0xffull<<32)) | ((n<<24) & (0xffull<<40)) | ((n<<40) & (0xffull<<48)) | ((n<<56) & (0xffull<<56)));
}

int8_t
SgAsmExecutableFileFormat::swap_bytes(int8_t n)
{
    return swap_bytes((uint8_t)n);
}

int16_t
SgAsmExecutableFileFormat::swap_bytes(int16_t n)
{
    return swap_bytes((uint16_t)n);
}

int32_t
SgAsmExecutableFileFormat::swap_bytes(int32_t n)
{
    return swap_bytes((uint32_t)n);
}

int64_t
SgAsmExecutableFileFormat::swap_bytes(int64_t n)
{
    return swap_bytes((uint64_t)n);
}

/* Little-endian byte order conversions */
uint8_t
SgAsmExecutableFileFormat::le_to_host(uint8_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

uint16_t
SgAsmExecutableFileFormat::le_to_host(uint16_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

uint32_t
SgAsmExecutableFileFormat::le_to_host(uint32_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

uint64_t
SgAsmExecutableFileFormat::le_to_host(uint64_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

int8_t
SgAsmExecutableFileFormat::le_to_host(int8_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

int16_t
SgAsmExecutableFileFormat::le_to_host(int16_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

int32_t
SgAsmExecutableFileFormat::le_to_host(int32_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

int64_t
SgAsmExecutableFileFormat::le_to_host(int64_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

void
SgAsmExecutableFileFormat::host_to_le(unsigned h, uint8_t *n)
{
    assert(0==(h & ~0xff));
    uint8_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(unsigned h, uint16_t *n)
{
    assert(0==(h & ~0xffff));
    uint16_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(unsigned h, uint32_t *n)
{
    assert(0==(h & ~0xfffffffful));
    uint32_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(addr_t h, uint64_t *n)
{
    assert(0==(h & ~0xffffffffffffffffull));
    uint64_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(rva_t h, uint32_t *n)
{
    host_to_le(h.get_rva(), n);
}

void
SgAsmExecutableFileFormat::host_to_le(rva_t h, uint64_t *n)
{
    host_to_le(h.get_rva(), n);
}

void
SgAsmExecutableFileFormat::host_to_le(int h, int8_t *n)
{
    assert((unsigned)h<=0x8f || ((unsigned)h|0xff)==(unsigned)-1);
    int8_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(int h, int16_t *n)
{
    assert((unsigned)h<=0x8fff || ((unsigned)h|0xffff)==(unsigned)-1);
    int16_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(int h, int32_t *n)
{
    assert((unsigned)h<=0x8fffffffu || ((unsigned)h|0xffffffffu)==(unsigned)-1);
    int32_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(int64_t h, int64_t *n)
{
    *n = ORDER_LSB==host_order() ? h : swap_bytes(h);
}

/* Big-endian byte order conversions */
uint8_t
SgAsmExecutableFileFormat::be_to_host(uint8_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

uint16_t
SgAsmExecutableFileFormat::be_to_host(uint16_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

uint32_t
SgAsmExecutableFileFormat::be_to_host(uint32_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

uint64_t
SgAsmExecutableFileFormat::be_to_host(uint64_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

int8_t
SgAsmExecutableFileFormat::be_to_host(int8_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

int16_t
SgAsmExecutableFileFormat::be_to_host(int16_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

int32_t
SgAsmExecutableFileFormat::be_to_host(int32_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

int64_t
SgAsmExecutableFileFormat::be_to_host(int64_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

void
SgAsmExecutableFileFormat::host_to_be(unsigned h, uint8_t *n)
{
    assert(0==(h & ~0xff));
    uint8_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(unsigned h, uint16_t *n)
{
    assert(0==(h & ~0xffff));
    uint16_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(unsigned h, uint32_t *n)
{
    assert(0==(h & ~0xfffffffful));
    uint32_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(addr_t h, uint64_t *n)
{
    assert(0==(h & ~0xffffffffffffffffull));
    uint64_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(rva_t h, uint32_t *n)
{
    host_to_be(h.get_rva(), n);
}

void
SgAsmExecutableFileFormat::host_to_be(rva_t h, uint64_t *n)
{
    host_to_be(h.get_rva(), n);
}

void
SgAsmExecutableFileFormat::host_to_be(int h, int8_t *n)
{
    assert((unsigned)h<0x8f || ((unsigned)h|0xff)==(unsigned)-1);
    int8_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(int h, int16_t *n)
{
    assert((unsigned)h<0x8fff || ((unsigned)h|0xffff)==(unsigned)-1);
    int16_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(int h, int32_t *n)
{
    assert((unsigned)h<0x8ffffffful || ((unsigned)h|0xfffffffful)==(unsigned)-1);
    int32_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(int64_t h, int64_t *n)
{
    *n = ORDER_MSB==host_order() ? h : swap_bytes(h);
}

/* Caller-specified byte order conversions */
uint8_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, uint8_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

uint16_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, uint16_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

uint32_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, uint32_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

uint64_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, uint64_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

int8_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, int8_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

int16_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, int16_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

int32_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, int32_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

int64_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, int64_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, unsigned h, uint8_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, unsigned h, uint16_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, unsigned h, uint32_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, addr_t h, uint64_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, rva_t h, uint64_t *np)
{
    host_to_disk(sex, h.get_rva(), np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, int h, int8_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, int h, int16_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, int h, int32_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, int64_t h, int64_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const unsigned char *data, size_t n, const HexdumpFormat &fmt)
{
    /* Provide default formats. This is done here so that the header file doesn't depend on <inttypes.h> */
    const char *addr_fmt = fmt.addr_fmt ? fmt.addr_fmt : "0x%08"PRIx64":";
    const char *numeric_fmt = fmt.numeric_fmt ? fmt.numeric_fmt : " %02x";
    const char *prefix = fmt.prefix ? fmt.prefix : "";

    char s[1024];
    sprintf(s, numeric_fmt, 0u);
    int numeric_width = strlen(s);

    if (fmt.multiline)
        fputs(prefix, f);

    for (size_t i=0; i<n; i+=fmt.width) {
        /* Prefix and/or address */
        if (i>0) {
            fputc('\n', f);
            fputs(prefix, f);
        }
        fprintf(f, addr_fmt, base_addr+i);

        /* Numeric byte values */
        if (fmt.show_numeric) {
            for (size_t j=0; j<fmt.width; j++) {
                if (i+j<n) {
                    if (j>0 && 0 == j % fmt.colsize)
                        fputc(' ', f);
                    fprintf(f, numeric_fmt, data[i+j]);
                } else if (fmt.pad_numeric) {
                    if (j>0 && 0 == j % fmt.colsize)
                        fputc(' ', f);
                    fprintf(f, "%*s", numeric_width, "");
                }
            }
        }

        if (fmt.show_numeric && fmt.show_chars)
            fputs("  |", f);
        
        /* Character byte values */
        if (fmt.show_chars) {
            for (size_t j=0; j<fmt.width; j++) {
                if (i+j>=n) {
                    if (fmt.pad_chars)
                        fputc(' ', f);
                } else if (isprint(data[i+j])) {
                    fputc(data[i+j], f);
                } else {
                    fputc('.', f);
                }
            }
            fputc('|', f);
        }
    }

    if (fmt.multiline)
        fputc('\n', f);
}

// DQ (11/8/2008): Alternative interface that works better for ROSE IR nodes
void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const std::string &prefix, const SgUnsignedCharList &data, 
                                   bool multiline)
{
    if (!data.empty()) {
        HexdumpFormat fmt;
        fmt.multiline = multiline;
        fmt.prefix = prefix.c_str();
        hexdump(f, base_addr, &(data[0]), data.size(), fmt);
    }
}

// DQ (8/31/2008): This is the newest interface function (could not remove the one based on SgUnsignedCharList since it
// is used in the symbol support).
void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const std::string &prefix, const SgFileContentList &data, 
                                   bool multiline)
{
    if (!data.empty()) {
        HexdumpFormat fmt;
        fmt.multiline = multiline;
        fmt.prefix = prefix.c_str();
        hexdump(f, base_addr, &(data[0]), data.size(), fmt);

    }
}

/** Writes a new file from the IR node for a parse executable file. Warning: This function might modify the AST by calling
 *  reallocate(), which makes sure all parts of the AST are consistent with respect to each other. */
void
SgAsmExecutableFileFormat::unparseBinaryFormat(const std::string &name, SgAsmFile *asmFile)
{
    std::ofstream f(name.c_str(), std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
    ROSE_ASSERT(f.is_open());
    f.exceptions(std::ios::badbit | std::ios::failbit);
    unparseBinaryFormat(f, asmFile);
    f.close();
}

/** Unparses an executable file into the supplied output stream. Warning: This function might modify the AST by calling
 *  reallocate(), which makes sure all parts of the AST are consistent with respect to each other. */
void
SgAsmExecutableFileFormat::unparseBinaryFormat(std::ostream &f, SgAsmFile *asmFile)
{
    ROSE_ASSERT(asmFile!=NULL);
    ROSE_ASSERT(asmFile->get_genericFile() != NULL);
    SgAsmGenericFile *ef = asmFile->get_genericFile();
    ROSE_ASSERT(ef);

    if (checkIsModifiedFlag(ef))
        ef->reallocate();

    ef->unparse(f);

    /* Extend the file to the full size. The unparser will not write zero bytes at the end of a file because some files
     * actually use the fact that sections that extend past the EOF will be zero padded.  For the time being we'll extend the
     * file to its full size. */
    if (!ef->get_truncate_zeros())
        ef->extend_to_eof(f);
}

/* Top-level binary executable file parser. Given the name of a file, open the file, detect the format, parse the file,
 * and return information about the file. */
void
SgAsmExecutableFileFormat::parseBinaryFormat(const std::string &name, SgAsmFile *asmFile)
{
    asmFile->set_name(name);
    SgAsmGenericFile *ef = parseBinaryFormat(name.c_str());
    ROSE_ASSERT(ef != NULL);
    ef->set_parent(asmFile);
    asmFile->set_genericFile(ef);
}

SgAsmGenericFile *
SgAsmExecutableFileFormat::parseBinaryFormat(const char *name)
{
    SgAsmGenericFile *ef = (new SgAsmGenericFile())->parse(name);
    ROSE_ASSERT(ef != NULL);

    if (SgAsmElfFileHeader::is_ELF(ef)) {
        (new SgAsmElfFileHeader(ef))->parse();
    } else if (SgAsmDOSFileHeader::is_DOS(ef)) {
        SgAsmDOSFileHeader *dos_hdr = new SgAsmDOSFileHeader(ef);
        dos_hdr->parse(false); /*delay parsing the DOS Real Mode Section*/

        /* DOS Files can be overloaded to also be PE, NE, LE, or LX. Such files have an Extended DOS Header immediately after
         * the DOS File Header (various forms of Extended DOS Header exist). The Extended DOS Header contains a file offset to
         * a PE, NE, LE, or LX File Header, the first bytes of which are a magic number. The is_* methods check for this magic
         * number. */
        SgAsmGenericHeader *big_hdr = NULL;
        if (SgAsmPEFileHeader::is_PE(ef)) {
            SgAsmDOSExtendedHeader *dos2_hdr = new SgAsmDOSExtendedHeader(dos_hdr);
            dos2_hdr->parse();
            SgAsmPEFileHeader *pe_hdr = new SgAsmPEFileHeader(ef);
            pe_hdr->set_offset(dos2_hdr->get_e_lfanew());
            pe_hdr->parse();
            big_hdr = pe_hdr;
        } else if (SgAsmNEFileHeader::is_NE(ef)) {
            SgAsmNEFileHeader::parse(dos_hdr);
        } else if (SgAsmLEFileHeader::is_LE(ef)) { /*or LX*/
            SgAsmLEFileHeader::parse(dos_hdr);
        }

#if 0 /*This iterferes with disassembling the DOS interpretation*/
        /* Now go back and add the DOS Real-Mode section but rather than using the size specified in the DOS header, constrain
         * it to not extend beyond the beginning of the PE, NE, LE, or LX file header. This makes detecting holes in the PE
         * format much easier. */
        dos_hdr->add_rm_section(big_hdr ? big_hdr->get_offset() : 0);
#else
        dos_hdr->add_rm_section(0);
#endif

    } else {
        delete ef; ef=NULL;
        /* Use file(1) to try to figure out the file type to report in the exception */
        int child_stdout[2];
        pipe(child_stdout);
        pid_t pid = fork();
        if (0==pid) {
            close(0);
            dup2(child_stdout[1], 1);
            close(child_stdout[0]);
            close(child_stdout[1]);
            execlp("file", "file", "-b", name, NULL);
            exit(1);
        } else if (pid>0) {
            char buf[4096];
            memset(buf, 0, sizeof buf);
            read(child_stdout[0], buf, sizeof buf);
            buf[sizeof(buf)-1] = '\0';
            if (char *nl = strchr(buf, '\n')) *nl = '\0'; /*keep only first line w/o LF*/
            waitpid(pid, NULL, 0);
            char mesg[64+sizeof buf];
            sprintf(mesg, "unrecognized file format: %s", buf);
            throw FormatError(mesg);
        } else {
            throw FormatError("unrecognized file format");
        }
    }

    ef->set_tracking_references(false); /*all done parsing*/

    /* Is the file large enough to hold all sections?  If any section extends past the EOF then set truncate_zeros, which will
     * cause the unparser to not write zero bytes to the end of the file. */
    ef->set_truncate_zeros(ef->get_current_size()>ef->get_orig_size());

    return ef;
}
