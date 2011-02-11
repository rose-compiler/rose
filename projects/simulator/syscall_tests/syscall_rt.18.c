/******************************************************************************
 *Copyright (C) International Business Machines Corp., 2004, 2006
 *
 *This program is free software; you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation; either version 2 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *the GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *
 *
 *FILE: testfileperms.c
 *
 *PURPOSE: The purpose of this test is to verify the file permissions
 *of all files under a given directory. The test makes 2 passes
 *through all the files.
 *Pass 1:
 *Using the existing file attributes, verify file access as the
 *file owner, group owner and other.
 *The results of "open" are used to determine access.
 *If the file owner is root, it is expected that access
 *will be granted even if permissions are explicitly
 *denied.
 *Pass 2:
 *An attempt is made to chown the file to the provided
 *testuser and testgroup.
 *If the chown fails, a message is logged and the file
 *is skipped.
 *If the chown succeeds, a stat is performed to verify
 *the chown was effective. If the file owner and group
 *has not been modified, a message is logged and the file
 *is skipped.
 *Once the file is chowned, file permissions are verified
 *as the testuser/testgroup.
 *
 *In all cases, links are skipped.
 *
 *SPECIAL CASES:
 *If the given directory is "ipc_obj", all IPC Objects are tested 
 *(Shared Memory, Semaphores and Message Queue).
 *
 *If the given directory is "dev/mqueue", all files in that directory
 *are tested with specifics functions of POSIX Message Queue.
 *
 *When testing functions those will change the original value of
 *IPC object, a pre-preparing of object is done to make sure that the test
 *will fail or accept by permission access only. After the execution of
 *test, the original values are returned to the object in test.
 *
 **************************  HISTORY  ****************************************
 *DATENAMEDESCRIPTION
 *01/29/2007  klausk@br.ibm.com Added shmdt to dettach shmemory when
 *          operation succeeds.
 *          Fixed file_select scandir param function
 *01/10/2007klausk@br.ibm.comMinor fix - added a special
 *case when file gid == gid_other
 *01/10/2007klausk@br.ibm.comFixed. Added setgroups() to
 *drop supplementary groups
 *      01/04/2007klausk@br.ibm.comUpdated nobody's uid/gid to 
 *match RHEL5 ones (99/99)
 *08/18/2006camiloyc@br.ibm.comAdded IPC Message Queue tests
 *08/16/2006camiloyc@br.ibm.comAdded IPC Semaphores Set tests
 *08/14/2006camiloyc@br.ibm.comAdded IPC Shared Memory tests
*08/11/2006camiloyc@br.ibm.comAdded POSIX Message Queue tests
*11/18/2004krisw@us.ibm.comIf access is expected but denied,
  *this is not a security problem,
  *it is a problem if denial is
  *expected but access allowed.
  *So if pass is expected but fail
  *received, this has been deemed
  *OK for certification purposes 
  *(A1).
  *10/10/2004danjones@us.ibm.comoriginated by Dan Jones.
  ******************************************************************************/
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <errno.h>
#include <mqueue.h>

#define tmpfile "/tmp/tmpfile"

extern int alphasort ();

static int file_select (const struct dirent *entry);

static int perms[] = { S_IRUSR, S_IWUSR, S_IXUSR,
  S_IRGRP, S_IWGRP, S_IXGRP,
  S_IROTH, S_IWOTH, S_IXOTH
};
static char *ptext[] = { "r", "w", "x", "r", "w", "x", "r", "w", "x" };

int totalpass = 0;
int totalfail = 0;
uid_t uid_nobody = 99;
gid_t gid_nobody = 99;

int test = 0;
static char *test_description[] = { "Check default permissions",
  "chown files to testuser/testgroup"
};

/*
 * Do not include . or .. in directory list.
 */
static int
file_select (const struct dirent *entry)
{
  if ((strcmp (entry->d_name, ".") == 0) ||
      (strcmp (entry->d_name, "..") == 0))
    return (0);
  else
    return (1);
}

