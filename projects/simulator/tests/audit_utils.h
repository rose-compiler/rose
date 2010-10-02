/*
 *   Copyright (C) International Business Machines  Corp., 2004,2005,2006,2007
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
 * File: audit_utils.h
 *
 * Overview:
 *      This file contains declarations and structures which are required by
 *	all audit tests to perform their functions.
 *
 ********************************** HISTORY ***********************************
 *
 * DATE         NAME                    DESCRIPTION
 * 01/03/2007	loulwa@us.ibm.com	modifications to ipsec related fields	
 * 09/20/2006	loulwa@us.ibm.com	modifications to mac related fields	
 * 09/09/2006	loulwa@us.ibm.com	Added new fields to audit structure
 * 08/16/2006	loulwa@us.ibm.com	Removed watch related fields
 * 08/08/2006	loulwa@us.ibm.com	Added new fields to audit structure
 * 07/17/2006	mcthomps@us.ibm.com	Added report_results()
 * 05/01/2006	loulwa@us.ibm.com	Addition of new fields
 * 01/16/2006	mcthomps@us.ibm.com	Added output control (SUPPRESS defines)
 *					with default values
 * 01/03/2006   loulwa@us.ibm.com       Added changes for watch tests 
 * 12/22/2005	loulwa@us.ibm.com	Added "mode" field to audit_record
 *					Added syscall_num audit rule field
 *					(needed by syscall 16 tests)
 *					Added inclusion of audit_read.h
 * 12/01/2005   mcthomps@us.ibm.com     Composite of old and new code
 *
 *****************************************************************************/

#ifndef _AUDIT_UTILS_H_
#define _AUDIT_UTILS_H_

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <time.h>
#include <values.h>

#include "includes.h"
#include "test.h"
#include "usctest.h"

#define SUPPRESS_COMPATABILITY_WARNINGS 0
#define SUPPRESS_VERBOSE_OUTPUT 0
#define SUPPRESS_AUDITD_OUTPUT 0
#define SUPPRESS_RUN_SYS_CMD_MSG 1

#define tempname "/tmp/lafXXXXXX"

#define MAX_PATH_DIR_SIZE	500
#define MAX_FILE_NAME_SIZE	100
#define MAX_ABS_PATH_SIZE	MAX_PATH_DIR_SIZE+MAX_FILE_NAME_SIZE
#define LOGIN_RESULT_SIZE	200
#define LOGIN_OP_SIZE	200
#define LOGIN_ACCT_SIZE	200
#define AUIDMSG_SIZE	500
#define SUBJ_SIZE	3000
#define DEV_SIZE        100
#define TCLASS_SIZE     50
#define NAME_SIZE	50
#define USER_SIZE	50
#define ROLE_SIZE	50
#define RANGE_SIZE	50
#define BOOL_SIZE	50
#define DOM_SIZE	50
#define PROTOCOL_SIZE	50
#define CIPSO_TYPE_SIZE	50
#define TOOL_NAME_SIZE	50
#define SPI_SIZE	50
#define PRINTER_SIZE	50
#define TITLE_SIZE	50
#define LABEL_SIZE	50
#define EXE_SIZE	300

/* Structure related to pam records */
struct login_information {
#define LOGIN_SUCCESS	1
#define LOGIN_FAIL	2
#define LOGIN_EXPIRED	3
        int result;
        unsigned long id;
#define LOGIN_USER_SIZE 50
        char user[LOGIN_USER_SIZE];
#define LOGIN_HOST_SIZE 50
        char host[LOGIN_HOST_SIZE];
#define LOGIN_ADDRESS_SIZE 50
        char address[LOGIN_ADDRESS_SIZE];
#define LOGIN_TERM_SIZE 50
        char terminal[LOGIN_TERM_SIZE];
	char op[LOGIN_OP_SIZE];
	int res;
	char acct[LOGIN_ACCT_SIZE];
	char subj[SUBJ_SIZE];
	char msg[AUIDMSG_SIZE];
	char exe[EXE_SIZE];
};

/**
 * TODO: Do we need more than this? I remember some things were missed because
 * the tests we not strict enough... does this mean we need to set them
 * to be more strict, or are we not worried? etc etc.
 */
