// This file represents support in the ROSE AST for Dwarf debug information.
// Dwarf is generated when source code is compiled using a compiler's debug mode.
// the Dwarf information is represented a special sections in the file format of
// the binary executable.  These sections are read using libdwarf (open source
// library used for reading and writing dwarf sections).  In ROSE we read the
// Dwarf information and build special IR nodes into the AST.  This work effects
// only AST for binary executable files (the ROSE Binary AST).
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <sage3basic.h>                                 // must be first

#include <Rose/BinaryAnalysis/Dwarf.h>
#include <Rose/Diagnostics.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <ROSE_UNUSED.h>

#include <stringify.h>

using namespace Rose::Diagnostics;

// This function implements the factory pattern.
SgAsmDwarfConstruct*
SgAsmDwarfConstruct::createDwarfConstruct(int tag, int nesting_level, uint64_t offset, uint64_t overall_offset) {
#ifdef ROSE_HAVE_LIBDWARF
    using namespace Rose::BinaryAnalysis::Dwarf;
    switch (tag) {
        case DW_TAG_array_type:
            return new SgAsmDwarfArrayType(nesting_level,offset, overall_offset);
        case DW_TAG_class_type:
            return new SgAsmDwarfClassType(nesting_level,offset, overall_offset);
        case DW_TAG_entry_point:
            return new SgAsmDwarfEntryPoint(nesting_level,offset, overall_offset);
        case DW_TAG_enumeration_type:
            return new SgAsmDwarfEnumerationType(nesting_level,offset, overall_offset);
        case DW_TAG_formal_parameter:
            return new SgAsmDwarfFormalParameter(nesting_level,offset, overall_offset);
        case DW_TAG_imported_declaration:
            return new SgAsmDwarfImportedDeclaration(nesting_level,offset, overall_offset);
        case DW_TAG_label:
            return new SgAsmDwarfLabel(nesting_level,offset, overall_offset);
        case DW_TAG_lexical_block:
            return new SgAsmDwarfLexicalBlock(nesting_level,offset, overall_offset);
        case DW_TAG_member:
            return new SgAsmDwarfMember(nesting_level,offset, overall_offset);
        case DW_TAG_pointer_type:
            return new SgAsmDwarfPointerType(nesting_level,offset, overall_offset);
        case DW_TAG_reference_type:
            return new SgAsmDwarfReferenceType(nesting_level,offset, overall_offset);
        case DW_TAG_compile_unit:
            return new SgAsmDwarfCompilationUnit(nesting_level,offset, overall_offset);
        case DW_TAG_string_type:
            return new SgAsmDwarfStringType(nesting_level,offset, overall_offset);
        case DW_TAG_structure_type:
            return new SgAsmDwarfStructureType(nesting_level,offset, overall_offset);
        case DW_TAG_subroutine_type:
            return new SgAsmDwarfSubroutineType(nesting_level,offset, overall_offset);
        case DW_TAG_typedef:
            return new SgAsmDwarfTypedef(nesting_level,offset, overall_offset);
        case DW_TAG_union_type:
            return new SgAsmDwarfUnionType(nesting_level,offset, overall_offset);
        case DW_TAG_unspecified_parameters:
            return new SgAsmDwarfUnspecifiedParameters(nesting_level,offset, overall_offset);
        case DW_TAG_variant:
            return new SgAsmDwarfVariant(nesting_level,offset, overall_offset);
        case DW_TAG_common_block:
            return new SgAsmDwarfCommonBlock(nesting_level,offset, overall_offset);
        case DW_TAG_common_inclusion:
            return new SgAsmDwarfCommonInclusion(nesting_level,offset, overall_offset);
        case DW_TAG_inheritance:
            return new SgAsmDwarfInheritance(nesting_level,offset, overall_offset);
        case DW_TAG_inlined_subroutine:
            return new SgAsmDwarfInlinedSubroutine(nesting_level,offset, overall_offset);
        case DW_TAG_module:
            return new SgAsmDwarfModule(nesting_level,offset, overall_offset);
        case DW_TAG_ptr_to_member_type:
            return new SgAsmDwarfPtrToMemberType(nesting_level,offset, overall_offset);
        case DW_TAG_set_type:
            return new SgAsmDwarfSetType(nesting_level,offset, overall_offset);
        case DW_TAG_subrange_type:
            return new SgAsmDwarfSubrangeType(nesting_level,offset, overall_offset);
        case DW_TAG_with_stmt:
            return new SgAsmDwarfWithStmt(nesting_level,offset, overall_offset);
        case DW_TAG_access_declaration:
            return new SgAsmDwarfAccessDeclaration(nesting_level,offset, overall_offset);
        case DW_TAG_base_type:
            return new SgAsmDwarfBaseType(nesting_level,offset, overall_offset);
        case DW_TAG_catch_block:
            return new SgAsmDwarfCatchBlock(nesting_level,offset, overall_offset);
        case DW_TAG_const_type:
            return new SgAsmDwarfConstType(nesting_level,offset, overall_offset);
        case DW_TAG_constant:
            return new SgAsmDwarfConstant(nesting_level,offset, overall_offset);
        case DW_TAG_enumerator:
            return new SgAsmDwarfEnumerator(nesting_level,offset, overall_offset);
        case DW_TAG_file_type:
            return new SgAsmDwarfFileType(nesting_level,offset, overall_offset);
        case DW_TAG_friend:
            return new SgAsmDwarfFriend(nesting_level,offset, overall_offset);
        case DW_TAG_namelist:
            return new SgAsmDwarfNamelist(nesting_level,offset, overall_offset);
        case DW_TAG_namelist_item:
            return new SgAsmDwarfNamelistItem(nesting_level,offset, overall_offset);
        case DW_TAG_packed_type:
            return new SgAsmDwarfPackedType(nesting_level,offset, overall_offset);
        case DW_TAG_subprogram:
            return new SgAsmDwarfSubprogram(nesting_level,offset, overall_offset);
        case DW_TAG_template_type_parameter:
            return new SgAsmDwarfTemplateTypeParameter(nesting_level,offset, overall_offset);
        case DW_TAG_template_value_parameter:
            return new SgAsmDwarfTemplateValueParameter(nesting_level,offset, overall_offset);
        case DW_TAG_thrown_type:
            return new SgAsmDwarfThrownType(nesting_level,offset, overall_offset);
        case DW_TAG_try_block:
            return new SgAsmDwarfTryBlock(nesting_level,offset, overall_offset);
        case DW_TAG_variant_part:
            return new SgAsmDwarfVariantPart(nesting_level,offset, overall_offset);
        case DW_TAG_variable:
            return new SgAsmDwarfVariable(nesting_level,offset, overall_offset);
        case DW_TAG_volatile_type:
            return new SgAsmDwarfVolatileType(nesting_level,offset, overall_offset);
        case DW_TAG_dwarf_procedure:
            return new SgAsmDwarfDwarfProcedure(nesting_level,offset, overall_offset);
        case DW_TAG_restrict_type:
            return new SgAsmDwarfRestrictType(nesting_level,offset, overall_offset);
        case DW_TAG_interface_type:
            return new SgAsmDwarfInterfaceType(nesting_level,offset, overall_offset);
        case DW_TAG_namespace:
            return new SgAsmDwarfNamespace(nesting_level,offset, overall_offset);
        case DW_TAG_imported_module:
            return new SgAsmDwarfImportedModule(nesting_level,offset, overall_offset);
        case DW_TAG_unspecified_type:
            return new SgAsmDwarfUnspecifiedType(nesting_level,offset, overall_offset);
        case DW_TAG_partial_unit:
            return new SgAsmDwarfPartialUnit(nesting_level,offset, overall_offset);
        case DW_TAG_imported_unit:
            return new SgAsmDwarfImportedUnit(nesting_level,offset, overall_offset);
        case DW_TAG_mutable_type:
            return new SgAsmDwarfMutableType(nesting_level,offset, overall_offset);
        case DW_TAG_condition:
            return new SgAsmDwarfCondition(nesting_level,offset, overall_offset);
        case DW_TAG_shared_type:
            return new SgAsmDwarfSharedType(nesting_level,offset, overall_offset);
        case DW_TAG_format_label:
            return new SgAsmDwarfFormatLabel(nesting_level,offset, overall_offset);
        case DW_TAG_function_template:
            return new SgAsmDwarfFunctionTemplate(nesting_level,offset, overall_offset);
        case DW_TAG_upc_shared_type:
            return new SgAsmDwarfUpcSharedType(nesting_level,offset, overall_offset);
        case DW_TAG_upc_strict_type:
            return new SgAsmDwarfUpcStrictType(nesting_level,offset, overall_offset);
        case DW_TAG_upc_relaxed_type:
            return new  SgAsmDwarfUpcRelaxedType(nesting_level,offset, overall_offset);
        default: {
            // Use AsmDwarfVariant as a default dwarf construct (we might want a SgAsmDwarfUnknownConstruct also).
            return new SgAsmDwarfUnknownConstruct(nesting_level,offset, overall_offset);
        }
    }
#else
    ROSE_UNUSED(tag);
    ROSE_UNUSED(nesting_level);
    ROSE_UNUSED(offset);
    ROSE_UNUSED(overall_offset);
    return nullptr;
#endif
}

