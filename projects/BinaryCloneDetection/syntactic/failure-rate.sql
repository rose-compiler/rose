-- Computes statistics about false negatives in a database populated with two or more specimens compiled in different ways.

drop table if exists fr_results;
drop table if exists fr_false_positives;
drop table if exists fr_false_negatives;
drop table if exists fr_clone_pairs;
drop table if exists fr_negative_pairs;
drop table if exists fr_positive_pairs;
drop table if exists fr_fio;
drop table if exists fr_functions;
drop table if exists fr_funcnames;
drop table if exists fr_specimens;

-- Files that are binary specimens; i.e., not shared libraries, etc.
create table fr_specimens as
    select distinct file as name from functions;

-- Function names that are present exactly once in each specimen
-- And which contain a certain number of instruction bytes
create table fr_funcnames as
    select func1.function_name as name
        from fr_specimens as file
        join functions as func1 on func1.file = file.name
        left join functions as func2
            on func1.id <> func2.id
            and func1.function_name <> ''
	    and func1.function_name = func2.function_name
	    and func1.file = func2.file
        where func2.id is null and func1.isize >= 7 -- bytes
        group by func1.function_name
        having count(*) = (select count(*) from fr_specimens);


-- Functions that have a name and which appear once per specimen by that name.
create table fr_functions as
    select func.*
        from fr_funcnames as named
        join functions as func on named.name = func.function_name;


-- Pairs of functions that should have been detected as being similar.  We're assuming that any pair of binary functions that
-- have the same name are in fact the same function at the source level and should therefore be similar.
create table fr_positive_pairs as
    select f1.row_number as func1_id, f2.row_number as func2_id
        from fr_functions as f1
        join fr_functions as f2
            on f1.function_name = f2.function_name
            and f1.row_number < f2.row_number
            and f1.file <> f2.file;

-- Pairs of functions that should not have been detected as being similar.  We're assuming that if the two functions have
-- different names then they are different functions in the source code, and that no two functions in source code are similar.
-- That second assumption almost certainly doesn't hold water!
create table fr_negative_pairs as
    select f1.row_number as func1_id, f2.row_number as func2_id
        from fr_functions as f1
        join fr_functions as f2 on f1.function_name <> f2.function_name and f1.file < f2.file;

-- Pairs of functions that were _detected_ as being similar.
create table fr_clone_pairs as
    select function_id_1 as func1_id, function_id_2 as func2_id
        from cluster_pairs
        where ratio_1 > 0.95 and ratio_2 > 0.95;

select 'pairs detected as similar' as x, count(*) from fr_clone_pairs;
select 'pairs from two files' as x, count(*)
    from fr_clone_pairs as pair
    join functions as func1 on pair.func1_id = func1.id
    join functions as func2 on pair.func2_id = func2.id
    where func1.file <> func2.file;

-- Table of false negative pairs.  These are pairs of functions that were not determined to be similar but which are present
-- in the fr_positives_pairs table.
create table fr_false_negatives as
    select * from fr_positive_pairs except select func1_id, func2_id from fr_clone_pairs;

-- Table of false positive pairs.  These are pairs of functions that were determined to be similar but which are present
-- in the fr_negative_pairs table.
create table fr_false_positives as
    select * from fr_negative_pairs intersect select func1_id, func2_id from fr_clone_pairs;

-- False negative rate is the ratio of false negatives to expected positives
-- False positive rate is the ratio of false positives to expected negatives
create table fr_results as
    select
        (select count(*) from fr_positive_pairs) as "Expected Positives",
        (select count(*) from fr_negative_pairs) as "Expected Negatives",
        (select count(*) from fr_false_negatives) as "False Negatives",
        (select count(*) from fr_false_positives) as "False Positives",
        ((select 100.0*count(*) from fr_false_negatives) / (select count(*) from fr_positive_pairs)) as "FN Percent",
        ((select 100.0*count(*) from fr_false_positives) / (select count(*) from fr_negative_pairs)) as "FP Percent";

-------------------------------------------------------------------------------------------------------------------------------
-- Some queries to show the results

select * from fr_results;

-- select 'The following table contains all the functions of interest: functions that
-- * appear in all specimens exactly once each
-- * have a certain number of instructions, and
-- * didn''t try to consume too many inputs, and
-- * are defined in a specimen, not a dynamic library' as "Notice";
-- select * from fr_functions order by function_name, file;

-- select 'The following table shows the false negative function pairs.
-- Both functions of the pair always have the same name.' as "Notice";
-- select
-- 	func1.function_name as name,
-- 	func1.file as file1,
-- 	func2.file as file2
--     from fr_false_negatives as falseneg
--     join fr_functions as func1 on falseneg.func1_id = func1.row_number
--     join fr_functions as func2 on falseneg.func2_id = func2.row_number
--     order by func1.function_name, func2.function_name;
