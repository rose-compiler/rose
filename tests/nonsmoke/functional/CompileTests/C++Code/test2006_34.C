#if 0
Hi Dan,

I have a question about SgMemberFunctionType::get_struct_name().

Consider the following program:

   class A
   {
     public: void foo (void);
   };

   // eof

Let
   SgMemberFunctionDeclaration* decl = /* the declaration for 'foo' above */
   SgMemberFunctionType* type = isSgMemberFunctionType (decl->get_type ());

I am finding that type->get_struct_name() is NULL. This is fine in the sense that I can get the same information from decl, but if I am inside a member function of SgMemberFunctionType, should I be able to use 'get_struct_name()' to reach the class definition? I have checked this with both the CVS version of ROSE and your internally released distribution of January 18, 2006.

Thanks,
--rich

#endif


