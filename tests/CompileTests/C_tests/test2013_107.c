
#ifdef _POSIX_SOURCE
#define	_W_INT(i)	(i)
#else
#define	_W_INT(w)	(*(int *)&(w))	/* convert union wait to int */
#define	WCOREFLAG	0200
#endif

#define	_WSTATUS(x)	(_W_INT(x) & 0177)
#define WTERMSIG(x)	(_WSTATUS(x))


typedef struct file_header_t {} file_header_t;

typedef struct archive_handle_t 
   {
     char* tar__to_command;
   } archive_handle_t;

void data_extract_to_command(archive_handle_t *archive_handle)
   {
     if (1) 
        {
          int status;

          status = 1;

          if (status)
               bb_error_msg_and_die("'%s' terminated on signal %d",archive_handle->tar__to_command, WTERMSIG(status));

            // foobar(WTERMSIG(status));
            // foobar(status);
            // status = WTERMSIG(status);

          status = 1;
        }
   }
