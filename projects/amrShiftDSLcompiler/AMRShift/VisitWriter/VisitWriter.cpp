
/*****************************************************************************
*
* Copyright (c) 2000 - 2008, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-400142
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/
 
/*****************************************************************************
*
* Copyright (c) 2000 - 2008, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-400142
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/
 
/* ************************************************************************* //
//                             visit_writer.c                                //
// ************************************************************************* */
 
#include <cstdio>
#include <cstdlib>
#include <cstring>
 
#include <VisitWriter.H>
 
 
/*
 * Globals.
 */
 
static FILE *fp = NULL;
static int useBinary = 0;
static int numInColumn = 0;
 
 
/* ****************************************************************************
 *  Function: end_line
 *
 *  Purpose:
 *      If floats or ints have been written using the write_float or write_int
 *      functions, this will issue a newline (if necessary) so that a new
 *      heading can be placed.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static void end_line(void)
{
    if (!useBinary)
    {
        char str2[8] = "\n";
        fprintf(fp, "%s",str2);
        numInColumn = 0;
    }
}
 
 
/* ****************************************************************************
 *  Function: open_file
 *
 *  Purpose:
 *      Opens a file for writing and assigns the handle to the global variable
 *      "fp".
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static void open_file(const char *filename)
{
    char full_filename[1024];
    if (strstr(filename, ".vtk") != NULL)
    {
        strcpy(full_filename, filename);
    }
    else
    {
        sprintf(full_filename, "%s.vtk", filename);
    }
 
    fp = fopen(full_filename, "w+");
}
 
 
/* ****************************************************************************
 *  Function: close_file
 *
 *  Purpose:
 *      Closes the file with handle "fp" (a global variable).
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static void close_file(void)
{
    end_line();
    fclose(fp);
    fp = NULL;
}
 
 
/* ****************************************************************************
 *  Function: force_big_endian
 *
 *  Purpose:
 *      Determines if the machine is little-endian.  If so, then, for binary
 *      data, it will force the data to be big-endian.
 *
 *  Note:       This assumes that all inputs are 4 bytes long.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static void force_big_endian(unsigned char *bytes)
{
    static int doneTest = 0;
    static int shouldSwap = 0;
    if (!doneTest)
    {
        int tmp1 = 1;
        unsigned char *tmp2 = (unsigned char *) &tmp1;
        if (*tmp2 != 0)
            shouldSwap = 1;
        doneTest = 1;
    }
 
    if (shouldSwap & useBinary)
    {
        unsigned char tmp = bytes[0];
        bytes[0] = bytes[3];
        bytes[3] = tmp;
        tmp = bytes[1];
        bytes[1] = bytes[2];
        bytes[2] = tmp;
    }
}
 
 
/* ****************************************************************************
 *  Function: write_string
 *
 *  Purpose:
 *      Writes a character to the open file.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static void write_string(const char *str)
{
    fprintf(fp, "%s",str);
}
 
 
/* ****************************************************************************
 *  Function: new_section
 *
 *  Purpose:
 *      Adds a new line, provided we didn't already just do so and we are
 *      writing an ASCII file.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static void new_section(void)
{
    if (numInColumn != 0)
        end_line();
    numInColumn = 0;
}
 
 
/* ****************************************************************************
 *  Function: write_int
 *
 *  Purpose:
 *      Writes an integer to the currently open file.  This routine takes
 *      care of ASCII vs binary issues.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static void write_int(int val)
{
    if (useBinary)
    {
        force_big_endian((unsigned char *) &val);
        fwrite(&val, sizeof(int), 1, fp);
    }
    else
    {
        char str[128];
        sprintf(str, "%d ", val);
        fprintf(fp, "%s", str);
        if (((numInColumn++) % 9) == 8)
        {
            char str2[8] = "\n";
            fprintf(fp, "%s",str2);
            numInColumn = 0;
        }
    }
}
 
 
/* ****************************************************************************
 *  Function: write_float
 *
 *  Purpose:
 *      Writes an float to the currently open file.  This routine takes
 *      care of ASCII vs binary issues.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 *  Modifications:
 *
 *    Hank Childs, Fri Apr 22 09:14:44 PDT 2005
 *    Make precision changes suggested by Jeff McAninch
 *
 * ************************************************************************* */
 
