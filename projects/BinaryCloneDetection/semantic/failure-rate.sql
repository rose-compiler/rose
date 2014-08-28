-- Computes statistics about false negatives in a database populated with two or more specimens compiled in different ways.
begin transaction;

drop table if exists fr_results;
drop table if exists fr_results_precision_recall;
drop table if exists fr_false_positives;
drop table if exists fr_false_negatives;
drop table if exists fr_true_positives;
drop table if exists fr_true_negatives;
drop table if exists fr_clone_pairs;
drop table if exists fr_negative_pairs;
drop table if exists fr_positive_pairs;
drop table if exists fr_true_negatives;
drop table if exists fr_true_positives;
drop table if exists fr_function_pairs;
drop index if exists fr_fio_func_id;
drop table if exists fr_fio;
drop table if exists fr_functions;
drop table if exists fr_funcnames;
drop table if exists fr_specimens;
drop table if exists fr_settings;

create table fr_settings as
    select 
    (select 0.70) as similarity_threshold,
    (select 0.50) as path_similarity_threshold,
    (select 0.30) as cg_similarity_threshold;

-- Files that are binary specimens; i.e., not shared libraries, etc.
create table fr_specimens as
    select distinct specimen_id as id
    from semantic_functions;

-- Function names that are present exactly once in each specimen
-- And which contain a certain number of instructions                                           !!FIXME: should be a parameter
-- And for which no test tried to consume too many inputs (status<>911000007)
-- And which come from the specimen, not from a dynamic library
create table fr_funcnames as
    select func1.name as name
        from fr_specimens as file
        join semantic_functions as func1 on func1.file_id = file.id
        left join semantic_functions as func2
            on func1.id<>func2.id and func1.name <> '' and func1.name=func2.name and func1.file_id=func2.file_id
        where func2.id is null and func1.ninsns >= 100                                          -- !!FIXME
        group by func1.name
        having count(*) = (select count(*) from fr_specimens)
--     except (
--       select func.name as name
--           from semantic_fio as fio
--           join semantic_functions as func on fio.func_id = func.id
--           where fio.globals_consumed > 0
--     )
;


-- Functions that have a name and which appear once per specimen by that name.
create table fr_functions as
    select distinct func.*
        from fr_funcnames as named
        join semantic_functions as func on named.name = func.name

        -- uncomment the following lines to consider only those functions that passed at least once
--      join semantic_fio as test on func.id=test.func_id and test.status=0
;

-- Tests that we want to consider
create table fr_fio as
    select fio.*
        from fr_functions as func
        join semantic_fio as fio on func.id = fio.func_id;
create index fr_fio_func_id on fr_fio(func_id);

-- Select all pairs of functions from two different specimens
create table fr_function_pairs as
    select distinct f1.id as func1_id, f2.id as func2_id
        from fr_functions as f1
        join fr_functions as f2 on f1.id < f2.id and f1.specimen_id <> f2.specimen_id 
        -- where f1.ninsns >= 100 AND f2.ninsns >= 100

        -- Uncomment the following lines to consider only those pairs of functions where both functions passed for
        -- at least one of the same input groups
        join fr_fio as test1 on f1.id = test1.func_id
        join fr_fio as test2 on f2.id = test2.func_id and test1.igroup_id = test2.igroup_id
        where test1.status = 0 and test2.status = 0 AND f1.ninsns >= 100 AND f2.ninsns >= 100

        -- Uncomment the following lines to consider only those pairs of functions where all tests were successful
--      except
--          select  f1.id as func1_id, f2.id as func2_id
--          from fr_functions as f1
--          join fr_functions as f2 on f1.name = f2.name and f1.id < f2.id and f1.specimen_id <> f2.specimen_id
--          join fr_fio as test1 on f1.id = test1.func_id
--          join fr_fio as test2 on f2.id = test2.func_id
--          where test1.status <> 0 or test2.status <> 0
;



-- Pairs of functions that should have been detected as being similar.  We're assuming that any pair of binary functions that
-- have the same name are in fact the same function at the source level and should therefore be similar.
create table fr_positive_pairs as
    select pair.*
        from fr_function_pairs as pair
        join fr_functions as f1 on pair.func1_id = f1.id
        join fr_functions as f2 on pair.func2_id = f2.id
        where f1.name = f2.name;

-- Pairs of functions that should not have been detected as being similar.  We're assuming that if the two functions have
-- different names then they are different functions in the source code, and that no two functions in source code are similar.
-- That second assumption almost certainly doesn't hold water!
create table fr_negative_pairs as
    select pair.*
        from fr_function_pairs as pair
        join fr_functions as f1 on pair.func1_id = f1.id
        join fr_functions as f2 on pair.func2_id = f2.id
        where f1.name <> f2.name;

