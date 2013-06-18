-- Computes statistics about false negatives in a database populated with two or more specimens compiled in different ways.
begin transaction;

drop table if exists fr_false_positives;
drop table if exists fr_false_negatives;
drop table if exists fr_clone_pairs;
drop table if exists fr_negative_pairs;
drop table if exists fr_positive_pairs;
drop table if exists fr_functions;
drop table if exists fr_funcnames;
drop table if exists fr_specimens;

-- Files that are binary specimens; i.e., not shared libraries, etc.
create table fr_specimens as
    select distinct specimen_id
    from semantic_functions;

-- Function names that are present exactly once in each specimen
-- And which contain 100 or more instructions						!!FIXME!!
create table fr_funcnames as
    select func1.name as name
	from semantic_functions as func1
	left join semantic_functions as func2
	    on func1.id<>func2.id and func1.name=func2.name and func1.specimen_id=func2.specimen_id
	where func2.id is null and func1.ninsns >= 100
	group by func1.name
	having count(*) = (select count(*) from fr_specimens);

-- Functions that have a name and which appear once per specimen by that name.
create table fr_functions as
    select func.*
        from fr_funcnames as named
        join semantic_functions as func on named.name = func.name;

-- Pairs of functions that should have been detected as being similar.  We're assuming that any pair of binary functions that
-- have the same name are in fact the same function at the source level and should therefore be similar.
create table fr_positive_pairs as
    select f1.id as func1_id, f2.id as func2_id
        from fr_functions as f1
        join fr_functions as f2 on f1.name = f2.name and f1.id < f2.id and f1.specimen_id <> f2.specimen_id;

-- Pairs of functions that should not have been detected as being similar.  We're assuming that if the two functions have
-- different names then they are different functions in the source code, and that no two functions in source code are similar.
-- That second assumption almost certainly doesn't hold water!
create table fr_negative_pairs as
    select f1.id as func1_id, f2.id as func2_id
        from fr_functions as f1
        join fr_functions as f2 on f1.name <> f2.name and f1.id < f2.id;

-- Pairs of functions that were _detected_ as being similar.
create table fr_clone_pairs as
    select distinct c1.func_id as func1_id, c2.func_id as func2_id
        from semantic_clusters as c1
        join semantic_clusters as c2 on c1.id = c2.id and c1.func_id < c2.func_id;

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

select * from fr_results;

select 'The following table shows functions that appear in all specimens exactly once each' as "Notice";
select * from fr_functions order by name, specimen_id;


rollback;
