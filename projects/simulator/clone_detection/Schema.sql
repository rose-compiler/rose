-- Schema for binary clone detection output

-- An inputvalue is either a pointer or non-pointer value that belongs to an inputset.  Each pointer value has a sequential
-- position in its inputset; similarly for non-pointers.  The sequence for pointers and non-pointers are independent of one
-- another; they compose two separate input value lists within the inputset as mentioned above.
create table if not exists semantic_inputvalues (
       id integer,                              -- ID for the inputset; non-unique
       vtype character,                         -- P (pointer) or N (non-pointer)
       pos integer,                             -- position of this inputvalue within an input sequence, per vtype
       val integer                              -- the integer (pointer or non-pointer) value used as input
);

-- An outputvalue is a member of an outputset.  The relative positions of outputvalues within an outputset is unimportant.
create table if not exists semantic_outputvalues (
       id integer,                              -- output set to which this value belongs; non-unique
       val integer                              -- value stored in the output set
);

-- A function is the unit of code that is fuzz tested.
create table if not exists semantic_functions (
       id integer primary key,                  -- unique function ID
       entry_va integer,                        -- unique starting virtual address within the binary specimen
       funcname text,                           -- name of function if known
       file_id integer references semantic_files(id), -- binary file in which function exists, if known
       isize integer,                           -- size of function instructions in bytes, non-overlapping
       dsize integer,                           -- size of function data in bytes, non-overlapping
       size integer,                            -- total size of function, non-overlapping
       listing text                             -- function assembly listing
);

-- This is the list of instructions for each function.  Functions need not be contiguous in memory, and two instructions
-- might overlap, which is why we have to store each instruction individually.  Every instruction belongs to exactly one
-- function.
create table if not exists semantic_instructions (
       address integer,                         -- virtual address for start of instruction
       size integer,                            -- size of instruction in bytes
       assembly text,                           -- unparsed instruction including hexadecimal address
       function_id integer references semantic_functions(id), --function to which this instruction belongs
       position integer,                        -- zero-origin index of instruction within function
       src_file_id integer,                     -- source code file that produced this instruction, or -1
       src_line integer                         -- source line number that produced this instruction, or -1
);

-- List of files. The other tables store file IDs rather than file names.
create table if not exists semantic_files (
       id integer primary key,                  -- unique positive file ID
       name text                                -- file name
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
       inputset_id integer references semantic_inputsets(id),
       outputset_id integer references semantic_outputsets(id),
       elapsed_time double precision,           -- number of seconds elapsed excluding ptr analysis
       cpu_time double precision                -- number of CPU seconds used excluding ptr analysis
);
