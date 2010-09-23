/*
 *   Copyright International Business Machines  Corp., 2006, 2007
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
 *******************************************************************************
 * File: audit_parse_verify.c
 *
 * Overview:
 *
 * Description:
 *   This file contains all the code to parse the audit log, as well as verify
 *   the existance of record expected by test case.
 *
 ********************************** HISTORY ***********************************
 *
 * DATE         NAME                    DESCRIPTION
 * 04/06/07   loulwa@us.ibm.com	 fix parse_user to parse op= and id= 
 * 04/03/07   loulwa@us.ibm.com	 fix parse_user to ignore new not needed fields 
 * 12/21/07   loulwa@us.ibm.com	 added 2 fields to role_change record
 * 12/20/07   loulwa@us.ibm.com  remove '\n' from parsed tokens
 * 02/09/07   kylene@us.ibm.com	 fix for s390x 32 bit mode in verify of a3 arg. 
 * 02/09/07   loulwa@us.ibm.com	 fix in the parse_role_msg function
 * 02/08/07   fleury@br.ibm.com  translate_cups: mapped src->desc to cups.msg
 * 01/10/07   loulwa@us.ibm.com	 changes for cups record and minor fixes
 * 01/03/07   loulwa@us.ibm.com	 Added support for ipsec audit records
 * 12/06/06   loulwa@us.ibm.com	 added support for cipso audit records 
 * 09/20/06   loulwa@us.ibm.com	 changed *_mac functions to add MAC_CONFIG_CHANGE
 *				 fixed bug in parse_mac()
 * 09/19/06   loulwa@us.ibm.com	 Added support for user_role_change audit type
 * 08/23/06   loulwa@us.ibm.com	 Changes to handle not checking time range 
 *				 for disk handling tests
 * 08/22/06   loulwa@us.ibm.com	 Removed sample record from function headers.
 *				 Fixed some prints in tst_resm()
 * 08/16/06   loulwa@us.ibm.com	 Removed watch related functions
 * 08/14/06   loulwa@us.ibm.com	 Added support for AVC and MAC_STATUS types
 * 08/08/06   loulwa@us.ibm.com  Added support for additional audit record types
 * 05/01/06   loulwa@us.ibm.com  Changed to accommodate changes in audit
 *				record. Also changed compare_rec()
 * 02/01/06   loulwa@us.ibm.com	 Added CAPP check and code cleanup.
 * 01/17/06   loulwa@us.ibm.com  Originated
 *
 *****************************************************************************/

#include "audit_parse_verify.h"
#include "test.h"
#include "usctest.h"

#define BUFF_SIZE	4096
#define TIME_SIZE	100

/* Might not need those any more ...
extern int pam_test;
extern int trusted;
*/

const char *delim = " ";
char value[200];	/* used in extract_value() */

/*
 * This functions translates provided fields by test case into
 *   a syscall block
 *
 * Return: pointer to head of block
 */
rec_part *translate_syscall(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_syscall");
		return NULL;
	}
	initialize_rec_part(rec, INIT_SYSCALL);

	strncpy(rec->type, "SYSCALL", AUDIT_TYPE_SIZE);

	rec->data.syscall.syscall_no = src->syscallno;
	rec->data.syscall.arch = src->arch;
	rec->data.syscall.auid = src->auid;
	rec->data.syscall.ppid = src->ppid;
	rec->data.syscall.pid = src->pid;
	/* VAL_NOT_SET is a potentional value for a few exit, since it is
	 * includes negative numbers in its range of possible values.
	 * This can cause a problem of semantic abiguity here, but we need
	 * to assume that VAL_NOT_SET is the intended value.
	 * 02/17/2006 - VAL_NOT_SET is currently MININT, which should never
	 *		be a return code, so we should not see collisions.
	 */
	if ((src->exit < 0) && (src->exit != VAL_NOT_SET)) {
		rec->data.syscall.exit = src->exit*-1;
	} else {
		rec->data.syscall.exit = src->exit;
	}
	rec->data.syscall.uid = src->uid;
	rec->data.syscall.euid = src->euid;
	rec->data.syscall.suid = src->suid;
	rec->data.syscall.fsuid = src->fsuid;
	rec->data.syscall.gid = src->gid;
	rec->data.syscall.egid = src->egid;
	rec->data.syscall.sgid = src->sgid;
	rec->data.syscall.fsgid = src->fsgid;
	rec->data.syscall.argv[0] = src->argv[0];
	rec->data.syscall.argv[1] = src->argv[1];
	rec->data.syscall.argv[2] = src->argv[2];
	rec->data.syscall.argv[3] = src->argv[3];

	if (src->success == SUCCESS_YES)
		strncpy(rec->data.syscall.success, "yes", SUCCESS_SIZE);
	else if (src->success == SUCCESS_NO)
		strncpy(rec->data.syscall.success, "no", SUCCESS_SIZE);
	else
		memset(rec->data.syscall.success, 0, SUCCESS_SIZE);

	strncpy(rec->data.syscall.comm, src->comm,COMM_SIZE);
	strncpy(rec->data.syscall.exe, src->exe,EXE_SIZE);
	strncpy(rec->data.syscall.subj, src->subj,SUBJ_SIZE);
	strncpy(rec->data.syscall.key, src->filterkey,WATCH_KEY_SIZE);

	return rec;
}

