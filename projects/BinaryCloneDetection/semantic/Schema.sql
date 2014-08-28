-- Schema for binary clone detection output
-- SQL contained herein should be written portably for either SQLite3 or PostgreSQL

-- Clean up. Tables need to be dropped in the opposite order they're created.
drop table if exists semantic_funcsim;
drop table if exists semantic_aggprops;
drop table if exists semantic_funcpartials;
drop table if exists semantic_fio_calls;
drop table if exists semantic_fio_coverage;
drop table if exists semantic_fio_inputs;
drop table if exists semantic_fio_trace;
drop table if exists semantic_fio_events;
drop table if exists api_call_similarity;
drop table if exists semantic_fio;
drop table if exists semantic_sources;
drop table if exists semantic_instructions;
drop table if exists semantic_cg;
drop table if exists semantic_rg;
drop table if exists semantic_binaries;
drop table if exists semantic_functions;
drop table if exists semantic_specfiles;
drop table if exists semantic_files;
drop table if exists semantic_faults;
drop table if exists semantic_outputvalues;
drop table if exists semantic_inputvalues;
drop table if exists semantic_input_queues;
drop table if exists semantic_history;
drop table if exists equivalent_classes;

-- A history of the commands that were run to produce this database, excluding SQL run by the user.
create table semantic_history (
       hashkey bigint unique primary key,       -- nonsequential ID number to identify the command
       begin_time bigint,                       -- Approx time that command started (Unix time, seconds since epoch)
       end_time bigint,                         -- Approx time command ended, or zero if command terminated with failure
       notation text,                           -- Notation describing what action was performed
       command text                             -- Command-line that was issued
);

-- Table of input queue names.
create table semantic_input_queues (
       id integer primary key,                  -- Queue ID or a special value (see InputGroupName)
       name varchar(16),                        -- Short name
       description text                         -- Single-line, full description
);

-- This table encodes the set of all input groups.  Each input group consists of an input queue which contains zero or
-- more values.  This table uses special values for queue_id and pos to encode some additional information:
--   When:
--     queue_id = -1 the val column is the collection_id, otherwise collection_id == igroup_id
--     pos == -1     the queue is an infinite sequence where the val column contains the padding value.
--     pos == -2     the val column is a queue number to which consumption requests are redirected.
create table semantic_inputvalues (
       igroup_id integer,                       -- ID for the input group; non-unique
       queue_id integer references semantic_input_queues(id),
       pos integer,                             -- position of this value within its queue (some positions are special)
       val bigint,                              -- the 64-bit unsigned value
       cmd bigint references semantic_history(hashkey) -- command that created this row
);

-- An output value is a value produced as output when a specimen function is "executed" during fuzz testing.  Each execution of
-- a function produces zero or more output values which are grouped together into a output group.  The values in an output
-- group have a position within the group, although the analysis can be written to treat output groups as either vectors or
-- sets.
create table semantic_outputvalues (
       hashkey bigint,                          -- output set to which this value belongs; non-unique
       vtype character,                         -- V=>value, R=retval, F=>fault, C=>function call, S=>syscall
       pos integer,                             -- position of value within its output group and vtype pair
       val bigint                               -- value, fault ID, or function ID depending on vtype
       -- command ID is not stored here because this table is already HUGE! One has a reasonably high chance of
       -- figuring out the command ID by looking at the semantic_fio table.
);

-- Some output values indicate special situations described by this table. See 00-create-schema.C for how this table
-- is populated.
create table semantic_faults (
       id integer primary key,                  -- the special integer value
       name varchar(16),                        -- short identifying name
       description text                         -- full description
);

-- List of files. The other tables store file IDs rather than file names.
create table semantic_files (
       id integer primary key,                  -- unique positive file ID
       name text,                               -- file name
       digest varchar(40),                      -- SHA1 digest if the file is stored in the semantic_binaries table
       ast varchar(40)                          -- SHA1 digest of the binary AST if one is stored in the database
);

-- Associatations between a specimen and its dynamic libraries.  A "specimen" is a file that appeared as an argument
-- to the 11-add-functions tool; the file_id is any other file (binary or otherwise) that's used by the specimen.
create table semantic_specfiles (
       specimen_id integer references semantic_files(id),
       file_id integer references semantic_files(id)
);

-- A function is the unit of code that is fuzz tested.
create table semantic_functions (
       id integer primary key,                  -- unique function ID
       entry_va bigint,                         -- unique starting virtual address within the binary specimen
       name text,                               -- name of function if known
       file_id integer references semantic_files(id), -- binary file in which function exists, specimen or shared library
       specimen_id integer references semantic_files(id), -- file specified in frontend() call
       ninsns integer,                          -- number of instructions in function
       isize integer,                           -- size of function instructions in bytes, non-overlapping
       dsize integer,                           -- size of function data in bytes, non-overlapping
       size integer,                            -- total size of function, non-overlapping
       callsites integer,                       -- number of places this function is called
       retvals_used integer,                    -- number of call sites where a return value is used
       digest varchar(40),                      -- SHA1 hash of the function's instructions and static data
       counts_b64 text,                         -- binary blob of signature vector, base64 encoded
       cmd bigint references semantic_history(hashkey) -- command that created this row
);

