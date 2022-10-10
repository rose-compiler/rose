all: demo1 demo2

demo1:
	codethorn demo1.C --context-sensitive=1 --abstraction-mode=1 --normalize-level=0 --vis
	cp transitiongraph2.dot demo1_ts.dot
	dot -Tpdf demo1_ts.dot -odemo1_ts.pdf
	cp cfg.dot demo1_icfg.dot
	dot -Tpdf demo1_icfg.dot -odemo1_icfg.pdf

	codethorn demo1.C --context-sensitive=1 --abstraction-mode=1 --normalize-level=0 --vis --std-io-only=1
	cp transitiongraph2.dot demo1_ts_reduced.dot
	dot -Tpdf demo1_ts_reduced.dot -odemo1_ts_reduced.pdf


demo2:
	codethorn demo2.C --context-sensitive=1 --abstraction-mode=1 --normalize-level=0 --vis
	cp transitiongraph2.dot demo2_ts.dot
	dot -Tpdf demo2_ts.dot -odemo2_ts.pdf
	cp cfg.dot demo2_icfg.dot
	dot -Tpdf demo2_icfg.dot -odemo2_icfg.pdf

	codethorn demo2.C --context-sensitive=1 --abstraction-mode=1 --normalize-level=0 --vis --std-io-only=1
	cp transitiongraph2.dot demo2_ts_reduced.dot
	dot -Tpdf demo2_ts_reduced.dot -odemo2_ts_reduced.pdf


release:
	tar cvzf demo.tgz demo1.C demo1_ts.dot demo1_ts.pdf demo1_icfg.dot demo1_icfg.pdf demo2.C demo2_ts.dot demo2_ts.pdf demo2_icfg.dot demo2_icfg.pdf


clean:
	rm -f *.dot *.pdf *.csv *.dat *.tgz


