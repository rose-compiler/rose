/* Copyright 2008 Lawrence Livermore National Security, LLC */

/* This is for temporary debugging only; will be removed shortly! (RPM 2008-09-04) */

// DQ (9/9/2008): Robb, I had to comment this out to make thinks work.
// I am unclear if that was expected or not.  We can discuss this
// in the morning (or anytime you want me to call you).
// #define USE_ELF_STRING

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#if 0 /* FIXME: Move these IR nodes into ROSETTA. They're here only to expedite development. (RPM 2008-08-25) */

/* An SgAsmGenericString represents a string as stored in some section/segment/object/part of the executable file. Using this
 * class for such strings (as opposed to just storing the std::string), allows us to modify the string value and thereby cause
 * the unparser to output the new value.
 *
 * In general, making even a very minor one-letter change to a string can cascade into lots of changes in the unparsed
 * executable. For instance, changing the symbol "main" to "pain", even though it's a single letter, could cause the string to
 * be reallocated in the string table (e.g., if the string "domain" shares the same storage), which could cause the string
 * table to grow, which could rearrange other file sections, which could change the way the loader needs to map segments,
 * which changes the section and segment tables, etc....
 * 
 * This class is intended to help make those changes more transparent. There are versions specialized for Elf (strings are
 * NUL-teraminted), PE (strings are run length encoded), etc.
 * 
 * Example usage in ELF:
 * An ELF symbol table points to an Elf String Table. Symbol entries contain offsets into the string table for their names. A
 * new string is constructed like this:
 * 
 *     symbol_entry[0].name = SgAsmElfString(string_table, offset);    // "domain"
 *     symbol_table[1].name = SgAsmElfString(string_table, offset+2);  // "main", overlaps with "domain"
 *     symbol_table[2].name = SgAsmElfString(string_table, offset);    // another symbol named "domain"
 *
 * When the SgAsmElfString constructor is called more than once with the same offset (as in entries 0 and 2 above) then the
 * two strings returned are independent of each other: freeing or modifying one string does not affect the other. On the other
 * hand, copying one string object to another (SgAsmElfString second=symbol_entry[0].name) will cause both to reference the
 * same storage so that changing one changes the other.
 *
 * The name is available with the "get_string" method. The offset within the string table is also available for ELF strings.
 * 
 *     cerr <<symbol_entry[0].name.to_string();
 *     printf("offset=%"PRIu64", name=\"%s\"\n", symbol_entry[0].name.get_offset(), symbol_entry[0].name.get_string().c_str());
 *
 * To change "main" to "pain" one just makes an assignment:
 *
 *     symbol_table[1].name = "pain";
 *     symbol_table[1].name.set_string("pain"); // an alternative method of assignment
 *
 * Name objects should not be modified or freed from a particular string table until all strings in that table have been
 * parsed (the code will detect when a string is parsed after the table is modified). Also, names will not be reallocated in
 * the symbol table until get_offset() is called for one of the table's modified strings.  This usually results in more
 * efficient repacking of the string table.
 * 
 * Regions of the string table that are never referenced are maintained as "holes" available through the usual
 * SgAsmGenericSection interface. The reallocation algorithm keeps all holes at their original offsets relative to the
 * beginning of the string table.
 *
 * The SgAsmBasicString is derived from SgAsmGenericString to be able to act almost like a plain old std::string for the case
 * when there is no underlying string table.
 */
class SgAsmGenericString {
  public:
    virtual ~SgAsmGenericString() {};
    virtual const std::string& get_string() const = 0;                  /*no non-const version; use only set_string() to modify!*/
    virtual void set_string(const std::string &s) = 0;                  /*assignment calls this*/
    virtual void dump(FILE*, const char *prefix, ssize_t idx) = 0;

    /* Assignment (plus the default assignment) */
    SgAsmGenericString& operator=(const std::string &s);                /*short for set_string()*/
    SgAsmGenericString& operator=(const char *s);                       /*short for set_string()*/
};


class SgAsmBasicString : public SgAsmGenericString {
  public:
    SgAsmBasicString() {}
    virtual ~SgAsmBasicString() {}
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors */
    virtual const std::string& get_string() const                       /*no non-const version; use only set_string() to modify!*/
        {return p_string;}
    virtual void set_string(const std::string &s)
        {p_string = s;}

  private:
    std::string p_string;
};

class SgAsmElfString : public SgAsmGenericString {
  public:
    SgAsmElfString(class SgAsmElfStrtab *strtab, rose_addr_t offset)    /*string in string table*/
        {ctor(strtab, offset, false);}
    SgAsmElfString(class SgAsmElfStrtab *strtab, const std::string &s)  /*new string in string table*/
        {ctor(strtab, s);}
    explicit SgAsmElfString(class SgAsmElfStringStorage *storage)       /*string shares other storage*/
        {ctor(storage);}
    explicit SgAsmElfString(const char *s)                              /*non-storage constructor*/
        {ctor(std::string(s));}
    explicit SgAsmElfString(const std::string &s)                       /*non-storage constructor*/
        {ctor(s);}
    virtual ~SgAsmElfString();
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    static const rose_addr_t unallocated = ~(rose_addr_t)0;

    /* Accessors */
    virtual const std::string& get_string() const;                      /*no non-const version; use only set_string() to modify!*/
    virtual void set_string(const std::string&);                        /*assignment calls this*/
    rose_addr_t get_offset() const;                                     /*offset is read-only--no set_offset(); triggers realloc*/
    class SgAsmElfStringStorage* get_storage() const {                  /*read only; managed by constructor/destructor*/
        return p_storage;
    }

  private:
    SgAsmElfString() {abort();}
    void ctor(class SgAsmElfStrtab*, rose_addr_t offset, bool shared);
    void ctor(class SgAsmElfStrtab*, const std::string&);
    void ctor(class SgAsmElfStringStorage*);
    void ctor(const std::string &s);
    class SgAsmElfStringStorage *p_storage;                             /*child of the string table in the AST*/
};

class SgAsmElfStrtab : public SgAsmElfSection {
  public:
    SgAsmElfStrtab(SgAsmElfFileHeader *fhdr, SgAsmElfSectionTableEntry *shdr)
        : SgAsmElfSection(fhdr, shdr), num_freed(0), empty_string(0)
        {ctor(fhdr, shdr);}
    virtual ~SgAsmElfStrtab();
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    class SgAsmElfStringStorage *create_storage(addr_t offset, bool shared);
    SgAsmElfString *create_string(addr_t offset, bool shared);
    void free(class SgAsmElfStringStorage*);
    void free_all_strings(bool blow_away_holes=false);
    void reallocate(); /*allocate storage for all unallocated strings*/
    virtual void set_size(addr_t);
  private:
    void ctor(SgAsmElfFileHeader*, SgAsmElfSectionTableEntry*);
    void free(addr_t offset, addr_t size); /*mark part of table as free*/
    rose_addr_t best_fit(addr_t need); /*allocate from free list*/
    typedef std::vector<class SgAsmElfStringStorage*> referenced_t;
    referenced_t referenced;

    /* The following items are implementation details and probably not useful in the AST */
    typedef std::map<addr_t, addr_t> freelist_t; /*key is offset; value is size*/
    freelist_t freelist;
    size_t num_freed;
    class SgAsmElfStringStorage *empty_string; /*ptr to storage for empty string at offset zero if present*/
};

/* String storage class for SgAsmElfString.  The SgAsmElfString objects point to SgAsmElfStringStorage objects which are in turn
 * stored in the SgAsmElfSection. We do it this way so that all copies of the string (by assignment) still point to their
 * original location in the string table and we can reallocate all of them when necessary. */
class SgAsmElfStringStorage {
  public:
    SgAsmElfStringStorage(SgAsmElfStrtab *strtab, const std::string &string, rose_addr_t offset)
        : p_strtab(strtab), p_string(string), p_offset(offset) {}
    void dump(FILE *s, const char *prefix, ssize_t idx);

    /* Accessors. The set_* accessors are private because we don't want anyone messing with them. These data members are used
     * to control string allocation in ELF string tables and must only be modified by allocators in closely related classes.
     * For instance, to change the value of the string one should call SgAsmGenericString::set_string() instead. */
  public:
    SgAsmElfStrtab* get_strtab() const {
        return p_strtab;
    }
    const std::string& get_string() const {             /*read-only; set string with SgAsmGenericString::set_string()*/
        return p_string;
    }
    const rose_addr_t get_offset() const {              /*read-only; set only by the string table allocator*/
        return p_offset;                                /*does not trigger reallocation; cf. SgAsmElfString::get_offset()*/
    }
  private:
    friend class SgAsmElfString;                        /*allowed to set private data members*/
    friend class SgAsmElfStrtab;                        /*allowed to set private data members*/
    void set_string(const std::string &s) {
        p_string = s;
    }
    void set_strtab(SgAsmElfStrtab *strtab) {
        p_strtab = strtab;
    }
    void set_offset(rose_addr_t offset) {
        p_offset = offset;
    }
    
  private:
    SgAsmElfStringStorage() {abort();}                  /*no default constructor; cf. SgAsmElfString::create_storage()*/
    SgAsmElfStrtab *p_strtab;
    std::string p_string;
    rose_addr_t p_offset;
};
#endif /*END OF STUFF TO MOVE INTO ROSETTA*/
    

/* Truncate an address, ADDR, to be a multiple of the alignment, ALMNT, where ALMNT is a power of two and of the same
 * unsigned datatype as the address. */
#define ALIGN(ADDR,ALMNT) ((ADDR) & ~((ALMNT)-1))



// namespace Exec {
// namespace ELF {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File headers
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor reads and decodes the ELF header, whether it's 32- or 64-bit.  The 'offset' argument is normally zero since
 * ELF headers are at the beginning of the file. As mentioned in the header file, the section size is initialized as if we had
 * 32-bit words and if necessary we extend the section for 64-bit words herein. */
