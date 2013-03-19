-- The goal of this file is to obtain similarity sets whose functions are similar both syntactically and semantically.
-- The degree of similarity for both syntactic and semantic is ajusted in the commands that generate the two databases:
-- one for syntactic and one for semantic.
--
-- Before computing the combined similarity, the data needs to be moved into a single database. One way to do that is to
-- follow these instructions.
--    1. Run semantic clone detection to get a clones.db file. All tables are prefixed with "semantic_". The
--       schema is documented in clone_detection/Schema.sql
--    2. Run the syntactic vector creation to generate a database named syntactic.db.  The schema is undocumented
--       and created from compiled C source code. There is a README in the projects/BinaryCloneDetection directory.
--    3. Run the syntactic clone detection.  This fills in the "clusters" table of syntactic.db
--    4. Dump the syntactic database and load it into "clones.db".  Syntactic tables do not have a common prefix, but
--       are anything not starting with "semantic_".  The command to do this is:
--            echo .dump |sqlite3 syntactic.db |sqlite3 clones.db
--

-- All pairs of functions from each syntactic similarity set (aka., "cluster").
drop view syntactic_clone_pairs;
create view syntactic_clone_pairs as
    select distinct a.cluster as simset_id, c.entry_va as entry_va_1, d.entry_va as entry_va_2
    from clusters a
    join clusters b on a.cluster = b.cluster
    join function_ids c on a.function_id = c.row_number
    join function_ids d on b.function_id = d.row_number
    where c.entry_va < d.entry_va;

-- All pairs of functions from each semantic similarity set (aka., "simset").
drop view semantic_clone_pairs;
create view semantic_clone_pairs as
    select a.simset_id, a.entry_va as entry_va_1, b.entry_va as entry_va_2
    from semantic_functions a
    join semantic_functions b on a.simset_id = b.simset_id
    where a.entry_va < b.entry_va;

-- All pairs of functions that appear as both a syntactic pair and a semantic pair.
drop view clone_pairs;
create view clone_pairs as
    select a.simset_id as semantic_simset, b.simset_id as syntactic_simset, a.entry_va_1, a.entry_va_2
    from semantic_clone_pairs a
    join syntactic_clone_pairs b on a.entry_va_1 = b.entry_va_1 and a.entry_va_2 = b.entry_va_2;

-- The combined syntactic-semantic similarity sets obtained by unpivoting the entry_va_1 and entry_va_2 columns
-- and ignoring the syntactic similarity set (since a function can appear in more than one syntactic simset).
drop view combined_simsets;
create view combined_simsets as
    select distinct semantic_simset,
                    case choice when 1 then entry_va_1 when 2 then entry_va_2 end as entry_va
    from clone_pairs a
    cross join (select 1 as choice union select 2);

-- Finally, show the combined similarity sets and the entry addresses and names of the functions belonging to each set
select a.*, b.funcname
    from combined_simsets a
    join semantic_functions b on a.entry_va = b.entry_va;

