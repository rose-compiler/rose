-- Schema for binary clone detection output

-- ********************************************************************************
-- *** WARNING:  This file is incorporated into C++ code and executed via the   ***
-- ***           sqlite3x library.  SQL commands should be separated from one   ***
-- ***           another by a blank line because sqlite3x silently truncates    ***
-- ***           executenonquery() calls that contain more than one statement!  ***
-- ********************************************************************************

-- Clean up. Tables need to be dropped in the opposite order they're created.
drop table if exists semantic_clusters_tpl;
drop table if exists semantic_funcsim;
drop table if exists semantic_fio;
drop table if exists semantic_sources;
drop table if exists semantic_instructions;
drop table if exists semantic_cg;
drop table if exists semantic_ast;
drop table if exists semantic_functions;
drop table if exists semantic_files;
drop table if exists semantic_faults;
drop table if exists semantic_outputvalues;
drop table if exists semantic_inputvalues;
drop table if exists semantic_history;

-- A history of the commands that were run to produce this database, excluding SQL run by the user.
create table semantic_history (
       hashkey bigint unique,                   -- nonsequential ID number to identify the command
       begin_time bigint,                       -- Approx time that command started (Unix time, seconds since epoch)
       end_time bigint,                         -- Approx time command ended, or zero if command terminated with failure
       notation text,                           -- Notation describing what action was performed
       command text                             -- Command-line that was issued
);

-- An inputvalue is either an integer or pointer value that belongs to an input group.  Each integer value has a sequential
-- position in its input group; similarly for pointers.  The sequence for integers and pointers are independent of one
-- another; they compose two separate input value lists within the input group.
create table semantic_inputvalues (
       id integer,                              -- ID for the input group; non-unique
       vtype character,                         -- P (pointer) or I (integer)
       pos integer,                             -- position of this inputvalue within its input group, per vtype
       val bigint,                              -- the integer or pointer input value
       cmd bigint references semantic_history(hashkey) -- command that created this row
);

-- An output value is a value produced as output when a specimen function is "executed" during fuzz testing.  Each execution of
-- a function produces zero or more output values which are grouped together into a output group.  The values in an output
-- group have a position within the group, although the analysis can be written to treat output groups as either vectors or
-- sets.
create table semantic_outputvalues (
       hashkey bigint,                          -- output set to which this value belongs; non-unique
       vtype character,                         -- V=>value, F=>fault, C=>function call, S=>syscall
       pos integer,                             -- position of value within its output group and vtype pair
       val bigint                               -- value, fault ID, or function ID depending on vtype
       -- command ID is not stored here because this table is already HUGE! One has a reasonably high chance of
       -- figuring out the command ID by looking at the semantic_fio table.
);

-- Some output values indicate special situations described by this table
create table semantic_faults (
       id integer primary key,                  -- the special integer value
       name varchar(16),                        -- short identifying name
       description text                         -- full description
);

-- List of files. The other tables store file IDs rather than file names.
create table semantic_files (
       id integer primary key,                  -- unique positive file ID
       name text                                -- file name
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
       digest varchar(40),                      -- SHA1 hash of the function's instructions and static data
       cmd bigint references semantic_history(hashkey) -- command that created this row
);

-- Function call graph
create table semantic_cg (
       caller integer references semantic_functions(id),
       callee integer references semantic_functions(id),
       file_id integer references semantic_files(id), -- File from which this call originates (used during update)
       cmd bigint references semantic_history(hashkey) -- Command that created this row
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

-- A binary version of the AST
create table semantic_ast (
       file_id integer references semantic_files(id),
       cmd bigint references semantic_history(hashkey), -- command that created this row
       content text                             -- base-64 encoding of the binary AST data ("blob"/"bytea" is not portable)
);

-- List of source code.
create table semantic_sources (
       cmd bigint references semantic_history(hashkey), -- command that created this row
       file_id integer references semantic_files(id),
       linenum integer,                         -- one-origin line number within file
       line text                                -- one line from a source file
);

-- Function input/output. One of these is produced each time we fuzz-test a function.
create table semantic_fio (
       func_id integer references semantic_functions(id),
       igroup_id integer,                       -- references semantic_inputvalues.id
       pointers_consumed integer,               -- number of pointers from the inputgroup consumed by this test
       integers_consumed integer,               -- number of integers from the inputgroup consumed by this test
       instructions_executed integer,           -- number of instructions executed by this test
       ogroup_id bigint,                        -- output produced by this function, semantic_outputvalues.hashkey
       status integer references semantic_faults(id), -- exit status of the test
       elapsed_time double precision,           -- number of seconds elapsed excluding ptr analysis
       cpu_time double precision,               -- number of CPU seconds used excluding ptr analysis
       cmd bigint references semantic_history(hashkey) -- command that created this row
);

-- Function similarity--how similar are pairs of functions
create table semantic_funcsim (
       func1_id integer references semantic_functions(id),
       func2_id integer references semantic_functions(id), -- func1_id < func2_id
       similarity double precision,             -- a value between 0 and 1, with one being equality
       cmd bigint references semantic_history(hashkey) -- command that set the precision on this row
);

-- Clusters. The commands allow many different cluster tables to be created. This is the template for creating
-- those tables.
create table semantic_clusters_tpl (
       id integer,                              -- cluster ID number
       func_id integer references semantic_functions(id)
);
