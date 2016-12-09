/* This test uses forward enum declarations which are not strickly legal C or C++
   but are supported by a wide number of compilers, except GNU gcc and g++.
   Thus this test will fail where gcc or g++ are being used as backend compilers.
*/

/*
when compiling the following code:
typedef enum _cairo_clip_mode cairo_clip_mode_t;

I get the following error:
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-23a-Unsafe/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:3983:
SgDeclarationStatement* sage_gen_tag_reference(a_type*, a_boolean,
SgDeclarationStatement*&,
DataRequiredForComputationOfSourcePostionInformation*): Assertion
theSymbol != __null failed. 
*/

typedef enum _cairo_clip_mode cairo_clip_mode_t;
