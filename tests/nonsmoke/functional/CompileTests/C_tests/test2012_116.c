// Subset of file from zsh C application (file: subst.c).
// It is interesting because it shows label attached 
// to the TRUE branch of a conditional statment 
// (if statement).

typedef long unsigned int size_t;

typedef struct linknode *LinkNode;

struct linknode {
    LinkNode next;
    LinkNode prev;
    void *dat;
};

typedef union linkroot *LinkList;
struct linklist {
    LinkNode first;
    LinkNode last;
    int flags;
};

union linkroot {
    struct linklist list;
    struct linknode node;
};

extern int errflag;


enum {
    OPT_INVALID,
    ALIASESOPT,
    ALLEXPORT,
    ALWAYSLASTPROMPT,
    ALWAYSTOEND,
    APPENDHISTORY,
    AUTOCD,
    AUTOCONTINUE,
    AUTOLIST,
    AUTOMENU,
    AUTONAMEDIRS,
    AUTOPARAMKEYS,
    AUTOPARAMSLASH,
    AUTOPUSHD,
    AUTOREMOVESLASH,
    AUTORESUME,
    BADPATTERN,
    BANGHIST,
    BAREGLOBQUAL,
    BASHAUTOLIST,
    BASHREMATCH,
    BEEP,
    BGNICE,
    BRACECCL,
    BSDECHO,
    CASEGLOB,
    CASEMATCH,
    CBASES,
    CDABLEVARS,
    CHASEDOTS,
    CHASELINKS,
    CHECKJOBS,
    CLOBBER,
    COMBININGCHARS,
    COMPLETEALIASES,
    COMPLETEINWORD,
    CORRECT,
    CORRECTALL,
    CPRECEDENCES,
    CSHJUNKIEHISTORY,
    CSHJUNKIELOOPS,
    CSHJUNKIEQUOTES,
    CSHNULLCMD,
    CSHNULLGLOB,
    DEBUGBEFORECMD,
    EMACSMODE,
    EQUALS,
    ERREXIT,
    ERRRETURN,
    EXECOPT,
    EXTENDEDGLOB,
    EXTENDEDHISTORY,
    EVALLINENO,
    FLOWCONTROL,
    FUNCTIONARGZERO,
    GLOBOPT,
    GLOBALEXPORT,
    GLOBALRCS,
    GLOBASSIGN,
    GLOBCOMPLETE,
    GLOBDOTS,
    GLOBSUBST,
    HASHCMDS,
    HASHDIRS,
    HASHEXECUTABLESONLY,
    HASHLISTALL,
    HISTALLOWCLOBBER,
    HISTBEEP,
    HISTEXPIREDUPSFIRST,
    HISTFCNTLLOCK,
    HISTFINDNODUPS,
    HISTIGNOREALLDUPS,
    HISTIGNOREDUPS,
    HISTIGNORESPACE,
    HISTLEXWORDS,
    HISTNOFUNCTIONS,
    HISTNOSTORE,
    HISTREDUCEBLANKS,
    HISTSAVEBYCOPY,
    HISTSAVENODUPS,
    HISTSUBSTPATTERN,
    HISTVERIFY,
    HUP,
    IGNOREBRACES,
    IGNORECLOSEBRACES,
    IGNOREEOF,
    INCAPPENDHISTORY,
    INTERACTIVE,
    INTERACTIVECOMMENTS,
    KSHARRAYS,
    KSHAUTOLOAD,
    KSHGLOB,
    KSHOPTIONPRINT,
    KSHTYPESET,
    KSHZEROSUBSCRIPT,
    LISTAMBIGUOUS,
    LISTBEEP,
    LISTPACKED,
    LISTROWSFIRST,
    LISTTYPES,
    LOCALOPTIONS,
    LOCALTRAPS,
    LOGINSHELL,
    LONGLISTJOBS,
    MAGICEQUALSUBST,
    MAILWARNING,
    MARKDIRS,
    MENUCOMPLETE,
    MONITOR,
    MULTIBYTE,
    MULTIFUNCDEF,
    MULTIOS,
    NOMATCH,
    NOTIFY,
    NULLGLOB,
    NUMERICGLOBSORT,
    OCTALZEROES,
    OVERSTRIKE,
    PATHDIRS,
    PATHSCRIPT,
    POSIXALIASES,
    POSIXBUILTINS,
    POSIXCD,
    POSIXIDENTIFIERS,
    POSIXJOBS,
    POSIXSTRINGS,
    POSIXTRAPS,
    PRINTEIGHTBIT,
    PRINTEXITVALUE,
    PRIVILEGED,
    PROMPTBANG,
    PROMPTCR,
    PROMPTPERCENT,
    PROMPTSP,
    PROMPTSUBST,
    PUSHDIGNOREDUPS,
    PUSHDMINUS,
    PUSHDSILENT,
    PUSHDTOHOME,
    RCEXPANDPARAM,
    RCQUOTES,
    RCS,
    RECEXACT,
    REMATCHPCRE,
    RESTRICTED,
    RMSTARSILENT,
    RMSTARWAIT,
    SHAREHISTORY,
    SHFILEEXPANSION,
    SHGLOB,
    SHINSTDIN,
    SHNULLCMD,
    SHOPTIONLETTERS,
    SHORTLOOPS,
    SHWORDSPLIT,
    SINGLECOMMAND,
    SINGLELINEZLE,
    SOURCETRACE,
    SUNKEYBOARDHACK,
    TRANSIENTRPROMPT,
    TRAPSASYNC,
    TYPESETSILENT,
    UNSET,
    VERBOSE,
    VIMODE,
    WARNCREATEGLOBAL,
    XTRACE,
    USEZLE,
    DVORAK,
    OPT_SIZE
};

