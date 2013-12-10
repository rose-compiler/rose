/**
 * /brief  Compute the product of 2 matrices with one vector, v = A x B x u.
 *         Sequential version
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

#include <iostream>
#include <iomanip>
#include <fstream>

#include <cassert>

#include <cmath>

#define PRINT_ERROR 1
#define CHECK_RESULT 1
#define QUAD_ERROR 1
#define PRINT_OUTPUT 1

#ifndef PRINT_INPUT
# define PRINT_INPUT 0
#endif

#ifndef PRINT_OUTPUT
# define PRINT_OUTPUT 0
#endif

#ifndef PRINT_TARGET
# define PRINT_TARGET 0
#endif

#ifndef PRINT_ERROR
# define PRINT_ERROR 0
#endif

#ifndef __ERROR_LIMIT__
# define __ERROR_LIMIT__ 0.00001
#endif

#ifndef CHECK_RESULT
# define CHECK_RESULT 0
#endif

int main(int argc, char ** argv) {

  assert(argc == 2);

  std::ifstream file;
  file.open(argv[1]);

  assert(file.is_open());

  unsigned long n;
  unsigned long m;
  unsigned long p;
  unsigned long i, j, k;

  file >> n >> p >> m;

  float * A_ = new float[n * p];
  float ** A = new float*[n]; 
  for (i = 0; i < n; i++) {
    A[i] = &(A_[i * p]);
    for (j = 0; j < p; j++)
      file >> A[i][j];
  }

  float * B_ = new float[p * m];
  float ** B = new float*[p];
  for (i = 0; i < p; i++) {
    B[i] = &(B_[i * m]);
    for (j = 0; j < m; j++)
      file >> B[i][j];
  }

  float * res_ = new float[n * m];
  float ** res = new float*[n];
  for (i = 0; i < n; i++) {
    res[i] = &(res_[i * m]);
    for (j = 0; j < m; j++)
      file >> res[i][j];
  }

#if PRINT_INPUT
  std::cout << "A = | ";
  for (j = 0; j < p; j++)
    std::cout << A[j] << " ";
  std::cout << "|" << std::endl;
  for (i = 1; i < n; i++) {
    std::cout << "    | ";
    for (j = 0; j < p; j++)
      std::cout << A[i * p + j] << " ";
    std::cout << "|" << std::endl;
  }
  std::cout << std::endl;

  std::cout << "B = | ";
  for (j = 0; j < m; j++)
    std::cout << B[j] << " ";
  std::cout << "|" << std::endl;
  for (i = 1; i < p; i++) {
    std::cout << "    | ";
    for (j = 0; j < m; j++) 
      std::cout << B[i * m + j] << " ";
    std::cout << "|" << std::endl;
  }
  std::cout << std::endl;
#endif

#ifdef OPENACC
  float ** C;
#else
  float * C_ = new float[n * m];
  float ** C = new float*[n];
  for (i = 0; i < n; i++)
    C[i] = &(C_[i * n]);
#endif

  #pragma acc data copyin(A[0:n][0:p],B[0:p][0:m]) copyout(C[0:n][0:m])
  {
    #pragma acc parallel copyin(A[0:n][0:p],B[0:p][0:m]) copyout(C[0:n][0:m])
    {
      #pragma acc loop
      for (i = 0; i < n; i++) {
        #pragma acc loop
        for (j = 0; j < m; j++) {
          C[i][j] = 0.;
          for (k = 0; k < p; k++)
            C[i][j] += A[i][k] * B[k][j];
        }
      }
    }
  }

  delete [] A_;
  delete [] B_;

#if PRINT_TARGET
  std::cout << "Expected Result:" << std::endl;
  std::cout << "C = " << std::endl;
  for (i = 0; i < n; i++) {
    std::cout << "  | " << std::setw(8) << res[i][0];
    for (j = 1; j < m; j++)
      std::cout << "  " << std::setw(8) << res[i][j];
    std::cout << " |" << std::endl;
  }
#endif

#if PRINT_OUTPUT
  std::cout << "Actual Result:" << std::endl;
  std::cout << "C = " << std::endl;
  for (i = 0; i < n; i++) {
    std::cout << "  | " << std::setw(8) << C[i][0];
    for (j = 1; j < m; j++)
      std::cout << "  " << std::setw(8) << C[i][j];
    std::cout << " |" << std::endl;
  }
#endif

#if CHECK_RESULT
  bool success = true;
  float quad_error = 0.;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      quad_error += (C[i][j] - res[i][j]) * (C[i][j] - res[i][j]);
  quad_error = sqrt(quad_error/(n*m));
  success = quad_error < __ERROR_LIMIT__;
#endif

#if PRINT_ERROR
  std::cout << "Error = " << quad_error << std::endl;
#endif

#if CHECK_RESULT
  if (success)
    std::cout << "Success!" << std::endl;
  else {
    std::cout << "Failure!" << std::endl;
    return -1;
  }
#endif

  return 0;
}

