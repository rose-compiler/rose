/*
 * Calibrator v0.9e
 * by Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/Calibrator/
 *
 * All rights reserved.
 * No warranties.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above notice, this list
 *    of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above notice, this
 *    list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by
 *      Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/.
 * 4. Any publication of result obtained by use of this software must
 *    display a reference as follows:
 *      Results produced by Calibrator v0.9e
 *      (Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/)
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS `AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
#include        <unistd.h>
*/
#include        <stdlib.h>
#include        <stdio.h>
#include        <math.h>
#include        <string.h>
#include        <stdarg.h>

#define VERSION "0.9e"

#define NUMLOADS 100000
#define REDUCE   10
#define NUMTRIES 3
#define MINRANGE 1024
#define MAXLEVELS 9
#define LENPLATEAU 3

        #define EPSILON1 0.1    
/*      #define EPSILON3 2.0    */

/*      #define EPSILON2 0.04   */
        #define EPSILON4 1.0    


#define MIN(a,b)        (a<b?a:b)
#define MAX(a,b)        (a>b?a:b)
#define NOABS(x)        ((dbl)x)
#define FABS            fabs

#define lng     long 
#define dbl     float

lng MINTIME = 10000;

#ifdef WIN32

#include <Windows.h>

/*      #define MINTIME  100000 */

size_t getpagesize() { return 4096; }

lng oldtp = 0;

lng now(void)
{
        lng tp = (lng)timeGetTime();
        if (oldtp == 0) {
                /* timeBeginPeriod(1); */
                tp += 11;
                while ((lng)timeGetTime() <= tp) ;
                oldtp = tp = (lng)timeGetTime();
        }
        return (lng)((tp - oldtp) * (lng)1000);
}

#else

#include <sys/time.h>

/*      #define MINTIME  10000  */

struct timeval oldtp = { 0 };

lng now(void)
{
        struct timeval tp;
        gettimeofday(&tp, 0);
        if (oldtp.tv_sec == 0 && oldtp.tv_usec == 0) {
                oldtp = tp;
        }
        return (lng)( (lng)(tp.tv_sec  - oldtp.tv_sec ) * (lng)1000000 + 
                        (lng)(tp.tv_usec - oldtp.tv_usec)       );
}

#endif


void ErrXit(char *format, ...) {
        va_list ap;
        char    s[1024];

        va_start(ap,format);
        vsprintf(s, format, ap);
        va_end(ap);
        fprintf(stderr, "\n! %s !\n", s);
        fflush(stderr);
        exit(1);
}

lng round(dbl x)
{
        return (lng)(x + 0.5);
}

char last(char *s)
{
        while (*s++);
        return (s[-2]);
}

lng bytes(char *s)
{
        lng     n = atoi(s);

        if ((last(s) == 'k') || (last(s) == 'K'))
                n *= 1024;
        if ((last(s) == 'm') || (last(s) == 'M'))
                n *= (1024 * 1024);
        if ((last(s) == 'g') || (last(s) == 'G'))
                n *= (1024 * 1024 * 1024);
        return (n);
}

#define NSperIt(t)      (((dbl)(t)) / (((dbl)NUMLOADS) / 1000.0))
#define CYperIt(t)      (((dbl)((t) * MHz)) / ((dbl)NUMLOADS))

long use_result_dummy;  /* !static for optimizers. */
void use_pointer(void *result) { use_result_dummy += (long)result; }

lng loads(char *array, lng range, lng stride, lng MHz, FILE *fp, int delay)
{
        register char **p = 0;
        lng     i, j=1, tries;
        lng     time, best = 2000000000;

        fprintf(stderr, "\r%11ld %11ld %11ld", range, stride, range/stride);
        fflush(stderr);
/*
        for (i = stride; i < range; i += stride) {
                p = (char **)&array[i];
                *p = &array[i - stride];
        }
        p = (char **)&array[0];
        *p = &array[i - stride];
*/

        for (i = stride; i < range; i += stride);
        i -= stride;
        for (; i >= 0; i -= stride) {
                char    *next;

                p = (char **)&array[i];
                if (i < stride) {
                        next = &array[range - stride];
                } else {
                        next = &array[i - stride];
                }
                *p = next;
        }

        #define ONE      p = (char **)*p;
        #define TEN      ONE ONE ONE ONE ONE ONE ONE ONE ONE ONE
        #define HUNDRED  TEN TEN TEN TEN TEN TEN TEN TEN TEN TEN
/*
        #define THOUSAND HUNDRED HUNDRED HUNDRED HUNDRED HUNDRED \
                         HUNDRED HUNDRED HUNDRED HUNDRED HUNDRED
*/

        #define FILL     p++; p--;  p++; p--;  p++; p--;  p++; p--;  p++; p--;
        #define ONEx     p = (char **)*p; \
                         FILL FILL FILL FILL FILL FILL FILL FILL FILL FILL
/*
        #define TENx     ONEx ONEx ONEx ONEx ONEx ONEx ONEx ONEx ONEx ONEx
        #define HUNDREDx TENx TENx TENx TENx TENx TENx TENx TENx TENx TENx
*/

        if(delay) for (tries = 0; tries < NUMTRIES; ++tries) {
                i = (j * NUMLOADS) / REDUCE;
                time = now();
                while (i > 0) {
                        ONEx
                        i -= 1;
                }
                time = now() - time;
                use_pointer((void *)p);
                if (time <= MINTIME) {
                        j *= 2;
                        tries--;
                } else {
                        time *= REDUCE;
                        time /= j;
                        if (time < best) {
                                best = time;
                        }
                }
        } else for (tries = 0; tries < NUMTRIES; ++tries) {
                i = (j * NUMLOADS);
                time = now();
                while (i > 0) {
                        HUNDRED
                        i -= 100;
                }
                time = now() - time;
                use_pointer((void *)p);
                if (time <= MINTIME) {
                        j *= 2;
                        tries--;
                } else {
                        time /= j;
                        if (time < best) {
                                best = time;
                        }
                }
        }

        fprintf(stderr, " %11ld %11ld", best*j, best);
        fflush(stderr);

        if (fp) {
                fprintf(fp, " %06ld %05.1f %05.1f"
                        ,best                                           /* elapsed time [microseconds] */
                        ,NSperIt(best)                                  /* nanoseconds per iteration */
                        ,CYperIt(best)                                  /* clocks per iteration */
                );
                fflush(fp);
        }

        return best;
}

