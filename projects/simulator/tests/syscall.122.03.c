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
 *	uname03.c
 *
 * DESCRIPTION
 *	uname03 - call uname() and make sure it succeeds
 *
 * ALGORITHM
 *	loop if that option was specified
 *	issue the system call
 *	check the errno value
 *	  issue a PASS message if we get zero
 *	otherwise, the tests fails
 *	  issue a FAIL message
 *	  break any remaining tests
 *	  call cleanup
 *
 * USAGE:  <for command-line>
 *  uname03 [-c n] [-f] [-i n] [-I x] [-p x] [-t]
 *	where,  -c n : Run n copies concurrently.
 *		-f   : Turn off functionality Testing.
 *		-i n : Execute test n times.
 *		-I x : Execute test for x seconds.
 *		-P x : Pause for x seconds between iterations.
 *		-t   : Turn on syscall timing.
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
#include <string.h>
#include "assert.h"

void cleanup(void);
void setup(void);

char *TCID = "uname03";
int TST_TOTAL = 1;

#define LINUX	"Linux"

int main(int ac, char **av)
{
	int lc;			/* loop counter */
	char *msg;		/* message returned from parse_opts */
	struct utsname *buf;

#if 0
	/* parse standard options */
	if ((msg = parse_opts(ac, av, (option_t *) NULL, NULL)) != (char *)NULL) {
		tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
	 /*NOTREACHED*/}
#endif

	setup();		/* global setup */

	/* allocate some space for buf */

	if ((buf = (struct utsname *)malloc((size_t)
					    sizeof(struct utsname))) == NULL) {
    abort();
		//tst_brkm(TBROK, cleanup, "malloc failed for buf");
	 /*NOTREACHED*/}

	/* The following loop checks looping state if -i option given */

#if 0
	for (lc = 0; TEST_LOOPING(lc); lc++) {
#endif

		/* Now make the system call with the TEST() macro */

    int result = uname(buf);
    assert( result == 0 );
#if 0
	}
#endif

	free(buf);
	buf = NULL;

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
