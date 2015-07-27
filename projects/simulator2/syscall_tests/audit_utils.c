/*
 *   Copyright (C) International Business Machines  Corp., 2004,2005
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
 * File: audit_utils.c
 *
 * Overview:
 * 	This file contains utilities which are required by the audit tests to
 * 	perform their functions.
 *
 * Description:
 *
 ********************************** HISTORY ***********************************
 *
 * DATE         NAME                    DESCRIPTION
 * 02/08/2007   fleury@us.ibm.com       Added new field initializations (cups)
 * 09/20/2006	loulwa@us.ibm.com	Added new field initializations
 * 08/16/2006	loulwa@us.ibm.com	Added new field initializations
 *					Remved watch related fields
 * 01/16/2006	mcthomps@us.ibm.com	Added SUPPRESS_VERBOSE_OUTPUT,
 *					SUPPRESS_AUDITD_OUTPUT and added
 *					missing auditctl field processing
 * 01/03/2006	loulwa@us.ibm.com       added watch related code
 * 12/22/2005	loulwa@us.ibm.com	added syscall_num field, and error check
 *					in case user defines both syscall name
 *					and number
 * 12/15/2005   mcthomps@us.ibm.com     Changed the way createTempGroupName
 *					grows names. Was A->Z, now Z->A.
 * 12/01/2005   mcthomps@us.ibm.com     Created based on eal4's audit_utils.c
 *
 *****************************************************************************/

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "audit_utils.h"
#include "test.h"
#include "usctest.h"

/*======================================================*/
/*		Common Utilities Source			*/
/*======================================================*/

#define MAX_TESTNAME_SIZE 60
#define AUDIT_LOG_LOC_SIZE 4096
static char audit_log_location[AUDIT_LOG_LOC_SIZE];


/*======================================================*/
/*			SETUP				*/
/*======================================================*/
/**
 * Cleans up old logs files and registers the cleanup function with
 * the signal handler. Also saves the config and rules file for the
 * audit daemon. The audit daemon will be stopped when this returns.
 *
 * N.B. This should be called before any other setup is performed
 *	for the test.
 *	The cleanup function should end with a call to general_cleanup
 *	so that system integrety is preserved.
 *
 * @testname	Name of the current test
 * @cleanup	Pointer to function of type void fn(void) which should
 *		eventually call general_cleanup
 */
void general_setup(char *testname, void (*cleanup)())
{
	tst_resm(TINFO, "START OF RESULTS LOG FOR %s TEST\n", testname);
	stop_audit();
	run_sys_cmd("rm /tmp/%s -f",testname);
	clear_audit_log();
	tst_sig(FORK, DEF_HANDLER, cleanup);
	save_audit_config_file();
	save_audit_rules_file();
	tst_tmpdir();
	remove_all_audit_rules();
}

/**
 * Restores the audit config & rules file to their original state
 * before the test was run. The audit daemon will be running when
 * the test ends.
 * 
 * N.B. This function does not return. It must be the last function
 * 	called by the test.
 *
 * @testname	Name of the current test
 */
void general_cleanup(char *testname)
{
	tst_resm(TINFO, "END OF RESULTS LOG FOR %s TEST\n", testname);
	restore_audit_rules_file();
	restore_audit_config_file();
	TEST_CLEANUP;
	start_audit();
	tst_rmdir();
	tst_exit();
}


/*======================================================*/
/*		AUDITD MANIPULATION			*/
/*======================================================*/
/**
 * Restart the audit daemon
 * N.B. This does not ensure that the daemon will be in a
 *	ready state.
 *
 * @return	Zero on success, non-zero otherwise
 */
int restart_audit()
{
	int rc;

#if SUPPRESS_AUDITD_OUTPUT
	rc = run_sys_cmd("service auditd restart 2>&1 > /dev/null");
#else
	rc = run_sys_cmd("service auditd restart");
#endif
	if (rc) {
		tst_resm(TINFO, "RESTART AUDIT FAILED - returns %d", rc);
	}
	return rc;
}

/**
 * Start the audit daemon
 * N.B. This does not ensure that the daemon will be in a
 *	ready state.
 *
 * @return	Zero on success, non-zero otherwise
 */
int start_audit()
{
	int rc;
	
#if SUPPRESS_AUDITD_OUTPUT
	rc = run_sys_cmd("service auditd start 2>&1 > /dev/null");
#else
	rc = run_sys_cmd("service auditd start");
#endif
	if (rc) {
		tst_resm(TINFO, "START AUDIT FAILED - returns %d", rc);
	}
	return rc;
}

/**
 * Stop the audit daemon
 * N.B. This does not ensure that the daemon will have flushed
 *	records waiting in the queue.
 *
 * @return	Zero on success, non-zero otherwise
 */
int stop_audit()
{
	int rc;
	
#if SUPPRESS_AUDITD_OUTPUT
	rc = run_sys_cmd("service auditd stop 2>&1 > /dev/null");
#else
	rc = run_sys_cmd("service auditd stop");
#endif
	if (rc) {
		tst_resm(TINFO, "STOP AUDIT FAILED - returns %d", rc);
	}
	return rc;
}

void pause_for_auditd()
{
	sleep(1);
}

/*======================================================*/
/*		AUDIT RULE MANIPULATION			*/
/*======================================================*/
/**
 * Allocates and initializes the audit_rule_fields structure.
 * N.B. Any memory pointed to by this function is the responsibility
 *	of the user to free.
 *
 * @return	malloc'd memory which the user must free, NULL on failure
 */
struct audit_rule_fields *alloc_init_audit_fields()
{
	struct audit_rule_fields *fields = NULL;

	fields = (struct audit_rule_fields *)
			malloc(sizeof(struct audit_rule_fields));
	if (!fields) {
		tst_resm(TINFO, "Unable to allocate the fields structure");
		return NULL;
	}
	fields->watch = NULL;
	fields->w_key = NULL;
	fields->w_perms = NULL;