void
SgAsmElfFileHeader::ctor(SgAsmGenericFile *f, addr_t offset)
{
    set_name("ELF File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    ROSE_ASSERT(f != NULL);
 // printf ("In SgAsmElfFileHeader::ctor(): SgAsmGenericFile *f = %p addr_t offset = %zu \n",f,offset);

 // DQ (8/16/2008): Added code to set SgAsmPEFileHeader as parent of input SgAsmGenericFile
    f->set_parent(this);

    /* Read 32-bit header for now. Might need to re-read as 64-bit later. */
    ROSE_ASSERT(0 == get_size());
    Elf32FileHeader_disk disk32;
    extend_up_to(sizeof(disk32));
    content(0, sizeof(disk32), &disk32);

    ROSE_ASSERT(p_exec_format != NULL);

    /* Check magic number early */
    if (disk32.e_ident_magic[0]!=0x7f || disk32.e_ident_magic[1]!='E' ||
        disk32.e_ident_magic[2]!='L'  || disk32.e_ident_magic[3]!='F')
        throw FormatError("Bad ELF magic number");

    /* File byte order should be 1 or 2. However, we've seen at least one example that left the byte order at zero, implying
     * that it was the native order. We don't have the luxury of decoding the file on the native machine, so in that case we
     * try to infer the byte order by looking at one of the other multi-byte fields of the file. */
    ByteOrder sex;
    if (1 == disk32.e_ident_data_encoding) {
        sex = ORDER_LSB;
    } else if (2==disk32.e_ident_data_encoding) {
        sex = ORDER_MSB;
    } else if ((disk32.e_type & 0xff00)==0xff00) {
        /* One of the 0xffxx processor-specific flags in native order */
        if ((disk32.e_type & 0x00ff)==0xff)
            throw FormatError("invalid ELF header byte order"); /*ambiguous*/
        sex = host_order();
    } else if ((disk32.e_type & 0x00ff)==0x00ff) {
        /* One of the 0xffxx processor specific orders in reverse native order */
        sex = host_order()==ORDER_LSB ? ORDER_MSB : ORDER_LSB;
    } else if ((disk32.e_type & 0xff00)==0) {
        /* One of the low-valued file types in native order */
        if ((disk32.e_type & 0x00ff)==0)
            throw FormatError("invalid ELF header byte order"); /*ambiguous*/
        sex = host_order();
    } else if ((disk32.e_type & 0x00ff)==0) {
        /* One of the low-valued file types in reverse native order */
        sex = host_order() == ORDER_LSB ? ORDER_MSB : ORDER_LSB;
    } else {
        /* Ambiguous order */
        throw FormatError("invalid ELF header byte order");
    }

    ROSE_ASSERT(p_exec_format != NULL);

 // printf ("disk32.e_ident_file_class = %u \n",disk32.e_ident_file_class);

    /* Decode header to native format */
    if (1 == disk32.e_ident_file_class) {
        p_exec_format->set_word_size(4);

     // printf ("p_e_ident_padding.size()       = %zu \n",p_e_ident_padding.size());
     // printf ("sizeof(disk32.e_ident_padding) = %zu \n",sizeof(disk32.e_ident_padding));

     // ROSE_ASSERT(sizeof(p_e_ident_padding) == sizeof(disk32.e_ident_padding));
     // memcpy(p_e_ident_padding, disk32.e_ident_padding, sizeof(p_e_ident_padding));
        for (int i = 0; i < 9; i++)
             p_e_ident_padding.push_back(disk32.e_ident_padding[i]);
        ROSE_ASSERT(p_e_ident_padding.size() == sizeof(disk32.e_ident_padding));

        p_e_ident_file_class    = disk_to_host(sex, disk32.e_ident_file_class);
        p_e_ident_data_encoding = disk_to_host(sex, disk32.e_ident_data_encoding);
        p_e_ident_file_version  = disk_to_host(sex, disk32.e_ident_file_version);
        p_e_type                = disk_to_host(sex, disk32.e_type);
        p_e_machine             = disk_to_host(sex, disk32.e_machine);
        p_e_version             = disk_to_host(sex, disk32.e_version);
        p_e_entry               = disk_to_host(sex, disk32.e_entry);
        p_e_phoff               = disk_to_host(sex, disk32.e_phoff);
        p_e_shoff               = disk_to_host(sex, disk32.e_shoff);
        p_e_flags               = disk_to_host(sex, disk32.e_flags);
        p_e_ehsize              = disk_to_host(sex, disk32.e_ehsize);
        p_e_phentsize           = disk_to_host(sex, disk32.e_phentsize);
        p_e_phnum               = disk_to_host(sex, disk32.e_phnum);
        p_e_shentsize           = disk_to_host(sex, disk32.e_shentsize);
        p_e_shnum               = disk_to_host(sex, disk32.e_shnum);
        p_e_shstrndx            = disk_to_host(sex, disk32.e_shstrndx);
    } else if (2 == disk32.e_ident_file_class) {
        /* We guessed wrong. This is a 64-bit header, not 32-bit. */
        p_exec_format->set_word_size(8);
        Elf64FileHeader_disk disk64;
        extend_up_to(sizeof(Elf64FileHeader_disk)-sizeof(Elf32FileHeader_disk));
        content(0, sizeof disk64, &disk64);
        for (int i = 0; i < 9; i++)
             p_e_ident_padding.push_back(disk64.e_ident_padding[i]);
        ROSE_ASSERT(p_e_ident_padding.size() == sizeof(disk64.e_ident_padding));

        p_e_ident_file_class    = disk_to_host(sex, disk64.e_ident_file_class);
        p_e_ident_data_encoding = disk_to_host(sex, disk64.e_ident_data_encoding);
        p_e_ident_file_version  = disk_to_host(sex, disk64.e_ident_file_version);
        p_e_type                = disk_to_host(sex, disk64.e_type);
        p_e_machine             = disk_to_host(sex, disk64.e_machine);
        p_e_version             = disk_to_host(sex, disk64.e_version);
        p_e_entry               = disk_to_host(sex, disk64.e_entry);
        p_e_phoff               = disk_to_host(sex, disk64.e_phoff);
        p_e_shoff               = disk_to_host(sex, disk64.e_shoff);
        p_e_flags               = disk_to_host(sex, disk64.e_flags);
        p_e_ehsize              = disk_to_host(sex, disk64.e_ehsize);
        p_e_phentsize           = disk_to_host(sex, disk64.e_phentsize);
        p_e_phnum               = disk_to_host(sex, disk64.e_phnum);
        p_e_shentsize           = disk_to_host(sex, disk64.e_shentsize);
        p_e_shnum               = disk_to_host(sex, disk64.e_shnum);
        p_e_shstrndx            = disk_to_host(sex, disk64.e_shstrndx);
    } else {
        throw FormatError("invalid ELF header file class");
    }
    
    /* Magic number. disk32 and disk64 have header bytes at same offset */
    for (size_t i=0; i<sizeof(disk32.e_ident_magic); i++)
        p_magic.push_back(disk32.e_ident_magic[i]);
    
    /* File format */
    p_exec_format->set_family(FAMILY_ELF);
    switch (p_e_type) {
      case 0:
        p_exec_format->set_purpose(PURPOSE_UNSPECIFIED);
        break;
      case 1:
      case 3:
        p_exec_format->set_purpose(PURPOSE_LIBRARY);
        break;
      case 2:
        p_exec_format->set_purpose(PURPOSE_EXECUTABLE);
        break;
      case 4:
        p_exec_format->set_purpose(PURPOSE_CORE_DUMP);
        break;
      default:
        if (p_e_type >= 0xff00 && p_e_type <= 0xffff) {
            p_exec_format->set_purpose(PURPOSE_PROC_SPECIFIC);
        } else {
            p_exec_format->set_purpose(PURPOSE_OTHER);
        }
        break;
    }
    p_exec_format->set_sex(sex);
    p_exec_format->set_version(p_e_version);
    p_exec_format->set_is_current_version( (1 == p_e_version) );
    p_exec_format->set_abi(ABI_UNSPECIFIED);                 /* ELF specifies a target architecture rather than an ABI */
    p_exec_format->set_abi_version(0);
    //exec_format.word_size = ...; /*set above*/

    /* Target architecture */
    switch (p_e_machine) {                                /* These come from the Portable Formats Specification v1.1 */
      case 0:
        set_isa(ISA_UNSPECIFIED);
        break;
      case 1:
        set_isa(ISA_ATT_WE_32100);
        break;
      case 2:
        set_isa(ISA_SPARC_Family);
        break;
      case 3:
        set_isa(ISA_IA32_386);
        break;
      case 4:
        set_isa(ISA_M68K_Family);
        break;
      case 5:
        set_isa(ISA_M88K_Family);
        break;
      case 7:
        set_isa(ISA_I860_Family);
        break;
      case 8:
        set_isa(ISA_MIPS_Family);
        break;
      case 40:
        set_isa(ISA_ARM_Family);
        break;
      case 62:
        set_isa(ISA_X8664_Family);
        break;
      default:
        /*FIXME: There's a whole lot more. See Dan's Elf reader. */
        set_isa(ISA_OTHER);
        break;
    }

    /* Target architecture */
    /*FIXME*/

    /* Entry point */
    p_base_va = 0;

 // printf ("p_e_entry = %zu \n",p_e_entry);

    add_entry_rva(p_e_entry);

 // printf ("Leaving SgAsmElfFileHeader::ctor() \n");
}

/* Maximum page size according to the ABI. This is used by the loader when calculating the program base address. Since parts
 * of the file are mapped into the process address space those parts must be aligned (both in the file and in memory) on the
 * largest possible page boundary so that any smaller page boundary will also work correctly. */
uint64_t
SgAsmElfFileHeader::max_page_size()
{
    /* FIXME:
     *    System V max page size is 4k.
     *    IA32 is 4k
     *    x86_64 is 2MB
     * Other systems may vary! */
    return 4*1024;
}

/* Encode Elf header disk structure */
void *
SgAsmElfFileHeader::encode(ByteOrder sex, Elf32FileHeader_disk *disk)
{
    ROSE_ASSERT(p_magic.size() == NELMTS(disk->e_ident_magic));
    for (size_t i=0; i<NELMTS(disk->e_ident_magic); i++)
        disk->e_ident_magic[i] = p_magic[i];
    host_to_disk(sex, p_e_ident_file_class,    &(disk->e_ident_file_class));
    host_to_disk(sex, p_e_ident_data_encoding, &(disk->e_ident_data_encoding));
    host_to_disk(sex, p_e_ident_file_version,  &(disk->e_ident_file_version));
    ROSE_ASSERT(p_e_ident_padding.size() == NELMTS(disk->e_ident_padding));
    for (size_t i=0; i<NELMTS(disk->e_ident_padding); i++)
        disk->e_ident_padding[i] = p_e_ident_padding[i];
    host_to_disk(sex, p_e_type,                &(disk->e_type));
    host_to_disk(sex, p_e_machine,             &(disk->e_machine));
    host_to_disk(sex, p_e_version,             &(disk->e_version));
    host_to_disk(sex, p_e_entry,               &(disk->e_entry));
    host_to_disk(sex, p_e_phoff,               &(disk->e_phoff));
    host_to_disk(sex, p_e_shoff,               &(disk->e_shoff));
    host_to_disk(sex, p_e_flags,               &(disk->e_flags));
    host_to_disk(sex, p_e_ehsize,              &(disk->e_ehsize));
    host_to_disk(sex, p_e_phentsize,           &(disk->e_phentsize));
    host_to_disk(sex, p_e_phnum,               &(disk->e_phnum));
    host_to_disk(sex, p_e_shentsize,           &(disk->e_shentsize));
    host_to_disk(sex, p_e_shnum,               &(disk->e_shnum));
    host_to_disk(sex, p_e_shstrndx,            &(disk->e_shstrndx));

    return disk;
}
void *
SgAsmElfFileHeader::encode(ByteOrder sex, Elf64FileHeader_disk *disk)
{
    ROSE_ASSERT(p_magic.size() == NELMTS(disk->e_ident_magic));
    for (size_t i=0; i < NELMTS(disk->e_ident_magic); i++)
        disk->e_ident_magic[i] = p_magic[i];
    host_to_disk(sex, p_e_ident_file_class,    &(disk->e_ident_file_class));
    host_to_disk(sex, p_e_ident_data_encoding, &(disk->e_ident_data_encoding));
    host_to_disk(sex, p_e_ident_file_version,  &(disk->e_ident_file_version));
    ROSE_ASSERT(p_e_ident_padding.size() == NELMTS(disk->e_ident_padding));
    for (size_t i=0; i<NELMTS(disk->e_ident_padding); i++)
        disk->e_ident_padding[i] = p_e_ident_padding[i];
    host_to_disk(sex, p_e_type,                &(disk->e_type));
    host_to_disk(sex, p_e_machine,             &(disk->e_machine));
    host_to_disk(sex, p_e_version,             &(disk->e_version));
    host_to_disk(sex, p_e_entry,               &(disk->e_entry));
    host_to_disk(sex, p_e_phoff,               &(disk->e_phoff));
    host_to_disk(sex, p_e_shoff,               &(disk->e_shoff));
    host_to_disk(sex, p_e_flags,               &(disk->e_flags));
    host_to_disk(sex, p_e_ehsize,              &(disk->e_ehsize));
    host_to_disk(sex, p_e_phentsize,           &(disk->e_phentsize));
    host_to_disk(sex, p_e_phnum,               &(disk->e_phnum));
    host_to_disk(sex, p_e_shentsize,           &(disk->e_shentsize));
    host_to_disk(sex, p_e_shnum,               &(disk->e_shnum));
    host_to_disk(sex, p_e_shstrndx,            &(disk->e_shstrndx));

    return disk;
}

/* Write ELF contents back to a file. */
void
SgAsmElfFileHeader::unparse(FILE *f)
{
    /* Encode ELF file header */
    Elf32FileHeader_disk disk32;
    Elf64FileHeader_disk disk64;
    void *disk = NULL;
    size_t struct_size = 0;
    if (4 == get_word_size()) {
        disk = encode(get_sex(), &disk32);
        struct_size = sizeof(disk32);
    } else if (8 == get_word_size()) {
        disk = encode(get_sex(), &disk64);
        struct_size = sizeof(disk64);
    } else {
        ROSE_ASSERT(!"unsupported word size");
    }

    /* Make sure there's room in the file. If not, anything beyond the file section should be zero. */
    write(f, p_offset, struct_size, disk);

    /* Write the ELF section and segment tables and, indirectly, the sections themselves. */
    if (p_section_table) {
        ROSE_ASSERT(p_section_table->get_header()==this);
        p_section_table->unparse(f);
    }

    if (p_segment_table) {
        ROSE_ASSERT(p_segment_table->get_header()==this);
        p_segment_table->unparse(f);
    }

    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfFileHeader.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_file_class",     p_e_ident_file_class);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_data_encoding",  p_e_ident_data_encoding);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_file_version",   p_e_ident_file_version);
    for (size_t i=0; i < p_e_ident_padding.size(); i++)
        fprintf(f, "%s%-*s = [%zu] %u\n",                   p, w, "e_ident_padding",     i, p_e_ident_padding[i]);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_type",                 p_e_type);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_machine",              p_e_machine);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_version",              p_e_version);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",                p, w, "e_entry",                p_e_entry);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes into file\n",    p, w, "e_phoff",                p_e_phoff);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes into file\n",    p, w, "e_shoff",                p_e_shoff);
    fprintf(f, "%s%-*s = 0x%08lx\n",                        p, w, "e_flags",                p_e_flags);
    fprintf(f, "%s%-*s = %lu bytes\n",                      p, w, "e_ehsize",               p_e_ehsize);
    fprintf(f, "%s%-*s = %lu bytes\n",                      p, w, "e_phentsize",            p_e_phentsize);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_phnum",                p_e_phnum);
    fprintf(f, "%s%-*s = %lu bytes\n",                      p, w, "e_shentsize",            p_e_shentsize);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_shnum",                p_e_shnum);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_shstrndx",             p_e_shstrndx);

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sections
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmElfSection::ctor(SgAsmElfSectionTableEntry *shdr)
{
    set_synthesized(false);

    /* Section purpose */
    switch (shdr->get_sh_type()) {
      case SgAsmElfSectionTableEntry::SHT_PROGBITS:
        set_purpose(SP_PROGRAM);
        break;
      case SgAsmElfSectionTableEntry::SHT_STRTAB:
        set_purpose(SP_HEADER);
        break;
      case SgAsmElfSectionTableEntry::SHT_DYNSYM:
      case SgAsmElfSectionTableEntry::SHT_SYMTAB:
        set_purpose(SP_SYMTAB);
        break;
      default:
        set_purpose(SP_OTHER);
        break;
    }

    /* Section mapping */
    if (shdr->get_sh_addr() > 0) {
        set_mapped_rva(shdr->get_sh_addr());
        set_mapped_size(shdr->get_sh_size());
    }
}

