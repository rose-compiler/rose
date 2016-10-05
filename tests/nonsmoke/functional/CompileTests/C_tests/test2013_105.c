

typedef struct file_header_t {} file_header_t;

// struct hardlinks_t;

typedef struct archive_handle_t 
   {
     char* tar__to_command;
   } archive_handle_t;

void data_extract_to_command(archive_handle_t *archive_handle)
   {
     if (1) 
        {
          int status;

          if (WIFEXITED(status) && WEXITSTATUS(status))
               bb_error_msg_and_die("'%s' returned status %d",archive_handle->tar__to_command, WEXITSTATUS(status));

          if (WIFSIGNALED(status))
               bb_error_msg_and_die("'%s' terminated on signal %d",archive_handle->tar__to_command, WTERMSIG(status));
        }
   }
