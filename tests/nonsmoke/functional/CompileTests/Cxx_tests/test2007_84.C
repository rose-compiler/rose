/*
Email from Andreas:

Hi Dan,
finally I isolated one of the most painful bugs. The following code
when compiled with ROSE
class  nsIClassInfo;

void QueryInterface()
{
   extern nsIClassInfo *nsSampleImpl_classInfoGlobal;
};

gives the following error:

lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-14a-Unsafe/NEW_ROSE/src/backend/unparser/name_qualification_support.C:643:
SgName Unparser::generateNameQualifier(SgInitializedName*, const
SgUnparse_Info&, bool): Assertion alt_initializedName != __null
failed.
/home/andreas/links/g++-411: line 2: 18866 Aborted
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-14a-2007-Unsafe/tutorial/identityTranslator
"$@" 


*/



class  nsIClassInfo;

void QueryInterface()
{
   extern nsIClassInfo *nsSampleImpl_classInfoGlobal;
};