/*
 * Set euid, egid
 */
void
setids (uid_t uid, gid_t gid)
{
  int rc = 0;

  if ((rc = setegid (gid)) == -1)
    {
      printf ("ERROR: unable to set gid %d, errno %d", gid, errno);
      exit (1);
    }
  if ((rc = seteuid (uid)) == -1)
    {
      printf ("ERROR: unable to set uid %d, errno %d", uid, errno);
      exit (1);
    }
  printf ("euid/egid now %d/%d\n", geteuid (), getegid ());

  return;
}

/*
 * Check actual vs. expected access using open system call
 */
void
testaccess (char *pathname, int mode, int expected, char *outbuf)
{
  int testrc = 0;
  int myerr = 0;
  char dir[15];/* used for posix message queue */
  char *qname;

  if (expected == -1)
    {
      strcat (outbuf, "expected: fail  ");
    }
  else
    {
      strcat (outbuf, "expected: pass  ");
    }

  memset (dir, '\0', sizeof (dir));
  strncpy (dir, pathname, 12);

  /* Test if pathname is from POSIX Message Queue */
  if (strcmp (dir, "/dev/mqueue/") == 0)
    {
      qname = &pathname[11];
      if ((testrc = mq_open (qname, mode)) == -1)
{
  myerr = errno;
  strcat (outbuf, "actual: fail");
}
      else
{
  strcat (outbuf, "actual: pass");
  mq_close (testrc);
}
    }
  else
    {
      if ((testrc = open (pathname, mode)) == -1)
{
  myerr = errno;
  strcat (outbuf, "actual: fail");
}
      else
{
  strcat (outbuf, "actual: pass");
  close (testrc);
}
    }

  if (myerr == ENODEV)
    {
      sprintf (&(outbuf[strlen (outbuf)]),
       "\tresult: SKIP : no device : %s\n", pathname);
    }
  else if (myerr == EBUSY)
    {
      sprintf (&(outbuf[strlen (outbuf)]),
       "\tresult: SKIP : device busy : %s\n", pathname);
    }
  else if ((testrc == expected) || ((expected == 0) && (testrc != -1)))
    {
      strcat (outbuf, "\tresult: PASS\n");
      totalpass++;
    }
  else
    {
      /*
       * A1 OK if expected pass but got fail, but need to log
       * so can manually check errno.
       */
      if ((expected == 0) && (testrc == -1))
{
  sprintf (&(outbuf[strlen (outbuf)]),
   "\tresult: FAIL/PASS : errno = %d : %s\n",
   myerr, pathname);
  totalpass++;
}
      else
{
  sprintf (&(outbuf[strlen (outbuf)]),
   "\tresult: FAIL : myerr = %d, expected = %d, testrc = %d, euid = %u, egid = %u : %s\n",
   myerr, expected, testrc, geteuid (), getegid (), pathname);
  totalfail++;
}
    }

  printf ("%s", outbuf);

  return;
}

/*
 * Check actual vs. expected access using ipc system call
 */

