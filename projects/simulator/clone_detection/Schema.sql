-- Schema for binary clone detection output

-- The binary functions are clustered into similarity sets (abbreviated "simsets"); each set contains the functions that are
-- presumed to be semantically similar and the evidence for the similarity (inputs and outputs from fuzz testing) .
create table semantic_simsets (
       id integer primary key
);

-- An inputset contains the input values used when running a function. It is a set containing two lists of values: one list
-- of pointer values, and one list of non-pointer values. During fuzz testing, whenever a function needs an input value it
-- consumes the next pointer or non-pointer value from that list.  Once the list is consumed the function gets zero when it
-- needs an input value from that list.  A pointer analysis prior to fuzz testing determines whether particular function inputs
-- need pointers or non-pointers.
create table semantic_inputsets (
       id integer primary key
);

-- An inputvalue is either a pointer or non-pointer value that belongs to an inputset.  Each pointer value has a sequential
-- position in its inputset; similarly for non-pointers.  The sequence for pointers and non-pointers are independent of one
-- another; they compose two separate input value lists within the inputset as mentioned above.
create table semantic_inputvalues (
       inputset_id integer references semantic_inputsets,
       vtype character,                         -- P (pointer) or N (non-pointer)
       pos integer,                             -- position of this inputvalue within an input sequence, per vtype
       val integer                              -- the integer (pointer or non-pointer) value used as input
);

-- An outputset is a set of values produced as output from fuzz testing a function with a particular inputset. The order
-- of values in the outputset is not important (it is a set, not a list).
create table semantic_outputsets (
       id integer primary key
);

-- An outputvalue is a member of an outputset.  The relative positions of outputvalues within an outputset is unimportant.
create table semantic_outputvalues (
       outputset_id integer references semantic_outputsets,
       val integer
);

-- An inoutpair is a pair consisting of one inputset and one outputset.  The outputset is the output produced when some function
-- was fuzz tested using the specified inputset.
create table semantic_inoutpairs (
       simset_id integer references semantic_simsets,
       inputset_id integer references semantic_inputsets,
       outputset_id integer references semantic_outputsets
);

-- A function is the unit of code that is fuzz tested. Every function is a member of a simset.  Functions are uniquely
-- identified by their entry virtual address.  It's difficult to tell which file a function came from in the simulator (it
-- might have even come from no file at all), so we don't store a file name for the function.  The clone detection is
-- currently written in such a way that it wouldn't really need to be running inside a simulator, in which case knowing the
-- file name would be easy since all functions would have come from a file rather than specimen process memory.
create table semantic_functions (
       id integer primary key,	                      -- unique function ID
       entry_va integer,                              -- unique starting virtual address within the binary specimen
       funcname text,                                 -- name of function if known
       filename text,                                 -- name of file in which function exists, if known
       simset_id integer references semantic_simsets  -- which similarity set does this function belong to?
);
