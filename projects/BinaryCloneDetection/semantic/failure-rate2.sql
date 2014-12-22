-- This analysis calculates false positive rate, false negative rate, recall, and precision for functions whose
-- names appear across more than one specimen (but not more than once per specimen) and which pass all tests.

-------------------------------------------------------------------------------------------------------------------------------
-- Some settings for this analysis
-------------------------------------------------------------------------------------------------------------------------------

drop table if exists debug_settings;
create table debug_settings (
    npasses integer,					-- number of times test must have passed
    ret_threshold double precision,			-- whether a function returns a value [0..1]
    sim_threshold double precision			-- threshold of similarity [0..1]
);

insert into debug_settings (npasses, ret_threshold, sim_threshold) values (10, 0.25, 0.70);

-------------------------------------------------------------------------------------------------------------------------------
-- Clean-up. Cut and paste this block if you want to clean up all the tables. They're also dropped before each is created
-- so it's easier to cut-n-past just some queries.
-------------------------------------------------------------------------------------------------------------------------------

drop table if exists debug_posneg;
drop table if exists debug_en;
drop table if exists debug_ep;
drop table if exists debug_funcsim;
drop table if exists debug_functions;
drop table if exists debug_freturns;
drop table if exists debug_foutputs;
drop table if exists debug_outputvalues;
drop table if exists debug_fpass;
drop table if exists debug_fcardtotal;
drop table if exists debug_fcardfile;


-------------------------------------------------------------------------------------------------------------------------------
-- Calculate some function properties
-------------------------------------------------------------------------------------------------------------------------------

-- Extended output groups (same as semantic_outputvalues but include a function ID)
drop table if exists debug_outputvalues;
create table debug_outputvalues as
    select fio.func_id, ogroup.*
	from semantic_fio as fio
	join semantic_outputvalues as ogroup on fio.ogroup_id = ogroup.hashkey;

-- Functions that produce at least one output value (not counting possible return value)
drop table if exists debug_foutputs;
create table debug_foutputs as
    select func_id
	from debug_outputvalues
	where vtype = 'V'
	group by func_id
	having count(*) > 0;

-- Functions with high pass rate
drop table if exists debug_fpass;
create table debug_fpass as
    select func_id
	from semantic_fio
	where status = 0
	group by func_id
	having count(*) >= (select npasses from debug_settings);

-- Functions that return a value
drop table if exists debug_freturns;
create table debug_freturns as
    select func_id
        from semantic_aggprops as prop
        where prop.retprob >= (select ret_threshold from debug_settings);

-- Functions that pass tests and either produce output or return a value
drop table if exists debug_fgoodtest;
create table debug_fgoodtest as
    (
        select * from debug_fpass
    ) intersect (
        select func_id from debug_foutputs
        union
        select func_id from debug_freturns
    );

-- Number of times function name appears in total (considering only functions in debug_fgoodtest)
drop table if exists debug_fcardtotal;
create table debug_fcardtotal as
    select func.name, count(*) as card
        from debug_fgoodtest as f1
	join semantic_functions as func on f1.func_id = func.id
	group by func.name;

-- Number of files in which each function name appears (considering only functions in debug_fgoodtest)
drop table if exists debug_fcardfile;
create table debug_fcardfile as
    select name, count(*) as card
        from (
	    select distinct func.name, func.specimen_id 
                from debug_fgoodtest as f1
                join semantic_functions as func on f1.func_id = func.id
            ) as q1
	group by q1.name;

-- Function names that appear at least twice in total, but not more than once per file (only functions in debug_fgoodtest)
drop table if exists debug_ftwice;
create table debug_ftwice as
    select ntotal.name
	from debug_fcardtotal as ntotal
        join debug_fcardfile as nfiles on nfiles.name = ntotal.name and nfiles.card = ntotal.card
        where ntotal.card >= 2;

-------------------------------------------------------------------------------------------------------------------------------
-- Interesting functions
-------------------------------------------------------------------------------------------------------------------------------

-- Functions that pass tests and either produce output or return a value and have names from the ftwice table.
drop table if exists debug_functions;
create table debug_functions as
    select func.*
       from debug_fgoodtest as goodtest
       join semantic_functions as func on goodtest.func_id = func.id
       join debug_ftwice as twice on func.name = twice.name;

-------------------------------------------------------------------------------------------------------------------------------
-- Prune funcsim table down to only the functions in which we're interested.
-------------------------------------------------------------------------------------------------------------------------------

drop table if exists debug_funcsim;
create table debug_funcsim as
    select funcsim.*
        from semantic_funcsim as funcsim
	join debug_functions as f1 on funcsim.func1_id = f1.id
	join debug_functions as f2 on funcsim.func2_id = f2.id;

-- debug: these are the similarity pairs for functions in which we're interested.
select
    (select count(*) from debug_funcsim) as "size of debug_funcsim",
    (select count(*) from semantic_funcsim) as "size of semantic_funcsim",
    100.0 * (select count(*) from debug_funcsim) / (select count(*) from semantic_funcsim) as "funcsim percent";


