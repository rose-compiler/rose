
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

# Use this as a way to turn off this optional use to build smaller files
# so that we test this unter Linux before worring about use within Windows.
# tps (01/04/10) : Added name for if condition
message ( STATUS ">>>>>>>>>>>>>>>>>>>>>> smallerGeneratedFiles : " ${enable-smaller-generated-files} )
if ( enable-smaller-generated-files  )

########### next target ###############
set( libgeneratedIR_SRC
     SgAccessModifier.C
     SgAsmDwarfRestrictType.C
#     SgTest.C
   )


message ( STATUS ">>>>>>>>>>>>>>>>>>>>>> CxxGrammarMetaProgram_SRCS : " ${CxxGrammarMetaProgram_SRCS} )
#add_executable(CxxGrammarMetaProgram2 ${CxxGrammarMetaProgram_SRCS} ${rose_util_src})
#target_link_libraries(CxxGrammarMetaProgram2 ${KDE4_KDECORE_LIBS} rose_util_lib ${Boost_LIBRARIES})
get_target_property (META_EXE_LOC CxxGrammarMetaProgram LOCATION) 

# define a custom command to call the code generator and generate source/header files
# Generate the edg-sage connection using ROSETTA
add_custom_command(
   OUTPUT ${libgeneratedIR_SRC}
#  depend on the code generator and input files
   DEPENDS  CxxGrammarMetaProgram ${GRAMMAR_SOURCE_FILES}
#  We need to copy the input files  to the build tree since 
#  the code generator will output the generated source files into the same directory as where the input files locate
   COMMAND
       ${CMAKE_COMMAND}  -E copy_directory  ${ROSE_TOP_SRC_DIR}/src/ROSETTA/Grammar ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/GENERATED_CODE_DIRECTORY_Cxx_Grammar
#   create this directory is needed since a few generated files will be put there        
   COMMAND
      ${CMAKE_COMMAND}  -E make_directory  ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/GENERATED_CODE_DIRECTORY_Cxx_Grammar
   COMMAND ${META_EXE_LOC}
      ARGS ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/GENERATED_CODE_DIRECTORY_Cxx_Grammar
)


# a custom target depending on generated rosetta source and header files to trigger the building process
#add_custom_target( libgeneratedIR_SRC ALL 
#    )






add_library( libgeneratedIR STATIC ${libgeneratedIR_SRC} )
add_dependencies( libgeneratedIR ROSETTA)

########### install files ###############

# Need to define a mechanism to collect a list of all files (later).
install(FILES  SgAccessModifier.h SgAsmDwarfRestrictType.h DESTINATION ${INCLUDE_INSTALL_DIR})



endif ( enable-smaller-generated-files )
