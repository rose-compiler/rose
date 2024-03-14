#include <sageContainer.h>

/** JVM LineNumberTable attribute.
 *
 *  The LineNumberTable attribute is an optional variable-length attribute in the attributes table of a Code attribute (4.7.3). It
 *  may be used by debuggers to determine which part of the code array corresponds to a given line number in the original source
 *  file.  See section 4.7.12 of the JVM specification. */
class SgAsmJvmLineNumberTable: public SgAsmJvmAttribute {
public:
    /** Property: line_number_table
     *
     *  List of pointers to line_number_table entries (see the JVM specification 4.7.12). */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmLineNumberEntryPtrList line_number_table;

public:
    /** Initialize the LineNumberTable attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmLineNumberTable(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmLineNumberTable* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the line number table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