	fields->list_head = HEAD_NO;
	fields->list = NULL;
	fields->action = NULL;
	fields->syscall = NULL;
	fields->syscall_num = 0;
	fields->ppid = NULL;
	fields->pid = NULL;
	fields->uid = NULL;
	fields->euid = NULL;
	fields->suid = NULL;
	fields->fsuid = NULL;
	fields->gid = NULL;
	fields->egid = NULL;
	fields->sgid = NULL;
	fields->fsgid = NULL;
	fields->auid = NULL;
	fields->exit = NULL;
	fields->success = NULL;
	fields->arg0 = NULL;
	fields->arg1 = NULL;
	fields->arg2 = NULL;
	fields->arg3 = NULL;

	fields->arch = NULL;
	fields->pers = NULL;
	fields->devmajor = NULL;
	fields->devminor = NULL;
	fields->inode = NULL;

        fields->msgtype = NULL;
        fields->subj_user = NULL;
        fields->subj_role = NULL;
        fields->subj_type = NULL;
        fields->subj_sen = NULL;
        fields->subj_clr = NULL;
        fields->obj_user = NULL;
        fields->obj_role = NULL;
        fields->obj_type = NULL;
        fields->obj_lev_low = NULL;
        fields->obj_lev_high = NULL;
	fields->path = NULL;

	return fields;
}

/**
 * Restrictions of rules as defined by man auditctl (copied here for convience)
 * > watch rules are -w with optional -k and -p
 * > syscall rules are -a / -aA with optional -S and -F
 *
 * Note on formatting:
 * 	The current format for fields with numerical values (such as arg0)
 * 	is sprintf(command,"-F field%s", field->field). This is done to
 * 	support the various equality operations (notably '=' and '!=') in
 * 	a simple and clear fashion.
 *
 * N.B. We are always setting arch on a new audit rule unless it is
 *	specifically over-ridden with arch=ARCH_NO_SET ("NOSET");
 *
 * @fields		User-filled struct from add/delete_audit_rule
 * @auditctl_comm	The string to continue constructing on, it will be a
 *			statically allocated buffer of size AUDITCTL_COMM_SIZE
 */
#define SUB_FIELD_SIZE	50
#define AUDITCTL_COMM_SIZE 4096
static void construct_auditctl_command(struct audit_rule_fields *fields,
				       char *auditctl_comm)
{
	char *arch;
	char sub_field[SUB_FIELD_SIZE];

#if defined(__MODE_32)
	arch = "b32";
#else
	arch = "b64";
#endif
	/* watch_key and watch_perm only make sense with a watch */
	if (fields->watch) {
		if (fields->w_key) {
			snprintf(sub_field, SUB_FIELD_SIZE, "-k %s ",
				 fields->w_key);
			strcat(auditctl_comm, sub_field);
		}
		if (fields->w_perms) {
			snprintf(sub_field, SUB_FIELD_SIZE, "-p %s ",
				 fields->w_perms);
			strcat(auditctl_comm, sub_field);
		}
		return;
	} else if (!fields->list || !fields->action) {
		tst_resm(TINFO, "WARNING: Does an audit rule have any meaning"
				" without a list/action pair or a watch? "
				"!!Aborting add!!\n");
		return;
	}
	/* Assume anything else is for syscalls */
	if (!fields->arch) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F arch=%s ", arch);
		strcat(auditctl_comm, sub_field);
	} else if (strcmp(fields->arch,ARCH_NO_SET)) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F arch=%s ",
		fields->arch);
		strcat(auditctl_comm, sub_field);
	}
	if ( (fields->syscall) && (fields->syscall_num) ) {
		tst_resm(TINFO, "WARNING: Please supply either syscall number"
				" or name, but not both.\nAborting add\n");
		return;
        }
	if (fields->syscall) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-S %s ", fields->syscall);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->syscall_num) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-S %d ",
			 fields->syscall_num);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->ppid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'ppid%s' ", fields->ppid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->pid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'pid%s' ", fields->pid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->uid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'uid%s' ", fields->uid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->euid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'euid%s' ", fields->euid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->suid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'suid%s' ", fields->suid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->fsuid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'fsuid%s' ", fields->fsuid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->gid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'gid%s' ", fields->gid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->egid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'egid%s' ", fields->egid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->sgid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'sgid%s' ", fields->sgid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->fsgid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'fsgid%s' ", fields->fsgid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->auid) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'auid%s' ", fields->auid);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->exit) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'exit%s' ", fields->exit);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->success) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'success%s' ", fields->success);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->arg0) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'a0%s' ", fields->arg0);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->arg1) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'a1%s' ", fields->arg1);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->arg2) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'a2%s' ", fields->arg2);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->arg3) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'a3%s' ", fields->arg3);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->msgtype) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'msgtype%s' ", fields->msgtype);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->subj_user) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'subj_user%s' ",
			 fields->subj_user);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->subj_role) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'subj_role%s' ",
			 fields->subj_role);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->subj_type) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'subj_type%s' ",
			 fields->subj_type);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->subj_sen) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'subj_sen%s' ",
			 fields->subj_sen);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->subj_clr) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'subj_clr%s' ",
			 fields->subj_clr);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->obj_user) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'obj_user%s' ",
			 fields->obj_user);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->obj_role) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'obj_role%s' ",
			 fields->obj_role);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->obj_type) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'obj_type%s' ",
			 fields->obj_type);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->obj_lev_low) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'obj_lev_low%s' ",
			 fields->obj_lev_low);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->obj_lev_high) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'obj_lev_high%s' ",
			 fields->obj_lev_high);
		strcat(auditctl_comm, sub_field);
	}
	if (fields->path) {
		snprintf(sub_field, SUB_FIELD_SIZE, "-F 'path%s' ", fields->path);
		strcat(auditctl_comm, sub_field);
	}
}

/**
 * Using the values specified in the argument audit_rule_fields
 * structure, we construct the command necessary to add the audit
 * rule to the active rules list.
 *
 * @fields	A struct audit_rule_fields pointer obtained through a call to
 *		alloc_init_audit_fields()
 *
 * Returns 0 on success, non-zero otherwise
 */