rec_part *translate_path(struct audit_record *src, int arg)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_path");
		return NULL;
	}
	initialize_rec_part(rec, INIT_PATH);

	strncpy(rec->type, "PATH", AUDIT_TYPE_SIZE);
	rec->data.path.mode = src->mode;

	if (arg == 1) {
		strncpy(rec->data.path.name, src->objectname,OBJ_NAME_SIZE);
	} else { 
		strncpy(rec->data.path.name, src->objectname1,OBJ_NAME_SIZE);
	}
	strncpy(rec->data.path.obj, src->obj,SUBJ_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   a user block
 *
 * Return: pointer to head of block
 */
rec_part *translate_user(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_user");
		return NULL;
	}
	initialize_rec_part(rec, INIT_USER);

	if (src->audit_type == TYPE_USER_ACCT)
		strncpy(rec->type, "USER_ACCT", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_USER_AUTH)
		strncpy(rec->type, "USER_AUTH", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_USER_START)
		strncpy(rec->type, "USER_START", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_USER)
		strncpy(rec->type, "USER", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_USER_LOGIN)
		strncpy(rec->type, "USER_LOGIN", AUDIT_TYPE_SIZE);
	else
		strncpy(rec->type, "UNKNOWN", AUDIT_TYPE_SIZE);

	strncpy(rec->data.user.terminal, src->login_data.terminal, 
		LOGIN_TERM_SIZE);
	strncpy(rec->data.user.msg, src->login_data.msg, 
		AUIDMSG_SIZE);
	strncpy(rec->data.user.user, src->login_data.user, LOGIN_USER_SIZE);
	strncpy(rec->data.user.addr, src->login_data.address, 
		LOGIN_ADDRESS_SIZE);
	strncpy(rec->data.user.host, src->login_data.host, LOGIN_HOST_SIZE);
	rec->data.user.auid = src->auid;
	rec->data.user.pid = src->pid;
	rec->data.user.uid = src->uid;
	rec->data.user.user_login_uid = src->user_login_uid;
	strncpy(rec->data.user.subj, src->login_data.subj, SUBJ_SIZE);
	strncpy(rec->data.user.exe, src->login_data.exe, EXE_SIZE);

	if (src->login_data.result == LOGIN_SUCCESS)
		strncpy(rec->data.user.result, "success", LOGIN_RESULT_SIZE);
	else if (src->login_data.result == LOGIN_FAIL)
		strncpy(rec->data.user.result, "failed", 
			LOGIN_RESULT_SIZE);
	else if (src->login_data.result == LOGIN_EXPIRED)
		strncpy(rec->data.user.result, "User account has expired", 
			LOGIN_RESULT_SIZE);
	else
		memset(rec->data.user.result, 0, LOGIN_RESULT_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   a config block
 *
 * Return: pointer to head of block
 */
rec_part *translate_config(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_config");
		return NULL;
	}
	initialize_rec_part(rec, INIT_CONFIG);

	strncpy(rec->type, "CONFIG_CHANGE", AUDIT_TYPE_SIZE);
	strncpy(rec->data.config.msg, src->auidmsg, AUIDMSG_SIZE);
	strncpy(rec->data.config.subj, src->subj, SUBJ_SIZE);
	rec->data.config.auid = src->auid;

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   a user_chauthtok block
 *
 * Return: pointer to head of block
 */
rec_part *translate_chauthtok(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_chauthtok");
		return NULL;
	}
	initialize_rec_part(rec, INIT_CHAUTH);

	if (src->audit_type == TYPE_USER_CHAUTHTOK)
		strncpy(rec->type, "USER_CHAUTHTOK", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_USYS_CONFIG)
		strncpy(rec->type, "USYS_CONFIG", AUDIT_TYPE_SIZE);
	else
		strncpy(rec->type, "UNKNOWN", AUDIT_TYPE_SIZE);

	rec->data.chauthtok.auid = src->auid;
	rec->data.chauthtok.pid = src->pid;
	rec->data.chauthtok.uid = src->uid;
	rec->data.chauthtok.id = src->login_data.id;
	strncpy(rec->data.chauthtok.op, src->login_data.op, LOGIN_OP_SIZE);
	strncpy(rec->data.chauthtok.acct, src->login_data.acct,LOGIN_ACCT_SIZE);
	strncpy(rec->data.chauthtok.msg, src->login_data.msg, AUIDMSG_SIZE);
	strncpy(rec->data.chauthtok.subj, src->login_data.subj, SUBJ_SIZE);
	strncpy(rec->data.chauthtok.exe, src->login_data.exe, EXE_SIZE);

	if (src->login_data.res == LOGIN_SUCCESS)
		strncpy(rec->data.chauthtok.res, "success", LOGIN_RESULT_SIZE);
	else if (src->login_data.res == LOGIN_FAIL)
		strncpy(rec->data.chauthtok.res, "failed", 
			LOGIN_RESULT_SIZE);
	else
		memset(rec->data.chauthtok.res, 0, LOGIN_RESULT_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   a user_chauthtok block
 *
 * Return: pointer to head of block
 */
rec_part *translate_load_pol(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_load_pol");
		return NULL;
	}
	initialize_rec_part(rec, INIT_LOAD_POL);

	strncpy(rec->type, "MAC_POLICY_LOAD", AUDIT_TYPE_SIZE);

	rec->data.load_pol.auid = src->auid;
	strncpy(rec->data.load_pol.auidmsg, src->auidmsg,AUIDMSG_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   a user_chauthtok block
 *
 * Return: pointer to head of block
 */
rec_part *translate_rbac(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_rbac");
		return NULL;
	}
	initialize_rec_part(rec, INIT_RBAC);

	strncpy(rec->type, "ANOM_RBAC_FAIL", AUDIT_TYPE_SIZE);

	rec->data.rbac.auid = src->auid;
	rec->data.rbac.pid = src->pid;
	rec->data.rbac.uid = src->uid;
	strncpy(rec->data.rbac.subj, src->subj, SUBJ_SIZE);
	strncpy(rec->data.rbac.msg, src->auidmsg, AUIDMSG_SIZE);
	strncpy(rec->data.rbac.exe, src->exe, EXE_SIZE);

	if (src->res == SUCCESS_YES)
		strncpy(rec->data.rbac.res, "success", LOGIN_RESULT_SIZE);
	else if (src->res == SUCCESS_NO)
		strncpy(rec->data.rbac.res, "failed", 
			LOGIN_RESULT_SIZE);
	else
		memset(rec->data.rbac.res, 0, LOGIN_RESULT_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   a user_chauthtok block
 *
 * Return: pointer to head of block
 */
rec_part *translate_dev_usys(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_dev_usys");
		return NULL;
	}
	initialize_rec_part(rec, INIT_DEV_USYS);

	if (src->audit_type == TYPE_DEV_ALLOC)
		strncpy(rec->type, "AUDIT_DEV_ALLOC", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_DEV_DEALLOC)
		strncpy(rec->type, "AUDIT_DEV_DEALLOC", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_AUDIT_USYS_CONFIG)
		strncpy(rec->type, "AUDIT_USYS_CONFIG", AUDIT_TYPE_SIZE);
	else
		strncpy(rec->type, "UNKNOWN", AUDIT_TYPE_SIZE);

	rec->data.dev_usys.auid = src->auid;
	rec->data.dev_usys.pid = src->pid;
	rec->data.dev_usys.uid = src->uid;
	strncpy(rec->data.dev_usys.obj, src->obj, SUBJ_SIZE);
	strncpy(rec->data.dev_usys.msg, src->auidmsg, AUIDMSG_SIZE);
	strncpy(rec->data.dev_usys.dev, src->device, DEV_SIZE);
	strncpy(rec->data.dev_usys.desc, src->desc, AUIDMSG_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   an avc block
 *
 * Return: pointer to head of block
 */
rec_part *translate_avc(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_avc");
		return NULL;
	}
	initialize_rec_part(rec, INIT_AVC);

	strncpy(rec->type, "AVC", AUDIT_TYPE_SIZE);

	rec->data.avc.pid = src->pid;
	rec->data.avc.ino = src->inode;
	strncpy(rec->data.avc.desc, src->desc, AUIDMSG_SIZE);
	strncpy(rec->data.avc.comm, src->comm, COMM_SIZE);
	strncpy(rec->data.avc.dev, src->device, DEV_SIZE);
	strncpy(rec->data.avc.name, src->name, NAME_SIZE);
	strncpy(rec->data.avc.scontext, src->scontext, SUBJ_SIZE);
	strncpy(rec->data.avc.tcontext, src->tcontext, SUBJ_SIZE);
	strncpy(rec->data.avc.tclass, src->tclass, TCLASS_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   an mac block
 *
 * Return: pointer to head of block
 */
rec_part *translate_mac(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_mac");
		return NULL;
	}
	initialize_rec_part(rec, INIT_MAC);

	if (src->audit_type == TYPE_MAC)
		strncpy(rec->type, "MAC_STATUS", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_MAC_CONFIG)
		strncpy(rec->type, "MAC_CONFIG_CHANGE", AUDIT_TYPE_SIZE);
	else
		strncpy(rec->type, "UNKNOWN", AUDIT_TYPE_SIZE);


	strncpy(rec->data.mac.bool_name, src->bool_name, BOOL_SIZE);
	rec->data.mac.auid = src->auid;
	rec->data.mac.curr_val = src->curr_val;
	rec->data.mac.old_val = src->old_val;

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   a role_change block
 *
 * Return: pointer to head of block
 */
rec_part *translate_role_change(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: translate_role_change");
		return NULL;
	}
	initialize_rec_part(rec, INIT_ROLE_CHANGE);

	strncpy(rec->type, "USER_ROLE_CHANGE", AUDIT_TYPE_SIZE);

	rec->data.role_chng.pid = src->pid;
	rec->data.role_chng.uid = src->uid;
	rec->data.role_chng.auid = src->auid;
	strncpy(rec->data.role_chng.subj, src->subj, SUBJ_SIZE);
	strncpy(rec->data.role_chng.acct, src->login_data.acct, LOGIN_ACCT_SIZE);
	strncpy(rec->data.role_chng.msg, src->auidmsg, AUIDMSG_SIZE);
	strncpy(rec->data.role_chng.op, src->login_data.op, LOGIN_OP_SIZE);

	strncpy(rec->data.role_chng.def_cntx, src->def_cntx, SUBJ_SIZE);
	strncpy(rec->data.role_chng.sel_cntx, src->sel_cntx, SUBJ_SIZE);
	strncpy(rec->data.role_chng.old_cntx, src->old_cntx, SUBJ_SIZE);
	strncpy(rec->data.role_chng.new_cntx, src->new_cntx, SUBJ_SIZE);
	strncpy(rec->data.role_chng.old_user, src->old_user, USER_SIZE);
	strncpy(rec->data.role_chng.old_role, src->old_role, ROLE_SIZE);
	strncpy(rec->data.role_chng.old_range, src->old_range, RANGE_SIZE);
	strncpy(rec->data.role_chng.new_user, src->new_user, USER_SIZE);
	strncpy(rec->data.role_chng.new_role, src->new_role, ROLE_SIZE);
	strncpy(rec->data.role_chng.new_range, src->new_range, RANGE_SIZE);

	strncpy(rec->data.role_chng.exe, src->exe, EXE_SIZE);

	if (src->res == SUCCESS_YES)
		strncpy(rec->data.role_chng.res, "success", LOGIN_RESULT_SIZE);
	else if (src->res == SUCCESS_NO)
		strncpy(rec->data.role_chng.res, "failed", LOGIN_RESULT_SIZE);
	else
		memset(rec->data.role_chng.res, 0, LOGIN_RESULT_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   a cipso block
 *
 * Return: pointer to head of block
 */
rec_part *translate_cipso(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: cipso");
		return NULL;
	}
	initialize_rec_part(rec, INIT_CIPSO);

	if (src->audit_type == TYPE_CIPSO_ADD)
		strncpy(rec->type, "MAC_CIPSOV4_ADD", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_CIPSO_DEL)
		strncpy(rec->type, "MAC_CIPSOV4_DEL", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_MAP_ADD)
		strncpy(rec->type, "MAC_MAP_ADD", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_MAP_DEL)
		strncpy(rec->type, "MAC_MAP_DEL", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_UNLBL_ALLOW)
		strncpy(rec->type, "MAC_UNLBL_ALLOW", AUDIT_TYPE_SIZE);
	else
		strncpy(rec->type, "UNKNOWN", AUDIT_TYPE_SIZE);


	rec->data.cipso.auid = src->auid;
	rec->data.cipso.cipso_doi = src->cipso_doi;
	rec->data.cipso.res = src->res;
	rec->data.cipso.old = src->old_val;
	rec->data.cipso.unlbl_accept = src->unlbl_accept;
	strncpy(rec->data.cipso.subj, src->subj, SUBJ_SIZE);
	strncpy(rec->data.cipso.nlbl_domain, src->nlbl_domain, DOM_SIZE);
	strncpy(rec->data.cipso.nlbl_protocol, src->nlbl_protocol, PROTOCOL_SIZE);
	strncpy(rec->data.cipso.cipso_type, src->cipso_type, CIPSO_TYPE_SIZE);
	strncpy(rec->data.cipso.tool, src->cipso_tool, TOOL_NAME_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   an ipsec block
 *
 * Return: pointer to head of block
 */
rec_part *translate_ipsec(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: ipsec");
		return NULL;
	}
	initialize_rec_part(rec, INIT_IPSEC);

	if (src->audit_type == TYPE_IPSEC_ADDSA)
		strncpy(rec->type, "MAC_IPSEC_ADDSA", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_IPSEC_DELSA)
		strncpy(rec->type, "MAC_IPSEC_DELSA", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_IPSEC_ADDSPD)
		strncpy(rec->type, "MAC_IPSEC_ADDSPD", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_IPSEC_DELSPD)
		strncpy(rec->type, "MAC_IPSEC_DELSPD", AUDIT_TYPE_SIZE);
	else
		strncpy(rec->type, "UNKNOWN", AUDIT_TYPE_SIZE);


	rec->data.ipsec.auid = src->auid;
	rec->data.ipsec.sec_alg = src->sec_alg;
	rec->data.ipsec.sec_doi = src->sec_doi;
	rec->data.ipsec.res = src->res;
	strncpy(rec->data.ipsec.desc, src->desc, AUIDMSG_SIZE);
	strncpy(rec->data.ipsec.subj, src->subj, SUBJ_SIZE);
	strncpy(rec->data.ipsec.sec_obj, src->obj, SUBJ_SIZE);
	strncpy(rec->data.ipsec.spi, src->spi, SPI_SIZE);
	strncpy(rec->data.ipsec.protocol, src->protocol, PROTOCOL_SIZE);
	strncpy(rec->data.ipsec.src, src->src, LOGIN_HOST_SIZE);
	strncpy(rec->data.ipsec.dst, src->dst, LOGIN_HOST_SIZE);

	return rec;
}

/*
 * This functions translates provided fields by test case into
 *   an cups block
 *
 * Return: pointer to head of block
 */
rec_part *translate_cups(struct audit_record *src)
{
	rec_part *rec = NULL;

	rec = (rec_part *)malloc(sizeof(rec_part));
	if (!rec) {
		tst_resm(TINFO, "Unable to allocate memory: cups");
		return NULL;
	}
	initialize_rec_part(rec, INIT_CUPS);

	if (src->audit_type == TYPE_USER_LABELED_EXPORT)
		strncpy(rec->type, "USER_LABELED_EXPORT", AUDIT_TYPE_SIZE);
	else if (src->audit_type == TYPE_LABEL_OVERRIDE)
		strncpy(rec->type, "LABEL_OVERRIDE", AUDIT_TYPE_SIZE);
	else
		strncpy(rec->type, "UNKNOWN", AUDIT_TYPE_SIZE);


	rec->data.cups.auid = src->auid;
	rec->data.cups.auid2 = src->auid2;
	rec->data.cups.uid = src->uid;
	rec->data.cups.pid = src->pid;
	rec->data.cups.job = src->job;
	strncpy(rec->data.cups.acct, src->acct, LOGIN_ACCT_SIZE);
	strncpy(rec->data.cups.printer, src->printer, PRINTER_SIZE);
	strncpy(rec->data.cups.title, src->title, TITLE_SIZE);
	strncpy(rec->data.cups.label, src->label, LABEL_SIZE);
	strncpy(rec->data.cups.subj, src->subj, SUBJ_SIZE);
	strncpy(rec->data.cups.obj, src->obj, SUBJ_SIZE);
	strncpy(rec->data.cups.exe, src->exe, EXE_SIZE);
	strncpy(rec->data.cups.msg, src->desc, AUIDMSG_SIZE);

	if (src->res == SUCCESS_YES)
		strncpy(rec->data.cups.res, "success", LOGIN_RESULT_SIZE);
	else if (src->res == SUCCESS_NO)
		strncpy(rec->data.cups.res, "failed", LOGIN_RESULT_SIZE);
	else
		memset(rec->data.cups.res, 0, LOGIN_RESULT_SIZE);

	return rec;
}

/**
 * The function puts the flat structure provided by test case into the linked
 * list structure to match the design of structure used to store the parsed
 * records from the audit log
 * 
 * Note: Only fields set by the user are translated, if user doesn't care
 *   about a field, just don't set in test case.
 *
 * Args:
 * src: the audit record structure as provided by test case.
 *
 * Return	pointer to head of the list of parsed records	
 */
main_list* translate_for_parser(struct audit_record *src)
{
	rec_part *rec = NULL;
	rec_part *head = NULL;
	main_list *list = NULL;

	/* start a list to hold the translated record */
	list = (main_list *)malloc(sizeof(main_list));
	if (list == NULL)
	{
		tst_resm(TINFO, "Error allocating memory for list\n");
		return list;
	}
	list->rec_time = 0;
	list->myrecord = NULL;
	list->next_record = NULL;

	/* if CAPP=OFF, or CAPP is not defined, then we set the auid of the 
	 * src record to VAL_NOT_SET this way it will not be checked against 
	 * any auid field */
	if (getenv("CAPP") != NULL && (!strcmp(getenv("CAPP"), "ON")) ) {
		#if !SUPPRESS_VERBOSE_OUTPUT
			printf("CAPP CHECK ON\n");
		#endif
	} else {
		src->auid = VAL_NOT_SET;
		#if !SUPPRESS_VERBOSE_OUTPUT
			printf("CAPP CHECK OFF\n");
		#endif
	}
		
	/* filling up the SYSCALL record part */
	if (src->syscallno != VAL_NOT_SET || src->exit != VAL_NOT_SET) {
		rec = translate_syscall(src);
		head = append_record(&list, rec);
	}

	/* filling up PATH record part */
	if (src->objectname[0] != '\0') {
		/* the "1" indicates filling up the first path block */
		rec = translate_path(src, 1);
		head = append_record(&list, rec);
	}

	/* filling up second PATH record part */
	if (src->objectname1[0] != '\0') {
		/* the "2" indicates filling up the second path block */
		rec = translate_path(src, 2);
		head = append_record(&list, rec);
	}

	/* filling up CONFIG_CHANGE record part */
	if (src->audit_type == TYPE_CONFIG_CHANGE ) {
		rec = translate_config(src);
		head = append_record(&list, rec);
	}

	/* filling up USER record part */
	if (src->audit_type == TYPE_USER_ACCT 
		|| src->audit_type == TYPE_USER_AUTH 
		|| src->audit_type == TYPE_USER_START 
		|| src->audit_type == TYPE_USER 
		|| src->audit_type == TYPE_USER_LOGIN ) { 
		rec = translate_user(src);
		head = append_record(&list, rec);
	}

	/* filling up CHAUTHTOK record part */
	if (src->audit_type == TYPE_USER_CHAUTHTOK
		|| src->audit_type == TYPE_USYS_CONFIG) {
		rec = translate_chauthtok(src);
		head = append_record(&list, rec);
	}

	/* filling up MAC_POLICY_LOAD record part */
	if (src->audit_type == TYPE_MAC_POLICY_LOAD) { 
		rec = translate_load_pol(src);
		head = append_record(&list, rec);
	}

	/* filling up DEV and USYS record parts */
	if (src->audit_type == TYPE_DEV_ALLOC 
		|| src->audit_type == TYPE_DEV_DEALLOC
		|| src->audit_type == TYPE_AUDIT_USYS_CONFIG) {
		rec = translate_dev_usys(src);
		head = append_record(&list, rec);
	}

	/* filling up RBAC record part */
	if (src->audit_type == TYPE_RBAC) { 
		rec = translate_rbac(src);
		head = append_record(&list, rec);
	}

	/* filling up AVC record part */
	if (src->audit_type == TYPE_AVC) { 
		rec = translate_avc(src);
		head = append_record(&list, rec);
	}

	/* filling up MAC_STATUS and MAC_CONFIG_CHANGE record part */
	if ( (src->audit_type == TYPE_MAC) 
		|| src->audit_type == TYPE_MAC_CONFIG) { 
		rec = translate_mac(src);
		head = append_record(&list, rec);
	}

	/* filling up USER_ROLE_CHANGE record part */
	if (src->audit_type == TYPE_USER_ROLE_CHANGE) { 
		rec = translate_role_change(src);
		head = append_record(&list, rec);
	}

	/* filling up CIPCO related record parts */
	if (src->audit_type == TYPE_CIPSO_ADD 
		|| src->audit_type == TYPE_CIPSO_DEL
		|| src->audit_type == TYPE_MAP_ADD
		|| src->audit_type == TYPE_MAP_DEL
		|| src->audit_type == TYPE_UNLBL_ALLOW) {
		rec = translate_cipso(src);
		head = append_record(&list, rec);
	}

	/* filling up IPSEC related record parts */
	if (src->audit_type == TYPE_IPSEC_ADDSA 
		|| src->audit_type == TYPE_IPSEC_DELSA
		|| src->audit_type == TYPE_IPSEC_ADDSPD
		|| src->audit_type == TYPE_IPSEC_DELSPD) {
		rec = translate_ipsec(src);
		head = append_record(&list, rec);
	}

	/* filling up USER_LABELED_EXPORT and LABEL_OVERRIDE record part */
	if ( (src->audit_type == TYPE_USER_LABELED_EXPORT) 
		|| src->audit_type == TYPE_LABEL_OVERRIDE) { 
		rec = translate_cups(src);
		head = append_record(&list, rec);
	}

	return list;
}

/*
 * This function checks expected against actual syscall blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   sys_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_syscall(rec_part *expected_rec, rec_part *actual_rec, int sys_flag)
{
	if (expected_rec->data.syscall.syscall_no != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.syscall_no 
			== actual_rec->data.syscall.syscall_no);
	}
	if (expected_rec->data.syscall.exit != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.exit 
				== actual_rec->data.syscall.exit);
	}
	if (expected_rec->data.syscall.ppid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.ppid
				== actual_rec->data.syscall.ppid);
	}
	if (expected_rec->data.syscall.pid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.pid
				== actual_rec->data.syscall.pid);
	}
	if (expected_rec->data.syscall.auid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.auid 
				== actual_rec->data.syscall.auid);
	}
	if (expected_rec->data.syscall.uid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.uid 
				== actual_rec->data.syscall.uid);
	}
	if (expected_rec->data.syscall.euid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.euid 
				== actual_rec->data.syscall.euid);
	}
	if (expected_rec->data.syscall.suid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.suid 
				== actual_rec->data.syscall.suid);
	}
	if (expected_rec->data.syscall.fsuid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.fsuid 
				== actual_rec->data.syscall.fsuid);
	}
	if (expected_rec->data.syscall.gid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.gid 
				== actual_rec->data.syscall.gid);
	}
	if (expected_rec->data.syscall.egid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.egid 
				== actual_rec->data.syscall.egid);
	}
	if (expected_rec->data.syscall.sgid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.sgid 
				== actual_rec->data.syscall.sgid);
	}
	if (expected_rec->data.syscall.fsgid != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.fsgid 
				== actual_rec->data.syscall.fsgid);
	}
	if (expected_rec->data.syscall.success[0] != '\0') {
		sys_flag = sys_flag & 
			!(strcmp(expected_rec->data.syscall.success, 
			actual_rec->data.syscall.success));
	}
	if (expected_rec->data.syscall.argv[0] != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.argv[0] 
			 == actual_rec->data.syscall.argv[0]);
	}
	if (expected_rec->data.syscall.argv[1] != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.argv[1] 
			 == actual_rec->data.syscall.argv[1]);
	}
	if (expected_rec->data.syscall.argv[2] != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.argv[2] 
			 == actual_rec->data.syscall.argv[2]);
	}
	if (expected_rec->data.syscall.argv[3] != VAL_NOT_SET) {
		sys_flag = sys_flag & (expected_rec->data.syscall.argv[3] 
			 == actual_rec->data.syscall.argv[3]);
	}
	if (expected_rec->data.syscall.subj[0] != '\0') {
		sys_flag = sys_flag & 
			!(strncmp(expected_rec->data.syscall.subj, 
			actual_rec->data.syscall.subj, 
			strlen(expected_rec->data.syscall.subj)));
	}
	if (expected_rec->data.syscall.key[0] != '\0') {
		sys_flag = sys_flag & 
			!(strncmp(expected_rec->data.syscall.key, 
			actual_rec->data.syscall.key, 
			strlen(expected_rec->data.syscall.key)));
	}

	return !sys_flag;
}

/*
 * This function checks expected against actual path blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   path_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_path(rec_part *expected_rec, rec_part *actual_rec, int path_flag)
{
	if (expected_rec->data.path.name[0] != '\0') {
		path_flag = path_flag & !(strcmp(expected_rec->data.path.name, 
			actual_rec->data.path.name));
	}
	if (expected_rec->data.path.flags != VAL_NOT_SET) {
		path_flag = path_flag & (expected_rec->data.path.flags 
			== actual_rec->data.path.flags);
	}
	if (expected_rec->data.path.mode != VAL_NOT_SET) {
		path_flag = path_flag & (expected_rec->data.path.mode 
			== actual_rec->data.path.mode);
	}
	if (expected_rec->data.path.obj[0] != '\0') {
		path_flag = path_flag & !(strncmp(expected_rec->data.path.obj, 
			actual_rec->data.path.obj, 
			strlen(expected_rec->data.path.obj)));
	}

	return !path_flag;
}

/*
 * This function checks expected against actual user blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   user_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_user(rec_part *expected_rec, rec_part *actual_rec, int user_flag)
{
	if (expected_rec->data.user.auid != VAL_NOT_SET) {
		user_flag = user_flag & (expected_rec->data.user.auid 
			== actual_rec->data.user.auid);
	}
	if (expected_rec->data.user.uid != VAL_NOT_SET) {
		user_flag = user_flag & (expected_rec->data.user.uid 
			== actual_rec->data.user.uid);
	}
	if (expected_rec->data.user.pid != VAL_NOT_SET) {
		user_flag = user_flag & (expected_rec->data.user.pid 
			== actual_rec->data.user.pid);
	}
	if (expected_rec->data.user.user_login_uid != VAL_NOT_SET) {
		user_flag = user_flag & (expected_rec->data.user.user_login_uid 
			== actual_rec->data.user.user_login_uid);
	}
	if (expected_rec->data.user.user[0] != '\0') {
		user_flag = user_flag & !(strcmp(expected_rec->data.user.user, 
			actual_rec->data.user.user));
	}
	if (expected_rec->data.user.terminal[0] != '\0') {
		user_flag = user_flag & 
			!(strcmp(expected_rec->data.user.terminal, 
			actual_rec->data.user.terminal));
	}
	if (expected_rec->data.user.host[0] != '\0') {
		user_flag = user_flag & 
			!(strcmp(expected_rec->data.user.host, 
			actual_rec->data.user.host));
	}
	if (expected_rec->data.user.addr[0] != '\0') {
		user_flag = user_flag & !(strcmp(expected_rec->data.user.addr, 
			actual_rec->data.user.addr));
	}
	if (expected_rec->data.user.subj[0] != '\0') {
		user_flag = user_flag & !(strcmp(expected_rec->data.user.subj, 
			actual_rec->data.user.subj));
	}

	if (expected_rec->data.user.exe[0] != '\0') {
		user_flag = user_flag & !(strcmp(expected_rec->data.user.exe, 
			actual_rec->data.user.exe));
	}

	if (expected_rec->data.user.msg[0] != '\0') {
		user_flag = user_flag & 
			!(strncmp(expected_rec->data.user.msg, 
			actual_rec->data.user.msg, 
			strlen(expected_rec->data.user.msg)));
	}

/* using strncmp to to compare only the length of what is expected. this way
 * it eliminates the problem the ")'" chars that are parsed with the result 
 * field are causing */
	if (expected_rec->data.user.result[0] != '\0') {
 		user_flag = user_flag & 
			!(strncmp(expected_rec->data.user.result, 
			actual_rec->data.user.result, 
			strlen(expected_rec->data.user.result)));
	 }

	return !user_flag;
}

/*
 * This function checks expected against actual config blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   config_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_config(rec_part *expected_rec, rec_part *actual_rec, int config_flag)
{
/* using strncmp to avoid problems caused by extra spaces at end of msg */
	if (expected_rec->data.config.msg[0] != '\0') {
		config_flag = config_flag & 
			!(strncmp(expected_rec->data.config.msg, 
			actual_rec->data.config.msg, 
			strlen(expected_rec->data.config.msg)));
	}
	if (expected_rec->data.config.auid != VAL_NOT_SET) {
		config_flag = config_flag & (expected_rec->data.config.auid 
			== actual_rec->data.config.auid);
	}
	if (expected_rec->data.config.subj[0] != '\0') {
		config_flag = config_flag & 
			!(strncmp(expected_rec->data.config.subj, 
			actual_rec->data.config.subj, 
			strlen(expected_rec->data.config.subj)));
	}

	return !config_flag;
}

/*
 * This function checks expected against actual chauthtok blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   chauth_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_chauthtok(rec_part *expected_rec, rec_part *actual_rec, int chauth_flag)
{
	if (expected_rec->data.chauthtok.auid != VAL_NOT_SET) {
		chauth_flag = chauth_flag & (expected_rec->data.chauthtok.auid 
			== actual_rec->data.chauthtok.auid);
	}
	if (expected_rec->data.chauthtok.pid != VAL_NOT_SET) {
		chauth_flag = chauth_flag & (expected_rec->data.chauthtok.pid 
			== actual_rec->data.chauthtok.pid);
	}
	if (expected_rec->data.chauthtok.uid != VAL_NOT_SET) {
		chauth_flag = chauth_flag & (expected_rec->data.chauthtok.uid 
			== actual_rec->data.chauthtok.uid);
	}
	if (expected_rec->data.chauthtok.id != VAL_NOT_SET) {
		chauth_flag = chauth_flag & (expected_rec->data.chauthtok.id 
			== actual_rec->data.chauthtok.id);
	}
/* using strncmp to deal with the ":" char at end of parsed msg field */
	if (expected_rec->data.chauthtok.msg[0] != '\0') {
		chauth_flag = chauth_flag & 
			!(strncmp(expected_rec->data.chauthtok.msg, 
			actual_rec->data.chauthtok.msg, 
			strlen(expected_rec->data.chauthtok.msg)));
	}
	if (expected_rec->data.chauthtok.op[0] != '\0') {
		chauth_flag = chauth_flag & 
			!(strcmp(expected_rec->data.chauthtok.op, 
			actual_rec->data.chauthtok.op));
	}
	if (expected_rec->data.chauthtok.acct[0] != '\0') {
		chauth_flag = chauth_flag & 
			!(strcmp(expected_rec->data.chauthtok.acct, 
			actual_rec->data.chauthtok.acct));
	}
	if (expected_rec->data.chauthtok.res[0] != '\0') {
		chauth_flag = chauth_flag & 
			!(strcmp(expected_rec->data.chauthtok.res, 
			actual_rec->data.chauthtok.res));
	}
	if (expected_rec->data.chauthtok.subj[0] != '\0') {
		chauth_flag = chauth_flag & 
			!(strncmp(expected_rec->data.chauthtok.subj, 
			actual_rec->data.chauthtok.subj,
			strlen(expected_rec->data.chauthtok.subj)));
	}
	return !chauth_flag;
}

/*
 * This function checks expected against actual load_pol blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   pol_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_load_pol(rec_part *expected_rec, rec_part *actual_rec, int pol_flag)
{
	if (expected_rec->data.load_pol.auid != VAL_NOT_SET) {
		pol_flag = pol_flag & (expected_rec->data.load_pol.auid 
			== actual_rec->data.load_pol.auid);
	}
	if (expected_rec->data.load_pol.auidmsg[0] != '\0') {
		pol_flag = pol_flag & 
			!(strncmp(expected_rec->data.load_pol.auidmsg, 
			actual_rec->data.load_pol.auidmsg,
			strlen(expected_rec->data.load_pol.auidmsg)));
	}
	return !pol_flag;
}

/*
 * This function checks expected against actual dev_usys blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   dev_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_dev_usys(rec_part *expected_rec, rec_part *actual_rec, int dev_flag)
{
	if (expected_rec->data.dev_usys.auid != VAL_NOT_SET) {
		dev_flag = dev_flag & (expected_rec->data.dev_usys.auid 
			== actual_rec->data.dev_usys.auid);
	}
	if (expected_rec->data.dev_usys.pid != VAL_NOT_SET) {
		dev_flag = dev_flag & (expected_rec->data.dev_usys.pid 
			== actual_rec->data.dev_usys.pid);
	}
	if (expected_rec->data.dev_usys.uid != VAL_NOT_SET) {
		dev_flag = dev_flag & (expected_rec->data.dev_usys.uid 
			== actual_rec->data.dev_usys.uid);
	}
	if (expected_rec->data.dev_usys.msg[0] != '\0') {
		dev_flag = dev_flag & 
			!(strncmp(expected_rec->data.dev_usys.msg, 
			actual_rec->data.dev_usys.msg,
			strlen(expected_rec->data.dev_usys.msg)));
	}
	if (expected_rec->data.dev_usys.desc[0] != '\0') {
		dev_flag = dev_flag & 
			!(strncmp(expected_rec->data.dev_usys.desc, 
			actual_rec->data.dev_usys.desc,
			strlen(expected_rec->data.dev_usys.desc)));
	}
	if (expected_rec->data.dev_usys.dev[0] != '\0') {
		dev_flag = dev_flag & 
			!(strncmp(expected_rec->data.dev_usys.dev, 
			actual_rec->data.dev_usys.dev,
			strlen(expected_rec->data.dev_usys.dev)));
	}
	if (expected_rec->data.dev_usys.obj[0] != '\0') {
		dev_flag = dev_flag & 
			!(strncmp(expected_rec->data.dev_usys.obj, 
			actual_rec->data.dev_usys.obj,
			strlen(expected_rec->data.dev_usys.obj)));
	}
	if (expected_rec->data.dev_usys.op[0] != '\0') {
		dev_flag = dev_flag & 
			!(strncmp(expected_rec->data.dev_usys.op, 
			actual_rec->data.dev_usys.op,
			strlen(expected_rec->data.dev_usys.op)));
	}
	return !dev_flag;
}

/*
 * This function checks expected against actual rbac blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   rbac_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_rbac(rec_part *expected_rec, rec_part *actual_rec, int rbac_flag)
{
	if (expected_rec->data.rbac.auid != VAL_NOT_SET) {
		rbac_flag = rbac_flag & (expected_rec->data.rbac.auid 
			== actual_rec->data.rbac.auid);
	}
	if (expected_rec->data.rbac.pid != VAL_NOT_SET) {
		rbac_flag = rbac_flag & (expected_rec->data.rbac.pid 
			== actual_rec->data.rbac.pid);
	}
	if (expected_rec->data.rbac.uid != VAL_NOT_SET) {
		rbac_flag = rbac_flag & (expected_rec->data.rbac.uid 
			== actual_rec->data.rbac.uid);
	}
	if (expected_rec->data.rbac.subj[0] != '\0') {
		rbac_flag = rbac_flag & 
			!(strncmp(expected_rec->data.rbac.subj, 
			actual_rec->data.rbac.subj,
			strlen(expected_rec->data.rbac.subj)));
	}
	if (expected_rec->data.rbac.msg[0] != '\0') {
		rbac_flag = rbac_flag & 
			!(strncmp(expected_rec->data.rbac.msg, 
			actual_rec->data.rbac.msg,
			strlen(expected_rec->data.rbac.msg)));
	}
	if (expected_rec->data.rbac.res[0] != '\0') {
		rbac_flag = rbac_flag & 
			!(strcmp(expected_rec->data.rbac.res, 
			actual_rec->data.rbac.res));
	}

	return !rbac_flag;
}

/*
 * This function checks expected against actual avc blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   avc_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_avc(rec_part *expected_rec, rec_part *actual_rec, int avc_flag)
{
	if (expected_rec->data.avc.pid != VAL_NOT_SET) {
		avc_flag = avc_flag & (expected_rec->data.avc.pid 
			== actual_rec->data.avc.pid);
	}
	if (expected_rec->data.avc.ino != VAL_NOT_SET) {
		avc_flag = avc_flag & (expected_rec->data.avc.ino 
			== actual_rec->data.avc.ino);
	}
	if (expected_rec->data.avc.desc[0] != '\0') {
		avc_flag = avc_flag & 
			!(strncmp(expected_rec->data.avc.desc, 
			actual_rec->data.avc.desc,
			strlen(expected_rec->data.avc.desc)));
	}

	if (expected_rec->data.avc.comm[0] != '\0') {
		avc_flag = avc_flag & 
			!(strncmp(expected_rec->data.avc.comm, 
			actual_rec->data.avc.comm,
			strlen(expected_rec->data.avc.comm)));
	}

	if (expected_rec->data.avc.dev[0] != '\0') {
		avc_flag = avc_flag & 
			!(strncmp(expected_rec->data.avc.dev, 
			actual_rec->data.avc.dev,
			strlen(expected_rec->data.avc.dev)));
	}

	if (expected_rec->data.avc.name[0] != '\0') {
		avc_flag = avc_flag & 
			!(strncmp(expected_rec->data.avc.name, 
			actual_rec->data.avc.name,
			strlen(expected_rec->data.avc.name)));
	}

	if (expected_rec->data.avc.scontext[0] != '\0') {
		avc_flag = avc_flag & 
			!(strncmp(expected_rec->data.avc.scontext, 
			actual_rec->data.avc.scontext,
			strlen(expected_rec->data.avc.scontext)));
	}

	if (expected_rec->data.avc.tcontext[0] != '\0') {
		avc_flag = avc_flag & 
			!(strncmp(expected_rec->data.avc.tcontext, 
			actual_rec->data.avc.tcontext,
			strlen(expected_rec->data.avc.tcontext)));
	}

	if (expected_rec->data.avc.tclass[0] != '\0') {
		avc_flag = avc_flag & 
			!(strncmp(expected_rec->data.avc.tclass, 
			actual_rec->data.avc.tclass,
			strlen(expected_rec->data.avc.tclass)));
	}

	return !avc_flag;
}

/*
 * This function checks expected against actual mac blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   mac_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_mac(rec_part *expected_rec, rec_part *actual_rec, int mac_flag)
{
	if (expected_rec->data.mac.auid != VAL_NOT_SET) {
		mac_flag = mac_flag & (expected_rec->data.mac.auid 
			== actual_rec->data.mac.auid);
	}

	if (expected_rec->data.mac.curr_val != VAL_NOT_SET) {
		mac_flag = mac_flag & (expected_rec->data.mac.curr_val 
			== actual_rec->data.mac.curr_val);
	}

	if (expected_rec->data.mac.old_val != VAL_NOT_SET) {
		mac_flag = mac_flag & (expected_rec->data.mac.old_val 
			== actual_rec->data.mac.old_val);
	}

	if (expected_rec->data.mac.bool_name[0] != '\0') {
		mac_flag = mac_flag & 
			!(strncmp(expected_rec->data.mac.bool_name, 
			actual_rec->data.mac.bool_name, 
			strlen(expected_rec->data.mac.bool_name)));
	}

	return !mac_flag;
}

/*
 * This function checks expected against actual user_role_change blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   role_chng_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_role_change(rec_part *expected_rec, rec_part *actual_rec, int role_chng_flag)
{
	if (expected_rec->data.role_chng.pid != VAL_NOT_SET) {
		role_chng_flag = role_chng_flag & 
			(expected_rec->data.role_chng.pid 
			== actual_rec->data.role_chng.pid);
	}

	if (expected_rec->data.role_chng.auid != VAL_NOT_SET) {
		role_chng_flag = role_chng_flag & 
			(expected_rec->data.role_chng.auid 
			== actual_rec->data.role_chng.auid);
	}

	if (expected_rec->data.role_chng.uid != VAL_NOT_SET) {
		role_chng_flag = role_chng_flag & 
			(expected_rec->data.role_chng.uid 
			== actual_rec->data.role_chng.uid);
	}

	if (expected_rec->data.role_chng.subj[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.subj, 
			actual_rec->data.role_chng.subj,
			strlen(expected_rec->data.role_chng.subj)));
	}

	if (expected_rec->data.role_chng.op[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.op, 
			actual_rec->data.role_chng.op,
			strlen(expected_rec->data.role_chng.op)));
	}

	if (expected_rec->data.role_chng.acct[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.acct, 
			actual_rec->data.role_chng.acct,
			strlen(expected_rec->data.role_chng.acct)));
	}

	if (expected_rec->data.role_chng.old_cntx[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.old_cntx, 
			actual_rec->data.role_chng.old_cntx,
			strlen(expected_rec->data.role_chng.old_cntx)));
	}

	if (expected_rec->data.role_chng.new_cntx[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.new_cntx, 
			actual_rec->data.role_chng.new_cntx,
			strlen(expected_rec->data.role_chng.new_cntx)));
	}

	if (expected_rec->data.role_chng.def_cntx[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.def_cntx, 
			actual_rec->data.role_chng.def_cntx,
			strlen(expected_rec->data.role_chng.def_cntx)));
	}

	if (expected_rec->data.role_chng.sel_cntx[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.sel_cntx, 
			actual_rec->data.role_chng.sel_cntx,
			strlen(expected_rec->data.role_chng.sel_cntx)));
	}

	if (expected_rec->data.role_chng.old_user[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.old_user, 
			actual_rec->data.role_chng.old_user,
			strlen(expected_rec->data.role_chng.old_user)));
	}

	if (expected_rec->data.role_chng.new_user[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.new_user, 
			actual_rec->data.role_chng.new_user,
			strlen(expected_rec->data.role_chng.new_user)));
	}

	if (expected_rec->data.role_chng.old_role[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.old_role, 
			actual_rec->data.role_chng.old_role,
			strlen(expected_rec->data.role_chng.old_role)));
	}

	if (expected_rec->data.role_chng.new_role[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.new_role, 
			actual_rec->data.role_chng.new_role,
			strlen(expected_rec->data.role_chng.new_role)));
	}

	if (expected_rec->data.role_chng.old_range[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.old_range, 
			actual_rec->data.role_chng.old_range,
			strlen(expected_rec->data.role_chng.old_range)));
	}

	if (expected_rec->data.role_chng.new_range[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.new_range, 
			actual_rec->data.role_chng.new_range,
			strlen(expected_rec->data.role_chng.new_range)));
	}

	if (expected_rec->data.role_chng.exe[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.exe, 
			actual_rec->data.role_chng.exe,
			strlen(expected_rec->data.role_chng.exe)));
	}

	if (expected_rec->data.role_chng.res[0] != '\0') {
		role_chng_flag = role_chng_flag & 
			!(strncmp(expected_rec->data.role_chng.res, 
			actual_rec->data.role_chng.res,
			strlen(expected_rec->data.role_chng.res)));
	}

	return !role_chng_flag;
}

/*
 * This function checks expected against actual cipso blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   cipso_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_cipso(rec_part *expected_rec, rec_part *actual_rec, int cipso_flag)
{
	if (expected_rec->data.cipso.auid != VAL_NOT_SET) {
		cipso_flag = cipso_flag & (expected_rec->data.cipso.auid 
			== actual_rec->data.cipso.auid);
	}

	if (expected_rec->data.cipso.res != VAL_NOT_SET) {
		cipso_flag = cipso_flag & (expected_rec->data.cipso.res 
			== actual_rec->data.cipso.res);
	}

	if (expected_rec->data.cipso.cipso_doi != VAL_NOT_SET) {
		cipso_flag = cipso_flag & (expected_rec->data.cipso.cipso_doi 
			== actual_rec->data.cipso.cipso_doi);
	}

	if (expected_rec->data.cipso.unlbl_accept != VAL_NOT_SET) {
		cipso_flag = cipso_flag & 
			(expected_rec->data.cipso.unlbl_accept 
			== actual_rec->data.cipso.unlbl_accept);
	}

	if (expected_rec->data.cipso.old != VAL_NOT_SET) {
		cipso_flag = cipso_flag & (expected_rec->data.cipso.old 
			== actual_rec->data.cipso.old);
	}

	if (expected_rec->data.cipso.subj[0] != '\0') {
		cipso_flag = cipso_flag & 
			!(strncmp(expected_rec->data.cipso.subj, 
			actual_rec->data.cipso.subj,
			strlen(expected_rec->data.cipso.subj)));
	}

	if (expected_rec->data.cipso.nlbl_domain[0] != '\0') {
		cipso_flag = cipso_flag & 
			!(strncmp(expected_rec->data.cipso.nlbl_domain, 
			actual_rec->data.cipso.nlbl_domain,
			strlen(expected_rec->data.cipso.nlbl_domain)));
	}

	if (expected_rec->data.cipso.nlbl_protocol[0] != '\0') {
		cipso_flag = cipso_flag & 
			!(strncmp(expected_rec->data.cipso.nlbl_protocol, 
			actual_rec->data.cipso.nlbl_protocol,
			strlen(expected_rec->data.cipso.nlbl_protocol)));
	}

	if (expected_rec->data.cipso.cipso_type[0] != '\0') {
		cipso_flag = cipso_flag & 
			!(strncmp(expected_rec->data.cipso.cipso_type, 
			actual_rec->data.cipso.cipso_type,
			strlen(expected_rec->data.cipso.cipso_type)));
	}

	if (expected_rec->data.cipso.tool[0] != '\0') {
		cipso_flag = cipso_flag & 
			!(strncmp(expected_rec->data.cipso.tool, 
			actual_rec->data.cipso.tool,
			strlen(expected_rec->data.cipso.tool)));
	}

	return !cipso_flag;
}


/*
 * This function checks expected against actual ipsec blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   ipsec_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_ipsec(rec_part *expected_rec, rec_part *actual_rec, int ipsec_flag)
{
	if (expected_rec->data.ipsec.auid != VAL_NOT_SET) {
		ipsec_flag = ipsec_flag & (expected_rec->data.ipsec.auid 
			== actual_rec->data.ipsec.auid);
	}

	if (expected_rec->data.ipsec.res != VAL_NOT_SET) {
		ipsec_flag = ipsec_flag & (expected_rec->data.ipsec.res 
			== actual_rec->data.ipsec.res);
	}

	if (expected_rec->data.ipsec.sec_doi != VAL_NOT_SET) {
		ipsec_flag = ipsec_flag & (expected_rec->data.ipsec.sec_doi 
			== actual_rec->data.ipsec.sec_doi);
	}

	if (expected_rec->data.ipsec.sec_alg != VAL_NOT_SET) {
		ipsec_flag = ipsec_flag & (expected_rec->data.ipsec.sec_alg 
			== actual_rec->data.ipsec.sec_alg);
	}

	if (expected_rec->data.ipsec.subj[0] != '\0') {
		ipsec_flag = ipsec_flag & 
			!(strncmp(expected_rec->data.ipsec.subj, 
			actual_rec->data.ipsec.subj,
			strlen(expected_rec->data.ipsec.subj)));
	}

	if (expected_rec->data.ipsec.sec_obj[0] != '\0') {
		ipsec_flag = ipsec_flag & 
			!(strncmp(expected_rec->data.ipsec.sec_obj, 
			actual_rec->data.ipsec.sec_obj,
			strlen(expected_rec->data.ipsec.sec_obj)));
	}

	if (expected_rec->data.ipsec.src[0] != '\0') {
		ipsec_flag = ipsec_flag & 
			!(strncmp(expected_rec->data.ipsec.src, 
			actual_rec->data.ipsec.src,
			strlen(expected_rec->data.ipsec.src)));
	}

	if (expected_rec->data.ipsec.dst[0] != '\0') {
		ipsec_flag = ipsec_flag & 
			!(strncmp(expected_rec->data.ipsec.dst, 
			actual_rec->data.ipsec.dst,
			strlen(expected_rec->data.ipsec.dst)));
	}

	if (expected_rec->data.ipsec.spi[0] != '\0') {
		ipsec_flag = ipsec_flag & 
			!(strncmp(expected_rec->data.ipsec.spi, 
			actual_rec->data.ipsec.spi,
			strlen(expected_rec->data.ipsec.spi)));
	}

	if (expected_rec->data.ipsec.protocol[0] != '\0') {
		ipsec_flag = ipsec_flag & 
			!(strncmp(expected_rec->data.ipsec.protocol, 
			actual_rec->data.ipsec.protocol,
			strlen(expected_rec->data.ipsec.protocol)));
	}

	if (expected_rec->data.ipsec.desc[0] != '\0') {
		ipsec_flag = ipsec_flag & 
			!(strncmp(expected_rec->data.ipsec.desc, 
			actual_rec->data.ipsec.desc,
			strlen(expected_rec->data.ipsec.desc)));
	}

	return !ipsec_flag;
}

/*
 * This function checks expected against actual cups blocks
 *
 * Args:
 *   expected_rec: block expected by test case
 *   actual_rec: block parsed from actual audit record
 *   cups_flag: flag indicating found/not found status
 *
 * Return: 0=found, 1=not found 
 */
int check_cups(rec_part *expected_rec, rec_part *actual_rec, int cups_flag)
{
	if (expected_rec->data.cups.auid != VAL_NOT_SET) {
		cups_flag = cups_flag & (expected_rec->data.cups.auid 
			== actual_rec->data.cups.auid);
	}

	if (expected_rec->data.cups.auid2 != VAL_NOT_SET) {
		cups_flag = cups_flag & (expected_rec->data.cups.auid2 
			== actual_rec->data.cups.auid2);
	}

	if (expected_rec->data.cups.uid != VAL_NOT_SET) {
		cups_flag = cups_flag & (expected_rec->data.cups.uid 
			== actual_rec->data.cups.uid);
	}

	if (expected_rec->data.cups.pid != VAL_NOT_SET) {
		cups_flag = cups_flag & (expected_rec->data.cups.pid 
			== actual_rec->data.cups.pid);
	}

	if (expected_rec->data.cups.job != VAL_NOT_SET) {
		cups_flag = cups_flag & (expected_rec->data.cups.job 
			== actual_rec->data.cups.job);
	}

	if (expected_rec->data.cups.msg[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.msg, 
			actual_rec->data.cups.msg,
			strlen(expected_rec->data.cups.msg)));
	}

	if (expected_rec->data.cups.acct[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.acct, 
			actual_rec->data.cups.acct,
			strlen(expected_rec->data.cups.acct)));
	}

	if (expected_rec->data.cups.printer[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.printer, 
			actual_rec->data.cups.printer,
			strlen(expected_rec->data.cups.printer)));
	}

	if (expected_rec->data.cups.title[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.title, 
			actual_rec->data.cups.title,
			strlen(expected_rec->data.cups.title)));
	}

	if (expected_rec->data.cups.label[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.label, 
			actual_rec->data.cups.label,
			strlen(expected_rec->data.cups.label)));
	}

	if (expected_rec->data.cups.subj[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.subj, 
			actual_rec->data.cups.subj,
			strlen(expected_rec->data.cups.subj)));
	}

	if (expected_rec->data.cups.obj[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.obj, 
			actual_rec->data.cups.obj,
			strlen(expected_rec->data.cups.obj)));
	}

	if (expected_rec->data.cups.exe[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.exe, 
			actual_rec->data.cups.exe,
			strlen(expected_rec->data.cups.exe)));
	}

	if (expected_rec->data.cups.res[0] != '\0') {
		cups_flag = cups_flag & 
			!(strncmp(expected_rec->data.cups.res, 
			actual_rec->data.cups.res,
			strlen(expected_rec->data.cups.res)));
	}

	return !cups_flag;
}