std::pair<uint64_t,uint64_t>
SgAsmDwarfLineList::instructionRange() {
    DwarfInstructionSourceMapReturnType maps = buildInstructionAddressSourcePositionMaps();

    SgInstructionAddressSourcePositionMapPtrList & instruction_source_code_map = *(maps.first);

    SgInstructionAddressSourcePositionMapPtrList::iterator lowerBound = instruction_source_code_map.begin();
    SgInstructionAddressSourcePositionMapPtrList::reverse_iterator upperBound = instruction_source_code_map.rbegin();

    return std::pair<uint64_t,uint64_t>(lowerBound->first,upperBound->first);
}

std::pair<LineColumnFilePosition,LineColumnFilePosition>
SgAsmDwarfLineList::sourceCodeRange( int file_id ) {
    DwarfInstructionSourceMapReturnType maps = buildInstructionAddressSourcePositionMaps();

    SgSourcePositionInstructionAddressMapPtrList & source_code_instruction_map = *(maps.second);

    SgSourcePositionInstructionAddressMapPtrList::iterator lowerBound = source_code_instruction_map.begin();
    SgSourcePositionInstructionAddressMapPtrList::reverse_iterator upperBound = source_code_instruction_map.rbegin();

    // Find the first source position in the file specified by the input file_id
    while ( (lowerBound != source_code_instruction_map.end()) && (lowerBound->first.first != file_id) ) {
        lowerBound++;
    }

    while ( (upperBound != source_code_instruction_map.rend()) && (upperBound->first.first != file_id) ) {
        upperBound++;
    }

    LineColumnFilePosition start(lowerBound->first.second);
    LineColumnFilePosition end(upperBound->first.second);

    // Check if this was a case of there not being any entries for this file
    if (lowerBound == source_code_instruction_map.end()) {
        ASSERT_require(upperBound == source_code_instruction_map.rend());
        // printf ("lowerBound == source_code_instruction_map.end() --- no entries for file %d \n",file_id);

        // Reset the line and column information to indicate that there were no entries.
        start = LineColumnFilePosition(std::pair<int,int>(-1,-1));
        end = LineColumnFilePosition(std::pair<int,int>(-1,-1));
    }

    return std::pair<LineColumnFilePosition,LineColumnFilePosition>(start,end);
}


