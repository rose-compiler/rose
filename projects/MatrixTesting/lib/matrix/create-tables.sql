-- Creates the matrix tables without dropping them first.


begin transaction;

--
-- The folowing tables are created automatically by the web application
--
--  auth_users          -- application-level user information
--  auth_info           -- Wt::Auth information about each user
--  auth_identities     -- login name(s) for each user
--  auth_tokens         -- web browser tokens to maintain logins between sessions

--
-- Persistent interface settings
--
create table interface_settings (
    rose_public_version text default '',                -- version number to show by default on the public parts of the site
    pass_criteria text default 'end'                    -- what test needs to be reached for ROSE to be considered useful
);

insert into interface_settings (rose_public_version) values ('');

--
-- Information for the slaves doing the actual testing
--
create table slave_settings (
    name text default '',
    value text default ''
);

-- Information from the slaves about their health
create table slave_health (
    name varchar(256),					-- slave name
    timestamp integer,	   				-- time of report (Unix time)
    load_ave real,	   				-- machine load average between 0.0 and 1.0
    free_space integer,	   				-- free disk space in MB
    event varchar(16),	   				-- "boot", "shutdown", or "test"
    test_id integer					-- test reported if event is "test"
);

-- Info about what ROSE version should be tested and how
insert into slave_settings values ('TEST_REPOSITORY', 'https://github.com/rose-compiler/rose');
insert into slave_settings values ('TEST_COMMITTISH', 'origin/master');
insert into slave_settings values ('TEST_FLAGS', '--only-supported');

-- Info about what version of the MatrixTesting tools should be used
insert into slave_settings values ('MATRIX_REPOSITORY', 'https://github.com/rose-compiler/rose');
insert into slave_settings values ('MATRIX_COMMITTISH', 'origin/master');


--
-- List of software dependency packages.  There are a number of different kinds of dependencies:
--   1. Simple dependencies like "boost" whose value is a version number.
--   2. Complex dependencies like "compiler" where the value is more than a version number, e.g., "llvm-3.5".
--   3. Configuration settings like "debug" where the value is a setting like "yes", "no", etc.
--
-- This table is used for two things:
--   1. It provies a list of column names for the test_results table.
--   2. It is used to generate input to some optional scripts that configures ROSE using cmake or autotools
--

create table dependencies (
    name varchar(64) not null,                          -- name of package, such as "boost", "compiler", "languages".
    value varchar(64) not null,                         -- argument(s) for the "rmc_" function
    enabled integer not null,                           -- whether this version should be tested
    supported integer not null,				-- whether this is an officially supported configuration
    comment varchar(256) not null default ""
);

--
-- Initialize the dependencies table to show what values are possible.  These values are the INPUTS to the scripts that
-- configure ROSE, not necessarily the  values spit out at the conclusion of a test.  For instance, the "compiler" version
-- number in this table might be "gcc-4.8" yet the test results might report "gcc-4.8.4", "gcc-4.8.5", etc.
--
-- The names and values stored in this table become RMC commands and arguments in the RMC system, so please don't add new
-- names without first consulting with the RMC maintainer (Robb Matzke).  RMC (ROSE Meta Configuration) is a system of
-- shell scripts for resolving and building software dependencies and configuring and building ROSE.
--

-- Build system is either 'autoconf' or 'cmake'.
insert into dependencies values ('build',        'autoconf',         1, 1);
insert into dependencies values ('build',        'cmake',            0, 1);
insert into dependencies values ('build',        'tup',              1, 0);

-- Compiler is $VENDOR-$VERSION-$LANGUAGE or just $VENDOR-$VERSION or just $VENDOR.
-- A vendor is not the same thing as a compiler name (gcc vs. g++, llvm vs. clang++, intel vs. icc, etc)
insert into dependencies values ('compiler',     'gcc-4.0-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.1-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.2-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.3-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.4-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.5-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.6-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.7-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.8-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.8-c++11',    0, 1);
insert into dependencies values ('compiler',     'gcc-4.9-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-4.9-c++11',    0, 1);
insert into dependencies values ('compiler',     'gcc-5.1-default',  0, 1);
insert into dependencies values ('compiler',     'gcc-5.1-c++11',    0, 1);
insert into dependencies values ('compiler',     'llvm-3.5-default', 0, 1);
insert into dependencies values ('compiler',     'llvm-3.5-c++11',   0, 1);
insert into dependencies values ('compiler',     'llvm-3.7-default', 0, 1);
insert into dependencies values ('compiler',     'llvm-3.7-c++11',   0, 1);
insert into dependencies values ('compiler',     'installed',        1, 0);

