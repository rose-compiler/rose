
static int codec_id_to_pcm_format(int codec_id)
   {
     switch(codec_id) 
        {
       // The bug is that all of the statements associated with each case will be put into the first case.
          case 0: return 2;
          case 1: return 3;
          default: return 4;
        }
   }
