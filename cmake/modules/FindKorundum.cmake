# - Find Korundum - the KDE Ruby bindings
#
# This module finds if Korundum is installed.
# It defines the following variables:
#  KORUNDUM_PATH - the path to the korundum ruby file
#  KORUNDUM_FOUND - true if it has been found

# Copyright (c) 2006, Egon Willighagen, <egonw@users.sf.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(KORUNDUM_PATH Korundum.rb /usr/lib/ruby/1.8)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Korundum  DEFAULT_MSG  KORUNDUM_PATH)

# just for compat.:
set(Korumdum_PATH ${KORUNDUM_PATH})
set(Korumdum_FOUND ${KORUNDUM_FOUND})
