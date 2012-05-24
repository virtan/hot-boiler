AM_CPPFLAGS = -I../include $(INC_hot) $(INC_libxml2) $(INC_dl)
AM_CXXFLAGS = -fPIC -Wall -fexceptions -frtti


bin_PROGRAMS = boiler
boiler_SOURCES = main.cc
boiler_LDADD = -L../grouping -lboiler $(LIB_hot) $(LIB_dl) $(LIB_threads)