-- Function call graph
create table semantic_cg (
       caller integer references semantic_functions(id),
       callee integer references semantic_functions(id),
       file_id integer references semantic_files(id), -- File from which this call originates (used during update)
       cmd bigint references semantic_history(hashkey) -- Command that created this row
);

-- Equivalence classes
create table equivalent_classes (
      func_id integer references semantic_functions(id),
      equivalent_func_id integer references semantic_functions(id),
      cmd bigint references semantic_history(hashkey) -- Command that created this row
);

-- Function reachability graph
create table semantic_rg (
       caller integer references semantic_functions(id),
       callee integer references semantic_functions(id),
       file_id integer references semantic_files(id) -- File from which this call originates (used during update)
       --cmd bigint references semantic_history(hashkey) -- Command that created this row
);



-- This is the list of instructions for each function.  Functions need not be contiguous in memory, and two instructions
-- might overlap, which is why we have to store each instruction individually.  Every instruction belongs to exactly one
-- function.
create table semantic_instructions (
       address bigint,                          -- virtual address for start of instruction
       size integer,                            -- size of instruction in bytes
       assembly text,                           -- unparsed instruction including hexadecimal address
       func_id integer references semantic_functions(id), --function to which this instruction belongs
       position integer,                        -- zero-origin index of instruction within function
       src_file_id integer,                     -- source code file that produced this instruction, or -1
       src_line integer,                        -- source line number that produced this instruction, or -1
       cmd bigint references semantic_history(hashkey) -- command that created this row
);

-- Table that holds base-64 encoded binary information.  Large binary information is split into smaller, more manageable
-- chunks that are reassembled according to the 'pos' column.  The key used to look up data is a 20-byte (40 hexadecimal
-- characters) SHA1 computed across all the chunks.
create table semantic_binaries (
       hashkey character(40),                   -- SHA1 hash across all the chunks with this hashkey
       cmd bigint references semantic_history(hashkey), -- command that created this row
       pos integer,                             -- chunk number for content
       chunk text                               -- base-64 encoding of the binary data
);

-- List of source code.
create table semantic_sources (
       cmd bigint references semantic_history(hashkey), -- command that created this row
       file_id integer references semantic_files(id),
       linenum integer,                         -- one-origin line number within file
       line text                                -- one line from a source file
);

-- Function input/output. One of these is produced each time we fuzz-test a function.
-- Note: The *_consumed counters indicate the number of values consumed for a particular reason, not necessarily the
-- number of values consumed on a particular queue.  For instance, if arguments_consumed is 5 it means that the test
-- requested initializations for five function arguments even if the values for those arguments ultimately came from some
-- other queue or memhash.
create table semantic_fio (
       func_id integer references semantic_functions(id),
       igroup_id integer,                       -- references semantic_inputvalues.id
       arguments_consumed integer,              -- number of inputs consumed for arguments
       locals_consumed integer,                 -- number of inputs consumed for local variables
       globals_consumed integer,                -- number of inputs consumed for globals
       functions_consumed integer,              -- number of inputs consumed for return values of black-box (skipped) functions
       pointers_consumed integer,               -- number of inputs consumed for pointers
       integers_consumed integer,               -- number of inputs consumed for other reasons
       instructions_executed integer,           -- number of instructions executed by this test
       ogroup_id bigint,                        -- output produced by this function, semantic_outputvalues.hashkey
       counts_b64 text,                         -- binary blob (not sure what), base64 encoded
       syntactic_ninsns integer,                -- number of instructions covered by the syntactic signature
       instr_seq_b64 text,                      -- binary MD5 sum of vector, base64 encoded
       status integer references semantic_faults(id), -- exit status of the test
       elapsed_time double precision,           -- number of seconds elapsed excluding ptr analysis
       cpu_time double precision,               -- number of CPU seconds used excluding ptr analysis
       cmd bigint references semantic_history(hashkey) -- command that created this row
);

-- This table describes the kinds of events that can happen while a function is running. The events themselves are
-- stored in semantic_fio_trace. See 00-create-schema.C for how this table is populated.
create table semantic_fio_events (
       id integer primary key,
       name varchar(16),                        -- short name of event
       description text                         -- full event description
);

-- This table contains trace info per test.  Not all tests generate trace info, and the contents of this table is
-- mostly for debugging what happened during a test.
create table semantic_fio_trace (
       func_id integer, -- (need for speed) references semantic_functions(id),
       igroup_id integer,                       -- references semantic_inputvalues.id
       pos integer,                             -- sequence number of this event within this test
       addr bigint,                             -- specimen virtual address where event occurred
       event_id integer, --(need for speed) references semantic_fio_events(id),
       minor integer,                           -- event minor number (usually zero)
       val bigint                               -- event value, interpretation depends on event_id
);

