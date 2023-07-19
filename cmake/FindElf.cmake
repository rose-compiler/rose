# Elf detection for ROSE.
#
#  INPUTS:
#    ELF_ROOT       -- Elf installation directory or "no" or empty
#                       * If "no" then do not use Elf and do not search for it.
#                       * If empty use Elf if found, no error if not found
#                       * Else require Elf to exist at specified location and use it
#
#  OUTPUTS:
#    ELF_FOUND      -- Boolean: whether the Elf library was found.
#    ELF_LIBRARY    -- String: full name of Elf library of a string that ends with NOTFOUND
#    ELF_LIBRARIES  -- String: names of libraries necessary to use Elf
macro(find_elf)
  if("${ELF_ROOT}" STREQUAL "no")
    # Do not use Elf, and therefore do not even search for it. Make sure all outputs are cleared
    # to avoid problems with users maybe setting them.
    set(ELF_FOUND FALSE)
    set(ELF_LIBRARY "")
    set(ELF_LIBRARIES "")
  else()

    # Header files.
    if("${ELF_ROOT}" STREQUAL "")
      # no extra include directories necessary
    else()
      include_directories("${ELF_ROOT}/include")
    endif()

    # Elf library.
    if("${ELF_ROOT}" STREQUAL "")
      find_library(ELF_LIBRARY NAMES elf)
    else()
      find_library(ELF_LIBRARY NAMES elf PATHS "${ELF_ROOT}/lib" NO_DEFAULT_PATH)
    endif()

    if(ELF_LIBRARY)
      set(ELF_FOUND TRUE)
      set(ELF_LIBRARIES elf)
    endif()

    # Error if not found?
    if((NOT ("${ELF_ROOT}" STREQUAL "")) AND NOT ELF_FOUND)
      message(FATAL_ERROR "Elf requested by user at '${ELF_ROOT}' but not found")
    endif()
  endif()

  # Summarize
  if(VERBOSE)
    message(STATUS "ELF_ROOT       = '${ELF_ROOT}'")
    message(STATUS "ELF_FOUND      = '${ELF_FOUND}'")
    if(ELF_FOUND)
      message(STATUS "ELF_LIBRARY    = '${ELF_LIBRARY}'")
      message(STATUS "ELF_LIBRARIES  = '${ELF_LIBRARIES}'")
    endif()
  else()
    if(ELF_FOUND)
      message(STATUS "Found Elf: ${ELF_LIBRARY}")
    else()
      message(STATUS "Elf NOT found.")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_ELF ${ELF_FOUND})
endmacro()
