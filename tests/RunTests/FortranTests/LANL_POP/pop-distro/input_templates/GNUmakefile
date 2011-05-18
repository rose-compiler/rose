#
# File:  GNUmakefile
#
#----------------------------------------------------------------------------
#
#  To use this makefile, place setenv ARCHDIR xxxx
#  in your .login file, and defines in file xxxx.gnu
#
#----------------------------------------------------------------------------
#
#  The bourne shell is safest for gmake
SHELL = /bin/sh
#
#  This script should have been invoked from a working directory created 
#  by the "setup_run_dir" script.  TOP means this working directory.
TOP   = .
#
#  The compile subdirectory tree is meant to hold clutter that you normally 
#  wouldn't want to look at, including generated makefiles, preprocessed 
#  source files, object files, and libraries.
COMPDIR = compile
#
#  LEVEL indicates which directory is being processed.  It is a holdover 
#  from the time when this makefile system was used to process multilevel 
#  trees.
LEVEL = 0
#
#  By default, you build optimized code.  To override, add "OPTIMIZE=no" 
#  to the gmake command line.
OPTIMIZE   = yes
#
#----------------------------------------------------------------------------
#
#  Define a symbol (TARGETX) for the executable name (pop)
#
#----------------------------------------------------------------------------

TARGETX = pop
ifeq ($(OPTIMIZE),no)
  #  If building a debug version, append "_db" to the executable name
  TARGETX := $(TARGETX)_db
endif

#----------------------------------------------------------------------------
#
#  Define the dependency and include directories.
#
#----------------------------------------------------------------------------

#  Subdirectory for object files, object libraries, modules, and makefiles
ObjDepDir = ObjDepends
#
#  Separate subdirectory for preprocessor dependency makefiles
SrcDepDir = SrcDepends

#----------------------------------------------------------------------------
#
#  Set valid suffixes.
#
#----------------------------------------------------------------------------

#  First clean out current list of suffixes, then define them
.SUFFIXES: 
.SUFFIXES: .o .c .f .f90 .h .F .F90 .C .H .d .e .do .eo

#  Build the name of the one and only library, ./compile/ObjDepends/libpop.a
SUFFIX = .a
LIBRARIES = ./$(COMPDIR)/$(ObjDepDir)/lib$(TARGETX)$(SUFFIX)
#----------------------------------------------------------------------------
#
#  Include architecture flags. 
#
#----------------------------------------------------------------------------
ifneq (,$(ARCHDIR))
  include $(TOP)/$(ARCHDIR).gnu
  export ARCHDIR
else
  bogus:
	@echo "  Please setenv ARCHDIR"
endif

#----------------------------------------------------------------------------
#
#  Define sources in symbol SRCDIRS.
#
#----------------------------------------------------------------------------
OBJS = 

#  There are three source directories: this one $(TOP), $(TOP)/../source, and 
#  the architecture-specific $(TOP)/../$(COMMDIR).
SRCDIRS = $(TOP)/../source/
SRCDIRS := $(SRCDIRS) $(TOP)/
SRCDIRS := $(SRCDIRS) $(TOP)/../$(COMMDIR)/

#  VPATH is the built-in symbol whose value is the path that gmake will 
#  search for dependencies.
VPATH = $(SRCDIRS)

# Define .F sources that must be preprocessed into the build directory as .f
FSRCS   = $(strip $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.F)))
ifneq (,$(FSRCS))
  OBJS    := $(addprefix $(COMPDIR)/, $(notdir $(FSRCS:.F=.o))) $(OBJS)
endif

# Define .F90 sources to be preprocessed into the build directory as .f90
F90SRCS   = $(strip $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.F90)))
ifneq (,$(F90SRCS))
  OBJS    := $(addprefix $(COMPDIR)/, $(notdir $(F90SRCS:.F90=.o))) $(OBJS)
endif

# Define .C sources that must be preprocessed into the build directory as .c
CSRCS   = $(strip $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.C)))
ifneq (,$(CSRCS))
  OBJS    := $(addprefix $(COMPDIR)/, $(notdir $(CSRCS:.C=.o))) $(OBJS)
endif

