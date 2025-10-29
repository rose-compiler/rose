#include <sageContainer.h>

/** JVM LineNumberTable attribute.
 *
 *  The LineNumberTable attribute is an optional variable-length attribute in the attributes table of a Code attribute (4.7.3). It
 *  may be used by debuggers to determine which part of the code array corresponds to a given line number in the original source
 *  file.  See section 4.7.12 of the JVM specification. */
class SgAsmJvmLineNumberTable: public SgAsmJvmAttribute {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** JVM LineNumberEntry.
     *
     *  Each entry in the line_number_table array indicates that the line number in the original source file changes
     *  at a given point in the code array. */
    struct Entry {
        uint16_t start_pc = 0;
        uint16_t line_number = 0;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(start_pc);
            s & BOOST_SERIALIZATION_NVP(line_number);
        }
#endif
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: line_number_table
     *
     *  List of pointers to line_number_table entries (see the JVM specification 4.7.12). */
    [[using Rosebud: rosetta, large, mutators()]]
    std::vector<SgAsmJvmLineNumberTable::Entry*> line_number_table;

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