/* If the mapped address of a section changes make sure that the ELF header's entry point is updated also if it happens to
 * fall within this section. */
void
SgAsmElfSection::set_mapped_rva(addr_t new_rva)
{
    SgAsmGenericHeader *fhdr = get_header();
    if (fhdr) {
        for (SgAddressList::iterator i=fhdr->get_entry_rvas().begin(); i!=fhdr->get_entry_rvas().end(); ++i) {
            if (*i >= get_mapped_rva() && *i < get_mapped_rva()+get_mapped_size()) {
                *i = new_rva;
            }
        }
    }
    
    SgAsmGenericSection::set_mapped_rva(new_rva);
}

/* Print some debugging info */
void
SgAsmElfSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSection.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    SgAsmGenericSection::dump(f, p, -1);
    p_st_entry->dump(f, p, -1);
    if (p_linked_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\" @%" PRIu64 ", %" PRIu64 " bytes\n", p, w, "linked_to",
                p_linked_section->get_id(), p_linked_section->get_name().c_str(),
                p_linked_section->get_offset(), p_linked_section->get_size());
    } else {
        fprintf(f, "%s%-*s = NULL\n",    p, w, "linked_to");
    }

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String tables and strings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
/*FIXME: move to ROSE_ExecGeneric.C */
SgAsmGenericString&
SgAsmGenericString::operator=(const std::string &s)
{
    set_string(s);
    return *this;
}
/*FIXME: move to ROSE_ExecGeneric.C */
SgAsmGenericString&
SgAsmGenericString::operator=(const char *s)
{
    set_string(s);
    return *this;
}
#endif


/*FIXME: move to ROSE_ExecGeneric.C */
/* Print some debugging info */
void
SgAsmBasicString::dump(FILE *f, const char *prefix, ssize_t idx)
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
    
/* String constructors */
void
SgAsmElfString::ctor(SgAsmElfStrtab *strtab, rose_addr_t offset, bool shared)
{
    p_storage = strtab->create_storage(offset, shared);
}
void
SgAsmElfString::ctor(SgAsmElfStrtab *strtab, const std::string &s)
{
    p_storage = strtab->create_storage(0, false);
    set_string(s);
}
    
void
SgAsmElfString::ctor(SgAsmElfStringStorage *storage)
{
    p_storage = storage;
}
void
SgAsmElfString::ctor(const std::string &s)
{
    p_storage = new SgAsmElfStringStorage(NULL, s, unallocated);
}

#if 0
// DQ (9/9/2008): Use the destructor built automatically by ROSETTA.
SgAsmElfString::~SgAsmElfString()
{
#if 0 /* FIXME: Strings may share storage, so we can't free it. (RPM 2008-09-03) */
    /* Free storage if it isn't associated with a string table. */
    if (p_storage && NULL==p_storage->strtab)
        delete p_storage;
#endif
    p_storage = NULL;
}
#endif

/* Returns the std::string associated with the SgAsmElfString */
const std::string&
SgAsmElfString::get_string() const 
{
    static const std::string empty;
    return get_storage() ? get_storage()->get_string() : empty;
}

/* Returns the offset into the string table where the string is allocated. If the string is not allocated then this call
 * triggers a reallocation. */
rose_addr_t
SgAsmElfString::get_offset() const
{
    if (NULL==get_storage())
        return unallocated;
    if (get_storage()->get_offset() == unallocated) {
        ROSE_ASSERT(get_storage()->get_strtab()!=NULL);
        get_storage()->get_strtab()->reallocate();
        ROSE_ASSERT(get_storage()->get_offset() != unallocated);
    }
    return get_storage()->get_offset();
}

/* Give the string a new value */
void
SgAsmElfString::set_string(const std::string &s)
{
    if (get_string()==s) return; /* no change in value */
    SgAsmElfStringStorage *storage = get_storage();
    ROSE_ASSERT(storage!=NULL); /* we don't even know which string table! */
    storage->get_strtab()->free(storage);
    storage->set_string(s);
}

/* Print some debugging info */
void
SgAsmElfString::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfString[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfString.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = 0x%08lx\n", p, w, "storage", (unsigned long)get_storage());
    if (get_storage())
        get_storage()->dump(f, p, -1);
}

/* Print some debugging info */
void
SgAsmElfStringStorage::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfStringStorage[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfStringStorage.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s =", p, w, "sec,offset,val");
    SgAsmElfStrtab *strtab = get_strtab();
    if (strtab) {
        fprintf(f, " section [%d] \"%s\"", strtab->get_id(), strtab->get_name().c_str());
    } else {
        fputs(" no section", f);
    }
    if (!strtab || get_offset()==SgAsmElfString::unallocated) {
        fputs(", not allocated", f);
    } else {
        fprintf(f, ", offset 0x%08"PRIx64" (%"PRIu64")", get_offset(), get_offset());
    }
    fprintf(f, ", \"%s\"\n", get_string().c_str());
}

/* Constructor */
void
SgAsmElfStrtab::ctor(SgAsmElfFileHeader*, SgAsmElfSectionTableEntry*)
{
    /* The first byte of an ELF String Table should always be NUL. We don't want the allocation functions to ever free this
     * byte, so we'll create a special storage item for it. */
    if (content(0, 1)[0]=='\0')
        p_empty_string = create_storage(0, false);
}

/* Free ElfStrStorage objects associated with this string table. It may not be safe to blow them away yet since other objects
 * may still have SgAsmElfStrings pointing to these storage objects. So instead, we will mark all this strtab's storage
 * objects as no longer being associated with a string table. This allows the SgAsmElfString objects to still function
 * properly and their destructors (~SgAsmElfString) will free their storage. */
SgAsmElfStrtab::~SgAsmElfStrtab()
{
    for (referenced_t::iterator i = p_referenced_storage.begin(); i != p_referenced_storage.end(); ++i) {
        SgAsmElfStringStorage *storage = *i;
        storage->set_strtab(NULL);
        storage->set_offset(SgAsmElfString::unallocated);
    }
    p_referenced_storage.clear();
    p_empty_string = NULL; /*FIXME: can't delete for same reason as in SgAsmElfString destructor. (RPM 2008-09-05) */
}

/* Creates the storage item for the string at the specified offset. If 'shared' is true then attempt to re-use a previous
 * storage object, otherwise always create a new one. Each storage object is considered a separate string, therefore when two
 * strings share the same storage object, changing one string changes the other. */
SgAsmElfStringStorage *
SgAsmElfStrtab::create_storage(addr_t offset, bool shared)
{
    ROSE_ASSERT(offset!=SgAsmElfString::unallocated);

    /* Has this string already been created? If so, return previous storage object. However, never share the empty_string at
     * offset zero created when this string table was constructed because the ELF spec says it needs to stay there whether
     * referenced or not. */
    if (shared) {
        for (referenced_t::iterator i=p_referenced_storage.begin(); i!=p_referenced_storage.end(); i++) {
            if ((*i)->get_offset()==offset && (*i) != p_empty_string)
                return *i;
        }
    }

    /* Create a new storage object at this offset. */
    const char *s = content_str(offset);
    SgAsmElfStringStorage *storage = new SgAsmElfStringStorage(this, s, offset);

    /* It's a bad idea to free (e.g., modify) strings before we've identified all the strings in the table. Consider
     * the case where offset 1 is "domain" and offset 3 is "main" (i.e., they overlap). If we modify "main" before knowing
     * about "domain" then we'll end up freeing the last part of "domain" (and possibly replacing it with something else)!
     *
     * The only time we can guarantee this is OK is when the new storage points to the same file location as "empty_string"
     * since the latter is guaranteed to never be freed or shared. This exception is used when creating a new, unallocated
     * string (see SgAsmElfString(SgAsmElfStrtab,const std::string&)). */
    if (p_num_freed>0 && (!p_empty_string || offset!=p_empty_string->get_offset())) {
        fprintf(stderr,
                "SgAsmElfStrtab::create_storage(%"PRIu64"): %zu other string%s (of %zu created) in [%d] \"%s\""
                " %s been modified and/or reallocated!\n",
                offset, p_num_freed, 1==p_num_freed?"":"s", p_referenced_storage.size(), get_id(), get_name().c_str(),
                1==p_num_freed?"has":"have");
        ROSE_ASSERT(0==p_num_freed);
    }
    
    p_referenced_storage.push_back(storage);
    return storage;
}

/* Constructs an SgAsmElfString from an offset into this string table. */
SgAsmElfString *
SgAsmElfStrtab::create_string(addr_t offset, bool shared)
{
    SgAsmElfStringStorage *storage = create_storage(offset, shared);
    return new SgAsmElfString(storage);
}

/* Allocates storage for NEED bytes from the free list using best-fit; returns no_id when unable. */
rose_addr_t
SgAsmElfStrtab::best_fit(addr_t need)
{
    /* Find best entry in the free list */
    freelist_t::iterator best = p_freelist.end();
    for (freelist_t::iterator i=p_freelist.begin(); i!=p_freelist.end(); ++i) {
        if (need==(*i).second) {
            /* Best possible!  */
            addr_t retval = i->first;
            p_freelist.erase(i);
            return retval;
        } else if (need <= i->second &&
                   (best==p_freelist.end() || i->second<best->second)) {
            best = i;
        }
    }

    /* Can we rearrange free space to make enough? We do this after the loop above because it's less intrusive. */
    if (best==p_freelist.end())
        return SgAsmElfString::unallocated; /*FIXME: not implemented yet*/
    
    /* Adjust free list */
    ROSE_ASSERT(best != p_freelist.end());
    ROSE_ASSERT(need < best->second);
    addr_t retval = best->first;
    p_freelist.insert(freelist_t::value_type(best->first+need, best->second-need));
    p_freelist.erase(best);
    return retval;
}

/* Free area of this string table that corresponds to the string currently stored. Use this in preference to the offset/size
 * version of free() when possible. */