int add_audit_rule(struct audit_rule_fields *fields)
{
	int rc;
	char auditctl_comm[AUDITCTL_COMM_SIZE];

	if (fields->watch) {
		sprintf(auditctl_comm, "auditctl -w %s ", fields->watch);
	} else if (fields->list && fields->action) {
		if (fields->list_head == HEAD_YES) {
			sprintf(auditctl_comm, "auditctl -A %s,%s ",
				fields->list, fields->action);
		} else {
			sprintf(auditctl_comm, "auditctl -a %s,%s ",
				fields->list, fields->action);
		}
	} else {
		tst_resm(TINFO, "WARNING: Does an auditctl command have any "
				"meaning without a list & action pair? "
				"Aborting add");
		return -1;
	}
	construct_auditctl_command(fields, auditctl_comm);
	printf("\nAUDITCTL_COMM: %s\n\n", auditctl_comm);
	rc = run_sys_cmd(auditctl_comm);
	if (rc) {
		tst_resm(TINFO, "add_audit_rule failed - auditctl_comm [%s] "
				"returned %d", auditctl_comm, rc);
	}
	return rc;
}

/**
 * Using the values specified in the argument audit_rule_fields
 * structure, we construct the command necessary to remove an audit
 * rule from the active rules list.
 *
 * @fields	A struct audit_rule_fields pointer obtained through a call to
 *		alloc_init_audit_fields()
 *
 * Returns 0 on success, non-zero otherwise
 */
int delete_audit_rule(struct audit_rule_fields *fields)
{
	int rc;
	char auditctl_comm[AUDITCTL_COMM_SIZE];

	if (fields->watch) {
		sprintf(auditctl_comm, "auditctl -W %s ", fields->watch);
	} else if (fields->list && fields->action) {
		sprintf(auditctl_comm, "auditctl -d %s,%s ",
			fields->list, fields->action);
	} else {
		tst_resm(TINFO, "WARNING: Does an auditctl command have any "
				"meaning without a list & action pair? "
				"Aborting delete");
		return -1;
	}
	construct_auditctl_command(fields, auditctl_comm);
	rc = run_sys_cmd(auditctl_comm);
	if (rc) {
		tst_resm(TINFO, "delete_audit_rule failed - auditctl_comm [%s]"
				" returned %d", auditctl_comm, rc);
	}
	return rc;
}

/**
 * Removes all active audit rules
 *
 * Returns 0 on success, non-zero otherwise
 */
int remove_all_audit_rules()
{
	int rc = 0;	

#if SUPPRESS_VERBOSE_OUTPUT
	rc = run_sys_cmd("auditctl -D 2>&1 > /dev/null");
#else
	rc = run_sys_cmd("auditctl -D");
#endif
	if (rc) {
		tst_resm(TINFO, "REMOVE AUDIT RULES FAILED - returns %d\n",rc);
	}
	return rc;
}

/*
 * Remove watch on the path specified by the file_name argument
 *
 * Returns 0 on success, non-zero otherwise
 */
int remove_watch(char * file_name, char * filter_key)
{
        int rc;

	if (filter_key != NULL)
        	rc = run_sys_cmd("auditctl -W %s -k %s", file_name, filter_key);
	else
        	rc = run_sys_cmd("auditctl -W %s", file_name);

        if (rc) {
                tst_resm(TINFO, "REMOVE WATCH FAILED ON PATH %s", file_name);
        }
	return rc;
}

/**
 * Saves the list of active rules provided by auditctl and saves them
 * into the file specified by @filename
 *
 * @filename	Path to the file in which to store the active rules
 */
void save_active_audit_rules(char *filename)
{
	run_sys_cmd("auditctl -l > %s",filename);
}


/*======================================================*/
/*		LOG MANIPULATION			*/
/*======================================================*/
/**
 * Returns the absolute path to the syslog
 *
 * @return	Pointer to statically allocated memory
 */
const char *get_syslog_filename()
{
	return "/var/log/messages";
}

/**
 * Determines the location where the audit records are supposed to be
 * logged to by parsing the auditd.conf file. This will return a char*
 * to a location in memory which is staticly allocated so that the
 * user does not need to worry about using free.
 *
 * @return	Pointer to statically allocated memory
 */
const char *get_audit_log_filename()
{
	int rc, index = 0, tmp_loc_size = AUDIT_LOG_LOC_SIZE;
	char tmp_file[50];
	char tmp_loc[tmp_loc_size];
	FILE *tmp_stream;

	sprintf(tmp_file,"/tmp/logloc.%d",getpid());
	rc = run_sys_cmd("grep ^log_file %s | awk {'print$3'}>%s",
		    get_audit_config_filename(), tmp_file);
	if (rc) {
		tst_resm(TINFO, "Unable to determine where the audit log is "
				"located");
		goto exit;
	}
	tmp_stream = fopen(tmp_file, "r");
	if (!tmp_stream) {
		tst_resm(TINFO, "Unable to open tmp_file");
		goto exit;
	}
	if (fgets(tmp_loc, tmp_loc_size, tmp_stream) != NULL) {
		/* N.B. We subtract 1 here because of an extra '\n' */
		index = strlen(tmp_loc) - 1;
		strncpy(audit_log_location,tmp_loc,AUDIT_LOG_LOC_SIZE-1);
	} else {
		tst_resm(TINFO, "Unable to read %s\n", tmp_file);
	}
	fclose(tmp_stream);
	unlink(tmp_file);
exit:
	audit_log_location[index] = '\0';
	return audit_log_location;
}

/**
 * Clears the contents of the syslog with a simple cat /dev/null
 */
void clear_syslog()
{
	run_sys_cmd("cat /dev/null > %s",get_syslog_filename());
}

/**
 * Clears the contents of the audit log with a cat /dev/null
 */
void clear_audit_log()
{
	run_sys_cmd("cat /dev/null > %s",get_audit_log_filename());
}

/**
 * Removes the audit log and any rotated audit logs
 */
void delete_audit_log()
{
	run_sys_cmd("rm -f %s*",get_audit_log_filename());
}

