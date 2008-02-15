#ifndef _mpi_port_h
#define _mpi_port_h

/* Specified as part of P++ diagnostics */
extern int PARTI_numberOfMessagesSent;
extern int PARTI_numberOfMessagesRecieved;
extern int PARTI_messagePassingInterpretationReport;

#if 0
extern int PARTI_MPI_numprocs();
extern int PARTI_MPI_gsync();

extern int PARTI_MPI_myproc();

extern int PARTI_MPI_csend();
extern int PARTI_MPI_crecv();

extern int PARTI_MPI_isend();
extern int PARTI_MPI_irecv();
extern int PARTI_MPI_msgwait();
extern int PARTI_MPI_msgdone();
#else
int PARTI_MPI_numprocs();
int PARTI_MPI_gsync();

int PARTI_MPI_myproc();

#if 1
int PARTI_MPI_csend(int tag, char* buf, int len, int node, MPI_Request* pid);
int PARTI_MPI_crecv(int tag, char* buf, int len, int node, MPI_Request* pid);
#endif

int PARTI_MPI_isend(int tag, char* buf, int len, int node, MPI_Request* pid);
int PARTI_MPI_irecv(int tag, char* buf, int len, int node, MPI_Request* pid);
int PARTI_MPI_msgwait(MPI_Request* gid);
int PARTI_MPI_msgdone(MPI_Request* gid);
#endif

#endif
