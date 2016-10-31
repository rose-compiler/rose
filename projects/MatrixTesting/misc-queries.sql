-- ########################################################################################################################
--                                         Query to remove duplicate tests
-- ########################################################################################################################

-- This query only marks duplicate rows as being disabled; it doesn't actually remove them from the table. Rows that are
-- already marked as disabled are not considered and are not modified.

update test_results set enabled = false where id not in (
    -- subquery is to choose the test IDs that should be kept
    select max(id) from test_results where enabled
    group by reporting_user, tester, os, rose, rose_date, status, first_error,
        rmc_assertions,
	rmc_boost,
	rmc_build,
	rmc_compiler,
	rmc_debug,
	rmc_dlib,
	rmc_doxygen,
	rmc_dwarf,
	rmc_edg,
	rmc_languages,
	rmc_magic,
	rmc_optimize,
	rmc_python,
	rmc_qt,
	rmc_readline,
	rmc_sqlite,
	rmc_wt,
	rmc_yaml,
	rmc_yices,
	rmc_java);
