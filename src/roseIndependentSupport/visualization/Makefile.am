include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#  ROSE_USE_VISUALIZATION
if ROSE_USE_VISUALIZATION

# FLTK_INCLUDE     = -I/home/thuerey1/local/include/ 
# GRAPHVIZ_INCLUDE = -I/home/thuerey1/local/include/graphviz
# OPENGL_INCLUDE   =
# FLTK_LIBS     = -L/home/thuerey1/local/lib/ -L/usr/X11R6/lib -lfltk -lfltk_gl -lX11
# GRAPHVIZ_LIBS = -L/home/thuerey1/local/lib/graphviz  -ldotneato -lfreetype -lpng -ljpeg -lz
# OPENGL_LIBS   = -lGL -lGLU -lglut

FLTK_INCLUDE     = -I@FLTK_include_path@
GRAPHVIZ_INCLUDE = -I@GraphViz_include_path@
GLUT_INCLUDE     = -I@GLUT_include_path@

FLTK_LIBS     = -L@FLTK_libs_path@ -L/usr/X11R6/lib -lfltk -lfltk_gl -lX11
GRAPHVIZ_LIBS = -L@GraphViz_libs_path@ -ldotneato -lfreetype -lpng -ljpeg -lz
GLUT_LIBS     = -L@GLUT_libs_path@
OPENGL_LIBS   = -lGL -lGLU -lglut

#FLTK_LIBS     = -lfltk -lfltk_gl -lX11
#GRAPHVIZ_LIBS = -ldotneato -lfreetype -lpng -ljpeg -lz
#OPENGL_LIBS   = -lGL -lGLU -lglut

# DQ (4/20/2005): New variables that are required (from Andreas)
# LDFLAGS = -L/usr/local/lib/graphviz/ -L/usr/lib/ -L/usr/X11/lib/ -L/usr/local/lib/
# LD_INCLUDE_PATH = /usr/lib/:/usr/include/:/usr/local/include/graphviz:/usr/X11/include/:$LD_INCLUDE_PATH
# CPPFLAGS = -I/usr/local/include/graphviz/ -I/usr/include/ -I/usr/X11/include/
# CFLAGS   = -I/usr/local/include/graphviz/ -I/usr/include/ -I/usr/X11/include/

# LDFLAGS = -L/usr/local/lib/graphviz/ -L/usr/lib/ -L/usr/X11R6/lib/ -L/usr/local/lib/
# CPPFLAGS = -I/usr/local/include/graphviz/ -I/usr/X11R6/include/
# CFLAGS   = -I/usr/local/include/graphviz/ -I/usr/X11R6/include/

AM_CPPFLAGS =  $(FLTK_INCLUDE) $(GRAPHVIZ_INCLUDE) $(GLUT_INCLUDE) $(OPENGL_INCLUDE)

bin_PROGRAMS = dotgl

dotgl_SOURCES = \
     dotvisualizer.cpp \
     glcodegen.cpp \
     glf.c \
     graphinfo.cpp \
     mapcodegen.cpp \
     vizgroup.cpp \
     main.cpp

DEPENDENCIES = 

# Header files that need to be included in the distribution
pkginclude_HEADERS = \
     dotvisualizer.h \
     glcodegen.h \
     glf.h \
     graphinfo.h \
     mapcodegen.h \
     style.h \
     vizgroup.h \
     ntl_vector3dim.h

# LDFLAGS = -Xlinker -rpath/home/thuerey1/local/lib/ -Xlinker -rpath/usr/X11R6/lib -Xlinker -rpath/home/thuerey1/local/lib/graphviz
LDADD =  $(FLTK_LIBS) $(GRAPHVIZ_LIBS) $(GLUT_LIBS) $(OPENGL_LIBS) -lm

# Extra flags specified exlicitly
AM_CXXFLAGS = -Wno-unknown-pragmas
# CXXFLAGS = -g -Wall -Wno-unknown-pragmas

endif

EXTRA_DIST = CMakeLists.txt
