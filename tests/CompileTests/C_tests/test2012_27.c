
#define WEXITSTATUS(n) n

void
cherokee_server_handle_panic()
   {
     int               re;

     if (re < 0) {
#ifdef WEXITSTATUS
		int val = WEXITSTATUS(re);
#else
		int val = re;
#endif

		42;
		
//		LOG_CRITICAL (CHEROKEE_ERROR_SERVER_PANIC, cmd.buf, val);
		42;
	}

//	cherokee_buffer_mrproper (&cmd);
		42;
fin:
	abort();
}
