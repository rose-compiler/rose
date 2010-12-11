/*AS FAILING because hudson can't open /dev/tty
./syscall_tst.54
specimen exited with status 0
specimen exited with status 0
audit_syscall_ioctl    1  TFAIL  :  ERROR: Cannot open tty device  /dev/tty

audit_syscall_ioctl    2  TFAIL  :  ioctl for success test failed.
make[4]: *** [check-syscall_tst.54] Error 1
make[4]: *** Waiting for unfinished jobs....

*/
/*
 *   Copyright (c) International Business Machines  Corp., 2005,2006,2007
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
 * File: ioctl_test.c
 *
 * Test Name:
 *	ioctl Test [audit_syscall_ioctl]
 *
 * Test Decription:
 *	Verify that:
 *	1. Appropriate audit log created on successfull ioctl() execution
 * 	2. Appropriate audit log created on failing ioctl() execution
 *	   ENOTTY return value
 *
 * Total Tests:
 *	6
 *
 * Test Assertions & Strategy:
 *	Set audit rules:   
 * 	1. entry,always
 *	2. exit,always 
 *	3. entry,never; exit,never
 *
 *	Each set of rules will be tested for the following:
 *	1.ioctl()-
 *	a) open /dev/tty device
 *	b) Calls ioctl with the device file descriptor,TCGETA command and
 *	   termio structure.
 *	c) Tests the result of the call against the expected successful return.
 *	   The function returns zero
 *	d) Close the file descriptor
 *
 *	2. ioctl()-
 *	a) Opens a temporary file
 *	b) Calls ioctl with temporary file, file descriptor
 *	c) Tests the result of the call against the expected successful return
 *	   The function returns  -1 with errno set to ENOTTY
 *	d) Close the file descriptor and  unlink the path.
 *
 *	Delete each rule set before adding the next.
 *	Delete the temporary directory(s)/file(s) created.
 *
 * Usage:
 *	./ioctl_test
 *
 ********************************** HISTORY ***********************************
 *
 * DATE		NAME			DESCRIPTION
 * 02/06/2006	mcthomps@us.ibm.com	Updated prologue and fixed includes
 * 12/16/2005	loulwa@us.ibm.com	test re-write - pull out common code
 * 02/09/2005	deepakraj@in.ibm.com	Created this test
 *
 *****************************************************************************/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/ioctl.h>

#include "test.h"
#include "usctest.h"
#include "audit_utils.h"

char *TCID = "audit_syscall_ioctl";
int TST_TOTAL = 6;
extern int Tst_count;

#define LOG_HEADER_SIZE 100
int fd;

struct audit_record *success_audit_record = NULL;
struct audit_record *fail_audit_record = NULL;

void test_setup();
void test_cleanup();
void syscall_success();
void syscall_fail();

int main(int ac, char **av)
{

	syscall_success();
	syscall_fail();
	return(0);
}

void syscall_success()
{
	TEST_RETURN = -1;       /* reset verify value */
	char* dev = "/dev/tty";
	struct termio tio;

	if(( fd = open( dev,O_RDWR,0777 )) == -1 ){ 
		tst_resm(TFAIL, "ERROR: Cannot open tty device  %s\n",dev);
                exit(1);
	}

	TEST(syscall( __NR_ioctl, fd, TCGETA, &tio ));
	
	/* Check if syscall got expected error */
	if (TEST_RETURN < 0 ){
		tst_resm(TFAIL, "ioctl for success test failed.");
        exit(1);
	}else {
		tst_resm(TINFO, "ioctl for success test succeeded.");
	}
}

void syscall_fail()
{
	TEST_RETURN = -1;       /* reset verify value */
	char* notty = "/tmp/notty";
	struct termio tio;

	if ((fd = open(notty, O_CREAT, 0777)) == -1){
		tst_resm(TFAIL, "ERROR: Cannot create file %s\n",notty);
                exit(1);
	}

	TEST(syscall( __NR_ioctl, fd, TCGETA, &tio ));

	if (TEST_ERRNO != ENOTTY){
		tst_resm(TFAIL,"Expected ENOTTY got %d",TEST_ERRNO);
        exit(1);
	}else {
		tst_resm(TINFO, "ioctl returned expected ENOTTY error");
	}
}


