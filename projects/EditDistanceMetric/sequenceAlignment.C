#include "rose.h"

using namespace std;

#include "treeIsomorphism.h"
#include <math.h>

#if 0
// These seem to be good values for AST diff
static float compare(uint8_t a, uint8_t b) { return (a == b) ? 1.0f : -1.0f; }
static const float gapPenalty    = 1.8f;
static const float minCloneScore = 1.2f;
static const size_t cloneSep     = 2;
#endif

#define OUTPUT_CLONE_TRACE 0

static float compare(uint8_t a, uint8_t b)
   {
  // For binaries where padding can consist of 0's we want to provide less weight to them in the clone recognition.
  // return (a == b) ? (a == 0 ? .2f : 1.0f) : -1.0f;

  // For AST's there is no preference for 0's.
     return (a == b) ? 1.0f : -1.0f;
   }

static const float gapPenalty = 1.8f;

static const float minCloneScore = 1.2f;

static const size_t cloneSep = 2;



enum Step {END_OF_PATH, LEFT, UP, CHANGE_OR_KEEP};

#if OUTPUT_CLONE_TRACE
static char tochar(char x) 
   {
     if (isprint(x))
          return x;
       else 
          return '.';
   }
#endif

void
traceClone(const float* swTable, char* swDirs, const vector<int> & in1data, const vector<int> & in2data, size_t x, size_t y)
     {
       size_t in1size = in1data.size();
#if OUTPUT_CLONE_TRACE || !defined(NDEBUG)
       size_t origX = x, origY = y;
#endif
#if OUTPUT_CLONE_TRACE
       float score = swTable[y * in1size + x];
#endif
       vector<char> dirsInReversedTrace;
       top:
          {
            char dir = swDirs[y * in1size + x];
            swDirs[y * in1size + x] = dir | 4;
            switch (dir & 3)
               {
                 case END_OF_PATH:
                      goto pathEnd;

                 case LEFT:
                      if (x == 0)
                           goto pathEnd;
                      --x; 
                      dirsInReversedTrace.push_back(LEFT);
                      goto top;

                 case UP:
                      if (y == 0)
                           goto pathEnd;
                      --y;
                      dirsInReversedTrace.push_back(UP);
                      goto top;

                 case CHANGE_OR_KEEP:
                      if (x == 0 || y == 0)
                           goto pathEnd;
                      dirsInReversedTrace.push_back(CHANGE_OR_KEEP);
                      --x;
                      --y;
                      goto top;

                 default: assert (false);
               }
        }

     pathEnd:
#if OUTPUT_CLONE_TRACE
          fprintf(stderr, "Found similarity between [%zu, %zu) of file 1 and [%zu, %zu) of file 2 with score %f\n", x, origX, y, origY, score);
#endif
          for (size_t i = dirsInReversedTrace.size(); i > 0; --i)
             {
               char dir = dirsInReversedTrace[i - 1];
               switch (dir)
                  {
                    case END_OF_PATH:
                         break;

                    case LEFT:
                       {
                         ++x;
#if OUTPUT_CLONE_TRACE
                      // fprintf(stderr, "%02X %c d\n", (unsigned int)in1data[x], tochar(in1data[x]));
                         string s = Cxx_GrammarTerminalNames[in1data[x]].name;
                         fprintf(stderr, "%2d %30s d\n",in1data[x],s.c_str());
#endif
                         break;
                       }

                    case UP:
                       {
                         ++y;
#if OUTPUT_CLONE_TRACE
                      // fprintf(stderr, "     i %02X %c\n", (unsigned int)in2data[y], tochar(in2data[y]));
                         string s = Cxx_GrammarTerminalNames[in2data[y]].name;
                         fprintf(stderr, "     i %2d %s\n",in2data[y],s.c_str());
#endif
                         break;
                       }

                    case CHANGE_OR_KEEP:
                       {
                         ++x; ++y;
#if OUTPUT_CLONE_TRACE
                         unsigned int data1 = in1data[x];
                         unsigned int data2 = in2data[y];

                         string s1 = Cxx_GrammarTerminalNames[data1].name;
                         string s2 = Cxx_GrammarTerminalNames[data2].name;
                         if (data1 == data2)
                            {
                           // fprintf(stderr, "%02X %c   %02X %c\n", data1, tochar(data1), data2, tochar(data2));
                              fprintf(stderr, "%2d %30s    %2d %30s\n", data1, s1.c_str(), data2, s2.c_str());
                            }
                           else
                            {
                           // fprintf(stderr, "%02X %c c %02X %c\n", data1, tochar(data1), data2, tochar(data2));
                              fprintf(stderr, "%2d %30s c %2d %30s\n", data1, s1.c_str(), data2, s2.c_str());
                            }
#endif
                         break;
                       }

                    default: assert (false);
                  }
             }

     assert (x == origX && y == origY);
#if OUTPUT_CLONE_TRACE
     fprintf(stderr, "\n");
#endif
   }

