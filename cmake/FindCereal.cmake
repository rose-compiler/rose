# Cereal detection for ROSE.
#
# Cereal is a header-only library similar to boost::serialization. It was developed at the
# University of Southern California. Cite as:
#
#     Grant, W. Shane and Voorhies, Randolph (2017).
#     cereal - A C++11 library for serialization.
#     URL: http://uscilab.github.io/cereal/
#
#  INPUTS:
#    CEREAL_ROOT       -- Cereal installation directory or "no" or empty
#                       * If "no" then do not use Cereal and do not search for it.
#                       * If empty use Cereal if found, no error if not found
#                       * Else require Cereal to exist at specified location and use it
#
#  OUTPUTS:
#    CEREAL_FOUND      -- Boolean: whether the Cereal library was found.
#    CEREAL_LIBRARY    -- String: empty (this is a header-only library)
#    CEREAL_LIBRARIES  -- String: empty (this is a header-only library)

macro(find_cereal)
  set(CEREAL_LIBRARY "")
  set(CEREAL_LIBRARIES "")

  if(("${CEREAL_ROOT}" STREQUAL "no") OR ("${CEREAL_ROOT}" STREQUAL "OFF"))
    # Do not use Cereal even if it's present
    set(CEREAL_FOUND FALSE)
  else()

    if("${CEREAL_ROOT}" STREQUAL "")
      # Use Cereal if its header files are present
      check_include_file_cxx("cereal/cereal.hpp" CEREAL_FOUND)
      set(CEREAL_ROOT "")

    else()
      # Assume Cereal is present at the specified installation prefix
      set(CEREAL_FOUND TRUE)
      include_directories("${CEREAL_ROOT}/include")
    endif()
  endif()

  # Summarize
  if(CEREAL_FOUND)
    message(STATUS "Found Cereal: ${CEREAL_ROOT}")
  else()
    message(STATUS "Cereal not found.")
  endif()

  # ROSE variables
  set(ROSE_HAVE_CEREAL ${CEREAL_FOUND})
endmacro()