/*
 * This function compares each block of the expected record to the record
 * passed as "actual" which is the record parsed from audit log file
 *
 * Note: the flag variables are opposite to intended return code so that
 *   if any part does not match, the record is not found. Therefore to
 *   account for this, the return statment is the negation of flag variable.
 *   Each type has its own flag, and the result is a bit-wise and of all 
 *   their values
 * The expected argument in this function is the translated version of 
 *   the record passed by the test case.
 *
 * Args:
 *   actual: the record parsed from audit log
 *   exppected: the record we are looking for
 *
 * Return : 0 if record is found, 1 if not found
 */
int compare_rec(main_list *actual, main_list *expected)
{
	/* set a pointer to inner part of the record in the lists */
	rec_part *expected_rec = expected->myrecord;
	rec_part *actual_rec = actual->myrecord;

	int flag = VAL_NOT_SET;
	int path_flag = VAL_NOT_SET;
	int sys_flag = VAL_NOT_SET;
	int config_flag = VAL_NOT_SET;
	int user_flag = VAL_NOT_SET;
	int chauth_flag = VAL_NOT_SET;
	int pol_flag = VAL_NOT_SET;
	int dev_flag = VAL_NOT_SET;
	int rbac_flag = VAL_NOT_SET;
	int avc_flag = VAL_NOT_SET;
	int mac_flag = VAL_NOT_SET;
	int role_chng_flag = VAL_NOT_SET;
	int cipso_flag = VAL_NOT_SET;
	int ipsec_flag = VAL_NOT_SET;
	int cups_flag = VAL_NOT_SET;

	while (expected_rec != NULL) {
		while (actual_rec != NULL) {
			if (strcmp(expected_rec->type,"SYSCALL") == 0 ) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				sys_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					sys_flag = check_syscall(expected_rec, 
						actual_rec, sys_flag);
					if (sys_flag == 0) /* found part */
						break;
				}

			} else if (strcmp(expected_rec->type,"PATH") == 0 ) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				path_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					path_flag = check_path(expected_rec, 
						actual_rec, path_flag);
					if (path_flag == 0) /* found part */
						break;
				}

			} else if (strcmp(expected_rec->type, "CONFIG_CHANGE") == 0 ) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				config_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					config_flag = check_config(expected_rec,
						actual_rec, config_flag);
					if (config_flag == 0) /* found part */
						break;
				}

			} else if ((strcmp(expected_rec->type, "USER_START") == 0) 
				|| (strcmp(expected_rec->type, "USER_ACCT") == 0)
				|| (strcmp(expected_rec->type, "USER") == 0)
				|| (strcmp(expected_rec->type, "USER_AUTH") == 0) 
				|| (strcmp(expected_rec->type, "USER_LOGIN") == 0) ) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				user_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					user_flag = check_user(expected_rec, 
						actual_rec, user_flag);
					if (user_flag == 0) /* found part */
						break;
				}

			} else if ((strcmp(expected_rec->type, "USER_CHAUTHTOK") == 0)
				|| (strcmp(expected_rec->type, "USYS_CONFIG") == 0) ){
				if (flag == VAL_NOT_SET)
					flag = 1;
				chauth_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					chauth_flag = check_chauthtok(expected_rec, actual_rec, chauth_flag);
					if (chauth_flag == 0) /* found part */
						break;
				}

			} else if (strcmp(expected_rec->type, "MAC_POLICY_LOAD") == 0) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				pol_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					pol_flag = check_load_pol(expected_rec, actual_rec, pol_flag);
					if (pol_flag == 0) /* found part */
						break;
				}
			} else if ((strcmp(expected_rec->type, "AUDIT_DEV_ALLOC") == 0)
				|| (strcmp(expected_rec->type, "AUDIT_DEV_DEALLOC") == 0)
				|| (strcmp(expected_rec->type, "AUDIT_USYS_CONFIG") == 0)){
				if (flag == VAL_NOT_SET)
					flag = 1;
				dev_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					dev_flag = check_dev_usys(expected_rec, actual_rec, dev_flag);
					if (dev_flag == 0) /* found part */
						break;
				}
			} else if (strcmp(expected_rec->type, "ANOM_RBAC_FAIL") == 0) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				rbac_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					rbac_flag = check_rbac(expected_rec, actual_rec, rbac_flag);
					if (rbac_flag == 0) /* found part */
						break;
				}
			} else if (strcmp(expected_rec->type, "AVC") == 0) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				avc_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					avc_flag = check_avc(expected_rec, actual_rec, avc_flag);
					if (avc_flag == 0) /* found part */
						break;
				}
			} else if ( (strcmp(expected_rec->type, "MAC_STATUS") == 0)
				|| (strcmp(expected_rec->type, "MAC_CONFIG_CHANGE") == 0)){
				if (flag == VAL_NOT_SET)
					flag = 1;
				mac_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					mac_flag = check_mac(expected_rec, actual_rec, mac_flag);
					if (mac_flag == 0) /* found part */
						break;
				}
			} else if (strcmp(expected_rec->type, "USER_ROLE_CHANGE") == 0) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				role_chng_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					role_chng_flag = check_role_change(expected_rec, actual_rec, role_chng_flag);
					if (role_chng_flag == 0) /* found part */
						break;
				}
			} else if ((strcmp(expected_rec->type, "MAC_CIPSOV4_ADD") == 0)
				|| (strcmp(expected_rec->type, "MAC_CIPSOV4_DEL") == 0)
				|| (strcmp(expected_rec->type, "MAC_MAP_DEL") == 0)
				|| (strcmp(expected_rec->type, "MAC_MAP_ADD") == 0)
				|| (strcmp(expected_rec->type, "MAC_UNLBL_ALLOW") == 0)){
				if (flag == VAL_NOT_SET)
					flag = 1;
				cipso_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					cipso_flag = check_cipso(expected_rec, actual_rec, cipso_flag);
					if (cipso_flag == 0) /* found part */
						break;
				}
			} else if ((strcmp(expected_rec->type, "MAC_IPSEC_ADDSA") == 0)
				|| (strcmp(expected_rec->type, "MAC_IPSEC_DELSA") == 0)
				|| (strcmp(expected_rec->type, "MAC_IPSEC_DELSPD") == 0)
				|| (strcmp(expected_rec->type, "MAC_IPSEC_ADDSPD") == 0)) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				ipsec_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					ipsec_flag = check_ipsec(expected_rec, actual_rec, ipsec_flag);
					if (ipsec_flag == 0) /* found part */
						break;
				}
			} else if ((strcmp(expected_rec->type, "USER_LABELED_EXPORT") == 0)
				|| (strcmp(expected_rec->type, "LABEL_OVERRIDE") == 0)) {
				if (flag == VAL_NOT_SET)
					flag = 1;
				cups_flag = 1;
				if (strcmp(actual_rec->type, expected_rec->type) == 0) {
					cups_flag = check_cups(expected_rec, actual_rec, cups_flag);
					if (cups_flag == 0) /* found part */
						break;
				}
			} else {
				break;
			}
			actual_rec = actual_rec->next_part;
		}

		if (flag == VAL_NOT_SET)
			return 1; /* indicating nothing found */

		/* determine if Found/Not Found */
		if (sys_flag != VAL_NOT_SET)
			flag = flag & !sys_flag;
		if (path_flag != VAL_NOT_SET)
			flag = flag & !path_flag;
		if (config_flag != VAL_NOT_SET)
			flag = flag & !config_flag;
		if (user_flag != VAL_NOT_SET)
			flag = flag & !user_flag;
		if (chauth_flag != VAL_NOT_SET)
			flag = flag & !chauth_flag;
		if (pol_flag != VAL_NOT_SET)
			flag = flag & !pol_flag;
		if (dev_flag != VAL_NOT_SET)
			flag = flag & !dev_flag;
		if (rbac_flag != VAL_NOT_SET)
			flag = flag & !rbac_flag;
		if (avc_flag != VAL_NOT_SET)
			flag = flag & !avc_flag;
		if (mac_flag != VAL_NOT_SET)
			flag = flag & !mac_flag;
		if (role_chng_flag != VAL_NOT_SET)
			flag = flag & !role_chng_flag;
		if (cipso_flag != VAL_NOT_SET)
			flag = flag & !cipso_flag;
		if (ipsec_flag != VAL_NOT_SET)
			flag = flag & !ipsec_flag;
		if (cups_flag != VAL_NOT_SET)
			flag = flag & !cups_flag;

		if (flag == 0) {
			return !flag;	/* indicating failure to find record */
		} else {
			/* restart search from top of list */ 
			actual_rec = actual->myrecord;
			expected_rec = expected_rec->next_part;
		}
	}

	if (flag == VAL_NOT_SET)
		return 1;
	else
		return !flag;
}

