/**
 * /author Tristan Vanderbruggen (vanderbruggentristan@gmail.com)
 * /date   08/2012
 */

__kernel void task_1(unsigned int size, __global float * a, __global float * b, __global float * r) {
  unsigned int i = get_global_id(0);
  unsigned int j = get_global_id(1);
  unsigned int k;

  r[i * size + j] = 0;
  for (k = 0; k < size; k++)
    r[i * size + j] += a[i * size + k] * b[k * size + j];
}

#define BLOCK_SIZE 16

__kernel void matmul(__global float* A, __global float* B, __global float* C) {

  __local float scratchA[BLOCK_SIZE][BLOCK_SIZE];
  __local float scratchB[BLOCK_SIZE][BLOCK_SIZE];

  int globalX = get_global_id(0);
  int globalY = get_global_id(1);
  int size = get_global_size(0);
  int k;
  float sum = 0.0f;
  int numBlocks = size / BLOCK_SIZE;
  int b;

  int tidX = get_local_id(0);
  int tidY = get_local_id(1);

  for(b = 0; b < numBlocks; ++b)
  {
    // Populate a cache for A/B
    int x;
    int y;

    x = b * BLOCK_SIZE + tidX;
    y = globalY;

    scratchA[tidY][tidX] = A[y * size + x];

    x = globalX;
    y = b * BLOCK_SIZE + tidY;

    scratchB[tidY][tidX] = B[y * size + x];

    barrier(CLK_LOCAL_MEM_FENCE);

    for(k = 0; k < BLOCK_SIZE; ++k)
    {
      float myA;
      float myB;

      myA = scratchA[tidY][k];
      myB = scratchB[k][tidX];

      sum += myA * myB;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
  }

  C[globalY * size + globalX] = sum;
}