/**
 * Takes the contents of a log file, and saves it to a file whose name is
 * derived from the provided test name. If a header is provided, the header
 * string will prefix the log contents.
 * Example:
 *	A test with a name of "audit_test1" will have the contents of the
 *	audit log file saved to /tmp/audit_test1 with header before the log
 *	contents, if @header is non-NULL
 *
 * @testname	Name of test which is saving log contents
 * @header	Header text to insert into file before log contents
 */
void save_audit_log(char *testname, char *header)
{
	FILE* log_file;
	char file[MAX_TESTNAME_SIZE+6];

	sprintf(file,"/tmp/%s",testname);
	if (header) {
		log_file = fopen(file,"a");
		if (log_file) {
			fputs(header,log_file);
			fclose(log_file);
		} else {
			tst_resm(TINFO, "Unable to open log file.");
		}
	}
	run_sys_cmd("cat %s >> %s",get_audit_log_filename(),file);
}

/**
 * Saves (as per function save_audit_log) and then clears the audit log
 *
 * @testname	Name of test which is saving log contents
 * @header	Header text to insert into file before log contents
 */
void save_and_clear_audit_log(char *testname, char *header)
{
	save_audit_log(testname,header);
	clear_audit_log();
}

/**
 * Extracts the last @lines lines from the syslog, and stores them
 * in the file specified by @filename.
 *
 * @lines	Number of lines to capture (at end of file)
 * @filename	Path to the file to store those lines in
 */
void grab_and_save_syslog_lines(int lines, char *filename)
{
	run_sys_cmd("tail -n %d %s > %s",
		    lines,get_syslog_filename(),filename);
}

/**
 * Extracts the last @lines lines from the audit log, and stores them
 * in the file specified by @filename.
 *
 * @lines	Number of lines to capture (at end of file)
 * @filename	Path to the file to store those lines in
 */
void grab_and_save_audit_log_lines(int lines, char *filename)
{
	run_sys_cmd("tail -n %d %s > %s",
		    lines,get_audit_log_filename(),filename);
}

/**
 * Verifys that atleast @limit number of logs exist. No claims are made for
 * more than limit logs, since none are checked for after @limit is reached,
 * but the return value will be less than or equal to that of @limit.
 *
 * @limit	Maximum number of logs to look for
 * @return	Number of logs found (bounded by @limit)
 */
int check_audit_logs_exist(int limit)
{
	int i;
	int count = 0;
	char buf[100];
	struct stat notused;
	const char *log_path = get_audit_log_filename();
	
	sprintf(buf,"%s",log_path);
	if (stat(buf,&notused)) {
		if (errno == -ENOENT) {
			tst_resm(TINFO,"Normal audit log file does not exist");
		} else {
			tst_resm(TINFO,"Unable to access normal log file");
		}
		return count;
	}
	count++;
	/* The log extensions start from .1 and count upwards */
	for (i = 1; i < limit; i++) {
		sprintf(buf,"%s.%d",log_path,i);
		if (!stat(buf,&notused)) {
			count++;
		} else {
			return count;
		}
	}
	return count;
}

/*======================================================*/
/*		CONFIG FILE MANIPULATION		*/
/*======================================================*/
/**
 * Returns a handle to the absolute path to the audit config file
 *
 * @return	Pointer to statically allocated character string
 */
char *get_audit_config_filename()
{
	return "/etc/audit/auditd.conf";
}

/**
 * Returns a handle to the absolute path to the audit rules file
 *
 * @return	Pointer to statically allocated character string
 */
char *get_audit_rules_filename()
{
	return "/etc/audit/audit.rules";
}

/**
 * Returns the number of maximum rotated logs possible
 */
int get_num_logs_limit()
{
	return 99;
}

/**
 * Saves the audit config file in such a way that we can restore it later
 */
void save_audit_config_file()
{
	run_sys_cmd("cp -f %s %s.save.%d",get_audit_config_filename(),
		    get_audit_config_filename(),getpid());
}

/**
 * Saves the audit rules file in such a way that we can restore it later
 */
void save_audit_rules_file()
{
	run_sys_cmd("mv -f %s %s.save.%d",get_audit_rules_filename(),
		    get_audit_rules_filename(),getpid());
}

/**
 * Restores the audit config file that was saved using save_audit_config_file()
 */
void restore_audit_config_file()
{
	run_sys_cmd("mv -f %s.save.%d %s",get_audit_config_filename(),
		    getpid(),get_audit_config_filename());
}

/**
 * Restores the audit rules file that was saved using save_audit_rules_file()
 */
void restore_audit_rules_file()
{
	run_sys_cmd("mv -f %s.save.%d %s",get_audit_rules_filename(),
		    getpid(),get_audit_rules_filename());
}

/**
 * Replace the current audit config file with a file specificed by
 * @replacement.
 *
 * N.B. This does not cause a save to take place. In order to backup,
 * 	replace and then restore the default files.
 *	You must call in order:
 *	save_audit_X_file()
 *	replace_audit_X_file(name)
 *	restore_audit_X_file()
 *
 * @replacement	File to replace the current audit config file
 */
void replace_audit_config_file(char *replacement)
{
	run_sys_cmd("cp -f %s %s",replacement,get_audit_config_filename());
}

/**
 * Replace the current audit rules file with a file specificed by
 * @replacement.
 *
 * N.B. See replace_audit_config_file() for proper usage.
 *
 * @replacement	File to replace the current audit config file
 */
void replace_audit_rules_file(char *replacement)
{
	run_sys_cmd("cp -f %s %s",replacement,get_audit_rules_filename());
}


/*======================================================*/
/*		AUDIT RECORD ACTIONS			*/
/*======================================================*/
/**
 * Takes an allocated struct audit_record and sets the values to be
 * "initialized" so that when we go to translate this structure into
 * what the parser expects, we only need to set fields we care about
 * and don't need to know how to explicitly initialize the parser's
 * structure.
 *
 * @rec		An allocated struct audit_record
 */
