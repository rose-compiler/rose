-- Creates the matrix tables without dropping them first.


begin transaction;

--
-- User list.  These are users that are permitted insert new test results.
--

create table users (
    uid serial primary key,                             -- unique user ID number
    name varchar(64),                                   -- user name, email, etc.
    salt varchar(16),                                   -- random password salt
    password varchar(64),                               -- cryptographically hashed password
    enabled integer                                     -- non-zero if user is enabled
);

insert into users (name) values ('matzke');
insert into users (name) values ('jenkins');


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
    name varchar(64),                                   -- name of package, such as "boost", "compiler", "languages".
    value varchar(64),                                  -- argument(s) for the "rmc_" function
    enabled integer                                     -- whether this version should be tested
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
insert into dependencies values ('build',        'autoconf',         1);
insert into dependencies values ('build',        'cmake',            0);

-- Compiler is $VENDOR-$VERSION-$LANGUAGE or just $VENDOR-$VERSION or just $VENDOR.
insert into dependencies values ('compiler',     'gcc-4.2-default',  0);
insert into dependencies values ('compiler',     'gcc-4.3-default',  0);
insert into dependencies values ('compiler',     'gcc-4.4-default',  0);
insert into dependencies values ('compiler',     'gcc-4.4-c++11',    0);
insert into dependencies values ('compiler',     'gcc-4.8-default',  1);
insert into dependencies values ('compiler',     'gcc-4.8-c++11',    1);
insert into dependencies values ('compiler',     'gcc-4.9-default',  1);
insert into dependencies values ('compiler',     'gcc-4.9-c++11',    1);
insert into dependencies values ('compiler',     'llvm-3.5-default', 1);

-- Whether to compile ROSE with debugging support. Value is 'yes' or 'no'.
-- NOTE: cmake is not set up for all combinations of debug and optimize.
insert into dependencies values ('debug',        'yes',              1);
insert into dependencies values ('debug',        'no',               0);

-- Whether to optimize ROSE when compiling. Value is 'yes' or 'no'.
-- NOTE: cmake is not set up for all combinations of debug and optimize.
insert into dependencies values ('optimize',     'yes',              0);
insert into dependencies values ('optimize',     'no',               1);

-- Whether compiler warnings should be turned on. Value is 'yes' or 'no'
insert into dependencies values ('warnings',     'yes',              1);
insert into dependencies values ('warnings',     'no',               0);

-- How ASSERT_* failures are handled by default. Values are 'abort', 'exit', or 'throw'.
insert into dependencies values ('assertions',   'abort',            1);
insert into dependencies values ('assertions',   'exit',             0);
insert into dependencies values ('assertions',   'throw',            0);

-- What frontend languages ROSE supports.  Value is a comma-separated list or the word 'all'.
insert into dependencies values ('languages',    'all',              1);
insert into dependencies values ('languages',    'c,c++',            1);
insert into dependencies values ('languages',    'binaries',         1);

-- Boost version numbers or "system" to use the system-installed version.
insert into dependencies values ('boost',        '1.43',             0);
insert into dependencies values ('boost',        '1.44',             0);
insert into dependencies values ('boost',        '1.45',             0);
insert into dependencies values ('boost',        '1.46',             0);
insert into dependencies values ('boost',        '1.47',             1);
insert into dependencies values ('boost',        '1.48',             1);
insert into dependencies values ('boost',        '1.49',             1);
insert into dependencies values ('boost',        '1.50',             1);
insert into dependencies values ('boost',        '1.51',             1);
insert into dependencies values ('boost',        '1.52',             1);
insert into dependencies values ('boost',        '1.53',             1);
insert into dependencies values ('boost',        '1.54',             1);
insert into dependencies values ('boost',        '1.55',             1);
insert into dependencies values ('boost',        '1.56',             1);
insert into dependencies values ('boost',        '1.57',             1);
insert into dependencies values ('boost',        '1.58',             1);
insert into dependencies values ('boost',        '1.59',             1);
insert into dependencies values ('boost',        '1.60',             1);

-- DLib version numbers or "system" or "none"
insert into dependencies values ('dlib',         'none',             1);
insert into dependencies values ('dlib',         '18.10',            0);
insert into dependencies values ('dlib',         '18.11',            0);
insert into dependencies values ('dlib',         '18.12',            0);
insert into dependencies values ('dlib',         '18.13',            0);
insert into dependencies values ('dlib',         '18.14',            0);
insert into dependencies values ('dlib',         '18.15',            0);
insert into dependencies values ('dlib',         '18.16',            0);
insert into dependencies values ('dlib',         '18.17',            1);

-- Doxygen version numbers or "system" or "none"
insert into dependencies values ('doxygen',      'none',             0);
insert into dependencies values ('doxygen',      '1.8.1',            0);
insert into dependencies values ('doxygen',      '1.8.2',            0);
insert into dependencies values ('doxygen',      '1.8.3',            0);
insert into dependencies values ('doxygen',      '1.8.4',            0);
insert into dependencies values ('doxygen',      '1.8.5',            0);
insert into dependencies values ('doxygen',      '1.8.6',            0);
insert into dependencies values ('doxygen',      '1.8.7',            0);
insert into dependencies values ('doxygen',      '1.8.8',            0);
insert into dependencies values ('doxygen',      '1.8.9',            0);
insert into dependencies values ('doxygen',      '1.8.10',           1);

