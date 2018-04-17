double abs(double x__78)
{
  if (x__78 > (0)) {
    return x__78;
  }
  else {
    return -x__78;
  }
}

// Original code (correct code from Rich's older version of ROSE)
double vec_norm1(double *v__79,int x__80)
{
  double sum__81 = (0);
  for (int i__82 = 0; i__82 < x__80; ++i__82) {
    double rose_temp__3__83;
    double x__shadow__84 = (v__79[i__82]);
    if (x__shadow__84 > (0)) {
      rose_temp__3__83 = x__shadow__84;
      goto rose_inline_end__2__86;
    }
    else {
      rose_temp__3__83 = (-x__shadow__84);
      goto rose_inline_end__2__86;
    }
    rose_inline_end__2__86:
    sum__81 += rose_temp__3__83;
  }
  return sum__81;
}

#if 0
// Bad code
double vec_norm1(double *v__75,int x__76)
{
  double sum__77 = (0);
  for (int i__78 = 0; i__78 < x__76; ++i__78) {
    double rose_temp__3__79;
    double x__81;
    if (x__81 > (0)) {
      rose_temp__3__79 = x__81;
      goto rose_inline_end__2__82;
    }
    else {
      rose_temp__3__79 = (-x__81);
      goto rose_inline_end__2__82;
    }
    rose_inline_end__2__82:
    sum__77 += rose_temp__3__79;
  }
  return sum__77;
}
#endif

#if 0
// Bad code (without flattenBlocks() and cleanupInlinedCode()
double vec_norm1(double *v__75,int x__76)
{
  double sum__77 = (0);
  for (int i__78 = 0; i__78 < x__76; ++i__78) {
    double rose_temp__3__79;
{
      double x__shadow__80;
{
        double x__81;
{
          if (x__81 > (0)) {{
              rose_temp__3__79 = x__81;
              goto rose_inline_end__2__82;
            }
          }
          else {{
              rose_temp__3__79 = (-x__81);
              goto rose_inline_end__2__82;
            }
          }
        }
        rose_inline_end__2__82:
        0;
      }
    }
    sum__77 += rose_temp__3__79;
  }
  return sum__77;
}
#endif

double mat_norm1(double *v__87,int m__88,int n__89)
{
  double norm__90 = ((-1.) / (0));
  for (int i__91 = 0; i__91 < n__89; ++i__91) {
    double *v__shadow__93 = (v__87 + (i__91 * m__88));
    double *v__95 = v__shadow__93;
    double sum__97 = (0);
    for (int i__98 = 0; i__98 < m__88; ++i__98) {
      double rose_temp__3__99;
      double x__shadow__100 = (v__95[i__98]);
      if (x__shadow__100 > (0)) {
        rose_temp__3__99 = x__shadow__100;
        goto rose_inline_end__2__102;
      }
      else {
        rose_temp__3__99 = (-x__shadow__100);
        goto rose_inline_end__2__102;
      }
      rose_inline_end__2__102:
      sum__97 += rose_temp__3__99;
    }
    double rose_temp__6__92 = sum__97;
    double *v__shadow__106 = (v__87 + (((n__89 - i__91) + 1) * m__88));
    double *v__108 = v__shadow__106;
    double sum__110 = (0);
    for (int i__111 = 0; i__111 < m__88; ++i__111) {
      double rose_temp__3__112;
      double x__shadow__113 = (v__108[i__111]);
      if (x__shadow__113 > (0)) {
        rose_temp__3__112 = x__shadow__113;
        goto rose_inline_end__2__115;
      }
      else {
        rose_temp__3__112 = (-x__shadow__113);
        goto rose_inline_end__2__115;
      }
      rose_inline_end__2__115:
      sum__110 += rose_temp__3__112;
    }
    norm__90 = ((rose_temp__6__92 > norm__90)?sum__97:norm__90);
  }
  return norm__90;
}


int main(int ,char **)
{
  double a__118[10][10];
  for (int i__119 = 0; i__119 < 10; ++i__119) {
    for (int j__120 = 0; j__120 < 10; ++j__120) {
      (a__118[i__119])[j__120] = ((i__119 == j__120)?1.:0.);
    }
  }
  double *v__shadow__122 = ((a__118[0]) + 0);
  double norm__128 = ((-1.) / (0));
  for (int i__129 = 0; i__129 < 10; ++i__129) {
    double *v__shadow__131 = (v__shadow__122 + (i__129 * 10));
    double *v__133 = v__shadow__131;
    double sum__135 = (0);
    for (int i__136 = 0; i__136 < 10; ++i__136) {
      double rose_temp__3__137;
      double x__shadow__138 = (v__133[i__136]);
      if (x__shadow__138 > (0)) {
        rose_temp__3__137 = x__shadow__138;
        goto rose_inline_end__2__140;
      }
      else {
        rose_temp__3__137 = (-x__shadow__138);
        goto rose_inline_end__2__140;
      }
      rose_inline_end__2__140:
      sum__135 += rose_temp__3__137;
    }
    double rose_temp__6__130 = sum__135;
    double *v__shadow__144 = (v__shadow__122 + (((10 - i__129) + 1) * 10));
    double *v__146 = v__shadow__144;
    double sum__148 = (0);
    for (int i__149 = 0; i__149 < 10; ++i__149) {
      double rose_temp__3__150;
      double x__shadow__151 = (v__146[i__149]);
      if (x__shadow__151 > (0)) {
        rose_temp__3__150 = x__shadow__151;
        goto rose_inline_end__2__153;
      }
      else {
        rose_temp__3__150 = (-x__shadow__151);
        goto rose_inline_end__2__153;
      }
      rose_inline_end__2__153:
      sum__148 += rose_temp__3__150;
    }
    norm__128 = ((rose_temp__6__130 > norm__128)?sum__135:norm__128);
  }
  return (!(norm__128 == 1.));
}
