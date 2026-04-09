find_program(LCOV_EXECUTABLE NAMES lcov REQUIRED)
find_program(GENHTML_EXECUTABLE NAMES genhtml REQUIRED)

#Get all directories in the build tree
file(GLOB_RECURSE all_paths LIST_DIRECTORIES true "${BUILD_DIR}/*")

set(dir_args "")
foreach(path IN LISTS all_paths)
    if(IS_DIRECTORY "${path}")
    list(APPEND dir_args -d "${path}")
    endif()
endforeach()

#Call lcov on all the directories
execute_process(COMMAND "${LCOV_EXECUTABLE}" --capture --no-recursion ${dir_args} -o all-lcov.info RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "Initial lcov capture failed with exit code ${rc}")
endif()

#Remove some directories that we don't want to track
execute_process(COMMAND "${LCOV_EXECUTABLE}" --extract all-lcov.info "${SOURCE_DIR}/src/*" -o librose-lcov.info RESULT_VARIABLE rc)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "lcov source extraction failed with exit code ${rc}")
endif()

execute_process(COMMAND "${LCOV_EXECUTABLE}" --remove librose-lcov.info
    "${SOURCE_DIR}/src/frontend/CxxFrontend/EDG/edgRose/debugging.*"
    "${SOURCE_DIR}/src/frontend/CxxFrontend/EDG/edgRose/edgGraph.*"
    "${SOURCE_DIR}/src/backend/unparser/PythonCodeGeneration/*" 
    "${SOURCE_DIR}/src/backend/unparser/PHPCodeGeneration/*"
    "${SOURCE_DIR}/src/midend/programAnalysis/OAWrap/*"
    "${SOURCE_DIR}/src/midend/programAnalysis/OpenAnalysis/*"
    "${SOURCE_DIR}/src/midend/programAnalysis/genericDataflow/*"
    "${SOURCE_DIR}/src/frontend/CxxFrontend/EDG/EDG_6.5/*"
    "${SOURCE_DIR}/src/midend/astRewriteMechanism/*"
    "${SOURCE_DIR}/src/midend/programAnalysis/dominanceAnalysis/*"
    "${SOURCE_DIR}/src/midend/programAnalysis/pointerAnal/*"
    "${SOURCE_DIR}/src/midend/programAnalysis/ssaUnfilteredCfg/*"
    "${SOURCE_DIR}/src/src/midend/programAnalysis/staticInterproceduralSlicing/*"
    "${SOURCE_DIR}/src/midend/programAnalysis/staticSingleAssignment/*"
    "${SOURCE_DIR}/src/midend/programAnalysis/variableRenaming/*"
    "${SOURCE_DIR}/src/midend/programTransformation/extractFunctionArgumentsNormalization/*"
    "${SOURCE_DIR}/src/midend/programTransformation/finiteDifferencing/*"
    "${SOURCE_DIR}/src/midend/programTransformation/functionCallNormalization/*"
    "${SOURCE_DIR}/src/midend/programTransformation/implicitCodeGeneration/*"
    "${SOURCE_DIR}/src/midend/programTransformation/singleStatementToBlockNormalization/*"
    "${SOURCE_DIR}/src/roseSupport/stringify.C"
    "${SOURCE_DIR}/src/roseExtensions/roseHPCToolkit/*"
    "${SOURCE_DIR}/src/frontend/SageIII/nodeBuildFunctionsForAterms.C"
    "${SOURCE_DIR}/src/frontend/SageIII/astVisualization/*"
    "${SOURCE_DIR}/src/midend/astProcessing/AstProcessing.h"
    "${SOURCE_DIR}/src/frontend/SageIII/sageInterface/sageGeneric.h"
    "${SOURCE_DIR}/src/3rdPartyLibraries/json*"
    "${SOURCE_DIR}/src/roseSupport/*"
    -o librose-lcov.info
    RESULT_VARIABLE rc
)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "lcov source trim failed with exit code ${rc}")
endif()

#Use genhtml to create the webpages
execute_process(COMMAND "${GENHTML_EXECUTABLE}"
    --title librose
    --legend
    --demangle-cpp
    --rc genhtml_hi_limit=85
    --rc genhtml_med_limit=60
    --prefix ${SOURCE_DIR}
    --ignore-errors=source
    --output-directory librose_lcov_web_pages
    librose-lcov.info
    RESULT_VARIABLE rc
)
if(NOT rc EQUAL 0)
    message(FATAL_ERROR "genhtml failed with exit code ${rc}")
endif()
