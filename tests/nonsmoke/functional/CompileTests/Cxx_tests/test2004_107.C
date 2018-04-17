enum {X, Y} val = X;

// This generates the strange case in the EDG/Sage connection of case tk_enum in sage_gen_tag_reference.
enum tag {T, F} XYenumVariable = T;
#if __cplusplus
tag anotherEnumVariable;
#else
enum tag anotherEnumVariable;
#endif

