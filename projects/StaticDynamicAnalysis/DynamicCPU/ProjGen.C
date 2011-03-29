
/*

                              COPYRIGHT

Copyright 1993-2007, Lawrence Livermore National Security, LLC. All
rights reserved.  This work was produced at the Lawrence Livermore
National Laboratory (LLNL) under contract no. DE-AC52-07NA27344
(Contract 44) between the U.S. Department of Energy (DOE) and Lawrence
Livermore National Security, LLC (LLNS) for the operation of
LLNL. Copyright is reserved to Lawrence Livermore National Security,
LLC for purposes of controlled dissemination, commercialization
through formal licensing, or other disposition under terms of Contract
44; DOE policies, regulations and orders; and U.S. statutes. The
rights of the Federal Government are reserved under Contract 44.

                              DISCLAIMER

This work was prepared as an account of work sponsored by an agency of
the United States Government. Neither the United States Government nor
Lawrence Livermore National Security, LLC nor any of their employees,
makes any warranty, express or implied, or assumes any liability or
responsibility for the accuracy, completeness, or usefulness of any
information, apparatus, product, or process disclosed, or represents
that its use would not infringe privately-owned rights. Reference
herein to any specific commercial products, process, or service by
trade name, trademark, manufacturer or otherwise does not necessarily
constitute or imply its endorsement, recommendation, or favoring by
the United States Government or Lawrence Livermore National Security,
LLC. The views and opinions of authors expressed herein do not
necessarily state or reflect those of the United States Government or
Lawrence Livermore National Security, LLC, and shall not be used for
advertising or product endorsement purposes.

                        LICENSING REQUIREMENTS

Any use, reproduction, modification, or distribution of this software
or documentation for commercial purposes requires a license from
Lawrence Livermore National Security, LLC. Contact: Lawrence Livermore
National Laboratory, Industrial Partnerships Office, P.O. Box 808,
L-795, Livermore, CA 94551.

The Government is granted for itself and others acting on its behalf a
paid-up, nonexclusive, irrevocable worldwide license in this data to
reproduce, prepare derivative works, and perform publicly and display
publicly.

*/

/* Author:  Jeff Keasler */
/* This is a program that writes simulated programs containing    */
/* nested loops and sequential segments spread across several     */
/* functions and files.  Seqential code segment work is simulated */
/* by the memset function call.  Function call seeding has been   */
/* added so that two (or more) separate high level functions can  */
/* call a single lower level function.  This allows us to make    */
/* sure that unique measurement takes place for separate          */
/* call paths touching the same section of code.                  */

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#define ET_MAX_LEV    5
#define ET_MAX_FILES 10

using namespace std ;

static int levIts[5] = { 5, 10, 15, 25, 1000 } ;

static int numFiles ;
static int numFuncs ;
static int numLoops ;
static int maxDepth ;
static int levelCount[ET_MAX_LEV] ;
static int fileHist[ET_MAX_FILES] ;

struct ETnode
{
   bool isFunc ;
   int iterations ;
   int fileNum ;
   int funcNum ;
   int depth ;
   vector< ETnode *> children ;
   ETnode() { isFunc = false ; iterations = 0 ;
              fileNum = -1 ; funcNum = -1 ; depth = -1 ; }
   ~ETnode() {} 
} ;

static ETnode *funcPtr[10000] ;

/* generate a tree of loop level nests, creating random */
/* function/file boundaries at some nodes. */

void genTree(int level, ETnode **node)
{

   *node = new ETnode() ;
   (*node)->iterations = levIts[level] ;
   (*node)->depth = level ;
   if (random()%(2+2*level) == 0)
   {
      funcPtr[numFuncs] = *node ;
      (*node)->isFunc = true ;
      (*node)->funcNum = numFuncs++ ;
      (*node)->fileNum = random()%numFiles ;
      ++fileHist[(*node)->fileNum] ;
   }
   if (level+1 > maxDepth)
   {
      maxDepth = level+1 ;
   }
   ++numLoops ;
   ++levelCount[level] ;
   if ((level < ET_MAX_LEV-1) && ((random()%10) > 2) )
   {
      int i ;
      int numChildren = random()%7 + 2 ;
      (*node)->children.resize(numChildren) ;
      for (i=0; i<numChildren; ++i)
      {
         genTree(level+1, &(*node)->children[i]) ;
      }
   }

   return ;
}