lng** runCache(char *array, lng maxrange, lng minstride, lng MHz, FILE *fp, lng *maxstride)
{
        lng     i, r, x, y, z, range = maxrange, stride = minstride / 2;
        dbl     f = 0.25;
        lng     last, time = 0, **result;
        lng     pgsz = getpagesize();
        int     delay;

        if (*maxstride) {
                fprintf(stderr, "analyzing cache latency...\n");
        } else {
                fprintf(stderr, "analyzing cache throughput...\n");
        }
        fprintf(stderr, "      range      stride       spots     brutto-  netto-time\n");
        fflush(stderr);

        if (!(*maxstride)) {
                do {
                        stride *= 2;
                        last = time;
                        time = loads(array, range, stride, MHz, 0, 0);
                        if (!time)
                                ErrXit("runCache: 'loads(%x(array), %ld(range), %ld(stride), %ld(MHz), 0(fp), 0(delay))` returned elapsed time of 0us",
                                        array, range, stride, MHz);
                #ifdef EPSILON1
                } while (((fabs(time - last) / (dbl)time) > EPSILON1) && (stride <= (maxrange / 2)));
                #endif
                #ifdef EPSILON3
                } while ((fabs(CYperIt(time) - CYperIt(last)) > EPSILON3) && (stride <= (maxrange / 2)));
                #endif
                *maxstride = stride;
                delay = 0;
        } else if (*maxstride < 0) {
                *maxstride *= -1;
                delay = 0;
        } else {
                delay = 1;
        }

        for (r = MINRANGE, y = 1; r <= maxrange; r *= 2) {
                for (i = 3; i <= 5; i++) {
                        range = r * f * i;
                        if ((*maxstride <= range) && (range <= maxrange)) {
                                for (stride = *maxstride, x = 1; stride >= minstride; stride /= 2, x++) {
                                }
                                y++;
                        }
                }
        }
        if (!(result = (lng**)malloc(y * sizeof(lng*))))
                ErrXit("runCache: 'result = malloc(%ld)` failed", y * sizeof(lng*));
        for (z = 0; z < y; z++) {
                if (!(result[z] = (lng*)malloc(x * sizeof(lng))))
                        ErrXit("runCache: 'result[%ld] = malloc(%ld)` failed", z, x * sizeof(lng));
                memset(result[z], 0, x * sizeof(lng));
        }
        result[0][0] = (y << 24) | x;

        fprintf(fp, "# Calibrator v%s\n", VERSION);
        fprintf(fp, "# (by Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/)\n");

        for (r = MINRANGE, y = 1; r <= maxrange; r *= 2) {
                for (i = 3; i <= 5; i++) {
                        range = r * f * i;
                        if ((*maxstride <= range) && (range <= maxrange)) {
                                result[y][0] = range;
                                fprintf(fp, "%08.1f %05ld"
                                        ,range / 1024.0 /* range in KB */
                                        ,range / pgsz   /* # pages covered */
                                );
                                fflush(fp);
                                for (stride = *maxstride, x = 1; stride >= minstride; stride /= 2, x++) {
                                        if (!result[0][x]) {
                                                result[0][x] = stride;
                                        }
                                        fprintf(fp, "  %03ld %09ld %08.1f"
                                                ,stride                                         /* stride */
                                                ,range / stride                                 /* # spots accessed  */
                                                ,((dbl)NUMLOADS) / ((dbl)(range / stride))      /* # accesses per spot */
                                        );
                                        fflush(fp);
                                        result[y][x] = loads(array, range, stride, MHz, fp, delay);
                                }
                                fprintf(fp, "\n");
                                fflush(fp);
                                y++;
                        }
                }
        }

        fprintf(stderr, "\n\n");
        fflush(stderr);

        return result;
}

lng** runTLB(char *array, lng maxrange, lng minstride, lng shift, lng mincachelines, lng MHz, FILE *fp, lng *maxstride)
{
        lng     i, x, y, z, stride, minspots, maxspots, p;
        lng     range = maxrange, s = minstride / 2, spots = mincachelines / 2;
        dbl     f = 0.25;
        lng     tmax, smin, xmin, last, time = 0, **result;
        lng     pgsz = getpagesize();
        int     delay;

        fprintf(stderr, "analyzing TLB latency...\n");
        fprintf(stderr, "      range      stride       spots     brutto-  netto-time\n");
        fflush(stderr);

        if (!(*maxstride)) {
                do {
                        s *= 2;
                        stride = s + shift;
                        range = stride * spots;
                        last = time;
                        time = loads(array, range, stride, MHz, 0, 0);
                        if (!time)
                                ErrXit("runTLB: 'loads(%x(array), %ld(range), %ld(stride), %ld(MHz), 0(fp), 0(delay))` returned elapsed time of 0us",
                                        array, range, stride, MHz);
                #ifdef EPSILON1
                } while ((((fabs(time - last) / (dbl)time) > EPSILON1) || (stride < (pgsz / 1))) && (range <= (maxrange / 2))); 
        /*      } while (((fabs(time - last) / (dbl)time) > EPSILON1) && (range <= (maxrange / 2)));    */
                #endif
                #ifdef EPSILON3
                } while ((fabs(CYperIt(time) - CYperIt(last)) > EPSILON3) || (stride < (pgsz / 1))) && (range <= (maxrange / 2)));      
        /*      } while (fabs(CYperIt(time) - CYperIt(last)) > EPSILON3) && (range <= (maxrange / 2))); */
                #endif
                *maxstride = s;
                delay = 0;
        } else {
                delay = 1;
        }
        minspots = MAX(MINRANGE / (minstride + shift), 4);
        maxspots = maxrange / (*maxstride + shift);     
