/*
 *   Copyright (c) International Business Machines  Corp.,2005,2006,2007
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
 *  File: fchown_test.c
 *
 *  Description:
 *	The fchown_test() function builds into the ltp framework to 
 *	verify that the Linux Audit Framework accurately logs both 
 *	successful and erroneous execution of the "fchown" system call.
 *
 *  Verify that:
 *   1. Appropriate audit log created on successful fchown() execution
 *   2. Appropriate audit log created on failing fchown() execution
 *      EPERM return value 
 *
 *  Total Tests: 6 assertions
 *
 *  Test Name: fchown_test
 *
 *  Test Assertion & Strategy:
 *   Create a temporary file and try to access it as follows:
 *   
 *  Add audit rules
 *   1. entry,always
 *   2. exit,always 
 *   3  entry,never; exit,never. 
 *	
 *   Each set of rules will be tested for the following:
 *   1. fchown()- Change owner/group with valid file descriptor. 
 *	Success, good audit record
 *   2. fchown()- Change owner/group with valid file descriptor by non-root 
 *	user. Failure, good audit record
 *   NOTE: the id related fields (uid, gid,..etc) are filled in by the switch user
 *      functions. switch_to_super_user() is passed a NULL so the audit record
 *      id related fields do not change.
 *	
 *  History:
 * FLAG DATE    		 NAME        	    		 DESCRIPTION
 *      1/6/05 	DEEPAK RAJ <deepakraj@in.ibm.com> Created this test
 *	12/15/05 L.Salem <loulwa@us.ibm.com> test re-write - pull out common code
 *      03/07/07  Debora V. <dvelarde@us.ibm.com>  add context check
 *
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <selinux/selinux.h>
#include "test.h"
#include "usctest.h"
#include "audit_utils.h"

char *TCID = "audit_syscall_fchown";
int TST_TOTAL = 6;
extern int Tst_count;
security_context_t subj;

#define LOG_HEADER_SIZE 100

int fd;				/* file descriptor */

void syscall_success();
void syscall_fail();
void test_cleanup(){ };
int main(int ac, char **av)
{
  syscall_success();

  return(0);
}

void syscall_success()
{
  TEST_RETURN = -1;	/* reset verify value */

  char pfilname[40] = "test-file-fchown32";
  if ( ( fd =creat(pfilname,0777)) == -1) {
		tst_resm(TFAIL, "SOURCE FILE CREATION ERROR - %i",errno);
  }	
  /* open a file for read/write */
  if ( (fd = open(pfilname, O_RDWR|O_CREAT)) == -1 ){
    tst_brkm(TBROK, test_cleanup, "Unable to open %s for read/write.  Error:%d, %s",
        pfilname, errno, strerror(errno));
    exit(1);
  }

  TEST(syscall(__NR_fchown,fd,geteuid(),getegid()));

  /* Check if syscall got expected return code. */
  if (TEST_RETURN < 0 ){
    tst_resm(TFAIL, "fchown for success test failed.");
    exit(1);
  }else {
    tst_resm(TINFO, "fchown for success test succeeded.");
  }

  

  close(fd);

  remove(pfilname);
}