extern char opts[OPT_SIZE];
extern short int typtab[256];
extern char ztokens[];



extern char*getproc (char*cmd,char**eptr);

// Note that if this function prototype is not included then we get strange errors (function prototype is output in the true branch of a if statment???).
extern LinkList getoutput (char*cmd,int qt);

extern char*getoutputfile (char*cmd,char**eptr);
extern size_t strlen (__const char *__s) __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
extern void*hcalloc (size_t size);
extern void *memcpy (void *__restrict __dest, __const void *__restrict __src, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern int skipparens (char inpar,char outpar,char**s);
extern void zerr(const char *fmt, ...);
static char*arithsubst (char*a,char**bptr,char*rest);
static char*stringsubstquote (char*strstart,char**pstrdpos);
static LinkNode paramsubst (LinkList l,LinkNode n,char**str,int qt,int pf_flags);
extern char *strncpy (char *__restrict __dest, __const char *__restrict __src, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern void*ugetnode (LinkList list);
extern void shtokenize (char*s);
extern void insertlinklist (LinkList l,LinkNode where,LinkList x);


static LinkNode
stringsubst(LinkList list, LinkNode node, int pf_flags, int asssub)
   {
     int qt;
     char *str3 = (char *)((node)->dat);
     char *str = str3, c;

     while (!errflag && (c = *str))
        {
          if (((c = *str) == ((char) 0x92) || c == ((char) 0x94) || (str == str3 && c == ((char) 0x8b))) && str[1] == ((char) 0x88))
             {
               char *subst, *rest, *snew, *sptr;
               int str3len = str - str3, sublen, restlen;

               if (c == ((char) 0x92) || c == ((char) 0x94))
                    subst = getproc(str, &rest);
                 else
                    subst = getoutputfile(str, &rest);
               if (errflag)
                    return ((void *)0);
               if (!subst)
                    subst = "";

               sublen = strlen(subst);
               restlen = strlen(rest);
               sptr = snew = hcalloc(str3len + sublen + restlen + 1);
               if (str3len)
                  {
                    memcpy(sptr, str3, str3len);
                    sptr += str3len;
                  }
               if (sublen)
                  {
                    memcpy(sptr, subst, sublen);
                    sptr += sublen;
                  }
               if (restlen)
                    memcpy(sptr, rest, restlen);
               sptr[restlen] = '\0';
               str3 = snew;
               str = snew + str3len + sublen;
               ((node)->dat = (str3));
             }
            else
               str++;
        }

     str = str3;

     while (!errflag && (c = *str))
        {
          if ((qt = c == ((char) 0x8a)) || c == ((char) 0x85))
             {
               if ((c = str[1]) == ((char) 0x88))
                  {
                    if (!qt)
                         list->list.flags |= 1;
                    str++;

                 // This jumps to the false branch of the outer most conditional statement within the while loop.
                    goto comsub;
                  }
                 else
                    if (c == ((char) 0x8f))
                       {
                         char *str2 = str;
                         str2++;
                         if (skipparens(((char) 0x8f), ((char) 0x90), &str2))
                            {
                              zerr("closing bracket missing");
                              return ((void *)0);
                            }
                         str2[-1] = *str = '\0';
                         str = arithsubst(str + 2, &str3, str2);
                         ((node)->dat = ((void *) str3));
                         continue;
                       } 
                      else
                         if (c == ((char) 0x99))
                            {
                              str3 = stringsubstquote(str3, &str);
                              ((node)->dat = ((void *) str3));
                              continue;
                            }
                           else
                            {
// # 232 "subst.c"
                              if (((opts[SHWORDSPLIT]) && !(pf_flags & 0x20)) || (pf_flags & 0x08))
                                   pf_flags |= 0x10;
                              node = paramsubst(list, node, &str, qt, pf_flags & (0x04|0x10));
                              if (errflag || !node)
                                   return ((void *)0);
                              str3 = (char *)((node)->dat);
                              continue;
                            }
             }
            else
               if ((qt = c == ((char) 0x97)) || (c == ((char) 0x91) ? (list->list.flags |= 1) : 0))
          comsub: {
                 // This label above is destination of a jump coming from the true branch 
                 // of a nested true branch all contained in the outer while loop).

                    LinkList pl;
                    char *s, *str2 = str;
                    char endchar;
                    int l1, l2;

                    if (c == ((char) 0x88))
                       {
                         endchar = ((char) 0x89);
                         str[-1] = '\0';
                         skipparens(((char) 0x88), ((char) 0x89), &str);
                         str--;
                       }
                      else
                       {
                         endchar = c;
                         *str = '\0';

                         while (*++str != endchar);
                       }
                    *str++ = '\0';
                    if (endchar == ((char) 0x89) && str2[1] == '(' && str[-2] == ')')
                       {
                         str[-2] = '\0';
                         if ((opts[EXECOPT]))
                              str = arithsubst(str2 + 2, &str3, str);
                           else
                              strncpy(str3, str2, 1);
                         ((node)->dat = ((void *) str3));
                         continue;
                       }

                    for (str = str2; (c = *++str); )
                         if ((typtab[((unsigned char)(c))] & (1 << 4)) && c != ((char) 0x9d) && !(endchar != ((char) 0x89) && c == ((char) 0x9b) && (str[1] == '$' || str[1] == '\\' || str[1] == '`' || (qt && str[1] == '"'))))
                              *str = ztokens[c - ((char) 0x84)];
                    str++;
                    if (!(pl = getoutput(str2 + 1, qt || (pf_flags & 0x04)))) 
                       {
                         zerr("parse error in command substitution");
                         return ((void *)0);
                       }
                    if (endchar == ((char) 0x89))
                         str2--;
                    if (!(s = (char *) ugetnode(pl)))
                       {
                         str = strcpy(str2, str);
                         continue;
                       }
                    if (!qt && (pf_flags & 0x04) && (opts[GLOBSUBST]))
                         shtokenize(s);
                    l1 = str2 - str3;
                    l2 = strlen(s);
                    if ((((pl)->list.first) != ((void *)0)))
                       {
                         LinkNode n = ((pl)->list.last);
                         str2 = (char *) hcalloc(l1 + l2 + 1);
                         strcpy(str2, str3);
                         strcpy(str2 + l1, s);
                         ((node)->dat = (str2));
                         insertlinklist(pl, node, list);
                         s = (char *) ((node = n)->dat);
                         l1 = 0;
                         l2 = strlen(s);
                       }
                    str2 = (char *) hcalloc(l1 + l2 + strlen(str) + 1);
                    if (l1)
                         strcpy(str2, str3);
                    strcpy(str2 + l1, s);
                    str = strcpy(str2 + l1 + l2, str);
                    str3 = str2;
                    ((node)->dat = (str3));
                    continue;
                  } 
                 else
                    if (asssub && ((c == '=') || c == ((char) 0x8b)) && str != str3)
                       {
                         pf_flags |= 0x04;
                       }
          str++;
        }

     return errflag ? ((void *)0) : node;
   }
