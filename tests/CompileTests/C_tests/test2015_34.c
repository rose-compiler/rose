//#include "stdint.h"

#define ZIP_HEADER_LEN 26

#define PACKED __attribute__ ((__packed__))

typedef unsigned long int size_t;

#define offsetof(type, member)  __builtin_offsetof (type, member)

typedef union {
	uint8_t raw[ZIP_HEADER_LEN];
	struct {
		uint16_t version;               /* 0-1 */
		uint16_t zip_flags;             /* 2-3 */
		uint16_t method;                /* 4-5 */
		uint16_t modtime;               /* 6-7 */
		uint16_t moddate;               /* 8-9 */
		uint32_t crc32 PACKED;          /* 10-13 */
		uint32_t cmpsize PACKED;        /* 14-17 */
		uint32_t ucmpsize PACKED;       /* 18-21 */
		uint16_t filename_len;          /* 22-23 */
		uint16_t extra_len;             /* 24-25 */
	} formatted PACKED;
} zip_header_t; /* PACKED - gcc 4.2.1 doesn't like it (spews warning) */

struct BUG_zip_header_must_be_26_bytes 
   {
  // char BUG_zip_header_must_be_26_bytes[offsetof(zip_header_t, formatted.extra_len) + 2 == ZIP_HEADER_LEN ? 1 : -1];
     char BUG_zip_header_must_be_26_bytes[offsetof(zip_header_t, formatted.extra_len) + 2 == ZIP_HEADER_LEN ? 1 : -1];
   };
