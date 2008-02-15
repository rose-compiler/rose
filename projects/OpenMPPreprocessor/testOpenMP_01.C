
#if 0
  // Original A++ code before transformation to explicit for loops
     int n; \
     Range I,J,K; \
     floatArray A(n,n,n); \
     floatArray rhs(n,n,n); \
     floatArray B(n,n,n); \
     A(I,J,K) = rhs(I,J,K) + ( B(I+1,J,K) + B(I-1,J,K) + B(I,J-1,K) +
                B(I,J+1,K) + B(I,J,K-1) + B(I,J,K+1) - 6.0 * B (I,J,K) );

  // main body
         _A_I_J_K_pointer[SC(_1,_2,_3)] =
              _rhs_I_J_K_pointer[SC(_1,_2,_3)] +
              ((((((_B_I_J_K_pointer[SC((_1 + 1),_2,_3)]  + _B_I_J_K_pointer[SC((_1 - 1),_2,_3)]) + 
                    _B_I_J_K_pointer[SC(_1,(_2 - 1),_3)]) + _B_I_J_K_pointer[SC(_1,(_2 + 1),_3)]) +
                    _B_I_J_K_pointer[SC(_1,_2,(_3 - 1))]) + _B_I_J_K_pointer[SC(_1,_2,(_3 + 1))]) - 6.0 *
                    _B_I_J_K_pointer[SC(_1,_2,_3)]);
#endif

int
main()
   {
     int _1,_2,_3;
     int _length1,_length2,_length3;
     int _size1,_size2;
     float* AIJKpointer;

#define SC(x1,x2,x3) /* case UniformSizeUnitStride */ (x1)+(x2)*_size1+(x3)*_size2

     /*
  // OpenMP directive omp parallel for
#pragma omp parallel for
     for (_3 = 0; _3 < _length3; _3++)
        { 
          for (_2 = 0; _2 < _length2; _2++)
             { 
               for (_1 = 0; _1 < _length1; _1++)
                 {
                   AIJKpointer[SC(_1,_2,_3)] = 0;
                 }
             }
        }
     */
     return 0;
   }