# Define .f sources that need to be copied into the build directory
LFSRCS  = $(strip $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.f)))
ifneq (,$(LFSRCS))
  ifneq (,$(FSRCS))
    LFSRCS    := $(filter-out $(FSRCS:.F=.f),$(LFSRCS))
  endif
  ifneq (,$(strip $(LFSRCS)))
    OBJS    := $(addprefix $(COMPDIR)/, $(notdir $(LFSRCS:.f=.o))) $(OBJS)
  endif
endif

# Define .f90 sources that need to be copied into the build directory
LF90SRCS  = $(strip $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.f90)))
ifneq (,$(LF90SRCS))
  ifneq (,$(F90SRCS))
    LF90SRCS    := $(filter-out $(F90SRCS:.F90=.f90),$(LF90SRCS))
  endif
  ifneq (,$(strip $(LF90SRCS)))
    OBJS    := $(addprefix $(COMPDIR)/, $(notdir $(LF90SRCS:.f90=.o))) $(OBJS)
  endif
endif

# Define .c sources that need to be copied into the build directory
LCSRCS   = $(strip $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.c)))
ifneq (,$(LCSRCS))
  ifneq (,$(CSRCS))
    LCSRCS    := $(filter-out $(CSRCS:.C=.c),$(LCSRCS))
  endif
  ifneq (,$(strip $(LCSRCS)))
    OBJS    := $(addprefix $(COMPDIR)/, $(notdir $(LCSRCS:.c=.o))) $(OBJS)
  endif
endif

#----------------------------------------------------------------------------
#
#  Make all packages, including dependencies, and then create an executable.
#
#----------------------------------------------------------------------------

#
# $(COMPDIR).m is a dummy target which will cause gmake to drop down into 
# the compile subdirectory
all: $(Incl) $(Dincl) $(COMPDIR).m
	@echo "  GNUmakefile is making target 'all' "
	@$(MAKE) $(TARGETX) LEVEL=1 OPTIMIZE=$(OPTIMIZE)

#----------------------------------------------------------------------------
#
#  Make a specific package.
#
#----------------------------------------------------------------------------

%.m: 
	@echo "  GNUmakefile is making $(ARCHDIR) target 'lib' in subdirectory '$*'"
	@cd $*; $(MAKE) objs LEVEL=0 ARCHDIR=$(ARCHDIR) OPTIMIZE=$(OPTIMIZE)

#----------------------------------------------------------------------------
#
#  Echo makefile variables.
#
#----------------------------------------------------------------------------

flags:
	@echo "  GNUmakefile is making target 'flags'"
	@echo ARCHDIR=$(ARCHDIR)
	@echo COMPDIR=$(COMPDIR)
	@echo DEPENDS=$(DEPENDS)
	@echo ObjDepDir=$(ObjDepDir)
	@echo SrcDepDir=$(SrcDepDir)
	@echo TARGETX=$(TARGETX)
	@echo TARGETLIB=$(TARGETLIB)
	@echo Cpp_opts=$(Cpp_opts)
	@echo LIBRARIES=$(LIBRARIES)
	@echo CFLAGS=$(CFLAGS)
	@echo FFLAGS=$(FFLAGS)
	@echo LDFLAGS=$(LDFLAGS)
	@echo LDLIBS=$(LDLIBS)

#----------------------------------------------------------------------------
#
#  Echo sources, objects, and dependencies.
#
#----------------------------------------------------------------------------

sources:
	@echo "  GNUmakefile is making target 'sources'"
	@echo SRCDIRS=$(SRCDIRS)
	@echo VPATH=$(VPATH)
	@echo FSRCS=$(FSRCS)
	@echo F90SRCS=$(F90SRCS)
	@echo CSRCS=$(CSRCS)
	@echo LFSRCS=$(LFSRCS)
	@echo LF90SRCS=$(LF90SRCS)
	@echo LCSRCS=$(LCSRCS)
	@echo OBJS=$(OBJS)
	@echo DEPENDS=$(DEPENDS)
	@echo LEVEL=$(LEVEL)

#----------------------------------------------------------------------------
#
#  Make the executable.
#
#----------------------------------------------------------------------------

$(TARGETX): $(OBJS)
	@echo "  GNUmakefile is making target '$(TARGETX)'"
	@$(LD) -o $(TARGETX) $(LDFLAGS) $(OBJS) $(LDLIBS) 

#----------------------------------------------------------------------------
#
#  Include utilities makefile.
#
#----------------------------------------------------------------------------

include $(TOP)/utilities.gnu

