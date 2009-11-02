# - Try to find msgfmt 
# Once done this will define
#
#  MSGFMT_FOUND - system has msgfmt

# Copyright (c) 2007, Montel Laurent <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

### TODO: KDE4 needs msgfmt of version 0.15 or greater (cf. msgfmt --version )

if(MSGFMT_EXECUTABLE)
  set(MSGFMT_FOUND TRUE)
else(MSGFMT_EXECUTABLE)

  FIND_PROGRAM(MSGFMT_EXECUTABLE NAMES msgfmt)
  if (MSGFMT_EXECUTABLE)
    set(MSGFMT_FOUND TRUE)
  else (MSGFMT_EXECUTABLE)
    if (Msgfmt_FIND_REQUIRED)
      message(SEND_ERROR "Could NOT find msgfmt program")
    endif (Msgfmt_FIND_REQUIRED)
  endif (MSGFMT_EXECUTABLE)
  MARK_AS_ADVANCED(MSGFMT_EXECUTABLE)

endif (MSGFMT_EXECUTABLE)

