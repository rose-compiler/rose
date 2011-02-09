/*
 *   Copyright (C) International Business Machines  Corp., 2006, 2007
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
 * File: audit_parse_verify_utils.c
 *
 * Overview: This file contains the helper functions for the audit_parse_verify
 *	file. These functions do the behind the scene work to handle the list
 *	structure of the record. Mostly these functions don't need to be changed.
 *
 ********************************** HISTORY ***********************************
 *
 * DATE         NAME                    DESCRIPTION
 * 01/10/07   loulwa@us.ibm.com  Added support for cups audit records
 * 01/03/07   loulwa@us.ibm.com  Added support for ipsec audit records
 * 09/20/06   loulwa@us.ibm.com  Modifications to init/print mac_config_change
 * 09/19/06   loulwa@us.ibm.com  Added init and print for user_role_change
 * 08/23/06   loulwa@us.ibm.com  Changes to handle not checking time range 
 *                               for disk handling tests
 * 08/16/06   loulwa@us.ibm.com   Added init and prints for AVC and MAC_STATUS
 *				  Removed watch related init and print
 * 08/08/06   loulwa@us.ibm.com   Added inits and prints for new record types
 * 05/01/06   loulwa@us.ibm.com   Addition of new fields
 * 01/17/06   loulwa@us.ibm.com   Originated
 *
 *****************************************************************************/
#include "audit_parse_verify.h"

/*
 * This function initializes the record appropriate record part before it
 * is filled with the translated values provided by the test case.
 * This way, only fields set by the test case are checked during the record
 * comparison function.
 *
 * Args:
 * rec: record part to be initialized
 * type: indicates which record type to initialize
 *      Possible: syscall, path, fs_watch, config, user, chauth
 *
 * Return : Nothing
 */
