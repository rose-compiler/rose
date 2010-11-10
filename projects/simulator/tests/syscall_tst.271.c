/*
 *   Copyright (c) International Business Machines  Corp. 2005,2006,2007
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
 *
 ***************************************************************************
 * File: utimes_test.c
 *
 * Description: 
 *	The utimes_test() function builds into the LTP
 *	to verify that the Linux Audit Framework accurately
 *	logs both successful and erroneous execution of the
 *	"utimes" system call.
 *
 * Total Tests: 6 (2 assertions)
 *
 * Test Name: utimes_test
 *
 * Test Assertion & Strategy:
 *
 *  Verify that:
 *   1. Appropriate audit log created on successfull utimes() execution
 *   2. Appropriate audit log created on failing utimes() execution
 *      EPERM return value
 *
 * Set audit rules:
 *   1. entry,always
 *   2. exit,always 
 *   3. entry,never; exit,never
 *
 * Each set of rules will be tested for the following:
 *   1. utimes()- Success Case
 *	a) Creates the temporary file
 *	b) Creates the utimes data structure
 *	c) Executes the "utimes" system call
 *   2. utimes()- Erroneous Case
 *	a) Creates the temporary file
 *	b) Creates the utimes data structure
 *	c) Executes the "utimes" system call as non-root user
 *	d) Generates EPERM errno
 *   NOTE: the id related fields (uid, gid,..etc) are filled in by the switch user
 *      functions. switch_to_super_user() is passed a NULL so the audit record
 *      id related fields do not change.
 *
 *   Delete each rule set before adding the next.
 *   Delete the temporary directory(s)/file(s) created.
 *
 * Usage:  <for command-line>
 * 	utimes_test
 *
 * History:
 * DATE		NAME					DESCRIPTION
 * 05/05/05 	Debora Velarde <dvelarde@us.ibm.com>	Created this test based on utime test.
 * 05/27/05 	Debora Velarde <dvelarde@us.ibm.com>	Don't run if on s390 or s390x
 * 12/19/05	Loulwa Salem<loulwa@us.ibm.com> test re-write - pull out common code
 *
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/time.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <selinux/selinux.h>
#include "test.h"
#include "usctest.h"
#include "audit_utils.h"

char *TCID = "audit_syscall_utimes";
int TST_TOTAL = 6;
extern int Tst_count;

#define LOG_HEADER_SIZE 100

void syscall_success();
void syscall_fail();

int main(int ac, char **av)
{
	struct audit_rule_fields *fields = NULL;


	syscall_success(fields);
	syscall_fail(fields);
	return(0);
}

void syscall_success()
{
	TEST_RETURN = -1;       /* reset verify value */
	int fd;                     /* return code  */
	struct timeval tvp;
	int rc=0;

    char filename[40] = "test-input-success-utimes";

	/* utimes setup  */
	/* tvp[0].tv_sec = actime */
	/* tvp[1].tv_sec = modtime */
	tvp.tv_sec = 30;
	tvp.tv_usec = 0;

	if ((fd = creat(filename, 0777)) == -1) {
		tst_resm(TFAIL, "SOURCE FILE CREATION ERROR - %i", errno);
                exit(1);
	}

	TEST(syscall(__NR_utimes, filename, &tvp));

	/* Check if syscall got expected return code. */
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL, "utimes for success test failed. %d",
				errno);
        exit(1);
	} else {
		tst_resm(TINFO, "utimes for success test succeeded.");
	}

    remove(filename);
}

void syscall_fail()
{
	TEST_RETURN = -1;       /* reset verify value */
	int rc;                     /* return code  */
	struct timeval tvp;

	/* utimes setup  */
	/* tvp[0].tv_sec = actime */
	/* tvp[1].tv_sec = modtime */
	tvp.tv_sec = 30;
	tvp.tv_usec = 0;

    char filename2[40] = "/bin/grep";

	TEST(syscall(__NR_utimes, filename2, &tvp));

	/* Check if syscall got expected return code. */
	if (errno != EPERM) {
		tst_resm(TFAIL, "Expected EPERM got %d", TEST_ERRNO);
                exit(1);
	} else {
		tst_resm(TINFO, "utimes returned expected EPERM error");
	}
}


