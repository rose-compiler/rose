include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Separate Transformations and specialied support is isolated into separate directories
SUBDIRS = \
     IndexOffsetQuery \
     ArrayAssignmentStatementQuery

INCLUDES = $(ROSE_INCLUDES) \
	 -I$(top_srcdir)/projects/A++DataLayout/include \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor/ArrayAssignmentStatementQuery \
     -I$(top_srcdir)/projects/A++DataLayout/Preprocessor/IndexOffsetQuery

# Define the template repository path used in the autoconf
# subsitution for various C++ template control variables.
CXX_TEMPLATE_REPOSITORY_PATH = .
CXXFLAGS += -DDEBUG 


noinst_LIBRARIES = libarraypreprocessor.a

# version of A++ preprocessor (initial development)
libarraypreprocessor_a_SOURCES = \
     arrayInheritedAttributeBaseClass.C \
     arrayTransformationSupport.C \
     programTransformation.C \
     transformationOptions.C \
     operandDataBase.C \
     arrayVariableDataBase.C \
     indexVariableDataBase.C \
     arrayPreprocessor.C

libarraypreprocessor_a_includedir = $(includedir)/Preprocessor

libarraypreprocessor_a_include_HEADERS = \
     arrayInheritedAttributeBaseClass.h \
     operandDataBase.h \
     programTransformation.h \
     arrayTransformationSupport.h \
     arrayPreprocessor.h 

# Note that because rose dependes upon the sage and sage 
# depends upon rose we have to list the sage library twice.
LDADD = \
     $(LIBS_WITH_RPATH) \
     $(ROSE_LIBS) \
     $(top_builddir)/projects/A++DataLayout/Preprocessor/ArrayAssignmentStatementQuery/libarrayassignmentquery.a \
     $(top_builddir)/projects/A++DataLayout/Preprocessor/IndexOffsetQuery/libindexoffsetquery.a


clean-local:
	rm -rf $(CXX_TEMPLATE_OBJECTS) Templates.DB ii_files ti_files
	rm -f *.o rose_*.C *.sage.C test2001_??.C.show \
              *.C.pdf *.C.edg.pdf *.pdf *.dot *.int.C rose_directives_list.txt core
	rm -rf ti_files

# This rule is run after automake's internal check rule (which we don't want to use)
check-local:
	@$(MAKE) $(TEST_Objects)




























