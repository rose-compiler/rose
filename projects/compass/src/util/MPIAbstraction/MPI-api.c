/* Use this file to derive MPI.api */
#include <mpi.h>

void declareConstants (void)
{
  const int MPI_SUCCESS;
  const int MPI_ERR_BUFFER;
  const int MPI_ERR_COUNT;
  const int MPI_ERR_TYPE;
  const int MPI_ERR_TAG;
  const int MPI_ERR_COMM;
  const int MPI_ERR_RANK;
  const int MPI_ERR_REQUEST;
  const int MPI_ERR_ROOT;
  const int MPI_ERR_GROUP;
  const int MPI_ERR_OP;
  const int MPI_ERR_TOPOLOGY;
  const int MPI_ERR_DIMS;
  const int MPI_ERR_ARG;
  const int MPI_ERR_UNKNOWN;
  const int MPI_ERR_TRUNCATE;
  const int MPI_ERR_OTHER;
  const int MPI_ERR_INTERN;
  const int MPI_PENDING;
  const int MPI_ERR_IN_STATUS;
  const int MPI_ERR_LASTCODE;

  /* Assorted constants */
  const int MPI_BOTTOM;
  const int MPI_PROC_NULL;
  const int MPI_ANY_SOURCE;
  const int MPI_ANY_TAG;
  const int MPI_UNDEFINED;
  const int MPI_BSEND_OVERHEAD;
  const int MPI_KEYVAL_INVALID;

  const int MPI_STATUS_SIZE;
  const int MPI_SOURCE;
  const int MPI_TAG;
  const int MPI_ERROR;

  const int MPI_ERRORS_ARE_FATAL;
  const int MPI_ERRORS_RETURN;

  const int MPI_MAX_PROCESSOR_NAME;
  const int MPI_MAX_ERROR_STRING;

  /* Primitive C types */
  const int MPI_CHAR;
  const int MPI_SHORT;
  const int MPI_INT;
  const int MPI_LONG;
  const int MPI_UNSIGNED_CHAR;
  const int MPI_UNSIGNED_SHORT;
  const int MPI_UNSIGNED;
  const int MPI_UNSIGNED_LONG;
  const int MPI_FLOAT;
  const int MPI_DOUBLE;
  const int MPI_LONG_DOUBLE;
  const int MPI_BYTE;
  const int MPI_PACKED;

  /* Optional primitive C types */
  const int MPI_LONG_LONG;
  const int MPI_LONG_LONG_INT;

  /* Reductions in C */
  const int MPI_FLOAT_INT;
  const int MPI_DOUBLE_INT;
  const int MPI_LONG_INT;
  const int MPI_2INT;
  const int MPI_SHORT_INT;
  const int MPI_LONG_DOUBLE_INT;

#if 0 /* Fortran types */
  const int MPI_INTEGER;
  const int MPI_REAL;
  const int MPI_DOUBLE_PRECISION;
  const int MPI_COMPLEX;
  const int MPI_DOUBLE_COMPLEX;
  const int MPI_LOGICAL;
  const int MPI_CHARACTER;

  /* Optional Fortran types */
  const int MPI_INTEGER1;
  const int MPI_INTEGER2;
  const int MPI_INTEGER4;
  const int MPI_REAL2;
  const int MPI_REAL4;
  const int MPI_REAL8;

  /* Reductions in Fortran */
  const int MPI_2REAL;
  const int MPI_2DOUBLE_PRECISION;
  const int MPI_2INTEGER;
#endif /* Fortran */

  /* Derived datatype related */
  const int MPI_UB;
  const int MPI_LB;

  /* Communicators */
  const int MPI_COMM_WORLD;
  const int MPI_COMM_SELF;

  /* Results of communicators and group comparisons */
  const int MPI_IDENT;
  const int MPI_CONGRUENT;
  const int MPI_SIMILAR;
  const int MPI_UNEQUAL;

  /* Environmental inquiry */
  const int MPI_TAG_UB;
  const int MPI_IO;
  const int MPI_HOST;
  const int MPI_WTIME_IS_GLOBAL;

  /* Collective operators */
  const int MPI_MAX;
  const int MPI_MIN;
  const int MPI_SUM;
  const int MPI_PROD;
  const int MPI_MAXLOC;
  const int MPI_MINLOC;
  const int MPI_BAND;
  const int MPI_BOR;
  const int MPI_BXOR;
  const int MPI_LAND;
  const int MPI_LOR;
  const int MPI_LXOR;

  /* NULL handles */
  const int MPI_GROUP_NULL;
  const int MPI_COMM_NULL;
  const int MPI_DATATYPE_NULL;
  const int MPI_REQUEST_NULL;
  const int MPI_OP_NULL;
  const int MPI_ERRHANDLER_NULL;

  /* Empty group */
  const int MPI_GROUP_EMPTY;

  /* Topologies */
  const int MPI_GRAPH;
  const int MPI_CART;
}

