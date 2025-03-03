#include <Rose/BinaryAnalysis/Address.h>

/** ELF string table. */
class SgAsmElfStrtab: public SgAsmGenericStrtab {
public:
    /** Non-parsing constructor.
     *
     *  The table is created to be at least one byte long and having a NUL character as the first byte. */
    explicit SgAsmElfStrtab(class SgAsmElfSection *containing_section);

    /** Free StringStorage objects associated with this string table.
     *
     *  It may not be safe to blow them away yet since other objects may still have @ref SgAsmStoredString objects pointing to
     *  these storage objects. So instead, we will mark all this strtab's storage objects as no longer being associated
     *  with a string table. This allows the @ref SgAsmStoredString objects to still function properly and their
     *  destructors will free their storage. */
    void destructorHelper() override;

    /** Parses the string table.
     *
     *  All that actually happens at this point is we look to see if the table begins with an empty string. */
    virtual SgAsmElfStrtab *parse() override;

    /** Write string table back to disk.
     *
     *  Free space is zeroed out; holes are left as they are. */
    virtual void unparse(std::ostream&) const;

    /** Creates the storage item for the string at the specified offset.
     *
     *  If @p shared is true then attempt to re-use a previous storage object, otherwise always create a new one. Each
     *  storage object is considered a separate string, therefore when two strings share the same storage object, changing
     *  one string changes the other. */
    virtual SgAsmStringStorage *createStorage(Rose::BinaryAnalysis::Address offset, bool shared) override;

    /** Returns the number of bytes required to store the string in the string table.
     *
     *  This is the length of the string plus one for the NUL terminator. */
    virtual Rose::BinaryAnalysis::Address get_storageSize(const SgAsmStringStorage*) override;

    /** Find offset for a string.
     *
     *  Tries to find a suitable offset for a string such that it overlaps with some other string already allocated. If the new
     *  string is the same as the end of some other string (new="main", existing="domain") then we just use an offset into that
     *  string since the space is already allocated for the existing string. If the new string ends with an existing string
     *  (new="domain", existing="main") and there's enough free space before the existing string (two bytes in this case) then we
     *  allocate some of that free space and use a suitable offset. In any case, upon return `storage->get_offset()` will return the
     *  allocated offset if successful, or @ref SgAsmGenericString::unallocated if we couldn't find an overlap. */
    virtual void allocateOverlap(SgAsmStringStorage*) override;

    /** Similar to `create_storage` but uses a storage object that's already been allocated. */
    virtual void rebind(SgAsmStringStorage*, Rose::BinaryAnalysis::Address) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual SgAsmStringStorage *create_storage(Rose::BinaryAnalysis::Address, bool) override ROSE_DEPRECATED("use createStorage");
    virtual Rose::BinaryAnalysis::Address get_storage_size(const SgAsmStringStorage*) override
        ROSE_DEPRECATED("use get_storageSize");
    virtual void allocate_overlap(SgAsmStringStorage*) override ROSE_DEPRECATED("use allocateOverlap");
};
