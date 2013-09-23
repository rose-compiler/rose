# Check compilers and version number
#  Liao 11/25/2009
# This is translated from config/choose-backend-compiler.m4
#
# use the results to setup rose_config.h later on
# --------check C compiler -----------------------
include (CMakeDetermineCCompiler)

if (CMAKE_COMPILER_IS_GNUCC)
#  message("find gnucc  ${CMAKE_C_COMPILER}")
  if(NOT BACKEND_C_COMPILER)
    set (BACKEND_C_COMPILER  ${CMAKE_C_COMPILER})
  endif()
  execute_process ( COMMAND ${BACKEND_C_COMPILER} -dumpversion
                  COMMAND cut -d. -f1
                  OUTPUT_VARIABLE BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER
  )
# for unknown reason, some tail control character will show up in OUTPUT_VARIABLE
# We use regex to fill out non numeric characters to get the right numbers
  string (REGEX MATCH "[0-9]+" BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER ${BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER})
  message("CMAKE_C_COMPILER= ${CMAKE_C_COMPILER}^")

  execute_process ( COMMAND basename ${BACKEND_C_COMPILER}
#                   COMMAND sed 's/.$//'  # remove the trailing return or whatever strange character
                  OUTPUT_VARIABLE BACKEND_C_COMPILER_NAME_WITHOUT_PATH
  )
#  message("BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER = ${BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER}")
  string (REGEX MATCH "[a-zA-Z+-]+" BACKEND_C_COMPILER_NAME_WITHOUT_PATH ${BACKEND_C_COMPILER_NAME_WITHOUT_PATH})
  message("BACKEND_C_COMPILER_NAME_WITHOUT_PATH=${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}^")

  execute_process ( COMMAND ${BACKEND_C_COMPILER} -dumpversion
                  COMMAND cut -d. -f2
                  OUTPUT_VARIABLE BACKEND_C_COMPILER_MINOR_VERSION_NUMBER
  )
  string (REGEX MATCH "[0-9]+" BACKEND_C_COMPILER_MINOR_VERSION_NUMBER ${BACKEND_C_COMPILER_MINOR_VERSION_NUMBER})
  execute_process ( COMMAND ${BACKEND_C_COMPILER} -dumpversion
                  COMMAND cut -d. -f3
                  OUTPUT_VARIABLE BACKEND_C_COMPILER_PATCH_LEVEL_NUMBER
  )
  string (REGEX MATCH "[0-9]+" BACKEND_C_COMPILER_PATCH_LEVEL_NUMBER ${BACKEND_C_COMPILER_PATCH_LEVEL_NUMBER})

#  message("BACKEND_C_COMPILER_MINOR_VERSION_NUMBER= ${BACKEND_C_COMPILER_MINOR_VERSION_NUMBER}")

endif (CMAKE_COMPILER_IS_GNUCC)

# --------check CXX compiler -----------------------
include (roseCMakeDetermineCXXCompiler)

if (CMAKE_COMPILER_IS_GNUCXX)
#  message("find gnucxx  ${CMAKE_CXX_COMPILER}")
  if(NOT BACKEND_CXX_COMPILER)
    set (BACKEND_CXX_COMPILER  ${CMAKE_CXX_COMPILER})
  endif()
  execute_process ( COMMAND ${BACKEND_CXX_COMPILER} -dumpversion
                  COMMAND cut -d. -f1
                  OUTPUT_VARIABLE BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER
  )
  string (REGEX MATCH "[0-9]+" BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER  ${BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER})
  execute_process ( COMMAND basename ${BACKEND_CXX_COMPILER}  
                      OUTPUT_VARIABLE BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH
                        )
  message("BACKEND_CXX_COMPILER= ${BACKEND_CXX_COMPILER}")
  string (REGEX MATCH "[a-zA-Z+-]+" BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH ${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH})
  message("BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH= ${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}")

  execute_process ( COMMAND ${BACKEND_CXX_COMPILER} -dumpversion
                  COMMAND cut -d. -f2
                  OUTPUT_VARIABLE BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER
  )
  string (REGEX MATCH "[0-9]+" BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER ${BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER})
#  message("BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER= ${BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER}")

endif (CMAKE_COMPILER_IS_GNUCXX)

# --------check Fortran compiler -----------------------
# CMakeDetermineFortranCompiler does not recognize gfortran first
# we use a slightly modified CMakeDetermineFortranCompiler.cmake to put gfortran to the highest priority
include (roseCMakeDetermineFortranCompiler)
# message ("${CMAKE_Fortran_COMPILER_ID}") #gnu
# message ("${CMAKE_Fortran_PLATFORM_ID}") #Linux
if ("${CMAKE_Fortran_COMPILER}"  MATCHES ".*gfortran$")
  message("find gfortran compiler ${CMAKE_Fortran_COMPILER}")
  if(NOT BACKEND_FORTRAN_COMPILER)
    set (BACKEND_FORTRAN_COMPILER  ${CMAKE_Fortran_COMPILER})
  endif()

  execute_process ( COMMAND ${BACKEND_FORTRAN_COMPILER} --version
                   COMMAND head -1
                   COMMAND cut -f2 -d\)
                   COMMAND tr -d \ # must have a space
                   COMMAND cut -d. -f1
                  OUTPUT_VARIABLE BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER
  )
  string (REGEX MATCH "[0-9]+" BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER ${BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER})
  execute_process ( COMMAND basename ${BACKEND_FORTRAN_COMPILER}
                      OUTPUT_VARIABLE BACKEND_FORTRAN_COMPILER_NAME_WITHOUT_PATH
                        )
  string (REGEX MATCH "[a-zA-Z+-]+" BACKEND_FORTRAN_COMPILER_NAME_WITHOUT_PATH ${BACKEND_FORTRAN_COMPILER_NAME_WITHOUT_PATH})

#  message("BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER = ${BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER}")

  execute_process ( COMMAND ${BACKEND_FORTRAN_COMPILER} --version
                   COMMAND head -1
                   COMMAND cut -f2 -d\)
                   COMMAND tr -d \ # must have a space
                   COMMAND cut -d. -f2
                  OUTPUT_VARIABLE BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER
  )
  string (REGEX MATCH "[0-9]+" BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER ${BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER})

#  message("BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER= ${BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER}")

endif  ("${CMAKE_Fortran_COMPILER}"  MATCHES ".*gfortran$")

