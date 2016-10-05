

struct interface_operation 
   {
     void *func;
   };

int downloader_xfer_deliver ()
   {
     int rc;
     return rc;
   }

static struct interface_operation downloader_xfer_operations[] = {
   { .func = ( ( ( ( typeof ( downloader_xfer_deliver ) * ) ((void *)0) ) == ( ( typeof ( int ( struct downloader *, struct io_buffer *iobuf, struct xfer_metadata *meta ) ) * ) ((void *)0) ) ) ? downloader_xfer_deliver : downloader_xfer_deliver ), },
};