uint64_t
SgAsmDwarfLineList::sourceCodeToAddress ( FileIdLineColumnFilePosition sourcePosition ) {
    // Return the nearest address for the source code position
    int file_id = sourcePosition.first;
    // int line    = sourcePosition.second.first;
    // int column  = sourcePosition.second.second;

    uint64_t returnAddress = 0;

    DwarfInstructionSourceMapReturnType maps = buildInstructionAddressSourcePositionMaps();
    SgSourcePositionInstructionAddressMapPtrList & source_code_instruction_map = *(maps.second);

    SgSourcePositionInstructionAddressMapPtrList::iterator lowerBound = source_code_instruction_map.lower_bound(sourcePosition);

    returnAddress = lowerBound->second;

    if (lowerBound == source_code_instruction_map.begin()) {
        // printf ("lowerBound == source_code_instruction_map.begin() \n");
        if (lowerBound->first.first != file_id) {
            // printf ("This source position is not from a valide file in the map: file_id = %d lowerBound->first = %d \n",file_id,lowerBound->first.first);
            returnAddress = 0;
        }
    }

    if (lowerBound == source_code_instruction_map.end()) {
        // printf ("lowerBound == source_code_instruction_map.end() \n");
#if 0
        if (lowerBound->first.first != file_id)
            {
                printf ("This source position is not from a valide file in the map: file_id = %d lowerBound->first = %d \n",file_id,lowerBound->first.first);
                returnAddress = NULL;
            }
        else
            {
                returnAddress = NULL;
            }
#else
        returnAddress = 0;
#endif
    }
#if 0
    // I think this is redundant code
    if (upperBound == source_code_instruction_map.end())
        {
            // printf ("upperBound == source_code_instruction_map.end() \n");
            returnAddress = NULL;
        }
#endif

    return returnAddress;
}

