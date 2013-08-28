set output "stg789.pdf"
set terminal pdf
set title "Benchmarks: Specialized Analyzers: EStates and STG"
C = "#0000ff"; Cpp = "#a547ff"; Java = "#44ff44"; Python = "#44ff44";
set auto x
set auto y
set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 0.7
set xtic scale 0
# 2, 3, 4, 5 are the indexes of the columns; 'fc' stands for 'fillcolor'
plot 'stg789.dat' using ($2/1):xtic(1) ti col fc rgb C, '' u ($3/1) ti col fc rgb Cpp
