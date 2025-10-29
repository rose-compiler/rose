# - Find the KDE4 include and library dirs, KDE preprocessors and define a some macros
#
# This module defines the following variables:
#
#  KDE4_FOUND               - set to TRUE if everything required for building KDE software has been found
#
#  KDE4_DEFINITIONS         - compiler definitions required for compiling KDE software
#  KDE4_INCLUDE_DIR         - the KDE 4 include directory
#  KDE4_INCLUDES            - all include directories required for KDE, i.e.
#                             KDE4_INCLUDE_DIR, but also the Qt4 include directories
#                             and other platform specific include directories
#  KDE4_LIB_DIR             - the directory where the KDE libraries are installed,
#                             intended to be used with LINK_DIRECTORIES()
#
# The following variables are defined for the various tools required to
# compile KDE software:
#
#  KDE4_KCFGC_EXECUTABLE    - the kconfig_compiler executable
#  KDE4_AUTOMOC_EXECUTABLE  - the kde4automoc executable, deprecated, use AUTOMOC4_EXECUTABLE instead
#  KDE4_MEINPROC_EXECUTABLE - the meinproc4 executable
#  KDE4_MAKEKDEWIDGETS_EXECUTABLE - the makekdewidgets executable
#
# The following variables point to the location of the KDE libraries,
# but shouldn't be used directly:
#
#  KDE4_KDECORE_LIBRARY     - the kdecore library
#  KDE4_KDEUI_LIBRARY       - the kdeui library
#  KDE4_KIO_LIBRARY         - the kio library
#  KDE4_KPARTS_LIBRARY      - the kparts library
#  KDE4_KUTILS_LIBRARY      - the kutils library
#  KDE4_KDE3SUPPORT_LIBRARY - the kde3support library
#  KDE4_KFILE_LIBRARY       - the kfile library
#  KDE4_KHTML_LIBRARY       - the khtml library
#  KDE4_KJS_LIBRARY         - the kjs library
#  KDE4_KJSAPI_LIBRARY      - the kjs public api library
#  KDE4_KNEWSTUFF2_LIBRARY  - the knewstuff2 library
#  KDE4_KDNSSD_LIBRARY      - the kdnssd library
#  KDE4_PHONON_LIBRARY      - the phonon library
#  KDE4_THREADWEAVER_LIBRARY- the threadweaver library
#  KDE4_SOLID_LIBRARY       - the solid library
#  KDE4_KNOTIFYCONFIG_LIBRARY- the knotifyconfig library
#  KDE4_KROSSCORE_LIBRARY   - the krosscore library
#  KDE4_KTEXTEDITOR_LIBRARY - the ktexteditor library
#  KDE4_KNEPOMUK_LIBRARY    - the knepomuk library
#  KDE4_KMETADATA_LIBRARY   - the kmetadata library
#  KDE4_PLASMA_LIBRARY      - the plasma library
#
#  KDE4_PLASMA_OPENGL_FOUND  - TRUE if the OpenGL support of Plasma has been found, NOTFOUND otherwise
#
# Compared to the variables above, the following variables
# also contain all of the depending libraries, so the variables below
# should be used instead of the ones above:
#
#  KDE4_KDECORE_LIBS          - the kdecore library and all depending libraries
#  KDE4_KDEUI_LIBS            - the kdeui library and all depending libraries
#  KDE4_KIO_LIBS              - the kio library and all depending libraries
#  KDE4_KPARTS_LIBS           - the kparts library and all depending libraries
#  KDE4_KUTILS_LIBS           - the kutils library and all depending libraries
#  KDE4_KDE3SUPPORT_LIBS      - the kde3support library and all depending libraries
#  KDE4_KFILE_LIBS            - the kfile library and all depending libraries
#  KDE4_KHTML_LIBS            - the khtml library and all depending libraries
#  KDE4_KJS_LIBS              - the kjs library and all depending libraries
#  KDE4_KJSAPI_LIBS           - the kjs public api library and all depending libraries
#  KDE4_KNEWSTUFF2_LIBS       - the knewstuff2 library and all depending libraries
#  KDE4_KDNSSD_LIBS           - the kdnssd library and all depending libraries
#  KDE4_KDESU_LIBS            - the kdesu library and all depending libraries
#  KDE4_KPTY_LIBS             - the kpty library and all depending libraries
#  KDE4_PHONON_LIBS           - the phonon library and all depending librairies
#  KDE4_THREADWEAVER_LIBRARIES- the threadweaver library and all depending libraries
#  KDE4_SOLID_LIBS            - the solid library and all depending libraries
#  KDE4_KNOTIFYCONFIG_LIBS    - the knotify config library and all depending libraries
#  KDE4_KROSSCORE_LIBS        - the kross core library and all depending libraries
#  KDE4_KROSSUI_LIBS          - the kross ui library which includes core and all depending libraries
#  KDE4_KTEXTEDITOR_LIBS      - the ktexteditor library and all depending libraries
#  KDE4_KNEPOMUK_LIBS         - the knepomuk library and all depending libraries
#  KDE4_KMETADATA_LIBS        - the kmetadata library and all depending libraries
#  KDE4_PLASMA_LIBS           - the plasma library and all depending librairies
#
# This module defines a bunch of variables used as locations for install directories. 
# They can be relative (to CMAKE_INSTALL_PREFIX) or absolute.
# Under Windows they are always relative.
#
#  BIN_INSTALL_DIR          - the directory where executables will be installed (default is prefix/bin)
#  BUNDLE_INSTALL_DIR       - Mac only: the directory where application bundles will be installed (default is /Applications/KDE4 )
#  SBIN_INSTALL_DIR         - the directory where system executables will be installed (default is prefix/sbin)
#  LIB_INSTALL_DIR          - the directory where libraries will be installed (default is prefix/lib)
#  CONFIG_INSTALL_DIR       - the config file install dir
#  DATA_INSTALL_DIR         - the parent directory where applications can install their data
#  HTML_INSTALL_DIR         - the HTML install dir for documentation
#  ICON_INSTALL_DIR         - the icon install dir (default prefix/share/icons/)
#  INFO_INSTALL_DIR         - the kde info install dir (default prefix/info)
#  KCFG_INSTALL_DIR         - the install dir for kconfig files
#  LOCALE_INSTALL_DIR       - the install dir for translations
#  MAN_INSTALL_DIR          - the kde man page install dir (default prefix/man/)
#  MIME_INSTALL_DIR         - the install dir for the mimetype desktop files
#  PLUGIN_INSTALL_DIR       - the subdirectory relative to the install prefix where plugins will be installed (default is ${KDE4_LIB_INSTALL_DIR}/kde4)
#  SERVICES_INSTALL_DIR     - the install dir for service (desktop, protocol, ...) files
#  SERVICETYPES_INSTALL_DIR - the install dir for servicestypes desktop files
#  SOUND_INSTALL_DIR        - the install dir for sound files
#  TEMPLATES_INSTALL_DIR    - the install dir for templates (Create new file...)
#  WALLPAPER_INSTALL_DIR    - the install dir for wallpapers
#  DEMO_INSTALL_DIR         - the install dir for demos
#  KCONF_UPDATE_INSTALL_DIR - the kconf_update install dir
#  XDG_APPS_INSTALL_DIR     - the XDG apps dir
#  XDG_DIRECTORY_INSTALL_DIR- the XDG directory
#  XDG_MIME_INSTALL_DIR     - the XDG mimetypes install dir
#  DBUS_INTERFACES_INSTALL_DIR - the directory where dbus interfaces be installed (default is prefix/share/dbus-1/interfaces)
#  DBUS_SERVICES_INSTALL_DIR        - the directory where dbus services be installed (default is prefix/share/dbus-1/services )
#
# The following variable is provided, but seem to be unused:
#  LIBS_HTML_INSTALL_DIR    /share/doc/HTML            CACHE STRING "Is this still used ?")
#
# The following user adjustable options are provided:
#
#  KDE4_ENABLE_FINAL - enable KDE-style enable-final all-in-one-compilation
#  KDE4_BUILD_TESTS  - enable this to build the testcases
#  KDE4_ENABLE_FPIE  - enable it to use gcc Position Independent Executables feature
#  KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR - only present for CMake >= 2.6.3, defaults to TRUE
#                      If enabled, the package should install its <package>Config.cmake file to 
#                      lib/cmake/<package>/ instead to lib/<package>/cmake
#
# It also adds the following macros and functions (from KDE4Macros.cmake)
#  KDE4_ADD_UI_FILES (SRCS_VAR file1.ui ... fileN.ui)
#    Use this to add Qt designer ui files to your application/library.
#
#  KDE4_ADD_UI3_FILES (SRCS_VAR file1.ui ... fileN.ui)
#    Use this to add Qt designer ui files from Qt version 3 to your application/library.
#
#  KDE4_ADD_KCFG_FILES (SRCS_VAR [GENERATE_MOC] file1.kcfgc ... fileN.kcfgc)
#    Use this to add KDE config compiler files to your application/library.
#    Use optional GENERATE_MOC to generate moc if you use signals in your kcfg files.
#
#  KDE4_ADD_WIDGET_FILES (SRCS_VAR file1.widgets ... fileN.widgets)
#    Use this to add widget description files for the makekdewidgets code generator
#    for Qt Designer plugins.
#
#  KDE4_CREATE_FINAL_FILES (filename_CXX filename_C file1 ... fileN)
#    This macro is intended mainly for internal uses.
#    It is used for enable-final. It will generate two source files,
#    one for the C files and one for the C++ files.
#    These files will have the names given in filename_CXX and filename_C.
#
#  KDE4_ADD_PLUGIN ( name [WITH_PREFIX] file1 ... fileN )
#    Create a KDE plugin (KPart, kioslave, etc.) from the given source files.
#    It supports KDE4_ENABLE_FINAL.
#    If WITH_PREFIX is given, the resulting plugin will have the prefix "lib", otherwise it won't.
#
#  KDE4_ADD_KDEINIT_EXECUTABLE (name [NOGUI] [RUN_UNINSTALLED] file1 ... fileN)
#    Create a KDE application in the form of a module loadable via kdeinit.
#    A library named kdeinit_<name> will be created and a small executable which links to it.
#    It supports KDE4_ENABLE_FINAL
#    If the executable has to be run from the buildtree (e.g. unit tests and code generators
#    used later on when compiling), set the option RUN_UNINSTALLED.
#    If the executable doesn't have a GUI, use the option NOGUI. By default on OS X
#    application bundles are created, with the NOGUI option no bundles but simple executables
#    are created. Currently it doesn't have any effect on other platforms.
#
#  KDE4_ADD_EXECUTABLE (name [NOGUI] [TEST] [RUN_UNINSTALLED] file1 ... fileN)
#    Equivalent to ADD_EXECUTABLE(), but additionally adds some more features:
#    -support for KDE4_ENABLE_FINAL
#    -support for automoc
#    -automatic RPATH handling
#    If the executable has to be run from the buildtree (e.g. unit tests and code generators
#    used later on when compiling), set the option RUN_UNINSTALLED.
#    If the executable doesn't have a GUI, use the option NOGUI. By default on OS X
#    application bundles are created, with the NOGUI option no bundles but simple executables
#    are created. Currently it doesn't have any effect on other platforms.
#
#  KDE4_ADD_LIBRARY (name [STATIC | SHARED | MODULE ] file1 ... fileN)
#    Equivalent to ADD_LIBRARY(), but additionally it supports KDE4_ENABLE_FINAL
#    and under Windows it adds a -DMAKE_<name>_LIB definition to the compilation.
#
#  KDE4_ADD_UNIT_TEST (testname [TESTNAME targetname] file1 ... fileN)
#    add a unit test, which is executed when running make test
#    it will be built with RPATH poiting to the build dir
#    The targets are always created, but only built for the "all"
#    target if the option KDE4_BUILD_TESTS is enabled. Otherwise the rules for the target
#    are created but not built by default. You can build them by manually building the target.
#    The name of the target can be specified using TESTNAME <targetname>, if it is not given
#    the macro will default to the <testname>
#    KDESRCDIR is set to the source directory of the test, this can be used with
#    KGlobal::dirs()->addResourceDir( "data", KDESRCDIR )
#
#  KDE4_UPDATE_ICONCACHE()
#    Notifies the icon cache that new icons have been installed by updating
#    mtime of ${ICON_INSTALL_DIR}/hicolor directory.
#
#  KDE4_INSTALL_ICONS( path theme)
#    Installs all png and svgz files in the current directory to the icon
#    directoy given in path, in the subdirectory for the given icon theme.
#
#  KDE4_CREATE_HANDBOOK( docbookfile [INSTALL_DESTINATION installdest] [SUBDIR subdir])
#   Create the handbook from the docbookfile (using meinproc4)
#   The resulting handbook will be installed to <installdest> when using
#   INSTALL_DESTINATION <installdest>, or to <installdest>/<subdir> if
#   SUBDIR <subdir> is specified.
#
#  KDE4_CREATE_MANPAGE( docbookfile section )
#   Create the manpage for the specified section from the docbookfile (using meinproc4)
#   The resulting manpage will be installed to <installdest> when using
#   INSTALL_DESTINATION <installdest>, or to <installdest>/<subdir> if
#   SUBDIR <subdir> is specified.
#
#
#  A note on the possible values for CMAKE_BUILD_TYPE and how KDE handles
#  the flags for those buildtypes. FindKDE4Internal supports the values
#  Debug, Release, RelWithDebInfo, Profile and Debugfull
#
#  Release
#          optimised for speed, qDebug/kDebug turned off, no debug symbols
#  Release with debug info
#          optimised for speed, debugging symbols on (-g)
#  Debug
#          optimised but debuggable, debugging on (-g)
#          (-fno-reorder-blocks -fno-schedule-insns -fno-inline)
#  DebugFull
#          no optimisation, full debugging on (-g3)
#  Profile
#          DebugFull + -ftest-coverage -fprofile-arcs
#
#  It is expected that the "Debug" build type be still debuggable with gdb
#  without going all over the place, but still produce better performance.
#  It's also important to note that gcc cannot detect all warning conditions
#  unless the optimiser is active.
#
#  This module allows to depend on a particular minimum version of kdelibs.
#  To acomplish that one should use the apropriate cmake syntax for
#  find_package. For example to depend on kdelibs >= 4.1.0 one should use
#
#  find_package(KDE4 4.1.0 REQUIRED)
#
#  In earlier versions of KDE you could use the variable KDE_MIN_VERSION to
#  have such a dependency. This variable is deprecated with KDE 4.2.0, but
#  will still work to make the module backwards-compatible.