FileIdLineColumnFilePosition
SgAsmDwarfLineList::addressToSourceCode ( uint64_t address ) {
    // Set to default value
    FileIdLineColumnFilePosition sourcePosition(0,std::pair<int,int>(0,0));

    std::pair<uint64_t,uint64_t> validInstructionRange = instructionRange();
    if ( (address < validInstructionRange.first) || (address > validInstructionRange.second) ) {
        // printf ("Address out of range: address = 0x%lx  range (0x%lx, 0x%lx) \n",address,validInstructionRange.first,validInstructionRange.second);

        // Set to error value
        sourcePosition = FileIdLineColumnFilePosition(-1,std::pair<int,int>(-1,-1));
    } else {
        DwarfInstructionSourceMapReturnType maps = buildInstructionAddressSourcePositionMaps();
        SgInstructionAddressSourcePositionMapPtrList & instruction_source_code_map = *(maps.first);

        SgInstructionAddressSourcePositionMapPtrList::iterator lowerBound = instruction_source_code_map.lower_bound(address);

        // Set the the lower bound found in the map
        sourcePosition = lowerBound->second;

#if 0
        int file_id = sourcePosition.first;
        int line    = sourcePosition.second.first;
        int column  = sourcePosition.second.second;
        string filename = Sg_File_Info::getFilenameFromID(file_id);

        // printf ("address = 0x%lx maps to source position (file = %d = %s, line = %d, column = %d) \n",address,file_id,filename.c_str(),line,column);
#endif
    }

    // return FileIdLineColumnFilePosition(-1,std::pair<int,int>(-1,-1));
    return sourcePosition;
}


