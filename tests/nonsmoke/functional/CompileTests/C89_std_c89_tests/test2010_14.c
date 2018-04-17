/*
From spec cpu's gcc benchmark

Liao 11/11/2010
*/
struct predictor_info
{
  const char *const name;
  const int hitrate;
};


#define DEF_PREDICTOR(ENUM, NAME, HITRATE, FLAGS) {NAME, FLAGS},

static const struct predictor_info predictor_info[]= {
#include "predict.def"

  /* Upper bound on predictors.  */
  {0, 0}
};
#undef DEF_PREDICTOR