void
initialize_rec_part (rec_part * rec, int type)
{
  rec->next_part = NULL;

  switch (type)
    {
    case INIT_SYSCALL:
      rec->data.syscall.syscall_no = VAL_NOT_SET;
      rec->data.syscall.arch = VAL_NOT_SET;
      rec->data.syscall.exit = VAL_NOT_SET;
      rec->data.syscall.ppid = VAL_NOT_SET;
      rec->data.syscall.pid = VAL_NOT_SET;
      rec->data.syscall.auid = VAL_NOT_SET;
      rec->data.syscall.uid = VAL_NOT_SET;
      rec->data.syscall.gid = VAL_NOT_SET;
      rec->data.syscall.euid = VAL_NOT_SET;
      rec->data.syscall.suid = VAL_NOT_SET;
      rec->data.syscall.fsuid = VAL_NOT_SET;
      rec->data.syscall.egid = VAL_NOT_SET;
      rec->data.syscall.sgid = VAL_NOT_SET;
      rec->data.syscall.fsgid = VAL_NOT_SET;
      rec->data.syscall.argv[0] = VAL_NOT_SET;
      rec->data.syscall.argv[1] = VAL_NOT_SET;
      rec->data.syscall.argv[2] = VAL_NOT_SET;
      rec->data.syscall.argv[3] = VAL_NOT_SET;
      memset (rec->data.syscall.success, 0, SUCCESS_SIZE);
      memset (rec->data.syscall.comm, 0, COMM_SIZE);
      memset (rec->data.syscall.exe, 0, EXE_SIZE);
      memset (rec->data.syscall.subj, 0, SUBJ_SIZE);
      memset (rec->data.syscall.key, 0, WATCH_KEY_SIZE);
      break;

    case INIT_PATH:
      memset (rec->data.path.name, 0, OBJ_NAME_SIZE);
      memset (rec->data.path.dev, 0, 10);
      memset (rec->data.path.rdev, 0, 10);
      rec->data.path.inode = VAL_NOT_SET;
      rec->data.path.parent = VAL_NOT_SET;
      rec->data.path.flags = VAL_NOT_SET;
      rec->data.path.mode = VAL_NOT_SET;
      rec->data.path.ouid = VAL_NOT_SET;
      rec->data.path.ogid = VAL_NOT_SET;
      memset (rec->data.path.obj, 0, SUBJ_SIZE);
      break;

    case INIT_CONFIG:
      rec->data.config.audit_en = VAL_NOT_SET;
      rec->data.config.old = VAL_NOT_SET;
      rec->data.config.auid = VAL_NOT_SET;
      rec->data.config.res = VAL_NOT_SET;
      memset (rec->data.config.msg, 0, AUIDMSG_SIZE);
      memset (rec->data.config.subj, 0, SUBJ_SIZE);
      break;

    case INIT_USER:
      rec->data.user.pid = VAL_NOT_SET;
      rec->data.user.id = VAL_NOT_SET;
      rec->data.user.uid = VAL_NOT_SET;
      rec->data.user.auid = VAL_NOT_SET;
      rec->data.user.user_login_uid = VAL_NOT_SET;
      memset (rec->data.user.msg, 0, AUIDMSG_SIZE);
      memset (rec->data.user.op, 0, LOGIN_OP_SIZE);
      memset (rec->data.user.addr, 0, LOGIN_ADDRESS_SIZE);
      memset (rec->data.user.terminal, 0, LOGIN_TERM_SIZE);
      memset (rec->data.user.result, 0, LOGIN_RESULT_SIZE);
      memset (rec->data.user.host, 0, LOGIN_HOST_SIZE);
      memset (rec->data.user.subj, 0, SUBJ_SIZE);
      break;

    case INIT_CHAUTH:
      rec->data.chauthtok.pid = VAL_NOT_SET;
      rec->data.chauthtok.uid = VAL_NOT_SET;
      rec->data.chauthtok.auid = VAL_NOT_SET;
      rec->data.chauthtok.id = VAL_NOT_SET;
      memset (rec->data.chauthtok.msg, 0, AUIDMSG_SIZE);
      memset (rec->data.chauthtok.acct, 0, LOGIN_ACCT_SIZE);
      memset (rec->data.chauthtok.op, 0, LOGIN_OP_SIZE);
      memset (rec->data.chauthtok.res, 0, LOGIN_RESULT_SIZE);
      memset (rec->data.chauthtok.subj, 0, SUBJ_SIZE);
      memset (rec->data.chauthtok.terminal, 0, LOGIN_TERM_SIZE);
      memset (rec->data.chauthtok.addr, 0, LOGIN_ADDRESS_SIZE);
      memset (rec->data.chauthtok.host, 0, LOGIN_HOST_SIZE);
      break;

    case INIT_LOAD_POL:
      rec->data.load_pol.auid = VAL_NOT_SET;
      memset (rec->data.load_pol.auidmsg, 0, AUIDMSG_SIZE);
      break;

    case INIT_RBAC:
      rec->data.rbac.uid = VAL_NOT_SET;
      rec->data.rbac.pid = VAL_NOT_SET;
      rec->data.rbac.auid = VAL_NOT_SET;
      memset (rec->data.rbac.msg, 0, AUIDMSG_SIZE);
      memset (rec->data.rbac.subj, 0, SUBJ_SIZE);
      memset (rec->data.rbac.exe, 0, EXE_SIZE);
      memset (rec->data.rbac.host, 0, LOGIN_HOST_SIZE);
      memset (rec->data.rbac.addr, 0, LOGIN_ADDRESS_SIZE);
      memset (rec->data.rbac.terminal, 0, LOGIN_TERM_SIZE);
      memset (rec->data.rbac.res, 0, LOGIN_RESULT_SIZE);
      break;

    case INIT_DEV_USYS:
      rec->data.dev_usys.uid = VAL_NOT_SET;
      rec->data.dev_usys.pid = VAL_NOT_SET;
      rec->data.dev_usys.auid = VAL_NOT_SET;
      memset (rec->data.dev_usys.msg, 0, AUIDMSG_SIZE);
      memset (rec->data.dev_usys.obj, 0, SUBJ_SIZE);
      memset (rec->data.dev_usys.op, 0, LOGIN_OP_SIZE);
      memset (rec->data.dev_usys.dev, 0, DEV_SIZE);
      memset (rec->data.dev_usys.desc, 0, AUIDMSG_SIZE);
      break;

    case INIT_AVC:
      rec->data.avc.pid = VAL_NOT_SET;
      memset (rec->data.avc.desc, 0, AUIDMSG_SIZE);
      memset (rec->data.avc.comm, 0, COMM_SIZE);
      memset (rec->data.avc.dev, 0, DEV_SIZE);
      memset (rec->data.avc.name, 0, NAME_SIZE);
      memset (rec->data.avc.scontext, 0, SUBJ_SIZE);
      memset (rec->data.avc.tcontext, 0, SUBJ_SIZE);
      memset (rec->data.avc.tclass, 0, TCLASS_SIZE);
      break;

    case INIT_MAC:
      memset (rec->data.mac.bool_name, 0, BOOL_SIZE);
      rec->data.mac.auid = VAL_NOT_SET;
      rec->data.mac.curr_val = VAL_NOT_SET;
      rec->data.mac.old_val = VAL_NOT_SET;
      break;

    case INIT_ROLE_CHANGE:
      rec->data.role_chng.pid = VAL_NOT_SET;
      rec->data.role_chng.uid = VAL_NOT_SET;
      rec->data.role_chng.auid = VAL_NOT_SET;
      memset (rec->data.role_chng.msg, 0, AUIDMSG_SIZE);
      memset (rec->data.role_chng.subj, 0, SUBJ_SIZE);
      memset (rec->data.role_chng.def_cntx, 0, SUBJ_SIZE);
      memset (rec->data.role_chng.sel_cntx, 0, SUBJ_SIZE);
      memset (rec->data.role_chng.old_cntx, 0, SUBJ_SIZE);
      memset (rec->data.role_chng.new_cntx, 0, SUBJ_SIZE);
      memset (rec->data.role_chng.old_user, 0, USER_SIZE);
      memset (rec->data.role_chng.old_role, 0, ROLE_SIZE);
      memset (rec->data.role_chng.old_range, 0, RANGE_SIZE);
      memset (rec->data.role_chng.new_user, 0, USER_SIZE);
      memset (rec->data.role_chng.new_role, 0, ROLE_SIZE);
      memset (rec->data.role_chng.new_range, 0, RANGE_SIZE);
      memset (rec->data.role_chng.op, 0, LOGIN_OP_SIZE);
      memset (rec->data.role_chng.acct, 0, LOGIN_ACCT_SIZE);
      memset (rec->data.role_chng.exe, 0, EXE_SIZE);
      memset (rec->data.role_chng.host, 0, LOGIN_HOST_SIZE);
      memset (rec->data.role_chng.addr, 0, LOGIN_ADDRESS_SIZE);
      memset (rec->data.role_chng.terminal, 0, LOGIN_TERM_SIZE);
      memset (rec->data.role_chng.res, 0, LOGIN_RESULT_SIZE);
      break;

    case INIT_CIPSO:
      rec->data.cipso.auid = VAL_NOT_SET;
      rec->data.cipso.res = VAL_NOT_SET;
      rec->data.cipso.cipso_doi = VAL_NOT_SET;
      rec->data.cipso.unlbl_accept = VAL_NOT_SET;
      rec->data.cipso.old = VAL_NOT_SET;
      memset (rec->data.cipso.tool, 0, TOOL_NAME_SIZE);
      memset (rec->data.cipso.subj, 0, SUBJ_SIZE);
      memset (rec->data.cipso.nlbl_domain, 0, DOM_SIZE);
      memset (rec->data.cipso.nlbl_protocol, 0, PROTOCOL_SIZE);
      memset (rec->data.cipso.cipso_type, 0, CIPSO_TYPE_SIZE);
      break;

    case INIT_IPSEC:
      rec->data.ipsec.auid = VAL_NOT_SET;
      rec->data.ipsec.res = VAL_NOT_SET;
      rec->data.ipsec.sec_doi = VAL_NOT_SET;
      rec->data.ipsec.sec_alg = VAL_NOT_SET;
      memset (rec->data.ipsec.desc, 0, AUIDMSG_SIZE);
      memset (rec->data.ipsec.subj, 0, SUBJ_SIZE);
      memset (rec->data.ipsec.sec_obj, 0, SUBJ_SIZE);
      memset (rec->data.ipsec.src, 0, LOGIN_HOST_SIZE);
      memset (rec->data.ipsec.dst, 0, LOGIN_HOST_SIZE);
      memset (rec->data.ipsec.spi, 0, SPI_SIZE);
      memset (rec->data.ipsec.protocol, 0, PROTOCOL_SIZE);
      break;

    case INIT_CUPS:
      rec->data.cups.pid = VAL_NOT_SET;
      rec->data.cups.uid = VAL_NOT_SET;
      rec->data.cups.auid = VAL_NOT_SET;
      rec->data.cups.auid2 = VAL_NOT_SET;
      rec->data.cups.job = VAL_NOT_SET;
      memset (rec->data.cups.msg, 0, AUIDMSG_SIZE);
      memset (rec->data.cups.subj, 0, SUBJ_SIZE);
      memset (rec->data.cups.obj, 0, SUBJ_SIZE);
      memset (rec->data.cups.acct, 0, LOGIN_ACCT_SIZE);
      memset (rec->data.cups.printer, 0, PRINTER_SIZE);
      memset (rec->data.cups.title, 0, TITLE_SIZE);
      memset (rec->data.cups.label, 0, LABEL_SIZE);
      memset (rec->data.cups.exe, 0, EXE_SIZE);
      memset (rec->data.cups.host, 0, LOGIN_HOST_SIZE);
      memset (rec->data.cups.addr, 0, LOGIN_ADDRESS_SIZE);
      memset (rec->data.cups.terminal, 0, LOGIN_TERM_SIZE);
      break;

    default:
      printf ("don't know what to initialize\n");
      break;
    }
}