void initialize_audit_record(struct audit_record *rec)
{
        rec->exit = VAL_NOT_SET;
	rec->success = VAL_NOT_SET;
	rec->syscallno = VAL_NOT_SET;
	rec->uid = VAL_NOT_SET;
	rec->user_login_uid = VAL_NOT_SET;
	rec->ruid = VAL_NOT_SET;
	rec->euid = VAL_NOT_SET;
	rec->suid = VAL_NOT_SET;
	rec->fsuid = VAL_NOT_SET;
	rec->auid = VAL_NOT_SET;
	rec->auid2 = VAL_NOT_SET;
	rec->gid = VAL_NOT_SET;
	rec->rgid = VAL_NOT_SET;
	rec->egid = VAL_NOT_SET;
	rec->sgid = VAL_NOT_SET;
	rec->fsgid = VAL_NOT_SET;
	rec->ppid = VAL_NOT_SET;
	rec->pid = VAL_NOT_SET;
	rec->mode = VAL_NOT_SET;
	rec->time = VAL_NOT_SET;
	rec->start_time = VAL_NOT_SET;
	rec->end_time = VAL_NOT_SET;
	memset(rec->objectname,0,OBJ_NAME_SIZE);
	memset(rec->objectname1,0,OBJ_NAME_SIZE);
	memset(rec->filterkey,0,WATCH_KEY_SIZE);
	rec->inode = VAL_NOT_SET;
	rec->perm = VAL_NOT_SET;
	rec->perm_mask = VAL_NOT_SET;

	memset(rec->comm,0,COMM_SIZE);
	memset(rec->exe,0,EXE_SIZE);
	memset(rec->data,0,DATA_SIZE);
	memset(rec->auidmsg,0,AUIDMSG_SIZE);
	rec->argv[0] = VAL_NOT_SET;
	rec->argv[1] = VAL_NOT_SET;
	rec->argv[2] = VAL_NOT_SET;
	rec->argv[3] = VAL_NOT_SET;
	rec->audit_type = VAL_NOT_SET;
	rec->arch = VAL_NOT_SET;
	memset(rec->subj,0,SUBJ_SIZE);
	memset(rec->obj,0,SUBJ_SIZE);
	memset(rec->device,0,DEV_SIZE);
	memset(rec->desc,0,AUIDMSG_SIZE);
	rec->curr_val = VAL_NOT_SET;
	rec->old_val = VAL_NOT_SET;
	memset(rec->bool_name,0,BOOL_SIZE);
	memset(rec->name,0,NAME_SIZE);
	memset(rec->scontext,0,SUBJ_SIZE);
	memset(rec->tcontext,0,SUBJ_SIZE);
	memset(rec->tclass,0,TCLASS_SIZE);
	memset(rec->old_cntx,0,SUBJ_SIZE);
	memset(rec->new_cntx,0,SUBJ_SIZE);
	memset(rec->sel_cntx,0,SUBJ_SIZE);
	memset(rec->def_cntx,0,SUBJ_SIZE);
	memset(rec->old_user,0,USER_SIZE);
	memset(rec->old_role,0,ROLE_SIZE);
	memset(rec->old_range,0,RANGE_SIZE);
	memset(rec->new_user,0,USER_SIZE);
	memset(rec->new_role,0,ROLE_SIZE);
	memset(rec->new_range,0,RANGE_SIZE);

	rec->login_data.result = VAL_NOT_SET;
	rec->login_data.id = VAL_NOT_SET;
	memset(rec->login_data.user,0,LOGIN_USER_SIZE);
	memset(rec->login_data.host,0,LOGIN_HOST_SIZE);
	memset(rec->login_data.address,0,LOGIN_ADDRESS_SIZE);
	memset(rec->login_data.terminal,0,LOGIN_TERM_SIZE);
	memset(rec->login_data.acct,0,LOGIN_ACCT_SIZE);
	memset(rec->login_data.subj,0,SUBJ_SIZE);
	rec->login_data.res = VAL_NOT_SET;
	memset(rec->login_data.msg,0,AUIDMSG_SIZE);
	memset(rec->login_data.op,0,LOGIN_OP_SIZE);

	rec->res = VAL_NOT_SET;
	rec->cipso_doi = VAL_NOT_SET;
	rec->unlbl_accept = VAL_NOT_SET;
	memset(rec->nlbl_domain,0,DOM_SIZE);
	memset(rec->nlbl_protocol,0,PROTOCOL_SIZE);
	memset(rec->cipso_type,0,CIPSO_TYPE_SIZE);
	memset(rec->cipso_tool,0,TOOL_NAME_SIZE);

	rec->sec_alg = VAL_NOT_SET;
	rec->sec_doi = VAL_NOT_SET;
	memset(rec->spi,0,SPI_SIZE);
	memset(rec->src,0,LOGIN_HOST_SIZE);
	memset(rec->dst,0,LOGIN_HOST_SIZE);
	memset(rec->protocol,0,PROTOCOL_SIZE);

	rec->job = VAL_NOT_SET;
	memset(rec->acct,0,LOGIN_ACCT_SIZE);
	memset(rec->title,0,TITLE_SIZE);
	memset(rec->label,0,LABEL_SIZE);
	memset(rec->printer,0,PRINTER_SIZE);
	
}

/**
 * This links our test core (audit_utils.c and related files) into
 * our backend hookup to the parsing API. This will handle things
 * like translation.
 *
 * @rec		struct audit_record filled out with search criteria
 * @return	Zero on success, non-zero otherwise
 */
int verify_record_existence(struct audit_record *rec)
{
	return parse_for_record(rec);
}

/*======================================================*/
/*		GENERIC FUNCTIONAL SUPPORT		*/
/*======================================================*/
/**
 * Start the audit daemon and allow for a sleep grace period
 * to ensure that the daemon is ready before proceeding.
 */
void begin_test()
{
	start_audit();
	sleep(1);
}

/**
 * Stop the audit daemon after waiting for a small time. This is
 * done to ensure that the records that we generated have ample
 * time to be retrieved from the queue before the daemon stops.
 */
void end_test()
{
	sleep(1);
	stop_audit();
}

