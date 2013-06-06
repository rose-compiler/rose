-- Schema for binary clone detection output

-- ********************************************************************************
-- *** WARNING:  This file is incorporated into C++ code and executed via the   ***
-- ***           sqlite3x library.  SQL commands should be separated from one   ***
-- ***           another by a blank line because sqlite3x silently truncates    ***
-- ***           executenonquery() calls that contain more than one statement!  ***
-- ********************************************************************************




-- An inputvalue is either a pointer or non-pointer value that belongs to an input group.  Each pointer value has a sequential
-- position in its input group; similarly for non-pointers.  The sequence for pointers and non-pointers are independent of one
-- another; they compose two separate input value lists within the input group.
create table if not exists semantic_inputvalues (
       id integer,                              -- ID for the input group; non-unique
       vtype character,                         -- P (pointer) or N (non-pointer)
       pos integer,                             -- position of this inputvalue within its input group, per vtype
       val bigint                               -- the integer (pointer or non-pointer) value used as input
);

-- An output value is a value produced as output when a specimen function is "executed" during fuzz testing.  Each execution of
-- a function produces zero or more output values which are grouped together into a output group.  The values in an output
-- group have a position within the group, although the analysis can be written to treat output groups as either vectors or
-- sets.
create table if not exists semantic_outputvalues (
       id integer,                              -- output set to which this value belongs; non-unique
       vtype character,				-- V=>value, F=>fault, C=>function call, S=>syscall
       pos integer,                             -- position of value within its output group and vtype pair
       val bigint                               -- value, fault ID, or function ID depending on vtype
);

-- Some output values indicate special situations described by this table
create table if not exists semantic_faults (
       id integer,                              -- the special integer value
       name varchar(16),                        -- short identifying name
       description text                         -- full description
);

-- List of files. The other tables store file IDs rather than file names.
create table if not exists semantic_files (
       id integer primary key,                  -- unique positive file ID
       name text                                -- file name
);

-- A function is the unit of code that is fuzz tested.
create table if not exists semantic_functions (
       id integer primary key,                  -- unique function ID
       entry_va bigint,                         -- unique starting virtual address within the binary specimen
       funcname text,                           -- name of function if known
       file_id integer references semantic_files(id), -- binary file in which function exists, if known
       ninsns integer,                          -- number of instructions in function
       isize integer,                           -- size of function instructions in bytes, non-overlapping
       dsize integer,                           -- size of function data in bytes, non-overlapping
       size integer,                            -- total size of function, non-overlapping
       digest varchar(40),			-- SHA1 hash of the function's instructions and static data
       listing text                             -- function assembly listing
);

-- Function call graph
create table if not exists semantic_cg (
       caller integer references semantic_functions(id),
       callee integer references semantic_functions(id),
       file_id integer references semantic_files(id)	-- File from which this call originates (used during update)
);

-- This is the list of instructions for each function.  Functions need not be contiguous in memory, and two instructions
-- might overlap, which is why we have to store each instruction individually.  Every instruction belongs to exactly one
-- function.
create table if not exists semantic_instructions (
       address bigint,                          -- virtual address for start of instruction
       size integer,                            -- size of instruction in bytes
       assembly text,                           -- unparsed instruction including hexadecimal address
       function_id integer references semantic_functions(id), --function to which this instruction belongs
       position integer,                        -- zero-origin index of instruction within function
       src_file_id integer,                     -- source code file that produced this instruction, or -1
       src_line integer                         -- source line number that produced this instruction, or -1
);

-- List of source code.
create table if not exists semantic_sources (
       file_id integer references semantic_files(id),
       linenum integer,                         -- one-origin line number within file
       line text                                -- one line from a source file
);

-- Function input/output. One of these is produced each time we fuzz-test a function.
create table if not exists semantic_fio (
       func_id integer references semantic_functions(id),
       inputgroup_id integer references semantic_inputvalues(id),
       pointers_consumed integer,		-- number of pointers from the inputgroup consumed by this test
       nonpointers_consumed integer,		-- number of non-pointers from the inputgroup consumed by this test
       instructions_executed integer,           -- number of instructions executed by this test
       actual_outputgroup integer references semantic_outputvalues(id), -- actual output produced by this function
       effective_outputgroup integer references semantic_outputvalues(id), -- effective output used for similarity
       elapsed_time double precision,           -- number of seconds elapsed excluding ptr analysis
       cpu_time double precision                -- number of CPU seconds used excluding ptr analysis
);
