/*
Around line 2537 of sage_gen_be.C, the following code appears:

if (innermost_function_scope)
  theSymbol = isSgClassSymbol(get_symbol_ptr(innermost_function_scope->variant.assoc_type->source_corresp.parent.class_type->source_corresp));
else
  theSymbol = isSgClassSymbol(get_symbol_ptr(base_type->variant.pointer.type->source_corresp));

When I ran this on the example test2001_11.C which breaks in 64-bit mode, the tag of innermost_function_scope was set to sck_function, even though you are accessing the assoc_type member of the discriminated union.  Is this an actual bug in ROSE?  I don't understand the context enough to tell for sure or to fix it.

-- Jeremiah Willcock 




Around line 18772 of sage_gen_be.C, there is the following code:

case enk_variable_address:
  {
     ...
#if FACILITATE_PREINIT_IN_EXISTING_SAGE_ROUTINES
     if( preinit_pass && current_preinit_task_failed ) return NULL;
     if( result == NULL ) { }
#endif
     a_SgType_ptr rtype;
#if FACILITATE_PREINIT_IN_EXISTING_SAGE_ROUTINES
     int addAmpersand = sage_gen_ampersand(expr->variant.routine->type, rtype);

This appears to be incorrect -- although the tag of the expr object is specifically known to be enk_variable_address, you are accessing the routine member of the variant union.  One test that triggers this behavior is test2003_15.C in CompileTests/Cxx_tests.  By the way, this bug and the one I reported before lunch are the only invalid union accesses I have found so far, and if you send me fixes for them I can do more tests to try to find other similar bugs.

-- Jeremiah Willcock 

*/