-- Pairs of functions that were _detected_ as being similar.
create table fr_clone_pairs as
    select sim.func1_id, sim.func2_id
        from semantic_funcsim sim 
        join api_call_similarity api_sem on api_sem.func1_id = sim.func1_id AND api_sem.func2_id = sim.func2_id
        where sim.similarity >= (select similarity_threshold from fr_settings) 
  AND api_sem.min_similarity >= (select path_similarity_threshold from fr_settings) 
  AND api_sem.cg_similarity >=  (select cg_similarity_threshold   from fr_settings)
  --AND sim.path_max_euclidean_d_ratio - sim.path_min_euclidean_d_ratio < 0.5
  --AND sim.path_max_euclidean_d_ratio < 3.0
  ;

-- Table of false negative pairs.  These are pairs of functions that were not determined to be similar but which are present
-- in the fr_positives_pairs table.
create table fr_false_negatives as
    select * from fr_positive_pairs except select func1_id, func2_id from fr_clone_pairs;

-- Table of false positive pairs.  These are pairs of functions that were determined to be similar but which are present
-- in the fr_negative_pairs table.
create table fr_false_positives as
    select * from fr_negative_pairs intersect select func1_id, func2_id from fr_clone_pairs;

create table fr_true_positives as
    select * from fr_positive_pairs intersect select func1_id, func2_id from fr_clone_pairs;

create table fr_true_negatives as
    select * from fr_negative_pairs except select func1_id, func2_id from fr_clone_pairs;


select 'The following table shows the true positives function pairs.
Both functions of the pair always have the same name.' as "Notice";
select
        func1.name as name,
        sim.func1_id, func1.ninsns, sim.func2_id, func2.ninsns, sim.similarity, api_sem.cg_similarity, api_sem.min_similarity, sim.hamming_d, sim.euclidean_d, sim.euclidean_d, sim.ncompares,
        sim.path_ave_hamming_d, sim.path_min_hamming_d, sim.path_max_hamming_d,
        sim.path_ave_euclidean_d_ratio, sim.path_min_euclidean_d, sim.path_max_euclidean_d_ratio
    from fr_true_positives as falseneg
    join fr_functions as func1 on falseneg.func1_id = func1.id
    join fr_functions as func2 on falseneg.func2_id = func2.id
    join semantic_funcsim as sim on falseneg.func1_id=sim.func1_id and falseneg.func2_id=sim.func2_id
    join api_call_similarity api_sem on api_sem.func1_id = sim.func1_id AND api_sem.func2_id = sim.func2_id
    order by func1.name;


select 'The following table shows the false positives function pairs.
Both functions of the pair always have the same name.' as "Notice";
select
        func1.name ,
        sim.func1_id, func1.ninsns, func2.name, sim.func2_id, func2.ninsns, sim.similarity, api_sem.cg_similarity, api_sem.min_similarity, sim.hamming_d, sim.euclidean_d, sim.euclidean_d, sim.ncompares, 
        sim.path_ave_hamming_d, sim.path_min_hamming_d, sim.path_max_hamming_d,
        sim.path_ave_euclidean_d_ratio, sim.path_min_euclidean_d, sim.path_max_euclidean_d_ratio
    from fr_false_positives as falseneg
    join fr_functions as func1 on falseneg.func1_id = func1.id
    join fr_functions as func2 on falseneg.func2_id = func2.id
    join semantic_funcsim as sim on falseneg.func1_id=sim.func1_id and falseneg.func2_id=sim.func2_id
    join api_call_similarity api_sem on api_sem.func1_id = sim.func1_id AND api_sem.func2_id = sim.func2_id
    order by func1.name;



-- False negative rate is the ratio of false negatives to expected positives
-- False positive rate is the ratio of false positives to expected negatives
create table fr_results as
    select
        (select count(*) from fr_positive_pairs)  as expected_positives,
        (select count(*) from fr_negative_pairs)  as expected_negatives,
        ((select count(*) from fr_true_positives ) + (select count(*) from fr_false_positives) ) as positives,
        ((select count(*) from fr_true_negatives ) + (select count(*) from fr_false_negatives) ) as negatives,
        (select count(*) from fr_true_positives ) as true_positives,
        (select count(*) from fr_true_negatives ) as true_negatives,
        (select count(*) from fr_false_positives) as false_positives,
        (select count(*) from fr_false_negatives) as false_negatives,
        ((select 100.0*count(*) from fr_true_positives ) / (0.001+((select count(*) from fr_true_positives ) + (select count(*) from fr_false_positives)))) as tp_percent,
        (100.0*((select 1.0*count(*) from fr_negative_pairs ) - (select 1.0*count(*) from fr_false_negatives) ) / (0.001+(select count(*) from fr_negative_pairs))) as tn_percent,
        ((select 100.0*count(*) from fr_false_positives) / (0.001+((select count(*) from fr_true_positives ) + (select count(*) from fr_false_positives) ))) as fp_percent,
        ((select 100.0*count(*) from fr_false_negatives) / (0.001+(select count(*) from fr_negative_pairs))) as fn_percent
