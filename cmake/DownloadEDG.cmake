include(ExternalProject)

string(REPLACE "." "-" Local_EDG_Version "${EDG_VERSION}")

# Detect platform
set(platform "")
if(APPLE)
  set(platform "i686-apple-darwin")
elseif(UNIX)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(platform "x86_64-pc-linux-gnu")
  else()
    set(platform "i686-pc-linux-gnu")
  endif()
endif()
if(platform STREQUAL "")
  message(FATAL_ERROR
    "Unable to detect platform when attempting to download EDG binary tarball")
endif()

# Detect compiler by asking GCC what version it is
set(compiler "")
set(min_supported "4.0")
set(max_supported "4.4")
execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                OUTPUT_VARIABLE GCC_version)
# strip patch version; we only care about major & minor
string(REGEX MATCH "([0-9]\\.[0-9])" GCC_version ${GCC_version})

if(GCC_version VERSION_GREATER max_supported OR
   GCC_version VERSION_LESS min_supported)
  message(FATAL_ERROR
    "ROSE only supports GCC versions ${min_supported} to ${max_supported}.\n"
    "<gcc -dumpversion> reported ${GCC_version}")
endif()
if(GCC_version VERSION_EQUAL 4.4)
  set(compiler "GNU-4.4")
elseif(GCC_version VERSION_EQUAL 4.3)
  set(compiler "GNU-4.3")
elseif(GCC_version VERSION_EQUAL 4.2)
  set(compiler "GNU-4.2")
elseif(GCC_version VERSION_EQUAL 4.1)
  set(compiler "GNU-4.1")
elseif(GCC_version VERSION_EQUAL 4.0)
  set(compiler "GNU-4.0")
endif()
if(compiler STREQUAL "")
  message(FATAL_ERROR
    "Unable to detect a supported compiler when attempting to download EDG binary tarball")
endif()

# Get binary compatibility signature
execute_process(
  COMMAND "${PROJECT_SOURCE_DIR}/scripts/bincompat-sig"
  OUTPUT_VARIABLE signature)
string(STRIP ${signature} signature)

set(tarball_site "http://www.rosecompiler.org/edg_binaries")
set(tarball_filename "roseBinaryEDG-${Local_EDG_Version}-${platform}-${compiler}-${signature}.tar.gz")

ExternalProject_Add("EDG_tarball"
  URL ${tarball_site}/${tarball_filename}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend/EDG
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  )

add_library(edg33 STATIC IMPORTED)
set_property(TARGET edg33 PROPERTY IMPORTED_LOCATION
  ${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend/EDG/.libs/libroseEDG.a)
