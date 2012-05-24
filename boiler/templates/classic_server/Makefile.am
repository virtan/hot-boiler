BUILT_SOURCES = hack_link_config_h

SUBDIRS = doller
EXTRA_DIST = doller/acinclude.m4 doller/configure.ac

hack_link_config_h: ../../config.h
	cd ../.. && diff -qN config.h templates/classic_server/doller/config.h || ( cp -f config.h templates/classic_server/doller/ )