-- Input values actually consumed by each test.
create table semantic_fio_inputs (
       func_id integer,                         -- references semantic_functions(id) [commented out for speed]
       igroup_id integer,                       -- references semantic_inputvalues(id) [commented out for speed]
       request_queue_id integer references semantic_input_queues(id), -- queue whose value was requested
       actual_queue_id integer references semantic_input_queues(id),  -- queue whose value was consumed after redirecting
       pos integer,                             -- sequence number indicates the order that input values were consumed
       val bigint                               -- the actual input value
);

-- Information about instructions that were executed for each test.  This table is only populated when coverage capability is
-- enabled in the 25-run-tests tool. The (func_id,igroup_id) pair identifies the test for which the coverage info is computed
-- and the pair can be used to look up test results in the semantic_fio table.  The (func_id,insn_va) pair can be used to find
-- info about the instruction in the semantic_instructions table.
create table semantic_fio_coverage (
        func_id integer,                        -- references semantic_functions(id) [commented out for speed]
        igroup_id integer,                      -- references semantic_inputvalues(id)
        address bigint,                         -- the starting address of the instruction; cf. semantic_instructions
        first_seen_pos integer,                 -- sequence number for when this instruction was first seen in this test
        nhits integer                           -- number of times this address was executed (regardless of execution order)
);

-- Function calls that occur while a test runs.  This table is only populated if the run-time function call tracing capability
-- is enabled in the 25-run-tests tool.  Similar info might be available in the semantic_fio_trace table.
create table semantic_fio_calls (
        func_id integer,                        -- references semantic_functions(id) [commented out for speed]
        igroup_id integer,                      -- references semantic_inputvalues(id)
        caller_id integer,                      -- ID of the function making the call
        callee_id integer,                      -- ID of the function being called
        pos integer,                            -- Relative position of this call w.r.t. the other calls of this test
        ncalls integer                          -- Number of consecutive calls without intervening calls to/from other functions
);

-- Results of various kinds of function analyses.  A function might be represented more than once in this table because
-- some of these analyses occur when the function is called by another function rather than when the function is tested
-- by 25-run-tests.
create table semantic_funcpartials (
       func_id integer references semantic_functions(id),
       ncalls integer,                          -- Number of times this function was called by another function
       nretused integer,                        -- Number of ncalls where a return value was used in the caller
       ntests integer,                          -- Number of times this function was tested
       nvoids integer                           -- Number of ntests where the function did not write to EAX
);

-- Function properties aggregated over multiples tests. This table holds per-function properties whose values are computed
-- from multiple runs (rows of semantics_fio), usually depending on rows from semantic_funcpartials.
create table semantic_aggprops (
       func_id integer references semantic_functions(id),
       retprob double precision                 -- probability that this function returns a value
);

-- Function similarity--how similar are pairs of functions.  The relation_id allows us to store multiple function similarity
-- relationships where all the similarity values for a given relationship were calculated the same way.  E.g., relationship
-- #0 could be computed as the maximum output group similarity while relationship #1 could be the average.
create table semantic_funcsim (
       func1_id integer references semantic_functions(id),
       func2_id integer references semantic_functions(id), -- func1_id < func2_id
       similarity double precision,             -- a value between 0 and 1, with one being equality
       ncompares integer,                       -- number of output groups compared to reach this value
       maxcompares integer,                     -- potential number of comparisons possible (ncompares is a random sample)
       relation_id int,                         -- Identying number for this set of function similarity values (default is zero)
       hamming_d integer,                       -- Hamming distance
       euclidean_d double precision,            -- Euclidean distance
       euclidean_d_ratio double precision,      -- Euclidean distance divided by the total number of instructions in both functions
       combined_d integer,                      -- Combined distance
       path_ave_hamming_d integer,              -- Path sensistive average hamming distance
       path_min_hamming_d integer,              -- Path sensistive minimum hamming distance
       path_max_hamming_d integer,              -- Path sensistive maximum hamming distance
       path_ave_euclidean_d double precision,   -- Path sensistive average euclidean distance
       path_min_euclidean_d double precision,   -- Path sensistive minimum euclidean distance
       path_max_euclidean_d double precision,   -- Path sensistive maximum euclidean distance
       path_ave_euclidean_d_ratio double precision,   -- Path sensistive average euclidean distance ratio
       path_min_euclidean_d_ratio double precision,   -- Path sensistive minimum euclidean distance ratio
       path_max_euclidean_d_ratio double precision,   -- Path sensistive maximum euclidean distance ratio
       cmd bigint references semantic_history(hashkey) -- command that set the precision on this row
);

-- API Call similarity - how similar are pairs of functions.
create table api_call_similarity (
    func1_id integer references semantic_functions(id),
    func2_id integer references semantic_functions(id), -- func1_id < func2_id
    max_similarity double precision,             -- a value between 0 and 1, with one being equality
    min_similarity double precision,             -- a value between 0 and 1, with one being equality
    ave_similarity double precision,             -- a value between 0 and 1, with one being equality
    cg_similarity  double precision              -- static call graph similarity
  
    );
