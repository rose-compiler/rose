#ifndef _WRITERECTMDARRAY_H_
#define _WRITERECTMDARRAY_H_
#include "RectMDArray.H"
//#include "ParticleSet.H"
#include <cstring>
using namespace std;
//class ParticleSet;
const char* MDWrite(RectMDArray<float>* a_array);

const char* MDWrite(RectMDArray<double>& a_array);

void MDWrite(const char* filename, RectMDArray<float>* a_array);

void MDWrite(const char* filename, RectMDArray<double>& a_array);

void MDWrite(const char* filename, RectMDArray<double>& a_array, 
             const double* a_corner, const double& a_h);

//void PWrite(const char* filename, const ParticleSet* a_particles);

//const char* PWrite(const ParticleSet* a_particles);

#endif 
