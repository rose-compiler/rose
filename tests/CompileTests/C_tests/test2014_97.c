// double* __attribute__((__may_alias__)) global_orange_ptr = 0L;
// double* __attribute__((__orange__)) global_orange_ptr;
// double* __attribute__((__aligned__)) global_orange_ptr;
// double* __attribute__((__orange__)) global_orange_ptr;

// This attribute shows up as type: ak_unrecognized and location: al_prefix
// This is OK, since no other position appears to put the attribute onto the type.
__attribute__((__orange__)) double* global_orange_ptr;

// This attribute will be have a location: al_id_equivalent
double* __attribute__((__orange__)) global_orange_ptr_2;

// This attribute will be have a location: al_specifier
double __attribute__((__orange__)) *global_orange_ptr_3;