/**
 * Returns the auid (loginuid) of the current process, which should
 * also be the auid of the current session, and therefore, the initial
 * uid used to login with (which is the auid (aka loginuid)).
 *
 * @return	auid (aka loginuid) of the current process.
 */
uid_t get_auid()  
{
	uid_t auid;
	int fd, rc, max = 15;
	char *filename = "/tmp/current_auid";
	char buf[max+1];

	memset(buf,0,max);
	rc = run_sys_cmd("cat /proc/%d/loginuid > %s",getpid(),filename);
	if (!rc) {
		fd = open(filename,O_RDONLY);
		if (fd != -1) {
			read(fd,buf,max);
			close(fd);
			sscanf(buf,"%u",&auid);
			unlink(filename);
			return auid;
		}
	}
	unlink(filename);
	return VAL_NOT_SET;
}

/**
 * This is a wrapper for the system command, which will construct an arbitrary
 * length shell command from the input provided. This will also check the
 * return code of the system call, and will output an error system()'s failure.
 * The return code of the shell call made will be returned upon failure.
 *
 * N.B.	It is assumed that a return from the shell command of non-zero warrents
 *	user notification. This does not _always_ indicate failure, but it is
 *	probable that a failure occured.
 *
 * @fmt		Character string formatted like that of printf
 * @...		Values to be used in the formatted string
 * @return	0 on success, non-zero otherwise (return code from the command)
 */
#define COMM_BUF_SIZE 4096
int run_sys_cmd(const char *fmt, ...)
{
	int rc;
	va_list args;
	char buf[COMM_BUF_SIZE];

	va_start(args, fmt);
	rc = vsnprintf(buf,COMM_BUF_SIZE,fmt,args);
	if (rc >= COMM_BUF_SIZE) {
		tst_resm(TINFO, "Unable to construct sys command: no space");
		return -E2BIG;
	} else if (rc < 0) {
		tst_resm(TINFO, "Error occured while constructing sys command");
		return rc;
	}
	va_end(args);
	rc = system(buf);
	if (rc == -1) {
		tst_resm(TINFO, "Unable to perform the system command: "
				"fork failed");
	} else if (WEXITSTATUS(rc) != 0) {
#if !SUPPRESS_RUN_SYS_CMD_MSG
		tst_resm(TINFO, "System command [%s] returned with code rc=[%d]"
				"errno=[%d]",buf,rc,WEXITSTATUS(rc));
#endif
		rc = WEXITSTATUS(rc);
	}
	return rc;
}

/**
 * Ensure that the results are either supposed to be, or conversely,
 * not supposed to be, found depending on the value of fields->action
 *
 * N.B. This call will determine the success of your assertion, use
 *	only when this test makes sense for your test.
 *
 * @fields	The audit_rule_fields structure used to create the current
 *		audit rule.
 */
void check_results(struct audit_rule_fields *fields)
{
	/* For action never entry/exit rules record is not created.*/
	if ( (fields->action) && (strcmp(fields->action,"never") == 0) ) {
		if (!TEST_RETURN) {
			tst_resm(TFAIL, "Unexpected record found with never "
					"rule.");
		} else {
			tst_resm(TPASS, "Record not found with never rule as "
					"expected.");
		}
	}  else  {
		if (TEST_RETURN) {
			tst_resm(TFAIL, "Expected record not found.");
		} else {
			tst_resm(TPASS, "Expected record found!");
		}
	}
}

/**
 * Wrapper for check_results
 */
void report_results(int rc, struct audit_rule_fields *fields)
{
	TEST_RETURN = rc;
	check_results(fields);
}

/**
 * Determine if we are in a CAPP environment.
 * Return 1 for true, 0 for false
 */
int is_capp_environment()
{
	if (getenv("CAPP") != NULL && (!strcmp(getenv("CAPP"), "ON")) ) {
#if !SUPPRESS_VERBOSE_OUTPUT
		printf("CAPP ON - Checking auid \n");
#endif
		return 1;
	} else {
#if !SUPPRESS_VERBOSE_OUTPUT
		printf("CAPP OFF - Not checking auid\n");
		printf("Warning: In order to test auid, you must export CAPP=ON\n");
#endif
		return 0;
	}
}

/**
 * Set the low and high pointers to the low and high levels in the given
 * mls range
 * N.B. This 'corrupts' the mls_range, as it inserts a '\0' into the
 *	middle of the range
 */
void split_mls_range(char *mls_range, char **low, char **high)
{
        char *tmp = NULL;

        tmp = strstr(mls_range,"-");
	if (tmp) { /* we're splitting a range, like "s0-s15" */
		*low = mls_range;
		*tmp = '\0';
		*high = tmp+1;
	} else { /* no range, it looks like "s0" */
		*low = mls_range;
		*high = mls_range;
	}
#if !SUPPRESS_VERBOSE_OUTPUT
        printf("Low: %s || High: %s\n", *low, *high);
#endif
}


/*======================================================*/
/*		TEST USER CREATION MECHANISM		*/
/*======================================================*/
/**
 * Adds a test user to the system and returns a structure which has the
 * new user's name, uid and passwords (good, bad and ecrypted). The caller
 * can also specify groups which the user can be added to. Currently, only
 * WHEEL (-G wheel) is defined.
 *
 * N.B. If gid is needed, we can add this to the structure later and read it in
 *	from the /etc/passwd file.
 *
 * @group	The group toggle, currently only controls -G wheel
 * @return	malloc'd memory. User should pass this to delete_test_user()
 *		where it will be free'd. Use this for calls to switch_to_X_user
 */
#define PLAINTEXT "eal"
#define ENCRYPTED "42VmxaOByKwlA"
#define BADPASSWD "noteal"
struct test_user_data *create_test_user(int group)
{
	int rc, test_user_uid;
	char *test_user_name;
	char *test_user_home;
	struct passwd *passwd_data;
	struct test_user_data *user;

