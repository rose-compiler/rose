set output "ltlruntimes456.pdf"
set terminal pdf
set title "Benchmarks Specialized Analyzers (Analysis Phase Times + LTL1 vs LTL2 in Minutes)"
C = "#f50f0f"; Cpp = "#668278"; Java = "#f000ff"; Python = "#ffff88"; P5 = "#ffdd00";
set auto x
set yrange [0:10]
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.7
set xtic scale 0
# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot 'ltlruntimes456.dat' using ($2/1000/60):xtic(1) ti col fc rgb C, '' u ($3/1000/60) ti col fc rgb Cpp, '' u ($4/1000/60) ti col fc rgb Java, '' u ($5/1000/60) ti col fc rgb Python, '' u ($6/1000/60) ti col fc rgb P5
