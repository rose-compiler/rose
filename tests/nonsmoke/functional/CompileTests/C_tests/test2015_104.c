
typedef unsigned long uint64_t;
typedef unsigned int  uint32_t;

struct e820_entry {
 uint64_t start;
 uint64_t len;
 uint32_t type;
} __attribute__ (( packed ));

static struct e820_entry __attribute__ (( section ( ".text16.data" ) )) _text16_e820map [] __asm__ ( "e820map" ) __attribute__ (( used )) = {
 { 0x00000000ULL, ( 0x000a0000ULL - 0x00000000ULL ), 1 },
 { 0x00100000ULL, ( 0xcfb50000ULL - 0x00100000ULL ), 1 },
 { 0xcfb50000ULL, ( 0xcfb64000ULL - 0xcfb50000ULL ), 2 },
 { 0xcfb64000ULL, ( 0xcfb66000ULL - 0xcfb64000ULL ), 2 },
 { 0xcfb66000ULL, ( 0xcfb85c00ULL - 0xcfb66000ULL ), 3 },
 { 0xcfb85c00ULL, ( 0xd0000000ULL - 0xcfb85c00ULL ), 2 },
 { 0xe0000000ULL, ( 0xf0000000ULL - 0xe0000000ULL ), 2 },
 { 0xfe000000ULL, (0x100000000ULL - 0xfe000000ULL ), 2 },
 {0x100000000ULL, (0x230000000ULL -0x100000000ULL ), 1 },
};
