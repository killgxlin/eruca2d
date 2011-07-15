
SUBDIRS = src include

EXTRA_DIST = autogen.sh

# Grab the demo programs for the distribution:
dist-hook:
	cp -rp demo $(distdir)
	rm -rf `find $(distdir) -type d -name CVS -print`

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: dist

# Rule to build RPM distribution package
rpm: $(PACKAGE)-$(VERSION).tar.gz
	cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES
	rpm -ba $(PACKAGE).spec

MINGWDIR=/usr/local/cross-tools/i386-mingw32msvc

$(PACKAGE)-$(VERSION)-mingw32msvc.tar.gz : $(MINGWDIR)/lib/$(PACKAGE).dll
	tar cvzf $(PACKAGE)-$(VERSION)-mingw32msvc.tar.gz -C / \
		$(MINGWDIR)/lib/$(PACKAGE).dll \
		$(MINGWDIR)/lib/lib$(PACKAGE).a \
		$(MINGWDIR)/include/SDL/SDL_gui.h

mingw32msvc: $(PACKAGE)-$(VERSION)-mingw32msvc.tar.gz
