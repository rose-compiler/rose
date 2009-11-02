
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(btoc_SRCS $(BTOC_SRCS))

add_executable(btoc ${btoc_SRCS})

target_link_libraries(btoc ${KDE4_KDECORE_LIBS})


########### next target ###############

set(restart_SRCS $(RESTART_SRCS))

add_executable(restart ${restart_SRCS})

target_link_libraries(restart ${KDE4_KDECORE_LIBS})


########### install files ###############




#original Makefile.am contents follow:

## This should not depend on ROSE in any way so I don't think we need this
#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#lib_LIBRARIES = libckpt.so librestart.so
#
#noinst_PROGRAMS = btoc restart
#
### Specifications for building the different parts of the check point library.
#LIB_CKPT_SRCS =\
#	ckpt.c\
#	init.c\
#	mem.c\
#	signals.c\
#	ckptclt.c\
#	sockaddr.c\
#	util.c\
#	config.c\
#	fd.c\
#	elfckpt.c \
#   wraprestart.c
#
#LIB_RESTART_SRCS = safe.c
#
#RESTART_SRCS =\
#	restartmain.c\
#	restart.c\
#	mem.c\
#	util.c\
#	ckptclt.c\
#	sockaddr.c\
#	wrapsafe.c\
#	elfrestart.c
#
#CKPT_SRCS =\
#	ckptmain.c\
#	wrapckpt.c\
#	hijack.c\
#	refun.c\
#	util.c
#
#CKPTSRV_SRCS =\
#	ckptsrv.c\
#	util.c\
#	sockaddr.c
#BTOC_SRCS = btoc.c
#TEST_SRCS = foo.c
#
#wrapsafe.c: btoc librestart.so
#	./btoc librestart < librestart.so > wrapsafe.c
#
#wraprestart.c: btoc restart
#	./btoc restartbin < restart > wraprestart.c
#
#wrapckpt.c: btoc libckpt.so
#	./btoc libckpt < libckpt.so > wrapckpt.c
#
#libckpt_so_SOURCES = $(LIB_CKPT_SRCS)
## libckpt_so_DEPENDENCIES = $(LIB_CKPT_SRCS)
#
#librestart_so_SOURCES = $(LIB_RESTART_SRCS)
#
#restart_SOURCES = $(RESTART_SRCS)
#btoc_SOURCES    = $(BTOC_SRCS)
#
#restart_LDFLAGS = -Xlinker --script=$(srcdir)/restart.script
#
#
#
#libckpt.a: $(libckpt_a_OBJECTS)
#	-rm -f $@
#	$(CXX_STATIC_LIB_UPDATE) $@ $(libckpt_a_OBJECTS)
#	test -f $@ || exit 1
#
#libckpt.so: $(libckpt_so_OBJECTS)
#	-rm -f $@
#	$(CXX_SHARED_LIB_UPDATE) $@ -nostartfiles $(libckpt_so_OBJECTS)
#	test -f $@ || exit 1
#
## librestart.a: $(librestart_a_OBJECTS)
##	-rm -f $@
##	$(CXX_STATIC_LIB_UPDATE) $@ $(librestart_a_OBJECTS)
##	test -f $@ || exit 1
#
##librestart.so: $(librestart_so_OBJECTS)
##	-rm -f $@
##	$(CXX_SHARED_LIB_UPDATE) $@ $(librestart_so_OBJECTS)
##	test -f $@ || exit 1
#
#noinst_HEADERS = ckpt.h  ckptimpl.h  cs.h  sys.h
#
## include_HEADERS = 
#
#
