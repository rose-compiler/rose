#include <sageContainer.h>

/** Represents a JVM InnerClasses attribute.
 *
 *  The InnerClasses attribute is a variable-length attribute in the attributes table of a ClassFile structure. See section 4.7.6 of
 *  the JVM specification.
 */
class SgAsmJvmInnerClasses: public SgAsmJvmAttribute {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** JVM InnerClassesEntry.
     *
     *  Every CONSTANT_Class_info entry in the constant_pool table which represents a class or interface C that
     *  is not a package member must have exactly one corresponding entry in the classes array.  See section 4.7.6 of
     *  the JVM specification. */
    struct Entry {
        uint16_t inner_class_info_index = 0;
        uint16_t outer_class_info_index = 0;
        uint16_t inner_name_index = 0;
        uint16_t inner_class_access_flags = 0;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(inner_class_info_index);
            s & BOOST_SERIALIZATION_NVP(outer_class_info_index);
            s & BOOST_SERIALIZATION_NVP(inner_name_index);
            s & BOOST_SERIALIZATION_NVP(inner_class_access_flags);
        }
#endif
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: List of pointers to InnerClasses attribute entry. */
    [[using Rosebud: rosetta, large, mutators()]]
    std::vector<SgAsmJvmInnerClasses::Entry*> classes;

    /** Initialize the InnerClasses attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing as it sets its parent. */
    explicit SgAsmJvmInnerClasses(SgAsmJvmAttributeTable* table);

    /** Parses a JVM InnerClasses attribute.
     *
     * Parses a JVM InnerClasses attribute and constructs and parses all classes
     * entries reachable from the table. Returns a pointer to this object. */
    SgAsmJvmInnerClasses* parse(SgAsmJvmConstantPool*) override;

    /** Write the InnerClasses attribute to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
