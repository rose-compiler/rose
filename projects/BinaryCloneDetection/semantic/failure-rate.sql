-- Reads a database containing semantic, syntactic, and combined clone pair tables and their call-graph variants
-- from runs where the same source code was compiled with multiple compilers or compiler switches, and tries to figure
-- out what the ideal answer should have been, and how each of those six tables compares to the ideal answer.
--
-- The input clone-pair tables are:
--     semantic_clone_pairs
--     syntactic_clone_pairs
--     combined_clone_pairs
--     semantic_cgclone_pairs
--     syntacitc_cgclone_pairs
--     combined_cgclone_pairs
--
-- Additional inputs:
--     combined_functions	-- table that associates semantic and syntactic function IDs
--     semantic_functions	-- information about individual functions
--
-- Outputs ("fr" prefix for "failure_rates")
--     fr_persistent_functions  -- functions whose name appears exactly once per specimen
--     fr_positive_pairs        -- pairs of functions that should be clones
--     fr_negative_pairs        -- pairs of functions that should not be clones
--     fr_fn_*                  -- false negatives per clone pairs input table
--     fr_fp_*                  -- false positives per clone pairs input table
--     fr_failures		-- the number of failures for each clone pairs input table and the failure rates
--
-- Assumptions:
--    1. If a function has no name, then it is excluded from consideration.
--    2. If a function was not analyzed by both semantic and syntactic clone detection, then it is excluded from consideration.
--    3. If a 

--    1. If a function name does not appear exactly once in every specimen then all functions with that name are excluded
--       from consideration.  We cannot say whether a pair of such functions is similar or dissimilar.
--    2. Of the remaining functions, any pair of functions is considered a clone pair if and only if both functions have
--       the same name.
--
--=============================================================================================================================




-------------------------------------------------------------------------------------------------------------------------------
-- Build the fr_persistent_functions table that lists those named functions whose names appear exactly once per specimen
begin transaction;

-- All functions that have names, and exist in all files, and were processed by both semantic and syntactic clone detection.
drop table if exists fr_all_functions;
create table fr_all_functions as
    select func.id as id, func.file_id as file_id, func.funcname as name
        from combined_functions as combined
        join semantic_functions as func on combined.semantic_id = func.id
        where combined.semantic_id is not null and combined.syntactic_id is not null and name <> '';

-- Functions whose names appear only once in a single specimen file.
drop table if exists fr_nondups;
create table fr_nondups as
    select id, file_id, name
    from fr_all_functions
    group by file_id, name
    having count(*) = 1;

-- Function names that appear in every specimen
drop table if exists fr_counts;
create table fr_namecounts as
    select func.name as name
        from fr_nondups as func
        group by func.name
        having count(*) = (select count(*) from (select distinct file_id from fr_nondups));

-- Functions by name that appear exactly once in each specimen
drop table if exists fr_persistent_functions;
create table fr_persistent_functions as
    select func.*
        from fr_nondups as func
        join fr_namecounts as counts on func.name = counts.name;


drop table fr_all_functions;
drop table fr_nondups;
drop table fr_namecounts;

commit;

-------------------------------------------------------------------------------------------------------------------------------
-- Build the fr_positive_pairs and fr_negative_pairs tables.

drop table if exists fr_positive_pairs;
create table fr_positive_pairs as
    select f1.id as func_id_1, f2.id as func_id_2
        from fr_persistent_functions as f1
        join fr_persistent_functions as f2 on f1.name = f2.name and f1.id < f2.id;

drop table if exists fr_negative_pairs;
create table fr_negative_pairs as
    select f1.id as func_id_1, f2.id as func_id_2
        from fr_persistent_functions as f1
        join fr_persistent_functions as f2 on f1.name <> f2.name and f1.id < f2.id;

-------------------------------------------------------------------------------------------------------------------------------
-- Build the false negatives (fn) and false positives (fp) tables for each of the clone pair input tables.
-- False negatives are those pairs which appear in the fr_positive_pairs table but were not in the input table.
-- False positives are those pairs from the input table which are also present in the fr_negatives_pairs.

drop table if exists fr_fn_semantic;
create table fr_fn_semantic as
    select * from fr_positive_pairs except select func_id_1, func_id_2 from semantic_clone_pairs;
drop table if exists fr_fp_semantic;
create table fr_fp_semantic as
    select * from fr_negative_pairs intersect select func_id_1, func_id_2 from semantic_clone_pairs;

