#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <libgen.h>
int readseqs(int first_seq, char *filename);
void forward_pass(char *ia, char *ib, int n, int m, int *se1, int *se2, int *maxscore, int g, int gh);
void reverse_pass(char *ia, char *ib, int se1, int se2, int *sb1, int *sb2, int maxscore, int g, int gh);
int diff (int A, int B, int M, int N, int tb, int te, int *print_ptr, int *last_print, int *displ, int seq1, int seq2, int g, int gh);
double tracepath(int tsb1, int tsb2, int *print_ptr, int *last_print, int *displ, int seq1, int seq2);
int get_matrix(int*, int*, int);
void bots_message (char*);
int ktup, window, signif;
int prot_ktup, prot_window, prot_signif;

int gap_pos1, gap_pos2, mat_avscore;
int nseqs, max_aa;
#define MAX_ALN_LENGTH 5000
#define NUMRES 256
int *seqlen_array, def_aa_xref[NUMRES+1];

int *bench_output, *seq_output;

double gap_open,      gap_extend;
double prot_gap_open, prot_gap_extend;
double pw_go_penalty, pw_ge_penalty;
double prot_pw_go_penalty, prot_pw_ge_penalty;

char **args, **names, **seq_array;

int matrix[NUMRES][NUMRES];

#define MIN(a,b) ((a)<(b)?(a):(b))
#define tbgap(k) ((k) <= 0 ? 0 : tb + gh * (k))
#define tegap(k) ((k) <= 0 ? 0 : te + gh * (k))

int pairalign(int istart, int iend, int jstart, int jend)
{
   int i, n, m, si, sj;
   int len1, len2, maxres;
   double gg, mm_score;
   int    *mat_xref, *matptr;

   matptr   = 0;
   mat_xref = def_aa_xref;
   maxres = get_matrix(matptr, mat_xref, 10);
   if (maxres == 0) return(-1);

        bots_message("Start aligning ");
   #pragma omp parallel
   {
   #pragma omp for schedule(dynamic) private(i,n,si,sj,len1,m)
      for (si = 0; si < nseqs; si++) {
         if ((n = seqlen_array[si+1]) != 0){
            for (i = 1, len1 = 0; i <= n; i++) {
               char c = seq_array[si+1][i];
               if ((c != gap_pos1) && (c != gap_pos2)) len1++;
            }
	    printf("len %p %d n %d\n", &len1, len1, n);

            for (sj = si + 1; sj < nseqs; sj++) 
            {
               if ((m = seqlen_array[sj+1]) != 0)
               {
                  #pragma omp task untied \
                  private(i,gg,len2,mm_score) firstprivate(m,n,si,sj,len1) \
                  shared(nseqs, bench_output,seqlen_array,seq_array,gap_pos1,gap_pos2,pw_ge_penalty,pw_go_penalty,mat_avscore)
                  {
                  int se1, se2, sb1, sb2, maxscore, seq1, seq2, g, gh;
                  int displ[2*MAX_ALN_LENGTH+1];
                  int print_ptr, last_print;

                  for (i = 1, len2 = 0; i <= m; i++) {
                     char c = seq_array[sj+1][i];
                     if ((c != gap_pos1) && (c != gap_pos2)) len2++;
                  }

                  gh = 10 * pw_ge_penalty;
                  gg = pw_go_penalty + log((double) MIN(n, m));
                  g  = (mat_avscore <= 0) ? 20 * gg : 2 * mat_avscore * gg;

                  seq1 = si + 1;
                  seq2 = sj + 1;

                  forward_pass(&seq_array[seq1][0], &seq_array[seq2][0], n, m, &se1, &se2, &maxscore, g, gh);
                  reverse_pass(&seq_array[seq1][0], &seq_array[seq2][0], se1, se2, &sb1, &sb2, maxscore, g, gh);

                  print_ptr  = 1;
                  last_print = 0;

                  diff(sb1-1, sb2-1, se1-sb1+1, se2-sb2+1, 0, 0, &print_ptr, &last_print, displ, seq1, seq2, g, gh);
                  mm_score = tracepath(sb1, sb2, &print_ptr, &last_print, displ, seq1, seq2);

                  if (len1 == 0 || len2 == 0) mm_score  = 0.0;
                  else                        mm_score /= (double) MIN(len1,len2);

                  bench_output[si*nseqs+sj] = mm_score;
                  }
               }
            }
         }
      }
   }
        bots_message(" completed!\n");
   return 0;
}
