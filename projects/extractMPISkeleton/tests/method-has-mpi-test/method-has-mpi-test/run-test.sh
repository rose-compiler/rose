
../../extractMPISkeleton -skel:d -skel:s ../../apis.coll -I../../mpi-fakeheader Simple.cc main.cc > split_test.out 2>&1
for FILE in rose_Simple.cc rose_main.cc; do
        mv $FILE $FILE.split;
done

../../extractMPISkeleton -skel:d -skel:s ../../apis.coll -I../../mpi-fakeheader main.cc Simple.cc > split_test_main_first.out 2>&1
for FILE in rose_Simple.cc rose_main.cc; do
        mv $FILE $FILE.split_main_first;
done

../../extractMPISkeleton -skel:d -skel:s ../../apis.coll -I../../mpi-fakeheader all.cc > all_test.out 2>&1

