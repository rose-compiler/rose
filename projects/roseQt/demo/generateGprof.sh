g++ -g3 -pg inputTest.cpp -o inputTest
./inputTest 100000
gprof -lL inputTest gmon.out > gprof_data
