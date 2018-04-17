/*BHEADER**********************************************************************
 * (c) 1997   The Regents of the University of California
 *
 * See the file COPYRIGHT_and_DISCLAIMER for a complete copyright
 * notice, contact person, and disclaimer.
 *
 * $Revision: 1.1 $
 *********************************************************************EHEADER*/
/******************************************************************************
 *
 * Routine for computing residuals in the SMG code
 *
 *****************************************************************************/

#include "headers.h"

/*--------------------------------------------------------------------------
 * hypre_SMGResidualData data structure
 *--------------------------------------------------------------------------*/

typedef struct
{
   hypre_Index          base_index;
   hypre_Index          base_stride;

   hypre_StructMatrix  *A;
   hypre_StructVector  *x;
   hypre_StructVector  *b;
   hypre_StructVector  *r;
   hypre_BoxArray      *base_points;
   hypre_ComputePkg    *compute_pkg;

   int                  time_index;
   int                  flops;

} hypre_SMGResidualData;

/*--------------------------------------------------------------------------
 * hypre_SMGResidualCreate
 *--------------------------------------------------------------------------*/

void *
hypre_SMGResidualCreate( )
{
   hypre_SMGResidualData *residual_data;

   residual_data = hypre_CTAlloc(hypre_SMGResidualData, 1);

   (residual_data -> time_index)  = hypre_InitializeTiming("SMGResidual");

   /* set defaults */
   hypre_SetIndex((residual_data -> base_index), 0, 0, 0);
   hypre_SetIndex((residual_data -> base_stride), 1, 1, 1);

   return (void *) residual_data;
}

/*--------------------------------------------------------------------------
 * hypre_SMGResidualSetup
 *--------------------------------------------------------------------------*/

int
hypre_SMGResidualSetup( void               *residual_vdata,
                        hypre_StructMatrix *A,
                        hypre_StructVector *x,
                        hypre_StructVector *b,
                        hypre_StructVector *r              )
{
   int ierr = 0;

   hypre_SMGResidualData  *residual_data = (hypre_SMGResidualData *)residual_vdata;

   hypre_IndexRef          base_index  = (residual_data -> base_index);
   hypre_IndexRef          base_stride = (residual_data -> base_stride);
   hypre_Index             unit_stride;

   hypre_StructGrid       *grid;
   hypre_StructStencil    *stencil;
                       
   hypre_BoxArrayArray    *send_boxes;
   hypre_BoxArrayArray    *recv_boxes;
   int                   **send_processes;
   int                   **recv_processes;
   hypre_BoxArrayArray    *indt_boxes;
   hypre_BoxArrayArray    *dept_boxes;
                       
   hypre_BoxArray         *base_points;
   hypre_ComputePkg       *compute_pkg;

   /*----------------------------------------------------------
    * Set up base points and the compute package
    *----------------------------------------------------------*/

   grid    = hypre_StructMatrixGrid(A);
   stencil = hypre_StructMatrixStencil(A);

   hypre_SetIndex(unit_stride, 1, 1, 1);

   base_points = hypre_BoxArrayDuplicate(hypre_StructGridBoxes(grid));
   hypre_ProjectBoxArray(base_points, base_index, base_stride);

   hypre_CreateComputeInfo(grid, stencil,
                        &send_boxes, &recv_boxes,
                        &send_processes, &recv_processes,
                        &indt_boxes, &dept_boxes);

   hypre_ProjectBoxArrayArray(indt_boxes, base_index, base_stride);
   hypre_ProjectBoxArrayArray(dept_boxes, base_index, base_stride);

   hypre_ComputePkgCreate(send_boxes, recv_boxes,
                       unit_stride, unit_stride,
                       send_processes, recv_processes,
                       indt_boxes, dept_boxes,
                       base_stride, grid,
                       hypre_StructVectorDataSpace(x), 1,
                       &compute_pkg);

   /*----------------------------------------------------------
    * Set up the residual data structure
    *----------------------------------------------------------*/

   (residual_data -> A)           = hypre_StructMatrixRef(A);
   (residual_data -> x)           = hypre_StructVectorRef(x);
   (residual_data -> b)           = hypre_StructVectorRef(b);
   (residual_data -> r)           = hypre_StructVectorRef(r);
   (residual_data -> base_points) = base_points;
   (residual_data -> compute_pkg) = compute_pkg;

   /*-----------------------------------------------------
    * Compute flops
    *-----------------------------------------------------*/

   (residual_data -> flops) =
      (hypre_StructMatrixGlobalSize(A) + hypre_StructVectorGlobalSize(x)) /
      (hypre_IndexX(base_stride) *
       hypre_IndexY(base_stride) *
       hypre_IndexZ(base_stride)  );

   return ierr;
}

/*--------------------------------------------------------------------------
 * hypre_SMGResidual
 *--------------------------------------------------------------------------*/

#include <assert.h>

