#include <cstdio>
#include <iostream>
#include "WriteRectMDArray.H"
#include "Box.H"
#include "RectMDArray.H"
#include "VisitWriter.H"
//#include "ParticleSet.H"
using namespace std;


const char* MDWrite(RectMDArray<float,1>* a_array)
{
  static int fileCount = 0;
  static char nameBuffer[10];
  if(a_array == NULL)
    {
      return nameBuffer;
    }
  sprintf(nameBuffer, "md%d",fileCount);
  MDWrite(nameBuffer, a_array);
  fileCount++;
  return nameBuffer;
};
/*

const char* PWrite(const ParticleSet* a_array)
{
  static int fileCount = 0;
  static char nameBuffer[10];
  if(a_array == NULL)
    {
      return nameBuffer;
    }
  sprintf(nameBuffer, "PART.%d",fileCount);
  PWrite(nameBuffer, a_array);
  fileCount++;
  return nameBuffer;
}

void PWrite(const char* a_filename, const ParticleSet* a_p)
{
  if(a_filename == NULL || a_p == NULL)
    {
      return;
    }
  unsigned int size = a_p->m_particles.size();
  std::vector<float> x(3*size);
  for(unsigned int i=0; i<size; i++)
    {
      const Particle& p = a_p->m_particles[i];
      x[i*3] = p.m_x[0];
      x[i*3+1] = p.m_x[1];
#if DIM==3
      x[i*3+2] = p.m_x[2];
#else
      x[i*3+2] = 0.0;
#endif
    }

  write_point_mesh(a_filename, 0, size,
		   &(x[0]), 0, 0,
		   0, 0);
                   } */
void MDWrite(const char* a_filename, RectMDArray<float,1>* a_array)
{
  if(a_filename == NULL || a_array == NULL)
    {
      return;
    }
  int dim[3] = {1,1,1};
  int vardims[1] ={1};
  int centering[1]={0};
  float* vars[1];
 
  const char * const varnames[] = { "cellCentered" };
  Point lo, hi;
  const Box& box = a_array->getBox();
  lo = box.getLowCorner();
  hi = box.getHighCorner();
  for(int i=0; i<DIM;i++)
    {
      dim[i] = hi[i]-lo[i] + 2;
      // cout << dim[i] << " ";
    }
  // cout << endl;
  // float& val = (*a_array)[0];
  float& val = a_array->index(0);
  vars[0] = &val;
  write_regular_mesh(a_filename, 1, dim, 1, vardims, centering,  varnames, vars);
}
void MDWrite(const char* a_filename, RectMDArray<double,1>& a_array)
{
  Box bx = a_array.getBox();
  RectMDArray<float,1> array(bx);
  for (int k = 0; k < bx.sizeOf();k++)
    {
      double foo = a_array[k];
      array[k] = float(foo);
      Point tuple = bx.getPoint(k);
      int kout = bx.getIndex(tuple);
      // cout << kout << " " << tuple[0] << " " << tuple[1] << endl;
    }
  MDWrite(a_filename,&array);
}
const char* MDWrite(RectMDArray<double,1>& a_array, const double* a_corner ,
                    const double&  a_h)
{
  static int fileCountd = 0;
  static char nameBufferd[11];
  sprintf(nameBufferd, "mdd%d",fileCountd);
  MDWrite(nameBufferd, a_array,a_corner,a_h);
  fileCountd++;
  return nameBufferd;
};
void MDWrite(const char* a_filename,RectMDArray<double>& a_array,
                    const double* a_corner, const double& a_h)
{
  if(a_filename == NULL || &a_array == NULL)
    {
      return;
    }
  int dim[3] = {1,1,1};
  int vardims[1] ={1}; // vardims[2] = {1,1}
  int centering[1]={0}; //centering[2] = {0,0}
  float* vars[1];// vars[2]
  float* x[3];
  Box bx = a_array.getBox();
  RectMDArray<float> array(bx);
  for (int k = 0; k < bx.sizeOf();k++)
    {
      array[k] = (float) a_array[k];
    }
  const char * const varnames[] = { "cellCentered" };//{"cx","cy"}
  Point lo, hi;
  const Box& box = a_array.getBox();
  lo = box.getLowCorner();
  hi = box.getHighCorner();
  for(int i=0; i<DIM;i++)
    {
      dim[i] = hi[i]-lo[i]+1+1;
      cout << dim[i] << " " ;
      x[i] = new float[dim[i]+1];
      for (int k = 0;k < dim[i]+1;k++)
        {
          x[i][k] = (float) (a_corner[i] + a_h*k);
        }
    }
  cout << endl;
  if (DIM == 2)
    {
      x[2] = new float[2];
      x[2][0] = 0.;
      x[2][1] = 1.;
    }
  // float& val = array[0];
  float& val = array.index(0);
  vars[0] = &val; // vars[1] = &val + ...
  write_rectilinear_mesh(a_filename, 1,//2
                         dim, x[0], x[1], x[2],
                         1, vardims, centering,
                         varnames,vars);
  
for(int i=0; i<3;i++)
    {
      delete [] x[i];
    }
}
const char* MDWrite(RectMDArray<double>& a_array)
{
  Box bx = a_array.getBox();
  RectMDArray<float> array(bx);
  for (int k = 0; k < bx.sizeOf();k++)
    {
      array[k] = (float) a_array[k];
    }
  return MDWrite(&array);  
};