/*
 * this function searches the main list of parsed records for a time match
 * between start and end time. If such record is found, it is passed to 
 * compare_rec() in order to compare it to expected record
 *
 * Args:
 *   start: start time
 *   expected: record we are looking for. This is passed to and used in compare_rec()
 *   end: end time
 *
 * Return: 0 if found, 1 if not found
 */
int
search_for_record (main_list * actual, time_t start, main_list * expected,
                   time_t end)
{
  main_list *tmp = actual;
  int rc = 1;

  /* printf("Search time: Start = %d, End = %d\n", start, end); */

  while (tmp != NULL)
    {
      /* If Statement explanation:
       * search for record in following cases:
       * start time is specified and within range  OR
       * end time is specified and within range    OR
       * start and end times are not specified at all (Disk Handling)
       */
      if (((start != VAL_NOT_SET) && (tmp->rec_time >= start))
          || ((end != VAL_NOT_SET) && (tmp->rec_time <= end))
          || ((start == VAL_NOT_SET) && (end == VAL_NOT_SET)))
        {
          rc = compare_rec (tmp, expected);
          if (rc == 0)          /* found */
            return rc;
          else
            tmp = tmp->next_record;     /* keep looking */
        }
      else
        {
          tmp = tmp->next_record;       /* Keep looking */
        }
    }

  return rc;                    /* did not find match */
}

