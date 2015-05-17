// From: Xen: static void tapdisk_control_handle_request(event_id_t id, char mode, void *private)
void foobar()
   {
     switch (0) 
        {
          case 42:
            // Note that the return type for the function is "void" yet we are returning "0".
            // This is a source sequence issue where we are overly strict in ROSE.
            // The bug was that we were testing for there being no extra source sequence points
            // (this test is now supressed, though it can be helpful in debugging more complex switch statements).
               return 0;
        }
   }