void
testaccess_ipc (int ipc_id, char opt, int mode, int expected, char *outbuf)
{
  int actual, semval, rc;
  int myerror = 0;
  char *chPtr;
  struct sembuf sop;
  uid_t tmpuid;
  gid_t tmpgid;
  struct msqbuf
  {
    long mtype;
    char mtext[80];
  } s_message, r_message;

  /* If we are root, we expect to succeed event
   * without explicit permission.
   */
  strcat (outbuf, (expected == -1) ? "expected: fail  " : "expected: pass  ");

  switch (opt)
    {
      /* Shared Memory */
    case 'm':
      /* Try to get (mode) access
       * There is no notion of a write-only shared memory
       * segment. We are testing READ ONLY and READWRITE access.
       */
      chPtr = shmat (ipc_id, NULL, (mode == O_RDONLY) ? SHM_RDONLY : 0);
      if (chPtr != (void *) -1)
{
  strcat (outbuf, "actual: pass ");
  actual = 0;
  if (shmdt (chPtr) == -1)
    {
      perror ("Warning: Could not dettach memory segment");
    }
}
      else
{
  myerror = errno;
  strcat (outbuf, "actual: fail ");
  actual = -1;
}
      break;
      /* Semaphores */
    case 's':
      tmpuid = geteuid ();
      tmpgid = getegid ();
      semval = semctl (ipc_id, 0, GETVAL);
      /* Need semaphore value == 0 to execute read permission test */
      if ((mode == O_RDONLY) && (semval > 0))
{
  setids (0, 0);
  if ((semctl (ipc_id, 0, SETVAL, 0)) == -1)
    {
      printf ("Unable to set semaphore value: %d\n", errno);
    }
  setids (tmpuid, tmpgid);
}
      /* Try to get mode access */
      sop.sem_num = 0;
      sop.sem_op = mode;
      sop.sem_flg = SEM_UNDO;
      actual = semop (ipc_id, &sop, 1);
      myerror = errno;
      if (actual != -1)
{
  strcat (outbuf, "actual: pass ");
  /* back to semaphore original value */
  if (mode != O_RDONLY)
    {
      sop.sem_op = -1;/* decrement semaphore */
      rc = semop (ipc_id, &sop, 1);
    }
}
      else
{
  /* Back to semaphore original value */
  if ((mode == O_RDONLY) && (semval > 0))
    {
      setids (0, 0);
      if ((semctl (ipc_id, 0, SETVAL, semval)) == -1)
{
  printf ("Unable to set semaphore " "value: %d\n", errno);
}
      setids (tmpuid, tmpgid);
    }
  strcat (outbuf, "actual: fail ");
}
      break;
      /* Message Queues */
    case 'q':
      tmpuid = geteuid ();
      tmpgid = getegid ();
      if (mode == O_RDONLY)
{
  setids (0, 0);
  /* Send a message to test msgrcv function */
  s_message.mtype = 1;
  memset (s_message.mtext, '\0', sizeof (s_message.mtext));
  strcpy (s_message.mtext, "First Message\0");
  if ((rc = msgsnd (ipc_id, &s_message,
    strlen (s_message.mtext), 0)) == -1)
    {
      printf ("Error sending first message: %d\n", errno);
    }
  setids (tmpuid, tmpgid);
}
      s_message.mtype = 1;
      memset (s_message.mtext, '\0', sizeof (s_message.mtext));
      strcpy (s_message.mtext, "Write Test\0");

      /* Try to get WRITE access */
      if (mode == O_WRONLY)
{
  actual = msgsnd (ipc_id, &s_message, strlen (s_message.mtext), 0);
}
      else
{
  /* Try to get READ access */
  actual = msgrcv (ipc_id, &r_message,
   sizeof (r_message.mtext), 0, IPC_NOWAIT);
}
      myerror = errno;
      if (actual != -1)
{
  strcat (outbuf, "actual: pass ");
}
      else
{
  strcat (outbuf, "actual: fail ");
}
      if (((mode == O_RDONLY) && (actual == -1)) ||
  ((mode == O_WRONLY) && (actual != -1)))
{
  setids (0, 0);
  /* discard the message send */
  rc = msgrcv (ipc_id, &r_message,
       sizeof (r_message.mtext), 0, IPC_NOWAIT);
  setids (tmpuid, tmpgid);
}
      break;
    }

  if ((actual == expected) || ((expected == 0) && (actual != -1)))
    {
      strcat (outbuf, "\tresult: PASS\n");
      totalpass++;
    }
  else
    {
      errno = myerror;// restore errno from correct error code
      sprintf (&(outbuf[strlen (outbuf)]), "\tresult: FAIL : "
       "errno = %d\n", errno);
      totalfail++;
    }
  printf ("%s", outbuf);
  return;
}

