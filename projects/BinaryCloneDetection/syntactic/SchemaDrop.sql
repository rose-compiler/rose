-- Clean up. Objects need to be dropped in the opposite order they're created.
-- See also, SchemaCreate.sql
drop table if exists cluster_pairs;
drop table if exists group_timing;
drop index if exists postprocessed_clusters_by_function;
drop index if exists clusters_index_by_cluster;
drop index if exists function_statistics_by_function;
drop index if exists postprocessed_clusters_by_function;
drop index if exists clusters_by_function;
drop table if exists total_coverage;
drop table if exists function_coverage;
drop table if exists postprocessed_clusters;
drop table if exists clusters;
drop index if exists vectors_sum_of_counts_index;
drop table if exists vectors;
drop table if exists function_statistics;
drop table if exists results;
drop table if exists timing;
drop table if exists vector_generator_timing;
drop table if exists detection_parameters;
drop table if exists run_parameters;