#  _KDE4_PLATFORM_INCLUDE_DIRS is used only internally
#  _KDE4_PLATFORM_DEFINITIONS is used only internally

# Copyright (c) 2006-2009, Alexander Neundorf <neundorf@kde.org>
# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


# this is required now by cmake 2.6 and so must not be skipped by if(KDE4_FOUND) below
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)
# set the cmake policies to the 2.4.x compatibility settings (may change for KDE 4.3)
cmake_policy(VERSION 2.4.5)

# CMake 2.6, set compatibility behaviour to cmake 2.4
# this must be executed always, because the CMAKE_MINIMUM_REQUIRED() command above
# resets the policy settings, so we get a lot of warnings

# CMP0000: don't require cmake_minimum_version() directly in the top level CMakeLists.txt, FindKDE4Internal.cmake is good enough
cmake_policy(SET CMP0000 OLD)
# CMP0002: in KDE4 we have multiple targets with the same name for the unit tests
cmake_policy(SET CMP0002 OLD)
# CMP0003: add the link paths to the link command as with cmake 2.4
cmake_policy(SET CMP0003 OLD)
# CMP0005: keep escaping behaviour for definitions added via add_definitions()
cmake_policy(SET CMP0005 OLD)
# since cmake 2.6.3: NEW behaviour is that setting policies doesn't "escape" the file 
# where this is done, macros and functions are executed with the policies as they
# were when the were defined. Keep the OLD behaviour so we can set the policies here
# for all KDE software without the big warning
if(POLICY CMP0011)
   cmake_policy(SET CMP0011 OLD)
endif(POLICY CMP0011)


# Only do something if it hasn't been found yet
if(NOT KDE4_FOUND)

include (MacroEnsureVersion)

# We may only search for other packages with "REQUIRED" if we are required ourselves.
# This file can be processed either (usually) included in FindKDE4.cmake or
# (when building kdelibs) directly via FIND_PACKAGE(KDE4Internal), that's why
# we have to check for both KDE4_FIND_REQUIRED and KDE4Internal_FIND_REQUIRED.
if(KDE4_FIND_REQUIRED  OR  KDE4Internal_FIND_REQUIRED)
  set(_REQ_STRING_KDE4 "REQUIRED")
  set(_REQ_STRING_KDE4_MESSAGE "FATAL_ERROR")
else(KDE4_FIND_REQUIRED  OR  KDE4Internal_FIND_REQUIRED)
  set(_REQ_STRING_KDE4 )
  set(_REQ_STRING_KDE4_MESSAGE "STATUS")
endif(KDE4_FIND_REQUIRED  OR  KDE4Internal_FIND_REQUIRED)

set(QT_MIN_VERSION "4.4.0")
#this line includes FindQt4.cmake, which searches the Qt library and headers
find_package(Qt4 ${_REQ_STRING_KDE4})

# automoc4 (from kdesupport) is now required, Alex
# AS do not need AUTOMOC
#find_package(Automoc4 ${_REQ_STRING_KDE4})

