// Sequence Alignment Algorithms

#if 0
// Details from: http://www.seas.gwu.edu/~simhaweb/cs151/lectures/module12/align.html

Algorithm: align (s1, s2)
Input: string s1 of length m, string s2 of length n

1.    Initialize matrix D properly;

      // Build the matrix D row by row.
2.    for i=1 to m
3.      for j=1 to n

          // Initialize max to the first of the three terms (NORTH).
4.        max = D[i-1][j] + gapScore2 (s2[j-1])

          // See if the second term is larger (WEST).
5.        if max < D[i][j-1] + gapScore1 (s1[i-1]) 
6.          max = D[i][j-1] + gapScore1 (s2[i-1]) 
7.        endif

          // See if the third term is the largest (NORTHWEST).
8.        if max < D[i-1][j-1] + matchScore (s1[i-1], s2[j-1])
9.          max = D[i-1][j-1] + matchScore (s1[i-1], s2[j-1])
10.       endif

11.        D[i][j] = max

12.     endfor
13.   endfor

      // Return the optimal value in bottom-right corner.
14.   return D[m][n]

Output: value of optimal alignment
#endif

const int n = 10;
const int m = 10;

#define matchScore(n,m) ((n) == (m) ? 1 : 0)
#define gapScore1(n) -1
#define gapScore2(n) -2

void
NeedlemanWunschAlgorithm( vector<int> s1, vector<int> s2 )
   {
      int D[m][n];
      for (int i=1; i < m; i++)
        {
           for (int j=1; j < n; j++)
             {
               D[i][j] = 0;
             }
        }

      for (int i=1; i < m; i++)
        {
           for (int j=1; j < n; j++)
             {
            // Initialize max to the first of the three terms (NORTH).
               int max = D[i-1][j] + gapScore2(s2[j-1]);

            // See if the second term is larger (WEST).
               if max < D[i][j-1] + gapScore1(s1[i-1]) 
                    max = D[i][j-1] + gapScore1(s2[i-1]);

            // See if the third term is the largest (NORTHWEST).
               if max < D[i-1][j-1] + matchScore(s1[i-1], s2[j-1])
                    max = D[i-1][j-1] + matchScore(s1[i-1], s2[j-1]);

               D[i][j] = max;
             }
        }
   }
