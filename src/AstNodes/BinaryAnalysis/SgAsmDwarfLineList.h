#include <sageContainer.h>

class SgAsmDwarfLineList: public SgAsmDwarfInformation {
public:
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmDwarfLinePtrList line_list;

#if 0
    // I [DQ] am having trouble making these proper data members so just use function to return them, so that they are
    // computed dynamically.
    AsmDwarfLineList.setDataPrototype("SgInstructionAddressSourcePositionMapPtrList", "instructionToSourceMap", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfLineList.setDataPrototype("SgSourcePositionInstructionAddressMapPtrList", "sourceToInstructionMap", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

public:
    // Once the maps are setup using a valid SgAsmDwarfCompilationUnit, NULL is an acceptable value.
    //! Support for building maps to and from instruction addresses to source positions (files, line numbers, column numbers).
    static DwarfInstructionSourceMapReturnType buildInstructionAddressSourcePositionMaps( SgAsmDwarfCompilationUnit* dwarf_cu = NULL );

    //! Output information about instruction address <--> source position
    void display( const std::string & label );

    static std::pair<uint64_t,uint64_t> instructionRange();
    static std::pair<LineColumnFilePosition,LineColumnFilePosition> sourceCodeRange( int file_id );

    static uint64_t sourceCodeToAddress ( FileIdLineColumnFilePosition sourcePosition );
    static FileIdLineColumnFilePosition addressToSourceCode ( uint64_t address );
};
