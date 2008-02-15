# Calibrator v0.9e
# (by Stefan.Manegold@cwi.nl, http://www.cwi.nl/~manegold/)
 set term postscript portrait enhanced
 set output 'output.cache-replace-time.ps'
#set term gif transparent interlace small size 500, 707 # xFFFFFF x333333 x333333 x0055FF x005522 x660000 xFF0000 x00FF00 x0000FF
#set output 'output.cache-replace-time.gif'
set data style linespoints
set key below
set title 'output.cache-replace-time'
set xlabel 'memory range [bytes]'
set x2label ''
set ylabel 'nanosecs per iteration'
set y2label 'cycles per iteration'
set logscale x 2
set logscale x2 2
set logscale y 10
set logscale y2 10
set format x '%1.0f'
set format x2 '%1.0f'
set format y '%1.0f'
set format y2 ''
set xrange[0.750000:640.000000]
#set x2range[0.750000:640.000000]
set yrange[1.000000:1000.000000]
#set y2range[1.000000:1000.000000]
set grid x2tics
set xtics mirror ('1k' 1, '' 2, '4k' 4, '' 8, '16k' 16, '' 32, '64k' 64, '' 128, '256k' 256, '' 512)
set x2tics mirror ('[8k]' 8, '[384k]' 384)
set y2tics ('(9)' 4.150000, '(35)' 16.030000, '(298)' 135.240000, '2.2' 1, '22' 10, '220' 100, '2.2e+03' 1000)
set label 1 '(4.09)  ' at 0.750000,4.090909 right
set arrow 1 from 0.750000,4.090909 to 640.000000,4.090909 nohead lt 0
set label 2 '(15.9)  ' at 0.750000,15.909091 right
set arrow 2 from 0.750000,15.909091 to 640.000000,15.909091 nohead lt 0
set label 3 '(135)  ' at 0.750000,135.454544 right
set arrow 3 from 0.750000,135.454544 to 640.000000,135.454544 nohead lt 0
 set label 4 '^{ Calibrator v0.9e (Stefan.Manegold\@cwi.nl, www.cwi.nl/~manegold) }' at graph 0.5,graph 0.02 center
#set label 4    'Calibrator v0.9e (Stefan.Manegold@cwi.nl, www.cwi.nl/~manegold)'    at graph 0.5,graph 0.03 center
plot \
0.1 title 'stride:' with points pt 0 ps 0 , \
'output.cache-replace-time.data' using 1:($7-0.000000) title '256' with linespoints lt 1 pt 3 , \
'output.cache-replace-time.data' using 1:($13-0.000000) title '\{128\}' with linespoints lt 2 pt 4 , \
'output.cache-replace-time.data' using 1:($19-0.000000) title '\{64\}' with linespoints lt 3 pt 5 , \
'output.cache-replace-time.data' using 1:($25-0.000000) title '32' with linespoints lt 4 pt 6 , \
'output.cache-replace-time.data' using 1:($31-0.000000) title '16' with linespoints lt 5 pt 7 , \
'output.cache-replace-time.data' using 1:($37-0.000000) title '8' with linespoints lt 6 pt 8 , \
'output.cache-replace-time.data' using 1:($43-0.000000) title '4' with linespoints lt 7 pt 9
set nolabel
set noarrow