static void write_float(float val)
{
    if (useBinary)
    {
        force_big_endian((unsigned char *) &val);
        fwrite(&val, sizeof(float), 1, fp);
    }
    else
    {
        char str[128];
        sprintf(str, "%20.12e ", val);
        fprintf(fp, "%s",str);
        if (((numInColumn++) % 9) == 8)
        {
            end_line();
        }
    }
}
 
 
/* ****************************************************************************
 *  Function: write_header
 *
 *  Purpose:
 *      Writes the standard VTK header to the file.  This should be the first
 *      thing written to the file.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static void write_header(void)
{
    fprintf(fp, "# vtk DataFile Version 2.0\n");
    fprintf(fp, "Written using VisIt writer\n");
    if (useBinary)
        fprintf(fp, "BINARY\n");
    else
        fprintf(fp, "ASCII\n");
}
 
 
/* ****************************************************************************
 *  Function: write_variables
 *
 *  Purpose:
 *      Writes the variables to the file.  This can be a bit tricky.  The
 *      cell data must be written first, followed by the point data.  When
 *      writing the [point|cell] data, one variable must be declared the
 *      primary scalar and another the primary vector (provided scalars
 *      or vectors exist).  The rest of the arrays are added through the
 *      "field data" mechanism.  Field data should support groups of arrays
 *      with different numbers of components (ie a scalar and a vector), but
 *      there is a failure with the VTK reader.  So the scalars are all written
 *      one group of field data and then the vectors as another.  If you don't
 *      write it this way, the vectors do not show up.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
void write_variables(int nvars, int *vardim, int *centering,
                     const char * const * varname, float **vars,
                     int npts, int ncells)
{
    char str[1024];
    int i, j, first_scalar, first_vector;
    int num_scalars, num_vectors;
    int num_field = 0;
 
    new_section();
    sprintf(str, "CELL_DATA %d\n", ncells);
    write_string(str);
 
    first_scalar = 0;
    first_vector = 0;
    num_scalars = 0;
    num_vectors = 0;
    /* The field data is where the non-primary scalars and vectors are
     * stored.  They must all be grouped together at the end of the point
     * data.  So write out the primary scalars and vectors first.
     */
    for (i = 0 ; i < nvars ; i++)
    {
        if (centering[i] == 0)
        {
            int num_to_write = 0;
            int should_write = 0;
 
            if (vardim[i] == 1)
            {
                if (first_scalar == 0)
                {
                    should_write = 1;
                    sprintf(str, "SCALARS %s float\n", varname[i]);
                    write_string(str);
                    write_string("LOOKUP_TABLE default\n");
                    first_scalar = 1;
                }
                else
                    num_scalars++;
            }
            else if (vardim[i] == 3)
            {
                if (first_vector == 0)
                {
                    should_write = 1;
                    sprintf(str, "VECTORS %s float\n", varname[i]);
                    write_string(str);
                    first_vector = 1;
                }
                else
                    num_vectors++;
            }
            else
            {
                printf("Only supported variable dimensions are 1 and 3.\n");
                printf("Ignoring variable %s.\n", varname[i]);
                continue;
            }
 
            if (should_write)
            {
                num_to_write = ncells*vardim[i];
                for (j = 0 ; j < num_to_write ; j++)
                {
                    write_float(vars[i][j]);
                }
                end_line();
            }
        }
    }
 
    first_scalar = 0;
    if (num_scalars > 0)
    {
        sprintf(str, "FIELD FieldData %d\n", num_scalars);
        write_string(str);
        for (i = 0 ; i < nvars ; i++)
        {
            int should_write = 0;
            if (centering[i] == 0)
            {
                if (vardim[i] == 1)
                {
                    if (first_scalar == 0)
                    {
                        first_scalar = 1;
                    }
                    else
                    {
                        should_write = 1;
                        sprintf(str, "%s 1 %d float\n", varname[i], ncells);
                        write_string(str);
                    }
                }
            }
 
            if (should_write)
            {
                int num_to_write = ncells*vardim[i];
                for (j = 0 ; j < num_to_write ; j++)
                {
                    write_float(vars[i][j]);
                }
                end_line();
            }
        }
    }
 
    first_vector = 0;
    if (num_vectors > 0)
    {
        sprintf(str, "FIELD FieldData %d\n", num_vectors);
        write_string(str);
        for (i = 0 ; i < nvars ; i++)
        {
            int should_write = 0;
            if (centering[i] == 0)
            {
                int num_to_write = 0;
 
                if (vardim[i] == 3)
                {
                    if (first_vector == 0)
                    {
                        first_vector = 1;
                    }
                    else
                    {
                        should_write = 1;
                        sprintf(str, "%s 3 %d float\n", varname[i], ncells);
                        write_string(str);
                    }
                }
            }
 
            if (should_write)
            {
                int num_to_write = ncells*vardim[i];
                for (j = 0 ; j < num_to_write ; j++)
                {
                    write_float(vars[i][j]);
                }
                end_line();
            }
        }
    }
 
    new_section();
    sprintf(str, "POINT_DATA %d\n", npts);
    write_string(str);
 
    first_scalar = 0;
    first_vector = 0;
    num_scalars = 0;
    num_vectors = 0;
    /* The field data is where the non-primary scalars and vectors are
     * stored.  They must all be grouped together at the end of the point
     * data.  So write out the primary scalars and vectors first.
     */
    for (i = 0 ; i < nvars ; i++)
    {
        if (centering[i] != 0)
        {
            int num_to_write = 0;
            int should_write = 0;
 
            if (vardim[i] == 1)
            {
                if (first_scalar == 0)
                {
                    should_write = 1;
                    sprintf(str, "SCALARS %s float\n", varname[i]);
                    write_string(str);
                    write_string("LOOKUP_TABLE default\n");
                    first_scalar = 1;
                }
                else
                    num_scalars++;
            }
            else if (vardim[i] == 3)
            {
                if (first_vector == 0)
                {
                    should_write = 1;
                    sprintf(str, "VECTORS %s float\n", varname[i]);
                    write_string(str);
                    first_vector = 1;
                }
                else
                    num_vectors++;
            }
            else
            {
                printf("Only supported variable dimensions are 1 and 3.\n");
                printf("Ignoring variable %s.\n", varname[i]);
                continue;
            }
 
            if (should_write)
            {
                num_to_write = npts*vardim[i];
                for (j = 0 ; j < num_to_write ; j++)
                {
                    write_float(vars[i][j]);
                }
                end_line();
            }
        }
    }
 
    first_scalar = 0;
    if (num_scalars > 0)
    {
        sprintf(str, "FIELD FieldData %d\n", num_scalars);
        write_string(str);
        for (i = 0 ; i < nvars ; i++)
        {
            int should_write = 0;
            if (centering[i] != 0)
            {
                if (vardim[i] == 1)
                {
                    if (first_scalar == 0)
                    {
                        first_scalar = 1;
                    }
                    else
                    {
                        should_write = 1;
                        sprintf(str, "%s 1 %d float\n", varname[i], npts);
                        write_string(str);
                    }
                }
            }
 
            if (should_write)
            {
                int num_to_write = npts*vardim[i];
                for (j = 0 ; j < num_to_write ; j++)
                {
                    write_float(vars[i][j]);
                }
                end_line();
            }
        }
    }
 
    first_vector = 0;
    if (num_vectors > 0)
    {
        sprintf(str, "FIELD FieldData %d\n", num_vectors);
        write_string(str);
        for (i = 0 ; i < nvars ; i++)
        {
            int should_write = 0;
            if (centering[i] != 0)
            {
                int num_to_write = 0;
 
                if (vardim[i] == 3)
                {
                    if (first_vector == 0)
                    {
                        first_vector = 1;
                    }
                    else
                    {
                        should_write = 1;
                        sprintf(str, "%s 3 %d float\n", varname[i], npts);
                        write_string(str);
                    }
                }
            }
 
            if (should_write)
            {
                int num_to_write = npts*vardim[i];
                for (j = 0 ; j < num_to_write ; j++)
                {
                    write_float(vars[i][j]);
                }
                end_line();
            }
        }
    }
}
 
 
/* ****************************************************************************
//  Function: write_point_mesh
//
//  Purpose:
//      Writes out a point mesh.
//
//  Arguments:
//      filename   The name of the file to write.  If the extension ".vtk" is
//                 not present, it will be added.
//      useBinary  '0' to write ASCII, !0 to write binary
//      npts       The number of points in the mesh.
//      pts        The spatial locations of the points.  This array should
//                 be size 3*npts.  The points should be encoded as:
//                 <x1, y1, z1, x2, y2, z2, ..., xn, yn, zn>
//      nvars      The number of variables.
//      vardim     The dimension of each variable.  The size of vardim should
//                 be nvars.  If var i is a scalar, then vardim[i] = 1.
//                 If var i is a vector, then vardim[i] = 3.
//      vars       An array of variables.  The size of vars should be nvars.
//                 The size of vars[i] should be npts*vardim[i].
//
//  Programmer: Hank Childs
//  Creation:   September 2, 2004
//
// ***************************************************************************/
 
