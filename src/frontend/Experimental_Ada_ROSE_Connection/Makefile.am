include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ADA_SRC_DIR := $(top_srcdir)/src/frontend/Experimental_Ada_ROSE_Connection

AM_CPPFLAGS := $(ROSE_INCLUDES) -I$(ADA_SRC_DIR) -I$(ADA_SRC_DIR)/parser/asis_adapter/include/ -I$(ADA_SRC_DIR)/parser/a_nodes/include/

noinst_LTLIBRARIES := libexperimentalRoseAda.la

# DQ (8/23/2017): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
SUBDIRS := parser

  libexperimentalRoseAda_la_SOURCES := \
    ada_support.C \
    Ada_to_ROSE_translation.C \
    Ada_to_ROSE.C \
    AdaExpression.C \
    AdaMaker.C \
    AdaStatement.C \
    AdaType.C
else
# Compile using a function that represents the API of the Ada support.
  libexperimentalRoseAda_la_SOURCES = dummy_ada_main.C
endif

pkginclude_HEADERS := \
    AdaMaker.h 

# DQ (9/5/2017): We don't need this here, and linking is represented in ROSE/src directory's Makefile.am.
# Temporary location, but these libraries need to be built into ROSE's build tree, and later installed into ROSE's install tree.
# dot_asis_lib_dir = /home/quinlan1/ROSE/ADA/dot_asis/dot_asis_library/lib
# asis_lib_dir = @GNAT_HOME@/lib/asis/asislib
# gnat_lib_dir = @GNAT_HOME@/lib/gcc/x86_64-pc-linux-gnu/6.3.1/adalib
# libexperimentalRoseAda_la_DEPENDENCES = ${dot_asis_lib_dir}/libdot_asis.a ${asis_lib_dir}/libasis.a ${gnat_lib_dir}/libgnarl.a ${gnat_lib_dir}/libgnat.a
# ${gcc_home}/bin/gcc ${tool_name}.o ${dot_asis_lib_dir}/libdot_asis.a ${asis_lib_dir}/libasis.a ${gnat_lib_dir}/libgnarl.a ${gnat_lib_dir}/libgnat.a -static-libgcc -lpthread -lrt -ldl -o ${tool_name} || status=1

test:
