AM_CPPFLAGS = -I../include $(INC_hot) $(INC_libxml2) $(INC_dl)
AM_CXXFLAGS = -fPIC -Wall -fexceptions -frtti


lib_LTLIBRARIES = libboiler_environment.la
libboiler_environment_la_SOURCES = bconfig.cc globals.cc server.cc log.cc stack.cc stack_pool.cc classic_server.cc signals.cc
libboiler_environment_la_LIBADD = $(LIB_hot) $(LIB_libxml2) $(LIB_dl) $(LIB_threads)