-- Whether to compile ROSE with debugging support. Value is 'yes' or 'no'.
-- NOTE: cmake is not set up for all combinations of debug and optimize.
insert into dependencies values ('debug',        'yes',              1, 1);
insert into dependencies values ('debug',        'no',               0, 1);

-- Whether to optimize ROSE when compiling. Value is 'yes' or 'no'.
-- NOTE: cmake is not set up for all combinations of debug and optimize.
insert into dependencies values ('optimize',     'yes',              0, 1);
insert into dependencies values ('optimize',     'no',               1, 1);

-- Whether compiler warnings should be turned on. Value is 'yes' or 'no'
insert into dependencies values ('warnings',     'yes',              1, 1);
insert into dependencies values ('warnings',     'no',               0, 1);

-- How ASSERT_* failures are handled by default. Values are 'abort', 'exit', or 'throw'.
insert into dependencies values ('assertions',   'abort',            1, 1);
insert into dependencies values ('assertions',   'exit',             0, 1);
insert into dependencies values ('assertions',   'throw',            0, 1);

-- What frontend languages ROSE supports.  Value is a comma-separated list or the word 'all'.
insert into dependencies values ('languages',    'all',              1, 1);
insert into dependencies values ('languages',    'c,c++',            1, 1);
insert into dependencies values ('languages',    'c,c++,fortran'     1, 1);
insert into dependencies values ('languages',    'binaries',         1, 1);

-- Boost version numbers or "system" to use the system-installed version.
insert into dependencies values ('boost',        '1.43.0',           0, 0);
insert into dependencies values ('boost',        '1.44.0',           0, 0);
insert into dependencies values ('boost',        '1.45.0',           0, 0);
insert into dependencies values ('boost',        '1.46.0',           0, 0);
insert into dependencies values ('boost',        '1.47.0',           0, 0);
insert into dependencies values ('boost',        '1.48.0',           0, 0);
insert into dependencies values ('boost',        '1.49.0',           0, 0);
insert into dependencies values ('boost',        '1.50.0',           0, 0);
insert into dependencies values ('boost',        '1.51.0',           0, 0);
insert into dependencies values ('boost',        '1.52.0',           0, 0);
insert into dependencies values ('boost',        '1.53.0',           0, 0);
insert into dependencies values ('boost',        '1.54.0',           0, 0);
insert into dependencies values ('boost',        '1.55.0',           0, 0);
insert into dependencies values ('boost',        '1.56.0',           0, 0);
insert into dependencies values ('boost',        '1.57.0',           0, 0);
insert into dependencies values ('boost',        '1.58.0',           0, 0);
insert into dependencies values ('boost',        '1.59.0',           0, 0);
insert into dependencies values ('boost',        '1.60.0',           0, 0);
insert into dependencies values ('boost',        '1.61.0',           1, 1);

-- DLib version numbers or "system" or "none"
insert into dependencies values ('dlib',         'none',             1, 1);
insert into dependencies values ('dlib',         '18.10',            1, 0);
insert into dependencies values ('dlib',         '18.11',            1, 0);
insert into dependencies values ('dlib',         '18.12',            1, 0);
insert into dependencies values ('dlib',         '18.13',            1, 0);
insert into dependencies values ('dlib',         '18.14',            1, 0);
insert into dependencies values ('dlib',         '18.15',            1, 0);
insert into dependencies values ('dlib',         '18.16',            1, 0);
insert into dependencies values ('dlib',         '18.17',            1, 0);
insert into dependencies values ('dlib',         '18.18',            1, 0);

-- Doxygen version numbers or "system" or "none"
insert into dependencies values ('doxygen',      'none',             1, 1);
insert into dependencies values ('doxygen',      '1.8.1',            0, 0);
insert into dependencies values ('doxygen',      '1.8.2',            0, 0);
insert into dependencies values ('doxygen',      '1.8.3',            0, 0);
insert into dependencies values ('doxygen',      '1.8.4',            0, 0);
insert into dependencies values ('doxygen',      '1.8.5',            0, 0);
insert into dependencies values ('doxygen',      '1.8.6',            0, 0);
insert into dependencies values ('doxygen',      '1.8.7',            0, 0);
insert into dependencies values ('doxygen',      '1.8.8',            0, 0);
insert into dependencies values ('doxygen',      '1.8.9',            0, 0);
insert into dependencies values ('doxygen',      '1.8.10',           1, 0);