;
 
create table fr_results_precision_recall as
    select
        ( (select 100.0*true_positives from fr_results limit 1) / (0.001+( ( select true_positives from fr_results  limit 1 ) + (select false_negatives from fr_results limit 1) ))  ) as recall,
        ( (select 100.0*true_negatives from fr_results limit 1) / (0.001+( ( select true_negatives from fr_results  limit 1) + (select false_positives from fr_results limit 1)) )) as specificity,
        ( (select 100.0*true_positives from fr_results limit 1) / (0.001+( ( select true_positives from fr_results  limit 1) + (select false_positives from fr_results  limit 1 )))) as precision
;
-------------------------------------------------------------------------------------------------------------------------------
-- Some queries to show the results

select * from fr_results;
select * from fr_results_precision_recall;


-- select 'The following table contains all the functions of interest: functions that
-- * appear in all specimens exactly once each
-- * have a certain number of instructions, and
-- * didn''t try to consume too many inputs, and
-- * are defined in a specimen, not a dynamic library' as "Notice";
-- select * from fr_functions order by name, specimen_id;

select 'The following table shows the termination status for all
tests performed on the functions of interest.' as "Notice";
select
        fault.name as status,
        count(*) as ntests,
        100.0*count(*)/(select count(*) from fr_fio) as percent
    from fr_fio as fio
    join semantic_faults as fault on fio.status = fault.id
    group by fault.id, fault.name;

select 'The following table shows the false negative function pairs.
Both functions of the pair always have the same name.' as "Notice";
select
        func1.name as name,
        sim.func1_id, sim.func2_id, sim.similarity, sim.euclidean_d, sim.euclidean_d,  sim.ncompares,
        sim.path_ave_hamming_d, sim.path_min_hamming_d, sim.path_max_hamming_d,
        sim.path_ave_euclidean_d, sim.path_min_euclidean_d, sim.path_max_euclidean_d
 
    from fr_false_negatives as falseneg
    join fr_functions as func1 on falseneg.func1_id = func1.id
    join fr_functions as func2 on falseneg.func2_id = func2.id
    join semantic_funcsim as sim on falseneg.func1_id=sim.func1_id and falseneg.func2_id=sim.func2_id
    order by func1.name;




select 'The following table shows the status breakdown for tests
that were performed on each function of interest.' as "Notice";

select
        func.id as func_id,
        func.name as func_name,
        func.file_id as file_id,
        fault.name as status,
        count(*) as ntests
    from fr_functions as func
    join fr_fio as fio on fio.func_id = func.id
    join semantic_faults as fault on fault.id = fio.status
    group by func.id, func.name, func.file_id, fault.name
    order by func.name, func.file_id;

select 'The following table shows a list of function names that are
false negatives, and the average number of instructions executed by
tests on this name.' as "Notice";
select
        func.name, func.id, func.file_id,
	count(*) as npass,
        sum(fio.instructions_executed)/count(*) as ave_insns_exec
    from fr_fio as fio
    join fr_functions as func on fio.func_id = func.id
    join fr_false_negatives as neg on fio.func_id=neg.func1_id or fio.func_id=neg.func2_id
    where fio.status = 0
    group by func.name, func.id, func.file_id
    order by func.name, func.file_id;

-- output that can be fed to the optimizer
--select 'y' as oracle, t2.similarity as sim1
--    from fr_positive_pairs as t1
--    join semantic_funcsim as t2 on t1.func1_id=t2.func1_id and t1.func2_id=t2.func2_id
--  union all
--select 'n' as oracle, t2.similarity as sim1
--    from fr_negative_pairs as t1
--    join semantic_funcsim as t2 on t1.func1_id=t2.func1_id and t1.func2_id=t2.func2_id;



select 'The following table shows a list of function names that are
true_positives, and the average number of instructions executed by
tests on this name.' as "Notice";
select
        func.name, func.id, func.file_id,
	count(*) as npass,
        sum(fio.instructions_executed)/count(*) as ave_insns_exec
    from fr_fio as fio
    join fr_functions as func on fio.func_id = func.id
    join fr_false_positives as neg on fio.func_id=neg.func1_id or fio.func_id=neg.func2_id
    where fio.status = 0
    group by func.name, func.id, func.file_id
    order by ave_insns_exec;



rollback;
