include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (6/7/2013): Adding support for new experimental fortran frontend.
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
endif


# SUBDIRS = syntax-v0.14 rose_traverse
# SUBDIRS = sdf_syntax stratego_transformations aterm_traversal
SUBDIRS = sdf_syntax stratego_transformations aterm_traversal

# DQ (1/22/2016): This makefile rule uses the hard-coded path to the OFP directory.
# This is not a problem since it is used only for development purposes.
# OFP_PATH = /home/dquinlan/ROSE/FORTRAN_LLNL/OFP-SDF/ofp-sdf/fortran
OFP_PATH = /home/dquinlan/ROSE/OpenFortranParser/GITHUB/ofp-sdf/fortran

OLD_copy_sources:
	@echo "Copy OFP sources from local OFP repository into ROSE source tree..."
	cp $(OFP_PATH)/syntax/FortranFixed.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/FortranLexFixed.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/FortranLexFree.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/FortranLex.sdf $(srcdir)/sdf_syntax
#	cp $(OFP_PATH)/syntax/Main.rtg $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/Main.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/Main.str $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/trans/ofp-simplify.str $(srcdir)/stratego_transformations
	cp $(OFP_PATH)/rose_traverse/traversal.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/traversal.hpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/traverse.hpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/traverse.cpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/OFPNodes.hpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/OFPNodes.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/OFPUnparser.hpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/OFPUnparser.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/ofp_traverse_productions.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/OFPExpr.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/OFPExpr.hpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/UntypedNodes.h $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/ASTBuilder.hpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/rose_traverse/UntypedASTBuilder.hpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/UntypedASTBuilder.cpp $(srcdir)/aterm_traversal

copy_sources:
	@echo "Copy OFP sources from local OFP repository into ROSE source tree..."
	cp $(OFP_PATH)/syntax/FortranFixed.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/FortranLexFixed.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/FortranLexFree.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/FortranLex.sdf $(srcdir)/sdf_syntax
#	cp $(OFP_PATH)/syntax/Main.rtg $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/Main.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/Main.str $(srcdir)/sdf_syntax
###	cp $(OFP_PATH)/syntax/FAST.sdf $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/syntax/FAST.str $(srcdir)/sdf_syntax
	cp $(OFP_PATH)/pp/Fortran.pp $(srcdir)/pretty_print
	cp $(OFP_PATH)/trans/ofp-simplify.str $(srcdir)/stratego_transformations
	cp $(OFP_PATH)/trans/no-to-none.str $(srcdir)/stratego_transformations
	cp $(OFP_PATH)/trans/ofp2fast.str $(srcdir)/stratego_transformations
	cp $(OFP_PATH)/trans/fast2pp.str $(srcdir)/stratego_transformations
#	cp $(OFP_PATH)/trans/fast2sage.str $(srcdir)/stratego_transformations
##	cp $(OFP_PATH)/trans/fast2sage.str $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/trans/fast2sage.str $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/traversal.cpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/traversal.hpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/traverse.hpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/traverse.cpp $(srcdir)/aterm_traversal
###	cp $(OFP_PATH)/../ROSE/traverse_main.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/../ROSE/traverse_SgUntypedNodes.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/../ROSE/traverse_SgUntypedNodes.hpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/../ROSE/SgUntypedNodes.hpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/../ROSE/traverse_lists.cpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/OFPNodes.hpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/OFPNodes.cpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/OFPUnparser.hpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/OFPUnparser.cpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/ofp_traverse_productions.cpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/OFPExpr.cpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/OFPExpr.hpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/UntypedNodes.h $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/ASTBuilder.hpp $(srcdir)/aterm_traversal
##	cp $(OFP_PATH)/rose_traverse/UntypedASTBuilder.hpp $(srcdir)/aterm_traversal
###	cp $(OFP_PATH)/../ROSE/SgUntyped.rtg $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/rose_traverse/UntypedASTBuilder.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/../ROSE/SgUntyped.str $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/../ROSE/build_SgUntypedNodes.cpp $(srcdir)/aterm_traversal
	cp $(OFP_PATH)/../ROSE/build_SgUntypedNodes.hpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/../ROSE/rose_build_SgUntypedNodes.cpp $(srcdir)/aterm_traversal
#	cp $(OFP_PATH)/../ROSE/rose_build_SgUntypedNodes.hpp $(srcdir)/aterm_traversal



# traversal.o: traversal.cpp traversal.hpp OFPNodes.hpp OFPExpr.hpp UntypedNodes.h ofp_traverse_productions.o OFPExpr.o OFPUnparser.hpp OFPUnparser.cpp
# ofp_traverse_productions.o: ofp_traverse_productions.cpp traversal.hpp OFPNodes.hpp UntypedNodes.h
# OFPExpr.o: OFPExpr.cpp OFPExpr.hpp UntypedNodes.h


