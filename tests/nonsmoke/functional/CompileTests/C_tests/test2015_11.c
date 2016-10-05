
typedef void (*xen_event_channel_notification_t)();

xen_event_channel_notification_t xen_consumers[8];

// struct TEST_STRUCT{};
// struct {} TEST_VAR;

void get_xen_consumer()
   {
  // This works fine, but the case in a conditional fails.
  // unsigned int x = sizeof(struct { typeof(&xen_consumers[0]); });

  // This fails to unparse the defining declaration (in the conditional).
     if ( sizeof(struct { typeof(&xen_consumers[0]); }) )
        {
        }
   }