/*      maxspots = mincachelines;       */

        for (p = minspots, y = 2; p <= maxspots; p *= 2) {
                for (i = 3; i <= 5; i++) {
                        spots = p * f * i;
                        if ((spots * (*maxstride + shift)) <= maxrange) {
                                for (s = *maxstride, x = 2; s >= minstride; s /= 2, x++) {
                                }
                                y++;
                        }
                }
        }
        if (!(result = (lng**)malloc(y * sizeof(lng*))))
                ErrXit("runTLB: 'result = malloc(%ld)` failed", y * sizeof(lng*));
        for (z = 0; z < y; z++) {
                if (!(result[z] = (lng*)malloc(x * sizeof(lng))))
                        ErrXit("runTLB: 'result[%ld] = malloc(%ld)` failed", z, x * sizeof(lng*));
                memset(result[z], 0, x * sizeof(lng));
        }
        result[0][0] = (y << 24) | x;

        fprintf(fp, "# Calibrator v%s\n", VERSION);
        fprintf(fp, "# (by Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/)\n");

        for (p = minspots, y = 2; p <= maxspots; p *= 2) {
                for (i = 3; i <= 5; i++) {
                        spots = p * f * i;
                        if ((spots * (*maxstride + shift)) <= maxrange) {
                                result[y][0] = spots;
                                fprintf(fp, "%09ld %08.1f"
                                        ,spots                                  /* # spots accessed  */
                                        ,((float)NUMLOADS) / ((float)spots)     /* # accesses per spot */
                                );
                                fflush(fp);
                                tmax = 0;
                                smin = *maxstride + shift;
                                xmin = 2;
                                for (s = *maxstride, x = 2; s >= minstride; s /= 2, x++) {
                                        stride = s + shift;
                                        if (!result[0][x]) {
                                                result[0][x] = stride;
                                        }
                                        range = stride * spots;
                                        fprintf(fp, "  %06ld %08.1f %05ld"
                                                ,stride         /* stride */
                                                ,range / 1024.0 /* range in KB */
                                                ,range / pgsz   /* # pages covered */
                                        );
                                        fflush(fp);
                                        result[y][x] = loads(array, range, stride, MHz, fp, delay);
                                        if (result[y][x] > tmax) {
                                                tmax = result[y][x];
                                                if (stride < smin) {
                                                        smin = stride;
                                                        xmin = x;
                                                }
                                        }
                                }
                                result[y][1] = tmax;
                                result[1][xmin]++;
                                fprintf(fp, "\n");
                                fflush(fp);
                                y++;
                        }
                }
        }
        xmin = --x;
        for (--x; x >= 2; x--) {
                if (result[1][x] > result[1][xmin]) {
                        xmin = x;
                }
        }
        result[0][1] = result[0][xmin];

        fprintf(stderr, "\n\n");
        fflush(stderr);
                
        return result;
}

typedef struct {
        lng     levels;
        lng     size[MAXLEVELS];
        lng     linesize[MAXLEVELS];
        lng     latency1[MAXLEVELS];
        lng     latency2[MAXLEVELS];
} cacheInfo;

