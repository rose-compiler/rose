#include <sageContainer.h>

/** JVM LocalVariableTable attribute.
 *
 *  The LocalVariableTable attribute is an optional variable-length attribute in the attributes table of a Code
 *  attribute (4.7.3). It may be used by debuggers to determine the value of a given local variable during the
 *  execution of a method. See section 4.7.13 of the JVM specification. */
class SgAsmJvmLocalVariableTable: public SgAsmJvmAttribute {
public:
    /** Property: local_variable_table
     *
     *  List of pointers to local_variable_table entries (see the JVM specification 4.7.13). */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmLocalVariableEntryPtrList local_variable_table;

public:
    /** Initialize the LocalVariableTable attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmLocalVariableTable(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmLocalVariableTable* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
