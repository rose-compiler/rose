
# DQ (6/7/2013): Adding support for new experimental fortran frontend.
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
endif

# TEST_DIR = ../../tests/rule-tests

TestFiles = $(wildcard $(TEST_DIR)/*.f90)
ASTFiles  = $(wildcard $(TEST_DIR)/*.f90.ast)

SDF_FILES =  Fortran.sdf              \
             FortranLex.sdf           \
             R400DataTypes.sdf        \
             R500DataDeclarations.sdf \
             R600UseDataObjects.sdf   \
             R700Expressions.sdf      \
             R800ExecutionControl.sdf \
             R900IOStatements.sdf     \
             R1000_IOEditing.sdf      \
             R1100ProgramUnits.sdf    \
             R1200Procedures.sdf

%.tbl : %
	sdf2table -i $< -o $@

all : Fortran.tbl

Fortran.tbl : Fortran.def
	sdf2table -i $< -o $@ -m Fortran

Fortran.def : $(SDF_FILES)
	pack-sdf -i $< -o $@

Fortran.rtg : Fortran.def
	sdf2rtg -i Fortran.def -m Fortran -o Fortran.rtg --ignore-missing-cons

Fortran.sig : Fortran.rtg
	rtg2sig -i Fortran.rtg -o Fortran.sig

FExpr.tbl : FExpr.sdf
	pack-sdf -i $< -o FExpr.def
	sdf2table -i FExpr.def -o $@ -m FExpr

Fortran.def : $(SDF_FILES)

check : Fortran.tbl
	@for File in $(TestFiles) ;\
           do \
              sglr -v -p Fortran.tbl -i $$File -o $$File.ast ; \
           done

pretty : Fortran.tbl
	@for File in $(ASTFiles) ;\
           do \
              echo "Pretty printing $$File..." ; \
              pp-aterm -i $$File -o $$File.pp ; \
           done

junk.ptree : junk.f90 Fortran.tbl
	sglri -i junk.f90 -p Fortran.tbl -o junk.ptree

junk.ast.pp : junk.f90 Fortran.tbl
	sglri -i junk.f90 -p Fortran.tbl | implode-asfix | pp-aterm -o junk.ast.pp

junk.ast-info.pp : junk.f90 Fortran.tbl
	sglri -i junk.f90 -p Fortran.tbl | addPosInfo -i junk.pt -p junk.f90 -m | implode-asfix | pp-aterm -o junk.ast-info.pp

clean :
	rm -f *.def *.tbl
	rm -f *.ast *.pp
	rm -f *.o *.mod
	rm -f $(TEST_DIR)/*.ast $(TEST_DIR)/*.ast.pp