-- EDG version numbers.
insert into dependencies values ('edg',          '4.4',              0);
insert into dependencies values ('edg',          '4.7',              1);
insert into dependencies values ('edg',          '4.8',              1);
insert into dependencies values ('edg',          '4.9',              1);

-- libmagic version numbers or "system" or "none"
insert into dependencies values ('magic',        'none',             1);
insert into dependencies values ('magic',        'system',           0);

-- Python executable path
insert into dependencies values ('python',       '/usr/bin/python3', 1);

-- Qt version numbers or "system" or "none"
insert into dependencies values ('qt',           'none',             1);
insert into dependencies values ('qt',           'system',           0);

-- GNU Readline version or "system" or "none"
insert into dependencies values ('readline',     'none',             1);
insert into dependencies values ('readline',     'system',           1);
insert into dependencies values ('readline',     'ambivalent',       1);

-- SQLite library version number or "system" or "none"
insert into dependencies values ('sqlite',       'none',             1);
insert into dependencies values ('sqlite',       'system',           0);

-- Wt (web toolkit) version number or "system" or "none"
insert into dependencies values ('wt',           'none',             1);
insert into dependencies values ('wt',           '3.3.3',            0);
insert into dependencies values ('wt',           '3.3.4',            1);
insert into dependencies values ('wt',           '3.3.5',            0);

-- YAML-CC library version number or "system" or "none"
insert into dependencies values ('yaml',         'none',             1);
insert into dependencies values ('yaml',         '0.5.1',            1);
insert into dependencies values ('yaml',         '0.5.2',            0);
insert into dependencies values ('yaml',         '0.5.3',            0);

-- Yices SMT solver version or "system" or "none"
insert into dependencies values ('yices',        'no',               1);
insert into dependencies values ('yices',        '1.0.28',           0);
insert into dependencies values ('yices',        '1.0.34',           1);


--
-- Table stores test names (status)
--

create table test_names (
    name varchar(64) not null,                          -- name of the test
    position integer                                    -- display position relative to other tests
);

insert into test_names values ( 'configure',        10  );
insert into test_names values ( 'library-build',    20  );
insert into test_names values ( 'libtest-build',    30  );
insert into test_names values ( 'libtest-check',    40  );
insert into test_names values ( 'project-bintools', 50  );
insert into test_names values ( 'end',              999 );

--
-- The table that stores the results of each test.
--

create table test_results (
    id serial primary key,

    -- who did the testing and reporting
    reporting_user integer references users(uid),       -- user making this report
    reporting_time integer,                             -- when report was made (unix time)
    tester varchar(256),                                -- who did the testing (e.g., a Jenkins slave name)
    os varchar(64),                                     -- operating system information

    -- what version of ROSE was tested?
    rose varchar(64),                                   -- SHA1 for the commit being tested
    rose_date integer,                                  -- time at which version was created if known (unix time)

    -- Software dependencies/configuration.  These column names come from the "name" column of the "dependencies" table
    -- with "rmc_" prepended.  The values need not be the same as the values in the "dependencies" table.  For instance,
    -- the dependencies table might list compiler = "gcc-4.8" but the tester might report "gcc-4.8.4".  In other words,
    -- the dependencies table says what we want to test, and this results table shows what was actually tested.
    rmc_assertions      varchar(64) default 'unknown',
    rmc_boost           varchar(64) default 'unknown',
    rmc_build           varchar(64) default 'unknown',
    rmc_compiler        varchar(64) default 'unknown',
    rmc_debug           varchar(64) default 'unknown',
    rmc_dlib            varchar(64) default 'unknown',
    rmc_doxygen         varchar(64) default 'unknown',
    rmc_edg             varchar(64) default 'unknown',
    rmc_languages       varchar(64) default 'unknown',
    rmc_magic           varchar(64) default 'unknown',
    rmc_optimize        varchar(64) default 'unknown',
    rmc_python          varchar(64) default 'unknown',
    rmc_qt              varchar(64) default 'unknown',
    rmc_readline        varchar(64) default 'unknown',
    rmc_sqlite          varchar(64) default 'unknown',
    rmc_warnings        varchar(64) default 'unknown',
    rmc_wt              varchar(64) default 'unknown',
    rmc_yaml            varchar(64) default 'unknown',
    rmc_yices           varchar(64) default 'unknown',

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

    -- Additional optional information reported by the tester.
    duration integer,                                   -- time it took to run the test (seconds)
    noutput integer,                                    -- total lines of output (compiler, tests, etc)
    nwarnings integer,                                  -- number of compiler warnings (pattern "warning:")

    -- Information about the first error message.
    first_error text
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
    status text not null,				-- point at which error was detected
    message text not null,				-- the error message
    commentary text,					-- commentary about the error message
    mtime int	     					-- time that commentary was added/changed (unix)
);

commit;
