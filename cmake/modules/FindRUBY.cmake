# - Find Ruby
# This module finds if Ruby is installed and determines where the include files
# and libraries are. It also determines what the name of the library is. This
# code sets the following variables:
#
#  RUBY_LIBRARY      = full path+file to the ruby library
#  RUBY_INCLUDE_PATH = path to where ruby.h can be found
#  RUBY_EXECUTABLE   = full path+file to the ruby binary
#  RUBY_FOUND        = Ruby was found under system.

# Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if(RUBY_LIBRARY AND RUBY_INCLUDE_PATH)
   # Already in cache, be silent
   SET(RUBY_FOUND TRUE)
endif (RUBY_LIBRARY AND RUBY_INCLUDE_PATH)		

#   RUBY_ARCHDIR=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"archdir"@:>@)'`
#   RUBY_SITEARCHDIR=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"sitearchdir"@:>@)'`
#   RUBY_SITEDIR=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"sitelibdir"@:>@)'`
#   RUBY_LIBDIR=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"libdir"@:>@)'`
#   RUBY_LIBRUBYARG=`$RUBY -r rbconfig -e 'printf("%s",Config::CONFIG@<:@"LIBRUBYARG_SHARED"@:>@)'`

FIND_PROGRAM(RUBY_EXECUTABLE NAMES ruby ruby1.8 ruby18 ruby1.9 ruby19)

IF(RUBY_EXECUTABLE)
	EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "puts Config::CONFIG['archdir'] || Config::CONFIG['rubyincludedir']"
   		OUTPUT_VARIABLE RUBY_ARCH_DIR)

	EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "puts Config::CONFIG['libdir']"
   		OUTPUT_VARIABLE RUBY_POSSIBLE_LIB_PATH)

	EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "puts Config::CONFIG['rubylibdir']"
   		OUTPUT_VARIABLE RUBY_RUBY_LIB_PATH)

	EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "puts Config::CONFIG['ruby_version']"
   		OUTPUT_VARIABLE RUBY_VERSION)

	EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "puts Config::CONFIG['sitelibdir']"
		OUTPUT_VARIABLE RUBY_SITE_LIB_DIR)

	EXECUTE_PROCESS(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "puts Config::CONFIG['sitearchdir']"
		OUTPUT_VARIABLE RUBY_SITE_ARCH_DIR)

	# remove the new lines from the output by replacing them with empty strings
	STRING(REPLACE "\n" "" RUBY_ARCH_DIR "${RUBY_ARCH_DIR}")
	STRING(REPLACE "\n" "" RUBY_POSSIBLE_LIB_PATH "${RUBY_POSSIBLE_LIB_PATH}")
	STRING(REPLACE "\n" "" RUBY_RUBY_LIB_PATH "${RUBY_RUBY_LIB_PATH}")
	STRING(REPLACE "\n" "" RUBY_VERSION "${RUBY_VERSION}")
	STRING(REPLACE "." "" RUBY_VERSION_NODOT "${RUBY_VERSION}")
	STRING(REPLACE "\n" "" RUBY_SITE_LIB_DIR "${RUBY_SITE_LIB_DIR}")
	STRING(REPLACE "\n" "" RUBY_SITE_ARCH_DIR "${RUBY_SITE_ARCH_DIR}")

	FIND_PATH(RUBY_INCLUDE_PATH 
  		NAMES ruby.h
  		PATHS ${RUBY_ARCH_DIR} /usr/include/ruby-${RUBY_VERSION}
  	)

	FIND_LIBRARY(RUBY_LIBRARY
		NAMES ruby${RUBY_VERSION} ruby msvcrt-ruby${RUBY_VERSION_NODOT}
		PATHS ${RUBY_POSSIBLE_LIB_PATH} ${RUBY_RUBY_LIB_PATH}
  	)

	if(RUBY_LIBRARY AND RUBY_INCLUDE_PATH)
		SET(RUBY_FOUND TRUE)
	endif (RUBY_LIBRARY AND RUBY_INCLUDE_PATH)

	MARK_AS_ADVANCED(
	  RUBY_EXECUTABLE
	  RUBY_LIBRARY
	  RUBY_INCLUDE_PATH
	  )
ENDIF(RUBY_EXECUTABLE)

IF(RUBY_FOUND)
  IF(NOT RUBY_FIND_QUIETLY)
    MESSAGE(STATUS "Ruby found: ${RUBY_LIBRARY}")
  ENDIF(NOT RUBY_FIND_QUIETLY)
ELSE(RUBY_FOUND)
  IF(RUBY_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Ruby")
  ENDIF(RUBY_FIND_REQUIRED)
ENDIF(RUBY_FOUND)


