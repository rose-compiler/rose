# - Try to find ROSE include dirs and libraries
# Usage of this module as follows:
#
# Selecting different ROSE modules:
#
# set( ROSE_USE_ROSEHPCT ON )
# set( ROSE_USE_COMPASS ON )
#
#
# The following variables will be set to the correct items:
#
# ROSE_INCLUDE_DIR
# ROSE_LIBRARIES
#
# If cmake has problems finding your rose installation,
# set ROSE_ROOT. the header files shall be in ${ROSE_ROOT}/include
# the libraries in ${ROSE_ROOT}/lib
# If cmake has problems finding your ROSE library installation
# set LD_LIBRARY_PATH to include the rose library directory
#
#
# After succesfully finding ROSE,
#
# ROSE_FOUND will be set.

set( ROSE_FOUND OFF )

if( NOT ROSE_ROOT STREQUAL ENV{ROSE_ROOT} ) 
    list( APPEND ROSE_ROOT $ENV{ROSE_ROOT} )
endif( NOT ROSE_ROOT STREQUAL ENV{ROSE_ROOT} )

# check for boost
set( Boost_USE_MULTITHREADED ON )
find_package( Boost )
if( NOT Boost_FOUND )
    message( SEND_ERROR "Boost not found" )
endif( NOT Boost_FOUND )

find_path( ROSE_INCLUDE_DIRS rose.h HINTS ${ROSE_ROOT}/include )

if( NOT ROSE_INCLUDE_DIRS )
    message( SEND_ERROR "Couldn't find rose.h" )
endif( NOT ROSE_INCLUDE_DIRS )

set ( ROSE_INCLUDE_DIRS ${ROSE_INCLUDE_DIRS} ${ROSE_INCLUDE_DIRS}/roseQtWidgets )
list( APPEND ROSE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} )

find_library( ROSE_LIB rose HINTS ENV LD_LIBRARY_PATH ${ROSE_ROOT}/lib )
get_filename_component( ROSE_LIB ${ROSE_LIB} PATH )

if( ROSE_USE_COMPASS )
    find_library( COMPASS_LIB Compass HINTS ENV LD_LIBRARY_PATH ${ROSE_ROOT}/lib )
    find_library( COMPASSCHECKERS_LIB CompassCheckers HINTS ENV LD_LIBRARY_PATH ${ROSE_ROOT}/lib )
endif( ROSE_USE_COMPASS )

if( ROSE_USE_ROSEHPCT )
    # check for libxml2 ... needed by rosehpct
    find_package( LibXml2 )
    if( NOT LIBXML2_FOUND )
        message( SEND_ERROR "libxml2 not found" )
    endif( NOT LIBXML2_FOUND )

    # find hpct library
    find_library( ROSEHPCT_LIB rosehpct HINTS ENV LD_LIBRARY_PATH ${ROSE_ROOT}/lib )

    # find hpct header
    find_file( ROSE_FOUND_HPCT rosehpct/rosehpct.hh ${ROSE_INCLUDE_DIRS} )
    if( NOT ROSE_FOUND_HPCT )
        set( ROSE_FOUND OFF )
        message( SEND_ERROR "RoseHPCToolkit header not found!" )
    endif( NOT ROSE_FOUND_HPCT )

    set( ROSEHPCT_LIB ${LIBXM2_LIBRARIES} ${ROSEHPCT_LIB} )
    set( ROSE_INCLUDE_DIRS ${ROSE_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIR} )
endif( ROSE_USE_ROSEHPCT )

set( ROSE_LIBRARIES ${ROSE_LIB} 
                    ${EDG310LIB}
                    ${EDG40LIB}
                    ${COMPASS_LIB}
                    ${COMPASSCHECKERS_LIB}
                    ${ROSEHPCT_LIB} )

if( ROSE_LIBRARIES AND ROSE_INCLUDE_DIRS )
    set( ROSE_FOUND ON )
else( ROSE_LIBRARIES AND ROSE_INCLUDE_DIRS )
    message( SEND_ERROR "Rose not found, please set ROSE_ROOT correctly" )
endif( ROSE_LIBRARIES AND ROSE_INCLUDE_DIRS )

set( ROSE_LIBRARY_DIRS ${Boost_LIBRARY_DIRS} ${ROSE_LIBRARIES} )
