
typedef struct AVRational{
    int num;
    int den;
} AVRational;

// void av_rescale_q(int a, AVRational bq, AVRational cq);
void av_rescale_q(AVRational cq);

#define ALT_AV_TIME_BASE_Q (AVRational){55, 66}

void foo()
   {
  // This is declared as: int64_t av_rescale_q(int64_t a, AVRational bq, AVRational cq) av_const;
  // Unparsed as: d = (av_rescale_q(ref -> pts,tb,(1), (1000000)));
  // AVRational tb;
  // av_rescale_q(42, tb, ALT_AV_TIME_BASE_Q);

  // This function must be unparsed as: av_rescale_q((AVRational){(55), (66)});
     av_rescale_q(ALT_AV_TIME_BASE_Q);
   }
