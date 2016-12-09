// dsw: pieces presumably from the linux kernel
// (cqual/tests/linux/rtc.i) containing __attribute__-s

/* check C comments ***** ***/
/* check C comments ***** oink*/
/* check C comments ***** oink*/
// check C++ comments /* /*
// */

// these should still show up
/* __attribute__((blah)) */
// __attribute__((blah))
int boink() {
  char *x = "__attribute__((rah))"; // should show up
  __attribute__/*a comment*/(// another comment
                             (oink
                              )// another comment
                             )// another comment
    int y;
}

typedef unsigned short umode_t;
typedef __signed__ char __s8;
typedef unsigned char __u8;
typedef __signed__ short __s16;
typedef unsigned short __u16;
typedef __signed__ int __s32;
typedef unsigned int __u32;
typedef __signed__ long long __s64;
typedef unsigned long long __u64;
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef u32 dma_addr_t;

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;
typedef unsigned char		unchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef		__u8		u_int8_t;
typedef		__s8		int8_t;
typedef		__u16		u_int16_t;
typedef		__s16		int16_t;
typedef		__u32		u_int32_t;
typedef		__s32		int32_t;
typedef		__u8		uint8_t;
typedef		__u16		uint16_t;
typedef		__u32		uint32_t;
typedef		__u64		uint64_t;
typedef		__u64		u_int64_t;
typedef		__s64		int64_t;

typedef struct { volatile int counter; } atomic_t;
extern int printk(const char * fmt, ...)
	__attribute__ ((format (printf, 1, 2)));
static const char __module_kernel_version[] __attribute__((section(".modinfo"))) =
"kernel_version=" "2.4.0-test4" ;
  void panic(const char * fmt, ...)
	__attribute__ ((noreturn,  format (printf, 1, 2)));
  void do_exit(long error_code)
	__attribute__((noreturn)) ;
  __attribute__((regparm(0)))  int printk(const char * fmt, ...)
	__attribute__ ((format (printf, 1, 2)));
struct i387_fxsave_struct {
	unsigned short	cwd;
	unsigned short	swd;
	unsigned short	twd;
	unsigned short	fop;
	long	fip;
	long	fcs;
	long	foo;
	long	fos;
	long	mxcsr;
	long	reserved;
	long	st_space[32];	 
	long	xmm_space[32];	 
	long	padding[56];
} __attribute__ ((aligned (16)));
extern void  __switch_to(struct task_struct *prev, struct task_struct *next)  __attribute__((regparm(3))) ;
  __attribute__((regparm(0)))  void __down_failed(void  );
  __attribute__((regparm(0)))  int  __down_failed_interruptible(void   );
  __attribute__((regparm(0)))  int  __down_failed_trylock(void   );
  __attribute__((regparm(0)))  void __up_wakeup(void  );
  __attribute__((regparm(0)))  void __down(struct semaphore * sem);
  __attribute__((regparm(0)))  int  __down_interruptible(struct semaphore * sem);
  __attribute__((regparm(0)))  int  __down_trylock(struct semaphore * sem);
  __attribute__((regparm(0)))  void __up(struct semaphore * sem);
extern struct rw_semaphore *  __down_read_failed(struct rw_semaphore *sem)  __attribute__((regparm(3))) ;
struct semaphore {};
struct ncp_inode_info {
	__u32	dirEntNum __attribute__((packed));
	__u32	DosDirNum __attribute__((packed));
	__u32	volNumber __attribute__((packed));
	__u32	nwattr;
	struct semaphore open_sem;
	atomic_t	opened;
	int	access;
	__u32	server_file_handle __attribute__((packed));
	__u8	open_create_action __attribute__((packed));
	__u8	file_handle[6] __attribute__((packed));
};
extern void ext2_error (struct super_block *, const char *, const char *, ...)
	__attribute__ ((format (printf, 3, 4)));
extern   void ext2_panic (struct super_block *, const char *,
				   const char *, ...)
	__attribute__ ((noreturn,  format (printf, 3, 4)));
  __attribute__((regparm(0)))  long sys_open(const char *, int, int);
  __attribute__((regparm(0)))  long sys_close(unsigned int);	 
  __attribute__((regparm(0)))  long sys_personality(unsigned long personality);