/* Turn the tree into a DAG by adding random edges that connect */
/* nodes to other function-call nodes in the tree */
void seedTree(ETnode *node, int nFuncs, int maxDep, int modulus)
{
   if (node->depth < maxDep)
   {
      int i ;
      for (i=0; i<node->children.size(); ++i)
      {
         seedTree(node->children[i], nFuncs, maxDep, modulus) ;
      }
      if (random()%modulus == 0)
      {
         int testIdx ;
         do {
            testIdx = random()%nFuncs ;
         } while (funcPtr[testIdx]->depth <= node->depth) ;
         /* add a call */
         node->children.push_back(funcPtr[testIdx]) ;
         ++levelCount[node->depth] ;
      }
   }
}

/* find a node in the program tree that represents */
/* a call to a specific function ID */

ETnode *searchFunc(ETnode *root, int searchFuncNum)
{
   if (root->funcNum == searchFuncNum)
      return root ;
   else
   {
      int i ;
      ETnode *retval ;
      for (i=0; i<root->children.size(); ++i)
      {
         if ((retval = searchFunc(root->children[i], searchFuncNum)) != 0)
            return retval ;
      }
      return 0 ;
   }
}

/* This code generates output for a loop or function call */

void dumpNode(FILE *fp, ETnode *node, int currFunc)
{
   if (random()%10 == 0) /* simulate sequential work */
   {
      fprintf(fp, "memset(scratch, 0, 1000) ;\n") ;
   }

   if ((node->funcNum == -1) || (node->funcNum == currFunc))
   {
      int i ;
      fprintf(fp, "for (int i%d = 0 ; i%d < %d; ++i%d) {\n",
              node->depth, node->depth, node->iterations, node->depth) ;
      for (i=0; i<node->children.size(); ++i)
      {
         dumpNode(fp, node->children[i], currFunc) ;
      }
      fprintf(fp, "}\n") ;
   }
   else
   {
     char name[20] ;
     sprintf(name, "f%d() ;\n", node->funcNum) ;
     fprintf(fp, name) ;
   }

   if (random()%10 == 0)
   {
      fprintf(fp, "memset(scratch, 0, 1000) ;\n") ;
   }
}

/* This code generates output for a function definition */

void dumpfunc(FILE *fp, const char *funcName, ETnode *root, int currFunc)
{
   /* currFunc == -2 means main() */

   if (currFunc == -2)
      fprintf(fp, "\nchar scratch[10000] ;\n\n") ;

   fprintf(fp, "%s %s()\n{\n", (currFunc == -2 ? "int" : "void"), funcName) ;

   dumpNode(fp, root, currFunc) ;

   if (currFunc == -2)
      fprintf(fp, "return 0 ;\n") ;

   fprintf(fp, "}\n\n\n") ;
}

