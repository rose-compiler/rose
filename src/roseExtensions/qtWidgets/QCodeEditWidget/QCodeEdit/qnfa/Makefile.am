#------------ qnfa Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libqnfa.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/..  -I$(srcdir)/../document -I$(srcdir)/../widgets 

# End of Makefile.am.pre Content

libqnfa_la_SOURCES = qnfa.cpp\
                     qnfadefinition.cpp\
                     xml2qnfa.cpp





EXTRA_DIST = 

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/QCodeEdit
headerTarget_HEADERS = light_vector.h\
                       qnfa.h\
                       qnfadefinition.h


endif
