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

#if !defined(__S390X) && !defined(__S390)
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
char filename[40] = "";

struct test_user_data *user = NULL;
struct audit_record *success_audit_record = NULL;
struct audit_record *fail_audit_record = NULL;
security_context_t subj,obj;

void test_setup();
void test_cleanup();
void syscall_success(struct audit_rule_fields *);
void syscall_fail(struct audit_rule_fields *);

int main(int ac, char **av)
{
	struct audit_rule_fields *fields = NULL;

	test_setup();

	fields = alloc_init_audit_fields();
	Tst_count = 0;

	fields->syscall = "utimes";

	/* case 1 : entry,always */
	fields->list = "entry";
	fields->action = "always";
	add_audit_rule(fields);

	syscall_success(fields);
	syscall_fail(fields);
	remove_all_audit_rules();

	/* case 2 : exit,always */
	fields->list = "exit";
	add_audit_rule(fields);

	syscall_success(fields);
	syscall_fail(fields);
	remove_all_audit_rules();

	/* case 3: entry,never - exit,never */
	fields->action = "never";
	fields->list = "entry";
	add_audit_rule(fields);
	fields->list = "exit";
	add_audit_rule(fields);

	syscall_success(fields);
	syscall_fail(fields);
	remove_all_audit_rules();

	free(fields);
	free(success_audit_record);
	free(fail_audit_record);
	freecon(subj);
	freecon(obj);
	test_cleanup();

	/*NOTREACHED*/
	return(0);
}

void syscall_success(struct audit_rule_fields *fields)
{
	char log_header[LOG_HEADER_SIZE] = "";
	TEST_RETURN = -1;       /* reset verify value */
	int fd;                     /* return code  */
	struct timeval tvp;
	int rc=0;

	/* utimes setup  */
	/* tvp[0].tv_sec = actime */
	/* tvp[1].tv_sec = modtime */
	tvp.tv_sec = 30;
	tvp.tv_usec = 0;

	if ((fd = creat(filename, 0777)) == -1) {
		tst_resm(TFAIL, "SOURCE FILE CREATION ERROR - %i", errno);
	}

	clear_audit_log();
	begin_test();
	success_audit_record->start_time=time(NULL)-1;
	TEST(syscall(__NR_utimes, filename, &tvp));
	success_audit_record->end_time=time(NULL)+1;
	end_test();

	/* Check if syscall got expected return code. */
	if (TEST_RETURN == -1) {
		tst_resm(TFAIL, "utimes for success test failed. %d",
				errno);
	} else {
		tst_resm(TINFO, "utimes for success test succeeded.");

		success_audit_record->audit_type = TYPE_SYSCALL;
		success_audit_record->syscallno = __NR_utimes;
		success_audit_record->auid=get_auid();
		success_audit_record->uid = getuid();
		success_audit_record->pid = getpid();
		success_audit_record->exit = TEST_RETURN;
		success_audit_record->argv[0] = (unsigned long) filename;
		success_audit_record->argv[1] = (unsigned long) &tvp;
		strcpy(success_audit_record->objectname, filename);
		getcon_raw(&subj);
                strcpy(success_audit_record->subj,(char *)subj);
                rc = getfilecon_raw(filename,&obj);
                if (rc < 0) {
                        tst_resm(TBROK,"Unable to get security context");
                }
                strcpy(success_audit_record->obj,(char *)obj);

		/* Search for the right record */
		TEST_RETURN  = verify_record_existence(success_audit_record);
		check_results(fields);
	}
	snprintf(log_header, LOG_HEADER_SIZE, "\n%s/%s : SUCCESS CASE"
		"\n==========\n", fields->list, fields->action);
	save_audit_log(TCID, log_header);
}

void syscall_fail(struct audit_rule_fields *fields)
{
	char log_header[LOG_HEADER_SIZE] = "";
	TEST_RETURN = -1;       /* reset verify value */
	int rc;                     /* return code  */
	struct timeval tvp;

	/* utimes setup  */
	/* tvp[0].tv_sec = actime */
	/* tvp[1].tv_sec = modtime */
	tvp.tv_sec = 30;
	tvp.tv_usec = 0;

	clear_audit_log();
	begin_test();

	/* change to test user */
	if ((rc = switch_to_test_user(user, fail_audit_record)) == -1) {
		tst_resm(TFAIL, "CREATING TEST USER ERROR %d\n", rc);
	}

	fail_audit_record->start_time=time(NULL)-1;
	TEST(syscall(__NR_utimes, filename, &tvp));
	fail_audit_record->end_time=time(NULL)+1;

	/* change  to super  user */
	if ((rc = switch_to_super_user(NULL)) == -1) {
		tst_resm(TFAIL, "CHANGING TO SUPER USER ERROR %d\n", rc);
	}
	end_test();

	TEST_ERROR_LOG(TEST_ERRNO);

	/* Check if syscall got expected return code. */
	if (TEST_ERRNO != EPERM) {
		tst_resm(TFAIL, "Expected EPERM got %d", TEST_ERRNO);
	} else {
		tst_resm(TINFO, "utimes returned expected EPERM error");

		fail_audit_record->audit_type = TYPE_SYSCALL;
		fail_audit_record->syscallno = __NR_utimes;
		fail_audit_record->auid=get_auid();
		fail_audit_record->pid = getpid();
		fail_audit_record->exit = TEST_ERRNO;
		fail_audit_record->argv[0] = (unsigned long) filename;
		fail_audit_record->argv[1] = (unsigned long) &tvp;
		strcpy(fail_audit_record->objectname, filename);
		getcon_raw(&subj);
                strcpy(fail_audit_record->subj,(char *)subj);
                rc = getfilecon_raw(filename,&obj);
                if (rc < 0) {
                        tst_resm(TBROK,"Unable to get security context");
                }
                strcpy(fail_audit_record->obj,(char *)obj);

		/* Search for the right record */
		TEST_RETURN  = verify_record_existence(fail_audit_record);
		check_results(fields);
	}
	snprintf(log_header, LOG_HEADER_SIZE, "\n%s/%s : FAIL CASE"
		"\n==========\n", fields->list, fields->action);
	save_audit_log(TCID, log_header);
}

void test_setup()
{
	success_audit_record =(struct audit_record *)malloc
		(sizeof(struct audit_record));
	initialize_audit_record(success_audit_record);

	fail_audit_record =(struct audit_record *)malloc
		(sizeof(struct audit_record));
	initialize_audit_record(fail_audit_record);

	sprintf(filename, "time.%d", getpid());
	user = create_test_user(WHEEL);
	general_setup(TCID, test_cleanup);
}

void test_cleanup()
{
	delete_test_user(user);
	general_cleanup(TCID);
}
#else
int main(int argc, char **argv)
{
	fprintf(stderr,"%s test skipped, not executing on this platform.\n",
		argv[0]);
	return(0);
}
#endif