# cmake 2.6.0 and automoc4 < 0.9.84 don't work right for -D flags
#if (NOT AUTOMOC4_VERSION)
   # the version macro was added for 0.9.84
#   set(AUTOMOC4_VERSION "0.9.83")
#endif (NOT AUTOMOC4_VERSION)
#macro_ensure_version("0.9.87" "${AUTOMOC4_VERSION}" _automoc4_version_ok)

# for compatibility with KDE 4.0.x
#set(KDE4_AUTOMOC_EXECUTABLE        "${AUTOMOC4_EXECUTABLE}" )

# Perl is required for building KDE software
find_package(Perl ${_REQ_STRING_KDE4})

# Check that we really found everything.
# If KDE4 was searched with REQUIRED, we error out with FATAL_ERROR if something wasn't found 
# already above in the other FIND_PACKAGE() calls.
# If KDE4 was searched without REQUIRED and something in the FIND_PACKAGE() calls above wasn't found, 
# then we get here and must check that everything has actually been found. If something is missing,
# we must not fail with FATAL_ERROR, but only not set KDE4_FOUND.
if(NOT PERL_FOUND)
   message(STATUS "KDE4 not found, because Perl not found")
   return()
endif(NOT PERL_FOUND)

if(NOT QT4_FOUND)
   message(STATUS "KDE4 not found, because Qt4 not found")
   return()
endif(NOT QT4_FOUND)

if(NOT AUTOMOC4_FOUND OR NOT _automoc4_version_ok)
   if(NOT AUTOMOC4_FOUND)
      message(${_REQ_STRING_KDE4_MESSAGE} "KDE4 not found, because Automoc4 not found.")
      return()
   else(NOT AUTOMOC4_FOUND)
      if(NOT _automoc4_version_ok)
         message(${_REQ_STRING_KDE4_MESSAGE} "Your version of automoc4 is too old. You have ${AUTOMOC4_VERSION}, you need at least 0.9.87")
         return()
      endif(NOT _automoc4_version_ok)
   endif(NOT AUTOMOC4_FOUND)
endif(NOT AUTOMOC4_FOUND OR NOT _automoc4_version_ok)


# now we are sure we have everything we need

include (MacroLibrary)
include (CheckCXXCompilerFlag)
include (CheckCXXSourceCompiles)


# get the directory of the current file, used later on in the file
get_filename_component( kde_cmake_module_dir  ${CMAKE_CURRENT_LIST_FILE} PATH)

# are we trying to compile kdelibs ? kdelibs_SOURCE_DIR comes from "project(kdelibs)" in kdelibs/CMakeLists.txt
# then enter bootstrap mode

if(kdelibs_SOURCE_DIR)
   set(_kdeBootStrapping TRUE)
   message(STATUS "Building kdelibs...")
else(kdelibs_SOURCE_DIR)
   set(_kdeBootStrapping FALSE)
endif(kdelibs_SOURCE_DIR)

#######################  #now try to find some kde stuff  ################################

if (_kdeBootStrapping)
   set(KDE4_INCLUDE_DIR ${kdelibs_SOURCE_DIR})
   set(KDE4_KDECORE_LIBS ${QT_QTCORE_LIBRARY} kdecore)
   set(KDE4_KDEUI_LIBS ${KDE4_KDECORE_LIBS} kdeui)
   set(KDE4_KIO_LIBS ${KDE4_KDEUI_LIBS} kio)
   set(KDE4_KPARTS_LIBS ${KDE4_KIO_LIBS} kparts)
   if (UNIX)
      set(KDE4_KPTY_LIBS ${KDE4_KDECORE_LIBS} kpty)
   endif (UNIX)
   set(KDE4_KUTILS_LIBS ${KDE4_KIO_LIBS} kutils)
   set(KDE4_KDE3SUPPORT_LIBS ${KDE4_KIO_LIBS} kde3support)
   set(KDE4_SOLID_LIBS ${KDE4_KDECORE_LIBS} solid)
   set(KDE4_KFILE_LIBS ${KDE4_KIO_LIBS} kfile)
   set(KDE4_KHTML_LIBS ${KDE4_KPARTS_LIBS} khtml)

   set(EXECUTABLE_OUTPUT_PATH ${kdelibs_BINARY_DIR}/bin )

   if (WIN32)
      set(LIBRARY_OUTPUT_PATH            ${EXECUTABLE_OUTPUT_PATH} )
      # CMAKE_CFG_INTDIR is the output subdirectory created e.g. by XCode and MSVC
      set(KDE4_KCFGC_EXECUTABLE          ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/kconfig_compiler )

      set(KDE4_MEINPROC_EXECUTABLE       ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/meinproc4 )
      set(KDE4_MAKEKDEWIDGETS_EXECUTABLE ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/makekdewidgets )
   else (WIN32)
      set(LIBRARY_OUTPUT_PATH            ${CMAKE_BINARY_DIR}/lib )
      set(KDE4_KCFGC_EXECUTABLE          ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/kconfig_compiler.shell )

      set(KDE4_MEINPROC_EXECUTABLE       ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/meinproc4.shell )
      set(KDE4_MAKEKDEWIDGETS_EXECUTABLE ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/makekdewidgets.shell )
   endif (WIN32)

   set(KDE4_LIB_DIR ${LIBRARY_OUTPUT_PATH}/${CMAKE_CFG_INTDIR})

   # when building kdelibs, make the kcfg rules depend on the binaries...
   set( _KDE4_KCONFIG_COMPILER_DEP kconfig_compiler)
   set( _KDE4_MAKEKDEWIDGETS_DEP makekdewidgets)
   set( _KDE4_MEINPROC_EXECUTABLE_DEP meinproc4)

   set(KDE4_INSTALLED_VERSION_OK TRUE)

