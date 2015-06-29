#if 0
typedef struct { void *p; } __guest_handle_void; 
typedef struct { const void *p; } __guest_handle_const_void;
#else
typedef struct { int *p; } __guest_handle_void; typedef struct { const void *p; } __guest_handle_const_void;
#endif
