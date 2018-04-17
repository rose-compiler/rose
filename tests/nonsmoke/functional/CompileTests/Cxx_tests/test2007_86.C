/*
The following code:
//AS(Jun 25 07) Example from
// mozilla/db/mork/src/morkRow.cpp
class morkAtom {
public:
 bool   GetYarn() const;
};

void SeekColumn( )
{
       ((morkAtom*) 0)->GetYarn(); // yes this will work
}

gives the following error

lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-23a-Unsafe/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:16203:
SgExpression* sage_gen_lvalue(an_expr_node*,
DataRequiredForComputationOfSourcePostionInformation&): Assertion false failed.
/home/andreas/links/g++-411: line 2: 21681 Aborted
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-23a-Unsafe/tutorial/identityTranslator
"$@" 

*/

class morkAtom {
public:
 bool   GetYarn() const;
};

void SeekColumn( )
{
       ((morkAtom*) 0)->GetYarn(); // yes this will work
}
