EXTRA_DIST = boiler.cfg

install-exec-hook:
	$(INSTALL) -d $(sysconfdir)
	if [ ! -f $(sysconfdir)/boiler.cfg ] ; then $(INSTALL) --mode=644 boiler.cfg $(sysconfdir)/ ; fi
