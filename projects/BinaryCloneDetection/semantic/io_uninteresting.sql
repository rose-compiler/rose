-- Get a list of functions that would be uninteresting for calculating function similarity
--
-- From Andreas:
--     > Uninteresting functions are functions:
--     >   1) where no input-output pair consumer more than 10 input values in total
--     >   2) the set of output values minus the set of input values contains only one value or no values at all.
--     >        - e.g a void function that returns no values is considered uninteresting.
--     >   3) if the set of output values minus the set of input values contains one value, the value is always the same
--     >      over all input-output pairs.
--     >        - e.g a function is uninteresting if it always returns 1.
--     > 
--
-- More formally:
--     Given:
--         F is a function,
--         T is a test of F,
--         InList(T) is the list of input values consumed by T
--         InSet(T)  is the set  of input values consumed by T
--         OutSet(T) is the set of output values produced by T
--         OutSet(F) is the set of output values produced over all T
--     Then:
--         F is uninteresting if
--           (1)      |InList(T)| < 11 for all T AND
--              (2.1)     |OutSet(T) - InSet(T)| = 0 for all T OR
--              (2.2)     |OutSet(F)| < 2 for all T
--



drop table if exists io_uninteresting;
drop table if exists function_oni_sizes;
drop table if exists test_oni_sizes;
drop table if exists outputs_not_inputs;
drop table if exists output_sets;
drop table if exists input_sets;
drop table if exists function_io_sizes;
drop table if exists test_io_sizes;

-- Number of inputs consumed and outputs produced for each test
create temporary table test_io_sizes as
    select fio.func_id,
           fio.igroup_id,
           fio.arguments_consumed + fio.locals_consumed + fio.globals_consumed + fio.functions_consumed +
               fio.pointers_consumed + fio.integers_consumed as input_list_size,
           count(*) as output_list_size,
           fio.ogroup_id
    from semantic_fio as fio
    left join semantic_outputvalues as output
        on fio.ogroup_id = output.hashkey and output.vtype in ('V', 'R')
    group by fio.func_id, fio.igroup_id, input_list_size, fio.ogroup_id;

-- Number of inputs consumed and outputs produced for each function
create temporary table function_io_sizes as
    select func_id,
           count(*) as ntests,
           min(input_list_size) as min_input_list_size,
           max(input_list_size) as max_input_list_size,
           min(output_list_size) as min_output_list_size,
           max(output_list_size) as max_output_list_size
    from test_io_sizes
    group by func_id;

-- Convert semantic_fio_inputs and semantic_outputvalues, which are lists of values, into sets and normalize the
-- projections so we can do set-difference.  We only have to do this for tests that consumed less than 11 input values
-- since case (1) is conjunctive with cases (2.1) and (2.2).
create index io_idx1 on semantic_fio_inputs(func_id, igroup_id);
create temporary table input_sets as
    select distinct test.func_id, test.igroup_id, input.val as val
    from test_io_sizes as test
    join semantic_fio_inputs as input
        on test.func_id = input.func_id and test.igroup_id = input.igroup_id and test.input_list_size < 11;
drop index io_idx1;

create index io_idx2 on semantic_outputvalues(hashkey);
create temporary table output_sets as
    select distinct test.func_id, test.igroup_id, output.val as val
    from test_io_sizes as test
    join semantic_outputvalues as output
        on test.ogroup_id = output.hashkey and test.input_list_size < 11
    where output.vtype in ('V', 'R');
drop index io_idx2;

-- Output values that are not input values ("ONI")
create temporary table outputs_not_inputs as
    (select * from output_sets)
    except
    (select * from input_sets);

-- The size of the outputs-not-inputs sets per test.  We also store the min and max ONI values for each set since this
create temporary table test_oni_sizes as
    select func_id,
           igroup_id,
           count(*) as size,
           min(val) as min_oni_val,
           max(val) as max_oni_val
    from outputs_not_inputs
    group by func_id, igroup_id;

-- The size of outputs-not-inputs sets per function, and their min/max values across all tests per function.
create temporary table function_oni_sizes as
    select func_id,
           min(size) as min_oni_size,
           max(size) as max_oni_size,
           min(min_oni_val) as min_oni_val,
           max(max_oni_val) as max_oni_val
    from test_oni_sizes
    group by func_id;

-- Uninteresting functions
create table io_uninteresting as
    select io.func_id,
           -- the rest of the projection is only for debugging
           io.max_input_list_size,
           case
               when oni.max_oni_size = 0 then 'all are empty'
               when oni.max_oni_size = 1 then 'all are singleton or empty'
               else 'at least one non-singleton'
               end as out_minus_in_sizes
    from function_io_sizes as io
    join function_oni_sizes as oni on io.func_id = oni.func_id
    where
        -- case (1)
        io.max_input_list_size < 11 and (
	    -- case (2.1)
	    oni.max_oni_size = 0 or
	    -- case (2.2)
	    oni.min_oni_val = oni.max_oni_val
        );
