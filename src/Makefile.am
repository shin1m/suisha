lib_LTLIBRARIES = libsuisha.la libsuishadbus.la
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
libsuisha_la_SOURCES = loop.cc
libsuishadbus_la_LIBADD = $(DBUS_LIBS)
libsuishadbus_la_SOURCES = dbus.cc