#define VAL_NOT_SET	(MININT)
struct audit_record {
	/* User set values */
#define TYPE_SYSCALL	1
#define TYPE_USER_ACCT	2
#define TYPE_USER_AUTH	3
#define TYPE_USER_START	4
#define TYPE_USER	5
#define TYPE_USYS_CONFIG 6
#define TYPE_USER_CHAUTHTOK 7
#define TYPE_CONFIG_CHANGE 8
#define TYPE_MAC_POLICY_LOAD 9
#define TYPE_DEV_ALLOC 10
#define TYPE_DEV_DEALLOC 11
#define TYPE_AUDIT_USYS_CONFIG 12
#define TYPE_RBAC 13
#define TYPE_AVC 14
#define TYPE_MAC 15
#define TYPE_USER_ROLE_CHANGE 16
#define TYPE_MAC_CONFIG 17
#define TYPE_USER_LOGIN 18
#define TYPE_CIPSO_ADD 19
#define TYPE_CIPSO_DEL 20
#define TYPE_MAP_ADD 21
#define TYPE_MAP_DEL 22
#define TYPE_UNLBL_ALLOW 23
#define TYPE_IPSEC_ADDSA 24
#define TYPE_IPSEC_DELSA 25
#define TYPE_IPSEC_ADDSPD 26
#define TYPE_IPSEC_DELSPD 27
#define TYPE_USER_LABELED_EXPORT 28
#define TYPE_LABEL_OVERRIDE 29
	int audit_type;
	int exit;
#define SUCCESS_YES	1
#define SUCCESS_NO	2
	int success;
	int syscallno;
	uid_t uid, ruid, euid, suid, fsuid, auid, auid2;
	uid_t user_login_uid;
	gid_t gid, rgid, egid, sgid, fsgid;
	pid_t ppid, pid;
	time_t time;
	time_t start_time, end_time;

/* path type related fields */
#define OBJ_NAME_SIZE	300
	char objectname[OBJ_NAME_SIZE];
	char objectname1[OBJ_NAME_SIZE];
	unsigned long inode;
	int perm;
	int perm_mask;
	int res;

#define COMM_SIZE	300
	char comm[COMM_SIZE];
	char exe[EXE_SIZE];
#define DATA_SIZE	2000
	char data[DATA_SIZE];
	char auidmsg[AUIDMSG_SIZE];
	unsigned long argv[4];
	unsigned long mode;
	char subj[SUBJ_SIZE];
	char def_cntx[SUBJ_SIZE];
	char sel_cntx[SUBJ_SIZE];
	char old_cntx[SUBJ_SIZE];
	char new_cntx[SUBJ_SIZE];
	char old_user[USER_SIZE];
	char old_role[ROLE_SIZE];
	char old_range[RANGE_SIZE];
	char new_user[USER_SIZE];
	char new_role[ROLE_SIZE];
	char new_range[RANGE_SIZE];
	char obj[SUBJ_SIZE];
	char device[DEV_SIZE];
	char desc[AUIDMSG_SIZE];

#define WATCH_KEY_SIZE	100
	char filterkey[WATCH_KEY_SIZE];

	/* Used for MAC_* record type */
	char bool_name[BOOL_SIZE];
	int curr_val;
	int old_val;

	struct login_information login_data;
	char name[NAME_SIZE];
	char scontext[SUBJ_SIZE];
	char tcontext[SUBJ_SIZE];
	char tclass[TCLASS_SIZE];

/* cipso related fields */
	int cipso_doi;
	int unlbl_accept;
	char nlbl_domain[DOM_SIZE];
	char nlbl_protocol[PROTOCOL_SIZE];
	char cipso_type[CIPSO_TYPE_SIZE];
	char cipso_tool[TOOL_NAME_SIZE];

/* ipsec related fields */
	int sec_alg;
	int sec_doi;
	char src[LOGIN_HOST_SIZE];
	char dst[LOGIN_HOST_SIZE];
	char spi[SPI_SIZE];
	char protocol[PROTOCOL_SIZE];

/* cups related records */
	int job;
	char acct[LOGIN_ACCT_SIZE];
	char title[TITLE_SIZE];
	char label[LABEL_SIZE];
	char printer[PRINTER_SIZE];
	/* Implied */
	int arch;
};

