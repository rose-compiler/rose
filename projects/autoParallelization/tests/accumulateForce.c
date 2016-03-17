void AccumulateForce(int *idxBound, int *idxList, int len,
                    double *tmp, double *force)
{
  for (register int ii = 0; ii<len; ++ii) {
    int count = idxBound[ii + 1] - idxBound[ii];
    int *list = &idxList[idxBound[ii]];
    double sum = 0.0;
    for (register int jj = 0; jj<count; ++jj) {
      int idx = list[jj];
      sum += tmp[idx] ;
    }
    force[ii] += sum ;
  }
  return ;
}