/*
 * Test access for owner, group, other
 */
void
testall (struct stat *ostatbufp, char *pathname, uid_t uid, gid_t gid)
{
  int i;
  int rc = 0;
  char outbuf[512];
  struct passwd passwd;
  struct passwd *passwdp;
  struct group group;
  struct group *groupp;
  struct stat statbuf;
  struct stat *statbufp = &statbuf;
  char *pbuf;
  char *gbuf;

  passwdp = (struct passwd *) malloc (sizeof (passwd));
  groupp = (struct group *) malloc (sizeof (group));
  pbuf = (char *) malloc (4096);
  gbuf = (char *) malloc (4096);
  memset (pbuf, '\0', 4096);
  memset (gbuf, '\0', 4096);

  setids (0, 0);
  printf ("\n%s\n", pathname);

  /* For test 1 we chown the file owner/group */
  if (test == 1)
    {
      if ((rc = chown (pathname, uid, gid)) == -1)
{
  printf ("ERROR: unable to chown %s to %d:%d\n", pathname, uid, gid);
  goto EXIT;
}
    }

  /* Start with clean buffers */
  memset (&statbuf, '\0', sizeof (statbuf));

  /* Get file stat info to determine actual owner and group */
  stat (pathname, &statbuf);

  /*
   * If we successfully chown'd the file, but the owner hasn't changed
   * log it and skip.
   */
  if ((test == 1) && ((statbufp->st_uid != uid) || (statbufp->st_gid != gid)))
    {
      printf ("INFO: chown success, but file owner "
      "did not change: %s\n", pathname);
      goto EXIT;
    }

  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "MODE: ");
  for (i = 0; i < sizeof (perms) / sizeof (int); i++)
    {
      if (statbufp->st_mode & perms[i])
{
  strcat (outbuf, ptext[i]);
}
      else
{
  strcat (outbuf, "-");
}
    }
  getpwuid_r (statbufp->st_uid, &passwd, pbuf, 4096, &passwdp);
  getgrgid_r (statbufp->st_gid, &group, gbuf, 4096, &groupp);

  sprintf (&(outbuf[strlen (outbuf)]), "%s:%s\n",
   passwd.pw_name, group.gr_name);
  printf ("%s", outbuf);

  /* Check owner access for read/write */
  setids (statbufp->st_uid, gid_nobody);
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Owner read\t");
  /*
   * If we are root, we expect to succeed event
   * without explicit permission.
   */
  if ((statbufp->st_mode & S_IRUSR) || (statbufp->st_uid == 0))
    {
      testaccess (pathname, O_RDONLY, 0, outbuf);
    }
  else
    {
      testaccess (pathname, O_RDONLY, -1, outbuf);
    }
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Owner write\t");
  /*
   * If we are root, we expect to succeed event
   * without explicit permission.
   */
  if ((statbufp->st_mode & S_IWUSR) || (statbufp->st_uid == 0))
    {
      testaccess (pathname, O_WRONLY, 0, outbuf);
    }
  else
    {
      testaccess (pathname, O_WRONLY, -1, outbuf);
    }

  /* Check group access for read/write */
  setids (0, 0);
  setids (uid_nobody, statbufp->st_gid);
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Group read\t");
  if (statbufp->st_mode & S_IRGRP)
    {
      testaccess (pathname, O_RDONLY, 0, outbuf);
    }
  else
    {
      testaccess (pathname, O_RDONLY, -1, outbuf);
    }
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Group write\t");
  if (statbufp->st_mode & S_IWGRP)
    {
      testaccess (pathname, O_WRONLY, 0, outbuf);
    }
  else
    {
      testaccess (pathname, O_WRONLY, -1, outbuf);
    }

  /* Check other access for read/write */
  setids (0, 0);
  setids (uid_nobody, gid_nobody);
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Other read\t");
  if (statbufp->st_gid == gid_nobody)
    {
      /* special case! file's gid == our 'other' gid */
      if (statbufp->st_mode & S_IRGRP)
{
  testaccess (pathname, O_RDONLY, 0, outbuf);
}
      else
{
  testaccess (pathname, O_RDONLY, -1, outbuf);
}
    }
  else
    {
      if (statbufp->st_mode & S_IROTH)
{
  testaccess (pathname, O_RDONLY, 0, outbuf);
}
      else
{
  testaccess (pathname, O_RDONLY, -1, outbuf);
}
    }
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Other write\t");
  if (statbufp->st_gid == gid_nobody)
    {
      /* special case! ile's gid == our 'other' gid */
      if (statbufp->st_mode & S_IWGRP)
{
  testaccess (pathname, O_WRONLY, 0, outbuf);
}
      else
{
  testaccess (pathname, O_WRONLY, -1, outbuf);
}
    }
  else
    {
      if (statbufp->st_mode & S_IWOTH)
{
  testaccess (pathname, O_WRONLY, 0, outbuf);
}
      else
{
  testaccess (pathname, O_WRONLY, -1, outbuf);
}
    }
  setids (0, 0);

  if (test == 1)
    {
      chown (pathname, ostatbufp->st_uid, ostatbufp->st_gid);
    }

