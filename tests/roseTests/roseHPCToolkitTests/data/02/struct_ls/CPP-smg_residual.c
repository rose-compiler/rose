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

int
hypre_SMGResidual (void *residual_vdata,
                   hypre_StructMatrix * A,
                   hypre_StructVector * x,
                   hypre_StructVector * b, hypre_StructVector * r)
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

  hypre_BeginTiming (residual_data->time_index);

  /*-----------------------------------------------------------------------
   * Compute residual r = b - Ax
   *-----------------------------------------------------------------------*/

  stencil = ((A)->stencil);
  stencil_shape = ((stencil)->shape);
  stencil_size = ((stencil)->size);

  for (compute_i = 0; compute_i < 2; compute_i++)
    {
      switch (compute_i)
        {
        case 0:
          {
            xp = ((x)->data);
            hypre_InitializeIndtComputations (compute_pkg, xp, &comm_handle);
            compute_box_aa = (compute_pkg->indt_boxes);

            /*----------------------------------------
             * Copy b into r
             *----------------------------------------*/

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
                  for (hypre__block = 0; hypre__block < hypre__num_blocks;
                       hypre__block++)
                    {
                      loopi = 0;
                      loopj = 0;
                      loopk = 0;
                      hypre__nx = hypre__mx;
                      hypre__ny = hypre__my;
                      hypre__nz = hypre__mz;
                      if (hypre__num_blocks > 1)
                        {
                          if (hypre__dir == 0)
                            {
                              loopi =
                                hypre__block * hypre__div +
                                (((hypre__mod) <
                                  (hypre__block)) ? (hypre__mod)
                                 : (hypre__block));
                              hypre__nx =
                                hypre__div +
                                ((hypre__mod > hypre__block) ? 1 : 0);
                            }
                          else if (hypre__dir == 1)
                            {
                              loopj =
                                hypre__block * hypre__div +
                                (((hypre__mod) <
                                  (hypre__block)) ? (hypre__mod)
                                 : (hypre__block));
                              hypre__ny =
                                hypre__div +
                                ((hypre__mod > hypre__block) ? 1 : 0);
                            }
                          else if (hypre__dir == 2)
                            {
                              loopk =
                                hypre__block * hypre__div +
                                (((hypre__mod) <
                                  (hypre__block)) ? (hypre__mod)
                                 : (hypre__block));
                              hypre__nz =
                                hypre__div +
                                ((hypre__mod > hypre__block) ? 1 : 0);
                            }
                        };
                      bi =
                        hypre__i1start + loopi * hypre__sx1 +
                        loopj * hypre__sy1 + loopk * hypre__sz1;
                      ri =
                        hypre__i2start + loopi * hypre__sx2 +
                        loopj * hypre__sy2 + loopk * hypre__sz2;
                      for (loopk = 0; loopk < hypre__nz; loopk++)
                        {
                          for (loopj = 0; loopj < hypre__ny; loopj++)
                            {
                              for (loopi = 0; loopi < hypre__nx; loopi++)
                                {
                                  {
                                    rp[ri] = bp[bi];
                                  }
                                  bi += hypre__sx1;
                                  ri += hypre__sx2;
                                }
                              bi += hypre__sy1 - hypre__nx * hypre__sx1;
                              ri += hypre__sy2 - hypre__nx * hypre__sx2;
                            }
                          bi += hypre__sz1 - hypre__ny * hypre__sy1;
                          ri += hypre__sz2 - hypre__ny * hypre__sy2;
                        }
                    }
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
        }

      /*--------------------------------------------------------------------
       * Compute r -= A*x
       *--------------------------------------------------------------------*/

      for (i = 0; i < ((compute_box_aa)->size); i++)
        {
          compute_box_a = ((compute_box_aa)->box_arrays[(i)]);

          A_data_box = &((((A)->data_space))->boxes[(i)]);
          x_data_box = &((((x)->data_space))->boxes[(i)]);
          r_data_box = &((((r)->data_space))->boxes[(i)]);

          rp = (((r)->data) + ((r)->data_indices)[i]);

          for (j = 0; j < ((compute_box_a)->size); j++)
            {
              compute_box = &((compute_box_a)->boxes[(j)]);

              start = ((compute_box)->imin);

              for (si = 0; si < stencil_size; si++)
                {
                  Ap = (((A)->data) + ((A)->data_indices)[i][si]);
                  xp = (((x)->data) + ((x)->data_indices)[i]) +
                    ((stencil_shape[si][0]) +
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

                  hypre_BoxGetStrideSize (compute_box, base_stride,
                                          loop_size);
                  {
                    int hypre__i1start =
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
                    int hypre__i2start =
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
                    int hypre__i3start =
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
                    int hypre__sx1 = ((base_stride[0]));
                    int hypre__sy1 =
                      ((base_stride[1]) *
                       (((0) <
                         ((((((A_data_box)->imax)[0])) -
                           ((((A_data_box)->imin)[0])) +
                           1))) ? ((((((A_data_box)->imax)[0])) -
                                    ((((A_data_box)->imin)[0])) + 1)) : (0)));
                    int hypre__sz1 =
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
                    int hypre__sx2 = ((base_stride[0]));
                    int hypre__sy2 =
                      ((base_stride[1]) *
                       (((0) <
                         ((((((x_data_box)->imax)[0])) -
                           ((((x_data_box)->imin)[0])) +
                           1))) ? ((((((x_data_box)->imax)[0])) -
                                    ((((x_data_box)->imin)[0])) + 1)) : (0)));
                    int hypre__sz2 =
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
                    int hypre__sx3 = ((base_stride[0]));
                    int hypre__sy3 =
                      ((base_stride[1]) *
                       (((0) <
                         ((((((r_data_box)->imax)[0])) -
                           ((((r_data_box)->imin)[0])) +
                           1))) ? ((((((r_data_box)->imax)[0])) -
                                    ((((r_data_box)->imin)[0])) + 1)) : (0)));
                    int hypre__sz3 =
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
/* # 287 "smg_residual.c" */
                    for (hypre__block = 0; hypre__block < hypre__num_blocks;
                         hypre__block++)
                      {
                        loopi = 0;
                        loopj = 0;
                        loopk = 0;
                        hypre__nx = hypre__mx;
                        hypre__ny = hypre__my;
                        hypre__nz = hypre__mz;
                        if (hypre__num_blocks > 1)
                          {
                            if (hypre__dir == 0)
                              {
                                loopi =
                                  hypre__block * hypre__div +
                                  (((hypre__mod) <
                                    (hypre__block)) ? (hypre__mod)
                                   : (hypre__block));
                                hypre__nx =
                                  hypre__div +
                                  ((hypre__mod > hypre__block) ? 1 : 0);
                              }
                            else if (hypre__dir == 1)
                              {
                                loopj =
                                  hypre__block * hypre__div +
                                  (((hypre__mod) <
                                    (hypre__block)) ? (hypre__mod)
                                   : (hypre__block));
                                hypre__ny =
                                  hypre__div +
                                  ((hypre__mod > hypre__block) ? 1 : 0);
                              }
                            else if (hypre__dir == 2)
                              {
                                loopk =
                                  hypre__block * hypre__div +
                                  (((hypre__mod) <
                                    (hypre__block)) ? (hypre__mod)
                                   : (hypre__block));
                                hypre__nz =
                                  hypre__div +
                                  ((hypre__mod > hypre__block) ? 1 : 0);
                              }
                          };
                        Ai =
                          hypre__i1start + loopi * hypre__sx1 +
                          loopj * hypre__sy1 + loopk * hypre__sz1;
                        xi =
                          hypre__i2start + loopi * hypre__sx2 +
                          loopj * hypre__sy2 + loopk * hypre__sz2;
                        ri =
                          hypre__i3start + loopi * hypre__sx3 +
                          loopj * hypre__sy3 + loopk * hypre__sz3;
                        for (loopk = 0; loopk < hypre__nz; loopk++)
                          {
                            for (loopj = 0; loopj < hypre__ny; loopj++)
                              {
                                for (loopi = 0; loopi < hypre__nx; loopi++)
                                  {
                                    {
                                      rp[ri] -= Ap[Ai] * xp[xi];
                                    }
                                    Ai += hypre__sx1;
                                    xi += hypre__sx2;
                                    ri += hypre__sx3;
                                  }
                                Ai += hypre__sy1 - hypre__nx * hypre__sx1;
                                xi += hypre__sy2 - hypre__nx * hypre__sx2;
                                ri += hypre__sy3 - hypre__nx * hypre__sx3;
                              }
                            Ai += hypre__sz1 - hypre__ny * hypre__sy1;
                            xi += hypre__sz2 - hypre__ny * hypre__sy2;
                            ri += hypre__sz3 - hypre__ny * hypre__sy3;
                          }
                      }
                  };
                }
            }
        }
    }

   /*-----------------------------------------------------------------------
    * Return
    *-----------------------------------------------------------------------*/

  hypre_IncFLOPCount (residual_data->flops);
  hypre_EndTiming (residual_data->time_index);

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

