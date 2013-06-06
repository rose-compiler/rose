// From Thomas Neimann <thomasn@jps.net> at http://www.cs.auckland.ac.nz/~jmor159/PLDS210/niemann/s_man.htm:
//     Permission to reproduce this document, in whole or in part, is given provided the original web site listed below is
//     referenced, and no additional restrictions apply. Source code, when part of a software project, may be used freely
//     without reference to the author.

/* external sort */

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace NAMESPACE {
namespace externalsort {

/****************************
 * implementation dependent *
 ****************************/

/* template for workfiles (8.3 format) */
#define FNAME "_sort%03d.dat"
#define LNAME 13

/* comparison operators */
#define compLT(x,y) (x < y)
#define compGT(x,y) (x > y)

/* define the record to be sorted here */
#define LRECL 100
typedef int keyType;
typedef struct recTypeTag {
    keyType key;                                /* sort key for record */
    #if LRECL
        char data[LRECL-sizeof(keyType)];       /* other fields */
    #endif
} recType;

/******************************
 * implementation independent *
 ******************************/

typedef struct tmpFileTag {
    FILE *fp;                   /* file pointer */
    char name[LNAME];           /* filename */
    recType rec;                /* last record read */
    int dummy;                  /* number of dummy runs */
    bool eof;                   /* end-of-file flag */
    bool eor;                   /* end-of-run flag */
    bool valid;                 /* true if rec is valid */
    int fib;                    /* ideal fibonacci number */
} tmpFileType;

static tmpFileType **file;      /* array of file info for tmp files */
static int nTmpFiles;           /* number of tmp files */
static char *ifName;            /* input filename */
static char *ofName;            /* output filename */

static int level;               /* level of runs */
static int nNodes;              /* number of nodes for selection tree */

void deleteTmpFiles(void) {
    int i;

    /* delete merge files and free resources */
    if (file) {
        for (i = 0; i < nTmpFiles; i++) {
            if (file[i]) {
                if (file[i]->fp) fclose(file[i]->fp);
                if (*file[i]->name) remove(file[i]->name);
                free (file[i]);
            }
        }
        free (file);
    }
}

void termTmpFiles(int rc) {

    /* cleanup files */
    remove(ofName);
    if (rc == 0) {
        int fileT;

        /* file[T] contains results */
        fileT = nTmpFiles - 1;
        fclose(file[fileT]->fp); file[fileT]->fp = NULL;
        if (rename(file[fileT]->name, ofName)) {
            perror("io1");
            deleteTmpFiles();
            exit(1);
        }
        *file[fileT]->name = 0;
    }
    deleteTmpFiles();
}

void cleanExit(int rc) {

    /* cleanup tmp files and exit */
    termTmpFiles(rc);
    exit(rc);
}

void *safeMalloc(size_t size) {
    void *p;

    /* safely allocate memory and initialize to zero */
    if ((p = calloc(1, size)) == NULL) {
        printf("error: malloc failed, size = %zu\n", size);
        cleanExit(1);
    }
    return p;
}

void initTmpFiles(void) {
    int i;
    tmpFileType *fileInfo;

    /* initialize merge files */
    if (nTmpFiles < 3) nTmpFiles = 3;
    file = (tmpFileType**)safeMalloc(nTmpFiles * sizeof(tmpFileType*));
    fileInfo = (tmpFileType*)safeMalloc(nTmpFiles * sizeof(tmpFileType));
    for (i = 0; i < nTmpFiles; i++) {
        file[i] = fileInfo + i;
        sprintf(file[i]->name, FNAME, i);
        if ((file[i]->fp = fopen(file[i]->name, "w+b")) == NULL) {
            perror("io2");
            cleanExit(1);
        }
    }
}

recType *readRec(void) {

    typedef struct iNodeTag {   /* internal node */
        struct iNodeTag *parent;/* parent of internal node */
        struct eNodeTag *loser; /* external loser */
    } iNodeType;

    typedef struct eNodeTag {   /* external node */
        struct iNodeTag *parent;/* parent of external node */
        recType rec;            /* input record */
        int run;                /* run number */
        bool valid;             /* input record is valid */
    } eNodeType;

    typedef struct nodeTag {
        iNodeType i;            /* internal node */
        eNodeType e;            /* external node */
    } nodeType;

    static nodeType *node;      /* array of selection tree nodes */
    static eNodeType *win;      /* new winner */
    static FILE *ifp;           /* input file */
    static bool eof;            /* true if end-of-file, input */
    static int maxRun;          /* maximum run number */
    static int curRun;          /* current run number */
    iNodeType *p;               /* pointer to internal nodes */
    static bool lastKeyValid;   /* true if lastKey is valid */
    static keyType lastKey;     /* last key written */

    /* read next record using replacement selection */

    /* check for first call */
    if (node == NULL) {
        int i;

        if (nNodes < 2) nNodes = 2;
        node = (nodeType*)safeMalloc(nNodes * sizeof(nodeType));
        for (i = 0; i < nNodes; i++) {
            node[i].i.loser = &node[i].e;
            node[i].i.parent = &node[i/2].i;
            node[i].e.parent = &node[(nNodes + i)/2].i;
            node[i].e.run = 0;
            node[i].e.valid = false;
        }
        win = &node[0].e;
        lastKeyValid = false;

        if ((ifp = fopen(ifName, "rb")) == NULL) {
            printf("error: file %s, unable to open\n", ifName);
            cleanExit(1);
        }
    }

    while (1) {

        /* replace previous winner with new record */
        if (!eof) {
            if (fread(&win->rec, sizeof(recType), 1, ifp) == 1) {
                if ((!lastKeyValid || compLT(win->rec.key, lastKey))
                && (++win->run > maxRun))
                    maxRun = win->run;
                win->valid = true;
            } else if (feof(ifp)) {
                fclose(ifp);
                eof = true;
                win->valid = false;
                win->run = maxRun + 1;
            } else {
                perror("io4");
                cleanExit(1);
            } 
        } else {
            win->valid = false;
            win->run = maxRun + 1;
        }

        /* adjust loser and winner pointers */
        p = win->parent;
        do {
            bool swap;
            swap = false;
            if (p->loser->run < win->run) {
                swap = true;
            } else if (p->loser->run == win->run) {
                if (p->loser->valid && win->valid) {
                    if (compLT(p->loser->rec.key, win->rec.key))
                        swap = true;
                } else {
                    swap = true;
                }
            }
            if (swap) {
                /* p should be winner */
                eNodeType *t;

                t = p->loser;
                p->loser = win;
                win = t;
            }
            p = p->parent;
        } while (p != &node[0].i);

        /* end of run? */
        if (win->run != curRun) {
            /* win->run = curRun + 1 */
            if (win->run > maxRun) {
                /* end of output */
                free(node);
                return NULL;
            }
            curRun = win->run;
        }

        /* output top of tree */
        if (win->run) {
            lastKey = win->rec.key;
            lastKeyValid = true;
            return &win->rec;
        }
    }
}

void makeRuns(void) {
    recType *win;       /* winner */
    int fileT;          /* last file */
    int fileP;          /* next to last file */
    int j;              /* selects file[j] */


    /* Make initial runs using replacement selection.
     * Runs are written using a Fibonacci distintbution.
     */

    /* initialize file structures */
    fileT = nTmpFiles - 1;
    fileP = fileT - 1;
    for (j = 0; j < fileT; j++) {
        file[j]->fib = 1;
        file[j]->dummy = 1;
    }
    file[fileT]->fib = 0;
    file[fileT]->dummy = 0;

    level = 1;
    j = 0;


    win = readRec();
    while (win) {
        bool anyrun;

        anyrun = false;
        for (j = 0; win && j <= fileP; j++) {
            bool run;

            run = false;
            if (file[j]->valid) {
                if (!compLT(win->key, file[j]->rec.key)) {
                    /* append to an existing run */
                    run = true;
                } else if (file[j]->dummy) {
                    /* start a new run */
                    file[j]->dummy--;
                    run = true;
                }
            } else {
                /* first run in file */
                file[j]->dummy--;
                run = true;
            }

            if (run) {
                anyrun = true;

                /* flush run */
                while(1) {
                    if (fwrite(win, sizeof(recType), 1, file[j]->fp) != 1) {
                        perror("io3");
                        cleanExit(1);
                    }
                    file[j]->rec.key = win->key;
                    file[j]->valid = true;
                    if ((win = readRec()) == NULL) break;
                    if (compLT(win->key, file[j]->rec.key)) break;
                }
            }
        }

        /* if no room for runs, up a level */
        if (!anyrun) {
            int t;
            level++;
            t = file[0]->fib;
            for (j = 0; j <= fileP; j++) {
                file[j]->dummy = t + file[j+1]->fib - file[j]->fib;
                file[j]->fib = t + file[j+1]->fib; 
            }
        }
    }
}

void rewindFile(int j) {
    /* rewind file[j] and read in first record */
    file[j]->eor = false;
    file[j]->eof = false;
    rewind(file[j]->fp);
    if (fread(&file[j]->rec, sizeof(recType), 1, file[j]->fp) != 1) {
        if (feof(file[j]->fp)) {
            file[j]->eor = true;
            file[j]->eof = true;
        } else {
            perror("io5");
            cleanExit(1);
        }
    }
}

void mergeSort(void) {
    int fileT;
    int fileP;
    int j;
    tmpFileType *tfile;

    /* polyphase merge sort */

    fileT = nTmpFiles - 1;
    fileP = fileT - 1;

    /* prime the files */
    for (j = 0; j < fileT; j++) {
        rewindFile(j);
    }

    /* each pass through loop merges one run */
    while (level) {
        while(1) {
            bool allDummies;
            bool anyRuns;

            /* scan for runs */
            allDummies = true;
            anyRuns = false;
            for (j = 0; j <= fileP; j++) {
                if (!file[j]->dummy) {
                    allDummies = false;
                    if (!file[j]->eof) anyRuns = true;
                }
            }

            if (anyRuns) {
                int k;
                keyType lastKey;

                /* merge 1 run file[0]..file[P] --> file[T] */

                while(1) {
                    /* each pass thru loop writes 1 record to file[fileT] */

                    /* find smallest key */
                    k = -1;
                    for (j = 0; j <= fileP; j++) {
                        if (file[j]->eor) continue;
                        if (file[j]->dummy) continue;
                        if (k < 0 || 
                        (k != j && compGT(file[k]->rec.key, file[j]->rec.key)))
                            k = j;
                    }
                    if (k < 0) break;

                    /* write record[k] to file[fileT] */
                    if (fwrite(&file[k]->rec, sizeof(recType), 1, 
                            file[fileT]->fp) != 1) {
                        perror("io6");
                        cleanExit(1);
                    }

                    /* replace record[k] */
                    lastKey = file[k]->rec.key;
                    if (fread(&file[k]->rec, sizeof(recType), 1,
                            file[k]->fp) == 1) {
                        /* check for end of run on file[s] */
                        if (compLT(file[k]->rec.key, lastKey))
                            file[k]->eor = true;
                    } else if (feof(file[k]->fp)) {
                        file[k]->eof = true;
                        file[k]->eor = true;
                    } else {
                        perror("io7");
                        cleanExit(1);
                    }
                }

                /* fixup dummies */
                for (j = 0; j <= fileP; j++) {
                    if (file[j]->dummy) file[j]->dummy--;
                    if (!file[j]->eof) file[j]->eor = false;
                }

            } else if (allDummies) {
                for (j = 0; j <= fileP; j++)
                    file[j]->dummy--;
                file[fileT]->dummy++;
            }

            /* end of run */
            if (file[fileP]->eof && !file[fileP]->dummy) {
                /* completed a fibonocci-level */
                level--;
                if (!level) {
                    /* we're done, file[fileT] contains data */
                    return;
                }

                /* fileP is exhausted, reopen as new */
                fclose(file[fileP]->fp);
                if ((file[fileP]->fp = fopen(file[fileP]->name, "w+b"))
                        == NULL) {
                    perror("io8");
                    cleanExit(1);
                }
                file[fileP]->eof = false;
                file[fileP]->eor = false;

                rewindFile(fileT);

                /* f[0],f[1]...,f[fileT] <-- f[fileT],f[0]...,f[T-1] */
                tfile = file[fileT];
                memmove(file + 1, file, fileT * sizeof(tmpFileType*));
                file[0] = tfile;

                /* start new runs */
                for (j = 0; j <= fileP; j++)
                    if (!file[j]->eof) file[j]->eor = false;
            }
        }

    }
}


void extSort(void) {
    initTmpFiles();
    makeRuns();
    mergeSort();
    termTmpFiles(0);
}

} // namespace
} // namespace
