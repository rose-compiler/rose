##  #set terminal png transparent nocrop enhanced font arial 8 size 420,320 
##  set terminal png transparent nocrop enhanced
##  
##  set boxwidth 0.9 absolute
##  set style fill solid 1.00 border -1
##  set style histogram clustered gap 1 title offset character 0, 0, 0
##  set datafile missing ''   # how missing data fields are marked
##  set style data histograms
##  
##  #set xtics border in scale 1,0.5 nomirror rotate by -45  offset character 0, 0, 0
##  set xtics nomirror rotate by -45
##  
##  #set xtics   ("srw_reverse" 0.00000, "srw_reverse_contra" 1.00000, "srw_insert" 2.00000, "srw_insert_contra" 3.00000, "artificialSum" 4.00000, "artificialSumWithDS" 5.00000, "DSW" 6.00000, "hackett" 7.00000)
##  
##  #set yrange [ 0.00000 : 300000. ] noreverse nowriteback
##  
##  


# for color:
set terminal postscript eps enhanced font "CMTI10, 10" color
set style fill solid 1.00 border -1

# for b/w
#set terminal postscript eps enhanced font "CMTI10, 10"
#set style fill pattern border -1


set key autotitle columnheader 

set boxwidth 0.7 absolute     # thickness of bars
set datafile missing ''       # how missing data is indicated in datafile
set style data histogram      # histogram (not points, lines, ...)
set style histogram clustered # clustered
set grid y                    # dotted horizontal lines

set ylabel ""
set xlabel ""
set xtics nomirror rotate by -45

set output 'diag.may.eps'
set title "NNH99/SRW98 Comparison: May-Aliases"
set ylabel "Number of May-Aliased Expressions"
set yrange [ 0.00000 : ] noreverse nowriteback
plot newhistogram, 'report.txt' using 6:xtic(1), '' using 12

set output 'diag.must.eps'
set title "NNH99/SRW98 Comparison: Must-Aliases"
set ylabel "Number of Must-Aliased Expressions"
plot newhistogram, 'report.txt' using 7:xtic(1), '' using 13

set output 'diag.time.analysis.eps'
set title "NNH99/SRW98 Comparison: Analysis Runtime"
set ylabel "Analysis Runtime (seconds)"
plot newhistogram, 'report.txt' using 2:xtic(1), '' using 8

#set output 'diag.time.combined.eps'
#set title "NNH99/SRW98 Comparison: Analysis Runtime"
#set ylabel "Analysis Runtime (seconds)"
#set style histogram rowstacked
##set yrange [ 0.00000 : 30.000 ] noreverse nowriteback
#plot \
#newhistogram "NNH99", 'report.txt' using 2:xtic(1), '' u 4, \
#newhistogram "SRW98", 'report.txt' using 8:xtic(1), '' u 10


set output 'diag.vs.may.eps'
set title "NNH99 Compared to SRW98"
set ylabel "Positive: NNH99 is better, Negative: NNh99 is worse"
set yrange [ -10.000 : 10.000 ] noreverse nowriteback
plot newhistogram, \
'report.txt' using (column(12)-column(6)):xtic(1) title 6, \
'report.txt' using (column(7)-column(13)):xtic(1) title 7, \
'report.txt' using (column(8)-column(2)):xtic(1) title 2, \
'report.txt' using (column(10)-column(4)):xtic(1) title 4