#define HEAD_NO 0
#define HEAD_YES 1
#define ARCH_NO_SET "NOSET"
struct audit_rule_fields {
	int list_head;
	char *list;
	char *action;

	char *watch;
	char *w_key;
	char *w_perms;

	char *syscall;
	int syscall_num;
	char *ppid;
	char *pid;
	char *uid;
	char *euid;
	char *suid;
	char *fsuid;
	char *gid;
	char *egid;
	char *sgid;
	char *fsgid;
	char *auid;
	char *exit;
	char *success;
	char *arg0;
	char *arg1;
	char *arg2;
	char *arg3;

	char *arch;
	char *pers;
	char *devmajor;
	char *devminor;
	char *inode;

	char *msgtype;
	char *subj_user;
	char *subj_role;
	char *subj_type;
	char *subj_sen;
	char *subj_clr;
	char *obj_user;
	char *obj_role;
	char *obj_type;
	char *obj_lev_low;
	char *obj_lev_high;
	char *path;
};

#define TEST_USER_DATA_NAME_SIZE	6
#define TEST_USER_DATA_PASSWD_SIZE	8
#define TEST_USER_DATA_ENCRYPTED_SIZE	14
struct test_user_data {
	char name[TEST_USER_DATA_NAME_SIZE];
	uid_t uid;
	gid_t gid;
	char passwd[TEST_USER_DATA_PASSWD_SIZE];
	char encrypted[TEST_USER_DATA_ENCRYPTED_SIZE];
	char badpasswd[TEST_USER_DATA_PASSWD_SIZE];
};

#include "audit_parse_verify.h"


/*======================================================*/
/*		Common Audit Utilities Header		*/
/*======================================================*/
/* setup */
void general_setup(char *, void (*cleanup)());
void general_cleanup(char *);

/* auditd manipulation */
int restart_audit();
int start_audit();
int stop_audit();
void pause_for_auditd();

/* audit rule manipulation */
struct audit_rule_fields* alloc_init_audit_fields();
int add_audit_rule(struct audit_rule_fields *fields);
int delete_audit_rule(struct audit_rule_fields *fields);
int remove_all_audit_rules();
void save_active_audit_rules(char *filename);
int remove_watch(char *, char *);

/* log manipulation */
const char *get_syslog_filename();
const char *get_audit_log_filename();
void clear_syslog();
void clear_audit_log();
void delete_audit_log();
void save_audit_log(char *testname, char *header);
void save_and_clear_audit_log(char *testname, char *header);
void grab_and_save_syslog_lines(int lines, char *filename);
void grab_and_save_audit_log_lines(int lines, char *filename);
int check_audit_logs_exist(int limit);

/* config file manipulation */
char *get_audit_config_filename();
char *get_audit_rules_filename();
int get_num_logs_limit();
void save_audit_config_file();
void save_audit_rules_file();
void restore_audit_config_file();
void restore_audit_rules_file();
void replace_audit_config_file(char *replacement);
void replace_audit_rules_file(char *replacement);

/* audit_record actions */
void initialize_audit_record(struct audit_record *rec);
int verify_record_existence(struct audit_record *rec);

/* generic functional support */
void begin_test();
void end_test();
uid_t get_auid();
int run_sys_cmd(const char *fmt, ...);
void check_results(struct audit_rule_fields *fields);
void report_results(int ret, struct audit_rule_fields *fields);
int is_capp_environment();
void split_mls_range(char *mls_range, char **low, char **high);

/* test user creation mechanism */
#define NONE 0
#define WHEEL 1
struct test_user_data *create_test_user(int group);
int switch_to_test_user(struct test_user_data *user,
			struct audit_record *audit_record);
int switch_to_super_user(struct audit_record *audit_record);
int delete_test_user(struct test_user_data *user);

/* temporary object support */
int createTempFile(char **fname, mode_t mode, uid_t uid, gid_t gid);
int createTempFileName(char **fname);
int createTempGroupName(char **group, int *gid);
int createTempUserName(char **user, int *uid, char **homedir);

#endif