-- libdwarf version numbers
insert into dependencies values ('dwarf',        'none',             1, 1);

-- EDG version numbers.
insert into dependencies values ('edg',          '4.4',              0, 0);
insert into dependencies values ('edg',          '4.7',              0, 0);
insert into dependencies values ('edg',          '4.8',              0, 0);
insert into dependencies values ('edg',          '4.9',              1, 1);
insert into dependencies values ('edg',          '4.11',             1, 1);
insert into dependencies values ('edg',          '4.12'              1, 1);

-- libmagic version numbers or "system" or "none"
insert into dependencies values ('magic',        'none',             1, 1);
insert into dependencies values ('magic',        'system',           0, 0);

-- Python executable path
insert into dependencies values ('python',       '3.6',		     1, 1);
insert into dependencies values ('python',       'none',	     1, 1);

-- Qt version numbers or "system" or "none"
insert into dependencies values ('qt',           'none',             1, 1);
insert into dependencies values ('qt',           'system',           0, 0);

-- GNU Readline version or "system" or "none"
insert into dependencies values ('readline',     'none',             1, 1);
insert into dependencies values ('readline',     'system',           1, 0);
insert into dependencies values ('readline',     'ambivalent',       1, 0);

-- SQLite library version number or "system" or "none"
insert into dependencies values ('sqlite',       'none',             1, 1);
insert into dependencies values ('sqlite',       'system',           0, 0);

-- Wt (web toolkit) version number or "system" or "none"
insert into dependencies values ('wt',           'none',             1, 1);
insert into dependencies values ('wt',           '3.3.3',            0, 0);
insert into dependencies values ('wt',           '3.3.4',            1, 0);
insert into dependencies values ('wt',           '3.3.5',            0, 0);

-- YAML-CC library version number or "system" or "none"
insert into dependencies values ('yaml',         'none',             1, 1);
insert into dependencies values ('yaml',         '0.5.1',            1, 0);
insert into dependencies values ('yaml',         '0.5.2',            0, 0);
insert into dependencies values ('yaml',         '0.5.3',            0, 0);

-- Yices SMT solver version or "system" or "none"
insert into dependencies values ('yices',        'none',             1, 1);
insert into dependencies values ('yices',        '1.0.28',           0, 0);
insert into dependencies values ('yices',        '1.0.34',           0, 0);

-- Z3 SMT solver version or "none"
insert into dependencies values ('z3',           'none',             1, 1);
insert into dependencies values ('z3',           '4.5.0',            1, 0);


--
-- Table stores test names (status)
--

create table test_names (
    name varchar(64) not null,                          -- name of the test
    position integer                                    -- display position relative to other tests
);

insert into test_names values ( 'configure',         10 );
insert into test_names values ( 'library-build',     20 );
insert into test_names values ( 'libtest-build',     30 );
insert into test_names values ( 'libtest-check',     40 );
insert into test_names values ( 'library-check',     41 );
insert into test_names values ( 'projects-robb',     50 );
insert into test_names values ( 'projects-justin',   60 );
insert into test_names values ( 'projects-dan',      70 );
insert into test_names values ( 'projects-markus',   80 );
insert into test_names values ( 'projects-peihung',  90 );
insert into test_names values ( 'projects-leo',     100 );
insert into test_names values ( 'tutorial-build',   110 );
insert into test_names values ( 'install',          120 );
insert into test_names values ( 'bindist',          130 );
insert into test_names values ( 'project',          140 );
insert into test_names values ( 'end',              999 );

--
-- The table that stores the results of each test.
--

