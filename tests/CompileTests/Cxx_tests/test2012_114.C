#if 1
// Here the parent of the declaration is NON-NULL; so it is NOT set properly in fixup_variable_or_field_declaration()
enum { 
  MINIMUM_C
} value;
#else
// Here the parent of the declaration is NULL; so it is set properly in fixup_variable_or_field_declaration()
class {} value;
#endif
