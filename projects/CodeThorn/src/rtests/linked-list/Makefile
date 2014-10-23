
all: test_forward test_commit

BACKSTROKE=reverser

CT_SRC_DIR=../..

rose_900_create_insert_delall_mod.C: 900_create_insert_delall_mod.C
	${BACKSTROKE} 900_create_insert_delall_mod.C

test_forward:  test_rose_900_create_insert_delall_mod.C rose_900_create_insert_delall.C
	g++ -O3 test_rose_900_create_insert_delall_mod.C -pthread -I${CT_SRC_DIR} -otest_forward

test_commit: test_rose_900_create_insert_delall_mod.C rose_900_create_insert_delall.C
	g++ -O3 -DCOMMIT_ONLY test_rose_900_create_insert_delall_mod.C -pthread -I${CT_SRC_DIR} -otest_commit

clean:
	rm -f rose_900_create_insert_delall_mod.C
	rm -f test_forward test_reverse

check: test_forward test_commit
	./test_forward
	./test_commit


