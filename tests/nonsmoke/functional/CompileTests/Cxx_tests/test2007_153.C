/*
Hi Christian,

Today is my last day and I found an assertion fail while parsing a program in system dependence graph.
It's an MPI program with mpi.h header file. I didn't know this as I didn't include the header file so far.
Hope this can help you finding why this happens.

Also, the other email I previously sent regarding "addLink" function defined in SimpleDirectedGraph.h, I couldn't reproduce the case..
I'll try again to find one but you know where it happens.

Anyway, thank you again for all your help.

Thank you

Han


====================================================================================


Cxx_Grammar.C:59051: SgFunctionSymbol* SgFunctionRefExp::get_symbol_i() const: Assertion `this != __null' failed.

Program received signal SIGABRT, Aborted.
[Switching to Thread -1209092416 (LWP 8466)]
0x004727a2 in _dl_sysinfo_int80 () from /lib/ld-linux.so.2
(gdb) where
#0  0x004727a2 in _dl_sysinfo_int80 () from /lib/ld-linux.so.2
#1  0x002447a5 in raise () from /lib/tls/libc.so.6
#2  0x00246209 in abort () from /lib/tls/libc.so.6
#3  0x0023dd91 in __assert_fail () from /lib/tls/libc.so.6
#4  0x00c04e32 in SgFunctionRefExp::get_symbol_i (this=0x0) at Cxx_Grammar.C:59051
#5  0x00c050be in SgFunctionRefExp::get_symbol (this=0x0) at Cxx_Grammar.C:59099
#6  0x018053d6 in SystemDependenceGraph::getPossibleFuncs (this=0x9e26930, funcCall=0x9883a10) at ../../../../../ROSE-May-8a-2007-src/src/midend/programAnalysis/staticInterproceduralSlicing/SystemDependenceGraph.C:413
#7  0x018057e4 in SystemDependenceGraph::doInterproceduralConnections (this=0x9e26930, ii=0xa787940) at ../../../../../ROSE-May-8a-2007-src/src/midend/programAnalysis/staticInterproceduralSlicing/SystemDependenceGraph.C:489
#8  0x0180531b in SystemDependenceGraph::performInterproceduralAnalysis (this=0x9e26930) at ../../../../../ROSE-May-8a-2007-src/src/midend/programAnalysis/staticInterproceduralSlicing/SystemDependenceGraph.C:403
#9  0x01803ed5 in SystemDependenceGraph::parseProject (this=0x9e26930, project=0x918dff8) at ../../../../../ROSE-May-8a-2007-src/src/midend/programAnalysis/staticInterproceduralSlicing/SystemDependenceGraph.C:152
#10 0x08078e86 in MPIOptimization::MPINonBlockingCodeMotion::run (this=0x9e35700) at MPINonBlockingCodeMotion.C:59
#11 0x0808114d in main (argc=4, argv=0xbfeacac4) at testMPINonBlockingCodeMotion.C:15

-- 
Han Suk Kim

Summer Student
@Lawrence Livermore National Laboratory

Email: kim54@llnl.gov
Tel: 925-422-1289
Mail Code: L-419
*/

#include "mpi.h"
#include <stdio.h>

#define BUFSIZE (1024*1024*50)
#define NITER 1

double buf[BUFSIZE];
double buf2[BUFSIZE*2];
double buf3[BUFSIZE*2];

int main(int argc, char* argv[])
{
  int i, iter;
  int nproc;
  int rank;
  int next;
  int prev;

  int tag=1;
  MPI_Request req[2];
  MPI_Status status[2];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  MPI_Barrier(MPI_COMM_WORLD);

  prev = (rank - 1 + nproc) % nproc;
  next = (rank + 1) % nproc;

  // computation
  for(iter = 0; iter < NITER; iter++)
  {
	for(i = 0; i < BUFSIZE; i++)
	{
		buf[i] = buf[i] / 1.6875;

	}
  }

   // communication
  MPI_Irecv(&buf2[1], BUFSIZE, MPI_DOUBLE, prev, tag, MPI_COMM_WORLD, &req[0]);
  MPI_Isend(&buf3[0], BUFSIZE, MPI_DOUBLE, next, tag, MPI_COMM_WORLD, &req[1]);
 
  MPI_Waitall(2, req, status);

  MPI_Finalize();

  return 0;
}
