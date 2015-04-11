typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u32 size_t;

struct descloc_s {
    u16 length;
    u32 addr;
//  u64 addr;
} __attribute__((packed));

u8 dummy_IDT __attribute__((section(".data.varfseg." "src/test2015_01.c" "." "134"))) ;

// const u64 dummy_IDT_ptr = (u64)&dummy_IDT;

struct descloc_s pmode_IDT_info __attribute__((section(".data.varfseg." "src/test2015_01.c" "." "137"))) = {
    .length = sizeof(dummy_IDT) - 1,
//  .addr = (u32)&dummy_IDT,
//  .addr = dummy_IDT_ptr,
//  .addr = ({ const u64 dummy_IDT_ptr = (u64)&dummy_IDT; .addr = dummy_IDT_ptr; }),
//  .addr = (u32)&dummy_IDT,
    .addr = 0L,
};

u64 rombios32_gdt[] __attribute__((section(".data.varfseg." "src/test2015_01.c" "." "143"))) __attribute__((aligned(8))) = {
    0x0000000000000000LL,
    ((0x1ULL << 55) | ((((u64)((0xffffffff) >> 12) & 0x000f0000) << 32) | (((u64)((0xffffffff) >> 12) & 0x0000ffff) << 0))) | (0x9bULL << 40) | (0x1ULL << 54),
    ((0x1ULL << 55) | ((((u64)((0xffffffff) >> 12) & 0x000f0000) << 32) | (((u64)((0xffffffff) >> 12) & 0x0000ffff) << 0))) | (0x93ULL << 40) | (0x1ULL << 54),
    ((((u64)(0x10000 -1) & 0x000f0000) << 32) | (((u64)(0x10000 -1) & 0x0000ffff) << 0)) | (0x9bULL << 40) | ((((u64)(0xf0000) & 0xff000000) << 32) | (((u64)(0xf0000) & 0x00ffffff) << 16)),
    ((((u64)(0x0ffff) & 0x000f0000) << 32) | (((u64)(0x0ffff) & 0x0000ffff) << 0)) | (0x93ULL << 40),
    ((0x1ULL << 55) | ((((u64)((0xffffffff) >> 12) & 0x000f0000) << 32) | (((u64)((0xffffffff) >> 12) & 0x0000ffff) << 0))) | (0x9bULL << 40) | ((((u64)(0xf0000) & 0xff000000) << 32) | (((u64)(0xf0000) & 0x00ffffff) << 16)),
    ((0x1ULL << 55) | ((((u64)((0xffffffff) >> 12) & 0x000f0000) << 32) | (((u64)((0xffffffff) >> 12) & 0x0000ffff) << 0))) | (0x93ULL << 40),
};

struct descloc_s rombios32_gdt_48 __attribute__((section(".data.varfseg." "src/test2015_01.c" "." "161"))) = {
    .length = sizeof(rombios32_gdt) - 1,
//  .addr = (u32)rombios32_gdt,
    .addr = 0L,
};

