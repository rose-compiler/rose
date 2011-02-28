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
 * File: audit_parse_verify.h
 *
 * Overview:
 *      This file defines the interface to get audit records.
 *
 ********************************** HISTORY ***********************************
 *
 * DATE         NAME                    DESCRIPTION
 * 01/03/2007	loulwa@us.ibm.com	added ipsec struct 
 * 09/20/2006	loulwa@us.ibm.com	modifications to mac struct 
 * 09/19/2006	loulwa@us.ibm.com	Added new audit type structs
 * 08/16/2006	loulwa@us.ibm.com	remved watch related struct
 * 07/15/2006	loulwa@us.ibm.com	Added new audit type structs
 * 05/01/2006	loulwa@us.ibm.com	Addition of new fields
 * 12/01/2005   mcthomps@us.ibm.com     Updated header
 * 01/17/2005	loulwa@us.ibm.com	Originated
 *
 *****************************************************************************/

#ifndef _AUDIT_PARSE_VERIFY_H
#define	_AUDIT_PARSE_VERIFY_H 

#include "audit_utils.h"

#define LOG_TIME_VARIANCE 1

struct audit_record;

#define AUDIT_SIZE      1000
#define AUDIT_TYPE_SIZE     30 
#define SUCCESS_SIZE	20

/** Used in the initialize function **/
#define INIT_SYSCALL    1
#define INIT_PATH       2
#define INIT_CONFIG     3
#define INIT_USER       4
#define INIT_CHAUTH     5
#define INIT_LOAD_POL   6
#define INIT_RBAC       7 
#define INIT_DEV_USYS   8 
#define INIT_AVC        9
#define INIT_MAC        10
#define INIT_ROLE_CHANGE   11
#define INIT_CIPSO      12 
#define INIT_IPSEC      13 
#define INIT_CUPS      14 

/** audit record structures **/
typedef struct syscall {
    int syscall_no;
    unsigned long arch;
    char success[SUCCESS_SIZE];
    int exit;
    int item;
    int ppid, pid;
    int auid;
    unsigned long argv[4];
    int uid, gid, euid, suid, fsuid, egid, sgid, fsgid;
    char comm[COMM_SIZE];
    char exe[EXE_SIZE];
    char subj[SUBJ_SIZE];
    char key[WATCH_KEY_SIZE];
} syscall_t;

typedef struct path {
    char name[OBJ_NAME_SIZE];
    char obj[SUBJ_SIZE];
    int flags;
    unsigned long parent;
    unsigned long inode;
    char dev[10];
    char rdev[10];
    unsigned long mode;
    int ouid, ogid;
} path_t;

typedef struct config {
    int audit_en;
    int old;
    int auid;
    int res;
    char msg[AUIDMSG_SIZE];
    char subj[SUBJ_SIZE];
} config_t;
    
typedef struct user_rec {
    int uid;
    int id;
    int user_login_uid;
    int auid;
    int pid;
    char op[LOGIN_OP_SIZE];
    char msg[AUIDMSG_SIZE];
    char subj[SUBJ_SIZE];
    char exe[EXE_SIZE];
    char user[LOGIN_USER_SIZE];
    char host[LOGIN_HOST_SIZE];
    char addr[LOGIN_ADDRESS_SIZE];
    char terminal[LOGIN_TERM_SIZE];
    char result[LOGIN_RESULT_SIZE];
} user_t;

typedef struct user_chauth {
    int uid;
    int auid;
    int pid;
    long int id;
    char subj[SUBJ_SIZE];
    char msg[AUIDMSG_SIZE];
    char op[LOGIN_OP_SIZE];
    char exe[EXE_SIZE];
    char acct[LOGIN_ACCT_SIZE];
    char host[LOGIN_HOST_SIZE];
    char addr[LOGIN_ADDRESS_SIZE];
    char terminal[LOGIN_TERM_SIZE];
    char res[LOGIN_RESULT_SIZE];
} user_chauth_t;

typedef struct load_pol {
    int auid;
    char auidmsg[AUIDMSG_SIZE];
} load_pol_t;

typedef struct rbac {
    int uid;
    int auid;
    int pid;
    char subj[SUBJ_SIZE];
    char msg[AUIDMSG_SIZE];
    char exe[EXE_SIZE];
    char host[LOGIN_HOST_SIZE];
    char addr[LOGIN_ADDRESS_SIZE];
    char terminal[LOGIN_TERM_SIZE];
    char res[LOGIN_RESULT_SIZE];
} rbac_t;