void write_point_mesh(const char *filename, int ub, int npts, float *pts,
                      int nvars, int *vardim, const char * const *varnames,
                      float **vars)
{
    int   i;
    char  str[128];
    int  *centering = NULL;
 
    useBinary = ub;
    open_file(filename);
    write_header();
 
    write_string("DATASET UNSTRUCTURED_GRID\n");
    sprintf(str, "POINTS %d float\n", npts);
    write_string(str);
    for (i = 0 ; i < 3*npts ; i++)
    {
        write_float(pts[i]);
    }
 
    new_section();
    sprintf(str, "CELLS %d %d\n", npts, 2*npts);
    write_string(str);
    for (i = 0 ; i < npts ; i++)
    {
        write_int(1);
        write_int(i);
        end_line();
    }
 
    new_section();
    sprintf(str, "CELL_TYPES %d\n", npts);
    write_string(str);
    for (i = 0 ; i < npts ; i++)
    {
        write_int(VISIT_VERTEX);
        end_line();
    }
 
    centering = (int *) malloc(nvars*sizeof(int));
    for (i = 0 ; i < nvars ; i++)
        centering[i] = 1;
    write_variables(nvars, vardim, centering, varnames, vars, npts, npts);
    free(centering);
 
    close_file();
}
 
 
/* ****************************************************************************
 *  Function: num_points_for_cell
 *
 *  Purpose:
 *      Determines the number of points for the type of cell.
 *
 *  Programmer: Hank Childs
 *  Creation:   September 3, 2004
 *
 * ************************************************************************* */
 
