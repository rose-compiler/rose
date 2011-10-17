# - Find Git
# Find a Git SCM client.
#
#   GIT_FOUND      - True if Git was found.
#   GIT_EXECUTABLE - Path to the git binary.
#   GIT_VERSION    - Version of found git binary.
#
# If Git is found, then following function(s) are defined:
#
#   GIT_TREE_INFO( DIR PREFIX )
#       Extracts info from given tree.
#       Arguments:
#           DIR    - Where the git tree is.
#           PREFIX - A prefix for defined variables.
#       Defined variables:
#           ${PREFIX}_HASH       - A hash of HEAD revision.
#           ${PREFIX}_HASH_SHORT - An abbreviation of the hash.
#

IF( GIT_EXECUTABLE )
    # Already in cache, be silent
    SET( Git_FIND_QUIETLY TRUE )
ENDIF( GIT_EXECUTABLE )

FIND_PROGRAM( GIT_EXECUTABLE "git"
              DOC "Path to the git binary." )

# handle the QUIETLY and REQUIRED arguments and set GIT_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE( "FindPackageHandleStandardArgs" )
FIND_PACKAGE_HANDLE_STANDARD_ARGS( "Git" DEFAULT_MSG GIT_EXECUTABLE )

MARK_AS_ADVANCED( GIT_EXECUTABLE GIT_VERSION )

IF( GIT_FOUND )
    EXECUTE_PROCESS( COMMAND "${GIT_EXECUTABLE}" --version
                     OUTPUT_VARIABLE GIT_VERSION
                     OUTPUT_STRIP_TRAILING_WHITESPACE )
    STRING( REPLACE "git version " "" GIT_VERSION
                    "${GIT_VERSION}" )

    FUNCTION( GIT_TREE_INFO DIR PREFIX )
        EXECUTE_PROCESS( COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
                         WORKING_DIRECTORY "${DIR}"
                         RESULT_VARIABLE   GIT_RESULT
                         OUTPUT_VARIABLE   "${PREFIX}_HASH"
                         ERROR_VARIABLE    GIT_ERROR
                         OUTPUT_STRIP_TRAILING_WHITESPACE )

        IF( NOT ${GIT_RESULT} EQUAL 0 )
            MESSAGE( SEND_ERROR "Command '${GIT_EXECUTABLE} rev-parse HEAD' failed with following output:\n${GIT_ERROR}" )
        ENDIF( NOT ${GIT_RESULT} EQUAL 0 )

        EXECUTE_PROCESS( COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
                         WORKING_DIRECTORY "${DIR}"
                         RESULT_VARIABLE   GIT_RESULT
                         OUTPUT_VARIABLE   "${PREFIX}_HASH_SHORT"
                         ERROR_VARIABLE    GIT_ERROR
                         OUTPUT_STRIP_TRAILING_WHITESPACE )

        IF( NOT ${GIT_RESULT} EQUAL 0 )
            MESSAGE( SEND_ERROR "Command '${GIT_EXECUTABLE} rev-parse --short HEAD' failed with following output:\n${GIT_ERROR}" )
        ENDIF( NOT ${GIT_RESULT} EQUAL 0 )

        SET( "${PREFIX}_HASH"       "${${PREFIX}_HASH}"       PARENT_SCOPE )
        SET( "${PREFIX}_HASH_SHORT" "${${PREFIX}_HASH_SHORT}" PARENT_SCOPE )
    ENDFUNCTION( GIT_TREE_INFO )
ENDIF( GIT_FOUND )
