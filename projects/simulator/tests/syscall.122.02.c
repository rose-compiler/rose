/*
 *
 *   Copyright (c) International Business Machines  Corp., 2001
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * NAME
 *	uname02.c
 *
 * DESCRIPTION
 *	uname02 - call uname() with an invalid address to produce a failure
 *
 * ALGORITHM
 *	loop if that option was specified
 *	issue the system call
 *	check the errno value
 *	  issue a PASS message if we get EFAULT - errno 14
 *	otherwise, the tests fails
 *	  issue a FAIL message
 *	  break any remaining tests
 *	  call cleanup
 *
 * USAGE:  <for command-line>
 *  uname02 [-c n] [-e] [-i n] [-I x] [-p x] [-t]
 *		where,  -c n : Run n copies concurrently.
 *			-e   : Turn on errno logging.
 *			-i n : Execute test n times.
 *			-I x : Execute test for x seconds.
 *			-P x : Pause for x seconds between iterations.
 *			-t   : Turn on syscall timing.
 *
 * History
 *	07/2001 John George
 *		-Ported
 *
 * Restrictions
 *	none
 */

#if 0
#include "test.h"
#include "usctest.h"
#endif

#include <errno.h>
#include <sys/utsname.h>
#include "assert.h"
#include <stdio.h>

void cleanup(void);
void setup(void);

char *TCID = "uname02";
int TST_TOTAL = 1;

int exp_enos[] = { 14, 0 };	/* 0 terminated list of expected errnos */

int main(int ac, char **av)
{
	int lc;			/* loop counter */
	char *msg;		/* message returned from parse_opts */

#if 0
	/* parse standard options */
	if ((msg = parse_opts(ac, av, (option_t *) NULL, NULL))
            != (char *)NULL) {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
	        /*NOTREACHED*/}
#endif

	setup();		/* global setup */

	/* The following loop checks looping state if -i option given */
#if 0
	for (lc = 0; TEST_LOOPING(lc); lc++) {
#endif

                /*
                 * call the system call with the TEST() macro
                 * send -1 for an illegal address
                 */

                int result = uname((struct utsname *)-1);
                fprintf(stderr,"Result: %d",result);
                assert( result == -1 );
                assert( errno == EFAULT );
#if 0
	}
#endif

	cleanup();

        /*NOTREACHED*/ return 0;

}

/*
 * setup() - performs all the ONE TIME setup for this test.
 */
void setup(void)
{
#if 0
	/* capture signals */
	tst_sig(FORK, DEF_HANDLER, cleanup);

	/* Set up the expected error numbers for -e option */
	TEST_EXP_ENOS(exp_enos);

	/* Pause if that option was specified */
	TEST_PAUSE;
#endif
}

/*
 * cleanup() - performs all the ONE TIME cleanup for this test at completion
 * 	       or premature exit.
 */
void cleanup(void)
{
#if 0
        /*
         * print timing stats if that option was specified.
         * print errno log if that option was specified.
         */
        TEST_CLEANUP;

        /* exit with return code appropriate for results */
        tst_exit();
#endif
}