int
hypre_SMGResidual( void *residual_vdata,
		   hypre_StructMatrix * A,
		   hypre_StructVector * x,
		   hypre_StructVector * b, hypre_StructVector * r )
{
  int ierr = 0;

  hypre_SMGResidualData *residual_data = (hypre_SMGResidualData *)residual_vdata;

  hypre_IndexRef base_stride = (residual_data->base_stride);
  hypre_BoxArray *base_points = (residual_data->base_points);
  hypre_ComputePkg *compute_pkg = (residual_data->compute_pkg);

  hypre_CommHandle *comm_handle;

  hypre_BoxArrayArray *compute_box_aa;
  hypre_BoxArray *compute_box_a;
  hypre_Box *compute_box;

  hypre_Box *A_data_box;
  hypre_Box *x_data_box;
  hypre_Box *b_data_box;
  hypre_Box *r_data_box;

  int Ai;
  int xi;
  int bi;
  int ri;

  double *Ap;
  double *xp;
  double *bp;
  double *rp;

  hypre_Index loop_size;
  hypre_IndexRef start;

  hypre_StructStencil *stencil;
  hypre_Index *stencil_shape;
  int stencil_size;

  int compute_i, i, j, si;
  int loopi, loopj, loopk;

  /* New static variables, precomputed */
  ;


  extern int IS_FINEST_GRID;
#define DO_LOG 0
#if DO_LOG
  FILE* fp_log = NULL;

  if( IS_FINEST_GRID ) {
    fp_log = fopen( "./matrix.dat", "wt" );
  }
#endif

  hypre_BeginTiming( residual_data->time_index );

  stencil = ((A)->stencil);
  stencil_shape = ((stencil)->shape);
  stencil_size = ((stencil)->size);

#if 0
  fprintf( stderr, "Stencil size: %d\n", stencil_size );
  for( si = 0; si < stencil_size; si++ ) {
    fprintf( stderr, "\tshape: [%d][0] == %d\n", si, stencil_shape[si][0] );
    fprintf( stderr, "\tshape: [%d][1] == %d\n", si, stencil_shape[si][1] );
    fprintf( stderr, "\tshape: [%d][2] == %d\n", si, stencil_shape[si][2] );
  }
#endif
#define MAX_STENCIL 15
  assert( stencil_size <= MAX_STENCIL );

  for (compute_i = 0; compute_i < 2; compute_i++)
    {
      switch (compute_i)
	{
	case 0:
	  {
	    xp = ((x)->data);
	    hypre_InitializeIndtComputations (compute_pkg, xp, &comm_handle);
	    compute_box_aa = (compute_pkg->indt_boxes);





	    compute_box_a = base_points;
	    for (i = 0; i < ((compute_box_a)->size); i++)
	      {
		compute_box = &((compute_box_a)->boxes[(i)]);
		start = ((compute_box)->imin);

		b_data_box = &((((b)->data_space))->boxes[(i)]);
		r_data_box = &((((r)->data_space))->boxes[(i)]);

		bp = (((b)->data) + ((b)->data_indices)[i]);
		rp = (((r)->data) + ((r)->data_indices)[i]);

		hypre_BoxGetStrideSize (compute_box, base_stride, loop_size);
		{
		  int hypre__i1start =
		    (((start[0]) - ((((b_data_box)->imin)[0]))) +
		     (((start[1]) - ((((b_data_box)->imin)[1]))) +
		      (((start[2]) -
			((((b_data_box)->imin)[2]))) * (((0) <
							 ((((((b_data_box)->
							      imax)[1])) -
							   ((((b_data_box)->
							      imin)[1])) +
							   1)))
							? ((((((b_data_box)->
							       imax)[1])) -
							    ((((b_data_box)->
							       imin)[1])) +
							    1)) : (0)))) *
		     (((0) <
		       ((((((b_data_box)->imax)[0])) -
			 ((((b_data_box)->imin)[0])) +
			 1))) ? ((((((b_data_box)->imax)[0])) -
				  ((((b_data_box)->imin)[0])) + 1)) : (0)));
		  int hypre__i2start =
		    (((start[0]) - ((((r_data_box)->imin)[0]))) +
		     (((start[1]) - ((((r_data_box)->imin)[1]))) +
		      (((start[2]) -
			((((r_data_box)->imin)[2]))) * (((0) <
							 ((((((r_data_box)->
							      imax)[1])) -
							   ((((r_data_box)->
							      imin)[1])) +
							   1)))
							? ((((((r_data_box)->
							       imax)[1])) -
							    ((((r_data_box)->
							       imin)[1])) +
							    1)) : (0)))) *
		     (((0) <
		       ((((((r_data_box)->imax)[0])) -
			 ((((r_data_box)->imin)[0])) +
			 1))) ? ((((((r_data_box)->imax)[0])) -
				  ((((r_data_box)->imin)[0])) + 1)) : (0)));
		  int hypre__sx1 = ((base_stride[0]));
		  int hypre__sy1 =
		    ((base_stride[1]) *
		     (((0) <
		       ((((((b_data_box)->imax)[0])) -
			 ((((b_data_box)->imin)[0])) +
			 1))) ? ((((((b_data_box)->imax)[0])) -
				  ((((b_data_box)->imin)[0])) + 1)) : (0)));
		  int hypre__sz1 =
		    ((base_stride[2]) *
		     (((0) <
		       ((((((b_data_box)->imax)[0])) -
			 ((((b_data_box)->imin)[0])) +
			 1))) ? ((((((b_data_box)->imax)[0])) -
				  ((((b_data_box)->imin)[0])) +
				  1)) : (0)) * (((0) <
						 ((((((b_data_box)->
						      imax)[1])) -
						   ((((b_data_box)->
						      imin)[1])) +
						   1)))
						? ((((((b_data_box)->
						       imax)[1])) -
						    ((((b_data_box)->
						       imin)[1])) +
						    1)) : (0)));
		  int hypre__sx2 = ((base_stride[0]));
		  int hypre__sy2 =
		    ((base_stride[1]) *
		     (((0) <
		       ((((((r_data_box)->imax)[0])) -
			 ((((r_data_box)->imin)[0])) +
			 1))) ? ((((((r_data_box)->imax)[0])) -
				  ((((r_data_box)->imin)[0])) + 1)) : (0)));
		  int hypre__sz2 =
		    ((base_stride[2]) *
		     (((0) <
		       ((((((r_data_box)->imax)[0])) -
			 ((((r_data_box)->imin)[0])) +
			 1))) ? ((((((r_data_box)->imax)[0])) -
				  ((((r_data_box)->imin)[0])) +
				  1)) : (0)) * (((0) <
						 ((((((r_data_box)->
						      imax)[1])) -
						   ((((r_data_box)->
						      imin)[1])) +
						   1)))
						? ((((((r_data_box)->
						       imax)[1])) -
						    ((((r_data_box)->
						       imin)[1])) +
						    1)) : (0)));
		  int hypre__nx = (loop_size[0]);
		  int hypre__ny = (loop_size[1]);
		  int hypre__nz = (loop_size[2]);
		  int hypre__mx = hypre__nx;
		  int hypre__my = hypre__ny;
		  int hypre__mz = hypre__nz;
		  int hypre__dir, hypre__max;
		  int hypre__div, hypre__mod;
		  int hypre__block, hypre__num_blocks;
		  hypre__dir = 0;
		  hypre__max = hypre__nx;
		  if (hypre__ny > hypre__max)
		    {
		      hypre__dir = 1;
		      hypre__max = hypre__ny;
		    }
		  if (hypre__nz > hypre__max)
		    {
		      hypre__dir = 2;
		      hypre__max = hypre__nz;
		    }
		  hypre__num_blocks = 1;
		  if (hypre__max < hypre__num_blocks)
		    {
		      hypre__num_blocks = hypre__max;
		    }
		  if (hypre__num_blocks > 0)
		    {
		      hypre__div = hypre__max / hypre__num_blocks;
		      hypre__mod = hypre__max % hypre__num_blocks;
		    };;
/* # 236 "smg_residual.c" */
		  assert( hypre__sx1 == 1 );
		  assert( hypre__sx2 == 1 );

		  if( hypre__num_blocks == 1 ) {
		    int ii, jj, kk;
		    const double* bp_0 = bp + hypre__i1start;
		    double* rp_0 = rp + hypre__i2start;

		    for( kk = 0; kk < hypre__mz; kk++ ) {
		      for( jj = 0; jj < hypre__my; jj++ ) {
			const double* bpp = bp_0
			  + jj*hypre__sy1 + kk*hypre__sz1;
			double* rpp = rp_0
			  + jj*hypre__sy2 + kk*hypre__sz2;
			for( ii = 0; ii < hypre__mx; ii++ ) {
			  rpp[ii] = bpp[ii];
			}
		      }
		    }
		  } else { /* hypre__num_blocks > 1 */
		    for (hypre__block = 0; hypre__block < hypre__num_blocks;
			 hypre__block++) {
		      loopi = 0;
		      loopj = 0;
		      loopk = 0;
		      hypre__nx = hypre__mx;
		      hypre__ny = hypre__my;
		      hypre__nz = hypre__mz;
		      if (hypre__dir == 0) {
			loopi = hypre__block * hypre__div +
			  (((hypre__mod) <
			    (hypre__block)) ? (hypre__mod)
			   : (hypre__block));
			hypre__nx = hypre__div +
			  ((hypre__mod > hypre__block) ? 1 : 0);
		      } else if (hypre__dir == 1) {
			loopj = hypre__block * hypre__div +
			  (((hypre__mod) <
			    (hypre__block)) ? (hypre__mod)
			   : (hypre__block));
			hypre__ny = hypre__div +
			  ((hypre__mod > hypre__block) ? 1 : 0);
		      } else if (hypre__dir == 2) {
			loopk = hypre__block * hypre__div +
			  (((hypre__mod) <
			    (hypre__block)) ? (hypre__mod)
			   : (hypre__block));
			hypre__nz = hypre__div +
			  ((hypre__mod > hypre__block) ? 1 : 0);
		      }

		      bi = hypre__i1start + loopi +
			loopj * hypre__sy1 + loopk * hypre__sz1;
		      ri = hypre__i2start + loopi +
			loopj * hypre__sy2 + loopk * hypre__sz2;

		      { /* AAA */
			int ii, jj, kk;
			const double* bp_0 = bp + bi;
			double* rp_0 = rp + ri;
			for( kk = 0; kk < hypre__nz; kk++ ) {
			  for( jj = 0; jj < hypre__ny; jj++ ) {
			    const double* bpp = bp_0
			      + jj*hypre__sy1 + kk*hypre__sz1;
			    double* rpp = rp_0
			      + jj*hypre__sy2 + kk*hypre__sz2;
			    for( ii = 0; ii < hypre__nx; ii++ ) {
			      rpp[ii] = bpp[ii];
			    }
			  }
			}
		      } /* AAA */
		    }
		  } /* hypre__num_blocks > 1 */
		};
	      }
	  }
	  break;

	case 1:
	  {
	    hypre_FinalizeIndtComputations (comm_handle);
	    compute_box_aa = (compute_pkg->dept_boxes);
	  }
	  break;
	} /* switch */





      for (i = 0; i < ((compute_box_aa)->size); i++)
	{
	  int dxp_s[MAX_STENCIL];

	  compute_box_a = ((compute_box_aa)->box_arrays[(i)]);

	  A_data_box = &((((A)->data_space))->boxes[(i)]);
	  x_data_box = &((((x)->data_space))->boxes[(i)]);
	  r_data_box = &((((r)->data_space))->boxes[(i)]);

	  rp = (((r)->data) + ((r)->data_indices)[i]);
	  for( si = 0; si < stencil_size; si++ ) {
	    dxp_s[si] = ((stencil_shape[si][0]) +
			 ((stencil_shape[si][1]) +
			  ((stencil_shape[si][2]) *
			   (((0) <
			     ((((((x_data_box)->imax)[1])) -
			       ((((x_data_box)->imin)[1])) +
			       1))) ? ((((((x_data_box)->imax)[1])) -
					((((x_data_box)->imin)[1])) +
					1)) : (0)))) * (((0) <
							 ((((((x_data_box)->
							      imax)[0])) -
							   ((((x_data_box)->
							      imin)[0])) +
							   1)))
							? ((((((x_data_box)->
							       imax)[0])) -
							    ((((x_data_box)->
							       imin)[0])) +
							    1)) : (0)));
	  }

	  for (j = 0; j < ((compute_box_a)->size); j++)
	    {
	      int hypre__i1start, hypre__i2start, hypre__i3start;
	      int hypre__sx1, hypre__sy1, hypre__sz1;
	      int hypre__sx2, hypre__sy2, hypre__sz2;
	      int hypre__sx3, hypre__sy3, hypre__sz3;
	      int hypre__nx, hypre__ny, hypre__nz;
	      int hypre__mx, hypre__my, hypre__mz;
	      int hypre__dir, hypre__max;
	      int hypre__div, hypre__mod;
	      int hypre__block, hypre__num_blocks;

	      compute_box = &((compute_box_a)->boxes[(j)]);
	      start = ((compute_box)->imin);
	      hypre__i1start =
		(((start[0]) - ((((A_data_box)->imin)[0]))) +
		 (((start[1]) - ((((A_data_box)->imin)[1]))) +
		  (((start[2]) -
		    ((((A_data_box)->imin)[2]))) * (((0) <
						     ((((((A_data_box)->
							  imax)[1])) -
						       ((((A_data_box)->
							  imin)[1])) +
						       1)))
						    ? ((((((A_data_box)->imax)[1])) - ((((A_data_box)->imin)[1])) + 1)) : (0)))) * (((0) < ((((((A_data_box)->imax)[0])) - ((((A_data_box)->imin)[0])) + 1))) ? ((((((A_data_box)->imax)[0])) - ((((A_data_box)->imin)[0])) + 1)) : (0)));
	      hypre__i2start =
		(((start[0]) - ((((x_data_box)->imin)[0]))) +
		 (((start[1]) - ((((x_data_box)->imin)[1]))) +
		  (((start[2]) -
		    ((((x_data_box)->imin)[2]))) * (((0) <
						     ((((((x_data_box)->
							  imax)[1])) -
						       ((((x_data_box)->
							  imin)[1])) +
						       1)))
						    ? ((((((x_data_box)->imax)[1])) - ((((x_data_box)->imin)[1])) + 1)) : (0)))) * (((0) < ((((((x_data_box)->imax)[0])) - ((((x_data_box)->imin)[0])) + 1))) ? ((((((x_data_box)->imax)[0])) - ((((x_data_box)->imin)[0])) + 1)) : (0)));
	      hypre__i3start =
		(((start[0]) - ((((r_data_box)->imin)[0]))) +
		 (((start[1]) - ((((r_data_box)->imin)[1]))) +
		  (((start[2]) -
		    ((((r_data_box)->imin)[2]))) * (((0) <
						     ((((((r_data_box)->
							  imax)[1])) -
						       ((((r_data_box)->
							  imin)[1])) +
						       1)))
						    ? ((((((r_data_box)->imax)[1])) - ((((r_data_box)->imin)[1])) + 1)) : (0)))) * (((0) < ((((((r_data_box)->imax)[0])) - ((((r_data_box)->imin)[0])) + 1))) ? ((((((r_data_box)->imax)[0])) - ((((r_data_box)->imin)[0])) + 1)) : (0)));
	      
	      hypre_BoxGetStrideSize (compute_box, base_stride,
				      loop_size);

	      hypre__sx1 = ((base_stride[0]));
	      hypre__sy1 =
		((base_stride[1]) *
		 (((0) <
		   ((((((A_data_box)->imax)[0])) -
		     ((((A_data_box)->imin)[0])) +
		     1))) ? ((((((A_data_box)->imax)[0])) -
			      ((((A_data_box)->imin)[0])) + 1)) : (0)));
	      hypre__sz1 =
		((base_stride[2]) *
		 (((0) <
		   ((((((A_data_box)->imax)[0])) -
		     ((((A_data_box)->imin)[0])) +
		     1))) ? ((((((A_data_box)->imax)[0])) -
			      ((((A_data_box)->imin)[0])) +
			      1)) : (0)) * (((0) <
					     ((((((A_data_box)->
						  imax)[1])) -
					       ((((A_data_box)->
						  imin)[1])) +
					       1)))
					    ? ((((((A_data_box)->
						   imax)[1])) -
						((((A_data_box)->
						   imin)[1])) +
						1)) : (0)));
	      hypre__sx2 = ((base_stride[0]));
	      hypre__sy2 =
		((base_stride[1]) *
		 (((0) <
		   ((((((x_data_box)->imax)[0])) -
		     ((((x_data_box)->imin)[0])) +
		     1))) ? ((((((x_data_box)->imax)[0])) -
			      ((((x_data_box)->imin)[0])) + 1)) : (0)));
	      hypre__sz2 =
		((base_stride[2]) *
		 (((0) <
		   ((((((x_data_box)->imax)[0])) -
		     ((((x_data_box)->imin)[0])) +
		     1))) ? ((((((x_data_box)->imax)[0])) -
			      ((((x_data_box)->imin)[0])) +
			      1)) : (0)) * (((0) <
					     ((((((x_data_box)->
						  imax)[1])) -
					       ((((x_data_box)->
						  imin)[1])) +
					       1)))
					    ? ((((((x_data_box)->
						   imax)[1])) -
						((((x_data_box)->
						   imin)[1])) +
						1)) : (0)));
	      hypre__sx3 = ((base_stride[0]));
	      hypre__sy3 =
		((base_stride[1]) *
		 (((0) <
		   ((((((r_data_box)->imax)[0])) -
		     ((((r_data_box)->imin)[0])) +
		     1))) ? ((((((r_data_box)->imax)[0])) -
			      ((((r_data_box)->imin)[0])) + 1)) : (0)));
	      hypre__sz3 =
		((base_stride[2]) *
		 (((0) <
		   ((((((r_data_box)->imax)[0])) -
		     ((((r_data_box)->imin)[0])) +
		     1))) ? ((((((r_data_box)->imax)[0])) -
			      ((((r_data_box)->imin)[0])) +
			      1)) : (0)) * (((0) <
					     ((((((r_data_box)->
						  imax)[1])) -
					       ((((r_data_box)->
						  imin)[1])) +
					       1)))
					    ? ((((((r_data_box)->
						   imax)[1])) -
						((((r_data_box)->
						   imin)[1])) +
						1)) : (0)));

	      /* Based on BG/L Milestone #46 */
	      assert( hypre__sx1 == 1 );
	      assert( hypre__sx2 == 1 );
	      assert( hypre__sx3 == 1 );
		    
	      hypre__mx = hypre__nx = (loop_size[0]);
	      hypre__my = hypre__ny = (loop_size[1]);
	      hypre__mz = hypre__nz = (loop_size[2]);

	      hypre__dir = 0;
	      hypre__max = hypre__nx;
	      if (hypre__ny > hypre__max) {
		hypre__dir = 1;
		hypre__max = hypre__ny;
	      }
	      if (hypre__nz > hypre__max) {
		hypre__dir = 2;
		hypre__max = hypre__nz;
	      }
	      hypre__num_blocks = 1;
	      if (hypre__max < hypre__num_blocks) {
		hypre__num_blocks = hypre__max;
	      }
	      if (hypre__num_blocks > 0) {
		hypre__div = hypre__max / hypre__num_blocks;
		hypre__mod = hypre__max % hypre__num_blocks;
	      }
	      else
		continue;

/* # 287 "smg_residual.c" */

	      if( hypre__num_blocks == 1 ) {
		int si, ii, jj, kk;
		const double* Ap_0 = A->data + hypre__i1start;
		const double* xp_0 =
		  x->data + hypre__i2start + x->data_indices[i]; 
		ri = hypre__i3start;
#if 1 /* transformed loop nest */
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* Ap_1 = Ap_0
			+ jj*hypre__sy1 + kk*hypre__sz1;
		      const double* xp_1 = xp_0
			+ jj*hypre__sy2 + kk*hypre__sz2;
		      double* rpp = rp + ri + jj*hypre__sy3 + kk*hypre__sz3;
		      for( si = 0; si < stencil_size; si++ ) {
			const double* App = Ap_1 + Adip[si];
			const double* xpp = xp_1 + dxp_s[si];
			for (ii = 0; ii < hypre__mx; ii++) {
			  rpp[ii] -= App[ii] * xpp[ii];
			}
		      }
		    }
		  }
		}
#elif 0
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      double* rpp = rp + ri + jj*hypre__sy3 + kk*hypre__sz3;
		      int II;

#define DECL_RP(i)   register double rp##i = rpp[II+i]
#define MAC_RP(i)    rp##i -= App[i] * xpp[i]
#define ST_RP(i)     rpp[II+i] = rp##i

		      for( II = 0; (II+8) <= hypre__mx; II += 8 ) {
			DECL_RP(0); DECL_RP(1); DECL_RP(2); DECL_RP(3);
			DECL_RP(4); DECL_RP(5); DECL_RP(6); DECL_RP(7);
			const double* Ap_1 = Ap_0 + II + jj*hypre__sy1 + kk*hypre__sz1;
			const double* xp_1 = xp_0 + II + jj*hypre__sy2 + kk*hypre__sz2;
			for( si = 0; si < stencil_size; si++ ) {
			  const double* App = Ap_1 + Adip[si];
			  const double* xpp = xp_1 + dxp_s[si];
 			  MAC_RP(0); MAC_RP(1); MAC_RP(2); MAC_RP(3);
			  MAC_RP(4); MAC_RP(5); MAC_RP(6); MAC_RP(7);
			}
			ST_RP(0); ST_RP(1); ST_RP(2); ST_RP(3);
			ST_RP(4); ST_RP(5); ST_RP(6); ST_RP(7);
		      }
		      while( II < hypre__mx ) {
			register double rp0 = rpp[II];
			const double* Ap_1 = Ap_0 + II + jj*hypre__sy1 + kk*hypre__sz1;
			const double* xp_1 = xp_0 + II + jj*hypre__sy2 + kk*hypre__sz2;
			for( si = 0; si < stencil_size; si++ ) {
			  const double* App = Ap_1 + Adip[si];
			  const double* xpp = xp_1 + dxp_s[si];
			  rp0 -= App[0] * xpp[0];
			}
			rpp[II] = rp0;
			II++;
		      }
		    }
		  }
		}