/* this function appends a full record to the main list of records
 *
 * Args:
 *   head: head of the main list of records
 *   new: the new record to be added
 *
 * Return: head pointer to the top of records list
 */
main_list *
append_list (main_list * head, main_list * new)
{
  main_list *ptr;

  if (head == NULL)
    {                           /* first record in list */
      head = new;
    }
  else
    {
      for (ptr = head; ptr->next_record != NULL; ptr = ptr->next_record)
        ;                       /* reach end of list */
      ptr->next_record = new;   /* add new part to end */
    }

  return head;
}

/* this function appends record parts to create a full record
 *
 * Args:
 *  list: pointer to pointer of the main list of records
 *  new: the record part that needs to be appended to the inner list
 *	of the "list" variable.
 *
 * Return: head pointer to the inner list (ie parts that make a full record)
 *
 */
rec_part *
append_record (main_list ** list, rec_part * new)
{
  rec_part *head = (*list)->myrecord;   /* pointer to head of inner list */
  rec_part *ptr;

  if (head == NULL)
    {                           /* first part of record */
      (*list)->myrecord = new;
    }
  else
    {
      for (ptr = head; ptr->next_part != NULL; ptr = ptr->next_part)
        ;                       /* reach end of list */
      ptr->next_part = new;     /* add new part to end */
    }

  return head;
}