	user = (struct test_user_data*)malloc(sizeof(struct test_user_data));
	if (!user) {
		tst_resm(TINFO,"Error allocating space for test_user_data");
		return NULL;
	}
	memset(user,0,sizeof(struct test_user_data));
	strncpy(user->passwd, PLAINTEXT, TEST_USER_DATA_PASSWD_SIZE);
	strncpy(user->encrypted, ENCRYPTED, TEST_USER_DATA_ENCRYPTED_SIZE);
	strncpy(user->badpasswd, BADPASSWD, TEST_USER_DATA_PASSWD_SIZE);

	rc = createTempUserName(&test_user_name, &(test_user_uid),
				&test_user_home);
	strncpy(user->name,test_user_name,TEST_USER_DATA_NAME_SIZE);
	user->uid = test_user_uid;
	if (rc) {
		tst_resm(TINFO, "Out of temporary test user names\n");
	} else {
		if (group == WHEEL) {
			rc = run_sys_cmd("/usr/sbin/useradd -u %d -d %s -m -G "
					 "wheel -p %s %s", user->uid,
					 test_user_home, user->encrypted,
					 user->name);
		} else {
			rc = run_sys_cmd("/usr/sbin/useradd -u %d -d %s -m -p "
					 "%s %s", user->uid, test_user_home,
					 user->encrypted, user->name);
		}
		if (rc) {
			tst_resm(TINFO, "Error creating test user [%s]\n",
					user->name);
		}
	}
	passwd_data = getpwnam(user->name);
	if (!passwd_data) {
		tst_resm(TINFO, "Error getting test user [%s] gid",user->name);
		user->gid = VAL_NOT_SET;
	} else {
		user->gid = passwd_data->pw_gid;
	}
	free(test_user_name);
	free(test_user_home);
	return user;
}

/**
 * The switch to the new user is done only by masking the current user's
 * effective id's with the new user's ids. This means that the groups
 * to which the original user belongs are still valid, despite the change.
 * The id's used to switch with are extracted the test user's information
 * as stored in /etc/passwd.
 *  
 * N.B. This is not the equivalent of "su". This switch does not
 *	remove the user from other groups (such as wheel and root group)
 * N.B. The new user's /etc files do not contain this change, so a
 *	/bin/su new_user will not permit group root(0) access.
 *
 * Example:
 *	root belongs to root(0), wheel(10), etc... if switch_to_test_user()
 *	is called as root, the effective user id & group id will be that of
 *	the new test user, but he will also still belong to root's groups
 *	i.e. root(0),wheel(10), etc...
 *
 * @user		user in question to switch to
 * @audit_record	struct audit_record to fill in with uid information
 *			can be NULL if information is irrelevant
 * @return		Zero on success, non-zero otherwise
 */
int switch_to_test_user(struct test_user_data *user,
			struct audit_record *audit_record)
{
	int rc;

	rc = setresgid(user->gid, user->gid, 0); 
	if (rc) {
		tst_resm(TINFO, "Unable to set GID to test user [%s] :- [%d]",
				user->name, errno);
		return rc;
	}
	rc = setresuid(user->uid, user->uid, 0);
	if (rc) { 
		tst_resm(TINFO, "Unable to set UID to test user [%s] :- [%d]",
				user->name, errno);
		return rc;
	}
	if (audit_record) {
		audit_record->uid = user->uid;
		audit_record->gid = user->gid;
		audit_record->euid = user->uid;
		audit_record->egid = user->gid;
		audit_record->ruid= user->uid;
		audit_record->rgid= user->gid;
	}
	return rc;
}

/**
 * Function only changes the effective IDs of the current user to root.
 * by setting back the effective user ID and group ID to root
 *
 * N.B. This change does not the user's associated groups
 *
 * @audit_record	struct audit_record to fill in with uid information
 *			can be NULL if information is irrelevant
 * @return		Zero on success, non-zero otherwise
 */
int switch_to_super_user(struct audit_record *audit_record)
{
	int rc;

	rc = setresuid(0,0,0);
	if (rc) {
		tst_resm(TINFO, "Unable to set UID to root:- [%i]", errno);
		return rc;
	}
	rc = setresgid(0,0,0);
	if (rc) {
		tst_resm(TINFO, "Unable to set GID to root:- [%i]", errno);
		return rc;
	}
	if (audit_record) {
		audit_record->uid = 0;
		audit_record->gid = 0;
		audit_record->euid = 0;
		audit_record->egid = 0;
		audit_record->ruid = 0;
		audit_record->rgid = 0;
	}
	return rc;
}

/**
 * This function uses /usr/sbin/userdel to remove the test user's entries in
 * /etc/group, /etc/passwd, and its /home directory contents.
 * It will also free the allocated memory associated with the test_user
 * variable.
 *
 * @user	user in question to delete, will free this memory
 * @return	Zero on success, non-zero otherwise
 */
int delete_test_user(struct test_user_data *user)
{
	int rc;

	switch_to_super_user(NULL);
	rc = run_sys_cmd("/usr/sbin/userdel -r %s",user->name);
	if (rc) {
		tst_resm(TINFO, "Error deleting test user [%s]\n", user->name);
	}
	rc = run_sys_cmd("/usr/sbin/groupdel %s",user->name);
	if (rc) {
		tst_resm(TINFO, "Error deleting test user's group [%s]\n",
				user->name);
	}
	free(user);
	return rc;
}


/*======================================================*/
/*		TEMPORARY OBJECT SUPPORT		*/
/*======================================================*/
/**
 * This defines a utility function that creates a file.
 * Some test cases must operate on an existing file.  It is this function that
 * guarantees that such files exist on the system before the system call is
 * issued by creating the file at runtime.  This function uses mkstemp() to
 * create a temporary file at runtime.
 *
 * N.B. malloc()'d memory for data will be returned to the caller.
 *	Caller must free() this memory.
 *
 * @fname	malloc'd memory will be linked to this, user must free
 * @return	Zero on success, non-zero otherwise
 */
