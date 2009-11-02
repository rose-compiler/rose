
# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

#using a ui3 file with uic3
if (KDE3_IMPL)

  EXECUTE_PROCESS(COMMAND ${KDE_UIC_EXECUTABLE}
    -nounload -tr tr2i18n
    -impl ${KDE_UIC_H_FILE}
    ${KDE_UIC_FILE}
    OUTPUT_VARIABLE _uic_CONTENTS
    ERROR_QUIET
  )
endif (KDE3_IMPL)


if (KDE3_HEADER)

  EXECUTE_PROCESS(COMMAND ${KDE_UIC_EXECUTABLE}
    -nounload -tr tr2i18n
    ${KDE_UIC_FILE}
    OUTPUT_VARIABLE _uic_CONTENTS
    ERROR_QUIET
  )
   set(KDE_UIC_CPP_FILE ${KDE_UIC_H_FILE})
endif (KDE3_HEADER)

# the kde4 branch
if (KDE4_HEADER)

  EXECUTE_PROCESS(COMMAND ${KDE_UIC_EXECUTABLE}
    -tr tr2i18n
    ${KDE_UIC_FILE}
    OUTPUT_VARIABLE _uic_CONTENTS
    ERROR_QUIET
  )

   set(KDE_UIC_CPP_FILE ${KDE_UIC_H_FILE})
endif (KDE4_HEADER)


#replace tr218n("") with QString::null to avoid warning from KLocale
STRING(REGEX REPLACE "tr2i18n\\(\"\"\\)" "QString::null" _uic_CONTENTS "${_uic_CONTENTS}" )
STRING(REGEX REPLACE "tr2i18n\\(\"\", \"\"\\)" "QString::null" _uic_CONTENTS "${_uic_CONTENTS}" )
#replace image15_data with img15_filename to make enable_final work
STRING(REGEX REPLACE "image([0-9]+)_data" "img\\1_${KDE_UIC_BASENAME}" _uic_CONTENTS "${_uic_CONTENTS}")
#fixup include guard
STRING(REGEX REPLACE "#ifndef " "#ifndef UI_" _uic_CONTENTS "${_uic_CONTENTS}")
STRING(REGEX REPLACE "#define " "#define UI_" _uic_CONTENTS "${_uic_CONTENTS}")

FILE(WRITE ${KDE_UIC_CPP_FILE} "#include <kdialog.h>\n#include <klocale.h>\n\n${_uic_CONTENTS}\n")