int main(int argc, char *argv[])
{
   int i ;
   ETnode *root ;
   char *dirname ;
   FILE *fp[ET_MAX_FILES] ;
   FILE *header ;
   FILE *makeFile ;

   if (argc != 3)
   {
      printf("Usage: %s <dirname> <num files>\n", argv[0]) ;
      exit(-1) ;
   }

   dirname  = argv[1] ;
   numFiles = atoi(argv[2]) ;

   if (numFiles > ET_MAX_FILES)
   {
      numFiles = ET_MAX_FILES ;
   }

   /****************************/
   /* Create Simulated Program */
   /****************************/

   genTree(0, &root) ;

   /* 25% of all nodes at a depth less than maxDepth */
   /* will be seeded with calls to functions at a lower tree depth */
   seedTree(root, numFuncs, maxDepth-1, (numLoops-levelCount[maxDepth-1])/25) ;

   /*****************************************/
   /* Echo Program statistics to the screen */
   /*****************************************/

   printf("numFiles = %d, numFuncs = %d, numLoops = %d, maxDepth = %d\n",
          numFiles, numFuncs, numLoops, maxDepth) ;

   for (i=0; i<ET_MAX_LEV; ++i)
   {
      printf("Lev %d = %d, ", i, levelCount[i]) ;
   }
   printf("\n") ;

   for (i=0; i<numFiles; ++i)
   {
      printf("File %d = %d, ", i, fileHist[i]) ;
   }
   printf("\n") ;

   /************************/
   /* Create Program Files */
   /************************/

   /* open the files */
   if (mkdir(dirname, 0x1ff) != 0)
   {
      printf("Could not create directory %s.  Aborting.\n", dirname) ;
      exit(-1) ;
   }
   if (chdir(dirname) != 0)
   {
      printf("Could not 'chdir %s'.  Aborting.\n", dirname) ;
      exit(-1) ;
   } 

   if ((header = fopen("header.h", "w")) == 0)
   {
      printf("could not create header file.  aborting\n") ;
      exit(-1) ;
   }

   fprintf(header, "\n#include <string.h>\n\nextern char scratch[10000] ;\n\n") ;
   for (i=0; i<numFuncs; ++i)
   {
      fprintf(header, "void f%d() ;\n", i) ;
   }
   fclose(header) ;

   for (i=0; i<numFiles; ++i)
   {
      if ((i == 0) || (fileHist[i] != 0))
      {
         char fname[30] ;
         sprintf(fname, "file%d.c", i) ;
         if ((fp[i] = fopen(fname, "w")) == 0)
         {
            printf("error creating %s. aborting\n", fname) ;
            numFiles = i ;
            for (i=0; i<numFiles; ++i)
            {
               if (fileHist[i] != 0)
               {
                  fclose(fp[i]) ;
               }
            }
            exit(-1) ;
         }
         fprintf(fp[i], "#include \"header.h\"\n\n") ;
      }
   }

   dumpfunc(fp[0], "main", root, -2) ;

   for (i=0; i<numFuncs; ++i)
   {
      char name[20] ;
      sprintf(name, "f%d", i) ;
      ETnode *node = searchFunc(root, i) ;
      dumpfunc(fp[node->fileNum], name, node, i) ;
   }

   for (i=0; i<numFiles; ++i)
   {
      if ((i == 0) || (fileHist[i] != 0))
      {
         fclose(fp[i]) ;
      }
   }

   /*******************/
   /* Create Makefile */
   /*******************/

   if ((makeFile = fopen("Makefile", "w")) == 0)
   {
      printf("could not create header file.  aborting\n") ;
      exit(-1) ;
   }

   fprintf(makeFile, "CFLAGS=-g -std=c99 -I$(srcdir)\nCC=../traceCPU\n\n") ;

   fprintf(makeFile, "checktest: ") ;
   for (i=0; i<numFiles; ++i)
   {
      if ((i == 0) || (fileHist[i] != 0))
      {
         char fname[30] ;
         sprintf(fname, "file%d.o ", i) ;
         fprintf(makeFile, fname) ;
      }
   }
   fprintf(makeFile, "\n\t$(CXX) ") ;
   for (i=0; i<numFiles; ++i)
   {
      if ((i == 0) || (fileHist[i] != 0))
      {
         char fname[30] ;
         sprintf(fname, "file%d.o ", i) ;
         fprintf(makeFile, fname) ;
      }
   }
   fprintf(makeFile, "-o checktest\n\n") ;

   /* system("ln -s ../SegDB.txt SegDB.txt") ; */
   fclose(makeFile); 
   return 0 ;
}

