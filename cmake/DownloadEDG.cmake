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
set(min_supported "4.4")
set(max_supported "5.2")
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
if(GCC_version VERSION_EQUAL 5.2)
  set(compiler "gnu-5.2")
elseif(GCC_version VERSION_EQUAL 5.1)
  set(compiler "gnu-5.1")
elseif(GCC_version VERSION_EQUAL 5.0)
  set(compiler "gnu-5.0")
elseif(GCC_version VERSION_EQUAL 4.9)
  set(compiler "gnu-4.9")
elseif(GCC_version VERSION_EQUAL 4.8)
  set(compiler "gnu-4.8")
elseif(GCC_version VERSION_EQUAL 4.7)
  set(compiler "gnu-4.7")
elseif(GCC_version VERSION_EQUAL 4.6)
  set(compiler "gnu-4.6")
elseif(GCC_version VERSION_EQUAL 4.5)
  set(compiler "gnu-4.5")
elseif(GCC_version VERSION_EQUAL 4.4)
  set(compiler "gnu-4.4")
endif()
if(compiler STREQUAL "")
  message(FATAL_ERROR
    "Unable to detect a supported compiler when attempting to download EDG binary tarball")
endif()

# Get binary compatibility signature
set(EDG_SIG_OUTPUT
  "${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend/edg-generate-sig.output"
)
#ADD_CUSTOM_COMMAND(TARGET EDG_tarball
#  PRE_BUILD
#  COMMAND "${CMAKE_SOURCE_DIR}/scripts/edg-generate-sig" "${CMAKE_SOURCE_DIR}" " ${CMAKE_BINARY_DIR}" " >" ${EDG_SIG_OUTPUT}
#  COMMAND "cat" ${EDG_SIG_OUTPUT}  ">" signature
#  DEPENDS roseutil rosetta_generated "${CMAKE_SOURCE_DIR}/scripts/edg-generate-sig"
#)
add_custom_target(get_EDG_name
#  COMMENT "EDG NAME: ${CMAKE_SOURCE_DIR}/scripts/edg-generate-sig ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
#    > ${EDG_SIG_OUTPUT}"
  COMMAND ${CMAKE_SOURCE_DIR}/scripts/edg-generate-sig ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    > ${EDG_SIG_OUTPUT}
  DEPENDS roseutil rosetta_generated)
#set(signature "")

add_custom_target( EDGSignature
#    COMMENT "EDGSignature arguments ${EDG_SIG_OUTPUT} ${Local_EDG_Version} ${platform} ${compiler}"
    COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/cmake/ProcessEDGBinary.cmake ${EDG_SIG_OUTPUT} ${Local_EDG_Version} ${platform} ${compiler} 
    DEPENDS get_EDG_name
)

#set(tarball_site "http://www.rosecompiler.org/edg_binaries")
#set(tarball_filename "roseBinaryEDG-${Local_EDG_Version}-${platform}-${compiler}-${signature}.tar.gz")

#add_custom_target(EDG_tarball
#COMMAND wget ${tarball_site}/roseBinaryEDG-${Local_EDG_Version}-${platform}-${compiler}-${signature}.tar.gz ${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend/EDG.tar.gz
#COMMENT "Untar EDG"
#COMMAND tar zxvf ${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend/EDG.tar.gz -C ${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend 
#DEPENDS EDGSignature
#)
#ExternalProject_Add(EDG_tarball
#  COMMENT "EDG signature = ${signature}"
#  URL ${tarball_site}/roseBinaryEDG-${Local_EDG_Version}-${platform}-${compiler}-${signature}.tar.gz
#  SOURCE_DIR ${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend/EDG
#  CONFIGURE_COMMAND ""
#  BUILD_COMMAND ""
#  INSTALL_COMMAND ""
#  DEPENDS EDGSignature 
#  )

add_library(EDG STATIC IMPORTED)
add_dependencies(EDG EDGSignature)
set_property(TARGET EDG PROPERTY IMPORTED_LOCATION
  ${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend/EDG/.libs/libroseEDG.a)

