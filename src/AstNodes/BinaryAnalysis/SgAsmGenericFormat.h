#include <Rose/BinaryAnalysis/ByteOrder.h>

/** Basic information about an executable container. */
class SgAsmGenericFormat: public SgAsmExecutableFileFormat {
    /** Property: Family. */
    [[using Rosebud: rosetta]]
    SgAsmGenericFormat::ExecFamily family = SgAsmGenericFormat::FAMILY_UNSPECIFIED;

    /** Property: Purpose. */
    [[using Rosebud: rosetta]]
    SgAsmGenericFormat::ExecPurpose purpose = SgAsmGenericFormat::PURPOSE_EXECUTABLE;

    /** Property: Byte order. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::ByteOrder::Endianness sex = Rose::BinaryAnalysis::ByteOrder::ORDER_UNSPECIFIED;

    /** Property: File format version number. */
    [[using Rosebud: rosetta]]
    unsigned version = 0;

    /** Property: Whether version is considered current.
     *
     *  True if the @ref get_version "version" property is considered to be the current, supported version for this file
     *  format. */
    [[using Rosebud: rosetta]]
    bool is_current_version = false;

    /** Property: Application binary interface. */
    [[using Rosebud: rosetta]]
    SgAsmGenericFormat::ExecABI abi = SgAsmGenericFormat::ABI_UNSPECIFIED;

    /** Property: Application binary interface version. */
    [[using Rosebud: rosetta]]
    unsigned abi_version = 0;

    /** Property: Natural word size in bytes. */
    [[using Rosebud: rosetta]]
    size_t word_size = 0;

public:
    /** Print some debugging info. */
    void dump(FILE*, const char *prefix, ssize_t idx) const;
};
