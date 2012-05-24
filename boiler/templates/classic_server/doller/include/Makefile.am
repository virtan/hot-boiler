# Uncomment commented
#BUILT_SOURCES = link_config
nobase_include_HEADERS = doller/*.h

link_config: ../config.h
	cd .. && diff -qN config.h include/doller/config.h || ( cp -f config.h include/doller/ )
