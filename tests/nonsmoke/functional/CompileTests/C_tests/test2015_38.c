

struct interface_operation {
 void *type;
 void *func;
};

static int downloader_xfer_deliver ( /* struct downloader *downloader, struct io_buffer *iobuf, struct xfer_metadata *meta */ ) 
   {
     int rc;
     return rc;
   }

#if 0
static void downloader_finished ( /* struct downloader *downloader, int rc */ ) 
   {
  // intf_shutdown ( &downloader->xfer, rc );
  // intf_shutdown ( &downloader->job, rc );
   }
#endif

extern int xfer_deliver ( /* struct interface *intf,struct io_buffer *iobuf,struct xfer_metadata *meta */ );

static struct interface_operation downloader_xfer_operations[] = {
 { .type = xfer_deliver, .func = ( ( ( ( typeof ( downloader_xfer_deliver ) * ) ((void *)0) ) == ( ( typeof ( int ( struct downloader *, struct io_buffer *iobuf, struct xfer_metadata *meta ) ) * ) ((void *)0) ) ) ? downloader_xfer_deliver : downloader_xfer_deliver ), },
// { .type = xfer_deliver, .func = ( ( ( ( typeof ( downloader_finished ) * ) ((void *)0) ) == ( ( typeof ( void ( struct downloader *, int rc ) ) * ) ((void *)0) ) ) ? downloader_finished : downloader_finished ), },
};
