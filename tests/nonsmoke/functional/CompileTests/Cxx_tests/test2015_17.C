void get_xen_consumer()
   {
  // This works fine, but the case in a conditional fails.
  // unsigned int x = sizeof(struct { typeof(&xen_consumers[0]); });

  // This files to unparse the defining declaration (in the conditional).
     if ( sizeof(struct { int x; }) )
        {
        }
   }

