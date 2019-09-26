create table testsuite (
       id int unique key,
       name varchar
);
       
create table specimen (
       id int unique key,
       testsuite foreign key(testsuite.id),
       name varchar,		-- name of the specimen, mostly for debugging
       image blob		-- a copy of the executable
);

create table testcase (
       id int unique,
       testsuite foreign key(testsuite.id),
       specimen foreign key(specimen.id),
       name varchar,
       concreteRank float,
       concreteResult blob
);
