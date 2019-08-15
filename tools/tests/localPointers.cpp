// not supported yet
// packaging pointers into arrays
// later local pointers to retrive array elements
// incomplete example for showing the problem for now.

#define LOOPSUITE_SETFACEPTRS(ptrs,v1,v2,v3,v4,v5,v6,v7,v8) { \
  /* Lower k face */                                          \
  ptrs[ 0 ][ 0 ] = v1;                                        \
  ptrs[ 0 ][ 1 ] = v2;                                        \
  ptrs[ 0 ][ 2 ] = v3;                                        \
  ptrs[ 0 ][ 3 ] = v4;                                        \
  /* Lower j face */                                          \
  ptrs[ 1 ][ 0 ] = v3;                                        \
  ptrs[ 1 ][ 1 ] = v7;                                        \
  ptrs[ 1 ][ 2 ] = v8;                                        \
  ptrs[ 1 ][ 3 ] = v4;                                        \
  /* Lower i face */                                          \
  ptrs[ 2 ][ 0 ] = v1;                                        \
  ptrs[ 2 ][ 1 ] = v5;                                        \
  ptrs[ 2 ][ 2 ] = v8;                                        \
  ptrs[ 2 ][ 3 ] = v4;                                        \
}


void loopExecute(double ax, double ay, double az)
{



  double* x_acc_face_ptrs[ 3 ][ 4 ];
  double* y_acc_face_ptrs[ 3 ][ 4 ];
  double* z_acc_face_ptrs[ 3 ][ 4 ];


  for ( int zone=frz; zone < lpz; ++zone ) {

    {                                                                               
      const int* idx   = plane[ face ];                                              

      /* get vectors on face, d1=(dx1,dy1,dz1)^T and d2=(dx2,dy2,dz2)^T */           
      const double dx1 = 0.25*(x[ idx[0] ]+x[ idx[1] ]-x[ idx[2] ]-x[ idx[3] ]);  
      const double dy1 = 0.25*(y[ idx[0] ]+y[ idx[1] ]-y[ idx[2] ]-y[ idx[3] ]);  
      const double dz1 = 0.25*(z[ idx[0] ]+z[ idx[1] ]-z[ idx[2] ]-z[ idx[3] ]);  

      const double dx2 = 0.25*(x[ idx[1] ]+x[ idx[2] ]-x[ idx[3] ]-x[ idx[0] ]);  
      const double dy2 = 0.25*(y[ idx[1] ]+y[ idx[2] ]-y[ idx[3] ]-y[ idx[0] ]);  
      const double dz2 = 0.25*(z[ idx[1] ]+z[ idx[2] ]-z[ idx[3] ]-z[ idx[0] ]);  

      /* get pointers to the nodes of the face */                                    
      double* x1 = x_acc_face_ptrs[ face ][ 0 ];                                     
      double* x2 = x_acc_face_ptrs[ face ][ 1 ];                                     
      double* x3 = x_acc_face_ptrs[ face ][ 2 ];                                     
      double* x4 = x_acc_face_ptrs[ face ][ 3 ];                                     

      double* y1 = y_acc_face_ptrs[ face ][ 0 ];                                     
      double* y2 = y_acc_face_ptrs[ face ][ 1 ];                                     
      double* y3 = y_acc_face_ptrs[ face ][ 2 ];                                     
      double* y4 = y_acc_face_ptrs[ face ][ 3 ];                                     

      double* z1 = z_acc_face_ptrs[ face ][ 0 ];                                     
      double* z2 = z_acc_face_ptrs[ face ][ 1 ];                                     
      double* z3 = z_acc_face_ptrs[ face ][ 2 ];                                     
      double* z4 = z_acc_face_ptrs[ face ][ 3 ];                                     

// also not really 4 statements in a row, but interleaved with others!!

      x1[ zone ] += ax; y1[ zone ] += ay; z1[ zone ] += az;                          
      x2[ zone ] += ax; y2[ zone ] += ay; z2[ zone ] += az;                          
      x3[ zone ] += ax; y3[ zone ] += ay; z3[ zone ] += az;                          
      x4[ zone ] += ax; y4[ zone ] += ay; z4[ zone ] += az;                          
    }

  }

}
