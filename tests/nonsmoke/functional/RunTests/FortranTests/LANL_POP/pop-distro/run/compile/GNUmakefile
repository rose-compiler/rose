SHELL = /bin/sh
TOP = ..
TARGETX = $(TOP)/pop
SRCDIRS = $(filter-out $(ObjDepDir)/ $(SrcDepDir)/ CVS/,$(wildcard */))

LEVEL = 0
OPTIMIZE   = yes

#----------------------------------------------------------------------------
#
#  Define the executable name (TARGETX) and target library (TARGETLIB) and
#  package (TARGETMOD).
#
#----------------------------------------------------------------------------

ifeq ($(OPTIMIZE),no)
  TARGETX := $(TARGETX)_db
endif
TARGETMOD = $(subst _db.m,.m,$(addsuffix .m,$(notdir $(TARGETX))))
TARGETLIB = $(ObjDepDir)/$(addprefix lib,$(addsuffix .a,$(notdir $(TARGETX))))

#----------------------------------------------------------------------------
#
#  Define the dependency and include directories
#
#----------------------------------------------------------------------------

ObjDepDir = ObjDepends
SrcDepDir = SrcDepends

#----------------------------------------------------------------------------
#
#  Set valid suffixes.
#
#----------------------------------------------------------------------------

.SUFFIXES: 
.SUFFIXES: .o .c .f .f90 .h .F .F90 .C .H .d .e .do .eo

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
	@echo "GNUmakefile_local"
	@echo Please setenv ARCHDIR
endif

#----------------------------------------------------------------------------
#
#  Make all packages, including dependencies, and then create an executable.
#
#----------------------------------------------------------------------------

all: $(Incl) $(Dincl)
	@echo "GNUmakefile_local is making target all"
	@cd $(TOP); $(MAKE) all LEVEL=0 ARCHDIR=$(ARCHDIR) OPTIMIZE=$(OPTIMIZE)

#
#  Make all the object files
objs:
	@echo "GNUmakefile_local is making target objs"
	+@if [ ! -d $(ObjDepDir) ]; then mkdir $(ObjDepDir); fi
	+@if [ ! -d $(SrcDepDir) ]; then mkdir $(SrcDepDir); fi
	@$(MAKE) $(OBJS) LEVEL=1 ARCHDIR=$(ARCHDIR) OPTIMIZE=$(OPTIMIZE)
	
#
#  Make the library for all the object files: ./compile/ObjDepends/libpop.a
lib:
	@echo "GNUmakefile_local is making target $(TARGETLIB)"
	+@if [ ! -d $(ObjDepDir) ]; then mkdir $(ObjDepDir); fi
	+@if [ ! -d $(SrcDepDir) ]; then mkdir $(SrcDepDir); fi
	@$(MAKE) $(TARGETLIB) LEVEL=1 ARCHDIR=$(ARCHDIR) OPTIMIZE=$(OPTIMIZE)
	
#----------------------------------------------------------------------------
#
#  Echo makefile variables.
#
#----------------------------------------------------------------------------

flags:
	@echo "GNUmakefile_local making target flags"
	@echo ARCHDIR=$(ARCHDIR)
	@echo DEPENDS=$(DEPENDS)
	@echo ObjDepDir=$(ObjDepDir)
	@echo SrcDepDir=$(SrcDepDir)
	@echo TARGETX=$(TARGETX)
	@echo TARGETLIB=$(TARGETLIB)
	@echo TARGETMOD=$(TARGETMOD)
	@echo PACKAGES=$(PACKAGES)
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
	@echo "GNUmakefile_local is making target 'sources'"
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
#  Define sources.
#
#----------------------------------------------------------------------------
#
# SRCDIRS is the list of all possible source directories
SRCDIRS = $(TOP)/
SRCDIRS := $(SRCDIRS) $(TOP)/../source/
SRCDIRS := $(SRCDIRS) $(TOP)/../$(COMMDIR)/

VPATH = $(SRCDIRS)

#
#  The following symbols, FSRCS, F90SRCS, CSRCS, LFSRCS, LF90SRCS, LCSRCS, list
#  files from the current (compile) directory and from the source directories
#  in SRCDIRS.  They are used to generate various other lists for makefiles
#  and object files.
#
# Each of the following commands has a similar pattern:
#    List all files of a certain type from the current directory 
#         plus all files of that same type from SRCDIRS
#
# The distinction between uppercase and lowercase suffixes has
# become less important; most Fortran files require no preprocessing
# and the C compiler now handles all preprocessing on C files.
#
# Uppercase .F - Fortran files that will be preprocessed
FSRCS   = $(strip $(wildcard *.F) \
                  $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.F)))
# Uppercase .F90 - Fortran free-form files that will be preprocessed
F90SRCS   = $(strip $(wildcard *.F90) \
                  $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.F90)))
# Uppercase .C - C files that used to be preprocessed, but are now copied
CSRCS   = $(strip $(wildcard *.C) \
                  $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.C)))
# Lowercase .f - Fortran files that can be simply copied
LFSRCS  = $(strip $(wildcard *.f) \
                  $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.f)))
