AM_CPPFLAGS = -I$(top_srcdir)/include $(DBUS_CFLAGS)
AM_CXXFLAGS = -std=c++17
AM_LDFLAGS =
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

noinst_PROGRAMS = test spp
test_LDADD = $(top_srcdir)/src/libsuisha.la
test_SOURCES = test.cc
spp_LDADD = $(top_srcdir)/src/libsuisha.la $(top_srcdir)/src/libsuishadbus.la
spp_SOURCES = dbus.h dbus.cc spp.cc