else (_kdeBootStrapping)

  # ... but NOT otherwise
   set( _KDE4_KCONFIG_COMPILER_DEP)
   set( _KDE4_MAKEKDEWIDGETS_DEP)
   set( _KDE4_MEINPROC_EXECUTABLE_DEP)

   set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib )

   if (WIN32)
      # we don't want to be forced to set two paths into the build tree 
      set(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin )

      # on win32 the install dir is determined on runtime not install time
      # KDELIBS_INSTALL_DIR and QT_INSTALL_DIR are used in KDELibsDependencies.cmake to setup 
      # kde install paths and library dependencies
      get_filename_component(_DIR ${KDE4_KDECONFIG_EXECUTABLE} PATH )
      get_filename_component(KDE4_INSTALL_DIR ${_DIR} PATH )
      get_filename_component(_DIR ${QT_QMAKE_EXECUTABLE} PATH )
      get_filename_component(QT_INSTALL_DIR ${_DIR} PATH )
   endif (WIN32)

   # This file contains information about the installed kdelibs, Alex
   include(${kde_cmake_module_dir}/KDELibsDependencies.cmake)

   # Check the version of KDE. It must be at least KDE_MIN_VERSION as set by the user.
   # KDE_VERSION is set in KDELibsDependencies.cmake since KDE 4.0.x. Alex
   # Support for the new-style (>= 2.6.0) support for requiring some version of a package:
   if (NOT KDE_MIN_VERSION)
      if (KDE4_FIND_VERSION_MAJOR)
         set(KDE_MIN_VERSION "${KDE4_FIND_VERSION_MAJOR}.${KDE4_FIND_VERSION_MINOR}.${KDE4_FIND_VERSION_PATCH}")
      else (KDE4_FIND_VERSION_MAJOR)
         set(KDE_MIN_VERSION "4.0.0")
      endif (KDE4_FIND_VERSION_MAJOR)
   endif (NOT KDE_MIN_VERSION)

   #message(FATAL_ERROR "KDE_MIN_VERSION=${KDE_MIN_VERSION}  found ${KDE_VERSION} exact: -${KDE4_FIND_VERSION_EXACT}- version: -${KDE4_FIND_VERSION}-")
   macro_ensure_version( ${KDE_MIN_VERSION} ${KDE_VERSION} KDE4_INSTALLED_VERSION_OK )

   # This file contains the exported library target from kdelibs (new with cmake 2.6.x), e.g.
   # the library target "kdeui" is exported as "KDE4__kdeui". The "KDE4__" is used as
   # "namespace" to separate the imported targets from "normal" targets, it is stored in 
   # KDE4_TARGET_PREFIX, which is set in KDELibsDependencies.cmake .
   # Include it to "import" the libraries from kdelibs into the current projects as targets.
   # This makes setting the _LIBRARY and _LIBS variables actually a bit superfluos, since e.g.
   # the kdeui library could now also be used just as "KDE4__kdeui" and still have all their
   # dependent libraries handled correctly. But to keep compatibility and not to change
   # behaviour we set all these variables anyway as seen below. Alex
   include(${kde_cmake_module_dir}/KDELibs4LibraryTargets.cmake)

   # helper macro, sets both the KDE4_FOO_LIBRARY and KDE4_FOO_LIBS variables to KDE4__foo
   macro(_KDE4_SET_LIB_VARIABLES _var _lib _prefix)
      set(KDE4_${_var}_LIBRARY ${_prefix}${_lib} )
      set(KDE4_${_var}_LIBS    ${_prefix}${_lib} )
   endmacro(_KDE4_SET_LIB_VARIABLES _var _lib _prefix)

   # sorted by names
   _kde4_set_lib_variables(KDE3SUPPORT   kde3support   ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KDECORE       kdecore       ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KDEUI         kdeui         ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KDNSSD        kdnssd        ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KFILE         kfile         ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KHTML         khtml         ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KIO           kio           ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KJS           kjs           ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KJSAPI        kjsapi        ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KNEWSTUFF2    knewstuff2    ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KNOTIFYCONFIG knotifyconfig ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KPARTS        kparts        ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KROSSCORE     krosscore     ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KROSSUI       krossui       ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KTEXTEDITOR   ktexteditor   ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(PLASMA        plasma        ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(SOLID         solid         ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(THREADWEAVER  threadweaver  ${KDE4_TARGET_PREFIX})
   _kde4_set_lib_variables(KUTILS        kutils        ${KDE4_TARGET_PREFIX})

   if (UNIX)
      _kde4_set_lib_variables(KDEFAKES kdefakes ${KDE4_TARGET_PREFIX})
      _kde4_set_lib_variables(KDESU kdesu ${KDE4_TARGET_PREFIX})
      _kde4_set_lib_variables(KPTY kpty ${KDE4_TARGET_PREFIX})
   endif (UNIX)

   # these targets do not always exist, since they are built conditionally:
   if(TARGET ${KDE4_TARGET_PREFIX}knepomuk)
      _kde4_set_lib_variables(KNEPOMUK knepomuk ${KDE4_TARGET_PREFIX})
   endif(TARGET ${KDE4_TARGET_PREFIX}knepomuk)

   if(TARGET ${KDE4_TARGET_PREFIX}kmetadata)
      _kde4_set_lib_variables(KMETADATA kmetadata ${KDE4_TARGET_PREFIX})
   endif(TARGET ${KDE4_TARGET_PREFIX}kmetadata)

   # and this one for compatibility:
   set(KDE4_THREADWEAVER_LIBRARIES ${KDE4_TARGET_PREFIX}threadweaver )

   # KDE4_LIB_INSTALL_DIR and KDE4_INCLUDE_INSTALL_DIR are set in KDELibsDependencies.cmake,
   # use them to set the KDE4_LIB_DIR and KDE4_INCLUDE_DIR "public interface" variables
   set(KDE4_LIB_DIR ${KDE4_LIB_INSTALL_DIR} )
   set(KDE4_INCLUDE_DIR ${KDE4_INCLUDE_INSTALL_DIR} )


   # This setting is currently not recorded in KDELibsDependencies.cmake: 
   find_file(KDE4_PLASMA_OPENGL_FOUND plasma/glapplet.h PATHS ${KDE4_INCLUDE_DIR} NO_DEFAULT_PATH)

   # now include the file with the imported tools (executable targets)
   # Having the libs and tools in two separate files should help with cross compiling.
   include(${kde_cmake_module_dir}/KDELibs4ToolsTargets.cmake)

   # get the build CONFIGURATIONS which were exported in this file, and use just the first
   # of them to get the location of the installed executables
   get_target_property(_importedConfigurations  ${KDE4_TARGET_PREFIX}kconfig_compiler IMPORTED_CONFIGURATIONS )
   list(GET _importedConfigurations 0 _firstConfig)

   get_target_property(KDE4_KCFGC_EXECUTABLE          ${KDE4_TARGET_PREFIX}kconfig_compiler LOCATION_${firstConfig})
   get_target_property(KDE4_MEINPROC_EXECUTABLE       ${KDE4_TARGET_PREFIX}meinproc4        LOCATION_${firstConfig})
   get_target_property(KDE4_MAKEKDEWIDGETS_EXECUTABLE ${KDE4_TARGET_PREFIX}makekdewidgets   LOCATION_${firstConfig})

   # allow searching cmake modules in all given kde install locations (KDEDIRS based)
   execute_process(COMMAND "${KDE4_KDECONFIG_EXECUTABLE}" --path data OUTPUT_VARIABLE _data_DIR ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
   file(TO_CMAKE_PATH "${_data_DIR}" _data_DIR)
   foreach(dir ${_data_DIR})
      set (apath "${dir}/cmake/modules")
      if (EXISTS "${apath}")
         set (included 0)
         string(TOLOWER "${apath}" _apath)
         # ignore already added pathes, case insensitive
         foreach(adir ${CMAKE_MODULE_PATH})
            string(TOLOWER "${adir}" _adir)
            if ("${_adir}" STREQUAL "${_apath}")
               set (included 1)
            endif ("${_adir}" STREQUAL "${_apath}")
         endforeach(adir)
         if (NOT included)
            message(STATUS "Adding ${apath} to CMAKE_MODULE_PATH")
            set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${apath}")
         endif (NOT included)
      endif (EXISTS "${apath}")
   endforeach(dir)

endif (_kdeBootStrapping)


#####################  provide some options   ##########################################

option(KDE4_ENABLE_FINAL "Enable final all-in-one compilation")
option(KDE4_BUILD_TESTS  "Build the tests")
option(KDE4_ENABLE_HTMLHANDBOOK  "Create targets htmlhandbook for creating the html versions of the docbook docs")

# if CMake 2.6.3 or above is used, provide an option which should be used by other KDE packages
# whether to install a CMake FooConfig.cmake into lib/foo/cmake/ or /lib/cmake/foo/
# (with 2.6.3 and above also lib/cmake/foo/ is supported):
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.6.2)
   option(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR "Prefer to install the <package>Config.cmake files to lib/cmake/<package> instead to lib/<package>/cmake" TRUE)
else(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.6.2)
   set(KDE4_USE_COMMON_CMAKE_PACKAGE_CONFIG_DIR  FALSE)
endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION} VERSION_GREATER 2.6.2)

# Position-Independent-Executable is a feature of Binutils, Libc, and GCC that creates an executable
# which is something between a shared library and a normal executable.
# Programs compiled with these features appear as ?shared object? with the file command.
# info from "http://www.linuxfromscratch.org/hlfs/view/unstable/glibc/chapter02/pie.html"
option(KDE4_ENABLE_FPIE  "Enable platform supports PIE linking")


#####################  some more settings   ##########################################

if( KDE4_ENABLE_FINAL)
   add_definitions(-DKDE_USE_FINAL)
endif(KDE4_ENABLE_FINAL)

# If we are building ! kdelibs, check where kdelibs are installed.
# If they are installed in a directory which contains "lib64", we default to "64" for LIB_SUFFIX,
# so the current project will by default also go into lib64.
# The same for lib32. Alex
set(_Init_LIB_SUFFIX "")
if ("${KDE4_LIB_DIR}" MATCHES lib64)
   set(_Init_LIB_SUFFIX 64)
endif ("${KDE4_LIB_DIR}" MATCHES lib64)
if ("${KDE4_LIB_DIR}" MATCHES lib32)
   set(_Init_LIB_SUFFIX 32)
endif ("${KDE4_LIB_DIR}" MATCHES lib32)

set(LIB_SUFFIX "${_Init_LIB_SUFFIX}" CACHE STRING "Define suffix of directory name (32/64)" )


########## the following are directories where stuff will be installed to  ###########
#
# this has to be after find_xxx() block above, since there KDELibsDependencies.cmake is included
# which contains the install dirs from kdelibs, which are reused below

if (WIN32)
# use relative install prefix to avoid hardcoded install paths in cmake_install.cmake files

   set(LIB_INSTALL_DIR      "lib${LIB_SUFFIX}" )            # The subdirectory relative to the install prefix where libraries will be installed (default is ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})

   set(EXEC_INSTALL_PREFIX  "" )        # Base directory for executables and libraries
   set(SHARE_INSTALL_PREFIX "share" )   # Base directory for files which go to share/
   set(BIN_INSTALL_DIR      "bin"   )   # The install dir for executables (default ${EXEC_INSTALL_PREFIX}/bin)
   set(SBIN_INSTALL_DIR     "sbin"  )   # The install dir for system executables (default ${EXEC_INSTALL_PREFIX}/sbin)

   set(LIBEXEC_INSTALL_DIR  "${BIN_INSTALL_DIR}"          ) # The subdirectory relative to the install prefix where libraries will be installed (default is ${BIN_INSTALL_DIR})
   set(INCLUDE_INSTALL_DIR  "include"                     ) # The subdirectory to the header prefix

   set(PLUGIN_INSTALL_DIR       "lib${LIB_SUFFIX}/kde4"   ) #                "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/kde4)
   set(CONFIG_INSTALL_DIR       "share/config"            ) # The config file install dir
   set(DATA_INSTALL_DIR         "share/kde4/apps"              ) # The parent directory where applications can install their data
   set(HTML_INSTALL_DIR         "share/doc/kde/HTML"          ) # The HTML install dir for documentation
   set(ICON_INSTALL_DIR         "share/icons"             ) # The icon install dir (default ${SHARE_INSTALL_PREFIX}/share/icons/)
   set(KCFG_INSTALL_DIR         "share/kde4/config.kcfg"       ) # The install dir for kconfig files
   set(LOCALE_INSTALL_DIR       "share/locale"            ) # The install dir for translations
   set(MIME_INSTALL_DIR         "share/mimelnk"           ) # The install dir for the mimetype desktop files
   set(SERVICES_INSTALL_DIR     "share/kde4/services"     ) # The install dir for service (desktop, protocol, ...) files
   set(SERVICETYPES_INSTALL_DIR "share/kde4/servicetypes" ) # The install dir for servicestypes desktop files
   set(SOUND_INSTALL_DIR        "share/sounds"            ) # The install dir for sound files
   set(TEMPLATES_INSTALL_DIR    "share/templates"         ) # The install dir for templates (Create new file...)
   set(WALLPAPER_INSTALL_DIR    "share/wallpapers"        ) # The install dir for wallpapers
   set(DEMO_INSTALL_DIR         "share/demos"             ) # The install dir for demos
   set(KCONF_UPDATE_INSTALL_DIR "share/apps/kconf_update" ) # The kconf_update install dir
   set(AUTOSTART_INSTALL_DIR    "share/autostart"         ) # The install dir for autostart files

   set(XDG_APPS_INSTALL_DIR      "share/applications/kde4"   ) # The XDG apps dir
   set(XDG_DIRECTORY_INSTALL_DIR "share/desktop-directories" ) # The XDG directory
   set(XDG_MIME_INSTALL_DIR      "share/mime/packages"       ) # The install dir for the xdg mimetypes

   set(SYSCONF_INSTALL_DIR       "etc"                       ) # The kde sysconfig install dir (default /etc)
   set(MAN_INSTALL_DIR           "share/man"                 ) # The kde man install dir (default ${SHARE_INSTALL_PREFIX}/man/)
   set(INFO_INSTALL_DIR          "share/info"                ) # The kde info install dir (default ${SHARE_INSTALL_PREFIX}/info)")
   set(DBUS_INTERFACES_INSTALL_DIR "share/dbus-1/interfaces" ) # The kde dbus interfaces install dir (default  ${SHARE_INSTALL_PREFIX}/dbus-1/interfaces)")
   set(DBUS_SERVICES_INSTALL_DIR "share/dbus-1/services"     ) # The kde dbus services install dir (default  ${SHARE_INSTALL_PREFIX}/dbus-1/services)")

else (WIN32)

   # This macro implements some very special logic how to deal with the cache.
   # By default the various install locations inherit their value from their "parent" variable
   # so if you set CMAKE_INSTALL_PREFIX, then EXEC_INSTALL_PREFIX, PLUGIN_INSTALL_DIR will
   # calculate their value by appending subdirs to CMAKE_INSTALL_PREFIX .
   # This would work completely without using the cache.
   # But if somebody wants e.g. a different EXEC_INSTALL_PREFIX this value has to go into
   # the cache, otherwise it will be forgotten on the next cmake run.
   # Once a variable is in the cache, it doesn't depend on its "parent" variables
   # anymore and you can only change it by editing it directly.
   # this macro helps in this regard, because as long as you don't set one of the
   # variables explicitely to some location, it will always calculate its value from its
   # parents. So modifying CMAKE_INSTALL_PREFIX later on will have the desired effect.
   # But once you decide to set e.g. EXEC_INSTALL_PREFIX to some special location
   # this will go into the cache and it will no longer depend on CMAKE_INSTALL_PREFIX.
   #
   # additionally if installing to the same location as kdelibs, the other install
   # directories are reused from the installed kdelibs
   macro(_SET_FANCY _var _value _comment)
      set(predefinedvalue "${_value}")
      if ("${CMAKE_INSTALL_PREFIX}" STREQUAL "${KDE4_INSTALL_DIR}" AND DEFINED KDE4_${_var})
         set(predefinedvalue "${KDE4_${_var}}")
      endif ("${CMAKE_INSTALL_PREFIX}" STREQUAL "${KDE4_INSTALL_DIR}" AND DEFINED KDE4_${_var})

      if (NOT DEFINED ${_var})
         set(${_var} ${predefinedvalue})
      else (NOT DEFINED ${_var})
         set(${_var} "${${_var}}" CACHE PATH "${_comment}")
      endif (NOT DEFINED ${_var})
   endmacro(_SET_FANCY)

   if(APPLE)
      set(BUNDLE_INSTALL_DIR "/Applications/KDE4" CACHE PATH "Directory where application bundles will be installed to on OSX" )
   endif(APPLE)

   _set_fancy(EXEC_INSTALL_PREFIX  "${CMAKE_INSTALL_PREFIX}"                 "Base directory for executables and libraries")
   _set_fancy(SHARE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/share"           "Base directory for files which go to share/")
   _set_fancy(BIN_INSTALL_DIR      "${EXEC_INSTALL_PREFIX}/bin"              "The install dir for executables (default ${EXEC_INSTALL_PREFIX}/bin)")
   _set_fancy(SBIN_INSTALL_DIR     "${EXEC_INSTALL_PREFIX}/sbin"             "The install dir for system executables (default ${EXEC_INSTALL_PREFIX}/sbin)")
   _set_fancy(LIB_INSTALL_DIR      "${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX}" "The subdirectory relative to the install prefix where libraries will be installed (default is ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX})")
   _set_fancy(LIBEXEC_INSTALL_DIR  "${LIB_INSTALL_DIR}/kde4/libexec"         "The subdirectory relative to the install prefix where libraries will be installed (default is ${LIB_INSTALL_DIR}/kde4/libexec)")
   _set_fancy(INCLUDE_INSTALL_DIR  "${CMAKE_INSTALL_PREFIX}/include"         "The subdirectory to the header prefix")

   _set_fancy(PLUGIN_INSTALL_DIR       "${LIB_INSTALL_DIR}/kde4"                "The subdirectory relative to the install prefix where plugins will be installed (default is ${LIB_INSTALL_DIR}/kde4)")
   _set_fancy(CONFIG_INSTALL_DIR       "${SHARE_INSTALL_PREFIX}/config"         "The config file install dir")
   _set_fancy(DATA_INSTALL_DIR         "${SHARE_INSTALL_PREFIX}/apps"           "The parent directory where applications can install their data")
   _set_fancy(HTML_INSTALL_DIR         "${SHARE_INSTALL_PREFIX}/doc/HTML"       "The HTML install dir for documentation")
   _set_fancy(ICON_INSTALL_DIR         "${SHARE_INSTALL_PREFIX}/icons"          "The icon install dir (default ${SHARE_INSTALL_PREFIX}/share/icons/)")
   _set_fancy(KCFG_INSTALL_DIR         "${SHARE_INSTALL_PREFIX}/config.kcfg"    "The install dir for kconfig files")
   _set_fancy(LOCALE_INSTALL_DIR       "${SHARE_INSTALL_PREFIX}/locale"         "The install dir for translations")
   _set_fancy(MIME_INSTALL_DIR         "${SHARE_INSTALL_PREFIX}/mimelnk"        "The install dir for the mimetype desktop files")
   _set_fancy(SERVICES_INSTALL_DIR     "${SHARE_INSTALL_PREFIX}/kde4/services"  "The install dir for service (desktop, protocol, ...) files")
   _set_fancy(SERVICETYPES_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/kde4/servicetypes" "The install dir for servicestypes desktop files")
   _set_fancy(SOUND_INSTALL_DIR        "${SHARE_INSTALL_PREFIX}/sounds"         "The install dir for sound files")
   _set_fancy(TEMPLATES_INSTALL_DIR    "${SHARE_INSTALL_PREFIX}/templates"      "The install dir for templates (Create new file...)")
   _set_fancy(WALLPAPER_INSTALL_DIR    "${SHARE_INSTALL_PREFIX}/wallpapers"     "The install dir for wallpapers")
   _set_fancy(DEMO_INSTALL_DIR         "${SHARE_INSTALL_PREFIX}/demos"          "The install dir for demos")
   _set_fancy(KCONF_UPDATE_INSTALL_DIR "${DATA_INSTALL_DIR}/kconf_update"       "The kconf_update install dir")
   _set_fancy(AUTOSTART_INSTALL_DIR    "${SHARE_INSTALL_PREFIX}/autostart"      "The install dir for autostart files")

   _set_fancy(XDG_APPS_INSTALL_DIR     "${SHARE_INSTALL_PREFIX}/applications/kde4"         "The XDG apps dir")
   _set_fancy(XDG_DIRECTORY_INSTALL_DIR "${SHARE_INSTALL_PREFIX}/desktop-directories"      "The XDG directory")
   _set_fancy(XDG_MIME_INSTALL_DIR     "${SHARE_INSTALL_PREFIX}/mime/packages"  "The install dir for the xdg mimetypes")

   _set_fancy(SYSCONF_INSTALL_DIR      "${CMAKE_INSTALL_PREFIX}/etc"            "The kde sysconfig install dir (default ${CMAKE_INSTALL_PREFIX}/etc)")
   _set_fancy(MAN_INSTALL_DIR          "${SHARE_INSTALL_PREFIX}/man"            "The kde man install dir (default ${SHARE_INSTALL_PREFIX}/man/)")
   _set_fancy(INFO_INSTALL_DIR         "${SHARE_INSTALL_PREFIX}/info"           "The kde info install dir (default ${SHARE_INSTALL_PREFIX}/info)")
   _set_fancy(DBUS_INTERFACES_INSTALL_DIR      "${SHARE_INSTALL_PREFIX}/dbus-1/interfaces" "The kde dbus interfaces install dir (default  ${SHARE_INSTALL_PREFIX}/dbus-1/interfaces)")
   _set_fancy(DBUS_SERVICES_INSTALL_DIR      "${SHARE_INSTALL_PREFIX}/dbus-1/services"     "The kde dbus services install dir (default  ${SHARE_INSTALL_PREFIX}/dbus-1/services)")

endif (WIN32)


# The INSTALL_TARGETS_DEFAULT_ARGS variable should be used when libraries are installed.
# It should also be used when installing applications, since then 
# on OS X application bundles will be installed to BUNDLE_INSTALL_DIR.
# The variable MUST NOT be used for installing plugins.
# It also MUST NOT be used for executables which are intended to go into sbin/ or libexec/.
#
# Usage is like this:
#    install(TARGETS kdecore kdeui ${INSTALL_TARGETS_DEFAULT_ARGS} )
#
# This will install libraries correctly under UNIX, OSX and Windows (i.e. dll's go
# into bin/.
# Later on it will be possible to extend this for installing OSX frameworks
# The COMPONENT Devel argument has the effect that static libraries belong to the 
# "Devel" install component. If we use this also for all install() commands
# for header files, it will be possible to install
#   -everything: make install OR cmake -P cmake_install.cmake
#   -only the development files: cmake -DCOMPONENT=Devel -P cmake_install.cmake
#   -everything except the development files: cmake -DCOMPONENT=Unspecified -P cmake_install.cmake
# This can then also be used for packaging with cpack.

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel )


# on the Mac support an extra install directory for application bundles starting with cmake 2.6
if(APPLE)
   set(INSTALL_TARGETS_DEFAULT_ARGS  ${INSTALL_TARGETS_DEFAULT_ARGS}
                               BUNDLE DESTINATION "${BUNDLE_INSTALL_DIR}" )
endif(APPLE)


##############  add some more default search paths  ###############
#
# the KDE4_xxx_INSTALL_DIR variables are empty when building kdelibs itself
# and otherwise point to the kde4 install dirs

set(CMAKE_SYSTEM_INCLUDE_PATH ${CMAKE_SYSTEM_INCLUDE_PATH}
                              "${KDE4_INCLUDE_INSTALL_DIR}")

set(CMAKE_SYSTEM_PROGRAM_PATH ${CMAKE_SYSTEM_PROGRAM_PATH}
                              "${KDE4_BIN_INSTALL_DIR}" )

set(CMAKE_SYSTEM_LIBRARY_PATH ${CMAKE_SYSTEM_LIBRARY_PATH} 
                              "${KDE4_LIB_INSTALL_DIR}" )

# under Windows dlls may be also installed in bin/
if(WIN32)
  set(CMAKE_SYSTEM_LIBRARY_PATH ${CMAKE_SYSTEM_LIBRARY_PATH} 
                                "${_CMAKE_INSTALL_DIR}/bin" 
                                "${CMAKE_INSTALL_PREFIX}/bin" )
endif(WIN32)


######################################################
#  and now the platform specific stuff
######################################################

# Set a default build type for single-configuration
# CMake generators if no build type is set.
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)


if (WIN32)

   if(CYGWIN)
      message(FATAL_ERROR "Cygwin is NOT supported, use mingw or MSVC to build KDE4.")
   endif(CYGWIN)

   find_package(KDEWIN32 REQUIRED)

   # limit win32 packaging to kdelibs at now 
   # don't know if package name, version and notes are always available 
   if(_kdeBootStrapping)
      find_package(KDEWIN_Packager)
      if (KDEWIN_PACKAGER_FOUND)
         kdewin_packager("kdelibs" "${KDE_VERSION}" "KDE base library" "")
      endif (KDEWIN_PACKAGER_FOUND)

      include(Win32Macros)
      addExplorerWrapper("kdelibs")
   endif(_kdeBootStrapping)

   set( _KDE4_PLATFORM_INCLUDE_DIRS ${KDEWIN32_INCLUDES})

   # if we are compiling kdelibs, add KDEWIN32_LIBRARIES explicitely,
   # otherwise they come from KDELibsDependencies.cmake, Alex
   if (_kdeBootStrapping)
      set( KDE4_KDECORE_LIBS ${KDE4_KDECORE_LIBS} ${KDEWIN32_LIBRARIES} )
   endif (_kdeBootStrapping)

   # we prefer to use a different postfix for debug libs only on Windows
   # does not work atm
   set(CMAKE_DEBUG_POSTFIX "")

   # windows, microsoft compiler
   if(MSVC)
      set( _KDE4_PLATFORM_DEFINITIONS -DKDE_FULL_TEMPLATE_EXPORT_INSTANTIATION -DWIN32_LEAN_AND_MEAN -DUNICODE )
      # C4250: 'class1' : inherits 'class2::member' via dominance
      set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4250" )
      # C4251: 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
      set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4251" )
      # C4396: 'identifier' : 'function' the inline specifier cannot be used when a friend declaration refers to a specialization of a function template
      set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4396" )
      if(CMAKE_COMPILER_2005)
         # to avoid a lot of deprecated warnings
         add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS )
         # 'identifier' : no suitable definition provided for explicit template instantiation request
         set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4661" )
      endif(CMAKE_COMPILER_2005)
   endif(MSVC)


   # for visual studio IDE set the path correctly for custom commands
   # maybe under windows bat-files should be generated for running apps during the build
   if(MSVC_IDE)
     get_filename_component(PERL_LOCATION "${PERL_EXECUTABLE}" PATH)
     file(TO_NATIVE_PATH "${PERL_LOCATION}" PERL_PATH_WINDOWS)
     file(TO_NATIVE_PATH "${QT_BINARY_DIR}" QT_BIN_DIR_WINDOWS)
     set(CMAKE_MSVCIDE_RUN_PATH "${PERL_PATH_WINDOWS}\;${QT_BIN_DIR_WINDOWS}"
       CACHE STATIC "MSVC IDE Run path" FORCE)
   endif(MSVC_IDE)