int createTempFile(char **fname, mode_t mode, uid_t uid, gid_t gid)
{
	int rc = 0;
	int fd = 0;
	char *writedata = "This is bogus test file content\n";

	if (!fname) {
		return -1;
	}
	*fname = (char *)malloc(strlen(tempname)+1);
	if (!*fname) {
		tst_resm(TINFO, "Unable to allocate a temporary string");
		return -1;
	}
	strcpy(*fname, tempname);

	fd = mkstemp(*fname);
	if (fd == -1) {
		tst_resm(TINFO, "ERROR: Unable to create %s: errno=%i\n",
				*fname, errno);
		rc = fd;
		goto out;
	}
	rc = write(fd, writedata, strlen(writedata));
	if (rc == -1) {
		tst_resm(TINFO, "ERROR: Unable to write data to file %s: "
				"errno=%i\n", *fname, errno);
		goto out;
	}
	rc = close(fd);
	if (rc) {
		tst_resm(TINFO, "ERROR: Unable to close file %s: errno=%i\n",
				*fname,	errno);
		goto out;
	}
	rc = chmod(*fname, mode);
	if (rc) {
		tst_resm(TINFO, "ERROR: Unable to chmod file %s: errno=%i\n",
				*fname,	errno);
		goto out;
	}
	rc = chown(*fname, uid, gid);
	if (rc) {
		tst_resm(TINFO, "ERROR: Unable to chown file %s: errno=%i\n",
				*fname, errno);
		goto out;
	}
	return rc;
out:
	if (fd != -1) {
		unlink(*fname);
	}
	free(*fname);
	return rc;
}


/**
 * This utility function creates a unique filename. This function uses
 * mkstemp() to create a file, and then immediately deletes it,
 * thus momentarily freeing the filename.  Note that this is not a
 * perfect solution. Its possible, though hopefully unlikely, that
 * another file might be created with the same name in the time between
 * which the calling function requests a unique filename and the time
 * at which the calling function creates the file. This is basically
 * the same problem as using tempnam(). Risk is mitigated with a
 * distinct prefix on the file.  As long as the user understands this
 * small probability, the risk is likely acceptable.
 *
 * N.B. malloc()'d memory for data will be returned to the caller.
 *	Caller must free() this memory.
 *
 * @fname	malloc'd memory will be linked to this, user must free
 * @return	Zero on success, non-zero otherwise
 */
int createTempFileName(char **fname)
{
	int rc = 0;
	int fd = 0;

	if (!fname) {
		return -1;
	}
	*fname = (char *) malloc(strlen(tempname)+1);
	if (!*fname) {
		tst_resm(TINFO, "Unable to allocate a temporary string");
		return -1;
	}

	strcpy(*fname, tempname);
	fd = mkstemp(*fname);
	if (fd == -1) {
		tst_resm(TINFO, "ERROR: Unable to create %s: errno=%i\n",
				*fname, errno);
		return fd;
	}
	rc = close(fd);
	if (rc) {
		tst_resm(TINFO, "ERROR: Unable to close file %s: errno=%i\n",
				*fname,	errno);
		return rc;
	}
	rc = unlink(*fname);
	if (rc) {
		tst_resm(TINFO, "ERROR: Unable to remove file %s: errno=%i\n",
				*fname,	errno);
		return rc;
	}
	return rc;
}

/**
 * Generate unique group name and gid. This function will allocate the required
 * spare to store the newly generated group name. Allocations are returned only
 * if the funciton succeeded without failure. We now grow the names from Z->A 
 * to avoid possible collisions under RHEL4 with useradd / userdel & 
 * createTempUserName.
 *
 * N.B. malloc()'d memory for data will be returned to the caller.
 *	Caller must free() this memory.
 *
 * @group	malloc'd memory will be linked to this, user must free
 * @uid		gets set to new uid
 * @return	Zero on success, non-zero otherwise
 */
int createTempGroupName(char **group, int *gid)
{
	char *groupmask = "laf_";
	char c;

	if (!group) {
		return -1;
	}
	*group = (char *)malloc(strlen(groupmask) + 2);
	if (!*group) {
                tst_resm(TINFO, "Unable to allocate space for group name");
                return -1;
        }

	c = 'z';
	do {
		sprintf(*group, "%s%c", groupmask, c);
		c--;
	} while ((getgrnam(*group) != NULL) && (c >= 'a'));

	*gid = 500;
	while ((getgrgid(*gid) != NULL) && ((*gid) < 1000)) {
		(*gid)++;
	}

	if (((*gid) >= 1000) || (c <= 'a')) {
		free(*group);
		*group = NULL;
		return -1;
	}
	return 0;
}

/**
 * Generate unique user name, uid, homedir. This function attempts to generate
 * a unique username, user home directory, and uid. It will allocated the
 * memory necessary to store the user name and the home directory strings.
 * It is the responsibility of the caller to free() them when they are done.
 * Allocations are returned only if the funciton succeeded without failure.
 *
 * N.B. malloc()'d memory for data will be returned to the caller.
 *	Caller must free() this memory.
 *
 * @user	malloc'd memory will be linked to this, user must free
 * @uid		gets set to new uid
 * @homedir	malloc'd memory will be linked to this, user must free
 * @return	Zero on success, non-zero otherwise
 */
int createTempUserName(char **user, int *uid, char **homedir)
{
	char *base = "/home";
	char *usermask = "laf_";
	char c;

	*homedir = (char*)malloc(strlen(base) + strlen(usermask) + 3);
	if (!*homedir) {
                tst_resm(TINFO, "Unable to allocate space for homedir");
                return -1;
        }
	*user = (char*)malloc(strlen(usermask) + 2);
	if (!*user) {
                tst_resm(TINFO, "Unable to allocate space for user name");
		free(*homedir);
		*homedir=NULL;
                return -1;
        }

	c = 'a';
	do {
		sprintf(*user, "%s%c", usermask, c);
		sprintf(*homedir, "%s/%s%c", base, usermask, c);
		c++;
	} while ((getpwnam(*user) != NULL) && (c <= 'z'));

	*uid = 500;
	while ((getpwuid(*uid) != NULL) && (*uid < 1000)) {
		(*uid)++;
	}

	if ((*uid >= 1000) || (c > 'z')) {
		free(*user);
		*user = NULL;
		*uid = -1;
		free(*homedir);
		*homedir = NULL;
		return -1;
	}
	return 0;
}