#elif 0
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* Ap_1 = Ap_0 + jj*DJA1 + kk*DKA1;
		      const double* xp_1 = xp_0 + jj*DJX1 + kk*DKX1;
		      double* rpp = rp + ri + jj*DJR1 + kk*DKR1;
		      int II;
		      for( II = 0; (II+4) <= hypre__mx; II += 4 ) {
			register double rp0 = rpp[II+0];
			register double rp1 = rpp[II+1];
			register double rp2 = rpp[II+2];
			register double rp3 = rpp[II+3];
			const double* Ap_1 = Ap_0 + II + jj*DJA1 + kk*DKA1;
			const double* xp_1 = xp_0 + II + jj*DJX1 + kk*DKX1;
			for( si = 0; si < stencil_size; si++ ) {
			  const double* App = Ap_1 + Adip[si];
			  const double* xpp = xp_1 + dxp_s[si];
			  rp0 -= App[0] * xpp[0];
			  rp1 -= App[1] * xpp[1];
			  rp2 -= App[2] * xpp[2];
			  rp3 -= App[3] * xpp[3];
			}
			rpp[II+0] = rp0;
			rpp[II+1] = rp1;
			rpp[II+2] = rp2;
			rpp[II+3] = rp3;
		      }
		      while( II < hypre__mx ) {
			register double rp0 = rpp[II];
			const double* Ap_1 = Ap_0 + II + jj*DJA1 + kk*DKA1;
			const double* xp_1 = xp_0 + II + jj*DJX1 + kk*DKX1;
			for( si = 0; si < stencil_size; si++ ) {
			  const double* App = Ap_1 + Adip[si];
			  const double* xpp = xp_1 + dxp_s[si];
			  rp0 -= App[0] * xpp[0];
			}
			rpp[II] = rp0;
			II++;
		      }
		    }
		  }
		}