endif (WIN32)


# setup default RPATH/install_name handling, may be overridden by KDE4_HANDLE_RPATH_FOR_EXECUTABLE
# It sets up to build with full RPATH. When installing, RPATH will be changed to the LIB_INSTALL_DIR
# and all link directories which are not inside the current build dir.
if (UNIX)
   set( _KDE4_PLATFORM_INCLUDE_DIRS)

   # the rest is RPATH handling
   # here the defaults are set
   # which are partly overwritten in kde4_handle_rpath_for_library()
   # and kde4_handle_rpath_for_executable(), both located in KDE4Macros.cmake, Alex
   if (APPLE)
      set(CMAKE_INSTALL_NAME_DIR ${LIB_INSTALL_DIR})
   else (APPLE)
      # add our LIB_INSTALL_DIR to the RPATH and use the RPATH figured out by cmake when compiling
      set(CMAKE_INSTALL_RPATH ${LIB_INSTALL_DIR} )
      set(CMAKE_SKIP_BUILD_RPATH FALSE)
      set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
      set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
   endif (APPLE)
endif (UNIX)


if (Q_WS_X11)
   # Done by FindQt4.cmake already
   #find_package(X11 REQUIRED)
   # UNIX has already set _KDE4_PLATFORM_INCLUDE_DIRS, so append
   set(_KDE4_PLATFORM_INCLUDE_DIRS ${_KDE4_PLATFORM_INCLUDE_DIRS} ${X11_INCLUDE_DIR} )