/*
 * This function parses the syscall record type of audit log into a block
 * 
 * NOTE:
 *   Special code handles the absense of certain fields in some cases.
 *   Those fields are: success, exit, and item.
 *
 * Return: pointer to head of block
 */
rec_part* parse_syscall(char *type)
{
	rec_part *rec = NULL;
	char *token;
	long long temp_arg;

	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_SYSCALL);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);
	sscanf(extract_value(token), "%lx", &rec->data.syscall.arch);

	token = strtok(NULL, delim);
	rec->data.syscall.syscall_no = atoi(extract_value(token));

	token = strtok(NULL, delim);
	if (token[0] == 's') {  /* we have success and exit fields */
		strcpy(rec->data.syscall.success, extract_value(token));

		token = strtok(NULL, delim);
		rec->data.syscall.exit = atoi(extract_value(token));
		/* multiply by -1 to get rid of the "-" indicating failure */
		if (rec->data.syscall.exit < 0 )
			rec->data.syscall.exit = rec->data.syscall.exit * -1;

		/* scan next token to get the a0 field */
		token = strtok(NULL, delim);
	}
	sscanf(extract_value(token), "%lx", &rec->data.syscall.argv[0]);

	token = strtok(NULL, delim);
	sscanf(extract_value(token), "%lx", &rec->data.syscall.argv[1]);

	token = strtok(NULL, delim);
	sscanf(extract_value(token), "%lx", &rec->data.syscall.argv[2]);

	token = strtok(NULL, delim);
