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
     char *str3 = 0L;
     char *str = str3, c;

     while (1)
        {
           if (2)
             {
                if (3)
                  {
                 // This jumps to the false branch of the outer most conditional statement within the while loop.
                    goto comsub;
                  }
             }
            else
               if (4)
          comsub: {
                 // This label above is destination of a jump coming from the true branch 
                 // of a nested true branch all contained in the outer while loop).

                    int x = 42;
                  }
#if 1
                 else
                    if (asssub && ((c == '=') || c == ((char) 0x8b)) && str != str3)
                       {
                         pf_flags |= 0x04;
                       }
#endif
          str++;
        }

     return errflag ? ((void *)0) : node;
   }