static int num_points_for_cell(int celltype)
{
    int npts = 0;
    switch (celltype)
    {
       case VISIT_VERTEX:
         npts = 1;
         break;
       case VISIT_LINE:
         npts = 2;
         break;
       case VISIT_TRIANGLE:
         npts = 3;
         break;
       case VISIT_QUAD:
         npts = 4;
         break;
       case VISIT_TETRA:
         npts = 4;
         break;
       case VISIT_HEXAHEDRON:
         npts = 8;
         break;
       case VISIT_WEDGE:
         npts = 6;
         break;
       case VISIT_PYRAMID:
         npts = 5;
         break;
    }
    return npts;
}
 
 
/* ****************************************************************************
//  Function: write_unstructured_mesh
//
//  Purpose:
//      Writes out a unstructured mesh.
//
//
//  Arguments:
//      filename   The name of the file to write.  If the extension ".vtk" is
//                 not present, it will be added.
//      useBinary  '0' to write ASCII, !0 to write binary
//      npts       The number of points in the mesh.
//      pts        The spatial locations of the points.  This array should
//                 be size 3*npts.  The points should be encoded as:
//                 <x1, y1, z1, x2, y2, z2, ..., xn, yn, zn>
//      ncells     The number of cells.
//      celltypes  The type of each cell.
//      conn       The connectivity array.
//      nvars      The number of variables.
//      vardim     The dimension of each variable.  The size of vardim should
//                 be nvars.  If var i is a scalar, then vardim[i] = 1.
//                 If var i is a vector, then vardim[i] = 3.
//      centering  The centering of each variable.  The size of centering
//                 should be nvars.  If centering[i] == 0, then the variable
//                 is cell-based.  If centering[i] != 0, then the variable
//                 is point-based.
//      vars       An array of variables.  The size of vars should be nvars.
//                 The size of vars[i] should be npts*vardim[i].
//
//  Programmer: Hank Childs
//  Creation:   September 2, 2004
//
// ***************************************************************************/
 
