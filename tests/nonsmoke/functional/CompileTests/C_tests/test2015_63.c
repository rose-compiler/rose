

typedef int uint8_t;
typedef int uint16_t;
typedef int uint32_t;
typedef int uint64_t;
typedef int int8_t;
typedef int int16_t;
typedef int int32_t;
typedef int int64_t;

struct xenpf_efi_runtime_call {
    uint32_t function;
    uint32_t misc;
    unsigned long status;
    union {
        struct {
            struct xenpf_efi_time {
                uint16_t year;
                uint8_t month;
                uint8_t day;
                uint8_t hour;
                uint8_t min;
                uint8_t sec;
                uint32_t ns;
                int16_t tz;
                uint8_t daylight;
            } time;
            uint32_t resolution;
            uint32_t accuracy;
        } get_time;
        struct xenpf_efi_time set_time;
        struct xenpf_efi_time get_wakeup_time;
        struct xenpf_efi_time set_wakeup_time;
        struct {
         // __guest_handle_void name;
            unsigned long size;
         // __guest_handle_void data;
            struct xenpf_efi_guid {
                uint32_t data1;
                uint16_t data2;
                uint16_t data3;
                uint8_t data4[8];
            } vendor_guid;
        } get_variable, set_variable;
        struct {
            unsigned long size;
         // __guest_handle_void name;
            struct xenpf_efi_guid vendor_guid;
        } get_next_variable_name;
        struct {
            uint32_t attr;
            uint64_t max_store_size;
            uint64_t remain_store_size;
            uint64_t max_size;
        } query_variable_info;
        struct {
         // __guest_handle_void capsule_header_array;
            unsigned long capsule_count;
            uint64_t max_capsule_size;
            unsigned int reset_type;
        } query_capsule_capabilities;
        struct {
         // __guest_handle_void capsule_header_array;
            unsigned long capsule_count;
            uint64_t sg_list;
        } update_capsule;
    } u;
};
