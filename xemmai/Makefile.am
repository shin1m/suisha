xemmaidir = $(XEMMAI_MODULE_PATH)

xemmai_LTLIBRARIES = suisha.la suishadbus.la
AM_CPPFLAGS = -I$(top_srcdir)/include $(DBUS_CFLAGS) $(XEMMAI_CFLAGS)
AM_CXXFLAGS = -std=c++17
AM_LDFLAGS = -module -avoid-version
if DEBUG
AM_CXXFLAGS += -g
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
suisha_la_LIBADD = $(top_srcdir)/src/libsuisha.la $(XEMMAI_LIBS)
suisha_la_SOURCES = \
	loop.h \
	loop.cc \
	suisha.h \
	suisha.cc
suishadbus_la_LIBADD = $(top_srcdir)/src/libsuishadbus.la $(XEMMAI_LIBS)
suishadbus_la_SOURCES = suishadbus.cc
