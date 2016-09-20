
typedef unsigned long uint64_t;
typedef unsigned int  uint32_t;

struct e820_entry 
   {
     uint64_t start;
     uint64_t len;
     uint32_t type;
   } __attribute__ (( packed ));

// Original code:
// static struct e820_entry __attribute__ (( section ( ".text16.data" ) )) _text16_e820map [] __asm__ ( "e820map" ) __attribute__ (( used )) = { { 0x00000000ULL, ( 0x000a0000ULL - 0x00000000ULL ), 1 }, };
// Error unparses to:
// static struct e820_entry _text16_e820map asm ("e820map")[] __attribute__((used))  = {{(0x00000000ULL), ((0x000a0000ULL - 0x00000000ULL)), (1)}};
// Note that asm ("e820map") should be unparsed after the "[]".
static struct e820_entry __attribute__ (( section ( ".text16.data" ) )) _text16_e820map [] __asm__ ( "e820map" ) __attribute__ (( used )) = 
   {
     { 0x00000000ULL, ( 0x000a0000ULL - 0x00000000ULL ), 1 },
   };