#elif 0
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* Ap_1 = Ap_0
			+ jj*hypre__sy1 + kk*hypre__sz1;
		      const double* xp_1 = xp_0
			+ jj*hypre__sy2 + kk*hypre__sz2;
		      double* rpp = rp + ri + jj*hypre__sy3 + kk*hypre__sz3;
		      if( stencil_size == 5 && IS_FINEST_GRID
			  && jj > 0 && jj < hypre__my-1
			  && dxp_s[3]-dxp_s[2]==1 ) {

			const double* AppLL = Ap_1 + Adip[1];
			const double* xppLL = xp_1 + dxp_s[1];

			const double* AppL = Ap_1 + Adip[0];
			const double* AppM = Ap_1 + Adip[2];
			const double* xppM = xp_1 + dxp_s[2];
			const double* AppR = Ap_1 + Adip[3];
			
			const double* AppRR = Ap_1 + Adip[4];
			const double* xppRR = xp_1 + dxp_s[4];

			double xppM_L = xppM[-1];
			double xppM_M = xppM[0];
			double xppM_R = xppM[1];
			
			rpp[0] -= AppLL[0] * xppLL[0];
			rpp[0] -= AppL[0] * xppM_L;
			rpp[0] -= AppM[0] * xppM_M;
			rpp[0] -= AppR[0] * xppM_R;
			rpp[0] -= AppRR[0] * xppRR[0];

			for( ii = 1; ii < hypre__mx; ii++ )
			  rpp[ii] += xppLL[ii];
			for( ii = 1; ii < hypre__mx; ii++ )
			  rpp[ii] += xppM[ii-1];
			for( ii = 1; ii < hypre__mx; ii++ )
			  rpp[ii] -= 6 * xppM[ii];
			for( ii = 1; ii < hypre__mx; ii++ )
			  rpp[ii] += xppM[ii+1];
			for( ii = 1; ii < hypre__mx; ii++ )
			  rpp[ii] += xppRR[ii];
		      } else { /* default implementation */
			for( si = 0; si < stencil_size; si++ ) {
			  const double* App = Ap_1 + Adip[si];
			  const double* xpp = xp_1 + dxp_s[si];
			  for (ii = 0; ii < hypre__mx; ii++) {
			    rpp[ii] -= App[ii] * xpp[ii];
			  }
			}
		      }
		    }
		  }
		}
