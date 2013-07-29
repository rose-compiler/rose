
typedef struct AVClass {
      const char* class_name;
      int version;
      const char* (*item_name)(void* ctx);
      int version_A;
   } AVClass;

static const AVClass alsa_demuxer_class = {
    .class_name     = "ALSA demuxer",
    .version = 42,
    .version_A = 7,
//  .item_name      = av_default_item_name,
//  .option         = options,
//  .version        = LIBAVUTIL_VERSION_INT,
};