endif (Q_WS_X11)


# This will need to be modified later to support either Qt/X11 or Qt/Mac builds
if (APPLE)

  set ( _KDE4_PLATFORM_DEFINITIONS -D__APPLE_KDE__ )

  # we need to set MACOSX_DEPLOYMENT_TARGET to (I believe) at least 10.2 or maybe 10.3 to allow
  # -undefined dynamic_lookup; in the future we should do this programmatically
  # hmm... why doesn't this work?
  set (ENV{MACOSX_DEPLOYMENT_TARGET} 10.3)

  # "-undefined dynamic_lookup" means we don't care about missing symbols at link-time by default
  # this is bad, but unavoidable until there is the equivalent of libtool -no-undefined implemented
  # or perhaps it already is, and I just don't know where to look  ;)

  set (CMAKE_SHARED_LINKER_FLAGS "-single_module -multiply_defined suppress ${CMAKE_SHARED_LINKER_FLAGS}")
  set (CMAKE_MODULE_LINKER_FLAGS "-multiply_defined suppress ${CMAKE_MODULE_LINKER_FLAGS}")
  #set(CMAKE_SHARED_LINKER_FLAGS "-single_module -undefined dynamic_lookup -multiply_defined suppress")
  #set(CMAKE_MODULE_LINKER_FLAGS "-undefined dynamic_lookup -multiply_defined suppress")

  # we profile...
  if(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)
    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
  endif(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)

  # removed -Os, was there a special reason for using -Os instead of -O2 ?, Alex
  # optimization flags are set below for the various build types
  set (CMAKE_C_FLAGS     "${CMAKE_C_FLAGS} -fno-common")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common")