void
SgAsmElfStrtab::free(SgAsmElfStringStorage *storage)
{
    ROSE_ASSERT(storage!=NULL);
    ROSE_ASSERT(storage!=p_empty_string);
    addr_t old_offset = storage->get_offset();
    if (old_offset!=SgAsmElfString::unallocated) {
        storage->set_offset(SgAsmElfString::unallocated);
        free(old_offset, storage->get_string().size()+1);
    }
}

/* Add a range of bytes to the free list. Coalesce adjacent free areas.  An ELF string table can have a pointer to the
 * beginning of a string, but may also have pointers into the middle of strings. For instance, a string table that stores
 * "bar" and "foobar" can be optimized to store them as "foobar\0" with "bar" at offset 3 and "foobar" at offset 0. So we have
 * to be careful when freeing one string so we don't inadvertently mark the other string as being free. We do that by scanning
 * the "referenced" list and not freeing anything that's still referenced. */
void
SgAsmElfStrtab::free(addr_t offset, addr_t size)
{
    ROSE_ASSERT(offset+size <= get_size());
    
    /* Make sure area is not already in free list. */
    for (freelist_t::const_iterator i=p_freelist.begin(); i!=p_freelist.end(); ++i) {
        ROSE_ASSERT(offset+size <= i->first ||       /*to-free area is entirely left of existing free item or*/
                    offset >= i->first + i->second); /*to-free area is entirely right of existing free item*/
    }

    /* Preserve anything that's still referenced. The caller should have assigned SgAsmGenericString::no_id to the "offset"
     * member of the string storage to indicate that it's memory in the string table is no longer in use. */
    for (size_t i=0; i<p_referenced_storage.size() && size>0; i++) {
        if (p_referenced_storage[i]->get_offset()==SgAsmElfString::unallocated) continue;
        if (p_referenced_storage[i]->get_offset() <= offset && p_referenced_storage[i]->get_offset()+p_referenced_storage[i]->get_string().size()+1 > offset) {
            /* We are freeing "bar" but something references the overlapping "foobar" (or even just "bar"). Do not free
             * anything. */
            ROSE_ASSERT(offset+size == p_referenced_storage[i]->get_offset()+p_referenced_storage[i]->get_string().size()+1);
            size = 0;
        }
        if (p_referenced_storage[i]->get_offset() > offset && p_referenced_storage[i]->get_offset() < offset+size) {
            /* We are freeing "foobar" but something references overlapping "bar". Free only up to "bar" */
            size = p_referenced_storage[i]->get_offset() - offset;
        }
    }

    /* Nothing to free! */
    if (0==size) return;
    p_num_freed++;

    /* Coalesce with neighbors */
    freelist_t::iterator right = p_freelist.end();
    freelist_t::iterator left  = p_freelist.end();
    for (freelist_t::iterator i=p_freelist.begin(); i!=p_freelist.end() && (right!=p_freelist.end() || left!=p_freelist.end()); ++i) {
        if (offset + size == i->first)
            right = i;
        if (offset == i->first + i->second)
            left = i;
    }
    if (left!=p_freelist.end() && right!=p_freelist.end()) {
        left->second += size + right->second;
        p_freelist.erase(right);
    } else if (left!=p_freelist.end()) {
        left->second += size;
    } else if (right!=p_freelist.end()) {
        p_freelist.insert(freelist_t::value_type(offset, right->second+size));
        p_freelist.erase(right);
    } else {
        p_freelist.insert(freelist_t::value_type(offset, size));
    }
}

/* Free all strings so they will be reallocated later. This is more efficient than calling free() for each storage object. If
 * blow_way_holes is true then any areas that are unreferenced in the string table will be marked as referenced and added to
 * the free list. */
void
SgAsmElfStrtab::free_all_strings(bool blow_away_holes)
{
    bool was_congealed = get_congealed();

    /* Mark all storage objects as being unallocated. Never free the empty_string at offset zero. */
    for (size_t i=0; i<p_referenced_storage.size(); i++) {
        if (p_referenced_storage[i]->get_offset()!=SgAsmElfString::unallocated && p_referenced_storage[i]!=p_empty_string) {
            p_num_freed++;
            p_referenced_storage[i]->set_offset(SgAsmElfString::unallocated);
        }
    }

    /* Mark holes as referenced */
    if (blow_away_holes) {
        uncongeal();
        content(0, get_size());
    }
    
    /* Get list of referenced extents (i.e., complement of holes). */
    const RefMap &refs = uncongeal();

    /* Prime the freelist_extent to point to the first freeable byte */
    ExtentPair free_extent(0, 0); /*begin, end addresses (NOT SIZE!)*/
    if (p_empty_string) {
        /* Do not free empty_string at offset zero */
        ROSE_ASSERT(p_empty_string->get_offset()==0);
        ROSE_ASSERT(p_empty_string->get_string()=="");
        free_extent = ExtentPair(1, 1);
    }
    
    /* Recompute free list to include entire section sans holes. */
    p_freelist.clear();
    for (RefMap::const_iterator i=refs.begin(); i!=refs.end(); ++i) {
        ExtentPair ref_extent = *i;
        ROSE_ASSERT(ref_extent.first <= ref_extent.second);
        ROSE_ASSERT(ref_extent.first >= free_extent.first || free_extent.first==1);

        if (ref_extent.first > free_extent.second) {
            /* Save old free area, start new free area. */
            if (free_extent.first<free_extent.second)
                p_freelist.insert(freelist_t::value_type(free_extent.first, free_extent.second-free_extent.first));
            free_extent = ref_extent;
        } else if (ref_extent.second > free_extent.second) {
            /* Referenced extent extends free extent */
            free_extent.second = ref_extent.second;
        }
    }
    if (free_extent.first<free_extent.second)
        p_freelist.insert(freelist_t::value_type(free_extent.first, free_extent.second-free_extent.first));
    
    /* Restore state */
    if (was_congealed)
        congeal();
}

/* Allocates storage for strings that have been modified but not allocated. We first try to fit unallocated strings into free
 * space. Any that are left will cause the string table to be extended. */
void
SgAsmElfStrtab::reallocate()
{
    addr_t extend_size = 0;                                     /* amount by which to extend string table */

    /* Get list of strings that need to be allocated and sort by descending size. */
    std::vector<size_t> map;
    for (size_t i=0; i<p_referenced_storage.size(); i++) {
        SgAsmElfStringStorage *storage = p_referenced_storage[i];
        if (storage->get_offset()==SgAsmElfString::unallocated) {
            map.push_back(i);
        }
    }
    for (size_t i=1; i<map.size(); i++) {
        for (size_t j=0; j<i; j++) {
            if (p_referenced_storage[map[j]]->get_string().size() < p_referenced_storage[map[i]]->get_string().size()) {
                size_t x = map[i];
                map[i] = map[j];
                map[j] = x;
            }
        }
    }

    /* Allocate from largest to smallest so we have the best chance of finding overlaps */
    for (size_t i=0; i<map.size(); i++) {
        SgAsmElfStringStorage *storage = p_referenced_storage[map[i]];
        ROSE_ASSERT(storage->get_offset()==SgAsmElfString::unallocated);

        /* Empty strings should point to the first byte of the file (without sharing empty_string) according to ELF spec. */
        if (storage->get_string()=="" && p_empty_string) {
            ROSE_ASSERT(p_empty_string->get_offset()==0);
            ROSE_ASSERT(p_empty_string->get_string()=="");
            storage->set_offset(0);
        }

#if 1 /*safe to comment this out to avoid sharing*/
        /* Is there an existing string that we can use? */
        if (storage->get_offset()==SgAsmElfString::unallocated) {
            for (size_t j=0; j<p_referenced_storage.size(); j++) {
                SgAsmElfStringStorage *previous = p_referenced_storage[j];
                size_t need = storage->get_string().size();
                size_t have = previous->get_string().size();
                if (previous->get_offset()!=SgAsmElfString::unallocated &&
                    need <= have && 0==previous->get_string().compare(have-need, need, storage->get_string())) {
                    storage->set_offset(previous->get_offset()+(have-need));
                    break;
                }
            }
        }
#endif
        
        /* If we couldn't share another string then try to allocate from free space (avoiding holes) */
        if (storage->get_offset()==SgAsmElfString::unallocated) {
            addr_t new_offset = best_fit(storage->get_string().size()+1);    /* +1 for NUL terminator */
            storage->set_offset(new_offset);
        }

        /* If no free space area large enough then prepare to extend the section. */
        if (storage->get_offset()==SgAsmElfString::unallocated) {
            extend_size += storage->get_string().size() + 1;
        }
    }
    
    /* Extend the string table if necessary and reallocate things that didn't get allocated in the previous loop. */
    if (extend_size>0) {
        fprintf(stderr, "SgAsmElfStrtab::reallocate(): need to extend [%d] \"%s\" by %zu byte%s\n", 
                get_id(), get_name().c_str(), extend_size, 1==extend_size?"":"s");
        static bool recursive=false;
        ROSE_ASSERT(!recursive);
        recursive = true;
        get_file()->resize(this, get_size()+extend_size);
        reallocate();
        recursive = false;
    }
}

/* Augments superclass to make sure free list and such are adjusted properly */
void
SgAsmElfStrtab::set_size(addr_t newsize)
{
    ROSE_ASSERT(newsize>=get_size()); /*can only enlarge for now*/
    addr_t orig_size = get_size();
    addr_t adjustment = newsize - orig_size;

    SgAsmElfSection::set_size(newsize);

    if (adjustment>0)
        freelist.insert(freelist_t::value_type(orig_size, adjustment));
}

