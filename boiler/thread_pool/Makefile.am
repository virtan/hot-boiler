AM_CPPFLAGS = -I../include $(INC_hot) $(INC_libxml2)
AM_CXXFLAGS = -fPIC -Wall -fexceptions -frtti

# Libraries

lib_LTLIBRARIES = libboiler_thread_pool.la
libboiler_thread_pool_la_SOURCES = thread_pool.cc
libboiler_thread_pool_la_LIBADD = $(LIB_hot) $(LIB_threads)
