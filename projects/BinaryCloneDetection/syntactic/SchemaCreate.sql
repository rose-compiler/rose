-- Schema for syntactic binary clone detection
-- SQL contained herein should be written portably for either SQLite3 or PostgreSQL
-- See also, SchemaDrop.sql

create table run_parameters (
       window_size integer,                     -- the --windowSize argument for createVectorsBinary
       stride integer,                          -- the --stride argument for createVectorsBinary
       similarity_threshold double precision,   -- the argument for the -t switch of findClones
       min_coverage double precision,           -- minimum coverage ratio for clone pairs
       min_func_ninsns integer                  -- minimum function size measured in instructions
);

create table detection_parameters (     
       similarity_threshold float,
       false_negative_rate float
);

create table vector_generator_timing (
        file text,
        total_wallclock float,
        total_usertime float,
        total_systime float,
        vecgen_wallclock float,
        vecgen_usertime float,
        vecgen_systime float
);

create table timing (
        property_name text,
        total_wallclock float,
        total_usertime float,
        total_systime float,
        wallclock float,
        usertime float,
        systime float
);

create table results (
        row_number integer primary key,
        edit_distance integer,
        false_positive_rate float
);

create table function_statistics (
        function_id integer references semantic_functions(id),
        num_instructions integer
);

create table vectors (
        id integer primary key,                 -- once called "row_number"
        function_id integer references semantic_functions(id),
        index_within_function integer,          -- zero-origin index of starting instruction within function
        line integer,                           -- starting virtual address of first instruction
        last_insn_va integer,                   -- virtual address of last instruction (not very useful; use "size" instead)
        size integer,                           -- total size of instructions in bytes, non-overlapping
        sum_of_counts integer,
        counts_b64 text,                        -- binary blob (not sure what), base64 encoded
        instr_seq_b64 text                      -- binary MD5 sum of vector, base64 encoded
);

-- index_within_function values are only unique within a file and function
create index vectors_sum_of_counts_index on vectors(sum_of_counts);

create table clusters (
       id integer primary key,                  -- used to be called "row_num"; 1-origin
       cluster integer,
       function_id integer references semantic_functions(id),
       index_within_function integer,
       vectors_row integer,
       dist integer
);

create table postprocessed_clusters (
       row_number SERIAL primary key,
       cluster integer,
       function_id integer references semantic_functions(id),
       index_within_function integer,
       vectors_row integer,
       dist integer
);

create table function_coverage (
       function_id integer references semantic_functions(id),
       num_instructions_covered_not_postprocessed integer,
       fraction_instructions_covered_not_postprocessed float,
       num_instructions_covered_postprocessed integer,
       fraction_instructions_covered_postprocessed float
);

create table total_coverage (
       num_instructions_covered_not_postprocessed integer,
       fraction_instructions_covered_not_postprocessed float,
       num_instructions_covered_postprocessed integer,
       fraction_instructions_covered_postprocessed float
);

create index clusters_by_function on clusters(function_id);
create index postprocessed_clusters_by_function on postprocessed_clusters(function_id);
create index function_statistics_by_function on function_statistics(function_id);
create index clusters_index_by_cluster on clusters(cluster);
create index postprocessed_clusters_index_by_cluster on postprocessed_clusters(cluster);

-- Moved here from lshCloneDetection.C (but undocumented there originally)
create table group_timing (
       groupLow integer,
       groupHigh integer,
       num_elements integer,
       k integer,
       l integer,
       total_wallclock float,
       total_usertime float,
       total_systime float,
       wallclock float,
       usertime float,
       systime float
);

create table cluster_pairs (
        cluster_id integer,
        function_id_1 integer references semantic_functions(id),
        function_id_2 integer references semantic_functions(id),
        nbytes_1 integer,     -- number of function_id_1 bytes that are similar to function_id_2
        nbytes_2 integer,     -- number of function_id_2 bytes that are similar to function_id_1
        ratio_1 real,         -- ratio nbytes_1 to total size of function_id_1
        ratio_2 real          -- ratio nbytes_2 to total size of function_id_2
);
