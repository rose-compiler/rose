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


-- ########################################################################################################################
--					Delete tests that reported "No space left on device"
-- ########################################################################################################################

create table bad_tests (test_id int);
insert into bad_tests (select test_id from attachments where content ~ 'No space left on device');

delete from attachments where test_id in (select test_id from bad_tests);
delete from test_results where id in (select test_id from bad_tests);

drop table bad_tests;

-- ########################################################################################################################
-- First and last date that each error was reported
-- ########################################################################################################################

create table stats as select to_timestamp(min(rose_date)) as first_date, to_timestamp(max(rose_date)) as last_date, count(*) as occurrences, first_error from test_results group by first_error order by first_error;

-- List of all distinct error messages
select * from stats order by first_error;

-- Number of errors first encountered per date
select first_date, count(*) as nerrors from stats group by first_date order by first_date;

-- Number of errors last encountered per date
select last_date, count(*) as nerrors from stats group by last_date order by last_date;

drop table stats;


-- ########################################################################################################################
-- Creates a view of test_results that has only those results whose configurations are officially supported
-- ########################################################################################################################

create view supported_results as
    select test.* from test_results as test
        join dependencies as d01 on test.rmc_assertions  = d01.value
	join dependencies as d02 on test.rmc_boost       = d02.value
	join dependencies as d03 on test.rmc_build       = d03.value
	join dependencies as d04 on test.rmc_debug       = d04.value
	join dependencies as d05 on test.rmc_dlib        = d05.value
	join dependencies as d06 on test.rmc_doxygen     = d06.value
	join dependencies as d07 on test.rmc_edg         = d07.value
	join dependencies as d08 on test.rmc_edg_compile = d08.value
	join dependencies as d09 on test.rmc_languages   = d09.value
	join dependencies as d10 on test.rmc_magic       = d10.value
	join dependencies as d11 on test.rmc_optimize    = d11.value
	join dependencies as d12 on test.rmc_python      = d12.value
	join dependencies as d13 on test.rmc_qt          = d13.value
	join dependencies as d14 on test.rmc_readline    = d14.value
	join dependencies as d15 on test.rmc_sqlite      = d15.value
	join dependencies as d16 on test.rmc_warnings    = d16.value
	join dependencies as d17 on test.rmc_wt          = d17.value
	join dependencies as d18 on test.rmc_yaml        = d18.value
	join dependencies as d19 on test.rmc_yices       = d19.value
	join dependencies as d20 on test.rmc_java        = d20.value
	join dependencies as d21 on test.rmc_dwarf       = d21.value
	join dependencies as d22 on test.rmc_z3          = d22.value
	join dependencies as d23 on test.rmc_cmake       = d23.value
	join dependencies as d24 on test.rmc_tup         = d24.value
	where d01.name = 'assertions'  and d01.supported > 0
	and   d02.name = 'boost'       and d02.supported > 0
	and   d03.name = 'build'       and d03.supported > 0
	and   d04.name = 'debug'       and d04.supported > 0
	and   d05.name = 'dlib'        and d05.supported > 0
	and   d06.name = 'doxygen'     and d06.supported > 0
	and   d07.name = 'edg'         and d07.supported > 0
	and   d08.name = 'edg_compile' and d08.supported > 0
	and   d09.name = 'languages'   and d09.supported > 0
	and   d10.name = 'magic'       and d10.supported > 0
	and   d11.name = 'optimize'    and d11.supported > 0
	and   d12.name = 'python'      and d12.supported > 0
	and   d13.name = 'qt'          and d13.supported > 0
	and   d14.name = 'readline'    and d14.supported > 0
	and   d15.name = 'sqlite'      and d15.supported > 0
	and   d16.name = 'warnings'    and d16.supported > 0
	and   d17.name = 'wt'          and d17.supported > 0
	and   d18.name = 'yaml'        and d18.supported > 0
	and   d19.name = 'yices'       and d19.supported > 0
	and   d20.name = 'java'        and d20.supported > 0
	and   d21.name = 'dwarf'       and d21.supported > 0
	and   d22.name = 'z3'          and d22.supported > 0
	and   d23.name = 'cmake'       and d23.supported > 0
	and   d24.name = 'tup'         and d24.supported > 0
	;
	
