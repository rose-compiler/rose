
enum AVSampleFormat 
   {
     AV_SAMPLE_FMT_NONE = -1,
     AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
     AV_SAMPLE_FMT_FLTP,        ///< float, planar
     AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
   };

typedef struct AVCodec 
   {
     const char *name;
     const enum AVSampleFormat *sample_fmts; ///< array of supported sample formats, or NULL if unknown, array is terminated by -1
   } AVCodec;

#if 1
// DQ (9/2/2013): This simpler case demonstrates the failure (p->source_corresp.name != __null test failing).
const enum AVSampleFormat *sample_fmts_alt = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_NONE };
#endif

#if 0
AVCodec ff_eac3_encoder = 
   {
     .name            = "eac3",
#if 1
  // DQ (9/2/2013): I think this may be a special problem still...
     .sample_fmts     = (const enum AVSampleFormat[]) { AV_SAMPLE_FMT_FLTP, AV_SAMPLE_FMT_NONE },
#endif
   };
#endif
