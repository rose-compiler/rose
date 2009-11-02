# cmake macro to see if we have kdepim installed

# KDEPIM_INCLUDE_DIR
# KDEPIM_FOUND
# Copyright (C) 2007 Laurent Montel <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (KDEPIM_INCLUDE_DIR)
    # Already in cache, be silent
    set(KDEPIM_FOUND TRUE)
endif (KDEPIM_INCLUDE_DIR)


FIND_PATH(KDEPIM_INCLUDE_DIR NAMES kdepimprotocols.h
	PATHS
	${INCLUDE_INSTALL_DIR}
)

FIND_LIBRARY(KDEPIM_LIBRARIES NAMES kdepim
	PATHS
	${LIB_INSTALL_DIR}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Kdepim DEFAULT_MSG KDEPIM_LIBRARIES KDEPIM_INCLUDE_DIR )

MARK_AS_ADVANCED(KDEPIM_INCLUDE_DIR KDEPIM_LIBRARIES)

