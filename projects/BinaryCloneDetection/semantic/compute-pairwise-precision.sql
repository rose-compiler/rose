-- Computes statistics about false negatives in a database populated with two or more specimens compiled in different ways.

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

-- Files that are binary specimens; i.e., not shared libraries, etc.
create table fr_specimens as
    select distinct specimen_id as id
    from semantic_functions;

-- Function names that are present exactly once in each specimen
-- And which contain a certain number of instructions                                       
-- And for which no test tried to consume too many inputs (status<>911000007)
-- And which come from the specimen, not from a dynamic library
create table fr_funcnames as
    select func1.name as name
        from fr_specimens as file
        join semantic_functions as func1 on func1.file_id = file.id
        left join semantic_functions as func2
            on func1.id<>func2.id and func1.name <> '' and func1.name=func2.name and func1.file_id=func2.file_id
        where func2.id is null and func1.ninsns >= ( select min_insns from fr_settings )                             
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
        where test1.status = 0 and test2.status = 0 AND f1.ninsns >= ( select min_insns from fr_settings ) AND f2.ninsns >= ( select min_insns from fr_settings )

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
  AND api_sem.ave_similarity >= (select path_similarity_threshold from fr_settings) 
  AND api_sem.cg_similarity >=  (select cg_similarity_threshold   from fr_settings)
  --AND sim.path_max_euclidean_d_ratio - sim.path_min_euclidean_d_ratio < 0.5
  --AND sim.path_max_euclidean_d_ratio < 3.0
  except(
      select func1_id, func2_id from fr_ignored_function_pairs 
  )
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
        ( (select 100.0*true_positives from fr_results limit 1) / (0.001+( ( select true_positives from fr_results  limit 1) + (select false_positives from fr_results  limit 1 )))) as precision,
        ( 2 *
          ( (select 100.0*true_positives from fr_results limit 1) / (0.001+( ( select true_positives from fr_results  limit 1 ) + (select false_negatives from fr_results limit 1) ))  )
          * ( (select 100.0*true_positives from fr_results limit 1) / (0.001+( ( select true_positives from fr_results  limit 1) + (select false_positives from fr_results  limit 1 ))))
          / (0.001+
            ( (select 100.0*true_positives from fr_results limit 1) / (0.001+( ( select true_positives from fr_results  limit 1 ) + (select false_negatives from fr_results limit 1) ))  )
            + ( (select 100.0*true_positives from fr_results limit 1) / (0.001+( ( select true_positives from fr_results  limit 1) + (select false_positives from fr_results  limit 1 ))))
            )
         ) as fscore
;

