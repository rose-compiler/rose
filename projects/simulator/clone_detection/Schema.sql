-- Schema for binary clone detection output

-- The binary functions are organized into similarity sets; each set contains the functions that are presumed to be semantically
-- similar and the evidence (inputs and outputs) for the similarity.
create table simsets (
       id integer primary key
);

-- An inputset contains the input values used when running a function. It is a set containing two sequences of values: pointers
-- and non-pointers.
create table inputsets (
       id integer primary key
);

-- An inputvalue is either a pointer or non-pointer value that belongs to an inputset.  Each pointer value has a sequential
-- position in its inputset; similarly for non-pointers.  The sequence for pointers and non-pointers are independent of one
-- another; they compose two separate sequences within the inputset.
create table inputvalues (
       inputset_id integer references inputsets,
       vtype character,				-- P (pointer) or N (non-pointer)
       pos integer,				-- position of this inputvalue within an input sequence, per vtype
       val integer				-- the integer (pointer or non-pointer) value used as input
);

-- An outputset is a set of values produced as output by a function.
create table outputsets (
       id integer primary key
);

-- An outputvalue is a member of an outputset.  The relative positions of outputvalues within an outputset is unimportant.
create table outputvalues (
       outputset_id integer references outputsets,
       val integer
);

-- An inoutpair is a pair consisting of one inputset and one outputset.  The outputset is the output produced when some function
-- was run using the specified inputset.
create table inoutpairs (
       simset_id integer references simsets,
       inputset_id integer references inputsets,
       outputset_id integer references outputsets
);

-- A function is a member of a simset.  Functions are uniquely identified by their entry virtual address.
create table functions (
       entry_va integer primary key,
       simset_id integer references simsets
);
