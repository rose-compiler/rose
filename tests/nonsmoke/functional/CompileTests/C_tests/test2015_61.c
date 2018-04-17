#if 0
struct { void *p; } __guest_handle_void; 
struct { const void *p; } __guest_handle_const_void;
#else
struct { int *p; } __guest_handle_void; struct { const void *p; } __guest_handle_const_void;
#endif