struct local_apic {
 	struct { unsigned int  __reserved[4]; } __reserved_01;
 	struct { unsigned int  __reserved[4]; } __reserved_02;
 	struct {  
		unsigned int    __reserved_1	: 24,
			phys_apic_id	:  4,
			__reserved_2	:  4;
		unsigned int  __reserved[3];
	} id;
 	const
	struct {  
		unsigned int    version		:  8,
			__reserved_1	:  8,
			max_lvt		:  8,
			__reserved_2	:  8;
		unsigned int  __reserved[3];
	} version;
 	struct { unsigned int  __reserved[4]; } __reserved_03;
 	struct { unsigned int  __reserved[4]; } __reserved_04;
 	struct { unsigned int  __reserved[4]; } __reserved_05;
 	struct { unsigned int  __reserved[4]; } __reserved_06;
 	struct {  
		unsigned int    priority	:  8,
			__reserved_1	: 24;
		unsigned int  __reserved_2[3];
	} tpr;
 	const
	struct {  
		unsigned int    priority	:  8,
			__reserved_1	: 24;
		unsigned int  __reserved_2[3];
	} apr;
 	const
	struct {  
		unsigned int    priority	:  8,
			__reserved_1	: 24;
		unsigned int  __reserved_2[3];
	} ppr;
 	struct {  
		unsigned int    eoi;
		unsigned int  __reserved[3];
	} eoi;
 	struct { unsigned int  __reserved[4]; } __reserved_07;
 	struct {  
		unsigned int    __reserved_1	: 24,
			logical_dest	:  8;
		unsigned int  __reserved_2[3];
	} ldr;
 	struct {  
		unsigned int    __reserved_1	: 28,
			model		:  4;
		unsigned int  __reserved_2[3];
	} dfr;
 	struct {  
		unsigned int 	spurious_vector	:  8,
			apic_enabled	:  1,
			focus_cpu	:  1,
			__reserved_2	: 22;
		unsigned int  __reserved_3[3];
	} svr;
 	struct {  
 		unsigned int  bitfield;
		unsigned int  __reserved[3];
	} isr [8];
 	struct {  
 		unsigned int  bitfield;
		unsigned int  __reserved[3];
	} tmr [8];
 	struct {  
 		unsigned int  bitfield;
		unsigned int  __reserved[3];
	} irr [8];
 	union {  
		struct {
			unsigned int    send_cs_error			:  1,
				receive_cs_error		:  1,
				send_accept_error		:  1,
				receive_accept_error		:  1,
				__reserved_1			:  1,
				send_illegal_vector		:  1,
				receive_illegal_vector		:  1,
				illegal_register_address	:  1,
				__reserved_2			: 24;
			unsigned int  __reserved_3[3];
		} error_bits;
		struct {
			unsigned int  errors;
			unsigned int  __reserved_3[3];
		} all_errors;
	} esr;
 	struct { unsigned int  __reserved[4]; } __reserved_08;
 	struct { unsigned int  __reserved[4]; } __reserved_09;
 	struct { unsigned int  __reserved[4]; } __reserved_10;
 	struct { unsigned int  __reserved[4]; } __reserved_11;
 	struct { unsigned int  __reserved[4]; } __reserved_12;
 	struct { unsigned int  __reserved[4]; } __reserved_13;
 	struct { unsigned int  __reserved[4]; } __reserved_14;
 	struct {  
		unsigned int    vector			:  8,
			delivery_mode		:  3,
			destination_mode	:  1,
			delivery_status		:  1,
			__reserved_1		:  1,
			level			:  1,
			trigger			:  1,
			__reserved_2		:  2,
			shorthand		:  2,
			__reserved_3		:  12;
		unsigned int  __reserved_4[3];
	} icr1;
 	struct {  
		union {
			unsigned int    __reserved_1	: 24,
				phys_dest	:  4,
				__reserved_2	:  4;
			unsigned int    __reserved_3	: 24,
				logical_dest	:  8;
		} dest;
		unsigned int  __reserved_4[3];
	} icr2;
 	struct {  
		unsigned int    vector		:  8,
			__reserved_1	:  4,
			delivery_status	:  1,
			__reserved_2	:  3,
			mask		:  1,
			timer_mode	:  1,
			__reserved_3	: 14;
		unsigned int  __reserved_4[3];
	} lvt_timer;
 	struct { unsigned int  __reserved[4]; } __reserved_15;
 	struct {  
		unsigned int    vector		:  8,
			delivery_mode	:  3,
			__reserved_1	:  1,
			delivery_status	:  1,
			__reserved_2	:  3,
			mask		:  1,
			__reserved_3	: 15;
		unsigned int  __reserved_4[3];
	} lvt_pc;
 	struct {  
		unsigned int    vector		:  8,
			delivery_mode	:  3,
			__reserved_1	:  1,
			delivery_status	:  1,
			polarity	:  1,
			remote_irr	:  1,
			trigger		:  1,
			mask		:  1,
			__reserved_2	: 15;
		unsigned int  __reserved_3[3];
	} lvt_lint0;
 	struct {  
		unsigned int    vector		:  8,
			delivery_mode	:  3,
			__reserved_1	:  1,
			delivery_status	:  1,
			polarity	:  1,
			remote_irr	:  1,
			trigger		:  1,
			mask		:  1,
			__reserved_2	: 15;
		unsigned int  __reserved_3[3];
	} lvt_lint1;
 	struct {  
		unsigned int    vector		:  8,
			__reserved_1	:  4,
			delivery_status	:  1,
			__reserved_2	:  3,
			mask		:  1,
			__reserved_3	: 15;
		unsigned int  __reserved_4[3];
	} lvt_error;
 	struct {  
		unsigned int    initial_count;
		unsigned int  __reserved_2[3];
	} timer_icr;
 	const
	struct {  
		unsigned int    curr_count;
		unsigned int  __reserved_2[3];
	} timer_ccr;
 	struct { unsigned int  __reserved[4]; } __reserved_16;
 	struct { unsigned int  __reserved[4]; } __reserved_17;
 	struct { unsigned int  __reserved[4]; } __reserved_18;
 	struct { unsigned int  __reserved[4]; } __reserved_19;
 	struct {  
		unsigned int    divisor		:  4,
			__reserved_1	: 28;
		unsigned int  __reserved_2[3];
	} timer_dcr;
 	struct { unsigned int  __reserved[4]; } __reserved_20;
} __attribute__ ((packed));

typedef int (*__init_module_func_t)(void);
typedef void (*__cleanup_module_func_t)(void);

// dsw: doctored from the real thing
static int   rtc_init(void)
{ return 0; }
static void   rtc_exit (void)
{ return 0; }

int init_module(void) __attribute__((alias("rtc_init"))); extern inline __init_module_func_t __init_module_inline(void) { return  rtc_init ; } ;
void cleanup_module(void) __attribute__((alias("rtc_exit"))); extern inline __cleanup_module_func_t __cleanup_module_inline(void) { return  rtc_exit ; } ;
