/*
 *   Copyright (c) International Business Machines  Corp., 2006,2007
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
 ******************************************************************************
 * File: clock_settime_test.c
 *
 * Test Name:
 *	clock_settime Test [audit_syscall_clock_settime]
 *
 * Test Decription:
 *	This test builds into the LTP to verify correct audit record generation
 *	and to functionally verify the behaviour of the "clock_settime" system
 *	call. Verifing the audit record generation entails that verification
 *	is done on the Linux Audit Framework, ensuring accurate logs of both
 *	successful and erroneous execution of the "clock_settime" system call.
 *	This is not a functional test of the system call.
 *
 * Total Tests:
 *      6
 *
 * Test Assertions & Strategy:
 *	Verify that:
 *	1. Appropriate audit log created on successful clock_settime() execution
 *	2. Appropriate audit log created on failing clock_settime() execution
 *	   (EPERM return value)
 *
 *	Set audit rules:
 *	1. entry,always
 *	2. exit,always 
 *	3. entry,never; exit,never
 *
 *	Each set of rules will be tested for the following:
 *	1. clock_settime()- Success Case
 *	   a) Obtain timespec struct values from clock_gettime()
 *	   b) Use clock_settime with the previously obtained value as root
 *	   d) Verify record existence
 *	2. clock_settime()- Erroneous Case
 *	   a) Obtain timespec struct values from clock_gettime()
 *	   b) Use clock_settime with the previously obtained value as non-root
 *	   c) Generates EPERM errno
 *	   d) Verify record existence
 *
 * N.B. the id related fields (uid, gid,..etc) are filled in by the switch user
 *      functions. switch_to_super_user() is passed a NULL so the audit record
 *      record id related fields do not change.
 *
 *   Delete each rule set before adding the next.
 *
 * Usage:
 *	./clock_settime_test
 *
 ********************************** HISTORY ***********************************
 *
 * DATE         NAME                    DESCRIPTION
 * 10/10/2006	mcthomps@us.ibm.com	Originated
 * 03/07/2007	dvelarde@us.ibm.com	Check security context
 *
 *****************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <selinux/selinux.h>

#include "audit_utils.h"
#include "test.h"
#include "usctest.h"

/* LTP required globals */
char *TCID = "audit_syscall_clocl_settime";
int TST_TOTAL = 6;

void testcase_cleanup()
{
}

void syscall_success()
{
  int errnum, rc;
  struct timespec tspec;
  clockid_t clock = CLOCK_REALTIME;
  rc = clock_gettime(clock, &tspec);
  if (rc) {
    errnum = errno;
    tst_brkm(TBROK, testcase_cleanup, "Unable to extract time: "
        "errno=[%d], %s", errnum, strerror(errnum));
  }

  /* Check if syscall got expected return code. */
  if (rc) {
    tst_resm(TFAIL, "clock_settime for success test failed. "
        "rc=[%d] errno=[%d]", rc, errnum);
    exit(1);
  } else {
    tst_resm(TINFO, "clock_settime for success test succeeded.");
  }
}

void syscall_fail()
{
  int err, errnum, rc;
  struct timespec tspec;
  clockid_t clock = CLOCK_REALTIME;
  rc = clock_gettime(clock, &tspec);

  if (rc) {
    errnum = errno;
    tst_brkm(TBROK, testcase_cleanup, "Unable to extract time: "
        "errno=[%d], %s", errnum, strerror(errnum));
  }

  /* Check if syscall got expected return code. */
  if ((rc) && (errnum != EPERM)) {
    tst_resm(TFAIL, "Expected EPERM, got rc=[%d] errno=[%d], %s",
        rc, errnum), strerror(errnum);
    exit(1);
  } else {
    tst_resm(TINFO, "clock_settime returned expected EPERM error");
  }
}

int main(int argc, char **argv)
{
  syscall_success();
  syscall_fail();
  return 0;
}
