AM_CPPFLAGS = -I../include $(INC_hot) $(INC_libxml2)
AM_CXXFLAGS = -fPIC -Wall -fexceptions -frtti

# Libraries

lib_LTLIBRARIES = libboiler_reactor.la
libboiler_reactor_la_SOURCES = reactor.cc reactor_poll.cc reactor_kqueue.cc reactor_epoll.cc
libboiler_reactor_la_LIBADD = -L../thread_pool -lboiler_thread_pool $(LIB_hot) $(LIB_threads)
