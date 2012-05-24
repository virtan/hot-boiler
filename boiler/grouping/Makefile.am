AM_CPPFLAGS = -I../include $(INC_hot)
AM_CXXFLAGS = -fPIC -Wall -fexceptions -frtti

# Libraries

lib_LTLIBRARIES = libboiler.la
libboiler_la_SOURCES = empty.cc
libboiler_la_LIBADD = -L../environment -lboiler_environment \
		      -L../reactor -lboiler_reactor \
		      -L../thread_pool -lboiler_thread_pool \
		      $(LIB_hot) $(LIB_threads)
