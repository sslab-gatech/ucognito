SHELL=/bin/bash
DEST=/disk/am0/httpd/htdocs/mbox

#arch:
#	(cd dist/arch && {                                     \
#	makepkg -cs -f;                                    \
#	})

debian:
	(cd dist && {                                          \
		fakeroot ./debian/rules build;                              \
		fakeroot ./debian/rules binary-arch;                        \
	})

#pub:
#	pandoc -s -p --no-wrap                         \
#	  -T Mbox -f markdown                          \
#	  -t html5                                     \
#	  --template=doc/template.html                 \
#	  --email-obfuscation=javascript doc/NOTE.web

clean:
	(cd dist/build && (rm -rf *; rm -rf .deps))

install-debian: debian
	sudo dpkg -r mbox; sudo dpkg -i mbox_*.deb

all: debian install-debian

.PHONY: debian install-debian