EXIT:
  return;
}

/*
 * Test an IPC object.
 */

int
testall_ipc (int ipc_id, char opt, uid_t uid, gid_t gid)
{
  int rc = 0;
  char outbuf[256];
  struct passwd passwd;
  struct passwd *passwdp;
  struct group group;
  struct group *groupp;
  struct shmid_ds shbuf;
  struct semid_ds sembuf;
  struct msqid_ds msqbuf;
  struct ipc_perm *ipcperm;
  char *pbuf;
  char *gbuf;
  uid_t tmpuid;
  gid_t tmpgid;

  pbuf = (char *) malloc (4096);
  gbuf = (char *) malloc (4096);
  memset (pbuf, '\0', 4096);
  memset (gbuf, '\0', 4096);

  setids (0, 0);
  printf ("\nIPC ID: %d - %s\n", ipc_id,
  ((opt == 's') ? "Semaphore Sets" : (opt ==
      'm') ? "Shared Memory Segment" :
   "Message Queues"));

  switch (opt)
    {
      /* Shared Memory */
    case 'm':
      /* Get stat information of shared memory. */
      if ((rc = shmctl (ipc_id, IPC_STAT, &shbuf)) == -1)
{
  printf ("Error getting stat of Shared Memory: %d\n", errno);
  return (rc);
}
      else
{
  /* ipcperm get the shm_perm memory address to manipulate
   * permissions with the same struct (ipc_perm).
   */
  ipcperm = &(shbuf.shm_perm);
}
      break;
      /* Semaphores */
    case 's':
      /* Set permission mode for semaphore. */
      if ((rc = semctl (ipc_id, 0, IPC_STAT, &sembuf)) == -1)
{
  printf ("Error getting stat of Semaphores: %d\n", errno);
  return (rc);
}
      else
{
  /* ipcperm get the sem_perm memory address to manipulate
   * permissions with the same struct (ipc_perm).
   */
          ipcperm = &(sembuf.sem_perm);
        }
      break;
      /* Message Queues */
    case 'q':
      if ((rc = msgctl (ipc_id, IPC_STAT, &msqbuf)) == -1)
        {
          printf ("Error getting stat of Message Queue: %d\n", errno);
          return (rc);
        }
      else
        {
          /* ipcperm get the msg_perm memory address to manipulate
           * permissions with the same struct (ipc_perm).
           */
          ipcperm = &(msqbuf.msg_perm);
        }
      break;
    }
  /* For test 1, change owner and group */
  if (test == 1)
    {
      tmpuid = ipcperm->uid;
      tmpgid = ipcperm->gid;
      ipcperm->uid = uid;
      ipcperm->gid = gid;
      switch (opt)
        {
          /* Shared Memory */
        case 'm':
          if ((rc = shmctl (ipc_id, IPC_SET, &shbuf)) == -1)
            {
              printf ("Error setting stat of Shared Memory: " "%d\n", errno);
              return (rc);
            }
          break;
          /* Semaphores */
        case 's':
          if ((rc = semctl (ipc_id, 0, IPC_SET, &sembuf)) == -1)
            {
              printf ("Error setting stat of Semaphores: " "%d\n", errno);
              return (rc);
            }
          break;
          /* Message Queues */
        case 'q':
          if ((rc = msgctl (ipc_id, IPC_SET, &msqbuf)) == -1)
            {
              printf ("Error setting stat of Message Queue: " "%d\n", errno);
              return (rc);
            }
          break;
        }
    }

  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "MODE: ");

  strcat (outbuf, (((ipcperm->mode & S_IRUSR) != 0) ? "r" : "-"));
  strcat (outbuf, (((ipcperm->mode & S_IWUSR) != 0) ? "w" : "-"));
  strcat (outbuf, "-");
  strcat (outbuf, (((ipcperm->mode & S_IRGRP) != 0) ? "r" : "-"));
  strcat (outbuf, (((ipcperm->mode & S_IWGRP) != 0) ? "w" : "-"));
  strcat (outbuf, "-");
  strcat (outbuf, (((ipcperm->mode & S_IROTH) != 0) ? "r" : "-"));
  strcat (outbuf, (((ipcperm->mode & S_IWOTH) != 0) ? "w" : "-"));
  strcat (outbuf, "-");

  getpwuid_r (ipcperm->uid, &passwd, pbuf, 4096, &passwdp);
  getgrgid_r (ipcperm->gid, &group, gbuf, 4096, &groupp);

  /* Print owner:group */
  sprintf (&(outbuf[strlen (outbuf)]), " %s:%s\n",
           passwd.pw_name, group.gr_name);
  /* Change current owner of process to access IPC function */
  setids (ipcperm->uid, gid_nobody);
  strcat (outbuf, "Owner read\t");
  /*
   * If we are root, we expect to succeed event
   * without explicit permission.
   */
  if ((ipcperm->mode & S_IRUSR) || (ipcperm->uid == 0))
    {
      testaccess_ipc (ipc_id, opt, O_RDONLY, 0, outbuf);
    }
  else
    {
      testaccess_ipc (ipc_id, opt, O_RDONLY, -1, outbuf);
    }
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Owner write\t");
  /*
   * If we are root, we expect to succeed event
   * without explicit permission.
   */
  if ((ipcperm->mode & S_IWUSR) || (ipcperm->uid == 0))
    {
      testaccess_ipc (ipc_id, opt, O_WRONLY, 0, outbuf);
    }
  else
    {
      testaccess_ipc (ipc_id, opt, O_WRONLY, -1, outbuf);
    }

  /* Check group access for read/write */
  setids (0, 0);
  setids (uid_nobody, ipcperm->gid);
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Group read\t");
  if (ipcperm->mode & S_IRGRP)
    {
      testaccess_ipc (ipc_id, opt, O_RDONLY, 0, outbuf);
    }
  else
    {
      testaccess_ipc (ipc_id, opt, O_RDONLY, -1, outbuf);
    }
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Group write\t");
  if (ipcperm->mode & S_IWGRP)
    {
      testaccess_ipc (ipc_id, opt, O_WRONLY, 0, outbuf);
    }
  else
    {
      testaccess_ipc (ipc_id, opt, O_WRONLY, -1, outbuf);
    }

  /* Check other access for read/write */
  setids (0, 0);
  setids (uid_nobody, gid_nobody);
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Other read\t");
  if (ipcperm->mode & S_IROTH)
    {
      testaccess_ipc (ipc_id, opt, O_RDONLY, 0, outbuf);
    }
  else
    {
      testaccess_ipc (ipc_id, opt, O_RDONLY, -1, outbuf);
    }
  memset (outbuf, '\0', sizeof (outbuf));
  strcat (outbuf, "Other write\t");
  if (ipcperm->mode & S_IWOTH)
    {
      testaccess_ipc (ipc_id, opt, O_WRONLY, 0, outbuf);
    }
  else
    {
      testaccess_ipc (ipc_id, opt, O_WRONLY, -1, outbuf);
    }
  setids (0, 0);
  /* Back to the original owner */
  if (test == 1)
    {
      ipcperm->uid = tmpuid;
      ipcperm->gid = tmpgid;
      switch (opt)
        {
          /* Shared Memory */
        case 'm':
          if ((rc = shmctl (ipc_id, IPC_SET, &shbuf)) == -1)
            {
              printf ("Error setting stat back of Shared "
                      "Memory: %d\n", errno);
              return (rc);
            }
          break;
          /* Semaphores */
        case 's':
          if ((rc = semctl (ipc_id, 0, IPC_SET, &sembuf)) == -1)
            {
              printf ("Error setting stat back of Semaphores: "
                      "%d\n", errno);
              return (rc);
            }
          break;
          /* Message Queues */
        case 'q':
          if ((rc = msgctl (ipc_id, IPC_SET, &msqbuf)) == -1)
            {
              printf ("Error setting stat back of Message "
      "Queue: %d\n", errno);
              return (rc);
            }
          break;
        }
    }
  return (0);
}