typedef struct audit_dev_usys {
    int uid;
    int auid;
    int pid;
    char msg[AUIDMSG_SIZE];
    char op[LOGIN_OP_SIZE];
    char dev[DEV_SIZE];
    char obj[SUBJ_SIZE];
    char desc[AUIDMSG_SIZE];
} dev_usys_t;

typedef struct audit_avc {
    int pid;
    int ino;
    char desc[AUIDMSG_SIZE];
    char comm[COMM_SIZE];
    char dev[DEV_SIZE];
    char name[NAME_SIZE];
    char scontext[SUBJ_SIZE];
    char tcontext[SUBJ_SIZE];
    char tclass[TCLASS_SIZE];
} avc_t;

typedef struct audit_mac {
    char bool_name[BOOL_SIZE];
    int auid;
    int curr_val;
    int old_val;
} mac_t;

typedef struct audit_user_role_change {
    int pid;
    int uid;
    int auid;
    char msg[AUIDMSG_SIZE];
    char op[LOGIN_OP_SIZE];
    char subj[SUBJ_SIZE];
    char acct[LOGIN_ACCT_SIZE];
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
    char exe[EXE_SIZE];
    char host[LOGIN_HOST_SIZE];
    char addr[LOGIN_ADDRESS_SIZE];
    char terminal[LOGIN_TERM_SIZE];
    char res[LOGIN_RESULT_SIZE];
} role_change_t;

typedef struct cipso_add_del {
    int auid;
    int cipso_doi;
    int res;
    int old;
    int unlbl_accept;
    char tool[TOOL_NAME_SIZE];
    char subj[SUBJ_SIZE];
    char nlbl_domain[DOM_SIZE];
    char nlbl_protocol[PROTOCOL_SIZE];
    char cipso_type[CIPSO_TYPE_SIZE];
} cipso_t;

typedef struct ipsec {
    int auid;
    int sec_alg;
    int sec_doi;
    int res;
    char desc[AUIDMSG_SIZE];
    char subj[SUBJ_SIZE];
    char sec_obj[SUBJ_SIZE];
    char src[LOGIN_HOST_SIZE];
    char dst[LOGIN_HOST_SIZE];
    char spi[SPI_SIZE];
    char protocol[PROTOCOL_SIZE];
} ipsec_t;

typedef struct cups {
    int pid;
    int uid;
    int auid;
    int auid2;
    int job;
    char acct[LOGIN_ACCT_SIZE];
    char printer[PRINTER_SIZE];
    char title[TITLE_SIZE];
    char label[LABEL_SIZE];
    char subj[SUBJ_SIZE];
    char obj[SUBJ_SIZE];
    char exe[EXE_SIZE];
    char host[LOGIN_HOST_SIZE];
    char addr[LOGIN_ADDRESS_SIZE];
    char terminal[LOGIN_TERM_SIZE];
    char res[LOGIN_RESULT_SIZE];
    //char job_sheet[JOB_SHEET_SIZE];
    //char banners[BANNER_SIZE];
    char msg[AUIDMSG_SIZE];
} cups_t;

union data_t
{
        syscall_t syscall;
        path_t path;
	config_t config;
	user_t user;
	user_chauth_t chauthtok;
	load_pol_t load_pol;
	rbac_t rbac;
	dev_usys_t dev_usys;
	avc_t avc;
	mac_t mac;
	role_change_t role_chng;
	cipso_t cipso;
	ipsec_t ipsec;
	cups_t cups;
};

typedef struct record_t
{
        char type[AUDIT_TYPE_SIZE];
        union data_t data;
        struct record_t *next_part;
} rec_part;

typedef struct main_list_t
{
        unsigned long rec_time;
        unsigned long rec_serial;
        rec_part *myrecord;
        struct main_list_t *next_record;
} main_list;

/*********  FUNCTIONS *********/

int parse_for_record(struct audit_record *);
main_list* append_list (main_list *, main_list *);
rec_part* append_record (main_list **, rec_part *);
main_list* search_for_serial(main_list *, unsigned long);
void print_list(rec_part *);
void cleanup_mem(main_list **);
main_list* parse_audit_log(time_t , time_t , int *);
char *extract_value(char *);
char *extract_special(char *);
unsigned long extract_time(char *);
unsigned long extract_serial(char *);
void initialize_rec_part(rec_part *, int);
main_list* translate_for_parser(struct audit_record *);
int compare_rec(main_list *, main_list *);
int search_for_record(main_list *, time_t , main_list *, time_t );

#endif