#elif 0
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* Ap_1 = Ap_0
			+ jj*hypre__sy1 + kk*hypre__sz1;
		      const double* xp_1 = xp_0
			+ jj*hypre__sy2 + kk*hypre__sz2;
		      double* rpp = rp + ri + jj*hypre__sy3 + kk*hypre__sz3;
		      if( stencil_size == 5 && IS_FINEST_GRID
			  && jj > 0 && jj < hypre__my-1
			  && dxp_s[3]-dxp_s[2]==1 ) {

			const double* AppLL = Ap_1 + Adip[1];
			const double* xppLL = xp_1 + dxp_s[1];

			const double* AppL = Ap_1 + Adip[0];
			const double* AppM = Ap_1 + Adip[2];
			const double* xppM = xp_1 + dxp_s[2];
			const double* AppR = Ap_1 + Adip[3];
			
			const double* AppRR = Ap_1 + Adip[4];
			const double* xppRR = xp_1 + dxp_s[4];

			double xppM_L = xppM[-1];
			double xppM_M = xppM[0];
			double xppM_R = xppM[1];
			
			rpp[0] -= AppLL[0] * xppLL[0];
			rpp[0] -= AppL[0] * xppM_L;
			rpp[0] -= AppM[0] * xppM_M;
			rpp[0] -= AppR[0] * xppM_R;
			rpp[0] -= AppRR[0] * xppRR[0];

			for( ii = 1; (ii+3) <= hypre__mx; ii += 3 ) {
			  double r0 = 0;
			  double r1 = 0;
			  double r2 = 0;

			  xppM_L = xppM[ii+1];
			  r0 += xppLL[ii+0];
			  r0 += xppM_M;
			  r0 -= 6 * xppM_R;
			  r0 += xppM_L;
			  r0 += xppRR[ii+0];
			  rpp[ii+0] += r0;

			  xppM_M = xppM[ii+2];
			  r1 += xppLL[ii+1];
			  r1 += xppM_R;
			  r1 -= 6 * xppM_L;
			  r1 += xppM_M;
			  r1 += xppRR[ii+1];
			  rpp[ii+1] += r1;

			  xppM_R = xppM[ii+3];
			  r2 += xppLL[ii+2];
			  r2 += xppM_L;
			  r2 -= 6 * xppM_M;
			  r2 += xppM_R;
			  r2 += xppRR[ii+2];
			  rpp[ii+2] += r2;
			}
			for( ; ii < hypre__mx; ii++ ) {
			  rpp[ii] += xppLL[ii];
			  rpp[ii] += xppM[ii-1];
			  rpp[ii] -= 6 * xppM[ii+0];
			  rpp[ii] += xppM[ii+1];
			  rpp[ii] += xppRR[ii];
			}
		      } else { /* default implementation */
			for( si = 0; si < stencil_size; si++ ) {
			  const double* App = Ap_1 + Adip[si];
			  const double* xpp = xp_1 + dxp_s[si];
			  for (ii = 0; ii < hypre__mx; ii++) {
			    rpp[ii] -= App[ii] * xpp[ii];
			  }
			}
		      }
		    }
		  }
		}
