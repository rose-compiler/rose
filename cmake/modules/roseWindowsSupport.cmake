# define rose_config.h variables

if (MSVC_VERSION STREQUAL "1200")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 6)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 0)
elseif (MSVC_VERSION STREQUAL "1300")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 7)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 0)
elseif (MSVC_VERSION STREQUAL "1310")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 7)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 1)
elseif (MSVC_VERSION STREQUAL "1400")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 8)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 0)
elseif (MSVC_VERSION STREQUAL "1500")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 9)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 0)
elseif (MSVC_VERSION STREQUAL "1600")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 10)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 0)
elseif (MSVC_VERSION STREQUAL "1700")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 11)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 0)
elseif (MSVC_VERSION STREQUAL "1800")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 12)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 0)
elseif (MSVC_VERSION STREQUAL "1900")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 15)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 0)
elseif (MSVC_VERSION STREQUAL "1915")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 15)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 8)
elseif (MSVC_VERSION STREQUAL "1916")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER 15)
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER 9)
else()
  message(FATAL_ERROR "Unable to extract major & minor version from MSVC_VERSION ${MSVC_VERSION}")
endif()

get_filename_component(
  BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH
  "${CMAKE_CXX_COMPILER}"
  NAME)

# For Visual Studio, the C & CXX compilers are the same.
set(BACKEND_C_COMPILER_NAME_WITHOUT_PATH ${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH})
set(BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER ${BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER})
set(BACKEND_C_COMPILER_MINOR_VERSION_NUMBER ${BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER})

# TODO:
#BACKEND_JAVA_COMPILER
#BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH

# list of registry keys to help us find where VS2008 is installed
set(vs9_registry_keys
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VC;ProductDir]"
  # English SP1
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\InstalledProducts\\KB948484;]"
  # Team System English SP1
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\InstalledProducts\\KB947888;]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VCExpress\\9.0\\Setup\\VC;ProductDir]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows\\v6.0A;InstallationFolder]"
)

# Because it takes a while, we only copy system headers the first time.
# You can trigger a rerun of this step by deleting the following file:
# <rose-bin>/include-staging/cl.exe_HEADERS/Windows.h
set (rose_system_headers_dir
  "${CMAKE_BINARY_DIR}/include-staging/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/")
if (NOT EXISTS "${rose_system_headers_dir}/Windows.h")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory "${rose_system_headers_dir}")

  # check each registry key
  message(STATUS "copying system headers...")
  foreach(registry_key ${vs9_registry_keys})
    get_filename_component(registry_value "${registry_key}" ABSOLUTE)
    set(vs9_include_dir "${registry_value}/include")
    # if we found an existent include directory, copy its contents into
    # <rose-bin>/include
    if (EXISTS "${vs9_include_dir}")
      message(STATUS "copying from ${vs9_include_dir}")
      file(GLOB_RECURSE header_files "${vs9_include_dir}/*")
      set(headers ${header_files})
      foreach(header ${headers})
        string(REPLACE "${vs9_include_dir}/" "" relative_path "${header}")
        execute_process(
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${header}" "${rose_system_headers_dir}${relative_path}")
      endforeach()
    endif()
  endforeach()
  message(STATUS "done copying system headers")

  # ROSE wants <string>, but Windows provides <string.h>
#  configure_file(
#    ${rose_system_headers_dir}/string.h
#    ${rose_system_headers_dir}/string
#    COPYONLY
#  )
endif()

configure_file(
  ${CMAKE_SOURCE_DIR}/config/rose_edg_required_macros_and_functions.h.cmake
  ${rose_system_headers_dir}/rose_edg_required_macros_and_functions.h
)

