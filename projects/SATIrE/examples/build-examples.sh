 mkdir $(testdir)
        echo "Testing sl1rd"
        cp -ur $(top_srcdir)/examples/sl1rd $(testdir)
        chmod u+rwx -R $(testdir)/sl1rd
        cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis sl1rd
        $(MAKE) check -C $(testdir)/sl1rd

        echo "Testing sl2rd"
        cp -ur $(top_srcdir)/examples/sl2rd $(testdir)
        chmod u+rwx -R $(testdir)/sl2rd
        cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis sl2rd
        $(MAKE) check -C $(testdir)/sl2rd

        echo "Testing constprop"
        cp -ur $(top_srcdir)/examples/constprop $(testdir)
        chmod u+rwx -R $(testdir)/constprop
        cd $(testdir) ; $(DESTDIR)/$(bindir)/newanalysis constprop
        $(MAKE) check -C $(testdir)/constprop
#       rm -rf $(testdir)
