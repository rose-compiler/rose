-- Given a database containing both syntactic clusters and semantic similarity information, compute pairs of functions
-- that are both syntactically and semantically similar.  Tables are prefixed with "ipt" for "intellectual property theft".

drop table if exists ipt_combined_pairs;
drop table if exists ipt_semantic_pairs;
drop table if exists ipt_syntactic_pairs;
drop table if exists ipt_parameters;

-- Change these values to whatever you want
create table ipt_parameters as select
  0.7 as semantic_similarity,		-- min similarity for two functions to be considered semantic clones
  0.9 as syntactic_overlap;             -- min vector overlap for two functions to be considered syntactic clones

-- Pairs of syntactic clones (func1_id < func2_id)
create table ipt_syntactic_pairs as
  select distinct function_id_1 as func1_id, function_id_2 as func2_id
  from cluster_pairs
  where ratio_1 >= (select syntactic_overlap from ipt_parameters) and
        ratio_2 >= (select syntactic_overlap from ipt_parameters);

-- Pairs of semantic clones (func1_id < func2_id)
create table ipt_semantic_pairs as
  select func1_id, func2_id
  from semantic_funcsim
  where similarity >= (select semantic_similarity from ipt_parameters);

-- Pairs of functions that are both syntactic and semantic clones
create table ipt_combined_pairs as
  select * from ipt_syntactic_pairs
  intersect
  select * from ipt_semantic_pairs;