#elif 0
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* Ap_1 = Ap_0
			+ jj*hypre__sy1 + kk*hypre__sz1;
		      const double* xp_1 = xp_0
			+ jj*hypre__sy2 + kk*hypre__sz2;
		      double* rpp = rp + ri + jj*hypre__sy3 + kk*hypre__sz3;
		      si = 0;
		      while( (si+2) <= stencil_size ) {
			const double* App0 = Ap_1 + Adip[si+0];
			const double* App1 = Ap_1 + Adip[si+1];
			const double* xpp0 = xp_1 + dxp_s[si+0];
			const double* xpp1 = xp_1 + dxp_s[si+1];
			if( xpp1 - xpp0 == 1 ) {
			  double x0 = xpp0[0];
			  double x1 = xpp1[0];
			  //			  fprintf( stderr, "X 1\n" );
			  for( ii = 0; ii < hypre__mx-1; ii++ ) {
			    rpp[ii] -= App0[ii] * x0;
			    x0 = x1;
			    rpp[ii] -= App1[ii] * x1;
			    x1 = xpp1[ii+1];
			  }
			  rpp[ii] -= App0[ii] * x0;
			  rpp[ii] -= App1[ii] * x1;
			  si += 2;
			} else if( xpp0 - xpp1 == 1 ) {
			  double x0 = xpp0[0];
			  double x1 = xpp1[0];
			  //			  fprintf( stderr, "X 1\n" );
			  for( ii = 0; ii < hypre__mx-1; ii++ ) {
			    rpp[ii] -= App1[ii] * x1;
			    x1 = x0;
			    rpp[ii] -= App0[ii] * x0;
			    x0 = xpp0[ii+1];
			  }
			  rpp[ii] -= App0[ii] * x0;
			  rpp[ii] -= App1[ii] * x1;
			  si += 2;
			} else {
			  //			  fprintf( stderr, "X 2\n" );
			  for( ii = 0; ii < hypre__mx; ii++ ) {
			    rpp[ii] -= App0[ii] * xpp0[ii];
			  }
			  si++;
			}
		      }
		      while( si < stencil_size ) {
			const double* App = Ap_1 + Adip[si];
			const double* xpp = xp_1 + dxp_s[si];
			for( ii = 0; ii < hypre__mx; ii++ ) {
			  rpp[ii] -= App[ii]*xpp[ii];
			}
			si++;
		      }
		    }
		  }
		}
