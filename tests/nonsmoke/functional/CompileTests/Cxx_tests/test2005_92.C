
#define __BIG_ENDIAN 0
#define __LITTLE_ENDIAN 1

#define __BYTE_ORDER __LITTLE_ENDIAN

union wait
  {
    int w_status;

    struct
      {
# if	__BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int __w_termsig:7; /* Terminating signal.  */
	unsigned int __w_coredump:1; /* Set if dumped core.  */
	unsigned int __w_retcode:8; /* Return code if exited normally.  */
	unsigned int:16;
# endif				/* Little endian.  */
# if	__BYTE_ORDER == __BIG_ENDIAN
	unsigned int:16;
	unsigned int __w_retcode:8;
	unsigned int __w_coredump:1;
	unsigned int __w_termsig:7;
# endif				/* Big endian.  */
      } __wait_terminated;

    struct
      {
# if	__BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int __w_stopval:8; /* W_STOPPED if stopped.  */
	unsigned int __w_stopsig:8; /* Stopping signal.  */
	unsigned int:16;
# endif				/* Little endian.  */
# if	__BYTE_ORDER == __BIG_ENDIAN
	unsigned int:16;
	unsigned int __w_stopsig:8; /* Stopping signal.  */
	unsigned int __w_stopval:8; /* W_STOPPED if stopped.  */
# endif				/* Big endian.  */
      } __wait_stopped;
  };

