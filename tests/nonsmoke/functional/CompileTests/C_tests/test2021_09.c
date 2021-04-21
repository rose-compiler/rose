// This code fails the switch support when compiled using -std=c89 
// mode (and works fine with the default mode or -std=c99 mode).

// The problem is that the C89 mode causes lables to be introduced 
// into the EDG representation that is not there for C99 mode.  The 
// solution might be to build a C mode independent representation 
// of the EDG block (the block_list_perspective vector used in 
// EDG_ROSE_Translation::setup_duffs_device_switch_data() function.

void foobar()
   {
     switch(42) 
        {
          case 1:
               switch(42) 
                  {
                    case 11:
                      42;
                    break;
                  }
          break;

          case 2:
               switch(42) 
                  {
                    case 22:
                      42;
                    break;
                    default:
                      42;
                  }
          break;
          case 3:
          default:
               42;
          break;
        }
   }


#if 0
void foobar()
   {
     switch(42) 
        {
#if 1
          case 1:
               switch(42) 
                  {
                    case 1:
                      42;
                    break;
                  }
          break;
#endif
#if 1
          case 2:
               switch(42) 
                  {
#if 1
                    case 2:
                      42;
                    break;
#endif
                    default:
                      42;
                  }
          break;
#endif
          case 3:
          default:
               42;
          break;
        }
   }
#endif
