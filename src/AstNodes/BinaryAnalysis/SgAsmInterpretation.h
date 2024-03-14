#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionMap.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>

#ifdef ROSE_IMPL
#include <SgAsmBlock.h>
#include <SgAsmGenericHeaderList.h>
#endif

/** Represents an interpretation of a binary container.
 *
 *  An interpretation is a collection of the parts of a binary specimen that represent a coherent program, library, core
 *  dump, etc. For instance, a Windows PE executable has a DOS interpretation and a Windows interpretation--really two
 *  executables in the one container. All the DOS-related stuff will be under one SgAsmInterpretation AST and all the
 *  Windows-related stuff will be under another SgAsmInterpretation AST. */
class SgAsmInterpretation: public SgAsmNode {
public:
    /** Property: File headers.
     *
     *  List of pointers to the file headers that compose this interpretation.
     *
     *  These headers are not considered to be children of this interpretation in the AST--they are reached from other
     *  traversal paths. */
    [[using Rosebud: rosetta]]
    SgAsmGenericHeaderList* headers = createAndParent<SgAsmGenericHeaderList>(this);

    /** Property: Global block.
     *
     *  The global block is the top of the AST for this interpretation's functions, basic blocks, and instructions. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmBlock* globalBlock = nullptr;

    // ROSETTA doesn't understand this type, but we want this treated like a property whose data member name is "p_map"
    // and which has automatically generator accessors and mutators named "get_map" and "set_map" and is serialized.
    /** Property: Memory map.
     *
     *  This is the memory map representing the entire interpretation. */
    [[using Rosebud: data(p_map), accessors(get_map), mutators(set_map)]]
    Rose::BinaryAnalysis::MemoryMap::Ptr map;

    // ROSETTA doesn't understand this type, but we want it serialized. Therfore, we'll define it as a property, but we'll
    // supply our own accessor and no mutator.
    /** Property: Cached map of instructions by address.
     *
     *  Returns the @ref InstructionMap associated with an interpretation. The instruction map is recomputed if the
     *  currently cached map is empty or if the @p recompute argument is true. Otherwise this just returns the existing
     *  map. No attempt is made to make sure that the map is up-to-date with respect to the current state of the AST.
     *
     *  @{ */
    [[using Rosebud: data(instruction_map), accessors(), mutators()]]
    mutable Rose::BinaryAnalysis::InstructionMap instruction_map; // cached instruction map

    Rose::BinaryAnalysis::InstructionMap& get_instructionMap(bool recompute = false);
    void set_instructionMap(const Rose::BinaryAnalysis::InstructionMap&);
    /** @} */

    /** Property: code coverage percent.
     *
     *  True iff percentageCoverage has been computed.
     *
     *  Results of a code coverage analysis. */
    [[using Rosebud: rosetta]]
    bool coverageComputed = false;

    /** Property: code coverage percent.
     *
     *  The percentage of an interpretation where each section is marked as executable and identified for
     *  disassembly into instructions. */
    [[using Rosebud: rosetta]]
    double percentageCoverage = NAN;

public:
    /** Returns a list of all files referenced by an interpretation.
     *
     *  It does this by looking at the file headers referenced by the interpretation, following their parent pointers up to an
     *  SgAsmGenericFile node, and returning a vector of those nodes with duplicate files removed. */
    SgAsmGenericFilePtrList get_files() const;

    /** Populate a map of instructions indexed by their virtual addresses.
     *
     *  This function traverses the AST rooted at the @ref globalBlock and inserts each encountered instruction into the provided
     *  @ref Rose::BinaryAnalysis::InstructionMap based on its starting virtual address. */
    void insertInstructions(Rose::BinaryAnalysis::InstructionMap&/*in,out*/);

    /** Erase instructions from a map.
     *
     *  This function traverses the AST rooted at the @ref globalBlock and erases each encountered instruction from the provided
     *  @ref Rose::BinaryAnalysis::InstructionMap based on its starting virtual address. */
    void eraseInstructions(Rose::BinaryAnalysis::InstructionMap&/*in,out*/);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmBlock* get_global_block() const ROSE_DEPRECATED("use get_globalBlock");
    void set_global_block(SgAsmBlock*) ROSE_DEPRECATED("use set_globalBlock");
    Rose::BinaryAnalysis::InstructionMap& get_instruction_map(bool=false) ROSE_DEPRECATED("use get_instructionMap");
    void set_instruction_map(const Rose::BinaryAnalysis::InstructionMap&) ROSE_DEPRECATED("use set_instructionMap");
    void insert_instructions(Rose::BinaryAnalysis::InstructionMap&) ROSE_DEPRECATED("use insertInstructions");
    void erase_instructions(Rose::BinaryAnalysis::InstructionMap&) ROSE_DEPRECATED("use eraseInstructions");
};