# Lowercase .f90 - Fortran free-form files that can be simply copied
LF90SRCS  = $(strip $(wildcard *.f90) \
                  $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.f90)))
# Lowercase .c - C files that can be simply copied
LCSRCS   = $(strip $(wildcard *.c) \
                  $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.c)))

#----------------------------------------------------------------------------
#
#  Define objects and dependency files.
#
#----------------------------------------------------------------------------

#  Key points:
#    All .F and .C files are preprocessed into '../compile' as .f and .c
#    All other .f and .c files are copied into '../compile'
ifneq (,$(FSRCS))
# Ensure that a dependency makefile is listed both for .f:.F and for .o:.f
  #
  # This rule creates a list of names like 'SrcDepends/POP.do'
  DEPENDS := $(addprefix $(SrcDepDir)/, $(notdir $(FSRCS:.F=$(DEPSUF))))
  #
  # This rule adds to the list names like 'ObjDepends/POP.do'
  DEPENDS := $(DEPENDS) $(addprefix $(ObjDepDir)/, $(notdir $(FSRCS:.F=$(DEPSUF))))
  #
  # This rule changes every .F suffix to .o, then strips all but the file name
  OBJS    := $(notdir $(FSRCS:.F=.o)) $(OBJS)
endif

ifneq (,$(F90SRCS))
# Ensure that a dependency makefile is listed both for .f90:.F90 and for .o:.f90
  #
  # This rule creates a list of names like 'SrcDepends/POP.do'
  DEPENDS := $(addprefix $(SrcDepDir)/, $(notdir $(F90SRCS:.F90=$(DEPSUF))))
  #
  # This rule adds to the list names like 'ObjDepends/POP.do'
  DEPENDS := $(DEPENDS) $(addprefix $(ObjDepDir)/, $(notdir $(F90SRCS:.F90=$(DEPSUF))))
  #
  # This rule changes every .F90 suffix to .o, then strips all but the file name
  OBJS    := $(notdir $(F90SRCS:.F90=.o)) $(OBJS)
endif

ifneq (,$(CSRCS))
# Ensure that a dependency makefile is listed both for .c:.C and for .o:.c
  # This rule adds to the list names like 'SrcDepends/fix_64.do'
  DEPENDS := $(DEPENDS) $(addprefix $(SrcDepDir)/, $(notdir $(CSRCS:.C=$(DEPSUF))))
  # This rule adds to the list names like 'ObjDepends/fix_64.do'
  DEPENDS := $(DEPENDS) $(addprefix $(ObjDepDir)/, $(notdir $(CSRCS:.C=$(DEPSUF))))
  # This rule changes every .C suffix to .o, then strips all but the file name
  OBJS    := $(notdir $(CSRCS:.C=.o)) $(OBJS)
endif

ifneq (,$(LFSRCS))
  ifneq (,$(FSRCS))
    LFSRCS    := $(filter-out $(FSRCS:.F=.f),$(LFSRCS)) 
  endif
  ifneq (,$(strip $(LFSRCS)))
    OBJS    := $(notdir $(LFSRCS:.f=.o)) $(OBJS) 
    DEPENDS := $(DEPENDS) $(addprefix $(ObjDepDir)/, $(notdir $(LFSRCS:.f=$(DEPSUF))))
  endif
endif

ifneq (,$(LF90SRCS))
  ifneq (,$(F90SRCS))
    LFSRCS    := $(filter-out $(F90SRCS:.F90=.f90),$(LF90SRCS)) 
  endif
  ifneq (,$(strip $(LF90SRCS)))
    OBJS    := $(notdir $(LF90SRCS:.f90=.o)) $(OBJS) 
    DEPENDS := $(DEPENDS) $(addprefix $(ObjDepDir)/, $(notdir $(LF90SRCS:.f90=$(DEPSUF))))
  endif
endif

ifneq (,$(LCSRCS))
  ifneq (,$(CSRCS))
    LCSRCS    := $(filter-out $(CSRCS:.C=.c),$(LCSRCS))
  endif
  ifneq (,$(strip $(LCSRCS)))
    OBJS    := $(notdir $(LCSRCS:.c=.o)) $(OBJS)
    DEPENDS := $(DEPENDS) $(addprefix $(ObjDepDir)/, $(notdir $(LCSRCS:.c=$(DEPSUF))))
  endif
endif

#----------------------------------------------------------------------------
#
#  List the dependencies.
#
#----------------------------------------------------------------------------

# Sort to remove duplicates
DEPENDS := $(sort $(DEPENDS))

# The actual makefiles in list DEPENDS were created for specific architectures.
# (See <ARCHDIR>.gnu)
ifneq (0,$(LEVEL))
  -include $(DEPENDS)
endif

#----------------------------------------------------------------------------
#
#  Make a library file.
#
#----------------------------------------------------------------------------

$(TARGETLIB): $(TARGETLIB)($(OBJS))
	@echo "GNUmakefile_local is making target TARGETLIB"
	$(RANLIB) $(TARGETLIB)

#----------------------------------------------------------------------------
#
#  Include utilities makefile.
#
#----------------------------------------------------------------------------

include $(TOP)/utilities.gnu

