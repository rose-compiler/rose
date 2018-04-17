/* Copyright Robert C. Ferrell, CPCA Ltd.  1995 */

/* $Id: send-rcv.h,v 1.1 2004/07/07 10:27:17 dquinlan Exp $ */

/* Header file for send and recive routines. */

#define SEND_RCV_INCLUDE_FILE 1


typedef struct {
  int PE;
} PHYS_ADDRESS;

/* SEND_BUFFER is replicated once for each message to send */
typedef struct {
  PHYS_ADDRESS    dest_PE;
  int             n_send_items;
  int             n_send_words;
  int             send_TAG;
  int		 *send_data_int;
  float		 *send_data_float;
  double	 *send_data_double;
  int		 *send_data_log;
} SEND_BUFFER;

/* COMM_SEND is the structure for a send/receive call.  It holds the information
   for all the sends.*/
typedef struct {
  int           N_Send;
  SEND_BUFFER **S_Buffers;
  MPI_Request	*send_request;
  MPI_Status	*send_status;
} COMM_SEND;

/* RCV_BUFFER is replicated once for each message to be 
   received or actually received */
typedef struct {
  PHYS_ADDRESS    src_PE;
  int             n_max_rcv_items;
  int             n_max_rcv_words;
  int             n_rcvd_words;
  int             rcv_TAG;
  int		  *rcv_data_int;
  float		  *rcv_data_float;
  double	  *rcv_data_double;
  int		  *rcv_data_log;
} RCV_BUFFER;

/* COMM_RCV is the strucutre for a send/receive call.  It holds the
   received information. */
typedef struct {
  int          N_Rcv_Buffers;
  int          N_Rcvd;
  int          USE_TAG;
  int          KNOWN_SRC;
  int          ALLOW_RCV_BUFF_RESIZE;
  int          RCV_BUFF_PREALLOCATED;
  int          Element_Size;
  RCV_BUFFER **R_Buffers;
} COMM_RCV;

/* Function declarations for send-rcv */
  int  cpcomm_attempt_receive(COMM_RCV *, int);
  void cpcomm_constrained_send_rcv(COMM_SEND, COMM_RCV *);
  void cpcomm_random_send_rcv(COMM_SEND, COMM_RCV *);
