typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;

#define PACKED __attribute__ ((__packed__))

struct msdos_volume_info { /* (offsets are relative to start of boot sector) */
	uint8_t  drive_number;    /* 040 BIOS drive number */
	uint8_t  reserved;        /* 041 unused */
	uint8_t  ext_boot_sign;	  /* 042 0x29 if fields below exist (DOS 3.3+) */
	uint32_t volume_id32;     /* 043 volume ID number */
	char     volume_label[11];/* 047 volume label */
	char     fs_type[8];      /* 052 typically "FATnn" */
} PACKED;                         /* 05a end. Total size 26 (0x1a) bytes */

struct bug_check 
   {
     char BUG2[sizeof(struct msdos_volume_info) == 0x1a ? 1 : -1];
   };