-------------------------------------------------------------------------------------------------------------------------------
-- Calculate the various positive/negative sets
-------------------------------------------------------------------------------------------------------------------------------

-- Expected positives: those pairs of functions that have the same name.
-- False negatives are those rows where the similarity is too low
drop table if exists debug_ep;
create table debug_ep as
    select sim.func1_id, sim.func2_id, sim.similarity
        from debug_funcsim as sim
	join semantic_functions as func1 on sim.func1_id = func1.id
	join semantic_functions as func2 on sim.func2_id = func2.id
	where func1.name = func2.name;

-- Expected negatives: those pairs of functions that have different names
-- False positives are those rows where the similarity is too high
drop table if exists debug_en;
create table debug_en as
    select sim.func1_id, sim.func2_id, sim.similarity
        from debug_funcsim as sim
	join semantic_functions as func1 on sim.func1_id = func1.id
	join semantic_functions as func2 on sim.func2_id = func2.id
	where func1.name <> func2.name;

-------------------------------------------------------------------------------------------------------------------------------
-- Statistics
-------------------------------------------------------------------------------------------------------------------------------


drop table if exists debug_posneg;
create table debug_posneg as
    select
	-- Number of expected positives -- those pairs which the oracle says should be similar
	(select count(*) from debug_ep) as num_ep,

        -- Number of true positives -- pairs we correctly identified as similar
	(select count(*) from debug_ep where similarity >= (select sim_threshold from debug_settings)) as num_tp,

	-- Number of false negatives -- pairs of expected similarity that we failed to detect
	(select count(*) from debug_ep where similarity < (select sim_threshold from debug_settings)) as num_fn,

	(select null)::varchar(1) as " ",

	-- Number of expected negatives -- those pairs which the oracle says should not be similar
	(select count(*) from debug_en) as num_en,

	-- Number of true negatives -- pairs we correctly identified as being not similar
	(select count(*) from debug_en where similarity < (select sim_threshold from debug_settings)) as num_tn,

	-- Number of false positives -- pairs we unexpectedly detected as similar
	(select count(*) from debug_en where similarity >= (select sim_threshold from debug_settings)) as num_fp,

	(select null)::varchar(1) as "  ",

	-- Number of similarities detected (true positives plus false positives)
	((select count(*) from debug_ep where similarity >= (select sim_threshold from debug_settings)) +
	 (select count(*) from debug_en where similarity >= (select sim_threshold from debug_settings))) as num_dp,

	-- Number of non-similarities detected (true negatives plus false negatives)
	((select count(*) from debug_ep where similarity < (select sim_threshold from debug_settings)) +
	 (select count(*) from debug_en where similarity < (select sim_threshold from debug_settings))) as num_dn;

select * from debug_posneg;

select
    (100.0 * (select num_fp from debug_posneg) / (select num_en from debug_posneg)) as "False positive percent",
    (100.0 * (select num_fn from debug_posneg) / (select num_ep from debug_posneg)) as "False negative percent";

select
    -- Recall is |tp| / (|tp|+|fn|) where the denominator simplifies to |ep|
    (100.0 * (select num_tp from debug_posneg) / (select num_ep from debug_posneg)) as "Recall percent",

    -- Precision is |tp| / (|tp|+|fp|) where the denominator simplifies to |dp|
    (100.0 * (select num_tp from debug_posneg) / (select num_dp from debug_posneg)) as "Precision percent";

-------------------------------------------------------------------------------------------------------------------------------
-- Show some abnormal results
-------------------------------------------------------------------------------------------------------------------------------

-- Show the file names so we don't need such wide columns in the other tables. Limit them only to the files that
-- contain functions in which we're interested and show the number of such functions
select file.id, file.name, file.digest, count(*) as nfunctions
    from debug_functions as func
    join semantic_files as file on func.specimen_id = file.id
    group by file.id, file.name, file.digest
    order by file.id;

-- Some false negatives
select func1.name,
       func1.specimen_id as specimen1_id, func1.file_id as file1_id, func1.id as func1_id,
       func2.specimen_id as specimen2_id, func2.file_id as file2_id, func2.id as func2_id,
       debug_ep.similarity
    from debug_ep
    join debug_functions as func1 on debug_ep.func1_id = func1.id
    join debug_functions as func2 on debug_ep.func2_id = func2.id
    where debug_ep.similarity < (select sim_threshold from debug_settings)
    order by func1.id, func2.id
    limit 20;

-- Some false positives
select func1.name, func1.specimen_id, func1.file_id as file1_id, func1.id as func1_id,
       func2.name, func2.specimen_id, func2.file_id as file2_id, func2.id as func2_id,
       debug_en.similarity
    from debug_en
    join debug_functions as func1 on debug_en.func1_id = func1.id
    join debug_functions as func2 on debug_en.func2_id = func2.id
    where debug_en.similarity >= (select sim_threshold from debug_settings)
    order by func1.id, func2.id
    limit 20;