/*
 * Test all IPC Objects
 *
 * This method check access on actives ipc onjects in kernel
 */

int
check_ipc (uid_t uid, gid_t gid)
{
  FILE *fd;
  char *ret_char;
  char readbuf[80];
  char ipcscommand[80];
  char ipc_buf[10];
  int ipc_id;
  char ipc_obj[3] = { 'm', 's', 'q' };        /* m => SHM, s => SEM, Q => MSQ */
  int opt;

  setids (0, 0);
  for (opt = 0; opt < 3; opt++)
    {
      /* Store the resulto of ipcscommand in tmpfile and read each 
       * line to test IPC objects
       */
      memset (ipcscommand, '\0', sizeof (ipcscommand));
      sprintf (ipcscommand, "ipcs -%c >%s", ipc_obj[opt], tmpfile);
      system (ipcscommand);
      fd = fopen (tmpfile, "r");
      /* Skip the header */
      fgets (readbuf, sizeof (readbuf), fd);
      fgets (readbuf, sizeof (readbuf), fd);
      fgets (readbuf, sizeof (readbuf), fd);
      memset (readbuf, '\0', sizeof (readbuf));
      ret_char = fgets (readbuf, sizeof (readbuf), fd);
      while ((ret_char != NULL) && (*readbuf != '\n'))
        {
          sscanf (readbuf, "%s %d", ipc_buf, &ipc_id);
          /* ipc_id stores the id of ipc object */
          testall_ipc (ipc_id, ipc_obj[opt], uid, gid);
          memset (readbuf, '\0', sizeof (readbuf));
          ret_char = fgets (readbuf, sizeof (readbuf), fd);
        }
      fclose (fd);
      unlink (tmpfile);
    }
  return (0);
}