void write_unstructured_mesh(const char *filename, int ub, int npts,
                             float *pts, int ncells, int *celltypes, int *conn,
                             int nvars, int *vardim, int *centering,
                             const char * const *varnames, float **vars)
{
    int   i, j;
    char  str[128];
    int   conn_size = 0;
    int  *curr_conn = conn;
 
    useBinary = ub;
    open_file(filename);
    write_header();
 
    write_string("DATASET UNSTRUCTURED_GRID\n");
    sprintf(str, "POINTS %d float\n", npts);
    write_string(str);
    for (i = 0 ; i < 3*npts ; i++)
    {
        write_float(pts[i]);
    }
 
    new_section();
    for (i = 0 ; i < ncells ; i++)
    {
        int npts = num_points_for_cell(celltypes[i]);
 
        conn_size += npts+1;
    }
    sprintf(str, "CELLS %d %d\n", ncells, conn_size);
    write_string(str);
    for (i = 0 ; i < ncells ; i++)
    {
        int npts = num_points_for_cell(celltypes[i]);
        write_int(npts);
        for (j = 0 ; j < npts ; j++)
            write_int(*curr_conn++);
        end_line();
    }
 
    new_section();
    sprintf(str, "CELL_TYPES %d\n", ncells);
    write_string(str);
    for (i = 0 ; i < ncells ; i++)
    {
        write_int(celltypes[i]);
        end_line();
    }
 
    write_variables(nvars, vardim, centering, varnames, vars, npts, ncells);
 
    close_file();
}
 
 
/* ****************************************************************************
//  Function: write_rectilinear_mesh
//
//  Purpose:
//      Writes out a rectilinear mesh.
//
//
//  Arguments:
//      filename   The name of the file to write.  If the extension ".vtk" is
//                 not present, it will be added.
//      useBinary  '0' to write ASCII, !0 to write binary
//      dims       An array of size 3 = { nX, nY, nZ }, where nX is the
//                 number of points in the X-dimension, etc.
//      x          An array of size dims[0] that contains the x-coordinates.
//      y          An array of size dims[1] that contains the x-coordinates.
//      z          An array of size dims[2] that contains the x-coordinates.
//      nvars      The number of variables.
//      vardim     The dimension of each variable.  The size of vardim should
//                 be nvars.  If var i is a scalar, then vardim[i] = 1.
//                 If var i is a vector, then vardim[i] = 3.
//      centering  The centering of each variable.  The size of centering
//                 should be nvars.  If centering[i] == 0, then the variable
//                 is cell-based.  If centering[i] != 0, then the variable
//                 is point-based.
//      vars       An array of variables.  The size of vars should be nvars.
//                 The size of vars[i] should be npts*vardim[i].
//
//
//  Programmer: Hank Childs
//  Creation:   September 2, 2004
//
//  Modifications:
//
//    Hank Childs, Wed Apr  6 16:22:57 PDT 2005
//    Fix problem with 2D structured meshes and assessing cell count.
//
// ***************************************************************************/
 
void write_rectilinear_mesh(const char *filename, int ub, int *dims,
                            float *x, float *y, float *z,
                            int nvars, int *vardim, int *centering,
                            const char * const *varnames, float **vars)
{
    int   i, j;
    char  str[128];
    int npts = dims[0]*dims[1]*dims[2];
    int ncX = (dims[0] - 1 < 1 ? 1 : dims[0] - 1);
    int ncY = (dims[1] - 1 < 1 ? 1 : dims[1] - 1);
    int ncZ = (dims[2] - 1 < 1 ? 1 : dims[2] - 1);
    int ncells = ncX*ncY*ncZ;
 
    useBinary = ub;
    open_file(filename);
    write_header();
 
    write_string("DATASET RECTILINEAR_GRID\n");
    sprintf(str, "DIMENSIONS %d %d %d\n", dims[0], dims[1], dims[2]);
    write_string(str);
    sprintf(str, "X_COORDINATES %d float\n", dims[0]);
    write_string(str);
    for (i = 0 ; i < dims[0] ; i++)
        write_float(x[i]);
    new_section();
    sprintf(str, "Y_COORDINATES %d float\n", dims[1]);
    write_string(str);
    for (i = 0 ; i < dims[1] ; i++)
        write_float(y[i]);
    new_section();
    sprintf(str, "Z_COORDINATES %d float\n", dims[2]);
    write_string(str);
    for (i = 0 ; i < dims[2] ; i++)
        write_float(z[i]);
 
    write_variables(nvars, vardim, centering, varnames, vars, npts, ncells);
 
    close_file();
}
 
 
/* ****************************************************************************
//  Function: write_regular_mesh
//
//  Purpose:
//      Writes out a regular mesh.  A regular mesh is one where the data lies
//      along regular intervals.  "Brick of bytes/floats",
//      "Block of bytes/floats", and MRI data all are examples of data that
//      lie on regular meshes.
//
//
//  Arguments:
//      filename   The name of the file to write.  If the extension ".vtk" is
//                 not present, it will be added.
//      useBinary  '0' to write ASCII, !0 to write binary
//      dims       An array of size 3 = { nX, nY, nZ }, where nX is the
//                 number of points in the X-dimension, etc.
//      nvars      The number of variables.
//      vardim     The dimension of each variable.  The size of vardim should
//                 be nvars.  If var i is a scalar, then vardim[i] = 1.
//                 If var i is a vector, then vardim[i] = 3.
//      centering  The centering of each variable.  The size of centering
//                 should be nvars.  If centering[i] == 0, then the variable
//                 is cell-based.  If centering[i] != 0, then the variable
//                 is point-based.
//      vars       An array of variables.  The size of vars should be nvars.
//                 The size of vars[i] should be npts*vardim[i].
//
//
//  Programmer: Hank Childs
//  Creation:   September 2, 2004
//
// ***************************************************************************/
 