/* Write string table back to disk. Free space is zeroed out; holes are left as they are. */
void
SgAsmElfStrtab::unparse(FILE *f)
{
    /*FIXME: What happens if the reallocation causes the string table to be resized at this point? (RPM 2008-09-03)*/
    reallocate();
    
    /* Write strings with NUL termination. Shared strings will be written more than once, but that's OK. */
    for (size_t i=0; i<p_referenced_storage.size(); i++) {
        SgAsmElfStringStorage *storage = p_referenced_storage[i];
        ROSE_ASSERT(storage->get_offset()!=SgAsmElfString::unallocated);
        addr_t at = write(f, storage->get_offset(), storage->get_string());
        write(f, at, '\0');
    }
    
    /* Fill free areas with zero */
    for (freelist_t::const_iterator i=p_freelist.begin(); i!=p_freelist.end(); ++i) {
        write(f, i->first, std::string(i->second, '\0'));
    }
    
    /* Write original data from holes */
    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfStrtab::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfStrtab[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfStrtab.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    SgAsmElfSection::dump(f, p, -1);

    fprintf(f, "%s%-*s = 0x%08lx", p, w, "empty_string", (unsigned long)p_empty_string);
    for (size_t i=0; i<p_referenced_storage.size(); ++i) {
        if (p_referenced_storage[i] == p_empty_string)
            fprintf(f, " p_referenced_storage[%zu]", i);
    }
    fputc('\n', f);
    
    fprintf(f, "%s%-*s = %zu strings\n", p, w, "referenced", p_referenced_storage.size());
    for (size_t i=0; i<p_referenced_storage.size(); i++) {
        p_referenced_storage[i]->dump(f, p, i);
    }

    fprintf(f, "%s%-*s = %zu free regions\n", p, w, "freelist", p_freelist.size());
    freelist_t::iterator flit = p_freelist.begin();
    for (size_t i=0; i<p_freelist.size(); ++i, ++flit) {
        addr_t offset = flit->first;
        addr_t size = flit->second;
        char label[64];
        sprintf(label, "freelist[%zu]", i);
        fprintf(f, "%s%-*s = %"PRIu64" bytes at offset rel 0x%08"PRIx64" (%"PRIu64"), abs 0x%08"PRIx64" (%"PRIu64")\n", 
                p, w, label, size, offset, offset, get_offset()+offset, get_offset()+offset);
    }
}   

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Section tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Converts 32-bit disk representation to host representation */
void
SgAsmElfSectionTableEntry::ctor(ByteOrder sex, const Elf32SectionTableEntry_disk *disk) 
{
    p_sh_name      = disk_to_host(sex, disk->sh_name);
    p_sh_type      = disk_to_host(sex, disk->sh_type);
    p_sh_flags     = disk_to_host(sex, disk->sh_flags);
    p_sh_addr      = disk_to_host(sex, disk->sh_addr);
    p_sh_offset    = disk_to_host(sex, disk->sh_offset);
    p_sh_size      = disk_to_host(sex, disk->sh_size);
    p_sh_link      = disk_to_host(sex, disk->sh_link);
    p_sh_info      = disk_to_host(sex, disk->sh_info);
    p_sh_addralign = disk_to_host(sex, disk->sh_addralign);
    p_sh_entsize   = disk_to_host(sex, disk->sh_entsize);
}
    
/* Converts 64-bit disk representation to host representation */
void
SgAsmElfSectionTableEntry::ctor(ByteOrder sex, const Elf64SectionTableEntry_disk *disk) 
{
    p_sh_name      = disk_to_host(sex, disk->sh_name);
    p_sh_type      = disk_to_host(sex, disk->sh_type);
    p_sh_flags     = disk_to_host(sex, disk->sh_flags);
    p_sh_addr      = disk_to_host(sex, disk->sh_addr);
    p_sh_offset    = disk_to_host(sex, disk->sh_offset);
    p_sh_size      = disk_to_host(sex, disk->sh_size);
    p_sh_link      = disk_to_host(sex, disk->sh_link);
    p_sh_info      = disk_to_host(sex, disk->sh_info);
    p_sh_addralign = disk_to_host(sex, disk->sh_addralign);
    p_sh_entsize   = disk_to_host(sex, disk->sh_entsize);
}

/* Encode a section table entry into the disk structure */
void *
SgAsmElfSectionTableEntry::encode(ByteOrder sex, Elf32SectionTableEntry_disk *disk)
{
    host_to_disk(sex, p_sh_name,      &(disk->sh_name));
    host_to_disk(sex, p_sh_type,      &(disk->sh_type));
    host_to_disk(sex, p_sh_flags,     &(disk->sh_flags));
    host_to_disk(sex, p_sh_addr,      &(disk->sh_addr));
    host_to_disk(sex, p_sh_offset,    &(disk->sh_offset));
    host_to_disk(sex, p_sh_size,      &(disk->sh_size));
    host_to_disk(sex, p_sh_link,      &(disk->sh_link));
    host_to_disk(sex, p_sh_info,      &(disk->sh_info));
    host_to_disk(sex, p_sh_addralign, &(disk->sh_addralign));
    host_to_disk(sex, p_sh_entsize,   &(disk->sh_entsize));

    return disk;
}
void *
SgAsmElfSectionTableEntry::encode(ByteOrder sex, Elf64SectionTableEntry_disk *disk)
{
    host_to_disk(sex, p_sh_name,      &(disk->sh_name));
    host_to_disk(sex, p_sh_type,      &(disk->sh_type));
    host_to_disk(sex, p_sh_flags,     &(disk->sh_flags));
    host_to_disk(sex, p_sh_addr,      &(disk->sh_addr));
    host_to_disk(sex, p_sh_offset,    &(disk->sh_offset));
    host_to_disk(sex, p_sh_size,      &(disk->sh_size));
    host_to_disk(sex, p_sh_link,      &(disk->sh_link));
    host_to_disk(sex, p_sh_info,      &(disk->sh_info));
    host_to_disk(sex, p_sh_addralign, &(disk->sh_addralign));
    host_to_disk(sex, p_sh_entsize,   &(disk->sh_entsize));

    return disk;
}

/* Constructor reads the Elf Section Table (i.e., array of section headers) */
void
SgAsmElfSectionTable::ctor()
{
    set_synthesized(true);                              /* the section table isn't really a section itself */
    set_name("ELF section table");
    set_purpose(SP_HEADER);

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();

    if (fhdr->get_e_shnum() > 0) {

        /* Check sizes */
        ROSE_ASSERT(4 == fhdr->get_word_size() || 8 == fhdr->get_word_size());
        size_t struct_size = 4 == fhdr->get_word_size() ? sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk) : 
                                                          sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk);
        if (fhdr->get_e_shentsize() < struct_size)
            throw FormatError("ELF header shentsize is too small");

        /* Read all the section headers. We can't just cast this to an array like with other structs because
         * the ELF header specifies the size of each entry. */
        std::vector<SgAsmElfSectionTableEntry*> entries;
        addr_t offset = 0;

     // This is an abby normally complex loop increment expression
        for (size_t i = 0; i < fhdr->get_e_shnum(); i++, offset += fhdr->get_e_shentsize()) {
            SgAsmElfSectionTableEntry *shdr = NULL;
            if (4 == fhdr->get_word_size()) {
                const SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk *disk =
                    (const SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk*)content(offset, fhdr->get_e_shentsize());
                shdr = new SgAsmElfSectionTableEntry(sex, disk);
            } else {
                const SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk *disk =
                    (const SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk*)content(offset, fhdr->get_e_shentsize());
                shdr = new SgAsmElfSectionTableEntry(sex, disk);
            }
            shdr->set_nextra(fhdr->get_e_shentsize() - struct_size);
            if (shdr->get_nextra() > 0)
                shdr->get_extra() = content_ucl(offset+struct_size, shdr->get_nextra());
            entries.push_back(shdr);
        }

        /* Read the string table section first because we'll need this to initialize section names. */
        SgAsmElfStrtab *strtab = NULL;
        if (fhdr->get_e_shstrndx() > 0) {
            SgAsmElfSectionTableEntry *shdr = entries[fhdr->get_e_shstrndx()];
            strtab = new SgAsmElfStrtab(fhdr, shdr);
            strtab->set_id(fhdr->get_e_shstrndx());
            strtab->set_st_entry(shdr);
#ifdef USE_ELF_STRING /*FIXME*/
            SgAsmElfString name(strtab, shdr->get_sh_name());
            strtab->set_name(name.get_string());
#else
            strtab->set_name(strtab->content_str(shdr->get_sh_name()));
#endif
        }

        /* Read all other sections */
        for (size_t i = 0; i<entries.size(); i++) {
            SgAsmElfSectionTableEntry *shdr = entries[i];
            SgAsmElfSection *section = NULL;
            if (i == fhdr->get_e_shstrndx()) continue; /*we already read string table*/
            switch (shdr->get_sh_type()) {
              case SgAsmElfSectionTableEntry::SHT_NULL:
                /* Null entry. We still create the section just to hold the section header. */
                section = new SgAsmElfSection(fhdr, shdr, 0);
                break;
              case SgAsmElfSectionTableEntry::SHT_NOBITS:
                /* These types of sections don't occupy any file space (e.g., BSS) */
                section = new SgAsmElfSection(fhdr, shdr, 0);
                break;
              case SgAsmElfSectionTableEntry::SHT_DYNAMIC:
                section = new SgAsmElfDynamicSection(fhdr, shdr);
                break;
              case SgAsmElfSectionTableEntry::SHT_DYNSYM:
              case SgAsmElfSectionTableEntry::SHT_SYMTAB:
                section = new SgAsmElfSymbolSection(fhdr, shdr);
                break;
              case SgAsmElfSectionTableEntry::SHT_STRTAB:
                section = new SgAsmElfStrtab(fhdr, shdr);
                break;
              default:
                section = new SgAsmElfSection(fhdr, shdr);
                break;
            }
            section->set_id(i);
            section->set_st_entry(shdr);
            if (strtab) {
#ifdef USE_ELF_STRING /*FIXME*/
                SgAsmElfString name(strtab, shdr->get_sh_name());
                section->set_name(name.get_string());
#else
                section->set_name(strtab->content_str(shdr->get_sh_name()));
#endif
            }
            section->set_mapped_wperm((shdr->get_sh_flags() & 0x01) == 0x01);
            section->set_mapped_xperm((shdr->get_sh_flags() & 0x04) == 0x04);

            shdr->set_parent(section);
        }

        /* Initialize links between sections */
        for (size_t i = 0; i < entries.size(); i++) {
            SgAsmElfSectionTableEntry *shdr = entries[i];
            if (shdr->get_sh_link() > 0) {
                SgAsmElfSection *source = dynamic_cast<SgAsmElfSection*>(fhdr->get_file()->get_section_by_id(i));
                SgAsmElfSection *target = dynamic_cast<SgAsmElfSection*>(fhdr->get_file()->get_section_by_id(shdr->get_sh_link()));
                source->set_linked_section(target);
            }
        }
    }
}

/* Print some debugging info */
void
SgAsmElfSectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u bytes into strtab\n",          p, w, "sh_name",        p_sh_name);
    fprintf(f, "%s%-*s = %lu\n",                           p, w, "sh_type",        p_sh_type);
    fprintf(f, "%s%-*s = %lu\n",                           p, w, "sh_link",        p_sh_link);
    fprintf(f, "%s%-*s = %lu\n",                           p, w, "sh_info",        p_sh_info);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",               p, w, "sh_flags",       p_sh_flags);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",               p, w, "sh_addr",        p_sh_addr);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes into file\n",   p, w, "sh_offset",      p_sh_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",             p, w, "sh_size",        p_sh_size);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",                   p, w, "sh_addralign",   p_sh_addralign);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",             p, w, "sh_entsize",     p_sh_entsize);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",             p, w, "nextra",         p_nextra);
    if (p_nextra > 0)
        fprintf(f, "%s%-*s = %s\n",                        p, w, "extra",          "<FIXME>");
}

/* Write the section table section back to disk */
void
SgAsmElfSectionTable::unparse(FILE *f)
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();

    /* Write the remaining entries */
    for (size_t i = 0; i < sections.size(); i++) {
        if (sections[i]->get_id() >= 0) {
            SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
            SgAsmElfSectionTableEntry *shdr = section->get_st_entry();
            SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk disk32;
            SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk disk64;
            void *disk  = NULL;
            size_t size = 0;

            if (4==fhdr->get_word_size()) {
                disk = shdr->encode(sex, &disk32);
                size = sizeof disk32;
            } else if (8==fhdr->get_word_size()) {
                disk = shdr->encode(sex, &disk64);
                size = sizeof disk64;
            } else {
                ROSE_ASSERT(!"invalid word size");
            }

            /* The disk struct */
            addr_t extra_offset = write(f, section->get_id() * fhdr->get_e_shentsize(), size, disk);
            if (shdr->get_nextra() > 0)
                write(f, extra_offset, shdr->get_extra());

            /* The section itself */
            sections[i]->unparse(f);
        }
    }

    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfSectionTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSectionTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}
    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Segment tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Converts 32-bit disk representation to host representation */
void
SgAsmElfSegmentTableEntry::ctor(ByteOrder sex, const struct Elf32SegmentTableEntry_disk *disk) 
{
    p_type      = (SegmentType)disk_to_host(sex, disk->p_type);
    p_offset    = disk_to_host(sex, disk->p_offset);
    p_vaddr     = disk_to_host(sex, disk->p_vaddr);
    p_paddr     = disk_to_host(sex, disk->p_paddr);
    p_filesz    = disk_to_host(sex, disk->p_filesz);
    p_memsz     = disk_to_host(sex, disk->p_memsz);
    p_flags     = (SegmentFlags)disk_to_host(sex, disk->p_flags);
    p_align     = disk_to_host(sex, disk->p_align);
}

/* Converts 64-bit disk representation to host representation */
void
SgAsmElfSegmentTableEntry::ctor(ByteOrder sex, const Elf64SegmentTableEntry_disk *disk) 
{
    p_type      = (SegmentType)disk_to_host(sex, disk->p_type);
    p_offset    = disk_to_host(sex, disk->p_offset);
    p_vaddr     = disk_to_host(sex, disk->p_vaddr);
    p_paddr     = disk_to_host(sex, disk->p_paddr);
    p_filesz    = disk_to_host(sex, disk->p_filesz);
    p_memsz     = disk_to_host(sex, disk->p_memsz);
    p_flags     = (SegmentFlags)disk_to_host(sex, disk->p_flags);
    p_align     = disk_to_host(sex, disk->p_align);
}

