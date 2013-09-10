-- Number of tests that produce faults.
--
-- Sample output:
--     name                        nfaults     percent         
--     --------------------------  ----------  ----------------
--     AnalysisFault::DISASSEMBLY  9300        80.1724137931034
--     AnalysisFault::INSN_LIMIT   800         6.89655172413793

select
        fault.name as fault,
        count(*) as nfaults,
        100.0*count(*)/(select count(*) from semantic_fio) as percent
    from semantic_fio as fio
    join semantic_faults as fault on fio.status=fault.id
    group by fio.status, fault.name
    order by fio.status;

-- =============================================================================================================================
-- Numbers of instructions executed.
--
-- Sample output
--     ninsns      ntests      percent         
--     ----------  ----------  ----------------
--     13          2           3.44827586206897
--     19          1           1.72413793103448
--     22          1           1.72413793103448
--     35          1           1.72413793103448
--     43          1           1.72413793103448
--     46          1           1.72413793103448
--     49          1           1.72413793103448
--     51          1           1.72413793103448
--     53          1           1.72413793103448
--     62          1           1.72413793103448
--     63          2           3.44827586206897
--     64          1           1.72413793103448
--     72          1           1.72413793103448
--     76          1           1.72413793103448
--     84          1           1.72413793103448
--     87          1           1.72413793103448
--     88          1           1.72413793103448
--     95          1           1.72413793103448
--     100         1           1.72413793103448
--     106         1           1.72413793103448
--     113         1           1.72413793103448
--     114         1           1.72413793103448
--     132         1           1.72413793103448
--     141         1           1.72413793103448
--     146         1           1.72413793103448
--     147         1           1.72413793103448
--     151         1           1.72413793103448
--     192         1           1.72413793103448
--     960         1           1.72413793103448
--     1240        1           1.72413793103448
--     1448        1           1.72413793103448
--     1824        1           1.72413793103448
--     1948        1           1.72413793103448
--     2142        1           1.72413793103448
--     2308        1           1.72413793103448
--     2563        1           1.72413793103448
--     2638        1           1.72413793103448
--     13437       1           1.72413793103448
--     34721       1           1.72413793103448
--     84771       1           1.72413793103448
--     384522      1           1.72413793103448
--     446719      1           1.72413793103448
--     1000000     14          24.1379310344828

select
       instructions_executed as ninsns,
       count(*) as ntests,
       100.0*count(*)/(select count(*) from semantic_fio) as percent
    from semantic_fio
    group by instructions_executed
    order by instructions_executed;

-- =============================================================================================================================
-- To get a list of false negatives for semantic anysis after having run the failure-rate.sql queries.  Replace fr_fn_semantic
-- using "fp" instead of "fn" to see false positives, or replacing "semantic" with "syntactic", "combined", etc.
--
-- Sample output:
--     func_id_1   func_id_2   funcname    entry_va_1  entry_va_2
--     ----------  ----------  ----------  ----------  ----------
--     0           59          GEAcompile  134519808   134520640 
--     1           58          EGexecute   134520622   134519216 
--     2           60          Fexecute    134522506   134521840 
--     4           62          fillbuf     134524272   134524560 
--     5           63          print_line  134526014   134525696 
--     7           64          prline      134527186   134526928 
--     8           65          prtext      134527934   134528144 
--     9           66          grepbuf     134528929   134529184 
--     11          67          grepfile    134530702   134529744 
--     12          69          grepdir     134531700   134538240 
--     
select
        fn.*,
        func1.funcname,
        func1.entry_va as entry_va_1,
        func2.entry_va as entry_va_2
    from fr_fn_semantic as fn
    join semantic_functions as func1 on fn.func_id_1=func1.id
    join semantic_functions as func2 on fn.func_id_2=func2.id;