/*
 * searching the main list for the record that matches the time stamp
 * so we can append record part to make full record in the list
 *
 * Args: 
 *  head: pointer to the head of the main list of records.
 *
 * Return: Pointer to record with matching serial if one is found
 *	NULL if no match is found.
 */
main_list *
search_for_serial (main_list * head, unsigned long audit_serial)
{
  main_list *ptr = head;
  if (head == NULL)
    return head;                /* list empty, return null */
  else
    {
      while (ptr != NULL)
        {
          if (ptr->rec_serial == audit_serial)  /*Found record */
            {
              return ptr;
            }
          else
            {
              ptr = ptr->next_record;
            }
        }
    }
  return NULL;                  /* reached end without finding matching serial */
}

/* 
 * This functions prints record blocks based on the type
 *
 * Args:
 *  head: the head of the parts making up a complete record
 */
void
print_list (rec_part * head)
{
  rec_part *ptr = head;

  if (head == NULL)
    {
      printf ("List is empty\n");
      return;
    }
  else
    {
      while (ptr != NULL)
        {
          if (strcmp (ptr->type, "SYSCALL") == 0)
            {
              printf ("\n%s: arch=%lx ", ptr->type, ptr->data.syscall.arch);
              printf ("syscall=%d ", ptr->data.syscall.syscall_no);
              printf ("success=%s ", ptr->data.syscall.success);
              printf ("exit=%d ", ptr->data.syscall.exit);
              printf ("a0=%lx ", ptr->data.syscall.argv[0]);
              printf ("a1=%lx ", ptr->data.syscall.argv[1]);
              printf ("a2=%lx ", ptr->data.syscall.argv[2]);
              printf ("a3=%lx ", ptr->data.syscall.argv[3]);
              printf ("ppid=%d ", ptr->data.syscall.ppid);
              printf ("pid=%d ", ptr->data.syscall.pid);
              printf ("auid=%d ", ptr->data.syscall.auid);
              printf ("uid=%d ", ptr->data.syscall.uid);
              printf ("gid=%d ", ptr->data.syscall.gid);
              printf ("euid=%d ", ptr->data.syscall.euid);
              printf ("suid=%d ", ptr->data.syscall.suid);
              printf ("fsuid=%d ", ptr->data.syscall.fsuid);
              printf ("egid=%d ", ptr->data.syscall.egid);
              printf ("sgid=%d ", ptr->data.syscall.sgid);
              printf ("fsgid=%d ", ptr->data.syscall.fsgid);
              printf ("subj=%s ", ptr->data.syscall.subj);
              printf ("key=%s", ptr->data.syscall.key);

            }
          else if (strcmp (ptr->type, "PATH") == 0)
            {
              printf ("\n%s: name=%s ", ptr->type, ptr->data.path.name);
              printf ("flags=%d ", ptr->data.path.flags);
              printf ("parent=%ld ", ptr->data.path.parent);
              printf ("inode=%ld ", ptr->data.path.inode);
              printf ("mode=%ld ", ptr->data.path.mode);
              printf ("dev=%s ", ptr->data.path.dev);
              printf ("ouid=%d ", ptr->data.path.ouid);
              printf ("ogid=%d ", ptr->data.path.ogid);
              printf ("rdev=%s ", ptr->data.path.rdev);
              printf ("obj=%s", ptr->data.path.obj);

            }
          else if (strcmp (ptr->type, "CONFIG_CHANGE") == 0)
            {
              printf ("\n%s: ", ptr->type);
              printf ("audit_enabled=%d ", ptr->data.config.audit_en);
              printf ("old=%d ", ptr->data.config.old);
              printf ("auid=%d ", ptr->data.config.auid);
              printf ("msg=%s ", ptr->data.config.msg);
              printf ("subj=%s ", ptr->data.config.subj);
              printf ("res=%d", ptr->data.config.res);

            }
          else if ((strcmp (ptr->type, "USER_START") == 0)
                   || (strcmp (ptr->type, "USER") == 0)
                   || (strcmp (ptr->type, "USER_ACCT") == 0)
                   || (strcmp (ptr->type, "USER_AUTH") == 0)
                   || (strcmp (ptr->type, "USER_LOGIN") == 0))
            {
              printf ("\n%s: pid=%d ", ptr->type, ptr->data.user.pid);
              printf ("uid=%d ", ptr->data.user.uid);
              printf ("auid=%d ", ptr->data.user.auid);
              printf ("subj=%s ", ptr->data.user.subj);
              printf ("msg=%s ", ptr->data.user.msg);
              printf ("op=%s ", ptr->data.user.op);
              printf ("id=%d ", ptr->data.user.id);
              printf ("acct=%s ", ptr->data.user.user);
              printf ("uid=%d ", ptr->data.user.user_login_uid);
              printf ("exe=%s ", ptr->data.user.exe);
              printf ("host=%s ", ptr->data.user.host);
              printf ("addr=%s ", ptr->data.user.addr);
              printf ("terminal=%s ", ptr->data.user.terminal);
              printf ("result=%s", ptr->data.user.result);

            }
          else if ((strcmp (ptr->type, "USER_CHAUTHTOK") == 0)
                   || (strcmp (ptr->type, "USYS_CONFIG") == 0))
            {
              printf ("\n%s: pid=%d ", ptr->type, ptr->data.chauthtok.pid);
              printf ("uid=%d ", ptr->data.chauthtok.uid);
              printf ("auid=%d ", ptr->data.chauthtok.auid);
              printf ("subj=%s ", ptr->data.chauthtok.subj);
              printf ("msg=%s ", ptr->data.chauthtok.msg);
              printf ("op=%s ", ptr->data.chauthtok.op);
              printf ("id=%ld ", ptr->data.chauthtok.id);
              printf ("acct=%s ", ptr->data.chauthtok.acct);
              printf ("host=%s ", ptr->data.chauthtok.host);
              printf ("addr=%s ", ptr->data.chauthtok.addr);
              printf ("terminal=%s ", ptr->data.chauthtok.terminal);
              printf ("res=%s", ptr->data.chauthtok.res);

            }
          else if (strcmp (ptr->type, "MAC_POLICY_LOAD") == 0)
            {
              printf ("\n%s: auid=%d ", ptr->type, ptr->data.load_pol.auid);
              printf ("msg=%s", ptr->data.load_pol.auidmsg);

            }
          else if (strcmp (ptr->type, "ANOM_RBAC_FAIL") == 0)
            {
              printf ("\n%s: uid=%d ", ptr->type, ptr->data.rbac.uid);
              printf ("pid=%d ", ptr->data.rbac.pid);
              printf ("auid=%d ", ptr->data.rbac.auid);
              printf ("subj=%s ", ptr->data.rbac.subj);
              printf ("msg=%s ", ptr->data.rbac.msg);
              printf ("host=%s ", ptr->data.rbac.host);
              printf ("addr=%s ", ptr->data.rbac.addr);
              printf ("terminal=%s ", ptr->data.rbac.terminal);
              printf ("res=%s", ptr->data.rbac.res);

            }
          else if ((strcmp (ptr->type, "AUDIT_DEV_ALLOC") == 0)
                   || (strcmp (ptr->type, "AUDIT_DEV_DEALLOC") == 0)
                   || (strcmp (ptr->type, "AUDIT_USYS_CONFIG") == 0))
            {
              printf ("\n%s: uid=%d ", ptr->type, ptr->data.dev_usys.uid);
              printf ("pid=%d ", ptr->data.dev_usys.pid);
              printf ("auid=%d ", ptr->data.dev_usys.auid);
              printf ("msg=%s ", ptr->data.dev_usys.msg);
              printf ("device=%s ", ptr->data.dev_usys.dev);
              printf ("op=%s ", ptr->data.dev_usys.op);
              printf ("obj=%s ", ptr->data.dev_usys.obj);
              printf ("desc=%s ", ptr->data.dev_usys.desc);

            }
          else if (strcmp (ptr->type, "AVC") == 0)
            {
              printf ("\n%s: desc=%s ", ptr->type, ptr->data.avc.desc);
              printf ("pid=%d ", ptr->data.avc.pid);
              printf ("comm=%s ", ptr->data.avc.comm);
              printf ("name=%s ", ptr->data.avc.name);
              printf ("dev=%s ", ptr->data.avc.dev);
              printf ("ino=%d ", ptr->data.avc.ino);
              printf ("scontext=%s ", ptr->data.avc.scontext);
              printf ("tcontext=%s ", ptr->data.avc.tcontext);
              printf ("tclass=%s", ptr->data.avc.tclass);

            }
          else if ((strcmp (ptr->type, "MAC_STATUS") == 0)
                   || (strcmp (ptr->type, "MAC_CONFIG_CHANGE") == 0))
            {
              printf ("\n%s: val=%d ", ptr->type, ptr->data.mac.curr_val);
              printf ("old_val=%d ", ptr->data.mac.old_val);
              printf ("bool=%s ", ptr->data.mac.bool_name);
              printf ("auid=%d", ptr->data.mac.auid);

            }
          else if (strcmp (ptr->type, "USER_ROLE_CHANGE") == 0)
            {
              printf ("\n%s: uid=%d ", ptr->type, ptr->data.role_chng.uid);
              printf ("pid=%d ", ptr->data.role_chng.pid);
              printf ("auid=%d ", ptr->data.role_chng.auid);
              printf ("subj=%s ", ptr->data.role_chng.subj);
              printf ("def-context=%s ", ptr->data.role_chng.def_cntx);
              printf ("sel-context=%s ", ptr->data.role_chng.sel_cntx);
              printf ("old-context=%s ", ptr->data.role_chng.old_cntx);
              printf ("new-context=%s ", ptr->data.role_chng.new_cntx);
              printf ("old-seuser=%s ", ptr->data.role_chng.old_user);
              printf ("old-role=%s ", ptr->data.role_chng.old_role);
              printf ("old-range=%s ", ptr->data.role_chng.old_range);
              printf ("new-seuser=%s ", ptr->data.role_chng.new_user);
              printf ("new-role=%s ", ptr->data.role_chng.new_role);
              printf ("new-range=%s ", ptr->data.role_chng.new_range);
              printf ("msg=%s ", ptr->data.role_chng.msg);
              printf ("acct=%s ", ptr->data.role_chng.acct);
              printf ("op=%s ", ptr->data.role_chng.op);
              printf ("host=%s ", ptr->data.role_chng.host);
              printf ("addr=%s ", ptr->data.role_chng.addr);
              printf ("terminal=%s ", ptr->data.role_chng.terminal);
              printf ("res=%s", ptr->data.role_chng.res);

            }
          else if ((strcmp (ptr->type, "MAC_CIPSOV4_ADD") == 0)
                   || (strcmp (ptr->type, "MAC_CIPSOV4_DEL") == 0))
            {
              printf ("\n%s: %s ", ptr->type, ptr->data.cipso.tool);
              printf ("auid=%d ", ptr->data.cipso.auid);
              printf ("subj=%s ", ptr->data.cipso.subj);
              printf ("cipso_doi=%d ", ptr->data.cipso.cipso_doi);
              printf ("cipso_type=%s ", ptr->data.cipso.cipso_type);
              printf ("res=%d", ptr->data.cipso.res);

            }
          else if ((strcmp (ptr->type, "MAC_MAP_ADD") == 0)
                   || (strcmp (ptr->type, "MAC_MAP_DEL") == 0))
            {
              printf ("\n%s: %s ", ptr->type, ptr->data.cipso.tool);
              printf ("auid=%d ", ptr->data.cipso.auid);
              printf ("subj=%s ", ptr->data.cipso.subj);
              printf ("nlbl_domain=%s ", ptr->data.cipso.nlbl_domain);
              printf ("nlbl_protocol=%s ", ptr->data.cipso.nlbl_protocol);
              printf ("cipso_doi=%d ", ptr->data.cipso.cipso_doi);
              printf ("res=%d", ptr->data.cipso.res);

            }
          else if (strcmp (ptr->type, "MAC_UNLBL_ALLOW") == 0)
            {
              printf ("\n%s: %s ", ptr->type, ptr->data.cipso.tool);
              printf ("auid=%d ", ptr->data.cipso.auid);
              printf ("subj=%s ", ptr->data.cipso.subj);
              printf ("unlbl_accept=%d ", ptr->data.cipso.unlbl_accept);
              printf ("old=%d", ptr->data.cipso.old);

            }
          else if ((strcmp (ptr->type, "MAC_IPSEC_ADDSA") == 0)
                   || (strcmp (ptr->type, "MAC_IPSEC_DELSA") == 0)
                   || (strcmp (ptr->type, "MAC_IPSEC_ADDSPD") == 0)
                   || (strcmp (ptr->type, "MAC_IPSEC_DELSPD") == 0))
            {
              printf ("\n%s: %s ", ptr->type, ptr->data.ipsec.desc);
              printf ("auid=%d ", ptr->data.ipsec.auid);
              printf ("subj=%s ", ptr->data.ipsec.subj);
              printf ("sec_alg=%d ", ptr->data.ipsec.sec_alg);
              printf ("sec_doi=%d ", ptr->data.ipsec.sec_doi);
              printf ("sec_obj=%s ", ptr->data.ipsec.sec_obj);
              printf ("src=%s ", ptr->data.ipsec.src);
              printf ("dst=%s ", ptr->data.ipsec.dst);
              printf ("spi=%s ", ptr->data.ipsec.spi);
              printf ("protocol=%s ", ptr->data.ipsec.protocol);
              printf ("res=%d", ptr->data.ipsec.res);

            }
          else if (strcmp (ptr->type, "USER_LABELED_EXPORT") == 0)
            {
              printf ("\n%s: user pid=%d ", ptr->type, ptr->data.cups.pid);
              printf ("uid=%d ", ptr->data.cups.uid);
              printf ("auid=%d ", ptr->data.cups.auid);
              printf ("subj=%s ", ptr->data.cups.subj);
              printf ("job=%d ", ptr->data.cups.job);
              printf ("acct=%s ", ptr->data.cups.acct);
              printf ("auid=%d ", ptr->data.cups.auid2);
              printf ("printer=%s ", ptr->data.cups.printer);
              printf ("msg=%s ", ptr->data.cups.msg);
              printf ("title=%s ", ptr->data.cups.title);
              printf ("obj=%s ", ptr->data.cups.obj);
              printf ("label=%s ", ptr->data.cups.label);
              printf ("exe=%s ", ptr->data.cups.exe);
              printf ("host=%s ", ptr->data.cups.host);
              printf ("addr=%s ", ptr->data.cups.addr);
              printf ("terminal=%s ", ptr->data.cups.terminal);
              printf ("res=%s", ptr->data.cups.res);

            }
          else if (strcmp (ptr->type, "LABEL_OVERRIDE") == 0)
            {
              printf ("\n%s: user pid=%d ", ptr->type, ptr->data.cups.pid);
              printf ("uid=%d ", ptr->data.cups.uid);
              printf ("auid=%d ", ptr->data.cups.auid);
              printf ("subj=%s ", ptr->data.cups.subj);
              printf ("msg=%s ", ptr->data.cups.msg);
              printf ("exe=%s ", ptr->data.cups.exe);
              printf ("host=%s ", ptr->data.cups.host);
              printf ("addr=%s ", ptr->data.cups.addr);
              printf ("terminal=%s ", ptr->data.cups.terminal);
              printf ("res=%s", ptr->data.cups.res);

            }
          else
            {
              printf ("\nHave no idea what type to print\n");
            }

          ptr = ptr->next_part;
        }                       // end while
    }                           // end else
}

/*
 * This functions cleans up the memory used by the lists of records
 *
 * Args:
 *  head: pointer to a pointer to the head of the list of records
 */
void
cleanup_mem (main_list ** head)
{
  main_list *n_list = NULL;
  rec_part *ptr = NULL;
  rec_part *n_ptr = NULL;

  while (*head != NULL)
    {
      n_list = (*head)->next_record;
      ptr = (*head)->myrecord;  // pointer to inner list
      // delete all record parts before loosing the pointer
      while (ptr != NULL)
        {
          n_ptr = ptr->next_part;
          free (ptr);
          ptr = n_ptr;
        }
      free (*head);
      *head = n_list;
    }
}