/* Converts segment table entry back into disk structure */
void *
SgAsmElfSegmentTableEntry::encode(ByteOrder sex, Elf32SegmentTableEntry_disk *disk)
{
    host_to_disk(sex, p_type, &(disk->p_type));
    host_to_disk(sex, p_offset, &(disk->p_offset));
    host_to_disk(sex, p_vaddr, &(disk->p_vaddr));
    host_to_disk(sex, p_paddr, &(disk->p_paddr));
    host_to_disk(sex, p_filesz, &(disk->p_filesz));
    host_to_disk(sex, p_memsz, &(disk->p_memsz));
    host_to_disk(sex, p_flags, &(disk->p_flags));
    host_to_disk(sex, p_align, &(disk->p_align));
    return disk;
}
void *
SgAsmElfSegmentTableEntry::encode(ByteOrder sex, Elf64SegmentTableEntry_disk *disk)
{
    host_to_disk(sex, p_type, &(disk->p_type));
    host_to_disk(sex, p_offset, &(disk->p_offset));
    host_to_disk(sex, p_vaddr, &(disk->p_vaddr));
    host_to_disk(sex, p_paddr, &(disk->p_paddr));
    host_to_disk(sex, p_filesz, &(disk->p_filesz));
    host_to_disk(sex, p_memsz, &(disk->p_memsz));
    host_to_disk(sex, p_flags, &(disk->p_flags));
    host_to_disk(sex, p_align, &(disk->p_align));
    return disk;
}


/* Print some debugging info */
void
SgAsmElfSegmentTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSegmentTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSegmentTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

 // DQ (8/25/2008): Output type in hex because some enum values are set to hex values.
#if 0
 // fprintf(f, "%s%-*s = %u\n",                              p, w, "p_type",         p_type);
    fprintf(f, "%s%-*s = 0x%08x\n",                          p, w, "p_type",         p_type);
#else
    fprintf(f, "%s%-*s = 0x%08x = %s\n", p, w, "p_type",  p_type,  stringifyType(p_type).c_str());
#endif
    fprintf(f, "%s%-*s = 0x%08x ",                           p, w, "p_flags",        p_flags);
    fputc(p_flags & PF_RPERM ? 'r' : '-', f);
    fputc(p_flags & PF_WPERM ? 'w' : '-', f);
    fputc(p_flags & PF_XPERM ? 'x' : '-', f);
    if (p_flags & PF_PROC_MASK) fputs(" proc", f);
    if (p_flags & PF_RESERVED) fputs(" *", f);
    fputc('\n', f);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " bytes into file\n", p, w, "p_offset",       p_offset);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",                 p, w, "p_vaddr",        p_vaddr);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",                 p, w, "p_paddr",        p_paddr);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",               p, w, "p_filesz",       p_filesz);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",               p, w, "p_memsz",        p_memsz);
    fprintf(f, "%s%-*s = %" PRIu64 " byte boundary\n",       p, w, "p_align",        p_align);
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

// DQ (26/2008): Support output of named enum values
std::string
SgAsmElfSegmentTableEntry::stringifyType ( SgAsmElfSegmentTableEntry::SegmentType kind ) const
   {
     std::string s;

     switch (kind)
        {
          case SgAsmElfSegmentTableEntry::PT_NULL:    s = "PT_NULL";    break;
          case SgAsmElfSegmentTableEntry::PT_LOAD:    s = "PT_LOAD";    break;
          case SgAsmElfSegmentTableEntry::PT_DYNAMIC: s = "PT_DYNAMIC"; break;
          case SgAsmElfSegmentTableEntry::PT_INTERP:  s = "PT_INTERP";  break;
          case SgAsmElfSegmentTableEntry::PT_NOTE:    s = "PT_NOTE";    break;
          case SgAsmElfSegmentTableEntry::PT_SHLIB:   s = "PT_SHLIB";   break;
          case SgAsmElfSegmentTableEntry::PT_PHDR:    s = "PT_PHDR";    break;
          case SgAsmElfSegmentTableEntry::PT_LOPROC:  s = "PT_LOPROC";  break;
          case SgAsmElfSegmentTableEntry::PT_HIPROC:  s = "PT_HIPROC";  break;

          default:
             {
               s = "error";

            // DQ (8/29/2008): This case is exercised frequently, I think it warrants only a warning, instead of an error.
               printf ("Warning: default reached for SgAsmElfSegmentTableEntry::stringifyType = 0x%x \n",kind);
             }
        }

     return s;
   }

#if 0
// In retrospect I don't think we need this...
// DQ (26/2008): Support output of named enum values 
std::string
SgAsmElfSegmentTableEntry::stringifyFlags ( SgAsmElfSegmentTableEntry::SegmentFlags kind ) const
   {
     std::string s;

     switch (kind)
        {
          case SgAsmElfSegmentTableEntry::PF_RESERVED:  s = "PF_RESERVED"; break;
          case SgAsmElfSegmentTableEntry::PF_EPERM:     s = "PF_EPERM"; break;
          case SgAsmElfSegmentTableEntry::PF_WPERM:     s = "PF_WPERM"; break;
          case SgAsmElfSegmentTableEntry::PF_RPERM:     s = "PF_RPERM"; break;
          case SgAsmElfSegmentTableEntry::PF_PROC_MASK: s = "PF_PROC_MASK"; break;

          default:
             {
               s = "error";
               printf ("Error: default reached for SgAsmElfSegmentTableEntry::stringifyFlags = %d \n",kind);
             }
        }

     return s;
   }
#endif

/* Constructor reads the Elf Segment (Program Header) Table */
void
SgAsmElfSegmentTable::ctor()
{
    set_synthesized(true);                              /* the segment table isn't part of any explicit section */
    set_name("ELF Segment Table");
    set_purpose(SP_HEADER);

    p_entries = new SgAsmElfSegmentTableEntryList;
    p_entries->set_parent(this);
    
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    
    if (fhdr->get_e_phnum() > 0) {

        /* Check sizes */
        ROSE_ASSERT(4==fhdr->get_word_size() || 8==fhdr->get_word_size());
        size_t struct_size = 4 == fhdr->get_word_size() ? 
                            sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk) : 
                            sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk);

        if (fhdr->get_e_phentsize() < struct_size)
            throw FormatError("ELF header phentsize is too small");

        addr_t offset=0;                                /* w.r.t. the beginning of this section */
        for (size_t i=0; i<fhdr->get_e_phnum(); i++, offset += fhdr->get_e_phentsize()) {
            /* Read/decode the segment header */
            SgAsmElfSegmentTableEntry *shdr = NULL;
            if (4==fhdr->get_word_size()) {
                const SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk *disk =
                    (const SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk*)content(offset, struct_size);
                shdr = new SgAsmElfSegmentTableEntry(sex, disk);
            } else {
                const SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk *disk =
                    (const SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk*)content(offset, struct_size);
                shdr = new SgAsmElfSegmentTableEntry(sex, disk);
            }
            p_entries->get_entries().push_back(shdr);

            p_entries->get_entries().back()->set_parent(p_entries);

            /* Save extra bytes */
            addr_t nextra = fhdr->get_e_phentsize() - struct_size;
            if (nextra>0)
                shdr->get_extra() = content_ucl(offset+struct_size, nextra);

            /* Null segments are just unused slots in the table; no real section to create */
            if (SgAsmElfSegmentTableEntry::PT_NULL == shdr->get_type())
                continue;

            /* Create sections. ELF files can have a section table and a segment table. The former is used for linking and the
             * latter for loading. At this point we've already read the section table and created sections, and now we'll
             * create more sections from the segment table -- but only if the segment table describes a section we haven't
             * already seen. */
            std::vector<SgAsmGenericSection*> possible =
                fhdr->get_file()->get_sections_by_offset(shdr->get_offset(), shdr->get_filesz());
            std::vector<SgAsmGenericSection*> matching;
            for (size_t j = 0; j < possible.size(); j++) {
                if (possible[j]->get_offset()!=shdr->get_offset() || possible[j]->get_size()!=shdr->get_filesz())
                    continue; /*different file extent*/
                if (possible[j]->is_mapped()) {
                    if (possible[j]->get_mapped_rva() != shdr->get_vaddr() || possible[j]->get_mapped_size() != shdr->get_memsz())
                        continue; /*different mapped*/
                    unsigned section_perms = (possible[j]->get_mapped_rperm() ? 0x01 : 0x00) |
                                             (possible[j]->get_mapped_wperm() ? 0x02 : 0x00) |
                                             (possible[j]->get_mapped_xperm() ? 0x04 : 0x00);
                    unsigned segment_perms = (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? 0x01 : 0x00) |
                                             (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? 0x02 : 0x00) |
                                             (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? 0x04 : 0x00);
                    if (section_perms != segment_perms)
                        continue;
                }
                matching.push_back(possible[j]);
            }

            SgAsmGenericSection *s = NULL;
            if (0 == matching.size()) {
                /* No matching section; create a new one */
                char name[128];
                switch (shdr->get_type()) {
                  case SgAsmElfSegmentTableEntry::PT_LOAD:
                    sprintf(name, "ELF load (segment %zu)", i);
                    break;
                  case SgAsmElfSegmentTableEntry::PT_DYNAMIC:
                    sprintf(name, "ELF dynamic (segment %zu)", i);
                    break;
                  case SgAsmElfSegmentTableEntry::PT_INTERP:
                    sprintf(name, "ELF interpreter (segment %zu)", i);
                    break;
                  case SgAsmElfSegmentTableEntry::PT_NOTE:
                    sprintf(name, "ELF note (segment %zu)", i);
                    break;
                  case SgAsmElfSegmentTableEntry::PT_SHLIB:
                    sprintf(name, "ELF shlib (segment %zu)", i);
                    break;
                  case SgAsmElfSegmentTableEntry::PT_PHDR:
                    sprintf(name, "ELF segment table (segment %zu)", i);
                    break;
                  default:
                    sprintf(name, "ELF segment 0x%08x (segment %zu)", shdr->get_type(), i);
                    break;
                }
                s = new SgAsmGenericSection(fhdr->get_file(), fhdr, shdr->get_offset(), shdr->get_filesz());
                s->set_synthesized(true);
                s->set_name(name);
                s->set_purpose(SP_HEADER);
            } else if (1 == matching.size()) {
                /* Use the single matching section. */
                s = matching[0];
            }

            /* Make sure the section is mapped. */
            if (!s->is_mapped()) {
                s->set_mapped_rva(shdr->get_vaddr());
                s->set_mapped_size(shdr->get_memsz());
                s->set_mapped_rperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? true : false);
                s->set_mapped_wperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? true : false);
                s->set_mapped_xperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? true : false);
            }
        }
    }
}

/* Write the segment table to disk. */
void
SgAsmElfSegmentTable::unparse(FILE *f)
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();

    for (size_t i=0; i < p_entries->get_entries().size(); i++) {
        SgAsmElfSegmentTableEntry *shdr = p_entries->get_entries()[i];
        SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk disk32;
        SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk disk64;
        void *disk = NULL;
        size_t size = 0;
        
        if (4==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk32);
            size = sizeof disk32;
        } else if (8==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk64);
            size = sizeof disk64;
        } else {
            ROSE_ASSERT(!"invalid word size");
        }
        
        /* The disk struct */
        addr_t extra_offset = write(f, i * fhdr->get_e_phentsize(), size, disk);
        if (shdr->get_extra().size() > 0)
            write(f, extra_offset, shdr->get_extra());
    }

    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfSegmentTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSegmentTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSegmentTable.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zd entries\n", p, w, "size", p_entries->get_entries().size());
    for (size_t i=0; i < p_entries->get_entries().size(); i++) {
        p_entries->get_entries()[i]->dump(f, p, i);
    }

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Linking
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructors */
void
SgAsmElfDynamicEntry::ctor(ByteOrder sex, const Elf32DynamicEntry_disk *disk)
{
    p_d_tag = disk_to_host(sex, disk->d_tag);
    p_d_val = disk_to_host(sex, disk->d_val);
}

void
SgAsmElfDynamicEntry::ctor(ByteOrder sex, const Elf64DynamicEntry_disk *disk)
{
    p_d_tag = disk_to_host(sex, disk->d_tag);
    p_d_val = disk_to_host(sex, disk->d_val);
}

/* Encode a native entry back into disk format */
void *
SgAsmElfDynamicEntry::encode(ByteOrder sex, Elf32DynamicEntry_disk *disk)
{
    host_to_disk(sex, p_d_tag, &(disk->d_tag));
    host_to_disk(sex, p_d_val, &(disk->d_val));
    return disk;
}
void *
SgAsmElfDynamicEntry::encode(ByteOrder sex, Elf64DynamicEntry_disk *disk)
{
    host_to_disk(sex, p_d_tag, &(disk->d_tag));
    host_to_disk(sex, p_d_val, &(disk->d_val));
    return disk;
}