create table test_results (
    id serial primary key,
    enabled boolean default true,                       -- can be set to false to prevent test from showing in browser

    -- who did the testing and reporting
    reporting_user integer references auth_users(id),   -- user making this report
    reporting_time integer,                             -- when report was made (unix time)
    tester varchar(256),                                -- who did the testing (e.g., a Jenkins slave name)
    os varchar(64),                                     -- operating system information

    -- what version of ROSE was tested?
    rose varchar(64),                                   -- SHA1 for the commit being tested
    rose_date integer default 0,                        -- time at which version was created if known (unix time)

    -- Software dependencies/configuration.  These column names come from the "name" column of the "dependencies" table
    -- with "rmc_" prepended.  The values need not be the same as the values in the "dependencies" table.  For instance,
    -- the dependencies table might list compiler = "gcc-4.8" but the tester might report "gcc-4.8.4".  In other words,
    -- the dependencies table says what we want to test, and this results table shows what was actually tested.
    rmc_assertions      varchar(64) default 'unknown',
    rmc_boost           varchar(64) default 'unknown',
    rmc_build           varchar(64) default 'unknown',
    rmc_cmake           varchar(64) default 'unknown',
    rmc_compiler        varchar(64) default 'unknown',
    rmc_debug           varchar(64) default 'unknown',
    rmc_dlib            varchar(64) default 'unknown',
    rmc_doxygen         varchar(64) default 'unknown',
    rmc_dwarf           varchar(64) default 'unknown',
    rmc_edg             varchar(64) default 'unknown',
    rmc_edg_compile     varchar(64) default 'unknown',
    rmc_java            varchar(64) default 'unknown',
    rmc_jq              varchar(64) default 'unknown',
    rmc_languages       varchar(64) default 'unknown',
    rmc_magic           varchar(64) default 'unknown',
    rmc_optimize        varchar(64) default 'unknown',
    rmc_python          varchar(64) default 'unknown',
    rmc_qt              varchar(64) default 'unknown',
    rmc_readline        varchar(64) default 'unknown',
    rmc_sqlite          varchar(64) default 'unknown',
    rmc_tup             varchar(64) default 'unknown',
    rmc_warnings        varchar(64) default 'unknown',
    rmc_wt              varchar(64) default 'unknown',
    rmc_yaml            varchar(64) default 'unknown',
    rmc_yices           varchar(64) default 'unknown',
    rmc_z3              varchar(64) default 'unknown',

    -- Test disposition.  This is a word that says where the test failed. Rather than simply "passed"
    -- or "failed", we have a lattice of dispositions. A simple example is a lattice with a single path:
    --    environment       -- configures and checks the ROSE build environment
    --    configure         -- runs "configure" or "cmake"
    --    library-build     -- runs "make -C $ROSE/src"
    --    libtest-build     -- runs "make -C $ROSE/tests"
    --    libtest-check     -- runs "make -C $ROSE/tests check"
    --    project-bintools  -- runs "make -C $ROSE/projects/BinaryAnalysisTools checK"
    --    project-foo       -- runs "make -C $ROSE/projects/foo check"
    status varchar(64) not null,
    blacklistd varchar(80) default '',

    -- Additional optional information reported by the tester.
    duration integer default 0,                         -- time it took to run the test (seconds)
    noutput integer default 0,                          -- total lines of output (compiler, tests, etc)
    nwarnings integer default 0,                        -- number of compiler warnings (pattern "warning:")

    -- Information about the first error message.
    first_error text,
    first_error_staging text                            -- temporary column when for searching for errors
);

--
-- Stores optional text attachments (like output) for each test
--
create table attachments (
    id serial primary key,
    test_id integer references test_results(id),        -- the test to which this attachment belongs
    name varchar(64),                                   -- short name for this attachment
    content text                                        -- the content of the attachment
);

--
-- Stores info about error messages
--
create table errors (
    status text not null,                               -- point at which error was detected
    message text not null,                              -- the error message
    issue_name text default '',                         -- name of corresponding JIRA issue if any
    commentary text default '',                         -- commentary about the error message
    mtime int default 0                                 -- time that commentary was added/changed (unix)
);

--
-- Table for tracking progress compiling applications
--
create table application_results (
    id serial primary key,
    enabled boolean default true,                       -- can be set to false to prevent test from showing in browser

    -- who did the testing and reporting
    reporting_user integer references auth_users(id),   -- user making this report
    reporting_time integer,                             -- when report was made (unix time)
    tester varchar(256),                                -- who did the testing (e.g., a Jenkins slave name)
    os varchar(64),                                     -- operating system information

    -- what version of ROSE did the testing
    rose varchar(64) not null,                          -- SHA1 of the ROSE version that was compiling
    rose_date integer,                                  -- time at which version was created if known (unix time)

    -- what application was compiled
    application varchar(64) not null,                   -- name of the application
    application_version varchar(64),                    -- version string or other identifying information
    application_date integer,                           -- application version date if known (unix time)

    -- status of the test
    nfiles integer,                                     -- total number of files processed by ROSE, failing or not
    npass integer,                                      -- number of files which were compiled successfully
    duration integer,                                   -- length of compilation in seconds if known
    noutput integer,                                    -- number of lines of stdout + stderr produced by ROSE
    nwarnings integer                                   -- number or warnings produced by ROSE
);
    
    
commit;