void write_regular_mesh(const char *filename, int ub, int *dims,
                        int nvars, int *vardim, int *centering,
                        const char * const *varnames, float **vars)
{
    int  i;
 
    float *x = (float *) malloc(sizeof(float)*dims[0]);
    float *y = (float *) malloc(sizeof(float)*dims[1]);
    float *z = (float *) malloc(sizeof(float)*dims[2]);
 
    for (i = 0 ; i < dims[0] ; i++)
        x[i] = (float) i;
    for (i = 0 ; i < dims[1] ; i++)
        y[i] = (float) i;
    for (i = 0 ; i < dims[2] ; i++)
        z[i] = (float) i;
 
    write_rectilinear_mesh(filename, ub, dims, x, y, z, nvars, vardim,
                           centering, varnames, vars);
 
    free(x);
    free(y);
    free(z);
}
 
 
/* ****************************************************************************
//  Function: write_curvilinear_mesh
//
//  Purpose:
//      Writes out a curvilinear mesh.
//
//
//  Arguments:
//      filename   The name of the file to write.  If the extension ".vtk" is
//                 not present, it will be added.
//      useBinary  '0' to write ASCII, !0 to write binary
//      dims       An array of size 3 = { nI, nJ, nK }, where nI is the
//                 number of points in the logical I dimension, etc.
//      pts        An array of size nI*nJ*nK*3.  The array should be layed
//                 out as (pt(i=0,j=0,k=0), pt(i=1,j=0,k=0), ...
//                 pt(i=nI-1,j=0,k=0), pt(i=0,j=1,k=0), ...).
//      nvars      The number of variables.
//      vardim     The dimension of each variable.  The size of vardim should
//                 be nvars.  If var i is a scalar, then vardim[i] = 1.
//                 If var i is a vector, then vardim[i] = 3.
//      centering  The centering of each variable.  The size of centering
//                 should be nvars.  If centering[i] == 0, then the variable
//                 is cell-based.  If centering[i] != 0, then the variable
//                 is point-based.
//      vars       An array of variables.  The size of vars should be nvars.
//                 The size of vars[i] should be npts*vardim[i].
//
//
//  Programmer: Hank Childs
//  Creation:   September 2, 2004
//
//  Modifications:
//
//    Hank Childs, Wed Apr  6 16:22:57 PDT 2005
//    Fix problem with 2D structured meshes and assessing cell count.
//
// ***************************************************************************/
 
void write_curvilinear_mesh(const char *filename, int ub, int *dims,float *pts,
                            int nvars, int *vardim, int *centering,
                            const char * const *varnames, float **vars)
{
    int   i, j;
    char  str[128];
    int npts = dims[0]*dims[1]*dims[2];
    int ncX = (dims[0] - 1 < 1 ? 1 : dims[0] - 1);
    int ncY = (dims[1] - 1 < 1 ? 1 : dims[1] - 1);
    int ncZ = (dims[2] - 1 < 1 ? 1 : dims[2] - 1);
    int ncells = ncX*ncY*ncZ;
 
    useBinary = ub;
    open_file(filename);
    write_header();
 
    write_string("DATASET STRUCTURED_GRID\n");
    sprintf(str, "DIMENSIONS %d %d %d\n", dims[0], dims[1], dims[2]);
    write_string(str);
    sprintf(str, "POINTS %d float\n", npts);
    write_string(str);
    for (i = 0 ; i < 3*npts ; i++)
    {
        write_float(pts[i]);
    }
 
    write_variables(nvars, vardim, centering, varnames, vars, npts, ncells);
 
    close_file();
}
