/*
Here are the 3 problems I encountered:

[1] Unparsing an enum value in an SgTemplateInstantiationDefn context: An assertion failure occurs at line 1551 of unparse_expr.C, in Unparser::unparseEnumVal().

I think there is a missing case in unparse_expr.C. In particular, there is a test for the variant value, V_SgClassDefinition, but I think you also need the case, V_SgTemplateInstantiationDefn. See the attached patch, PATCH-unparse_expr.C.

The failure occurred in a SWIG file, and I did not successfully reduce the file to a small example you could include in a test. However, applying the above patch seemed to work.

[2] Failure for files with very long lines.

One of the SWIG-generated files has a line with over 8000 characters in it, exceeding the hard-coded limit of 5,000 set in frontend/EDG_SAGE_Connection/sageSupport.C, line 1407. I changed the current limit to 20,000, as shown in the attached patch, PATCH-sageSupport.C, which seemed to work fine for KULL.

[3] Forward reference to an 'enum' value within a class.

I may have reported this issue before. Within a couple of its classes, KULL references an enum value before the enum declaration, causing the following error:

   Finished building EDG AST, now build the SAGE AST ...
   DONE: edg_main
   Warnings in EDG/Sage III Translation Processing! (continuing ...)
   identity-astFileIO: /g/g18/vuduc2/projects/ROSE/src-0.8.7a/src/frontend/SageIII/astPostProcessing/fixupDefiningAndNondefiningDeclarations.C:113: virtual void FixupAstDefiningAndNondefiningDeclarations::visit(SgNode*): Assertion `declaration == definingDeclaration' failed.

Here is a sample code that demonstrates the bug:

class A
   {
   public:

   #if defined(WORKAROUND_FAILURE)
     enum EValues { VAL_A, VAL_B, VAL_C };
     A () : val_ (VAL_A) {}
   #else
     A () : val_ (VAL_A) {}
     enum EValues { VAL_A, VAL_B, VAL_C };
   #endif

   private:
     EValues val_;
   };
*/


class A
   {
   public:

   #if defined(WORKAROUND_FAILURE)
     enum EValues { VAL_A, VAL_B, VAL_C };
     A () : val_ (VAL_A) {}
   #else
     A () : val_ (VAL_A) {}
     enum EValues { VAL_A, VAL_B, VAL_C };
   #endif

   private:
     EValues val_;
   };