/* Print some debugging info */
void
SgAsmElfDynamicEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfDynamicEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfDynamicEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    if (p_d_tag>=34) {
        fprintf(f, "%s%-*s = 0x%08u\n",          p, w, "d_tag",      p_d_tag);
    } else {
        fprintf(f, "%s%-*s = %u\n",              p, w, "d_tag",      p_d_tag);
    }
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",     p, w, "d_val_addr", p_d_val);
}

/* Constructor */
void
SgAsmElfDynamicSection::ctor(SgAsmElfFileHeader *fhdr, SgAsmElfSectionTableEntry *shdr)
{
    p_other_entries = new SgAsmElfDynamicEntryList;
    p_all_entries   = new SgAsmElfDynamicEntryList;

    p_other_entries->set_parent(this);
    p_all_entries->set_parent(this);
}

/* Set linked section (the string table) and finish parsing this section. */
void
SgAsmElfDynamicSection::set_linked_section(SgAsmElfSection *strtab) 
{
    SgAsmElfSection::set_linked_section(strtab);
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL && fhdr == strtab->get_elf_header());

    ROSE_ASSERT(strtab != NULL);
    ROSE_ASSERT(strtab->get_name() == ".dynstr");

    size_t section_size = get_size(), entry_size = 0, nentries = 0;
    if (4==fhdr->get_word_size()) {
        const SgAsmElfDynamicEntry::Elf32DynamicEntry_disk *disk =
            (const SgAsmElfDynamicEntry::Elf32DynamicEntry_disk*)content(0, section_size);
        entry_size = sizeof(SgAsmElfDynamicEntry::Elf32DynamicEntry_disk);
        nentries = section_size / entry_size;
        for (size_t i = 0; i < nentries; i++) {
            p_all_entries->get_entries().push_back(new SgAsmElfDynamicEntry(fhdr->get_sex(), disk+i));
        }
    } else if (8==fhdr->get_word_size()) {
        const SgAsmElfDynamicEntry::Elf64DynamicEntry_disk *disk =
            (const SgAsmElfDynamicEntry::Elf64DynamicEntry_disk*)content(0, section_size);
        entry_size = sizeof(SgAsmElfDynamicEntry::Elf64DynamicEntry_disk);
        nentries = section_size / entry_size;
        for (size_t i = 0; i < nentries; i++) {
            p_all_entries->get_entries().push_back(new SgAsmElfDynamicEntry(fhdr->get_sex(), disk+i));
        }
    } else {
        throw FormatError("bad ELF word size");
    }

    for (size_t i=0; i < nentries; i++) {
        switch (p_all_entries->get_entries()[i]->get_d_tag()) {
          case 0:
            /* DT_NULL: unused entry */
            break;
          case 1: {
              /* DT_NEEDED: offset to NUL-terminated library name in the linked-to (".dynstr") section. */
#ifdef USE_ELF_STRING
              /*FIXME: Testing out some new stuff. Eventually the SgAsmGenericDLL will contain an SgAsmGenericString*/
              SgAsmElfStrtab *xxx = dynamic_cast<SgAsmElfStrtab*>(strtab);
              SgAsmElfString name(xxx, p_all_entries->get_entries()[i]->get_d_val());
              fhdr->add_dll(new SgAsmGenericDLL(name.get_string()));
#else
              const char *name = (const char*)strtab->content_str(p_all_entries->get_entries()[i]->get_d_val());
              fhdr->add_dll(new SgAsmGenericDLL(name));
#endif
              break;
          }
          case 2:
            p_dt_pltrelsz = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 3:
            p_dt_pltgot = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 4:
            p_dt_hash = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 5:
            p_dt_strtab = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 6:
            p_dt_symtab = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 7:
            p_dt_rela = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 8:
            p_dt_relasz = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 9:
            p_dt_relaent = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 10:
            p_dt_strsz = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 11:
            p_dt_symentsz = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 12:
            p_dt_init = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 13:
            p_dt_fini = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 20:
            p_dt_pltrel = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 23:
            p_dt_jmprel = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 0x6fffffff:
            p_dt_verneednum = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 0x6ffffffe:
            p_dt_verneed = p_all_entries->get_entries()[i]->get_d_val();
            break;
          case 0x6ffffff0:
            p_dt_versym = p_all_entries->get_entries()[i]->get_d_val();
            break;
          default:
            p_other_entries->get_entries().push_back(p_all_entries->get_entries()[i]);
            break;
        }
    }
}

/* Helper for ElfDynamicSection::dump */
// static 
void
SgAsmElfDynamicSection::dump_section_rva(FILE *f, const char *p, int w, const char *name, addr_t addr, SgAsmGenericFile *ef)
{
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, name, addr);
    std::vector<SgAsmGenericSection*> sections = ef->get_sections_by_rva(addr);
    for (size_t i=0; i<sections.size(); i++) {
        fprintf(f, "%s%-*s     [%d] \"%s\"", p, w, "...", sections[i]->get_id(), sections[i]->get_name().c_str());
        addr_t offset = addr - sections[i]->get_mapped_rva();
        if (offset>0) {
            addr_t nbytes = sections[i]->get_size() - offset;
            fprintf(f, " @(0x%08"PRIx64"+%"PRIu64") %"PRIu64" bytes", sections[i]->get_mapped_rva(), offset, nbytes);
        } else {
            fprintf(f, " @0x%08"PRIx64" %"PRIu64" bytes" , sections[i]->get_mapped_rva(), sections[i]->get_size());
        }
        fprintf(f, "\n");
    }
}

/* Write the dynamic section back to disk */
void
SgAsmElfDynamicSection::unparse(FILE *f)
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder sex = fhdr->get_sex();
    addr_t spos = 0; /*output position in section*/

    for (size_t i = 0; i < p_all_entries->get_entries().size(); i++) {
        SgAsmElfDynamicEntry::Elf32DynamicEntry_disk disk32;
        SgAsmElfDynamicEntry::Elf64DynamicEntry_disk disk64;
        void *disk  = NULL;
        size_t size = 0;
        
        if (4==fhdr->get_word_size()) {
            disk = p_all_entries->get_entries()[i]->encode(sex, &disk32);
            size = sizeof disk32;
        } else if (8==fhdr->get_word_size()) {
            disk = p_all_entries->get_entries()[i]->encode(sex, &disk64);
            size = sizeof disk64;
        } else {
            ROSE_ASSERT(!"unsupported word size");
        }
        spos = write(f, spos, size, disk);
    }

    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfDynamicSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sDynamicSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sDynamicSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    SgAsmGenericFile *ef = get_file();

    SgAsmElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "pltrelsz",   p_dt_pltrelsz);
    dump_section_rva(f, p, w, "pltgot",  p_dt_pltgot, ef);
    dump_section_rva(f, p, w, "hash",    p_dt_hash,   ef);
    dump_section_rva(f, p, w, "strtab",  p_dt_strtab, ef);
    dump_section_rva(f, p, w, "symtab",  p_dt_symtab, ef);
    dump_section_rva(f, p, w, "rela",    p_dt_rela,   ef);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "relasz",     p_dt_relasz);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "relaent",    p_dt_relaent);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "strsz",      p_dt_strsz);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "symentsz",   p_dt_symentsz);
    dump_section_rva(f, p, w, "init",    p_dt_init,   ef);
    dump_section_rva(f, p, w, "fini",    p_dt_fini,   ef);
    fprintf(f, "%s%-*s = %u\n",              p, w, "pltrel",     p_dt_pltrel);
    dump_section_rva(f, p, w, "jmprel",  p_dt_jmprel, ef);
    fprintf(f, "%s%-*s = %u\n",              p, w, "verneednum", p_dt_verneednum);
    dump_section_rva(f, p, w, "verneed", p_dt_verneed, ef);
    dump_section_rva(f, p, w, "versym",  p_dt_versym,  ef);

    for (size_t i = 0; i < p_other_entries->get_entries().size(); i++) {
        p_other_entries->get_entries()[i]->dump(f, p, i);
    }

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbol Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructors */
void
SgAsmElfSymbol::ctor(ByteOrder sex, const Elf32SymbolEntry_disk *disk)
{
    p_st_name  = disk_to_host(sex, disk->st_name);
    p_st_info  = disk_to_host(sex, disk->st_info);
    p_st_res1  = disk_to_host(sex, disk->st_res1);
    p_st_shndx = disk_to_host(sex, disk->st_shndx);
    p_st_size  = disk_to_host(sex, disk->st_size);

    p_value    = disk_to_host(sex, disk->st_value);
    p_size     = p_st_size;
    ctor_common();
}
void
SgAsmElfSymbol::ctor(ByteOrder sex, const Elf64SymbolEntry_disk *disk)
{
    p_st_name  = disk_to_host(sex, disk->st_name);
    p_st_info  = disk_to_host(sex, disk->st_info);
    p_st_res1  = disk_to_host(sex, disk->st_res1);
    p_st_shndx = disk_to_host(sex, disk->st_shndx);
    p_st_size  = disk_to_host(sex, disk->st_size);

    p_value    = disk_to_host(sex, disk->st_value);
    p_size     = p_st_size;
    ctor_common();
}
void
SgAsmElfSymbol::ctor_common()
{
    /* Binding */
    switch (get_elf_binding()) {
      case STB_LOCAL:   p_binding = SYM_LOCAL;  break;
      case STB_GLOBAL:  p_binding = SYM_GLOBAL; break;
      case STB_WEAK:    p_binding = SYM_WEAK;   break;
      default:
        fprintf(stderr, "ROBB: unknown elf symbol binding: %u\n", get_elf_binding());
        ROSE_ASSERT(0);
        break;
    }

    /* Type */
    switch (get_elf_type()) {
      case STT_NOTYPE:  p_type = SYM_NO_TYPE; break;
      case STT_OBJECT:  p_type = SYM_DATA;    break;
      case STT_FUNC:    p_type = SYM_FUNC;    break;
      case STT_SECTION: p_type = SYM_SECTION; break;
      case STT_FILE:    p_type = SYM_FILE;    break;
      default:
        fprintf(stderr, "ROBB: unknown elf symbol type: %u\n", get_elf_type());
        ROSE_ASSERT(0);
        break;
    }

    /* Definition state */
    if (p_value || p_size) {
        p_def_state = SYM_DEFINED;
    } else if (p_st_name > 0 || get_elf_type()) {
        p_def_state = SYM_TENTATIVE;
    } else {
        p_def_state = SYM_UNDEFINED;
    }
}

/* Encode a symbol into disk format */
void *
SgAsmElfSymbol::encode(ByteOrder sex, Elf32SymbolEntry_disk *disk)
{
    host_to_disk(sex, p_st_name,     &(disk->st_name));
    host_to_disk(sex, p_st_info,     &(disk->st_info));
    host_to_disk(sex, p_st_res1,     &(disk->st_res1));
    host_to_disk(sex, p_st_shndx,    &(disk->st_shndx));
    host_to_disk(sex, p_st_size,     &(disk->st_size));
    host_to_disk(sex, get_value(), &(disk->st_value));
    return disk;
}
void *
SgAsmElfSymbol::encode(ByteOrder sex, Elf64SymbolEntry_disk *disk)
{
    host_to_disk(sex, p_st_name,     &(disk->st_name));
    host_to_disk(sex, p_st_info,     &(disk->st_info));
    host_to_disk(sex, p_st_res1,     &(disk->st_res1));
    host_to_disk(sex, p_st_shndx,    &(disk->st_shndx));
    host_to_disk(sex, p_st_size,     &(disk->st_size));
    host_to_disk(sex, get_value(), &(disk->st_value));
    return disk;
}