#if defined(__MODE_32) && defined(__S390X)
	sscanf(extract_value(token), "%Lx", &temp_arg);
	rec->data.syscall.argv[3] = 0x00000000FFFFFFFF & temp_arg;
#else
	sscanf(extract_value(token), "%lx", &rec->data.syscall.argv[3]);
#endif

	token = strtok(NULL, delim);
	if (token[0] == 'i' && token[1] == 't') {
		/* we have item field */
		rec->data.syscall.item = atoi(extract_value(token));

		/* scan next token to get the pid field */
		token = strtok(NULL, delim);
	}
	rec->data.syscall.ppid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.pid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.auid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.uid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.gid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.euid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.suid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.fsuid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.egid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.sgid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.syscall.fsgid = atoi(extract_value(token));

	/* skip the tty= field */
	token = strtok(NULL, delim);

	token = strtok(NULL, delim);
	strcpy(rec->data.syscall.comm, extract_value(token));

	token = strtok(NULL, delim);
	if (token[0] == 'e' && token[1] == 'x' && token[2] == 'e') {
		/* we have exe field */
		strcpy(rec->data.syscall.exe, extract_value(token));

		/* scan next token to get the subj field */
		token = strtok(NULL, delim);
	} else {
		#if !SUPPRESS_VERBOSE_OUTPUT
		printf("Syscall record number %d has no exec field",
			rec->data.syscall.syscall_no);
		#endif
	}
	strncpy(rec->data.syscall.subj, extract_value(token), SUBJ_SIZE);
	
	token = strtok(NULL, delim);
	strcpy(rec->data.syscall.key, extract_value(token));

	return rec;
}

