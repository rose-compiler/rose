
typedef int uint8_t;
typedef int uint16_t;
typedef int uint32_t;
typedef int uint64_t;
typedef int int8_t;
typedef int int16_t;
typedef int int32_t;
typedef int int64_t;

struct xenpf_efi_runtime_call {
//  unsigned long status;
    union {
#if 1
        struct {
            struct xenpf_efi_time {
                uint8_t daylight;
            } time;
            uint32_t accuracy;
        } get_time;
        struct xenpf_efi_time set_time;
#endif
#if 1
        struct {
            unsigned long size;
            struct xenpf_efi_guid {
                uint32_t data1;
                uint16_t data2;
            } vendor_guid;
        } get_variable, set_variable;
        struct {
            unsigned long size;
            struct xenpf_efi_guid vendor_guid;
        } get_next_variable_name;
#endif
    } u;
};
