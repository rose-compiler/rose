
How to Find Dataset For Visualization
------------------------------
1. CREATE DATABASE
    Create database of clone vectors to look for clones in
          /path/to/BinaryCloneDetection/createVectorsBinary --database db-name.sql \
             --tsv-directory tsv-directory --stride 1 --windowSize 40  
    If you want to run this using ROSE as a frontend you have to currently ignore
    procedure boundaries since ROSE does not correctly recognize functions. This
    is done using the '-i' flag as e.g
          /path/to//BinaryCloneDetection/createVectorsBinary --database db-name.sql \
             --tsv-directory /name/of/binary --stride 1 --windowSize 120  -i

    Create database of all binary files represented as a tsv directory in a sub-directory
      find -name "*-tsv" -exec /path/to/BinaryCloneDetection/createVectorsBinary \
        --database db-name.sql --tsv-directory {} --stride 1 --windowSize 40 \; 

2. RUN CLONE DETECTION TOOL
    Decide which similarity grade you want between 0 and 1.0 where 1.0 is
    exact clone detection and 0 is saying everything is a clone of everything
    /path/to/BinaryCloneDetection/findClones --database db-name.sql  \
       -t ${similarity-grade}

OPTIONAL STEPS FOR EXACT CLONE DETECTION

The next steps will only work reliably for exact clone detection due to
clone-edges that cross each other due to loss of specificity, but you can
apply it to inexct clone detetion at your own risk.

3. FIND LARGEST CLONES
    This algorithm will find the largest clone *pairs*, not clusters as
    before. It does that by decoupling the set of clone clusters into a 
    set of clone pairs, and then optimistically merging adjacent clone
    pairs.
       /path/to/BinaryCloneDetection/findLargestClones --database db-name.sql
    
4. FIND CLUSTERS OF LARGEST CLONES
    This algorithm will find all exact disjoint sets.
       /path/to/BinaryCloneDetection/findExactDisjointSets --database db-name.sql

5. VISUALIATION
    To generate the gml file for visualizing the largest clones
       /path/to/BinaryCloneDetection/printOutClones_distinctFiles --database db-name.sql


How To Get Sizes
----------------
To get the total number of vectors in the database 
   sqlite3 db-name.sql   'select max(row_number) from vectors'
INEXACT CLONE DETECTION (similarith < 1.0): To get the total number of elements of the clusters in the database 
   sqlite3 db-name.sql   'select max(row_number) from clusters'
EXACT CLONE DETECTION (similarity == 1.0) :   To get the total number of elements of the postprocessed clusters in the database 
   sqlite3 db-name.sql   'select max(row_number) from postprocessed_clusters'

How to Build 
------------
autoreconf --install
./configure
make

How To Create Input Format for ROSE
------------------------------------

Put all files that you want to disassemble into a directory. 
Change the path in create_dbs.py
   ab='/home/andreas/local/idaadv/idal -B '
to the correct idal binary. The same for
    ab='/home/andreas/local/idaadv/idal
-OIDAPython:/home/andreas/local/idaadv/ida2sql.py '
and there change it for -OIDSPython: as well so that it points
to your ida2sql.py.

When that is done 
    cd /directory/with/binaries/to/disassemble
    python /path/to/BinaryCloneDetection/create_dbs.py
    find -name "*.sql" -exec
tclsh /path/to/BinaryCloneDetection/split-into-tables.tcl {} \;

This will give you the input "*-tsv" directies that ROSE needs.



How To Create Clone Vectors
------------
Create database of clone vectors to look for clones in
   /repos/prjdata/clonedetection/binary/BinaryCloneDetection/createVectorsBinary
    --database `pwd`/sqlite-database-name.sql --tsv-directory tsv-directory --stride 1 --windowSize 40  
If you want to run this using ROSE as a frontend you have to currently ignore
procedure boundaries since ROSE does not correctly recognize functions. This
is done using the '-i' flag as e.g
   /repos/prjdata/clonedetection/binary/BinaryCloneDetection/createVectorsBinary
    --database `pwd`/sqlite-database-name.sql --tsv-directory /name/of/binary --stride 1 --windowSize 120  -i


Create database of all binary files represented as a tsv directory in a
sub-directory
find -name "*-tsv" -exec
/repos/prjdata/clonedetection/binary/BinaryCloneDetection/createVectorsBinary
--database `pwd`/windowsXP-1-40.sql --tsv-directory {} --stride 1 --windowSize
40 \; 

How To Run Clone Detection
------------------------------------------
First, decide on the similarity grade you want (e.g t=1.0 which is exact clone
detection).
Then run
     ./findLargestClones --database /name/of/database -t 1.0



How To Run Given a Set of Clone Vectors When You Are Only Interested In A
Subset
---------------------------------------

1. Find the functions you want to remove
     sqlite3 sqlite-database-name.sql
     sqlite> select * from function_ids where lower(function_name)=lower("IcfGetAdapters");
         231568|./hnetcfg.dll-tsv|IcfGetAdapters
         1108953|./wscsvc.dll-tsv|IcfGetAdapters

2. Find the clones with the functions you are interested in
     ./exactCloneDetection --database /repos/prjdata/clonedetection/binary/system32/windowsXP-1-40.sql  -f 1108953

3. Filter the data set into a set of largest possible clones:
     ./findLargestClones --database ../system32/windowsXP-1-40.sql

4. Create a more readable format by
     sqlite3 ../system32/windowsXP-1-40.sql
     sqlite3>select b.file, b.function_name, a.begin_index_within_function_A,a.end_index_within_function_A, c.file, c.function_name, a.begin_index_within_function_B,a.end_index_within_function_B from largest_clones a join function_ids b  on a.function_id_A = b.row_number join function_ids c  on a.function_id_B = c.row_number;