/* Print some debugging info. The 'section' is an optional section pointer for the st_shndx member. */
void
SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmGenericSection *section)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    const char *s;
    char sbuf[256];

    SgAsmGenericSymbol::dump(f, p, -1);

    fprintf(f, "%s%-*s = %"PRIu64" offset into strtab\n", p, w, "st_name",  p_st_name);

    fprintf(f, "%s%-*s = %u (",          p, w, "st_info",  p_st_info);
    switch (get_elf_binding()) {
      case STB_LOCAL:  s = "local";  break;
      case STB_GLOBAL: s = "global"; break;
      case STB_WEAK:   s = "weak";   break;
      default:
        sprintf(sbuf, "binding-%d", get_elf_binding());
        s = sbuf;
        break;
    }
    fputs(s, f);
    switch (get_elf_type()) {
      case STT_NOTYPE:  s = " no-type";   break;
      case STT_OBJECT:  s = " object";    break;
      case STT_FUNC:    s = " function";  break;
      case STT_SECTION: s = " section";   break;
      case STT_FILE:    s = " file";      break;
      default:
        sprintf(sbuf, " type-%d", get_elf_type());
        s = sbuf;
        break;
    }
    fputs(s, f);
    fputs(")\n", f);

    fprintf(f, "%s%-*s = %u\n",         p, w, "st_res1", p_st_res1);
    fprintf(f, "%s%-*s = %"PRIu64"\n",  p, w, "st_size", p_st_size);

    if (section && section->get_id() == (int)p_st_shndx) {
        fprintf(f, "%s%-*s = [%d] \"%s\" @%"PRIu64", %"PRIu64" bytes\n", p, w, "st_shndx",
                section->get_id(), section->get_name().c_str(), section->get_offset(), section->get_size());
    } else {
        fprintf(f, "%s%-*s = %u\n",         p, w, "st_shndx", p_st_shndx);        
    }
}

/* Constructor */
void
SgAsmElfSymbolSection::ctor(SgAsmElfSectionTableEntry *shdr)
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);

    p_symbols = new SgAsmElfSymbolList;

    if (4==fhdr->get_word_size()) {
        const SgAsmElfSymbol::Elf32SymbolEntry_disk *disk =
            (const SgAsmElfSymbol::Elf32SymbolEntry_disk*)content(0, get_size());
        size_t nentries = get_size() / sizeof(SgAsmElfSymbol::Elf32SymbolEntry_disk);
        for (size_t i=0; i<nentries; i++) {
#if 0
            fprintf(stderr, "ROBB: SgAsmElfSymbolSection::ctor():\n");
            fprintf(stderr, "           p_symbols=0x%08lx\n", (unsigned long)p_symbols);
#endif
            p_symbols->get_symbols().push_back(new SgAsmElfSymbol(fhdr->get_sex(), disk+i));
        }
    } else {
        const SgAsmElfSymbol::Elf64SymbolEntry_disk *disk =
            (const SgAsmElfSymbol::Elf64SymbolEntry_disk*)content(0, get_size());
        size_t nentries = get_size() / sizeof(SgAsmElfSymbol::Elf64SymbolEntry_disk);
        for (size_t i=0; i<nentries; i++) {
            p_symbols->get_symbols().push_back(new SgAsmElfSymbol(fhdr->get_sex(), disk+i));
        }
    }
}

/* Symbol table sections link to their string tables. Updating the string table should cause the symbol names to be updated.
 * Also update section pointers for locally-bound symbols since we know that the section table has been read and all
 * non-synthesized sections have been created.
 * 
 * The st_shndx is the index (ID) of the section to which the symbol is bound. Special values are:
 *   0x0000        no section (section table entry zero should be all zeros anyway)
 *   0xff00-0xffff reserved values, not an index
 *   0xff00-0xff1f processor specific values
 *   0xfff1        symbol has absolute value not affected by relocation
 *   0xfff2        symbol is fortran common or unallocated C extern */
void
SgAsmElfSymbolSection::set_linked_section(SgAsmElfSection *strtab)
{
    SgAsmElfSection::set_linked_section(strtab);
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmElfSymbol *symbol = p_symbols->get_symbols()[i];

        /* Get symbol name */
        symbol->set_name(strtab->content_str(symbol->get_st_name()));

        /* Get bound section ptr */
        if (symbol->get_st_shndx() > 0 && symbol->get_st_shndx() < 0xff00) {
            SgAsmGenericSection *bound = get_file()->get_section_by_id(symbol->get_st_shndx());
            ROSE_ASSERT(bound != NULL);
            symbol->set_bound(bound);
        }

        /* Section symbols may need names and sizes */
        if (symbol->get_type() == SgAsmElfSymbol::SYM_SECTION && symbol->get_bound()) {
            if (symbol->get_name().size() == 0)
                symbol->set_name(symbol->get_bound()->get_name());
            if (symbol->get_size() == 0)
                symbol->set_size(symbol->get_bound()->get_size());
        }
    }
}

/* Write symbol table sections back to disk */
void
SgAsmElfSymbolSection::unparse(FILE *f)
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder sex = fhdr->get_sex();
    addr_t spos=0; /*output position in section*/

    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmElfSymbol::Elf32SymbolEntry_disk disk32;
        SgAsmElfSymbol::Elf64SymbolEntry_disk disk64;
        void *disk=NULL;
        size_t size = 0;
        
        if (4==fhdr->get_word_size()) {
            disk = p_symbols->get_symbols()[i]->encode(sex, &disk32);
            size = sizeof disk32;
        } else if (8==fhdr->get_word_size()) {
            disk = p_symbols->get_symbols()[i]->encode(sex, &disk64);
            size = sizeof disk64;
        } else {
            ROSE_ASSERT(!"unsupported word size");
        }
        spos = write(f, spos, size, disk);
    }

    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfSymbolSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymbolSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymbolSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu symbols\n", p, w, "ElfSymbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmGenericSection *section = get_file()->get_section_by_id(p_symbols->get_symbols()[i]->get_st_shndx());
        p_symbols->get_symbols()[i]->dump(f, p, i, section);
    }

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns true if a cursory look at the file indicates that it could be an ELF file. */
bool
SgAsmElfFileHeader::is_ELF(SgAsmGenericFile *f)
{
    SgAsmElfFileHeader *hdr = NULL;

    bool retval = false;

    ROSE_ASSERT(f != NULL);
    
    try {
        hdr = new SgAsmElfFileHeader(f, 0);
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }

    delete hdr;
    return retval;
}

/* Parses the structure of an ELF file and adds the info to the ExecFile */
SgAsmElfFileHeader *
SgAsmElfFileHeader::parse(SgAsmGenericFile *ef)
{
    ROSE_ASSERT(ef);
    
    SgAsmElfFileHeader *fhdr = new SgAsmElfFileHeader(ef, 0);
    ROSE_ASSERT(fhdr != NULL);

    /* Read the optional section and segment tables and the sections to which they point. */
    if (fhdr->get_e_shnum())
        fhdr->set_section_table( new SgAsmElfSectionTable(fhdr) );
    if (fhdr->get_e_phnum())
        fhdr->set_segment_table( new SgAsmElfSegmentTable(fhdr) );

    /* Use symbols from either ".symtab" or ".dynsym" */
    SgAsmElfSymbolSection *symtab = dynamic_cast<SgAsmElfSymbolSection*>(ef->get_section_by_name(".symtab"));
    if (!symtab)
        symtab = dynamic_cast<SgAsmElfSymbolSection*>(ef->get_section_by_name(".dynsym"));
    if (symtab) {
        std::vector<SgAsmElfSymbol*> & symbols = symtab->get_symbols()->get_symbols();
        for (size_t i=0; i<symbols.size(); i++)
            fhdr->add_symbol(symbols[i]);
    }

#if 0 /* Temporary tests */
    /* Some tests for string allocation functions. Some of these assume that the string table complies with the ELF
     * specification, which guarantees that the first byte of the string table is NUL. The parser can handle non-compliant
     * string tables. */
    SgAsmElfSymbolSection *dynsym = dynamic_cast<SgAsmElfSymbolSection*>(ef->get_section_by_name(".dynsym"));
    ROSE_ASSERT(dynsym!=NULL);
    SgAsmElfStrtab *dynstr = dynamic_cast<SgAsmElfStrtab*>(dynsym->get_linked_section());
    ROSE_ASSERT(dynstr!=NULL);
    const SgAsmElfSymbolPtrList &symbols = dynsym->get_symbols()->get_symbols();
    SgAsmElfString *test = NULL;
    for (size_t i=0; i<symbols.size(); i++) {
        if (symbols[i]->get_name()=="memset") {
            test = new SgAsmElfString(dynstr, symbols[i]->get_st_name());
        }
    }
    ROSE_ASSERT(test!=NULL);

#if 1
    /* What happens if the dynamic string table needs to grow? */
    test->set_string("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
#endif

#if 0 /*First batch of tests*/
    /* Test 1: Create another reference to the empty string and then try to modify it. This should create a new string rather
     *         than modifying the empty string. The ELF specification reserves offset zero to hold a NUL to represent the
     *         empty string and we must leave the NUL there even if nothing references it. */
    SgAsmElfString *s1 = new SgAsmElfString(dynstr, 0);
    ROSE_ASSERT(s1->get_string()==""); /*must comply with spec!*/
    s1->set_string("fprintf");
    ROSE_ASSERT(s1->get_offset()!=SgAsmElfString::unallocated);
    ROSE_ASSERT(s1->get_offset()!=0);
    
    /* Test 2: Create a new string that happens to have the same initial value as something already in the string table. When
     *         allocated the new string will share the same space as the existing string. */
    s1 = new SgAsmElfString(dynstr, test->get_offset());
    ROSE_ASSERT(s1->get_offset()!=SgAsmElfString::unallocated);
    ROSE_ASSERT(s1->get_offset()==test->get_offset());

    /* Test 3: Change one of two strings that share the same offset by removing the first character. The new offset should be
     *         incremented by one. */
    s1->set_string(&(s1->get_string()[1]));
    ROSE_ASSERT(s1->get_offset()!=SgAsmElfString::unallocated);
    ROSE_ASSERT(s1->get_offset()==test->get_offset()+1);
    
    /* Test 4: Change one of two strings that share memory and see if it gets allocated elsewhere. */
    s1->set_string("intf"); /*probably to the end of "fprintf"*/
    ROSE_ASSERT(s1->get_offset()!=SgAsmElfString::unallocated);
    ROSE_ASSERT(s1->get_offset()+s1->get_string().size()+1 <= test->get_offset() ||  /*left of test*/
                s1->get_offset() >= test->get_offset()+test->get_string().size()+1);       /*right of test*/

    /* Test 5: Copy a string and change the copy. Both strings should change. */
    SgAsmElfString copy = *s1;
    copy.set_string(test->get_string());
    ROSE_ASSERT(s1->get_offset()!=SgAsmElfString::unallocated);
    ROSE_ASSERT(s1->get_offset()==copy.get_offset());
    ROSE_ASSERT(s1->get_string()==copy.get_string());

    /* Test 6: Effectively delete a string by setting it to the empty string. Its offset should revert to zero along with all
     *         copies of the string. */
    s1->set_string("");
    ROSE_ASSERT(s1->get_offset()==0);
    ROSE_ASSERT(copy.get_offset()==0);
    ROSE_ASSERT(copy.get_string()=="");

    /* Test 7: Deleting one copy should not cause problems for the other copy. */
    delete s1;
    s1 = NULL;
    ROSE_ASSERT(copy.get_offset()==0);
    ROSE_ASSERT(copy.get_string()=="");

    /* Test 8: Reallocate the entire string table, leaving holes (unparsed areas) at their original offsets. Ideally the table
     *         size should not increase even though the compiler/linker may have agressively shared string storage. */
    addr_t orig_size = dynstr->get_size();
    dynstr->free_all_strings();
    dynstr->reallocate();
    ROSE_ASSERT(orig_size==dynstr->get_size());

    /* Test 9: Reallocate the *entire* string table, blowing away holes in the process. */
    dynstr->free_all_strings(true);
    dynstr->reallocate();
    ROSE_ASSERT(orig_size==dynstr->get_size());

    /* Test 10: After reallocating an entire table, the empty string should remain at offset zero. */
    SgAsmElfString *s2 = new SgAsmElfString(dynstr, 0);
    ROSE_ASSERT(s2->get_offset()==0);
    ROSE_ASSERT(s2->get_string()=="");

#if 0
    /* Test 11: It's not legal to (re)parse a new region of the string table after we've made modifications. */
    fprintf(stderr, "TESTING: an error message and abort should follow this line.\n");
    s2 = new SgAsmElfString(dynstr, test->get_offset());
#endif
#endif

#endif

    return fhdr;
}
    
    
// }; //namespace ELF
// }; //namespace Exec
