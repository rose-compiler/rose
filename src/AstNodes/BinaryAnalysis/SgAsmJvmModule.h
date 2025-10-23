#include <sageContainer.h>

/** JVM Module attribute.
 *
 *  The Module attribute is a variable-length attribute in the attributes table of a ClassFile structure
 *  (section 4.1). The Module attribute indicates the modules required by a module; the packages exported
 *  and opened by a module; and the services used and provided by a module. See section 4.7.25 of the
 *  JVM specification. */
class SgAsmJvmModule: public SgAsmJvmAttribute {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Module requires.
     *
     *  */
    struct Requires {
      uint16_t requires_index;
      uint16_t requires_flags;
      uint16_t requires_version_index;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(requires_index);
            s & BOOST_SERIALIZATION_NVP(requires_flags);
            s & BOOST_SERIALIZATION_NVP(requires_version_index);
        }
#endif
    };


    /** Module exports.
     *
     *  */
    struct Exports {
      uint16_t exports_index;
      uint16_t exports_flags;
      std::vector<uint16_t> exports_to_index;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(exports_index);
            s & BOOST_SERIALIZATION_NVP(exports_flags);
            s & BOOST_SERIALIZATION_NVP(exports_to_index);
        }
#endif
    };

    /** Module opens.
     *
     *  */
    struct Opens {
      uint16_t opens_index;
      uint16_t opens_flags;
      std::vector<uint16_t> opens_to_index;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(opens_index);
            s & BOOST_SERIALIZATION_NVP(opens_flags);
            s & BOOST_SERIALIZATION_NVP(opens_to_index);
        }
#endif
    };

    /** Module provides.
     *
     *  */
    struct Provides {
      uint16_t provides_index;
      std::vector<uint16_t> provides_with_index;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(provides_index);
            s & BOOST_SERIALIZATION_NVP(provides_with_index);
        }
#endif
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: module_name_index
     *
     *  The value of the module_name_index item must be a valid index into the constant_pool table. The
     *  constant_pool entry at that index must be a CONSTANT_Module_info structure (section 4.4.11).
     *  denoting the current module. */
    [[using Rosebud: rosetta]]
    uint16_t module_name_index = 0;

    /** Property: module_flags
     *
     *  The value of the module_flags item is as follows:
     *    0x0020 (ACC_OPEN) Indicates that this module is open;
     *    0x1000 (ACC_SYNTHETIC) Indicates that this module was not explicitly or implicitly declared;
     *    0x8000 (ACC_MANDATED) Indicates that this module was implicitly declared. */
    [[using Rosebud: rosetta]]
    uint16_t module_flags = 0;

    /** Property: module_version_index
     *
     *  The value of each entry in the uses_index table must be a valid index into the constant_pool table.
     *  The constant_pool entry at that index must be a CONSTANT_Class_info structure (section 4.4.1)
     *  representing a service interface which the current module may discover via java.util.ServiceLoader. */
    [[using Rosebud: rosetta]]
    uint16_t module_version_index;

    /** Property: requires
     *
     *  Each entry in the requires table specifies a dependence of the current module. */
    [[using Rosebud: rosetta, large]]
    std::vector<SgAsmJvmModule::Requires*> requires;

    /** Property: exports
     *
     *  Each entry in the exports table specifies a package exported by the current module, such that public
     *  and protected types in the package, and their public and protected members, may be accessed from
     *  outside the current module, possibly from a limited set of "friend" modules. */
    [[using Rosebud: rosetta, large]]
    std::vector<SgAsmJvmModule::Exports*> exports;

    /** Property: opens
     *
     *  Each entry in the opens table specifies a package opened by the current module, such that all types
     *  in the package, and all their members, may be accessed from outside the current module via the
     *  reflection libraries of the Java SE Platform, possibly from a limited set of "friend" modules. */
    [[using Rosebud: rosetta, large]]
    std::vector<SgAsmJvmModule::Opens*> opens;

    /** Property: uses_index
     *
     *  The value of each entry in the uses_index table must be a valid index into the constant_pool table.
     *  The constant_pool entry at that index must be a CONSTANT_Class_info structure (section 4.4.1)
     *  representing a service interface which the current module may discover via java.util.ServiceLoader. */
    [[using Rosebud: rosetta, large]]
    std::vector<uint16_t> uses_index;

    /** Property: provides
     *
     * Each entry in the provides table represents a service implementation for a given service interface. */
    [[using Rosebud: rosetta, large]]
    std::vector<SgAsmJvmModule::Provides*> provides;

public:
    /** Initialize the Module attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmModule(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmModule* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