drop table if exists fr_fn_syntactic;
create table fr_fn_syntactic as
    select * from fr_positive_pairs except select func_id_1, func_id_2 from syntactic_clone_pairs;
drop table if exists fr_fp_syntactic;
create table fr_fp_syntactic as
    select * from fr_negative_pairs intersect select func_id_1, func_id_2 from syntactic_clone_pairs;

drop table if exists fr_fn_combined;
create table fr_fn_combined as
    select * from fr_positive_pairs except select func_id_1, func_id_2 from combined_clone_pairs;
drop table if exists fr_fp_combined;
create table fr_fp_combined as
    select * from fr_negative_pairs intersect select func_id_1, func_id_2 from combined_clone_pairs;

drop table if exists fr_fn_semantic_cg;
create table fr_fn_semantic_cg as
    select * from fr_positive_pairs except select func_id_1, func_id_2 from semantic_cgclone_pairs;
drop table if exists fr_fp_semantic_cg;
create table fr_fp_semantic_cg as
    select * from fr_negative_pairs intersect select func_id_1, func_id_2 from semantic_cgclone_pairs;

drop table if exists fr_fn_syntactic_cg;
create table fr_fn_syntactic_cg as
    select * from fr_positive_pairs except select func_id_1, func_id_2 from syntactic_cgclone_pairs;
drop table if exists fr_fp_syntactic_cg;
create table fr_fp_syntactic_cg as
    select * from fr_negative_pairs intersect select func_id_1, func_id_2 from syntactic_cgclone_pairs;

drop table if exists fr_fn_combined_cg;
create table fr_fn_combined_cg as
    select * from fr_positive_pairs except select func_id_1, func_id_2 from combined_cgclone_pairs;
drop table if exists fr_fp_combined_cg;
create table fr_fp_combined_cg as
    select * from fr_negative_pairs intersect select func_id_1, func_id_2 from combined_cgclone_pairs;

-------------------------------------------------------------------------------------------------------------------------------
-- Count the false positives and false negatives for each input pairs table

drop table if exists fr_failures;
create table fr_failures (
    name varchar(64),                   -- name of the table
    false_negatives integer,            -- number of false negatives missing from that table
    false_positives integer,            -- number of false positives present in that table
    false_negatives_rate double precision, -- ratio of false negatives to expected positives
    false_positives_rate double precision  -- ratio of false positives to expected negatives
);

insert into fr_failures (name, false_negatives, false_positives) values (
       'semantic',
       (select count(*) from fr_fn_semantic),
       (select count(*) from fr_fp_semantic));
insert into fr_failures (name, false_negatives, false_positives) values (
       'syntactic',
       (select count(*) from fr_fn_syntactic),
       (select count(*) from fr_fp_syntactic));
insert into fr_failures (name, false_negatives, false_positives) values (
       'combined',
       (select count(*) from fr_fn_combined),
       (select count(*) from fr_fp_combined));
insert into fr_failures (name, false_negatives, false_positives) values (
       'semantic_cg',
       (select count(*) from fr_fn_semantic_cg),
       (select count(*) from fr_fp_semantic_cg));
insert into fr_failures (name, false_negatives, false_positives) values (
       'syntactic_cg',
       (select count(*) from fr_fn_syntactic_cg),
       (select count(*) from fr_fp_syntactic_cg));
insert into fr_failures (name, false_negatives, false_positives) values (
       'combined_cg',
       (select count(*) from fr_fn_combined_cg),
       (select count(*) from fr_fp_combined_cg));

-- Answers
select
    (select count(*) from fr_positive_pairs) as positives,
    (select count(*) from fr_negative_pairs) as negatives;

-- Update the ratio columns
select '' as ""; --blank line
update fr_failures
    set false_negatives_rate = (1.0*false_negatives) / (select count(*) from fr_positive_pairs);
update fr_failures
    set false_positives_rate = (1.0*false_positives) / (select count(*) from fr_negative_pairs);

select * from fr_failures;

-- One can easily compute true positive and negative rates from these tables
select '' as ""; --blank line
select
	name,
	(select count(*) from fr_positive_pairs) - false_negatives as true_positives,
	1.0 - false_negatives_rate as true_positives_rate,
	(select count(*) from fr_negative_pairs) - false_positives as true_negatives,
	1.0 - false_positives_rate as true_negatives_rate
    from fr_failures;

