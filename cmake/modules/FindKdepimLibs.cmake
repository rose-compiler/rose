# Find if we installed kdepimlibs before to compile it
# Once done this will define
#
#  KDEPIMLIBS_FOUND - system has KDE PIM Libraries
#  KDEPIMLIBS_INCLUDE_DIR - the KDE PIM Libraries include directory
#
# It also sets variables for the following libraries:
#   KDEPIMLIBS_AKONADI_LIBS
#   KDEPIMLIBS_AKONADI_KMIME_LIBS
#   KDEPIMLIBS_AKONADI_KABC_LIBS
#   KDEPIMLIBS_GPGMEPP_LIBS
#   KDEPIMLIBS_KABC_LIBS
#   KDEPIMLIBS_KBLOG_LIBS
#   KDEPIMLIBS_KCAL_LIBS
#   KDEPIMLIBS_KIMAP_LIBS
#   KDEPIMLIBS_KLDAP_LIBS
#   KDEPIMLIBS_KMIME_LIBS
#   KDEPIMLIBS_KPIMIDENTITIES_LIBS
#   KDEPIMLIBS_KPIMUTILS_LIBS
#   KDEPIMLIBS_KRESOURCES_LIBS
#   KDEPIMLIBS_KTNEF_LIBS
#   KDEPIMLIBS_KXMLRPCCLIENT_LIBS
#   KDEPIMLIBS_MAILTRANSPORT_LIBS
#   KDEPIMLIBS_QGPGME_LIBS
#   KDEPIMLIBS_SYNDICATION_LIBS
#
# And the following locations:
#   KDEPIMLIBS_DATA_DIR
#   KDEPIMLIBS_DBUS_INTERFACES_DIR
#   KDEPIMLIBS_DBUS_SERVICES_DIR
#   KDEPIMLIBS_INCLUDE_DIR
#   KDEPIMLIBS_LIB_DIR
#   KDEPIMLIBS_BIN_DIR
#   KDEPIMLIBS_LIBEXEC_DIR
#   KDEPIMLIBS_SBIN_DIR
#   KDEPIMLIBS_HTML_DIR
#   KDEPIMLIBS_CONFIG_DIR
#   KDEPIMLIBS_ICON_DIR
#   KDEPIMLIBS_KCFG_DIR
#   KDEPIMLIBS_LOCALE_DIR
#   KDEPIMLIBS_MIME_DIR
#   KDEPIMLIBS_SOUND_DIR
#   KDEPIMLIBS_TEMPLATES_DIR
#   KDEPIMLIBS_KCONF_UPDATE_DIR
#   KDEPIMLIBS_AUTOSTART_DIR
#   KDEPIMLIBS_XDG_APPS_DIR
#   KDEPIMLIBS_XDG_DIRECTORY_DIR
#   KDEPIMLIBS_SYSCONF_DIR
#   KDEPIMLIBS_MAN_DIR
#   KDEPIMLIBS_INFO_DIR
#   KDEPIMLIBS_SERVICES_DIR
#   KDEPIMLIBS_SERVICETYPES_DIR


# Copyright (c) 2008, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# The find_package() call below loads the file KdepimLibsConfig.cmake file.
# This file is created and installed by kdepimlibs/CMakeLists.txt
# It contains settings for all install location of kdepimlibs, as e.g.
# KDEPIMLIBS_INCLUDE_DIR, and also variables for all libraries.
# See kdepimlibs/CMakeLists.txt and kdepimlibs/KdepimLibsConfig.cmake.in 
# for details. Alex


set(_KdepimLibs_FIND_QUIETLY  ${KdepimLibs_FIND_QUIETLY})
find_package(KdepimLibs ${KdepimLibs_FIND_VERSION} QUIET NO_MODULE PATHS ${KDE4_LIB_DIR}/KdepimLibs/cmake )
set(KdepimLibs_FIND_QUIETLY ${_KdepimLibs_FIND_QUIETLY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KdepimLibs DEFAULT_MSG KdepimLibs_CONFIG )

