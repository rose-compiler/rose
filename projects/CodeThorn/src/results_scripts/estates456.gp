set output "estates456.pdf"
set terminal pdf
set title "Benchmarks: EStates"
C = "#ff0000"; Cpp = "#ff7700"; Java = "#0000ff"; Python = "#0000bb";
set auto x
set auto y
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.7
set xtic scale 0
# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot 'estates456.dat' using ($2/1):xtic(1) ti col fc rgb C, '' u ($3/1) ti col fc rgb Cpp, '' u ($4/1) ti col fc rgb Java, '' u ($5/1) ti col fc rgb Python
