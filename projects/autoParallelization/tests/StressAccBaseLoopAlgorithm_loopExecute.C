
#define LOOPSUITE_NDSET3D(v,v1,v2,v3,v4,v5,v6,v7,v8)  \
   v4 = v ;                                           \
   v1 = v4 + 1 ;                                      \
   v2 = v1 + m_Domain->m_jp ;                         \
   v3 = v4 + m_Domain->m_jp ;                         \
   v5 = v1 + m_Domain->m_kp ;                         \
   v6 = v2 + m_Domain->m_kp ;                         \
   v7 = v3 + m_Domain->m_kp ;                         \
   v8 = v4 + m_Domain->m_kp ;

#define VECADD(xx,yy,zz)  xx[i] += ax ; yy[i] += ay ; zz[i] += az ;
void StressAccBaseLoopAlgorithm_loopExecute ()
{
  int i; 
  double *xa1, *xa2, *xa3, *xa4, *xa5, *xa6, *xa7, *xa8 ;
  double *ya1, *ya2, *ya3, *ya4, *ya5, *ya6, *ya7, *ya8 ;
  double *za1, *za2, *za3, *za4, *za5, *za6, *za7, *za8 ;

  double ax, ay, az, nx, ny, nz ;

  LOOPSUITE_NDSET3D(m_Domain->m_x,x1,x2,x3,x4,x5,x6,x7,x8) ;
  for (i = m_frz; i<= m_lpz; i++)
  {
    dxi = 0.25*(x1[i] + x2[i] - x3[i] - x4[i]) ;
    VECADD(xa4,ya4,za4) ;
  }
}