/*
 * This function parses the path record type of audit log into a block
 * 
 * NOTE:
 *   Special code handles cases when the records only has name and flas fields
 *   (The goto statement is used to exit the if case if no more tokens 
 *   are parsed).
 *
 * Return: pointer to head of block
 */
rec_part* parse_path(char *type)
{
	rec_part *rec = NULL;
	char *token;

	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_PATH);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	/* We have item field - We don't need it, read next token */
	if ( token[0]=='i' && token[1]=='t' && token[2]=='e') {
		token=strtok(NULL,delim);
	}
	strcpy(rec->data.path.name, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	if (token[0]=='p' && token[1]=='a' && token[2]=='r') {
		rec->data.path.parent = atoi(extract_value(token));
	
		token = strtok(NULL, delim);
	}
	if (token[0]=='i' && token[1]=='n' && token[2]=='o') {
		rec->data.path.inode = atoi(extract_value(token));

		token = strtok(NULL, delim);
	}
	strcpy(rec->data.path.dev, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.path.mode = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.path.ouid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.path.ogid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.path.rdev, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strncpy(rec->data.path.obj, extract_value(token), SUBJ_SIZE);

READ_NEXT:

	return rec;
}

/*
 * This function parses the config record type of audit log into a block
 * 
 * NOTE:
 *   Special code handles the two different types of this record as shown
 *   in the sample records section above.
 *
 * Return: pointer to head of block
 */
rec_part* parse_config(char *type)
{
	rec_part *rec = NULL;
	char *token;

	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_CONFIG);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);
	if (token[2] == 'd' && token[3] == 'i' ) {
		/* we have "audit_enabled=1 old=1 by auid=500" */
		rec->data.config.audit_en = atoi(extract_value(token));

		token = strtok(NULL, delim);
		rec->data.config.old = atoi(extract_value(token));

		/* ignore the "by" word in there */
		token = strtok(NULL, delim);

		token = strtok(NULL, delim);
		rec->data.config.auid = atoi(extract_value(token));

		token = strtok(NULL, delim);
		strncpy(rec->data.config.subj, extract_value(token), SUBJ_SIZE);

	} else if (token[2] == 'i' && token[3] == 'd') {
		/* we have "auid=500 added/Removed an audit rule" */
		rec->data.config.auid = atoi(extract_value(token));

		token = strtok(NULL, delim);
		strncpy(rec->data.config.subj, extract_value(token), SUBJ_SIZE);

		token = strtok(NULL, delim);
		strncpy(rec->data.config.msg, token, AUIDMSG_SIZE);
		while ( (token = strtok(NULL, delim)) != NULL) {
			if ( token[0] == 'r' && token[1] == 'e' && 
			     token[3] == 's') {
				rec->data.config.res = atoi(extract_value(token));
				break; 
			}
			strncat(rec->data.config.msg, " ", AUIDMSG_SIZE);
			strncat(rec->data.config.msg, token, AUIDMSG_SIZE);
		}

	} else {
		tst_resm(TINFO, "Don't know what CONFIG type to parse\n");
	}

	return rec;
}

/*
 * This function parses the user record type of audit log into a block
 * 
 * NOTE:
 *   Multiple type records share the above sample format, however the type might
 *   differ. Other types include: USER_AUTH, USER, USER_START, CRED_REFR, 
 *	USER_ERR.
 *
 * Return: pointer to head of block
 */
rec_part* parse_user(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_USER);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);

	token = strtok(NULL, delim);
	rec->data.user.pid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.user.uid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.user.auid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	strcpy(rec->data.user.subj, extract_value(token));

	token = strtok(NULL, delim);
        /* we have msg='op= */
	if ( token[5] == 'o' && token[6] == 'p' && token[7] == '=') {
		strcpy(rec->data.user.op, extract_special(token));
		token = strtok(NULL, delim);
		rec->data.user.id = atoi(extract_value(token));
	} else {
		strcpy(rec->data.user.msg, extract_value(token));
		while ( (token = strtok(NULL, delim)) != NULL) {
			/* we reached acct= token */
			if ( token[0] == 'a' && token[1] == 'c' && 
				token[3] == 't') {
				strcpy(rec->data.user.user, extract_value(token));
				break; 
		/* we have uid= reset=. We don't care about them, just parse*/
			} else if (token[0]=='u' && token[1]=='i' && 
				token[2]=='d' && token[3]=='=') {
				token = strtok(NULL, delim);
				break; 
			
        		}
			strncat(rec->data.user.msg, " ", AUIDMSG_SIZE);
			strncat(rec->data.user.msg, token, AUIDMSG_SIZE);
		}
	}

	/* if there is a ":" then ignore it */
	token = strtok(NULL,delim);
	if (token[0] == ':') {
		token = strtok(NULL,delim);
	}
	strcpy(rec->data.user.exe, extract_value(token));
		
	token = strtok(NULL, delim);
	strcpy(rec->data.user.host, extract_value(token));

	token = strtok(NULL, delim);
	strcpy(rec->data.user.addr, extract_value(token));

	token = strtok(NULL, delim);
	strcpy(rec->data.user.terminal, extract_value(token));

	token = strtok(NULL, delim);
	strcpy(rec->data.user.result, extract_value(token));
	while ( (token = strtok(NULL, delim)) != NULL) {
		strncat(rec->data.user.result, " ", LOGIN_RESULT_SIZE);
		strncat(rec->data.user.result, token, LOGIN_RESULT_SIZE);
	}

	return rec;
}


/*
 * This function parses the user_login record type of audit log into a block
 * 
 * Return: pointer to head of block
 */
rec_part* parse_user_login(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_USER);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);

	token = strtok(NULL, delim);
	rec->data.user.pid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.user.uid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	rec->data.user.auid = atoi(extract_value(token));

	token = strtok(NULL, delim);
	strcpy(rec->data.user.subj, extract_value(token));

	/* next block parses the following ..
	 * msg='acct=X 
	 * msg='uid=X
	 */
	token = strtok(NULL, delim);
	if ( token[5]=='a' && token[6]=='c' && token[8]=='t' 
	     && token[9]=='=' ) {
		strcpy(rec->data.user.user, extract_special(token));
	} else if (token[5]=='u' && token[6]=='i' && token[7]=='d' 
		   && token[8]=='=') {
		rec->data.user.user_login_uid = atoi(extract_special(token));
	}

	token = strtok(NULL,delim);
	strcpy(rec->data.user.exe, extract_value(token));
		
	token = strtok(NULL, delim);
	strcpy(rec->data.user.host, extract_value(token));

	token = strtok(NULL, delim);
	strcpy(rec->data.user.addr, extract_value(token));

	token = strtok(NULL, delim);
	strcpy(rec->data.user.terminal, extract_value(token));

	token = strtok(NULL, delim);
	strcpy(rec->data.user.result, extract_value(token));

	return rec;
}
/*
 * This function parses the msg string part of the USER_CHAUTHTOK record
 *
 * A slightly complex conditional is needed because that record may look 
 * as either one of the following:
 *	msg='XXX op=XXX id=X exe=XXX
 *	msg='XXX op=XXX acct=XXX exe=XXX
 *	msg='op=XXX id=X exe=XXX
 *	msg='op=XXX acct=XXX exe=XXX
 */
void parse_msg(rec_part **rec, char *tmp_buff)
{
	char *token;

	token = strtok(tmp_buff, delim);
	if ( token[0] == 'o' && token[1] == 'p' && token[2] == '=') {
		strcpy((*rec)->data.chauthtok.op, extract_value(token));
		while ( (token = strtok(NULL, delim)) != NULL) {
		/* We reached acct= token */
			if ( token[0] == 'a' && token[1] == 'c' 
			&& token[2] == 'c' && token[4] == '=') {
				strcpy((*rec)->data.chauthtok.acct, 
					extract_value(token));
				break;
			} else if (token[0] == 'i' && token[1] == 'd' 
				&& token[2] == '=') {
				(*rec)->data.chauthtok.id = atol(extract_value(token));
				break;
			}
			strncat((*rec)->data.chauthtok.op, " ", 
				LOGIN_OP_SIZE);
			strncat((*rec)->data.chauthtok.op, token, 
				LOGIN_OP_SIZE);
		}
	} else {
		strcpy((*rec)->data.chauthtok.msg, token);
		while ( (token = strtok(NULL, delim)) != NULL) {
		/* we reached op= token */
			if ( token[0] == 'o' && token[1] == 'p' 
				&& token[2] == '=') {
				strcpy((*rec)->data.chauthtok.op, 
					extract_value(token));
				while ( (token = strtok(NULL, delim)) != NULL) {
				/* We reached acct= token */
					if ( token[0] == 'a' && token[1] == 'c' 
						&& token[2] == 'c' 
						&& token[4] == '=') {
						strcpy((*rec)->data.chauthtok.acct,
							 extract_value(token));
						break;
					} else if (token[0] == 'i' 
						&& token[1] == 'd' 
						&& token[2] == '=') {
						(*rec)->data.chauthtok.id = 
							atol(extract_value(token));
						break;
					}
					strncat((*rec)->data.chauthtok.op, " ", 
						LOGIN_OP_SIZE);
					strncat((*rec)->data.chauthtok.op, token, 
						LOGIN_OP_SIZE);
				}
				break; 
			}
			strncat((*rec)->data.chauthtok.msg, " ", AUIDMSG_SIZE);
			strncat((*rec)->data.chauthtok.msg, token, AUIDMSG_SIZE);
		}

	}
}

/*
 * This function parses the chauthtok record type of audit log into a block
 * 
 * NOTE:
 *   Special code handles the id/acct fields as shown in the above sample
 *   records.
 *   Also code handles case where "msg" is last thing included in record and 
 *   nothing comes after it. 
 *   Special function parse_msg() is used to handle exception of sometimes
 *   not having a msg value, and having op value directly after msg
 *
 * Return: pointer to head of block
 */
rec_part* parse_chauthtok(char *type)
{
	rec_part *rec = NULL;
	char tmp_buff[300];
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_CHAUTH);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.chauthtok.pid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.chauthtok.uid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.chauthtok.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.chauthtok.subj, extract_value(token));

	/* starting at msg= field, keep reading into temporary 
	   buffer until exe= fields is reached. This is later parsed in
	   parse_msg()
	*/
	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(tmp_buff, extract_value(token));
	while ( (token = strtok(NULL, delim)) != NULL) {
		if ( token[0] == 'e' && token[1] == 'x' && token[2] == 'e') {
			strcpy(rec->data.chauthtok.exe, extract_value(token));
			break;
		}
		strncat(tmp_buff, " ", 300);
		strncat(tmp_buff, token, 300);
	}

	if( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.chauthtok.host, extract_value(token));

	if( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.chauthtok.addr, extract_value(token));

	if( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.chauthtok.terminal, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.chauthtok.res, extract_value(token));
	while ( (token = strtok(NULL, delim)) != NULL) {
		strncat(rec->data.chauthtok.res, " ", LOGIN_RESULT_SIZE);
		strncat(rec->data.chauthtok.res, token, LOGIN_RESULT_SIZE);
	}

READ_NEXT:
	parse_msg(&rec, tmp_buff);
	return rec;
}

