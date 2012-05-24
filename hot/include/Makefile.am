BUILT_SOURCES = link_config
nobase_include_HEADERS = hot/*.h

link_config: ../config.h
	cd .. && diff -qN config.h include/hot/config.h || ( cp -f config.h include/hot/ )