#elif 0
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* Ap_1 = Ap_0
			+ jj*hypre__sy1 + kk*hypre__sz1;
		      const double* xp_1 = xp_0
			+ jj*hypre__sy2 + kk*hypre__sz2;
		      double* rpp = rp + ri + jj*hypre__sy3 + kk*hypre__sz3;
		      for( si = 0; si < stencil_size; si++ ) {
			const double* App = Ap_1 + Adip[si];
			const double* xpp = xp_1 + dxp_s[si];
			if( IS_FINEST_GRID ) {
			  rpp[0] -= App[0] * xpp[0];
			  if( App[1] == 6 ) {
			    for (ii = 1; ii < hypre__mx; ii++) {
			      rpp[ii] -= 6*xpp[ii];
			    }
			  } else if( App[1] == -1 ) {
			    for (ii = 1; ii < hypre__mx; ii++) {
			      rpp[ii] += xpp[ii];
			    }
			  } else if( App[1] != 0 ) {
			    for (ii = 1; ii < hypre__mx; ii++) {
			      rpp[ii] -= App[ii]*xpp[ii];
			    }
			  }
			}
			else {
			  for (ii = 0; ii < hypre__mx; ii++) {
			    rpp[ii] -= App[ii] * xpp[ii];
			  }
			}
		      }
		    }
		  }
		}
#elif 0
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* Ap_1 = Ap_0
			+ jj*hypre__sy1 + kk*hypre__sz1;
		      const double* xp_1 = xp_0
			+ jj*hypre__sy2 + kk*hypre__sz2;
		      double* rpp = rp + ri + jj*hypre__sy3 + kk*hypre__sz3;
		      for( si = 0; si < stencil_size; si++ ) {
			const double* App = Ap_1 + Adip[si];
			const double* xpp = xp_1 + dxp_s[si];
			for (ii = 0; ii < hypre__mx; ii++) {
			  rpp[ii] -= App[ii] * xpp[ii];
			  if( IS_FINEST_GRID && fp_log != NULL ) {
			    fprintf( fp_log, "%d %d %g %d %d %d %d %d %d 1\n", (int)(rpp-r->data), (int)(xpp-x->data), App[ii], ii, si, jj, kk, j, i );
			  }
			}
		      }
		    }
		  }
		}
#elif 0
		{
		  const int* Adip = A->data_indices[i];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* Ap_1 = Ap_0 + jj*DJA1 + kk*DKA1;
		      const double* xp_1 = xp_0 + jj*DJX1 + kk*DKX1;
		      double* rpp = rp + ri + jj*DJR1 + kk*DKR1;
		      for( si = 0; si < stencil_size; si++ ) {
			const double* App = Ap_1 + Adip[si];
			const double* xpp = xp_1 + dxp_s[si];
			double* rpp2 = rpp;
#define MAC(i)    rpp2[i] -= App[i] * xpp[i]
#define UF  65
			for( ii = 0; (ii+UF) <= hypre__mx; ii += UF ) {
			  MAC(0); MAC(1); MAC(2); MAC(3); MAC(4);
			  MAC(5); MAC(6); MAC(7); MAC(8); MAC(9);
			  MAC(10); MAC(11); MAC(12); MAC(13); MAC(14);
			  MAC(15); MAC(16); MAC(17); MAC(18); MAC(19);
			  MAC(20); MAC(21); MAC(22); MAC(23); MAC(24);
			  MAC(25); MAC(26); MAC(27); MAC(28); MAC(29);
			  MAC(30); MAC(31); MAC(32); MAC(33); MAC(34);
			  MAC(35); MAC(36); MAC(37); MAC(38); MAC(39);
			  MAC(40); MAC(41); MAC(42); MAC(43); MAC(44);
			  MAC(45); MAC(46); MAC(47); MAC(48); MAC(49);
			  MAC(50); MAC(51); MAC(52); MAC(53); MAC(54);
			  MAC(55); MAC(56); MAC(57); MAC(58); MAC(59);
			  MAC(60); MAC(61); MAC(62); MAC(63); MAC(64);
			  rpp2 += UF;
			  App += UF;
			  xpp += UF;
			}
			while( ii < hypre__mx ) {
			  MAC(0);
			  rpp2++;
			  App++;
			  xpp++;
			  ii++;
			}
		      }
		    }
		  }
		}
#else /* effective original loop nest */
		for( si = 0; si < stencil_size; si++ ) {
		  Ap = (((A)->data) + ((A)->data_indices)[i][si]);
		  xp = (((x)->data) + ((x)->data_indices)[i]) + dxp_s[si];
		  for (kk = 0; kk < hypre__mz; kk++) {
		    for (jj = 0; jj < hypre__my; jj++) {
		      const double* App = Ap + Ai + jj*DJA1 + kk*DKA1;
		      const double* xpp = xp + xi + jj*DJX1 + kk*DKX1;
		      double* rpp = rp + ri + jj*DJR1 + kk*DKR1;
		      for (ii = 0; ii < hypre__mx; ii++) {
			rpp[ii] -= App[ii] * xpp[ii];
		      }
		    }
		  }
		}
