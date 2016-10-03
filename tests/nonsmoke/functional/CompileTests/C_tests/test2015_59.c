#if 0
typedef struct { void *p; } __guest_handle_void; 
typedef struct { const void *p; } __guest_handle_const_void;
#else
typedef struct { void *p; } __guest_handle_void; typedef struct { const void *p; } __guest_handle_const_void;
#endif

struct mmuext_op {
    unsigned int cmd;
    union {
        unsigned long linear_addr;
    } arg1;
    union {
        unsigned int nr_ents;
        __guest_handle_const_void vcpumask;
    } arg2;
};