endif (APPLE)


if (CMAKE_SYSTEM_NAME MATCHES Linux)
   if (CMAKE_COMPILER_IS_GNUCXX)
      set ( _KDE4_PLATFORM_DEFINITIONS -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_GNU_SOURCE)
      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")

      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_MODULE_LINKER_FLAGS}")
      set ( CMAKE_EXE_LINKER_FLAGS "-Wl,--enable-new-dtags ${CMAKE_EXE_LINKER_FLAGS}")

      # we profile...
      if(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)
        set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
        set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
      endif(CMAKE_BUILD_TYPE_TOLOWER MATCHES profile)
   endif (CMAKE_COMPILER_IS_GNUCXX)
   if (CMAKE_C_COMPILER MATCHES "icc")
      set ( _KDE4_PLATFORM_DEFINITIONS -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -D_GNU_SOURCE)
      set ( CMAKE_SHARED_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_SHARED_LINKER_FLAGS}")
      set ( CMAKE_MODULE_LINKER_FLAGS "-Wl,--fatal-warnings -Wl,--no-undefined -lc ${CMAKE_MODULE_LINKER_FLAGS}")
   endif (CMAKE_C_COMPILER MATCHES "icc")
endif (CMAKE_SYSTEM_NAME MATCHES Linux)

if (UNIX)
   set ( _KDE4_PLATFORM_DEFINITIONS "${_KDE4_PLATFORM_DEFINITIONS} -D_LARGEFILE64_SOURCE")

   check_cxx_source_compiles("
#include <sys/types.h>
 /* Check that off_t can represent 2**63 - 1 correctly.
    We can't simply define LARGE_OFF_T to be 9223372036854775807,
    since some C++ compilers masquerading as C compilers
    incorrectly reject 9223372036854775807.  */
#define LARGE_OFF_T (((off_t) 1 << 62) - 1 + ((off_t) 1 << 62))

  int off_t_is_large[(LARGE_OFF_T % 2147483629 == 721 && LARGE_OFF_T % 2147483647 == 1) ? 1 : -1];
  int main() { return 0; }
" _OFFT_IS_64BIT)

   if (NOT _OFFT_IS_64BIT)
     set ( _KDE4_PLATFORM_DEFINITIONS "${_KDE4_PLATFORM_DEFINITIONS} -D_FILE_OFFSET_BITS=64")
   endif (NOT _OFFT_IS_64BIT)
endif (UNIX)

if (CMAKE_SYSTEM_NAME MATCHES BSD)
   set ( _KDE4_PLATFORM_DEFINITIONS -D_GNU_SOURCE )
   set ( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lc")
   set ( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -lc")
endif (CMAKE_SYSTEM_NAME MATCHES BSD)


############################################################
# compiler specific settings
############################################################


# this macro is for internal use only.
macro(KDE_CHECK_FLAG_EXISTS FLAG VAR DOC)
   if(NOT ${VAR} MATCHES "${FLAG}")
      set(${VAR} "${${VAR}} ${FLAG}" CACHE STRING "Flags used by the linker during ${DOC} builds." FORCE)
   endif(NOT ${VAR} MATCHES "${FLAG}")
endmacro(KDE_CHECK_FLAG_EXISTS FLAG VAR)

if (MSVC)
   set (KDE4_ENABLE_EXCEPTIONS -EHsc)

   # make sure that no header adds libcmt by default using #pragma comment(lib, "libcmt.lib") as done by mfc/afx.h
   kde_check_flag_exists("/NODEFAULTLIB:libcmt /DEFAULTLIB:msvcrt" CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "Release with Debug Info")
   kde_check_flag_exists("/NODEFAULTLIB:libcmt /DEFAULTLIB:msvcrt" CMAKE_EXE_LINKER_FLAGS_RELEASE "release")
   kde_check_flag_exists("/NODEFAULTLIB:libcmt /DEFAULTLIB:msvcrt" CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "release minsize")
   kde_check_flag_exists("/NODEFAULTLIB:libcmtd /DEFAULTLIB:msvcrtd" CMAKE_EXE_LINKER_FLAGS_DEBUG "debug")
endif(MSVC)


if (CMAKE_COMPILER_IS_GNUCXX)
   set (KDE4_ENABLE_EXCEPTIONS -fexceptions)
   # Select flags.
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
   set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
   set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline")
   set(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
   set(CMAKE_CXX_FLAGS_DEBIAN         "-DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
   set(CMAKE_C_FLAGS_RELEASE          "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_C_FLAGS_DEBUG            "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
   set(CMAKE_C_FLAGS_DEBUGFULL        "-g3 -fno-inline")
   set(CMAKE_C_FLAGS_PROFILE          "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
   set(CMAKE_C_FLAGS_DEBIAN           "-DNDEBUG -DQT_NO_DEBUG")

   if (CMAKE_SYSTEM_NAME MATCHES Linux)
     set ( CMAKE_C_FLAGS     "${CMAKE_C_FLAGS} -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
     set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -fno-check-new -fno-common")
     add_definitions (-D_BSD_SOURCE)
   endif (CMAKE_SYSTEM_NAME MATCHES Linux)

   # gcc under Windows
   if (MINGW)
      set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--export-all-symbols -Wl,--disable-auto-import")
      set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--export-all-symbols -Wl,--disable-auto-import")

      # we always link against the release version of QT with mingw
      # (even for debug builds). So we need to define QT_NO_DEBUG
      # or else QPluginLoader rejects plugins because it thinks
      # they're built against the wrong QT.
      add_definitions(-DQT_NO_DEBUG)
   endif (MINGW)

   check_cxx_compiler_flag(-fPIE HAVE_FPIE_SUPPORT)
   if(KDE4_ENABLE_FPIE)
       if(HAVE_FPIE_SUPPORT)
        set (KDE4_CXX_FPIE_FLAGS "-fPIE")
        set (KDE4_PIE_LDFLAGS "-pie")
       else(HAVE_FPIE_SUPPORT)
        message(STATUS "Your compiler doesn't support the PIE flag")
       endif(HAVE_FPIE_SUPPORT)
   endif(KDE4_ENABLE_FPIE)

   check_cxx_compiler_flag(-Woverloaded-virtual __KDE_HAVE_W_OVERLOADED_VIRTUAL)
   if(__KDE_HAVE_W_OVERLOADED_VIRTUAL)
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")
   endif(__KDE_HAVE_W_OVERLOADED_VIRTUAL)

   # visibility support
   check_cxx_compiler_flag(-fvisibility=hidden __KDE_HAVE_GCC_VISIBILITY)
   set( __KDE_HAVE_GCC_VISIBILITY ${__KDE_HAVE_GCC_VISIBILITY} CACHE BOOL "GCC support for hidden visibility")

   # get the gcc version
   exec_program(${CMAKE_C_COMPILER} ARGS --version OUTPUT_VARIABLE _gcc_version_info)

   string (REGEX MATCH "[345]\\.[0-9]\\.[0-9]" _gcc_version "${_gcc_version_info}")
   # gcc on mac just reports: "gcc (GCC) 3.3 20030304 ..." without the patch level, handle this here:
   if (NOT _gcc_version)
      string (REGEX REPLACE ".*\\(GCC\\).* ([34]\\.[0-9]) .*" "\\1.0" _gcc_version "${_gcc_version_info}")
   endif (NOT _gcc_version)

   macro_ensure_version("4.1.0" "${_gcc_version}" GCC_IS_NEWER_THAN_4_1)
   macro_ensure_version("4.2.0" "${_gcc_version}" GCC_IS_NEWER_THAN_4_2)
   macro_ensure_version("4.3.0" "${_gcc_version}" GCC_IS_NEWER_THAN_4_3)

   # save a little by making local statics not threadsafe
   # ### do not enable it for older compilers, see
   # ### http://gcc.gnu.org/bugzilla/show_bug.cgi?id=31806
   if (GCC_IS_NEWER_THAN_4_3)
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-threadsafe-statics")
   endif (GCC_IS_NEWER_THAN_4_3)

   set(_GCC_COMPILED_WITH_BAD_ALLOCATOR FALSE)
   if (GCC_IS_NEWER_THAN_4_1)
      exec_program(${CMAKE_C_COMPILER} ARGS -v OUTPUT_VARIABLE _gcc_alloc_info)
      string(REGEX MATCH "(--enable-libstdcxx-allocator=mt)" _GCC_COMPILED_WITH_BAD_ALLOCATOR "${_gcc_alloc_info}")
   endif (GCC_IS_NEWER_THAN_4_1)

   if (__KDE_HAVE_GCC_VISIBILITY AND GCC_IS_NEWER_THAN_4_1 AND NOT _GCC_COMPILED_WITH_BAD_ALLOCATOR)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
      set (KDE4_C_FLAGS "-fvisibility=hidden")
      # check that Qt defines Q_DECL_EXPORT as __attribute__ ((visibility("default")))
      # if it doesn't and KDE compiles with hidden default visibiltiy plugins will break
      set(_source "#include <QtCore/QtGlobal>\n int main()\n {\n #ifdef QT_VISIBILITY_AVAILABLE \n return 0;\n #else \n return 1; \n #endif \n }\n")
      set(_source_file ${CMAKE_BINARY_DIR}/CMakeTmp/check_qt_visibility.cpp)
      file(WRITE "${_source_file}" "${_source}")
      set(_include_dirs "")

      try_run(_run_result _compile_result ${CMAKE_BINARY_DIR} ${_source_file} CMAKE_FLAGS "${_include_dirs}" COMPILE_OUTPUT_VARIABLE _compile_output_var)

      if(NOT _compile_result)
         message(FATAL_ERROR "Could not compile simple test program:\n ${_source}\n${_compile_output_var}")
      endif(NOT _compile_result)
      if(_run_result)
         message(FATAL_ERROR "Qt compiled without support for -fvisibility=hidden. This will break plugins and linking of some applications. Please fix your Qt installation.")
      endif(_run_result)

      if (GCC_IS_NEWER_THAN_4_2)
          set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
      endif (GCC_IS_NEWER_THAN_4_2)
   else (__KDE_HAVE_GCC_VISIBILITY AND GCC_IS_NEWER_THAN_4_1 AND NOT _GCC_COMPILED_WITH_BAD_ALLOCATOR)
      set (__KDE_HAVE_GCC_VISIBILITY 0)
   endif (__KDE_HAVE_GCC_VISIBILITY AND GCC_IS_NEWER_THAN_4_1 AND NOT _GCC_COMPILED_WITH_BAD_ALLOCATOR)

endif (CMAKE_COMPILER_IS_GNUCXX)


if (CMAKE_C_COMPILER MATCHES "icc")

   set (KDE4_ENABLE_EXCEPTIONS -fexceptions)
   # Select flags.
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
   set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_CXX_FLAGS_DEBUG          "-O2 -g -fno-inline -noalign")
   set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g -fno-inline -noalign")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
   set(CMAKE_C_FLAGS_RELEASE          "-O2 -DNDEBUG -DQT_NO_DEBUG")
   set(CMAKE_C_FLAGS_DEBUG            "-O2 -g -fno-inline -noalign")
   set(CMAKE_C_FLAGS_DEBUGFULL        "-g -fno-inline -noalign")

   set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -ansi -Wall -w1 -Wpointer-arith -fno-common")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -Wall -w1 -Wpointer-arith -fno-exceptions -fno-common")

   # visibility support
   set(__KDE_HAVE_ICC_VISIBILITY)
#   check_cxx_compiler_flag(-fvisibility=hidden __KDE_HAVE_ICC_VISIBILITY)
#   if (__KDE_HAVE_ICC_VISIBILITY)
#      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
#   endif (__KDE_HAVE_ICC_VISIBILITY)

endif (CMAKE_C_COMPILER MATCHES "icc")


###########    end of platform specific stuff  ##########################


# KDE4Macros.cmake contains all the KDE specific macros
include(${kde_cmake_module_dir}/KDE4Macros.cmake)


# decide whether KDE4 has been found
set(KDE4_FOUND FALSE)
if (KDE4_INCLUDE_DIR AND KDE4_LIB_DIR AND KDE4_KCFGC_EXECUTABLE AND KDE4_INSTALLED_VERSION_OK)
   set(KDE4_FOUND TRUE)
endif (KDE4_INCLUDE_DIR AND KDE4_LIB_DIR AND KDE4_KCFGC_EXECUTABLE AND KDE4_INSTALLED_VERSION_OK)


macro (KDE4_PRINT_RESULTS)

   # inside kdelibs the include dir and lib dir are internal, not "found"
   if (NOT _kdeBootStrapping)
       if(KDE4_INCLUDE_DIR)
          message(STATUS "Found KDE 4.2 include dir: ${KDE4_INCLUDE_DIR}")
       else(KDE4_INCLUDE_DIR)
          message(STATUS "ERROR: unable to find the KDE 4 headers")
       endif(KDE4_INCLUDE_DIR)

       if(KDE4_LIB_DIR)
          message(STATUS "Found KDE 4.2 library dir: ${KDE4_LIB_DIR}")
       else(KDE4_LIB_DIR)
          message(STATUS "ERROR: unable to find the KDE 4 core library")
       endif(KDE4_LIB_DIR)
   endif (NOT _kdeBootStrapping)

   if(KDE4_KCFGC_EXECUTABLE)
      message(STATUS "Found the KDE4 kconfig_compiler preprocessor: ${KDE4_KCFGC_EXECUTABLE}")
   else(KDE4_KCFGC_EXECUTABLE)
      message(STATUS "Didn't find the KDE4 kconfig_compiler preprocessor")
   endif(KDE4_KCFGC_EXECUTABLE)

   if(AUTOMOC4_EXECUTABLE)
      message(STATUS "Found automoc4: ${AUTOMOC4_EXECUTABLE}")
   else(AUTOMOC4_EXECUTABLE)
      message(STATUS "Didn't find automoc4")
   endif(AUTOMOC4_EXECUTABLE)
endmacro (KDE4_PRINT_RESULTS)


if (KDE4Internal_FIND_REQUIRED AND NOT KDE4_FOUND)
   #bail out if something wasn't found
   kde4_print_results()
   if (NOT KDE4_INSTALLED_VERSION_OK)
     message(FATAL_ERROR "ERROR: the installed kdelibs version ${KDE_VERSION} is too old, at least version ${KDE_MIN_VERSION} is required")
   endif (NOT KDE4_INSTALLED_VERSION_OK)

   if (NOT KDE4_KCFGC_EXECUTABLE)
     message(FATAL_ERROR "ERROR: could not detect a usable kconfig_compiler")
   endif (NOT KDE4_KCFGC_EXECUTABLE)

   message(FATAL_ERROR "ERROR: could NOT find everything required for compiling KDE 4 programs")
endif (KDE4Internal_FIND_REQUIRED AND NOT KDE4_FOUND)

find_package(Phonon REQUIRED)
set(KDE4_PHONON_LIBRARY ${PHONON_LIBRARY})
set(KDE4_PHONON_LIBS ${PHONON_LIBS})
set(KDE4_PHONON_INCLUDES ${PHONON_INCLUDES})

if (NOT KDE4Internal_FIND_QUIETLY)
   kde4_print_results()
endif (NOT KDE4Internal_FIND_QUIETLY)

#add the found Qt and KDE include directories to the current include path
#the ${KDE4_INCLUDE_DIR}/KDE directory is for forwarding includes, eg. #include <KMainWindow>
set(KDE4_INCLUDES
   ${KDE4_INCLUDE_DIR}
   ${KDE4_INCLUDE_DIR}/KDE
   ${KDE4_PHONON_INCLUDES}
   ${_KDE4_PLATFORM_INCLUDE_DIRS} 
)

set(KDE4_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS} -DQT_NO_STL -DQT_NO_CAST_TO_ASCII -D_REENTRANT -DKDE_DEPRECATED_WARNINGS )

if (NOT _kde4_uninstall_rule_created)
   set(_kde4_uninstall_rule_created TRUE)

   configure_file("${kde_cmake_module_dir}/kde4_cmake_uninstall.cmake.in" "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake" @ONLY)

   add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")

endif (NOT _kde4_uninstall_rule_created)

endif(NOT KDE4_FOUND)