rec_part* parse_load_pol(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_LOAD_POL);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);
	strcpy(rec->data.load_pol.auidmsg, token);

	while ( (token = strtok(NULL, delim)) != NULL) {
		/* we reached auid= token */
		if ( token[0] == 'a' && token[1] == 'u' && token[3] == 'd') {
			rec->data.load_pol.auid = atoi(extract_value(token));
			break; 
		}
		strncat(rec->data.load_pol.auidmsg, " ", AUIDMSG_SIZE);
		strncat(rec->data.load_pol.auidmsg, token, AUIDMSG_SIZE);
	}

	return rec;
}

rec_part* parse_rbac(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_RBAC);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.rbac.pid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.rbac.uid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.rbac.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.rbac.subj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.rbac.msg, extract_value(token));
	while ( (token = strtok(NULL, delim)) != NULL) {
		/* we reached exe= token */
		if ( token[0] == 'e' && token[1] == 'x' && token[2] == 'e') {
			strcpy(rec->data.rbac.exe, extract_value(token));
			break; 
		}
		strncat(rec->data.rbac.msg, " ", AUIDMSG_SIZE);
		strncat(rec->data.rbac.msg, token, AUIDMSG_SIZE);
	}
	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.rbac.host, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.rbac.addr, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.rbac.terminal, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.rbac.res, extract_value(token));

READ_NEXT:
	return rec;
}

rec_part* parse_dev_usys(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_DEV_USYS);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.dev_usys.pid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.dev_usys.uid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.dev_usys.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.dev_usys.msg, extract_value(token));
	while ( (token = strtok(NULL, delim)) != NULL) {
		/* We reached device= token */
		if (token[0] == 'd' && token[2] == 'v' 
			&& token[4] == 'c' && token[6] == '=') {
			strcpy(rec->data.dev_usys.dev, extract_value(token));
			break;
		/* We reached op= token */
		} else if ( token[0] == 'o' && token[1] == 'p' && token[2] == '=') { 
			strcpy(rec->data.dev_usys.op, extract_value(token));
			break;
		}
		strcat(rec->data.dev_usys.msg, " ");
		strncat(rec->data.dev_usys.msg, token, AUIDMSG_SIZE);
	}

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.dev_usys.obj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.dev_usys.desc, extract_value(token));
	while ( (token = strtok(NULL, delim)) != NULL) {
		strcat(rec->data.dev_usys.desc, " ");
		strncat(rec->data.dev_usys.desc, token, AUIDMSG_SIZE);
	}

READ_NEXT:
	return rec;
}

rec_part* parse_avc(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_AVC);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strncpy(rec->data.avc.desc, token, AUIDMSG_SIZE);
	while ( (token = strtok(NULL, delim)) != NULL) {
		/* We reached pid= token */
		if (token[0] == 'p' && token[1] == 'i' 
			&& token[2] == 'd' && token[3] == '=') {
			rec->data.avc.pid = atoi(extract_value(token));
			break;
		}
		strcat(rec->data.avc.desc, " ");
		strncat(rec->data.avc.desc, token, AUIDMSG_SIZE);
	}

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strncpy(rec->data.avc.comm, extract_value(token), COMM_SIZE);

        /* keep reading until we reach scontext= token */
        while ( ((token = strtok(NULL, delim)) != NULL) &&
                (token[0] != 's') && (token[1] != 'c') && (token[3] != 'n') ){
                /* We reached name= token */
                if ( token[0] == 'n' && token[1] == 'a' && token[2] == 'm' ) {
                        strcpy(rec->data.avc.name, extract_value(token));
                /* We reached dev= token */
                } else if (token[0] == 'd' && token[1] == 'e' && token[2] == 'v') {
                        strcpy(rec->data.avc.dev, extract_value(token));
                /* We reached ino= token */
                } else if (token[0] == 'i' && token[1] == 'n' && token[2] == 'o') {
                        rec->data.avc.ino = atoi(extract_value(token));
                }
        }
	/* when we exit the while loop, token has scontext in it, so fill in
	   that field in the record block */
	strncpy(rec->data.avc.scontext, extract_value(token), SUBJ_SIZE);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strncpy(rec->data.avc.tcontext, extract_value(token), SUBJ_SIZE);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strncpy(rec->data.avc.tclass, extract_value(token), TCLASS_SIZE);

READ_NEXT:
	return rec;
}

rec_part* parse_mac(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_MAC);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

/* if we have a MAC_CONFIG_CHANGE then the first field is the boolean name */
	if ( strcmp(rec->type, "MAC_CONFIG_CHANGE") == 0 ) {
		if ( (token = strtok(NULL, delim)) == NULL)
			goto READ_NEXT;
		strncpy(rec->data.mac.bool_name, extract_value(token), BOOL_SIZE);
	}

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.mac.curr_val = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.mac.old_val = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.mac.auid = atoi(extract_value(token));

READ_NEXT:
	return rec;
}

/*
 * This function parses the msg string part of the USER_ROLE_CHANGE record
 *
 * A slightly complex conditional is needed because that record may look 
 * as either one of the following:
 *	msg='X old_context=X new_context=X
 *	msg='op=X acct=X old_user=X old_role=X old_range=X new_user=X
 *		new_role=X new_range=X
 */
void parse_role_msg(rec_part **rec, char *tmp_buff)
{
	char *token;

	token = strtok(tmp_buff, delim);
	if ( token[0] == 'o' && token[1] == 'p' && token[2] == '=') {
	/* we have msg='op= field */
		strcpy((*rec)->data.role_chng.op, extract_value(token));
		while ( (token = strtok(NULL, delim)) != NULL) {
		/* We reached acct= token */
			if ( token[0] == 'a' && token[1] == 'c' 
			&& token[2] == 'c' && token[4] == '=') {
				strcpy((*rec)->data.role_chng.acct, 
					extract_value(token));
				break;
			}
			strncat((*rec)->data.role_chng.op, " ", 
				LOGIN_OP_SIZE);
			strncat((*rec)->data.role_chng.op, token, 
				LOGIN_OP_SIZE);
		}
		token = strtok(NULL, delim);
		strcpy((*rec)->data.role_chng.old_user, extract_value(token));
		
		token = strtok(NULL, delim);
		strcpy((*rec)->data.role_chng.old_role, extract_value(token));
		
		token = strtok(NULL, delim);
		strcpy((*rec)->data.role_chng.old_range, extract_value(token));
		
		token = strtok(NULL, delim);
		strcpy((*rec)->data.role_chng.new_user, extract_value(token));
		
		token = strtok(NULL, delim);
		strcpy((*rec)->data.role_chng.new_role, extract_value(token));
		
		token = strtok(NULL, delim);
		strcpy((*rec)->data.role_chng.new_range, extract_value(token));
		
	} else {
	/* we have msg='X: default-context=... selected-context=... 
	   or msg='X: old-context= ... new-context=... */
		strcpy((*rec)->data.role_chng.msg, token);
		token = strtok(NULL, delim);
		/* we reached default-context=  selected-context= tokens */
		if (token[0] == 'd' && token[1] == 'e' && token[2] == 'f' 
		    && token[3]=='a') {
			strcpy((*rec)->data.role_chng.def_cntx, 
				extract_value(token));

			token = strtok(NULL, delim);
			strcpy((*rec)->data.role_chng.sel_cntx, 
				extract_value(token));

		} else if (token[0] == 'o' && token[1] == 'l' 
			&& token[3] == '-' && token[4]=='c') {
	   	/* we reached old-context= new-context= tokens*/
			strcpy((*rec)->data.role_chng.old_cntx, 
			extract_value(token));

			token = strtok(NULL, delim);
			strcpy((*rec)->data.role_chng.new_cntx, 
				extract_value(token));
		} else {
			printf("USER_ROLE_CHANGE - Unknown audit format\n");
		}
	}
}

/*
 * This function parses the user_role_change record type into a block
 * 
 * NOTE:
 *   Special function parse_role_msg() is used to handle exception of sometimes
 *   not having a msg value, and having op value directly after msg
 *
 * Return: pointer to head of block
 */
rec_part* parse_role_change(char *type)
{
	rec_part *rec = NULL;
	char tmp_buff[300];
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_ROLE_CHANGE);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.role_chng.pid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.role_chng.uid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.role_chng.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	if ( (token[0]=='s') && (token[2]=='b') && (token[3]=='j') ) {
		/* We have subj= field */
		strcpy(rec->data.role_chng.subj, extract_value(token));
		token = strtok(NULL, delim);
	}

	if ((token[0]=='m') && (token[1]=='s') && (token[2]=='g') ) {
		/* starting at msg= field, keep reading into temporary 
	   	buffer until exe= fields is reached. This is later parsed in
	   	parse_role_msg()
		*/
		strncpy(tmp_buff, extract_value(token), 300);
		while ( (token = strtok(NULL, delim)) != NULL) {
			if ( token[0] == 'e' && token[1] == 'x' && token[2] == 'e') {
				strcpy(rec->data.role_chng.exe, extract_value(token));
				break;
			}
			strncat(tmp_buff, " ", 300);
			strncat(tmp_buff, token, 300);
		}
	}

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.role_chng.host, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.role_chng.addr, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.role_chng.terminal, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.role_chng.res, extract_value(token));
	
READ_NEXT:
	parse_role_msg(&rec, tmp_buff);
	return rec;
}

/*
 * This function parses the cipsov4-add record type into a block
 * 
 * Return: pointer to head of block
 */
rec_part* parse_cipso(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_CIPSO);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);
	strcpy(rec->data.cipso.tool, token);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cipso.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cipso.subj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cipso.cipso_doi = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	if ( (token[0]=='c') && (token[2]=='p') && (token[6]=='t') ) {
		/* We have cipso_type= field */
		strcpy(rec->data.cipso.cipso_type, extract_value(token));
		token = strtok(NULL, delim);	// we get next token 
	}

	/* we have res value in token at this point */
	if ( token  == NULL )
		goto READ_NEXT;
	rec->data.cipso.res = atoi(extract_value(token));

READ_NEXT:
	return rec;
}

/*
 * This function parses the map-add and map-del record type into a block
 * 
 * Return: pointer to head of block
 */
rec_part* parse_map_add_del(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_CIPSO);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);
	strcpy(rec->data.cipso.tool, token);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cipso.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cipso.subj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cipso.nlbl_domain, extract_value(token));

        /* keep reading until we reach res= token */
        while ( ((token = strtok(NULL, delim)) != NULL) &&
                (token[0] != 'r') && (token[1] != 'e') && (token[2] != 's') ){
                /* We reached nlbl_protocol= token */
                if ( token[0] == 'n' && token[1] == 'l' && token[5] == 'p' ) {
                        strcpy(rec->data.cipso.nlbl_protocol, extract_value(token));
                /* We reached cipso_doi= token */
                } else if (token[0] == 'c' && token[1] == 'i' && token[6] == 'd') {
                        rec->data.cipso.cipso_doi = atoi(extract_value(token));
		}
        }
	/* when we exit the while loop, token has res in it, so fill in
	   that field in the record block */
	rec->data.cipso.res = atoi(extract_value(token));

READ_NEXT:
	return rec;
}

/*
 * This function parses the unlbl-allow record type into a block
 * 
 * Return: pointer to head of block
 */
rec_part* parse_unlbl_allow(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_CIPSO);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);
	strcpy(rec->data.cipso.tool, token);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cipso.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cipso.subj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cipso.unlbl_accept = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cipso.old = atoi(extract_value(token));

READ_NEXT:
	return rec;
}

/*
 * This function parses the ipsec_addsa, ipsec_delsa,ipsec_addspd, and 
 *    ipsec_delspd record type into a block
 * 
 * Return: pointer to head of block
 */
rec_part* parse_ipsec(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_IPSEC);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	/* copying the description SAD add/delete or SPD add/delete */
	strncpy(rec->data.ipsec.desc, strtok(NULL, delim), AUIDMSG_SIZE);
	strncat(rec->data.ipsec.desc, " ", AUIDMSG_SIZE);
	strncat(rec->data.ipsec.desc, strtok(NULL, delim), AUIDMSG_SIZE);

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.ipsec.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.ipsec.subj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.ipsec.sec_alg = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.ipsec.sec_doi = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.ipsec.sec_obj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.ipsec.src, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.ipsec.dst, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	if ( (token[0]=='s') && (token[1]=='p') && (token[2]=='i') ) {
		/* Dealing with SAD add/del, we have spi= and protocol= */
		strcpy(rec->data.ipsec.spi, extract_value(token));
		if ( (token = strtok(NULL, delim)) == NULL)
			goto READ_NEXT;
		strcpy(rec->data.ipsec.protocol, extract_value(token));

		token = strtok(NULL, delim);	// we get next token res=
	}
	rec->data.ipsec.res = atoi(extract_value(token));

READ_NEXT:
	return rec;
}

/*
 * This function parses the user_labeled_export (cups) block  
 * 
 * Return: pointer to head of block
 */
