include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

if ROSE_USE_LLVM

INCLUDES = $(ROSE_INCLUDES) \
		   $(ROSE_LLVM_INCLUDES) \
		   -I$(srcdir)/include

pkginclude_HEADERS 	= \
	include/rosetollvm/RootAstAttribute.h \
	include/rosetollvm/ManagerAstAttribute.h \
	include/rosetollvm/AggregateAstAttribute.h \
	include/rosetollvm/AttributesAstAttribute.h \
	include/rosetollvm/CaseAstAttribute.h \
	include/rosetollvm/CodeAttributesVisitor.h \
	include/rosetollvm/CodeEmitter.h \
	include/rosetollvm/CodeGeneratorVisitor.h \
	include/rosetollvm/ConditionalAstAttribute.h \
	include/rosetollvm/ConditionalComponentAstAttribute.h \
	include/rosetollvm/DeclarationsAstAttribute.h \
	include/rosetollvm/DoAstAttribute.h \
	include/rosetollvm/ForAstAttribute.h \
	include/rosetollvm/FunctionAstAttribute.h \
	include/rosetollvm/IfAstAttribute.h \
	include/rosetollvm/IfComponentAstAttribute.h \
	include/rosetollvm/IntAstAttribute.h \
	include/rosetollvm/LLVMAstAttributes.h \
	include/rosetollvm/LLVMVisitor.h \
	include/rosetollvm/LogicalAstAttribute.h \
	include/rosetollvm/Option.h \
	include/rosetollvm/Optimize.h \
	include/rosetollvm/ObjectGenerator.h \
	include/rosetollvm/Control.h \
	include/rosetollvm/RoseToLLVMModule.hpp \
	include/rosetollvm/SgTypeAstAttribute.h \
	include/rosetollvm/SizeOfAstAttribute.h \
	include/rosetollvm/StringAstAttribute.h \
	include/rosetollvm/StringSet.h \
	include/rosetollvm/SwitchAstAttribute.h \
	include/rosetollvm/WhileAstAttribute.h

lib_LTLIBRARIES = librosetollvm.la

librosetollvm_la_SOURCES = \
	RoseToLLVMModule.cpp \
	CodeAttributesVisitor.cpp \
	CodeEmitter.cpp \
	CodeGeneratorVisitor.cpp \
	LLVMAstAttributes.cpp \
	Option.cpp \
	Optimize.cpp \
	ObjectGenerator.cpp \
	Control.cpp \
	StringSet.cpp

librosetollvm_la_CPPFLAGS	= -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS -Wno-deprecated -g -O0
#	$(ROSE_CPPFLAGS) $(BOOST_CPPFLAGS) $(LLVM_CPPFLAGS)

librosetollvm_la_LDFLAGS = $(LIBS_WITH_RPATH) \
						   $(ROSE_LIBS) \
						   $(ROSE_LLVM_LIBS)
endif

#MAINTAINERCLEANFILES = Makefile.in

