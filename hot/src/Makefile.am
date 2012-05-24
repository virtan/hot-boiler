AM_CPPFLAGS = -I../include $(INC_iconv) $(INC_rt)
AM_CXXFLAGS = -fPIC -Wall -W -fexceptions -frtti
AM_LDFLAGS = 

# Libraries

lib_LTLIBRARIES = libhot.la
libhot_la_SOURCES = exceptions.cc string.cc system_resolver.cc hot_iconv.cc filefcn.cc hot_time.cc \
		    bpthread_attr.cc bpthread.cc bpthread_once.cc condition.cc mutex.cc rwlock.cc \
		    timestamp.cc options.cc md5.cc \
		    mapper.cc
libhot_la_LIBADD = $(LIB_threads) $(LIB_iconv) $(LIB_rt)