rec_part* parse_user_labeled_export(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_CUPS);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);
	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cups.pid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cups.uid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cups.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.subj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cups.job = atoi(extract_special(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cups.auid2 = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.acct, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	if ( (token[0]=='p') && (token[1]=='r') && (token[2]=='i') ) {
		/* Success case: we have printer= title= obj= label= */
		strcpy(rec->data.cups.printer, extract_value(token));

		if ( (token = strtok(NULL, delim)) == NULL)
			goto READ_NEXT;
		strcpy(rec->data.cups.title, extract_value(token));

		if ( (token = strtok(NULL, delim)) == NULL)
			goto READ_NEXT;
		strcpy(rec->data.cups.obj, extract_value(token));

		if ( (token = strtok(NULL, delim)) == NULL)
			goto READ_NEXT;
		strcpy(rec->data.cups.label, extract_value(token));

	} else {
		/* Fail case: we have obj= <msg> printer= */
		strcpy(rec->data.cups.obj, extract_value(token));

		if ( (token = strtok(NULL, delim)) == NULL)
			goto READ_NEXT;
		strcpy(rec->data.cups.msg, token);
		while ( (token = strtok(NULL, delim)) != NULL) {
			if ( token[0] == 'p' && token[1] == 'r' && token[2] == 'i') {
				strcpy(rec->data.cups.printer, extract_value(token));
				break;
			}
			strncat(rec->data.cups.msg, " ", AUIDMSG_SIZE);
			strncat(rec->data.cups.msg, token, AUIDMSG_SIZE);
		}
	}

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.exe, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.host, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.addr, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.terminal, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.res, extract_value(token));

READ_NEXT:
	return rec;
}

/*
 * This function parses the label_override (cups) block  
 * 
 * Return: pointer to head of block
 */
rec_part* parse_label_override(char *type)
{
	rec_part *rec = NULL;
	char *token;
	if ( (rec=(rec_part *)malloc(sizeof(rec_part))) == NULL ) {
		tst_resm(TINFO, "Memory not allocated\n");
		return rec;
	}
	initialize_rec_part(rec, INIT_CUPS);
	strncpy(rec->type, type, AUDIT_TYPE_SIZE);

	token = strtok(NULL, delim);
	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cups.pid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cups.uid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	rec->data.cups.auid = atoi(extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.subj, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.msg, extract_value(token));
	while ( (token = strtok(NULL, delim)) != NULL) {
		if ( token[0] == 'e' && token[1] == 'x' && token[2] == 'e') {
			strcpy(rec->data.cups.exe, extract_value(token));
			break;
		}
		strncat(rec->data.cups.msg, " ", AUIDMSG_SIZE);
		strncat(rec->data.cups.msg, token, AUIDMSG_SIZE);
	}

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.host, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.addr, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.terminal, extract_value(token));

	if ( (token = strtok(NULL, delim)) == NULL)
		goto READ_NEXT;
	strcpy(rec->data.cups.res, extract_value(token));

READ_NEXT:
	return rec;
}

/*
 * This function parses the audit log and fills a list like structure
 * The function calls specialized parsing functions to parse specific
 *    record types.
 *
 * Args: None
 *
 * Return: a pointer to the head of the parsed list of audit records
 */
main_list* parse_audit_log(time_t start, time_t end, int *no_record) {
	char buf[BUFF_SIZE];
	char tempbuf[BUFF_SIZE];
	char type[AUDIT_TYPE_SIZE];
	char time[TIME_SIZE];
	char *token;
	int do_nothing;
	main_list *ret_list = NULL;
	main_list *new_rec = NULL;
	main_list *parsed_logs = NULL;
	rec_part *rec = NULL;
	rec_part *head = NULL;
	FILE *log_file;
	unsigned long audit_time = 0;
	unsigned long audit_serial = 0;

	log_file = fopen(get_audit_log_filename(), "r");
	if (log_file == NULL) {
		tst_resm(TINFO, "Can't open log file\n");
		return NULL;
	}

	while ( fgets(buf, sizeof(buf), log_file) != NULL ) {
		do_nothing = 0;
		strncpy(tempbuf, buf, sizeof(buf));

		token = strtok(tempbuf, delim);
		strncpy(type, extract_value(token), AUDIT_TYPE_SIZE);

		token = strtok(NULL, delim);
		strcpy(time, token);
		audit_time = extract_time(time);
		audit_serial = extract_serial(time);

		/* If time is specified, then use it as a range to parse,
		 * if not specified at all then parse (Disk Handling)
		 */
		if ( ((start != VAL_NOT_SET) && (audit_time < start)) 
		   || ((end != VAL_NOT_SET) && (audit_time > end)) ) {

		/* tst_resm(TINFO, "Time out of range ... not parsing\n"); */
			do_nothing = 1;
			*no_record = 1;
		} else if( strcmp(type, "SYSCALL") == 0 ) {
			rec = parse_syscall(type);

		} else if (strcmp(type, "PATH") == 0 ) {
			rec = parse_path(type);

		} else if (strcmp(type, "CONFIG_CHANGE") == 0 ) {
			rec = parse_config(type);

		} else if ((strcmp(type, "USER_START") == 0) 
			|| (strcmp(type, "USER_AUTH") == 0) 
			|| (strcmp(type, "USER_ACCT") == 0) 
			|| (strcmp(type, "USER") == 0) ){
			rec = parse_user(type);

		} else if (strcmp(type, "USER_LOGIN") == 0 ) {
			rec = parse_user_login(type);

		} else if ((strcmp(type, "USER_CHAUTHTOK") == 0) 
			|| (strcmp(type, "USYS_CONFIG") == 0) ){
			rec = parse_chauthtok(type);

		} else if ((strcmp(type, "AUDIT_DEV_ALLOC") == 0) 
			|| (strcmp(type, "AUDIT_DEV_DEALLOC") == 0)
			|| (strcmp(type, "USYS_CONFIG") == 0) ){
			rec = parse_dev_usys(type);

		} else if (strcmp(type, "ANOM_RBAC_FAIL") == 0 ) {
			rec = parse_rbac(type);

		} else if (strcmp(type, "MAC_POLICY_LOAD") == 0 ) {
			rec = parse_load_pol(type);

		} else if (strcmp(type, "AVC") == 0 ) {
			rec = parse_avc(type);

		} else if ( (strcmp(type, "MAC_STATUS") == 0 ) 
			|| (strcmp(type, "MAC_CONFIG_CHANGE") == 0) ){
			rec = parse_mac(type);

		} else if (strcmp(type, "USER_ROLE_CHANGE") == 0 ) {
			rec = parse_role_change(type);

		} else if ( (strcmp(type, "MAC_CIPSOV4_ADD") == 0 ) 
			|| (strcmp(type, "MAC_CIPSOV4_DEL") == 0) ){
			rec = parse_cipso(type);

		} else if ((strcmp(type, "MAC_MAP_ADD") == 0 ) 
			|| (strcmp(type, "MAC_MAP_DEL") == 0 )){
			rec = parse_map_add_del(type);

		} else if (strcmp(type, "MAC_UNLBL_ALLOW") == 0 ) {
			rec = parse_unlbl_allow(type);

		} else if ((strcmp(type, "MAC_IPSEC_ADDSA") == 0) 
			|| (strcmp(type, "MAC_IPSEC_DELSA") == 0) 
			|| (strcmp(type, "MAC_IPSEC_ADDSPD") == 0) 
			|| (strcmp(type, "MAC_IPSEC_DELSPD") == 0) ) {
			rec = parse_ipsec(type);

		} else if (strcmp(type, "USER_LABELED_EXPORT") == 0 ) {
			rec = parse_user_labeled_export(type);

		} else if (strcmp(type, "LABEL_OVERRIDE") == 0 ) {
			rec = parse_label_override(type);

		} else {
			do_nothing = 1;
			*no_record = 1;
		}

		if (do_nothing) {
			/* tst_resm(TINFO, "Ignoring type %s\n", type); */
		} else {
			ret_list = search_for_serial(parsed_logs, audit_serial);
			if ( ret_list == NULL ){
				/* create new node in list */
				new_rec = (main_list *)malloc(sizeof(main_list));
				if (new_rec == NULL) {
					tst_resm(TINFO, "Error allocating memory\n");
					/** TODO: Is this what we want to do here? */
					return parsed_logs;
				}

				new_rec->rec_time = audit_time;
				new_rec->rec_serial = audit_serial;
				new_rec->myrecord = rec;
				new_rec->next_record = NULL;
				parsed_logs = append_list(parsed_logs, new_rec);
			} else {
				/* pass reference to ret_list, so we can 
				append to it's inner list */
				head = append_record(&ret_list, rec);
			}
		}
	}       /* end while loop */

	fclose(log_file);
	return parsed_logs;

}

/**
 * Translates the testcore defined audit record structure into what the
 * parsing API requires and calls the parsing API accordingly. The actual
 * translation of the structure is done in translate_for_parser().
 *
 * @return      Zero on success, non-zero otherwise
 */
int parse_for_record(struct audit_record *src)
{
	int rc = -1;
	int no_record = 0;	/* indicates no relevant records were found */
	main_list *list = NULL;
	main_list *parsed = NULL;
	long start_time = VAL_NOT_SET;
	long end_time = VAL_NOT_SET;

	/* translate flat structure provided by test case into a structure */
	list = translate_for_parser(src);
	if (list == NULL) {
		tst_resm(TINFO, "Error translating record\n");
		return 1;
	}

	/* if time is specified by test case, then expand search window */
	if (src->start_time != VAL_NOT_SET) {
		start_time = src->start_time - LOG_TIME_VARIANCE;
	}
	if (src->end_time != VAL_NOT_SET) {
		end_time = src->end_time + LOG_TIME_VARIANCE;
	}

	/* parse audit log file */
	parsed = parse_audit_log(start_time, end_time, &no_record);

	/* no_record=1 indicates we went through the parse function, but 
	 * did not find any records of a type we care about, therefore this 
	 * with a null value for "parsed" indicates pass when we are not 
	 * expecting any records */
	if ( (parsed == NULL) && (no_record == 1) )
	{
		tst_resm(TINFO, "No relevant records found\n");
		cleanup_mem(&list);
		return no_record; /* return=1 indicating no match found */
	}

	if (parsed == NULL)
	{
		tst_resm(TINFO, "Error parsing audit log\n");
		cleanup_mem(&list);
		return 1;
	}

	#if !SUPPRESS_VERBOSE_OUTPUT
	{
		main_list *tmp = NULL;
		printf("\n>>> Printing Translated Record ...");
		print_list(list->myrecord);

		printf("\n\n>>> Printing Parsed Record ...");
		tmp = parsed;
		while (tmp != NULL) {
			printf("\n\nTime %ld - Serial_No %ld",tmp->rec_time, 
				tmp->rec_serial);
			print_list(tmp->myrecord);
			tmp = tmp->next_record;
		}
		printf("\n --- Done Printing Parsed Records ---\n");
	}
	#endif

	/* Search for our expected record */
	rc = search_for_record(parsed, start_time, list, end_time);

	/* cleanup the memory used by the structures */
	cleanup_mem(&list);
	cleanup_mem(&parsed);

	return rc;
}

/*
 * This function extracts the value from its input argument token
 *
 * Args:
 *  token: a string in the form of field=value
 *
 * Return: the extracted value
 */
char *extract_value(char *token)
{
	int index, value_index = 0, k = 0;

	if (token == NULL) {
		return token;
	}

	while (token[k++] != '=');
	for (index = k; index < (int) strlen(token); index++) 
	{
		/* to remove special characters from the value field */
		if (token[index] == '"' || token[index] == '\'' 
			|| token[index] == ',' || token[index] == ')' 
			|| token[index] == '\n' )
			continue;
		value[value_index++] = token[index];
	}
	value[value_index] = '\0';

/* This will print lots of info even in verbose mode - 
 * uncomment only if you really need to
	#if !SUPPRESS_VERBOSE_OUTPUT
		printf("Extracted Value = %s\n", value);
	#endif
*/

	return value;
}

/*
 * This function extracts the value from its input argument token
 * a special function was needed when we had two "=" in the token
 *
 * Args:
 *  token: a string in the form of field='field=value
 *
 * Return: the extracted value
 */
char *extract_special(char *token)
{
	int index, value_index = 0, k = 0;

	if (token == NULL) {
		return token;
	}

	while (token[k++] != '=');	/* get past first "=" */
	while (token[k++] != '=');	/* get past second "=" */
	for (index = k; index < (int) strlen(token); index++) 
	{
		/* to remove special characters from the value field */
		if (token[index] == '"' || token[index] == '\'' 
			|| token[index] == ',' || token[index] == ')' 
			|| token[index] == ':' || token[index] == '\n')
			continue;
		value[value_index++] = token[index];
	}
	value[value_index] = '\0';

	return value;
}

/*
 * This function extracts the time from input argument
 *
 * Args:
 *  time: a string in the form of msg=audit(1138189639.402:2828) 
 *
 * Return: the extracted value (The value 1138189639 from above sample input)
 */
unsigned long extract_time(char *time)
{
	int i = 0, index, time_index = 0;
	unsigned long audit_time;
	char tmp_time[25]; /* array long enough to hold record time */

	while (time[i++] != '('); /* go through until first '(' character */
	for (index = i; index < (int) strlen(time); index++)
	{
		if (time[index] == '.')
			break;
		else
			tmp_time[time_index++] = time[index];
	}
	tmp_time[time_index] = '\0';
	audit_time = atol(tmp_time);
	return audit_time;
}

/*
 * This function extracts the serial number from input argument
 *
 * Args:
 *  token: a string in the form of msg=audit(1138189639.402:2828) 
 *
 * Return: the extracted value (The value 2828 from above sample input)
 */
unsigned long extract_serial(char *token)
{
	int i=0, index, serial_index = 0;
	char tmp_serial[25]; /* array long enough to hold record serial */
	unsigned long audit_serial;

	while (token[i++] != ':');	/* go until first ':' character */
	for (index = i; index < (int) strlen(token); index++) 
	{
		if (token[index] == ')')
			break;
		else
			tmp_serial[serial_index++] = token[index];
	}
	tmp_serial[serial_index] = '\0';
	audit_serial = atol(tmp_serial);
	return audit_serial;
}
