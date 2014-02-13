
typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;

#define PACKED __attribute__ ((__packed__))

struct msdos_dir_entry {
	char     name[11];       /* 000 name and extension */
	uint8_t  attr;           /* 00b attribute bits */
	uint8_t  lcase;          /* 00c case for base and extension */
	uint8_t  ctime_cs;       /* 00d creation time, centiseconds (0-199) */
	uint16_t ctime;          /* 00e creation time */
	uint16_t cdate;          /* 010 creation date */
	uint16_t adate;          /* 012 last access date */
	uint16_t starthi;        /* 014 high 16 bits of cluster in FAT32 */
	uint16_t time;           /* 016 time */
	uint16_t date;           /* 018 date */
	uint16_t start;          /* 01a first cluster */
	uint32_t size;           /* 01c file size in bytes */
} PACKED;

struct msdos_volume_info { /* (offsets are relative to start of boot sector) */
	uint8_t  drive_number;    /* 040 BIOS drive number */
	uint8_t  reserved;        /* 041 unused */
	uint8_t  ext_boot_sign;	  /* 042 0x29 if fields below exist (DOS 3.3+) */
	uint32_t volume_id32;     /* 043 volume ID number */
	char     volume_label[11];/* 047 volume label */
	char     fs_type[8];      /* 052 typically "FATnn" */
} PACKED;                         /* 05a end. Total size 26 (0x1a) bytes */

struct msdos_boot_sector {
	/* We use strcpy to fill both, and gcc-4.4.x complains if they are separate */
	char     boot_jump_and_sys_id[3+8]; /* 000 short or near jump instruction */
	/*char   system_id[8];*/     /* 003 name - can be used to special case partition manager volumes */
	uint16_t bytes_per_sect;     /* 00b bytes per logical sector */
	uint8_t  sect_per_clust;     /* 00d sectors/cluster */
	uint16_t reserved_sect;      /* 00e reserved sectors (sector offset of 1st FAT relative to volume start) */
	uint8_t  fats;               /* 010 number of FATs */
	uint16_t dir_entries;        /* 011 root directory entries */
	uint16_t volume_size_sect;   /* 013 volume size in sectors */
	uint8_t  media_byte;         /* 015 media code */
	uint16_t sect_per_fat;       /* 016 sectors/FAT */
	uint16_t sect_per_track;     /* 018 sectors per track */
	uint16_t heads;              /* 01a number of heads */
	uint32_t hidden;             /* 01c hidden sectors (sector offset of volume within physical disk) */
	uint32_t fat32_volume_size_sect; /* 020 volume size in sectors (if volume_size_sect == 0) */
	uint32_t fat32_sect_per_fat; /* 024 sectors/FAT */
	uint16_t fat32_flags;        /* 028 bit 8: fat mirroring, low 4: active fat */
	uint8_t  fat32_version[2];   /* 02a major, minor filesystem version (I see 0,0) */
	uint32_t fat32_root_cluster; /* 02c first cluster in root directory */
	uint16_t fat32_info_sector;  /* 030 filesystem info sector (usually 1) */
	uint16_t fat32_backup_boot;  /* 032 backup boot sector (usually 6) */
	uint32_t reserved2[3];       /* 034 unused */
	struct msdos_volume_info vi; /* 040 */
	char     boot_code[0x200 - 0x5a - 2]; /* 05a */
#define BOOT_SIGN 0xAA55
	uint16_t boot_sign;          /* 1fe */
} PACKED;

#define FAT_FSINFO_SIG1 0x41615252

#define FAT_FSINFO_SIG2 0x61417272
struct fat32_fsinfo {
	uint32_t signature1;         /* 0x52,0x52,0x41,0x61, "RRaA" */
	uint32_t reserved1[128 - 8];
	uint32_t signature2;         /* 0x72,0x72,0x61,0x41, "rrAa" */
	uint32_t free_clusters;      /* free cluster count.  -1 if unknown */
	uint32_t next_cluster;       /* most recently allocated cluster */
	uint32_t reserved2[3];
	uint16_t reserved3;          /* 1fc */
	uint16_t boot_sign;          /* 1fe */
} PACKED;

struct bug_check 
   {
     char BUG1[sizeof(struct msdos_dir_entry  ) == 0x20 ? 1 : -1];
     char BUG2[sizeof(struct msdos_volume_info) == 0x1a ? 1 : -1];
     char BUG3[sizeof(struct msdos_boot_sector) == 0x200 ? 1 : -1];
     char BUG4[sizeof(struct fat32_fsinfo     ) == 0x200 ? 1 : -1];
};

