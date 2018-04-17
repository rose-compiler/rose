struct interface 
   {
     int abc;
   };


extern void * intf_get_dest_op_untyped ( struct interface *intf, void *type, struct interface **dest );

void resolv_done ( struct interface *intf, struct sockaddr *sa ) 
   {

     struct interface **dest;

     typeof ( void ( void *, struct sockaddr *sa ) ) *op = ( ( typeof ( void ( void *, struct sockaddr *sa ) ) * ) intf_get_dest_op_untyped ( intf, resolv_done, &dest ) );

   }