#endif

	      } else { /* hypre__num_blocks > 1 */

		for( si = 0; si < stencil_size; si++ ) {
		  Ap = (((A)->data) + ((A)->data_indices)[i][si]);
		  xp = (((x)->data) + ((x)->data_indices)[i]) + dxp_s[si];

		  for (hypre__block = 0; hypre__block < hypre__num_blocks;
		       hypre__block++) {
		    loopi = 0;
		    loopj = 0;
		    loopk = 0;
		    hypre__nx = hypre__mx;
		    hypre__ny = hypre__my;
		    hypre__nz = hypre__mz;
		    if (hypre__dir == 0) {
		      loopi = hypre__block * hypre__div +
			(((hypre__mod) <
			  (hypre__block)) ? (hypre__mod)
			 : (hypre__block));
		      hypre__nx = hypre__div +
			((hypre__mod > hypre__block) ? 1 : 0);
		    } else if (hypre__dir == 1) {
		      loopj = hypre__block * hypre__div +
			(((hypre__mod) <
			  (hypre__block)) ? (hypre__mod)
			 : (hypre__block));
		      hypre__ny = hypre__div +
			((hypre__mod > hypre__block) ? 1 : 0);
		    } else if (hypre__dir == 2) {
		      loopk = hypre__block * hypre__div +
			(((hypre__mod) <
			  (hypre__block)) ? (hypre__mod)
			 : (hypre__block));
		      hypre__nz = hypre__div +
			((hypre__mod > hypre__block) ? 1 : 0);
		    }
		    Ai = hypre__i1start + loopi * hypre__sx1 +
		      loopj * hypre__sy1 + loopk * hypre__sz1;
		    xi = hypre__i2start + loopi * hypre__sx2 +
		      loopj * hypre__sy2 + loopk * hypre__sz2;
		    ri = hypre__i3start + loopi * hypre__sx3 +
		      loopj * hypre__sy3 + loopk * hypre__sz3;
		    
		    /* CORE LOOP BEGIN */
		    assert( hypre__sx1 == 1 );
		    assert( hypre__sx2 == 1 );
		    assert( hypre__sx3 == 1 );
		    {
		      int ii, jj, kk;
		      int DJA0 = hypre__sy1 - hypre__nx * hypre__sx1;
		      int DJX0 = hypre__sy2 - hypre__nx * hypre__sx2;
		      int DJR0 = hypre__sy3 - hypre__nx * hypre__sx3;
		      int DKA0 = hypre__sz1 - hypre__ny * hypre__sy1;
		      int DKX0 = hypre__sz2 - hypre__ny * hypre__sy2;
		      int DKR0 = hypre__sz3 - hypre__ny * hypre__sy3;
		      int DJA1 = DJA0 + hypre__nx;
		      int DKA1 = DKA0 + hypre__ny*DJA1;
		      int DJX1 = DJX0 + hypre__nx;
		      int DKX1 = DKX0 + hypre__ny*DJX1;
		      int DJR1 = DJR0 + hypre__nx;
		      int DKR1 = DKR0 + hypre__ny*DJR1;
		      
		      /* In essence, this loop computes:
		       *
			     FOR_ALL i, j, k DO
			     rp[ri + i + j*DJ_R + k*DK_R]
			     -= Ap[Ai + i + j*DJ_A + k*DK_A]
			     * xp[xi + i + j*DJ_X + k*DK_X];
		       */
		      for (kk = 0; kk < hypre__nz; kk++) {
			for (jj = 0; jj < hypre__ny; jj++) {
			  const double* App = Ap + Ai + jj*DJA1 + kk*DKA1;
			  const double* xpp = xp + xi + jj*DJX1 + kk*DKX1;
			  double* rpp = rp + ri + jj*DJR1 + kk*DKR1;
			  for (ii = 0; ii < hypre__nx; ii++) {
			    rpp[ii] -= App[ii] * xpp[ii];
#if DO_LOG
			    if( IS_FINEST_GRID && fp_log != NULL ) {
			      fprintf( fp_log, "%d %d %g %d %d %d %d %d %d 0\n", (int)(rpp-r->data), (int)(xpp-x->data), App[ii], ii, si, jj, kk, j, i );
			    }
#endif
			  }
			}
		      }
		    }
		    /* CORE LOOP END */
		  } /* hypre__block */
		} /* si */
	      } /* else hypre__num_blocks > 1 */

	    } /* j */
	} /* i */
    } /* compute_i */





  ;
  ;

#if DO_LOG
  if( IS_FINEST_GRID && fp_log != NULL ) {
    fclose( fp_log );
    fprintf( stderr, "exiting early...\n" );
    exit( 1 );
  }
#endif

  hypre_IncFLOPCount( residual_data->flops );
  hypre_EndTiming( residual_data->time_index );
  return ierr;
}

/*--------------------------------------------------------------------------
 * hypre_SMGResidualSetBase
 *--------------------------------------------------------------------------*/
 
int
hypre_SMGResidualSetBase( void        *residual_vdata,
                          hypre_Index  base_index,
                          hypre_Index  base_stride )
{
   hypre_SMGResidualData *residual_data = (hypre_SMGResidualData *)residual_vdata;
   int                    d;
   int                    ierr = 0;
 
   for (d = 0; d < 3; d++)
   {
      hypre_IndexD((residual_data -> base_index),  d)
         = hypre_IndexD(base_index,  d);
      hypre_IndexD((residual_data -> base_stride), d)
         = hypre_IndexD(base_stride, d);
   }
 
   return ierr;
}

/*--------------------------------------------------------------------------
 * hypre_SMGResidualDestroy
 *--------------------------------------------------------------------------*/

int
hypre_SMGResidualDestroy( void *residual_vdata )
{
   int ierr = 0;

   hypre_SMGResidualData *residual_data = (hypre_SMGResidualData *)residual_vdata;

   if (residual_data)
   {
      hypre_StructMatrixDestroy(residual_data -> A);
      hypre_StructVectorDestroy(residual_data -> x);
      hypre_StructVectorDestroy(residual_data -> b);
      hypre_StructVectorDestroy(residual_data -> r);
      hypre_BoxArrayDestroy(residual_data -> base_points);
      hypre_ComputePkgDestroy(residual_data -> compute_pkg );
      hypre_FinalizeTiming(residual_data -> time_index);
      hypre_TFree(residual_data);
   }

   return ierr;
}

