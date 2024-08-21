#ifdef ROSE_IMPL
#include <SgAsmCilMetadataRoot.h>
#endif

/** CIL Managed Code section.
 *
 *  "CLI" means "common language infrastructure; "CIL" is "common intermediate language". */
class SgAsmCliHeader: public SgAsmGenericSection {
public:
    /** Property: Cb
     *
     *  Size of the header in bytes */
    [[using Rosebud: rosetta]]
    uint32_t cb = 0;

    /** Property: MajorRuntimeVersion
     *
     *  The minimum version of the runtime required to run this program, currently 2. */
    [[using Rosebud: rosetta]]
    uint16_t majorRuntimeVersion = 0;

    /** Property: MinorRuntimeVersion
     *
     *  The minor portion of the version, currently 0. */
    [[using Rosebud: rosetta]]
    uint16_t minorRuntimeVersion = 0;

    /** Property: MetaData
     *
     *  RVA and size of implementation-specific resources (II.24). */
    [[using Rosebud: rosetta]]
    uint64_t metaData = 0;

    /** Property: Flags
     *
     *  Flags describing this runtime image (II.25.3.3.1). */
    [[using Rosebud: rosetta]]
    uint32_t flags = 0;

    /** Property: EntryPointToken
     *
     *  Token for the MethodDef or File of the entry point for the image. */
    [[using Rosebud: rosetta]]
    uint32_t entryPointToken = 0;

    /** Property: Resources
     *
     *  RVA and size of implementation-specific resources. */
    [[using Rosebud: rosetta]]
    uint64_t resources = 0;

    /** Property: StrongNameSignature
     *
     *  RVA of the hash data for this PE file used by the CLI loader for binding and versioning. */
    [[using Rosebud: rosetta]]
    uint64_t strongNameSignature = 0;

    /** Property: CodeManagerTable
     *
     *  Always 0 (II.24.1) */
    [[using Rosebud: rosetta]]
    uint64_t codeManagerTable = 0;

    /** Property: VTableFixups
     *
     *  RVA of an array of locations in the file that contain an array of function pointers (e.g., vtable slots). */
    [[using Rosebud: rosetta]]
    uint64_t vTableFixups = 0;

    /** Property: ExportAddressTableJumps
     *
     *  Always 0 (II.24.1) */
    [[using Rosebud: rosetta]]
    uint64_t exportAddressTableJumps = 0;

    /** Property: ManagedNativeHeader
     *
     *  Always 0 (II.24.1) */
    [[using Rosebud: rosetta]]
    uint64_t managedNativeHeader = 0;

    // FIXME[Robb Matzke 2023-03-20]: is the lack of serialization a bug?
    /** Property: pointer to the root of the CIL Metadata. */
    [[using Rosebud: rosetta, traverse, serialize()]]
    SgAsmCilMetadataRoot* metadataRoot = nullptr;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmCliHeader(SgAsmPEFileHeader*);

    /** Initialize the object by parsing content from the PE file. */
    virtual SgAsmCliHeader* parse() override;

    /** Print some debugging information */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