void
sequenceAlignment ( vector< vector< SgNode* > > traversalTraceList )
   {
  // Convert AST trace to sequence of variants that can be compared in each sequence.
     size_t in1size = traversalTraceList[0].size();
     size_t in2size = traversalTraceList[1].size();

     printf ("in1size = %zu in2size = %zu \n",in1size,in2size);

     vector<int> in1data(in1size); // traversalTraceList[0]
     vector<int> in2data(in2size); // traversalTraceList[1]

  // Build the first input sequences
  // printf ("Sequence #1:");
     for (size_t i = 0; i < in1size; i++)
        {
          in1data[i] = traversalTraceList[0][i]->variantT();
       // printf (" %3d",in1data[i]);
        }
  // printf ("\n");

  // Build the second input sequences
  // printf ("Sequence #2:");
     for (size_t i = 0; i < in2size; i++)
        {
          in2data[i] = traversalTraceList[1][i]->variantT();
       // printf (" %3d",in2data[i]);
        }
  // printf ("\n");

     float *swTable = new float[in1size * in2size];
     char *swDirs   = new char [in1size * in2size];

     for (size_t y = 0; y < in2size; y++)
        {
          for (size_t x = 0; x < in1size; x++)
             {
               float left   = ((x == 0) ? 0.0f : swTable[y * in1size + x - 1]) - gapPenalty;
               float up     = ((y == 0) ? 0.0f : swTable[(y - 1) * in1size + x]) - gapPenalty;
               float upLeft = ((x == 0 || y == 0) ? 0.0f : swTable[(y - 1) * in1size + x - 1]) + compare(in1data[x], in2data[y]);
               char dir = END_OF_PATH;
               float best = 0.0f;
               if (upLeft > best) {best = upLeft; dir = CHANGE_OR_KEEP;}
               if (left > best) {best = left; dir = LEFT;}
               if (up > best) {best = up; dir = UP;}
               swTable[y * in1size + x] = best;
               swDirs[y * in1size + x] = dir;
             }
        }

#if 0
     printf ("\n\nOutput swTable:\n");
     for (size_t y = 0; y < in2size; y++)
        {
          for (size_t x = 0; x < in1size; x++)
             {
               printf (" %5.2f",swTable[y * in1size + x]);
             }
          printf ("\n");
        }

     printf ("\n\nOutput swDirs:\n");
     for (size_t y = 0; y < in2size; y++)
        {
          for (size_t x = 0; x < in1size; x++)
             {
               printf (" %2d",(int)swDirs[y * in1size + x]);
             }
          printf ("\n");
        }
#endif

  // Trace clones
  // This sets bit 2 in swDirs to mark cells that are parts of clones
     for (size_t y = 0; y < in2size; ++y)
        {
#if 0
          printf ("y = %zu in2data[y] = %d \n",y,in2data[y]);
#endif
          if (y != 0 && y % 16 == 0) {fprintf(stderr, "Tracing row %zu of %zu\n", y, in2size);}
          for (size_t x = 0; x < in1size; ++x)
             {
#if 0
               printf ("x = %zu in1data[x] = %d \n",x,in1data[x]);
#endif
               if (in1data[x] != in2data[y]) continue; // Not a candidate

               float val = swTable[y * in1size + x];
               if (val < minCloneScore) continue; // Not a candidate

            // Test to see if this cell is the maximum within the next cloneSep cells in its row and that they are not already in clones
               bool failed = false;
               for (size_t x2 = (x >= cloneSep ? x - cloneSep : 0); x2 < (x + cloneSep >= in1size ? in1size : x + cloneSep); ++x2)
                  {
                    if (x2 == x) continue;
                    if ((swDirs[y * in1size + x2] & 4) || (swTable[y * in1size + x2] > val))
                       {
                         failed = true;
                         break;
                       }
                  }

               if (failed) continue;

            // Test to see if this cell is the maximum within cloneSep cells in any direction and they are not in clones
               for (size_t y2 = (y >= cloneSep ? y - cloneSep : 0); y2 < (y + cloneSep >= in2size ? in2size : y + cloneSep); ++y2)
                  {
                    if (y2 == y) continue;
                    for (size_t x2 = (x >= cloneSep ? x - cloneSep : 0); x2 < (x + cloneSep >= in1size ? in1size : x + cloneSep); ++x2)
                       {
                         if ((swDirs[y2 * in1size + x2] & 4) || (swTable[y2 * in1size + x2] > val))
                            {
                              failed = true;
                              break;
                            }
                       }
                    if (failed) break;
                  }

               if (failed) continue;

            // printf ("Calling traceClone() \n");
               traceClone(swTable, swDirs, in1data, in2data, x, y);
            // printf ("DONE: Calling traceClone() \n");
             }
        }

  // printf ("Output a file... \n");

  // Print plot
     FILE* output1 = popen("pnmtopng > output1.png", "w");
     assert (output1);
     fprintf(output1, "P6 %zu %zu 255\n", in1size, in2size);
     FILE* output2 = popen("pnmtopng > output2.png", "w");
     assert (output2);
     fprintf(output2, "P6 %zu %zu 255\n", in1size, in2size);
     for (size_t y = 0; y < in2size; ++y)
        {
          if (y != 0 && y % 16 == 0) {fprintf(stderr, "Plotting row %zu of %zu\n", y, in2size);}

          for (size_t x = 0; x < in1size; ++x)
             {
               uint8_t r, g, b;
               heatmap(swTable[y * in1size + x], r, g, b);
               fprintf(output1, "%c%c%c", r, g, b);
               if (swDirs[y * in1size + x] & 4)
                  {
                    fprintf(output2, "\377\377\377");
                  }
                 else
                  {
                    fprintf(output2, "%c%c%c", r / 2, g / 2, b / 2);
                  }
             }
        }

     pclose(output1);
     pclose(output2);
  
     delete[] swTable;
     delete[] swDirs;
   }