cacheInfo* analyzeCache(lng **result1, lng **result2, lng MHz)
{
        lng     x, y, xx, yy, range, lastrange, stride, level, a, l, n;
        lng     last[LENPLATEAU], time1, time2, lasttime1, lasttime2;
        lng     diff;
        cacheInfo *draft, *cache;

        if (!(draft = (cacheInfo*)malloc(4 * sizeof(cacheInfo))))
                ErrXit("analyzeCache: 'draft = malloc(%ld)` failed", 4 * sizeof(cacheInfo));
        if (!(cache = (cacheInfo*)malloc(sizeof(cacheInfo))))
                ErrXit("analyzeCache: 'cache = malloc(%ld)` failed", sizeof(cacheInfo));
        memset(draft, 0, 4 * sizeof(cacheInfo));
        memset(cache, 0, sizeof(cacheInfo));
        
        xx = (result1[0][0] & 0xffffff) - 1;
        yy = (result1[0][0] >> 24) - 1;
        level = 0;
        memset(last, 0, LENPLATEAU * sizeof(last[0]));
        a = LENPLATEAU;
        lastrange = 0;
        lasttime1 = 0;
        lasttime2 = 0;
        for (y = 1; y <= yy ; y++) {
                range = result1[y][0];
                for (x = 1; x <= xx; x++) {
                        stride = result1[0][x];
                        time1 = result1[y][x];
                        time2 = result2[y][x];
                        if (draft[1].linesize[level] && last[a] && (range == draft[1].size[level])) {
                                #ifdef EPSILON1
                                if ((fabs(time1 - last[a]) / (dbl)time1) < EPSILON1) {
                                #endif
                                #ifdef EPSILON3
                                if (fabs(CYperIt(time1) - CYperIt(last[a])) < EPSILON3) {
                                #endif
                                        draft[0].linesize[level] = stride;
                                        draft[1].linesize[level] = stride;
                                }
                        }
                        if (draft[2].linesize[level] && last[0] && lastrange && (lastrange == draft[2].size[level])) {
                                #ifdef EPSILON1
                                if ((fabs(time1 - last[0]) / (dbl)time1) < EPSILON1) {
                                #endif
                                #ifdef EPSILON3
                                if (fabs(CYperIt(time1) - CYperIt(last[0])) < EPSILON3) {
                                #endif
                                        draft[2].linesize[level] = stride;
                                        draft[3].linesize[level] = stride;
                                        if (x == xx) {
                                                level++;
                                                memset(last, 0, LENPLATEAU * sizeof(last[0]));
                                                a = LENPLATEAU;
                                        }
                                } else {
                                        level++;
                                        memset(last, 0, LENPLATEAU * sizeof(last[0]));
                                        a = LENPLATEAU;
                                }
                        }
                        #ifdef EPSILON2
                        if ((x == 1) && (!draft[2].linesize[level]) && ((last[0] && ((FABS(time1 - last[LENPLATEAU - 1]) / (dbl)last[LENPLATEAU - 1]) > EPSILON2)) || (y == yy))) {
                        #endif
                        #ifdef EPSILON4
                        if ((x == 1) && (!draft[2].linesize[level]) && ((last[0] && (FABS(CYperIt(time1) - CYperIt(last[LENPLATEAU - 1])) >= EPSILON4)) || (y == yy))) {
                        #endif
                                draft[2].linesize[level] = draft[1].linesize[level];
                                draft[2].size[level] = lastrange;
                                draft[2].latency1[level] = lasttime1;
                                draft[2].latency2[level] = lasttime2;
                                draft[3].linesize[level] = stride;
                                draft[3].size[level] = range;
                                draft[3].latency1[level] = time1;
                                draft[3].latency2[level] = time2;
                                last[0] = time1;
                        }
                        if ((x == 1) && (a < LENPLATEAU) && (!last[0])) {
                                #ifdef EPSILON2
                                if ((FABS(time1 - last[LENPLATEAU - 1]) / (dbl)last[LENPLATEAU - 1]) < EPSILON2) {
                                #endif
                                #ifdef EPSILON4
                                if (FABS(CYperIt(time1) - CYperIt(last[LENPLATEAU - 1])) <= EPSILON4) {
                                #endif
                                        last[--a] = time1;
                                } else {
                                        memset(last, 0, LENPLATEAU * sizeof(last[0]));
                                        a = LENPLATEAU;
                                }
                        }
                        if ((x == 1) && (a == LENPLATEAU)) {
                                last[--a] = time1;
                                draft[0].linesize[level] = stride;
                                draft[0].size[level] = lastrange;
                                draft[0].latency1[level] = lasttime1;
                                draft[0].latency2[level] = lasttime2;
                                draft[1].linesize[level] = stride;
                                draft[1].size[level] = range;
                                draft[1].latency1[level] = time1;
                                draft[1].latency2[level] = time2;
                        }
                        if (x == 1) {
                                lasttime1 = time1;
                                lasttime2 = time2;
                        }
                }
                lastrange = range;
        }

        #ifdef DEBUG
        {
        lng ll;
        for (l = 0; l < level; l++) {
                for (ll = 0; ll < 4; ll++) {
                        fprintf(stderr, "%2ld %5ld %3ld  %05.1f %05.1f\n",
                                l, draft[ll].size[l] / 1024, draft[ll].linesize[l], NSperIt(draft[ll].latency1[l]), CYperIt(draft[ll].latency1[l]));
                }
                fprintf(stderr, "\n");
        }
        fflush(stderr);
        }
        #endif

        for (l = n = 0 ; n < level; n++) {
                cache->latency1[l] = ((dbl)(draft[2].latency1[n] + draft[1].latency1[n]) / 2.0);
                cache->latency2[l] = ((dbl)(draft[2].latency2[n] + draft[1].latency2[n]) / 2.0);
                if ((l == 0) || ((log10(cache->latency1[l]) - log10(cache->latency1[l - 1])) > 0.3)) {
                        cache->linesize[l] = draft[1].linesize[n];
                        diff = -1;
                        for (range = 1; range < result1[1][0]; range *= 2);
                        for (y = 1; result1[y][0] < range; y++);
                        if (l) {
                                int yyy = 1;
                                for (; y <= yy; y += yyy) {
                                        range = result1[y][0];
                                        if ((draft[2].size[n - 1] <= range) && (range < draft[1].size[n])) {
                                                if ((y > yyy) && (((result1[y][1]) - (result1[y - yyy][1])) > diff)) {
                                                        diff = (result1[y][1]) - (result1[y - yyy][1]);
                                                        cache->size[l - 1] = range;
                                                }
                                                if (((y + yyy) <= yy) && (((result1[y + yyy][1]) - (result1[y][1])) > diff)) {
                                                        diff = (result1[y + yyy][1]) - (result1[y][1]);
                                                        cache->size[l - 1] = range;
                                                }
                                        }
                                }
                        }
                        l++;
                }
        }
        cache->size[--l] = draft[3].size[--n];
        cache->levels = l;
        
        #ifdef DEBUG
        for (l = 0; l <= cache->levels; l++) {
                fprintf(stderr, "%2ld %5ld %3ld  %05.1f %05.1f\n",
                        l, cache->size[l] / 1024, cache->linesize[l], NSperIt(cache->latency1[l]), CYperIt(cache->latency1[l]));
        }
        fprintf(stderr, "\n");
        fflush(stderr);
        #endif

        free(draft);
        draft = 0;

        return cache;
}

typedef struct {
        lng     levels;
        lng     shift;
        lng     mincachelines;
        lng     entries[MAXLEVELS];
        lng     pagesize[MAXLEVELS];
        lng     latency1[MAXLEVELS];
        lng     latency2[MAXLEVELS];
} TLBinfo;

TLBinfo* analyzeTLB(lng **result1, lng **result2, lng shift, lng mincachelines, lng MHz)
{
        lng     x, y, xx, yy, spots, lastspots, stride, level, a, l, limit = 0, n;
        lng     last[LENPLATEAU], time1, time2, lasttime1, lasttime2;
        dbl     diff;
        TLBinfo *draft, *TLB;

        if (!(draft = (TLBinfo*)malloc(4 * sizeof(TLBinfo))))
                ErrXit("analyzeCache: 'draft = malloc(%ld)` failed", 4 * sizeof(TLBinfo));
        if (!(TLB = (TLBinfo*)malloc(sizeof(TLBinfo))))
                ErrXit("analyzeCache: 'TLB = malloc(%ld)` failed", sizeof(TLBinfo));
        memset(draft, 0, 4 * sizeof(TLBinfo));
        memset(TLB, 0, sizeof(TLBinfo));
        TLB->shift = shift;
        TLB->mincachelines = mincachelines;
        
        xx = (result1[0][0] & 0xffffff) - 1;
        yy = (result1[0][0] >> 24) - 1;
        level = 0;
        memset(last, 0, LENPLATEAU * sizeof(last[0]));
        a = LENPLATEAU;
        lastspots = 0;
        lasttime1 = 0;
        lasttime2 = 0;
        for (y = 2; !limit; y++) {
                spots = result1[y][0];
                limit = (y >= yy) || (spots >= (TLB->mincachelines * 1.25));
                for (x = 1; x <= xx; x++) {
                        stride = result1[0][x];
                        time1 = result1[y][x];
                        time2 = result2[y][x];
                        if (draft[1].pagesize[level] && last[a] && (spots == draft[1].entries[level])) {
                                #ifdef EPSILON1
                                if (((fabs(time1 - last[a]) / (dbl)time1) < EPSILON1) || (stride >= result1[0][1])) {
                                #endif
                                #ifdef EPSILON3
                                if ((fabs(CYperIt(time1) - CYperIt(last[a])) < EPSILON3) || (stride >= result1[0][1])) {
                                #endif
                                        draft[0].pagesize[level] = stride;
                                        draft[1].pagesize[level] = stride;
                                }
                        }
                        if (draft[2].pagesize[level] && last[0] && lastspots && (lastspots == draft[2].entries[level])) {
                                #ifdef EPSILON1
                                if (((fabs(time1 - last[0]) / (dbl)time1) < EPSILON1) || (stride >= result1[0][1])) {
                                #endif
                                #ifdef EPSILON3
                                if ((fabs(CYperIt(time1) - CYperIt(last[0])) < EPSILON3) || (stride >= result1[0][1])) {
                                #endif
                                        draft[2].pagesize[level] = stride;
                                        draft[3].pagesize[level] = stride;
                                        if (x == xx) {
                                                level++;
                                                memset(last, 0, LENPLATEAU * sizeof(last[0]));
                                                a = LENPLATEAU;
                                        }
                                } else {
                                        level++;
                                        memset(last, 0, LENPLATEAU * sizeof(last[0]));
                                        a = LENPLATEAU;
                                }
                        }
                        #ifdef EPSILON2
                        if ((x == 1) && (!draft[2].pagesize[level]) && ((last[0] && ((FABS(time1 - last[LENPLATEAU - 1]) / (dbl)last[LENPLATEAU - 1]) > EPSILON2)) || limit)) {
                        #endif
                        #ifdef EPSILON4
                        if ((x == 1) && (!draft[2].pagesize[level]) && ((last[0] && (FABS(CYperIt(time1) - CYperIt(last[LENPLATEAU - 1])) >= EPSILON4)) || limit)) {
                        #endif
                                draft[2].pagesize[level] = draft[1].pagesize[level];
                                draft[2].entries[level] = lastspots;
                                draft[2].latency1[level] = lasttime1;
                                draft[2].latency2[level] = lasttime2;
                                draft[3].pagesize[level] = stride;
                                draft[3].entries[level] = spots;
                                draft[3].latency1[level] = time1;
                                draft[3].latency2[level] = time2;
                                last[0] = time1;
                        }
                        if ((x == 1) && (a < LENPLATEAU) && (!last[0])) {
                                #ifdef EPSILON2
                                if ((FABS(time1 - last[LENPLATEAU - 1]) / (dbl)last[LENPLATEAU - 1]) < EPSILON2) {
                                #endif
                                #ifdef EPSILON4
                                if (FABS(CYperIt(time1) - CYperIt(last[LENPLATEAU - 1])) <= EPSILON4) {
                                #endif
                                        last[--a] = time1;
                                } else {
                                        memset(last, 0, LENPLATEAU * sizeof(last[0]));
                                        a = LENPLATEAU;
                                }
                        }
                        if ((x == 1) && (a == LENPLATEAU)) {
                                last[--a] = time1;
                                draft[0].pagesize[level] = stride;
                                draft[0].entries[level] = lastspots;
                                draft[0].latency1[level] = lasttime1;
                                draft[0].latency2[level] = lasttime2;
                                draft[1].pagesize[level] = stride;
                                draft[1].entries[level] = spots;
                                draft[1].latency1[level] = time1;
                                draft[1].latency2[level] = time2;
                        }
                        if (x == 1) {
                                lasttime1 = time1;
                                lasttime2 = time2;
                        }
                }
                lastspots = spots;
        }

        #ifdef DEBUG
        {
        lng ll;
        for (l = 0; l < level; l++) {
                for (ll = 0; ll < 4; ll++) {
                        fprintf(stderr, "%2ld %5ld %5ld  %05.1f %05.1f\n",
                                l, draft[ll].entries[l], draft[ll].pagesize[l], NSperIt(draft[ll].latency1[l]), CYperIt(draft[ll].latency1[l]));
                }
                fprintf(stderr, "\n");
        }
        fflush(stderr);
        }
        #endif

        for (l = n = 0; n < level; n++) {
                TLB->latency1[l] = ((dbl)(draft[2].latency1[n] + draft[1].latency1[n]) / 2.0);
                TLB->latency2[l] = ((dbl)(draft[2].latency2[n] + draft[1].latency2[n]) / 2.0);
                if ((l == 0) || (((log10(TLB->latency1[l]) - log10(TLB->latency1[l - 1])) > 0.3) && (draft[2].entries[l] > draft[1].entries[l]))) {
                        TLB->pagesize[l] = draft[1].pagesize[n];
                        diff = -1.0;
                        for (spots = 1; spots < result1[2][0]; spots *= 2);
                        for (y = 2; result1[y][0] < spots; y++);
                        if (l) {
                                int yyy = 1;
                                for (; y <= yy; y += yyy) {
                                        spots = result1[y][0];
                                        if ((draft[2].entries[n - 1] <= spots) && (spots < draft[1].entries[n])) {
                                                if ((y > 4) && ((log(result1[y][1]) - log(result1[y - yyy][1])) > diff)) {
                                                        diff = log(result1[y][1]) - log(result1[y - yyy][1]);
                                                        TLB->entries[l - 1] = spots;
                                                }
                                                if (((y + yyy) <= yy) && ((log(result1[y + yyy][1]) - log(result1[y][1])) > diff)) {
                                                        diff = log(result1[y + yyy][1]) - log(result1[y][1]);
                                                        TLB->entries[l - 1] = spots;
                                                }
                                        }
                                }
                        }
                        l++;
                }
        }
        TLB->entries[--l] = draft[3].entries[--n];
        TLB->levels = l;
        
        #ifdef DEBUG
        for (l = 0; l <= TLB->levels; l++) {
                fprintf(stderr, "%2ld %5ld %5ld  %05.1f %05.1f\n",
                        l, TLB->entries[l], TLB->pagesize[l], NSperIt(TLB->latency1[l]), CYperIt(TLB->latency1[l]));
        }
        fprintf(stderr, "\n");
        fflush(stderr);
        #endif

        free(draft);
        draft = 0;

        return TLB;
}

void plotCache(cacheInfo *cache, lng **result, lng MHz, char *fn, FILE *fp, lng delay)
{
        lng     l, x, xx = (result[0][0] & 0xffffff) - 1, y, yy = (result[0][0] >> 24) - 1;
        dbl     xl, xh, yl, yh, z;
        char    *s;
        
        xl = (dbl)result[1][0] / 1024.0;
        xh = (dbl)result[yy][0] / 1024.0;
        yl = 1.0;
        for (yh = 1000; yh < (lng)NSperIt(result[yy][1] - delay); yh *= 10);
        fprintf(fp, "# Calibrator v%s\n", VERSION);
        fprintf(fp, "# (by Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/)\n");
        fprintf(fp, " set term postscript portrait enhanced\n");
        fprintf(fp, " set output '%s.ps'\n", fn);
        fprintf(fp, "#set term gif transparent interlace small size 500, 707 # xFFFFFF x333333 x333333 x0055FF x005522 x660000 xFF0000 x00FF00 x0000FF\n");
        fprintf(fp, "#set output '%s.gif'\n", fn);
        fprintf(fp, "set data style linespoints\n");
        fprintf(fp, "set key below\n");
        fprintf(fp, "set title '%s'\n", fn);
        fprintf(fp, "set xlabel 'memory range [bytes]'\n");
        fprintf(fp, "set x2label ''\n");
        fprintf(fp, "set ylabel 'nanosecs per iteration'\n");
        fprintf(fp, "set y2label 'cycles per iteration'\n");
        fprintf(fp, "set logscale x 2\n");
        fprintf(fp, "set logscale x2 2\n");
        fprintf(fp, "set logscale y 10\n");
        fprintf(fp, "set logscale y2 10\n");
        fprintf(fp, "set format x '%%1.0f'\n");
        fprintf(fp, "set format x2 '%%1.0f'\n");
        fprintf(fp, "set format y '%%1.0f'\n");
        fprintf(fp, "set format y2 ''\n");
        fprintf(fp, "set xrange[%f:%f]\n", xl, xh);
        fprintf(fp, "#set x2range[%f:%f]\n", xl, xh);
        fprintf(fp, "set yrange[%f:%f]\n", yl, yh);
        fprintf(fp, "#set y2range[%f:%f]\n", yl, yh);
        fprintf(fp, "set grid x2tics\n");
        fprintf(fp, "set xtics mirror");
        for (x = 1, l = 1, s = " ("; x <= xh; x *= 2, l++, s = ", ") {
                if (l&1) {
                        if (x >= (1024 * 1024)) {
                                fprintf(fp, "%s'%ldG' %ld", s, x / (1024 * 1024), x);
                        } else if (x >= 1024) {
                                fprintf(fp, "%s'%ldM' %ld", s, x / 1024, x);
                        } else {
                                fprintf(fp, "%s'%ldk' %ld", s, x, x);
                        }
                } else {
                        fprintf(fp, "%s'' %ld", s, x);
                }
        }
        fprintf(fp, ")\n");
        fprintf(fp, "set x2tics mirror");
        for (l = 0, s = " ("; l < cache->levels; l++, s = ", ") {
                if (cache->size[l] >= (1024 * 1024 * 1024)) {
                        fprintf(fp, "%s'[%ldG]' %ld", s, cache->size[l] / (1024 * 1024 * 1024), cache->size[l] / 1024);
                } else if (cache->size[l] >= (1024 * 1024)) {
                        fprintf(fp, "%s'[%ldM]' %ld", s, cache->size[l] / (1024 * 1024), cache->size[l] / 1024);
                } else {
                        fprintf(fp, "%s'[%ldk]' %ld", s, cache->size[l] / 1024, cache->size[l] / 1024);
                }
        }
        fprintf(fp, ")\n");
        fprintf(fp, "set y2tics");
        for (l = 0, s = " ("; l <= cache->levels; l++, s = ", ") {
                if (!delay)     fprintf(fp, "%s'(%ld)' %f", s, round(CYperIt(cache->latency1[l] - delay)), NSperIt(cache->latency1[l] - delay));
                        else    fprintf(fp, "%s'(%ld)' %f", s, round(CYperIt(cache->latency2[l] - delay)), NSperIt(cache->latency2[l] - delay));
        }
        for (y = 1; y <= yh; y *= 10) {
                fprintf(fp, "%s'%1.3g' %ld", s, (dbl)(y * MHz) / 1000.0, y);
        }
        fprintf(fp, ")\n");
        for (l = 0; l <= cache->levels; l++) {
                if (!delay)     z = (dbl)round(CYperIt(cache->latency1[l] - delay)) * 1000.0 / (dbl)MHz;
                        else    z = (dbl)round(CYperIt(cache->latency2[l] - delay)) * 1000.0 / (dbl)MHz;
                fprintf(fp, "set label %ld '(%1.3g)  ' at %f,%f right\n", l + 1, z, xl, z);
                fprintf(fp, "set arrow %ld from %f,%f to %f,%f nohead lt 0\n", l + 1, xl, z, xh, z);
        }
        fprintf(fp, " set label %ld '^{ Calibrator v%s (Stefan.Manegold\\@cwi.nl, www.cwi.nl/~manegold) }' at graph 0.5,graph 0.02 center\n", l + 1, VERSION);
        fprintf(fp, "#set label %ld    'Calibrator v%s (Stefan.Manegold@cwi.nl, www.cwi.nl/~manegold)'    at graph 0.5,graph 0.03 center\n",  l + 1, VERSION);
        fprintf(fp, "plot \\\n0.1 title 'stride:' with points pt 0 ps 0");
        for (x = 1, l = cache->levels; x <= xx; x++) {
                fprintf(fp, " , \\\n'%s.data' using 1:($%ld-%f) title '", fn, (6 * x) + 1, NSperIt(delay));
                if ((l > 0) && (result[0][x] == cache->linesize[l])) {
                        fprintf(fp, "\\{%ld\\}", result[0][x]);
                        while ((--l >= 0) && (result[0][x] == cache->linesize[l]));
                } else {
                        fprintf(fp, "%ld", result[0][x]);
                }
                fprintf(fp, "' with linespoints lt %ld pt %ld", x, x + 2);
        }
        fprintf(fp, "\n");
        fprintf(fp, "set nolabel\n");
        fprintf(fp, "set noarrow\n");
        fflush(fp);
}

void plotTLB(TLBinfo *TLB, lng **result, lng MHz, char *fn, FILE *fp, lng delay)
{
        lng     l, x, xx = (result[0][0] & 0xffffff) - 1, y, yy = (result[0][0] >> 24) - 1;
        dbl     xl, xh, yl, yh, z;
        char    *s;
        
        xl = (dbl)result[2][0];
        xh = (dbl)result[yy][0];
        yl = 1.0;
        for (yh = 1000; yh < (lng)NSperIt(result[yy][2] - delay); yh *= 10);
        fprintf(fp, "# Calibrator v%s\n", VERSION);
        fprintf(fp, "# (by Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/)\n");
        fprintf(fp, " set term postscript portrait enhanced\n");
        fprintf(fp, " set output '%s.ps'\n", fn);
        fprintf(fp, "#set term gif transparent interlace small size 500, 707 # xFFFFFF x333333 x333333 x0055FF x005522 x660000 xFF0000 x00FF00 x0000FF\n");
        fprintf(fp, "#set output '%s.gif'\n", fn);
        fprintf(fp, "set data style linespoints\n");
        fprintf(fp, "set key below\n");
        fprintf(fp, "set title '%s'\n", fn);
        fprintf(fp, "set xlabel 'spots accessed'\n");
        fprintf(fp, "set x2label ''\n");
        fprintf(fp, "set ylabel 'nanosecs per iteration'\n");
        fprintf(fp, "set y2label 'cycles per iteration'\n");
        fprintf(fp, "set logscale x 2\n");
        fprintf(fp, "set logscale x2 2\n");
        fprintf(fp, "set logscale y 10\n");
        fprintf(fp, "set logscale y2 10\n");
        fprintf(fp, "set format x '%%1.0f'\n");
        fprintf(fp, "set format x2 '%%1.0f'\n");
        fprintf(fp, "set format y '%%1.0f'\n");
        fprintf(fp, "set format y2 ''\n");
        fprintf(fp, "set xrange[%f:%f]\n", xl, xh);
        fprintf(fp, "#set x2range[%f:%f]\n", xl, xh);
        fprintf(fp, "set yrange[%f:%f]\n", yl, yh);
        fprintf(fp, "#set y2range[%f:%f]\n", yl, yh);
        fprintf(fp, "set grid x2tics\n");
        fprintf(fp, "set xtics mirror");
        for (x = 1, l = 1, s = " ("; x <= xh; x *= 2, l++, s = ", ") {
                if (l | 1) {
                        if (x >= (1024 * 1024)) {
                                fprintf(fp, "%s'%ldM' %ld", s, x / (1024 * 1024), x);
                        } else if (x >= 1024) {
                                fprintf(fp, "%s'%ldk' %ld", s, x / 1024, x);
                        } else {
                                fprintf(fp, "%s'%ld' %ld", s, x, x);
                        }
                } else {
                        fprintf(fp, "%s'' %ld", s, x);
                }
        }
        fprintf(fp, ")\n");
        fprintf(fp, "set x2tics mirror");
        for (l = 0, s = " ("; l < TLB->levels; l++, s = ", ") {
                if (TLB->entries[l] >= (1024 * 1024)) {
                        fprintf(fp, "%s'[%ldM]' %ld", s, TLB->entries[l] / (1024 * 1024), TLB->entries[l]);
                } else if (TLB->entries[l] >= 1024) {
                        fprintf(fp, "%s'[%ldk]' %ld", s, TLB->entries[l] / 1024, TLB->entries[l]);
                } else {
                        fprintf(fp, "%s'[%ld]' %ld", s, TLB->entries[l], TLB->entries[l]);
                }
        }
        fprintf(fp, "%s'<L1>' %ld)\n", s, TLB->mincachelines);
        fprintf(fp, "set y2tics");
        for (l = 0, s = " ("; l <= TLB->levels; l++, s = ", ") {
                if (!delay)     fprintf(fp, "%s'(%ld)' %f", s, round(CYperIt(TLB->latency1[l] - delay)), NSperIt(TLB->latency1[l] - delay));
                        else    fprintf(fp, "%s'(%ld)' %f", s, round(CYperIt(TLB->latency2[l] - delay)), NSperIt(TLB->latency2[l] - delay));
        }
        for (y = 1; y <= yh; y *= 10) {
                fprintf(fp, "%s'%1.3g' %ld", s, (dbl)(y * MHz) / 1000.0, y);
        }
        fprintf(fp, ")\n");
        for (l = 0; l <= TLB->levels; l++) {
                if (!delay)     z = (dbl)round(CYperIt(TLB->latency1[l] - delay)) * 1000.0 / (dbl)MHz;
                        else    z = (dbl)round(CYperIt(TLB->latency2[l] - delay)) * 1000.0 / (dbl)MHz;
                fprintf(fp, "set label %ld '(%1.3g)  ' at %f,%f right\n", l + 1, z, xl, z);
                fprintf(fp, "set arrow %ld from %f,%f to %f,%f nohead lt 0\n", l + 1, xl, z, xh, z);
        }
        fprintf(fp, " set label %ld '^{ Calibrator v%s (Stefan.Manegold\\@cwi.nl, www.cwi.nl/~manegold) }' at graph 0.5,graph 0.02 center\n", l + 1, VERSION);
        fprintf(fp, "#set label %ld    'Calibrator v%s (Stefan.Manegold@cwi.nl, www.cwi.nl/~manegold)'    at graph 0.5,graph 0.03 center\n",  l + 1, VERSION);
        fprintf(fp, "plot \\\n0.1 title 'stride:' with points pt 0 ps 0");
        for (x = 2, l = TLB->levels; x <= xx; x++) {
                fprintf(fp, " , \\\n'%s.data' using 1:($%ld-%f) title '", fn, (6 * (x - 1)) + 1, NSperIt(delay));
                if ((l > 0) && (result[0][x] == TLB->pagesize[l])) {
                        fprintf(fp, "\\{%ld\\}", result[0][x]);
                        while ((--l >= 0) && (result[0][x] == TLB->pagesize[l]));
                } else {
                        fprintf(fp, "%ld", result[0][x]);
                }
                fprintf(fp, "' with linespoints lt %ld pt %ld", x, x + 2);
        }
        fprintf(fp, "\n");
        fprintf(fp, "set nolabel\n");
        fprintf(fp, "set noarrow\n");
        fflush(fp);
}

void printCPU(cacheInfo *cache, lng MHz, lng delay)
{
        FILE    *fp = stdout;
                
        fprintf(fp, "CPU loop + L1 access:    ");
        fprintf(fp, " %6.2f ns = %3ld cy\n", NSperIt(cache->latency1[0]), round(CYperIt(cache->latency1[0])));
        fprintf(fp, "             ( delay:    ");
        fprintf(fp, " %6.2f ns = %3ld cy )\n", NSperIt(delay),            round(CYperIt(delay)));
        fprintf(fp, "\n");
        fflush(fp);
}

void printCache(cacheInfo *cache, lng MHz)
{
        lng     l;
        FILE    *fp = stdout;
                
        fprintf(fp, "caches:\n");
        fprintf(fp, "level  size    linesize   miss-latency        replace-time\n");
        for (l = 0; l < cache->levels; l++) {
                fprintf(fp, "  %1ld   ", l+1);
                if (cache->size[l] >= (1024 * 1024 * 1024)) {
                        fprintf(fp, " %3ld GB ", cache->size[l] / (1024 * 1024 * 1024));
                } else if (cache->size[l] >= (1024 * 1024)) {
                        fprintf(fp, " %3ld MB ", cache->size[l] / (1024 * 1024));
                } else {
                        fprintf(fp, " %3ld KB ", cache->size[l] / 1024);
                }
                fprintf(fp, " %3ld bytes ", cache->linesize[l + 1]);
                fprintf(fp, " %6.2f ns = %3ld cy " , NSperIt(cache->latency2[l + 1] - cache->latency2[l]), round(CYperIt(cache->latency2[l + 1] - cache->latency2[l])));
                fprintf(fp, " %6.2f ns = %3ld cy\n", NSperIt(cache->latency1[l + 1] - cache->latency1[l]), round(CYperIt(cache->latency1[l + 1] - cache->latency1[l])));
        }
        fprintf(fp, "\n");
        fflush(fp);
}

void printTLB(TLBinfo *TLB, lng MHz)
{
        lng     l;
        FILE    *fp = stdout;
                
        fprintf(fp, "TLBs:\n");
        fprintf(fp, "level #entries  pagesize  miss-latency");
/*
        fprintf(fp, "        replace-time");
*/
        fprintf(fp, "\n");
        for (l = 0; l < TLB->levels; l++) {
                fprintf(fp, "  %1ld   ", l+1);
                fprintf(fp, "   %3ld   ", TLB->entries[l]);
                if (TLB->pagesize[l + 1] >= (1024 * 1024 * 1024)) {
                        fprintf(fp, "  %3ld GB  ", TLB->pagesize[l + 1] / (1024 * 1024 * 1024));
                } else if (TLB->pagesize[l + 1] >= (1024 * 1024)) {
                        fprintf(fp, "  %3ld MB  ", TLB->pagesize[l + 1] / (1024 * 1024));
                } else {
                        fprintf(fp, "  %3ld KB  ", TLB->pagesize[l + 1] / 1024);
                }
                fprintf(fp, " %6.2f ns = %3ld cy ", NSperIt(TLB->latency2[l + 1] - TLB->latency2[l]), round(CYperIt(TLB->latency2[l + 1] - TLB->latency2[l])));
/*
                fprintf(fp, " %6.2f ns = %3ld cy" , NSperIt(TLB->latency1[l + 1] - TLB->latency1[l]), round(CYperIt(TLB->latency1[l + 1] - TLB->latency1[l])));
*/
                fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
        fflush(fp);
}

lng getMINTIME () {
        lng t0=0, t1=0;
        t0=t1=now();
        while(t0==t1){
                t1=now();
        }
        return (t1-t0);
}

int main(int ac, char **av)
{
        lng     MHz, maxrange, mincachelines, maxlinesize, minstride = (lng)sizeof(char*), yy, y;
        lng     maxCstride=0, maxTstride=0, delayC, delayT;
        char    *array0, *array, fnn1[1024], fnx1[1024], fnn2[1024], fnx2[1024];
        FILE    *fp;
        lng     **result1, **result2;
        cacheInfo *cache;
        TLBinfo *TLB;
        lng     align = 0, pgsz  = getpagesize();

        fprintf(stdout,"\nCalibrator v%s\n(by Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/)\n", VERSION);

        if (ac < 4) ErrXit("usage: '%s <MHz> <size>[k|M|G] <filename>`", av[0]);

        MHz      = atoi(av[1]);
        maxrange = bytes(av[2]) * 1.25;

        if (ac > 4) align = atoi(av[4]) % pgsz;
        if (ac > 5) maxCstride = -1 * abs(atoi(av[5]));

        if (!(array0 = (char *)malloc(maxrange+pgsz)))
                ErrXit("main: 'array0 = malloc(%ld)` failed", maxrange+pgsz);

        array = array0;
        fprintf(stderr,"%x %ld %ld %5ld\n",array,(lng)array,pgsz,(lng)array%pgsz);
        while (((lng)array % pgsz) != align) {
                fprintf(stderr,"\r%x %ld %ld %5ld",array,(lng)array,pgsz,(lng)array%pgsz);
                fflush(stderr);
                array++;
        }
        fprintf(stderr,"\n%x %ld %ld %5ld\n\n",array,(lng)array,pgsz,(lng)array%pgsz);
        fflush(stderr);
        
        MINTIME = MAX( MINTIME, 10*getMINTIME() );
        fprintf(stderr,"MINTIME = %ld\n\n",MINTIME);
        fflush(stderr);

        sprintf(fnn1, "%s.cache-replace-time", av[3]);
        sprintf(fnx1, "%s.data", fnn1);
        if (!(fp = fopen(fnx1,"w"))) ErrXit("main: 'fp = fopen(%s,\"w\")` failed", fnx1);
        result1 = runCache(array, maxrange, minstride, MHz, fp, &maxCstride);
        fclose(fp);

        sprintf(fnn2, "%s.cache-miss-latency", av[3]);
        sprintf(fnx2, "%s.data", fnn2);
        if (!(fp = fopen(fnx2,"w"))) ErrXit("main: 'fp = fopen(%s,\"w\")` failed", fnx2);
        result2 = runCache(array, maxrange, minstride, MHz, fp, &maxCstride);
        fclose(fp);

        cache = analyzeCache(result1, result2, MHz);
        mincachelines = ( cache->size[0] && cache->linesize[1] ? cache->size[0] / cache->linesize[1] : 1024 );
        maxlinesize = ( cache->linesize[cache->levels] ? cache->linesize[cache->levels] : maxCstride / 2 );
        delayC = cache->latency2[0] - cache->latency1[0];

        sprintf(fnx1, "%s.gp", fnn1);
        if (!(fp = fopen(fnx1,"w"))) ErrXit("main: 'fp = fopen(%s,\"w\")` failed", fnx1);
        plotCache(cache, result1, MHz, fnn1, fp, 0);
        fclose(fp);

        sprintf(fnx2, "%s.gp", fnn2);
        if (!(fp = fopen(fnx2,"w"))) ErrXit("main: 'fp = fopen(%s,\"w\")` failed", fnx2);
        plotCache(cache, result2, MHz, fnn2, fp, delayC);
        fclose(fp);

        yy = (result1[0][0] >> 24) - 1;
        for (y = 0; y <= yy; y++) {
                free(result1[y]);
                result1[y] = 0;
        }
        free(result1);
        result1 = 0;

        yy = (result2[0][0] >> 24) - 1;
        for (y = 0; y <= yy; y++) {
                free(result2[y]);
                result2[y] = 0;
        }
        free(result2);
        result2 = 0;


        sprintf(fnn1, "%s.TLB-miss-latency", av[3]);
        sprintf(fnx1, "%s.data", fnn1);
        if (!(fp = fopen(fnx1,"w"))) ErrXit("main: 'fp = fopen(%s,\"w\")` failed", fnx1);
        result1 = runTLB(array, maxrange, 1024, maxlinesize, mincachelines, MHz, fp, &maxTstride);
        fclose(fp);

/*
        sprintf(fnn2, "%s.TLB2", av[3]);
        sprintf(fnx2, "%s.data", fnn2);
        if (!(fp = fopen(fnx2,"w"))) ErrXit("main: 'fp = fopen(%s,\"w\")` failed", fnx2);
        result2 = runTLB(array, maxrange, 1024, maxlinesize, mincachelines, MHz, fp, &maxTstride);
        fclose(fp);
*/
        result2 = result1;

        TLB = analyzeTLB(result1, result2, maxlinesize, mincachelines, MHz);
        delayT = TLB->latency2[0] - TLB->latency1[0];

        sprintf(fnx1, "%s.gp", fnn1);
        if (!(fp = fopen(fnx1,"w"))) ErrXit("main: 'fp = fopen(%s,\"w\")` failed", fnx1);
        plotTLB(TLB, result1, MHz, fnn1, fp, 0);
        fclose(fp);

/*
        sprintf(fnx2, "%s.gp", fnn2);
        if (!(fp = fopen(fnx2,"w"))) ErrXit("main: 'fp = fopen(%s,\"w\")` failed", fnx2);
        plotTLB(TLB, result2, MHz, fnn2, fp, delayT);
        fclose(fp);
*/

        yy = (result1[0][0] >> 24) - 1;
        for (y = 0; y <= yy; y++) {
                free(result1[y]);
                result1[y] = 0;
        }
        free(result1);
        result1 = 0;

/*
        yy = (result2[0][0] >> 24) - 1;
        for (y = 0; y <= yy; y++) {
                free(result2[y]);
                result2[y] = 0;
        }
        free(result2);
*/
        result2 = 0;


        fprintf(stdout,"\n");

        printCPU(cache, MHz, delayC);
        printCache(cache, MHz);
        printTLB(TLB, MHz);

        free(cache);
        cache = 0;
        free(TLB);
        TLB = 0;

        return(0);
}

