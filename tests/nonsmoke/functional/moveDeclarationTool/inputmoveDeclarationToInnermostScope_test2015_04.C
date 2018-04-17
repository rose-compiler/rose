void foo(int len, float* compression, float* vnewc , float* delvc, float* compHalfStep)
{
  /* declared once, used once */
  float vchalf ;  
  for ( int zidx =0, xxx = 42 ; zidx < len; ++zidx) {
    compression[zidx] = 0.1 / vnewc[zidx] - 1.0;
    vchalf = vnewc[zidx] - delvc[zidx] * 0.5;
    compHalfStep[zidx] = 1.0 / vchalf - 1.0;
  }
}
