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
 * File: utime_test.c
 *
 * Description: The utime_test() function builds into the LTP
 *    to  verify that the Linux Audit Framework accurately
 *    logs both successful and erroneous execution of the
 *    "utime" system call.
 *
 * Total Tests: 6 (2 assertions)
 *
 * Test Name: utime_test
 *
 * Test Assertion & Strategy:
 *
 *  Verify that:
 *   1. Appropriate audit log created on successfull utime() execution
 *   2. Appropriate audit log created on failing utime() execution
 *      EPERM return value
 *
 * Set audit rules:   
 *   1. entry,always
 *   2. exit,always 
 *   3. entry,never , exit,never
 *
 * Each set of rules will be tested for the following:
 *   1. utime()- Success Case
 *             a) Creates the temporary file
 *             b) Creates the utime data structure
 *             c) Executes the "utime" system call
 *   2. utime()- Erroneous Case
 *             a) Creates the temporary file
 *             b) Creates the utime data structure
 *             c) Executes the "utime" system call as non-root user
 *   NOTE: the id related fields (uid, gid,..etc) are filled in by the switch user
 *      functions. switch_to_super_user() is passed a NULL so the audit record
 *      id related fields do not change.
 *   The erroneous case executes the faulty conditions
 *   described by the "EPERM" errno.
 *
 *   Delete each rule set before adding the next.
 *   Delete the temporary directory(s)/file(s) created.
 *
 * Usage:  <for command-line>
 *  utime_test
 *
 * History:
 * FLAG   DATE             NAME         	      DESCRIPTION
 *        7/1/04   Ashok T.M <ashok-atm@in.ibm.com>  Created this test
 * 	12/19/05 Loulwa Salem<loulwa@us.ibm.com> test re-write - pull out common code
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
#include <selinux/selinux.h>
#include <utime.h>

#include <sys/types.h>
#include <sys/msg.h>

#include "test.h"
#include "usctest.h"
#include "audit_utils.h"

char *TCID = "audit_syscall_utime";
int TST_TOTAL = 6;
extern int Tst_count;

#define LOG_HEADER_SIZE 100

struct timespec mod_time, acc_time;
struct utimbuf utbuf;

void syscall_success();
void syscall_fail(); 

int main(int ac, char **av)
{
	struct audit_rule_fields *fields = NULL;

    syscall_fail(fields);

    syscall_success(fields);

    
    	
	return(0);
}

void syscall_success()
{
	TEST_RETURN = -1;       /* reset verify value */
	int fd;				/* file descriptor  */	
	int rc=0;
    char filename[40] ="test-input-success-utime";

	/* utime setup  */
	/* variabls below are globals used also in the fail case */
	utbuf.modtime = mod_time.tv_sec = 10;
	mod_time.tv_nsec = 0;
	utbuf.actime = acc_time.tv_sec = 30;
	acc_time.tv_nsec = 0;
	if ( ( fd =creat(filename,0777)) == -1) {
		tst_resm(TFAIL, "SOURCE FILE CREATION ERROR - %i",errno);
	}

	TEST(syscall( __NR_utime, filename, &utbuf ));

	/* Check if syscall got expected return code. */
	if (TEST_RETURN == -1 ){
		tst_resm(TFAIL, "utime for success test failed. %d",errno);
        exit(1);
	}else {
		tst_resm(TINFO, "utime for success test succeeded.");
	}
    remove(filename);
}

void syscall_fail()
{
	char log_header[LOG_HEADER_SIZE] = "";
	TEST_RETURN = -1;       /* reset verify value */
	int rc;				/* return code  */	
    char filename2[40] ="/bin/grep";

	TEST(syscall( __NR_utime, filename2, &utbuf ));

	/* Check if syscall got expected return code. */
	if (TEST_ERRNO != EPERM){
		tst_resm(TFAIL,"Expected EPERM got %d",TEST_ERRNO);
        exit(1);
	}else {
		tst_resm(TINFO, "utime returned expected EPERM error");
   	}
}


