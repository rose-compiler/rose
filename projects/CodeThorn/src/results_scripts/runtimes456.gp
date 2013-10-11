set output "runtimes456.pdf"
set terminal pdf
set title "Benchmarks (Analysis Time in Minutes)"
C = "#ff0000"; Cpp = "#ff0000"; Java = "#44ff44"; Python = "#44ff44"; P5 = "#0000ff";
set auto x
set auto y
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.7
set xtic scale 0
# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot 'runtimes456.dat' using ($2/1000/60):xtic(1) ti col fc rgb C, '' u ($3/1000/60) ti col fc rgb Cpp, '' u ($4/1000/60) ti col fc rgb Java, '' u ($5/1000/60) ti col fc rgb Python, '' u ($6/1000/60) ti col fc rgb P5