void
SgAsmDwarfLineList::display( const std::string & label ) { // blame 8e7b5bd1d0
    // Note that once the maps are setup NULL is an acceptable value (perhaps it should be the default parameter!)
    DwarfInstructionSourceMapReturnType maps = buildInstructionAddressSourcePositionMaps();

    // Output the SgInstructionAddressSourcePositionMapPtrList map so the we can test the linear ordering of the addresses.
    SgInstructionAddressSourcePositionMapPtrList & instruction_source_code_map = *(maps.first);
    SgSourcePositionInstructionAddressMapPtrList & source_code_instruction_map = *(maps.second);

    std::pair<uint64_t,uint64_t> addressRange = instructionRange();
    printf ("addressRange = (0x%" PRIx64 ", 0x%" PRIx64 ") \n",addressRange.first,addressRange.second);

    // Iterate over all the files in the static Sg_File_Info::get_fileidtoname_map
    int numberOfSourceFiles = Sg_File_Info::numberOfSourceFiles();
    printf ("numberOfSourceFiles = %d \n",numberOfSourceFiles);

    // DQ: I think that the initial value is 1 not 0!
    for (int i=1; i < numberOfSourceFiles; i++) {
        std::pair<LineColumnFilePosition,LineColumnFilePosition> sourceFileRange = sourceCodeRange( i );

        std::string filename = Sg_File_Info::getFilenameFromID(i);

        if ( (sourceFileRange.first.first < 0) && (sourceFileRange.second.first < 0) ) {
            printf ("This file_id = %d is not a valid source file: filename = %s \n",i,filename.c_str());
        } else {
            printf ("Source range for file = %s (id = %d) [(line=%d, col=%d), (line=%d, col=%d)] \n",
                    filename.c_str(),i,
                    sourceFileRange.first.first, sourceFileRange.first.second,
                    sourceFileRange.second.first, sourceFileRange.second.second);
        }
    }


    printf ("\n\nTest sourceCodeToAddress: \n");
    FileIdLineColumnFilePosition s1(2,std::pair<int,int>(10,-1));
    uint64_t instructionAddress1 = sourceCodeToAddress(s1);
    printf ("sourceCodeToAddress(%d,%d,%d) = 0x%" PRIx64 "\n", s1.first,s1.second.first,s1.second.second,instructionAddress1);

    FileIdLineColumnFilePosition s2(2,std::pair<int,int>(11,-1));
    uint64_t instructionAddress2 = sourceCodeToAddress(s2);
    printf ("sourceCodeToAddress(%d,%d,%d) = 0x%" PRIx64 "\n",s2.first,s2.second.first,s2.second.second,instructionAddress2);

    FileIdLineColumnFilePosition s3(1,std::pair<int,int>(11,-1));
    uint64_t instructionAddress3 = sourceCodeToAddress(s3);
    printf ("sourceCodeToAddress(%d,%d,%d) = 0x%" PRIx64 "\n",s3.first,s3.second.first,s3.second.second,instructionAddress3);

    for (int fileNumber = 1; fileNumber < 4; fileNumber++) {
        for (int lineNumber = -2; lineNumber < 35; lineNumber++) {
            for (int columnNumber = -2; columnNumber < 1; columnNumber++) {
                FileIdLineColumnFilePosition s(fileNumber,std::pair<int,int>(lineNumber,columnNumber));
                uint64_t instructionAddress = sourceCodeToAddress(s);
                printf ("sourceCodeToAddress(%d,%d,%d) = 0x%" PRIx64 "\n",
                        s.first,s.second.first,s.second.second,instructionAddress);
            }
        }
    }

    printf ("\n\nTest addressToSourceCode: (not tested yet) \n");

    FileIdLineColumnFilePosition s1map = addressToSourceCode(instructionAddress1);
    printf ("addressToSourceCode: address 0x%" PRIx64 " = (%d,%d,%d)\n",
            instructionAddress1,s1map.first,s1map.second.first,s1map.second.second);

    for (uint64_t address = instructionAddress1-15; address < instructionAddress1+ 85; address++) {
        FileIdLineColumnFilePosition s_map = addressToSourceCode(address);
        printf ("addressToSourceCode: address 0x%" PRIx64 " = (%d,%d,%d)\n",
                address,s_map.first,s_map.second.first,s_map.second.second);
    }

    printf ("\nOutput entries in instruction_source_code_map \n");
    SgInstructionAddressSourcePositionMapPtrList::iterator it1 = instruction_source_code_map.begin();
    while ( it1 != instruction_source_code_map.end() ) {
        uint64_t address = it1->first;

        // This is a std::map<uint64_t,std::pair<int,std::pair<int,int> > >, so we get
        //    "it->second.first, it->second.second.first, it->second.second.second"
        // for the last three terms.
        int file_id = it1->second.first;
        int line    = it1->second.second.first;
        int column  = it1->second.second.second;

        printf ("instruction_source_code_map[0x%" PRIx64 "] = (file=%d, line=%d, col=%d)\n", address, file_id, line, column);

        // A test of the evaluation of ranges of lines for each instruction
        SgInstructionAddressSourcePositionMapPtrList::iterator it1_lb = instruction_source_code_map.lower_bound(address);
        SgInstructionAddressSourcePositionMapPtrList::iterator it1_ub = instruction_source_code_map.upper_bound(address);

        if (it1_lb != it1_ub) {
            if (it1_ub != instruction_source_code_map.end()) {
                printf ("   ----- range = [(file=%d, line=%d, col=%d), (file=%d, line=%d, col=%d)) \n",
                        it1_lb->second.first, it1_lb->second.second.first, it1_lb->second.second.second,
                        it1_ub->second.first, it1_ub->second.second.first, it1_ub->second.second.second);
            } else {
                printf ("   ----- range = [(file=%d, line=%d, col=%d), last_source_position) \n",
                        it1_lb->second.first, it1_lb->second.second.first, it1_lb->second.second.second);
            }
        }

        it1++;
    }

    printf ("\nOutput entries in source_code_instruction_map \n");
    SgSourcePositionInstructionAddressMapPtrList::iterator it2 = source_code_instruction_map.begin();
    while ( it2 != source_code_instruction_map.end() ) {
        uint64_t address = it2->second;

        // This is a std::map<uint64_t,std::pair<int,std::pair<int,int> > >, so we get
        //    "it->second.first, it->second.second.first, it->second.second.second"
        // for the last three terms.
        int file_id = it2->first.first;
        int line    = it2->first.second.first;
        int column  = it2->first.second.second;

        printf ("source_code_instruction_map[file=%d, line=%d, col=%d] = 0x%" PRIx64 "\n", file_id, line, column, address);

        // A test of the evaluation of ranges of instructions for each line of source code.
        FileIdLineColumnFilePosition file_info(file_id,std::pair<int,int>(line,column));
        SgSourcePositionInstructionAddressMapPtrList::iterator it2_lb = source_code_instruction_map.lower_bound(file_info);
        SgSourcePositionInstructionAddressMapPtrList::iterator it2_ub = source_code_instruction_map.upper_bound(file_info);

        if (it2_lb != it2_ub) {
            if (it2_ub != source_code_instruction_map.end()) {
                printf ("   ----- range = [0x%" PRIx64 ", 0x%" PRIx64 ")\n", it2_lb->second,it2_ub->second);
            } else {
                printf ("   ----- range = [0x%" PRIx64 ", last_instruction)\n", it2_lb->second);
            }
        }

        it2++;
    }
}

#endif
