include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/roseQt/UseQtWidgets.inc

# Path where includes are installed
roseQtWidgetsIncPath = $(includedir)/roseQtWidgets


# The includes for compiling RoseQtWidgets

ROSE_QT_WIDGETS_ROOT = $(top_srcdir)/projects/roseQt/widgets
INCLUDES = -I. \
           -I$(ROSE_QT_WIDGETS_ROOT)\
           $(ROSE_INCLUDES)\
           $(QT_INCLUDES)\
           $(ROSEQT_INCLUDES)\
           -I$(ROSE_QT_WIDGETS_ROOT)/src
              

