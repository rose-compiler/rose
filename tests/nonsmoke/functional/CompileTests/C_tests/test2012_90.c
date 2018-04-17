
int ngx_show_version = 1;
int ngx_show_help = 1;
int ngx_show_configure = 1;
int ngx_test_config = 1;
int ngx_quiet_mode = 1;
char* ngx_prefix;
char* ngx_conf_params;
char* ngx_conf_file;
char* ngx_signal;



int
ngx_get_options(int argc, char *const *argv)
{
    unsigned char *p;
    int i;

  for (i = 1; i < argc; i++) {
#if 0
        p = (char *) argv[i];

        if (*p++ != '-') {
         // ngx_log_stderr(0, "invalid option: \"%s\"", argv[i]);
            return -1;
        }
#endif
//      while (*p) {

            switch (*p++) {

            case '?':
            case 'h':
                ngx_show_version = 1;
                ngx_show_help = 1;
                break;
#if 1
            case 'v':
                ngx_show_version = 1;
                break;

            case 'V':
                ngx_show_version = 1;
                ngx_show_configure = 1;
                break;
#endif
#if 0
            case 't':
                ngx_test_config = 1;
                break;

            case 'q':
                ngx_quiet_mode = 1;
                break;

            case 'p':
                if (*p) {
                    ngx_prefix = p;
                    goto next;
                }

                if (argv[++i]) {
                    ngx_prefix = (char *) argv[i];
                    goto next;
                }

             // ngx_log_stderr(0, "option \"-p\" requires directory name");
                return -1;
#endif
#if 0
            case 'c':
                if (*p) {
                    ngx_conf_file = p;
                    goto next;
                }

                if (argv[++i]) {
                    ngx_conf_file = (char *) argv[i];
                    goto next;
                }

             // ngx_log_stderr(0, "option \"-c\" requires file name");
                return -1;

            case 'g':
                if (*p) {
                    ngx_conf_params = p;
                    goto next;
                }

                if (argv[++i]) {
                    ngx_conf_params = (char *) argv[i];
                    goto next;
                }

                ngx_log_stderr(0, "option \"-g\" requires parameter");
                return -1;

            case 's':
                if (*p) {
                    ngx_signal = (char *) p;

                } else if (argv[++i]) {
                    ngx_signal = argv[i];

                } else {
                // ngx_log_stderr(0, "option \"-s\" requires parameter");
                    return -1;
                }
#if 0
                if (strcmp((const char *) ngx_signal, (const char *) "stop") == 0
                    || strcmp((const char *) ngx_signal, (const char *) "quit") == 0
                    || strcmp((const char *) ngx_signal, (const char *) "reopen") == 0
                    || strcmp((const char *) ngx_signal, (const char *) "reload") == 0)
                {
                    ngx_process = 2;
                    goto next;
                }
#endif
             // ngx_log_stderr(0, "invalid option: \"-s %s\"", ngx_signal);
                return -1;
#endif
#if 0
            default:
             // ngx_log_stderr(0, "invalid option: \"%c\"", *(p - 1));
                return -1;
#endif
            }
//      }
// #if 1
    next:
      continue;
// #endif
  }

    return 0;
}
