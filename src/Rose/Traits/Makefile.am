include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES)

libRoseTraits_la_public_includedir = $(prefix)/include/Rose
libRoseTraits_la_public_include_HEADERS = ../Traits.h

libRoseTraits_la_includedir = $(prefix)/include/Rose/Traits
libRoseTraits_la_include_HEADERS = Describe.h Traversal.h

libRoseTraits_la_describe_includedir = $(prefix)/include/Rose/Traits/describe
libRoseTraits_la_describe_include_HEADERS = describe/declarations.h describe/predicates.h describe/fields.h describe/nodes.h

libRoseTraits_la_grammar_includedir = $(prefix)/include/Rose/Traits/grammar
libRoseTraits_la_grammar_include_HEADERS = grammar/traversal.h grammar/lists.h grammar/predicates.h

EXTRA_DIST = CMakeLists.txt

