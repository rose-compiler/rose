include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

# The ROSE library should not be linked into Sawyer tools since (1) it's not needed, and (2) it hasn't been built yet at
# the point where Sawyer tools are being linked. In other words, Sawyer tools are not ROSE tools.  Sawyer tools probably
# don't need most of the other ROSE tool dependency libraries either, but at least those are already available.
AM_LDFLAGS = $(ROSE_RPATHS) $(filter-out %/librose.la,$(ROSE_LIBS))

noinst_PROGRAMS = rosebud
rosebud_SOURCES =				\
	rosebud.C				\
	Ast.C					\
	BoostSerializer.C			\
	CxxGenerator.C				\
	Generator.C				\
	NoneGenerator.C				\
	NoneSerializer.C			\
	RoseGenerator.C				\
	RosettaGenerator.C			\
	Serializer.C				\
	Utility.C				\
	YamlGenerator.C
rosebud_LDADD = $(top_builddir)/src/util/Sawyer/libroseSawyer.la $(ROSE_BOOST_LIBS) $(RT_LIBS)

# Headers are mentioned here so the policy checkers think they're used. There's no reason
# to install them.
headers =					\
	Ast.h					\
	BasicTypes.h				\
	BoostSerializer.h			\
	CxxGenerator.h				\
	Generator.h				\
	NoneGenerator.h				\
	NoneSerializer.h			\
	RoseGenerator.h				\
	RosettaGenerator.h			\
	Serializer.h				\
	Utility.h				\
	YamlGenerator.h

other_files =					\
	README
