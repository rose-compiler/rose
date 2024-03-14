#include <sageContainer.h>

#ifdef ROSE_IMPL
#include <SgAsmBasicString.h>
#endif

/** One entry of an ELF notes table. */
class SgAsmElfNoteEntry: public SgAsmExecutableFileFormat {
public:
    /** Property: Type of note.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    unsigned type = 0;

    /** Property: Note name.
     *
     *  A string note name stored in an ELF string table in the binary specimen. Changing the name of a note also changes
     *  the contents of the string table. */
    [[using Rosebud: rosetta, traverse, accessors(), mutators()]]
    SgAsmGenericString* name = createAndParent<SgAsmBasicString>(this);

    /** Property: Note payload.
     *
     *  This is the data associated with the note. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList payload;

public:
    /** Constructor adds the new note to the list of notes for the note section. */
    SgAsmElfNoteEntry(SgAsmElfNoteSection*);

    /** Property: Note name.
     *
     *  A string note name stored in an ELF string table in the binary specimen. Changing the name of a note also changes
     *  the contents of the string table.
     *
     * @{ */
    SgAsmGenericString *get_name() const;
    void set_name(SgAsmGenericString *name);
    /** @} */

    /** Initialize a note by parsing it from the specified location in the note section.
     *
     *  Return value is the offset to the beginning of the next note. */
    rose_addr_t parse(rose_addr_t starting_offset);

    /** Write a note at the specified offset to the section containing the note.
     *
     *  Returns the offset for the first byte past the end of the note. */
    rose_addr_t unparse(std::ostream &f, rose_addr_t starting_offset);

    /** Print some debugging information */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Property: Note payload.
     *
     *  This is the data associated with the note. */
    void set_payload(const void*, size_t nbytes);

    /** Returns the number of bytes needed to store this note. */
    rose_addr_t calculateSize() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    rose_addr_t calculate_size() const ROSE_DEPRECATED("use calculateSize");
};
