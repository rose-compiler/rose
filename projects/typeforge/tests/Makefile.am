
CHECK_DEFAULT=check-typeforge

check-local: $(CHECK_DEFAULT)

TYPEFORGE=../src/typeforge


check-typeforge: ../src/typeforge
	@echo ================================================================
	@echo RUNNING TYPEFORGE TESTS
	@echo ================================================================
	$(MAKE) check-typeforge-set-analysis

check-typeforge-set-analysis:
	$(TYPEFORGE) --set-analysis $(srcdir)/tf-full-000.C


check-typeforge-old:
	rm -f typegraph.dot craft_initial.json rose_casttest3.C
	$(TYPEFORGE) --spec-file=$(srcdir)/type_replace_test1.tf $(srcdir)/type_replace_test1.C
	$(TYPEFORGE) --spec-file=$(srcdir)/basetype_replace_test1.tf $(srcdir)/basetype_replace_test1.C
	$(TYPEFORGE) --spec-file=$(srcdir)/ad_transformation_test.tf $(srcdir)/casttest1.C
	$(TYPEFORGE) --spec-file=$(srcdir)/ad_transformation_test.tf $(srcdir)/ad_compound_test.C
	$(TYPEFORGE) --spec-file=$(srcdir)/ad_transformation_test.tf $(srcdir)/ad_array_test.C
	$(TYPEFORGE) --spec-file=$(srcdir)/ad_transformation_test.tf $(srcdir)/ad_global_test.C
	$(TYPEFORGE) --spec-file=$(srcdir)/tf_file_test.tf $(srcdir)/tf_file_test.C
	$(TYPEFORGE) --spec-file=$(srcdir)/casttest3.tf $(srcdir)/casttest3.C --compile
	$(TYPEFORGE) --plugin=$(srcdir)/json_replace_test.json $(srcdir)/json_replace_test.C 
	$(TYPEFORGE) --plugin=$(srcdir)/ad_include_pragma.json $(srcdir)/include_pragma_test1.C
	$(TYPEFORGE) --plugin=$(srcdir)/ad_include_pragma.json $(srcdir)/include_pragma_test2.C
	$(TYPEFORGE) --plugin=$(srcdir)/ad_all.json $(srcdir)/ad_all.C
	$(TYPEFORGE) --plugin=$(srcdir)/json_out_test.json $(srcdir)/json_out_test.C --compile
	$(TYPEFORGE) --plugin=craft_initial.json $(srcdir)/json_out_test.C --compile
	rm -f a.out *.json

clean-local:
	rm -f craft_initial.json 
	rm -f *.dot
	rm -f *.ps
	rm -f *.jpg
	rm -f rose_*.C
	rm -f *.ti

distclean-local: clean
	rm -f *.tgz
	rm -f *~