void declareTypes (void)
{
  typedef int MPI_Aint;
  typedef int MPI_Status;
  typedef int MPI_Group;
  typedef int MPI_Comm;
  typedef int MPI_Datatype;
  typedef int MPI_Request;
  typedef int MPI_Op;
  typedef int MPI_User_function;
}

void declareBindings (void)
{
  /* === Point-to-point === */
  void* buf;
  int count;
  MPI_Datatype datatype;
  int dest;
  int tag;
  MPI_Comm comm;
  MPI_Send (buf, count, datatype, dest, tag, comm); // L12
  int source;
  MPI_Status status;
  MPI_Recv (buf, count, datatype, source, tag, comm, &status); // L15
  MPI_Get_count (&status, datatype, &count);
  MPI_Bsend (buf, count, datatype, dest, tag, comm);
  MPI_Ssend (buf, count, datatype, dest, tag, comm);
  MPI_Rsend (buf, count, datatype, dest, tag, comm);
  void* buffer;
  int size;
  MPI_Buffer_attach (buffer, size); // L22
  MPI_Buffer_detach (buffer, &size);
  MPI_Request request;
  MPI_Isend (buf, count, datatype, dest, tag, comm, &request); // L25
  MPI_Ibsend (buf, count, datatype, dest, tag, comm, &request);
  MPI_Issend (buf, count, datatype, dest, tag, comm, &request);
  MPI_Irsend (buf, count, datatype, dest, tag, comm, &request);
  MPI_Irecv (buf, count, datatype, source, tag, comm, &request);
  MPI_Wait (&request, &status);
  int flag;
  MPI_Test (&request, &flag, &status); // L32
  MPI_Request_free (&request);
  MPI_Request* array_of_requests;
  int index;
  MPI_Waitany (count, array_of_requests, &index, &status); // L36
  MPI_Testany (count, array_of_requests, &index, &flag, &status);
  MPI_Status* array_of_statuses;
  MPI_Waitall (count, array_of_requests, array_of_statuses); // L39
  MPI_Testall (count, array_of_requests, &flag, array_of_statuses);
  int incount;
  int outcount;
  int* array_of_indices;
  MPI_Waitsome (incount, array_of_requests, &outcount, array_of_indices,
		array_of_statuses); // L44--45
  MPI_Testsome (incount, array_of_requests, &outcount, array_of_indices,
		array_of_statuses); // L46--47
  MPI_Iprobe (source, tag, comm, &flag, &status); // L48
  MPI_Probe (source, tag, comm, &status);
  MPI_Cancel (&request);
  MPI_Test_cancelled (&status, &flag);
  MPI_Send_init (buf, count, datatype, dest, tag, comm, &request);
  MPI_Bsend_init (buf, count, datatype, dest, tag, comm, &request);
  MPI_Ssend_init (buf, count, datatype, dest, tag, comm, &request);
  MPI_Rsend_init (buf, count, datatype, dest, tag, comm, &request);
  MPI_Recv_init (buf, count, datatype, source, tag, comm, &request);
  MPI_Start (&request);
  MPI_Startall (count, array_of_requests);
  void* sendbuf;
  int sendcount;
  MPI_Datatype sendtype;
  int sendtag;
  void* recvbuf;
  int recvcount;
  MPI_Datatype recvtype;
  MPI_Datatype recvtag;
  MPI_Sendrecv (sendbuf, sendcount, sendtype, dest, sendtag,
		recvbuf, recvcount, recvtype, source, recvtag,
		comm, &status); // L67--69
  MPI_Sendrecv_replace (buf, count, datatype, dest, sendtag, source, recvtag,
			comm, &status); // L70--71
  MPI_Datatype oldtype;
  MPI_Datatype newtype;
  MPI_Type_contiguous (count, oldtype, &newtype); // L74
  int blocklength;
  {
    int stride;
    MPI_Type_vector (count, blocklength, stride, oldtype, &newtype); // L78
  }
  {
    MPI_Aint stride;
    MPI_Type_hvector (count, blocklength, stride, oldtype, &newtype); // L82
  }
  int* array_of_blocklengths;
  {
    int* array_of_displacements;
    MPI_Type_indexed (count, array_of_blocklengths, array_of_displacements,
		      oldtype, &newtype); // L87--88
  }
  {
    MPI_Aint* array_of_displacements;
    MPI_Type_hindexed (count, array_of_blocklengths, array_of_displacements,
                       oldtype, &newtype); // L92--93
    MPI_Datatype* array_of_types;
    MPI_Type_struct (count, array_of_blocklengths, array_of_displacements,
                     array_of_types, &newtype); // L95--96
  }
  void* location;
  MPI_Aint address;
  MPI_Address (location, &address); // L100
  MPI_Aint extent;
  MPI_Type_extent (datatype, &extent); // L102
  MPI_Type_size (datatype, &size);
  MPI_Aint displacement;
  MPI_Type_lb (datatype, &displacement); // L105
  MPI_Type_ub (datatype, &displacement);
  MPI_Type_commit (&datatype);
  MPI_Type_free (&datatype);
  MPI_Get_elements (&status, datatype, &count);
  void* inbuf;
  void* outbuf;
  int outsize;
  int position;
  MPI_Pack (inbuf, incount, datatype, outbuf, outsize, &position, comm); // L114
  int insize;
  MPI_Unpack (inbuf, insize, &position, outbuf, outcount, datatype,
	      comm); // L116--117
  MPI_Pack_size (incount, datatype, comm, &size);

  /* === Collectives === */
  MPI_Barrier (comm); // L121
  int root;
  MPI_Bcast (buffer, count, datatype, root, comm); // L123
  MPI_Gather (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype,
	      root, comm); // L124--125
  int* recvcounts;
  int* displs;
  MPI_Gatherv (sendbuf, sendcount, sendtype,
               recvbuf, recvcounts, displs, recvtype,
	       root, comm); // L128--130
  MPI_Scatter (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype,
               root, comm); // L131--132
  int* sendcounts;
  MPI_Scatterv (sendbuf, sendcounts, displs, sendtype,
		recvbuf, recvcount, recvtype, root, comm); // L134--135
  MPI_Allgather (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype,
                 comm); // L136--137
  MPI_Allgatherv (sendbuf, sendcount, sendtype,
		  recvbuf, recvcounts, displs, recvtype,
		  comm); // L138--140
  MPI_Alltoall (sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype,
		comm); // L141--142
  int* sdispls;
  int* rdispls;
  MPI_Alltoallv (sendbuf, sendcounts, sdispls, sendtype,
                 recvbuf, recvcounts, rdispls, recvtype,
		 comm); // L145--147
  MPI_Op op;
  MPI_Reduce (sendbuf, recvbuf, count, datatype, op, root, comm); // L149
#if 0
  MPI_User_function function;
  int commute;
  MPI_Op_create (function, commute, &op); // L153
#endif
  MPI_Op_free (&op); // L155
  MPI_Allreduce (sendbuf, recvbuf, count, datatype, op, comm);
  MPI_Reduce_scatter (sendbuf, recvbuf, recvcounts, datatype, op, comm);
  MPI_Scan (sendbuf, recvbuf, count, datatype, op, comm);

  /* === Groups, contexts, and communicators === */
  MPI_Group group;
  MPI_Group_size (group, &size); // L162
  int rank;
  MPI_Group_rank (group, &rank); // L164
  MPI_Group group1;
  int n;
  int* ranks1;
  MPI_Group group2;
  int* ranks2;
  MPI_Group_translate_ranks (group1, n, ranks1, group2, ranks2); // L170
  int result;
  MPI_Group_compare (group1, group2, &result); // L172
  MPI_Group newgroup;
  MPI_Group_union (group1, group2, &newgroup); // L174
  MPI_Group_intersection (group1, group2, &newgroup);
  MPI_Group_difference (group1, group2, &newgroup);
  int* ranks;
  MPI_Group_incl (group, n, ranks, &newgroup); // L178
  MPI_Group_excl (group, n, ranks, &newgroup);
  extern int ranges[][3];
  MPI_Group_range_incl (group, n, ranges, &newgroup); // L181
  MPI_Group_range_excl (group, n, ranges, &newgroup);
  MPI_Group_free (&group);
  MPI_Comm_size (comm, &size);
  MPI_Comm_rank (comm, &rank);
  MPI_Comm comm1;
  MPI_Comm comm2;
  MPI_Comm_compare (comm1, comm2, &result);
  MPI_Comm newcomm;
  MPI_Comm_dup (comm, &newcomm);
  MPI_Comm_create (comm, group, &newcomm);
  int color;
  int key;
  MPI_Comm_split (comm, color, key, &newcomm); // L194
  MPI_Comm_free (&comm);
  MPI_Comm_test_inter (comm, &flag);
  MPI_Comm_remote_size (comm, &size);
  MPI_Comm_remote_group (comm, &group);
  MPI_Comm local_comm;
  int local_leader;
  MPI_Comm peer_comm;
  int remote_leader;
  MPI_Comm newintercomm;
  MPI_Intercomm_create (local_comm, local_leader, peer_comm, remote_leader, tag,
			&newintercomm); // L204--205
  MPI_Comm intercomm;
  MPI_Comm newintracomm;
  int high;
  MPI_Intercomm_merge (intercomm, high, &newintracomm); // L209
  int keyval;
#if 0
  MPI_Copy_function copy_fn;
  MPI_Delete_function delete_fn;
  void* extra_state;
  MPI_Keyval_create (copy_fn, delete_fn, &keyval, extra_state); // L215
#endif
  MPI_Keyval_free (&keyval); // L217
  void* attribute_val;
  MPI_Attr_put (comm, keyval, attribute_val); // L219
  MPI_Attr_get (comm, keyval, attribute_val, &flag);
  MPI_Attr_delete (comm, keyval);

  /* === Environmental inquiry === */
  char* name;
  int resultlen;
  MPI_Get_processor_name (name, &resultlen); // L226
  MPI_Errhandler errhandler;
#if 0
  MPI_Handler_function function;
  MPI_Errhandler_create (function, &errhandler); // L230
#endif
  MPI_Errhandler_set (comm, errhandler); // L232
  MPI_Errhandler_get (comm, &errhandler);
  MPI_Errhandler_free (&errhandler);
  int errorcode;
  char* string;
  MPI_Error_string (errorcode, string, &resultlen); // L237
  int errorclass;
  MPI_Error_class (errorcode, &errorclass); // L239
  MPI_Wtime ();
  MPI_Wtick ();
  int argc;
  char** argv;
  MPI_Init (&argc, &argv); // L244
  MPI_Finalize ();
  MPI_Initialized (&flag);
  MPI_Abort (comm, errorcode);
}

/* eof */
