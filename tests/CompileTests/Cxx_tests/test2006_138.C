
// Test wchar example:
wchar_t *wchar_ptr_null  = 0L;
// wchar_t *wchar_ptr_valid = L"\u00FC";
wchar_t *wchar_ptr_valid = L"\u00FC";

// Note that gnu reports a warning about the character literal
// being too long for the type, but I think this is an incorrect 
// warning.
wchar_t wchar_ptr_value = L'ab';

// This is some strange variable name!  And not supported by GNU
char * \u00FC = "u-umlaut variable";
