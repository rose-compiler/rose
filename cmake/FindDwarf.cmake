# Dwarf detection for ROSE.
#
#  INPUTS:
#    DWARF_ROOT       -- DWARF installation directory or "no" or empty
#                       * If "no" then do not use DWARF and do not search for it.
#                       * If empty use DWARF if found, no error if not found
#                       * Else require DWARF to exist at specified location and use it
#
#  OUTPUTS:
#    DWARF_FOUND      -- Boolean: whether the DWARF library was found.
#    DWARF_LIBRARY    -- String: full name of DWARF library of a string that ends with NOTFOUND
#    DWARF_LIBRARIES  -- String: names of libraries necessary to use DWARF

macro(find_dwarf)
  if("${DWARF_ROOT}" STREQUAL "no")
    # Do not use DWARF, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with users
    # maybe setting them.
    set(DWARF_FOUND FALSE)
    set(DWARF_LIBRARY "")
    set(DWARF_LIBRARIES "")

    if(ELF_ROOT)
      message(WARNING "Elf requested but not needed since Dwarf is not requested")
    endif()
    set(ELF_ROOT no)
    include(FindElf)
    find_elf()

  else()

    # Look for libelf since libdwarf depends on it
    if("${ELF_ROOT}" STREQUAL "no")
      message(FATAL_ERROR "Dwarf requested but Elf prohibited")
    else()
      include(FindElf)
      find_elf()
    endif()
    
    if(NOT ELF_FOUND)
      if(NOT DWARF_ROOT)
	message(STATUS "Elf not found, so Dwarf will not be used")
	set(DWARF_FOUND FALSE)
	set(DWARF_LIBRARY "")
	set(DWARF_LIBRARIES "")
      else()
	message(FATAL_ERROR "Dwarf requested but Elf not found")
      endif()
      
    else()
      message(STATUS "Elf found and Dwarf not disabled, so checking for dwarf")
    
      # Header files.
      if("${DWARF_ROOT}" STREQUAL "")
	# no extra include directories necessary
      else()
	include_directories("${DWARF_ROOT}/include")
      endif()

      # DWARF library.
      if("${DWARF_ROOT}" STREQUAL "")
	find_library(DWARF_LIBRARY NAMES dwarf)
      else()
	find_library(DWARF_LIBRARY NAMES dwarf PATHS "${DWARF_ROOT}/lib" NO_DEFAULT_PATH)
      endif()
      if(DWARF_LIBRARY)
	set(DWARF_FOUND TRUE)
	set(DWARF_LIBRARIES dwarf)
      endif()

      # Error if not found?
      if((NOT ("${DWARF_ROOT}" STREQUAL "")) AND NOT DWARF_FOUND)
	message(FATAL_ERROR "Dwarf requested by user at '${DWARF_ROOT}' but not found")
      endif()

      # If Elf was found but Dwarf was not found, then there's no reason to use Elf.
      if(NOT DWARF_FOUND)
	message(STATUS "Elf found but Dwarf not found, so disabling Elf")
	set(ELF_FOUND FALSE)
	set(ELF_LIBRARY "")
	set(ELF_LIBRARIES "")
      endif()
    endif()
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "DWARF_ROOT       = '${DWARF_ROOT}'")
    message(STATUS "DWARF_FOUND      = '${DWARF_FOUND}'")
    if(DWARF_FOUND)
      message(STATUS "DWARF_LIBRARY    = '${DWARF_LIBRARY}'")
      message(STATUS "DWARF_LIBRARIES  = '${DWARF_LIBRARIES}'")
    endif()
  else()
    if(DWARF_FOUND)
      message(STATUS "Found Dwarf: ${DWARF_LIBRARY}")
    else()
      message(STATUS "Dwarf NOT found.")
    endif()
  endif()

  # ROSE variables
  set(ROSE_HAVE_LIBDWARF ${DWARF_FOUND})
endmacro()
