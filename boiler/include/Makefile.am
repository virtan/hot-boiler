BUILT_SOURCES = link_config
nobase_include_HEADERS = boiler/*.h

link_config: ../config.h
	cd .. && diff -qN config.h include/boiler/config.h || ( cp -f config.h include/boiler/ )
