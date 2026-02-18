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

#define DEBUG_DWARF_CONSTRUCT 0

 // DQ (11/19/2025): Modified function to have a single return.
    SgAsmDwarfConstruct* result = NULL;

    switch (tag) {
        case DW_TAG_array_type:
            result = new SgAsmDwarfArrayType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_class_type:
            result = new SgAsmDwarfClassType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_entry_point:
            result = new SgAsmDwarfEntryPoint(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_enumeration_type:
            result = new SgAsmDwarfEnumerationType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_formal_parameter:
            result = new SgAsmDwarfFormalParameter(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_imported_declaration:
            result = new SgAsmDwarfImportedDeclaration(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_label:
            result = new SgAsmDwarfLabel(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_lexical_block:
            result = new SgAsmDwarfLexicalBlock(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_member:
            result = new SgAsmDwarfMember(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_pointer_type:
            result = new SgAsmDwarfPointerType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_reference_type:
            result = new SgAsmDwarfReferenceType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_compile_unit:
            result = new SgAsmDwarfCompilationUnit(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_string_type:
            result = new SgAsmDwarfStringType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_structure_type:
            result = new SgAsmDwarfStructureType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_subroutine_type:
            result = new SgAsmDwarfSubroutineType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_typedef:
            result = new SgAsmDwarfTypedef(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_union_type:
            result = new SgAsmDwarfUnionType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_unspecified_parameters:
            result = new SgAsmDwarfUnspecifiedParameters(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_variant:
            result = new SgAsmDwarfVariant(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_common_block:
            result = new SgAsmDwarfCommonBlock(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_common_inclusion:
            result = new SgAsmDwarfCommonInclusion(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_inheritance:
            result = new SgAsmDwarfInheritance(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_inlined_subroutine:
            result = new SgAsmDwarfInlinedSubroutine(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_module:
            result = new SgAsmDwarfModule(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_ptr_to_member_type:
            result = new SgAsmDwarfPtrToMemberType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_set_type:
            result = new SgAsmDwarfSetType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_subrange_type:
            result = new SgAsmDwarfSubrangeType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_with_stmt:
            result = new SgAsmDwarfWithStmt(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_access_declaration:
            result = new SgAsmDwarfAccessDeclaration(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_base_type:
            result = new SgAsmDwarfBaseType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_catch_block:
            result = new SgAsmDwarfCatchBlock(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_const_type:
            result = new SgAsmDwarfConstType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_constant:
            result = new SgAsmDwarfConstant(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_enumerator:
            result = new SgAsmDwarfEnumerator(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_file_type:
            result = new SgAsmDwarfFileType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_friend:
            result = new SgAsmDwarfFriend(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_namelist:
            result = new SgAsmDwarfNamelist(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_namelist_item:
            result = new SgAsmDwarfNamelistItem(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_packed_type:
            result = new SgAsmDwarfPackedType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_subprogram:
            result = new SgAsmDwarfSubprogram(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_template_type_parameter:
            result = new SgAsmDwarfTemplateTypeParameter(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_template_value_parameter:
            result = new SgAsmDwarfTemplateValueParameter(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_thrown_type:
            result = new SgAsmDwarfThrownType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_try_block:
            result = new SgAsmDwarfTryBlock(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_variant_part:
            result = new SgAsmDwarfVariantPart(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_variable:
            result = new SgAsmDwarfVariable(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_volatile_type:
            result = new SgAsmDwarfVolatileType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_dwarf_procedure:
            result = new SgAsmDwarfDwarfProcedure(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_restrict_type:
            result = new SgAsmDwarfRestrictType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_interface_type:
            result = new SgAsmDwarfInterfaceType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_namespace:
            result = new SgAsmDwarfNamespace(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_imported_module:
            result = new SgAsmDwarfImportedModule(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_unspecified_type:
            result = new SgAsmDwarfUnspecifiedType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_partial_unit:
            result = new SgAsmDwarfPartialUnit(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_imported_unit:
            result = new SgAsmDwarfImportedUnit(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_mutable_type:
            result = new SgAsmDwarfMutableType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_condition:
            result = new SgAsmDwarfCondition(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_shared_type:
            result = new SgAsmDwarfSharedType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_format_label:
            result = new SgAsmDwarfFormatLabel(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_function_template:
            result = new SgAsmDwarfFunctionTemplate(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_upc_shared_type:
            result = new SgAsmDwarfUpcSharedType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_upc_strict_type:
            result = new SgAsmDwarfUpcStrictType(nesting_level,offset, overall_offset);
            break;
        case DW_TAG_upc_relaxed_type:
            result = new  SgAsmDwarfUpcRelaxedType(nesting_level,offset, overall_offset);
            break;

        default: {
            // Use AsmDwarfVariant as a default dwarf construct (we might want a SgAsmDwarfUnknownConstruct also).
            result = new SgAsmDwarfUnknownConstruct(nesting_level,offset, overall_offset);
            break;
        }
    }

    ROSE_ASSERT(result != NULL);

#if DEBUG_DWARF_CONSTRUCT
    printf ("############ In createDwarfConstruct(tag == %d): result = %p = %s ############ \n",tag,result,result->class_name().c_str());
#endif

    return result;
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

#if 1

// DQ (11/28/2025): This is the more efficent ROSE version of the support for source position from addresses.
FileIdLineColumnFilePosition
SgAsmDwarfLineList::addressToSourceCode ( uint64_t address )
   {
  // Set to default values
     FileIdLineColumnFilePosition sourcePosition(0,std::pair<int,int>(0,0));

     std::pair<uint64_t,uint64_t> validInstructionRange = instructionRange();
     if ( (address < validInstructionRange.first) || (address > validInstructionRange.second) )
        {
       // printf ("Address out of range: address = 0x%lx  range (0x%lx, 0x%lx) \n",address,validInstructionRange.first,validInstructionRange.second);

       // Set to error value
          sourcePosition = FileIdLineColumnFilePosition(-1,std::pair<int,int>(-1,-1));
        }
       else
        {
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

#else

// DQ (11/28/2025): Alternative implementation accessing libdwarf support directly for
// converting an address to source code position.  This might be a better idea
// than implementing the support in ROSE specific data structures, however the
// previous ROSE design might be faster. Since after the update to Dwarf 4, the ROSE
// version now works fine, we will use the more computationally efficent ROSE version.
// This can be a backup bersion, however we would ahve to take step to support
// Dwarf_Debug and Dwarf_Die (saving references to them in read_dwarf() and extending
// their lifetimes; there are a few optional approaches here).

FileIdLineColumnFilePosition
SgAsmDwarfLineList::addressToSourceCode(uint64_t address)
   {
  // Default error value
     FileIdLineColumnFilePosition sourcePosition(-1, std::pair<int,int>(-1,-1));

#ifdef ROSE_HAVE_LIBDWARF  
  // Get the DWARF debug handle from the compilation unit  
  // Note: You'll need to pass or store the Dwarf_Debug handle  
     Dwarf_Debug dbg = /* get from context */;  
     Dwarf_Die cu_die = /* get compilation unit DIE */;  
     Dwarf_Error error;

  // Get the line context for this compilation unit
     Dwarf_Unsigned version = 0;
     Dwarf_Small tableCount = 0;
     Dwarf_Line_Context lineContext;
     int res = dwarf_srclines_b(cu_die, &version, &tableCount, &lineContext, &error);
     if (res != DW_DLV_OK)
        {
          return sourcePosition;
        }

  // Get all lines from the line context
     Dwarf_Line *lines = nullptr;
     Dwarf_Signed lineCount = 0;
     res = dwarf_srclines_from_linecontext(lineContext, &lines, &lineCount, &error);
     if (res != DW_DLV_OK)
        {
          dwarf_srclines_dealloc_b(lineContext);
          return sourcePosition;
        }

  // Binary search or linear search for the address  
     Dwarf_Addr bestAddr = 0;
     int bestIndex = -1;

     for (Dwarf_Signed i = 0; i < lineCount; ++i)
        {
          Dwarf_Addr lineAddr = 0;
          if (dwarf_lineaddr(lines[i], &lineAddr, &error) == DW_DLV_OK)
             {
            // Find the line with address <= target address
               if (lineAddr <= address && lineAddr >= bestAddr)
                  {
                    bestAddr = lineAddr;
                    bestIndex = i;
                 }
             }
        }

     if (bestIndex >= 0)
        {
       // Extract file name and convert to file ID
          char *fileName = nullptr;
          if (dwarf_linesrc(lines[bestIndex], &fileName, &error) == DW_DLV_OK)
             {
               int fileId = Sg_File_Info::addFilenameToMap(fileName);
               sourcePosition.first = fileId;
             }

       // Extract line number
          Dwarf_Unsigned lineNumber = 0;
          if (dwarf_lineno(lines[bestIndex], &lineNumber, &error) == DW_DLV_OK)
             {
               sourcePosition.second.first = lineNumber;
             }

       // Extract column number
          Dwarf_Unsigned columnNumber = 0;
          if (dwarf_lineoff_b(lines[bestIndex], &columnNumber, &error) == DW_DLV_OK)
            {
              sourcePosition.second.second = columnNumber;
            }
        }

     dwarf_srclines_dealloc_b(lineContext);
#endif

     return sourcePosition;  
   }

#endif

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