/*
 * Check access.
 *
 * This method check a file or recursively scan directories and verify
 * the file access modes are enforced.
 */
void
check_access (char *pathname, uid_t uid, gid_t gid)
{
  int count = 0;
  int i = 0;
  int rc = 0;
  char entry[MAXPATHLEN];
  struct dirent **entries;
  struct stat statbuf;

  /* Start with clean buffers */
  memset (&statbuf, '\0', sizeof (statbuf));

  /* Test System V Message Queue - IPC */
  if ((strcmp (pathname, "ipc_obj")) == 0)
    {
      printf ("Testing IPC objects\n");
      if (check_ipc (uid, gid) == -1)
        {
          printf ("\nERROR: %s. Could not obtain ipc "
                  "status. errno = %d\n", pathname, errno);
          goto EXIT;
        }
      goto EXIT;
    }
  /* Get file stat info. */
  if ((rc = lstat (pathname, &statbuf)) == -1)
    {
      printf ("\nERROR: %s. Could not obtain file status. errno = %d\n",
              pathname, errno);
      goto EXIT;
    }

  /* If link, skip it. */
  if (S_ISLNK (statbuf.st_mode))
    {
      printf ("Link: skipping %s\n", entry);
      goto EXIT;
    }

  /* If not a directory, check it and leave. */
  if (!(S_ISDIR (statbuf.st_mode)))
    {
      testall (&statbuf, pathname, uid, gid);
      goto EXIT;
    }

  /*
   *If directory, recurse through all subdirectories,
   * checking all files.
   */
  if ((count = scandir (pathname, &entries, file_select, alphasort)) == -1)
    {
      printf ("\nERROR: %s. Could not scandir. errno = %d\n",
              pathname, errno);
      goto EXIT;
    }
  for (i = 0; i < count; i++)
    {
      sprintf (entry, "%s/%s", pathname, entries[i]->d_name);
      /*
       * If link, skip it
       * Else if directory, call check_access() recursively
       */
      if (entries[i]->d_type == DT_LNK)
        {
          printf ("Link: skipping %s\n", entry);
          continue;
        }
      else if (entries[i]->d_type == DT_DIR)
        {
          check_access (entry, uid, gid);
          continue;
        }

      /* Clean the buffer */
      memset (&statbuf, '\0', sizeof (statbuf));

      /* Get file stat info. */
      if ((rc = lstat (entry, &statbuf)) == -1)
        {
          printf ("\nERROR: %s. Could not obtain file status. errno = %d\n",
                  pathname, errno);
          continue;
        }

      /* The directory entry doesn't always seem to have the
       * right info. So we check again after the stat().
       *
       * If link, skip it
       * Else if directory, call check_access() recursively
       * Else check access
       */
      if (S_ISLNK (statbuf.st_mode))
        {
          printf ("Link: (2) skipping %s\n", entry);
          continue;
        }
      else if (S_ISDIR (statbuf.st_mode))
        {
          check_access (entry, uid, gid);
          continue;
        }
      else
        {
          testall (&statbuf, entry, uid, gid);
          continue;
        }
    }
EXIT:
  return;
}

int
main (int argc, char *argv[])
{

  int i = 0;
  struct passwd *pw;
  struct group *gr;

  if (argc != 4)
    {
      printf ("usage: %s <directory> <testuser> <testgroup>\n", argv[0]);
      goto EXIT;
    }

  if ((pw = getpwnam (argv[2])) == NULL)
    {
      printf ("ERROR: invalid username %s\n", argv[2]);
      goto EXIT;
    }
  if ((gr = getgrnam (argv[3])) == NULL)
    {
      printf ("ERROR: invalid group %s\n", argv[3]);
      goto EXIT;
    }

#if 0
  /* drop all supplementary groups - we only rely on uid/gid */
  if (setgroups (0, NULL) == -1)
    {
      perror ("ERROR: can't drop supplementary groups");
      goto EXIT;
    }
#endif
  for (i = 0; i < 2; i++)
    {
      totalpass = 0;
      totalfail = 0;
      test = i;
      printf ("Test: %s\n\n", test_description[i]);
      check_access (argv[1], pw->pw_uid, gr->gr_gid);
      printf ("\n TESTS PASSED = %d, FAILED = %d\n", totalpass, totalfail);
    }
EXIT:
  return (0);
}
